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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)

# define തിരയൽ_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		prio,
    പൂർണ്ണസംഖ്യ		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		hlg_id;
    regprog_T	*regprog = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		rtype = UPD_SOME_VALID;

    എങ്കിൽ (*grp == NUL || (pat != ശൂന്യ && *pat == NUL))
	തിരികെ -1;
    എങ്കിൽ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	തിരികെ -1;
    }
    എങ്കിൽ (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    അല്ലെങ്കിൽ
    {
	// check the given ID is not already in use
	വേണ്ടി (cur = wp->w_match_head; cur != ശൂന്യ; cur = cur->mit_next)
	    എങ്കിൽ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		തിരികെ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	എങ്കിൽ (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    എങ്കിൽ ((hlg_id = syn_namen2id(grp, (പൂർണ്ണസംഖ്യ)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	തിരികെ -1;
    }
    എങ്കിൽ (pat != ശൂന്യ && (regprog = vim_regcomp(pat, RE_MAGIC)) == ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str), pat);
	തിരികെ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    എങ്കിൽ (m == ശൂന്യ)
	പോകുക fail;
    എങ്കിൽ (pos_list != ശൂന്യ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	എങ്കിൽ (m->mit_pos_array == ശൂന്യ)
	    പോകുക fail;
	m->mit_pos_എണ്ണം = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == ശൂന്യ ? ശൂന്യ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# എങ്കിൽ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    എങ്കിൽ (conceal_char != ശൂന്യ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    എങ്കിൽ (pos_list != ശൂന്യ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	പൂർണ്ണസംഖ്യ		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	വേണ്ടി (i = 0, li = pos_list->lv_first; li != ശൂന്യ; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    പൂർണ്ണസംഖ്യ		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    പൂർണ്ണസംഖ്യ		error = FALSE;

	    എങ്കിൽ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		എങ്കിൽ (subl == ശൂന്യ)
		    പോകുക fail;
		subli = subl->lv_first;
		എങ്കിൽ (subli == ശൂന്യ)
		    പോകുക fail;
		lnum = tv_get_സംഖ്യ_chk(&subli->li_tv, &error);
		എങ്കിൽ (error == TRUE)
		    പോകുക fail;
		എങ്കിൽ (lnum == 0)
		{
		    --i;
		    തുടരുക;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		എങ്കിൽ (subli != ശൂന്യ)
		{
		    col = tv_get_സംഖ്യ_chk(&subli->li_tv, &error);
		    എങ്കിൽ (error == TRUE)
			പോകുക fail;
		    subli = subli->li_next;
		    എങ്കിൽ (subli != ശൂന്യ)
		    {
			len = tv_get_സംഖ്യ_chk(&subli->li_tv, &error);
			എങ്കിൽ (error == TRUE)
			    പോകുക fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (li->li_tv.v_type == VAR_സംഖ്യ)
	    {
		എങ്കിൽ (li->li_tv.vval.v_സംഖ്യ == 0)
		{
		    --i;
		    തുടരുക;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_സംഖ്യ;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		emsg(_(e_list_or_സംഖ്യ_required));
		പോകുക fail;
	    }
	    എങ്കിൽ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    എങ്കിൽ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	എങ്കിൽ (toplnum != 0)
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
    ആകുമ്പോൾ (cur != ശൂന്യ && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    എങ്കിൽ (cur == prev)
	wp->w_match_head = m;
    അല്ലെങ്കിൽ
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    തിരികെ id;

fail:
    vim_regfree(regprog);
    എങ്കിൽ (m != ശൂന്യ)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    തിരികെ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_delete(win_T *wp, പൂർണ്ണസംഖ്യ id, പൂർണ്ണസംഖ്യ perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    പൂർണ്ണസംഖ്യ		rtype = UPD_SOME_VALID;

    എങ്കിൽ (id < 1)
    {
	എങ്കിൽ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	തിരികെ -1;
    }
    ആകുമ്പോൾ (cur != ശൂന്യ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    എങ്കിൽ (cur == ശൂന്യ)
    {
	എങ്കിൽ (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	തിരികെ -1;
    }
    എങ്കിൽ (cur == prev)
	wp->w_match_head = cur->mit_next;
    അല്ലെങ്കിൽ
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    എങ്കിൽ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    തിരികെ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    ശൂന്യം
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ആകുമ്പോൾ (wp->w_match_head != ശൂന്യ)
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
    സ്ഥിതം matchitem_T *
get_match(win_T *wp, പൂർണ്ണസംഖ്യ id)
{
    matchitem_T *cur = wp->w_match_head;

    ആകുമ്പോൾ (cur != ശൂന്യ && cur->mit_id != id)
	cur = cur->mit_next;
    തിരികെ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    ശൂന്യം
init_തിരയൽ_hl(win_T *wp, match_T *തിരയൽ_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    ആകുമ്പോൾ (cur != ശൂന്യ)
    {
	cur->mit_hl.rm = cur->mit_match;
	എങ്കിൽ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	അല്ലെങ്കിൽ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    തിരയൽ_hl->attr = HL_ATTR(HLF_L);

    തിരയൽ_hl->buf = wp->w_buffer;
    തിരയൽ_hl->lnum = 0;
    തിരയൽ_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
next_തിരയൽ_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    found = -1;

    വേണ്ടി (i = match->mit_pos_cur; i < match->mit_pos_എണ്ണം; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	എങ്കിൽ (pos->lnum == 0)
	    നിർത്തുക;
	എങ്കിൽ (pos->len == 0 && pos->col < mincol)
	    തുടരുക;
	എങ്കിൽ (pos->lnum == lnum)
	{
	    എങ്കിൽ (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		എങ്കിൽ (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    അല്ലെങ്കിൽ
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    എങ്കിൽ (found >= 0)
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
	തിരികെ 1;
    }
    തിരികെ 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    സ്ഥിതം ശൂന്യം
next_തിരയൽ_hl(
    win_T	    *win,
    match_T	    *തിരയൽ_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    എങ്കിൽ ((lnum < തിരയൽ_first_വരി || lnum > തിരയൽ_last_വരി) && cur == ശൂന്യ)
    {
	shl->lnum = 0;
	തിരികെ;
    }

    എങ്കിൽ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	എങ്കിൽ (lnum > l)
	    shl->lnum = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (lnum < l || shl->rm.endpos[0].col > mincol)
	    തിരികെ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    വേണ്ടി (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	എങ്കിൽ (shl->lnum == 0)
	    matchcol = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(p_cpo, CPO_തിരയൽ) == ശൂന്യ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    എങ്കിൽ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		നിർത്തുക;
	    }
	    എങ്കിൽ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    അല്ലെങ്കിൽ
		++matchcol;
	}
	അല്ലെങ്കിൽ
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	എങ്കിൽ (shl->rm.regprog != ശൂന്യ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    പൂർണ്ണസംഖ്യ regprog_is_copy = (shl != തിരയൽ_hl && cur != ശൂന്യ
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    എങ്കിൽ (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    എങ്കിൽ (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		എങ്കിൽ (shl == തിരയൽ_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ശൂന്യ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cur != ശൂന്യ)
	    nmatched = next_തിരയൽ_hl_pos(shl, lnum, cur, matchcol);
	അല്ലെങ്കിൽ
	    nmatched = 0;
	എങ്കിൽ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    നിർത്തുക;
	}
	എങ്കിൽ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    നിർത്തുക;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    ശൂന്യം
prepare_തിരയൽ_hl(win_T *wp, match_T *തിരയൽ_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    പൂർണ്ണസംഖ്യ		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    പൂർണ്ണസംഖ്യ		pos_inprogress;	// marks that position match search is
				// in progress
    പൂർണ്ണസംഖ്യ		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ആകുമ്പോൾ (cur != ശൂന്യ || shl_flag == FALSE)
    {
	എങ്കിൽ (shl_flag == FALSE)
	{
	    shl = തിരയൽ_hl;
	    shl_flag = TRUE;
	}
	അല്ലെങ്കിൽ
	    shl = &cur->mit_hl;
	എങ്കിൽ (shl->rm.regprog != ശൂന്യ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    എങ്കിൽ (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		വേണ്ടി (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    എങ്കിൽ (hasFoldingWin(wp, shl->first_lnum - 1,
						      ശൂന്യ, ശൂന്യ, TRUE, ശൂന്യ))
			നിർത്തുക;
# അല്ലെങ്കിൽ
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    എങ്കിൽ (cur != ശൂന്യ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ആകുമ്പോൾ (shl->first_lnum < lnum && (shl->rm.regprog != ശൂന്യ
					  || (cur != ശൂന്യ && pos_inprogress)))
	    {
		next_തിരയൽ_hl(wp, തിരയൽ_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == തിരയൽ_hl ? ശൂന്യ : cur);
		pos_inprogress = cur == ശൂന്യ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		എങ്കിൽ (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		അല്ലെങ്കിൽ
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	എങ്കിൽ (shl != തിരയൽ_hl && cur != ശൂന്യ)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    സ്ഥിതം ശൂന്യം
check_cur_തിരയൽ_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    എങ്കിൽ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    അല്ലെങ്കിൽ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    പൂർണ്ണസംഖ്യ
prepare_തിരയൽ_hl_വരി(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **വരി,
	match_T	    *തിരയൽ_hl,
	പൂർണ്ണസംഖ്യ	    *തിരയൽ_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    പൂർണ്ണസംഖ്യ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    പൂർണ്ണസംഖ്യ		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ആകുമ്പോൾ (cur != ശൂന്യ || shl_flag == FALSE)
    {
	എങ്കിൽ (shl_flag == FALSE)
	{
	    shl = തിരയൽ_hl;
	    shl_flag = TRUE;
	}
	അല്ലെങ്കിൽ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	എങ്കിൽ (cur != ശൂന്യ)
	    cur->mit_pos_cur = 0;
	next_തിരയൽ_hl(wp, തിരയൽ_hl, shl, lnum, mincol,
						shl == തിരയൽ_hl ? ശൂന്യ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);

	എങ്കിൽ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    എങ്കിൽ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    അല്ലെങ്കിൽ
		shl->startcol = 0;
	    എങ്കിൽ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    അല്ലെങ്കിൽ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    എങ്കിൽ (shl == തിരയൽ_hl)
		check_cur_തിരയൽ_hl(wp, shl);

	    // Highlight one character for an empty match.
	    എങ്കിൽ (shl->startcol == shl->endcol)
	    {
		എങ്കിൽ (has_mbyte && (*വരി)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*വരി) + shl->endcol);
		അല്ലെങ്കിൽ
		    ++shl->endcol;
	    }
	    എങ്കിൽ ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*തിരയൽ_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	എങ്കിൽ (shl != തിരയൽ_hl && cur != ശൂന്യ)
	    cur = cur->mit_next;
    }
    തിരികെ area_highlighting;
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
    പൂർണ്ണസംഖ്യ
update_തിരയൽ_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **വരി,
	match_T	    *തിരയൽ_hl,
	പൂർണ്ണസംഖ്യ	    *has_match_conc UNUSED,
	പൂർണ്ണസംഖ്യ	    *match_conc UNUSED,
	പൂർണ്ണസംഖ്യ	    did_വരി_attr,
	പൂർണ്ണസംഖ്യ	    lcs_eol_one,
	പൂർണ്ണസംഖ്യ	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    പൂർണ്ണസംഖ്യ		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    പൂർണ്ണസംഖ്യ		pos_inprogress;	    // marks that position match search is in
				    // progress
    പൂർണ്ണസംഖ്യ		തിരയൽ_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ആകുമ്പോൾ (cur != ശൂന്യ || shl_flag == FALSE)
    {
	എങ്കിൽ (shl_flag == FALSE
		&& (cur == ശൂന്യ
			|| cur->mit_priority > തിരയൽ_HL_PRIORITY))
	{
	    shl = തിരയൽ_hl;
	    shl_flag = TRUE;
	}
	അല്ലെങ്കിൽ
	    shl = &cur->mit_hl;
	എങ്കിൽ (cur != ശൂന്യ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ആകുമ്പോൾ (shl->rm.regprog != ശൂന്യ || (cur != ശൂന്യ && pos_inprogress))
	{
	    എങ്കിൽ (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		പൂർണ്ണസംഖ്യ next_col = col + mb_ptr2len(*വരി + col);

		എങ്കിൽ (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		എങ്കിൽ (cur != ശൂന്യ
			&& shl != തിരയൽ_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		അല്ലെങ്കിൽ
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		എങ്കിൽ (shl == തിരയൽ_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    എങ്കിൽ (shl->attr_cur != shl->attr)
			തിരയൽ_hl_has_cursor_lnum = lnum;
		}

	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_തിരയൽ_hl(wp, തിരയൽ_hl, shl, lnum, col,
					       shl == തിരയൽ_hl ? ശൂന്യ : cur);
		pos_inprogress = !(cur == ശൂന്യ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);

		എങ്കിൽ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    എങ്കിൽ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    അല്ലെങ്കിൽ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    എങ്കിൽ (shl == തിരയൽ_hl)
			check_cur_തിരയൽ_hl(wp, shl);

		    എങ്കിൽ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			എങ്കിൽ (has_mbyte)
			{
			    char_u *p = *വരി + shl->endcol;

			    എങ്കിൽ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    അല്ലെങ്കിൽ
				shl->endcol += (*mb_ptr2len)(p);
			}
			അല്ലെങ്കിൽ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    തുടരുക;
		}
	    }
	    നിർത്തുക;
	}
	എങ്കിൽ (shl != തിരയൽ_hl && cur != ശൂന്യ)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ആകുമ്പോൾ (cur != ശൂന്യ || shl_flag == FALSE)
    {
	എങ്കിൽ (shl_flag == FALSE
		&& (cur == ശൂന്യ ||
			cur->mit_priority > തിരയൽ_HL_PRIORITY))
	{
	    shl = തിരയൽ_hl;
	    shl_flag = TRUE;
	}
	അല്ലെങ്കിൽ
	    shl = &cur->mit_hl;
	എങ്കിൽ (shl->attr_cur != 0)
	{
	    തിരയൽ_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	എങ്കിൽ (shl != തിരയൽ_hl && cur != ശൂന്യ)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    എങ്കിൽ (*(*വരി + col) == NUL && (did_വരി_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	തിരയൽ_attr = 0;
    തിരികെ തിരയൽ_attr;
}

    പൂർണ്ണസംഖ്യ
get_prevcol_hl_flag(win_T *wp, match_T *തിരയൽ_hl, long curcol)
{
    long	prevcol = curcol;
    പൂർണ്ണസംഖ്യ		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# എങ്കിൽ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    എങ്കിൽ (popup_is_popup(wp))
	തിരികെ FALSE;
# endif

    // we're not really at that column when skipping some text
    എങ്കിൽ ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    എങ്കിൽ (!തിരയൽ_hl->is_addpos && (prevcol == (long)തിരയൽ_hl->startcol
		|| (prevcol > (long)തിരയൽ_hl->startcol
					      && തിരയൽ_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    അല്ലെങ്കിൽ
    {
	cur = wp->w_match_head;
	ആകുമ്പോൾ (cur != ശൂന്യ)
	{
	    എങ്കിൽ (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		നിർത്തുക;
	    }
	    cur = cur->mit_next;
	}
    }
    തിരികെ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    ശൂന്യം
get_തിരയൽ_match_hl(win_T *wp, match_T *തിരയൽ_hl, long col, പൂർണ്ണസംഖ്യ *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    പൂർണ്ണസംഖ്യ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ആകുമ്പോൾ (cur != ശൂന്യ || shl_flag == FALSE)
    {
	എങ്കിൽ (shl_flag == FALSE
		&& ((cur != ശൂന്യ
			&& cur->mit_priority > തിരയൽ_HL_PRIORITY)
		    || cur == ശൂന്യ))
	{
	    shl = തിരയൽ_hl;
	    shl_flag = TRUE;
	}
	അല്ലെങ്കിൽ
	    shl = &cur->mit_hl;
	എങ്കിൽ (col - 1 == (long)shl->startcol
		&& (shl == തിരയൽ_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	എങ്കിൽ (shl != തിരയൽ_hl && cur != ശൂന്യ)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#എങ്കിൽ defined(FEAT_EVAL)
# ifdef FEAT_തിരയൽ_EXTRA
    സ്ഥിതം പൂർണ്ണസംഖ്യ
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    എങ്കിൽ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	തിരികെ FAIL;
    }

    എങ്കിൽ (dict_has_താക്കോൽ(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    എങ്കിൽ ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == ശൂന്യ)
	തിരികെ OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    എങ്കിൽ (*win == ശൂന്യ)
    {
	emsg(_(e_invalid_window_സംഖ്യ));
	തിരികെ FAIL;
    }

    തിരികെ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    ശൂന്യം
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    win_T   *win;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    win = get_optional_window(argvars, 0);
    എങ്കിൽ (win != ശൂന്യ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    ശൂന്യം
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    പൂർണ്ണസംഖ്യ		i;
    win_T	*win;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    win = get_optional_window(argvars, 0);
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL || win == ശൂന്യ)
	തിരികെ;

    cur = win->w_match_head;
    ആകുമ്പോൾ (cur != ശൂന്യ)
    {
	dict = dict_alloc();
	എങ്കിൽ (dict == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (cur->mit_match.regprog == ശൂന്യ)
	{
	    // match added with matchaddpos()
	    വേണ്ടി (i = 0; i < cur->mit_pos_എണ്ണം; ++i)
	    {
		llpos_T	*llpos;
		അക്ഷരം	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		എങ്കിൽ (llpos->lnum == 0)
		    നിർത്തുക;
		l = list_alloc();
		എങ്കിൽ (l == ശൂന്യ)
		    നിർത്തുക;
		list_append_സംഖ്യ(l, (varnumber_T)llpos->lnum);
		എങ്കിൽ (llpos->col > 0)
		{
		    list_append_സംഖ്യ(l, (varnumber_T)llpos->col);
		    list_append_സംഖ്യ(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		എങ്കിൽ (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_സംഖ്യ(dict, "priority", (long)cur->mit_priority);
	dict_add_സംഖ്യ(dict, "id", (long)cur->mit_id);
#  എങ്കിൽ defined(FEAT_CONCEAL)
	എങ്കിൽ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    പൂർണ്ണസംഖ്യ	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	എങ്കിൽ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    ശൂന്യം
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ശൂന്യ;
    win_T	*win;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;
    win = get_optional_window(argvars, 1);
    എങ്കിൽ (win == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ ((l = argvars[0].vval.v_list) != ശൂന്യ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	ആകുമ്പോൾ (li != ശൂന്യ)
	{
	    എങ്കിൽ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ശൂന്യ)
	    {
		emsg(_(e_invalid_argument));
		തിരികെ;
	    }
	    എങ്കിൽ (!(dict_has_താക്കോൽ(d, "group")
			&& (dict_has_താക്കോൽ(d, "pattern")
			    || dict_has_താക്കോൽ(d, "pos1"))
			&& dict_has_താക്കോൽ(d, "priority")
			&& dict_has_താക്കോൽ(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		തിരികെ;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	ആകുമ്പോൾ (li != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ		i = 0;
	    അക്ഷരം	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    പൂർണ്ണസംഖ്യ		priority;
	    പൂർണ്ണസംഖ്യ		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    എങ്കിൽ (!dict_has_താക്കോൽ(d, "pattern"))
	    {
		എങ്കിൽ (s == ശൂന്യ)
		{
		    s = list_alloc();
		    എങ്കിൽ (s == ശൂന്യ)
			തിരികെ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		വേണ്ടി (i = 1; i < 9; i++)
		{
		    sprintf((അക്ഷരം *)buf, (അക്ഷരം *)"pos%d", i);
		    എങ്കിൽ ((di = dict_find(d, (char_u *)buf, -1)) != ശൂന്യ)
		    {
			എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    തിരികെ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    അല്ലെങ്കിൽ
			നിർത്തുക;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "priority");
	    id = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "id");
	    conceal = dict_has_താക്കോൽ(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ശൂന്യ;
	    എങ്കിൽ (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, ശൂന്യ, conceal);
	    }
	    അല്ലെങ്കിൽ
	    {
		match_add(win, group, ശൂന്യ, priority, id, s, conceal);
		list_unref(s);
		s = ശൂന്യ;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_സംഖ്യ = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    ശൂന്യം
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    പൂർണ്ണസംഖ്യ		prio = 10;	// default priority
    പൂർണ്ണസംഖ്യ		id = -1;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    char_u	*conceal_char = ശൂന്യ;
    win_T	*win = curwin;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	തിരികെ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    എങ്കിൽ (grp == ശൂന്യ || pat == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[3], &error);
	    എങ്കിൽ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		തിരികെ;
	}
    }
    എങ്കിൽ (error == TRUE)
	തിരികെ;
    എങ്കിൽ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	തിരികെ;
    }

    rettv->vval.v_സംഖ്യ = match_add(win, grp, pat, prio, id, ശൂന്യ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    ശൂന്യം
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    പൂർണ്ണസംഖ്യ		prio = 10;
    പൂർണ്ണസംഖ്യ		id = -1;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ശൂന്യ;
    win_T	*win = curwin;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	തിരികെ;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    എങ്കിൽ (group == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	തിരികെ;
    }
    l = argvars[1].vval.v_list;
    എങ്കിൽ (l == ശൂന്യ || l->lv_len == 0)
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[3], &error);

	    എങ്കിൽ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		തിരികെ;
	}
    }
    എങ്കിൽ (error == TRUE)
	തിരികെ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    എങ്കിൽ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	തിരികെ;
    }

    rettv->vval.v_സംഖ്യ = match_add(win, group, ശൂന്യ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    ശൂന്യം
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) != OK)
	തിരികെ;

# ifdef FEAT_തിരയൽ_EXTRA
    പൂർണ്ണസംഖ്യ	    id;
    matchitem_T *m;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
    എങ്കിൽ (id >= 1 && id <= 3)
    {
	എങ്കിൽ ((m = get_match(curwin, id)) != ശൂന്യ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	അല്ലെങ്കിൽ
	{
	    list_append_string(rettv->vval.v_list, ശൂന്യ, -1);
	    list_append_string(rettv->vval.v_list, ശൂന്യ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    ശൂന്യം
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_തിരയൽ_EXTRA
    win_T   *win;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    win = get_optional_window(argvars, 1);
    എങ്കിൽ (win == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = match_delete(win,
				       (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]), TRUE);
# endif
}
#endif

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    ശൂന്യം
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ശൂന്യ;
    char_u	*end;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		id;

    എങ്കിൽ (eap->line2 <= 3)
	id = eap->line2;
    അല്ലെങ്കിൽ
    {
	emsg(_(e_invalid_command));
	തിരികെ;
    }

    // First clear any old pattern.
    എങ്കിൽ (!eap->skip)
	match_delete(curwin, id, FALSE);

    എങ്കിൽ (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    അല്ലെങ്കിൽ എങ്കിൽ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    അല്ലെങ്കിൽ
    {
	p = skiptowhite(eap->arg);
	എങ്കിൽ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	എങ്കിൽ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    തിരികെ;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	എങ്കിൽ (!eap->skip)
	{
	    എങ്കിൽ (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		തിരികെ;
	    }
	    എങ്കിൽ (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		തിരികെ;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, ശൂന്യ, ശൂന്യ);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
