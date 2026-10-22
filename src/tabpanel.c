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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_TABPANEL)

ସ୍ଥିର ଶୂନ୍ୟ do_by_tplmode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tplmode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_end,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pcurtab_row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ptabpagenr);
ସ୍ଥିର ଶୂନ୍ୟ tabpanel_free_click_regions(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabnr);
ସ୍ଥିର ଶୂନ୍ୟ draw_tabpanel_scrollbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_col);

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
#define ALIGN_ବାମ		0
#define ALIGN_ଡାହାଣ		1

ସ୍ଥିର char_u *opt_name = (char_u *)"tabpanel";
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_scope = OPT_LOCAL;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tpl_align = ALIGN_ବାମ;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tpl_columns = 20;
ସ୍ଥିର bool tpl_is_vert = false;
ସ୍ଥିର bool tpl_scrollbar = false;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tpl_scroll_offset = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tpl_total_rows = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
ସ୍ଥିର tabpage_T *tpl_last_curtab = ଖାଲି;   // last curtab seen by draw_tabpanel

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	maxrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	offsetrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*prow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*pcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_end;
} tabpanel_T;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabpanelopt_changed(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_align = ALIGN_ବାମ;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ଯଦି (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ବାମ;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ଡାହାଣ;
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ଯଦି (new_columns < 0 || new_columns > 1000)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = ଖାଲି;

    shell_new_columns();
    ଫେରନ୍ତୁ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    ଶୂନ୍ୟ
tabpanel_forget_tabpage(ଅଚଳ tabpage_T *tp)
{
    ଯଦି (tpl_last_curtab == tp)
	tpl_last_curtab = ଖାଲି;
}

/*
 * Return the width of tabpanel.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabpanel_width(ଶୂନ୍ୟ)
{
    ବାଛ (p_stpl)
    {
	ସ୍ଥିତି 0:
	    ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି 1:
	    ଯଦି (first_tabpage->tp_next == ଖାଲି)
		ଫେରନ୍ତୁ 0;
    }
    ଯଦି (Columns < tpl_columns)
	ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabpanel_leftcol(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ tpl_align == ALIGN_ଡାହାଣ ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tabpanel_free_click_regions(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

    ଯଦି (tabpanel_stl_click != ଖାଲି)
    {
	ପାଇଁ (n = 0; n < tabpanel_stl_click_ଗଣନା; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_ଗଣନା = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_end,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabnr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		base_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		acc_width = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_minwid = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region_start_col;
    stl_click_region_T *new_arr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		limit;

    ଯଦି (clicktab == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (n = 0; clicktab[n].start != ଖାଲି; n++)
	ଗଣନା_2++;
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ;

    base_col = (tpl_align == ALIGN_ଡାହାଣ ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    ଆକାର(stl_click_region_T) * (tabpanel_stl_click_ଗଣନା + ଗଣନା_2 + 1));
    ଯଦି (new_arr == ଖାଲି)
	ଫେରନ୍ତୁ;
    tabpanel_stl_click = new_arr;

    p = buf;
    ପାଇଁ (n = 0; clicktab[n].start != ଖାଲି; n++)
    {
	acc_width += vim_strnsize(p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	ଯଦି (cur_funcname != ଖାଲି)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ଗଣନା];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ଗଣନା++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    ଯଦି (cur_funcname != ଖାଲି)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ଗଣନା];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_ଗଣନା++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
follow_curtab_if_needed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା curtab_row)
{
    ଯଦି (Rows <= 0 || curtab == tpl_last_curtab)
	ଫେରନ୍ତୁ;

    ଯଦି (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    ଅନ୍ୟଥା ଯଦି (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    ଯଦି (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    ଅନ୍ୟଥା ଯଦି (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    ଶୂନ୍ୟ
draw_tabpanel(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_ଚାବି_Typed = ଚାବି_Typed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_got_int = got_int;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxwidth = tabpanel_width();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vs_attr = HL_ATTR(HLF_C);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା curtab_row = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vsrow = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_ଡାହାଣ = tpl_align == ALIGN_ଡାହାଣ;

    ଯଦି (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ଫେରନ୍ତୁ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ଯଦି (tpl_is_vert)
    {
	ଯଦି (is_ଡାହାଣ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, ଖାଲି);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, ଖାଲି);
	    ପାଇଁ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    ଯଦି (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	ଅନ୍ୟଥା
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, ଖାଲି);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, ଖାଲି);
	    ପାଇଁ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ଯଦି (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (is_ଡାହାଣ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ଖାଲି);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ଖାଲି);
	    ଯଦି (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	ଅନ୍ୟଥା
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ଖାଲି);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ଖାଲି);
	    ଯଦି (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    ଯଦି (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ଚାବି_Typed = saved_ଚାବି_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tabpagenr_on_tabpanel(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth = tabpanel_width();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curtab_row = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabpagenr = 0;

    ଯଦି (maxwidth == 0)
	ଫେରନ୍ତୁ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, ଖାଲି);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    ଫେରନ୍ତୁ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
screen_fill_tailing_area(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tplmode,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row_start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row_end,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_end,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_ଡାହାଣ = tpl_align == ALIGN_ଡାହାଣ;
    ଯଦି (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_ଡାହାଣ ? topframe->fr_width : 0) + col_start,
		(is_ଡାହାଣ ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
screen_puts_len_for_tabpanel(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tplmode,
	char_u	    *p,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr,
	tabpanel_T  *pargs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		chlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    ପାଇଁ (j = 0; j < len;)
    {
	ଯଦି (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ଭାଙ୍ଗ;

	ଯଦି (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	ଅନ୍ୟଥା
	    chlen = 1;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	ଯଦି (temp != ଖାଲି)
	{
	    vim_strncpy(buf, temp, ଆକାର(buf) - 1);
	    vim_free(temp);
	}

	ଯଦି (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	ଅନ୍ୟଥା
	    chcells = 1;

	ଯଦି (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ଯଦି (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    ଯଦି (pargs->col_end < chcells)
		ଭାଙ୍ଗ;
	}

	ଯଦି (*pargs->pcol + chcells <= pargs->col_end)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = (tpl_align == ALIGN_ଡାହାଣ)
		    ? topframe->fr_width
		    : 0;
	    ଯଦି (tplmode == TPLMODE_REDRAW
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
    ସ୍ଥିର ଶୂନ୍ୟ
draw_tabpanel_default(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tplmode, tabpanel_T *pargs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modified;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wincount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    ପାଇଁ (wincount = 0; pargs->wp != ଖାଲି;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	ଯଦି (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    ଯଦି (modified || wincount > 1)
    {
	ଯଦି (wincount > 1)
	{
	    vim_snprintf((ଅକ୍ଷର *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ଯଦି defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#ଅନ୍ୟଥା
		    pargs->attr,
#endif
		    pargs);
	}
	ଯଦି (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ଯଦି (*pargs->prow - pargs->offsetrow >= 0
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
    ସ୍ଥିର ଶୂନ୍ୟ
draw_tabpanel_with_highlight(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curattr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    curattr = pargs->attr;
    p = buf;
    ପାଇଁ (n = 0; hltab[n].start != ଖାଲି; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	ଯଦି (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	ଅନ୍ୟଥା ଯଦି (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	ଅନ୍ୟଥା ଯଦି (pargs->wp != ଖାଲି && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	ଅନ୍ୟଥା ଯଦି (pargs->wp != ଖାଲି && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	ଅନ୍ୟଥା ଯଦି (pargs->wp != ଖାଲି && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	ଅନ୍ୟଥା
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ଯଦି (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_by_tplmode(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tplmode,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_end,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*pcurtab_row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ptabpagenr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr_tplf = HL_ATTR(HLF_TPLF);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr_tpls = HL_ATTR(HLF_TPLS);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr_tpl = HL_ATTR(HLF_TPL);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = col_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = 0;
    tabpage_T	*tp = ଖାଲି;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    ଯଦି (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    ପାଇଁ (row = 0; tp != ଖାଲି; row++)
    {
	ଯଦି (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    ଭାଙ୍ଗ;

	col = col_start;

	v.v_type = VAR_ସଂଖ୍ୟା;
	v.vval.v_ସଂଖ୍ୟା = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ଯଦି (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    ଯଦି (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	ଅନ୍ୟଥା
	    args.attr = attr_tpl;

	ଯଦି (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	ଅନ୍ୟଥା
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ଯଦି (usefmt != ଖାଲି && *usefmt != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	carry_hl = 0;

	    ଯେତେବେଳେ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ଖାଲି;

		ଯଦି (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    ଭାଙ୍ଗ;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(ଶୂନ୍ୟ)build_stl_str_hl_mline_nl
#ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)build_stl_str_hl_mline
#endif
			(args.cwp, buf, ଆକାର(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ଖାଲି,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		ଯଦି (tplmode == TPLMODE_REDRAW && clicktab != ଖାଲି)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_row = row - args.offsetrow;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;

		    ଯଦି (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)v.vval.v_ସଂଖ୍ୟା);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    ପାଇଁ (m = 0; clicktab[m].start != ଖାଲି; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ଯଦି (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    args.user_defined = ଖାଲି;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	ଯଦି ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_ସଂଖ୍ୟା;
	    ଭାଙ୍ଗ;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    ଯଦି (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
draw_tabpanel_scrollbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr_sb = HL_ATTR(HLF_PSB);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr_thumb = HL_ATTR(HLF_PST);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା thumb_ଶୀର୍ଷ = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା thumb_height = 0;

    ଯଦି (tpl_total_rows > Rows && Rows > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset = tpl_total_rows - Rows;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	ଯଦି (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	ଯଦି (track_range > 0 && max_offset > 0)
	    thumb_ଶୀର୍ଷ = track_range * tpl_scroll_offset / max_offset;
	ଅନ୍ୟଥା
	    thumb_ଶୀର୍ଷ = 0;
	ଯଦି (thumb_ଶୀର୍ଷ + thumb_height > Rows)
	    thumb_ଶୀର୍ଷ = Rows - thumb_height;
	ଯଦି (thumb_ଶୀର୍ଷ < 0)
	    thumb_ଶୀର୍ଷ = 0;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_ଶୀର୍ଷ && r < thumb_ଶୀର୍ଷ + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(ଶୂନ୍ୟ)
{
    ଯଦି (tabpanel_width() == 0)
	ଫେରନ୍ତୁ false;
    ଫେରନ୍ତୁ mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା thumb_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା track_range;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା thumb_ଶୀର୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_offset;

    ଯଦି (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	ଫେରନ୍ତୁ false;

    thumb_height = Rows * Rows / tpl_total_rows;
    ଯଦି (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    ଯଦି (track_range <= 0)
	ଫେରନ୍ତୁ true;

    max_offset = tpl_total_rows - Rows;
    thumb_ଶୀର୍ଷ = screen_row - thumb_height / 2;
    ଯଦି (thumb_ଶୀର୍ଷ < 0)
	thumb_ଶୀର୍ଷ = 0;
    ଯଦି (thumb_ଶୀର୍ଷ > track_range)
	thumb_ଶୀର୍ଷ = track_range;

    new_offset = thumb_ଶୀର୍ଷ * max_offset / track_range;
    ଯଦି (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    ଫେରନ୍ତୁ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_offset;

    ଯଦି (tabpanel_width() == 0)
	ଫେରନ୍ତୁ false;

    max_offset = tpl_total_rows - Rows;
    ଯଦି (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? ଗଣନା_2 : -ଗଣନା_2);
    ଯଦି (new_offset < 0)
	new_offset = 0;
    ଯଦି (new_offset > max_offset)
	new_offset = max_offset;
    ଯଦି (new_offset == tpl_scroll_offset)
	ଫେରନ୍ତୁ false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    ଫେରନ୍ତୁ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset;

    ଯଦି (tabpanel_width() == 0)
	ଫେରନ୍ତୁ false;

    max_offset = tpl_total_rows - Rows;
    ଯଦି (max_offset < 0)
	max_offset = 0;

    ଯଦି (offset < 0)
	offset = 0;
    ଯଦି (offset > max_offset)
	offset = max_offset;
    ଯଦି (offset == tpl_scroll_offset)
	ଫେରନ୍ତୁ false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    ଫେରନ୍ତୁ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    ଶୂନ୍ୟ
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_offset;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    ଯଦି (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ଡାହାଣ ? "right" : "left"));
    dict_add_ସଂଖ୍ୟା(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_ସଂଖ୍ୟା(d, "offset", tpl_scroll_offset);
    dict_add_ସଂଖ୍ୟା(d, "total", tpl_total_rows);
    dict_add_ସଂଖ୍ୟା(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    ଶୂନ୍ୟ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    n = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ଯଦି (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ଖାଲି)
	{
	    emsg(_(e_dictionary_required));
	    ଫେରନ୍ତୁ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ଯଦି (n > INT_MAX)
	n = INT_MAX;
    ଅନ୍ୟଥା ଯଦି (n < -INT_MAX)
	n = -INT_MAX;

    ଯଦି (absolute)
	changed = tabpanel_set_offset((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
    ଅନ୍ୟଥା
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(n >= 0 ? n : -n));

    rettv->vval.v_ସଂଖ୍ୟା = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
