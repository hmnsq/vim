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

#include "vim.h"

#གལ་སྲིད defined(FEAT_TABPANEL)

static void do_by_tplmode(ཧྲིལ tplmode, ཧྲིལ col_start, ཧྲིལ col_end,
	ཧྲིལ *pcurtab_row, ཧྲིལ *ptabpagenr);
static void tabpanel_free_click_regions(void);
static void tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ཧྲིལ row, ཧྲིལ col_start, ཧྲིལ col_end, ཧྲིལ tabnr);
static void draw_tabpanel_scrollbar(ཧྲིལ screen_col);

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
#define ALIGN_LEFT		0
#define ALIGN_RIGHT		1

static char_u *opt_name = (char_u *)"tabpanel";
static ཧྲིལ opt_scope = OPT_LOCAL;
static ཧྲིལ tpl_align = ALIGN_LEFT;
static ཧྲིལ tpl_columns = 20;
static bool tpl_is_vert = false;
static bool tpl_scrollbar = false;
static ཧྲིལ tpl_scroll_offset = 0;
static ཧྲིལ tpl_total_rows = 0;
static ཧྲིལ tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
static tabpage_T *tpl_last_curtab = NULL;   // last curtab seen by draw_tabpanel

typedef sa_nracanaa {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ཧྲིལ	maxrow;
    ཧྲིལ	offsetrow;
    ཧྲིལ	*prow;
    ཧྲིལ	*pcol;
    ཧྲིལ	attr;
    ཧྲིལ	col_start;
    ཧྲིལ	col_end;
} tabpanel_T;

    ཧྲིལ
