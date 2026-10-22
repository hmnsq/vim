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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)

# define ਖੋਜ_HL_ਤਰਜੀਹ 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_ਜੋੜੋ(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    ਪੂਰਨ		prio,
    ਪੂਰਨ		id,
    list_T	*pos_list,
    char_u      *conceal_char ਅਣਵਰਤਿਆ) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ਨਿਰਾ;
    ਪੂਰਨ		hlg_id;
    regprog_T	*regprog = ਨਿਰਾ;
    ਪੂਰਨ		rtype = UPD_SOME_VALID;

    ਜੇ (*grp == NUL || (pat != ਨਿਰਾ && *pat == NUL))
	ਵਾਪਸ -1;
    ਜੇ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_ਮੰਜ਼ਿਲ_one_1), id);
	ਵਾਪਸ -1;
    }
    ਜੇ (id == -1)
    {
	// use the next available match ID
	id = wp->w_ਅਗਲਾ_ਮੇਲ_id++;
    }
    ਨਹੀਂਤਾਂ
    {
	// check the given ID is not already in use
	ਲਈ (cur = wp->w_ਮੇਲ_ਸਿਰ; cur != ਨਿਰਾ; cur = cur->mit_ਅਗਲਾ)
	    ਜੇ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_ਲਿਆ_nr), id);
		ਵਾਪਸ -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ਜੇ (wp->w_ਅਗਲਾ_ਮੇਲ_id < id + 100)
	    wp->w_ਅਗਲਾ_ਮੇਲ_id = id + 100;
    }

    ਜੇ ((hlg_id = syn_namen2id(grp, (ਪੂਰਨ)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_ਸਮੂਹ_name_str), grp);
	ਵਾਪਸ -1;
    }
    ਜੇ (pat != ਨਿਰਾ && (regprog = vim_regcomp(pat, RE_MAGIC)) == ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), pat);
	ਵਾਪਸ -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ਜੇ (m == ਨਿਰਾ)
	ਜਾਓ fail;
    ਜੇ (pos_list != ਨਿਰਾ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ਜੇ (m->mit_pos_array == ਨਿਰਾ)
	    ਜਾਓ fail;
	m->mit_pos_ਗਿਣਤੀ = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_ਤਰਜੀਹ = prio;
    m->mit_ਨਮੂਨਾ = pat == ਨਿਰਾ ? ਨਿਰਾ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_ਮੇਲ.regprog = regprog;
    m->mit_ਮੇਲ.rmm_ic = FALSE;
    m->mit_ਮੇਲ.rmm_maxcol = 0;
# ਜੇ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ਜੇ (conceal_char != ਨਿਰਾ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ਜੇ (pos_list != ਨਿਰਾ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	ਪੂਰਨ		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	ਲਈ (i = 0, li = pos_list->lv_ਪਹਿਲਾ; li != ਨਿਰਾ; i++, li = li->li_ਅਗਲਾ)
	{
	    linenr_T	lnum = 0;
	    colnr_T	ਖਾਨਾ = 0;
	    ਪੂਰਨ		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    ਪੂਰਨ		error = FALSE;

	    ਜੇ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ਜੇ (subl == ਨਿਰਾ)
		    ਜਾਓ fail;
		subli = subl->lv_ਪਹਿਲਾ;
		ਜੇ (subli == ਨਿਰਾ)
		    ਜਾਓ fail;
		lnum = tv_get_ਸੰਖਿਆ_chk(&subli->li_tv, &error);
		ਜੇ (error == TRUE)
		    ਜਾਓ fail;
		ਜੇ (lnum == 0)
		{
		    --i;
		    ਜਾਰੀ;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_ਅਗਲਾ;
		ਜੇ (subli != ਨਿਰਾ)
		{
		    ਖਾਨਾ = tv_get_ਸੰਖਿਆ_chk(&subli->li_tv, &error);
		    ਜੇ (error == TRUE)
			ਜਾਓ fail;
		    subli = subli->li_ਅਗਲਾ;
		    ਜੇ (subli != ਨਿਰਾ)
		    {
			len = tv_get_ਸੰਖਿਆ_chk(&subli->li_tv, &error);
			ਜੇ (error == TRUE)
			    ਜਾਓ fail;
		    }
		}
		m->mit_pos_array[i].ਖਾਨਾ = ਖਾਨਾ;
		m->mit_pos_array[i].len = len;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (li->li_tv.v_type == VAR_ਸੰਖਿਆ)
	    {
		ਜੇ (li->li_tv.vval.v_ਸੰਖਿਆ == 0)
		{
		    --i;
		    ਜਾਰੀ;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_ਸੰਖਿਆ;
		m->mit_pos_array[i].ਖਾਨਾ = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		emsg(_(e_list_or_ਸੰਖਿਆ_required));
		ਜਾਓ fail;
	    }
	    ਜੇ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ਜੇ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ਜੇ (toplnum != 0)
	{
	    redraw_win_range_later(wp, toplnum, botlnum);
	    m->mit_toplnum = toplnum;
	    m->mit_botlnum = botlnum;
	    rtype = UPD_VALID;
	}
    }

    // Insert new match.  The match list is in ascending order with regard to
    // the match priorities.
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    prev = cur;
    ਜਦੋਂ (cur != ਨਿਰਾ && prio >= cur->mit_ਤਰਜੀਹ)
    {
	prev = cur;
	cur = cur->mit_ਅਗਲਾ;
    }
    ਜੇ (cur == prev)
	wp->w_ਮੇਲ_ਸਿਰ = m;
    ਨਹੀਂਤਾਂ
	prev->mit_ਅਗਲਾ = m;
    m->mit_ਅਗਲਾ = cur;

    redraw_win_later(wp, rtype);
    ਵਾਪਸ id;

fail:
    vim_regfree(regprog);
    ਜੇ (m != ਨਿਰਾ)
    {
	vim_free(m->mit_ਨਮੂਨਾ);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    ਵਾਪਸ -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_delete(win_T *wp, ਪੂਰਨ id, ਪੂਰਨ perr)
{
    matchitem_T	*cur = wp->w_ਮੇਲ_ਸਿਰ;
    matchitem_T	*prev = cur;
    ਪੂਰਨ		rtype = UPD_SOME_VALID;

    ਜੇ (id < 1)
    {
	ਜੇ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_ਮੰਜ਼ਿਲ_one_2),
									   id);
	ਵਾਪਸ -1;
    }
    ਜਦੋਂ (cur != ਨਿਰਾ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_ਅਗਲਾ;
    }
    ਜੇ (cur == ਨਿਰਾ)
    {
	ਜੇ (perr == TRUE)
	    semsg(_(e_id_not_ਮਿਲਿਆ_nr), id);
	ਵਾਪਸ -1;
    }
    ਜੇ (cur == prev)
	wp->w_ਮੇਲ_ਸਿਰ = cur->mit_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	prev->mit_ਅਗਲਾ = cur->mit_ਅਗਲਾ;
    vim_regfree(cur->mit_ਮੇਲ.regprog);
    vim_free(cur->mit_ਨਮੂਨਾ);
    ਜੇ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    ਵਾਪਸ 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    ਖਾਲੀ
clear_matches(win_T *wp)
{
    matchitem_T *m;

    ਜਦੋਂ (wp->w_ਮੇਲ_ਸਿਰ != ਨਿਰਾ)
    {
	m = wp->w_ਮੇਲ_ਸਿਰ->mit_ਅਗਲਾ;
	vim_regfree(wp->w_ਮੇਲ_ਸਿਰ->mit_ਮੇਲ.regprog);
	vim_free(wp->w_ਮੇਲ_ਸਿਰ->mit_ਨਮੂਨਾ);
	vim_free(wp->w_ਮੇਲ_ਸਿਰ->mit_pos_array);
	vim_free(wp->w_ਮੇਲ_ਸਿਰ);
	wp->w_ਮੇਲ_ਸਿਰ = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    ਸਥਿਰ matchitem_T *
get_ਮੇਲ(win_T *wp, ਪੂਰਨ id)
{
    matchitem_T *cur = wp->w_ਮੇਲ_ਸਿਰ;

    ਜਦੋਂ (cur != ਨਿਰਾ && cur->mit_id != id)
	cur = cur->mit_ਅਗਲਾ;
    ਵਾਪਸ cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    ਖਾਲੀ
init_ਖੋਜ_hl(win_T *wp, ਮੇਲ_T *ਖੋਜ_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //ਮੇਲ
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    ਜਦੋਂ (cur != ਨਿਰਾ)
    {
	cur->mit_hl.rm = cur->mit_ਮੇਲ;
	ਜੇ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ਨਹੀਂਤਾਂ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_ਬਫਰ;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.ਪਹਿਲਾ_lnum = 0;
	cur = cur->mit_ਅਗਲਾ;
    }
    // Must update this every time since highlight group override can change it.
    ਖੋਜ_hl->attr = HL_ATTR(HLF_L);

    ਖੋਜ_hl->buf = wp->w_ਬਫਰ;
    ਖੋਜ_hl->lnum = 0;
    ਖੋਜ_hl->ਪਹਿਲਾ_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
ਅਗਲਾ_ਖੋਜ_hl_pos(
    ਮੇਲ_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *ਮੇਲ,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ਮਿਲਿਆ = -1;

    ਲਈ (i = ਮੇਲ->mit_pos_cur; i < ਮੇਲ->mit_pos_ਗਿਣਤੀ; i++)
    {
	llpos_T	*pos = &ਮੇਲ->mit_pos_array[i];

	ਜੇ (pos->lnum == 0)
	    ਰੋਕੋ;
	ਜੇ (pos->len == 0 && pos->ਖਾਨਾ < mincol)
	    ਜਾਰੀ;
	ਜੇ (pos->lnum == lnum)
	{
	    ਜੇ (ਮਿਲਿਆ >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ਜੇ (pos->ਖਾਨਾ < ਮੇਲ->mit_pos_array[ਮਿਲਿਆ].ਖਾਨਾ)
		{
		    llpos_T	tmp = *pos;

		    *pos = ਮੇਲ->mit_pos_array[ਮਿਲਿਆ];
		    ਮੇਲ->mit_pos_array[ਮਿਲਿਆ] = tmp;
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਮਿਲਿਆ = i;
	}
    }
    ਮੇਲ->mit_pos_cur = 0;
    ਜੇ (ਮਿਲਿਆ >= 0)
    {
	colnr_T	ਸ਼ੁਰੂ = ਮੇਲ->mit_pos_array[ਮਿਲਿਆ].ਖਾਨਾ == 0
				     ? 0 : ਮੇਲ->mit_pos_array[ਮਿਲਿਆ].ਖਾਨਾ - 1;
	colnr_T	ਸਿਰਾ = ਮੇਲ->mit_pos_array[ਮਿਲਿਆ].ਖਾਨਾ == 0
			    ? MAXCOL : ਸ਼ੁਰੂ + ਮੇਲ->mit_pos_array[ਮਿਲਿਆ].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].ਖਾਨਾ = ਸ਼ੁਰੂ;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].ਖਾਨਾ = ਸਿਰਾ;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	ਮੇਲ->mit_pos_cur = ਮਿਲਿਆ + 1;
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ਸਥਿਰ ਖਾਲੀ
ਅਗਲਾ_ਖੋਜ_hl(
    win_T	    *win,
    ਮੇਲ_T	    *ਖੋਜ_hl,
    ਮੇਲ_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    ਲੰਮਾ	nmatched;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    ਪੂਰਨ		timed_ਬਾਹਰਲਾ = FALSE;

    // for :{range}s/pat only highlight inside the range
    ਜੇ ((lnum < ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ || lnum > ਖੋਜ_ਆਖਰੀ_ਸਤਰ) && cur == ਨਿਰਾ)
    {
	shl->lnum = 0;
	ਵਾਪਸ;
    }

    ਜੇ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ਜੇ (lnum > l)
	    shl->lnum = 0;
	ਨਹੀਂਤਾਂ ਜੇ (lnum < l || shl->rm.endpos[0].ਖਾਨਾ > mincol)
	    ਵਾਪਸ;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    ਲਈ (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ਜੇ (shl->lnum == 0)
	    matchcol = 0;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(p_cpo, CPO_ਖੋਜ) == ਨਿਰਾ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].ਖਾਨਾ <= shl->rm.startpos[0].ਖਾਨਾ))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].ਖਾਨਾ;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ਜੇ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ਰੋਕੋ;
	    }
	    ਜੇ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ਨਹੀਂਤਾਂ
		++matchcol;
	}
	ਨਹੀਂਤਾਂ
	    matchcol = shl->rm.endpos[0].ਖਾਨਾ;

	shl->lnum = lnum;
	ਜੇ (shl->rm.regprog != ਨਿਰਾ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    ਪੂਰਨ regprog_is_copy = (shl != ਖੋਜ_hl && cur != ਨਿਰਾ
			  && shl == &cur->mit_hl
			  && cur->mit_ਮੇਲ.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_ਬਾਹਰਲਾ);
	    // Copy the regprog, in case it got freed and recompiled.
	    ਜੇ (regprog_is_copy)
		cur->mit_ਮੇਲ.regprog = cur->mit_hl.rm.regprog;

	    ਜੇ (called_emsg > called_emsg_before || got_int || timed_ਬਾਹਰਲਾ)
	    {
		// Error while handling regexp: stop using this regexp.
		ਜੇ (shl == ਖੋਜ_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    ਸੈੱਟ_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ਨਿਰਾ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cur != ਨਿਰਾ)
	    nmatched = ਅਗਲਾ_ਖੋਜ_hl_pos(shl, lnum, cur, matchcol);
	ਨਹੀਂਤਾਂ
	    nmatched = 0;
	ਜੇ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ਰੋਕੋ;
	}
	ਜੇ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].ਖਾਨਾ >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].ਖਾਨਾ > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ਰੋਕੋ;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    ਖਾਲੀ
prepare_ਖੋਜ_hl(win_T *wp, ਮੇਲ_T *ਖੋਜ_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    ਮੇਲ_T	*shl;		// points to search_hl or a match
    ਪੂਰਨ		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    ਪੂਰਨ		pos_inprogress;	// marks that position match search is
				// in progress
    ਪੂਰਨ		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    ਜਦੋਂ (cur != ਨਿਰਾ || shl_flag == FALSE)
    {
	ਜੇ (shl_flag == FALSE)
	{
	    shl = ਖੋਜ_hl;
	    shl_flag = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    shl = &cur->mit_hl;
	ਜੇ (shl->rm.regprog != ਨਿਰਾ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ਜੇ (shl->ਪਹਿਲਾ_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		ਲਈ (shl->ਪਹਿਲਾ_lnum = lnum;
			   shl->ਪਹਿਲਾ_lnum > wp->w_topline; --shl->ਪਹਿਲਾ_lnum)
		    ਜੇ (hasFoldingWin(wp, shl->ਪਹਿਲਾ_lnum - 1,
						      ਨਿਰਾ, ਨਿਰਾ, TRUE, ਨਿਰਾ))
			ਰੋਕੋ;
# ਨਹੀਂਤਾਂ
		shl->ਪਹਿਲਾ_lnum = wp->w_topline;
# endif
	    }
	    ਜੇ (cur != ਨਿਰਾ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    ਜਦੋਂ (shl->ਪਹਿਲਾ_lnum < lnum && (shl->rm.regprog != ਨਿਰਾ
					  || (cur != ਨਿਰਾ && pos_inprogress)))
	    {
		ਅਗਲਾ_ਖੋਜ_hl(wp, ਖੋਜ_hl, shl, shl->ਪਹਿਲਾ_lnum, (colnr_T)n,
					       shl == ਖੋਜ_hl ? ਨਿਰਾ : cur);
		pos_inprogress = cur == ਨਿਰਾ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ਜੇ (shl->lnum != 0)
		{
		    shl->ਪਹਿਲਾ_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].ਖਾਨਾ;
		}
		ਨਹੀਂਤਾਂ
		{
		    ++shl->ਪਹਿਲਾ_lnum;
		    n = 0;
		}
	    }
	}
	ਜੇ (shl != ਖੋਜ_hl && cur != ਨਿਰਾ)
	    cur = cur->mit_ਅਗਲਾ;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ਸਥਿਰ ਖਾਲੀ
check_cur_ਖੋਜ_hl(win_T *wp, ਮੇਲ_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ਜੇ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.ਖਾਨਾ >= shl->rm.startpos[0].ਖਾਨਾ)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.ਖਾਨਾ < shl->rm.endpos[0].ਖਾਨਾ))
	shl->has_cursor = TRUE;
    ਨਹੀਂਤਾਂ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    ਪੂਰਨ
prepare_ਖੋਜ_hl_ਸਤਰ(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **ਸਤਰ,
	ਮੇਲ_T	    *ਖੋਜ_hl,
	ਪੂਰਨ	    *ਖੋਜ_attr)
{
    matchitem_T *cur;			// points to the match list
    ਮੇਲ_T	*shl;			// points to search_hl or a match
    ਪੂਰਨ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    ਪੂਰਨ		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    shl_flag = WIN_IS_POPUP(wp);
    ਜਦੋਂ (cur != ਨਿਰਾ || shl_flag == FALSE)
    {
	ਜੇ (shl_flag == FALSE)
	{
	    shl = ਖੋਜ_hl;
	    shl_flag = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ਜੇ (cur != ਨਿਰਾ)
	    cur->mit_pos_cur = 0;
	ਅਗਲਾ_ਖੋਜ_hl(wp, ਖੋਜ_hl, shl, lnum, mincol,
						shl == ਖੋਜ_hl ? ਨਿਰਾ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

	ਜੇ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ਜੇ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
		shl->startcol = 0;
	    ਜੇ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ਜੇ (shl == ਖੋਜ_hl)
		check_cur_ਖੋਜ_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ਜੇ (shl->startcol == shl->endcol)
	    {
		ਜੇ (has_mbyte && (*ਸਤਰ)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*ਸਤਰ) + shl->endcol);
		ਨਹੀਂਤਾਂ
		    ++shl->endcol;
	    }
	    ਜੇ ((ਲੰਮਾ)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*ਖੋਜ_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ਜੇ (shl != ਖੋਜ_hl && cur != ਨਿਰਾ)
	    cur = cur->mit_ਅਗਲਾ;
    }
    ਵਾਪਸ area_highlighting;
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
    ਪੂਰਨ
update_ਖੋਜ_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    ਖਾਨਾ,
	char_u	    **ਸਤਰ,
	ਮੇਲ_T	    *ਖੋਜ_hl,
	ਪੂਰਨ	    *has_ਮੇਲ_conc ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	    *ਮੇਲ_conc ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	    did_ਸਤਰ_attr,
	ਪੂਰਨ	    lcs_eol_one,
	ਪੂਰਨ	    *on_ਆਖਰੀ_ਖਾਨਾ)
{
    matchitem_T *cur;		    // points to the match list
    ਮੇਲ_T	*shl;		    // points to search_hl or a match
    ਪੂਰਨ		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    ਪੂਰਨ		pos_inprogress;	    // marks that position match search is in
				    //ਤਰੱਕੀ
    ਪੂਰਨ		ਖੋਜ_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    shl_flag = WIN_IS_POPUP(wp);
    ਜਦੋਂ (cur != ਨਿਰਾ || shl_flag == FALSE)
    {
	ਜੇ (shl_flag == FALSE
		&& (cur == ਨਿਰਾ
			|| cur->mit_ਤਰਜੀਹ > ਖੋਜ_HL_ਤਰਜੀਹ))
	{
	    shl = ਖੋਜ_hl;
	    shl_flag = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    shl = &cur->mit_hl;
	ਜੇ (cur != ਨਿਰਾ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	ਜਦੋਂ (shl->rm.regprog != ਨਿਰਾ || (cur != ਨਿਰਾ && pos_inprogress))
	{
	    ਜੇ (shl->startcol != MAXCOL
		    && ਖਾਨਾ >= shl->startcol
		    && ਖਾਨਾ < shl->endcol)
	    {
		ਪੂਰਨ ਅਗਲਾ_ਖਾਨਾ = ਖਾਨਾ + mb_ptr2len(*ਸਤਰ + ਖਾਨਾ);

		ਜੇ (shl->endcol < ਅਗਲਾ_ਖਾਨਾ)
		    shl->endcol = ਅਗਲਾ_ਖਾਨਾ;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ਜੇ (cur != ਨਿਰਾ
			&& shl != ਖੋਜ_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_ਮੇਲ_conc = ਖਾਨਾ == shl->startcol ? 2 : 1;
		    *ਮੇਲ_conc = cur->mit_conceal_char;
		}
		ਨਹੀਂਤਾਂ
		    *has_ਮੇਲ_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ਜੇ (shl == ਖੋਜ_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ਜੇ (shl->attr_cur != shl->attr)
			ਖੋਜ_hl_has_cursor_lnum = lnum;
		}

	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ == shl->endcol)
	    {
		shl->attr_cur = 0;
		ਅਗਲਾ_ਖੋਜ_hl(wp, ਖੋਜ_hl, shl, lnum, ਖਾਨਾ,
					       shl == ਖੋਜ_hl ? ਨਿਰਾ : cur);
		pos_inprogress = !(cur == ਨਿਰਾ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

		ਜੇ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].ਖਾਨਾ;
		    ਜੇ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].ਖਾਨਾ;
		    ਨਹੀਂਤਾਂ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ਜੇ (shl == ਖੋਜ_hl)
			check_cur_ਖੋਜ_hl(wp, shl);

		    ਜੇ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ਜੇ (has_mbyte)
			{
			    char_u *p = *ਸਤਰ + shl->endcol;

			    ਜੇ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    ਨਹੀਂਤਾਂ
				shl->endcol += (*mb_ptr2len)(p);
			}
			ਨਹੀਂਤਾਂ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    ਜਾਰੀ;
		}
	    }
	    ਰੋਕੋ;
	}
	ਜੇ (shl != ਖੋਜ_hl && cur != ਨਿਰਾ)
	    cur = cur->mit_ਅਗਲਾ;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_ਮੇਲ_ਸਿਰ;
    shl_flag = WIN_IS_POPUP(wp);
    ਜਦੋਂ (cur != ਨਿਰਾ || shl_flag == FALSE)
    {
	ਜੇ (shl_flag == FALSE
		&& (cur == ਨਿਰਾ ||
			cur->mit_ਤਰਜੀਹ > ਖੋਜ_HL_ਤਰਜੀਹ))
	{
	    shl = ਖੋਜ_hl;
	    shl_flag = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    shl = &cur->mit_hl;
	ਜੇ (shl->attr_cur != 0)
	{
	    ਖੋਜ_attr = shl->attr_cur;
	    *on_ਆਖਰੀ_ਖਾਨਾ = ਖਾਨਾ + 1 >= shl->endcol;
	}
	ਜੇ (shl != ਖੋਜ_hl && cur != ਨਿਰਾ)
	    cur = cur->mit_ਅਗਲਾ;
    }
    // Only highlight one character after the last column.
    ਜੇ (*(*ਸਤਰ + ਖਾਨਾ) == NUL && (did_ਸਤਰ_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	ਖੋਜ_attr = 0;
    ਵਾਪਸ ਖੋਜ_attr;
}

    ਪੂਰਨ
get_prevcol_hl_flag(win_T *wp, ਮੇਲ_T *ਖੋਜ_hl, ਲੰਮਾ curcol)
{
    ਲੰਮਾ	prevcol = curcol;
    ਪੂਰਨ		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ਜੇ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ਜੇ (popup_is_popup(wp))
	ਵਾਪਸ FALSE;
# endif

    // we're not really at that column when skipping some text
    ਜੇ ((ਲੰਮਾ)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ਜੇ (!ਖੋਜ_hl->is_addpos && (prevcol == (ਲੰਮਾ)ਖੋਜ_hl->startcol
		|| (prevcol > (ਲੰਮਾ)ਖੋਜ_hl->startcol
					      && ਖੋਜ_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ਨਹੀਂਤਾਂ
    {
	cur = wp->w_ਮੇਲ_ਸਿਰ;
	ਜਦੋਂ (cur != ਨਿਰਾ)
	{
	    ਜੇ (!cur->mit_hl.is_addpos && (prevcol == (ਲੰਮਾ)cur->mit_hl.startcol
			|| (prevcol > (ਲੰਮਾ)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ਰੋਕੋ;
	    }
	    cur = cur->mit_ਅਗਲਾ;
	}
    }
    ਵਾਪਸ prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    ਖਾਲੀ
get_ਖੋਜ_ਮੇਲ_hl(win_T *wp, ਮੇਲ_T *ਖੋਜ_hl, ਲੰਮਾ ਖਾਨਾ, ਪੂਰਨ *char_attr)
{
    matchitem_T *cur;			// points to the match list
    ਮੇਲ_T	*shl;			// points to search_hl or a match
    ਪੂਰਨ		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_ਮੇਲ_ਸਿਰ;
    shl_flag = WIN_IS_POPUP(wp);
    ਜਦੋਂ (cur != ਨਿਰਾ || shl_flag == FALSE)
    {
	ਜੇ (shl_flag == FALSE
		&& ((cur != ਨਿਰਾ
			&& cur->mit_ਤਰਜੀਹ > ਖੋਜ_HL_ਤਰਜੀਹ)
		    || cur == ਨਿਰਾ))
	{
	    shl = ਖੋਜ_hl;
	    shl_flag = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    shl = &cur->mit_hl;
	ਜੇ (ਖਾਨਾ - 1 == (ਲੰਮਾ)shl->startcol
		&& (shl == ਖੋਜ_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ਜੇ (shl != ਖੋਜ_hl && cur != ਨਿਰਾ)
	    cur = cur->mit_ਅਗਲਾ;
    }
}

#ਜੇ_ਅੰਤ // FEAT_SEARCH_EXTRA

#ਜੇ defined(FEAT_EVAL)
# ifdef FEAT_ਖੋਜ_EXTRA
    ਸਥਿਰ ਪੂਰਨ
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ਜੇ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (dict_has_ਕੁੰਜੀ(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ਜੇ ((di = dict_ਲੱਭੋ(tv->vval.v_dict, (char_u *)"window", -1)) == ਨਿਰਾ)
	ਵਾਪਸ OK;

    *win = ਲੱਭੋ_win_by_nr_or_id(&di->di_tv);
    ਜੇ (*win == ਨਿਰਾ)
    {
	emsg(_(e_invalid_ਖਿੜਕੀ_ਸੰਖਿਆ));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}
# endif

/*
 * "clearmatches()" function
 */
    ਖਾਲੀ
f_clearmatches(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    win_T   *win;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    win = get_optional_ਖਿੜਕੀ(argvars, 0);
    ਜੇ (win != ਨਿਰਾ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    ਖਾਲੀ
f_getmatches(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    ਪੂਰਨ		i;
    win_T	*win;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    win = get_optional_ਖਿੜਕੀ(argvars, 0);
    ਜੇ (rettv_list_alloc(rettv) == FAIL || win == ਨਿਰਾ)
	ਵਾਪਸ;

    cur = win->w_ਮੇਲ_ਸਿਰ;
    ਜਦੋਂ (cur != ਨਿਰਾ)
    {
	dict = dict_alloc();
	ਜੇ (dict == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (cur->mit_ਮੇਲ.regprog == ਨਿਰਾ)
	{
	    // match added with matchaddpos()
	    ਲਈ (i = 0; i < cur->mit_pos_ਗਿਣਤੀ; ++i)
	    {
		llpos_T	*llpos;
		ਅੱਖਰ	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ਜੇ (llpos->lnum == 0)
		    ਰੋਕੋ;
		l = list_alloc();
		ਜੇ (l == ਨਿਰਾ)
		    ਰੋਕੋ;
		list_append_ਸੰਖਿਆ(l, (varnumber_T)llpos->lnum);
		ਜੇ (llpos->ਖਾਨਾ > 0)
		{
		    list_append_ਸੰਖਿਆ(l, (varnumber_T)llpos->ਖਾਨਾ);
		    list_append_ਸੰਖਿਆ(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ਜੇ (dict_ਜੋੜੋ_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    dict_ਜੋੜੋ_string(dict, "pattern", cur->mit_ਨਮੂਨਾ);
	}
	dict_ਜੋੜੋ_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "priority", (ਲੰਮਾ)cur->mit_ਤਰਜੀਹ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", (ਲੰਮਾ)cur->mit_id);
#  ਜੇ defined(FEAT_CONCEAL)
	ਜੇ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    ਪੂਰਨ	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_ਜੋੜੋ_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ਜੇ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_ਅਗਲਾ;
    }
# endif
}

/*
 * "setmatches()" function
 */
    ਖਾਲੀ
f_setmatches(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ਨਿਰਾ;
    win_T	*win;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    win = get_optional_ਖਿੜਕੀ(argvars, 1);
    ਜੇ (win == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ ((l = argvars[0].vval.v_list) != ਨਿਰਾ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_ਪਹਿਲਾ;
	ਜਦੋਂ (li != ਨਿਰਾ)
	{
	    ਜੇ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ਨਿਰਾ)
	    {
		emsg(_(e_invalid_ਦਲੀਲ));
		ਵਾਪਸ;
	    }
	    ਜੇ (!(dict_has_ਕੁੰਜੀ(d, "group")
			&& (dict_has_ਕੁੰਜੀ(d, "pattern")
			    || dict_has_ਕੁੰਜੀ(d, "pos1"))
			&& dict_has_ਕੁੰਜੀ(d, "priority")
			&& dict_has_ਕੁੰਜੀ(d, "id")))
	    {
		emsg(_(e_invalid_ਦਲੀਲ));
		ਵਾਪਸ;
	    }
	    li = li->li_ਅਗਲਾ;
	}

	clear_matches(win);
	li = l->lv_ਪਹਿਲਾ;
	ਜਦੋਂ (li != ਨਿਰਾ)
	{
	    ਪੂਰਨ		i = 0;
	    ਅੱਖਰ	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*ਸਮੂਹ;
	    ਪੂਰਨ		ਤਰਜੀਹ;
	    ਪੂਰਨ		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ਜੇ (!dict_has_ਕੁੰਜੀ(d, "pattern"))
	    {
		ਜੇ (s == ਨਿਰਾ)
		{
		    s = list_alloc();
		    ਜੇ (s == ਨਿਰਾ)
			ਵਾਪਸ;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		ਲਈ (i = 1; i < 9; i++)
		{
		    sprintf((ਅੱਖਰ *)buf, (ਅੱਖਰ *)"pos%d", i);
		    ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)buf, -1)) != ਨਿਰਾ)
		    {
			ਜੇ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    ਵਾਪਸ;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ਨਹੀਂਤਾਂ
			ਰੋਕੋ;
		}
	    }

	    ਸਮੂਹ = dict_get_string(d, "group", TRUE);
	    ਤਰਜੀਹ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "priority");
	    id = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "id");
	    conceal = dict_has_ਕੁੰਜੀ(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ਨਿਰਾ;
	    ਜੇ (i == 0)
	    {
		ਮੇਲ_ਜੋੜੋ(win, ਸਮੂਹ,
		    dict_get_string(d, "pattern", FALSE),
		    ਤਰਜੀਹ, id, ਨਿਰਾ, conceal);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਮੇਲ_ਜੋੜੋ(win, ਸਮੂਹ, ਨਿਰਾ, ਤਰਜੀਹ, id, s, conceal);
		list_unref(s);
		s = ਨਿਰਾ;
	    }
	    vim_free(ਸਮੂਹ);
	    vim_free(conceal);

	    li = li->li_ਅਗਲਾ;
	}
	rettv->vval.v_ਸੰਖਿਆ = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    ਖਾਲੀ
f_matchadd(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//ਸਮੂਹ
    char_u	*pat;		//ਨਮੂਨਾ
    ਪੂਰਨ		prio = 10;	// default priority
    ਪੂਰਨ		id = -1;
    ਪੂਰਨ		error = FALSE;
    char_u	*conceal_char = ਨਿਰਾ;
    win_T	*win = curwin;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ਵਾਪਸ;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//ਸਮੂਹ
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//ਨਮੂਨਾ
    ਜੇ (grp == ਨਿਰਾ || pat == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], &error);
	    ਜੇ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ਵਾਪਸ;
	}
    }
    ਜੇ (error == TRUE)
	ਵਾਪਸ;
    ਜੇ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_ਮੇਲ_nr), id);
	ਵਾਪਸ;
    }

    rettv->vval.v_ਸੰਖਿਆ = ਮੇਲ_ਜੋੜੋ(win, grp, pat, prio, id, ਨਿਰਾ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    ਖਾਲੀ
f_matchaddpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*ਸਮੂਹ;
    ਪੂਰਨ		prio = 10;
    ਪੂਰਨ		id = -1;
    ਪੂਰਨ		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ਨਿਰਾ;
    win_T	*win = curwin;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	ਵਾਪਸ;

    ਸਮੂਹ = tv_get_string_buf_chk(&argvars[0], buf);
    ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list), "matchaddpos()");
	ਵਾਪਸ;
    }
    l = argvars[1].vval.v_list;
    ਜੇ (l == ਨਿਰਾ || l->lv_len == 0)
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], &error);

	    ਜੇ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		ਵਾਪਸ;
	}
    }
    ਜੇ (error == TRUE)
	ਵਾਪਸ;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ਜੇ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_ਮੇਲ_nr), id);
	ਵਾਪਸ;
    }

    rettv->vval.v_ਸੰਖਿਆ = ਮੇਲ_ਜੋੜੋ(win, ਸਮੂਹ, ਨਿਰਾ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    ਖਾਲੀ
f_matcharg(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) != OK)
	ਵਾਪਸ;

# ifdef FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ	    id;
    matchitem_T *m;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਜੇ (id >= 1 && id <= 3)
    {
	ਜੇ ((m = get_ਮੇਲ(curwin, id)) != ਨਿਰਾ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_ਨਮੂਨਾ, -1);
	}
	ਨਹੀਂਤਾਂ
	{
	    list_append_string(rettv->vval.v_list, ਨਿਰਾ, -1);
	    list_append_string(rettv->vval.v_list, ਨਿਰਾ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    ਖਾਲੀ
f_matchdelete(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_ਖੋਜ_EXTRA
    win_T   *win;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    win = get_optional_ਖਿੜਕੀ(argvars, 1);
    ਜੇ (win == ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = ਮੇਲ_delete(win,
				       (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]), TRUE);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    ਖਾਲੀ
ex_ਮੇਲ(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ਨਿਰਾ;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		c;
    ਪੂਰਨ		id;

    ਜੇ (eap->line2 <= 3)
	id = eap->line2;
    ਨਹੀਂਤਾਂ
    {
	emsg(_(e_invalid_command));
	ਵਾਪਸ;
    }

    // First clear any old pattern.
    ਜੇ (!eap->skip)
	ਮੇਲ_delete(curwin, id, FALSE);

    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, eap->arg))
	ਸਿਰਾ = eap->arg;
    ਨਹੀਂਤਾਂ ਜੇ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || ਸਿਰੇ_excmd2(eap->arg, eap->arg + 4))))
	ਸਿਰਾ = eap->arg + 4;
    ਨਹੀਂਤਾਂ
    {
	p = skiptowhite(eap->arg);
	ਜੇ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ਜੇ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	    ਵਾਪਸ;
	}
	ਸਿਰਾ = skip_regexp(p + 1, *p, TRUE);
	ਜੇ (!eap->skip)
	{
	    ਜੇ (*ਸਿਰਾ != NUL && !ਸਿਰੇ_excmd2(ਸਿਰਾ, skipwhite(ਸਿਰਾ + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, ਸਿਰਾ);
		ਵਾਪਸ;
	    }
	    ਜੇ (*ਸਿਰਾ != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_ਦਲੀਲ_str), p);
		ਵਾਪਸ;
	    }

	    c = *ਸਿਰਾ;
	    *ਸਿਰਾ = NUL;
	    ਮੇਲ_ਜੋੜੋ(curwin, g, p + 1, 10, id, ਨਿਰਾ, ਨਿਰਾ);
	    vim_free(g);
	    *ਸਿਰਾ = c;
	}
    }
    eap->nextcmd = ਲੱਭੋ_nextcmd(ਸਿਰਾ);
}
#ਜੇ_ਅੰਤ
