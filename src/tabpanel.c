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

ساكن فراغ do_by_tplmode(صحيح tplmode, صحيح ستون_شروع, صحيح ستون_انتها,
	صحيح *pcurtab_ردیف, صحيح *ptabpagenr);
ساكن فراغ tabpanel_free_click_regions(فراغ);
ساكن فراغ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحيح ردیف, صحيح ستون_شروع, صحيح ستون_انتها, صحيح tabnr);
ساكن فراغ draw_tabpanel_scrollbar(صحيح screen_ستون);

// set pcurtab_row. don't redraw tabpanel.
#تعریف_کن TPLMODE_GET_CURTAB_ردیف	0
// set ptabpagenr. don't redraw tabpanel.
#تعریف_کن TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#تعریف_کن TPLMODE_REDRAW		2

#تعریف_کن TPL_FILLCHAR		' '

#تعریف_کن VERT_LEN		1
#تعریف_کن SCROLL_LEN		1

// tpl_align's values
#تعریف_کن ALIGN_چپ		0
#تعریف_کن ALIGN_راست		1

ساكن char_u *opt_name = (char_u *)"tabpanel";
ساكن صحيح opt_scope = OPT_LOCAL;
ساكن صحيح tpl_align = ALIGN_چپ;
ساكن صحيح tpl_columns = 20;
ساكن bool tpl_is_vert = false;
ساكن bool tpl_scrollbar = false;
ساكن صحيح tpl_scroll_جابه_جایی = 0;
ساكن صحيح tpl_مجموع_ردیفها = 0;
ساكن صحيح tpl_scrollbar_ستون = -1;  // screen column of scrollbar, -1 if none
ساكن tabpage_T *tpl_آخرین_curtab = عدم;   // last curtab seen by draw_tabpanel

تعريفنوع بنية {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحيح	maxrow;
    صحيح	offsetrow;
    صحيح	*prow;
    صحيح	*pcol;
    صحيح	attr;
    صحيح	ستون_شروع;
    صحيح	ستون_انتها;
} tabpanel_T;

    صحيح
tabpanelopt_تغییریافته(فراغ)
{
    char_u	*p;
    صحيح		new_align = ALIGN_چپ;
    دراز	new_columns = 20;
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
		new_align = ALIGN_چپ;
	    }
	    وإلا إذا (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_راست;
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
    tpl_آخرین_curtab = عدم;

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
    إذا (tpl_آخرین_curtab == tp)
	tpl_آخرین_curtab = عدم;
}

/*
 * Return the width of tabpanel.
 */
    صحيح
tabpanel_عرض(فراغ)
{
    بدل (p_stpl)
    {
	حالة 0:
	    أرجع 0;
	حالة 1:
	    إذا (اولی_tabpage->tp_بعدی == عدم)
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
    أرجع tpl_align == ALIGN_راست ? 0 : tabpanel_عرض();
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
	لكل (n = 0; n < tabpanel_stl_click_شمار; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_شمار = 0;
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
	صحيح		ردیف,
	صحيح		ستون_شروع,
	صحيح		ستون_انتها,
	صحيح		tabnr)
{
    صحيح		شمار = 0;
    صحيح		n;
    صحيح		پایه_ستون;
    صحيح		acc_عرض = 0;
    صحيح		max_w = ستون_انتها - ستون_شروع;
    char_u	*p;
    char_u	*cur_funcname = عدم;
    صحيح		cur_minwid = 0;
    صحيح		region_شروع_ستون;
    stl_click_region_T *new_arr;
    صحيح		حد;

    إذا (clicktab == عدم)
	أرجع;

    لكل (n = 0; clicktab[n].شروع != عدم; n++)
	شمار++;
    إذا (شمار == 0)
	أرجع;

    پایه_ستون = (tpl_align == ALIGN_راست ? topframe->fr_عرض : 0) + ستون_شروع;
    region_شروع_ستون = پایه_ستون;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    حجم(stl_click_region_T) * (tabpanel_stl_click_شمار + شمار + 1));
    إذا (new_arr == عدم)
	أرجع;
    tabpanel_stl_click = new_arr;

    p = buf;
    لكل (n = 0; clicktab[n].شروع != عدم; n++)
    {
	acc_عرض += vim_strnsize(p, (صحيح)(clicktab[n].شروع - p));
	p = clicktab[n].شروع;
	حد = acc_عرض < max_w ? acc_عرض : max_w;

	إذا (cur_funcname != عدم)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_شمار];
	    r->ردیف = ردیف;
	    r->ستون_شروع = region_شروع_ستون;
	    r->ستون_انتها = پایه_ستون + حد;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_شمار++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_شروع_ستون = پایه_ستون + حد;
    }

    // Close the final region if it extends to the end.
    إذا (cur_funcname != عدم)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_شمار];
	r->ردیف = ردیف;
	r->ستون_شروع = region_شروع_ستون;
	r->ستون_انتها = پایه_ستون + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_شمار++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ساكن فراغ