tabpanelopt_changed(void)
{
    char_u	*p;
    ཧྲིལ		new_align = ALIGN_LEFT;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    སྐབས (*p != NUL)
    {
	གལ་སྲིད (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    གལ་སྲིད (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_LEFT;
	    }
	    དེ་མིན གལ་སྲིད (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_RIGHT;
	    }
	    དེ་མིན
		ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    གལ་སྲིད (new_columns < 0 || new_columns > 1000)
		ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	དེ་མིན གལ་སྲིད (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	གལ་སྲིད (*p != ',' && *p != NUL)
	    ལོག FAIL;
	གལ་སྲིད (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = NULL;

    shell_new_columns();
    ལོག OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    void
tabpanel_forget_tabpage(const tabpage_T *tp)
{
    གལ་སྲིད (tpl_last_curtab == tp)
	tpl_last_curtab = NULL;
}

/*
 * Return the width of tabpanel.
 */
    ཧྲིལ
tabpanel_width(void)
{
    switch (p_stpl)
    {
	pak_sa 0:
	    ལོག 0;
	pak_sa 1:
	    གལ་སྲིད (first_tabpage->tp_next == NULL)
		ལོག 0;
    }
    གལ་སྲིད (Columns < tpl_columns)
	ལོག 0;
    དེ་མིན
	ལོག tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ཧྲིལ
tabpanel_leftcol(void)
{
    ལོག tpl_align == ALIGN_RIGHT ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    static void
tabpanel_free_click_regions(void)
{
    ཧྲིལ n;

    གལ་སྲིད (tabpanel_stl_click != NULL)
    {
	དོན་ལུ (n = 0; n < tabpanel_stl_click_count; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_count = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    static void
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	ཧྲིལ		row,
	ཧྲིལ		col_start,
	ཧྲིལ		col_end,
	ཧྲིལ		tabnr)
{
    ཧྲིལ		count = 0;
    ཧྲིལ		n;
    ཧྲིལ		base_col;
    ཧྲིལ		acc_width = 0;
    ཧྲིལ		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = NULL;
    ཧྲིལ		cur_minwid = 0;
    ཧྲིལ		region_start_col;
    stl_click_region_T *new_arr;
    ཧྲིལ		limit;

    གལ་སྲིད (clicktab == NULL)
	ལོག;

    དོན་ལུ (n = 0; clicktab[n].start != NULL; n++)
	count++;
    གལ་སྲིད (count == 0)
	ལོག;

    base_col = (tpl_align == ALIGN_RIGHT ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    parimaa_na(stl_click_region_T) * (tabpanel_stl_click_count + count + 1));
    གལ་སྲིད (new_arr == NULL)
	ལོག;
    tabpanel_stl_click = new_arr;

    p = buf;
    དོན་ལུ (n = 0; clicktab[n].start != NULL; n++)
    {
	acc_width += vim_strnsize(p, (ཧྲིལ)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	གལ་སྲིད (cur_funcname != NULL)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_count];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_count++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    གལ་སྲིད (cur_funcname != NULL)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_count];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_count++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    static void
follow_curtab_if_needed(ཧྲིལ curtab_row)
{
    གལ་སྲིད (Rows <= 0 || curtab == tpl_last_curtab)
	ལོག;

    གལ་སྲིད (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    དེ་མིན གལ་སྲིད (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    ཧྲིལ max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    གལ་སྲིད (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    དེ་མིན གལ་སྲིད (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    void
draw_tabpanel(void)
{
    ཧྲིལ saved_KeyTyped = KeyTyped;
    ཧྲིལ saved_got_int = got_int;
    ཧྲིལ maxwidth = tabpanel_width();
    ཧྲིལ vs_attr = HL_ATTR(HLF_C);
    ཧྲིལ curtab_row = 0;
    ཧྲིལ vsrow = 0;
    ཧྲིལ is_right = tpl_align == ALIGN_RIGHT;

    གལ་སྲིད (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ལོག;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ཧྲིལ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ཧྲིལ sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    གལ་སྲིད (tpl_is_vert)
    {
	གལ་སྲིད (is_right)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, NULL);
	    དོན་ལུ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    གལ་སྲིད (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	དེ་མིན
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, NULL);
	    དོན་ལུ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    གལ་སྲིད (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    དེ་མིན
    {
	གལ་སྲིད (is_right)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    གལ་སྲིད (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	དེ་མིན
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    གལ་སྲིད (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    གལ་སྲིད (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    KeyTyped = saved_KeyTyped;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    ཧྲིལ
get_tabpagenr_on_tabpanel(void)
{
    ཧྲིལ		maxwidth = tabpanel_width();
    ཧྲིལ		curtab_row = 0;
    ཧྲིལ		tabpagenr = 0;

    གལ་སྲིད (maxwidth == 0)
	ལོག -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, NULL);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    ལོག tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    static void
screen_fill_tailing_area(
	ཧྲིལ	tplmode,
	ཧྲིལ	row_start,
	ཧྲིལ	row_end,
	ཧྲིལ	col_start,
	ཧྲིལ	col_end,
	ཧྲིལ	attr)
{
    ཧྲིལ is_right = tpl_align == ALIGN_RIGHT;
    གལ་སྲིད (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_right ? topframe->fr_width : 0) + col_start,
		(is_right ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    static void
screen_puts_len_for_tabpanel(
	ཧྲིལ	    tplmode,
	char_u	    *p,
	ཧྲིལ	    len,
	ཧྲིལ	    attr,
	tabpanel_T  *pargs)
{
    ཧྲིལ		j;
    ཧྲིལ		chlen;
    ཧྲིལ		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    དོན་ལུ (j = 0; j < len;)
    {
	གལ་སྲིད (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    bha_nga;

	གལ་སྲིད (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	དེ་མིན
	    chlen = 1;

	དོན་ལུ (ཧྲིལ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	གལ་སྲིད (temp != NULL)
	{
	    vim_strncpy(buf, temp, parimaa_na(buf) - 1);
	    vim_free(temp);
	}

	གལ་སྲིད (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	དེ་མིན
	    chcells = 1;

	གལ་སྲིད (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    གལ་སྲིད (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    གལ་སྲིད (pargs->col_end < chcells)
		bha_nga;
	}

	གལ་སྲིད (*pargs->pcol + chcells <= pargs->col_end)
	{
	    ཧྲིལ off = (tpl_align == ALIGN_RIGHT)
		    ? topframe->fr_width
		    : 0;
	    གལ་སྲིད (tplmode == TPLMODE_REDRAW
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
    static void
draw_tabpanel_default(ཧྲིལ tplmode, tabpanel_T *pargs)
{
    ཧྲིལ		modified;
    ཧྲིལ		wincount;
    ཧྲིལ		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    དོན་ལུ (wincount = 0; pargs->wp != NULL;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	གལ་སྲིད (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    གལ་སྲིད (modified || wincount > 1)
    {
	གལ་སྲིད (wincount > 1)
	{
	    vim_snprintf((var_na *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ཧྲིལ)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#གལ་སྲིད defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#དེ་མིན
		    pargs->attr,
#endif
		    pargs);
	}
	གལ་སྲིད (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (ཧྲིལ)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    གལ་སྲིད (*pargs->prow - pargs->offsetrow >= 0
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
    static void
draw_tabpanel_with_highlight(
	ཧྲིལ	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ཧྲིལ		curattr;
    ཧྲིལ		n;

    curattr = pargs->attr;
    p = buf;
    དོན་ལུ (n = 0; hltab[n].start != NULL; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ཧྲིལ)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	གལ་སྲིད (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	དེ་མིན གལ་སྲིད (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	དེ་མིན གལ་སྲིད (pargs->wp != NULL && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	དེ་མིན གལ་སྲིད (pargs->wp != NULL && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	དེ་མིན གལ་སྲིད (pargs->wp != NULL && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	དེ་མིན
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ཧྲིལ)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    གལ་སྲིད (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    static void
do_by_tplmode(
	ཧྲིལ	tplmode,
	ཧྲིལ	col_start,
	ཧྲིལ	col_end,
	ཧྲིལ	*pcurtab_row,
	ཧྲིལ	*ptabpagenr)
{
    ཧྲིལ		attr_tplf = HL_ATTR(HLF_TPLF);
    ཧྲིལ		attr_tpls = HL_ATTR(HLF_TPLS);
    ཧྲིལ		attr_tpl = HL_ATTR(HLF_TPL);
    ཧྲིལ		col = col_start;
    ཧྲིལ		row = 0;
    tabpage_T	*tp = NULL;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    གལ་སྲིད (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    དོན་ལུ (row = 0; tp != NULL; row++)
    {
	གལ་སྲིད (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    bha_nga;

	col = col_start;

	v.v_type = VAR_NUMBER;
	v.vval.v_number = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	གལ་སྲིད (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    གལ་སྲིད (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	དེ་མིན
	    args.attr = attr_tpl;

	གལ་སྲིད (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	དེ་མིན
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	གལ་སྲིད (usefmt != NULL && *usefmt != NUL)
	{
	    ཧྲིལ	carry_hl = 0;

	    སྐབས (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = NULL;

		གལ་སྲིད (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    bha_nga;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(void)build_stl_str_hl_mline_nl
#དེ་མིན
		(void)build_stl_str_hl_mline
#endif
			(args.cwp, buf, parimaa_na(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : NULL,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		གལ་སྲིད (tplmode == TPLMODE_REDRAW && clicktab != NULL)
		{
		    ཧྲིལ screen_row = row - args.offsetrow;
		    ཧྲིལ m;

		    གལ་སྲིད (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(ཧྲིལ)v.vval.v_number);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    དོན་ལུ (m = 0; clicktab[m].start != NULL; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		གལ་སྲིད (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	དེ་མིན
	{
	    args.user_defined = NULL;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	གལ་སྲིད ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_number;
	    bha_nga;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    གལ་སྲིད (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    static void
draw_tabpanel_scrollbar(ཧྲིལ screen_col)
{
    ཧྲིལ attr_sb = HL_ATTR(HLF_PSB);
    ཧྲིལ attr_thumb = HL_ATTR(HLF_PST);
    ཧྲིལ thumb_top = 0;
    ཧྲིལ thumb_height = 0;

    གལ་སྲིད (tpl_total_rows > Rows && Rows > 0)
    {
	ཧྲིལ max_offset = tpl_total_rows - Rows;
	ཧྲིལ track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	གལ་སྲིད (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	གལ་སྲིད (track_range > 0 && max_offset > 0)
	    thumb_top = track_range * tpl_scroll_offset / max_offset;
	དེ་མིན
	    thumb_top = 0;
	གལ་སྲིད (thumb_top + thumb_height > Rows)
	    thumb_top = Rows - thumb_height;
	གལ་སྲིད (thumb_top < 0)
	    thumb_top = 0;
    }

    དོན་ལུ (ཧྲིལ r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_top && r < thumb_top + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(void)
{
    གལ་སྲིད (tabpanel_width() == 0)
	ལོག false;
    ལོག mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(void)
{
    ལོག tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ཧྲིལ screen_row)
{
    ཧྲིལ thumb_height;
    ཧྲིལ max_offset;
    ཧྲིལ track_range;
    ཧྲིལ thumb_top;
    ཧྲིལ new_offset;

    གལ་སྲིད (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	ལོག false;

    thumb_height = Rows * Rows / tpl_total_rows;
    གལ་སྲིད (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    གལ་སྲིད (track_range <= 0)
	ལོག true;

    max_offset = tpl_total_rows - Rows;
    thumb_top = screen_row - thumb_height / 2;
    གལ་སྲིད (thumb_top < 0)
	thumb_top = 0;
    གལ་སྲིད (thumb_top > track_range)
	thumb_top = track_range;

    new_offset = thumb_top * max_offset / track_range;
    གལ་སྲིད (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    ལོག true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ཧྲིལ dir, ཧྲིལ count)
{
    ཧྲིལ max_offset;
    ཧྲིལ new_offset;

    གལ་སྲིད (tabpanel_width() == 0)
	ལོག false;

    max_offset = tpl_total_rows - Rows;
    གལ་སྲིད (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? count : -count);
    གལ་སྲིད (new_offset < 0)
	new_offset = 0;
    གལ་སྲིད (new_offset > max_offset)
	new_offset = max_offset;
    གལ་སྲིད (new_offset == tpl_scroll_offset)
	ལོག false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    ལོག true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(ཧྲིལ offset)
{
    ཧྲིལ max_offset;

    གལ་སྲིད (tabpanel_width() == 0)
	ལོག false;

    max_offset = tpl_total_rows - Rows;
    གལ་སྲིད (max_offset < 0)
	max_offset = 0;

    གལ་སྲིད (offset < 0)
	offset = 0;
    གལ་སྲིད (offset > max_offset)
	offset = max_offset;
    གལ་སྲིད (offset == tpl_scroll_offset)
	ལོག false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    ལོག true;
}

/*
 * "tabpanel_getinfo()" function
 */
    void
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    ཧྲིལ		max_offset;

    གལ་སྲིད (rettv_dict_alloc(rettv) == FAIL)
	ལོག;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    གལ་སྲིད (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_RIGHT ? "right" : "left"));
    dict_add_number(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_number(d, "offset", tpl_scroll_offset);
    dict_add_number(d, "total", tpl_total_rows);
    dict_add_number(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    void
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    ཧྲིལ		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    གལ་སྲིད (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ལོག;

    n = tv_get_number_chk(&argvars[0], NULL);
    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	གལ་སྲིད (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == NULL)
	{
	    emsg(_(e_dictionary_required));
	    ལོག;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    གལ་སྲིད (n > INT_MAX)
	n = INT_MAX;
    དེ་མིན གལ་སྲིད (n < -INT_MAX)
	n = -INT_MAX;

    གལ་སྲིད (absolute)
	changed = tabpanel_set_offset((ཧྲིལ)n);
    དེ་མིན
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ཧྲིལ)(n >= 0 ? n : -n));

    rettv->vval.v_number = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
