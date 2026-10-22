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

ساكن فراغ do_by_tplmode(صحيح tplmode, صحيح ستون_پیل, صحيح ستون_پای_ټکی,
	صحيح *pcurtab_لیکه, صحيح *ptabpagenr);
ساكن فراغ tabpanel_free_click_regions(فراغ);
ساكن فراغ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحيح لیکه, صحيح ستون_پیل, صحيح ستون_پای_ټکی, صحيح tabnr);
ساكن فراغ draw_tabpanel_scrollbar(صحيح screen_ستون);

// set pcurtab_row. don't redraw tabpanel.
#تعریف_کړه TPLMODE_GET_CURTAB_لیکه	0
// set ptabpagenr. don't redraw tabpanel.
#تعریف_کړه TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#تعریف_کړه TPLMODE_REDRAW		2

#تعریف_کړه TPL_FILLCHAR		' '

#تعریف_کړه VERT_LEN		1
#تعریف_کړه SCROLL_LEN		1

// tpl_align's values
#تعریف_کړه ALIGN_کیڼ		0
#تعریف_کړه ALIGN_ښی		1

ساكن char_u *opt_name = (char_u *)"tabpanel";
ساكن صحيح opt_scope = OPT_LOCAL;
ساكن صحيح tpl_align = ALIGN_کیڼ;
ساكن صحيح tpl_columns = 20;
ساكن bool tpl_is_vert = false;
ساكن bool tpl_scrollbar = false;
ساكن صحيح tpl_scroll_دننه_واټن = 0;
ساكن صحيح tpl_ټول_لیکې = 0;
ساكن صحيح tpl_scrollbar_ستون = -1;  // screen column of scrollbar, -1 if none
ساكن tabpage_T *tpl_وروستی_curtab = عدم;   // last curtab seen by draw_tabpanel

تعريفنوع بنية {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحيح	maxrow;
    صحيح	offsetrow;
    صحيح	*prow;
    صحيح	*pcol;
    صحيح	attr;
    صحيح	ستون_پیل;
    صحيح	ستون_پای_ټکی;
} tabpanel_T;

    صحيح
tabpanelopt_بدل_شوی(فراغ)
{
    char_u	*p;
    صحيح		new_align = ALIGN_کیڼ;
    اوږد	new_columns = 20;
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
		new_align = ALIGN_کیڼ;
	    }
	    وإلا إذا (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ښی;
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
    tpl_وروستی_curtab = عدم;

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
    إذا (tpl_وروستی_curtab == tp)
	tpl_وروستی_curtab = عدم;
}

/*
 * Return the width of tabpanel.
 */
    صحيح
tabpanel_پلنوالی(فراغ)
{
    بدل (p_stpl)
    {
	حالة 0:
	    أرجع 0;
	حالة 1:
	    إذا (لومړی_tabpage->tp_بل == عدم)
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
    أرجع tpl_align == ALIGN_ښی ? 0 : tabpanel_پلنوالی();
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
	لكل (n = 0; n < tabpanel_stl_click_شمېر; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_شمېر = 0;
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
	صحيح		لیکه,
	صحيح		ستون_پیل,
	صحيح		ستون_پای_ټکی,
	صحيح		tabnr)
{
    صحيح		شمېر = 0;
    صحيح		n;
    صحيح		بنسټ_ستون;
    صحيح		acc_پلنوالی = 0;
    صحيح		max_w = ستون_پای_ټکی - ستون_پیل;
    char_u	*p;
    char_u	*cur_funcname = عدم;
    صحيح		cur_minwid = 0;
    صحيح		region_پیل_ستون;
    stl_click_region_T *new_arr;
    صحيح		حد;

    إذا (clicktab == عدم)
	أرجع;

    لكل (n = 0; clicktab[n].پیل != عدم; n++)
	شمېر++;
    إذا (شمېر == 0)
	أرجع;

    بنسټ_ستون = (tpl_align == ALIGN_ښی ? topframe->fr_پلنوالی : 0) + ستون_پیل;
    region_پیل_ستون = بنسټ_ستون;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    حجم(stl_click_region_T) * (tabpanel_stl_click_شمېر + شمېر + 1));
    إذا (new_arr == عدم)
	أرجع;
    tabpanel_stl_click = new_arr;

    p = buf;
    لكل (n = 0; clicktab[n].پیل != عدم; n++)
    {
	acc_پلنوالی += vim_strnsize(p, (صحيح)(clicktab[n].پیل - p));
	p = clicktab[n].پیل;
	حد = acc_پلنوالی < max_w ? acc_پلنوالی : max_w;

	إذا (cur_funcname != عدم)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_شمېر];
	    r->لیکه = لیکه;
	    r->ستون_پیل = region_پیل_ستون;
	    r->ستون_پای_ټکی = بنسټ_ستون + حد;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_شمېر++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_پیل_ستون = بنسټ_ستون + حد;
    }

    // Close the final region if it extends to the end.
    إذا (cur_funcname != عدم)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_شمېر];
	r->لیکه = لیکه;
	r->ستون_پیل = region_پیل_ستون;
	r->ستون_پای_ټکی = بنسټ_ستون + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_شمېر++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ساكن فراغ
