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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_தேடல்_EXTRA)

# define தேடல்_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    நிலையான முழு
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    முழு		prio,
    முழு		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = வெற்று;
    முழு		hlg_id;
    regprog_T	*regprog = வெற்று;
    முழு		rtype = UPD_SOME_VALID;

    எனில் (*grp == NUL || (pat != வெற்று && *pat == NUL))
	திரும்பு -1;
    எனில் (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	திரும்பு -1;
    }
    எனில் (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    இல்லையெனில்
    {
	// check the given ID is not already in use
	க்கு (cur = wp->w_match_head; cur != வெற்று; cur = cur->mit_next)
	    எனில் (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		திரும்பு -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	எனில் (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    எனில் ((hlg_id = syn_namen2id(grp, (முழு)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	திரும்பு -1;
    }
    எனில் (pat != வெற்று && (regprog = vim_regcomp(pat, RE_MAGIC)) == வெற்று)
    {
	semsg(_(e_invalid_argument_str), pat);
	திரும்பு -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    எனில் (m == வெற்று)
	செல் fail;
    எனில் (pos_list != வெற்று && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	எனில் (m->mit_pos_array == வெற்று)
	    செல் fail;
	m->mit_pos_மொத்தம் = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == வெற்று ? வெற்று : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# எனில் defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    எனில் (conceal_char != வெற்று)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    எனில் (pos_list != வெற்று)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	முழு		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	க்கு (i = 0, li = pos_list->lv_first; li != வெற்று; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    முழு		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    முழு		error = FALSE;

	    எனில் (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		எனில் (subl == வெற்று)
		    செல் fail;
		subli = subl->lv_first;
		எனில் (subli == வெற்று)
		    செல் fail;
		lnum = tv_get_எண்_chk(&subli->li_tv, &error);
		எனில் (error == TRUE)
		    செல் fail;
		எனில் (lnum == 0)
		{
		    --i;
		    தொடர்;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		எனில் (subli != வெற்று)
		{
		    col = tv_get_எண்_chk(&subli->li_tv, &error);
		    எனில் (error == TRUE)
			செல் fail;
		    subli = subli->li_next;
		    எனில் (subli != வெற்று)
		    {
			len = tv_get_எண்_chk(&subli->li_tv, &error);
			எனில் (error == TRUE)
			    செல் fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    இல்லையெனில் எனில் (li->li_tv.v_type == VAR_எண்)
	    {
		எனில் (li->li_tv.vval.v_எண் == 0)
		{
		    --i;
		    தொடர்;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_எண்;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    இல்லையெனில்
	    {
		emsg(_(e_list_or_எண்_required));
		செல் fail;
	    }
	    எனில் (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    எனில் (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	எனில் (toplnum != 0)
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
    போது (cur != வெற்று && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    எனில் (cur == prev)
	wp->w_match_head = m;
    இல்லையெனில்
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    திரும்பு id;

fail:
    vim_regfree(regprog);
    எனில் (m != வெற்று)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    திரும்பு -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    நிலையான முழு
match_delete(win_T *wp, முழு id, முழு perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    முழு		rtype = UPD_SOME_VALID;

    எனில் (id < 1)
    {
	எனில் (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	திரும்பு -1;
    }
    போது (cur != வெற்று && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    எனில் (cur == வெற்று)
    {
	எனில் (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	திரும்பு -1;
    }
    எனில் (cur == prev)
	wp->w_match_head = cur->mit_next;
    இல்லையெனில்
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    எனில் (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    திரும்பு 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    வெறுமை
clear_matches(win_T *wp)
{
    matchitem_T *m;

    போது (wp->w_match_head != வெற்று)
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
    நிலையான matchitem_T *
get_match(win_T *wp, முழு id)
{
    matchitem_T *cur = wp->w_match_head;

    போது (cur != வெற்று && cur->mit_id != id)
	cur = cur->mit_next;
    திரும்பு cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    வெறுமை
init_தேடல்_hl(win_T *wp, match_T *தேடல்_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    போது (cur != வெற்று)
    {
	cur->mit_hl.rm = cur->mit_match;
	எனில் (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	இல்லையெனில்
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    தேடல்_hl->attr = HL_ATTR(HLF_L);

    தேடல்_hl->buf = wp->w_buffer;
    தேடல்_hl->lnum = 0;
    தேடல்_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    நிலையான முழு
next_தேடல்_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    முழு	    i;
    முழு	    found = -1;

    க்கு (i = match->mit_pos_cur; i < match->mit_pos_மொத்தம்; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	எனில் (pos->lnum == 0)
	    நிறுத்து;
	எனில் (pos->len == 0 && pos->col < mincol)
	    தொடர்;
	எனில் (pos->lnum == lnum)
	{
	    எனில் (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		எனில் (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    இல்லையெனில்
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    எனில் (found >= 0)
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
	திரும்பு 1;
    }
    திரும்பு 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    நிலையான வெறுமை
next_தேடல்_hl(
    win_T	    *win,
    match_T	    *தேடல்_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    முழு		called_emsg_before = called_emsg;
    முழு		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    எனில் ((lnum < தேடல்_first_வரி || lnum > தேடல்_last_வரி) && cur == வெற்று)
    {
	shl->lnum = 0;
	திரும்பு;
    }

    எனில் (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	எனில் (lnum > l)
	    shl->lnum = 0;
	இல்லையெனில் எனில் (lnum < l || shl->rm.endpos[0].col > mincol)
	    திரும்பு;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    க்கு (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	எனில் (shl->lnum == 0)
	    matchcol = 0;
	இல்லையெனில் எனில் (vim_strchr(p_cpo, CPO_தேடல்) == வெற்று
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    எனில் (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		நிறுத்து;
	    }
	    எனில் (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    இல்லையெனில்
		++matchcol;
	}
	இல்லையெனில்
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	எனில் (shl->rm.regprog != வெற்று)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    முழு regprog_is_copy = (shl != தேடல்_hl && cur != வெற்று
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    எனில் (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    எனில் (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		எனில் (shl == தேடல்_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = வெற்று;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (cur != வெற்று)
	    nmatched = next_தேடல்_hl_pos(shl, lnum, cur, matchcol);
	இல்லையெனில்
	    nmatched = 0;
	எனில் (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    நிறுத்து;
	}
	எனில் (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    நிறுத்து;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    வெறுமை
prepare_தேடல்_hl(win_T *wp, match_T *தேடல்_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    முழு		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    முழு		pos_inprogress;	// marks that position match search is
				// in progress
    முழு		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    போது (cur != வெற்று || shl_flag == FALSE)
    {
	எனில் (shl_flag == FALSE)
	{
	    shl = தேடல்_hl;
	    shl_flag = TRUE;
	}
	இல்லையெனில்
	    shl = &cur->mit_hl;
	எனில் (shl->rm.regprog != வெற்று
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    எனில் (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		க்கு (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    எனில் (hasFoldingWin(wp, shl->first_lnum - 1,
						      வெற்று, வெற்று, TRUE, வெற்று))
			நிறுத்து;
# இல்லையெனில்
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    எனில் (cur != வெற்று)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    போது (shl->first_lnum < lnum && (shl->rm.regprog != வெற்று
					  || (cur != வெற்று && pos_inprogress)))
	    {
		next_தேடல்_hl(wp, தேடல்_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == தேடல்_hl ? வெற்று : cur);
		pos_inprogress = cur == வெற்று || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		எனில் (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		இல்லையெனில்
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	எனில் (shl != தேடல்_hl && cur != வெற்று)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    நிலையான வெறுமை
check_cur_தேடல்_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    எனில் (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    இல்லையெனில்
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    முழு
prepare_தேடல்_hl_வரி(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **வரி,
	match_T	    *தேடல்_hl,
	முழு	    *தேடல்_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    முழு		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    முழு		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    போது (cur != வெற்று || shl_flag == FALSE)
    {
	எனில் (shl_flag == FALSE)
	{
	    shl = தேடல்_hl;
	    shl_flag = TRUE;
	}
	இல்லையெனில்
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	எனில் (cur != வெற்று)
	    cur->mit_pos_cur = 0;
	next_தேடல்_hl(wp, தேடல்_hl, shl, lnum, mincol,
						shl == தேடல்_hl ? வெற்று : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);

	எனில் (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    எனில் (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    இல்லையெனில்
		shl->startcol = 0;
	    எனில் (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    இல்லையெனில்
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    எனில் (shl == தேடல்_hl)
		check_cur_தேடல்_hl(wp, shl);

	    // Highlight one character for an empty match.
	    எனில் (shl->startcol == shl->endcol)
	    {
		எனில் (has_mbyte && (*வரி)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*வரி) + shl->endcol);
		இல்லையெனில்
		    ++shl->endcol;
	    }
	    எனில் ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*தேடல்_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	எனில் (shl != தேடல்_hl && cur != வெற்று)
	    cur = cur->mit_next;
    }
    திரும்பு area_highlighting;
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
    முழு
update_தேடல்_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **வரி,
	match_T	    *தேடல்_hl,
	முழு	    *has_match_conc UNUSED,
	முழு	    *match_conc UNUSED,
	முழு	    did_வரி_attr,
	முழு	    lcs_eol_one,
	முழு	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    முழு		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    முழு		pos_inprogress;	    // marks that position match search is in
				    // progress
    முழு		தேடல்_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    போது (cur != வெற்று || shl_flag == FALSE)
    {
	எனில் (shl_flag == FALSE
		&& (cur == வெற்று
			|| cur->mit_priority > தேடல்_HL_PRIORITY))
	{
	    shl = தேடல்_hl;
	    shl_flag = TRUE;
	}
	இல்லையெனில்
	    shl = &cur->mit_hl;
	எனில் (cur != வெற்று)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	போது (shl->rm.regprog != வெற்று || (cur != வெற்று && pos_inprogress))
	{
	    எனில் (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		முழு next_col = col + mb_ptr2len(*வரி + col);

		எனில் (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		எனில் (cur != வெற்று
			&& shl != தேடல்_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		இல்லையெனில்
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		எனில் (shl == தேடல்_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    எனில் (shl->attr_cur != shl->attr)
			தேடல்_hl_has_cursor_lnum = lnum;
		}

	    }
	    இல்லையெனில் எனில் (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_தேடல்_hl(wp, தேடல்_hl, shl, lnum, col,
					       shl == தேடல்_hl ? வெற்று : cur);
		pos_inprogress = !(cur == வெற்று || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);

		எனில் (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    எனில் (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    இல்லையெனில்
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    எனில் (shl == தேடல்_hl)
			check_cur_தேடல்_hl(wp, shl);

		    எனில் (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			எனில் (has_mbyte)
			{
			    char_u *p = *வரி + shl->endcol;

			    எனில் (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    இல்லையெனில்
				shl->endcol += (*mb_ptr2len)(p);
			}
			இல்லையெனில்
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    தொடர்;
		}
	    }
	    நிறுத்து;
	}
	எனில் (shl != தேடல்_hl && cur != வெற்று)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    போது (cur != வெற்று || shl_flag == FALSE)
    {
	எனில் (shl_flag == FALSE
		&& (cur == வெற்று ||
			cur->mit_priority > தேடல்_HL_PRIORITY))
	{
	    shl = தேடல்_hl;
	    shl_flag = TRUE;
	}
	இல்லையெனில்
	    shl = &cur->mit_hl;
	எனில் (shl->attr_cur != 0)
	{
	    தேடல்_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	எனில் (shl != தேடல்_hl && cur != வெற்று)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    எனில் (*(*வரி + col) == NUL && (did_வரி_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	தேடல்_attr = 0;
    திரும்பு தேடல்_attr;
}

    முழு
get_prevcol_hl_flag(win_T *wp, match_T *தேடல்_hl, long curcol)
{
    long	prevcol = curcol;
    முழு		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# எனில் defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    எனில் (popup_is_popup(wp))
	திரும்பு FALSE;
# endif

    // we're not really at that column when skipping some text
    எனில் ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    எனில் (!தேடல்_hl->is_addpos && (prevcol == (long)தேடல்_hl->startcol
		|| (prevcol > (long)தேடல்_hl->startcol
					      && தேடல்_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    இல்லையெனில்
    {
	cur = wp->w_match_head;
	போது (cur != வெற்று)
	{
	    எனில் (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		நிறுத்து;
	    }
	    cur = cur->mit_next;
	}
    }
    திரும்பு prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    வெறுமை
get_தேடல்_match_hl(win_T *wp, match_T *தேடல்_hl, long col, முழு *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    முழு		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    போது (cur != வெற்று || shl_flag == FALSE)
    {
	எனில் (shl_flag == FALSE
		&& ((cur != வெற்று
			&& cur->mit_priority > தேடல்_HL_PRIORITY)
		    || cur == வெற்று))
	{
	    shl = தேடல்_hl;
	    shl_flag = TRUE;
	}
	இல்லையெனில்
	    shl = &cur->mit_hl;
	எனில் (col - 1 == (long)shl->startcol
		&& (shl == தேடல்_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	எனில் (shl != தேடல்_hl && cur != வெற்று)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#எனில் defined(FEAT_EVAL)
# ifdef FEAT_தேடல்_EXTRA
    நிலையான முழு
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    எனில் (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	திரும்பு FAIL;
    }

    எனில் (dict_has_சாவி(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    எனில் ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == வெற்று)
	திரும்பு OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    எனில் (*win == வெற்று)
    {
	emsg(_(e_invalid_window_எண்));
	திரும்பு FAIL;
    }

    திரும்பு OK;
}
# endif

/*
 * "clearmatches()" function
 */
    வெறுமை
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    win_T   *win;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    win = get_optional_window(argvars, 0);
    எனில் (win != வெற்று)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    வெறுமை
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    முழு		i;
    win_T	*win;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    win = get_optional_window(argvars, 0);
    எனில் (rettv_list_alloc(rettv) == FAIL || win == வெற்று)
	திரும்பு;

    cur = win->w_match_head;
    போது (cur != வெற்று)
    {
	dict = dict_alloc();
	எனில் (dict == வெற்று)
	    திரும்பு;
	எனில் (cur->mit_match.regprog == வெற்று)
	{
	    // match added with matchaddpos()
	    க்கு (i = 0; i < cur->mit_pos_மொத்தம்; ++i)
	    {
		llpos_T	*llpos;
		எழுத்து	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		எனில் (llpos->lnum == 0)
		    நிறுத்து;
		l = list_alloc();
		எனில் (l == வெற்று)
		    நிறுத்து;
		list_append_எண்(l, (varnumber_T)llpos->lnum);
		எனில் (llpos->col > 0)
		{
		    list_append_எண்(l, (varnumber_T)llpos->col);
		    list_append_எண்(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		எனில் (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	இல்லையெனில்
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_எண்(dict, "priority", (long)cur->mit_priority);
	dict_add_எண்(dict, "id", (long)cur->mit_id);
#  எனில் defined(FEAT_CONCEAL)
	எனில் (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    முழு	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	எனில் (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    வெறுமை
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = வெற்று;
    win_T	*win;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;
    win = get_optional_window(argvars, 1);
    எனில் (win == வெற்று)
	திரும்பு;

    எனில் ((l = argvars[0].vval.v_list) != வெற்று)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	போது (li != வெற்று)
	{
	    எனில் (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == வெற்று)
	    {
		emsg(_(e_invalid_argument));
		திரும்பு;
	    }
	    எனில் (!(dict_has_சாவி(d, "group")
			&& (dict_has_சாவி(d, "pattern")
			    || dict_has_சாவி(d, "pos1"))
			&& dict_has_சாவி(d, "priority")
			&& dict_has_சாவி(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		திரும்பு;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	போது (li != வெற்று)
	{
	    முழு		i = 0;
	    எழுத்து	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    முழு		priority;
	    முழு		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    எனில் (!dict_has_சாவி(d, "pattern"))
	    {
		எனில் (s == வெற்று)
		{
		    s = list_alloc();
		    எனில் (s == வெற்று)
			திரும்பு;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		க்கு (i = 1; i < 9; i++)
		{
		    sprintf((எழுத்து *)buf, (எழுத்து *)"pos%d", i);
		    எனில் ((di = dict_find(d, (char_u *)buf, -1)) != வெற்று)
		    {
			எனில் (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    திரும்பு;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    இல்லையெனில்
			நிறுத்து;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (முழு)dict_get_எண்(d, "priority");
	    id = (முழு)dict_get_எண்(d, "id");
	    conceal = dict_has_சாவி(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : வெற்று;
	    எனில் (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, வெற்று, conceal);
	    }
	    இல்லையெனில்
	    {
		match_add(win, group, வெற்று, priority, id, s, conceal);
		list_unref(s);
		s = வெற்று;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_எண் = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    வெறுமை
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    முழு		prio = 10;	// default priority
    முழு		id = -1;
    முழு		error = FALSE;
    char_u	*conceal_char = வெற்று;
    win_T	*win = curwin;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_எண்_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	திரும்பு;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    எனில் (grp == வெற்று || pat == வெற்று)
	திரும்பு;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (முழு)tv_get_எண்_chk(&argvars[3], &error);
	    எனில் (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		திரும்பு;
	}
    }
    எனில் (error == TRUE)
	திரும்பு;
    எனில் (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	திரும்பு;
    }

    rettv->vval.v_எண் = match_add(win, grp, pat, prio, id, வெற்று,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    வெறுமை
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    முழு		prio = 10;
    முழு		id = -1;
    முழு		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = வெற்று;
    win_T	*win = curwin;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_எண்_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	திரும்பு;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    எனில் (group == வெற்று)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	திரும்பு;
    }
    l = argvars[1].vval.v_list;
    எனில் (l == வெற்று || l->lv_len == 0)
	திரும்பு;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (முழு)tv_get_எண்_chk(&argvars[3], &error);

	    எனில் (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		திரும்பு;
	}
    }
    எனில் (error == TRUE)
	திரும்பு;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    எனில் (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	திரும்பு;
    }

    rettv->vval.v_எண் = match_add(win, group, வெற்று, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    வெறுமை
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) != OK)
	திரும்பு;

# ifdef FEAT_தேடல்_EXTRA
    முழு	    id;
    matchitem_T *m;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(&argvars[0]);
    எனில் (id >= 1 && id <= 3)
    {
	எனில் ((m = get_match(curwin, id)) != வெற்று)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	இல்லையெனில்
	{
	    list_append_string(rettv->vval.v_list, வெற்று, -1);
	    list_append_string(rettv->vval.v_list, வெற்று, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    வெறுமை
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_தேடல்_EXTRA
    win_T   *win;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    win = get_optional_window(argvars, 1);
    எனில் (win == வெற்று)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = match_delete(win,
				       (முழு)tv_get_எண்(&argvars[0]), TRUE);
# endif
}
#endif

#எனில் defined(FEAT_தேடல்_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    வெறுமை
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = வெற்று;
    char_u	*end;
    முழு		c;
    முழு		id;

    எனில் (eap->line2 <= 3)
	id = eap->line2;
    இல்லையெனில்
    {
	emsg(_(e_invalid_command));
	திரும்பு;
    }

    // First clear any old pattern.
    எனில் (!eap->skip)
	match_delete(curwin, id, FALSE);

    எனில் (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    இல்லையெனில் எனில் ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    இல்லையெனில்
    {
	p = skiptowhite(eap->arg);
	எனில் (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	எனில் (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    திரும்பு;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	எனில் (!eap->skip)
	{
	    எனில் (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		திரும்பு;
	    }
	    எனில் (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		திரும்பு;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, வெற்று, வெற்று);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
