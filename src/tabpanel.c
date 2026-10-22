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

#ಆದರೆ defined(FEAT_TABPANEL)

ಸ್ಥಿರ ಶೂನ್ಯ do_by_tplmode(ಪೂರ್ಣಾಂಕ tplmode, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಕೊನೆ,
	ಪೂರ್ಣಾಂಕ *pcurtab_ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ *ptabpagenr);
ಸ್ಥಿರ ಶೂನ್ಯ tabpanel_free_click_regions(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಕೊನೆ, ಪೂರ್ಣಾಂಕ tabnr);
ಸ್ಥಿರ ಶೂನ್ಯ draw_tabpanel_scrollbar(ಪೂರ್ಣಾಂಕ screen_ಲಂಬಸಾಲು);

// set pcurtab_row. don't redraw tabpanel.
#ವ್ಯಾಖ್ಯಾನಿಸು TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು	0
// set ptabpagenr. don't redraw tabpanel.
#ವ್ಯಾಖ್ಯಾನಿಸು TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#ವ್ಯಾಖ್ಯಾನಿಸು TPLMODE_REDRAW		2

#ವ್ಯಾಖ್ಯಾನಿಸು TPL_FILLCHAR		' '

#ವ್ಯಾಖ್ಯಾನಿಸು VERT_LEN		1
#ವ್ಯಾಖ್ಯಾನಿಸು SCROLL_LEN		1

// tpl_align's values
#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_ಎಡ		0
#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_ಬಲ		1

ಸ್ಥಿರ char_u *opt_name = (char_u *)"tabpanel";
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ opt_scope = OPT_LOCAL;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tpl_align = ALIGN_ಎಡ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tpl_columns = 20;
ಸ್ಥಿರ bool tpl_is_vert = false;
ಸ್ಥಿರ bool tpl_scrollbar = false;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tpl_scroll_ಸ್ಥಾನಾಂತರ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tpl_scrollbar_ಲಂಬಸಾಲು = -1;  // screen column of scrollbar, -1 if none
ಸ್ಥಿರ tabpage_T *tpl_ಕೊನೆಯ_curtab = ಖಾಲಿ;   // last curtab seen by draw_tabpanel

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    ಪೂರ್ಣಾಂಕ	maxrow;
    ಪೂರ್ಣಾಂಕ	offsetrow;
    ಪೂರ್ಣಾಂಕ	*prow;
    ಪೂರ್ಣಾಂಕ	*pcol;
    ಪೂರ್ಣಾಂಕ	attr;
    ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಕೊನೆ;
} tabpanel_T;

    ಪೂರ್ಣಾಂಕ
tabpanelopt_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		new_align = ALIGN_ಎಡ;
    ದೀರ್ಘ	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    ಆದರೆ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ಎಡ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ಬಲ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    ಆದರೆ (new_columns < 0 || new_columns > 1000)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_ಕೊನೆಯ_curtab = ಖಾಲಿ;

    shell_new_columns();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    ಶೂನ್ಯ
tabpanel_forget_tabpage(ಸ್ಥಿರಾಂಕ tabpage_T *tp)
{
    ಆದರೆ (tpl_ಕೊನೆಯ_curtab == tp)
	tpl_ಕೊನೆಯ_curtab = ಖಾಲಿ;
}

/*
 * Return the width of tabpanel.
 */
    ಪೂರ್ಣಾಂಕ