follow_curtab_if_needed(صحيح curtab_لیکه)
{
    إذا (لیکې <= 0 || curtab == tpl_وروستی_curtab)
	أرجع;

    إذا (curtab_لیکه < tpl_scroll_دننه_واټن)
	tpl_scroll_دننه_واټن = curtab_لیکه;
    وإلا إذا (curtab_لیکه >= tpl_scroll_دننه_واټن + لیکې)
	tpl_scroll_دننه_واټن = curtab_لیکه - لیکې + 1;

    صحيح max_دننه_واټن = tpl_ټول_لیکې > لیکې ? tpl_ټول_لیکې - لیکې : 0;

    إذا (tpl_scroll_دننه_واټن < 0)
	tpl_scroll_دننه_واټن = 0;
    وإلا إذا (tpl_scroll_دننه_واټن > max_دننه_واټن)
	tpl_scroll_دننه_واټن = max_دننه_واټن;
}

/*
 * draw the tabpanel.
 */
    فراغ
draw_tabpanel(فراغ)
{
    صحيح saved_کلی_Typed = کلی_Typed;
    صحيح saved_got_int = got_int;
    صحيح maxwidth = tabpanel_پلنوالی();
    صحيح vs_attr = HL_ATTR(HLF_C);
    صحيح curtab_لیکه = 0;
    صحيح vsrow = 0;
    صحيح is_ښی = tpl_align == ALIGN_ښی;

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
    صحيح sb_screen_ستون = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    إذا (tpl_is_vert)
    {
	إذا (is_ښی)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_لیکه, VERT_LEN,
		    maxwidth - sb_len, &curtab_لیکه, عدم);
	    follow_curtab_if_needed(curtab_لیکه);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_لیکه, عدم);
	    لكل (vsrow = 0; vsrow < لیکې; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_پلنوالی, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_پلنوالی + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_لیکه, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_لیکه, عدم);
	    follow_curtab_if_needed(curtab_لیکه);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_لیکه, عدم);
	    لكل (vsrow = 0; vsrow < لیکې; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    وإلا
    {
	إذا (is_ښی)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_لیکه, 0, maxwidth - sb_len,
		    &curtab_لیکه, عدم);
	    follow_curtab_if_needed(curtab_لیکه);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_لیکه, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_پلنوالی + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_لیکه, 0, maxwidth - sb_len,
		    &curtab_لیکه, عدم);
	    follow_curtab_if_needed(curtab_لیکه);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_لیکه, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_ستون = sb_screen_ستون;
    إذا (sb_screen_ستون >= 0)
	draw_tabpanel_scrollbar(sb_screen_ستون);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    کلی_Typed = saved_کلی_Typed;

    tpl_وروستی_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحيح
get_tabpagenr_on_tabpanel(فراغ)
{
    صحيح		maxwidth = tabpanel_پلنوالی();
    صحيح		curtab_لیکه = 0;
    صحيح		tabpagenr = 0;

    إذا (maxwidth == 0)
	أرجع -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_لیکه, 0, maxwidth, &curtab_لیکه, عدم);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_لیکه,
	    &tabpagenr);

    أرجع tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ساكن فراغ
