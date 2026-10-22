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

#include "vim.h"

#ако defined(FEAT_SEARCH_EXTRA)

# define SEARCH_HL_PRIORITY 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    static цео
match_add(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    цео		prio,
    цео		id,
    list_T	*pos_list,
    char_u      *conceal_char UNUSED) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = NULL;
    цео		hlg_id;
    regprog_T	*regprog = NULL;
    цео		rtype = UPD_SOME_VALID;

    ако (*grp == NUL || (pat != NULL && *pat == NUL))
	врати -1;
    ако (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_1), id);
	врати -1;
    }
    ако (id == -1)
    {
	// use the next available match ID
	id = wp->w_next_match_id++;
    }
    инако
    {
	// check the given ID is not already in use
	за (cur = wp->w_match_head; cur != NULL; cur = cur->mit_next)
	    ако (cur->mit_id == id)
	    {
		semsg(_(e_id_already_taken_nr), id);
		врати -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ако (wp->w_next_match_id < id + 100)
	    wp->w_next_match_id = id + 100;
    }

    ако ((hlg_id = syn_namen2id(grp, (цео)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_group_name_str), grp);
	врати -1;
    }
    ако (pat != NULL && (regprog = vim_regcomp(pat, RE_MAGIC)) == NULL)
    {
	semsg(_(e_invalid_argument_str), pat);
	врати -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ако (m == NULL)
	goto fail;
    ако (pos_list != NULL && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ако (m->mit_pos_array == NULL)
	    goto fail;
	m->mit_pos_count = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_priority = prio;
    m->mit_pattern = pat == NULL ? NULL : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_match.regprog = regprog;
    m->mit_match.rmm_ic = FALSE;
    m->mit_match.rmm_maxcol = 0;
# ако defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ако (conceal_char != NULL)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ако (pos_list != NULL)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	цео		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	за (i = 0, li = pos_list->lv_first; li != NULL; i++, li = li->li_next)
	{
	    linenr_T	lnum = 0;
	    colnr_T	col = 0;
	    цео		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    цео		error = FALSE;

	    ако (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ако (subl == NULL)
		    goto fail;
		subli = subl->lv_first;
		ако (subli == NULL)
		    goto fail;
		lnum = tv_get_number_chk(&subli->li_tv, &error);
		ако (error == TRUE)
		    goto fail;
		ако (lnum == 0)
		{
		    --i;
		    continue;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_next;
		ако (subli != NULL)
		{
		    col = tv_get_number_chk(&subli->li_tv, &error);
		    ако (error == TRUE)
			goto fail;
		    subli = subli->li_next;
		    ако (subli != NULL)
		    {
			len = tv_get_number_chk(&subli->li_tv, &error);
			ако (error == TRUE)
			    goto fail;
		    }
		}
		m->mit_pos_array[i].col = col;
		m->mit_pos_array[i].len = len;
	    }
	    инако ако (li->li_tv.v_type == VAR_NUMBER)
	    {
		ако (li->li_tv.vval.v_number == 0)
		{
		    --i;
		    continue;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_number;
		m->mit_pos_array[i].col = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    инако
	    {
		emsg(_(e_list_or_number_required));
		goto fail;
	    }
	    ако (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ако (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ако (toplnum != 0)
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
    док (cur != NULL && prio >= cur->mit_priority)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ако (cur == prev)
	wp->w_match_head = m;
    инако
	prev->mit_next = m;
    m->mit_next = cur;

    redraw_win_later(wp, rtype);
    врати id;

fail:
    vim_regfree(regprog);
    ако (m != NULL)
    {
	vim_free(m->mit_pattern);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    врати -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    static цео
match_delete(win_T *wp, цео id, цео perr)
{
    matchitem_T	*cur = wp->w_match_head;
    matchitem_T	*prev = cur;
    цео		rtype = UPD_SOME_VALID;

    ако (id < 1)
    {
	ако (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_to_one_2),
									   id);
	врати -1;
    }
    док (cur != NULL && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_next;
    }
    ако (cur == NULL)
    {
	ако (perr == TRUE)
	    semsg(_(e_id_not_found_nr), id);
	врати -1;
    }
    ако (cur == prev)
	wp->w_match_head = cur->mit_next;
    инако
	prev->mit_next = cur->mit_next;
    vim_regfree(cur->mit_match.regprog);
    vim_free(cur->mit_pattern);
    ако (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    врати 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    void
clear_matches(win_T *wp)
{
    matchitem_T *m;

    док (wp->w_match_head != NULL)
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
    static matchitem_T *
get_match(win_T *wp, цео id)
{
    matchitem_T *cur = wp->w_match_head;

    док (cur != NULL && cur->mit_id != id)
	cur = cur->mit_next;
    врати cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    void
init_search_hl(win_T *wp, match_T *search_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    // match
    cur = wp->w_match_head;
    док (cur != NULL)
    {
	cur->mit_hl.rm = cur->mit_match;
	ако (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	инако
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_buffer;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.first_lnum = 0;
	cur = cur->mit_next;
    }
    // Must update this every time since highlight group override can change it.
    search_hl->attr = HL_ATTR(HLF_L);

    search_hl->buf = wp->w_buffer;
    search_hl->lnum = 0;
    search_hl->first_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    static цео
next_search_hl_pos(
    match_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *match,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    цео	    i;
    цео	    found = -1;

    за (i = match->mit_pos_cur; i < match->mit_pos_count; i++)
    {
	llpos_T	*pos = &match->mit_pos_array[i];

	ако (pos->lnum == 0)
	    break;
	ако (pos->len == 0 && pos->col < mincol)
	    continue;
	ако (pos->lnum == lnum)
	{
	    ако (found >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ако (pos->col < match->mit_pos_array[found].col)
		{
		    llpos_T	tmp = *pos;

		    *pos = match->mit_pos_array[found];
		    match->mit_pos_array[found] = tmp;
		}
	    }
	    инако
		found = i;
	}
    }
    match->mit_pos_cur = 0;
    ако (found >= 0)
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
	врати 1;
    }
    врати 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    static void
next_search_hl(
    win_T	    *win,
    match_T	    *search_hl,
    match_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;
    цео		called_emsg_before = called_emsg;
    цео		timed_out = FALSE;

    // for :{range}s/pat only highlight inside the range
    ако ((lnum < search_first_line || lnum > search_last_line) && cur == NULL)
    {
	shl->lnum = 0;
	врати;
    }

    ако (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ако (lnum > l)
	    shl->lnum = 0;
	инако ако (lnum < l || shl->rm.endpos[0].col > mincol)
	    врати;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    за (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ако (shl->lnum == 0)
	    matchcol = 0;
	инако ако (vim_strchr(p_cpo, CPO_SEARCH) == NULL
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ако (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		break;
	    }
	    ако (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    инако
		++matchcol;
	}
	инако
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	ако (shl->rm.regprog != NULL)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    цео regprog_is_copy = (shl != search_hl && cur != NULL
			  && shl == &cur->mit_hl
			  && cur->mit_match.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_out);
	    // Copy the regprog, in case it got freed and recompiled.
	    ако (regprog_is_copy)
		cur->mit_match.regprog = cur->mit_hl.rm.regprog;

	    ако (called_emsg > called_emsg_before || got_int || timed_out)
	    {
		// Error while handling regexp: stop using this regexp.
		ако (shl == search_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    set_no_hlsearch(TRUE);
		}
		shl->rm.regprog = NULL;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		break;
	    }
	}
	инако ако (cur != NULL)
	    nmatched = next_search_hl_pos(shl, lnum, cur, matchcol);
	инако
	    nmatched = 0;
	ако (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    break;
	}
	ако (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    break;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    void
prepare_search_hl(win_T *wp, match_T *search_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    match_T	*shl;		// points to search_hl or a match
    цео		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    цео		pos_inprogress;	// marks that position match search is
				// in progress
    цео		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    док (cur != NULL || shl_flag == FALSE)
    {
	ако (shl_flag == FALSE)
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	инако
	    shl = &cur->mit_hl;
	ако (shl->rm.regprog != NULL
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ако (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		за (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    ако (hasFoldingWin(wp, shl->first_lnum - 1,
						      NULL, NULL, TRUE, NULL))
			break;
# инако
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    ако (cur != NULL)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    док (shl->first_lnum < lnum && (shl->rm.regprog != NULL
					  || (cur != NULL && pos_inprogress)))
	    {
		next_search_hl(wp, search_hl, shl, shl->first_lnum, (colnr_T)n,
					       shl == search_hl ? NULL : cur);
		pos_inprogress = cur == NULL || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ако (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		инако
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	ако (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    static void
check_cur_search_hl(win_T *wp, match_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ако (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.col >= shl->rm.startpos[0].col)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.col < shl->rm.endpos[0].col))
	shl->has_cursor = TRUE;
    инако
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    цео
prepare_search_hl_line(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **line,
	match_T	    *search_hl,
	цео	    *search_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    цео		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    цео		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    док (cur != NULL || shl_flag == FALSE)
    {
	ако (shl_flag == FALSE)
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	инако
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ако (cur != NULL)
	    cur->mit_pos_cur = 0;
	next_search_hl(wp, search_hl, shl, lnum, mincol,
						shl == search_hl ? NULL : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	ако (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ако (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    инако
		shl->startcol = 0;
	    ако (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    инако
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ако (shl == search_hl)
		check_cur_search_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ако (shl->startcol == shl->endcol)
	    {
		ако (has_mbyte && (*line)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*line) + shl->endcol);
		инако
		    ++shl->endcol;
	    }
	    ако ((long)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*search_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ако (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
    врати area_highlighting;
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
    цео
update_search_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    col,
	char_u	    **line,
	match_T	    *search_hl,
	цео	    *has_match_conc UNUSED,
	цео	    *match_conc UNUSED,
	цео	    did_line_attr,
	цео	    lcs_eol_one,
	цео	    *on_last_col)
{
    matchitem_T *cur;		    // points to the match list
    match_T	*shl;		    // points to search_hl or a match
    цео		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    цео		pos_inprogress;	    // marks that position match search is in
				    // progress
    цео		search_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    док (cur != NULL || shl_flag == FALSE)
    {
	ако (shl_flag == FALSE
		&& (cur == NULL
			|| cur->mit_priority > SEARCH_HL_PRIORITY))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	инако
	    shl = &cur->mit_hl;
	ако (cur != NULL)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	док (shl->rm.regprog != NULL || (cur != NULL && pos_inprogress))
	{
	    ако (shl->startcol != MAXCOL
		    && col >= shl->startcol
		    && col < shl->endcol)
	    {
		цео next_col = col + mb_ptr2len(*line + col);

		ако (shl->endcol < next_col)
		    shl->endcol = next_col;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ако (cur != NULL
			&& shl != search_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_match_conc = col == shl->startcol ? 2 : 1;
		    *match_conc = cur->mit_conceal_char;
		}
		инако
		    *has_match_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ако (shl == search_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ако (shl->attr_cur != shl->attr)
			search_hl_has_cursor_lnum = lnum;
		}

	    }
	    инако ако (col == shl->endcol)
	    {
		shl->attr_cur = 0;
		next_search_hl(wp, search_hl, shl, lnum, col,
					       shl == search_hl ? NULL : cur);
		pos_inprogress = !(cur == NULL || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*line = ml_get_buf(wp->w_buffer, lnum, FALSE);

		ако (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].col;
		    ако (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].col;
		    инако
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ако (shl == search_hl)
			check_cur_search_hl(wp, shl);

		    ако (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ако (has_mbyte)
			{
			    char_u *p = *line + shl->endcol;

			    ако (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    инако
				shl->endcol += (*mb_ptr2len)(p);
			}
			инако
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    continue;
		}
	    }
	    break;
	}
	ако (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    док (cur != NULL || shl_flag == FALSE)
    {
	ако (shl_flag == FALSE
		&& (cur == NULL ||
			cur->mit_priority > SEARCH_HL_PRIORITY))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	инако
	    shl = &cur->mit_hl;
	ако (shl->attr_cur != 0)
	{
	    search_attr = shl->attr_cur;
	    *on_last_col = col + 1 >= shl->endcol;
	}
	ако (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
    // Only highlight one character after the last column.
    ако (*(*line + col) == NUL && (did_line_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	search_attr = 0;
    врати search_attr;
}

    цео
get_prevcol_hl_flag(win_T *wp, match_T *search_hl, long curcol)
{
    long	prevcol = curcol;
    цео		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ако defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ако (popup_is_popup(wp))
	врати FALSE;
# endif

    // we're not really at that column when skipping some text
    ако ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ако (!search_hl->is_addpos && (prevcol == (long)search_hl->startcol
		|| (prevcol > (long)search_hl->startcol
					      && search_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    инако
    {
	cur = wp->w_match_head;
	док (cur != NULL)
	{
	    ако (!cur->mit_hl.is_addpos && (prevcol == (long)cur->mit_hl.startcol
			|| (prevcol > (long)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		break;
	    }
	    cur = cur->mit_next;
	}
    }
    врати prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    void
get_search_match_hl(win_T *wp, match_T *search_hl, long col, цео *char_attr)
{
    matchitem_T *cur;			// points to the match list
    match_T	*shl;			// points to search_hl or a match
    цео		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_match_head;
    shl_flag = WIN_IS_POPUP(wp);
    док (cur != NULL || shl_flag == FALSE)
    {
	ако (shl_flag == FALSE
		&& ((cur != NULL
			&& cur->mit_priority > SEARCH_HL_PRIORITY)
		    || cur == NULL))
	{
	    shl = search_hl;
	    shl_flag = TRUE;
	}
	инако
	    shl = &cur->mit_hl;
	ако (col - 1 == (long)shl->startcol
		&& (shl == search_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ако (shl != search_hl && cur != NULL)
	    cur = cur->mit_next;
    }
}

#endif // FEAT_SEARCH_EXTRA

#ако defined(FEAT_EVAL)
# ifdef FEAT_SEARCH_EXTRA
    static цео
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ако (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	врати FAIL;
    }

    ако (dict_has_key(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ако ((di = dict_find(tv->vval.v_dict, (char_u *)"window", -1)) == NULL)
	врати OK;

    *win = find_win_by_nr_or_id(&di->di_tv);
    ако (*win == NULL)
    {
	emsg(_(e_invalid_window_number));
	врати FAIL;
    }

    врати OK;
}
# endif

/*
 * "clearmatches()" function
 */
    void
f_clearmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    win_T   *win;

    ако (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	врати;

    win = get_optional_window(argvars, 0);
    ако (win != NULL)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    void
f_getmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    цео		i;
    win_T	*win;

    ако (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	врати;

    win = get_optional_window(argvars, 0);
    ако (rettv_list_alloc(rettv) == FAIL || win == NULL)
	врати;

    cur = win->w_match_head;
    док (cur != NULL)
    {
	dict = dict_alloc();
	ако (dict == NULL)
	    врати;
	ако (cur->mit_match.regprog == NULL)
	{
	    // match added with matchaddpos()
	    за (i = 0; i < cur->mit_pos_count; ++i)
	    {
		llpos_T	*llpos;
		char	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ако (llpos->lnum == 0)
		    break;
		l = list_alloc();
		ако (l == NULL)
		    break;
		list_append_number(l, (varnumber_T)llpos->lnum);
		ако (llpos->col > 0)
		{
		    list_append_number(l, (varnumber_T)llpos->col);
		    list_append_number(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ако (dict_add_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	инако
	{
	    dict_add_string(dict, "pattern", cur->mit_pattern);
	}
	dict_add_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_add_number(dict, "priority", (long)cur->mit_priority);
	dict_add_number(dict, "id", (long)cur->mit_id);
#  ако defined(FEAT_CONCEAL)
	ако (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    цео	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_add_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ако (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_next;
    }
# endif
}

/*
 * "setmatches()" function
 */
    void
f_setmatches(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = NULL;
    win_T	*win;

    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	врати;

    ако (check_for_list_arg(argvars, 0) == FAIL)
	врати;
    win = get_optional_window(argvars, 1);
    ако (win == NULL)
	врати;

    ако ((l = argvars[0].vval.v_list) != NULL)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_first;
	док (li != NULL)
	{
	    ако (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == NULL)
	    {
		emsg(_(e_invalid_argument));
		врати;
	    }
	    ако (!(dict_has_key(d, "group")
			&& (dict_has_key(d, "pattern")
			    || dict_has_key(d, "pos1"))
			&& dict_has_key(d, "priority")
			&& dict_has_key(d, "id")))
	    {
		emsg(_(e_invalid_argument));
		врати;
	    }
	    li = li->li_next;
	}

	clear_matches(win);
	li = l->lv_first;
	док (li != NULL)
	{
	    цео		i = 0;
	    char	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*group;
	    цео		priority;
	    цео		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ако (!dict_has_key(d, "pattern"))
	    {
		ако (s == NULL)
		{
		    s = list_alloc();
		    ако (s == NULL)
			врати;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		за (i = 1; i < 9; i++)
		{
		    sprintf((char *)buf, (char *)"pos%d", i);
		    ако ((di = dict_find(d, (char_u *)buf, -1)) != NULL)
		    {
			ако (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    врати;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    инако
			break;
		}
	    }

	    group = dict_get_string(d, "group", TRUE);
	    priority = (цео)dict_get_number(d, "priority");
	    id = (цео)dict_get_number(d, "id");
	    conceal = dict_has_key(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : NULL;
	    ако (i == 0)
	    {
		match_add(win, group,
		    dict_get_string(d, "pattern", FALSE),
		    priority, id, NULL, conceal);
	    }
	    инако
	    {
		match_add(win, group, NULL, priority, id, s, conceal);
		list_unref(s);
		s = NULL;
	    }
	    vim_free(group);
	    vim_free(conceal);

	    li = li->li_next;
	}
	rettv->vval.v_number = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    void
f_matchadd(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		// group
    char_u	*pat;		// pattern
    цео		prio = 10;	// default priority
    цео		id = -1;
    цео		error = FALSE;
    char_u	*conceal_char = NULL;
    win_T	*win = curwin;

    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	врати;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	// group
    pat = tv_get_string_buf_chk(&argvars[1], buf);	// pattern
    ако (grp == NULL || pat == NULL)
	врати;
    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (цео)tv_get_number_chk(&argvars[2], &error);
	ако (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (цео)tv_get_number_chk(&argvars[3], &error);
	    ако (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		врати;
	}
    }
    ако (error == TRUE)
	врати;
    ако (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	врати;
    }

    rettv->vval.v_number = match_add(win, grp, pat, prio, id, NULL,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    void
f_matchaddpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*group;
    цео		prio = 10;
    цео		id = -1;
    цео		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = NULL;
    win_T	*win = curwin;

    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	врати;

    group = tv_get_string_buf_chk(&argvars[0], buf);
    ако (group == NULL)
	врати;

    ако (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "matchaddpos()");
	врати;
    }
    l = argvars[1].vval.v_list;
    ако (l == NULL || l->lv_len == 0)
	врати;

    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (цео)tv_get_number_chk(&argvars[2], &error);
	ако (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (цео)tv_get_number_chk(&argvars[3], &error);

	    ако (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		врати;
	}
    }
    ако (error == TRUE)
	врати;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ако (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_match_nr), id);
	врати;
    }

    rettv->vval.v_number = match_add(win, group, NULL, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    void
f_matcharg(typval_T *argvars UNUSED, typval_T *rettv)
{
    ако (rettv_list_alloc(rettv) != OK)
	врати;

# ifdef FEAT_SEARCH_EXTRA
    цео	    id;
    matchitem_T *m;

    ако (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	врати;

    id = (цео)tv_get_number(&argvars[0]);
    ако (id >= 1 && id <= 3)
    {
	ако ((m = get_match(curwin, id)) != NULL)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_pattern, -1);
	}
	инако
	{
	    list_append_string(rettv->vval.v_list, NULL, -1);
	    list_append_string(rettv->vval.v_list, NULL, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    void
f_matchdelete(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_SEARCH_EXTRA
    win_T   *win;

    ако (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	врати;

    win = get_optional_window(argvars, 1);
    ако (win == NULL)
	rettv->vval.v_number = -1;
    инако
	rettv->vval.v_number = match_delete(win,
				       (цео)tv_get_number(&argvars[0]), TRUE);
# endif
}
#endif

#ако defined(FEAT_SEARCH_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    void
ex_match(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = NULL;
    char_u	*end;
    цео		c;
    цео		id;

    ако (eap->line2 <= 3)
	id = eap->line2;
    инако
    {
	emsg(_(e_invalid_command));
	врати;
    }

    // First clear any old pattern.
    ако (!eap->skip)
	match_delete(curwin, id, FALSE);

    ако (ends_excmd2(eap->cmd, eap->arg))
	end = eap->arg;
    инако ако ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ends_excmd2(eap->arg, eap->arg + 4))))
	end = eap->arg + 4;
    инако
    {
	p = skiptowhite(eap->arg);
	ако (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ако (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_argument_str), eap->arg);
	    врати;
	}
	end = skip_regexp(p + 1, *p, TRUE);
	ако (!eap->skip)
	{
	    ако (*end != NUL && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, end);
		врати;
	    }
	    ако (*end != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_argument_str), p);
		врати;
	    }

	    c = *end;
	    *end = NUL;
	    match_add(curwin, g, p + 1, 10, id, NULL, NULL);
	    vim_free(g);
	    *end = c;
	}
    }
    eap->nextcmd = find_nextcmd(end);
}
#endif
