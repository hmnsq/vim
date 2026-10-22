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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_TABPANEL)

ಸ್ಥಿರ ಶೂನ್ಯ do_by_tplmode(ಪೂರ್ಣಾಙ್ಕ tplmode, ಪೂರ್ಣಾಙ್ಕ col_start, ಪೂರ್ಣಾಙ್ಕ col_end,
	ಪೂರ್ಣಾಙ್ಕ *pcurtab_row, ಪೂರ್ಣಾಙ್ಕ *ptabpagenr);
ಸ್ಥಿರ ಶೂನ್ಯ tabpanel_free_click_regions(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col_start, ಪೂರ್ಣಾಙ್ಕ col_end, ಪೂರ್ಣಾಙ್ಕ tabnr);
ಸ್ಥಿರ ಶೂನ್ಯ draw_tabpanel_scrollbar(ಪೂರ್ಣಾಙ್ಕ screen_col);

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
#define ALIGN_ಎಡ		0
#define ALIGN_ಬಲ		1

ಸ್ಥಿರ char_u *opt_name = (char_u *)"tabpanel";
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ opt_scope = OPT_LOCAL;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tpl_align = ALIGN_ಎಡ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tpl_columns = 20;
ಸ್ಥಿರ bool tpl_is_vert = false;
ಸ್ಥಿರ bool tpl_scrollbar = false;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tpl_scroll_offset = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tpl_total_rows = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
ಸ್ಥಿರ tabpage_T *tpl_last_curtab = ರಿಕ್ತ;   // last curtab seen by draw_tabpanel

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ಪೂರ್ಣಾಙ್ಕ	maxrow;
    ಪೂರ್ಣಾಙ್ಕ	offsetrow;
    ಪೂರ್ಣಾಙ್ಕ	*prow;
    ಪೂರ್ಣಾಙ್ಕ	*pcol;
    ಪೂರ್ಣಾಙ್ಕ	attr;
    ಪೂರ್ಣಾಙ್ಕ	col_start;
    ಪೂರ್ಣಾಙ್ಕ	col_end;
} tabpanel_T;

    ಪೂರ್ಣಾಙ್ಕ
tabpanelopt_changed(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		new_align = ALIGN_ಎಡ;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ಯದಿ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ಎಡ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ಬಲ;
	    }
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ಯದಿ (new_columns < 0 || new_columns > 1000)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = ರಿಕ್ತ;

    shell_new_columns();
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    ಶೂನ್ಯ
tabpanel_forget_tabpage(ಧ್ರುವ tabpage_T *tp)
{
    ಯದಿ (tpl_last_curtab == tp)
	tpl_last_curtab = ರಿಕ್ತ;
}

