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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)

# define ସନ୍ଧାନ_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prio,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hlg_id;
    regprog_T	*regprog = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rtype = UPD_SOME_VALID;

    ଯଦି (*grp == NUL || (pat != ଖାଲି && *pat == NUL))
	ଫେରନ୍ତୁ -1;
    ଯଦି (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	ଫେରନ୍ତୁ -1;
    }
    ଯଦି (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    ଅନ୍ୟଥା
    {
	// check the given ID is not already in use
	ପାଇଁ (cur = wp->w_match_head; cur != ଖାଲି; cur = cur->mit_next)
	    ଯଦି (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		ଫେରନ୍ତୁ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ଯଦି (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    ଯଦି ((hlg_id = syn_namen2id(grp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	ଫେରନ୍ତୁ -1;
    }
    ଯଦି (pat != ଖାଲି && (regprog = vim_regcomp(pat, RE_MAGIC)) == ଖାଲି)
    {
	semsg(_(e_invalid_argument_str), pat);
	ଫେରନ୍ତୁ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ଯଦି (m == ଖାଲି)
	ଯାଅ fail;
    ଯଦି (pos_list != ଖାଲି && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ଯଦି (m->mit_pos_array == ଖାଲି)
	    ଯାଅ fail;
	m->mit_pos_ଗଣନା = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == ଖାଲି ? ଖାଲି : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# ଯଦି defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ଯଦି (conceal_char != ଖାଲି)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ଯଦି (pos_list != ଖାଲି)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	ପାଇଁ (i = 0, li = pos_list->lv_first; li != ଖାଲି; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	    ଯଦି (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ଯଦି (subl == ଖାଲି)
		    ଯାଅ fail;
		subli = subl->lv_first;
		ଯଦି (subli == ଖାଲି)
		    ଯାଅ fail;
		lnum = tv_get_ସଂଖ୍ୟା_chk(&subli->li_tv, &error);
		ଯଦି (error == TRUE)
		    ଯାଅ fail;
		ଯଦି (lnum == 0)
		{
		    --i;
		    ଜାରି;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		ଯଦି (subli != ଖାଲି)
		{
		    col = tv_get_ସଂଖ୍ୟା_chk(&subli->li_tv, &error);
		    ଯଦି (error == TRUE)
			ଯାଅ fail;
		    subli = subli->li_next;
		    ଯଦି (subli != ଖାଲି)
		    {
			len = tv_get_ସଂଖ୍ୟା_chk(&subli->li_tv, &error);
			ଯଦି (error == TRUE)
			    ଯାଅ fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    ଅନ୍ୟଥା ଯଦି (li->li_tv.v_type == VAR_ସଂଖ୍ୟା)
	    {
		ଯଦି (li->li_tv.vval.v_ସଂଖ୍ୟା == 0)
		{
		    --i;
		    ଜାରି;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_ସଂଖ୍ୟା;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		emsg(_(e_list_or_ସଂଖ୍ୟା_required));
		ଯାଅ fail;
	    }
	    ଯଦି (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ଯଦି (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ଯଦି (toplnum != 0)
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
    ଯେତେବେଳେ (cur != ଖାଲି && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ଯଦି (cur == prev)
	wp->w_match_head = m;
    ଅନ୍ୟଥା
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    ଫେରନ୍ତୁ id;

fail:
    vim_regfree(regprog);
    ଯଦି (m != ଖାଲି)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_delete(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rtype = UPD_SOME_VALID;

    ଯଦି (id < 1)
    {
	ଯଦି (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	ଫେରନ୍ତୁ -1;
    }
    ଯେତେବେଳେ (cur != ଖାଲି && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ଯଦି (cur == ଖାଲି)
    {
	ଯଦି (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	ଫେରନ୍ତୁ -1;
    }
    ଯଦି (cur == prev)
	wp->w_match_head = cur->mit_next;
    ଅନ୍ୟଥା
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    ଯଦି (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ଫେରନ୍ତୁ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    ଶୂନ୍ୟ
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ଯେତେବେଳେ (wp->w_match_head != ଖାଲି)
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
    ସ୍ଥିର matchitem_T *
get_match(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    matchitem_T *cur = wp->w_match_head;

    ଯେତେବେଳେ (cur != ଖାଲି && cur->mit_id != id)
	cur = cur->mit_next;
    ଫେରନ୍ତୁ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    ଶୂନ୍ୟ
init_ସନ୍ଧାନ_hl(win_T *wp, match_T *ସନ୍ଧାନ_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    ଯେତେବେଳେ (cur != ଖାଲି)
    {
	cur->mit_hl.rm = cur->mit_match;
	ଯଦି (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ଅନ୍ୟଥା
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    ସନ୍ଧାନ_hl->attr = HL_ATTR(HLF_L);

    ସନ୍ଧାନ_hl->buf = wp->w_buffer;
    ସନ୍ଧାନ_hl->lnum = 0;
    ସନ୍ଧାନ_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
next_ସନ୍ଧାନ_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    found = -1;

    ପାଇଁ (i = match->mit_pos_cur; i < match->mit_pos_ଗଣନା; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	ଯଦି (pos->lnum == 0)
	    ଭାଙ୍ଗ;
	ଯଦି (pos->len == 0 && pos->col < mincol)
	    ଜାରି;
	ଯଦି (pos->lnum == lnum)
	{
	    ଯଦି (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ଯଦି (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    ଅନ୍ୟଥା
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    ଯଦି (found >= 0)
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
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
next_ସନ୍ଧାନ_hl(
    win_T	    *win,
    match_T	    *ସନ୍ଧାନ_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    ଯଦି ((lnum < ସନ୍ଧାନ_first_ଧାଡ଼ି || lnum > ସନ୍ଧାନ_last_ଧାଡ଼ି) && cur == ଖାଲି)
    {
	shl->lnum = 0;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ଯଦି (lnum > l)
	    shl->lnum = 0;
	ଅନ୍ୟଥା ଯଦି (lnum < l || shl->rm.endpos[0].col > mincol)
	    ଫେରନ୍ତୁ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    ପାଇଁ (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ଯଦି (shl->lnum == 0)
	    matchcol = 0;
	ଅନ୍ୟଥା ଯଦି (vim_strchr(p_cpo, CPO_ସନ୍ଧାନ) == ଖାଲି
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ଯଦି (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ଅନ୍ୟଥା
		++matchcol;
	}
	ଅନ୍ୟଥା
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	ଯଦି (shl->rm.regprog != ଖାଲି)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା regprog_is_copy = (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    ଯଦି (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    ଯଦି (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		ଯଦି (shl == ସନ୍ଧାନ_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ଖାଲି;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (cur != ଖାଲି)
	    nmatched = next_ସନ୍ଧାନ_hl_pos(shl, lnum, cur, matchcol);
	ଅନ୍ୟଥା
	    nmatched = 0;
	ଯଦି (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ଭାଙ୍ଗ;
	}
	ଯଦି (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ଭାଙ୍ଗ;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    ଶୂନ୍ୟ
prepare_ସନ୍ଧାନ_hl(win_T *wp, match_T *ସନ୍ଧାନ_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos_inprogress;	// marks that position match search is
				// in progress
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ଯେତେବେଳେ (cur != ଖାଲି || shl_flag == FALSE)
    {
	ଯଦି (shl_flag == FALSE)
	{
	    shl = ସନ୍ଧାନ_hl;
	    shl_flag = TRUE;
	}
	ଅନ୍ୟଥା
	    shl = &cur->mit_hl;
	ଯଦି (shl->rm.regprog != ଖାଲି
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ଯଦି (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		ପାଇଁ (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    ଯଦି (hasFoldingWin(wp, shl->first_lnum - 1,
						      ଖାଲି, ଖାଲି, TRUE, ଖାଲି))
			ଭାଙ୍ଗ;
# ଅନ୍ୟଥା
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    ଯଦି (cur != ଖାଲି)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ଯେତେବେଳେ (shl->first_lnum < lnum && (shl->rm.regprog != ଖାଲି
					  || (cur != ଖାଲି && pos_inprogress)))
	    {
		next_ସନ୍ଧାନ_hl(wp, ସନ୍ଧାନ_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == ସନ୍ଧାନ_hl ? ଖାଲି : cur);
		pos_inprogress = cur == ଖାଲି || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ଯଦି (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		ଅନ୍ୟଥା
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	ଯଦି (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_cur_ସନ୍ଧାନ_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ଯଦି (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    ଅନ୍ୟଥା
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prepare_ସନ୍ଧାନ_hl_ଧାଡ଼ି(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **ଧାଡ଼ି,
	match_T	    *ସନ୍ଧାନ_hl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *ସନ୍ଧାନ_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ଯେତେବେଳେ (cur != ଖାଲି || shl_flag == FALSE)
    {
	ଯଦି (shl_flag == FALSE)
	{
	    shl = ସନ୍ଧାନ_hl;
	    shl_flag = TRUE;
	}
	ଅନ୍ୟଥା
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ଯଦି (cur != ଖାଲି)
	    cur->mit_pos_cur = 0;
	next_ସନ୍ଧାନ_hl(wp, ସନ୍ଧାନ_hl, shl, lnum, mincol,
						shl == ସନ୍ଧାନ_hl ? ଖାଲି : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

	ଯଦି (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ଯଦି (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    ଅନ୍ୟଥା
		shl->startcol = 0;
	    ଯଦି (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    ଅନ୍ୟଥା
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ଯଦି (shl == ସନ୍ଧାନ_hl)
		check_cur_ସନ୍ଧାନ_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ଯଦି (shl->startcol == shl->endcol)
	    {
		ଯଦି (has_mbyte && (*ଧାଡ଼ି)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*ଧାଡ଼ି) + shl->endcol);
		ଅନ୍ୟଥା
		    ++shl->endcol;
	    }
	    ଯଦି ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*ସନ୍ଧାନ_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ଯଦି (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି)
	    cur = cur->mit_next;
    }
    ଫେରନ୍ତୁ area_highlighting;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
update_ସନ୍ଧାନ_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **ଧାଡ଼ି,
	match_T	    *ସନ୍ଧାନ_hl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *has_match_conc UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *match_conc UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_ଧାଡ଼ି_attr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lcs_eol_one,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos_inprogress;	    // marks that position match search is in
				    // progress
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ଯେତେବେଳେ (cur != ଖାଲି || shl_flag == FALSE)
    {
	ଯଦି (shl_flag == FALSE
		&& (cur == ଖାଲି
			|| cur->mit_priority > ସନ୍ଧାନ_HL_PRIORITY))
	{
	    shl = ସନ୍ଧାନ_hl;
	    shl_flag = TRUE;
	}
	ଅନ୍ୟଥା
	    shl = &cur->mit_hl;
	ଯଦି (cur != ଖାଲି)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ଯେତେବେଳେ (shl->rm.regprog != ଖାଲି || (cur != ଖାଲି && pos_inprogress))
	{
	    ଯଦି (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_col = col + mb_ptr2len(*ଧାଡ଼ି + col);

		ଯଦି (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ଯଦି (cur != ଖାଲି
			&& shl != ସନ୍ଧାନ_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		ଅନ୍ୟଥା
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ଯଦି (shl == ସନ୍ଧାନ_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ଯଦି (shl->attr_cur != shl->attr)
			ସନ୍ଧାନ_hl_has_cursor_lnum = lnum;
		}

	    }
	    ଅନ୍ୟଥା ଯଦି (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_ସନ୍ଧାନ_hl(wp, ସନ୍ଧାନ_hl, shl, lnum, col,
					       shl == ସନ୍ଧାନ_hl ? ଖାଲି : cur);
		pos_inprogress = !(cur == ଖାଲି || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

		ଯଦି (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    ଯଦି (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    ଅନ୍ୟଥା
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ଯଦି (shl == ସନ୍ଧାନ_hl)
			check_cur_ସନ୍ଧାନ_hl(wp, shl);

		    ଯଦି (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ଯଦି (has_mbyte)
			{
			    char_u *p = *ଧାଡ଼ି + shl->endcol;

			    ଯଦି (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    ଅନ୍ୟଥା
				shl->endcol += (*mb_ptr2len)(p);
			}
			ଅନ୍ୟଥା
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ଜାରି;
		}
	    }
	    ଭାଙ୍ଗ;
	}
	ଯଦି (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ଯେତେବେଳେ (cur != ଖାଲି || shl_flag == FALSE)
    {
	ଯଦି (shl_flag == FALSE
		&& (cur == ଖାଲି ||
			cur->mit_priority > ସନ୍ଧାନ_HL_PRIORITY))
	{
	    shl = ସନ୍ଧାନ_hl;
	    shl_flag = TRUE;
	}
	ଅନ୍ୟଥା
	    shl = &cur->mit_hl;
	ଯଦି (shl->attr_cur != 0)
	{
	    ସନ୍ଧାନ_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	ଯଦି (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    ଯଦି (*(*ଧାଡ଼ି + col) == NUL && (did_ଧାଡ଼ି_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	ସନ୍ଧାନ_attr = 0;
    ଫେରନ୍ତୁ ସନ୍ଧାନ_attr;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_prevcol_hl_flag(win_T *wp, match_T *ସନ୍ଧାନ_hl, long curcol)
{
    long	prevcol = curcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ଯଦି defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ଯଦି (popup_is_popup(wp))
	ଫେରନ୍ତୁ FALSE;
# endif

    // we're not really at that column when skipping some text
    ଯଦି ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ଯଦି (!ସନ୍ଧାନ_hl->is_addpos && (prevcol == (long)ସନ୍ଧାନ_hl->startcol
		|| (prevcol > (long)ସନ୍ଧାନ_hl->startcol
					      && ସନ୍ଧାନ_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ଅନ୍ୟଥା
    {
	cur = wp->w_match_head;
	ଯେତେବେଳେ (cur != ଖାଲି)
	{
	    ଯଦି (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ଭାଙ୍ଗ;
	    }
	    cur = cur->mit_next;
	}
    }
    ଫେରନ୍ତୁ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    ଶୂନ୍ୟ
get_ସନ୍ଧାନ_match_hl(win_T *wp, match_T *ସନ୍ଧାନ_hl, long col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    ଯେତେବେଳେ (cur != ଖାଲି || shl_flag == FALSE)
    {
	ଯଦି (shl_flag == FALSE
		&& ((cur != ଖାଲି
			&& cur->mit_priority > ସନ୍ଧାନ_HL_PRIORITY)
		    || cur == ଖାଲି))
	{
	    shl = ସନ୍ଧାନ_hl;
	    shl_flag = TRUE;
	}
	ଅନ୍ୟଥା
	    shl = &cur->mit_hl;
	ଯଦି (col - 1 == (long)shl->startcol
		&& (shl == ସନ୍ଧାନ_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ଯଦି (shl != ସନ୍ଧାନ_hl && cur != ଖାଲି)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#ଯଦି defined(FEAT_EVAL)
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ଯଦି (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (dict_has_ଚାବି(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ଯଦି ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    ଯଦି (*win == ଖାଲି)
    {
	emsg(_(e_invalid_window_ସଂଖ୍ୟା));
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    ଶୂନ୍ୟ
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    win_T   *win;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    win = get_optional_window(argvars, 0);
    ଯଦି (win != ଖାଲି)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    ଶୂନ୍ୟ
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*win;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    win = get_optional_window(argvars, 0);
    ଯଦି (rettv_list_alloc(rettv) == FAIL || win == ଖାଲି)
	ଫେରନ୍ତୁ;

    cur = win->w_match_head;
    ଯେତେବେଳେ (cur != ଖାଲି)
    {
	dict = dict_alloc();
	ଯଦି (dict == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (cur->mit_match.regprog == ଖାଲି)
	{
	    // match added with matchaddpos()
	    ପାଇଁ (i = 0; i < cur->mit_pos_ଗଣନା; ++i)
	    {
		llpos_T	*llpos;
		ଅକ୍ଷର	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ଯଦି (llpos->lnum == 0)
		    ଭାଙ୍ଗ;
		l = list_alloc();
		ଯଦି (l == ଖାଲି)
		    ଭାଙ୍ଗ;
		list_append_ସଂଖ୍ୟା(l, (varnumber_T)llpos->lnum);
		ଯଦି (llpos->col > 0)
		{
		    list_append_ସଂଖ୍ୟା(l, (varnumber_T)llpos->col);
		    list_append_ସଂଖ୍ୟା(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ଯଦି (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ଅନ୍ୟଥା
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_ସଂଖ୍ୟା(dict, "priority", (long)cur->mit_priority);
	dict_add_ସଂଖ୍ୟା(dict, "id", (long)cur->mit_id);
#  ଯଦି defined(FEAT_CONCEAL)
	ଯଦି (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ଯଦି (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    ଶୂନ୍ୟ
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ଖାଲି;
    win_T	*win;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    win = get_optional_window(argvars, 1);
    ଯଦି (win == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି ((l = argvars[0].vval.v_list) != ଖାଲି)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	ଯେତେବେଳେ (li != ଖାଲି)
	{
	    ଯଦି (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ଖାଲି)
	    {
		emsg(_(e_invalid_argument));
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (!(dict_has_ଚାବି(d, "group")
			&& (dict_has_ଚାବି(d, "pattern")
			    || dict_has_ଚାବି(d, "pos1"))
			&& dict_has_ଚାବି(d, "priority")
			&& dict_has_ଚାବି(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		ଫେରନ୍ତୁ;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	ଯେତେବେଳେ (li != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
	    ଅକ୍ଷର	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		priority;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ଯଦି (!dict_has_ଚାବି(d, "pattern"))
	    {
		ଯଦି (s == ଖାଲି)
		{
		    s = list_alloc();
		    ଯଦି (s == ଖାଲି)
			ଫେରନ୍ତୁ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		ପାଇଁ (i = 1; i < 9; i++)
		{
		    sprintf((ଅକ୍ଷର *)buf, (ଅକ୍ଷର *)"pos%d", i);
		    ଯଦି ((di = dict_find(d, (char_u *)buf, -1)) != ଖାଲି)
		    {
			ଯଦି (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ଫେରନ୍ତୁ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ଅନ୍ୟଥା
			ଭାଙ୍ଗ;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "priority");
	    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "id");
	    conceal = dict_has_ଚାବି(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ଖାଲି;
	    ଯଦି (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, ଖାଲି, conceal);
	    }
	    ଅନ୍ୟଥା
	    {
		match_add(win, group, ଖାଲି, priority, id, s, conceal);
		list_unref(s);
		s = ଖାଲି;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    ଶୂନ୍ୟ
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prio = 10;	// default priority
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    char_u	*conceal_char = ଖାଲି;
    win_T	*win = curwin;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ଫେରନ୍ତୁ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    ଯଦି (grp == ଖାଲି || pat == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], &error);
	    ଯଦି (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (error == TRUE)
	ଫେରନ୍ତୁ;
    ଯଦି (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ଫେରନ୍ତୁ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = match_add(win, grp, pat, prio, id, ଖାଲି,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    ଶୂନ୍ୟ
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prio = 10;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ଖାଲି;
    win_T	*win = curwin;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ଫେରନ୍ତୁ;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    ଯଦି (group == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	ଫେରନ୍ତୁ;
    }
    l = argvars[1].vval.v_list;
    ଯଦି (l == ଖାଲି || l->lv_len == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], &error);

	    ଯଦି (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (error == TRUE)
	ଫେରନ୍ତୁ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ଯଦି (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	ଫେରନ୍ତୁ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = match_add(win, group, ଖାଲି, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    ଶୂନ୍ୟ
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) != OK)
	ଫେରନ୍ତୁ;

# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;
    matchitem_T *m;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    ଯଦି (id >= 1 && id <= 3)
    {
	ଯଦି ((m = get_match(curwin, id)) != ଖାଲି)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	ଅନ୍ୟଥା
	{
	    list_append_string(rettv->vval.v_list, ଖାଲି, -1);
	    list_append_string(rettv->vval.v_list, ଖାଲି, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    ଶୂନ୍ୟ
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    win_T   *win;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    win = get_optional_window(argvars, 1);
    ଯଦି (win == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = match_delete(win,
				       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]), TRUE);
# endif
}
#endif

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    ଶୂନ୍ୟ
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ଖାଲି;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;

    ଯଦି (eap->line2 <= 3)
	id = eap->line2;
    ଅନ୍ୟଥା
    {
	emsg(_(e_invalid_command));
	ଫେରନ୍ତୁ;
    }

    // First clear any old pattern.
    ଯଦି (!eap->skip)
	match_delete(curwin, id, FALSE);

    ଯଦି (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    ଅନ୍ୟଥା ଯଦି ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    ଅନ୍ୟଥା
    {
	p = skiptowhite(eap->arg);
	ଯଦି (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ଯଦି (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	ଯଦି (!eap->skip)
	{
	    ଯଦି (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		ଫେରନ୍ତୁ;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, ଖାଲି, ଖାଲି);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
