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

#यदि defined(FEAT_TABPANEL)

स्थिर शून्य do_by_tplmode(पूर्णाङ्क tplmode, पूर्णाङ्क col_start, पूर्णाङ्क col_end,
	पूर्णाङ्क *pcurtab_row, पूर्णाङ्क *ptabpagenr);
स्थिर शून्य tabpanel_free_click_regions(शून्य);
स्थिर शून्य tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, पूर्णाङ्क row, पूर्णाङ्क col_start, पूर्णाङ्क col_end, पूर्णाङ्क tabnr);
स्थिर शून्य draw_tabpanel_scrollbar(पूर्णाङ्क screen_col);

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
#define ALIGN_बायाँ		0
#define ALIGN_दायाँ		1

स्थिर char_u *opt_name = (char_u *)"tabpanel";
स्थिर पूर्णाङ्क opt_scope = OPT_LOCAL;
स्थिर पूर्णाङ्क tpl_align = ALIGN_बायाँ;
स्थिर पूर्णाङ्क tpl_columns = 20;
स्थिर bool tpl_is_vert = false;
स्थिर bool tpl_scrollbar = false;
स्थिर पूर्णाङ्क tpl_scroll_offset = 0;
स्थिर पूर्णाङ्क tpl_total_rows = 0;
स्थिर पूर्णाङ्क tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
स्थिर tabpage_T *tpl_last_curtab = रिक्त;   // last curtab seen by draw_tabpanel

प्रकारनिर्देश संरचना {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    पूर्णाङ्क	maxrow;
    पूर्णाङ्क	offsetrow;
    पूर्णाङ्क	*prow;
    पूर्णाङ्क	*pcol;
    पूर्णाङ्क	attr;
    पूर्णाङ्क	col_start;
    पूर्णाङ्क	col_end;
} tabpanel_T;

    पूर्णाङ्क
