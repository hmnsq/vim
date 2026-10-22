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

ساكن فراغ do_by_tplmode(صحيح tplmode, صحيح کانو_شروعات, صحيح کانو_پڇاڙي,
	صحيح *pcurtab_صف, صحيح *ptabpagenr);
ساكن فراغ tabpanel_free_click_regions(فراغ);
ساكن فراغ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحيح صف, صحيح کانو_شروعات, صحيح کانو_پڇاڙي, صحيح tabnr);
ساكن فراغ draw_tabpanel_scrollbar(صحيح screen_کانو);

// set pcurtab_row. don't redraw tabpanel.
#وصف_ڪريو TPLMODE_GET_CURTAB_صف	0
// set ptabpagenr. don't redraw tabpanel.
#وصف_ڪريو TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#وصف_ڪريو TPLMODE_REDRAW		2

#وصف_ڪريو TPL_FILLCHAR		' '

#وصف_ڪريو VERT_LEN		1
#وصف_ڪريو SCROLL_LEN		1

// tpl_align's values
#وصف_ڪريو ALIGN_کاٻو		0
#وصف_ڪريو ALIGN_ساڄو		1

ساكن char_u *opt_name = (char_u *)"tabpanel";
ساكن صحيح opt_scope = OPT_LOCAL;
ساكن صحيح tpl_align = ALIGN_کاٻو;
ساكن صحيح tpl_columns = 20;
ساكن bool tpl_is_vert = false;
ساكن bool tpl_scrollbar = false;
ساكن صحيح tpl_scroll_وٿي = 0;
ساكن صحيح tpl_ڪل_صفون = 0;
ساكن صحيح tpl_scrollbar_کانو = -1;  // screen column of scrollbar, -1 if none
ساكن tabpage_T *tpl_آخري_curtab = عدم;   // last curtab seen by draw_tabpanel

تعريفنوع بنية {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحيح	maxrow;
    صحيح	offsetrow;
    صحيح	*prow;
    صحيح	*pcol;
    صحيح	attr;
    صحيح	کانو_شروعات;
    صحيح	کانو_پڇاڙي;
} tabpanel_T;

    صحيح
tabpanelopt_بدليو(فراغ)
{
    char_u	*p;
    صحيح		new_align = ALIGN_کاٻو;
    ڊگهو	new_columns = 20;
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
		new_align = ALIGN_کاٻو;
	    }
	    وإلا إذا (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ساڄو;
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
    tpl_آخري_curtab = عدم;

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
    إذا (tpl_آخري_curtab == tp)
	tpl_آخري_curtab = عدم;
}

/*
 * Return the width of tabpanel.
 */
    صحيح
tabpanel_ويڪر(فراغ)
{
    بدل (p_stpl)
    {
	حالة 0:
	    أرجع 0;
	حالة 1:
	    إذا (پهريون_tabpage->tp_ايندڙ == عدم)
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
    أرجع tpl_align == ALIGN_ساڄو ? 0 : tabpanel_ويڪر();
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
	لكل (n = 0; n < tabpanel_stl_click_ڳڻپ; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_ڳڻپ = 0;
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
	صحيح		صف,
	صحيح		کانو_شروعات,
	صحيح		کانو_پڇاڙي,
	صحيح		tabnr)
{
    صحيح		ڳڻپ = 0;
    صحيح		n;
    صحيح		بنياد_کانو;
    صحيح		acc_ويڪر = 0;
    صحيح		max_w = کانو_پڇاڙي - کانو_شروعات;
    char_u	*p;
    char_u	*cur_funcname = عدم;
    صحيح		cur_minwid = 0;
    صحيح		region_شروعات_کانو;
    stl_click_region_T *new_arr;
    صحيح		حد_بندي;

    إذا (clicktab == عدم)
	أرجع;

    لكل (n = 0; clicktab[n].شروعات != عدم; n++)
	ڳڻپ++;
    إذا (ڳڻپ == 0)
	أرجع;

    بنياد_کانو = (tpl_align == ALIGN_ساڄو ? topframe->fr_ويڪر : 0) + کانو_شروعات;
    region_شروعات_کانو = بنياد_کانو;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    حجم(stl_click_region_T) * (tabpanel_stl_click_ڳڻپ + ڳڻپ + 1));
    إذا (new_arr == عدم)
	أرجع;
    tabpanel_stl_click = new_arr;

    p = buf;
    لكل (n = 0; clicktab[n].شروعات != عدم; n++)
    {
	acc_ويڪر += vim_strnsize(p, (صحيح)(clicktab[n].شروعات - p));
	p = clicktab[n].شروعات;
	حد_بندي = acc_ويڪر < max_w ? acc_ويڪر : max_w;

	إذا (cur_funcname != عدم)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ڳڻپ];
	    r->صف = صف;
	    r->کانو_شروعات = region_شروعات_کانو;
	    r->کانو_پڇاڙي = بنياد_کانو + حد_بندي;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ڳڻپ++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_شروعات_کانو = بنياد_کانو + حد_بندي;
    }

    // Close the final region if it extends to the end.
    إذا (cur_funcname != عدم)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ڳڻپ];
	r->صف = صف;
	r->کانو_شروعات = region_شروعات_کانو;
	r->کانو_پڇاڙي = بنياد_کانو + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_ڳڻپ++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ساكن فراغ
