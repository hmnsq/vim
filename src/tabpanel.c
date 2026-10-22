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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_TABPANEL)

ਸਥਿਰ ਖਾਲੀ do_by_tplmode(ਪੂਰਨ tplmode, ਪੂਰਨ ਖਾਨਾ_ਸ਼ੁਰੂ, ਪੂਰਨ ਖਾਨਾ_ਸਿਰਾ,
	ਪੂਰਨ *pcurtab_ਪੰਕਤੀ, ਪੂਰਨ *ptabpagenr);
ਸਥਿਰ ਖਾਲੀ tabpanel_free_click_regions(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ_ਸ਼ੁਰੂ, ਪੂਰਨ ਖਾਨਾ_ਸਿਰਾ, ਪੂਰਨ tabnr);
ਸਥਿਰ ਖਾਲੀ draw_tabpanel_scrollbar(ਪੂਰਨ screen_ਖਾਨਾ);

// set pcurtab_row. don't redraw tabpanel.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPLMODE_GET_CURTAB_ਪੰਕਤੀ	0
// set ptabpagenr. don't redraw tabpanel.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPLMODE_REDRAW		2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPL_FILLCHAR		' '

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERT_LEN		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCROLL_LEN		1

// tpl_align's values
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_ਖੱਬਾ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_ਸੱਜਾ		1

ਸਥਿਰ char_u *opt_name = (char_u *)"tabpanel";
ਸਥਿਰ ਪੂਰਨ opt_scope = OPT_LOCAL;
ਸਥਿਰ ਪੂਰਨ tpl_align = ALIGN_ਖੱਬਾ;
ਸਥਿਰ ਪੂਰਨ tpl_columns = 20;
ਸਥਿਰ bool tpl_is_vert = false;
ਸਥਿਰ bool tpl_scrollbar = false;
ਸਥਿਰ ਪੂਰਨ tpl_scroll_ਵਿੱਥ = 0;
ਸਥਿਰ ਪੂਰਨ tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ = 0;
ਸਥਿਰ ਪੂਰਨ tpl_scrollbar_ਖਾਨਾ = -1;  // screen column of scrollbar, -1 if none
ਸਥਿਰ tabpage_T *tpl_ਆਖਰੀ_curtab = ਨਿਰਾ;   // last curtab seen by draw_tabpanel

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ਪੂਰਨ	maxrow;
    ਪੂਰਨ	offsetrow;
    ਪੂਰਨ	*prow;
    ਪੂਰਨ	*pcol;
    ਪੂਰਨ	attr;
    ਪੂਰਨ	ਖਾਨਾ_ਸ਼ੁਰੂ;
    ਪੂਰਨ	ਖਾਨਾ_ਸਿਰਾ;
} tabpanel_T;

    ਪੂਰਨ