/*
 * Return the width of tabpanel.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabpanel_width(ಶೂನ್ಯ)
{
    ವಿಕಲ್ಪ (p_stpl)
    {
	ಪಕ್ಷ 0:
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ 1:
	    ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (Columns < tpl_columns)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabpanel_leftcol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ tpl_align == ALIGN_ಬಲ ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabpanel_free_click_regions(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ n;

    ಯದಿ (tabpanel_stl_click != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (n = 0; n < tabpanel_stl_click_ಎಣಿಕೆ; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_ಎಣಿಕೆ = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	ಪೂರ್ಣಾಙ್ಕ		row,
	ಪೂರ್ಣಾಙ್ಕ		col_start,
	ಪೂರ್ಣಾಙ್ಕ		col_end,
	ಪೂರ್ಣಾಙ್ಕ		tabnr)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		base_col;
    ಪೂರ್ಣಾಙ್ಕ		acc_width = 0;
    ಪೂರ್ಣಾಙ್ಕ		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		cur_minwid = 0;
    ಪೂರ್ಣಾಙ್ಕ		region_start_col;
    stl_click_region_T *new_arr;
    ಪೂರ್ಣಾಙ್ಕ		limit;

    ಯದಿ (clicktab == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (n = 0; clicktab[n].start != ರಿಕ್ತ; n++)
	ಎಣಿಕೆ++;
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ;

    base_col = (tpl_align == ALIGN_ಬಲ ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    ಪರಿಮಾಣ(stl_click_region_T) * (tabpanel_stl_click_ಎಣಿಕೆ + ಎಣಿಕೆ + 1));
    ಯದಿ (new_arr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    tabpanel_stl_click = new_arr;

    p = buf;
    ಪ್ರತಿ (n = 0; clicktab[n].start != ರಿಕ್ತ; n++)
    {
	acc_width += vim_strnsize(p, (ಪೂರ್ಣಾಙ್ಕ)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	ಯದಿ (cur_funcname != ರಿಕ್ತ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ಎಣಿಕೆ];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ಎಣಿಕೆ++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    ಯದಿ (cur_funcname != ರಿಕ್ತ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ಎಣಿಕೆ];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_ಎಣಿಕೆ++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
follow_curtab_if_needed(ಪೂರ್ಣಾಙ್ಕ curtab_row)
{
    ಯದಿ (Rows <= 0 || curtab == tpl_last_curtab)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    ಅನ್ಯಥಾ ಯದಿ (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    ಪೂರ್ಣಾಙ್ಕ max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    ಯದಿ (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    ಅನ್ಯಥಾ ಯದಿ (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    ಶೂನ್ಯ
draw_tabpanel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ saved_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಪೂರ್ಣಾಙ್ಕ saved_got_int = got_int;
    ಪೂರ್ಣಾಙ್ಕ maxwidth = tabpanel_width();
    ಪೂರ್ಣಾಙ್ಕ vs_attr = HL_ATTR(HLF_C);
    ಪೂರ್ಣಾಙ್ಕ curtab_row = 0;
    ಪೂರ್ಣಾಙ್ಕ vsrow = 0;
    ಪೂರ್ಣಾಙ್ಕ is_ಬಲ = tpl_align == ALIGN_ಬಲ;

    ಯದಿ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ಪೂರ್ಣಾಙ್ಕ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ಪೂರ್ಣಾಙ್ಕ sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ಯದಿ (tpl_is_vert)
    {
	ಯದಿ (is_ಬಲ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, ರಿಕ್ತ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    ಪ್ರತಿ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    ಯದಿ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	ಅನ್ಯಥಾ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, ರಿಕ್ತ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    ಪ್ರತಿ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ಯದಿ (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (is_ಬಲ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    ಯದಿ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	ಅನ್ಯಥಾ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ರಿಕ್ತ);
	    ಯದಿ (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    ಯದಿ (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ಕೀಲಿ_Typed = saved_ಕೀಲಿ_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_tabpagenr_on_tabpanel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		maxwidth = tabpanel_width();
    ಪೂರ್ಣಾಙ್ಕ		curtab_row = 0;
    ಪೂರ್ಣಾಙ್ಕ		tabpagenr = 0;

    ಯದಿ (maxwidth == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, ರಿಕ್ತ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    ಪ್ರತ್ಯಾಗಮ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_fill_tailing_area(
	ಪೂರ್ಣಾಙ್ಕ	tplmode,
	ಪೂರ್ಣಾಙ್ಕ	row_start,
	ಪೂರ್ಣಾಙ್ಕ	row_end,
	ಪೂರ್ಣಾಙ್ಕ	col_start,
	ಪೂರ್ಣಾಙ್ಕ	col_end,
	ಪೂರ್ಣಾಙ್ಕ	attr)
{
    ಪೂರ್ಣಾಙ್ಕ is_ಬಲ = tpl_align == ALIGN_ಬಲ;
    ಯದಿ (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_ಬಲ ? topframe->fr_width : 0) + col_start,
		(is_ಬಲ ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_puts_len_for_tabpanel(
	ಪೂರ್ಣಾಙ್ಕ	    tplmode,
	char_u	    *p,
	ಪೂರ್ಣಾಙ್ಕ	    len,
	ಪೂರ್ಣಾಙ್ಕ	    attr,
	tabpanel_T  *pargs)
{
    ಪೂರ್ಣಾಙ್ಕ		j;
    ಪೂರ್ಣಾಙ್ಕ		chlen;
    ಪೂರ್ಣಾಙ್ಕ		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    ಪ್ರತಿ (j = 0; j < len;)
    {
	ಯದಿ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ಭಙ್ಗ;

	ಯದಿ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	ಅನ್ಯಥಾ
	    chlen = 1;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	ಯದಿ (temp != ರಿಕ್ತ)
	{
	    vim_strncpy(buf, temp, ಪರಿಮಾಣ(buf) - 1);
	    vim_free(temp);
	}

	ಯದಿ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	ಅನ್ಯಥಾ
	    chcells = 1;

	ಯದಿ (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ಯದಿ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    ಯದಿ (pargs->col_end < chcells)
		ಭಙ್ಗ;
	}

	ಯದಿ (*pargs->pcol + chcells <= pargs->col_end)
	{
	    ಪೂರ್ಣಾಙ್ಕ off = (tpl_align == ALIGN_ಬಲ)
		    ? topframe->fr_width
		    : 0;
	    ಯದಿ (tplmode == TPLMODE_REDRAW
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
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_tabpanel_default(ಪೂರ್ಣಾಙ್ಕ tplmode, tabpanel_T *pargs)
{
    ಪೂರ್ಣಾಙ್ಕ		modified;
    ಪೂರ್ಣಾಙ್ಕ		wincount;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    ಪ್ರತಿ (wincount = 0; pargs->wp != ರಿಕ್ತ;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	ಯದಿ (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    ಯದಿ (modified || wincount > 1)
    {
	ಯದಿ (wincount > 1)
	{
	    vim_snprintf((ವರ್ಣ *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ಯದಿ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#ಅನ್ಯಥಾ
		    pargs->attr,
#endif
		    pargs);
	}
	ಯದಿ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ಯದಿ (*pargs->prow - pargs->offsetrow >= 0
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
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_tabpanel_with_highlight(
	ಪೂರ್ಣಾಙ್ಕ	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		curattr;
    ಪೂರ್ಣಾಙ್ಕ		n;

    curattr = pargs->attr;
    p = buf;
    ಪ್ರತಿ (n = 0; hltab[n].start != ರಿಕ್ತ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ಪೂರ್ಣಾಙ್ಕ)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	ಯದಿ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	ಅನ್ಯಥಾ ಯದಿ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	ಅನ್ಯಥಾ ಯದಿ (pargs->wp != ರಿಕ್ತ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	ಅನ್ಯಥಾ ಯದಿ (pargs->wp != ರಿಕ್ತ && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	ಅನ್ಯಥಾ ಯದಿ (pargs->wp != ರಿಕ್ತ && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	ಅನ್ಯಥಾ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ಯದಿ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_by_tplmode(
	ಪೂರ್ಣಾಙ್ಕ	tplmode,
	ಪೂರ್ಣಾಙ್ಕ	col_start,
	ಪೂರ್ಣಾಙ್ಕ	col_end,
	ಪೂರ್ಣಾಙ್ಕ	*pcurtab_row,
	ಪೂರ್ಣಾಙ್ಕ	*ptabpagenr)
{
    ಪೂರ್ಣಾಙ್ಕ		attr_tplf = HL_ATTR(HLF_TPLF);
    ಪೂರ್ಣಾಙ್ಕ		attr_tpls = HL_ATTR(HLF_TPLS);
    ಪೂರ್ಣಾಙ್ಕ		attr_tpl = HL_ATTR(HLF_TPL);
    ಪೂರ್ಣಾಙ್ಕ		col = col_start;
    ಪೂರ್ಣಾಙ್ಕ		row = 0;
    tabpage_T	*tp = ರಿಕ್ತ;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    ಯದಿ (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    ಪ್ರತಿ (row = 0; tp != ರಿಕ್ತ; row++)
    {
	ಯದಿ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    ಭಙ್ಗ;

	col = col_start;

	v.v_type = VAR_ಸಂಖ್ಯೆ;
	v.vval.v_ಸಂಖ್ಯೆ = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ಯದಿ (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    ಯದಿ (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	ಅನ್ಯಥಾ
	    args.attr = attr_tpl;

	ಯದಿ (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	ಅನ್ಯಥಾ
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ಯದಿ (usefmt != ರಿಕ್ತ && *usefmt != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ	carry_hl = 0;

	    ಯಾವತ್ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ರಿಕ್ತ;

		ಯದಿ (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    ಭಙ್ಗ;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(ಶೂನ್ಯ)build_stl_str_hl_mline_nl
#ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)build_stl_str_hl_mline
#endif
			(args.cwp, buf, ಪರಿಮಾಣ(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ರಿಕ್ತ,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		ಯದಿ (tplmode == TPLMODE_REDRAW && clicktab != ರಿಕ್ತ)
		{
		    ಪೂರ್ಣಾಙ್ಕ screen_row = row - args.offsetrow;
		    ಪೂರ್ಣಾಙ್ಕ m;

		    ಯದಿ (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(ಪೂರ್ಣಾಙ್ಕ)v.vval.v_ಸಂಖ್ಯೆ);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    ಪ್ರತಿ (m = 0; clicktab[m].start != ರಿಕ್ತ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ಯದಿ (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    args.user_defined = ರಿಕ್ತ;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	ಯದಿ ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_ಸಂಖ್ಯೆ;
	    ಭಙ್ಗ;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    ಯದಿ (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_tabpanel_scrollbar(ಪೂರ್ಣಾಙ್ಕ screen_col)
{
    ಪೂರ್ಣಾಙ್ಕ attr_sb = HL_ATTR(HLF_PSB);
    ಪೂರ್ಣಾಙ್ಕ attr_thumb = HL_ATTR(HLF_PST);
    ಪೂರ್ಣಾಙ್ಕ thumb_ತುದಿ = 0;
    ಪೂರ್ಣಾಙ್ಕ thumb_height = 0;

    ಯದಿ (tpl_total_rows > Rows && Rows > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ max_offset = tpl_total_rows - Rows;
	ಪೂರ್ಣಾಙ್ಕ track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	ಯದಿ (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	ಯದಿ (track_range > 0 && max_offset > 0)
	    thumb_ತುದಿ = track_range * tpl_scroll_offset / max_offset;
	ಅನ್ಯಥಾ
	    thumb_ತುದಿ = 0;
	ಯದಿ (thumb_ತುದಿ + thumb_height > Rows)
	    thumb_ತುದಿ = Rows - thumb_height;
	ಯದಿ (thumb_ತುದಿ < 0)
	    thumb_ತುದಿ = 0;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_ತುದಿ && r < thumb_ತುದಿ + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(ಶೂನ್ಯ)
{
    ಯದಿ (tabpanel_width() == 0)
	ಪ್ರತ್ಯಾಗಮ false;
    ಪ್ರತ್ಯಾಗಮ mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ಪೂರ್ಣಾಙ್ಕ screen_row)
{
    ಪೂರ್ಣಾಙ್ಕ thumb_height;
    ಪೂರ್ಣಾಙ್ಕ max_offset;
    ಪೂರ್ಣಾಙ್ಕ track_range;
    ಪೂರ್ಣಾಙ್ಕ thumb_ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ new_offset;

    ಯದಿ (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	ಪ್ರತ್ಯಾಗಮ false;

    thumb_height = Rows * Rows / tpl_total_rows;
    ಯದಿ (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    ಯದಿ (track_range <= 0)
	ಪ್ರತ್ಯಾಗಮ true;

    max_offset = tpl_total_rows - Rows;
    thumb_ತುದಿ = screen_row - thumb_height / 2;
    ಯದಿ (thumb_ತುದಿ < 0)
	thumb_ತುದಿ = 0;
    ಯದಿ (thumb_ತುದಿ > track_range)
	thumb_ತುದಿ = track_range;

    new_offset = thumb_ತುದಿ * max_offset / track_range;
    ಯದಿ (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ಪೂರ್ಣಾಙ್ಕ dir, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ max_offset;
    ಪೂರ್ಣಾಙ್ಕ new_offset;

    ಯದಿ (tabpanel_width() == 0)
	ಪ್ರತ್ಯಾಗಮ false;

    max_offset = tpl_total_rows - Rows;
    ಯದಿ (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? ಎಣಿಕೆ : -ಎಣಿಕೆ);
    ಯದಿ (new_offset < 0)
	new_offset = 0;
    ಯದಿ (new_offset > max_offset)
	new_offset = max_offset;
    ಯದಿ (new_offset == tpl_scroll_offset)
	ಪ್ರತ್ಯಾಗಮ false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(ಪೂರ್ಣಾಙ್ಕ offset)
{
    ಪೂರ್ಣಾಙ್ಕ max_offset;

    ಯದಿ (tabpanel_width() == 0)
	ಪ್ರತ್ಯಾಗಮ false;

    max_offset = tpl_total_rows - Rows;
    ಯದಿ (max_offset < 0)
	max_offset = 0;

    ಯದಿ (offset < 0)
	offset = 0;
    ಯದಿ (offset > max_offset)
	offset = max_offset;
    ಯದಿ (offset == tpl_scroll_offset)
	ಪ್ರತ್ಯಾಗಮ false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    ಶೂನ್ಯ
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		max_offset;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    ಯದಿ (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ಬಲ ? "right" : "left"));
    dict_add_ಸಂಖ್ಯೆ(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_ಸಂಖ್ಯೆ(d, "offset", tpl_scroll_offset);
    dict_add_ಸಂಖ್ಯೆ(d, "total", tpl_total_rows);
    dict_add_ಸಂಖ್ಯೆ(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    ಶೂನ್ಯ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    ಪೂರ್ಣಾಙ್ಕ		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ರಿಕ್ತ)
	{
	    emsg(_(e_dictionary_required));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ಯದಿ (n > INT_MAX)
	n = INT_MAX;
    ಅನ್ಯಥಾ ಯದಿ (n < -INT_MAX)
	n = -INT_MAX;

    ಯದಿ (absolute)
	changed = tabpanel_set_offset((ಪೂರ್ಣಾಙ್ಕ)n);
    ಅನ್ಯಥಾ
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ಪೂರ್ಣಾಙ್ಕ)(n >= 0 ? n : -n));

    rettv->vval.v_ಸಂಖ್ಯೆ = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
