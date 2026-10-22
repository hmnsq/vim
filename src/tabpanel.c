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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_TABPANEL)

స్థిర శూన్య do_by_tplmode(పూర్ణాంకం tplmode, పూర్ణాంకం నిలువు_ప్రారంభం, పూర్ణాంకం నిలువు_చివర,
	పూర్ణాంకం *pcurtab_అడ్డువరుస, పూర్ణాంకం *ptabpagenr);
స్థిర శూన్య tabpanel_free_click_regions(శూన్య);
స్థిర శూన్య tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు_ప్రారంభం, పూర్ణాంకం నిలువు_చివర, పూర్ణాంకం tabnr);
స్థిర శూన్య draw_tabpanel_scrollbar(పూర్ణాంకం screen_నిలువు);

// set pcurtab_row. don't redraw tabpanel.
#నిర్వచించు TPLMODE_GET_CURTAB_అడ్డువరుస	0
// set ptabpagenr. don't redraw tabpanel.
#నిర్వచించు TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#నిర్వచించు TPLMODE_REDRAW		2

#నిర్వచించు TPL_FILLCHAR		' '

#నిర్వచించు VERT_LEN		1
#నిర్వచించు SCROLL_LEN		1

// tpl_align's values
#నిర్వచించు ALIGN_ఎడమ		0
#నిర్వచించు ALIGN_కుడి		1

స్థిర char_u *opt_name = (char_u *)"tabpanel";
స్థిర పూర్ణాంకం opt_scope = OPT_LOCAL;
స్థిర పూర్ణాంకం tpl_align = ALIGN_ఎడమ;
స్థిర పూర్ణాంకం tpl_columns = 20;
స్థిర bool tpl_is_vert = false;
స్థిర bool tpl_scrollbar = false;
స్థిర పూర్ణాంకం tpl_scroll_స్థానభ్రంశం = 0;
స్థిర పూర్ణాంకం tpl_మొత్తం_అడ్డువరుసలు = 0;
స్థిర పూర్ణాంకం tpl_scrollbar_నిలువు = -1;  // screen column of scrollbar, -1 if none
స్థిర tabpage_T *tpl_చివరి_curtab = ఖాళీ;   // last curtab seen by draw_tabpanel

రకనిర్వచనం నిర్మాణం {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    పూర్ణాంకం	maxrow;
    పూర్ణాంకం	offsetrow;
    పూర్ణాంకం	*prow;
    పూర్ణాంకం	*pcol;
    పూర్ణాంకం	attr;
    పూర్ణాంకం	నిలువు_ప్రారంభం;
    పూర్ణాంకం	నిలువు_చివర;
} tabpanel_T;

    పూర్ణాంకం