tabpanel_ಅಗಲ(ಶೂನ್ಯ)
{
    ಆಯ್ಕೆ (p_stpl)
    {
	ಸ್ಥಿತಿ 0:
	    ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ 1:
	    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (Columns < tpl_columns)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    ಪೂರ್ಣಾಂಕ
tabpanel_leftcol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ tpl_align == ALIGN_ಬಲ ? 0 : tabpanel_ಅಗಲ();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabpanel_free_click_regions(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ n;

    ಆದರೆ (tabpanel_stl_click != ಖಾಲಿ)
    {
	ಗಾಗಿ (n = 0; n < tabpanel_stl_click_ಎಣಿಕೆ; n++)
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
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಕೊನೆ,
	ಪೂರ್ಣಾಂಕ		tabnr)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		ತಳ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		acc_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ		max_w = ಲಂಬಸಾಲು_ಕೊನೆ - ಲಂಬಸಾಲು_ಆರಂಭ;
    char_u	*p;
    char_u	*cur_funcname = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		cur_minwid = 0;
    ಪೂರ್ಣಾಂಕ		region_ಆರಂಭ_ಲಂಬಸಾಲು;
    stl_click_region_T *new_arr;
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ;

    ಆದರೆ (clicktab == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (n = 0; clicktab[n].ಆರಂಭ != ಖಾಲಿ; n++)
	ಎಣಿಕೆ_2++;
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ;

    ತಳ_ಲಂಬಸಾಲು = (tpl_align == ALIGN_ಬಲ ? topframe->fr_ಅಗಲ : 0) + ಲಂಬಸಾಲು_ಆರಂಭ;
    region_ಆರಂಭ_ಲಂಬಸಾಲು = ತಳ_ಲಂಬಸಾಲು;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    ಗಾತ್ರ(stl_click_region_T) * (tabpanel_stl_click_ಎಣಿಕೆ + ಎಣಿಕೆ_2 + 1));
    ಆದರೆ (new_arr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    tabpanel_stl_click = new_arr;

    p = buf;
    ಗಾಗಿ (n = 0; clicktab[n].ಆರಂಭ != ಖಾಲಿ; n++)
    {
	acc_ಅಗಲ += vim_strnsize(p, (ಪೂರ್ಣಾಂಕ)(clicktab[n].ಆರಂಭ - p));
	p = clicktab[n].ಆರಂಭ;
	ಎಲ್ಲೆ = acc_ಅಗಲ < max_w ? acc_ಅಗಲ : max_w;

	ಆದರೆ (cur_funcname != ಖಾಲಿ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ಎಣಿಕೆ];
	    r->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	    r->ಲಂಬಸಾಲು_ಆರಂಭ = region_ಆರಂಭ_ಲಂಬಸಾಲು;
	    r->ಲಂಬಸಾಲು_ಕೊನೆ = ತಳ_ಲಂಬಸಾಲು + ಎಲ್ಲೆ;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ಎಣಿಕೆ++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_ಆರಂಭ_ಲಂಬಸಾಲು = ತಳ_ಲಂಬಸಾಲು + ಎಲ್ಲೆ;
    }

    // Close the final region if it extends to the end.
    ಆದರೆ (cur_funcname != ಖಾಲಿ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ಎಣಿಕೆ];
	r->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	r->ಲಂಬಸಾಲು_ಆರಂಭ = region_ಆರಂಭ_ಲಂಬಸಾಲು;
	r->ಲಂಬಸಾಲು_ಕೊನೆ = ತಳ_ಲಂಬಸಾಲು + max_w;
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
follow_curtab_if_needed(ಪೂರ್ಣಾಂಕ curtab_ಅಡ್ಡಸಾಲು)
{
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು <= 0 || curtab == tpl_ಕೊನೆಯ_curtab)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curtab_ಅಡ್ಡಸಾಲು < tpl_scroll_ಸ್ಥಾನಾಂತರ)
	tpl_scroll_ಸ್ಥಾನಾಂತರ = curtab_ಅಡ್ಡಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curtab_ಅಡ್ಡಸಾಲು >= tpl_scroll_ಸ್ಥಾನಾಂತರ + ಅಡ್ಡಸಾಲುಗಳು)
	tpl_scroll_ಸ್ಥಾನಾಂತರ = curtab_ಅಡ್ಡಸಾಲು - ಅಡ್ಡಸಾಲುಗಳು + 1;

    ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು > ಅಡ್ಡಸಾಲುಗಳು ? tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು : 0;

    ಆದರೆ (tpl_scroll_ಸ್ಥಾನಾಂತರ < 0)
	tpl_scroll_ಸ್ಥಾನಾಂತರ = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tpl_scroll_ಸ್ಥಾನಾಂತರ > max_ಸ್ಥಾನಾಂತರ)
	tpl_scroll_ಸ್ಥಾನಾಂತರ = max_ಸ್ಥಾನಾಂತರ;
}

