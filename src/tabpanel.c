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

#شامل "vim.h"

#اگر defined(FEAT_TABPANEL)

ساکن خالی do_by_tplmode(صحیح tplmode, صحیح ستون_آغاز, صحیح ستون_انتہا,
	صحیح *pcurtab_صف, صحیح *ptabpagenr);
ساکن خالی tabpanel_free_click_regions(خالی);
ساکن خالی tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحیح صف, صحیح ستون_آغاز, صحیح ستون_انتہا, صحیح tabnr);
ساکن خالی draw_tabpanel_scrollbar(صحیح screen_ستون);

// set pcurtab_row. don't redraw tabpanel.
#تعریف_کرو TPLMODE_GET_CURTAB_صف	0
// set ptabpagenr. don't redraw tabpanel.
#تعریف_کرو TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#تعریف_کرو TPLMODE_REDRAW		2

#تعریف_کرو TPL_FILLCHAR		' '

#تعریف_کرو VERT_LEN		1
#تعریف_کرو SCROLL_LEN		1

// tpl_align's values
#تعریف_کرو ALIGN_بایاں		0
#تعریف_کرو ALIGN_دایاں		1

ساکن char_u *opt_name = (char_u *)"tabpanel";
ساکن صحیح opt_scope = OPT_LOCAL;
ساکن صحیح tpl_align = ALIGN_بایاں;
ساکن صحیح tpl_columns = 20;
ساکن bool tpl_is_vert = false;
ساکن bool tpl_scrollbar = false;
ساکن صحیح tpl_scroll_انحراف = 0;
ساکن صحیح tpl_کل_صفیں = 0;
ساکن صحیح tpl_scrollbar_ستون = -1;  // screen column of scrollbar, -1 if none
ساکن tabpage_T *tpl_آخری_curtab = تہی;   // last curtab seen by draw_tabpanel

قسمتعریف ساخت {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحیح	maxrow;
    صحیح	offsetrow;
    صحیح	*prow;
    صحیح	*pcol;
    صحیح	attr;
    صحیح	ستون_آغاز;
    صحیح	ستون_انتہا;
} tabpanel_T;

    صحیح