tabpanelopt_ਬਦਲਿਆ(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		new_align = ALIGN_ਖੱਬਾ;
    ਲੰਮਾ	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ਜੇ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ਖੱਬਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ਸੱਜਾ;
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ਜੇ (new_columns < 0 || new_columns > 1000)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_ਆਖਰੀ_curtab = ਨਿਰਾ;

    shell_new_columns();
    ਵਾਪਸ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    ਖਾਲੀ
tabpanel_forget_tabpage(ਅਚਲ tabpage_T *tp)
{
    ਜੇ (tpl_ਆਖਰੀ_curtab == tp)
	tpl_ਆਖਰੀ_curtab = ਨਿਰਾ;
}

/*
 * Return the width of tabpanel.
 */
    ਪੂਰਨ
tabpanel_ਚੌੜਾਈ(ਖਾਲੀ)
{
    ਚੋਣ (p_stpl)
    {
	ਹਾਲਤ 0:
	    ਵਾਪਸ 0;
	ਹਾਲਤ 1:
	    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
		ਵਾਪਸ 0;
    }
    ਜੇ (Columns < tpl_columns)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ਪੂਰਨ
tabpanel_leftcol(ਖਾਲੀ)
{
    ਵਾਪਸ tpl_align == ALIGN_ਸੱਜਾ ? 0 : tabpanel_ਚੌੜਾਈ();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ਸਥਿਰ ਖਾਲੀ
tabpanel_free_click_regions(ਖਾਲੀ)
{
    ਪੂਰਨ n;

    ਜੇ (tabpanel_stl_click != ਨਿਰਾ)
    {
	ਲਈ (n = 0; n < tabpanel_stl_click_ਗਿਣਤੀ; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_ਗਿਣਤੀ = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ਸਥਿਰ ਖਾਲੀ
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	ਪੂਰਨ		ਪੰਕਤੀ,
	ਪੂਰਨ		ਖਾਨਾ_ਸ਼ੁਰੂ,
	ਪੂਰਨ		ਖਾਨਾ_ਸਿਰਾ,
	ਪੂਰਨ		tabnr)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		n;
    ਪੂਰਨ		ਅਧਾਰ_ਖਾਨਾ;
    ਪੂਰਨ		acc_ਚੌੜਾਈ = 0;
    ਪੂਰਨ		max_w = ਖਾਨਾ_ਸਿਰਾ - ਖਾਨਾ_ਸ਼ੁਰੂ;
    char_u	*p;
    char_u	*cur_funcname = ਨਿਰਾ;
    ਪੂਰਨ		cur_minwid = 0;
    ਪੂਰਨ		region_ਸ਼ੁਰੂ_ਖਾਨਾ;
    stl_click_region_T *new_arr;
    ਪੂਰਨ		ਹੱਦਬੰਦੀ;

    ਜੇ (clicktab == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (n = 0; clicktab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
	ਗਿਣਤੀ_2++;
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ;

    ਅਧਾਰ_ਖਾਨਾ = (tpl_align == ALIGN_ਸੱਜਾ ? topframe->fr_ਚੌੜਾਈ : 0) + ਖਾਨਾ_ਸ਼ੁਰੂ;
    region_ਸ਼ੁਰੂ_ਖਾਨਾ = ਅਧਾਰ_ਖਾਨਾ;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    ਆਕਾਰ(stl_click_region_T) * (tabpanel_stl_click_ਗਿਣਤੀ + ਗਿਣਤੀ_2 + 1));
    ਜੇ (new_arr == ਨਿਰਾ)
	ਵਾਪਸ;
    tabpanel_stl_click = new_arr;

    p = buf;
    ਲਈ (n = 0; clicktab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
    {
	acc_ਚੌੜਾਈ += vim_strnsize(p, (ਪੂਰਨ)(clicktab[n].ਸ਼ੁਰੂ - p));
	p = clicktab[n].ਸ਼ੁਰੂ;
	ਹੱਦਬੰਦੀ = acc_ਚੌੜਾਈ < max_w ? acc_ਚੌੜਾਈ : max_w;

	ਜੇ (cur_funcname != ਨਿਰਾ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ਗਿਣਤੀ];
	    r->ਪੰਕਤੀ = ਪੰਕਤੀ;
	    r->ਖਾਨਾ_ਸ਼ੁਰੂ = region_ਸ਼ੁਰੂ_ਖਾਨਾ;
	    r->ਖਾਨਾ_ਸਿਰਾ = ਅਧਾਰ_ਖਾਨਾ + ਹੱਦਬੰਦੀ;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ਗਿਣਤੀ++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_ਸ਼ੁਰੂ_ਖਾਨਾ = ਅਧਾਰ_ਖਾਨਾ + ਹੱਦਬੰਦੀ;
    }

    // Close the final region if it extends to the end.
    ਜੇ (cur_funcname != ਨਿਰਾ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ਗਿਣਤੀ];
	r->ਪੰਕਤੀ = ਪੰਕਤੀ;
	r->ਖਾਨਾ_ਸ਼ੁਰੂ = region_ਸ਼ੁਰੂ_ਖਾਨਾ;
	r->ਖਾਨਾ_ਸਿਰਾ = ਅਧਾਰ_ਖਾਨਾ + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_ਗਿਣਤੀ++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ਸਥਿਰ ਖਾਲੀ
follow_curtab_if_needed(ਪੂਰਨ curtab_ਪੰਕਤੀ)
{
    ਜੇ (ਪੰਕਤੀਆਂ <= 0 || curtab == tpl_ਆਖਰੀ_curtab)
	ਵਾਪਸ;

    ਜੇ (curtab_ਪੰਕਤੀ < tpl_scroll_ਵਿੱਥ)
	tpl_scroll_ਵਿੱਥ = curtab_ਪੰਕਤੀ;
    ਨਹੀਂਤਾਂ ਜੇ (curtab_ਪੰਕਤੀ >= tpl_scroll_ਵਿੱਥ + ਪੰਕਤੀਆਂ)
	tpl_scroll_ਵਿੱਥ = curtab_ਪੰਕਤੀ - ਪੰਕਤੀਆਂ + 1;

    ਪੂਰਨ max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ > ਪੰਕਤੀਆਂ ? tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ : 0;

    ਜੇ (tpl_scroll_ਵਿੱਥ < 0)
	tpl_scroll_ਵਿੱਥ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (tpl_scroll_ਵਿੱਥ > max_ਵਿੱਥ)
	tpl_scroll_ਵਿੱਥ = max_ਵਿੱਥ;
}

/*
 * draw the tabpanel.
 */
    ਖਾਲੀ
draw_tabpanel(ਖਾਲੀ)
{
    ਪੂਰਨ saved_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    ਪੂਰਨ saved_got_int = got_int;
    ਪੂਰਨ maxwidth = tabpanel_ਚੌੜਾਈ();
    ਪੂਰਨ vs_attr = HL_ATTR(HLF_C);
    ਪੂਰਨ curtab_ਪੰਕਤੀ = 0;
    ਪੂਰਨ vsrow = 0;
    ਪੂਰਨ is_ਸੱਜਾ = tpl_align == ALIGN_ਸੱਜਾ;

    ਜੇ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ਵਾਪਸ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ਪੂਰਨ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ਪੂਰਨ sb_screen_ਖਾਨਾ = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ਜੇ (tpl_is_vert)
    {
	ਜੇ (is_ਸੱਜਾ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ਪੰਕਤੀ, VERT_LEN,
		    maxwidth - sb_len, &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    follow_curtab_if_needed(curtab_ਪੰਕਤੀ);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    ਲਈ (vsrow = 0; vsrow < ਪੰਕਤੀਆਂ; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_ਚੌੜਾਈ, vs_attr);
	    ਜੇ (tpl_scrollbar)
		sb_screen_ਖਾਨਾ = topframe->fr_ਚੌੜਾਈ + maxwidth - SCROLL_LEN;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ਪੰਕਤੀ, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    follow_curtab_if_needed(curtab_ਪੰਕਤੀ);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    ਲਈ (vsrow = 0; vsrow < ਪੰਕਤੀਆਂ; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ਜੇ (tpl_scrollbar)
		sb_screen_ਖਾਨਾ = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (is_ਸੱਜਾ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ਪੰਕਤੀ, 0, maxwidth - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    follow_curtab_if_needed(curtab_ਪੰਕਤੀ);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    ਜੇ (tpl_scrollbar)
		sb_screen_ਖਾਨਾ = topframe->fr_ਚੌੜਾਈ + maxwidth - SCROLL_LEN;
	}
	ਨਹੀਂਤਾਂ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ਪੰਕਤੀ, 0, maxwidth - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    follow_curtab_if_needed(curtab_ਪੰਕਤੀ);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
	    ਜੇ (tpl_scrollbar)
		sb_screen_ਖਾਨਾ = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_ਖਾਨਾ = sb_screen_ਖਾਨਾ;
    ਜੇ (sb_screen_ਖਾਨਾ >= 0)
	draw_tabpanel_scrollbar(sb_screen_ਖਾਨਾ);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ਕੁੰਜੀ_Typed = saved_ਕੁੰਜੀ_Typed;

    tpl_ਆਖਰੀ_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    ਪੂਰਨ
get_tabpagenr_on_tabpanel(ਖਾਲੀ)
{
    ਪੂਰਨ		maxwidth = tabpanel_ਚੌੜਾਈ();
    ਪੂਰਨ		curtab_ਪੰਕਤੀ = 0;
    ਪੂਰਨ		tabpagenr = 0;

    ਜੇ (maxwidth == 0)
	ਵਾਪਸ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ਪੰਕਤੀ, 0, maxwidth, &curtab_ਪੰਕਤੀ, ਨਿਰਾ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_ਪੰਕਤੀ,
	    &tabpagenr);

    ਵਾਪਸ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ਸਥਿਰ ਖਾਲੀ
screen_fill_tailing_area(
	ਪੂਰਨ	tplmode,
	ਪੂਰਨ	ਪੰਕਤੀ_ਸ਼ੁਰੂ,
	ਪੂਰਨ	ਪੰਕਤੀ_ਸਿਰਾ,
	ਪੂਰਨ	ਖਾਨਾ_ਸ਼ੁਰੂ,
	ਪੂਰਨ	ਖਾਨਾ_ਸਿਰਾ,
	ਪੂਰਨ	attr)
{
    ਪੂਰਨ is_ਸੱਜਾ = tpl_align == ALIGN_ਸੱਜਾ;
    ਜੇ (tplmode == TPLMODE_REDRAW)
	screen_fill(ਪੰਕਤੀ_ਸ਼ੁਰੂ, ਪੰਕਤੀ_ਸਿਰਾ,
		(is_ਸੱਜਾ ? topframe->fr_ਚੌੜਾਈ : 0) + ਖਾਨਾ_ਸ਼ੁਰੂ,
		(is_ਸੱਜਾ ? topframe->fr_ਚੌੜਾਈ : 0) + ਖਾਨਾ_ਸਿਰਾ,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ਸਥਿਰ ਖਾਲੀ
screen_puts_len_for_tabpanel(
	ਪੂਰਨ	    tplmode,
	char_u	    *p,
	ਪੂਰਨ	    len,
	ਪੂਰਨ	    attr,
	tabpanel_T  *pargs)
{
    ਪੂਰਨ		j;
    ਪੂਰਨ		chlen;
    ਪੂਰਨ		chcells;
    char_u	buf[IOSIZE];
    char_u	*ਆਰਜ਼ੀ;

    ਲਈ (j = 0; j < len;)
    {
	ਜੇ (tplmode != TPLMODE_GET_CURTAB_ਪੰਕਤੀ
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ਰੋਕੋ;

	ਜੇ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	ਨਹੀਂਤਾਂ
	    chlen = 1;

	ਲਈ (ਪੂਰਨ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	ਆਰਜ਼ੀ = transstr(buf);
	ਜੇ (ਆਰਜ਼ੀ != ਨਿਰਾ)
	{
	    vim_strncpy(buf, ਆਰਜ਼ੀ, ਆਕਾਰ(buf) - 1);
	    vim_free(ਆਰਜ਼ੀ);
	}

	ਜੇ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	ਨਹੀਂਤਾਂ
	    chcells = 1;

	ਜੇ (pargs->ਖਾਨਾ_ਸਿਰਾ < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ਜੇ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->ਖਾਨਾ_ਸਿਰਾ, attr);
	    *pargs->pcol = pargs->ਖਾਨਾ_ਸਿਰਾ;

	    ਜੇ (pargs->ਖਾਨਾ_ਸਿਰਾ < chcells)
		ਰੋਕੋ;
	}

	ਜੇ (*pargs->pcol + chcells <= pargs->ਖਾਨਾ_ਸਿਰਾ)
	{
	    ਪੂਰਨ off = (tpl_align == ALIGN_ਸੱਜਾ)
		    ? topframe->fr_ਚੌੜਾਈ
		    : 0;
	    ਜੇ (tplmode == TPLMODE_REDRAW
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
    ਸਥਿਰ ਖਾਲੀ
draw_tabpanel_default(ਪੂਰਨ tplmode, tabpanel_T *pargs)
{
    ਪੂਰਨ		modified;
    ਪੂਰਨ		wincount;
    ਪੂਰਨ		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    ਲਈ (wincount = 0; pargs->wp != ਨਿਰਾ;
	    pargs->wp = pargs->wp->w_ਅਗਲਾ, ++wincount)
	ਜੇ (buf_Is_ਬਦਲਿਆ(pargs->wp->w_ਬਫਰ))
	    modified = TRUE;

    ਜੇ (modified || wincount > 1)
    {
	ਜੇ (wincount > 1)
	{
	    vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ਪੂਰਨ)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ਜੇ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#ਨਹੀਂਤਾਂ
		    pargs->attr,
#ਜੇ_ਅੰਤ
		    pargs);
	}
	ਜੇ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_ਬਫਰ);
    shorten_dir(NameBuff);
    len = (ਪੂਰਨ)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ਜੇ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->ਖਾਨਾ_ਸਿਰਾ, pargs->attr);
    *pargs->pcol = pargs->ਖਾਨਾ_ਸਿਰਾ;
}

/*
 * Draw tabpanel content with highlight handling.
 * Processes hltab entries and fills tailing area.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_tabpanel_with_highlight(
	ਪੂਰਨ	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ਪੂਰਨ		curattr;
    ਪੂਰਨ		n;

    curattr = pargs->attr;
    p = buf;
    ਲਈ (n = 0; hltab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ਪੂਰਨ)(hltab[n].ਸ਼ੁਰੂ - p),
		curattr, pargs);
	p = hltab[n].ਸ਼ੁਰੂ;
	ਜੇ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	ਨਹੀਂਤਾਂ ਜੇ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਨਹੀਂਤਾਂ ਜੇ (pargs->wp != ਨਿਰਾ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_ਬਫਰ)
		&& pargs->wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	ਨਹੀਂਤਾਂ ਜੇ (pargs->wp != ਨਿਰਾ && bt_terminal(pargs->wp->w_ਬਫਰ)
		&& pargs->wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (pargs->wp != ਨਿਰਾ && pargs->wp != curwin
		&& pargs->wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	ਨਹੀਂਤਾਂ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ਪੂਰਨ)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ਜੇ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->ਖਾਨਾ_ਸਿਰਾ, curattr);
    *pargs->pcol = pargs->ਖਾਨਾ_ਸਿਰਾ;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ਸਥਿਰ ਖਾਲੀ
do_by_tplmode(
	ਪੂਰਨ	tplmode,
	ਪੂਰਨ	ਖਾਨਾ_ਸ਼ੁਰੂ,
	ਪੂਰਨ	ਖਾਨਾ_ਸਿਰਾ,
	ਪੂਰਨ	*pcurtab_ਪੰਕਤੀ,
	ਪੂਰਨ	*ptabpagenr)
{
    ਪੂਰਨ		attr_tplf = HL_ATTR(HLF_TPLF);
    ਪੂਰਨ		attr_tpls = HL_ATTR(HLF_TPLS);
    ਪੂਰਨ		attr_tpl = HL_ATTR(HLF_TPL);
    ਪੂਰਨ		ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਪੰਕਤੀ = 0;
    tabpage_T	*tp = ਨਿਰਾ;
    typval_T	v;
    tabpanel_T	ਦਲੀਲਾਂ;

    ਦਲੀਲਾਂ.maxrow = ਪੰਕਤੀਆਂ;
    ਦਲੀਲਾਂ.offsetrow = 0;
    ਦਲੀਲਾਂ.ਖਾਨਾ_ਸ਼ੁਰੂ = ਖਾਨਾ_ਸ਼ੁਰੂ;
    ਦਲੀਲਾਂ.ਖਾਨਾ_ਸਿਰਾ = ਖਾਨਾ_ਸਿਰਾ;

    ਜੇ (tplmode != TPLMODE_GET_CURTAB_ਪੰਕਤੀ && ਦਲੀਲਾਂ.maxrow > 0)
	ਦਲੀਲਾਂ.offsetrow = tpl_scroll_ਵਿੱਥ;

    tp = ਪਹਿਲਾ_tabpage;

    ਲਈ (ਪੰਕਤੀ = 0; tp != ਨਿਰਾ; ਪੰਕਤੀ++)
    {
	ਜੇ (tplmode != TPLMODE_GET_CURTAB_ਪੰਕਤੀ
		&& ਦਲੀਲਾਂ.maxrow <= ਪੰਕਤੀ - ਦਲੀਲਾਂ.offsetrow)
	    ਰੋਕੋ;

	ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;

	v.v_type = VAR_ਸੰਖਿਆ;
	v.vval.v_ਸੰਖਿਆ = tabpage_ਸੂਚਕਾਂਕ(tp);
	ਸੈੱਟ_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ਜੇ (tp->tp_topframe == topframe)
	{
	    ਦਲੀਲਾਂ.attr = attr_tpls;
	    ਜੇ (tplmode == TPLMODE_GET_CURTAB_ਪੰਕਤੀ)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_ਪੰਕਤੀ = ਪੰਕਤੀ;
	}
	ਨਹੀਂਤਾਂ
	    ਦਲੀਲਾਂ.attr = attr_tpl;

	ਜੇ (tp == curtab)
	{
	    ਦਲੀਲਾਂ.cwp = curwin;
	    ਦਲੀਲਾਂ.wp = firstwin;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਦਲੀਲਾਂ.cwp = tp->tp_curwin;
	    ਦਲੀਲਾਂ.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ਜੇ (usefmt != ਨਿਰਾ && *usefmt != NUL)
	{
	    ਪੂਰਨ	ਹਾਸਲ_hl = 0;

	    ਜਦੋਂ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ਨਿਰਾ;

		ਜੇ (tplmode != TPLMODE_GET_CURTAB_ਪੰਕਤੀ
			&& ਦਲੀਲਾਂ.maxrow <= ਪੰਕਤੀ - ਦਲੀਲਾਂ.offsetrow)
		    ਰੋਕੋ;

		buf[0] = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
		(ਖਾਲੀ)build_stl_str_hl_mline_nl
#ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)build_stl_str_hl_mline
#ਜੇ_ਅੰਤ
			(ਦਲੀਲਾਂ.cwp, buf, ਆਕਾਰ(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			ਦਲੀਲਾਂ.ਖਾਨਾ_ਸਿਰਾ - ਦਲੀਲਾਂ.ਖਾਨਾ_ਸ਼ੁਰੂ, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ਨਿਰਾ,
			&ਹਾਸਲ_hl);

		ਦਲੀਲਾਂ.prow = &ਪੰਕਤੀ;
		ਦਲੀਲਾਂ.pcol = &ਖਾਨਾ;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &ਦਲੀਲਾਂ);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		ਜੇ (tplmode == TPLMODE_REDRAW && clicktab != ਨਿਰਾ)
		{
		    ਪੂਰਨ screen_ਪੰਕਤੀ = ਪੰਕਤੀ - ਦਲੀਲਾਂ.offsetrow;
		    ਪੂਰਨ m;

		    ਜੇ (screen_ਪੰਕਤੀ >= 0 && screen_ਪੰਕਤੀ < ਦਲੀਲਾਂ.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_ਪੰਕਤੀ, ਦਲੀਲਾਂ.ਖਾਨਾ_ਸ਼ੁਰੂ, ਦਲੀਲਾਂ.ਖਾਨਾ_ਸਿਰਾ,
				(ਪੂਰਨ)v.vval.v_ਸੰਖਿਆ);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    ਲਈ (m = 0; clicktab[m].ਸ਼ੁਰੂ != ਨਿਰਾ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ਜੇ (*usefmt != NUL)
		{
		    ਪੰਕਤੀ++;
		    ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਦਲੀਲਾਂ.user_defined = ਨਿਰਾ;
	    ਦਲੀਲਾਂ.prow = &ਪੰਕਤੀ;
	    ਦਲੀਲਾਂ.pcol = &ਖਾਨਾ;
	    draw_tabpanel_default(tplmode, &ਦਲੀਲਾਂ);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_ਅਗਲਾ;

	ਜੇ ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_ਪੰਕਤੀ <= (ਪੰਕਤੀ - ਦਲੀਲਾਂ.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_ਸੰਖਿਆ;
	    ਰੋਕੋ;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(ਪੰਕਤੀ - ਦਲੀਲਾਂ.offsetrow, 0), ਦਲੀਲਾਂ.maxrow,
	    ਦਲੀਲਾਂ.ਖਾਨਾ_ਸ਼ੁਰੂ, ਦਲੀਲਾਂ.ਖਾਨਾ_ਸਿਰਾ, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    ਜੇ (tplmode == TPLMODE_GET_CURTAB_ਪੰਕਤੀ)
	tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ = ਪੰਕਤੀ;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_tabpanel_scrollbar(ਪੂਰਨ screen_ਖਾਨਾ)
{
    ਪੂਰਨ attr_sb = HL_ATTR(HLF_PSB);
    ਪੂਰਨ attr_thumb = HL_ATTR(HLF_PST);
    ਪੂਰਨ thumb_ਸਿਖਰ = 0;
    ਪੂਰਨ thumb_ਉਚਾਈ = 0;

    ਜੇ (tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ > ਪੰਕਤੀਆਂ && ਪੰਕਤੀਆਂ > 0)
    {
	ਪੂਰਨ max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ;
	ਪੂਰਨ track_range;

	thumb_ਉਚਾਈ = ਪੰਕਤੀਆਂ * ਪੰਕਤੀਆਂ / tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ;
	ਜੇ (thumb_ਉਚਾਈ < 1)
	    thumb_ਉਚਾਈ = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = ਪੰਕਤੀਆਂ - thumb_ਉਚਾਈ;
	ਜੇ (track_range > 0 && max_ਵਿੱਥ > 0)
	    thumb_ਸਿਖਰ = track_range * tpl_scroll_ਵਿੱਥ / max_ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    thumb_ਸਿਖਰ = 0;
	ਜੇ (thumb_ਸਿਖਰ + thumb_ਉਚਾਈ > ਪੰਕਤੀਆਂ)
	    thumb_ਸਿਖਰ = ਪੰਕਤੀਆਂ - thumb_ਉਚਾਈ;
	ਜੇ (thumb_ਸਿਖਰ < 0)
	    thumb_ਸਿਖਰ = 0;
    }

    ਲਈ (ਪੂਰਨ r = 0; r < ਪੰਕਤੀਆਂ; r++)
    {
	bool on_thumb = thumb_ਉਚਾਈ > 0
	    && r >= thumb_ਸਿਖਰ && r < thumb_ਸਿਖਰ + thumb_ਉਚਾਈ;
	screen_putchar(TPL_FILLCHAR, r, screen_ਖਾਨਾ,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(ਖਾਲੀ)
{
    ਜੇ (tabpanel_ਚੌੜਾਈ() == 0)
	ਵਾਪਸ false;
    ਵਾਪਸ mouse_ਖਾਨਾ < firstwin->w_wincol
	|| mouse_ਖਾਨਾ >= firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(ਖਾਲੀ)
{
    ਵਾਪਸ tpl_scrollbar && tpl_scrollbar_ਖਾਨਾ >= 0
	&& mouse_ਖਾਨਾ == tpl_scrollbar_ਖਾਨਾ;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ਪੂਰਨ screen_ਪੰਕਤੀ)
{
    ਪੂਰਨ thumb_ਉਚਾਈ;
    ਪੂਰਨ max_ਵਿੱਥ;
    ਪੂਰਨ track_range;
    ਪੂਰਨ thumb_ਸਿਖਰ;
    ਪੂਰਨ new_ਵਿੱਥ;

    ਜੇ (!tpl_scrollbar || ਪੰਕਤੀਆਂ <= 0 || tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ <= ਪੰਕਤੀਆਂ)
	ਵਾਪਸ false;

    thumb_ਉਚਾਈ = ਪੰਕਤੀਆਂ * ਪੰਕਤੀਆਂ / tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ;
    ਜੇ (thumb_ਉਚਾਈ < 1)
	thumb_ਉਚਾਈ = 1;
    track_range = ਪੰਕਤੀਆਂ - thumb_ਉਚਾਈ;
    ਜੇ (track_range <= 0)
	ਵਾਪਸ true;

    max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ;
    thumb_ਸਿਖਰ = screen_ਪੰਕਤੀ - thumb_ਉਚਾਈ / 2;
    ਜੇ (thumb_ਸਿਖਰ < 0)
	thumb_ਸਿਖਰ = 0;
    ਜੇ (thumb_ਸਿਖਰ > track_range)
	thumb_ਸਿਖਰ = track_range;

    new_ਵਿੱਥ = thumb_ਸਿਖਰ * max_ਵਿੱਥ / track_range;
    ਜੇ (new_ਵਿੱਥ != tpl_scroll_ਵਿੱਥ)
    {
	tpl_scroll_ਵਿੱਥ = new_ਵਿੱਥ;
	redraw_tabpanel = TRUE;
    }
    ਵਾਪਸ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ਪੂਰਨ dir, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਪੂਰਨ max_ਵਿੱਥ;
    ਪੂਰਨ new_ਵਿੱਥ;

    ਜੇ (tabpanel_ਚੌੜਾਈ() == 0)
	ਵਾਪਸ false;

    max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ;
    ਜੇ (max_ਵਿੱਥ < 0)
	max_ਵਿੱਥ = 0;

    new_ਵਿੱਥ = tpl_scroll_ਵਿੱਥ + (dir > 0 ? ਗਿਣਤੀ_2 : -ਗਿਣਤੀ_2);
    ਜੇ (new_ਵਿੱਥ < 0)
	new_ਵਿੱਥ = 0;
    ਜੇ (new_ਵਿੱਥ > max_ਵਿੱਥ)
	new_ਵਿੱਥ = max_ਵਿੱਥ;
    ਜੇ (new_ਵਿੱਥ == tpl_scroll_ਵਿੱਥ)
	ਵਾਪਸ false;

    tpl_scroll_ਵਿੱਥ = new_ਵਿੱਥ;
    redraw_tabpanel = TRUE;
    ਵਾਪਸ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_ਸੈੱਟ_ਵਿੱਥ(ਪੂਰਨ ਵਿੱਥ)
{
    ਪੂਰਨ max_ਵਿੱਥ;

    ਜੇ (tabpanel_ਚੌੜਾਈ() == 0)
	ਵਾਪਸ false;

    max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ;
    ਜੇ (max_ਵਿੱਥ < 0)
	max_ਵਿੱਥ = 0;

    ਜੇ (ਵਿੱਥ < 0)
	ਵਿੱਥ = 0;
    ਜੇ (ਵਿੱਥ > max_ਵਿੱਥ)
	ਵਿੱਥ = max_ਵਿੱਥ;
    ਜੇ (ਵਿੱਥ == tpl_scroll_ਵਿੱਥ)
	ਵਾਪਸ false;

    tpl_scroll_ਵਿੱਥ = ਵਿੱਥ;
    redraw_tabpanel = TRUE;
    ਵਾਪਸ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    ਖਾਲੀ
f_tabpanel_getinfo(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    dict_T	*d;
    ਪੂਰਨ		max_ਵਿੱਥ;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    d = rettv->vval.v_dict;

    max_ਵਿੱਥ = tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ;
    ਜੇ (max_ਵਿੱਥ < 0)
	max_ਵਿੱਥ = 0;

    dict_ਜੋੜੋ_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ਸੱਜਾ ? "right" : "left"));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "columns", tabpanel_ਚੌੜਾਈ());
    dict_ਜੋੜੋ_bool(d, "scrollbar", tpl_scrollbar);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "offset", tpl_scroll_ਵਿੱਥ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "total", tpl_ਕੁੱਲ_ਪੰਕਤੀਆਂ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "max_offset", max_ਵਿੱਥ);
}

/*
 * "tabpanel_scroll()" function
 */
    ਖਾਲੀ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    ਪੂਰਨ		absolute = 0;
    bool	ਬਦਲਿਆ;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    n = tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਜੇ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ਨਿਰਾ)
	{
	    emsg(_(e_dictionary_required));
	    ਵਾਪਸ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ਜੇ (n > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	n = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਨਹੀਂਤਾਂ ਜੇ (n < -ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	n = -ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

    ਜੇ (absolute)
	ਬਦਲਿਆ = tabpanel_ਸੈੱਟ_ਵਿੱਥ((ਪੂਰਨ)n);
    ਨਹੀਂਤਾਂ
	ਬਦਲਿਆ = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ਪੂਰਨ)(n >= 0 ? n : -n));

    rettv->vval.v_ਸੰਖਿਆ = ਬਦਲਿਆ ? VVAL_TRUE : VVAL_FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_TABPANEL