tabpanelopt_మారింది(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		new_align = ALIGN_ఎడమ;
    దీర్ఘ	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    అయితే (*p != NUL)
    {
	ఒకవేళ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ఒకవేళ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ఎడమ;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_కుడి;
	    }
	    లేకపోతే
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ఒకవేళ (new_columns < 0 || new_columns > 1000)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_చివరి_curtab = ఖాళీ;

    shell_new_columns();
    తిరిగి OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    శూన్య
tabpanel_forget_tabpage(స్థిరరాశి tabpage_T *tp)
{
    ఒకవేళ (tpl_చివరి_curtab == tp)
	tpl_చివరి_curtab = ఖాళీ;
}

/*
 * Return the width of tabpanel.
 */
    పూర్ణాంకం
tabpanel_వెడల్పు(శూన్య)
{
    ఎంపిక (p_stpl)
    {
	స్థితి 0:
	    తిరిగి 0;
	స్థితి 1:
	    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
		తిరిగి 0;
    }
    ఒకవేళ (Columns < tpl_columns)
	తిరిగి 0;
    లేకపోతే
	తిరిగి tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    పూర్ణాంకం
tabpanel_leftcol(శూన్య)
{
    తిరిగి tpl_align == ALIGN_కుడి ? 0 : tabpanel_వెడల్పు();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    స్థిర శూన్య
tabpanel_free_click_regions(శూన్య)
{
    పూర్ణాంకం n;

    ఒకవేళ (tabpanel_stl_click != ఖాళీ)
    {
	కొరకు (n = 0; n < tabpanel_stl_click_లెక్క; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_లెక్క = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    స్థిర శూన్య
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	పూర్ణాంకం		అడ్డువరుస,
	పూర్ణాంకం		నిలువు_ప్రారంభం,
	పూర్ణాంకం		నిలువు_చివర,
	పూర్ణాంకం		tabnr)
{
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		n;
    పూర్ణాంకం		పునాది_నిలువు;
    పూర్ణాంకం		acc_వెడల్పు = 0;
    పూర్ణాంకం		max_w = నిలువు_చివర - నిలువు_ప్రారంభం;
    char_u	*p;
    char_u	*cur_funcname = ఖాళీ;
    పూర్ణాంకం		cur_minwid = 0;
    పూర్ణాంకం		region_ప్రారంభం_నిలువు;
    stl_click_region_T *new_arr;
    పూర్ణాంకం		హద్దు;

    ఒకవేళ (clicktab == ఖాళీ)
	తిరిగి;

    కొరకు (n = 0; clicktab[n].ప్రారంభం != ఖాళీ; n++)
	లెక్క++;
    ఒకవేళ (లెక్క == 0)
	తిరిగి;

    పునాది_నిలువు = (tpl_align == ALIGN_కుడి ? topframe->fr_వెడల్పు : 0) + నిలువు_ప్రారంభం;
    region_ప్రారంభం_నిలువు = పునాది_నిలువు;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    పరిమాణం(stl_click_region_T) * (tabpanel_stl_click_లెక్క + లెక్క + 1));
    ఒకవేళ (new_arr == ఖాళీ)
	తిరిగి;
    tabpanel_stl_click = new_arr;

    p = buf;
    కొరకు (n = 0; clicktab[n].ప్రారంభం != ఖాళీ; n++)
    {
	acc_వెడల్పు += vim_strnsize(p, (పూర్ణాంకం)(clicktab[n].ప్రారంభం - p));
	p = clicktab[n].ప్రారంభం;
	హద్దు = acc_వెడల్పు < max_w ? acc_వెడల్పు : max_w;

	ఒకవేళ (cur_funcname != ఖాళీ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_లెక్క];
	    r->అడ్డువరుస = అడ్డువరుస;
	    r->నిలువు_ప్రారంభం = region_ప్రారంభం_నిలువు;
	    r->నిలువు_చివర = పునాది_నిలువు + హద్దు;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_లెక్క++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_ప్రారంభం_నిలువు = పునాది_నిలువు + హద్దు;
    }

    // Close the final region if it extends to the end.
    ఒకవేళ (cur_funcname != ఖాళీ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_లెక్క];
	r->అడ్డువరుస = అడ్డువరుస;
	r->నిలువు_ప్రారంభం = region_ప్రారంభం_నిలువు;
	r->నిలువు_చివర = పునాది_నిలువు + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_లెక్క++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    స్థిర శూన్య
follow_curtab_if_needed(పూర్ణాంకం curtab_అడ్డువరుస)
{
    ఒకవేళ (అడ్డువరుసలు <= 0 || curtab == tpl_చివరి_curtab)
	తిరిగి;

    ఒకవేళ (curtab_అడ్డువరుస < tpl_scroll_స్థానభ్రంశం)
	tpl_scroll_స్థానభ్రంశం = curtab_అడ్డువరుస;
    లేకపోతే ఒకవేళ (curtab_అడ్డువరుస >= tpl_scroll_స్థానభ్రంశం + అడ్డువరుసలు)
	tpl_scroll_స్థానభ్రంశం = curtab_అడ్డువరుస - అడ్డువరుసలు + 1;

    పూర్ణాంకం max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు > అడ్డువరుసలు ? tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు : 0;

    ఒకవేళ (tpl_scroll_స్థానభ్రంశం < 0)
	tpl_scroll_స్థానభ్రంశం = 0;
    లేకపోతే ఒకవేళ (tpl_scroll_స్థానభ్రంశం > max_స్థానభ్రంశం)
	tpl_scroll_స్థానభ్రంశం = max_స్థానభ్రంశం;
}

/*
 * draw the tabpanel.
 */
    శూన్య
draw_tabpanel(శూన్య)
{
    పూర్ణాంకం saved_కీలకం_Typed = కీలకం_Typed;
    పూర్ణాంకం saved_got_int = got_int;
    పూర్ణాంకం maxwidth = tabpanel_వెడల్పు();
    పూర్ణాంకం vs_attr = HL_ATTR(HLF_C);
    పూర్ణాంకం curtab_అడ్డువరుస = 0;
    పూర్ణాంకం vsrow = 0;
    పూర్ణాంకం is_కుడి = tpl_align == ALIGN_కుడి;

    ఒకవేళ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	తిరిగి;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    పూర్ణాంకం sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    పూర్ణాంకం sb_screen_నిలువు = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ఒకవేళ (tpl_is_vert)
    {
	ఒకవేళ (is_కుడి)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_అడ్డువరుస, VERT_LEN,
		    maxwidth - sb_len, &curtab_అడ్డువరుస, ఖాళీ);
	    follow_curtab_if_needed(curtab_అడ్డువరుస);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    కొరకు (vsrow = 0; vsrow < అడ్డువరుసలు; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_వెడల్పు, vs_attr);
	    ఒకవేళ (tpl_scrollbar)
		sb_screen_నిలువు = topframe->fr_వెడల్పు + maxwidth - SCROLL_LEN;
	}
	లేకపోతే
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_అడ్డువరుస, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_అడ్డువరుస, ఖాళీ);
	    follow_curtab_if_needed(curtab_అడ్డువరుస);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    కొరకు (vsrow = 0; vsrow < అడ్డువరుసలు; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ఒకవేళ (tpl_scrollbar)
		sb_screen_నిలువు = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (is_కుడి)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_అడ్డువరుస, 0, maxwidth - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    follow_curtab_if_needed(curtab_అడ్డువరుస);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    ఒకవేళ (tpl_scrollbar)
		sb_screen_నిలువు = topframe->fr_వెడల్పు + maxwidth - SCROLL_LEN;
	}
	లేకపోతే
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_అడ్డువరుస, 0, maxwidth - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    follow_curtab_if_needed(curtab_అడ్డువరుస);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_అడ్డువరుస, ఖాళీ);
	    ఒకవేళ (tpl_scrollbar)
		sb_screen_నిలువు = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_నిలువు = sb_screen_నిలువు;
    ఒకవేళ (sb_screen_నిలువు >= 0)
	draw_tabpanel_scrollbar(sb_screen_నిలువు);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    కీలకం_Typed = saved_కీలకం_Typed;

    tpl_చివరి_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    పూర్ణాంకం
get_tabpagenr_on_tabpanel(శూన్య)
{
    పూర్ణాంకం		maxwidth = tabpanel_వెడల్పు();
    పూర్ణాంకం		curtab_అడ్డువరుస = 0;
    పూర్ణాంకం		tabpagenr = 0;

    ఒకవేళ (maxwidth == 0)
	తిరిగి -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_అడ్డువరుస, 0, maxwidth, &curtab_అడ్డువరుస, ఖాళీ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_అడ్డువరుస,
	    &tabpagenr);

    తిరిగి tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    స్థిర శూన్య
screen_fill_tailing_area(
	పూర్ణాంకం	tplmode,
	పూర్ణాంకం	అడ్డువరుస_ప్రారంభం,
	పూర్ణాంకం	అడ్డువరుస_చివర,
	పూర్ణాంకం	నిలువు_ప్రారంభం,
	పూర్ణాంకం	నిలువు_చివర,
	పూర్ణాంకం	attr)
{
    పూర్ణాంకం is_కుడి = tpl_align == ALIGN_కుడి;
    ఒకవేళ (tplmode == TPLMODE_REDRAW)
	screen_fill(అడ్డువరుస_ప్రారంభం, అడ్డువరుస_చివర,
		(is_కుడి ? topframe->fr_వెడల్పు : 0) + నిలువు_ప్రారంభం,
		(is_కుడి ? topframe->fr_వెడల్పు : 0) + నిలువు_చివర,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    స్థిర శూన్య
screen_puts_len_for_tabpanel(
	పూర్ణాంకం	    tplmode,
	char_u	    *p,
	పూర్ణాంకం	    len,
	పూర్ణాంకం	    attr,
	tabpanel_T  *pargs)
{
    పూర్ణాంకం		j;
    పూర్ణాంకం		chlen;
    పూర్ణాంకం		chcells;
    char_u	buf[IOSIZE];
    char_u	*తాత్కాలిక;

    కొరకు (j = 0; j < len;)
    {
	ఒకవేళ (tplmode != TPLMODE_GET_CURTAB_అడ్డువరుస
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ఆపు;

	ఒకవేళ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	లేకపోతే
	    chlen = 1;

	కొరకు (పూర్ణాంకం k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	తాత్కాలిక = transstr(buf);
	ఒకవేళ (తాత్కాలిక != ఖాళీ)
	{
	    vim_strncpy(buf, తాత్కాలిక, పరిమాణం(buf) - 1);
	    vim_free(తాత్కాలిక);
	}

	ఒకవేళ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	లేకపోతే
	    chcells = 1;

	ఒకవేళ (pargs->నిలువు_చివర < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ఒకవేళ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->నిలువు_చివర, attr);
	    *pargs->pcol = pargs->నిలువు_చివర;

	    ఒకవేళ (pargs->నిలువు_చివర < chcells)
		ఆపు;
	}

	ఒకవేళ (*pargs->pcol + chcells <= pargs->నిలువు_చివర)
	{
	    పూర్ణాంకం off = (tpl_align == ALIGN_కుడి)
		    ? topframe->fr_వెడల్పు
		    : 0;
	    ఒకవేళ (tplmode == TPLMODE_REDRAW
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
    స్థిర శూన్య
draw_tabpanel_default(పూర్ణాంకం tplmode, tabpanel_T *pargs)
{
    పూర్ణాంకం		modified;
    పూర్ణాంకం		wincount;
    పూర్ణాంకం		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    కొరకు (wincount = 0; pargs->wp != ఖాళీ;
	    pargs->wp = pargs->wp->w_తదుపరి, ++wincount)
	ఒకవేళ (buf_Is_మారింది(pargs->wp->w_బఫరు))
	    modified = TRUE;

    ఒకవేళ (modified || wincount > 1)
    {
	ఒకవేళ (wincount > 1)
	{
	    vim_snprintf((అక్షరం *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (పూర్ణాంకం)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ఒకవేళ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#లేకపోతే
		    pargs->attr,
#అయితే_ముగింపు
		    pargs);
	}
	ఒకవేళ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_బఫరు);
    shorten_dir(NameBuff);
    len = (పూర్ణాంకం)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ఒకవేళ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->నిలువు_చివర, pargs->attr);
    *pargs->pcol = pargs->నిలువు_చివర;
}

/*
 * Draw tabpanel content with highlight handling.
 * Processes hltab entries and fills tailing area.
 */
    స్థిర శూన్య
draw_tabpanel_with_highlight(
	పూర్ణాంకం	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    పూర్ణాంకం		curattr;
    పూర్ణాంకం		n;

    curattr = pargs->attr;
    p = buf;
    కొరకు (n = 0; hltab[n].ప్రారంభం != ఖాళీ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (పూర్ణాంకం)(hltab[n].ప్రారంభం - p),
		curattr, pargs);
	p = hltab[n].ప్రారంభం;
	ఒకవేళ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	లేకపోతే ఒకవేళ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#నిర్వచితమైతే FEAT_TERMINAL
	లేకపోతే ఒకవేళ (pargs->wp != ఖాళీ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_బఫరు)
		&& pargs->wp->w_స్థితి_ఎత్తు != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	లేకపోతే ఒకవేళ (pargs->wp != ఖాళీ && bt_terminal(pargs->wp->w_బఫరు)
		&& pargs->wp->w_స్థితి_ఎత్తు != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (pargs->wp != ఖాళీ && pargs->wp != curwin
		&& pargs->wp->w_స్థితి_ఎత్తు != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	లేకపోతే
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (పూర్ణాంకం)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ఒకవేళ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->నిలువు_చివర, curattr);
    *pargs->pcol = pargs->నిలువు_చివర;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    స్థిర శూన్య
do_by_tplmode(
	పూర్ణాంకం	tplmode,
	పూర్ణాంకం	నిలువు_ప్రారంభం,
	పూర్ణాంకం	నిలువు_చివర,
	పూర్ణాంకం	*pcurtab_అడ్డువరుస,
	పూర్ణాంకం	*ptabpagenr)
{
    పూర్ణాంకం		attr_tplf = HL_ATTR(HLF_TPLF);
    పూర్ణాంకం		attr_tpls = HL_ATTR(HLF_TPLS);
    పూర్ణాంకం		attr_tpl = HL_ATTR(HLF_TPL);
    పూర్ణాంకం		నిలువు = నిలువు_ప్రారంభం;
    పూర్ణాంకం		అడ్డువరుస = 0;
    tabpage_T	*tp = ఖాళీ;
    typval_T	v;
    tabpanel_T	వాదనలు;

    వాదనలు.maxrow = అడ్డువరుసలు;
    వాదనలు.offsetrow = 0;
    వాదనలు.నిలువు_ప్రారంభం = నిలువు_ప్రారంభం;
    వాదనలు.నిలువు_చివర = నిలువు_చివర;

    ఒకవేళ (tplmode != TPLMODE_GET_CURTAB_అడ్డువరుస && వాదనలు.maxrow > 0)
	వాదనలు.offsetrow = tpl_scroll_స్థానభ్రంశం;

    tp = మొదటి_tabpage;

    కొరకు (అడ్డువరుస = 0; tp != ఖాళీ; అడ్డువరుస++)
    {
	ఒకవేళ (tplmode != TPLMODE_GET_CURTAB_అడ్డువరుస
		&& వాదనలు.maxrow <= అడ్డువరుస - వాదనలు.offsetrow)
	    ఆపు;

	నిలువు = నిలువు_ప్రారంభం;

	v.v_type = VAR_సంఖ్య;
	v.vval.v_సంఖ్య = tabpage_సూచీ(tp);
	సమితి_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ఒకవేళ (tp->tp_topframe == topframe)
	{
	    వాదనలు.attr = attr_tpls;
	    ఒకవేళ (tplmode == TPLMODE_GET_CURTAB_అడ్డువరుస)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_అడ్డువరుస = అడ్డువరుస;
	}
	లేకపోతే
	    వాదనలు.attr = attr_tpl;

	ఒకవేళ (tp == curtab)
	{
	    వాదనలు.cwp = curwin;
	    వాదనలు.wp = firstwin;
	}
	లేకపోతే
	{
	    వాదనలు.cwp = tp->tp_curwin;
	    వాదనలు.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ఒకవేళ (usefmt != ఖాళీ && *usefmt != NUL)
	{
	    పూర్ణాంకం	వాహకం_hl = 0;

	    అయితే (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ఖాళీ;

		ఒకవేళ (tplmode != TPLMODE_GET_CURTAB_అడ్డువరుస
			&& వాదనలు.maxrow <= అడ్డువరుస - వాదనలు.offsetrow)
		    ఆపు;

		buf[0] = NUL;
#నిర్వచితమైతే ENABLE_STL_బహుళకం_MULTI_NL
		(శూన్య)build_stl_str_hl_mline_nl
#లేకపోతే
		(శూన్య)build_stl_str_hl_mline
#అయితే_ముగింపు
			(వాదనలు.cwp, buf, పరిమాణం(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			వాదనలు.నిలువు_చివర - వాదనలు.నిలువు_ప్రారంభం, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ఖాళీ,
			&వాహకం_hl);

		వాదనలు.prow = &అడ్డువరుస;
		వాదనలు.pcol = &నిలువు;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &వాదనలు);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		ఒకవేళ (tplmode == TPLMODE_REDRAW && clicktab != ఖాళీ)
		{
		    పూర్ణాంకం screen_అడ్డువరుస = అడ్డువరుస - వాదనలు.offsetrow;
		    పూర్ణాంకం m;

		    ఒకవేళ (screen_అడ్డువరుస >= 0 && screen_అడ్డువరుస < వాదనలు.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_అడ్డువరుస, వాదనలు.నిలువు_ప్రారంభం, వాదనలు.నిలువు_చివర,
				(పూర్ణాంకం)v.vval.v_సంఖ్య);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    కొరకు (m = 0; clicktab[m].ప్రారంభం != ఖాళీ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ఒకవేళ (*usefmt != NUL)
		{
		    అడ్డువరుస++;
		    నిలువు = నిలువు_ప్రారంభం;
		}
	    }
	}
	లేకపోతే
	{
	    వాదనలు.user_defined = ఖాళీ;
	    వాదనలు.prow = &అడ్డువరుస;
	    వాదనలు.pcol = &నిలువు;
	    draw_tabpanel_default(tplmode, &వాదనలు);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_తదుపరి;

	ఒకవేళ ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_అడ్డువరుస <= (అడ్డువరుస - వాదనలు.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_సంఖ్య;
	    ఆపు;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(అడ్డువరుస - వాదనలు.offsetrow, 0), వాదనలు.maxrow,
	    వాదనలు.నిలువు_ప్రారంభం, వాదనలు.నిలువు_చివర, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    ఒకవేళ (tplmode == TPLMODE_GET_CURTAB_అడ్డువరుస)
	tpl_మొత్తం_అడ్డువరుసలు = అడ్డువరుస;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    స్థిర శూన్య
draw_tabpanel_scrollbar(పూర్ణాంకం screen_నిలువు)
{
    పూర్ణాంకం attr_sb = HL_ATTR(HLF_PSB);
    పూర్ణాంకం attr_thumb = HL_ATTR(HLF_PST);
    పూర్ణాంకం thumb_శిఖరం = 0;
    పూర్ణాంకం thumb_ఎత్తు = 0;

    ఒకవేళ (tpl_మొత్తం_అడ్డువరుసలు > అడ్డువరుసలు && అడ్డువరుసలు > 0)
    {
	పూర్ణాంకం max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు;
	పూర్ణాంకం track_range;

	thumb_ఎత్తు = అడ్డువరుసలు * అడ్డువరుసలు / tpl_మొత్తం_అడ్డువరుసలు;
	ఒకవేళ (thumb_ఎత్తు < 1)
	    thumb_ఎత్తు = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = అడ్డువరుసలు - thumb_ఎత్తు;
	ఒకవేళ (track_range > 0 && max_స్థానభ్రంశం > 0)
	    thumb_శిఖరం = track_range * tpl_scroll_స్థానభ్రంశం / max_స్థానభ్రంశం;
	లేకపోతే
	    thumb_శిఖరం = 0;
	ఒకవేళ (thumb_శిఖరం + thumb_ఎత్తు > అడ్డువరుసలు)
	    thumb_శిఖరం = అడ్డువరుసలు - thumb_ఎత్తు;
	ఒకవేళ (thumb_శిఖరం < 0)
	    thumb_శిఖరం = 0;
    }

    కొరకు (పూర్ణాంకం r = 0; r < అడ్డువరుసలు; r++)
    {
	bool on_thumb = thumb_ఎత్తు > 0
	    && r >= thumb_శిఖరం && r < thumb_శిఖరం + thumb_ఎత్తు;
	screen_putchar(TPL_FILLCHAR, r, screen_నిలువు,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(శూన్య)
{
    ఒకవేళ (tabpanel_వెడల్పు() == 0)
	తిరిగి false;
    తిరిగి mouse_నిలువు < firstwin->w_wincol
	|| mouse_నిలువు >= firstwin->w_wincol + topframe->fr_వెడల్పు;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(శూన్య)
{
    తిరిగి tpl_scrollbar && tpl_scrollbar_నిలువు >= 0
	&& mouse_నిలువు == tpl_scrollbar_నిలువు;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(పూర్ణాంకం screen_అడ్డువరుస)
{
    పూర్ణాంకం thumb_ఎత్తు;
    పూర్ణాంకం max_స్థానభ్రంశం;
    పూర్ణాంకం track_range;
    పూర్ణాంకం thumb_శిఖరం;
    పూర్ణాంకం new_స్థానభ్రంశం;

    ఒకవేళ (!tpl_scrollbar || అడ్డువరుసలు <= 0 || tpl_మొత్తం_అడ్డువరుసలు <= అడ్డువరుసలు)
	తిరిగి false;

    thumb_ఎత్తు = అడ్డువరుసలు * అడ్డువరుసలు / tpl_మొత్తం_అడ్డువరుసలు;
    ఒకవేళ (thumb_ఎత్తు < 1)
	thumb_ఎత్తు = 1;
    track_range = అడ్డువరుసలు - thumb_ఎత్తు;
    ఒకవేళ (track_range <= 0)
	తిరిగి true;

    max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు;
    thumb_శిఖరం = screen_అడ్డువరుస - thumb_ఎత్తు / 2;
    ఒకవేళ (thumb_శిఖరం < 0)
	thumb_శిఖరం = 0;
    ఒకవేళ (thumb_శిఖరం > track_range)
	thumb_శిఖరం = track_range;

    new_స్థానభ్రంశం = thumb_శిఖరం * max_స్థానభ్రంశం / track_range;
    ఒకవేళ (new_స్థానభ్రంశం != tpl_scroll_స్థానభ్రంశం)
    {
	tpl_scroll_స్థానభ్రంశం = new_స్థానభ్రంశం;
	redraw_tabpanel = TRUE;
    }
    తిరిగి true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(పూర్ణాంకం dir, పూర్ణాంకం లెక్క)
{
    పూర్ణాంకం max_స్థానభ్రంశం;
    పూర్ణాంకం new_స్థానభ్రంశం;

    ఒకవేళ (tabpanel_వెడల్పు() == 0)
	తిరిగి false;

    max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు;
    ఒకవేళ (max_స్థానభ్రంశం < 0)
	max_స్థానభ్రంశం = 0;

    new_స్థానభ్రంశం = tpl_scroll_స్థానభ్రంశం + (dir > 0 ? లెక్క : -లెక్క);
    ఒకవేళ (new_స్థానభ్రంశం < 0)
	new_స్థానభ్రంశం = 0;
    ఒకవేళ (new_స్థానభ్రంశం > max_స్థానభ్రంశం)
	new_స్థానభ్రంశం = max_స్థానభ్రంశం;
    ఒకవేళ (new_స్థానభ్రంశం == tpl_scroll_స్థానభ్రంశం)
	తిరిగి false;

    tpl_scroll_స్థానభ్రంశం = new_స్థానభ్రంశం;
    redraw_tabpanel = TRUE;
    తిరిగి true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_సమితి_స్థానభ్రంశం(పూర్ణాంకం స్థానభ్రంశం)
{
    పూర్ణాంకం max_స్థానభ్రంశం;

    ఒకవేళ (tabpanel_వెడల్పు() == 0)
	తిరిగి false;

    max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు;
    ఒకవేళ (max_స్థానభ్రంశం < 0)
	max_స్థానభ్రంశం = 0;

    ఒకవేళ (స్థానభ్రంశం < 0)
	స్థానభ్రంశం = 0;
    ఒకవేళ (స్థానభ్రంశం > max_స్థానభ్రంశం)
	స్థానభ్రంశం = max_స్థానభ్రంశం;
    ఒకవేళ (స్థానభ్రంశం == tpl_scroll_స్థానభ్రంశం)
	తిరిగి false;

    tpl_scroll_స్థానభ్రంశం = స్థానభ్రంశం;
    redraw_tabpanel = TRUE;
    తిరిగి true;
}

/*
 * "tabpanel_getinfo()" function
 */
    శూన్య
f_tabpanel_getinfo(typval_T *argvars వాడని, typval_T *rettv)
{
    dict_T	*d;
    పూర్ణాంకం		max_స్థానభ్రంశం;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    d = rettv->vval.v_dict;

    max_స్థానభ్రంశం = tpl_మొత్తం_అడ్డువరుసలు - అడ్డువరుసలు;
    ఒకవేళ (max_స్థానభ్రంశం < 0)
	max_స్థానభ్రంశం = 0;

    dict_కూడు_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_కుడి ? "right" : "left"));
    dict_కూడు_సంఖ్య(d, "columns", tabpanel_వెడల్పు());
    dict_కూడు_bool(d, "scrollbar", tpl_scrollbar);
    dict_కూడు_సంఖ్య(d, "offset", tpl_scroll_స్థానభ్రంశం);
    dict_కూడు_సంఖ్య(d, "total", tpl_మొత్తం_అడ్డువరుసలు);
    dict_కూడు_సంఖ్య(d, "max_offset", max_స్థానభ్రంశం);
}

/*
 * "tabpanel_scroll()" function
 */
    శూన్య
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    పూర్ణాంకం		absolute = 0;
    bool	మారింది;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    n = tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ఖాళీ)
	{
	    emsg(_(e_dictionary_required));
	    తిరిగి;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ఒకవేళ (n > గరిష్ఠ_పూర్ణాంకం)
	n = గరిష్ఠ_పూర్ణాంకం;
    లేకపోతే ఒకవేళ (n < -గరిష్ఠ_పూర్ణాంకం)
	n = -గరిష్ఠ_పూర్ణాంకం;

    ఒకవేళ (absolute)
	మారింది = tabpanel_సమితి_స్థానభ్రంశం((పూర్ణాంకం)n);
    లేకపోతే
	మారింది = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (పూర్ణాంకం)(n >= 0 ? n : -n));

    rettv->vval.v_సంఖ్య = మారింది ? VVAL_TRUE : VVAL_FALSE;
}

#అయితే_ముగింపు // FEAT_TABPANEL