follow_curtab_if_needed(صحيح curtab_صف)
{
    إذا (صفون <= 0 || curtab == tpl_آخري_curtab)
	أرجع;

    إذا (curtab_صف < tpl_scroll_وٿي)
	tpl_scroll_وٿي = curtab_صف;
    وإلا إذا (curtab_صف >= tpl_scroll_وٿي + صفون)
	tpl_scroll_وٿي = curtab_صف - صفون + 1;

    صحيح max_وٿي = tpl_ڪل_صفون > صفون ? tpl_ڪل_صفون - صفون : 0;

    إذا (tpl_scroll_وٿي < 0)
	tpl_scroll_وٿي = 0;
    وإلا إذا (tpl_scroll_وٿي > max_وٿي)
	tpl_scroll_وٿي = max_وٿي;
}

/*
 * draw the tabpanel.
 */
    فراغ
draw_tabpanel(فراغ)
{
    صحيح saved_ڪنجي_Typed = ڪنجي_Typed;
    صحيح saved_got_int = got_int;
    صحيح maxwidth = tabpanel_ويڪر();
    صحيح vs_attr = HL_ATTR(HLF_C);
    صحيح curtab_صف = 0;
    صحيح vsrow = 0;
    صحيح is_ساڄو = tpl_align == ALIGN_ساڄو;

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
    صحيح sb_screen_کانو = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    إذا (tpl_is_vert)
    {
	إذا (is_ساڄو)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, VERT_LEN,
		    maxwidth - sb_len, &curtab_صف, عدم);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_صف, عدم);
	    لكل (vsrow = 0; vsrow < صفون; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_ويڪر, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_کانو = topframe->fr_ويڪر + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_صف, عدم);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_صف, عدم);
	    لكل (vsrow = 0; vsrow < صفون; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_کانو = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    وإلا
    {
	إذا (is_ساڄو)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth - sb_len,
		    &curtab_صف, عدم);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_صف, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_کانو = topframe->fr_ويڪر + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth - sb_len,
		    &curtab_صف, عدم);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_صف, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_کانو = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_کانو = sb_screen_کانو;
    إذا (sb_screen_کانو >= 0)
	draw_tabpanel_scrollbar(sb_screen_کانو);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ڪنجي_Typed = saved_ڪنجي_Typed;

    tpl_آخري_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحيح
get_tabpagenr_on_tabpanel(فراغ)
{
    صحيح		maxwidth = tabpanel_ويڪر();
    صحيح		curtab_صف = 0;
    صحيح		tabpagenr = 0;

    إذا (maxwidth == 0)
	أرجع -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth, &curtab_صف, عدم);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_صف,
	    &tabpagenr);

    أرجع tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ساكن فراغ
