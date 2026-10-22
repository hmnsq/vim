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

#ប្រសិនបើ defined(FEAT_TABPANEL)

static void do_by_tplmode(ចំនួនគត់ tplmode, ចំនួនគត់ col_start, ចំនួនគត់ col_end,
	ចំនួនគត់ *pcurtab_row, ចំនួនគត់ *ptabpagenr);
static void tabpanel_free_click_regions(void);
static void tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ចំនួនគត់ row, ចំនួនគត់ col_start, ចំនួនគត់ col_end, ចំនួនគត់ tabnr);
static void draw_tabpanel_scrollbar(ចំនួនគត់ screen_col);

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
static ចំនួនគត់ opt_scope = OPT_LOCAL;
static ចំនួនគត់ tpl_align = ALIGN_LEFT;
static ចំនួនគត់ tpl_columns = 20;
static bool tpl_is_vert = false;
static bool tpl_scrollbar = false;
static ចំនួនគត់ tpl_scroll_offset = 0;
static ចំនួនគត់ tpl_total_rows = 0;
static ចំនួនគត់ tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
static tabpage_T *tpl_last_curtab = NULL;   // last curtab seen by draw_tabpanel

typedef sa_nracanaa {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ចំនួនគត់	maxrow;
    ចំនួនគត់	offsetrow;
    ចំនួនគត់	*prow;
    ចំនួនគត់	*pcol;
    ចំនួនគត់	attr;
    ចំនួនគត់	col_start;
    ចំនួនគត់	col_end;
} tabpanel_T;

    ចំនួនគត់