screen_fill_tailing_area(
	صحيح	tplmode,
	صحيح	لیکه_پیل,
	صحيح	لیکه_پای_ټکی,
	صحيح	ستون_پیل,
	صحيح	ستون_پای_ټکی,
	صحيح	attr)
{
    صحيح is_ښی = tpl_align == ALIGN_ښی;
    إذا (tplmode == TPLMODE_REDRAW)
	screen_fill(لیکه_پیل, لیکه_پای_ټکی,
		(is_ښی ? topframe->fr_پلنوالی : 0) + ستون_پیل,
		(is_ښی ? topframe->fr_پلنوالی : 0) + ستون_پای_ټکی,
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
    char_u	*لنډمهاله;

    لكل (j = 0; j < len;)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_لیکه
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
	لنډمهاله = transstr(buf);
	إذا (لنډمهاله != عدم)
	{
	    vim_strncpy(buf, لنډمهاله, حجم(buf) - 1);
	    vim_free(لنډمهاله);
	}

	إذا (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	وإلا
	    chcells = 1;

	إذا (pargs->ستون_پای_ټکی < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    إذا (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->ستون_پای_ټکی, attr);
	    *pargs->pcol = pargs->ستون_پای_ټکی;

	    إذا (pargs->ستون_پای_ټکی < chcells)
		اكسر;
	}

	إذا (*pargs->pcol + chcells <= pargs->ستون_پای_ټکی)
	{
	    صحيح off = (tpl_align == ALIGN_ښی)
		    ? topframe->fr_پلنوالی
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
	    pargs->wp = pargs->wp->w_بل, ++wincount)
	إذا (buf_Is_بدل_شوی(pargs->wp->w_بفر))
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
#د_که_پای
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

    get_trans_bufname(pargs->cwp->w_بفر);
    shorten_dir(NameBuff);
    len = (صحيح)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    إذا (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->ستون_پای_ټکی, pargs->attr);
    *pargs->pcol = pargs->ستون_پای_ټکی;
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
    لكل (n = 0; hltab[n].پیل != عدم; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحيح)(hltab[n].پیل - p),
		curattr, pargs);
	p = hltab[n].پیل;
	إذا (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	وإلا إذا (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#که_تعریف_شوی FEAT_TERMINAL
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_لوړوالی != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	وإلا إذا (pargs->wp != عدم && bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_لوړوالی != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#د_که_پای
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& pargs->wp->w_حالت_لوړوالی != 0)
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
		pargs->ستون_پای_ټکی, curattr);
    *pargs->pcol = pargs->ستون_پای_ټکی;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ساكن فراغ
do_by_tplmode(
	صحيح	tplmode,
	صحيح	ستون_پیل,
	صحيح	ستون_پای_ټکی,
	صحيح	*pcurtab_لیکه,
	صحيح	*ptabpagenr)
{
    صحيح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحيح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحيح		attr_tpl = HL_ATTR(HLF_TPL);
    صحيح		ستون = ستون_پیل;
    صحيح		لیکه = 0;
    tabpage_T	*tp = عدم;
    typval_T	v;
    tabpanel_T	دلیلونه;

    دلیلونه.maxrow = لیکې;
    دلیلونه.offsetrow = 0;
    دلیلونه.ستون_پیل = ستون_پیل;
    دلیلونه.ستون_پای_ټکی = ستون_پای_ټکی;

    إذا (tplmode != TPLMODE_GET_CURTAB_لیکه && دلیلونه.maxrow > 0)
	دلیلونه.offsetrow = tpl_scroll_دننه_واټن;

    tp = لومړی_tabpage;

    لكل (لیکه = 0; tp != عدم; لیکه++)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_لیکه
		&& دلیلونه.maxrow <= لیکه - دلیلونه.offsetrow)
	    اكسر;

	ستون = ستون_پیل;

	v.v_type = VAR_شمېره;
	v.vval.v_شمېره = tabpage_شاخص(tp);
	سېټ_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	إذا (tp->tp_topframe == topframe)
	{
	    دلیلونه.attr = attr_tpls;
	    إذا (tplmode == TPLMODE_GET_CURTAB_لیکه)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_لیکه = لیکه;
	}
	وإلا
	    دلیلونه.attr = attr_tpl;

	إذا (tp == curtab)
	{
	    دلیلونه.cwp = curwin;
	    دلیلونه.wp = firstwin;
	}
	وإلا
	{
	    دلیلونه.cwp = tp->tp_curwin;
	    دلیلونه.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	إذا (usefmt != عدم && *usefmt != NUL)
	{
	    صحيح	لېږد_hl = 0;

	    طالما (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = عدم;

		إذا (tplmode != TPLMODE_GET_CURTAB_لیکه
			&& دلیلونه.maxrow <= لیکه - دلیلونه.offsetrow)
		    اكسر;

		buf[0] = NUL;
#که_تعریف_شوی ENABLE_STL_ډېرپېښ_MULTI_NL
		(فراغ)build_stl_str_hl_mline_nl
#وإلا
		(فراغ)build_stl_str_hl_mline
#د_که_پای
			(دلیلونه.cwp, buf, حجم(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			دلیلونه.ستون_پای_ټکی - دلیلونه.ستون_پیل, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : عدم,
			&لېږد_hl);

		دلیلونه.prow = &لیکه;
		دلیلونه.pcol = &ستون;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &دلیلونه);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		إذا (tplmode == TPLMODE_REDRAW && clicktab != عدم)
		{
		    صحيح screen_لیکه = لیکه - دلیلونه.offsetrow;
		    صحيح m;

		    إذا (screen_لیکه >= 0 && screen_لیکه < دلیلونه.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_لیکه, دلیلونه.ستون_پیل, دلیلونه.ستون_پای_ټکی,
				(صحيح)v.vval.v_شمېره);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    لكل (m = 0; clicktab[m].پیل != عدم; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		إذا (*usefmt != NUL)
		{
		    لیکه++;
		    ستون = ستون_پیل;
		}
	    }
	}
	وإلا
	{
	    دلیلونه.user_defined = عدم;
	    دلیلونه.prow = &لیکه;
	    دلیلونه.pcol = &ستون;
	    draw_tabpanel_default(tplmode, &دلیلونه);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_بل;

	إذا ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_لیکه <= (لیکه - دلیلونه.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_شمېره;
	    اكسر;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(لیکه - دلیلونه.offsetrow, 0), دلیلونه.maxrow,
	    دلیلونه.ستون_پیل, دلیلونه.ستون_پای_ټکی, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    إذا (tplmode == TPLMODE_GET_CURTAB_لیکه)
	tpl_ټول_لیکې = لیکه;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ساكن فراغ
draw_tabpanel_scrollbar(صحيح screen_ستون)
{
    صحيح attr_sb = HL_ATTR(HLF_PSB);
    صحيح attr_thumb = HL_ATTR(HLF_PST);
    صحيح thumb_سر = 0;
    صحيح thumb_لوړوالی = 0;

    إذا (tpl_ټول_لیکې > لیکې && لیکې > 0)
    {
	صحيح max_دننه_واټن = tpl_ټول_لیکې - لیکې;
	صحيح track_range;

	thumb_لوړوالی = لیکې * لیکې / tpl_ټول_لیکې;
	إذا (thumb_لوړوالی < 1)
	    thumb_لوړوالی = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = لیکې - thumb_لوړوالی;
	إذا (track_range > 0 && max_دننه_واټن > 0)
	    thumb_سر = track_range * tpl_scroll_دننه_واټن / max_دننه_واټن;
	وإلا
	    thumb_سر = 0;
	إذا (thumb_سر + thumb_لوړوالی > لیکې)
	    thumb_سر = لیکې - thumb_لوړوالی;
	إذا (thumb_سر < 0)
	    thumb_سر = 0;
    }

    لكل (صحيح r = 0; r < لیکې; r++)
    {
	bool on_thumb = thumb_لوړوالی > 0
	    && r >= thumb_سر && r < thumb_سر + thumb_لوړوالی;
	screen_putchar(TPL_FILLCHAR, r, screen_ستون,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(فراغ)
{
    إذا (tabpanel_پلنوالی() == 0)
	أرجع false;
    أرجع mouse_ستون < firstwin->w_wincol
	|| mouse_ستون >= firstwin->w_wincol + topframe->fr_پلنوالی;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(فراغ)
{
    أرجع tpl_scrollbar && tpl_scrollbar_ستون >= 0
	&& mouse_ستون == tpl_scrollbar_ستون;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(صحيح screen_لیکه)
{
    صحيح thumb_لوړوالی;
    صحيح max_دننه_واټن;
    صحيح track_range;
    صحيح thumb_سر;
    صحيح new_دننه_واټن;

    إذا (!tpl_scrollbar || لیکې <= 0 || tpl_ټول_لیکې <= لیکې)
	أرجع false;

    thumb_لوړوالی = لیکې * لیکې / tpl_ټول_لیکې;
    إذا (thumb_لوړوالی < 1)
	thumb_لوړوالی = 1;
    track_range = لیکې - thumb_لوړوالی;
    إذا (track_range <= 0)
	أرجع true;

    max_دننه_واټن = tpl_ټول_لیکې - لیکې;
    thumb_سر = screen_لیکه - thumb_لوړوالی / 2;
    إذا (thumb_سر < 0)
	thumb_سر = 0;
    إذا (thumb_سر > track_range)
	thumb_سر = track_range;

    new_دننه_واټن = thumb_سر * max_دننه_واټن / track_range;
    إذا (new_دننه_واټن != tpl_scroll_دننه_واټن)
    {
	tpl_scroll_دننه_واټن = new_دننه_واټن;
	redraw_tabpanel = TRUE;
    }
    أرجع true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحيح dir, صحيح شمېر)
{
    صحيح max_دننه_واټن;
    صحيح new_دننه_واټن;

    إذا (tabpanel_پلنوالی() == 0)
	أرجع false;

    max_دننه_واټن = tpl_ټول_لیکې - لیکې;
    إذا (max_دننه_واټن < 0)
	max_دننه_واټن = 0;

    new_دننه_واټن = tpl_scroll_دننه_واټن + (dir > 0 ? شمېر : -شمېر);
    إذا (new_دننه_واټن < 0)
	new_دننه_واټن = 0;
    إذا (new_دننه_واټن > max_دننه_واټن)
	new_دننه_واټن = max_دننه_واټن;
    إذا (new_دننه_واټن == tpl_scroll_دننه_واټن)
	أرجع false;

    tpl_scroll_دننه_واټن = new_دننه_واټن;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_سېټ_دننه_واټن(صحيح دننه_واټن)
{
    صحيح max_دننه_واټن;

    إذا (tabpanel_پلنوالی() == 0)
	أرجع false;

    max_دننه_واټن = tpl_ټول_لیکې - لیکې;
    إذا (max_دننه_واټن < 0)
	max_دننه_واټن = 0;

    إذا (دننه_واټن < 0)
	دننه_واټن = 0;
    إذا (دننه_واټن > max_دننه_واټن)
	دننه_واټن = max_دننه_واټن;
    إذا (دننه_واټن == tpl_scroll_دننه_واټن)
	أرجع false;

    tpl_scroll_دننه_واټن = دننه_واټن;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * "tabpanel_getinfo()" function
 */
    فراغ
f_tabpanel_getinfo(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    dict_T	*d;
    صحيح		max_دننه_واټن;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    max_دننه_واټن = tpl_ټول_لیکې - لیکې;
    إذا (max_دننه_واټن < 0)
	max_دننه_واټن = 0;

    dict_جمع_کړه_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ښی ? "right" : "left"));
    dict_جمع_کړه_شمېره(d, "columns", tabpanel_پلنوالی());
    dict_جمع_کړه_bool(d, "scrollbar", tpl_scrollbar);
    dict_جمع_کړه_شمېره(d, "offset", tpl_scroll_دننه_واټن);
    dict_جمع_کړه_شمېره(d, "total", tpl_ټول_لیکې);
    dict_جمع_کړه_شمېره(d, "max_offset", max_دننه_واټن);
}

/*
 * "tabpanel_scroll()" function
 */
    فراغ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحيح		absolute = 0;
    bool	بدل_شوی;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_شمېره = VVAL_FALSE;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    n = tv_get_شمېره_chk(&argvars[0], عدم);
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
    إذا (n > لوی_عدد)
	n = لوی_عدد;
    وإلا إذا (n < -لوی_عدد)
	n = -لوی_عدد;

    إذا (absolute)
	بدل_شوی = tabpanel_سېټ_دننه_واټن((صحيح)n);
    وإلا
	بدل_شوی = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحيح)(n >= 0 ? n : -n));

    rettv->vval.v_شمېره = بدل_شوی ? VVAL_TRUE : VVAL_FALSE;
}

#د_که_پای // FEAT_TABPANEL