follow_curtab_if_needed(صحيح curtab_ردیف)
{
    إذا (ردیفها <= 0 || curtab == tpl_آخرین_curtab)
	أرجع;

    إذا (curtab_ردیف < tpl_scroll_جابه_جایی)
	tpl_scroll_جابه_جایی = curtab_ردیف;
    وإلا إذا (curtab_ردیف >= tpl_scroll_جابه_جایی + ردیفها)
	tpl_scroll_جابه_جایی = curtab_ردیف - ردیفها + 1;

    صحيح max_جابه_جایی = tpl_مجموع_ردیفها > ردیفها ? tpl_مجموع_ردیفها - ردیفها : 0;

    إذا (tpl_scroll_جابه_جایی < 0)
	tpl_scroll_جابه_جایی = 0;
    وإلا إذا (tpl_scroll_جابه_جایی > max_جابه_جایی)
	tpl_scroll_جابه_جایی = max_جابه_جایی;
}

/*
 * draw the tabpanel.
 */
    فراغ
draw_tabpanel(فراغ)
{
    صحيح saved_کلید_Typed = کلید_Typed;
    صحيح saved_got_int = got_int;
    صحيح maxwidth = tabpanel_عرض();
    صحيح vs_attr = HL_ATTR(HLF_C);
    صحيح curtab_ردیف = 0;
    صحيح vsrow = 0;
    صحيح is_راست = tpl_align == ALIGN_راست;

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
	إذا (is_راست)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ردیف, VERT_LEN,
		    maxwidth - sb_len, &curtab_ردیف, عدم);
	    follow_curtab_if_needed(curtab_ردیف);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_ردیف, عدم);
	    لكل (vsrow = 0; vsrow < ردیفها; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_عرض, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_عرض + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ردیف, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_ردیف, عدم);
	    follow_curtab_if_needed(curtab_ردیف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_ردیف, عدم);
	    لكل (vsrow = 0; vsrow < ردیفها; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    وإلا
    {
	إذا (is_راست)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ردیف, 0, maxwidth - sb_len,
		    &curtab_ردیف, عدم);
	    follow_curtab_if_needed(curtab_ردیف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ردیف, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_عرض + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ردیف, 0, maxwidth - sb_len,
		    &curtab_ردیف, عدم);
	    follow_curtab_if_needed(curtab_ردیف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ردیف, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_ستون = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_ستون = sb_screen_ستون;
    إذا (sb_screen_ستون >= 0)
	draw_tabpanel_scrollbar(sb_screen_ستون);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    کلید_Typed = saved_کلید_Typed;

    tpl_آخرین_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحيح
get_tabpagenr_on_tabpanel(فراغ)
{
    صحيح		maxwidth = tabpanel_عرض();
    صحيح		curtab_ردیف = 0;
    صحيح		tabpagenr = 0;

    إذا (maxwidth == 0)
	أرجع -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ردیف, 0, maxwidth, &curtab_ردیف, عدم);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_ردیف,
	    &tabpagenr);

    أرجع tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ساكن فراغ
screen_fill_tailing_area(
	صحيح	tplmode,
	صحيح	ردیف_شروع,
	صحيح	ردیف_انتها,
	صحيح	ستون_شروع,
	صحيح	ستون_انتها,
	صحيح	attr)
{
    صحيح is_راست = tpl_align == ALIGN_راست;
    إذا (tplmode == TPLMODE_REDRAW)
	screen_fill(ردیف_شروع, ردیف_انتها,
		(is_راست ? topframe->fr_عرض : 0) + ستون_شروع,
		(is_راست ? topframe->fr_عرض : 0) + ستون_انتها,
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
    char_u	*موقت;

    لكل (j = 0; j < len;)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_ردیف
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
	موقت = transstr(buf);
	إذا (موقت != عدم)
	{
	    vim_strncpy(buf, موقت, حجم(buf) - 1);
	    vim_free(موقت);
	}

	إذا (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	وإلا
	    chcells = 1;

	إذا (pargs->ستون_انتها < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    إذا (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->ستون_انتها, attr);
	    *pargs->pcol = pargs->ستون_انتها;

	    إذا (pargs->ستون_انتها < chcells)
		اكسر;
	}

	إذا (*pargs->pcol + chcells <= pargs->ستون_انتها)
	{
	    صحيح off = (tpl_align == ALIGN_راست)
		    ? topframe->fr_عرض
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
	    pargs->wp = pargs->wp->w_بعدی, ++wincount)
	إذا (buf_Is_تغییریافته(pargs->wp->w_میانگیر))
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
#پایان_اگر
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

    get_trans_bufname(pargs->cwp->w_میانگیر);
    shorten_dir(NameBuff);
    len = (صحيح)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    إذا (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->ستون_انتها, pargs->attr);
    *pargs->pcol = pargs->ستون_انتها;
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
    لكل (n = 0; hltab[n].شروع != عدم; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحيح)(hltab[n].شروع - p),
		curattr, pargs);
	p = hltab[n].شروع;
	إذا (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	وإلا إذا (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#اگر_تعریف_شده FEAT_TERMINAL
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_میانگیر)
		&& pargs->wp->w_وضعیت_ارتفاع != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	وإلا إذا (pargs->wp != عدم && bt_terminal(pargs->wp->w_میانگیر)
		&& pargs->wp->w_وضعیت_ارتفاع != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#پایان_اگر
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& pargs->wp->w_وضعیت_ارتفاع != 0)
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
		pargs->ستون_انتها, curattr);
    *pargs->pcol = pargs->ستون_انتها;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ساكن فراغ
do_by_tplmode(
	صحيح	tplmode,
	صحيح	ستون_شروع,
	صحيح	ستون_انتها,
	صحيح	*pcurtab_ردیف,
	صحيح	*ptabpagenr)
{
    صحيح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحيح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحيح		attr_tpl = HL_ATTR(HLF_TPL);
    صحيح		ستون = ستون_شروع;
    صحيح		ردیف = 0;
    tabpage_T	*tp = عدم;
    typval_T	v;
    tabpanel_T	آرگومانها;

    آرگومانها.maxrow = ردیفها;
    آرگومانها.offsetrow = 0;
    آرگومانها.ستون_شروع = ستون_شروع;
    آرگومانها.ستون_انتها = ستون_انتها;

    إذا (tplmode != TPLMODE_GET_CURTAB_ردیف && آرگومانها.maxrow > 0)
	آرگومانها.offsetrow = tpl_scroll_جابه_جایی;

    tp = اولی_tabpage;

    لكل (ردیف = 0; tp != عدم; ردیف++)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_ردیف
		&& آرگومانها.maxrow <= ردیف - آرگومانها.offsetrow)
	    اكسر;

	ستون = ستون_شروع;

	v.v_type = VAR_عدد;
	v.vval.v_عدد = tabpage_شاخص(tp);
	مجموعه_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	إذا (tp->tp_topframe == topframe)
	{
	    آرگومانها.attr = attr_tpls;
	    إذا (tplmode == TPLMODE_GET_CURTAB_ردیف)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_ردیف = ردیف;
	}
	وإلا
	    آرگومانها.attr = attr_tpl;

	إذا (tp == curtab)
	{
	    آرگومانها.cwp = curwin;
	    آرگومانها.wp = firstwin;
	}
	وإلا
	{
	    آرگومانها.cwp = tp->tp_curwin;
	    آرگومانها.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	إذا (usefmt != عدم && *usefmt != NUL)
	{
	    صحيح	انتقال_hl = 0;

	    طالما (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = عدم;

		إذا (tplmode != TPLMODE_GET_CURTAB_ردیف
			&& آرگومانها.maxrow <= ردیف - آرگومانها.offsetrow)
		    اكسر;

		buf[0] = NUL;
#اگر_تعریف_شده ENABLE_STL_نما_MULTI_NL
		(فراغ)build_stl_str_hl_mline_nl
#وإلا
		(فراغ)build_stl_str_hl_mline
#پایان_اگر
			(آرگومانها.cwp, buf, حجم(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			آرگومانها.ستون_انتها - آرگومانها.ستون_شروع, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : عدم,
			&انتقال_hl);

		آرگومانها.prow = &ردیف;
		آرگومانها.pcol = &ستون;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &آرگومانها);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		إذا (tplmode == TPLMODE_REDRAW && clicktab != عدم)
		{
		    صحيح screen_ردیف = ردیف - آرگومانها.offsetrow;
		    صحيح m;

		    إذا (screen_ردیف >= 0 && screen_ردیف < آرگومانها.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_ردیف, آرگومانها.ستون_شروع, آرگومانها.ستون_انتها,
				(صحيح)v.vval.v_عدد);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    لكل (m = 0; clicktab[m].شروع != عدم; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		إذا (*usefmt != NUL)
		{
		    ردیف++;
		    ستون = ستون_شروع;
		}
	    }
	}
	وإلا
	{
	    آرگومانها.user_defined = عدم;
	    آرگومانها.prow = &ردیف;
	    آرگومانها.pcol = &ستون;
	    draw_tabpanel_default(tplmode, &آرگومانها);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_بعدی;

	إذا ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_ردیف <= (ردیف - آرگومانها.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_عدد;
	    اكسر;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(ردیف - آرگومانها.offsetrow, 0), آرگومانها.maxrow,
	    آرگومانها.ستون_شروع, آرگومانها.ستون_انتها, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    إذا (tplmode == TPLMODE_GET_CURTAB_ردیف)
	tpl_مجموع_ردیفها = ردیف;
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
    صحيح thumb_بالاترین = 0;
    صحيح thumb_ارتفاع = 0;

    إذا (tpl_مجموع_ردیفها > ردیفها && ردیفها > 0)
    {
	صحيح max_جابه_جایی = tpl_مجموع_ردیفها - ردیفها;
	صحيح track_range;

	thumb_ارتفاع = ردیفها * ردیفها / tpl_مجموع_ردیفها;
	إذا (thumb_ارتفاع < 1)
	    thumb_ارتفاع = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = ردیفها - thumb_ارتفاع;
	إذا (track_range > 0 && max_جابه_جایی > 0)
	    thumb_بالاترین = track_range * tpl_scroll_جابه_جایی / max_جابه_جایی;
	وإلا
	    thumb_بالاترین = 0;
	إذا (thumb_بالاترین + thumb_ارتفاع > ردیفها)
	    thumb_بالاترین = ردیفها - thumb_ارتفاع;
	إذا (thumb_بالاترین < 0)
	    thumb_بالاترین = 0;
    }

    لكل (صحيح r = 0; r < ردیفها; r++)
    {
	bool on_thumb = thumb_ارتفاع > 0
	    && r >= thumb_بالاترین && r < thumb_بالاترین + thumb_ارتفاع;
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
    إذا (tabpanel_عرض() == 0)
	أرجع false;
    أرجع mouse_ستون < firstwin->w_wincol
	|| mouse_ستون >= firstwin->w_wincol + topframe->fr_عرض;
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
tabpanel_drag_scrollbar(صحيح screen_ردیف)
{
    صحيح thumb_ارتفاع;
    صحيح max_جابه_جایی;
    صحيح track_range;
    صحيح thumb_بالاترین;
    صحيح new_جابه_جایی;

    إذا (!tpl_scrollbar || ردیفها <= 0 || tpl_مجموع_ردیفها <= ردیفها)
	أرجع false;

    thumb_ارتفاع = ردیفها * ردیفها / tpl_مجموع_ردیفها;
    إذا (thumb_ارتفاع < 1)
	thumb_ارتفاع = 1;
    track_range = ردیفها - thumb_ارتفاع;
    إذا (track_range <= 0)
	أرجع true;

    max_جابه_جایی = tpl_مجموع_ردیفها - ردیفها;
    thumb_بالاترین = screen_ردیف - thumb_ارتفاع / 2;
    إذا (thumb_بالاترین < 0)
	thumb_بالاترین = 0;
    إذا (thumb_بالاترین > track_range)
	thumb_بالاترین = track_range;

    new_جابه_جایی = thumb_بالاترین * max_جابه_جایی / track_range;
    إذا (new_جابه_جایی != tpl_scroll_جابه_جایی)
    {
	tpl_scroll_جابه_جایی = new_جابه_جایی;
	redraw_tabpanel = TRUE;
    }
    أرجع true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحيح dir, صحيح شمار)
{
    صحيح max_جابه_جایی;
    صحيح new_جابه_جایی;

    إذا (tabpanel_عرض() == 0)
	أرجع false;

    max_جابه_جایی = tpl_مجموع_ردیفها - ردیفها;
    إذا (max_جابه_جایی < 0)
	max_جابه_جایی = 0;

    new_جابه_جایی = tpl_scroll_جابه_جایی + (dir > 0 ? شمار : -شمار);
    إذا (new_جابه_جایی < 0)
	new_جابه_جایی = 0;
    إذا (new_جابه_جایی > max_جابه_جایی)
	new_جابه_جایی = max_جابه_جایی;
    إذا (new_جابه_جایی == tpl_scroll_جابه_جایی)
	أرجع false;

    tpl_scroll_جابه_جایی = new_جابه_جایی;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_مجموعه_جابه_جایی(صحيح جابه_جایی)
{
    صحيح max_جابه_جایی;

    إذا (tabpanel_عرض() == 0)
	أرجع false;

    max_جابه_جایی = tpl_مجموع_ردیفها - ردیفها;
    إذا (max_جابه_جایی < 0)
	max_جابه_جایی = 0;

    إذا (جابه_جایی < 0)
	جابه_جایی = 0;
    إذا (جابه_جایی > max_جابه_جایی)
	جابه_جایی = max_جابه_جایی;
    إذا (جابه_جایی == tpl_scroll_جابه_جایی)
	أرجع false;

    tpl_scroll_جابه_جایی = جابه_جایی;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * "tabpanel_getinfo()" function
 */
    فراغ
f_tabpanel_getinfo(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    dict_T	*d;
    صحيح		max_جابه_جایی;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    max_جابه_جایی = tpl_مجموع_ردیفها - ردیفها;
    إذا (max_جابه_جایی < 0)
	max_جابه_جایی = 0;

    dict_جمع_کن_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_راست ? "right" : "left"));
    dict_جمع_کن_عدد(d, "columns", tabpanel_عرض());
    dict_جمع_کن_bool(d, "scrollbar", tpl_scrollbar);
    dict_جمع_کن_عدد(d, "offset", tpl_scroll_جابه_جایی);
    dict_جمع_کن_عدد(d, "total", tpl_مجموع_ردیفها);
    dict_جمع_کن_عدد(d, "max_offset", max_جابه_جایی);
}

/*
 * "tabpanel_scroll()" function
 */
    فراغ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحيح		absolute = 0;
    bool	تغییریافته;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    n = tv_get_عدد_chk(&argvars[0], عدم);
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
    إذا (n > بیشترین_صحیح)
	n = بیشترین_صحیح;
    وإلا إذا (n < -بیشترین_صحیح)
	n = -بیشترین_صحیح;

    إذا (absolute)
	تغییریافته = tabpanel_مجموعه_جابه_جایی((صحيح)n);
    وإلا
	تغییریافته = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحيح)(n >= 0 ? n : -n));

    rettv->vval.v_عدد = تغییریافته ? VVAL_TRUE : VVAL_FALSE;
}

#پایان_اگر // FEAT_TABPANEL