tabpanelopt_changed(void)
{
    char_u	*p;
    ចំនួនគត់		new_align = ALIGN_LEFT;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ខណៈពេល (*p != NUL)
    {
	ប្រសិនបើ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ប្រសិនបើ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_LEFT;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_RIGHT;
	    }
	    បើមិនដូច្នេះ
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ប្រសិនបើ (new_columns < 0 || new_columns > 1000)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ប្រសិនបើ (*p != ',' && *p != NUL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = NULL;

    shell_new_columns();
    ត្រឡប់ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    void
tabpanel_forget_tabpage(const tabpage_T *tp)
{
    ប្រសិនបើ (tpl_last_curtab == tp)
	tpl_last_curtab = NULL;
}

/*
 * Return the width of tabpanel.
 */
    ចំនួនគត់
tabpanel_width(void)
{
    switch (p_stpl)
    {
	pak_sa 0:
	    ត្រឡប់ 0;
	pak_sa 1:
	    ប្រសិនបើ (first_tabpage->tp_next == NULL)
		ត្រឡប់ 0;
    }
    ប្រសិនបើ (Columns < tpl_columns)
	ត្រឡប់ 0;
    បើមិនដូច្នេះ
	ត្រឡប់ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ចំនួនគត់
tabpanel_leftcol(void)
{
    ត្រឡប់ tpl_align == ALIGN_RIGHT ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    static void
tabpanel_free_click_regions(void)
{
    ចំនួនគត់ n;

    ប្រសិនបើ (tabpanel_stl_click != NULL)
    {
	សម្រាប់ (n = 0; n < tabpanel_stl_click_count; n++)
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
	ចំនួនគត់		row,
	ចំនួនគត់		col_start,
	ចំនួនគត់		col_end,
	ចំនួនគត់		tabnr)
{
    ចំនួនគត់		count = 0;
    ចំនួនគត់		n;
    ចំនួនគត់		base_col;
    ចំនួនគត់		acc_width = 0;
    ចំនួនគត់		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = NULL;
    ចំនួនគត់		cur_minwid = 0;
    ចំនួនគត់		region_start_col;
    stl_click_region_T *new_arr;
    ចំនួនគត់		limit;

    ប្រសិនបើ (clicktab == NULL)
	ត្រឡប់;

    សម្រាប់ (n = 0; clicktab[n].start != NULL; n++)
	count++;
    ប្រសិនបើ (count == 0)
	ត្រឡប់;

    base_col = (tpl_align == ALIGN_RIGHT ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    parimaa_na(stl_click_region_T) * (tabpanel_stl_click_count + count + 1));
    ប្រសិនបើ (new_arr == NULL)
	ត្រឡប់;
    tabpanel_stl_click = new_arr;

    p = buf;
    សម្រាប់ (n = 0; clicktab[n].start != NULL; n++)
    {
	acc_width += vim_strnsize(p, (ចំនួនគត់)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	ប្រសិនបើ (cur_funcname != NULL)
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
    ប្រសិនបើ (cur_funcname != NULL)
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
follow_curtab_if_needed(ចំនួនគត់ curtab_row)
{
    ប្រសិនបើ (Rows <= 0 || curtab == tpl_last_curtab)
	ត្រឡប់;

    ប្រសិនបើ (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    បើមិនដូច្នេះ ប្រសិនបើ (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    ចំនួនគត់ max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    ប្រសិនបើ (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    បើមិនដូច្នេះ ប្រសិនបើ (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    void
draw_tabpanel(void)
{
    ចំនួនគត់ saved_KeyTyped = KeyTyped;
    ចំនួនគត់ saved_got_int = got_int;
    ចំនួនគត់ maxwidth = tabpanel_width();
    ចំនួនគត់ vs_attr = HL_ATTR(HLF_C);
    ចំនួនគត់ curtab_row = 0;
    ចំនួនគត់ vsrow = 0;
    ចំនួនគត់ is_right = tpl_align == ALIGN_RIGHT;

    ប្រសិនបើ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ត្រឡប់;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ចំនួនគត់ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ចំនួនគត់ sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ប្រសិនបើ (tpl_is_vert)
    {
	ប្រសិនបើ (is_right)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, NULL);
	    សម្រាប់ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    ប្រសិនបើ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	បើមិនដូច្នេះ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, NULL);
	    សម្រាប់ (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ប្រសិនបើ (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (is_right)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    ប្រសិនបើ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	បើមិនដូច្នេះ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    ប្រសិនបើ (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    ប្រសិនបើ (sb_screen_col >= 0)
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
    ចំនួនគត់
get_tabpagenr_on_tabpanel(void)
{
    ចំនួនគត់		maxwidth = tabpanel_width();
    ចំនួនគត់		curtab_row = 0;
    ចំនួនគត់		tabpagenr = 0;

    ប្រសិនបើ (maxwidth == 0)
	ត្រឡប់ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, NULL);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    ត្រឡប់ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    static void
screen_fill_tailing_area(
	ចំនួនគត់	tplmode,
	ចំនួនគត់	row_start,
	ចំនួនគត់	row_end,
	ចំនួនគត់	col_start,
	ចំនួនគត់	col_end,
	ចំនួនគត់	attr)
{
    ចំនួនគត់ is_right = tpl_align == ALIGN_RIGHT;
    ប្រសិនបើ (tplmode == TPLMODE_REDRAW)
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
	ចំនួនគត់	    tplmode,
	char_u	    *p,
	ចំនួនគត់	    len,
	ចំនួនគត់	    attr,
	tabpanel_T  *pargs)
{
    ចំនួនគត់		j;
    ចំនួនគត់		chlen;
    ចំនួនគត់		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    សម្រាប់ (j = 0; j < len;)
    {
	ប្រសិនបើ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    bha_nga;

	ប្រសិនបើ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	បើមិនដូច្នេះ
	    chlen = 1;

	សម្រាប់ (ចំនួនគត់ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	ប្រសិនបើ (temp != NULL)
	{
	    vim_strncpy(buf, temp, parimaa_na(buf) - 1);
	    vim_free(temp);
	}

	ប្រសិនបើ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	បើមិនដូច្នេះ
	    chcells = 1;

	ប្រសិនបើ (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ប្រសិនបើ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    ប្រសិនបើ (pargs->col_end < chcells)
		bha_nga;
	}

	ប្រសិនបើ (*pargs->pcol + chcells <= pargs->col_end)
	{
	    ចំនួនគត់ off = (tpl_align == ALIGN_RIGHT)
		    ? topframe->fr_width
		    : 0;
	    ប្រសិនបើ (tplmode == TPLMODE_REDRAW
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
draw_tabpanel_default(ចំនួនគត់ tplmode, tabpanel_T *pargs)
{
    ចំនួនគត់		modified;
    ចំនួនគត់		wincount;
    ចំនួនគត់		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    សម្រាប់ (wincount = 0; pargs->wp != NULL;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	ប្រសិនបើ (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    ប្រសិនបើ (modified || wincount > 1)
    {
	ប្រសិនបើ (wincount > 1)
	{
	    vim_snprintf((var_na *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ចំនួនគត់)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ប្រសិនបើ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#បើមិនដូច្នេះ
		    pargs->attr,
#endif
		    pargs);
	}
	ប្រសិនបើ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (ចំនួនគត់)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ប្រសិនបើ (*pargs->prow - pargs->offsetrow >= 0
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
	ចំនួនគត់	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ចំនួនគត់		curattr;
    ចំនួនគត់		n;

    curattr = pargs->attr;
    p = buf;
    សម្រាប់ (n = 0; hltab[n].start != NULL; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ចំនួនគត់)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	ប្រសិនបើ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	បើមិនដូច្នេះ ប្រសិនបើ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	បើមិនដូច្នេះ ប្រសិនបើ (pargs->wp != NULL && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	បើមិនដូច្នេះ ប្រសិនបើ (pargs->wp != NULL && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	បើមិនដូច្នេះ ប្រសិនបើ (pargs->wp != NULL && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	បើមិនដូច្នេះ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ចំនួនគត់)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ប្រសិនបើ (*pargs->prow - pargs->offsetrow >= 0
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
	ចំនួនគត់	tplmode,
	ចំនួនគត់	col_start,
	ចំនួនគត់	col_end,
	ចំនួនគត់	*pcurtab_row,
	ចំនួនគត់	*ptabpagenr)
{
    ចំនួនគត់		attr_tplf = HL_ATTR(HLF_TPLF);
    ចំនួនគត់		attr_tpls = HL_ATTR(HLF_TPLS);
    ចំនួនគត់		attr_tpl = HL_ATTR(HLF_TPL);
    ចំនួនគត់		col = col_start;
    ចំនួនគត់		row = 0;
    tabpage_T	*tp = NULL;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    ប្រសិនបើ (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    សម្រាប់ (row = 0; tp != NULL; row++)
    {
	ប្រសិនបើ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    bha_nga;

	col = col_start;

	v.v_type = VAR_NUMBER;
	v.vval.v_number = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ប្រសិនបើ (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    ប្រសិនបើ (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	បើមិនដូច្នេះ
	    args.attr = attr_tpl;

	ប្រសិនបើ (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	បើមិនដូច្នេះ
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ប្រសិនបើ (usefmt != NULL && *usefmt != NUL)
	{
	    ចំនួនគត់	carry_hl = 0;

	    ខណៈពេល (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = NULL;

		ប្រសិនបើ (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    bha_nga;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(void)build_stl_str_hl_mline_nl
#បើមិនដូច្នេះ
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
		ប្រសិនបើ (tplmode == TPLMODE_REDRAW && clicktab != NULL)
		{
		    ចំនួនគត់ screen_row = row - args.offsetrow;
		    ចំនួនគត់ m;

		    ប្រសិនបើ (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(ចំនួនគត់)v.vval.v_number);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    សម្រាប់ (m = 0; clicktab[m].start != NULL; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ប្រសិនបើ (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	បើមិនដូច្នេះ
	{
	    args.user_defined = NULL;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	ប្រសិនបើ ((tplmode == TPLMODE_GET_TABPAGENR)
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
    ប្រសិនបើ (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    static void
draw_tabpanel_scrollbar(ចំនួនគត់ screen_col)
{
    ចំនួនគត់ attr_sb = HL_ATTR(HLF_PSB);
    ចំនួនគត់ attr_thumb = HL_ATTR(HLF_PST);
    ចំនួនគត់ thumb_top = 0;
    ចំនួនគត់ thumb_height = 0;

    ប្រសិនបើ (tpl_total_rows > Rows && Rows > 0)
    {
	ចំនួនគត់ max_offset = tpl_total_rows - Rows;
	ចំនួនគត់ track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	ប្រសិនបើ (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	ប្រសិនបើ (track_range > 0 && max_offset > 0)
	    thumb_top = track_range * tpl_scroll_offset / max_offset;
	បើមិនដូច្នេះ
	    thumb_top = 0;
	ប្រសិនបើ (thumb_top + thumb_height > Rows)
	    thumb_top = Rows - thumb_height;
	ប្រសិនបើ (thumb_top < 0)
	    thumb_top = 0;
    }

    សម្រាប់ (ចំនួនគត់ r = 0; r < Rows; r++)
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
    ប្រសិនបើ (tabpanel_width() == 0)
	ត្រឡប់ false;
    ត្រឡប់ mouse_col < firstwin->w_wincol
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
    ត្រឡប់ tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ចំនួនគត់ screen_row)
{
    ចំនួនគត់ thumb_height;
    ចំនួនគត់ max_offset;
    ចំនួនគត់ track_range;
    ចំនួនគត់ thumb_top;
    ចំនួនគត់ new_offset;

    ប្រសិនបើ (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	ត្រឡប់ false;

    thumb_height = Rows * Rows / tpl_total_rows;
    ប្រសិនបើ (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    ប្រសិនបើ (track_range <= 0)
	ត្រឡប់ true;

    max_offset = tpl_total_rows - Rows;
    thumb_top = screen_row - thumb_height / 2;
    ប្រសិនបើ (thumb_top < 0)
	thumb_top = 0;
    ប្រសិនបើ (thumb_top > track_range)
	thumb_top = track_range;

    new_offset = thumb_top * max_offset / track_range;
    ប្រសិនបើ (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    ត្រឡប់ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ចំនួនគត់ dir, ចំនួនគត់ count)
{
    ចំនួនគត់ max_offset;
    ចំនួនគត់ new_offset;

    ប្រសិនបើ (tabpanel_width() == 0)
	ត្រឡប់ false;

    max_offset = tpl_total_rows - Rows;
    ប្រសិនបើ (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? count : -count);
    ប្រសិនបើ (new_offset < 0)
	new_offset = 0;
    ប្រសិនបើ (new_offset > max_offset)
	new_offset = max_offset;
    ប្រសិនបើ (new_offset == tpl_scroll_offset)
	ត្រឡប់ false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    ត្រឡប់ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(ចំនួនគត់ offset)
{
    ចំនួនគត់ max_offset;

    ប្រសិនបើ (tabpanel_width() == 0)
	ត្រឡប់ false;

    max_offset = tpl_total_rows - Rows;
    ប្រសិនបើ (max_offset < 0)
	max_offset = 0;

    ប្រសិនបើ (offset < 0)
	offset = 0;
    ប្រសិនបើ (offset > max_offset)
	offset = max_offset;
    ប្រសិនបើ (offset == tpl_scroll_offset)
	ត្រឡប់ false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    ត្រឡប់ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    void
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    ចំនួនគត់		max_offset;

    ប្រសិនបើ (rettv_dict_alloc(rettv) == FAIL)
	ត្រឡប់;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    ប្រសិនបើ (max_offset < 0)
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
    ចំនួនគត់		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    n = tv_get_number_chk(&argvars[0], NULL);
    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == NULL)
	{
	    emsg(_(e_dictionary_required));
	    ត្រឡប់;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ប្រសិនបើ (n > INT_MAX)
	n = INT_MAX;
    បើមិនដូច្នេះ ប្រសិនបើ (n < -INT_MAX)
	n = -INT_MAX;

    ប្រសិនបើ (absolute)
	changed = tabpanel_set_offset((ចំនួនគត់)n);
    បើមិនដូច្នេះ
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ចំនួនគត់)(n >= 0 ? n : -n));

    rettv->vval.v_number = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
