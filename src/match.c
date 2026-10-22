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

#जर defined(FEAT_शोध_EXTRA)

# define शोध_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    स्थिर पूर्णांक
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    पूर्णांक		prio,
    पूर्णांक		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = रिक्त;
    पूर्णांक		hlg_id;
    regprog_T	*regprog = रिक्त;
    पूर्णांक		rtype = UPD_SOME_VALID;

    जर (*grp == NUL || (pat != रिक्त && *pat == NUL))
	परत -1;
    जर (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	परत -1;
    }
    जर (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    अन्यथा
    {
	// check the given ID is not already in use
	साठी (cur = wp->w_match_head; cur != रिक्त; cur = cur->mit_next)
	    जर (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		परत -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	जर (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    जर ((hlg_id = syn_namen2id(grp, (पूर्णांक)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	परत -1;
    }
    जर (pat != रिक्त && (regprog = vim_regcomp(pat, RE_MAGIC)) == रिक्त)
    {
	semsg(_(e_invalid_argument_str), pat);
	परत -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    जर (m == रिक्त)
	जा fail;
    जर (pos_list != रिक्त && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	जर (m->mit_pos_array == रिक्त)
	    जा fail;
	m->mit_pos_मोजणी = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == रिक्त ? रिक्त : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# जर defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    जर (conceal_char != रिक्त)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    जर (pos_list != रिक्त)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	पूर्णांक		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	साठी (i = 0, li = pos_list->lv_first; li != रिक्त; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    पूर्णांक		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    पूर्णांक		error = FALSE;

	    जर (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		जर (subl == रिक्त)
		    जा fail;
		subli = subl->lv_first;
		जर (subli == रिक्त)
		    जा fail;
		lnum = tv_get_संख्या_chk(&subli->li_tv, &error);
		जर (error == TRUE)
		    जा fail;
		जर (lnum == 0)
		{
		    --i;
		    सुरूठेवा;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		जर (subli != रिक्त)
		{
		    col = tv_get_संख्या_chk(&subli->li_tv, &error);
		    जर (error == TRUE)
			जा fail;
		    subli = subli->li_next;
		    जर (subli != रिक्त)
		    {
			len = tv_get_संख्या_chk(&subli->li_tv, &error);
			जर (error == TRUE)
			    जा fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    अन्यथा जर (li->li_tv.v_type == VAR_संख्या)
	    {
		जर (li->li_tv.vval.v_संख्या == 0)
		{
		    --i;
		    सुरूठेवा;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_संख्या;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    अन्यथा
	    {
		emsg(_(e_list_or_संख्या_required));
		जा fail;
	    }
	    जर (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    जर (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	जर (toplnum != 0)
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
    पर्यंत (cur != रिक्त && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    जर (cur == prev)
	wp->w_match_head = m;
    अन्यथा
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    परत id;

fail:
    vim_regfree(regprog);
    जर (m != रिक्त)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    परत -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    स्थिर पूर्णांक
match_delete(win_T *wp, पूर्णांक id, पूर्णांक perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    पूर्णांक		rtype = UPD_SOME_VALID;

    जर (id < 1)
    {
	जर (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	परत -1;
    }
    पर्यंत (cur != रिक्त && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    जर (cur == रिक्त)
    {
	जर (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	परत -1;
    }
    जर (cur == prev)
	wp->w_match_head = cur->mit_next;
    अन्यथा
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    जर (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    परत 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    शून्य
clear_matches(win_T *wp)
{
    matchitem_T *m;

    पर्यंत (wp->w_match_head != रिक्त)
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
get_match(win_T *wp, पूर्णांक id)
{
    matchitem_T *cur = wp->w_match_head;

    पर्यंत (cur != रिक्त && cur->mit_id != id)
	cur = cur->mit_next;
    परत cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    शून्य
init_शोध_hl(win_T *wp, match_T *शोध_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    पर्यंत (cur != रिक्त)
    {
	cur->mit_hl.rm = cur->mit_match;
	जर (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	अन्यथा
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    शोध_hl->attr = HL_ATTR(HLF_L);

    शोध_hl->buf = wp->w_buffer;
    शोध_hl->lnum = 0;
    शोध_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    स्थिर पूर्णांक
next_शोध_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    पूर्णांक	    i;
    पूर्णांक	    found = -1;

    साठी (i = match->mit_pos_cur; i < match->mit_pos_मोजणी; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	जर (pos->lnum == 0)
	    थांबा;
	जर (pos->len == 0 && pos->col < mincol)
	    सुरूठेवा;
	जर (pos->lnum == lnum)
	{
	    जर (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		जर (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    अन्यथा
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    जर (found >= 0)
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
	परत 1;
    }
    परत 0;
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
next_शोध_hl(
    win_T	    *win,
    match_T	    *शोध_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    दीर्घ	nmatched;
    पूर्णांक		called_emsg_before = called_emsg;
    पूर्णांक		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    जर ((lnum < शोध_first_ओळ || lnum > शोध_last_ओळ) && cur == रिक्त)
    {
	shl->lnum = 0;
	परत;
    }

    जर (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	जर (lnum > l)
	    shl->lnum = 0;
	अन्यथा जर (lnum < l || shl->rm.endpos[0].col > mincol)
	    परत;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    साठी (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	जर (shl->lnum == 0)
	    matchcol = 0;
	अन्यथा जर (vim_strchr(p_cpo, CPO_शोध) == रिक्त
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    जर (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		थांबा;
	    }
	    जर (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    अन्यथा
		++matchcol;
	}
	अन्यथा
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	जर (shl->rm.regprog != रिक्त)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    पूर्णांक regprog_is_copy = (shl != शोध_hl && cur != रिक्त
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    जर (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    जर (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		जर (shl == शोध_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = रिक्त;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		थांबा;
	    }
	}
	अन्यथा जर (cur != रिक्त)
	    nmatched = next_शोध_hl_pos(shl, lnum, cur, matchcol);
	अन्यथा
	    nmatched = 0;
	जर (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    थांबा;
	}
	जर (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    थांबा;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    शून्य
prepare_शोध_hl(win_T *wp, match_T *शोध_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    पूर्णांक		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    पूर्णांक		pos_inprogress;	// marks that position match search is
				// in progress
    पूर्णांक		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    पर्यंत (cur != रिक्त || shl_flag == FALSE)
    {
	जर (shl_flag == FALSE)
	{
	    shl = शोध_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	जर (shl->rm.regprog != रिक्त
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    जर (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		साठी (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    जर (hasFoldingWin(wp, shl->first_lnum - 1,
						      रिक्त, रिक्त, TRUE, रिक्त))
			थांबा;
# अन्यथा
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    जर (cur != रिक्त)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    पर्यंत (shl->first_lnum < lnum && (shl->rm.regprog != रिक्त
					  || (cur != रिक्त && pos_inprogress)))
	    {
		next_शोध_hl(wp, शोध_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == शोध_hl ? रिक्त : cur);
		pos_inprogress = cur == रिक्त || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		जर (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		अन्यथा
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	जर (shl != शोध_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    स्थिर शून्य
check_cur_शोध_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    जर (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    अन्यथा
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    पूर्णांक
prepare_शोध_hl_ओळ(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **ओळ,
	match_T	    *शोध_hl,
	पूर्णांक	    *शोध_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    पूर्णांक		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    पूर्णांक		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    पर्यंत (cur != रिक्त || shl_flag == FALSE)
    {
	जर (shl_flag == FALSE)
	{
	    shl = शोध_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	जर (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	next_शोध_hl(wp, शोध_hl, shl, lnum, mincol,
						shl == शोध_hl ? रिक्त : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);

	जर (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    जर (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    अन्यथा
		shl->startcol = 0;
	    जर (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    अन्यथा
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    जर (shl == शोध_hl)
		check_cur_शोध_hl(wp, shl);

	    // Highlight one character for an empty match.
	    जर (shl->startcol == shl->endcol)
	    {
		जर (has_mbyte && (*ओळ)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*ओळ) + shl->endcol);
		अन्यथा
		    ++shl->endcol;
	    }
	    जर ((दीर्घ)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*शोध_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	जर (shl != शोध_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    परत area_highlighting;
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
    पूर्णांक
update_शोध_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **ओळ,
	match_T	    *शोध_hl,
	पूर्णांक	    *has_match_conc UNUSED,
	पूर्णांक	    *match_conc UNUSED,
	पूर्णांक	    did_ओळ_attr,
	पूर्णांक	    lcs_eol_one,
	पूर्णांक	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    पूर्णांक		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    पूर्णांक		pos_inprogress;	    // marks that position match search is in
				    // progress
    पूर्णांक		शोध_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    पर्यंत (cur != रिक्त || shl_flag == FALSE)
    {
	जर (shl_flag == FALSE
		&& (cur == रिक्त
			|| cur->mit_priority > शोध_HL_PRIORITY))
	{
	    shl = शोध_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	जर (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	पर्यंत (shl->rm.regprog != रिक्त || (cur != रिक्त && pos_inprogress))
	{
	    जर (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		पूर्णांक next_col = col + mb_ptr2len(*ओळ + col);

		जर (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		जर (cur != रिक्त
			&& shl != शोध_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		अन्यथा
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		जर (shl == शोध_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    जर (shl->attr_cur != shl->attr)
			शोध_hl_has_cursor_lnum = lnum;
		}

	    }
	    अन्यथा जर (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_शोध_hl(wp, शोध_hl, shl, lnum, col,
					       shl == शोध_hl ? रिक्त : cur);
		pos_inprogress = !(cur == रिक्त || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);

		जर (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    जर (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    अन्यथा
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    जर (shl == शोध_hl)
			check_cur_शोध_hl(wp, shl);

		    जर (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			जर (has_mbyte)
			{
			    char_u *p = *ओळ + shl->endcol;

			    जर (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    अन्यथा
				shl->endcol += (*mb_ptr2len)(p);
			}
			अन्यथा
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    सुरूठेवा;
		}
	    }
	    थांबा;
	}
	जर (shl != शोध_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    पर्यंत (cur != रिक्त || shl_flag == FALSE)
    {
	जर (shl_flag == FALSE
		&& (cur == रिक्त ||
			cur->mit_priority > शोध_HL_PRIORITY))
	{
	    shl = शोध_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	जर (shl->attr_cur != 0)
	{
	    शोध_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	जर (shl != शोध_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    जर (*(*ओळ + col) == NUL && (did_ओळ_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	शोध_attr = 0;
    परत शोध_attr;
}

    पूर्णांक
get_prevcol_hl_flag(win_T *wp, match_T *शोध_hl, दीर्घ curcol)
{
    दीर्घ	prevcol = curcol;
    पूर्णांक		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# जर defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    जर (popup_is_popup(wp))
	परत FALSE;
# endif

    // we're not really at that column when skipping some text
    जर ((दीर्घ)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    जर (!शोध_hl->is_addpos && (prevcol == (दीर्घ)शोध_hl->startcol
		|| (prevcol > (दीर्घ)शोध_hl->startcol
					      && शोध_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    अन्यथा
    {
	cur = wp->w_match_head;
	पर्यंत (cur != रिक्त)
	{
	    जर (!cur->mit_hl.is_addpos && (prevcol == (दीर्घ)cur->mit_hl.startcol
			|| (prevcol > (दीर्घ)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		थांबा;
	    }
	    cur = cur->mit_next;
	}
    }
    परत prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    शून्य
get_शोध_match_hl(win_T *wp, match_T *शोध_hl, दीर्घ col, पूर्णांक *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    पूर्णांक		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    पर्यंत (cur != रिक्त || shl_flag == FALSE)
    {
	जर (shl_flag == FALSE
		&& ((cur != रिक्त
			&& cur->mit_priority > शोध_HL_PRIORITY)
		    || cur == रिक्त))
	{
	    shl = शोध_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	जर (col - 1 == (दीर्घ)shl->startcol
		&& (shl == शोध_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	जर (shl != शोध_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#जर defined(FEAT_EVAL)
# ifdef FEAT_शोध_EXTRA
    स्थिर पूर्णांक
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    जर (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	परत FAIL;
    }

    जर (dict_has_किल्ली(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    जर ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == रिक्त)
	परत OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    जर (*win == रिक्त)
    {
	emsg(_(e_invalid_window_संख्या));
	परत FAIL;
    }

    परत OK;
}
# endif

/*
 * "clearmatches()" function
 */
    शून्य
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_शोध_EXTRA
    win_T   *win;

    जर (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	परत;

    win = get_optional_window(argvars, 0);
    जर (win != रिक्त)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    शून्य
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_शोध_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    पूर्णांक		i;
    win_T	*win;

    जर (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	परत;

    win = get_optional_window(argvars, 0);
    जर (rettv_list_alloc(rettv) == FAIL || win == रिक्त)
	परत;

    cur = win->w_match_head;
    पर्यंत (cur != रिक्त)
    {
	dict = dict_alloc();
	जर (dict == रिक्त)
	    परत;
	जर (cur->mit_match.regprog == रिक्त)
	{
	    // match added with matchaddpos()
	    साठी (i = 0; i < cur->mit_pos_मोजणी; ++i)
	    {
		llpos_T	*llpos;
		अक्षर	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		जर (llpos->lnum == 0)
		    थांबा;
		l = list_alloc();
		जर (l == रिक्त)
		    थांबा;
		list_append_संख्या(l, (varnumber_T)llpos->lnum);
		जर (llpos->col > 0)
		{
		    list_append_संख्या(l, (varnumber_T)llpos->col);
		    list_append_संख्या(l, (varnumber_T)llpos->len);
		}
		प्र_लिखो(buf, "pos%d", i + 1);
		जर (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	अन्यथा
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_संख्या(dict, "priority", (दीर्घ)cur->mit_priority);
	dict_add_संख्या(dict, "id", (दीर्घ)cur->mit_id);
#  जर defined(FEAT_CONCEAL)
	जर (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    पूर्णांक	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	जर (list_append_dict(rettv->vval.v_list, dict) == FAIL)
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
# ifdef FEAT_शोध_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = रिक्त;
    win_T	*win;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    जर (check_for_list_arg(argvars, 0) == FAIL)
	परत;
    win = get_optional_window(argvars, 1);
    जर (win == रिक्त)
	परत;

    जर ((l = argvars[0].vval.v_list) != रिक्त)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	पर्यंत (li != रिक्त)
	{
	    जर (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == रिक्त)
	    {
		emsg(_(e_invalid_argument));
		परत;
	    }
	    जर (!(dict_has_किल्ली(d, "group")
			&& (dict_has_किल्ली(d, "pattern")
			    || dict_has_किल्ली(d, "pos1"))
			&& dict_has_किल्ली(d, "priority")
			&& dict_has_किल्ली(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		परत;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	पर्यंत (li != रिक्त)
	{
	    पूर्णांक		i = 0;
	    अक्षर	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    पूर्णांक		priority;
	    पूर्णांक		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    जर (!dict_has_किल्ली(d, "pattern"))
	    {
		जर (s == रिक्त)
		{
		    s = list_alloc();
		    जर (s == रिक्त)
			परत;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		साठी (i = 1; i < 9; i++)
		{
		    प्र_लिखो((अक्षर *)buf, (अक्षर *)"pos%d", i);
		    जर ((di = dict_find(d, (char_u *)buf, -1)) != रिक्त)
		    {
			जर (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    परत;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    अन्यथा
			थांबा;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (पूर्णांक)dict_get_संख्या(d, "priority");
	    id = (पूर्णांक)dict_get_संख्या(d, "id");
	    conceal = dict_has_किल्ली(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : रिक्त;
	    जर (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, रिक्त, conceal);
	    }
	    अन्यथा
	    {
		match_add(win, group, रिक्त, priority, id, s, conceal);
		list_unref(s);
		s = रिक्त;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_संख्या = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    शून्य
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_शोध_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    पूर्णांक		prio = 10;	// default priority
    पूर्णांक		id = -1;
    पूर्णांक		error = FALSE;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_संख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	परत;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    जर (grp == रिक्त || pat == रिक्त)
	परत;
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], &error);
	जर (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णांक)tv_get_संख्या_chk(&argvars[3], &error);
	    जर (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		परत;
	}
    }
    जर (error == TRUE)
	परत;
    जर (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	परत;
    }

    rettv->vval.v_संख्या = match_add(win, grp, pat, prio, id, रिक्त,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    शून्य
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_शोध_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    पूर्णांक		prio = 10;
    पूर्णांक		id = -1;
    पूर्णांक		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_संख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	परत;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    जर (group == रिक्त)
	परत;

    जर (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	परत;
    }
    l = argvars[1].vval.v_list;
    जर (l == रिक्त || l->lv_len == 0)
	परत;

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], &error);
	जर (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णांक)tv_get_संख्या_chk(&argvars[3], &error);

	    जर (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		परत;
	}
    }
    जर (error == TRUE)
	परत;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    जर (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	परत;
    }

    rettv->vval.v_संख्या = match_add(win, group, रिक्त, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    शून्य
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    जर (rettv_list_alloc(rettv) != OK)
	परत;

# ifdef FEAT_शोध_EXTRA
    पूर्णांक	    id;
    matchitem_T *m;

    जर (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	परत;

    id = (पूर्णांक)tv_get_संख्या(&argvars[0]);
    जर (id >= 1 && id <= 3)
    {
	जर ((m = get_match(curwin, id)) != रिक्त)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	अन्यथा
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
# ifdef FEAT_शोध_EXTRA
    win_T   *win;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    win = get_optional_window(argvars, 1);
    जर (win == रिक्त)
	rettv->vval.v_संख्या = -1;
    अन्यथा
	rettv->vval.v_संख्या = match_delete(win,
				       (पूर्णांक)tv_get_संख्या(&argvars[0]), TRUE);
# endif
}
#endif

#जर defined(FEAT_शोध_EXTRA)
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
    पूर्णांक		c;
    पूर्णांक		id;

    जर (eap->line2 <= 3)
	id = eap->line2;
    अन्यथा
    {
	emsg(_(e_invalid_command));
	परत;
    }

    // First clear any old pattern.
    जर (!eap->skip)
	match_delete(curwin, id, FALSE);

    जर (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    अन्यथा जर ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    अन्यथा
    {
	p = skiptowhite(eap->arg);
	जर (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	जर (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    परत;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	जर (!eap->skip)
	{
	    जर (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		परत;
	    }
	    जर (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		परत;
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