tabpanelopt_بدلا(خالی)
{
    char_u	*p;
    صحیح		new_align = ALIGN_بایاں;
    طویل	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    جب (*p != NUL)
    {
	اگر (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    اگر (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_بایاں;
	    }
	    ورنہ اگر (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_دایاں;
	    }
	    ورنہ
		واپس FAIL;
	}
	ورنہ اگر (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    اگر (new_columns < 0 || new_columns > 1000)
		واپس FAIL;
	}
	ورنہ اگر (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	ورنہ اگر (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_آخری_curtab = تہی;

    shell_new_columns();
    واپس OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    خالی
tabpanel_forget_tabpage(ثابت tabpage_T *tp)
{
    اگر (tpl_آخری_curtab == tp)
	tpl_آخری_curtab = تہی;
}

/*
 * Return the width of tabpanel.
 */
    صحیح
tabpanel_چوڑائی(خالی)
{
    انتخاب (p_stpl)
    {
	حالت 0:
	    واپس 0;
	حالت 1:
	    اگر (پہلا_tabpage->tp_اگلا == تہی)
		واپس 0;
    }
    اگر (Columns < tpl_columns)
	واپس 0;
    ورنہ
	واپس tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    صحیح
tabpanel_leftcol(خالی)
{
    واپس tpl_align == ALIGN_دایاں ? 0 : tabpanel_چوڑائی();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ساکن خالی
tabpanel_free_click_regions(خالی)
{
    صحیح n;

    اگر (tabpanel_stl_click != تہی)
    {
	برائے (n = 0; n < tabpanel_stl_click_گنتی; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_گنتی = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ساکن خالی
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	صحیح		صف,
	صحیح		ستون_آغاز,
	صحیح		ستون_انتہا,
	صحیح		tabnr)
{
    صحیح		گنتی = 0;
    صحیح		n;
    صحیح		بنیاد_ستون;
    صحیح		acc_چوڑائی = 0;
    صحیح		max_w = ستون_انتہا - ستون_آغاز;
    char_u	*p;
    char_u	*cur_funcname = تہی;
    صحیح		cur_minwid = 0;
    صحیح		region_آغاز_ستون;
    stl_click_region_T *new_arr;
    صحیح		حد_بندی;

    اگر (clicktab == تہی)
	واپس;

    برائے (n = 0; clicktab[n].آغاز != تہی; n++)
	گنتی++;
    اگر (گنتی == 0)
	واپس;

    بنیاد_ستون = (tpl_align == ALIGN_دایاں ? topframe->fr_چوڑائی : 0) + ستون_آغاز;
    region_آغاز_ستون = بنیاد_ستون;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    حجم(stl_click_region_T) * (tabpanel_stl_click_گنتی + گنتی + 1));
    اگر (new_arr == تہی)
	واپس;
    tabpanel_stl_click = new_arr;

    p = buf;
    برائے (n = 0; clicktab[n].آغاز != تہی; n++)
    {
	acc_چوڑائی += vim_strnsize(p, (صحیح)(clicktab[n].آغاز - p));
	p = clicktab[n].آغاز;
	حد_بندی = acc_چوڑائی < max_w ? acc_چوڑائی : max_w;

	اگر (cur_funcname != تہی)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_گنتی];
	    r->صف = صف;
	    r->ستون_آغاز = region_آغاز_ستون;
	    r->ستون_انتہا = بنیاد_ستون + حد_بندی;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_گنتی++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_آغاز_ستون = بنیاد_ستون + حد_بندی;
    }

    // Close the final region if it extends to the end.
    اگر (cur_funcname != تہی)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_گنتی];
	r->صف = صف;
	r->ستون_آغاز = region_آغاز_ستون;
	r->ستون_انتہا = بنیاد_ستون + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_گنتی++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ساکن خالی
follow_curtab_if_needed(صحیح curtab_صف)
{
    اگر (صفیں <= 0 || curtab == tpl_آخری_curtab)
	واپس;

    اگر (curtab_صف < tpl_scroll_انحراف)
	tpl_scroll_انحراف = curtab_صف;
    ورنہ اگر (curtab_صف >= tpl_scroll_انحراف + صفیں)
	tpl_scroll_انحراف = curtab_صف - صفیں + 1;

    صحیح max_انحراف = tpl_کل_صفیں > صفیں ? tpl_کل_صفیں - صفیں : 0;

    اگر (tpl_scroll_انحراف < 0)
	tpl_scroll_انحراف = 0;
    ورنہ اگر (tpl_scroll_انحراف > max_انحراف)
	tpl_scroll_انحراف = max_انحراف;
}

/*
 * draw the tabpanel.
 */
    خالی
draw_tabpanel(خالی)
{
    صحیح saved_کلید_Typed = کلید_Typed;
    صحیح saved_got_int = got_int;
    صحیح maxwidth = tabpanel_چوڑائی();
    صحیح vs_attr = HL_ATTR(HLF_C);
    صحیح curtab_صف = 0;
    صحیح vsrow = 0;
    صحیح is_دایاں = tpl_align == ALIGN_دایاں;

    اگر (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	واپس;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    صحیح sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    صحیح sb_screen_ستون = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    اگر (tpl_is_vert)
    {
	اگر (is_دایاں)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, VERT_LEN,
		    maxwidth - sb_len, &curtab_صف, تہی);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_صف, تہی);
	    برائے (vsrow = 0; vsrow < صفیں; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_چوڑائی, vs_attr);
	    اگر (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_چوڑائی + maxwidth - SCROLL_LEN;
	}
	ورنہ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_صف, تہی);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_صف, تہی);
	    برائے (vsrow = 0; vsrow < صفیں; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    اگر (tpl_scrollbar)
		sb_screen_ستون = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    ورنہ
    {
	اگر (is_دایاں)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth - sb_len,
		    &curtab_صف, تہی);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_صف, تہی);
	    اگر (tpl_scrollbar)
		sb_screen_ستون = topframe->fr_چوڑائی + maxwidth - SCROLL_LEN;
	}
	ورنہ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth - sb_len,
		    &curtab_صف, تہی);
	    follow_curtab_if_needed(curtab_صف);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_صف, تہی);
	    اگر (tpl_scrollbar)
		sb_screen_ستون = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_ستون = sb_screen_ستون;
    اگر (sb_screen_ستون >= 0)
	draw_tabpanel_scrollbar(sb_screen_ستون);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    کلید_Typed = saved_کلید_Typed;

    tpl_آخری_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحیح
get_tabpagenr_on_tabpanel(خالی)
{
    صحیح		maxwidth = tabpanel_چوڑائی();
    صحیح		curtab_صف = 0;
    صحیح		tabpagenr = 0;

    اگر (maxwidth == 0)
	واپس -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_صف, 0, maxwidth, &curtab_صف, تہی);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_صف,
	    &tabpagenr);

    واپس tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ساکن خالی
screen_fill_tailing_area(
	صحیح	tplmode,
	صحیح	صف_آغاز,
	صحیح	صف_انتہا,
	صحیح	ستون_آغاز,
	صحیح	ستون_انتہا,
	صحیح	attr)
{
    صحیح is_دایاں = tpl_align == ALIGN_دایاں;
    اگر (tplmode == TPLMODE_REDRAW)
	screen_fill(صف_آغاز, صف_انتہا,
		(is_دایاں ? topframe->fr_چوڑائی : 0) + ستون_آغاز,
		(is_دایاں ? topframe->fr_چوڑائی : 0) + ستون_انتہا,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ساکن خالی
screen_puts_len_for_tabpanel(
	صحیح	    tplmode,
	char_u	    *p,
	صحیح	    len,
	صحیح	    attr,
	tabpanel_T  *pargs)
{
    صحیح		j;
    صحیح		chlen;
    صحیح		chcells;
    char_u	buf[IOSIZE];
    char_u	*عارضی;

    برائے (j = 0; j < len;)
    {
	اگر (tplmode != TPLMODE_GET_CURTAB_صف
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    توڑو;

	اگر (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	ورنہ
	    chlen = 1;

	برائے (صحیح k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	عارضی = transstr(buf);
	اگر (عارضی != تہی)
	{
	    vim_strncpy(buf, عارضی, حجم(buf) - 1);
	    vim_free(عارضی);
	}

	اگر (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	ورنہ
	    chcells = 1;

	اگر (pargs->ستون_انتہا < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    اگر (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->ستون_انتہا, attr);
	    *pargs->pcol = pargs->ستون_انتہا;

	    اگر (pargs->ستون_انتہا < chcells)
		توڑو;
	}

	اگر (*pargs->pcol + chcells <= pargs->ستون_انتہا)
	{
	    صحیح off = (tpl_align == ALIGN_دایاں)
		    ? topframe->fr_چوڑائی
		    : 0;
	    اگر (tplmode == TPLMODE_REDRAW
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
    ساکن خالی
draw_tabpanel_default(صحیح tplmode, tabpanel_T *pargs)
{
    صحیح		modified;
    صحیح		wincount;
    صحیح		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    برائے (wincount = 0; pargs->wp != تہی;
	    pargs->wp = pargs->wp->w_اگلا, ++wincount)
	اگر (buf_Is_بدلا(pargs->wp->w_بفر))
	    modified = TRUE;

    اگر (modified || wincount > 1)
    {
	اگر (wincount > 1)
	{
	    vim_snprintf((حرف *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (صحیح)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#اگر defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#ورنہ
		    pargs->attr,
#اگر_ختم
		    pargs);
	}
	اگر (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_بفر);
    shorten_dir(NameBuff);
    len = (صحیح)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    اگر (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->ستون_انتہا, pargs->attr);
    *pargs->pcol = pargs->ستون_انتہا;
}

/*
 * Draw tabpanel content with highlight handling.
 * Processes hltab entries and fills tailing area.
 */
    ساکن خالی
draw_tabpanel_with_highlight(
	صحیح	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    صحیح		curattr;
    صحیح		n;

    curattr = pargs->attr;
    p = buf;
    برائے (n = 0; hltab[n].آغاز != تہی; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحیح)(hltab[n].آغاز - p),
		curattr, pargs);
	p = hltab[n].آغاز;
	اگر (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	ورنہ اگر (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#اگر_تعریف_شدہ FEAT_TERMINAL
	ورنہ اگر (pargs->wp != تہی && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_اونچائی != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	ورنہ اگر (pargs->wp != تہی && bt_terminal(pargs->wp->w_بفر)
		&& pargs->wp->w_حالت_اونچائی != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#اگر_ختم
	ورنہ اگر (pargs->wp != تہی && pargs->wp != curwin
		&& pargs->wp->w_حالت_اونچائی != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	ورنہ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (صحیح)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    اگر (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->ستون_انتہا, curattr);
    *pargs->pcol = pargs->ستون_انتہا;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ساکن خالی
do_by_tplmode(
	صحیح	tplmode,
	صحیح	ستون_آغاز,
	صحیح	ستون_انتہا,
	صحیح	*pcurtab_صف,
	صحیح	*ptabpagenr)
{
    صحیح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحیح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحیح		attr_tpl = HL_ATTR(HLF_TPL);
    صحیح		ستون = ستون_آغاز;
    صحیح		صف = 0;
    tabpage_T	*tp = تہی;
    typval_T	v;
    tabpanel_T	دلائل;

    دلائل.maxrow = صفیں;
    دلائل.offsetrow = 0;
    دلائل.ستون_آغاز = ستون_آغاز;
    دلائل.ستون_انتہا = ستون_انتہا;

    اگر (tplmode != TPLMODE_GET_CURTAB_صف && دلائل.maxrow > 0)
	دلائل.offsetrow = tpl_scroll_انحراف;

    tp = پہلا_tabpage;

    برائے (صف = 0; tp != تہی; صف++)
    {
	اگر (tplmode != TPLMODE_GET_CURTAB_صف
		&& دلائل.maxrow <= صف - دلائل.offsetrow)
	    توڑو;

	ستون = ستون_آغاز;

	v.v_type = VAR_نمبر;
	v.vval.v_نمبر = tabpage_اشاریہ(tp);
	سیٹ_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	اگر (tp->tp_topframe == topframe)
	{
	    دلائل.attr = attr_tpls;
	    اگر (tplmode == TPLMODE_GET_CURTAB_صف)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_صف = صف;
	}
	ورنہ
	    دلائل.attr = attr_tpl;

	اگر (tp == curtab)
	{
	    دلائل.cwp = curwin;
	    دلائل.wp = firstwin;
	}
	ورنہ
	{
	    دلائل.cwp = tp->tp_curwin;
	    دلائل.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	اگر (usefmt != تہی && *usefmt != NUL)
	{
	    صحیح	حاصل_hl = 0;

	    جب (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = تہی;

		اگر (tplmode != TPLMODE_GET_CURTAB_صف
			&& دلائل.maxrow <= صف - دلائل.offsetrow)
		    توڑو;

		buf[0] = NUL;
#اگر_تعریف_شدہ ENABLE_STL_منوال_MULTI_NL
		(خالی)build_stl_str_hl_mline_nl
#ورنہ
		(خالی)build_stl_str_hl_mline
#اگر_ختم
			(دلائل.cwp, buf, حجم(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			دلائل.ستون_انتہا - دلائل.ستون_آغاز, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : تہی,
			&حاصل_hl);

		دلائل.prow = &صف;
		دلائل.pcol = &ستون;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &دلائل);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		اگر (tplmode == TPLMODE_REDRAW && clicktab != تہی)
		{
		    صحیح screen_صف = صف - دلائل.offsetrow;
		    صحیح m;

		    اگر (screen_صف >= 0 && screen_صف < دلائل.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_صف, دلائل.ستون_آغاز, دلائل.ستون_انتہا,
				(صحیح)v.vval.v_نمبر);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    برائے (m = 0; clicktab[m].آغاز != تہی; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		اگر (*usefmt != NUL)
		{
		    صف++;
		    ستون = ستون_آغاز;
		}
	    }
	}
	ورنہ
	{
	    دلائل.user_defined = تہی;
	    دلائل.prow = &صف;
	    دلائل.pcol = &ستون;
	    draw_tabpanel_default(tplmode, &دلائل);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_اگلا;

	اگر ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_صف <= (صف - دلائل.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_نمبر;
	    توڑو;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(صف - دلائل.offsetrow, 0), دلائل.maxrow,
	    دلائل.ستون_آغاز, دلائل.ستون_انتہا, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    اگر (tplmode == TPLMODE_GET_CURTAB_صف)
	tpl_کل_صفیں = صف;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ساکن خالی
draw_tabpanel_scrollbar(صحیح screen_ستون)
{
    صحیح attr_sb = HL_ATTR(HLF_PSB);
    صحیح attr_thumb = HL_ATTR(HLF_PST);
    صحیح thumb_چوٹی = 0;
    صحیح thumb_اونچائی = 0;

    اگر (tpl_کل_صفیں > صفیں && صفیں > 0)
    {
	صحیح max_انحراف = tpl_کل_صفیں - صفیں;
	صحیح track_range;

	thumb_اونچائی = صفیں * صفیں / tpl_کل_صفیں;
	اگر (thumb_اونچائی < 1)
	    thumb_اونچائی = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = صفیں - thumb_اونچائی;
	اگر (track_range > 0 && max_انحراف > 0)
	    thumb_چوٹی = track_range * tpl_scroll_انحراف / max_انحراف;
	ورنہ
	    thumb_چوٹی = 0;
	اگر (thumb_چوٹی + thumb_اونچائی > صفیں)
	    thumb_چوٹی = صفیں - thumb_اونچائی;
	اگر (thumb_چوٹی < 0)
	    thumb_چوٹی = 0;
    }

    برائے (صحیح r = 0; r < صفیں; r++)
    {
	bool on_thumb = thumb_اونچائی > 0
	    && r >= thumb_چوٹی && r < thumb_چوٹی + thumb_اونچائی;
	screen_putchar(TPL_FILLCHAR, r, screen_ستون,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(خالی)
{
    اگر (tabpanel_چوڑائی() == 0)
	واپس false;
    واپس mouse_ستون < firstwin->w_wincol
	|| mouse_ستون >= firstwin->w_wincol + topframe->fr_چوڑائی;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(خالی)
{
    واپس tpl_scrollbar && tpl_scrollbar_ستون >= 0
	&& mouse_ستون == tpl_scrollbar_ستون;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(صحیح screen_صف)
{
    صحیح thumb_اونچائی;
    صحیح max_انحراف;
    صحیح track_range;
    صحیح thumb_چوٹی;
    صحیح new_انحراف;

    اگر (!tpl_scrollbar || صفیں <= 0 || tpl_کل_صفیں <= صفیں)
	واپس false;

    thumb_اونچائی = صفیں * صفیں / tpl_کل_صفیں;
    اگر (thumb_اونچائی < 1)
	thumb_اونچائی = 1;
    track_range = صفیں - thumb_اونچائی;
    اگر (track_range <= 0)
	واپس true;

    max_انحراف = tpl_کل_صفیں - صفیں;
    thumb_چوٹی = screen_صف - thumb_اونچائی / 2;
    اگر (thumb_چوٹی < 0)
	thumb_چوٹی = 0;
    اگر (thumb_چوٹی > track_range)
	thumb_چوٹی = track_range;

    new_انحراف = thumb_چوٹی * max_انحراف / track_range;
    اگر (new_انحراف != tpl_scroll_انحراف)
    {
	tpl_scroll_انحراف = new_انحراف;
	redraw_tabpanel = TRUE;
    }
    واپس true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحیح dir, صحیح گنتی)
{
    صحیح max_انحراف;
    صحیح new_انحراف;

    اگر (tabpanel_چوڑائی() == 0)
	واپس false;

    max_انحراف = tpl_کل_صفیں - صفیں;
    اگر (max_انحراف < 0)
	max_انحراف = 0;

    new_انحراف = tpl_scroll_انحراف + (dir > 0 ? گنتی : -گنتی);
    اگر (new_انحراف < 0)
	new_انحراف = 0;
    اگر (new_انحراف > max_انحراف)
	new_انحراف = max_انحراف;
    اگر (new_انحراف == tpl_scroll_انحراف)
	واپس false;

    tpl_scroll_انحراف = new_انحراف;
    redraw_tabpanel = TRUE;
    واپس true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_سیٹ_انحراف(صحیح انحراف)
{
    صحیح max_انحراف;

    اگر (tabpanel_چوڑائی() == 0)
	واپس false;

    max_انحراف = tpl_کل_صفیں - صفیں;
    اگر (max_انحراف < 0)
	max_انحراف = 0;

    اگر (انحراف < 0)
	انحراف = 0;
    اگر (انحراف > max_انحراف)
	انحراف = max_انحراف;
    اگر (انحراف == tpl_scroll_انحراف)
	واپس false;

    tpl_scroll_انحراف = انحراف;
    redraw_tabpanel = TRUE;
    واپس true;
}

/*
 * "tabpanel_getinfo()" function
 */
    خالی
f_tabpanel_getinfo(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    dict_T	*d;
    صحیح		max_انحراف;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    d = rettv->vval.v_dict;

    max_انحراف = tpl_کل_صفیں - صفیں;
    اگر (max_انحراف < 0)
	max_انحراف = 0;

    dict_جوڑو_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_دایاں ? "right" : "left"));
    dict_جوڑو_نمبر(d, "columns", tabpanel_چوڑائی());
    dict_جوڑو_bool(d, "scrollbar", tpl_scrollbar);
    dict_جوڑو_نمبر(d, "offset", tpl_scroll_انحراف);
    dict_جوڑو_نمبر(d, "total", tpl_کل_صفیں);
    dict_جوڑو_نمبر(d, "max_offset", max_انحراف);
}

/*
 * "tabpanel_scroll()" function
 */
    خالی
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحیح		absolute = 0;
    bool	بدلا;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_FALSE;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    n = tv_get_نمبر_chk(&argvars[0], تہی);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == تہی)
	{
	    emsg(_(e_dictionary_required));
	    واپس;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    اگر (n > زیادہ_سے_زیادہ_عدد)
	n = زیادہ_سے_زیادہ_عدد;
    ورنہ اگر (n < -زیادہ_سے_زیادہ_عدد)
	n = -زیادہ_سے_زیادہ_عدد;

    اگر (absolute)
	بدلا = tabpanel_سیٹ_انحراف((صحیح)n);
    ورنہ
	بدلا = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحیح)(n >= 0 ? n : -n));

    rettv->vval.v_نمبر = بدلا ? VVAL_TRUE : VVAL_FALSE;
}

#اگر_ختم // FEAT_TABPANEL
