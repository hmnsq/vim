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

#यदि defined(FEAT_खोज_EXTRA)

# define खोज_HL_PRIORITY 0

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

    यदि (*grp == NUL || (pat != रिक्त && *pat == NUL))
	फर्काऊ -1;
    यदि (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	फर्काऊ -1;
    }
    यदि (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    अन्यथा
    {
	// check the given ID is not already in use
	लागि (cur = wp->w_match_head; cur != रिक्त; cur = cur->mit_next)
	    यदि (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		फर्काऊ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	यदि (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    यदि ((hlg_id = syn_namen2id(grp, (पूर्णाङ्क)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	फर्काऊ -1;
    }
    यदि (pat != रिक्त && (regprog = vim_regcomp(pat, RE_MAGIC)) == रिक्त)
    {
	semsg(_(e_invalid_argument_str), pat);
	फर्काऊ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    यदि (m == रिक्त)
	गच्छ fail;
    यदि (pos_list != रिक्त && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	यदि (m->mit_pos_array == रिक्त)
	    गच्छ fail;
	m->mit_pos_गन्ती = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == रिक्त ? रिक्त : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# यदि defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    यदि (conceal_char != रिक्त)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    यदि (pos_list != रिक्त)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	पूर्णाङ्क		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	लागि (i = 0, li = pos_list->lv_first; li != रिक्त; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    पूर्णाङ्क		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    पूर्णाङ्क		error = FALSE;

	    यदि (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		यदि (subl == रिक्त)
		    गच्छ fail;
		subli = subl->lv_first;
		यदि (subli == रिक्त)
		    गच्छ fail;
		lnum = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
		यदि (error == TRUE)
		    गच्छ fail;
		यदि (lnum == 0)
		{
		    --i;
		    जारी;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		यदि (subli != रिक्त)
		{
		    col = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
		    यदि (error == TRUE)
			गच्छ fail;
		    subli = subli->li_next;
		    यदि (subli != रिक्त)
		    {
			len = tv_get_सङ्ख्या_chk(&subli->li_tv, &error);
			यदि (error == TRUE)
			    गच्छ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    अन्यथा यदि (li->li_tv.v_type == VAR_सङ्ख्या)
	    {
		यदि (li->li_tv.vval.v_सङ्ख्या == 0)
		{
		    --i;
		    जारी;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_सङ्ख्या;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    अन्यथा
	    {
		emsg(_(e_list_or_सङ्ख्या_required));
		गच्छ fail;
	    }
	    यदि (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    यदि (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	यदि (toplnum != 0)
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
    जबसम्म (cur != रिक्त && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    यदि (cur == prev)
	wp->w_match_head = m;
    अन्यथा
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    फर्काऊ id;

fail:
    vim_regfree(regprog);
    यदि (m != रिक्त)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    फर्काऊ -1;
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

    यदि (id < 1)
    {
	यदि (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	फर्काऊ -1;
    }
    जबसम्म (cur != रिक्त && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    यदि (cur == रिक्त)
    {
	यदि (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	फर्काऊ -1;
    }
    यदि (cur == prev)
	wp->w_match_head = cur->mit_next;
    अन्यथा
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    यदि (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    फर्काऊ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    शून्य
clear_matches(win_T *wp)
{
    matchitem_T *m;

    जबसम्म (wp->w_match_head != रिक्त)
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

    जबसम्म (cur != रिक्त && cur->mit_id != id)
	cur = cur->mit_next;
    फर्काऊ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    शून्य
init_खोज_hl(win_T *wp, match_T *खोज_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    जबसम्म (cur != रिक्त)
    {
	cur->mit_hl.rm = cur->mit_match;
	यदि (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	अन्यथा
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    खोज_hl->attr = HL_ATTR(HLF_L);

    खोज_hl->buf = wp->w_buffer;
    खोज_hl->lnum = 0;
    खोज_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    स्थिर पूर्णाङ्क
next_खोज_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    found = -1;

    लागि (i = match->mit_pos_cur; i < match->mit_pos_गन्ती; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	यदि (pos->lnum == 0)
	    रोक;
	यदि (pos->len == 0 && pos->col < mincol)
	    जारी;
	यदि (pos->lnum == lnum)
	{
	    यदि (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		यदि (pos->col < match->mit_pos_array[found].col)
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
    यदि (found >= 0)
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
	फर्काऊ 1;
    }
    फर्काऊ 0;
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
next_खोज_hl(
    win_T	    *win,
    match_T	    *खोज_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    दीर्घ	nmatched;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    यदि ((lnum < खोज_first_पङ्क्ति || lnum > खोज_last_पङ्क्ति) && cur == रिक्त)
    {
	shl->lnum = 0;
	फर्काऊ;
    }

    यदि (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	यदि (lnum > l)
	    shl->lnum = 0;
	अन्यथा यदि (lnum < l || shl->rm.endpos[0].col > mincol)
	    फर्काऊ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    लागि (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	यदि (shl->lnum == 0)
	    matchcol = 0;
	अन्यथा यदि (vim_strchr(p_cpo, CPO_खोज) == रिक्त
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    यदि (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		रोक;
	    }
	    यदि (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    अन्यथा
		++matchcol;
	}
	अन्यथा
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	यदि (shl->rm.regprog != रिक्त)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    पूर्णाङ्क regprog_is_copy = (shl != खोज_hl && cur != रिक्त
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    यदि (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    यदि (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		यदि (shl == खोज_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = रिक्त;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		रोक;
	    }
	}
	अन्यथा यदि (cur != रिक्त)
	    nmatched = next_खोज_hl_pos(shl, lnum, cur, matchcol);
	अन्यथा
	    nmatched = 0;
	यदि (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    रोक;
	}
	यदि (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    रोक;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    शून्य
prepare_खोज_hl(win_T *wp, match_T *खोज_hl, linenr_T lnum)
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
    जबसम्म (cur != रिक्त || shl_flag == FALSE)
    {
	यदि (shl_flag == FALSE)
	{
	    shl = खोज_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	यदि (shl->rm.regprog != रिक्त
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    यदि (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		लागि (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    यदि (hasFoldingWin(wp, shl->first_lnum - 1,
						      रिक्त, रिक्त, TRUE, रिक्त))
			रोक;
# अन्यथा
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    यदि (cur != रिक्त)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    जबसम्म (shl->first_lnum < lnum && (shl->rm.regprog != रिक्त
					  || (cur != रिक्त && pos_inprogress)))
	    {
		next_खोज_hl(wp, खोज_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == खोज_hl ? रिक्त : cur);
		pos_inprogress = cur == रिक्त || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		यदि (shl->lnum != 0)
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
	यदि (shl != खोज_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    स्थिर शून्य
check_cur_खोज_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    यदि (wp->w_cursor.lnum >= shl->lnum
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
    पूर्णाङ्क
prepare_खोज_hl_पङ्क्ति(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **पङ्क्ति,
	match_T	    *खोज_hl,
	पूर्णाङ्क	    *खोज_attr)
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
    जबसम्म (cur != रिक्त || shl_flag == FALSE)
    {
	यदि (shl_flag == FALSE)
	{
	    shl = खोज_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	यदि (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	next_खोज_hl(wp, खोज_hl, shl, lnum, mincol,
						shl == खोज_hl ? रिक्त : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*पङ्क्ति = ml_get_buf(wp->w_buffer, lnum, FALSE);

	यदि (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    यदि (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    अन्यथा
		shl->startcol = 0;
	    यदि (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    अन्यथा
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    यदि (shl == खोज_hl)
		check_cur_खोज_hl(wp, shl);

	    // Highlight one character for an empty match.
	    यदि (shl->startcol == shl->endcol)
	    {
		यदि (has_mbyte && (*पङ्क्ति)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*पङ्क्ति) + shl->endcol);
		अन्यथा
		    ++shl->endcol;
	    }
	    यदि ((दीर्घ)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*खोज_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	यदि (shl != खोज_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    फर्काऊ area_highlighting;
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
update_खोज_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **पङ्क्ति,
	match_T	    *खोज_hl,
	पूर्णाङ्क	    *has_match_conc UNUSED,
	पूर्णाङ्क	    *match_conc UNUSED,
	पूर्णाङ्क	    did_पङ्क्ति_attr,
	पूर्णाङ्क	    lcs_eol_one,
	पूर्णाङ्क	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    पूर्णाङ्क		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    पूर्णाङ्क		pos_inprogress;	    // marks that position match search is in
				    // progress
    पूर्णाङ्क		खोज_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    जबसम्म (cur != रिक्त || shl_flag == FALSE)
    {
	यदि (shl_flag == FALSE
		&& (cur == रिक्त
			|| cur->mit_priority > खोज_HL_PRIORITY))
	{
	    shl = खोज_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	यदि (cur != रिक्त)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	जबसम्म (shl->rm.regprog != रिक्त || (cur != रिक्त && pos_inprogress))
	{
	    यदि (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		पूर्णाङ्क next_col = col + mb_ptr2len(*पङ्क्ति + col);

		यदि (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		यदि (cur != रिक्त
			&& shl != खोज_hl
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
		यदि (shl == खोज_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    यदि (shl->attr_cur != shl->attr)
			खोज_hl_has_cursor_lnum = lnum;
		}

	    }
	    अन्यथा यदि (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_खोज_hl(wp, खोज_hl, shl, lnum, col,
					       shl == खोज_hl ? रिक्त : cur);
		pos_inprogress = !(cur == रिक्त || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*पङ्क्ति = ml_get_buf(wp->w_buffer, lnum, FALSE);

		यदि (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    यदि (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    अन्यथा
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    यदि (shl == खोज_hl)
			check_cur_खोज_hl(wp, shl);

		    यदि (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			यदि (has_mbyte)
			{
			    char_u *p = *पङ्क्ति + shl->endcol;

			    यदि (*p == NUL)
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
		    जारी;
		}
	    }
	    रोक;
	}
	यदि (shl != खोज_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    जबसम्म (cur != रिक्त || shl_flag == FALSE)
    {
	यदि (shl_flag == FALSE
		&& (cur == रिक्त ||
			cur->mit_priority > खोज_HL_PRIORITY))
	{
	    shl = खोज_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	यदि (shl->attr_cur != 0)
	{
	    खोज_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	यदि (shl != खोज_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    यदि (*(*पङ्क्ति + col) == NUL && (did_पङ्क्ति_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	खोज_attr = 0;
    फर्काऊ खोज_attr;
}

    पूर्णाङ्क
get_prevcol_hl_flag(win_T *wp, match_T *खोज_hl, दीर्घ curcol)
{
    दीर्घ	prevcol = curcol;
    पूर्णाङ्क		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# यदि defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    यदि (popup_is_popup(wp))
	फर्काऊ FALSE;
# endif

    // we're not really at that column when skipping some text
    यदि ((दीर्घ)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    यदि (!खोज_hl->is_addpos && (prevcol == (दीर्घ)खोज_hl->startcol
		|| (prevcol > (दीर्घ)खोज_hl->startcol
					      && खोज_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    अन्यथा
    {
	cur = wp->w_match_head;
	जबसम्म (cur != रिक्त)
	{
	    यदि (!cur->mit_hl.is_addpos && (prevcol == (दीर्घ)cur->mit_hl.startcol
			|| (prevcol > (दीर्घ)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		रोक;
	    }
	    cur = cur->mit_next;
	}
    }
    फर्काऊ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    शून्य
get_खोज_match_hl(win_T *wp, match_T *खोज_hl, दीर्घ col, पूर्णाङ्क *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    पूर्णाङ्क		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    जबसम्म (cur != रिक्त || shl_flag == FALSE)
    {
	यदि (shl_flag == FALSE
		&& ((cur != रिक्त
			&& cur->mit_priority > खोज_HL_PRIORITY)
		    || cur == रिक्त))
	{
	    shl = खोज_hl;
	    shl_flag = TRUE;
	}
	अन्यथा
	    shl = &cur->mit_hl;
	यदि (col - 1 == (दीर्घ)shl->startcol
		&& (shl == खोज_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	यदि (shl != खोज_hl && cur != रिक्त)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#यदि defined(FEAT_EVAL)
# ifdef FEAT_खोज_EXTRA
    स्थिर पूर्णाङ्क
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    यदि (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	फर्काऊ FAIL;
    }

    यदि (dict_has_साँचो(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    यदि ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == रिक्त)
	फर्काऊ OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    यदि (*win == रिक्त)
    {
	emsg(_(e_invalid_window_सङ्ख्या));
	फर्काऊ FAIL;
    }

    फर्काऊ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    शून्य
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_खोज_EXTRA
    win_T   *win;

    यदि (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    win = get_optional_window(argvars, 0);
    यदि (win != रिक्त)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    शून्य
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_खोज_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    पूर्णाङ्क		i;
    win_T	*win;

    यदि (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    win = get_optional_window(argvars, 0);
    यदि (rettv_list_alloc(rettv) == FAIL || win == रिक्त)
	फर्काऊ;

    cur = win->w_match_head;
    जबसम्म (cur != रिक्त)
    {
	dict = dict_alloc();
	यदि (dict == रिक्त)
	    फर्काऊ;
	यदि (cur->mit_match.regprog == रिक्त)
	{
	    // match added with matchaddpos()
	    लागि (i = 0; i < cur->mit_pos_गन्ती; ++i)
	    {
		llpos_T	*llpos;
		वर्ण	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		यदि (llpos->lnum == 0)
		    रोक;
		l = list_alloc();
		यदि (l == रिक्त)
		    रोक;
		list_append_सङ्ख्या(l, (varnumber_T)llpos->lnum);
		यदि (llpos->col > 0)
		{
		    list_append_सङ्ख्या(l, (varnumber_T)llpos->col);
		    list_append_सङ्ख्या(l, (varnumber_T)llpos->len);
		}
		प्र_लिखो(buf, "pos%d", i + 1);
		यदि (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	अन्यथा
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_सङ्ख्या(dict, "priority", (दीर्घ)cur->mit_priority);
	dict_add_सङ्ख्या(dict, "id", (दीर्घ)cur->mit_id);
#  यदि defined(FEAT_CONCEAL)
	यदि (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    पूर्णाङ्क	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	यदि (list_append_dict(rettv->vval.v_list, dict) == FAIL)
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
# ifdef FEAT_खोज_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = रिक्त;
    win_T	*win;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    यदि (check_for_list_arg(argvars, 0) == FAIL)
	फर्काऊ;
    win = get_optional_window(argvars, 1);
    यदि (win == रिक्त)
	फर्काऊ;

    यदि ((l = argvars[0].vval.v_list) != रिक्त)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	जबसम्म (li != रिक्त)
	{
	    यदि (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == रिक्त)
	    {
		emsg(_(e_invalid_argument));
		फर्काऊ;
	    }
	    यदि (!(dict_has_साँचो(d, "group")
			&& (dict_has_साँचो(d, "pattern")
			    || dict_has_साँचो(d, "pos1"))
			&& dict_has_साँचो(d, "priority")
			&& dict_has_साँचो(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		फर्काऊ;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	जबसम्म (li != रिक्त)
	{
	    पूर्णाङ्क		i = 0;
	    वर्ण	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    पूर्णाङ्क		priority;
	    पूर्णाङ्क		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    यदि (!dict_has_साँचो(d, "pattern"))
	    {
		यदि (s == रिक्त)
		{
		    s = list_alloc();
		    यदि (s == रिक्त)
			फर्काऊ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		लागि (i = 1; i < 9; i++)
		{
		    प्र_लिखो((वर्ण *)buf, (वर्ण *)"pos%d", i);
		    यदि ((di = dict_find(d, (char_u *)buf, -1)) != रिक्त)
		    {
			यदि (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    फर्काऊ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    अन्यथा
			रोक;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "priority");
	    id = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "id");
	    conceal = dict_has_साँचो(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : रिक्त;
	    यदि (i == 0)
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
# ifdef FEAT_खोज_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    पूर्णाङ्क		prio = 10;	// default priority
    पूर्णाङ्क		id = -1;
    पूर्णाङ्क		error = FALSE;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	फर्काऊ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    यदि (grp == रिक्त || pat == रिक्त)
	फर्काऊ;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[3], &error);
	    यदि (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		फर्काऊ;
	}
    }
    यदि (error == TRUE)
	फर्काऊ;
    यदि (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	फर्काऊ;
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
# ifdef FEAT_खोज_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    पूर्णाङ्क		prio = 10;
    पूर्णाङ्क		id = -1;
    पूर्णाङ्क		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = रिक्त;
    win_T	*win = curwin;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	फर्काऊ;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    यदि (group == रिक्त)
	फर्काऊ;

    यदि (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	फर्काऊ;
    }
    l = argvars[1].vval.v_list;
    यदि (l == रिक्त || l->lv_len == 0)
	फर्काऊ;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[3], &error);

	    यदि (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		फर्काऊ;
	}
    }
    यदि (error == TRUE)
	फर्काऊ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    यदि (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	फर्काऊ;
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
    यदि (rettv_list_alloc(rettv) != OK)
	फर्काऊ;

# ifdef FEAT_खोज_EXTRA
    पूर्णाङ्क	    id;
    matchitem_T *m;

    यदि (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    id = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]);
    यदि (id >= 1 && id <= 3)
    {
	यदि ((m = get_match(curwin, id)) != रिक्त)
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
# ifdef FEAT_खोज_EXTRA
    win_T   *win;

    यदि (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    win = get_optional_window(argvars, 1);
    यदि (win == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अन्यथा
	rettv->vval.v_सङ्ख्या = match_delete(win,
				       (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]), TRUE);
# endif
}
#endif

#यदि defined(FEAT_खोज_EXTRA)
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

    यदि (eap->line2 <= 3)
	id = eap->line2;
    अन्यथा
    {
	emsg(_(e_invalid_command));
	फर्काऊ;
    }

    // First clear any old pattern.
    यदि (!eap->skip)
	match_delete(curwin, id, FALSE);

    यदि (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    अन्यथा यदि ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    अन्यथा
    {
	p = skiptowhite(eap->arg);
	यदि (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	यदि (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    फर्काऊ;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	यदि (!eap->skip)
	{
	    यदि (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		फर्काऊ;
	    }
	    यदि (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		फर्काऊ;
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
