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

#اگر defined(FEAT_تلاش_EXTRA)

# define تلاش_HL_ترجیح 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ساکن صحیح
مطابقت_جوڑو(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    صحیح		prio,
    صحیح		id,
    list_T	*pos_list,
    char_u      *conceal_char غیر_مستعمل) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*پچھلا;
    matchitem_T	*m = تہی;
    صحیح		hlg_id;
    regprog_T	*regprog = تہی;
    صحیح		rtype = UPD_SOME_VALID;

    اگر (*grp == NUL || (pat != تہی && *pat == NUL))
	واپس -1;
    اگر (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_1), id);
	واپس -1;
    }
    اگر (id == -1)
    {
	// use the next available match ID
	id = wp->w_اگلا_مطابقت_id++;
    }
    ورنہ
    {
	// check the given ID is not already in use
	برائے (cur = wp->w_مطابقت_سر; cur != تہی; cur = cur->mit_اگلا)
	    اگر (cur->mit_id == id)
	    {
		semsg(_(e_id_already_لیا_گیا_nr), id);
		واپس -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	اگر (wp->w_اگلا_مطابقت_id < id + 100)
	    wp->w_اگلا_مطابقت_id = id + 100;
    }

    اگر ((hlg_id = syn_namen2id(grp, (صحیح)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_گروہ_name_str), grp);
	واپس -1;
    }
    اگر (pat != تہی && (regprog = vim_regcomp(pat, RE_MAGIC)) == تہی)
    {
	semsg(_(e_invalid_دلیل_str), pat);
	واپس -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    اگر (m == تہی)
	جاؤ fail;
    اگر (pos_list != تہی && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	اگر (m->mit_pos_array == تہی)
	    جاؤ fail;
	m->mit_pos_گنتی = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_ترجیح = prio;
    m->mit_نمونہ = pat == تہی ? تہی : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_مطابقت.regprog = regprog;
    m->mit_مطابقت.rmm_ic = FALSE;
    m->mit_مطابقت.rmm_maxcol = 0;
# اگر defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    اگر (conceal_char != تہی)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    اگر (pos_list != تہی)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	صحیح		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	برائے (i = 0, li = pos_list->lv_پہلا; li != تہی; i++, li = li->li_اگلا)
	{
	    linenr_T	lnum = 0;
	    colnr_T	ستون = 0;
	    صحیح		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    صحیح		error = FALSE;

	    اگر (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		اگر (subl == تہی)
		    جاؤ fail;
		subli = subl->lv_پہلا;
		اگر (subli == تہی)
		    جاؤ fail;
		lnum = tv_get_نمبر_chk(&subli->li_tv, &error);
		اگر (error == TRUE)
		    جاؤ fail;
		اگر (lnum == 0)
		{
		    --i;
		    جاری;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_اگلا;
		اگر (subli != تہی)
		{
		    ستون = tv_get_نمبر_chk(&subli->li_tv, &error);
		    اگر (error == TRUE)
			جاؤ fail;
		    subli = subli->li_اگلا;
		    اگر (subli != تہی)
		    {
			len = tv_get_نمبر_chk(&subli->li_tv, &error);
			اگر (error == TRUE)
			    جاؤ fail;
		    }
		}
		m->mit_pos_array[i].ستون = ستون;
		m->mit_pos_array[i].len = len;
	    }
	    ورنہ اگر (li->li_tv.v_type == VAR_نمبر)
	    {
		اگر (li->li_tv.vval.v_نمبر == 0)
		{
		    --i;
		    جاری;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_نمبر;
		m->mit_pos_array[i].ستون = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    ورنہ
	    {
		emsg(_(e_list_or_نمبر_required));
		جاؤ fail;
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
    cur = wp->w_مطابقت_سر;
    پچھلا = cur;
    جب (cur != تہی && prio >= cur->mit_ترجیح)
    {
	پچھلا = cur;
	cur = cur->mit_اگلا;
    }
    اگر (cur == پچھلا)
	wp->w_مطابقت_سر = m;
    ورنہ
	پچھلا->mit_اگلا = m;
    m->mit_اگلا = cur;

    redraw_win_later(wp, rtype);
    واپس id;

fail:
    vim_regfree(regprog);
    اگر (m != تہی)
    {
	vim_free(m->mit_نمونہ);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    واپس -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ساکن صحیح
مطابقت_delete(win_T *wp, صحیح id, صحیح perr)
{
    matchitem_T	*cur = wp->w_مطابقت_سر;
    matchitem_T	*پچھلا = cur;
    صحیح		rtype = UPD_SOME_VALID;

    اگر (id < 1)
    {
	اگر (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_2),
									   id);
	واپس -1;
    }
    جب (cur != تہی && cur->mit_id != id)
    {
	پچھلا = cur;
	cur = cur->mit_اگلا;
    }
    اگر (cur == تہی)
    {
	اگر (perr == TRUE)
	    semsg(_(e_id_not_ملا_nr), id);
	واپس -1;
    }
    اگر (cur == پچھلا)
	wp->w_مطابقت_سر = cur->mit_اگلا;
    ورنہ
	پچھلا->mit_اگلا = cur->mit_اگلا;
    vim_regfree(cur->mit_مطابقت.regprog);
    vim_free(cur->mit_نمونہ);
    اگر (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    واپس 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    خالی
clear_matches(win_T *wp)
{
    matchitem_T *m;

    جب (wp->w_مطابقت_سر != تہی)
    {
	m = wp->w_مطابقت_سر->mit_اگلا;
	vim_regfree(wp->w_مطابقت_سر->mit_مطابقت.regprog);
	vim_free(wp->w_مطابقت_سر->mit_نمونہ);
	vim_free(wp->w_مطابقت_سر->mit_pos_array);
	vim_free(wp->w_مطابقت_سر);
	wp->w_مطابقت_سر = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    ساکن matchitem_T *
get_مطابقت(win_T *wp, صحیح id)
{
    matchitem_T *cur = wp->w_مطابقت_سر;

    جب (cur != تہی && cur->mit_id != id)
	cur = cur->mit_اگلا;
    واپس cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    خالی
init_تلاش_hl(win_T *wp, مطابقت_T *تلاش_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //مطابقت
    cur = wp->w_مطابقت_سر;
    جب (cur != تہی)
    {
	cur->mit_hl.rm = cur->mit_مطابقت;
	اگر (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	ورنہ
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_بفر;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.پہلا_lnum = 0;
	cur = cur->mit_اگلا;
    }
    // Must update this every time since highlight group override can change it.
    تلاش_hl->attr = HL_ATTR(HLF_L);

    تلاش_hl->buf = wp->w_بفر;
    تلاش_hl->lnum = 0;
    تلاش_hl->پہلا_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ساکن صحیح
اگلا_تلاش_hl_pos(
    مطابقت_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *مطابقت,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    صحیح	    i;
    صحیح	    ملا = -1;

    برائے (i = مطابقت->mit_pos_cur; i < مطابقت->mit_pos_گنتی; i++)
    {
	llpos_T	*pos = &مطابقت->mit_pos_array[i];

	اگر (pos->lnum == 0)
	    توڑو;
	اگر (pos->len == 0 && pos->ستون < mincol)
	    جاری;
	اگر (pos->lnum == lnum)
	{
	    اگر (ملا >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		اگر (pos->ستون < مطابقت->mit_pos_array[ملا].ستون)
		{
		    llpos_T	tmp = *pos;

		    *pos = مطابقت->mit_pos_array[ملا];
		    مطابقت->mit_pos_array[ملا] = tmp;
		}
	    }
	    ورنہ
		ملا = i;
	}
    }
    مطابقت->mit_pos_cur = 0;
    اگر (ملا >= 0)
    {
	colnr_T	آغاز = مطابقت->mit_pos_array[ملا].ستون == 0
				     ? 0 : مطابقت->mit_pos_array[ملا].ستون - 1;
	colnr_T	انتہا = مطابقت->mit_pos_array[ملا].ستون == 0
			    ? MAXCOL : آغاز + مطابقت->mit_pos_array[ملا].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].ستون = آغاز;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].ستون = انتہا;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	مطابقت->mit_pos_cur = ملا + 1;
	واپس 1;
    }
    واپس 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ساکن خالی
اگلا_تلاش_hl(
    win_T	    *win,
    مطابقت_T	    *تلاش_hl,
    مطابقت_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    طویل	nmatched;
    صحیح		called_emsg_before = called_emsg;
    صحیح		timed_برآمد = FALSE;

    // for :{range}s/pat only highlight inside the range
    اگر ((lnum < تلاش_پہلا_سطر || lnum > تلاش_آخری_سطر) && cur == تہی)
    {
	shl->lnum = 0;
	واپس;
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
	ورنہ اگر (lnum < l || shl->rm.endpos[0].ستون > mincol)
	    واپس;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    برائے (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	اگر (shl->lnum == 0)
	    matchcol = 0;
	ورنہ اگر (vim_strchr(p_cpo, CPO_تلاش) == تہی
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].ستون <= shl->rm.startpos[0].ستون))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].ستون;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    اگر (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		توڑو;
	    }
	    اگر (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    ورنہ
		++matchcol;
	}
	ورنہ
	    matchcol = shl->rm.endpos[0].ستون;

	shl->lnum = lnum;
	اگر (shl->rm.regprog != تہی)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    صحیح regprog_is_copy = (shl != تلاش_hl && cur != تہی
			  && shl == &cur->mit_hl
			  && cur->mit_مطابقت.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_برآمد);
	    // Copy the regprog, in case it got freed and recompiled.
	    اگر (regprog_is_copy)
		cur->mit_مطابقت.regprog = cur->mit_hl.rm.regprog;

	    اگر (called_emsg > called_emsg_before || got_int || timed_برآمد)
	    {
		// Error while handling regexp: stop using this regexp.
		اگر (shl == تلاش_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    سیٹ_no_hlsearch(TRUE);
		}
		shl->rm.regprog = تہی;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		توڑو;
	    }
	}
	ورنہ اگر (cur != تہی)
	    nmatched = اگلا_تلاش_hl_pos(shl, lnum, cur, matchcol);
	ورنہ
	    nmatched = 0;
	اگر (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    توڑو;
	}
	اگر (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].ستون >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].ستون > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    توڑو;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    خالی
prepare_تلاش_hl(win_T *wp, مطابقت_T *تلاش_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    مطابقت_T	*shl;		// points to search_hl or a match
    صحیح		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    صحیح		pos_inprogress;	// marks that position match search is
				// in progress
    صحیح		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_مطابقت_سر;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    جب (cur != تہی || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE)
	{
	    shl = تلاش_hl;
	    shl_flag = TRUE;
	}
	ورنہ
	    shl = &cur->mit_hl;
	اگر (shl->rm.regprog != تہی
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    اگر (shl->پہلا_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		برائے (shl->پہلا_lnum = lnum;
			   shl->پہلا_lnum > wp->w_topline; --shl->پہلا_lnum)
		    اگر (hasFoldingWin(wp, shl->پہلا_lnum - 1,
						      تہی, تہی, TRUE, تہی))
			توڑو;
# ورنہ
		shl->پہلا_lnum = wp->w_topline;
# endif
	    }
	    اگر (cur != تہی)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    جب (shl->پہلا_lnum < lnum && (shl->rm.regprog != تہی
					  || (cur != تہی && pos_inprogress)))
	    {
		اگلا_تلاش_hl(wp, تلاش_hl, shl, shl->پہلا_lnum, (colnr_T)n,
					       shl == تلاش_hl ? تہی : cur);
		pos_inprogress = cur == تہی || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		اگر (shl->lnum != 0)
		{
		    shl->پہلا_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].ستون;
		}
		ورنہ
		{
		    ++shl->پہلا_lnum;
		    n = 0;
		}
	    }
	}
	اگر (shl != تلاش_hl && cur != تہی)
	    cur = cur->mit_اگلا;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ساکن خالی
check_cur_تلاش_hl(win_T *wp, مطابقت_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    اگر (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.ستون >= shl->rm.startpos[0].ستون)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.ستون < shl->rm.endpos[0].ستون))
	shl->has_cursor = TRUE;
    ورنہ
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    صحیح
prepare_تلاش_hl_سطر(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **سطر,
	مطابقت_T	    *تلاش_hl,
	صحیح	    *تلاش_attr)
{
    matchitem_T *cur;			// points to the match list
    مطابقت_T	*shl;			// points to search_hl or a match
    صحیح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    صحیح		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_مطابقت_سر;
    shl_flag = WIN_IS_POPUP(wp);
    جب (cur != تہی || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE)
	{
	    shl = تلاش_hl;
	    shl_flag = TRUE;
	}
	ورنہ
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	اگر (cur != تہی)
	    cur->mit_pos_cur = 0;
	اگلا_تلاش_hl(wp, تلاش_hl, shl, lnum, mincol,
						shl == تلاش_hl ? تہی : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);

	اگر (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    اگر (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].ستون;
	    ورنہ
		shl->startcol = 0;
	    اگر (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].ستون;
	    ورنہ
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    اگر (shl == تلاش_hl)
		check_cur_تلاش_hl(wp, shl);

	    // Highlight one character for an empty match.
	    اگر (shl->startcol == shl->endcol)
	    {
		اگر (has_mbyte && (*سطر)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*سطر) + shl->endcol);
		ورنہ
		    ++shl->endcol;
	    }
	    اگر ((طویل)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*تلاش_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	اگر (shl != تلاش_hl && cur != تہی)
	    cur = cur->mit_اگلا;
    }
    واپس area_highlighting;
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
update_تلاش_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    ستون,
	char_u	    **سطر,
	مطابقت_T	    *تلاش_hl,
	صحیح	    *has_مطابقت_conc غیر_مستعمل,
	صحیح	    *مطابقت_conc غیر_مستعمل,
	صحیح	    did_سطر_attr,
	صحیح	    lcs_eol_one,
	صحیح	    *on_آخری_ستون)
{
    matchitem_T *cur;		    // points to the match list
    مطابقت_T	*shl;		    // points to search_hl or a match
    صحیح		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    صحیح		pos_inprogress;	    // marks that position match search is in
				    //پیش_رفت
    صحیح		تلاش_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_مطابقت_سر;
    shl_flag = WIN_IS_POPUP(wp);
    جب (cur != تہی || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& (cur == تہی
			|| cur->mit_ترجیح > تلاش_HL_ترجیح))
	{
	    shl = تلاش_hl;
	    shl_flag = TRUE;
	}
	ورنہ
	    shl = &cur->mit_hl;
	اگر (cur != تہی)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	جب (shl->rm.regprog != تہی || (cur != تہی && pos_inprogress))
	{
	    اگر (shl->startcol != MAXCOL
		    && ستون >= shl->startcol
		    && ستون < shl->endcol)
	    {
		صحیح اگلا_ستون = ستون + mb_ptr2len(*سطر + ستون);

		اگر (shl->endcol < اگلا_ستون)
		    shl->endcol = اگلا_ستون;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		اگر (cur != تہی
			&& shl != تلاش_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_مطابقت_conc = ستون == shl->startcol ? 2 : 1;
		    *مطابقت_conc = cur->mit_conceal_char;
		}
		ورنہ
		    *has_مطابقت_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		اگر (shl == تلاش_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    اگر (shl->attr_cur != shl->attr)
			تلاش_hl_has_cursor_lnum = lnum;
		}

	    }
	    ورنہ اگر (ستون == shl->endcol)
	    {
		shl->attr_cur = 0;
		اگلا_تلاش_hl(wp, تلاش_hl, shl, lnum, ستون,
					       shl == تلاش_hl ? تہی : cur);
		pos_inprogress = !(cur == تہی || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);

		اگر (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].ستون;
		    اگر (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].ستون;
		    ورنہ
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    اگر (shl == تلاش_hl)
			check_cur_تلاش_hl(wp, shl);

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
			    ورنہ
				shl->endcol += (*mb_ptr2len)(p);
			}
			ورنہ
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    جاری;
		}
	    }
	    توڑو;
	}
	اگر (shl != تلاش_hl && cur != تہی)
	    cur = cur->mit_اگلا;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_مطابقت_سر;
    shl_flag = WIN_IS_POPUP(wp);
    جب (cur != تہی || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& (cur == تہی ||
			cur->mit_ترجیح > تلاش_HL_ترجیح))
	{
	    shl = تلاش_hl;
	    shl_flag = TRUE;
	}
	ورنہ
	    shl = &cur->mit_hl;
	اگر (shl->attr_cur != 0)
	{
	    تلاش_attr = shl->attr_cur;
	    *on_آخری_ستون = ستون + 1 >= shl->endcol;
	}
	اگر (shl != تلاش_hl && cur != تہی)
	    cur = cur->mit_اگلا;
    }
    // Only highlight one character after the last column.
    اگر (*(*سطر + ستون) == NUL && (did_سطر_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	تلاش_attr = 0;
    واپس تلاش_attr;
}

    صحیح
get_prevcol_hl_flag(win_T *wp, مطابقت_T *تلاش_hl, طویل curcol)
{
    طویل	prevcol = curcol;
    صحیح		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# اگر defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    اگر (popup_is_popup(wp))
	واپس FALSE;
# endif

    // we're not really at that column when skipping some text
    اگر ((طویل)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    اگر (!تلاش_hl->is_addpos && (prevcol == (طویل)تلاش_hl->startcol
		|| (prevcol > (طویل)تلاش_hl->startcol
					      && تلاش_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    ورنہ
    {
	cur = wp->w_مطابقت_سر;
	جب (cur != تہی)
	{
	    اگر (!cur->mit_hl.is_addpos && (prevcol == (طویل)cur->mit_hl.startcol
			|| (prevcol > (طویل)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		توڑو;
	    }
	    cur = cur->mit_اگلا;
	}
    }
    واپس prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    خالی
get_تلاش_مطابقت_hl(win_T *wp, مطابقت_T *تلاش_hl, طویل ستون, صحیح *char_attr)
{
    matchitem_T *cur;			// points to the match list
    مطابقت_T	*shl;			// points to search_hl or a match
    صحیح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_مطابقت_سر;
    shl_flag = WIN_IS_POPUP(wp);
    جب (cur != تہی || shl_flag == FALSE)
    {
	اگر (shl_flag == FALSE
		&& ((cur != تہی
			&& cur->mit_ترجیح > تلاش_HL_ترجیح)
		    || cur == تہی))
	{
	    shl = تلاش_hl;
	    shl_flag = TRUE;
	}
	ورنہ
	    shl = &cur->mit_hl;
	اگر (ستون - 1 == (طویل)shl->startcol
		&& (shl == تلاش_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	اگر (shl != تلاش_hl && cur != تہی)
	    cur = cur->mit_اگلا;
    }
}

#اگر_ختم // FEAT_SEARCH_EXTRA

#اگر defined(FEAT_EVAL)
# ifdef FEAT_تلاش_EXTRA
    ساکن صحیح
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    اگر (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	واپس FAIL;
    }

    اگر (dict_has_کلید(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    اگر ((di = dict_ڈھونڈو(tv->vval.v_dict, (char_u *)"window", -1)) == تہی)
	واپس OK;

    *win = ڈھونڈو_win_by_nr_or_id(&di->di_tv);
    اگر (*win == تہی)
    {
	emsg(_(e_invalid_کھڑکی_نمبر));
	واپس FAIL;
    }

    واپس OK;
}
# endif

/*
 * "clearmatches()" function
 */
    خالی
f_clearmatches(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    win_T   *win;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    win = get_optional_کھڑکی(argvars, 0);
    اگر (win != تہی)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    خالی
f_getmatches(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    صحیح		i;
    win_T	*win;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    win = get_optional_کھڑکی(argvars, 0);
    اگر (rettv_list_alloc(rettv) == FAIL || win == تہی)
	واپس;

    cur = win->w_مطابقت_سر;
    جب (cur != تہی)
    {
	dict = dict_alloc();
	اگر (dict == تہی)
	    واپس;
	اگر (cur->mit_مطابقت.regprog == تہی)
	{
	    // match added with matchaddpos()
	    برائے (i = 0; i < cur->mit_pos_گنتی; ++i)
	    {
		llpos_T	*llpos;
		حرف	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		اگر (llpos->lnum == 0)
		    توڑو;
		l = list_alloc();
		اگر (l == تہی)
		    توڑو;
		list_append_نمبر(l, (varnumber_T)llpos->lnum);
		اگر (llpos->ستون > 0)
		{
		    list_append_نمبر(l, (varnumber_T)llpos->ستون);
		    list_append_نمبر(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		اگر (dict_جوڑو_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	ورنہ
	{
	    dict_جوڑو_string(dict, "pattern", cur->mit_نمونہ);
	}
	dict_جوڑو_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_جوڑو_نمبر(dict, "priority", (طویل)cur->mit_ترجیح);
	dict_جوڑو_نمبر(dict, "id", (طویل)cur->mit_id);
#  اگر defined(FEAT_CONCEAL)
	اگر (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    صحیح	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_جوڑو_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	اگر (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_اگلا;
    }
# endif
}

/*
 * "setmatches()" function
 */
    خالی
f_setmatches(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = تہی;
    win_T	*win;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;
    win = get_optional_کھڑکی(argvars, 1);
    اگر (win == تہی)
	واپس;

    اگر ((l = argvars[0].vval.v_list) != تہی)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_پہلا;
	جب (li != تہی)
	{
	    اگر (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == تہی)
	    {
		emsg(_(e_invalid_دلیل));
		واپس;
	    }
	    اگر (!(dict_has_کلید(d, "group")
			&& (dict_has_کلید(d, "pattern")
			    || dict_has_کلید(d, "pos1"))
			&& dict_has_کلید(d, "priority")
			&& dict_has_کلید(d, "id")))
	    {
		emsg(_(e_invalid_دلیل));
		واپس;
	    }
	    li = li->li_اگلا;
	}

	clear_matches(win);
	li = l->lv_پہلا;
	جب (li != تہی)
	{
	    صحیح		i = 0;
	    حرف	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*گروہ;
	    صحیح		ترجیح;
	    صحیح		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    اگر (!dict_has_کلید(d, "pattern"))
	    {
		اگر (s == تہی)
		{
		    s = list_alloc();
		    اگر (s == تہی)
			واپس;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		برائے (i = 1; i < 9; i++)
		{
		    sprintf((حرف *)buf, (حرف *)"pos%d", i);
		    اگر ((di = dict_ڈھونڈو(d, (char_u *)buf, -1)) != تہی)
		    {
			اگر (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    واپس;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    ورنہ
			توڑو;
		}
	    }

	    گروہ = dict_get_string(d, "group", TRUE);
	    ترجیح = (صحیح)dict_get_نمبر(d, "priority");
	    id = (صحیح)dict_get_نمبر(d, "id");
	    conceal = dict_has_کلید(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : تہی;
	    اگر (i == 0)
	    {
		مطابقت_جوڑو(win, گروہ,
		    dict_get_string(d, "pattern", FALSE),
		    ترجیح, id, تہی, conceal);
	    }
	    ورنہ
	    {
		مطابقت_جوڑو(win, گروہ, تہی, ترجیح, id, s, conceal);
		list_unref(s);
		s = تہی;
	    }
	    vim_free(گروہ);
	    vim_free(conceal);

	    li = li->li_اگلا;
	}
	rettv->vval.v_نمبر = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    خالی
f_matchadd(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//گروہ
    char_u	*pat;		//نمونہ
    صحیح		prio = 10;	// default priority
    صحیح		id = -1;
    صحیح		error = FALSE;
    char_u	*conceal_char = تہی;
    win_T	*win = curwin;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_نمبر_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	واپس;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//گروہ
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//نمونہ
    اگر (grp == تہی || pat == تہی)
	واپس;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحیح)tv_get_نمبر_chk(&argvars[3], &error);
	    اگر (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		واپس;
	}
    }
    اگر (error == TRUE)
	واپس;
    اگر (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_مطابقت_nr), id);
	واپس;
    }

    rettv->vval.v_نمبر = مطابقت_جوڑو(win, grp, pat, prio, id, تہی,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    خالی
f_matchaddpos(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*گروہ;
    صحیح		prio = 10;
    صحیح		id = -1;
    صحیح		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = تہی;
    win_T	*win = curwin;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_نمبر_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	واپس;

    گروہ = tv_get_string_buf_chk(&argvars[0], buf);
    اگر (گروہ == تہی)
	واپس;

    اگر (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_دلیل_کا_str_must_be_list), "matchaddpos()");
	واپس;
    }
    l = argvars[1].vval.v_list;
    اگر (l == تہی || l->lv_len == 0)
	واپس;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحیح)tv_get_نمبر_chk(&argvars[3], &error);

	    اگر (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		واپس;
	}
    }
    اگر (error == TRUE)
	واپس;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    اگر (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_مطابقت_nr), id);
	واپس;
    }

    rettv->vval.v_نمبر = مطابقت_جوڑو(win, گروہ, تہی, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    خالی
f_matcharg(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) != OK)
	واپس;

# ifdef FEAT_تلاش_EXTRA
    صحیح	    id;
    matchitem_T *m;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(&argvars[0]);
    اگر (id >= 1 && id <= 3)
    {
	اگر ((m = get_مطابقت(curwin, id)) != تہی)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_نمونہ, -1);
	}
	ورنہ
	{
	    list_append_string(rettv->vval.v_list, تہی, -1);
	    list_append_string(rettv->vval.v_list, تہی, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    خالی
f_matchdelete(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_تلاش_EXTRA
    win_T   *win;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    win = get_optional_کھڑکی(argvars, 1);
    اگر (win == تہی)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = مطابقت_delete(win,
				       (صحیح)tv_get_نمبر(&argvars[0]), TRUE);
# endif
}
#اگر_ختم

#اگر defined(FEAT_تلاش_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    خالی
ex_مطابقت(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = تہی;
    char_u	*انتہا;
    صحیح		c;
    صحیح		id;

    اگر (eap->line2 <= 3)
	id = eap->line2;
    ورنہ
    {
	emsg(_(e_invalid_command));
	واپس;
    }

    // First clear any old pattern.
    اگر (!eap->skip)
	مطابقت_delete(curwin, id, FALSE);

    اگر (سرے_excmd2(eap->cmd, eap->arg))
	انتہا = eap->arg;
    ورنہ اگر ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || سرے_excmd2(eap->arg, eap->arg + 4))))
	انتہا = eap->arg + 4;
    ورنہ
    {
	p = skiptowhite(eap->arg);
	اگر (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	اگر (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	    واپس;
	}
	انتہا = skip_regexp(p + 1, *p, TRUE);
	اگر (!eap->skip)
	{
	    اگر (*انتہا != NUL && !سرے_excmd2(انتہا, skipwhite(انتہا + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, انتہا);
		واپس;
	    }
	    اگر (*انتہا != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_دلیل_str), p);
		واپس;
	    }

	    c = *انتہا;
	    *انتہا = NUL;
	    مطابقت_جوڑو(curwin, g, p + 1, 10, id, تہی, تہی);
	    vim_free(g);
	    *انتہا = c;
	}
    }
    eap->nextcmd = ڈھونڈو_nextcmd(انتہا);
}
#اگر_ختم
