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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_অনুসন্ধান_EXTRA)

# define অনুসন্ধান_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    স্থির পূর্ণাঙ্ক
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    পূর্ণাঙ্ক		prio,
    পূর্ণাঙ্ক		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = রিক্ত;
    পূর্ণাঙ্ক		hlg_id;
    regprog_T	*regprog = রিক্ত;
    পূর্ণাঙ্ক		rtype = UPD_SOME_VALID;

    যদি (*grp == NUL || (pat != রিক্ত && *pat == NUL))
	ঘূৰাই -1;
    যদি (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	ঘূৰাই -1;
    }
    যদি (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    অন্যথা
    {
	// check the given ID is not already in use
	বাবে (cur = wp->w_match_head; cur != রিক্ত; cur = cur->mit_next)
	    যদি (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		ঘূৰাই -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	যদি (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    যদি ((hlg_id = syn_namen2id(grp, (পূর্ণাঙ্ক)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	ঘূৰাই -1;
    }
    যদি (pat != রিক্ত && (regprog = vim_regcomp(pat, RE_MAGIC)) == রিক্ত)
    {
	semsg(_(e_invalid_argument_str), pat);
	ঘূৰাই -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    যদি (m == রিক্ত)
	গচ্ছ fail;
    যদি (pos_list != রিক্ত && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	যদি (m->mit_pos_array == রিক্ত)
	    গচ্ছ fail;
	m->mit_pos_গুনতি = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == রিক্ত ? রিক্ত : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# যদি defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    যদি (conceal_char != রিক্ত)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    যদি (pos_list != রিক্ত)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	পূর্ণাঙ্ক		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	বাবে (i = 0, li = pos_list->lv_first; li != রিক্ত; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    পূর্ণাঙ্ক		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    পূর্ণাঙ্ক		error = FALSE;

	    যদি (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		যদি (subl == রিক্ত)
		    গচ্ছ fail;
		subli = subl->lv_first;
		যদি (subli == রিক্ত)
		    গচ্ছ fail;
		lnum = tv_get_সংখ্যা_chk(&subli->li_tv, &error);
		যদি (error == TRUE)
		    গচ্ছ fail;
		যদি (lnum == 0)
		{
		    --i;
		    অনুৱর্ত;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		যদি (subli != রিক্ত)
		{
		    col = tv_get_সংখ্যা_chk(&subli->li_tv, &error);
		    যদি (error == TRUE)
			গচ্ছ fail;
		    subli = subli->li_next;
		    যদি (subli != রিক্ত)
		    {
			len = tv_get_সংখ্যা_chk(&subli->li_tv, &error);
			যদি (error == TRUE)
			    গচ্ছ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    অন্যথা যদি (li->li_tv.v_type == VAR_সংখ্যা)
	    {
		যদি (li->li_tv.vval.v_সংখ্যা == 0)
		{
		    --i;
		    অনুৱর্ত;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_সংখ্যা;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    অন্যথা
	    {
		emsg(_(e_list_or_সংখ্যা_required));
		গচ্ছ fail;
	    }
	    যদি (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    যদি (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	যদি (toplnum != 0)
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
    যেতিয়ালৈ (cur != রিক্ত && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    যদি (cur == prev)
	wp->w_match_head = m;
    অন্যথা
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    ঘূৰাই id;

fail:
    vim_regfree(regprog);
    যদি (m != রিক্ত)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ঘূৰাই -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    স্থির পূর্ণাঙ্ক
match_delete(win_T *wp, পূর্ণাঙ্ক id, পূর্ণাঙ্ক perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    পূর্ণাঙ্ক		rtype = UPD_SOME_VALID;

    যদি (id < 1)
    {
	যদি (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	ঘূৰাই -1;
    }
    যেতিয়ালৈ (cur != রিক্ত && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    যদি (cur == রিক্ত)
    {
	যদি (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	ঘূৰাই -1;
    }
    যদি (cur == prev)
	wp->w_match_head = cur->mit_next;
    অন্যথা
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    যদি (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ঘূৰাই 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    শূন্য
clear_matches(win_T *wp)
{
    matchitem_T *m;

    যেতিয়ালৈ (wp->w_match_head != রিক্ত)
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
    স্থির matchitem_T *
get_match(win_T *wp, পূর্ণাঙ্ক id)
{
    matchitem_T *cur = wp->w_match_head;

    যেতিয়ালৈ (cur != রিক্ত && cur->mit_id != id)
	cur = cur->mit_next;
    ঘূৰাই cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    শূন্য
init_অনুসন্ধান_hl(win_T *wp, match_T *অনুসন্ধান_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    যেতিয়ালৈ (cur != রিক্ত)
    {
	cur->mit_hl.rm = cur->mit_match;
	যদি (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	অন্যথা
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    অনুসন্ধান_hl->attr = HL_ATTR(HLF_L);

    অনুসন্ধান_hl->buf = wp->w_buffer;
    অনুসন্ধান_hl->lnum = 0;
    অনুসন্ধান_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    স্থির পূর্ণাঙ্ক
next_অনুসন্ধান_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    found = -1;

    বাবে (i = match->mit_pos_cur; i < match->mit_pos_গুনতি; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	যদি (pos->lnum == 0)
	    ভঙ্গ;
	যদি (pos->len == 0 && pos->col < mincol)
	    অনুৱর্ত;
	যদি (pos->lnum == lnum)
	{
	    যদি (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		যদি (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    অন্যথা
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    যদি (found >= 0)
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
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    স্থির শূন্য
next_অনুসন্ধান_hl(
    win_T	    *win,
    match_T	    *অনুসন্ধান_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    পূর্ণাঙ্ক		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    যদি ((lnum < অনুসন্ধান_first_সাৰি || lnum > অনুসন্ধান_last_সাৰি) && cur == রিক্ত)
    {
	shl->lnum = 0;
	ঘূৰাই;
    }

    যদি (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	যদি (lnum > l)
	    shl->lnum = 0;
	অন্যথা যদি (lnum < l || shl->rm.endpos[0].col > mincol)
	    ঘূৰাই;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    বাবে (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	যদি (shl->lnum == 0)
	    matchcol = 0;
	অন্যথা যদি (vim_strchr(p_cpo, CPO_অনুসন্ধান) == রিক্ত
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    যদি (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ভঙ্গ;
	    }
	    যদি (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    অন্যথা
		++matchcol;
	}
	অন্যথা
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	যদি (shl->rm.regprog != রিক্ত)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    পূর্ণাঙ্ক regprog_is_copy = (shl != অনুসন্ধান_hl && cur != রিক্ত
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    যদি (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    যদি (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		যদি (shl == অনুসন্ধান_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = রিক্ত;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ভঙ্গ;
	    }
	}
	অন্যথা যদি (cur != রিক্ত)
	    nmatched = next_অনুসন্ধান_hl_pos(shl, lnum, cur, matchcol);
	অন্যথা
	    nmatched = 0;
	যদি (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ভঙ্গ;
	}
	যদি (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ভঙ্গ;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    শূন্য
prepare_অনুসন্ধান_hl(win_T *wp, match_T *অনুসন্ধান_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    পূর্ণাঙ্ক		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    পূর্ণাঙ্ক		pos_inprogress;	// marks that position match search is
				// in progress
    পূর্ণাঙ্ক		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    যেতিয়ালৈ (cur != রিক্ত || shl_flag == FALSE)
    {
	যদি (shl_flag == FALSE)
	{
	    shl = অনুসন্ধান_hl;
	    shl_flag = TRUE;
	}
	অন্যথা
	    shl = &cur->mit_hl;
	যদি (shl->rm.regprog != রিক্ত
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    যদি (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		বাবে (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    যদি (hasFoldingWin(wp, shl->first_lnum - 1,
						      রিক্ত, রিক্ত, TRUE, রিক্ত))
			ভঙ্গ;
# অন্যথা
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    যদি (cur != রিক্ত)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    যেতিয়ালৈ (shl->first_lnum < lnum && (shl->rm.regprog != রিক্ত
					  || (cur != রিক্ত && pos_inprogress)))
	    {
		next_অনুসন্ধান_hl(wp, অনুসন্ধান_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == অনুসন্ধান_hl ? রিক্ত : cur);
		pos_inprogress = cur == রিক্ত || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		যদি (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		অন্যথা
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	যদি (shl != অনুসন্ধান_hl && cur != রিক্ত)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    স্থির শূন্য
check_cur_অনুসন্ধান_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    যদি (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    অন্যথা
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    পূর্ণাঙ্ক
prepare_অনুসন্ধান_hl_সাৰি(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **সাৰি,
	match_T	    *অনুসন্ধান_hl,
	পূর্ণাঙ্ক	    *অনুসন্ধান_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    পূর্ণাঙ্ক		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    পূর্ণাঙ্ক		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    যেতিয়ালৈ (cur != রিক্ত || shl_flag == FALSE)
    {
	যদি (shl_flag == FALSE)
	{
	    shl = অনুসন্ধান_hl;
	    shl_flag = TRUE;
	}
	অন্যথা
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	যদি (cur != রিক্ত)
	    cur->mit_pos_cur = 0;
	next_অনুসন্ধান_hl(wp, অনুসন্ধান_hl, shl, lnum, mincol,
						shl == অনুসন্ধান_hl ? রিক্ত : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);

	যদি (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    যদি (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    অন্যথা
		shl->startcol = 0;
	    যদি (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    অন্যথা
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    যদি (shl == অনুসন্ধান_hl)
		check_cur_অনুসন্ধান_hl(wp, shl);

	    // Highlight one character for an empty match.
	    যদি (shl->startcol == shl->endcol)
	    {
		যদি (has_mbyte && (*সাৰি)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*সাৰি) + shl->endcol);
		অন্যথা
		    ++shl->endcol;
	    }
	    যদি ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*অনুসন্ধান_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	যদি (shl != অনুসন্ধান_hl && cur != রিক্ত)
	    cur = cur->mit_next;
    }
    ঘূৰাই area_highlighting;
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
    পূর্ণাঙ্ক
update_অনুসন্ধান_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **সাৰি,
	match_T	    *অনুসন্ধান_hl,
	পূর্ণাঙ্ক	    *has_match_conc UNUSED,
	পূর্ণাঙ্ক	    *match_conc UNUSED,
	পূর্ণাঙ্ক	    did_সাৰি_attr,
	পূর্ণাঙ্ক	    lcs_eol_one,
	পূর্ণাঙ্ক	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    পূর্ণাঙ্ক		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    পূর্ণাঙ্ক		pos_inprogress;	    // marks that position match search is in
				    // progress
    পূর্ণাঙ্ক		অনুসন্ধান_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    যেতিয়ালৈ (cur != রিক্ত || shl_flag == FALSE)
    {
	যদি (shl_flag == FALSE
		&& (cur == রিক্ত
			|| cur->mit_priority > অনুসন্ধান_HL_PRIORITY))
	{
	    shl = অনুসন্ধান_hl;
	    shl_flag = TRUE;
	}
	অন্যথা
	    shl = &cur->mit_hl;
	যদি (cur != রিক্ত)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	যেতিয়ালৈ (shl->rm.regprog != রিক্ত || (cur != রিক্ত && pos_inprogress))
	{
	    যদি (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		পূর্ণাঙ্ক next_col = col + mb_ptr2len(*সাৰি + col);

		যদি (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		যদি (cur != রিক্ত
			&& shl != অনুসন্ধান_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		অন্যথা
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		যদি (shl == অনুসন্ধান_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    যদি (shl->attr_cur != shl->attr)
			অনুসন্ধান_hl_has_cursor_lnum = lnum;
		}

	    }
	    অন্যথা যদি (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_অনুসন্ধান_hl(wp, অনুসন্ধান_hl, shl, lnum, col,
					       shl == অনুসন্ধান_hl ? রিক্ত : cur);
		pos_inprogress = !(cur == রিক্ত || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);

		যদি (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    যদি (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    অন্যথা
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    যদি (shl == অনুসন্ধান_hl)
			check_cur_অনুসন্ধান_hl(wp, shl);

		    যদি (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			যদি (has_mbyte)
			{
			    char_u *p = *সাৰি + shl->endcol;

			    যদি (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    অন্যথা
				shl->endcol += (*mb_ptr2len)(p);
			}
			অন্যথা
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    অনুৱর্ত;
		}
	    }
	    ভঙ্গ;
	}
	যদি (shl != অনুসন্ধান_hl && cur != রিক্ত)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    যেতিয়ালৈ (cur != রিক্ত || shl_flag == FALSE)
    {
	যদি (shl_flag == FALSE
		&& (cur == রিক্ত ||
			cur->mit_priority > অনুসন্ধান_HL_PRIORITY))
	{
	    shl = অনুসন্ধান_hl;
	    shl_flag = TRUE;
	}
	অন্যথা
	    shl = &cur->mit_hl;
	যদি (shl->attr_cur != 0)
	{
	    অনুসন্ধান_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	যদি (shl != অনুসন্ধান_hl && cur != রিক্ত)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    যদি (*(*সাৰি + col) == NUL && (did_সাৰি_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	অনুসন্ধান_attr = 0;
    ঘূৰাই অনুসন্ধান_attr;
}

    পূর্ণাঙ্ক
get_prevcol_hl_flag(win_T *wp, match_T *অনুসন্ধান_hl, long curcol)
{
    long	prevcol = curcol;
    পূর্ণাঙ্ক		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# যদি defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    যদি (popup_is_popup(wp))
	ঘূৰাই FALSE;
# endif

    // we're not really at that column when skipping some text
    যদি ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    যদি (!অনুসন্ধান_hl->is_addpos && (prevcol == (long)অনুসন্ধান_hl->startcol
		|| (prevcol > (long)অনুসন্ধান_hl->startcol
					      && অনুসন্ধান_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    অন্যথা
    {
	cur = wp->w_match_head;
	যেতিয়ালৈ (cur != রিক্ত)
	{
	    যদি (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ভঙ্গ;
	    }
	    cur = cur->mit_next;
	}
    }
    ঘূৰাই prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    শূন্য
get_অনুসন্ধান_match_hl(win_T *wp, match_T *অনুসন্ধান_hl, long col, পূর্ণাঙ্ক *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    পূর্ণাঙ্ক		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    যেতিয়ালৈ (cur != রিক্ত || shl_flag == FALSE)
    {
	যদি (shl_flag == FALSE
		&& ((cur != রিক্ত
			&& cur->mit_priority > অনুসন্ধান_HL_PRIORITY)
		    || cur == রিক্ত))
	{
	    shl = অনুসন্ধান_hl;
	    shl_flag = TRUE;
	}
	অন্যথা
	    shl = &cur->mit_hl;
	যদি (col - 1 == (long)shl->startcol
		&& (shl == অনুসন্ধান_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	যদি (shl != অনুসন্ধান_hl && cur != রিক্ত)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#যদি defined(FEAT_EVAL)
# ifdef FEAT_অনুসন্ধান_EXTRA
    স্থির পূর্ণাঙ্ক
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    যদি (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ঘূৰাই FAIL;
    }

    যদি (dict_has_চাবি(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    যদি ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == রিক্ত)
	ঘূৰাই OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    যদি (*win == রিক্ত)
    {
	emsg(_(e_invalid_window_সংখ্যা));
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}
# endif

/*
 * "clearmatches()" function
 */
    শূন্য
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    win_T   *win;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    win = get_optional_window(argvars, 0);
    যদি (win != রিক্ত)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    শূন্য
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    পূর্ণাঙ্ক		i;
    win_T	*win;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    win = get_optional_window(argvars, 0);
    যদি (rettv_list_alloc(rettv) == FAIL || win == রিক্ত)
	ঘূৰাই;

    cur = win->w_match_head;
    যেতিয়ালৈ (cur != রিক্ত)
    {
	dict = dict_alloc();
	যদি (dict == রিক্ত)
	    ঘূৰাই;
	যদি (cur->mit_match.regprog == রিক্ত)
	{
	    // match added with matchaddpos()
	    বাবে (i = 0; i < cur->mit_pos_গুনতি; ++i)
	    {
		llpos_T	*llpos;
		ৱর্ণ	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		যদি (llpos->lnum == 0)
		    ভঙ্গ;
		l = list_alloc();
		যদি (l == রিক্ত)
		    ভঙ্গ;
		list_append_সংখ্যা(l, (varnumber_T)llpos->lnum);
		যদি (llpos->col > 0)
		{
		    list_append_সংখ্যা(l, (varnumber_T)llpos->col);
		    list_append_সংখ্যা(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		যদি (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	অন্যথা
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_সংখ্যা(dict, "priority", (long)cur->mit_priority);
	dict_add_সংখ্যা(dict, "id", (long)cur->mit_id);
#  যদি defined(FEAT_CONCEAL)
	যদি (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    পূর্ণাঙ্ক	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	যদি (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    শূন্য
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = রিক্ত;
    win_T	*win;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ঘূৰাই;
    win = get_optional_window(argvars, 1);
    যদি (win == রিক্ত)
	ঘূৰাই;

    যদি ((l = argvars[0].vval.v_list) != রিক্ত)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	যেতিয়ালৈ (li != রিক্ত)
	{
	    যদি (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == রিক্ত)
	    {
		emsg(_(e_invalid_argument));
		ঘূৰাই;
	    }
	    যদি (!(dict_has_চাবি(d, "group")
			&& (dict_has_চাবি(d, "pattern")
			    || dict_has_চাবি(d, "pos1"))
			&& dict_has_চাবি(d, "priority")
			&& dict_has_চাবি(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		ঘূৰাই;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	যেতিয়ালৈ (li != রিক্ত)
	{
	    পূর্ণাঙ্ক		i = 0;
	    ৱর্ণ	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    পূর্ণাঙ্ক		priority;
	    পূর্ণাঙ্ক		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    যদি (!dict_has_চাবি(d, "pattern"))
	    {
		যদি (s == রিক্ত)
		{
		    s = list_alloc();
		    যদি (s == রিক্ত)
			ঘূৰাই;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		বাবে (i = 1; i < 9; i++)
		{
		    sprintf((ৱর্ণ *)buf, (ৱর্ণ *)"pos%d", i);
		    যদি ((di = dict_find(d, (char_u *)buf, -1)) != রিক্ত)
		    {
			যদি (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ঘূৰাই;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    অন্যথা
			ভঙ্গ;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "priority");
	    id = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "id");
	    conceal = dict_has_চাবি(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : রিক্ত;
	    যদি (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, রিক্ত, conceal);
	    }
	    অন্যথা
	    {
		match_add(win, group, রিক্ত, priority, id, s, conceal);
		list_unref(s);
		s = রিক্ত;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_সংখ্যা = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    শূন্য
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    পূর্ণাঙ্ক		prio = 10;	// default priority
    পূর্ণাঙ্ক		id = -1;
    পূর্ণাঙ্ক		error = FALSE;
    char_u	*conceal_char = রিক্ত;
    win_T	*win = curwin;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ঘূৰাই;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    যদি (grp == রিক্ত || pat == রিক্ত)
	ঘূৰাই;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[3], &error);
	    যদি (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ঘূৰাই;
	}
    }
    যদি (error == TRUE)
	ঘূৰাই;
    যদি (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ঘূৰাই;
    }

    rettv->vval.v_সংখ্যা = match_add(win, grp, pat, prio, id, রিক্ত,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    শূন্য
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    পূর্ণাঙ্ক		prio = 10;
    পূর্ণাঙ্ক		id = -1;
    পূর্ণাঙ্ক		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = রিক্ত;
    win_T	*win = curwin;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ঘূৰাই;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    যদি (group == রিক্ত)
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	ঘূৰাই;
    }
    l = argvars[1].vval.v_list;
    যদি (l == রিক্ত || l->lv_len == 0)
	ঘূৰাই;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[3], &error);

	    যদি (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ঘূৰাই;
	}
    }
    যদি (error == TRUE)
	ঘূৰাই;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    যদি (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ঘূৰাই;
    }

    rettv->vval.v_সংখ্যা = match_add(win, group, রিক্ত, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    শূন্য
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) != OK)
	ঘূৰাই;

# ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণাঙ্ক	    id;
    matchitem_T *m;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]);
    যদি (id >= 1 && id <= 3)
    {
	যদি ((m = get_match(curwin, id)) != রিক্ত)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	অন্যথা
	{
	    list_append_string(rettv->vval.v_list, রিক্ত, -1);
	    list_append_string(rettv->vval.v_list, রিক্ত, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    শূন্য
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_অনুসন্ধান_EXTRA
    win_T   *win;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    win = get_optional_window(argvars, 1);
    যদি (win == রিক্ত)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = match_delete(win,
				       (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]), TRUE);
# endif
}
#endif

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    শূন্য
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = রিক্ত;
    char_u	*end;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		id;

    যদি (eap->line2 <= 3)
	id = eap->line2;
    অন্যথা
    {
	emsg(_(e_invalid_command));
	ঘূৰাই;
    }

    // First clear any old pattern.
    যদি (!eap->skip)
	match_delete(curwin, id, FALSE);

    যদি (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    অন্যথা যদি ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    অন্যথা
    {
	p = skiptowhite(eap->arg);
	যদি (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	যদি (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ঘূৰাই;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	যদি (!eap->skip)
	{
	    যদি (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		ঘূৰাই;
	    }
	    যদি (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		ঘূৰাই;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, রিক্ত, রিক্ত);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