tabpanelopt_changed(शून्य)
{
    char_u	*p;
    पूर्णाङ्क		new_align = ALIGN_बायाँ;
    दीर्घ	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    जबसम्म (*p != NUL)
    {
	यदि (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    यदि (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_बायाँ;
	    }
	    अन्यथा यदि (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_दायाँ;
	    }
	    अन्यथा
		फर्काऊ FAIL;
	}
	अन्यथा यदि (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    यदि (new_columns < 0 || new_columns > 1000)
		फर्काऊ FAIL;
	}
	अन्यथा यदि (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	अन्यथा यदि (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	यदि (*p != ',' && *p != NUL)
	    फर्काऊ FAIL;
	यदि (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = रिक्त;

    shell_new_columns();
    फर्काऊ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    शून्य
tabpanel_forget_tabpage(अचर tabpage_T *tp)
{
    यदि (tpl_last_curtab == tp)
	tpl_last_curtab = रिक्त;
}

/*
 * Return the width of tabpanel.
 */
    पूर्णाङ्क
tabpanel_width(शून्य)
{
    विकल्प (p_stpl)
    {
	पक्ष 0:
	    फर्काऊ 0;
	पक्ष 1:
	    यदि (first_tabpage->tp_next == रिक्त)
		फर्काऊ 0;
    }
    यदि (Columns < tpl_columns)
	फर्काऊ 0;
    अन्यथा
	फर्काऊ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    पूर्णाङ्क
tabpanel_leftcol(शून्य)
{
    फर्काऊ tpl_align == ALIGN_दायाँ ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    स्थिर शून्य
tabpanel_free_click_regions(शून्य)
{
    पूर्णाङ्क n;

    यदि (tabpanel_stl_click != रिक्त)
    {
	लागि (n = 0; n < tabpanel_stl_click_गन्ती; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_गन्ती = 0;
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
	पूर्णाङ्क		row,
	पूर्णाङ्क		col_start,
	पूर्णाङ्क		col_end,
	पूर्णाङ्क		tabnr)
{
    पूर्णाङ्क		गन्ती = 0;
    पूर्णाङ्क		n;
    पूर्णाङ्क		base_col;
    पूर्णाङ्क		acc_width = 0;
    पूर्णाङ्क		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = रिक्त;
    पूर्णाङ्क		cur_minwid = 0;
    पूर्णाङ्क		region_start_col;
    stl_click_region_T *new_arr;
    पूर्णाङ्क		limit;

    यदि (clicktab == रिक्त)
	फर्काऊ;

    लागि (n = 0; clicktab[n].start != रिक्त; n++)
	गन्ती++;
    यदि (गन्ती == 0)
	फर्काऊ;

    base_col = (tpl_align == ALIGN_दायाँ ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    परिमाण(stl_click_region_T) * (tabpanel_stl_click_गन्ती + गन्ती + 1));
    यदि (new_arr == रिक्त)
	फर्काऊ;
    tabpanel_stl_click = new_arr;

    p = buf;
    लागि (n = 0; clicktab[n].start != रिक्त; n++)
    {
	acc_width += vim_strnsize(p, (पूर्णाङ्क)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	यदि (cur_funcname != रिक्त)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_गन्ती];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_गन्ती++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    यदि (cur_funcname != रिक्त)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_गन्ती];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_गन्ती++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    स्थिर शून्य
follow_curtab_if_needed(पूर्णाङ्क curtab_row)
{
    यदि (Rows <= 0 || curtab == tpl_last_curtab)
	फर्काऊ;

    यदि (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    अन्यथा यदि (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    पूर्णाङ्क max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    यदि (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    अन्यथा यदि (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    शून्य
draw_tabpanel(शून्य)
{
    पूर्णाङ्क saved_साँचो_Typed = साँचो_Typed;
    पूर्णाङ्क saved_got_int = got_int;
    पूर्णाङ्क maxwidth = tabpanel_width();
    पूर्णाङ्क vs_attr = HL_ATTR(HLF_C);
    पूर्णाङ्क curtab_row = 0;
    पूर्णाङ्क vsrow = 0;
    पूर्णाङ्क is_दायाँ = tpl_align == ALIGN_दायाँ;

    यदि (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	फर्काऊ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    पूर्णाङ्क sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    पूर्णाङ्क sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    यदि (tpl_is_vert)
    {
	यदि (is_दायाँ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    लागि (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    यदि (tpl_scrollbar)
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
	    लागि (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    यदि (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    अन्यथा
    {
	यदि (is_दायाँ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    यदि (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	अन्यथा
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    यदि (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    यदि (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    साँचो_Typed = saved_साँचो_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    पूर्णाङ्क
get_tabpagenr_on_tabpanel(शून्य)
{
    पूर्णाङ्क		maxwidth = tabpanel_width();
    पूर्णाङ्क		curtab_row = 0;
    पूर्णाङ्क		tabpagenr = 0;

    यदि (maxwidth == 0)
	फर्काऊ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, रिक्त);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    फर्काऊ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    स्थिर शून्य
screen_fill_tailing_area(
	पूर्णाङ्क	tplmode,
	पूर्णाङ्क	row_start,
	पूर्णाङ्क	row_end,
	पूर्णाङ्क	col_start,
	पूर्णाङ्क	col_end,
	पूर्णाङ्क	attr)
{
    पूर्णाङ्क is_दायाँ = tpl_align == ALIGN_दायाँ;
    यदि (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_दायाँ ? topframe->fr_width : 0) + col_start,
		(is_दायाँ ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    स्थिर शून्य
screen_puts_len_for_tabpanel(
	पूर्णाङ्क	    tplmode,
	char_u	    *p,
	पूर्णाङ्क	    len,
	पूर्णाङ्क	    attr,
	tabpanel_T  *pargs)
{
    पूर्णाङ्क		j;
    पूर्णाङ्क		chlen;
    पूर्णाङ्क		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    लागि (j = 0; j < len;)
    {
	यदि (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    रोक;

	यदि (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	अन्यथा
	    chlen = 1;

	लागि (पूर्णाङ्क k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	यदि (temp != रिक्त)
	{
	    vim_strncpy(buf, temp, परिमाण(buf) - 1);
	    vim_free(temp);
	}

	यदि (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	अन्यथा
	    chcells = 1;

	यदि (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    यदि (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    यदि (pargs->col_end < chcells)
		रोक;
	}

	यदि (*pargs->pcol + chcells <= pargs->col_end)
	{
	    पूर्णाङ्क off = (tpl_align == ALIGN_दायाँ)
		    ? topframe->fr_width
		    : 0;
	    यदि (tplmode == TPLMODE_REDRAW
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
draw_tabpanel_default(पूर्णाङ्क tplmode, tabpanel_T *pargs)
{
    पूर्णाङ्क		modified;
    पूर्णाङ्क		wincount;
    पूर्णाङ्क		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    लागि (wincount = 0; pargs->wp != रिक्त;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	यदि (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    यदि (modified || wincount > 1)
    {
	यदि (wincount > 1)
	{
	    vim_snprintf((वर्ण *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (पूर्णाङ्क)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#यदि defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#अन्यथा
		    pargs->attr,
#endif
		    pargs);
	}
	यदि (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (पूर्णाङ्क)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    यदि (*pargs->prow - pargs->offsetrow >= 0
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
	पूर्णाङ्क	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    पूर्णाङ्क		curattr;
    पूर्णाङ्क		n;

    curattr = pargs->attr;
    p = buf;
    लागि (n = 0; hltab[n].start != रिक्त; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (पूर्णाङ्क)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	यदि (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	अन्यथा यदि (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	अन्यथा यदि (pargs->wp != रिक्त && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	अन्यथा यदि (pargs->wp != रिक्त && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	अन्यथा यदि (pargs->wp != रिक्त && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	अन्यथा
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (पूर्णाङ्क)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    यदि (*pargs->prow - pargs->offsetrow >= 0
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
	पूर्णाङ्क	tplmode,
	पूर्णाङ्क	col_start,
	पूर्णाङ्क	col_end,
	पूर्णाङ्क	*pcurtab_row,
	पूर्णाङ्क	*ptabpagenr)
{
    पूर्णाङ्क		attr_tplf = HL_ATTR(HLF_TPLF);
    पूर्णाङ्क		attr_tpls = HL_ATTR(HLF_TPLS);
    पूर्णाङ्क		attr_tpl = HL_ATTR(HLF_TPL);
    पूर्णाङ्क		col = col_start;
    पूर्णाङ्क		row = 0;
    tabpage_T	*tp = रिक्त;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    यदि (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    लागि (row = 0; tp != रिक्त; row++)
    {
	यदि (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    रोक;

	col = col_start;

	v.v_type = VAR_सङ्ख्या;
	v.vval.v_सङ्ख्या = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	यदि (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    यदि (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	अन्यथा
	    args.attr = attr_tpl;

	यदि (tp == curtab)
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

	यदि (usefmt != रिक्त && *usefmt != NUL)
	{
	    पूर्णाङ्क	carry_hl = 0;

	    जबसम्म (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = रिक्त;

		यदि (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    रोक;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(शून्य)build_stl_str_hl_mline_nl
#अन्यथा
		(शून्य)build_stl_str_hl_mline
#endif
			(args.cwp, buf, परिमाण(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : रिक्त,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		यदि (tplmode == TPLMODE_REDRAW && clicktab != रिक्त)
		{
		    पूर्णाङ्क screen_row = row - args.offsetrow;
		    पूर्णाङ्क m;

		    यदि (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(पूर्णाङ्क)v.vval.v_सङ्ख्या);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    लागि (m = 0; clicktab[m].start != रिक्त; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		यदि (*usefmt != NUL)
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

	यदि ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_सङ्ख्या;
	    रोक;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    यदि (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    स्थिर शून्य
draw_tabpanel_scrollbar(पूर्णाङ्क screen_col)
{
    पूर्णाङ्क attr_sb = HL_ATTR(HLF_PSB);
    पूर्णाङ्क attr_thumb = HL_ATTR(HLF_PST);
    पूर्णाङ्क thumb_शीर्ष = 0;
    पूर्णाङ्क thumb_height = 0;

    यदि (tpl_total_rows > Rows && Rows > 0)
    {
	पूर्णाङ्क max_offset = tpl_total_rows - Rows;
	पूर्णाङ्क track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	यदि (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	यदि (track_range > 0 && max_offset > 0)
	    thumb_शीर्ष = track_range * tpl_scroll_offset / max_offset;
	अन्यथा
	    thumb_शीर्ष = 0;
	यदि (thumb_शीर्ष + thumb_height > Rows)
	    thumb_शीर्ष = Rows - thumb_height;
	यदि (thumb_शीर्ष < 0)
	    thumb_शीर्ष = 0;
    }

    लागि (पूर्णाङ्क r = 0; r < Rows; r++)
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
    यदि (tabpanel_width() == 0)
	फर्काऊ false;
    फर्काऊ mouse_col < firstwin->w_wincol
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
    फर्काऊ tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(पूर्णाङ्क screen_row)
{
    पूर्णाङ्क thumb_height;
    पूर्णाङ्क max_offset;
    पूर्णाङ्क track_range;
    पूर्णाङ्क thumb_शीर्ष;
    पूर्णाङ्क new_offset;

    यदि (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	फर्काऊ false;

    thumb_height = Rows * Rows / tpl_total_rows;
    यदि (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    यदि (track_range <= 0)
	फर्काऊ true;

    max_offset = tpl_total_rows - Rows;
    thumb_शीर्ष = screen_row - thumb_height / 2;
    यदि (thumb_शीर्ष < 0)
	thumb_शीर्ष = 0;
    यदि (thumb_शीर्ष > track_range)
	thumb_शीर्ष = track_range;

    new_offset = thumb_शीर्ष * max_offset / track_range;
    यदि (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    फर्काऊ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(पूर्णाङ्क dir, पूर्णाङ्क गन्ती)
{
    पूर्णाङ्क max_offset;
    पूर्णाङ्क new_offset;

    यदि (tabpanel_width() == 0)
	फर्काऊ false;

    max_offset = tpl_total_rows - Rows;
    यदि (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? गन्ती : -गन्ती);
    यदि (new_offset < 0)
	new_offset = 0;
    यदि (new_offset > max_offset)
	new_offset = max_offset;
    यदि (new_offset == tpl_scroll_offset)
	फर्काऊ false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    फर्काऊ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(पूर्णाङ्क offset)
{
    पूर्णाङ्क max_offset;

    यदि (tabpanel_width() == 0)
	फर्काऊ false;

    max_offset = tpl_total_rows - Rows;
    यदि (max_offset < 0)
	max_offset = 0;

    यदि (offset < 0)
	offset = 0;
    यदि (offset > max_offset)
	offset = max_offset;
    यदि (offset == tpl_scroll_offset)
	फर्काऊ false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    फर्काऊ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    शून्य
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    पूर्णाङ्क		max_offset;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	फर्काऊ;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    यदि (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_दायाँ ? "right" : "left"));
    dict_add_सङ्ख्या(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_सङ्ख्या(d, "offset", tpl_scroll_offset);
    dict_add_सङ्ख्या(d, "total", tpl_total_rows);
    dict_add_सङ्ख्या(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    शून्य
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    पूर्णाङ्क		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_सङ्ख्या = VVAL_FALSE;

    यदि (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	फर्काऊ;

    n = tv_get_सङ्ख्या_chk(&argvars[0], रिक्त);
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	यदि (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == रिक्त)
	{
	    emsg(_(e_dictionary_required));
	    फर्काऊ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    यदि (n > पूर्णांक_उच्च)
	n = पूर्णांक_उच्च;
    अन्यथा यदि (n < -पूर्णांक_उच्च)
	n = -पूर्णांक_उच्च;

    यदि (absolute)
	changed = tabpanel_set_offset((पूर्णाङ्क)n);
    अन्यथा
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (पूर्णाङ्क)(n >= 0 ? n : -n));

    rettv->vval.v_सङ्ख्या = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