/*
 * draw the tabpanel.
 */
    ಶೂನ್ಯ
draw_tabpanel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ saved_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಪೂರ್ಣಾಂಕ saved_got_int = got_int;
    ಪೂರ್ಣಾಂಕ maxwidth = tabpanel_ಅಗಲ();
    ಪೂರ್ಣಾಂಕ vs_attr = HL_ATTR(HLF_C);
    ಪೂರ್ಣಾಂಕ curtab_ಅಡ್ಡಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ vsrow = 0;
    ಪೂರ್ಣಾಂಕ is_ಬಲ = tpl_align == ALIGN_ಬಲ;

    ಆದರೆ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ಹಿಂತಿರುಗಿ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    ಪೂರ್ಣಾಂಕ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    ಪೂರ್ಣಾಂಕ sb_screen_ಲಂಬಸಾಲು = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    ಆದರೆ (tpl_is_vert)
    {
	ಆದರೆ (is_ಬಲ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು, VERT_LEN,
		    maxwidth - sb_len, &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    follow_curtab_if_needed(curtab_ಅಡ್ಡಸಾಲು);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    ಗಾಗಿ (vsrow = 0; vsrow < ಅಡ್ಡಸಾಲುಗಳು; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_ಅಗಲ, vs_attr);
	    ಆದರೆ (tpl_scrollbar)
		sb_screen_ಲಂಬಸಾಲು = topframe->fr_ಅಗಲ + maxwidth - SCROLL_LEN;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    follow_curtab_if_needed(curtab_ಅಡ್ಡಸಾಲು);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    ಗಾಗಿ (vsrow = 0; vsrow < ಅಡ್ಡಸಾಲುಗಳು; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    ಆದರೆ (tpl_scrollbar)
		sb_screen_ಲಂಬಸಾಲು = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (is_ಬಲ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು, 0, maxwidth - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    follow_curtab_if_needed(curtab_ಅಡ್ಡಸಾಲು);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    ಆದರೆ (tpl_scrollbar)
		sb_screen_ಲಂಬಸಾಲು = topframe->fr_ಅಗಲ + maxwidth - SCROLL_LEN;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು, 0, maxwidth - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    follow_curtab_if_needed(curtab_ಅಡ್ಡಸಾಲು);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
	    ಆದರೆ (tpl_scrollbar)
		sb_screen_ಲಂಬಸಾಲು = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_ಲಂಬಸಾಲು = sb_screen_ಲಂಬಸಾಲು;
    ಆದರೆ (sb_screen_ಲಂಬಸಾಲು >= 0)
	draw_tabpanel_scrollbar(sb_screen_ಲಂಬಸಾಲು);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ಕೀಲಿ_Typed = saved_ಕೀಲಿ_Typed;

    tpl_ಕೊನೆಯ_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    ಪೂರ್ಣಾಂಕ
get_tabpagenr_on_tabpanel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		maxwidth = tabpanel_ಅಗಲ();
    ಪೂರ್ಣಾಂಕ		curtab_ಅಡ್ಡಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		tabpagenr = 0;

    ಆದರೆ (maxwidth == 0)
	ಹಿಂತಿರುಗಿ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು, 0, maxwidth, &curtab_ಅಡ್ಡಸಾಲು, ಖಾಲಿ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_ಅಡ್ಡಸಾಲು,
	    &tabpagenr);

    ಹಿಂತಿರುಗಿ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_fill_tailing_area(
	ಪೂರ್ಣಾಂಕ	tplmode,
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು_ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು_ಕೊನೆ,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಕೊನೆ,
	ಪೂರ್ಣಾಂಕ	attr)
{
    ಪೂರ್ಣಾಂಕ is_ಬಲ = tpl_align == ALIGN_ಬಲ;
    ಆದರೆ (tplmode == TPLMODE_REDRAW)
	screen_fill(ಅಡ್ಡಸಾಲು_ಆರಂಭ, ಅಡ್ಡಸಾಲು_ಕೊನೆ,
		(is_ಬಲ ? topframe->fr_ಅಗಲ : 0) + ಲಂಬಸಾಲು_ಆರಂಭ,
		(is_ಬಲ ? topframe->fr_ಅಗಲ : 0) + ಲಂಬಸಾಲು_ಕೊನೆ,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_puts_len_for_tabpanel(
	ಪೂರ್ಣಾಂಕ	    tplmode,
	char_u	    *p,
	ಪೂರ್ಣಾಂಕ	    len,
	ಪೂರ್ಣಾಂಕ	    attr,
	tabpanel_T  *pargs)
{
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		chlen;
    ಪೂರ್ಣಾಂಕ		chcells;
    char_u	buf[IOSIZE];
    char_u	*ತಾತ್ಕಾಲಿಕ;

    ಗಾಗಿ (j = 0; j < len;)
    {
	ಆದರೆ (tplmode != TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	ಇಲ್ಲದಿದ್ದರೆ
	    chlen = 1;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	ತಾತ್ಕಾಲಿಕ = transstr(buf);
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ != ಖಾಲಿ)
	{
	    vim_strncpy(buf, ತಾತ್ಕಾಲಿಕ, ಗಾತ್ರ(buf) - 1);
	    vim_free(ತಾತ್ಕಾಲಿಕ);
	}

	ಆದರೆ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	ಇಲ್ಲದಿದ್ದರೆ
	    chcells = 1;

	ಆದರೆ (pargs->ಲಂಬಸಾಲು_ಕೊನೆ < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    ಆದರೆ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->ಲಂಬಸಾಲು_ಕೊನೆ, attr);
	    *pargs->pcol = pargs->ಲಂಬಸಾಲು_ಕೊನೆ;

	    ಆದರೆ (pargs->ಲಂಬಸಾಲು_ಕೊನೆ < chcells)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (*pargs->pcol + chcells <= pargs->ಲಂಬಸಾಲು_ಕೊನೆ)
	{
	    ಪೂರ್ಣಾಂಕ off = (tpl_align == ALIGN_ಬಲ)
		    ? topframe->fr_ಅಗಲ
		    : 0;
	    ಆದರೆ (tplmode == TPLMODE_REDRAW
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
draw_tabpanel_default(ಪೂರ್ಣಾಂಕ tplmode, tabpanel_T *pargs)
{
    ಪೂರ್ಣಾಂಕ		modified;
    ಪೂರ್ಣಾಂಕ		wincount;
    ಪೂರ್ಣಾಂಕ		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    ಗಾಗಿ (wincount = 0; pargs->wp != ಖಾಲಿ;
	    pargs->wp = pargs->wp->w_ಮುಂದಿನ, ++wincount)
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(pargs->wp->w_ಬಫರ್))
	    modified = TRUE;

    ಆದರೆ (modified || wincount > 1)
    {
	ಆದರೆ (wincount > 1)
	{
	    vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#ಆದರೆ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#ಇಲ್ಲದಿದ್ದರೆ
		    pargs->attr,
#ಆದರೆ_ಅಂತ್ಯ
		    pargs);
	}
	ಆದರೆ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_ಬಫರ್);
    shorten_dir(NameBuff);
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    ಆದರೆ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->ಲಂಬಸಾಲು_ಕೊನೆ, pargs->attr);
    *pargs->pcol = pargs->ಲಂಬಸಾಲು_ಕೊನೆ;
}

/*
 * Draw tabpanel content with highlight handling.
 * Processes hltab entries and fills tailing area.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_tabpanel_with_highlight(
	ಪೂರ್ಣಾಂಕ	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		curattr;
    ಪೂರ್ಣಾಂಕ		n;

    curattr = pargs->attr;
    p = buf;
    ಗಾಗಿ (n = 0; hltab[n].ಆರಂಭ != ಖಾಲಿ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (ಪೂರ್ಣಾಂಕ)(hltab[n].ಆರಂಭ - p),
		curattr, pargs);
	p = hltab[n].ಆರಂಭ;
	ಆದರೆ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pargs->wp != ಖಾಲಿ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_ಬಫರ್)
		&& pargs->wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pargs->wp != ಖಾಲಿ && bt_terminal(pargs->wp->w_ಬಫರ್)
		&& pargs->wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pargs->wp != ಖಾಲಿ && pargs->wp != curwin
		&& pargs->wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	ಇಲ್ಲದಿದ್ದರೆ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (ಪೂರ್ಣಾಂಕ)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    ಆದರೆ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->ಲಂಬಸಾಲು_ಕೊನೆ, curattr);
    *pargs->pcol = pargs->ಲಂಬಸಾಲು_ಕೊನೆ;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_by_tplmode(
	ಪೂರ್ಣಾಂಕ	tplmode,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_ಕೊನೆ,
	ಪೂರ್ಣಾಂಕ	*pcurtab_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	*ptabpagenr)
{
    ಪೂರ್ಣಾಂಕ		attr_tplf = HL_ATTR(HLF_TPLF);
    ಪೂರ್ಣಾಂಕ		attr_tpls = HL_ATTR(HLF_TPLS);
    ಪೂರ್ಣಾಂಕ		attr_tpl = HL_ATTR(HLF_TPL);
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = 0;
    tabpage_T	*tp = ಖಾಲಿ;
    typval_T	v;
    tabpanel_T	ವಾದಗಳು;

    ವಾದಗಳು.maxrow = ಅಡ್ಡಸಾಲುಗಳು;
    ವಾದಗಳು.offsetrow = 0;
    ವಾದಗಳು.ಲಂಬಸಾಲು_ಆರಂಭ = ಲಂಬಸಾಲು_ಆರಂಭ;
    ವಾದಗಳು.ಲಂಬಸಾಲು_ಕೊನೆ = ಲಂಬಸಾಲು_ಕೊನೆ;

    ಆದರೆ (tplmode != TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು && ವಾದಗಳು.maxrow > 0)
	ವಾದಗಳು.offsetrow = tpl_scroll_ಸ್ಥಾನಾಂತರ;

    tp = ಮೊದಲ_tabpage;

    ಗಾಗಿ (ಅಡ್ಡಸಾಲು = 0; tp != ಖಾಲಿ; ಅಡ್ಡಸಾಲು++)
    {
	ಆದರೆ (tplmode != TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು
		&& ವಾದಗಳು.maxrow <= ಅಡ್ಡಸಾಲು - ವಾದಗಳು.offsetrow)
	    ನಿಲ್ಲಿಸು;

	ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;

	v.v_type = VAR_ಸಂಖ್ಯೆ;
	v.vval.v_ಸಂಖ್ಯೆ = tabpage_ಸೂಚ್ಯಂಕ(tp);
	ಗಣ_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	ಆದರೆ (tp->tp_topframe == topframe)
	{
	    ವಾದಗಳು.attr = attr_tpls;
	    ಆದರೆ (tplmode == TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ವಾದಗಳು.attr = attr_tpl;

	ಆದರೆ (tp == curtab)
	{
	    ವಾದಗಳು.cwp = curwin;
	    ವಾದಗಳು.wp = firstwin;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ವಾದಗಳು.cwp = tp->tp_curwin;
	    ವಾದಗಳು.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	ಆದರೆ (usefmt != ಖಾಲಿ && *usefmt != NUL)
	{
	    ಪೂರ್ಣಾಂಕ	ವಾಹಕ_hl = 0;

	    ಆಗ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ಖಾಲಿ;

		ಆದರೆ (tplmode != TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು
			&& ವಾದಗಳು.maxrow <= ಅಡ್ಡಸಾಲು - ವಾದಗಳು.offsetrow)
		    ನಿಲ್ಲಿಸು;

		buf[0] = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_STL_ಬಹುಲಕ_MULTI_NL
		(ಶೂನ್ಯ)build_stl_str_hl_mline_nl
#ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)build_stl_str_hl_mline
#ಆದರೆ_ಅಂತ್ಯ
			(ವಾದಗಳು.cwp, buf, ಗಾತ್ರ(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			ವಾದಗಳು.ಲಂಬಸಾಲು_ಕೊನೆ - ವಾದಗಳು.ಲಂಬಸಾಲು_ಆರಂಭ, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ಖಾಲಿ,
			&ವಾಹಕ_hl);

		ವಾದಗಳು.prow = &ಅಡ್ಡಸಾಲು;
		ವಾದಗಳು.pcol = &ಲಂಬಸಾಲು;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &ವಾದಗಳು);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		ಆದರೆ (tplmode == TPLMODE_REDRAW && clicktab != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ screen_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು - ವಾದಗಳು.offsetrow;
		    ಪೂರ್ಣಾಂಕ m;

		    ಆದರೆ (screen_ಅಡ್ಡಸಾಲು >= 0 && screen_ಅಡ್ಡಸಾಲು < ವಾದಗಳು.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_ಅಡ್ಡಸಾಲು, ವಾದಗಳು.ಲಂಬಸಾಲು_ಆರಂಭ, ವಾದಗಳು.ಲಂಬಸಾಲು_ಕೊನೆ,
				(ಪೂರ್ಣಾಂಕ)v.vval.v_ಸಂಖ್ಯೆ);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    ಗಾಗಿ (m = 0; clicktab[m].ಆರಂಭ != ಖಾಲಿ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		ಆದರೆ (*usefmt != NUL)
		{
		    ಅಡ್ಡಸಾಲು++;
		    ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ವಾದಗಳು.user_defined = ಖಾಲಿ;
	    ವಾದಗಳು.prow = &ಅಡ್ಡಸಾಲು;
	    ವಾದಗಳು.pcol = &ಲಂಬಸಾಲು;
	    draw_tabpanel_default(tplmode, &ವಾದಗಳು);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_ಮುಂದಿನ;

	ಆದರೆ ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_ಅಡ್ಡಸಾಲು <= (ಅಡ್ಡಸಾಲು - ವಾದಗಳು.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_ಸಂಖ್ಯೆ;
	    ನಿಲ್ಲಿಸು;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(ಅಡ್ಡಸಾಲು - ವಾದಗಳು.offsetrow, 0), ವಾದಗಳು.maxrow,
	    ವಾದಗಳು.ಲಂಬಸಾಲು_ಆರಂಭ, ವಾದಗಳು.ಲಂಬಸಾಲು_ಕೊನೆ, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    ಆದರೆ (tplmode == TPLMODE_GET_CURTAB_ಅಡ್ಡಸಾಲು)
	tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲು;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_tabpanel_scrollbar(ಪೂರ್ಣಾಂಕ screen_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ attr_sb = HL_ATTR(HLF_PSB);
    ಪೂರ್ಣಾಂಕ attr_thumb = HL_ATTR(HLF_PST);
    ಪೂರ್ಣಾಂಕ thumb_ತುದಿ = 0;
    ಪೂರ್ಣಾಂಕ thumb_ಎತ್ತರ = 0;

    ಆದರೆ (tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು > ಅಡ್ಡಸಾಲುಗಳು && ಅಡ್ಡಸಾಲುಗಳು > 0)
    {
	ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು;
	ಪೂರ್ಣಾಂಕ track_range;

	thumb_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು * ಅಡ್ಡಸಾಲುಗಳು / tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು;
	ಆದರೆ (thumb_ಎತ್ತರ < 1)
	    thumb_ಎತ್ತರ = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = ಅಡ್ಡಸಾಲುಗಳು - thumb_ಎತ್ತರ;
	ಆದರೆ (track_range > 0 && max_ಸ್ಥಾನಾಂತರ > 0)
	    thumb_ತುದಿ = track_range * tpl_scroll_ಸ್ಥಾನಾಂತರ / max_ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    thumb_ತುದಿ = 0;
	ಆದರೆ (thumb_ತುದಿ + thumb_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು)
	    thumb_ತುದಿ = ಅಡ್ಡಸಾಲುಗಳು - thumb_ಎತ್ತರ;
	ಆದರೆ (thumb_ತುದಿ < 0)
	    thumb_ತುದಿ = 0;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < ಅಡ್ಡಸಾಲುಗಳು; r++)
    {
	bool on_thumb = thumb_ಎತ್ತರ > 0
	    && r >= thumb_ತುದಿ && r < thumb_ತುದಿ + thumb_ಎತ್ತರ;
	screen_putchar(TPL_FILLCHAR, r, screen_ಲಂಬಸಾಲು,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(ಶೂನ್ಯ)
{
    ಆದರೆ (tabpanel_ಅಗಲ() == 0)
	ಹಿಂತಿರುಗಿ false;
    ಹಿಂತಿರುಗಿ mouse_ಲಂಬಸಾಲು < firstwin->w_wincol
	|| mouse_ಲಂಬಸಾಲು >= firstwin->w_wincol + topframe->fr_ಅಗಲ;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ tpl_scrollbar && tpl_scrollbar_ಲಂಬಸಾಲು >= 0
	&& mouse_ಲಂಬಸಾಲು == tpl_scrollbar_ಲಂಬಸಾಲು;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(ಪೂರ್ಣಾಂಕ screen_ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ thumb_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ track_range;
    ಪೂರ್ಣಾಂಕ thumb_ತುದಿ;
    ಪೂರ್ಣಾಂಕ new_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (!tpl_scrollbar || ಅಡ್ಡಸಾಲುಗಳು <= 0 || tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು <= ಅಡ್ಡಸಾಲುಗಳು)
	ಹಿಂತಿರುಗಿ false;

    thumb_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು * ಅಡ್ಡಸಾಲುಗಳು / tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (thumb_ಎತ್ತರ < 1)
	thumb_ಎತ್ತರ = 1;
    track_range = ಅಡ್ಡಸಾಲುಗಳು - thumb_ಎತ್ತರ;
    ಆದರೆ (track_range <= 0)
	ಹಿಂತಿರುಗಿ true;

    max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು;
    thumb_ತುದಿ = screen_ಅಡ್ಡಸಾಲು - thumb_ಎತ್ತರ / 2;
    ಆದರೆ (thumb_ತುದಿ < 0)
	thumb_ತುದಿ = 0;
    ಆದರೆ (thumb_ತುದಿ > track_range)
	thumb_ತುದಿ = track_range;

    new_ಸ್ಥಾನಾಂತರ = thumb_ತುದಿ * max_ಸ್ಥಾನಾಂತರ / track_range;
    ಆದರೆ (new_ಸ್ಥಾನಾಂತರ != tpl_scroll_ಸ್ಥಾನಾಂತರ)
    {
	tpl_scroll_ಸ್ಥಾನಾಂತರ = new_ಸ್ಥಾನಾಂತರ;
	redraw_tabpanel = TRUE;
    }
    ಹಿಂತಿರುಗಿ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(ಪೂರ್ಣಾಂಕ dir, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ new_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (tabpanel_ಅಗಲ() == 0)
	ಹಿಂತಿರುಗಿ false;

    max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (max_ಸ್ಥಾನಾಂತರ < 0)
	max_ಸ್ಥಾನಾಂತರ = 0;

    new_ಸ್ಥಾನಾಂತರ = tpl_scroll_ಸ್ಥಾನಾಂತರ + (dir > 0 ? ಎಣಿಕೆ_2 : -ಎಣಿಕೆ_2);
    ಆದರೆ (new_ಸ್ಥಾನಾಂತರ < 0)
	new_ಸ್ಥಾನಾಂತರ = 0;
    ಆದರೆ (new_ಸ್ಥಾನಾಂತರ > max_ಸ್ಥಾನಾಂತರ)
	new_ಸ್ಥಾನಾಂತರ = max_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (new_ಸ್ಥಾನಾಂತರ == tpl_scroll_ಸ್ಥಾನಾಂತರ)
	ಹಿಂತಿರುಗಿ false;

    tpl_scroll_ಸ್ಥಾನಾಂತರ = new_ಸ್ಥಾನಾಂತರ;
    redraw_tabpanel = TRUE;
    ಹಿಂತಿರುಗಿ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_ಗಣ_ಸ್ಥಾನಾಂತರ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (tabpanel_ಅಗಲ() == 0)
	ಹಿಂತಿರುಗಿ false;

    max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (max_ಸ್ಥಾನಾಂತರ < 0)
	max_ಸ್ಥಾನಾಂತರ = 0;

    ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0)
	ಸ್ಥಾನಾಂತರ = 0;
    ಆದರೆ (ಸ್ಥಾನಾಂತರ > max_ಸ್ಥಾನಾಂತರ)
	ಸ್ಥಾನಾಂತರ = max_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == tpl_scroll_ಸ್ಥಾನಾಂತರ)
	ಹಿಂತಿರುಗಿ false;

    tpl_scroll_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;
    redraw_tabpanel = TRUE;
    ಹಿಂತಿರುಗಿ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    ಶೂನ್ಯ
f_tabpanel_getinfo(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		max_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    d = rettv->vval.v_dict;

    max_ಸ್ಥಾನಾಂತರ = tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (max_ಸ್ಥಾನಾಂತರ < 0)
	max_ಸ್ಥಾನಾಂತರ = 0;

    dict_ಕೂಡಿಸು_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ಬಲ ? "right" : "left"));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "columns", tabpanel_ಅಗಲ());
    dict_ಕೂಡಿಸು_bool(d, "scrollbar", tpl_scrollbar);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "offset", tpl_scroll_ಸ್ಥಾನಾಂತರ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "total", tpl_ಒಟ್ಟು_ಅಡ್ಡಸಾಲುಗಳು);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "max_offset", max_ಸ್ಥಾನಾಂತರ);
}

/*
 * "tabpanel_scroll()" function
 */
    ಶೂನ್ಯ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    ಪೂರ್ಣಾಂಕ		absolute = 0;
    bool	ಬದಲಾಯಿತು;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ಖಾಲಿ)
	{
	    emsg(_(e_dictionary_required));
	    ಹಿಂತಿರುಗಿ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    ಆದರೆ (n > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	n = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < -ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	n = -ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

    ಆದರೆ (absolute)
	ಬದಲಾಯಿತು = tabpanel_ಗಣ_ಸ್ಥಾನಾಂತರ((ಪೂರ್ಣಾಂಕ)n);
    ಇಲ್ಲದಿದ್ದರೆ
	ಬದಲಾಯಿತು = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (ಪೂರ್ಣಾಂಕ)(n >= 0 ? n : -n));

    rettv->vval.v_ಸಂಖ್ಯೆ = ಬದಲಾಯಿತು ? VVAL_TRUE : VVAL_FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TABPANEL
