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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_अन्वेषणम्_EXTRA)

# define अन्वेषणम्_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    स्थिर पूर्णाङ्क
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    पूर्णाङ्क		prio,
    पूर्णाङ्क		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = रिक्त;
    पूर्णाङ्क		hlg_id;
    regprog_T	*regprog = रिक्त;
    पूर्णाङ्क		rtype = UPD_SOME_VALID;

    सचे (*grp == NUL || (pat != रिक्त && *pat == NUL))
	पटिनिवत्त -1;
    सचे (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	पटिनिवत्त -1;
    }
    सचे (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    अञ्ञथा
    {
	// check the given ID is not already in use
	अत्थाय (cur = wp->w_match_head; cur != रिक्त; cur = cur->mit_next)
	    सचे (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		पटिनिवत्त -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	सचे (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    सचे ((hlg_id = syn_namen2id(grp, (पूर्णाङ्क)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	पटिनिवत्त -1;
    }
    सचे (pat != रिक्त && (regprog = vim_regcomp(pat, RE_MAGIC)) == रिक्त)
    {
	semsg(_(e_invalid_argument_str), pat);
	पटिनिवत्त -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    सचे (m == रिक्त)
	गच्छ fail;
    सचे (pos_list != रिक्त && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	सचे (m->mit_pos_array == रिक्त)
	    गच्छ fail;
	m->mit_pos_गणनम् = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == रिक्त ? रिक्त : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# सचे defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    सचे (conceal_char != रिक्त)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    सचे (pos_list != रिक्त)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	पूर्णाङ्क		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	अत्थाय (i = 0, li = pos_list->lv_first; li != रिक्त; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    पूर्णाङ्क		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    पूर्णाङ्क		error = FALSE;

	    सचे (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		सचे (subl == रिक्त)
		    गच्छ fail;
		subli = subl->lv_first;
		सचे (subli == रिक्त)
		    गच्छ fail;
		lnum = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
		सचे (error == TRUE)
		    गच्छ fail;
		सचे (lnum == 0)
		{
		    --i;
		    अनुवर्त;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		सचे (subli != रिक्त)
		{
		    col = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
		    सचे (error == TRUE)
			गच्छ fail;
		    subli = subli->li_next;
		    सचे (subli != रिक्त)
		    {
			len = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
			सचे (error == TRUE)
			    गच्छ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    अञ्ञथा सचे (li->li_tv.v_type == VAR_सङ्ख्या)
	    {
		सचे (li->li_tv.vval.v_सङ्ख्या == 0)
		{
		    --i;
		    अनुवर्त;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_सङ्ख्या;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    अञ्ञथा
	    {
		emsg(_(e_list_or_सङ्ख्या_required));
		गच्छ fail;
	    }
	    सचे (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    सचे (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	सचे (toplnum != 0)
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
    याव (cur != रिक्त && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    सचे (cur == prev)
	wp->w_match_head = m;
    अञ्ञथा
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    पटिनिवत्त id;

fail:
    vim_regfree(regprog);
    सचे (m != रिक्त)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    पटिनिवत्त -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    स्थिर पूर्णाङ्क
match_delete(win_T *wp, पूर्णाङ्क id, पूर्णाङ्क perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    पूर्णाङ्क		rtype = UPD_SOME_VALID;

    सचे (id < 1)
    {
	सचे (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	पटिनिवत्त -1;
    }
    याव (cur != रिक्त && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    सचे (cur == रिक्त)
    {
	सचे (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	पटिनिवत्त -1;
    }
    सचे (cur == prev)
	wp->w_match_head = cur->mit_next;
    अञ्ञथा
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    सचे (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    पटिनिवत्त 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    शून्य
clear_matches(win_T *wp)
{
    matchitem_T *m;

    याव (wp->w_match_head != रिक्त)
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
    स्थिर matchitem_T *
get_match(win_T *wp, पूर्णाङ्क id)
{
    matchitem_T *cur = wp->w_match_head;

    याव (cur != रिक्त && cur->mit_id != id)
	cur = cur->mit_next;
    पटिनिवत्त cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    शून्य
init_अन्वेषणम्_hl(win_T *wp, match_T *अन्वेषणम्_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    याव (cur != रिक्त)
    {
	cur->mit_hl.rm = cur->mit_match;
	सचे (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	अञ्ञथा
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    अन्वेषणम्_hl->attr = HL_ATTR(HLF_L);

    अन्वेषणम्_hl->buf = wp->w_buffer;
    अन्वेषणम्_hl->lnum = 0;
    अन्वेषणम्_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    स्थिर पूर्णाङ्क
next_अन्वेषणम्_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    found = -1;

    अत्थाय (i = match->mit_pos_cur; i < match->mit_pos_गणनम्; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	सचे (pos->lnum == 0)
	    भङ्ग;
	सचे (pos->len == 0 && pos->col < mincol)
	    अनुवर्त;
	सचे (pos->lnum == lnum)
	{
	    सचे (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		सचे (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    अञ्ञथा
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    सचे (found >= 0)
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
	पटिनिवत्त 1;
    }
    पटिनिवत्त 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    स्थिर शून्य
next_अन्वेषणम्_hl(
    win_T	    *win,
    match_T	    *अन्वेषणम्_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    सचे ((lnum < अन्वेषणम्_first_पङ्क्तिः || lnum > अन्वेषणम्_last_पङ्क्तिः) && cur == रिक्त)
    {
	shl->lnum = 0;
	पटिनिवत्त;
    }

    सचे (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	सचे (lnum > l)
	    shl->lnum = 0;
	अञ्ञथा सचे (lnum < l || shl->rm.endpos[0].col > mincol)
	    पटिनिवत्त;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    अत्थाय (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	सचे (shl->lnum == 0)
	    matchcol = 0;
	अञ्ञथा सचे (vim_strchr(p_cpo, CPO_अन्वेषणम्) == रिक्त
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    सचे (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		भङ्ग;
	    }
	    सचे (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    अञ्ञथा
		++matchcol;
	}
	अञ्ञथा
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	सचे (shl->rm.regprog != रिक्त)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    पूर्णाङ्क regprog_is_copy = (shl != अन्वेषणम्_hl && cur != रिक्त
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    सचे (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    सचे (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		सचे (shl == अन्वेषणम्_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = रिक्त;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		भङ्ग;
	    }
	}
	अञ्ञथा सचे (cur != रिक्त)
	    nmatched = next_अन्वेषणम्_hl_pos(shl, lnum, cur, matchcol);
	अञ्ञथा
	    nmatched = 0;
	सचे (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    भङ्ग;
	}
	सचे (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    भङ्ग;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    शून्य
prepare_अन्वेषणम्_hl(win_T *wp, match_T *अन्वेषणम्_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    पूर्णाङ्क		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    पूर्णाङ्क		pos_inprogress;	// marks that position match search is
				// in progress
    पूर्णाङ्क		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    याव (cur != रिक्त || shl_flag == FALSE)
    {
	सचे (shl_flag == FALSE)
	{
	    shl = अन्वेषणम्_hl;
	    shl_flag = TRUE;
	}
	अञ्ञथा
	    shl = &cur->mit_hl;
	सचे (shl->rm.regprog != रिक्त
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    सचे (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		अत्थाय (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    सचे (hasFoldingWin(wp, shl->first_lnum - 1,
						      रिक्त, रिक्त, TRUE, रिक्त))
			भङ्ग;
# अञ्ञथा
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    सचे (cur != रिक्त)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    याव (shl->first_lnum < lnum && (shl->rm.regprog != रिक्त
					  || (cur != रिक्त && pos_inprogress)))
	    {
		next_अन्वेषणम्_hl(wp, अन्वेषणम्_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == अन्वेषणम्_hl ? रिक्त : cur);
		pos_inprogress = cur == रिक्त || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		सचे (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		अञ्ञथा
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	सचे (shl != अन्वेषणम्_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    स्थिर शून्य
check_cur_अन्वेषणम्_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    सचे (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    अञ्ञथा
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    पूर्णाङ्क
prepare_अन्वेषणम्_hl_पङ्क्तिः(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **पङ्क्तिः,
	match_T	    *अन्वेषणम्_hl,
	पूर्णाङ्क	    *अन्वेषणम्_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    पूर्णाङ्क		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    पूर्णाङ्क		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    याव (cur != रिक्त || shl_flag == FALSE)
    {
	सचे (shl_flag == FALSE)
	{
	    shl = अन्वेषणम्_hl;
	    shl_flag = TRUE;
	}
	अञ्ञथा
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	सचे (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	next_अन्वेषणम्_hl(wp, अन्वेषणम्_hl, shl, lnum, mincol,
						shl == अन्वेषणम्_hl ? रिक्त : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum, FALSE);

	सचे (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    सचे (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    अञ्ञथा
		shl->startcol = 0;
	    सचे (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    अञ्ञथा
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    सचे (shl == अन्वेषणम्_hl)
		check_cur_अन्वेषणम्_hl(wp, shl);

	    // Highlight one character for an empty match.
	    सचे (shl->startcol == shl->endcol)
	    {
		सचे (has_mbyte && (*पङ्क्तिः)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*पङ्क्तिः) + shl->endcol);
		अञ्ञथा
		    ++shl->endcol;
	    }
	    सचे ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*अन्वेषणम्_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	सचे (shl != अन्वेषणम्_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    पटिनिवत्त area_highlighting;
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
    पूर्णाङ्क
update_अन्वेषणम्_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **पङ्क्तिः,
	match_T	    *अन्वेषणम्_hl,
	पूर्णाङ्क	    *has_match_conc UNUSED,
	पूर्णाङ्क	    *match_conc UNUSED,
	पूर्णाङ्क	    did_पङ्क्तिः_attr,
	पूर्णाङ्क	    lcs_eol_one,
	पूर्णाङ्क	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    पूर्णाङ्क		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    पूर्णाङ्क		pos_inprogress;	    // marks that position match search is in
				    // progress
    पूर्णाङ्क		अन्वेषणम्_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    याव (cur != रिक्त || shl_flag == FALSE)
    {
	सचे (shl_flag == FALSE
		&& (cur == रिक्त
			|| cur->mit_priority > अन्वेषणम्_HL_PRIORITY))
	{
	    shl = अन्वेषणम्_hl;
	    shl_flag = TRUE;
	}
	अञ्ञथा
	    shl = &cur->mit_hl;
	सचे (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	याव (shl->rm.regprog != रिक्त || (cur != रिक्त && pos_inprogress))
	{
	    सचे (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		पूर्णाङ्क next_col = col + mb_ptr2len(*पङ्क्तिः + col);

		सचे (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		सचे (cur != रिक्त
			&& shl != अन्वेषणम्_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		अञ्ञथा
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		सचे (shl == अन्वेषणम्_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    सचे (shl->attr_cur != shl->attr)
			अन्वेषणम्_hl_has_cursor_lnum = lnum;
		}

	    }
	    अञ्ञथा सचे (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_अन्वेषणम्_hl(wp, अन्वेषणम्_hl, shl, lnum, col,
					       shl == अन्वेषणम्_hl ? रिक्त : cur);
		pos_inprogress = !(cur == रिक्त || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum, FALSE);

		सचे (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    सचे (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    अञ्ञथा
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    सचे (shl == अन्वेषणम्_hl)
			check_cur_अन्वेषणम्_hl(wp, shl);

		    सचे (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			सचे (has_mbyte)
			{
			    char_u *p = *पङ्क्तिः + shl->endcol;

			    सचे (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    अञ्ञथा
				shl->endcol += (*mb_ptr2len)(p);
			}
			अञ्ञथा
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    अनुवर्त;
		}
	    }
	    भङ्ग;
	}
	सचे (shl != अन्वेषणम्_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    याव (cur != रिक्त || shl_flag == FALSE)
    {
	सचे (shl_flag == FALSE
		&& (cur == रिक्त ||
			cur->mit_priority > अन्वेषणम्_HL_PRIORITY))
	{
	    shl = अन्वेषणम्_hl;
	    shl_flag = TRUE;
	}
	अञ्ञथा
	    shl = &cur->mit_hl;
	सचे (shl->attr_cur != 0)
	{
	    अन्वेषणम्_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	सचे (shl != अन्वेषणम्_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    सचे (*(*पङ्क्तिः + col) == NUL && (did_पङ्क्तिः_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	अन्वेषणम्_attr = 0;
    पटिनिवत्त अन्वेषणम्_attr;
}

    पूर्णाङ्क
get_prevcol_hl_flag(win_T *wp, match_T *अन्वेषणम्_hl, long curcol)
{
    long	prevcol = curcol;
    पूर्णाङ्क		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# सचे defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    सचे (popup_is_popup(wp))
	पटिनिवत्त FALSE;
# endif

    // we're not really at that column when skipping some text
    सचे ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    सचे (!अन्वेषणम्_hl->is_addpos && (prevcol == (long)अन्वेषणम्_hl->startcol
		|| (prevcol > (long)अन्वेषणम्_hl->startcol
					      && अन्वेषणम्_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    अञ्ञथा
    {
	cur = wp->w_match_head;
	याव (cur != रिक्त)
	{
	    सचे (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		भङ्ग;
	    }
	    cur = cur->mit_next;
	}
    }
    पटिनिवत्त prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    शून्य
get_अन्वेषणम्_match_hl(win_T *wp, match_T *अन्वेषणम्_hl, long col, पूर्णाङ्क *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    पूर्णाङ्क		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    याव (cur != रिक्त || shl_flag == FALSE)
    {
	सचे (shl_flag == FALSE
		&& ((cur != रिक्त
			&& cur->mit_priority > अन्वेषणम्_HL_PRIORITY)
		    || cur == रिक्त))
	{
	    shl = अन्वेषणम्_hl;
	    shl_flag = TRUE;
	}
	अञ्ञथा
	    shl = &cur->mit_hl;
	सचे (col - 1 == (long)shl->startcol
		&& (shl == अन्वेषणम्_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	सचे (shl != अन्वेषणम्_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#सचे defined(FEAT_EVAL)
# ifdef FEAT_अन्वेषणम्_EXTRA
    स्थिर पूर्णाङ्क
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    सचे (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	पटिनिवत्त FAIL;
    }

    सचे (dict_has_कुञ्चिका(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    सचे ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == रिक्त)
	पटिनिवत्त OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    सचे (*win == रिक्त)
    {
	emsg(_(e_invalid_window_सङ्ख्या));
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
}
# endif

/*
 * "clearmatches()" function
 */
    शून्य
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    win_T   *win;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    win = get_optional_window(argvars, 0);
    सचे (win != रिक्त)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    शून्य
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    पूर्णाङ्क		i;
    win_T	*win;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    win = get_optional_window(argvars, 0);
    सचे (rettv_list_alloc(rettv) == FAIL || win == रिक्त)
	पटिनिवत्त;

    cur = win->w_match_head;
    याव (cur != रिक्त)
    {
	dict = dict_alloc();
	सचे (dict == रिक्त)
	    पटिनिवत्त;
	सचे (cur->mit_match.regprog == रिक्त)
	{
	    // match added with matchaddpos()
	    अत्थाय (i = 0; i < cur->mit_pos_गणनम्; ++i)
	    {
		llpos_T	*llpos;
		वर्ण	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		सचे (llpos->lnum == 0)
		    भङ्ग;
		l = list_alloc();
		सचे (l == रिक्त)
		    भङ्ग;
		list_append_सङ्ख्या(l, (varnumber_T)llpos->lnum);
		सचे (llpos->col > 0)
		{
		    list_append_सङ्ख्या(l, (varnumber_T)llpos->col);
		    list_append_सङ्ख्या(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		सचे (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	अञ्ञथा
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_सङ्ख्या(dict, "priority", (long)cur->mit_priority);
	dict_add_सङ्ख्या(dict, "id", (long)cur->mit_id);
#  सचे defined(FEAT_CONCEAL)
	सचे (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    पूर्णाङ्क	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	सचे (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    शून्य
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = रिक्त;
    win_T	*win;

    rettv->vval.v_सङ्ख्या = -1;

    सचे (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (check_for_list_arg(argvars, 0) == FAIL)
	पटिनिवत्त;
    win = get_optional_window(argvars, 1);
    सचे (win == रिक्त)
	पटिनिवत्त;

    सचे ((l = argvars[0].vval.v_list) != रिक्त)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	याव (li != रिक्त)
	{
	    सचे (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == रिक्त)
	    {
		emsg(_(e_invalid_argument));
		पटिनिवत्त;
	    }
	    सचे (!(dict_has_कुञ्चिका(d, "group")
			&& (dict_has_कुञ्चिका(d, "pattern")
			    || dict_has_कुञ्चिका(d, "pos1"))
			&& dict_has_कुञ्चिका(d, "priority")
			&& dict_has_कुञ्चिका(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		पटिनिवत्त;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	याव (li != रिक्त)
	{
	    पूर्णाङ्क		i = 0;
	    वर्ण	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    पूर्णाङ्क		priority;
	    पूर्णाङ्क		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    सचे (!dict_has_कुञ्चिका(d, "pattern"))
	    {
		सचे (s == रिक्त)
		{
		    s = list_alloc();
		    सचे (s == रिक्त)
			पटिनिवत्त;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		अत्थाय (i = 1; i < 9; i++)
		{
		    sprintf((वर्ण *)buf, (वर्ण *)"pos%d", i);
		    सचे ((di = dict_find(d, (char_u *)buf, -1)) != रिक्त)
		    {
			सचे (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    पटिनिवत्त;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    अञ्ञथा
			भङ्ग;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "priority");
	    id = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "id");
	    conceal = dict_has_कुञ्चिका(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : रिक्त;
	    सचे (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, रिक्त, conceal);
	    }
	    अञ्ञथा
	    {
		match_add(win, group, रिक्त, priority, id, s, conceal);
		list_unref(s);
		s = रिक्त;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_सङ्ख्या = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    शून्य
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    पूर्णाङ्क		prio = 10;	// default priority
    पूर्णाङ्क		id = -1;
    पूर्णाङ्क		error = FALSE;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_सङ्ख्या = -1;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	पटिनिवत्त;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    सचे (grp == रिक्त || pat == रिक्त)
	पटिनिवत्त;
    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	सचे (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[3], &error);
	    सचे (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		पटिनिवत्त;
	}
    }
    सचे (error == TRUE)
	पटिनिवत्त;
    सचे (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	पटिनिवत्त;
    }

    rettv->vval.v_सङ्ख्या = match_add(win, grp, pat, prio, id, रिक्त,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    शून्य
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    पूर्णाङ्क		prio = 10;
    पूर्णाङ्क		id = -1;
    पूर्णाङ्क		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_सङ्ख्या = -1;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	पटिनिवत्त;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    सचे (group == रिक्त)
	पटिनिवत्त;

    सचे (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	पटिनिवत्त;
    }
    l = argvars[1].vval.v_list;
    सचे (l == रिक्त || l->lv_len == 0)
	पटिनिवत्त;

    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	सचे (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[3], &error);

	    सचे (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		पटिनिवत्त;
	}
    }
    सचे (error == TRUE)
	पटिनिवत्त;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    सचे (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	पटिनिवत्त;
    }

    rettv->vval.v_सङ्ख्या = match_add(win, group, रिक्त, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    शून्य
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    सचे (rettv_list_alloc(rettv) != OK)
	पटिनिवत्त;

# ifdef FEAT_अन्वेषणम्_EXTRA
    पूर्णाङ्क	    id;
    matchitem_T *m;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    id = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]);
    सचे (id >= 1 && id <= 3)
    {
	सचे ((m = get_match(curwin, id)) != रिक्त)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	अञ्ञथा
	{
	    list_append_string(rettv->vval.v_list, रिक्त, -1);
	    list_append_string(rettv->vval.v_list, रिक्त, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    शून्य
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_अन्वेषणम्_EXTRA
    win_T   *win;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    win = get_optional_window(argvars, 1);
    सचे (win == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = match_delete(win,
				       (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]), TRUE);
# endif
}
#endif

#सचे defined(FEAT_अन्वेषणम्_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    शून्य
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = रिक्त;
    char_u	*end;
    पूर्णाङ्क		c;
    पूर्णाङ्क		id;

    सचे (eap->line2 <= 3)
	id = eap->line2;
    अञ्ञथा
    {
	emsg(_(e_invalid_command));
	पटिनिवत्त;
    }

    // First clear any old pattern.
    सचे (!eap->skip)
	match_delete(curwin, id, FALSE);

    सचे (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    अञ्ञथा सचे ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    अञ्ञथा
    {
	p = skiptowhite(eap->arg);
	सचे (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	सचे (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    पटिनिवत्त;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	सचे (!eap->skip)
	{
	    सचे (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		पटिनिवत्त;
	    }
	    सचे (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		पटिनिवत्त;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, रिक्त, रिक्त);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
