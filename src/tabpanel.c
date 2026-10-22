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

#если defined(FEAT_TABPANEL)

static void do_by_tplmode(целое tplmode, целое col_start, целое col_end,
	целое *pcurtab_row, целое *ptabpagenr);
static void tabpanel_free_click_regions(void);
static void tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, целое row, целое col_start, целое col_end, целое tabnr);
static void draw_tabpanel_scrollbar(целое screen_col);

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
static целое opt_scope = OPT_LOCAL;
static целое tpl_align = ALIGN_LEFT;
static целое tpl_columns = 20;
static bool tpl_is_vert = false;
static bool tpl_scrollbar = false;
static целое tpl_scroll_offset = 0;
static целое tpl_total_rows = 0;
static целое tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
static tabpage_T *tpl_last_curtab = NULL;   // last curtab seen by draw_tabpanel

typedef struct {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    целое	maxrow;
    целое	offsetrow;
    целое	*prow;
    целое	*pcol;
    целое	attr;
    целое	col_start;
    целое	col_end;
} tabpanel_T;

    целое
tabpanelopt_changed(void)
{
    char_u	*p;
    целое		new_align = ALIGN_LEFT;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    пока (*p != NUL)
    {
	если (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    если (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_LEFT;
	    }
	    иначе если (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_RIGHT;
	    }
	    иначе
		вернуть FAIL;
	}
	иначе если (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    если (new_columns < 0 || new_columns > 1000)
		вернуть FAIL;
	}
	иначе если (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	иначе если (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	если (*p != ',' && *p != NUL)
	    вернуть FAIL;
	если (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = NULL;

    shell_new_columns();
    вернуть OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    void
tabpanel_forget_tabpage(const tabpage_T *tp)
{
    если (tpl_last_curtab == tp)
	tpl_last_curtab = NULL;
}

/*
 * Return the width of tabpanel.
 */
    целое
tabpanel_width(void)
{
    switch (p_stpl)
    {
	case 0:
	    вернуть 0;
	case 1:
	    если (first_tabpage->tp_next == NULL)
		вернуть 0;
    }
    если (Columns < tpl_columns)
	вернуть 0;
    иначе
	вернуть tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    целое
tabpanel_leftcol(void)
{
    вернуть tpl_align == ALIGN_RIGHT ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    static void
tabpanel_free_click_regions(void)
{
    целое n;

    если (tabpanel_stl_click != NULL)
    {
	для (n = 0; n < tabpanel_stl_click_count; n++)
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
	целое		row,
	целое		col_start,
	целое		col_end,
	целое		tabnr)
{
    целое		count = 0;
    целое		n;
    целое		base_col;
    целое		acc_width = 0;
    целое		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = NULL;
    целое		cur_minwid = 0;
    целое		region_start_col;
    stl_click_region_T *new_arr;
    целое		limit;

    если (clicktab == NULL)
	вернуть;

    для (n = 0; clicktab[n].start != NULL; n++)
	count++;
    если (count == 0)
	вернуть;

    base_col = (tpl_align == ALIGN_RIGHT ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    sizeof(stl_click_region_T) * (tabpanel_stl_click_count + count + 1));
    если (new_arr == NULL)
	вернуть;
    tabpanel_stl_click = new_arr;

    p = buf;
    для (n = 0; clicktab[n].start != NULL; n++)
    {
	acc_width += vim_strnsize(p, (целое)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	если (cur_funcname != NULL)
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
    если (cur_funcname != NULL)
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
follow_curtab_if_needed(целое curtab_row)
{
    если (Rows <= 0 || curtab == tpl_last_curtab)
	вернуть;

    если (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    иначе если (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    целое max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    если (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    иначе если (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    void
draw_tabpanel(void)
{
    целое saved_KeyTyped = KeyTyped;
    целое saved_got_int = got_int;
    целое maxwidth = tabpanel_width();
    целое vs_attr = HL_ATTR(HLF_C);
    целое curtab_row = 0;
    целое vsrow = 0;
    целое is_right = tpl_align == ALIGN_RIGHT;

    если (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	вернуть;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    целое sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    целое sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    если (tpl_is_vert)
    {
	если (is_right)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, NULL);
	    для (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    если (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	иначе
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, NULL);
	    для (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    если (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    иначе
    {
	если (is_right)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    если (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	иначе
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, NULL);
	    если (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    если (sb_screen_col >= 0)
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
    целое
get_tabpagenr_on_tabpanel(void)
{
    целое		maxwidth = tabpanel_width();
    целое		curtab_row = 0;
    целое		tabpagenr = 0;

    если (maxwidth == 0)
	вернуть -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, NULL);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    вернуть tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    static void
screen_fill_tailing_area(
	целое	tplmode,
	целое	row_start,
	целое	row_end,
	целое	col_start,
	целое	col_end,
	целое	attr)
{
    целое is_right = tpl_align == ALIGN_RIGHT;
    если (tplmode == TPLMODE_REDRAW)
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
	целое	    tplmode,
	char_u	    *p,
	целое	    len,
	целое	    attr,
	tabpanel_T  *pargs)
{
    целое		j;
    целое		chlen;
    целое		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    для (j = 0; j < len;)
    {
	если (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    break;

	если (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	иначе
	    chlen = 1;

	для (целое k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	если (temp != NULL)
	{
	    vim_strncpy(buf, temp, sizeof(buf) - 1);
	    vim_free(temp);
	}

	если (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	иначе
	    chcells = 1;

	если (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    если (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    если (pargs->col_end < chcells)
		break;
	}

	если (*pargs->pcol + chcells <= pargs->col_end)
	{
	    целое off = (tpl_align == ALIGN_RIGHT)
		    ? topframe->fr_width
		    : 0;
	    если (tplmode == TPLMODE_REDRAW
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
draw_tabpanel_default(целое tplmode, tabpanel_T *pargs)
{
    целое		modified;
    целое		wincount;
    целое		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    для (wincount = 0; pargs->wp != NULL;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	если (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    если (modified || wincount > 1)
    {
	если (wincount > 1)
	{
	    vim_snprintf((char *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (целое)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#если defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#иначе
		    pargs->attr,
#endif
		    pargs);
	}
	если (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (целое)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    если (*pargs->prow - pargs->offsetrow >= 0
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
	целое	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    целое		curattr;
    целое		n;

    curattr = pargs->attr;
    p = buf;
    для (n = 0; hltab[n].start != NULL; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (целое)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	если (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	иначе если (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	иначе если (pargs->wp != NULL && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	иначе если (pargs->wp != NULL && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	иначе если (pargs->wp != NULL && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	иначе
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (целое)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    если (*pargs->prow - pargs->offsetrow >= 0
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
	целое	tplmode,
	целое	col_start,
	целое	col_end,
	целое	*pcurtab_row,
	целое	*ptabpagenr)
{
    целое		attr_tplf = HL_ATTR(HLF_TPLF);
    целое		attr_tpls = HL_ATTR(HLF_TPLS);
    целое		attr_tpl = HL_ATTR(HLF_TPL);
    целое		col = col_start;
    целое		row = 0;
    tabpage_T	*tp = NULL;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    если (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    для (row = 0; tp != NULL; row++)
    {
	если (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    break;

	col = col_start;

	v.v_type = VAR_NUMBER;
	v.vval.v_number = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	если (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    если (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	иначе
	    args.attr = attr_tpl;

	если (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	иначе
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	если (usefmt != NULL && *usefmt != NUL)
	{
	    целое	carry_hl = 0;

	    пока (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = NULL;

		если (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    break;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(void)build_stl_str_hl_mline_nl
#иначе
		(void)build_stl_str_hl_mline
#endif
			(args.cwp, buf, sizeof(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : NULL,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		если (tplmode == TPLMODE_REDRAW && clicktab != NULL)
		{
		    целое screen_row = row - args.offsetrow;
		    целое m;

		    если (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(целое)v.vval.v_number);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    для (m = 0; clicktab[m].start != NULL; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		если (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	иначе
	{
	    args.user_defined = NULL;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	если ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_number;
	    break;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    если (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    static void
draw_tabpanel_scrollbar(целое screen_col)
{
    целое attr_sb = HL_ATTR(HLF_PSB);
    целое attr_thumb = HL_ATTR(HLF_PST);
    целое thumb_top = 0;
    целое thumb_height = 0;

    если (tpl_total_rows > Rows && Rows > 0)
    {
	целое max_offset = tpl_total_rows - Rows;
	целое track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	если (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	если (track_range > 0 && max_offset > 0)
	    thumb_top = track_range * tpl_scroll_offset / max_offset;
	иначе
	    thumb_top = 0;
	если (thumb_top + thumb_height > Rows)
	    thumb_top = Rows - thumb_height;
	если (thumb_top < 0)
	    thumb_top = 0;
    }

    для (целое r = 0; r < Rows; r++)
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
    если (tabpanel_width() == 0)
	вернуть false;
    вернуть mouse_col < firstwin->w_wincol
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
    вернуть tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(целое screen_row)
{
    целое thumb_height;
    целое max_offset;
    целое track_range;
    целое thumb_top;
    целое new_offset;

    если (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	вернуть false;

    thumb_height = Rows * Rows / tpl_total_rows;
    если (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    если (track_range <= 0)
	вернуть true;

    max_offset = tpl_total_rows - Rows;
    thumb_top = screen_row - thumb_height / 2;
    если (thumb_top < 0)
	thumb_top = 0;
    если (thumb_top > track_range)
	thumb_top = track_range;

    new_offset = thumb_top * max_offset / track_range;
    если (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    вернуть true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(целое dir, целое count)
{
    целое max_offset;
    целое new_offset;

    если (tabpanel_width() == 0)
	вернуть false;

    max_offset = tpl_total_rows - Rows;
    если (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? count : -count);
    если (new_offset < 0)
	new_offset = 0;
    если (new_offset > max_offset)
	new_offset = max_offset;
    если (new_offset == tpl_scroll_offset)
	вернуть false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    вернуть true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(целое offset)
{
    целое max_offset;

    если (tabpanel_width() == 0)
	вернуть false;

    max_offset = tpl_total_rows - Rows;
    если (max_offset < 0)
	max_offset = 0;

    если (offset < 0)
	offset = 0;
    если (offset > max_offset)
	offset = max_offset;
    если (offset == tpl_scroll_offset)
	вернуть false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    вернуть true;
}

/*
 * "tabpanel_getinfo()" function
 */
    void
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    целое		max_offset;

    если (rettv_dict_alloc(rettv) == FAIL)
	вернуть;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    если (max_offset < 0)
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
    целое		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    если (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	вернуть;

    n = tv_get_number_chk(&argvars[0], NULL);
    если (argvars[1].v_type != VAR_UNKNOWN)
    {
	если (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == NULL)
	{
	    emsg(_(e_dictionary_required));
	    вернуть;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    если (n > INT_MAX)
	n = INT_MAX;
    иначе если (n < -INT_MAX)
	n = -INT_MAX;

    если (absolute)
	changed = tabpanel_set_offset((целое)n);
    иначе
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (целое)(n >= 0 ? n : -n));

    rettv->vval.v_number = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
