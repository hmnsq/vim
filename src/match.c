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

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)

# define ಹುಡುಕು_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		prio,
    ಪೂರ್ಣಾಙ್ಕ		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		hlg_id;
    regprog_T	*regprog = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		rtype = UPD_SOME_VALID;

    ಯದಿ (*grp == NUL || (pat != ರಿಕ್ತ && *pat == NUL))
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    ಅನ್ಯಥಾ
    {
	// check the given ID is not already in use
	ಪ್ರತಿ (cur = wp->w_match_head; cur != ರಿಕ್ತ; cur = cur->mit_next)
	    ಯದಿ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		ಪ್ರತ್ಯಾಗಮ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ಯದಿ (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    ಯದಿ ((hlg_id = syn_namen2id(grp, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (pat != ರಿಕ್ತ && (regprog = vim_regcomp(pat, RE_MAGIC)) == ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str), pat);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ಯದಿ (m == ರಿಕ್ತ)
	ಗಚ್ಛ fail;
    ಯದಿ (pos_list != ರಿಕ್ತ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ಯದಿ (m->mit_pos_array == ರಿಕ್ತ)
	    ಗಚ್ಛ fail;
	m->mit_pos_ಎಣಿಕೆ = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == ರಿಕ್ತ ? ರಿಕ್ತ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# ಯದಿ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ಯದಿ (conceal_char != ರಿಕ್ತ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ಯದಿ (pos_list != ರಿಕ್ತ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	ಪೂರ್ಣಾಙ್ಕ		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	ಪ್ರತಿ (i = 0, li = pos_list->lv_first; li != ರಿಕ್ತ; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    ಪೂರ್ಣಾಙ್ಕ		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	    ಯದಿ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ಯದಿ (subl == ರಿಕ್ತ)
		    ಗಚ್ಛ fail;
		subli = subl->lv_first;
		ಯದಿ (subli == ರಿಕ್ತ)
		    ಗಚ್ಛ fail;
		lnum = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
		ಯದಿ (error == TRUE)
		    ಗಚ್ಛ fail;
		ಯದಿ (lnum == 0)
		{
		    --i;
		    ಅನುವರ್ತ;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		ಯದಿ (subli != ರಿಕ್ತ)
		{
		    col = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
		    ಯದಿ (error == TRUE)
			ಗಚ್ಛ fail;
		    subli = subli->li_next;
		    ಯದಿ (subli != ರಿಕ್ತ)
		    {
			len = tv_get_ಸಂಖ್ಯೆ_chk(&subli->li_tv, &error);
			ಯದಿ (error == TRUE)
			    ಗಚ್ಛ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (li->li_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	    {
		ಯದಿ (li->li_tv.vval.v_ಸಂಖ್ಯೆ == 0)
		{
		    --i;
		    ಅನುವರ್ತ;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_ಸಂಖ್ಯೆ;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		emsg(_(e_list_or_ಸಂಖ್ಯೆ_required));
		ಗಚ್ಛ fail;
	    }
	    ಯದಿ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ಯದಿ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ಯದಿ (toplnum != 0)
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
    ಯಾವತ್ (cur != ರಿಕ್ತ && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ಯದಿ (cur == prev)
	wp->w_match_head = m;
    ಅನ್ಯಥಾ
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    ಪ್ರತ್ಯಾಗಮ id;

fail:
    vim_regfree(regprog);
    ಯದಿ (m != ರಿಕ್ತ)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_delete(win_T *wp, ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    ಪೂರ್ಣಾಙ್ಕ		rtype = UPD_SOME_VALID;

    ಯದಿ (id < 1)
    {
	ಯದಿ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯಾವತ್ (cur != ರಿಕ್ತ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ಯದಿ (cur == ರಿಕ್ತ)
    {
	ಯದಿ (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (cur == prev)
	wp->w_match_head = cur->mit_next;
    ಅನ್ಯಥಾ
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    ಯದಿ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    ಶೂನ್ಯ
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ಯಾವತ್ (wp->w_match_head != ರಿಕ್ತ)
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
    ಸ್ಥಿರ matchitem_T *
get_match(win_T *wp, ಪೂರ್ಣಾಙ್ಕ id)
{
    matchitem_T *cur = wp->w_match_head;

    ಯಾವತ್ (cur != ರಿಕ್ತ && cur->mit_id != id)
	cur = cur->mit_next;
    ಪ್ರತ್ಯಾಗಮ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    ಶೂನ್ಯ
init_ಹುಡುಕು_hl(win_T *wp, match_T *ಹುಡುಕು_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    ಯಾವತ್ (cur != ರಿಕ್ತ)
    {
	cur->mit_hl.rm = cur->mit_match;
	ಯದಿ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ಅನ್ಯಥಾ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    ಹುಡುಕು_hl->attr = HL_ATTR(HLF_L);

    ಹುಡುಕು_hl->buf = wp->w_buffer;
    ಹುಡುಕು_hl->lnum = 0;
    ಹುಡುಕು_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
next_ಹುಡುಕು_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    found = -1;

    ಪ್ರತಿ (i = match->mit_pos_cur; i < match->mit_pos_ಎಣಿಕೆ; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	ಯದಿ (pos->lnum == 0)
	    ಭಙ್ಗ;
	ಯದಿ (pos->len == 0 && pos->col < mincol)
	    ಅನುವರ್ತ;
	ಯದಿ (pos->lnum == lnum)
	{
	    ಯದಿ (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ಯದಿ (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    ಅನ್ಯಥಾ
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    ಯದಿ (found >= 0)
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
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
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
next_ಹುಡುಕು_hl(
    win_T	    *win,
    match_T	    *ಹುಡುಕು_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    ಯದಿ ((lnum < ಹುಡುಕು_first_ಸಾಲು || lnum > ಹುಡುಕು_last_ಸಾಲು) && cur == ರಿಕ್ತ)
    {
	shl->lnum = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ಯದಿ (lnum > l)
	    shl->lnum = 0;
	ಅನ್ಯಥಾ ಯದಿ (lnum < l || shl->rm.endpos[0].col > mincol)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    ಪ್ರತಿ (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ಯದಿ (shl->lnum == 0)
	    matchcol = 0;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr(p_cpo, CPO_ಹುಡುಕು) == ರಿಕ್ತ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ಯದಿ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ಅನ್ಯಥಾ
		++matchcol;
	}
	ಅನ್ಯಥಾ
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	ಯದಿ (shl->rm.regprog != ರಿಕ್ತ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    ಪೂರ್ಣಾಙ್ಕ regprog_is_copy = (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    ಯದಿ (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    ಯದಿ (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		ಯದಿ (shl == ಹುಡುಕು_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ರಿಕ್ತ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (cur != ರಿಕ್ತ)
	    nmatched = next_ಹುಡುಕು_hl_pos(shl, lnum, cur, matchcol);
	ಅನ್ಯಥಾ
	    nmatched = 0;
	ಯದಿ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ಭಙ್ಗ;
	}
	ಯದಿ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ಭಙ್ಗ;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    ಶೂನ್ಯ
prepare_ಹುಡುಕು_hl(win_T *wp, match_T *ಹುಡುಕು_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    ಪೂರ್ಣಾಙ್ಕ		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    ಪೂರ್ಣಾಙ್ಕ		pos_inprogress;	// marks that position match search is
				// in progress
    ಪೂರ್ಣಾಙ್ಕ		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ಯಾವತ್ (cur != ರಿಕ್ತ || shl_flag == FALSE)
    {
	ಯದಿ (shl_flag == FALSE)
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಅನ್ಯಥಾ
	    shl = &cur->mit_hl;
	ಯದಿ (shl->rm.regprog != ರಿಕ್ತ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ಯದಿ (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		ಪ್ರತಿ (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    ಯದಿ (hasFoldingWin(wp, shl->first_lnum - 1,
						      ರಿಕ್ತ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ))
			ಭಙ್ಗ;
# ಅನ್ಯಥಾ
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    ಯದಿ (cur != ರಿಕ್ತ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ಯಾವತ್ (shl->first_lnum < lnum && (shl->rm.regprog != ರಿಕ್ತ
					  || (cur != ರಿಕ್ತ && pos_inprogress)))
	    {
		next_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == ಹುಡುಕು_hl ? ರಿಕ್ತ : cur);
		pos_inprogress = cur == ರಿಕ್ತ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ಯದಿ (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		ಅನ್ಯಥಾ
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	ಯದಿ (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_cur_ಹುಡುಕು_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ಯದಿ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    ಅನ್ಯಥಾ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    ಪೂರ್ಣಾಙ್ಕ
prepare_ಹುಡುಕು_hl_ಸಾಲು(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **ಸಾಲು,
	match_T	    *ಹುಡುಕು_hl,
	ಪೂರ್ಣಾಙ್ಕ	    *ಹುಡುಕು_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    ಪೂರ್ಣಾಙ್ಕ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    ಪೂರ್ಣಾಙ್ಕ		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ಯಾವತ್ (cur != ರಿಕ್ತ || shl_flag == FALSE)
    {
	ಯದಿ (shl_flag == FALSE)
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಅನ್ಯಥಾ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ಯದಿ (cur != ರಿಕ್ತ)
	    cur->mit_pos_cur = 0;
	next_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, lnum, mincol,
						shl == ಹುಡುಕು_hl ? ರಿಕ್ತ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);

	ಯದಿ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ಯದಿ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    ಅನ್ಯಥಾ
		shl->startcol = 0;
	    ಯದಿ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    ಅನ್ಯಥಾ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ಯದಿ (shl == ಹುಡುಕು_hl)
		check_cur_ಹುಡುಕು_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ಯದಿ (shl->startcol == shl->endcol)
	    {
		ಯದಿ (has_mbyte && (*ಸಾಲು)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*ಸಾಲು) + shl->endcol);
		ಅನ್ಯಥಾ
		    ++shl->endcol;
	    }
	    ಯದಿ ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*ಹುಡುಕು_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ಯದಿ (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ)
	    cur = cur->mit_next;
    }
    ಪ್ರತ್ಯಾಗಮ area_highlighting;
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
    ಪೂರ್ಣಾಙ್ಕ
update_ಹುಡುಕು_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **ಸಾಲು,
	match_T	    *ಹುಡುಕು_hl,
	ಪೂರ್ಣಾಙ್ಕ	    *has_match_conc UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	    *match_conc UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	    did_ಸಾಲು_attr,
	ಪೂರ್ಣಾಙ್ಕ	    lcs_eol_one,
	ಪೂರ್ಣಾಙ್ಕ	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    ಪೂರ್ಣಾಙ್ಕ		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    ಪೂರ್ಣಾಙ್ಕ		pos_inprogress;	    // marks that position match search is in
				    // progress
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ಯಾವತ್ (cur != ರಿಕ್ತ || shl_flag == FALSE)
    {
	ಯದಿ (shl_flag == FALSE
		&& (cur == ರಿಕ್ತ
			|| cur->mit_priority > ಹುಡುಕು_HL_PRIORITY))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಅನ್ಯಥಾ
	    shl = &cur->mit_hl;
	ಯದಿ (cur != ರಿಕ್ತ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ಯಾವತ್ (shl->rm.regprog != ರಿಕ್ತ || (cur != ರಿಕ್ತ && pos_inprogress))
	{
	    ಯದಿ (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		ಪೂರ್ಣಾಙ್ಕ next_col = col + mb_ptr2len(*ಸಾಲು + col);

		ಯದಿ (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ಯದಿ (cur != ರಿಕ್ತ
			&& shl != ಹುಡುಕು_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		ಅನ್ಯಥಾ
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ಯದಿ (shl == ಹುಡುಕು_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ಯದಿ (shl->attr_cur != shl->attr)
			ಹುಡುಕು_hl_has_cursor_lnum = lnum;
		}

	    }
	    ಅನ್ಯಥಾ ಯದಿ (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_ಹುಡುಕು_hl(wp, ಹುಡುಕು_hl, shl, lnum, col,
					       shl == ಹುಡುಕು_hl ? ರಿಕ್ತ : cur);
		pos_inprogress = !(cur == ರಿಕ್ತ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);

		ಯದಿ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    ಯದಿ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    ಅನ್ಯಥಾ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ಯದಿ (shl == ಹುಡುಕು_hl)
			check_cur_ಹುಡುಕು_hl(wp, shl);

		    ಯದಿ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ಯದಿ (has_mbyte)
			{
			    char_u *p = *ಸಾಲು + shl->endcol;

			    ಯದಿ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    ಅನ್ಯಥಾ
				shl->endcol += (*mb_ptr2len)(p);
			}
			ಅನ್ಯಥಾ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ಅನುವರ್ತ;
		}
	    }
	    ಭಙ್ಗ;
	}
	ಯದಿ (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ಯಾವತ್ (cur != ರಿಕ್ತ || shl_flag == FALSE)
    {
	ಯದಿ (shl_flag == FALSE
		&& (cur == ರಿಕ್ತ ||
			cur->mit_priority > ಹುಡುಕು_HL_PRIORITY))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಅನ್ಯಥಾ
	    shl = &cur->mit_hl;
	ಯದಿ (shl->attr_cur != 0)
	{
	    ಹುಡುಕು_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	ಯದಿ (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    ಯದಿ (*(*ಸಾಲು + col) == NUL && (did_ಸಾಲು_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	ಹುಡುಕು_attr = 0;
    ಪ್ರತ್ಯಾಗಮ ಹುಡುಕು_attr;
}

    ಪೂರ್ಣಾಙ್ಕ
get_prevcol_hl_flag(win_T *wp, match_T *ಹುಡುಕು_hl, long curcol)
{
    long	prevcol = curcol;
    ಪೂರ್ಣಾಙ್ಕ		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ಯದಿ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ಯದಿ (popup_is_popup(wp))
	ಪ್ರತ್ಯಾಗಮ FALSE;
# endif

    // we're not really at that column when skipping some text
    ಯದಿ ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ಯದಿ (!ಹುಡುಕು_hl->is_addpos && (prevcol == (long)ಹುಡುಕು_hl->startcol
		|| (prevcol > (long)ಹುಡುಕು_hl->startcol
					      && ಹುಡುಕು_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ಅನ್ಯಥಾ
    {
	cur = wp->w_match_head;
	ಯಾವತ್ (cur != ರಿಕ್ತ)
	{
	    ಯದಿ (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ಭಙ್ಗ;
	    }
	    cur = cur->mit_next;
	}
    }
    ಪ್ರತ್ಯಾಗಮ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    ಶೂನ್ಯ
get_ಹುಡುಕು_match_hl(win_T *wp, match_T *ಹುಡುಕು_hl, long col, ಪೂರ್ಣಾಙ್ಕ *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    ಪೂರ್ಣಾಙ್ಕ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ಯಾವತ್ (cur != ರಿಕ್ತ || shl_flag == FALSE)
    {
	ಯದಿ (shl_flag == FALSE
		&& ((cur != ರಿಕ್ತ
			&& cur->mit_priority > ಹುಡುಕು_HL_PRIORITY)
		    || cur == ರಿಕ್ತ))
	{
	    shl = ಹುಡುಕು_hl;
	    shl_flag = TRUE;
	}
	ಅನ್ಯಥಾ
	    shl = &cur->mit_hl;
	ಯದಿ (col - 1 == (long)shl->startcol
		&& (shl == ಹುಡುಕು_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ಯದಿ (shl != ಹುಡುಕು_hl && cur != ರಿಕ್ತ)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#ಯದಿ defined(FEAT_EVAL)
# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ಯದಿ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (dict_has_ಕೀಲಿ(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ಯದಿ ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    ಯದಿ (*win == ರಿಕ್ತ)
    {
	emsg(_(e_invalid_window_ಸಂಖ್ಯೆ));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    ಶೂನ್ಯ
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    win_T   *win;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    win = get_optional_window(argvars, 0);
    ಯದಿ (win != ರಿಕ್ತ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    ಶೂನ್ಯ
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*win;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    win = get_optional_window(argvars, 0);
    ಯದಿ (rettv_list_alloc(rettv) == FAIL || win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    cur = win->w_match_head;
    ಯಾವತ್ (cur != ರಿಕ್ತ)
    {
	dict = dict_alloc();
	ಯದಿ (dict == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (cur->mit_match.regprog == ರಿಕ್ತ)
	{
	    // match added with matchaddpos()
	    ಪ್ರತಿ (i = 0; i < cur->mit_pos_ಎಣಿಕೆ; ++i)
	    {
		llpos_T	*llpos;
		ವರ್ಣ	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ಯದಿ (llpos->lnum == 0)
		    ಭಙ್ಗ;
		l = list_alloc();
		ಯದಿ (l == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->lnum);
		ಯದಿ (llpos->col > 0)
		{
		    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->col);
		    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ಯದಿ (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_ಸಂಖ್ಯೆ(dict, "priority", (long)cur->mit_priority);
	dict_add_ಸಂಖ್ಯೆ(dict, "id", (long)cur->mit_id);
#  ಯದಿ defined(FEAT_CONCEAL)
	ಯದಿ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    ಪೂರ್ಣಾಙ್ಕ	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ಯದಿ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    ಶೂನ್ಯ
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ರಿಕ್ತ;
    win_T	*win;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    win = get_optional_window(argvars, 1);
    ಯದಿ (win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((l = argvars[0].vval.v_list) != ರಿಕ್ತ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	ಯಾವತ್ (li != ರಿಕ್ತ)
	{
	    ಯದಿ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ರಿಕ್ತ)
	    {
		emsg(_(e_invalid_argument));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (!(dict_has_ಕೀಲಿ(d, "group")
			&& (dict_has_ಕೀಲಿ(d, "pattern")
			    || dict_has_ಕೀಲಿ(d, "pos1"))
			&& dict_has_ಕೀಲಿ(d, "priority")
			&& dict_has_ಕೀಲಿ(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	ಯಾವತ್ (li != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ		i = 0;
	    ವರ್ಣ	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    ಪೂರ್ಣಾಙ್ಕ		priority;
	    ಪೂರ್ಣಾಙ್ಕ		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ಯದಿ (!dict_has_ಕೀಲಿ(d, "pattern"))
	    {
		ಯದಿ (s == ರಿಕ್ತ)
		{
		    s = list_alloc();
		    ಯದಿ (s == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		ಪ್ರತಿ (i = 1; i < 9; i++)
		{
		    sprintf((ವರ್ಣ *)buf, (ವರ್ಣ *)"pos%d", i);
		    ಯದಿ ((di = dict_find(d, (char_u *)buf, -1)) != ರಿಕ್ತ)
		    {
			ಯದಿ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ಪ್ರತ್ಯಾಗಮ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ಅನ್ಯಥಾ
			ಭಙ್ಗ;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "priority");
	    id = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "id");
	    conceal = dict_has_ಕೀಲಿ(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ರಿಕ್ತ;
	    ಯದಿ (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, ರಿಕ್ತ, conceal);
	    }
	    ಅನ್ಯಥಾ
	    {
		match_add(win, group, ರಿಕ್ತ, priority, id, s, conceal);
		list_unref(s);
		s = ರಿಕ್ತ;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    ಶೂನ್ಯ
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    ಪೂರ್ಣಾಙ್ಕ		prio = 10;	// default priority
    ಪೂರ್ಣಾಙ್ಕ		id = -1;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    char_u	*conceal_char = ರಿಕ್ತ;
    win_T	*win = curwin;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    ಯದಿ (grp == ರಿಕ್ತ || pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	    ಯದಿ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (error == TRUE)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = match_add(win, grp, pat, prio, id, ರಿಕ್ತ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    ಶೂನ್ಯ
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    ಪೂರ್ಣಾಙ್ಕ		prio = 10;
    ಪೂರ್ಣಾಙ್ಕ		id = -1;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ರಿಕ್ತ;
    win_T	*win = curwin;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    ಯದಿ (group == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	ಪ್ರತ್ಯಾಗಮ;
    }
    l = argvars[1].vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ || l->lv_len == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);

	    ಯದಿ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (error == TRUE)
	ಪ್ರತ್ಯಾಗಮ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ಯದಿ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = match_add(win, group, ರಿಕ್ತ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    ಶೂನ್ಯ
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) != OK)
	ಪ್ರತ್ಯಾಗಮ;

# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ	    id;
    matchitem_T *m;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಯದಿ (id >= 1 && id <= 3)
    {
	ಯದಿ ((m = get_match(curwin, id)) != ರಿಕ್ತ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	ಅನ್ಯಥಾ
	{
	    list_append_string(rettv->vval.v_list, ರಿಕ್ತ, -1);
	    list_append_string(rettv->vval.v_list, ರಿಕ್ತ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    ಶೂನ್ಯ
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ಹುಡುಕು_EXTRA
    win_T   *win;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    win = get_optional_window(argvars, 1);
    ಯದಿ (win == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = match_delete(win,
				       (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), TRUE);
# endif
}
#endif

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    ಶೂನ್ಯ
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ರಿಕ್ತ;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		id;

    ಯದಿ (eap->line2 <= 3)
	id = eap->line2;
    ಅನ್ಯಥಾ
    {
	emsg(_(e_invalid_command));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // First clear any old pattern.
    ಯದಿ (!eap->skip)
	match_delete(curwin, id, FALSE);

    ಯದಿ (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    ಅನ್ಯಥಾ ಯದಿ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    ಅನ್ಯಥಾ
    {
	p = skiptowhite(eap->arg);
	ಯದಿ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ಯದಿ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	ಯದಿ (!eap->skip)
	{
	    ಯದಿ (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, ರಿಕ್ತ, ರಿಕ್ತ);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