screen_fill_tailing_area(
	صحيح	tplmode,
	صحيح	صف_شروعات,
	صحيح	صف_پڇاڙي,
	صحيح	کانو_شروعات,
	صحيح	کانو_پڇاڙي,
	صحيح	attr)
{
    صحيح is_ساڄو = tpl_align == ALIGN_ساڄو;
    إذا (tplmode == TPLMODE_REDRAW)
	screen_fill(صف_شروعات, صف_پڇاڙي,
		(is_ساڄو ? topframe->fr_ويڪر : 0) + کانو_شروعات,
		(is_ساڄو ? topframe->fr_ويڪر : 0) + کانو_پڇاڙي,
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
    char_u	*عارضي;

    لكل (j = 0; j < len;)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_صف
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
	عارضي = transstr(buf);
	إذا (عارضي != عدم)
	{
	    vim_strncpy(buf, عارضي, حجم(buf) - 1);
	    vim_free(عارضي);
	}

	إذا (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	وإلا
	    chcells = 1;

	إذا (pargs->کانو_پڇاڙي < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    إذا (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->کانو_پڇاڙي, attr);
	    *pargs->pcol = pargs->کانو_پڇاڙي;

	    إذا (pargs->کانو_پڇاڙي < chcells)
		اكسر;
	}

	إذا (*pargs->pcol + chcells <= pargs->کانو_پڇاڙي)
	{
	    صحيح off = (tpl_align == ALIGN_ساڄو)
		    ? topframe->fr_ويڪر
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
	    pargs->wp = pargs->wp->w_ايندڙ, ++wincount)
	إذا (buf_Is_بدليو(pargs->wp->w_بفر))
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
#جيڪڏهن_پورو
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
		*pargs->pcol, pargs->کانو_پڇاڙي, pargs->attr);
    *pargs->pcol = pargs->کانو_پڇاڙي;
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
    لكل (n = 0; hltab[n].شروعات != عدم; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحيح)(hltab[n].شروعات - p),
		curattr, pargs);
	p = hltab[n].شروعات;
	إذا (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	وإلا إذا (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_اوچائي != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	وإلا إذا (pargs->wp != عدم && bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_اوچائي != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#جيڪڏهن_پورو
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& pargs->wp->w_حالت_اوچائي != 0)
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
		pargs->کانو_پڇاڙي, curattr);
    *pargs->pcol = pargs->کانو_پڇاڙي;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ساكن فراغ
do_by_tplmode(
	صحيح	tplmode,
	صحيح	کانو_شروعات,
	صحيح	کانو_پڇاڙي,
	صحيح	*pcurtab_صف,
	صحيح	*ptabpagenr)
{
    صحيح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحيح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحيح		attr_tpl = HL_ATTR(HLF_TPL);
    صحيح		کانو = کانو_شروعات;
    صحيح		صف = 0;
    tabpage_T	*tp = عدم;
    typval_T	v;
    tabpanel_T	دليلون;

    دليلون.maxrow = صفون;
    دليلون.offsetrow = 0;
    دليلون.کانو_شروعات = کانو_شروعات;
    دليلون.کانو_پڇاڙي = کانو_پڇاڙي;

    إذا (tplmode != TPLMODE_GET_CURTAB_صف && دليلون.maxrow > 0)
	دليلون.offsetrow = tpl_scroll_وٿي;

    tp = پهريون_tabpage;

    لكل (صف = 0; tp != عدم; صف++)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_صف
		&& دليلون.maxrow <= صف - دليلون.offsetrow)
	    اكسر;

	کانو = کانو_شروعات;

	v.v_type = VAR_عدد;
	v.vval.v_عدد = tabpage_اشاريو(tp);
	سيٽ_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	إذا (tp->tp_topframe == topframe)
	{
	    دليلون.attr = attr_tpls;
	    إذا (tplmode == TPLMODE_GET_CURTAB_صف)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_صف = صف;
	}
	وإلا
	    دليلون.attr = attr_tpl;

	إذا (tp == curtab)
	{
	    دليلون.cwp = curwin;
	    دليلون.wp = firstwin;
	}
	وإلا
	{
	    دليلون.cwp = tp->tp_curwin;
	    دليلون.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	إذا (usefmt != عدم && *usefmt != NUL)
	{
	    صحيح	حاصل_hl = 0;

	    طالما (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = عدم;

		إذا (tplmode != TPLMODE_GET_CURTAB_صف
			&& دليلون.maxrow <= صف - دليلون.offsetrow)
		    اكسر;

		buf[0] = NUL;
#جيڪڏهن_وصف_ٿيل ENABLE_STL_گهڻائي_MULTI_NL
		(فراغ)build_stl_str_hl_mline_nl
#وإلا
		(فراغ)build_stl_str_hl_mline
#جيڪڏهن_پورو
			(دليلون.cwp, buf, حجم(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			دليلون.کانو_پڇاڙي - دليلون.کانو_شروعات, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : عدم,
			&حاصل_hl);

		دليلون.prow = &صف;
		دليلون.pcol = &کانو;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &دليلون);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		إذا (tplmode == TPLMODE_REDRAW && clicktab != عدم)
		{
		    صحيح screen_صف = صف - دليلون.offsetrow;
		    صحيح m;

		    إذا (screen_صف >= 0 && screen_صف < دليلون.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_صف, دليلون.کانو_شروعات, دليلون.کانو_پڇاڙي,
				(صحيح)v.vval.v_عدد);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    لكل (m = 0; clicktab[m].شروعات != عدم; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		إذا (*usefmt != NUL)
		{
		    صف++;
		    کانو = کانو_شروعات;
		}
	    }
	}
	وإلا
	{
	    دليلون.user_defined = عدم;
	    دليلون.prow = &صف;
	    دليلون.pcol = &کانو;
	    draw_tabpanel_default(tplmode, &دليلون);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_ايندڙ;

	إذا ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_صف <= (صف - دليلون.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_عدد;
	    اكسر;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(صف - دليلون.offsetrow, 0), دليلون.maxrow,
	    دليلون.کانو_شروعات, دليلون.کانو_پڇاڙي, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    إذا (tplmode == TPLMODE_GET_CURTAB_صف)
	tpl_ڪل_صفون = صف;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ساكن فراغ
draw_tabpanel_scrollbar(صحيح screen_کانو)
{
    صحيح attr_sb = HL_ATTR(HLF_PSB);
    صحيح attr_thumb = HL_ATTR(HLF_PST);
    صحيح thumb_چوٽي = 0;
    صحيح thumb_اوچائي = 0;

    إذا (tpl_ڪل_صفون > صفون && صفون > 0)
    {
	صحيح max_وٿي = tpl_ڪل_صفون - صفون;
	صحيح track_range;

	thumb_اوچائي = صفون * صفون / tpl_ڪل_صفون;
	إذا (thumb_اوچائي < 1)
	    thumb_اوچائي = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = صفون - thumb_اوچائي;
	إذا (track_range > 0 && max_وٿي > 0)
	    thumb_چوٽي = track_range * tpl_scroll_وٿي / max_وٿي;
	وإلا
	    thumb_چوٽي = 0;
	إذا (thumb_چوٽي + thumb_اوچائي > صفون)
	    thumb_چوٽي = صفون - thumb_اوچائي;
	إذا (thumb_چوٽي < 0)
	    thumb_چوٽي = 0;
    }

    لكل (صحيح r = 0; r < صفون; r++)
    {
	bool on_thumb = thumb_اوچائي > 0
	    && r >= thumb_چوٽي && r < thumb_چوٽي + thumb_اوچائي;
	screen_putchar(TPL_FILLCHAR, r, screen_کانو,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(فراغ)
{
    إذا (tabpanel_ويڪر() == 0)
	أرجع false;
    أرجع mouse_کانو < firstwin->w_wincol
	|| mouse_کانو >= firstwin->w_wincol + topframe->fr_ويڪر;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(فراغ)
{
    أرجع tpl_scrollbar && tpl_scrollbar_کانو >= 0
	&& mouse_کانو == tpl_scrollbar_کانو;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(صحيح screen_صف)
{
    صحيح thumb_اوچائي;
    صحيح max_وٿي;
    صحيح track_range;
    صحيح thumb_چوٽي;
    صحيح new_وٿي;

    إذا (!tpl_scrollbar || صفون <= 0 || tpl_ڪل_صفون <= صفون)
	أرجع false;

    thumb_اوچائي = صفون * صفون / tpl_ڪل_صفون;
    إذا (thumb_اوچائي < 1)
	thumb_اوچائي = 1;
    track_range = صفون - thumb_اوچائي;
    إذا (track_range <= 0)
	أرجع true;

    max_وٿي = tpl_ڪل_صفون - صفون;
    thumb_چوٽي = screen_صف - thumb_اوچائي / 2;
    إذا (thumb_چوٽي < 0)
	thumb_چوٽي = 0;
    إذا (thumb_چوٽي > track_range)
	thumb_چوٽي = track_range;

    new_وٿي = thumb_چوٽي * max_وٿي / track_range;
    إذا (new_وٿي != tpl_scroll_وٿي)
    {
	tpl_scroll_وٿي = new_وٿي;
	redraw_tabpanel = TRUE;
    }
    أرجع true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحيح dir, صحيح ڳڻپ)
{
    صحيح max_وٿي;
    صحيح new_وٿي;

    إذا (tabpanel_ويڪر() == 0)
	أرجع false;

    max_وٿي = tpl_ڪل_صفون - صفون;
    إذا (max_وٿي < 0)
	max_وٿي = 0;

    new_وٿي = tpl_scroll_وٿي + (dir > 0 ? ڳڻپ : -ڳڻپ);
    إذا (new_وٿي < 0)
	new_وٿي = 0;
    إذا (new_وٿي > max_وٿي)
	new_وٿي = max_وٿي;
    إذا (new_وٿي == tpl_scroll_وٿي)
	أرجع false;

    tpl_scroll_وٿي = new_وٿي;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_سيٽ_وٿي(صحيح وٿي)
{
    صحيح max_وٿي;

    إذا (tabpanel_ويڪر() == 0)
	أرجع false;

    max_وٿي = tpl_ڪل_صفون - صفون;
    إذا (max_وٿي < 0)
	max_وٿي = 0;

    إذا (وٿي < 0)
	وٿي = 0;
    إذا (وٿي > max_وٿي)
	وٿي = max_وٿي;
    إذا (وٿي == tpl_scroll_وٿي)
	أرجع false;

    tpl_scroll_وٿي = وٿي;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * "tabpanel_getinfo()" function
 */
    فراغ
f_tabpanel_getinfo(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    dict_T	*d;
    صحيح		max_وٿي;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    max_وٿي = tpl_ڪل_صفون - صفون;
    إذا (max_وٿي < 0)
	max_وٿي = 0;

    dict_جوڙيو_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ساڄو ? "right" : "left"));
    dict_جوڙيو_عدد(d, "columns", tabpanel_ويڪر());
    dict_جوڙيو_bool(d, "scrollbar", tpl_scrollbar);
    dict_جوڙيو_عدد(d, "offset", tpl_scroll_وٿي);
    dict_جوڙيو_عدد(d, "total", tpl_ڪل_صفون);
    dict_جوڙيو_عدد(d, "max_offset", max_وٿي);
}

/*
 * "tabpanel_scroll()" function
 */
    فراغ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحيح		absolute = 0;
    bool	بدليو;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    إذا (اندريون_vim9script()
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
    إذا (n > وڌ_کان_وڌ_عدد)
	n = وڌ_کان_وڌ_عدد;
    وإلا إذا (n < -وڌ_کان_وڌ_عدد)
	n = -وڌ_کان_وڌ_عدد;

    إذا (absolute)
	بدليو = tabpanel_سيٽ_وٿي((صحيح)n);
    وإلا
	بدليو = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحيح)(n >= 0 ? n : -n));

    rettv->vval.v_عدد = بدليو ? VVAL_TRUE : VVAL_FALSE;
}

#جيڪڏهن_پورو // FEAT_TABPANEL
