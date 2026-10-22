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

#include "vim.h"

#ᯀᯮᯝ defined(FEAT_SEARCH_EXTRA)

# define SEARCH_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    static puur_naa_nka
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    puur_naa_nka		prio,
    puur_naa_nka		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = NULL;
    puur_naa_nka		hlg_id;
    regprog_T	*regprog = NULL;
    puur_naa_nka		rtype = UPD_SOME_VALID;

    ᯀᯮᯝ (*grp == NUL || (pat != NULL && *pat == NUL))
	ᯀᯮᯞ -1;
    ᯀᯮᯝ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    ᯀᯮᯝᯂᯪ
    {
	// check the given ID is not already in use
	for (cur = wp->w_match_head; cur != NULL; cur = cur->mit_next)
	    ᯀᯮᯝ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		ᯀᯮᯞ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ᯀᯮᯝ (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    ᯀᯮᯝ ((hlg_id = syn_namen2id(grp, (puur_naa_nka)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (pat != NULL && (regprog = vim_regcomp(pat, RE_MAGIC)) == NULL)
    {
	semsg(_(e_invalid_argument_str), pat);
	ᯀᯮᯞ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ᯀᯮᯝ (m == NULL)
	goto fail;
    ᯀᯮᯝ (pos_list != NULL && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ᯀᯮᯝ (m->mit_pos_array == NULL)
	    goto fail;
	m->mit_pos_count = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == NULL ? NULL : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# ᯀᯮᯝ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ᯀᯮᯝ (conceal_char != NULL)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ᯀᯮᯝ (pos_list != NULL)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	puur_naa_nka		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	for (i = 0, li = pos_list->lv_first; li != NULL; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    puur_naa_nka		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    puur_naa_nka		error = FALSE;

	    ᯀᯮᯝ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ᯀᯮᯝ (subl == NULL)
		    goto fail;
		subli = subl->lv_first;
		ᯀᯮᯝ (subli == NULL)
		    goto fail;
		lnum = tv_get_number_chk(&subli->li_tv, &error);
		ᯀᯮᯝ (error == TRUE)
		    goto fail;
		ᯀᯮᯝ (lnum == 0)
		{
		    --i;
		    continue;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		ᯀᯮᯝ (subli != NULL)
		{
		    col = tv_get_number_chk(&subli->li_tv, &error);
		    ᯀᯮᯝ (error == TRUE)
			goto fail;
		    subli = subli->li_next;
		    ᯀᯮᯝ (subli != NULL)
		    {
			len = tv_get_number_chk(&subli->li_tv, &error);
			ᯀᯮᯝ (error == TRUE)
			    goto fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (li->li_tv.v_type == VAR_NUMBER)
	    {
		ᯀᯮᯝ (li->li_tv.vval.v_number == 0)
		{
		    --i;
		    continue;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_number;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		emsg(_(e_list_or_number_required));
		goto fail;
	    }
	    ᯀᯮᯝ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ᯀᯮᯝ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ᯀᯮᯝ (toplnum != 0)
	{
	    redraw_win_range_later(wp, toplnum, botlnum);
	    m->mit_toplnum = toplnum;
	    m->mit_botlnum = botlnum;
	    rtype = UPD_VALID;
	}
    }

    // Insert new match.  The match list is in ascending order with regard to
    // the match priorities.
    cur = wp->w_match_head;
    prev = cur;
    ᯀᯬᯝ (cur != NULL && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ᯀᯮᯝ (cur == prev)
	wp->w_match_head = m;
    ᯀᯮᯝᯂᯪ
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    ᯀᯮᯞ id;

fail:
    vim_regfree(regprog);
    ᯀᯮᯝ (m != NULL)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ᯀᯮᯞ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    static puur_naa_nka
match_delete(win_T *wp, puur_naa_nka id, puur_naa_nka perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    puur_naa_nka		rtype = UPD_SOME_VALID;

    ᯀᯮᯝ (id < 1)
    {
	ᯀᯮᯝ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	ᯀᯮᯞ -1;
    }
    ᯀᯬᯝ (cur != NULL && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ᯀᯮᯝ (cur == NULL)
    {
	ᯀᯮᯝ (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (cur == prev)
	wp->w_match_head = cur->mit_next;
    ᯀᯮᯝᯂᯪ
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    ᯀᯮᯝ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ᯀᯮᯞ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    void
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ᯀᯬᯝ (wp->w_match_head != NULL)
    {
	m = wp->w_match_head->mit_next;
	vim_regfree(wp->w_match_head->mit_match.regprog);
	vim_free(wp->w_match_head->mit_pattern);
	vim_free(wp->w_match_head->mit_pos_array);
	vim_free(wp->w_match_head);
	wp->w_match_head = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    static matchitem_T *
get_match(win_T *wp, puur_naa_nka id)
{
    matchitem_T *cur = wp->w_match_head;

    ᯀᯬᯝ (cur != NULL && cur->mit_id != id)
	cur = cur->mit_next;
    ᯀᯮᯞ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    void
init_search_hl(win_T *wp, match_T *search_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    ᯀᯬᯝ (cur != NULL)
    {
	cur->mit_hl.rm = cur->mit_match;
	ᯀᯮᯝ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ᯀᯮᯝᯂᯪ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    search_hl->attr = HL_ATTR(HLF_L);

    search_hl->buf = wp->w_buffer;
    search_hl->lnum = 0;
    search_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    static puur_naa_nka
next_search_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    puur_naa_nka	    i;
    puur_naa_nka	    found = -1;

    for (i = match->mit_pos_cur; i < match->mit_pos_count; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	ᯀᯮᯝ (pos->lnum == 0)
	    bha_nga;
	ᯀᯮᯝ (pos->len == 0 && pos->col < mincol)
	    continue;
	ᯀᯮᯝ (pos->lnum == lnum)
	{
	    ᯀᯮᯝ (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ᯀᯮᯝ (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    ᯀᯮᯝᯂᯪ
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    ᯀᯮᯝ (found >= 0)
    {
	colnr_T	start = match->mit_pos_array[found].col == 0
				     ? 0 : match->mit_pos_array[found].col - 1;
	colnr_T	end = match->mit_pos_array[found].col == 0
			    ? MAXCOL : start + match->mit_pos_array[found].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].col = start;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].col = end;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	match->mit_pos_cur = found + 1;
	ᯀᯮᯞ 1;
    }
    ᯀᯮᯞ 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    static void
next_search_hl(
    win_T	    *win,
    match_T	    *search_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    puur_naa_nka		called_emsg_before = called_emsg;
    puur_naa_nka		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    ᯀᯮᯝ ((lnum < search_first_line || lnum > search_last_line) && cur == NULL)
    {
	shl->lnum = 0;
	ᯀᯮᯞ;
    }

    ᯀᯮᯝ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ᯀᯮᯝ (lnum > l)
	    shl->lnum = 0;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (lnum < l || shl->rm.endpos[0].col > mincol)
	    ᯀᯮᯞ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    for (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ᯀᯮᯝ (shl->lnum == 0)
	    matchcol = 0;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (vim_strchr(p_cpo, CPO_SEARCH) == NULL
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ᯀᯮᯝ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		bha_nga;
	    }
	    ᯀᯮᯝ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ᯀᯮᯝᯂᯪ
		++matchcol;
	}
	ᯀᯮᯝᯂᯪ
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	ᯀᯮᯝ (shl->rm.regprog != NULL)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    puur_naa_nka regprog_is_copy = (shl != search_hl && cur != NULL
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    ᯀᯮᯝ (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    ᯀᯮᯝ (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		ᯀᯮᯝ (shl == search_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = NULL;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		bha_nga;
	    }
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur != NULL)
	    nmatched = next_search_hl_pos(shl, lnum, cur, matchcol);
	ᯀᯮᯝᯂᯪ
	    nmatched = 0;
	ᯀᯮᯝ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    bha_nga;
	}
	ᯀᯮᯝ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    bha_nga;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    void
prepare_search_hl(win_T *wp, match_T *search_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    puur_naa_nka		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    puur_naa_nka		pos_inprogress;	// marks that position match search is
				// in progress
    puur_naa_nka		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ᯀᯬᯝ (cur != NULL || shl_flag == FALSE)
    {
	ᯀᯮᯝ (shl_flag == FALSE)
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    shl = &cur->mit_hl;
	ᯀᯮᯝ (shl->rm.regprog != NULL
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ᯀᯮᯝ (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		for (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    ᯀᯮᯝ (hasFoldingWin(wp, shl->first_lnum - 1,
						      NULL, NULL, TRUE, NULL))
			bha_nga;
# ᯀᯮᯝᯂᯪ
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    ᯀᯮᯝ (cur != NULL)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ᯀᯬᯝ (shl->first_lnum < lnum && (shl->rm.regprog != NULL
					  || (cur != NULL && pos_inprogress)))
	    {
		next_search_hl(wp, search_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == search_hl ? NULL : cur);
		pos_inprogress = cur == NULL || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ᯀᯮᯝ (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		ᯀᯮᯝᯂᯪ
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	ᯀᯮᯝ (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    static void
check_cur_search_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ᯀᯮᯝ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    ᯀᯮᯝᯂᯪ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    puur_naa_nka
prepare_search_hl_line(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **line,
	match_T	    *search_hl,
	puur_naa_nka	    *search_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    puur_naa_nka		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    puur_naa_nka		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ᯀᯬᯝ (cur != NULL || shl_flag == FALSE)
    {
	ᯀᯮᯝ (shl_flag == FALSE)
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ᯀᯮᯝ (cur != NULL)
	    cur->mit_pos_cur = 0;
	next_search_hl(wp, search_hl, shl, lnum, mincol,
						shl == search_hl ? NULL : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	ᯀᯮᯝ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ᯀᯮᯝ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    ᯀᯮᯝᯂᯪ
		shl->startcol = 0;
	    ᯀᯮᯝ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    ᯀᯮᯝᯂᯪ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ᯀᯮᯝ (shl == search_hl)
		check_cur_search_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ᯀᯮᯝ (shl->startcol == shl->endcol)
	    {
		ᯀᯮᯝ (has_mbyte && (*line)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*line) + shl->endcol);
		ᯀᯮᯝᯂᯪ
		    ++shl->endcol;
	    }
	    ᯀᯮᯝ ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*search_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ᯀᯮᯝ (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
    ᯀᯮᯞ area_highlighting;
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
    puur_naa_nka
update_search_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **line,
	match_T	    *search_hl,
	puur_naa_nka	    *has_match_conc UNUSED,
	puur_naa_nka	    *match_conc UNUSED,
	puur_naa_nka	    did_line_attr,
	puur_naa_nka	    lcs_eol_one,
	puur_naa_nka	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    puur_naa_nka		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    puur_naa_nka		pos_inprogress;	    // marks that position match search is in
				    // progress
    puur_naa_nka		search_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ᯀᯬᯝ (cur != NULL || shl_flag == FALSE)
    {
	ᯀᯮᯝ (shl_flag == FALSE
		&& (cur == NULL
			|| cur->mit_priority > SEARCH_HL_PRIORITY))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    shl = &cur->mit_hl;
	ᯀᯮᯝ (cur != NULL)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ᯀᯬᯝ (shl->rm.regprog != NULL || (cur != NULL && pos_inprogress))
	{
	    ᯀᯮᯝ (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		puur_naa_nka next_col = col + mb_ptr2len(*line + col);

		ᯀᯮᯝ (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ᯀᯮᯝ (cur != NULL
			&& shl != search_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		ᯀᯮᯝᯂᯪ
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ᯀᯮᯝ (shl == search_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ᯀᯮᯝ (shl->attr_cur != shl->attr)
			search_hl_has_cursor_lnum = lnum;
		}

	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_search_hl(wp, search_hl, shl, lnum, col,
					       shl == search_hl ? NULL : cur);
		pos_inprogress = !(cur == NULL || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*line = ml_get_buf(wp->w_buffer, lnum, FALSE);

		ᯀᯮᯝ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    ᯀᯮᯝ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    ᯀᯮᯝᯂᯪ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ᯀᯮᯝ (shl == search_hl)
			check_cur_search_hl(wp, shl);

		    ᯀᯮᯝ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ᯀᯮᯝ (has_mbyte)
			{
			    char_u *p = *line + shl->endcol;

			    ᯀᯮᯝ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    ᯀᯮᯝᯂᯪ
				shl->endcol += (*mb_ptr2len)(p);
			}
			ᯀᯮᯝᯂᯪ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    continue;
		}
	    }
	    bha_nga;
	}
	ᯀᯮᯝ (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ᯀᯬᯝ (cur != NULL || shl_flag == FALSE)
    {
	ᯀᯮᯝ (shl_flag == FALSE
		&& (cur == NULL ||
			cur->mit_priority > SEARCH_HL_PRIORITY))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    shl = &cur->mit_hl;
	ᯀᯮᯝ (shl->attr_cur != 0)
	{
	    search_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	ᯀᯮᯝ (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    ᯀᯮᯝ (*(*line + col) == NUL && (did_line_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	search_attr = 0;
    ᯀᯮᯞ search_attr;
}

    puur_naa_nka
get_prevcol_hl_flag(win_T *wp, match_T *search_hl, long curcol)
{
    long	prevcol = curcol;
    puur_naa_nka		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ᯀᯮᯝ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ᯀᯮᯝ (popup_is_popup(wp))
	ᯀᯮᯞ FALSE;
# endif

    // we're not really at that column when skipping some text
    ᯀᯮᯝ ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ᯀᯮᯝ (!search_hl->is_addpos && (prevcol == (long)search_hl->startcol
		|| (prevcol > (long)search_hl->startcol
					      && search_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ᯀᯮᯝᯂᯪ
    {
	cur = wp->w_match_head;
	ᯀᯬᯝ (cur != NULL)
	{
	    ᯀᯮᯝ (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		bha_nga;
	    }
	    cur = cur->mit_next;
	}
    }
    ᯀᯮᯞ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    void
get_search_match_hl(win_T *wp, match_T *search_hl, long col, puur_naa_nka *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    puur_naa_nka		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ᯀᯬᯝ (cur != NULL || shl_flag == FALSE)
    {
	ᯀᯮᯝ (shl_flag == FALSE
		&& ((cur != NULL
			&& cur->mit_priority > SEARCH_HL_PRIORITY)
		    || cur == NULL))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    shl = &cur->mit_hl;
	ᯀᯮᯝ (col - 1 == (long)shl->startcol
		&& (shl == search_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ᯀᯮᯝ (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#ᯀᯮᯝ defined(FEAT_EVAL)
# ifdef FEAT_SEARCH_EXTRA
    static puur_naa_nka
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ᯀᯮᯝ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ᯀᯮᯞ FAIL;
    }

    ᯀᯮᯝ (dict_has_key(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ᯀᯮᯝ ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == NULL)
	ᯀᯮᯞ OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    ᯀᯮᯝ (*win == NULL)
    {
	emsg(_(e_invalid_window_number));
	ᯀᯮᯞ FAIL;
    }

    ᯀᯮᯞ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    void
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    win_T   *win;

    ᯀᯮᯝ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;

    win = get_optional_window(argvars, 0);
    ᯀᯮᯝ (win != NULL)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    void
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    puur_naa_nka		i;
    win_T	*win;

    ᯀᯮᯝ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;

    win = get_optional_window(argvars, 0);
    ᯀᯮᯝ (rettv_list_alloc(rettv) == FAIL || win == NULL)
	ᯀᯮᯞ;

    cur = win->w_match_head;
    ᯀᯬᯝ (cur != NULL)
    {
	dict = dict_alloc();
	ᯀᯮᯝ (dict == NULL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (cur->mit_match.regprog == NULL)
	{
	    // match added with matchaddpos()
	    for (i = 0; i < cur->mit_pos_count; ++i)
	    {
		llpos_T	*llpos;
		var_na	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ᯀᯮᯝ (llpos->lnum == 0)
		    bha_nga;
		l = list_alloc();
		ᯀᯮᯝ (l == NULL)
		    bha_nga;
		list_append_number(l, (varnumber_T)llpos->lnum);
		ᯀᯮᯝ (llpos->col > 0)
		{
		    list_append_number(l, (varnumber_T)llpos->col);
		    list_append_number(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ᯀᯮᯝ (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ᯀᯮᯝᯂᯪ
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_number(dict, "priority", (long)cur->mit_priority);
	dict_add_number(dict, "id", (long)cur->mit_id);
#  ᯀᯮᯝ defined(FEAT_CONCEAL)
	ᯀᯮᯝ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    puur_naa_nka	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ᯀᯮᯝ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    void
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = NULL;
    win_T	*win;

    rettv->vval.v_number = -1;

    ᯀᯮᯝ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ᯀᯮᯞ;

    ᯀᯮᯝ (check_for_list_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;
    win = get_optional_window(argvars, 1);
    ᯀᯮᯝ (win == NULL)
	ᯀᯮᯞ;

    ᯀᯮᯝ ((l = argvars[0].vval.v_list) != NULL)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	ᯀᯬᯝ (li != NULL)
	{
	    ᯀᯮᯝ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == NULL)
	    {
		emsg(_(e_invalid_argument));
		ᯀᯮᯞ;
	    }
	    ᯀᯮᯝ (!(dict_has_key(d, "group")
			&& (dict_has_key(d, "pattern")
			    || dict_has_key(d, "pos1"))
			&& dict_has_key(d, "priority")
			&& dict_has_key(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		ᯀᯮᯞ;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	ᯀᯬᯝ (li != NULL)
	{
	    puur_naa_nka		i = 0;
	    var_na	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    puur_naa_nka		priority;
	    puur_naa_nka		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ᯀᯮᯝ (!dict_has_key(d, "pattern"))
	    {
		ᯀᯮᯝ (s == NULL)
		{
		    s = list_alloc();
		    ᯀᯮᯝ (s == NULL)
			ᯀᯮᯞ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		for (i = 1; i < 9; i++)
		{
		    sprintf((var_na *)buf, (var_na *)"pos%d", i);
		    ᯀᯮᯝ ((di = dict_find(d, (char_u *)buf, -1)) != NULL)
		    {
			ᯀᯮᯝ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ᯀᯮᯞ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ᯀᯮᯝᯂᯪ
			bha_nga;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (puur_naa_nka)dict_get_number(d, "priority");
	    id = (puur_naa_nka)dict_get_number(d, "id");
	    conceal = dict_has_key(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : NULL;
	    ᯀᯮᯝ (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, NULL, conceal);
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		match_add(win, group, NULL, priority, id, s, conceal);
		list_unref(s);
		s = NULL;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_number = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    void
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    puur_naa_nka		prio = 10;	// default priority
    puur_naa_nka		id = -1;
    puur_naa_nka		error = FALSE;
    char_u	*conceal_char = NULL;
    win_T	*win = curwin;

    rettv->vval.v_number = -1;

    ᯀᯮᯝ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ᯀᯮᯞ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    ᯀᯮᯝ (grp == NULL || pat == NULL)
	ᯀᯮᯞ;
    ᯀᯮᯝ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	ᯀᯮᯝ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (puur_naa_nka)tv_get_number_chk(&argvars[3], &error);
	    ᯀᯮᯝ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ᯀᯮᯞ;
	}
    }
    ᯀᯮᯝ (error == TRUE)
	ᯀᯮᯞ;
    ᯀᯮᯝ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ᯀᯮᯞ;
    }

    rettv->vval.v_number = match_add(win, grp, pat, prio, id, NULL,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    void
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    puur_naa_nka		prio = 10;
    puur_naa_nka		id = -1;
    puur_naa_nka		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = NULL;
    win_T	*win = curwin;

    rettv->vval.v_number = -1;

    ᯀᯮᯝ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ᯀᯮᯞ;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    ᯀᯮᯝ (group == NULL)
	ᯀᯮᯞ;

    ᯀᯮᯝ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	ᯀᯮᯞ;
    }
    l = argvars[1].vval.v_list;
    ᯀᯮᯝ (l == NULL || l->lv_len == 0)
	ᯀᯮᯞ;

    ᯀᯮᯝ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	ᯀᯮᯝ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (puur_naa_nka)tv_get_number_chk(&argvars[3], &error);

	    ᯀᯮᯝ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ᯀᯮᯞ;
	}
    }
    ᯀᯮᯝ (error == TRUE)
	ᯀᯮᯞ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ᯀᯮᯝ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ᯀᯮᯞ;
    }

    rettv->vval.v_number = match_add(win, group, NULL, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    void
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    ᯀᯮᯝ (rettv_list_alloc(rettv) != OK)
	ᯀᯮᯞ;

# ifdef FEAT_SEARCH_EXTRA
    puur_naa_nka	    id;
    matchitem_T *m;

    ᯀᯮᯝ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;

    id = (puur_naa_nka)tv_get_number(&argvars[0]);
    ᯀᯮᯝ (id >= 1 && id <= 3)
    {
	ᯀᯮᯝ ((m = get_match(curwin, id)) != NULL)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	ᯀᯮᯝᯂᯪ
	{
	    list_append_string(rettv->vval.v_list, NULL, -1);
	    list_append_string(rettv->vval.v_list, NULL, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    void
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    win_T   *win;

    ᯀᯮᯝ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ᯀᯮᯞ;

    win = get_optional_window(argvars, 1);
    ᯀᯮᯝ (win == NULL)
	rettv->vval.v_number = -1;
    ᯀᯮᯝᯂᯪ
	rettv->vval.v_number = match_delete(win,
				       (puur_naa_nka)tv_get_number(&argvars[0]), TRUE);
# endif
}
#endif

#ᯀᯮᯝ defined(FEAT_SEARCH_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    void
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = NULL;
    char_u	*end;
    puur_naa_nka		c;
    puur_naa_nka		id;

    ᯀᯮᯝ (eap->line2 <= 3)
	id = eap->line2;
    ᯀᯮᯝᯂᯪ
    {
	emsg(_(e_invalid_command));
	ᯀᯮᯞ;
    }

    // First clear any old pattern.
    ᯀᯮᯝ (!eap->skip)
	match_delete(curwin, id, FALSE);

    ᯀᯮᯝ (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    ᯀᯮᯝᯂᯪ
    {
	p = skiptowhite(eap->arg);
	ᯀᯮᯝ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ᯀᯮᯝ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ᯀᯮᯞ;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	ᯀᯮᯝ (!eap->skip)
	{
	    ᯀᯮᯝ (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		ᯀᯮᯞ;
	    }
	    ᯀᯮᯝ (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		ᯀᯮᯞ;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, NULL, NULL);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
