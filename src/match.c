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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_శోధన_EXTRA)

# define శోధన_HL_ప్రాధాన్యత 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_కూడు(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    పూర్ణాంకం		prio,
    పూర్ణాంకం		id,
    list_T	*pos_list,
    char_u      *conceal_char వాడని) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = ఖాళీ;
    పూర్ణాంకం		hlg_id;
    regprog_T	*regprog = ఖాళీ;
    పూర్ణాంకం		rtype = UPD_SOME_VALID;

    ఒకవేళ (*grp == NUL || (pat != ఖాళీ && *pat == NUL))
	తిరిగి -1;
    ఒకవేళ (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_గమ్యం_one_1), id);
	తిరిగి -1;
    }
    ఒకవేళ (id == -1)
    {
	// use the next available match ID
	id = wp->w_తదుపరి_సరిపోలిక_id++;
    }
    లేకపోతే
    {
	// check the given ID is not already in use
	కొరకు (cur = wp->w_సరిపోలిక_తల; cur != ఖాళీ; cur = cur->mit_తదుపరి)
	    ఒకవేళ (cur->mit_id == id)
	    {
		semsg(_(e_id_already_తీసుకున్న_nr), id);
		తిరిగి -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	ఒకవేళ (wp->w_తదుపరి_సరిపోలిక_id < id + 100)
	    wp->w_తదుపరి_సరిపోలిక_id = id + 100;
    }

    ఒకవేళ ((hlg_id = syn_namen2id(grp, (పూర్ణాంకం)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_సమూహం_name_str), grp);
	తిరిగి -1;
    }
    ఒకవేళ (pat != ఖాళీ && (regprog = vim_regcomp(pat, RE_MAGIC)) == ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str), pat);
	తిరిగి -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    ఒకవేళ (m == ఖాళీ)
	వెళ్ళు fail;
    ఒకవేళ (pos_list != ఖాళీ && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	ఒకవేళ (m->mit_pos_array == ఖాళీ)
	    వెళ్ళు fail;
	m->mit_pos_లెక్క = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_ప్రాధాన్యత = prio;
    m->mit_నమూనా = pat == ఖాళీ ? ఖాళీ : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_సరిపోలిక.regprog = regprog;
    m->mit_సరిపోలిక.rmm_ic = FALSE;
    m->mit_సరిపోలిక.rmm_maxcol = 0;
# ఒకవేళ defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    ఒకవేళ (conceal_char != ఖాళీ)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    ఒకవేళ (pos_list != ఖాళీ)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	పూర్ణాంకం		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	కొరకు (i = 0, li = pos_list->lv_మొదటి; li != ఖాళీ; i++, li = li->li_తదుపరి)
	{
	    linenr_T	lnum = 0;
	    colnr_T	నిలువు = 0;
	    పూర్ణాంకం		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    పూర్ణాంకం		error = FALSE;

	    ఒకవేళ (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		ఒకవేళ (subl == ఖాళీ)
		    వెళ్ళు fail;
		subli = subl->lv_మొదటి;
		ఒకవేళ (subli == ఖాళీ)
		    వెళ్ళు fail;
		lnum = tv_get_సంఖ్య_chk(&subli->li_tv, &error);
		ఒకవేళ (error == TRUE)
		    వెళ్ళు fail;
		ఒకవేళ (lnum == 0)
		{
		    --i;
		    కొనసాగు;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_తదుపరి;
		ఒకవేళ (subli != ఖాళీ)
		{
		    నిలువు = tv_get_సంఖ్య_chk(&subli->li_tv, &error);
		    ఒకవేళ (error == TRUE)
			వెళ్ళు fail;
		    subli = subli->li_తదుపరి;
		    ఒకవేళ (subli != ఖాళీ)
		    {
			len = tv_get_సంఖ్య_chk(&subli->li_tv, &error);
			ఒకవేళ (error == TRUE)
			    వెళ్ళు fail;
		    }
		}
		m->mit_pos_array[i].నిలువు = నిలువు;
		m->mit_pos_array[i].len = len;
	    }
	    లేకపోతే ఒకవేళ (li->li_tv.v_type == VAR_సంఖ్య)
	    {
		ఒకవేళ (li->li_tv.vval.v_సంఖ్య == 0)
		{
		    --i;
		    కొనసాగు;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_సంఖ్య;
		m->mit_pos_array[i].నిలువు = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    లేకపోతే
	    {
		emsg(_(e_list_or_సంఖ్య_required));
		వెళ్ళు fail;
	    }
	    ఒకవేళ (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    ఒకవేళ (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	ఒకవేళ (toplnum != 0)
	{
	    redraw_win_range_later(wp, toplnum, botlnum);
	    m->mit_toplnum = toplnum;
	    m->mit_botlnum = botlnum;
	    rtype = UPD_VALID;
	}
    }

    // Insert new match.  The match list is in ascending order with regard to
    // the match priorities.
    cur = wp->w_సరిపోలిక_తల;
    prev = cur;
    అయితే (cur != ఖాళీ && prio >= cur->mit_ప్రాధాన్యత)
    {
	prev = cur;
	cur = cur->mit_తదుపరి;
    }
    ఒకవేళ (cur == prev)
	wp->w_సరిపోలిక_తల = m;
    లేకపోతే
	prev->mit_తదుపరి = m;
    m->mit_తదుపరి = cur;

    redraw_win_later(wp, rtype);
    తిరిగి id;

fail:
    vim_regfree(regprog);
    ఒకవేళ (m != ఖాళీ)
    {
	vim_free(m->mit_నమూనా);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    తిరిగి -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_delete(win_T *wp, పూర్ణాంకం id, పూర్ణాంకం perr)
{
    matchitem_T	*cur = wp->w_సరిపోలిక_తల;
    matchitem_T	*prev = cur;
    పూర్ణాంకం		rtype = UPD_SOME_VALID;

    ఒకవేళ (id < 1)
    {
	ఒకవేళ (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_గమ్యం_one_2),
									   id);
	తిరిగి -1;
    }
    అయితే (cur != ఖాళీ && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_తదుపరి;
    }
    ఒకవేళ (cur == ఖాళీ)
    {
	ఒకవేళ (perr == TRUE)
	    semsg(_(e_id_not_దొరికింది_nr), id);
	తిరిగి -1;
    }
    ఒకవేళ (cur == prev)
	wp->w_సరిపోలిక_తల = cur->mit_తదుపరి;
    లేకపోతే
	prev->mit_తదుపరి = cur->mit_తదుపరి;
    vim_regfree(cur->mit_సరిపోలిక.regprog);
    vim_free(cur->mit_నమూనా);
    ఒకవేళ (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    తిరిగి 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    శూన్య
clear_matches(win_T *wp)
{
    matchitem_T *m;

    అయితే (wp->w_సరిపోలిక_తల != ఖాళీ)
    {
	m = wp->w_సరిపోలిక_తల->mit_తదుపరి;
	vim_regfree(wp->w_సరిపోలిక_తల->mit_సరిపోలిక.regprog);
	vim_free(wp->w_సరిపోలిక_తల->mit_నమూనా);
	vim_free(wp->w_సరిపోలిక_తల->mit_pos_array);
	vim_free(wp->w_సరిపోలిక_తల);
	wp->w_సరిపోలిక_తల = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    స్థిర matchitem_T *
get_సరిపోలిక(win_T *wp, పూర్ణాంకం id)
{
    matchitem_T *cur = wp->w_సరిపోలిక_తల;

    అయితే (cur != ఖాళీ && cur->mit_id != id)
	cur = cur->mit_తదుపరి;
    తిరిగి cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    శూన్య
init_శోధన_hl(win_T *wp, సరిపోలిక_T *శోధన_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //సరిపోలిక
    cur = wp->w_సరిపోలిక_తల;
    అయితే (cur != ఖాళీ)
    {
	cur->mit_hl.rm = cur->mit_సరిపోలిక;
	ఒకవేళ (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	లేకపోతే
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_బఫరు;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.మొదటి_lnum = 0;
	cur = cur->mit_తదుపరి;
    }
    // Must update this every time since highlight group override can change it.
    శోధన_hl->attr = HL_ATTR(HLF_L);

    శోధన_hl->buf = wp->w_బఫరు;
    శోధన_hl->lnum = 0;
    శోధన_hl->మొదటి_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    స్థిర పూర్ణాంకం
తదుపరి_శోధన_hl_pos(
    సరిపోలిక_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *సరిపోలిక,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    దొరికింది = -1;

    కొరకు (i = సరిపోలిక->mit_pos_cur; i < సరిపోలిక->mit_pos_లెక్క; i++)
    {
	llpos_T	*pos = &సరిపోలిక->mit_pos_array[i];

	ఒకవేళ (pos->lnum == 0)
	    ఆపు;
	ఒకవేళ (pos->len == 0 && pos->నిలువు < mincol)
	    కొనసాగు;
	ఒకవేళ (pos->lnum == lnum)
	{
	    ఒకవేళ (దొరికింది >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		ఒకవేళ (pos->నిలువు < సరిపోలిక->mit_pos_array[దొరికింది].నిలువు)
		{
		    llpos_T	tmp = *pos;

		    *pos = సరిపోలిక->mit_pos_array[దొరికింది];
		    సరిపోలిక->mit_pos_array[దొరికింది] = tmp;
		}
	    }
	    లేకపోతే
		దొరికింది = i;
	}
    }
    సరిపోలిక->mit_pos_cur = 0;
    ఒకవేళ (దొరికింది >= 0)
    {
	colnr_T	ప్రారంభం = సరిపోలిక->mit_pos_array[దొరికింది].నిలువు == 0
				     ? 0 : సరిపోలిక->mit_pos_array[దొరికింది].నిలువు - 1;
	colnr_T	చివర = సరిపోలిక->mit_pos_array[దొరికింది].నిలువు == 0
			    ? MAXCOL : ప్రారంభం + సరిపోలిక->mit_pos_array[దొరికింది].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].నిలువు = ప్రారంభం;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].నిలువు = చివర;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	సరిపోలిక->mit_pos_cur = దొరికింది + 1;
	తిరిగి 1;
    }
    తిరిగి 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    స్థిర శూన్య
తదుపరి_శోధన_hl(
    win_T	    *win,
    సరిపోలిక_T	    *శోధన_hl,
    సరిపోలిక_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    దీర్ఘ	nmatched;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    పూర్ణాంకం		timed_బహిర్గతం = FALSE;

    // for :{range}s/pat only highlight inside the range
    ఒకవేళ ((lnum < శోధన_మొదటి_పంక్తి || lnum > శోధన_చివరి_పంక్తి) && cur == ఖాళీ)
    {
	shl->lnum = 0;
	తిరిగి;
    }

    ఒకవేళ (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	ఒకవేళ (lnum > l)
	    shl->lnum = 0;
	లేకపోతే ఒకవేళ (lnum < l || shl->rm.endpos[0].నిలువు > mincol)
	    తిరిగి;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    కొరకు (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	ఒకవేళ (shl->lnum == 0)
	    matchcol = 0;
	లేకపోతే ఒకవేళ (vim_strchr(p_cpo, CPO_శోధన) == ఖాళీ
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].నిలువు <= shl->rm.startpos[0].నిలువు))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].నిలువు;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    ఒకవేళ (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		ఆపు;
	    }
	    ఒకవేళ (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    లేకపోతే
		++matchcol;
	}
	లేకపోతే
	    matchcol = shl->rm.endpos[0].నిలువు;

	shl->lnum = lnum;
	ఒకవేళ (shl->rm.regprog != ఖాళీ)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    పూర్ణాంకం regprog_is_copy = (shl != శోధన_hl && cur != ఖాళీ
			  && shl == &cur->mit_hl
			  && cur->mit_సరిపోలిక.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_బహిర్గతం);
	    // Copy the regprog, in case it got freed and recompiled.
	    ఒకవేళ (regprog_is_copy)
		cur->mit_సరిపోలిక.regprog = cur->mit_hl.rm.regprog;

	    ఒకవేళ (called_emsg > called_emsg_before || got_int || timed_బహిర్గతం)
	    {
		// Error while handling regexp: stop using this regexp.
		ఒకవేళ (shl == శోధన_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    సమితి_no_hlsearch(TRUE);
		}
		shl->rm.regprog = ఖాళీ;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (cur != ఖాళీ)
	    nmatched = తదుపరి_శోధన_hl_pos(shl, lnum, cur, matchcol);
	లేకపోతే
	    nmatched = 0;
	ఒకవేళ (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    ఆపు;
	}
	ఒకవేళ (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].నిలువు >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].నిలువు > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    ఆపు;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    శూన్య
prepare_శోధన_hl(win_T *wp, సరిపోలిక_T *శోధన_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    సరిపోలిక_T	*shl;		// points to search_hl or a match
    పూర్ణాంకం		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    పూర్ణాంకం		pos_inprogress;	// marks that position match search is
				// in progress
    పూర్ణాంకం		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_సరిపోలిక_తల;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    అయితే (cur != ఖాళీ || shl_flag == FALSE)
    {
	ఒకవేళ (shl_flag == FALSE)
	{
	    shl = శోధన_hl;
	    shl_flag = TRUE;
	}
	లేకపోతే
	    shl = &cur->mit_hl;
	ఒకవేళ (shl->rm.regprog != ఖాళీ
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    ఒకవేళ (shl->మొదటి_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		కొరకు (shl->మొదటి_lnum = lnum;
			   shl->మొదటి_lnum > wp->w_topline; --shl->మొదటి_lnum)
		    ఒకవేళ (hasFoldingWin(wp, shl->మొదటి_lnum - 1,
						      ఖాళీ, ఖాళీ, TRUE, ఖాళీ))
			ఆపు;
# లేకపోతే
		shl->మొదటి_lnum = wp->w_topline;
# endif
	    }
	    ఒకవేళ (cur != ఖాళీ)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    అయితే (shl->మొదటి_lnum < lnum && (shl->rm.regprog != ఖాళీ
					  || (cur != ఖాళీ && pos_inprogress)))
	    {
		తదుపరి_శోధన_hl(wp, శోధన_hl, shl, shl->మొదటి_lnum, (colnr_T)n,
					       shl == శోధన_hl ? ఖాళీ : cur);
		pos_inprogress = cur == ఖాళీ || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		ఒకవేళ (shl->lnum != 0)
		{
		    shl->మొదటి_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].నిలువు;
		}
		లేకపోతే
		{
		    ++shl->మొదటి_lnum;
		    n = 0;
		}
	    }
	}
	ఒకవేళ (shl != శోధన_hl && cur != ఖాళీ)
	    cur = cur->mit_తదుపరి;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    స్థిర శూన్య
check_cur_శోధన_hl(win_T *wp, సరిపోలిక_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    ఒకవేళ (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.నిలువు >= shl->rm.startpos[0].నిలువు)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.నిలువు < shl->rm.endpos[0].నిలువు))
	shl->has_cursor = TRUE;
    లేకపోతే
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    పూర్ణాంకం
prepare_శోధన_hl_పంక్తి(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **పంక్తి,
	సరిపోలిక_T	    *శోధన_hl,
	పూర్ణాంకం	    *శోధన_attr)
{
    matchitem_T *cur;			// points to the match list
    సరిపోలిక_T	*shl;			// points to search_hl or a match
    పూర్ణాంకం		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    పూర్ణాంకం		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_సరిపోలిక_తల;
    shl_flag = WIN_IS_POPUP(wp);
    అయితే (cur != ఖాళీ || shl_flag == FALSE)
    {
	ఒకవేళ (shl_flag == FALSE)
	{
	    shl = శోధన_hl;
	    shl_flag = TRUE;
	}
	లేకపోతే
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	ఒకవేళ (cur != ఖాళీ)
	    cur->mit_pos_cur = 0;
	తదుపరి_శోధన_hl(wp, శోధన_hl, shl, lnum, mincol,
						shl == శోధన_hl ? ఖాళీ : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

	ఒకవేళ (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    ఒకవేళ (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].నిలువు;
	    లేకపోతే
		shl->startcol = 0;
	    ఒకవేళ (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].నిలువు;
	    లేకపోతే
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    ఒకవేళ (shl == శోధన_hl)
		check_cur_శోధన_hl(wp, shl);

	    // Highlight one character for an empty match.
	    ఒకవేళ (shl->startcol == shl->endcol)
	    {
		ఒకవేళ (has_mbyte && (*పంక్తి)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*పంక్తి) + shl->endcol);
		లేకపోతే
		    ++shl->endcol;
	    }
	    ఒకవేళ ((దీర్ఘ)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*శోధన_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	ఒకవేళ (shl != శోధన_hl && cur != ఖాళీ)
	    cur = cur->mit_తదుపరి;
    }
    తిరిగి area_highlighting;
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
    పూర్ణాంకం
update_శోధన_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    నిలువు,
	char_u	    **పంక్తి,
	సరిపోలిక_T	    *శోధన_hl,
	పూర్ణాంకం	    *has_సరిపోలిక_conc వాడని,
	పూర్ణాంకం	    *సరిపోలిక_conc వాడని,
	పూర్ణాంకం	    did_పంక్తి_attr,
	పూర్ణాంకం	    lcs_eol_one,
	పూర్ణాంకం	    *on_చివరి_నిలువు)
{
    matchitem_T *cur;		    // points to the match list
    సరిపోలిక_T	*shl;		    // points to search_hl or a match
    పూర్ణాంకం		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    పూర్ణాంకం		pos_inprogress;	    // marks that position match search is in
				    //పురోగతి
    పూర్ణాంకం		శోధన_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_సరిపోలిక_తల;
    shl_flag = WIN_IS_POPUP(wp);
    అయితే (cur != ఖాళీ || shl_flag == FALSE)
    {
	ఒకవేళ (shl_flag == FALSE
		&& (cur == ఖాళీ
			|| cur->mit_ప్రాధాన్యత > శోధన_HL_ప్రాధాన్యత))
	{
	    shl = శోధన_hl;
	    shl_flag = TRUE;
	}
	లేకపోతే
	    shl = &cur->mit_hl;
	ఒకవేళ (cur != ఖాళీ)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	అయితే (shl->rm.regprog != ఖాళీ || (cur != ఖాళీ && pos_inprogress))
	{
	    ఒకవేళ (shl->startcol != MAXCOL
		    && నిలువు >= shl->startcol
		    && నిలువు < shl->endcol)
	    {
		పూర్ణాంకం తదుపరి_నిలువు = నిలువు + mb_ptr2len(*పంక్తి + నిలువు);

		ఒకవేళ (shl->endcol < తదుపరి_నిలువు)
		    shl->endcol = తదుపరి_నిలువు;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		ఒకవేళ (cur != ఖాళీ
			&& shl != శోధన_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_సరిపోలిక_conc = నిలువు == shl->startcol ? 2 : 1;
		    *సరిపోలిక_conc = cur->mit_conceal_char;
		}
		లేకపోతే
		    *has_సరిపోలిక_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		ఒకవేళ (shl == శోధన_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    ఒకవేళ (shl->attr_cur != shl->attr)
			శోధన_hl_has_cursor_lnum = lnum;
		}

	    }
	    లేకపోతే ఒకవేళ (నిలువు == shl->endcol)
	    {
		shl->attr_cur = 0;
		తదుపరి_శోధన_hl(wp, శోధన_hl, shl, lnum, నిలువు,
					       shl == శోధన_hl ? ఖాళీ : cur);
		pos_inprogress = !(cur == ఖాళీ || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

		ఒకవేళ (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].నిలువు;
		    ఒకవేళ (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].నిలువు;
		    లేకపోతే
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    ఒకవేళ (shl == శోధన_hl)
			check_cur_శోధన_hl(wp, shl);

		    ఒకవేళ (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			ఒకవేళ (has_mbyte)
			{
			    char_u *p = *పంక్తి + shl->endcol;

			    ఒకవేళ (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    లేకపోతే
				shl->endcol += (*mb_ptr2len)(p);
			}
			లేకపోతే
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    కొనసాగు;
		}
	    }
	    ఆపు;
	}
	ఒకవేళ (shl != శోధన_hl && cur != ఖాళీ)
	    cur = cur->mit_తదుపరి;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_సరిపోలిక_తల;
    shl_flag = WIN_IS_POPUP(wp);
    అయితే (cur != ఖాళీ || shl_flag == FALSE)
    {
	ఒకవేళ (shl_flag == FALSE
		&& (cur == ఖాళీ ||
			cur->mit_ప్రాధాన్యత > శోధన_HL_ప్రాధాన్యత))
	{
	    shl = శోధన_hl;
	    shl_flag = TRUE;
	}
	లేకపోతే
	    shl = &cur->mit_hl;
	ఒకవేళ (shl->attr_cur != 0)
	{
	    శోధన_attr = shl->attr_cur;
	    *on_చివరి_నిలువు = నిలువు + 1 >= shl->endcol;
	}
	ఒకవేళ (shl != శోధన_hl && cur != ఖాళీ)
	    cur = cur->mit_తదుపరి;
    }
    // Only highlight one character after the last column.
    ఒకవేళ (*(*పంక్తి + నిలువు) == NUL && (did_పంక్తి_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	శోధన_attr = 0;
    తిరిగి శోధన_attr;
}

    పూర్ణాంకం
get_prevcol_hl_flag(win_T *wp, సరిపోలిక_T *శోధన_hl, దీర్ఘ curcol)
{
    దీర్ఘ	prevcol = curcol;
    పూర్ణాంకం		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# ఒకవేళ defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    ఒకవేళ (popup_is_popup(wp))
	తిరిగి FALSE;
# endif

    // we're not really at that column when skipping some text
    ఒకవేళ ((దీర్ఘ)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    ఒకవేళ (!శోధన_hl->is_addpos && (prevcol == (దీర్ఘ)శోధన_hl->startcol
		|| (prevcol > (దీర్ఘ)శోధన_hl->startcol
					      && శోధన_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    లేకపోతే
    {
	cur = wp->w_సరిపోలిక_తల;
	అయితే (cur != ఖాళీ)
	{
	    ఒకవేళ (!cur->mit_hl.is_addpos && (prevcol == (దీర్ఘ)cur->mit_hl.startcol
			|| (prevcol > (దీర్ఘ)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		ఆపు;
	    }
	    cur = cur->mit_తదుపరి;
	}
    }
    తిరిగి prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    శూన్య
get_శోధన_సరిపోలిక_hl(win_T *wp, సరిపోలిక_T *శోధన_hl, దీర్ఘ నిలువు, పూర్ణాంకం *char_attr)
{
    matchitem_T *cur;			// points to the match list
    సరిపోలిక_T	*shl;			// points to search_hl or a match
    పూర్ణాంకం		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_సరిపోలిక_తల;
    shl_flag = WIN_IS_POPUP(wp);
    అయితే (cur != ఖాళీ || shl_flag == FALSE)
    {
	ఒకవేళ (shl_flag == FALSE
		&& ((cur != ఖాళీ
			&& cur->mit_ప్రాధాన్యత > శోధన_HL_ప్రాధాన్యత)
		    || cur == ఖాళీ))
	{
	    shl = శోధన_hl;
	    shl_flag = TRUE;
	}
	లేకపోతే
	    shl = &cur->mit_hl;
	ఒకవేళ (నిలువు - 1 == (దీర్ఘ)shl->startcol
		&& (shl == శోధన_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	ఒకవేళ (shl != శోధన_hl && cur != ఖాళీ)
	    cur = cur->mit_తదుపరి;
    }
}

#అయితే_ముగింపు // FEAT_SEARCH_EXTRA

#ఒకవేళ defined(FEAT_EVAL)
# ifdef FEAT_శోధన_EXTRA
    స్థిర పూర్ణాంకం
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    ఒకవేళ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	తిరిగి FAIL;
    }

    ఒకవేళ (dict_has_కీలకం(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    ఒకవేళ ((di = dict_కనుగొను(tv->vval.v_dict, (char_u *)"window", -1)) == ఖాళీ)
	తిరిగి OK;

    *win = కనుగొను_win_by_nr_or_id(&di->di_tv);
    ఒకవేళ (*win == ఖాళీ)
    {
	emsg(_(e_invalid_కిటికీ_సంఖ్య));
	తిరిగి FAIL;
    }

    తిరిగి OK;
}
# endif

/*
 * "clearmatches()" function
 */
    శూన్య
f_clearmatches(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    win_T   *win;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    win = get_optional_కిటికీ(argvars, 0);
    ఒకవేళ (win != ఖాళీ)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    శూన్య
f_getmatches(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    పూర్ణాంకం		i;
    win_T	*win;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    win = get_optional_కిటికీ(argvars, 0);
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL || win == ఖాళీ)
	తిరిగి;

    cur = win->w_సరిపోలిక_తల;
    అయితే (cur != ఖాళీ)
    {
	dict = dict_alloc();
	ఒకవేళ (dict == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (cur->mit_సరిపోలిక.regprog == ఖాళీ)
	{
	    // match added with matchaddpos()
	    కొరకు (i = 0; i < cur->mit_pos_లెక్క; ++i)
	    {
		llpos_T	*llpos;
		అక్షరం	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		ఒకవేళ (llpos->lnum == 0)
		    ఆపు;
		l = list_alloc();
		ఒకవేళ (l == ఖాళీ)
		    ఆపు;
		list_append_సంఖ్య(l, (varnumber_T)llpos->lnum);
		ఒకవేళ (llpos->నిలువు > 0)
		{
		    list_append_సంఖ్య(l, (varnumber_T)llpos->నిలువు);
		    list_append_సంఖ్య(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		ఒకవేళ (dict_కూడు_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	లేకపోతే
	{
	    dict_కూడు_string(dict, "pattern", cur->mit_నమూనా);
	}
	dict_కూడు_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_కూడు_సంఖ్య(dict, "priority", (దీర్ఘ)cur->mit_ప్రాధాన్యత);
	dict_కూడు_సంఖ్య(dict, "id", (దీర్ఘ)cur->mit_id);
#  ఒకవేళ defined(FEAT_CONCEAL)
	ఒకవేళ (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    పూర్ణాంకం	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_కూడు_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	ఒకవేళ (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_తదుపరి;
    }
# endif
}

/*
 * "setmatches()" function
 */
    శూన్య
f_setmatches(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = ఖాళీ;
    win_T	*win;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;
    win = get_optional_కిటికీ(argvars, 1);
    ఒకవేళ (win == ఖాళీ)
	తిరిగి;

    ఒకవేళ ((l = argvars[0].vval.v_list) != ఖాళీ)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_మొదటి;
	అయితే (li != ఖాళీ)
	{
	    ఒకవేళ (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == ఖాళీ)
	    {
		emsg(_(e_invalid_వాదం));
		తిరిగి;
	    }
	    ఒకవేళ (!(dict_has_కీలకం(d, "group")
			&& (dict_has_కీలకం(d, "pattern")
			    || dict_has_కీలకం(d, "pos1"))
			&& dict_has_కీలకం(d, "priority")
			&& dict_has_కీలకం(d, "id")))
	    {
		emsg(_(e_invalid_వాదం));
		తిరిగి;
	    }
	    li = li->li_తదుపరి;
	}

	clear_matches(win);
	li = l->lv_మొదటి;
	అయితే (li != ఖాళీ)
	{
	    పూర్ణాంకం		i = 0;
	    అక్షరం	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*సమూహం;
	    పూర్ణాంకం		ప్రాధాన్యత;
	    పూర్ణాంకం		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    ఒకవేళ (!dict_has_కీలకం(d, "pattern"))
	    {
		ఒకవేళ (s == ఖాళీ)
		{
		    s = list_alloc();
		    ఒకవేళ (s == ఖాళీ)
			తిరిగి;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		కొరకు (i = 1; i < 9; i++)
		{
		    sprintf((అక్షరం *)buf, (అక్షరం *)"pos%d", i);
		    ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)buf, -1)) != ఖాళీ)
		    {
			ఒకవేళ (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    తిరిగి;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    లేకపోతే
			ఆపు;
		}
	    }

	    సమూహం = dict_get_string(d, "group", TRUE);
	    ప్రాధాన్యత = (పూర్ణాంకం)dict_get_సంఖ్య(d, "priority");
	    id = (పూర్ణాంకం)dict_get_సంఖ్య(d, "id");
	    conceal = dict_has_కీలకం(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : ఖాళీ;
	    ఒకవేళ (i == 0)
	    {
		సరిపోలిక_కూడు(win, సమూహం,
		    dict_get_string(d, "pattern", FALSE),
		    ప్రాధాన్యత, id, ఖాళీ, conceal);
	    }
	    లేకపోతే
	    {
		సరిపోలిక_కూడు(win, సమూహం, ఖాళీ, ప్రాధాన్యత, id, s, conceal);
		list_unref(s);
		s = ఖాళీ;
	    }
	    vim_free(సమూహం);
	    vim_free(conceal);

	    li = li->li_తదుపరి;
	}
	rettv->vval.v_సంఖ్య = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    శూన్య
f_matchadd(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//సమూహం
    char_u	*pat;		//నమూనా
    పూర్ణాంకం		prio = 10;	// default priority
    పూర్ణాంకం		id = -1;
    పూర్ణాంకం		error = FALSE;
    char_u	*conceal_char = ఖాళీ;
    win_T	*win = curwin;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	తిరిగి;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//సమూహం
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//నమూనా
    ఒకవేళ (grp == ఖాళీ || pat == ఖాళీ)
	తిరిగి;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[3], &error);
	    ఒకవేళ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		తిరిగి;
	}
    }
    ఒకవేళ (error == TRUE)
	తిరిగి;
    ఒకవేళ (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_సరిపోలిక_nr), id);
	తిరిగి;
    }

    rettv->vval.v_సంఖ్య = సరిపోలిక_కూడు(win, grp, pat, prio, id, ఖాళీ,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    శూన్య
f_matchaddpos(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*సమూహం;
    పూర్ణాంకం		prio = 10;
    పూర్ణాంకం		id = -1;
    పూర్ణాంకం		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = ఖాళీ;
    win_T	*win = curwin;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	తిరిగి;

    సమూహం = tv_get_string_buf_chk(&argvars[0], buf);
    ఒకవేళ (సమూహం == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_వాదం_యొక్క_str_must_be_list), "matchaddpos()");
	తిరిగి;
    }
    l = argvars[1].vval.v_list;
    ఒకవేళ (l == ఖాళీ || l->lv_len == 0)
	తిరిగి;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[3], &error);

	    ఒకవేళ (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		తిరిగి;
	}
    }
    ఒకవేళ (error == TRUE)
	తిరిగి;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    ఒకవేళ (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_సరిపోలిక_nr), id);
	తిరిగి;
    }

    rettv->vval.v_సంఖ్య = సరిపోలిక_కూడు(win, సమూహం, ఖాళీ, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    శూన్య
f_matcharg(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) != OK)
	తిరిగి;

# ifdef FEAT_శోధన_EXTRA
    పూర్ణాంకం	    id;
    matchitem_T *m;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
    ఒకవేళ (id >= 1 && id <= 3)
    {
	ఒకవేళ ((m = get_సరిపోలిక(curwin, id)) != ఖాళీ)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_నమూనా, -1);
	}
	లేకపోతే
	{
	    list_append_string(rettv->vval.v_list, ఖాళీ, -1);
	    list_append_string(rettv->vval.v_list, ఖాళీ, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    శూన్య
f_matchdelete(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_శోధన_EXTRA
    win_T   *win;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    win = get_optional_కిటికీ(argvars, 1);
    ఒకవేళ (win == ఖాళీ)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = సరిపోలిక_delete(win,
				       (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]), TRUE);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    శూన్య
ex_సరిపోలిక(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = ఖాళీ;
    char_u	*చివర;
    పూర్ణాంకం		c;
    పూర్ణాంకం		id;

    ఒకవేళ (eap->line2 <= 3)
	id = eap->line2;
    లేకపోతే
    {
	emsg(_(e_invalid_command));
	తిరిగి;
    }

    // First clear any old pattern.
    ఒకవేళ (!eap->skip)
	సరిపోలిక_delete(curwin, id, FALSE);

    ఒకవేళ (చివర్లు_excmd2(eap->cmd, eap->arg))
	చివర = eap->arg;
    లేకపోతే ఒకవేళ ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || చివర్లు_excmd2(eap->arg, eap->arg + 4))))
	చివర = eap->arg + 4;
    లేకపోతే
    {
	p = skiptowhite(eap->arg);
	ఒకవేళ (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	ఒకవేళ (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_వాదం_str), eap->arg);
	    తిరిగి;
	}
	చివర = skip_regexp(p + 1, *p, TRUE);
	ఒకవేళ (!eap->skip)
	{
	    ఒకవేళ (*చివర != NUL && !చివర్లు_excmd2(చివర, skipwhite(చివర + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, చివర);
		తిరిగి;
	    }
	    ఒకవేళ (*చివర != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_వాదం_str), p);
		తిరిగి;
	    }

	    c = *చివర;
	    *చివర = NUL;
	    సరిపోలిక_కూడు(curwin, g, p + 1, 10, id, ఖాళీ, ఖాళీ);
	    vim_free(g);
	    *చివర = c;
	}
    }
    eap->nextcmd = కనుగొను_nextcmd(చివర);
}
#అయితే_ముగింపు
