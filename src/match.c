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

#شامل "vim.h"

#اگر defined(FEAT_جستجو_EXTRA)

# define جستجو_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ایستا صحیح
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    صحیح		prio,
    صحیح		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = پوچ;
    صحیح		hlg_id;
    regprog_T	*regprog = پوچ;
    صحیح		rtype = UPD_SOME_VALID;

    اگر (*grp == NUL || (pat != پوچ && *pat == NUL))
	بازگشت -1;
    اگر (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	بازگشت -1;
    }
    اگر (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    وگرنه
    {
	// check the given ID is not already in use
	برای (cur = wp->w_match_head; cur != پوچ; cur = cur->mit_next)
	    اگر (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		بازگشت -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	اگر (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    اگر ((hlg_id = syn_namen2id(grp, (صحیح)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	بازگشت -1;
    }
    اگر (pat != پوچ && (regprog = vim_regcomp(pat, RE_MAGIC)) == پوچ)
    {
	semsg(_(e_invalid_argument_str), pat);
	بازگشت -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    اگر (m == پوچ)
	برو fail;
    اگر (pos_list != پوچ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	اگر (m->mit_pos_array == پوچ)
	    برو fail;
	m->mit_pos_شمار = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == پوچ ? پوچ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# اگر defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    اگر (conceal_char != پوچ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    اگر (pos_list != پوچ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	صحیح		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	برای (i = 0, li = pos_list->lv_first; li != پوچ; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    صحیح		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    صحیح		error = FALSE;

	    اگر (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		اگر (subl == پوچ)
		    برو fail;
		subli = subl->lv_first;
		اگر (subli == پوچ)
		    برو fail;
		lnum = tv_get_عدد_chk(&subli->li_tv, &error);
		اگر (error == TRUE)
		    برو fail;
		اگر (lnum == 0)
		{
		    --i;
		    ادامه;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		اگر (subli != پوچ)
		{
		    col = tv_get_عدد_chk(&subli->li_tv, &error);
		    اگر (error == TRUE)
			برو fail;
		    subli = subli->li_next;
		    اگر (subli != پوچ)
		    {
			len = tv_get_عدد_chk(&subli->li_tv, &error);
			اگر (error == TRUE)
			    برو fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    وگرنه اگر (li->li_tv.v_type == VAR_عدد)
	    {
		اگر (li->li_tv.vval.v_عدد == 0)
		{
		    --i;
		    ادامه;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_عدد;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    وگرنه
	    {
		emsg(_(e_list_or_عدد_required));
		برو fail;
	    }
	    اگر (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    اگر (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	اگر (toplnum != 0)
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
    وقتی (cur != پوچ && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    اگر (cur == prev)
	wp->w_match_head = m;
    وگرنه
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    بازگشت id;

fail:
    vim_regfree(regprog);
    اگر (m != پوچ)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    بازگشت -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ایستا صحیح
match_delete(win_T *wp, صحیح id, صحیح perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    صحیح		rtype = UPD_SOME_VALID;

    اگر (id < 1)
    {
	اگر (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	بازگشت -1;
    }
    وقتی (cur != پوچ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    اگر (cur == پوچ)
    {
	اگر (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	بازگشت -1;
    }
    اگر (cur == prev)
	wp->w_match_head = cur->mit_next;
    وگرنه
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    اگر (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    بازگشت 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    تهی
clear_matches(win_T *wp)
{
    matchitem_T *m;

    وقتی (wp->w_match_head != پوچ)
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
    ایستا matchitem_T *
get_match(win_T *wp, صحیح id)
{
    matchitem_T *cur = wp->w_match_head;

    وقتی (cur != پوچ && cur->mit_id != id)
	cur = cur->mit_next;
    بازگشت cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    تهی
init_جستجو_hl(win_T *wp, match_T *جستجو_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    وقتی (cur != پوچ)
    {
	cur->mit_hl.rm = cur->mit_match;
	اگر (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	وگرنه
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    جستجو_hl->attr = HL_ATTR(HLF_L);

    جستجو_hl->buf = wp->w_buffer;
    جستجو_hl->lnum = 0;
    جستجو_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ایستا صحیح
next_جستجو_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    صحیح	    i;
    صحیح	    found = -1;

    برای (i = match->mit_pos_cur; i < match->mit_pos_شمار; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	اگر (pos->lnum == 0)
	    بشکن;
	اگر (pos->len == 0 && pos->col < mincol)
	    ادامه;
	اگر (pos->lnum == lnum)
	{
	    اگر (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		اگر (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    وگرنه
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    اگر (found >= 0)
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
	بازگشت 1;
    }
    بازگشت 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ایستا تهی
next_جستجو_hl(
    win_T	    *win,
    match_T	    *جستجو_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    صحیح		called_emsg_before = called_emsg;
    صحیح		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    اگر ((lnum < جستجو_first_سطر || lnum > جستجو_last_سطر) && cur == پوچ)
    {
	shl->lnum = 0;
	بازگشت;
    }

    اگر (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	اگر (lnum > l)
	    shl->lnum = 0;
	وگرنه اگر (lnum < l || shl->rm.endpos[0].col > mincol)
	    بازگشت;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    برای (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	اگر (shl->lnum == 0)
	    matchcol = 0;
	وگرنه اگر (vim_strchr(p_cpo, CPO_جستجو) == پوچ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    اگر (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		بشکن;
	    }
	    اگر (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    وگرنه
		++matchcol;
	}
	وگرنه
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	اگر (shl->rm.regprog != پوچ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    صحیح regprog_is_copy = (shl != جستجو_hl && cur != پوچ
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    اگر (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    اگر (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		اگر (shl == جستجو_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = پوچ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		بشکن;
	    }
	}
	وگرنه اگر (cur != پوچ)
	    nmatched = next_جستجو_hl_pos(shl, lnum, cur, matchcol);
	وگرنه
	    nmatched = 0;
	اگر (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    بشکن;
	}
	اگر (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    بشکن;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    تهی
prepare_جستجو_hl(win_T *wp, match_T *جستجو_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    صحیح		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    صحیح		pos_inprogress;	// marks that position match search is
				// in progress
    صحیح		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    وقتی (cur != پوچ || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE)
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وگرنه
	    shl = &cur->mit_hl;
	اگر (shl->rm.regprog != پوچ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    اگر (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		برای (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    اگر (hasFoldingWin(wp, shl->first_lnum - 1,
						      پوچ, پوچ, TRUE, پوچ))
			بشکن;
# وگرنه
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    اگر (cur != پوچ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    وقتی (shl->first_lnum < lnum && (shl->rm.regprog != پوچ
					  || (cur != پوچ && pos_inprogress)))
	    {
		next_جستجو_hl(wp, جستجو_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == جستجو_hl ? پوچ : cur);
		pos_inprogress = cur == پوچ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		اگر (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		وگرنه
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	اگر (shl != جستجو_hl && cur != پوچ)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ایستا تهی
check_cur_جستجو_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    اگر (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    وگرنه
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    صحیح
prepare_جستجو_hl_سطر(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **سطر,
	match_T	    *جستجو_hl,
	صحیح	    *جستجو_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    صحیح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    صحیح		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    وقتی (cur != پوچ || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE)
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وگرنه
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	اگر (cur != پوچ)
	    cur->mit_pos_cur = 0;
	next_جستجو_hl(wp, جستجو_hl, shl, lnum, mincol,
						shl == جستجو_hl ? پوچ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);

	اگر (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    اگر (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    وگرنه
		shl->startcol = 0;
	    اگر (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    وگرنه
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    اگر (shl == جستجو_hl)
		check_cur_جستجو_hl(wp, shl);

	    // Highlight one character for an empty match.
	    اگر (shl->startcol == shl->endcol)
	    {
		اگر (has_mbyte && (*سطر)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*سطر) + shl->endcol);
		وگرنه
		    ++shl->endcol;
	    }
	    اگر ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*جستجو_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	اگر (shl != جستجو_hl && cur != پوچ)
	    cur = cur->mit_next;
    }
    بازگشت area_highlighting;
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
    صحیح
update_جستجو_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **سطر,
	match_T	    *جستجو_hl,
	صحیح	    *has_match_conc UNUSED,
	صحیح	    *match_conc UNUSED,
	صحیح	    did_سطر_attr,
	صحیح	    lcs_eol_one,
	صحیح	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    صحیح		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    صحیح		pos_inprogress;	    // marks that position match search is in
				    // progress
    صحیح		جستجو_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    وقتی (cur != پوچ || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& (cur == پوچ
			|| cur->mit_priority > جستجو_HL_PRIORITY))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وگرنه
	    shl = &cur->mit_hl;
	اگر (cur != پوچ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	وقتی (shl->rm.regprog != پوچ || (cur != پوچ && pos_inprogress))
	{
	    اگر (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		صحیح next_col = col + mb_ptr2len(*سطر + col);

		اگر (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		اگر (cur != پوچ
			&& shl != جستجو_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		وگرنه
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		اگر (shl == جستجو_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    اگر (shl->attr_cur != shl->attr)
			جستجو_hl_has_cursor_lnum = lnum;
		}

	    }
	    وگرنه اگر (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_جستجو_hl(wp, جستجو_hl, shl, lnum, col,
					       shl == جستجو_hl ? پوچ : cur);
		pos_inprogress = !(cur == پوچ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);

		اگر (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    اگر (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    وگرنه
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    اگر (shl == جستجو_hl)
			check_cur_جستجو_hl(wp, shl);

		    اگر (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			اگر (has_mbyte)
			{
			    char_u *p = *سطر + shl->endcol;

			    اگر (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    وگرنه
				shl->endcol += (*mb_ptr2len)(p);
			}
			وگرنه
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ادامه;
		}
	    }
	    بشکن;
	}
	اگر (shl != جستجو_hl && cur != پوچ)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    وقتی (cur != پوچ || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& (cur == پوچ ||
			cur->mit_priority > جستجو_HL_PRIORITY))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وگرنه
	    shl = &cur->mit_hl;
	اگر (shl->attr_cur != 0)
	{
	    جستجو_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	اگر (shl != جستجو_hl && cur != پوچ)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    اگر (*(*سطر + col) == NUL && (did_سطر_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	جستجو_attr = 0;
    بازگشت جستجو_attr;
}

    صحیح
get_prevcol_hl_flag(win_T *wp, match_T *جستجو_hl, long curcol)
{
    long	prevcol = curcol;
    صحیح		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# اگر defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    اگر (popup_is_popup(wp))
	بازگشت FALSE;
# endif

    // we're not really at that column when skipping some text
    اگر ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    اگر (!جستجو_hl->is_addpos && (prevcol == (long)جستجو_hl->startcol
		|| (prevcol > (long)جستجو_hl->startcol
					      && جستجو_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    وگرنه
    {
	cur = wp->w_match_head;
	وقتی (cur != پوچ)
	{
	    اگر (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		بشکن;
	    }
	    cur = cur->mit_next;
	}
    }
    بازگشت prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    تهی
get_جستجو_match_hl(win_T *wp, match_T *جستجو_hl, long col, صحیح *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    صحیح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    وقتی (cur != پوچ || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& ((cur != پوچ
			&& cur->mit_priority > جستجو_HL_PRIORITY)
		    || cur == پوچ))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وگرنه
	    shl = &cur->mit_hl;
	اگر (col - 1 == (long)shl->startcol
		&& (shl == جستجو_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	اگر (shl != جستجو_hl && cur != پوچ)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#اگر defined(FEAT_EVAL)
# ifdef FEAT_جستجو_EXTRA
    ایستا صحیح
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    اگر (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	بازگشت FAIL;
    }

    اگر (dict_has_کلید(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    اگر ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == پوچ)
	بازگشت OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    اگر (*win == پوچ)
    {
	emsg(_(e_invalid_window_عدد));
	بازگشت FAIL;
    }

    بازگشت OK;
}
# endif

/*
 * "clearmatches()" function
 */
    تهی
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    win_T   *win;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    win = get_optional_window(argvars, 0);
    اگر (win != پوچ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    تهی
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    صحیح		i;
    win_T	*win;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    win = get_optional_window(argvars, 0);
    اگر (rettv_list_alloc(rettv) == FAIL || win == پوچ)
	بازگشت;

    cur = win->w_match_head;
    وقتی (cur != پوچ)
    {
	dict = dict_alloc();
	اگر (dict == پوچ)
	    بازگشت;
	اگر (cur->mit_match.regprog == پوچ)
	{
	    // match added with matchaddpos()
	    برای (i = 0; i < cur->mit_pos_شمار; ++i)
	    {
		llpos_T	*llpos;
		نویسه	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		اگر (llpos->lnum == 0)
		    بشکن;
		l = list_alloc();
		اگر (l == پوچ)
		    بشکن;
		list_append_عدد(l, (varnumber_T)llpos->lnum);
		اگر (llpos->col > 0)
		{
		    list_append_عدد(l, (varnumber_T)llpos->col);
		    list_append_عدد(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		اگر (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	وگرنه
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_عدد(dict, "priority", (long)cur->mit_priority);
	dict_add_عدد(dict, "id", (long)cur->mit_id);
#  اگر defined(FEAT_CONCEAL)
	اگر (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    صحیح	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	اگر (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    تهی
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = پوچ;
    win_T	*win;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;
    win = get_optional_window(argvars, 1);
    اگر (win == پوچ)
	بازگشت;

    اگر ((l = argvars[0].vval.v_list) != پوچ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	وقتی (li != پوچ)
	{
	    اگر (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == پوچ)
	    {
		emsg(_(e_invalid_argument));
		بازگشت;
	    }
	    اگر (!(dict_has_کلید(d, "group")
			&& (dict_has_کلید(d, "pattern")
			    || dict_has_کلید(d, "pos1"))
			&& dict_has_کلید(d, "priority")
			&& dict_has_کلید(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		بازگشت;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	وقتی (li != پوچ)
	{
	    صحیح		i = 0;
	    نویسه	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    صحیح		priority;
	    صحیح		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    اگر (!dict_has_کلید(d, "pattern"))
	    {
		اگر (s == پوچ)
		{
		    s = list_alloc();
		    اگر (s == پوچ)
			بازگشت;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		برای (i = 1; i < 9; i++)
		{
		    sprintf((نویسه *)buf, (نویسه *)"pos%d", i);
		    اگر ((di = dict_find(d, (char_u *)buf, -1)) != پوچ)
		    {
			اگر (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    بازگشت;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    وگرنه
			بشکن;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (صحیح)dict_get_عدد(d, "priority");
	    id = (صحیح)dict_get_عدد(d, "id");
	    conceal = dict_has_کلید(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : پوچ;
	    اگر (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, پوچ, conceal);
	    }
	    وگرنه
	    {
		match_add(win, group, پوچ, priority, id, s, conceal);
		list_unref(s);
		s = پوچ;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_عدد = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    تهی
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    صحیح		prio = 10;	// default priority
    صحیح		id = -1;
    صحیح		error = FALSE;
    char_u	*conceal_char = پوچ;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	بازگشت;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    اگر (grp == پوچ || pat == پوچ)
	بازگشت;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحیح)tv_get_عدد_chk(&argvars[3], &error);
	    اگر (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		بازگشت;
	}
    }
    اگر (error == TRUE)
	بازگشت;
    اگر (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	بازگشت;
    }

    rettv->vval.v_عدد = match_add(win, grp, pat, prio, id, پوچ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    تهی
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    صحیح		prio = 10;
    صحیح		id = -1;
    صحیح		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = پوچ;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	بازگشت;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    اگر (group == پوچ)
	بازگشت;

    اگر (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	بازگشت;
    }
    l = argvars[1].vval.v_list;
    اگر (l == پوچ || l->lv_len == 0)
	بازگشت;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحیح)tv_get_عدد_chk(&argvars[3], &error);

	    اگر (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		بازگشت;
	}
    }
    اگر (error == TRUE)
	بازگشت;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    اگر (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	بازگشت;
    }

    rettv->vval.v_عدد = match_add(win, group, پوچ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    تهی
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) != OK)
	بازگشت;

# ifdef FEAT_جستجو_EXTRA
    صحیح	    id;
    matchitem_T *m;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(&argvars[0]);
    اگر (id >= 1 && id <= 3)
    {
	اگر ((m = get_match(curwin, id)) != پوچ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	وگرنه
	{
	    list_append_string(rettv->vval.v_list, پوچ, -1);
	    list_append_string(rettv->vval.v_list, پوچ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    تهی
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_جستجو_EXTRA
    win_T   *win;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    win = get_optional_window(argvars, 1);
    اگر (win == پوچ)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = match_delete(win,
				       (صحیح)tv_get_عدد(&argvars[0]), TRUE);
# endif
}
#endif

#اگر defined(FEAT_جستجو_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    تهی
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = پوچ;
    char_u	*end;
    صحیح		c;
    صحیح		id;

    اگر (eap->line2 <= 3)
	id = eap->line2;
    وگرنه
    {
	emsg(_(e_invalid_command));
	بازگشت;
    }

    // First clear any old pattern.
    اگر (!eap->skip)
	match_delete(curwin, id, FALSE);

    اگر (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    وگرنه اگر ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    وگرنه
    {
	p = skiptowhite(eap->arg);
	اگر (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	اگر (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    بازگشت;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	اگر (!eap->skip)
	{
	    اگر (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		بازگشت;
	    }
	    اگر (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		بازگشت;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, پوچ, پوچ);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
