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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_શોધ_EXTRA)

# define શોધ_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    સ્થિર પૂર્ણાંક
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    પૂર્ણાંક		prio,
    પૂર્ણાંક		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ખાલી;
    પૂર્ણાંક		hlg_id;
    regprog_T	*regprog = ખાલી;
    પૂર્ણાંક		rtype = UPD_SOME_VALID;

    જો (*grp == NUL || (pat != ખાલી && *pat == NUL))
	પાછા -1;
    જો (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	પાછા -1;
    }
    જો (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    નહીંતર
    {
	// check the given ID is not already in use
	માટે (cur = wp->w_match_head; cur != ખાલી; cur = cur->mit_next)
	    જો (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		પાછા -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	જો (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    જો ((hlg_id = syn_namen2id(grp, (પૂર્ણાંક)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	પાછા -1;
    }
    જો (pat != ખાલી && (regprog = vim_regcomp(pat, RE_MAGIC)) == ખાલી)
    {
	semsg(_(e_invalid_argument_str), pat);
	પાછા -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    જો (m == ખાલી)
	જાઓ fail;
    જો (pos_list != ખાલી && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	જો (m->mit_pos_array == ખાલી)
	    જાઓ fail;
	m->mit_pos_ગણતરી = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == ખાલી ? ખાલી : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# જો defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    જો (conceal_char != ખાલી)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    જો (pos_list != ખાલી)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	પૂર્ણાંક		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	માટે (i = 0, li = pos_list->lv_first; li != ખાલી; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    પૂર્ણાંક		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    પૂર્ણાંક		error = FALSE;

	    જો (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		જો (subl == ખાલી)
		    જાઓ fail;
		subli = subl->lv_first;
		જો (subli == ખાલી)
		    જાઓ fail;
		lnum = tv_get_સંખ્યા_chk(&subli->li_tv, &error);
		જો (error == TRUE)
		    જાઓ fail;
		જો (lnum == 0)
		{
		    --i;
		    ચાલુ;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		જો (subli != ખાલી)
		{
		    col = tv_get_સંખ્યા_chk(&subli->li_tv, &error);
		    જો (error == TRUE)
			જાઓ fail;
		    subli = subli->li_next;
		    જો (subli != ખાલી)
		    {
			len = tv_get_સંખ્યા_chk(&subli->li_tv, &error);
			જો (error == TRUE)
			    જાઓ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    નહીંતર જો (li->li_tv.v_type == VAR_સંખ્યા)
	    {
		જો (li->li_tv.vval.v_સંખ્યા == 0)
		{
		    --i;
		    ચાલુ;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_સંખ્યા;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    નહીંતર
	    {
		emsg(_(e_list_or_સંખ્યા_required));
		જાઓ fail;
	    }
	    જો (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    જો (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	જો (toplnum != 0)
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
    જ્યારે (cur != ખાલી && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    જો (cur == prev)
	wp->w_match_head = m;
    નહીંતર
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    પાછા id;

fail:
    vim_regfree(regprog);
    જો (m != ખાલી)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    પાછા -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    સ્થિર પૂર્ણાંક
match_delete(win_T *wp, પૂર્ણાંક id, પૂર્ણાંક perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    પૂર્ણાંક		rtype = UPD_SOME_VALID;

    જો (id < 1)
    {
	જો (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	પાછા -1;
    }
    જ્યારે (cur != ખાલી && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    જો (cur == ખાલી)
    {
	જો (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	પાછા -1;
    }
    જો (cur == prev)
	wp->w_match_head = cur->mit_next;
    નહીંતર
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    જો (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    પાછા 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    શૂન્ય
clear_matches(win_T *wp)
{
    matchitem_T *m;

    જ્યારે (wp->w_match_head != ખાલી)
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
    સ્થિર matchitem_T *
get_match(win_T *wp, પૂર્ણાંક id)
{
    matchitem_T *cur = wp->w_match_head;

    જ્યારે (cur != ખાલી && cur->mit_id != id)
	cur = cur->mit_next;
    પાછા cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    શૂન્ય
init_શોધ_hl(win_T *wp, match_T *શોધ_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    જ્યારે (cur != ખાલી)
    {
	cur->mit_hl.rm = cur->mit_match;
	જો (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	નહીંતર
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    શોધ_hl->attr = HL_ATTR(HLF_L);

    શોધ_hl->buf = wp->w_buffer;
    શોધ_hl->lnum = 0;
    શોધ_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    સ્થિર પૂર્ણાંક
next_શોધ_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    found = -1;

    માટે (i = match->mit_pos_cur; i < match->mit_pos_ગણતરી; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	જો (pos->lnum == 0)
	    તોડો;
	જો (pos->len == 0 && pos->col < mincol)
	    ચાલુ;
	જો (pos->lnum == lnum)
	{
	    જો (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		જો (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    નહીંતર
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    જો (found >= 0)
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
	પાછા 1;
    }
    પાછા 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    સ્થિર શૂન્ય
next_શોધ_hl(
    win_T	    *win,
    match_T	    *શોધ_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    પૂર્ણાંક		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    જો ((lnum < શોધ_first_લીટી || lnum > શોધ_last_લીટી) && cur == ખાલી)
    {
	shl->lnum = 0;
	પાછા;
    }

    જો (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	જો (lnum > l)
	    shl->lnum = 0;
	નહીંતર જો (lnum < l || shl->rm.endpos[0].col > mincol)
	    પાછા;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    માટે (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	જો (shl->lnum == 0)
	    matchcol = 0;
	નહીંતર જો (vim_strchr(p_cpo, CPO_શોધ) == ખાલી
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    જો (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		તોડો;
	    }
	    જો (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    નહીંતર
		++matchcol;
	}
	નહીંતર
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	જો (shl->rm.regprog != ખાલી)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    પૂર્ણાંક regprog_is_copy = (shl != શોધ_hl && cur != ખાલી
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    જો (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    જો (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		જો (shl == શોધ_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ખાલી;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		તોડો;
	    }
	}
	નહીંતર જો (cur != ખાલી)
	    nmatched = next_શોધ_hl_pos(shl, lnum, cur, matchcol);
	નહીંતર
	    nmatched = 0;
	જો (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    તોડો;
	}
	જો (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    તોડો;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    શૂન્ય
prepare_શોધ_hl(win_T *wp, match_T *શોધ_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    પૂર્ણાંક		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    પૂર્ણાંક		pos_inprogress;	// marks that position match search is
				// in progress
    પૂર્ણાંક		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    જ્યારે (cur != ખાલી || shl_flag == FALSE)
    {
	જો (shl_flag == FALSE)
	{
	    shl = શોધ_hl;
	    shl_flag = TRUE;
	}
	નહીંતર
	    shl = &cur->mit_hl;
	જો (shl->rm.regprog != ખાલી
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    જો (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		માટે (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    જો (hasFoldingWin(wp, shl->first_lnum - 1,
						      ખાલી, ખાલી, TRUE, ખાલી))
			તોડો;
# નહીંતર
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    જો (cur != ખાલી)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    જ્યારે (shl->first_lnum < lnum && (shl->rm.regprog != ખાલી
					  || (cur != ખાલી && pos_inprogress)))
	    {
		next_શોધ_hl(wp, શોધ_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == શોધ_hl ? ખાલી : cur);
		pos_inprogress = cur == ખાલી || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		જો (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		નહીંતર
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	જો (shl != શોધ_hl && cur != ખાલી)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    સ્થિર શૂન્ય
check_cur_શોધ_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    જો (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    નહીંતર
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    પૂર્ણાંક
prepare_શોધ_hl_લીટી(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **લીટી,
	match_T	    *શોધ_hl,
	પૂર્ણાંક	    *શોધ_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    પૂર્ણાંક		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    પૂર્ણાંક		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    જ્યારે (cur != ખાલી || shl_flag == FALSE)
    {
	જો (shl_flag == FALSE)
	{
	    shl = શોધ_hl;
	    shl_flag = TRUE;
	}
	નહીંતર
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	જો (cur != ખાલી)
	    cur->mit_pos_cur = 0;
	next_શોધ_hl(wp, શોધ_hl, shl, lnum, mincol,
						shl == શોધ_hl ? ખાલી : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);

	જો (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    જો (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    નહીંતર
		shl->startcol = 0;
	    જો (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    નહીંતર
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    જો (shl == શોધ_hl)
		check_cur_શોધ_hl(wp, shl);

	    // Highlight one character for an empty match.
	    જો (shl->startcol == shl->endcol)
	    {
		જો (has_mbyte && (*લીટી)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*લીટી) + shl->endcol);
		નહીંતર
		    ++shl->endcol;
	    }
	    જો ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*શોધ_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	જો (shl != શોધ_hl && cur != ખાલી)
	    cur = cur->mit_next;
    }
    પાછા area_highlighting;
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
    પૂર્ણાંક
update_શોધ_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **લીટી,
	match_T	    *શોધ_hl,
	પૂર્ણાંક	    *has_match_conc UNUSED,
	પૂર્ણાંક	    *match_conc UNUSED,
	પૂર્ણાંક	    did_લીટી_attr,
	પૂર્ણાંક	    lcs_eol_one,
	પૂર્ણાંક	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    પૂર્ણાંક		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    પૂર્ણાંક		pos_inprogress;	    // marks that position match search is in
				    // progress
    પૂર્ણાંક		શોધ_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    જ્યારે (cur != ખાલી || shl_flag == FALSE)
    {
	જો (shl_flag == FALSE
		&& (cur == ખાલી
			|| cur->mit_priority > શોધ_HL_PRIORITY))
	{
	    shl = શોધ_hl;
	    shl_flag = TRUE;
	}
	નહીંતર
	    shl = &cur->mit_hl;
	જો (cur != ખાલી)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	જ્યારે (shl->rm.regprog != ખાલી || (cur != ખાલી && pos_inprogress))
	{
	    જો (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		પૂર્ણાંક next_col = col + mb_ptr2len(*લીટી + col);

		જો (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		જો (cur != ખાલી
			&& shl != શોધ_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		નહીંતર
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		જો (shl == શોધ_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    જો (shl->attr_cur != shl->attr)
			શોધ_hl_has_cursor_lnum = lnum;
		}

	    }
	    નહીંતર જો (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_શોધ_hl(wp, શોધ_hl, shl, lnum, col,
					       shl == શોધ_hl ? ખાલી : cur);
		pos_inprogress = !(cur == ખાલી || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);

		જો (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    જો (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    નહીંતર
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    જો (shl == શોધ_hl)
			check_cur_શોધ_hl(wp, shl);

		    જો (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			જો (has_mbyte)
			{
			    char_u *p = *લીટી + shl->endcol;

			    જો (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    નહીંતર
				shl->endcol += (*mb_ptr2len)(p);
			}
			નહીંતર
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ચાલુ;
		}
	    }
	    તોડો;
	}
	જો (shl != શોધ_hl && cur != ખાલી)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    જ્યારે (cur != ખાલી || shl_flag == FALSE)
    {
	જો (shl_flag == FALSE
		&& (cur == ખાલી ||
			cur->mit_priority > શોધ_HL_PRIORITY))
	{
	    shl = શોધ_hl;
	    shl_flag = TRUE;
	}
	નહીંતર
	    shl = &cur->mit_hl;
	જો (shl->attr_cur != 0)
	{
	    શોધ_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	જો (shl != શોધ_hl && cur != ખાલી)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    જો (*(*લીટી + col) == NUL && (did_લીટી_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	શોધ_attr = 0;
    પાછા શોધ_attr;
}

    પૂર્ણાંક
get_prevcol_hl_flag(win_T *wp, match_T *શોધ_hl, long curcol)
{
    long	prevcol = curcol;
    પૂર્ણાંક		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# જો defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    જો (popup_is_popup(wp))
	પાછા FALSE;
# endif

    // we're not really at that column when skipping some text
    જો ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    જો (!શોધ_hl->is_addpos && (prevcol == (long)શોધ_hl->startcol
		|| (prevcol > (long)શોધ_hl->startcol
					      && શોધ_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    નહીંતર
    {
	cur = wp->w_match_head;
	જ્યારે (cur != ખાલી)
	{
	    જો (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		તોડો;
	    }
	    cur = cur->mit_next;
	}
    }
    પાછા prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    શૂન્ય
get_શોધ_match_hl(win_T *wp, match_T *શોધ_hl, long col, પૂર્ણાંક *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    પૂર્ણાંક		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    જ્યારે (cur != ખાલી || shl_flag == FALSE)
    {
	જો (shl_flag == FALSE
		&& ((cur != ખાલી
			&& cur->mit_priority > શોધ_HL_PRIORITY)
		    || cur == ખાલી))
	{
	    shl = શોધ_hl;
	    shl_flag = TRUE;
	}
	નહીંતર
	    shl = &cur->mit_hl;
	જો (col - 1 == (long)shl->startcol
		&& (shl == શોધ_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	જો (shl != શોધ_hl && cur != ખાલી)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#જો defined(FEAT_EVAL)
# ifdef FEAT_શોધ_EXTRA
    સ્થિર પૂર્ણાંક
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    જો (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	પાછા FAIL;
    }

    જો (dict_has_ચાવી(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    જો ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == ખાલી)
	પાછા OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    જો (*win == ખાલી)
    {
	emsg(_(e_invalid_window_સંખ્યા));
	પાછા FAIL;
    }

    પાછા OK;
}
# endif

/*
 * "clearmatches()" function
 */
    શૂન્ય
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    win_T   *win;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    win = get_optional_window(argvars, 0);
    જો (win != ખાલી)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    શૂન્ય
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    પૂર્ણાંક		i;
    win_T	*win;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    win = get_optional_window(argvars, 0);
    જો (rettv_list_alloc(rettv) == FAIL || win == ખાલી)
	પાછા;

    cur = win->w_match_head;
    જ્યારે (cur != ખાલી)
    {
	dict = dict_alloc();
	જો (dict == ખાલી)
	    પાછા;
	જો (cur->mit_match.regprog == ખાલી)
	{
	    // match added with matchaddpos()
	    માટે (i = 0; i < cur->mit_pos_ગણતરી; ++i)
	    {
		llpos_T	*llpos;
		અક્ષર	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		જો (llpos->lnum == 0)
		    તોડો;
		l = list_alloc();
		જો (l == ખાલી)
		    તોડો;
		list_append_સંખ્યા(l, (varnumber_T)llpos->lnum);
		જો (llpos->col > 0)
		{
		    list_append_સંખ્યા(l, (varnumber_T)llpos->col);
		    list_append_સંખ્યા(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		જો (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	નહીંતર
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_સંખ્યા(dict, "priority", (long)cur->mit_priority);
	dict_add_સંખ્યા(dict, "id", (long)cur->mit_id);
#  જો defined(FEAT_CONCEAL)
	જો (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    પૂર્ણાંક	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	જો (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    શૂન્ય
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ખાલી;
    win_T	*win;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;
    win = get_optional_window(argvars, 1);
    જો (win == ખાલી)
	પાછા;

    જો ((l = argvars[0].vval.v_list) != ખાલી)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	જ્યારે (li != ખાલી)
	{
	    જો (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ખાલી)
	    {
		emsg(_(e_invalid_argument));
		પાછા;
	    }
	    જો (!(dict_has_ચાવી(d, "group")
			&& (dict_has_ચાવી(d, "pattern")
			    || dict_has_ચાવી(d, "pos1"))
			&& dict_has_ચાવી(d, "priority")
			&& dict_has_ચાવી(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		પાછા;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	જ્યારે (li != ખાલી)
	{
	    પૂર્ણાંક		i = 0;
	    અક્ષર	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    પૂર્ણાંક		priority;
	    પૂર્ણાંક		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    જો (!dict_has_ચાવી(d, "pattern"))
	    {
		જો (s == ખાલી)
		{
		    s = list_alloc();
		    જો (s == ખાલી)
			પાછા;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		માટે (i = 1; i < 9; i++)
		{
		    sprintf((અક્ષર *)buf, (અક્ષર *)"pos%d", i);
		    જો ((di = dict_find(d, (char_u *)buf, -1)) != ખાલી)
		    {
			જો (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    પાછા;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    નહીંતર
			તોડો;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (પૂર્ણાંક)dict_get_સંખ્યા(d, "priority");
	    id = (પૂર્ણાંક)dict_get_સંખ્યા(d, "id");
	    conceal = dict_has_ચાવી(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ખાલી;
	    જો (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, ખાલી, conceal);
	    }
	    નહીંતર
	    {
		match_add(win, group, ખાલી, priority, id, s, conceal);
		list_unref(s);
		s = ખાલી;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_સંખ્યા = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    શૂન્ય
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    પૂર્ણાંક		prio = 10;	// default priority
    પૂર્ણાંક		id = -1;
    પૂર્ણાંક		error = FALSE;
    char_u	*conceal_char = ખાલી;
    win_T	*win = curwin;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	પાછા;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    જો (grp == ખાલી || pat == ખાલી)
	પાછા;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[3], &error);
	    જો (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		પાછા;
	}
    }
    જો (error == TRUE)
	પાછા;
    જો (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	પાછા;
    }

    rettv->vval.v_સંખ્યા = match_add(win, grp, pat, prio, id, ખાલી,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    શૂન્ય
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    પૂર્ણાંક		prio = 10;
    પૂર્ણાંક		id = -1;
    પૂર્ણાંક		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ખાલી;
    win_T	*win = curwin;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	પાછા;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    જો (group == ખાલી)
	પાછા;

    જો (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	પાછા;
    }
    l = argvars[1].vval.v_list;
    જો (l == ખાલી || l->lv_len == 0)
	પાછા;

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[3], &error);

	    જો (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		પાછા;
	}
    }
    જો (error == TRUE)
	પાછા;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    જો (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	પાછા;
    }

    rettv->vval.v_સંખ્યા = match_add(win, group, ખાલી, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    શૂન્ય
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) != OK)
	પાછા;

# ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક	    id;
    matchitem_T *m;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
    જો (id >= 1 && id <= 3)
    {
	જો ((m = get_match(curwin, id)) != ખાલી)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	નહીંતર
	{
	    list_append_string(rettv->vval.v_list, ખાલી, -1);
	    list_append_string(rettv->vval.v_list, ખાલી, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    શૂન્ય
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_શોધ_EXTRA
    win_T   *win;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    win = get_optional_window(argvars, 1);
    જો (win == ખાલી)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = match_delete(win,
				       (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]), TRUE);
# endif
}
#endif

#જો defined(FEAT_શોધ_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    શૂન્ય
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ખાલી;
    char_u	*end;
    પૂર્ણાંક		c;
    પૂર્ણાંક		id;

    જો (eap->line2 <= 3)
	id = eap->line2;
    નહીંતર
    {
	emsg(_(e_invalid_command));
	પાછા;
    }

    // First clear any old pattern.
    જો (!eap->skip)
	match_delete(curwin, id, FALSE);

    જો (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    નહીંતર જો ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    નહીંતર
    {
	p = skiptowhite(eap->arg);
	જો (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	જો (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    પાછા;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	જો (!eap->skip)
	{
	    જો (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		પાછા;
	    }
	    જો (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		પાછા;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, ખાલી, ખાલી);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
