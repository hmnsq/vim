/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * match.c: functions for highlighting matches
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)

# define ಹುಡುಕು_HL_ಆದ್ಯತೆ 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_ಕೂಡಿಸು(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		prio,
    ಪೂರ್ಣಾಂಕ		id,
    list_T	*pos_list,
    char_u      *conceal_char ಬಳಸದ) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		hlg_id;
    regprog_T	*regprog = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		rtype = UPD_SOME_VALID;

    ಆದರೆ (*grp == NUL || (pat != ಖಾಲಿ && *pat == NUL))
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_ಗಮ್ಯ_one_1), id);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (id == -1)
    {
	// use the next available match ID
	id = wp->w_ಮುಂದಿನ_ಹೊಂದಿಕೆ_id++;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// check the given ID is not already in use
	ಗಾಗಿ (cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ; cur != ಖಾಲಿ; cur = cur->mit_ಮುಂದಿನ)
	    ಆದರೆ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_ತೆಗೆದುಕೊಂಡ_nr), id);
		ಹಿಂತಿರುಗಿ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ಆದರೆ (wp->w_ಮುಂದಿನ_ಹೊಂದಿಕೆ_id < id + 100)
	    wp->w_ಮುಂದಿನ_ಹೊಂದಿಕೆ_id = id + 100;
    }

    ಆದರೆ ((hlg_id = syn_namen2id(grp, (ಪೂರ್ಣಾಂಕ)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_ಗುಂಪು_name_str), grp);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (pat != ಖಾಲಿ && (regprog = vim_regcomp(pat, RE_MAGIC)) == ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str), pat);
	ಹಿಂತಿರುಗಿ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ಆದರೆ (m == ಖಾಲಿ)
	ಹೋಗು fail;
    ಆದರೆ (pos_list != ಖಾಲಿ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ಆದರೆ (m->mit_pos_array == ಖಾಲಿ)
	    ಹೋಗು fail;
	m->mit_pos_ಎಣಿಕೆ = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_ಆದ್ಯತೆ = prio;
    m->mit_ನಮೂನೆ = pat == ಖಾಲಿ ? ಖಾಲಿ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_ಹೊಂದಿಕೆ.regprog = regprog;
    m->mit_ಹೊಂದಿಕೆ.rmm_ic = FALSE;
    m->mit_ಹೊಂದಿಕೆ.rmm_maxcol = 0;
# ಆದರೆ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ಆದರೆ (conceal_char != ಖಾಲಿ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ಆದರೆ (pos_list != ಖಾಲಿ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	ಪೂರ್ಣಾಂಕ		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	ಗಾಗಿ (i = 0, li = pos_list->lv_ಮೊದಲ; li != ಖಾಲಿ; i++, li = li->li_ಮುಂದಿನ)
	{
	    linenr_T	lnum = 0;
	    colnr_T	ಲಂಬಸಾಲು = 0;
	    ಪೂರ್ಣಾಂಕ		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    ಪೂರ್ಣಾಂಕ		error = FALSE;

	    ಆದರೆ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ಆದರೆ (subl == ಖಾಲಿ)
		    ಹೋಗು fail;
		subli = subl->lv_ಮೊದಲ;
		ಆದರೆ (subli == ಖಾಲಿ)
		    ಹೋಗು fail;
		lnum = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
		ಆದರೆ (error == TRUE)
		    ಹೋಗು fail;
		ಆದರೆ (lnum == 0)
		{
		    --i;
		    ಮುಂದುವರಿ;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_ಮುಂದಿನ;
		ಆದರೆ (subli != ಖಾಲಿ)
		{
		    ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
		    ಆದರೆ (error == TRUE)
			ಹೋಗು fail;
		    subli = subli->li_ಮುಂದಿನ;
		    ಆದರೆ (subli != ಖಾಲಿ)
		    {
			len = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
			ಆದರೆ (error == TRUE)
			    ಹೋಗು fail;
		    }
		}
		m->mit_pos_array[i].ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		m->mit_pos_array[i].len = len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (li->li_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	    {
		ಆದರೆ (li->li_tv.vval.v_ಸಂಖ್ಯೆ == 0)
		{
		    --i;
		    ಮುಂದುವರಿ;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_ಸಂಖ್ಯೆ;
		m->mit_pos_array[i].ಲಂಬಸಾಲು = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg(_(e_list_or_ಸಂಖ್ಯೆ_required));
		ಹೋಗು fail;
	    }
	    ಆದರೆ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ಆದರೆ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ಆದರೆ (toplnum != 0)
	{
	    redraw_win_range_later(wp, toplnum, botlnum);
	    m->mit_toplnum = toplnum;
	    m->mit_botlnum = botlnum;
	    rtype = UPD_VALID;
	}
    }

    // Insert new match.  The match list is in ascending order with regard to
    // the match priorities.
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    prev = cur;
    ಆಗ (cur != ಖಾಲಿ && prio >= cur->mit_ಆದ್ಯತೆ)
    {
	prev = cur;
	cur = cur->mit_ಮುಂದಿನ;
    }
    ಆದರೆ (cur == prev)
	wp->w_ಹೊಂದಿಕೆ_ತಲೆ = m;
    ಇಲ್ಲದಿದ್ದರೆ
	prev->mit_ಮುಂದಿನ = m;
    m->mit_ಮುಂದಿನ = cur;

    redraw_win_later(wp, rtype);
    ಹಿಂತಿರುಗಿ id;

fail:
    vim_regfree(regprog);
    ಆದರೆ (m != ಖಾಲಿ)
    {
	vim_free(m->mit_ನಮೂನೆ);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_delete(win_T *wp, ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ perr)
{
    matchitem_T	*cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    matchitem_T	*prev = cur;
    ಪೂರ್ಣಾಂಕ		rtype = UPD_SOME_VALID;

    ಆದರೆ (id < 1)
    {
	ಆದರೆ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_ಗಮ್ಯ_one_2),
									   id);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಆಗ (cur != ಖಾಲಿ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_ಮುಂದಿನ;
    }
    ಆದರೆ (cur == ಖಾಲಿ)
    {
	ಆದರೆ (perr == TRUE)
	    semsg(_(e_id_not_ಸಿಕ್ಕಿತು_nr), id);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (cur == prev)
	wp->w_ಹೊಂದಿಕೆ_ತಲೆ = cur->mit_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	prev->mit_ಮುಂದಿನ = cur->mit_ಮುಂದಿನ;
    vim_regfree(cur->mit_ಹೊಂದಿಕೆ.regprog);
    vim_free(cur->mit_ನಮೂನೆ);
    ಆದರೆ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    ಶೂನ್ಯ
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ಆಗ (wp->w_ಹೊಂದಿಕೆ_ತಲೆ != ಖಾಲಿ)
    {
	m = wp->w_ಹೊಂದಿಕೆ_ತಲೆ->mit_ಮುಂದಿನ;
	vim_regfree(wp->w_ಹೊಂದಿಕೆ_ತಲೆ->mit_ಹೊಂದಿಕೆ.regprog);
	vim_free(wp->w_ಹೊಂದಿಕೆ_ತಲೆ->mit_ನಮೂನೆ);
	vim_free(wp->w_ಹೊಂದಿಕೆ_ತಲೆ->mit_pos_array);
	vim_free(wp->w_ಹೊಂದಿಕೆ_ತಲೆ);
	wp->w_ಹೊಂದಿಕೆ_ತಲೆ = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    ಸ್ಥಿರ matchitem_T *
get_ಹೊಂದಿಕೆ(win_T *wp, ಪೂರ್ಣಾಂಕ id)
{
    matchitem_T *cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;

    ಆಗ (cur != ಖಾಲಿ && cur->mit_id != id)
	cur = cur->mit_ಮುಂದಿನ;
    ಹಿಂತಿರುಗಿ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    ಶೂನ್ಯ
init_ಹುಡುಕು_hl(win_T *wp, ಹೊಂದಿಕೆ_T *ಹುಡುಕು_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //ಹೊಂದಿಕೆ
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    ಆಗ (cur != ಖಾಲಿ)
    {
	cur->mit_hl.rm = cur->mit_ಹೊಂದಿಕೆ;
	ಆದರೆ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_ಬಫರ್;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.ಮೊದಲ_lnum = 0;
	cur = cur->mit_ಮುಂದಿನ;
    }
    // Must update this every time since highlight group override can change it.
    ಹುಡುಕು_hl->attr = HL_ATTR(HLF_L);

    ಹುಡುಕು_hl->buf = wp->w_ಬಫರ್;
    ಹುಡುಕು_hl->lnum = 0;
    ಹುಡುಕು_hl->ಮೊದಲ_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಮುಂದಿನ_ಹುಡುಕು_hl_pos(
    ಹೊಂದಿಕೆ_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *ಹೊಂದಿಕೆ,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ಸಿಕ್ಕಿತು = -1;

    ಗಾಗಿ (i = ಹೊಂದಿಕೆ->mit_pos_cur; i < ಹೊಂದಿಕೆ->mit_pos_ಎಣಿಕೆ; i++)
    {
	llpos_T	*pos = &ಹೊಂದಿಕೆ->mit_pos_array[i];

	ಆದರೆ (pos->lnum == 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (pos->len == 0 && pos->ಲಂಬಸಾಲು < mincol)
	    ಮುಂದುವರಿ;
	ಆದರೆ (pos->lnum == lnum)
	{
	    ಆದರೆ (ಸಿಕ್ಕಿತು >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ಆದರೆ (pos->ಲಂಬಸಾಲು < ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು].ಲಂಬಸಾಲು)
		{
		    llpos_T	tmp = *pos;

		    *pos = ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು];
		    ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು] = tmp;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಿಕ್ಕಿತು = i;
	}
    }
    ಹೊಂದಿಕೆ->mit_pos_cur = 0;
    ಆದರೆ (ಸಿಕ್ಕಿತು >= 0)
    {
	colnr_T	ಆರಂಭ = ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು].ಲಂಬಸಾಲು == 0
				     ? 0 : ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು].ಲಂಬಸಾಲು - 1;
	colnr_T	ಕೊನೆ = ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು].ಲಂಬಸಾಲು == 0
			    ? MAXCOL : ಆರಂಭ + ಹೊಂದಿಕೆ->mit_pos_array[ಸಿಕ್ಕಿತು].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].ಲಂಬಸಾಲು = ಆರಂಭ;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].ಲಂಬಸಾಲು = ಕೊನೆ;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	ಹೊಂದಿಕೆ->mit_pos_cur = ಸಿಕ್ಕಿತು + 1;
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಮುಂದಿನ_ಹುಡುಕು_hl(
    win_T	    *win,
    ಹೊಂದಿಕೆ_T	    *ಹುಡುಕು_hl,
    ಹೊಂದಿಕೆ_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    ದೀರ್ಘ	nmatched;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ		timed_ಹೊರಹೋಗು = FALSE;

    // for :{range}s/pat only highlight inside the range
    ಆದರೆ ((lnum < ಹುಡುಕು_ಮೊದಲ_ಸಾಲು || lnum > ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು) && cur == ಖಾಲಿ)
    {
	shl->lnum = 0;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ಆದರೆ (lnum > l)
	    shl->lnum = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum < l || shl->rm.endpos[0].ಲಂಬಸಾಲು > mincol)
	    ಹಿಂತಿರುಗಿ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    ಗಾಗಿ (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ಆದರೆ (shl->lnum == 0)
	    matchcol = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(p_cpo, CPO_ಹುಡುಕು) == ಖಾಲಿ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].ಲಂಬಸಾಲು <= shl->rm.startpos[0].ಲಂಬಸಾಲು))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].ಲಂಬಸಾಲು;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ಆದರೆ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ಇಲ್ಲದಿದ್ದರೆ
		++matchcol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    matchcol = shl->rm.endpos[0].ಲಂಬಸಾಲು;

	shl->lnum = lnum;
	ಆದರೆ (shl->rm.regprog != ಖಾಲಿ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    ಪೂರ್ಣಾಂಕ regprog_is_copy = (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ
			  && shl == &cur->mit_hl
			  && cur->mit_ಹೊಂದಿಕೆ.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_ಹೊರಹೋಗು);
	    // Copy the regprog, in case it got freed and recompiled.
	    ಆದರೆ (regprog_is_copy)
		cur->mit_ಹೊಂದಿಕೆ.regprog = cur->mit_hl.rm.regprog;

	    ಆದರೆ (called_emsg > called_emsg_before || got_int || timed_ಹೊರಹೋಗು)
	    {
		// Error while handling regexp: stop using this regexp.
		ಆದರೆ (shl == ಹುಡುಕು_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    ಗಣ_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ಖಾಲಿ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur != ಖಾಲಿ)
	    nmatched = ಮುಂದಿನ_ಹುಡುಕು_hl_pos(shl, lnum, cur, matchcol);
	ಇಲ್ಲದಿದ್ದರೆ
	    nmatched = 0;
	ಆದರೆ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].ಲಂಬಸಾಲು >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].ಲಂಬಸಾಲು > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ನಿಲ್ಲಿಸು;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    ಶೂನ್ಯ
prepare_ಹುಡುಕು_hl(win_T *wp, ಹೊಂದಿಕೆ_T *ಹುಡುಕು_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    ಹೊಂದಿಕೆ_T	*shl;		// points to search_hl or a match
    ಪೂರ್ಣಾಂಕ		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    ಪೂರ್ಣಾಂಕ		pos_inprogress;	// marks that position match search is
				// in progress
    ಪೂರ್ಣಾಂಕ		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ಆಗ (cur != ಖಾಲಿ || shl_flag == FALSE)
    {
	ಆದರೆ (shl_flag == FALSE)
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    shl = &cur->mit_hl;
	ಆದರೆ (shl->rm.regprog != ಖಾಲಿ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ಆದರೆ (shl->ಮೊದಲ_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		ಗಾಗಿ (shl->ಮೊದಲ_lnum = lnum;
			   shl->ಮೊದಲ_lnum > wp->w_topline; --shl->ಮೊದಲ_lnum)
		    ಆದರೆ (hasFoldingWin(wp, shl->ಮೊದಲ_lnum - 1,
						      ಖಾಲಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ))
			ನಿಲ್ಲಿಸು;
# ಇಲ್ಲದಿದ್ದರೆ
		shl->ಮೊದಲ_lnum = wp->w_topline;
# endif
	    }
	    ಆದರೆ (cur != ಖಾಲಿ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ಆಗ (shl->ಮೊದಲ_lnum < lnum && (shl->rm.regprog != ಖಾಲಿ
					  || (cur != ಖಾಲಿ && pos_inprogress)))
	    {
		ಮುಂದಿನ_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, shl->ಮೊದಲ_lnum, (colnr_T)n,
					       shl == ಹುಡುಕು_hl ? ಖಾಲಿ : cur);
		pos_inprogress = cur == ಖಾಲಿ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ಆದರೆ (shl->lnum != 0)
		{
		    shl->ಮೊದಲ_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].ಲಂಬಸಾಲು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++shl->ಮೊದಲ_lnum;
		    n = 0;
		}
	    }
	}
	ಆದರೆ (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ)
	    cur = cur->mit_ಮುಂದಿನ;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_cur_ಹುಡುಕು_hl(win_T *wp, ಹೊಂದಿಕೆ_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ಆದರೆ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.ಲಂಬಸಾಲು >= shl->rm.startpos[0].ಲಂಬಸಾಲು)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.ಲಂಬಸಾಲು < shl->rm.endpos[0].ಲಂಬಸಾಲು))
	shl->has_cursor = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    ಪೂರ್ಣಾಂಕ
prepare_ಹುಡುಕು_hl_ಸಾಲು(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **ಸಾಲು,
	ಹೊಂದಿಕೆ_T	    *ಹುಡುಕು_hl,
	ಪೂರ್ಣಾಂಕ	    *ಹುಡುಕು_attr)
{
    matchitem_T *cur;			// points to the match list
    ಹೊಂದಿಕೆ_T	*shl;			// points to search_hl or a match
    ಪೂರ್ಣಾಂಕ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    ಪೂರ್ಣಾಂಕ		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    shl_flag = WIN_IS_POPUP(wp);
    ಆಗ (cur != ಖಾಲಿ || shl_flag == FALSE)
    {
	ಆದರೆ (shl_flag == FALSE)
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ಆದರೆ (cur != ಖಾಲಿ)
	    cur->mit_pos_cur = 0;
	ಮುಂದಿನ_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, lnum, mincol,
						shl == ಹುಡುಕು_hl ? ಖಾಲಿ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

	ಆದರೆ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ಆದರೆ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		shl->startcol = 0;
	    ಆದರೆ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ಆದರೆ (shl == ಹುಡುಕು_hl)
		check_cur_ಹುಡುಕು_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ಆದರೆ (shl->startcol == shl->endcol)
	    {
		ಆದರೆ (has_mbyte && (*ಸಾಲು)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*ಸಾಲು) + shl->endcol);
		ಇಲ್ಲದಿದ್ದರೆ
		    ++shl->endcol;
	    }
	    ಆದರೆ ((ದೀರ್ಘ)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*ಹುಡುಕು_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ಆದರೆ (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ)
	    cur = cur->mit_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ area_highlighting;
}

/*
 * For a position in a line: Check for start/end of 'hlsearch' and other
 * matches.
 * After end, check for start/end of next match.
 * When another match, have to check for start again.
 * Watch out for matching an empty string!
 * "on_last_col" is set to TRUE with non-zero search_attr and the next column
 * is endcol.
 * Return the updated search_attr.
 */
    ಪೂರ್ಣಾಂಕ
update_ಹುಡುಕು_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    ಲಂಬಸಾಲು,
	char_u	    **ಸಾಲು,
	ಹೊಂದಿಕೆ_T	    *ಹುಡುಕು_hl,
	ಪೂರ್ಣಾಂಕ	    *has_ಹೊಂದಿಕೆ_conc ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	    *ಹೊಂದಿಕೆ_conc ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	    did_ಸಾಲು_attr,
	ಪೂರ್ಣಾಂಕ	    lcs_eol_one,
	ಪೂರ್ಣಾಂಕ	    *on_ಕೊನೆಯ_ಲಂಬಸಾಲು)
{
    matchitem_T *cur;		    // points to the match list
    ಹೊಂದಿಕೆ_T	*shl;		    // points to search_hl or a match
    ಪೂರ್ಣಾಂಕ		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    ಪೂರ್ಣಾಂಕ		pos_inprogress;	    // marks that position match search is in
				    //ಪ್ರಗತಿ
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    shl_flag = WIN_IS_POPUP(wp);
    ಆಗ (cur != ಖಾಲಿ || shl_flag == FALSE)
    {
	ಆದರೆ (shl_flag == FALSE
		&& (cur == ಖಾಲಿ
			|| cur->mit_ಆದ್ಯತೆ > ಹುಡುಕು_HL_ಆದ್ಯತೆ))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    shl = &cur->mit_hl;
	ಆದರೆ (cur != ಖಾಲಿ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ಆಗ (shl->rm.regprog != ಖಾಲಿ || (cur != ಖಾಲಿ && pos_inprogress))
	{
	    ಆದರೆ (shl->startcol != MAXCOL
		    && ಲಂಬಸಾಲು >= shl->startcol
		    && ಲಂಬಸಾಲು < shl->endcol)
	    {
		ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + mb_ptr2len(*ಸಾಲು + ಲಂಬಸಾಲು);

		ಆದರೆ (shl->endcol < ಮುಂದಿನ_ಲಂಬಸಾಲು)
		    shl->endcol = ಮುಂದಿನ_ಲಂಬಸಾಲು;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ಆದರೆ (cur != ಖಾಲಿ
			&& shl != ಹುಡುಕು_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_ಹೊಂದಿಕೆ_conc = ಲಂಬಸಾಲು == shl->startcol ? 2 : 1;
		    *ಹೊಂದಿಕೆ_conc = cur->mit_conceal_char;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    *has_ಹೊಂದಿಕೆ_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ಆದರೆ (shl == ಹುಡುಕು_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ಆದರೆ (shl->attr_cur != shl->attr)
			ಹುಡುಕು_hl_has_cursor_lnum = lnum;
		}

	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು == shl->endcol)
	    {
		shl->attr_cur = 0;
		ಮುಂದಿನ_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, lnum, ಲಂಬಸಾಲು,
					       shl == ಹುಡುಕು_hl ? ಖಾಲಿ : cur);
		pos_inprogress = !(cur == ಖಾಲಿ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

		ಆದರೆ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].ಲಂಬಸಾಲು;
		    ಆದರೆ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].ಲಂಬಸಾಲು;
		    ಇಲ್ಲದಿದ್ದರೆ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ಆದರೆ (shl == ಹುಡುಕು_hl)
			check_cur_ಹುಡುಕು_hl(wp, shl);

		    ಆದರೆ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ಆದರೆ (has_mbyte)
			{
			    char_u *p = *ಸಾಲು + shl->endcol;

			    ಆದರೆ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    ಇಲ್ಲದಿದ್ದರೆ
				shl->endcol += (*mb_ptr2len)(p);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ಮುಂದುವರಿ;
		}
	    }
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ)
	    cur = cur->mit_ಮುಂದಿನ;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    shl_flag = WIN_IS_POPUP(wp);
    ಆಗ (cur != ಖಾಲಿ || shl_flag == FALSE)
    {
	ಆದರೆ (shl_flag == FALSE
		&& (cur == ಖಾಲಿ ||
			cur->mit_ಆದ್ಯತೆ > ಹುಡುಕು_HL_ಆದ್ಯತೆ))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    shl = &cur->mit_hl;
	ಆದರೆ (shl->attr_cur != 0)
	{
	    ಹುಡುಕು_attr = shl->attr_cur;
	    *on_ಕೊನೆಯ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + 1 >= shl->endcol;
	}
	ಆದರೆ (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ)
	    cur = cur->mit_ಮುಂದಿನ;
    }
    // Only highlight one character after the last column.
    ಆದರೆ (*(*ಸಾಲು + ಲಂಬಸಾಲು) == NUL && (did_ಸಾಲು_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	ಹುಡುಕು_attr = 0;
    ಹಿಂತಿರುಗಿ ಹುಡುಕು_attr;
}

    ಪೂರ್ಣಾಂಕ
get_prevcol_hl_flag(win_T *wp, ಹೊಂದಿಕೆ_T *ಹುಡುಕು_hl, ದೀರ್ಘ curcol)
{
    ದೀರ್ಘ	prevcol = curcol;
    ಪೂರ್ಣಾಂಕ		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ಆದರೆ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ಆದರೆ (popup_is_popup(wp))
	ಹಿಂತಿರುಗಿ FALSE;
# endif

    // we're not really at that column when skipping some text
    ಆದರೆ ((ದೀರ್ಘ)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ಆದರೆ (!ಹುಡುಕು_hl->is_addpos && (prevcol == (ದೀರ್ಘ)ಹುಡುಕು_hl->startcol
		|| (prevcol > (ದೀರ್ಘ)ಹುಡುಕು_hl->startcol
					      && ಹುಡುಕು_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
	ಆಗ (cur != ಖಾಲಿ)
	{
	    ಆದರೆ (!cur->mit_hl.is_addpos && (prevcol == (ದೀರ್ಘ)cur->mit_hl.startcol
			|| (prevcol > (ದೀರ್ಘ)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    cur = cur->mit_ಮುಂದಿನ;
	}
    }
    ಹಿಂತಿರುಗಿ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    ಶೂನ್ಯ
get_ಹುಡುಕು_ಹೊಂದಿಕೆ_hl(win_T *wp, ಹೊಂದಿಕೆ_T *ಹುಡುಕು_hl, ದೀರ್ಘ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ *char_attr)
{
    matchitem_T *cur;			// points to the match list
    ಹೊಂದಿಕೆ_T	*shl;			// points to search_hl or a match
    ಪೂರ್ಣಾಂಕ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;
    shl_flag = WIN_IS_POPUP(wp);
    ಆಗ (cur != ಖಾಲಿ || shl_flag == FALSE)
    {
	ಆದರೆ (shl_flag == FALSE
		&& ((cur != ಖಾಲಿ
			&& cur->mit_ಆದ್ಯತೆ > ಹುಡುಕು_HL_ಆದ್ಯತೆ)
		    || cur == ಖಾಲಿ))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    shl = &cur->mit_hl;
	ಆದರೆ (ಲಂಬಸಾಲು - 1 == (ದೀರ್ಘ)shl->startcol
		&& (shl == ಹುಡುಕು_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ಆದರೆ (shl != ಹುಡುಕು_hl && cur != ಖಾಲಿ)
	    cur = cur->mit_ಮುಂದಿನ;
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SEARCH_EXTRA

#ಆದರೆ defined(FEAT_EVAL)
# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ಆದರೆ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (dict_has_ಕೀಲಿ(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(tv->vval.v_dict, (char_u *)"window", -1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    *win = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&di->di_tv);
    ಆದರೆ (*win == ಖಾಲಿ)
    {
	emsg(_(e_invalid_ಕಿಟಕಿ_ಸಂಖ್ಯೆ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    ಶೂನ್ಯ
f_clearmatches(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    win_T   *win;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    win = get_optional_ಕಿಟಕಿ(argvars, 0);
    ಆದರೆ (win != ಖಾಲಿ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    ಶೂನ್ಯ
f_getmatches(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*win;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    win = get_optional_ಕಿಟಕಿ(argvars, 0);
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL || win == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    cur = win->w_ಹೊಂದಿಕೆ_ತಲೆ;
    ಆಗ (cur != ಖಾಲಿ)
    {
	dict = dict_alloc();
	ಆದರೆ (dict == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (cur->mit_ಹೊಂದಿಕೆ.regprog == ಖಾಲಿ)
	{
	    // match added with matchaddpos()
	    ಗಾಗಿ (i = 0; i < cur->mit_pos_ಎಣಿಕೆ; ++i)
	    {
		llpos_T	*llpos;
		ಅಕ್ಷರ	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ಆದರೆ (llpos->lnum == 0)
		    ನಿಲ್ಲಿಸು;
		l = list_alloc();
		ಆದರೆ (l == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->lnum);
		ಆದರೆ (llpos->ಲಂಬಸಾಲು > 0)
		{
		    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->ಲಂಬಸಾಲು);
		    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dict_ಕೂಡಿಸು_string(dict, "pattern", cur->mit_ನಮೂನೆ);
	}
	dict_ಕೂಡಿಸು_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "priority", (ದೀರ್ಘ)cur->mit_ಆದ್ಯತೆ);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", (ದೀರ್ಘ)cur->mit_id);
#  ಆದರೆ defined(FEAT_CONCEAL)
	ಆದರೆ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    ಪೂರ್ಣಾಂಕ	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_ಕೂಡಿಸು_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ಆದರೆ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_ಮುಂದಿನ;
    }
# endif
}

/*
 * "setmatches()" function
 */
    ಶೂನ್ಯ
f_setmatches(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ಖಾಲಿ;
    win_T	*win;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    win = get_optional_ಕಿಟಕಿ(argvars, 1);
    ಆದರೆ (win == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((l = argvars[0].vval.v_list) != ಖಾಲಿ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_ಮೊದಲ;
	ಆಗ (li != ಖಾಲಿ)
	{
	    ಆದರೆ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ಖಾಲಿ)
	    {
		emsg(_(e_invalid_ವಾದ));
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (!(dict_has_ಕೀಲಿ(d, "group")
			&& (dict_has_ಕೀಲಿ(d, "pattern")
			    || dict_has_ಕೀಲಿ(d, "pos1"))
			&& dict_has_ಕೀಲಿ(d, "priority")
			&& dict_has_ಕೀಲಿ(d, "id")))
	    {
		emsg(_(e_invalid_ವಾದ));
		ಹಿಂತಿರುಗಿ;
	    }
	    li = li->li_ಮುಂದಿನ;
	}

	clear_matches(win);
	li = l->lv_ಮೊದಲ;
	ಆಗ (li != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ		i = 0;
	    ಅಕ್ಷರ	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*ಗುಂಪು;
	    ಪೂರ್ಣಾಂಕ		ಆದ್ಯತೆ;
	    ಪೂರ್ಣಾಂಕ		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ಆದರೆ (!dict_has_ಕೀಲಿ(d, "pattern"))
	    {
		ಆದರೆ (s == ಖಾಲಿ)
		{
		    s = list_alloc();
		    ಆದರೆ (s == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		ಗಾಗಿ (i = 1; i < 9; i++)
		{
		    sprintf((ಅಕ್ಷರ *)buf, (ಅಕ್ಷರ *)"pos%d", i);
		    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)buf, -1)) != ಖಾಲಿ)
		    {
			ಆದರೆ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ಹಿಂತಿರುಗಿ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ನಿಲ್ಲಿಸು;
		}
	    }

	    ಗುಂಪು = dict_get_string(d, "group", TRUE);
	    ಆದ್ಯತೆ = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "priority");
	    id = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "id");
	    conceal = dict_has_ಕೀಲಿ(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ಖಾಲಿ;
	    ಆದರೆ (i == 0)
	    {
		ಹೊಂದಿಕೆ_ಕೂಡಿಸು(win, ಗುಂಪು,
		    dict_get_string(d, "pattern", FALSE),
		    ಆದ್ಯತೆ, id, ಖಾಲಿ, conceal);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಹೊಂದಿಕೆ_ಕೂಡಿಸು(win, ಗುಂಪು, ಖಾಲಿ, ಆದ್ಯತೆ, id, s, conceal);
		list_unref(s);
		s = ಖಾಲಿ;
	    }
	    vim_free(ಗುಂಪು);
	    vim_free(conceal);

	    li = li->li_ಮುಂದಿನ;
	}
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    ಶೂನ್ಯ
f_matchadd(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//ಗುಂಪು
    char_u	*pat;		//ನಮೂನೆ
    ಪೂರ್ಣಾಂಕ		prio = 10;	// default priority
    ಪೂರ್ಣಾಂಕ		id = -1;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    char_u	*conceal_char = ಖಾಲಿ;
    win_T	*win = curwin;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//ಗುಂಪು
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//ನಮೂನೆ
    ಆದರೆ (grp == ಖಾಲಿ || pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	    ಆದರೆ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (error == TRUE)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_ಹೊಂದಿಕೆ_nr), id);
	ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = ಹೊಂದಿಕೆ_ಕೂಡಿಸು(win, grp, pat, prio, id, ಖಾಲಿ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    ಶೂನ್ಯ
f_matchaddpos(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*ಗುಂಪು;
    ಪೂರ್ಣಾಂಕ		prio = 10;
    ಪೂರ್ಣಾಂಕ		id = -1;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ಖಾಲಿ;
    win_T	*win = curwin;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    ಗುಂಪು = tv_get_string_buf_chk(&argvars[0], buf);
    ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_ವಾದ_ದ_str_must_be_list), "matchaddpos()");
	ಹಿಂತಿರುಗಿ;
    }
    l = argvars[1].vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ || l->lv_len == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);

	    ಆದರೆ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (error == TRUE)
	ಹಿಂತಿರುಗಿ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ಆದರೆ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_ಹೊಂದಿಕೆ_nr), id);
	ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = ಹೊಂದಿಕೆ_ಕೂಡಿಸು(win, ಗುಂಪು, ಖಾಲಿ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    ಶೂನ್ಯ
f_matcharg(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) != OK)
	ಹಿಂತಿರುಗಿ;

# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ	    id;
    matchitem_T *m;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆದರೆ (id >= 1 && id <= 3)
    {
	ಆದರೆ ((m = get_ಹೊಂದಿಕೆ(curwin, id)) != ಖಾಲಿ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_ನಮೂನೆ, -1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_append_string(rettv->vval.v_list, ಖಾಲಿ, -1);
	    list_append_string(rettv->vval.v_list, ಖಾಲಿ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    ಶೂನ್ಯ
f_matchdelete(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    win_T   *win;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    win = get_optional_ಕಿಟಕಿ(argvars, 1);
    ಆದರೆ (win == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = ಹೊಂದಿಕೆ_delete(win,
				       (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), TRUE);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    ಶೂನ್ಯ
ex_ಹೊಂದಿಕೆ(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ಖಾಲಿ;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		id;

    ಆದರೆ (eap->line2 <= 3)
	id = eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	emsg(_(e_invalid_command));
	ಹಿಂತಿರುಗಿ;
    }

    // First clear any old pattern.
    ಆದರೆ (!eap->skip)
	ಹೊಂದಿಕೆ_delete(curwin, id, FALSE);

    ಆದರೆ (ತುದಿಗಳು_excmd2(eap->cmd, eap->arg))
	ಕೊನೆ = eap->arg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ತುದಿಗಳು_excmd2(eap->arg, eap->arg + 4))))
	ಕೊನೆ = eap->arg + 4;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = skiptowhite(eap->arg);
	ಆದರೆ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ಆದರೆ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
	ಕೊನೆ = skip_regexp(p + 1, *p, TRUE);
	ಆದರೆ (!eap->skip)
	{
	    ಆದರೆ (*ಕೊನೆ != NUL && !ತುದಿಗಳು_excmd2(ಕೊನೆ, skipwhite(ಕೊನೆ + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, ಕೊನೆ);
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (*ಕೊನೆ != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_ವಾದ_str), p);
		ಹಿಂತಿರುಗಿ;
	    }

	    c = *ಕೊನೆ;
	    *ಕೊನೆ = NUL;
	    ಹೊಂದಿಕೆ_ಕೂಡಿಸು(curwin, g, p + 1, 10, id, ಖಾಲಿ, ಖಾಲಿ);
	    vim_free(g);
	    *ಕೊನೆ = c;
	}
    }
    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(ಕೊನೆ);
}
#ಆದರೆ_ಅಂತ್ಯ
