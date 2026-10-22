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

#ضمن "vim.h"

#إذا defined(FEAT_جستجو_EXTRA)

# define جستجو_HL_اولویت 0

/*
 * Add match to the match list of window "wp".
 * If "pat" is not NULL the pattern will be highlighted with the group "grp"
 * with priority "prio".
 * If "pos_list" is not NULL the list of positions defines the highlights.
 * Optionally, a desired ID "id" can be specified (greater than or equal to 1).
 * If no particular ID is desired, -1 must be specified for "id".
 * Return ID of added match, -1 on failure.
 */
    ساكن صحيح
تطابق_جمع_کن(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    صحيح		prio,
    صحيح		id,
    list_T	*pos_list,
    char_u      *conceal_char استفادهنشده) // pointer to conceal replacement char
{
    matchitem_T	*cur;
    matchitem_T	*prev;
    matchitem_T	*m = عدم;
    صحيح		hlg_id;
    regprog_T	*regprog = عدم;
    صحيح		rtype = UPD_SOME_VALID;

    إذا (*grp == NUL || (pat != عدم && *pat == NUL))
	أرجع -1;
    إذا (id < -1 || id == 0)
    {
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_1), id);
	أرجع -1;
    }
    إذا (id == -1)
    {
	// use the next available match ID
	id = wp->w_بعدی_تطابق_id++;
    }
    وإلا
    {
	// check the given ID is not already in use
	لكل (cur = wp->w_تطابق_سر; cur != عدم; cur = cur->mit_بعدی)
	    إذا (cur->mit_id == id)
	    {
		semsg(_(e_id_already_گرفته_nr), id);
		أرجع -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	إذا (wp->w_بعدی_تطابق_id < id + 100)
	    wp->w_بعدی_تطابق_id = id + 100;
    }

    إذا ((hlg_id = syn_namen2id(grp, (صحيح)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_گروه_name_str), grp);
	أرجع -1;
    }
    إذا (pat != عدم && (regprog = vim_regcomp(pat, RE_MAGIC)) == عدم)
    {
	semsg(_(e_invalid_آرگومان_str), pat);
	أرجع -1;
    }

    // Build new match.
    m = ALLOC_CLEAR_ONE(matchitem_T);
    إذا (m == عدم)
	اذهب fail;
    إذا (pos_list != عدم && pos_list->lv_len > 0)
    {
	m->mit_pos_array = ALLOC_CLEAR_MULT(llpos_T, pos_list->lv_len);
	إذا (m->mit_pos_array == عدم)
	    اذهب fail;
	m->mit_pos_شمار = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_اولویت = prio;
    m->mit_الگو = pat == عدم ? عدم : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_تطابق.regprog = regprog;
    m->mit_تطابق.rmm_ic = FALSE;
    m->mit_تطابق.rmm_maxcol = 0;
# إذا defined(FEAT_CONCEAL)
    m->mit_conceal_char = 0;
    إذا (conceal_char != عدم)
	m->mit_conceal_char = (*mb_ptr2char)(conceal_char);
# endif

    // Set up position matches
    إذا (pos_list != عدم)
    {
	linenr_T	toplnum = 0;
	linenr_T	botlnum = 0;
	listitem_T	*li;
	صحيح		i;

	CHECK_LIST_MATERIALIZE(pos_list);
	لكل (i = 0, li = pos_list->lv_اولی; li != عدم; i++, li = li->li_بعدی)
	{
	    linenr_T	lnum = 0;
	    colnr_T	ستون = 0;
	    صحيح		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    صحيح		error = FALSE;

	    إذا (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		إذا (subl == عدم)
		    اذهب fail;
		subli = subl->lv_اولی;
		إذا (subli == عدم)
		    اذهب fail;
		lnum = tv_get_عدد_chk(&subli->li_tv, &error);
		إذا (error == TRUE)
		    اذهب fail;
		إذا (lnum == 0)
		{
		    --i;
		    استمر;
		}
		m->mit_pos_array[i].lnum = lnum;
		subli = subli->li_بعدی;
		إذا (subli != عدم)
		{
		    ستون = tv_get_عدد_chk(&subli->li_tv, &error);
		    إذا (error == TRUE)
			اذهب fail;
		    subli = subli->li_بعدی;
		    إذا (subli != عدم)
		    {
			len = tv_get_عدد_chk(&subli->li_tv, &error);
			إذا (error == TRUE)
			    اذهب fail;
		    }
		}
		m->mit_pos_array[i].ستون = ستون;
		m->mit_pos_array[i].len = len;
	    }
	    وإلا إذا (li->li_tv.v_type == VAR_عدد)
	    {
		إذا (li->li_tv.vval.v_عدد == 0)
		{
		    --i;
		    استمر;
		}
		m->mit_pos_array[i].lnum = li->li_tv.vval.v_عدد;
		m->mit_pos_array[i].ستون = 0;
		m->mit_pos_array[i].len = 0;
	    }
	    وإلا
	    {
		emsg(_(e_list_or_عدد_required));
		اذهب fail;
	    }
	    إذا (toplnum == 0 || lnum < toplnum)
		toplnum = lnum;
	    إذا (botlnum == 0 || lnum >= botlnum)
		botlnum = lnum + 1;
	}

	// Calculate top and bottom lines for redrawing area
	إذا (toplnum != 0)
	{
	    redraw_win_range_later(wp, toplnum, botlnum);
	    m->mit_toplnum = toplnum;
	    m->mit_botlnum = botlnum;
	    rtype = UPD_VALID;
	}
    }

    // Insert new match.  The match list is in ascending order with regard to
    // the match priorities.
    cur = wp->w_تطابق_سر;
    prev = cur;
    طالما (cur != عدم && prio >= cur->mit_اولویت)
    {
	prev = cur;
	cur = cur->mit_بعدی;
    }
    إذا (cur == prev)
	wp->w_تطابق_سر = m;
    وإلا
	prev->mit_بعدی = m;
    m->mit_بعدی = cur;

    redraw_win_later(wp, rtype);
    أرجع id;

fail:
    vim_regfree(regprog);
    إذا (m != عدم)
    {
	vim_free(m->mit_الگو);
	vim_free(m->mit_pos_array);
	vim_free(m);
    }
    أرجع -1;
}

/*
 * Delete match with ID 'id' in the match list of window 'wp'.
 * Print error messages if 'perr' is TRUE.
 */
    ساكن صحيح
تطابق_delete(win_T *wp, صحيح id, صحيح perr)
{
    matchitem_T	*cur = wp->w_تطابق_سر;
    matchitem_T	*prev = cur;
    صحيح		rtype = UPD_SOME_VALID;

    إذا (id < 1)
    {
	إذا (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_2),
									   id);
	أرجع -1;
    }
    طالما (cur != عدم && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_بعدی;
    }
    إذا (cur == عدم)
    {
	إذا (perr == TRUE)
	    semsg(_(e_id_not_یافت_nr), id);
	أرجع -1;
    }
    إذا (cur == prev)
	wp->w_تطابق_سر = cur->mit_بعدی;
    وإلا
	prev->mit_بعدی = cur->mit_بعدی;
    vim_regfree(cur->mit_تطابق.regprog);
    vim_free(cur->mit_الگو);
    إذا (cur->mit_toplnum != 0)
    {
	redraw_win_range_later(wp, cur->mit_toplnum, cur->mit_botlnum);
	rtype = UPD_VALID;
    }
    vim_free(cur->mit_pos_array);
    vim_free(cur);
    redraw_win_later(wp, rtype);
    أرجع 0;
}

/*
 * Delete all matches in the match list of window 'wp'.
 */
    فراغ
clear_matches(win_T *wp)
{
    matchitem_T *m;

    طالما (wp->w_تطابق_سر != عدم)
    {
	m = wp->w_تطابق_سر->mit_بعدی;
	vim_regfree(wp->w_تطابق_سر->mit_تطابق.regprog);
	vim_free(wp->w_تطابق_سر->mit_الگو);
	vim_free(wp->w_تطابق_سر->mit_pos_array);
	vim_free(wp->w_تطابق_سر);
	wp->w_تطابق_سر = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    ساكن matchitem_T *
get_تطابق(win_T *wp, صحيح id)
{
    matchitem_T *cur = wp->w_تطابق_سر;

    طالما (cur != عدم && cur->mit_id != id)
	cur = cur->mit_بعدی;
    أرجع cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    فراغ
init_جستجو_hl(win_T *wp, تطابق_T *جستجو_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //تطابق
    cur = wp->w_تطابق_سر;
    طالما (cur != عدم)
    {
	cur->mit_hl.rm = cur->mit_تطابق;
	إذا (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	وإلا
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_میانگیر;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.اولی_lnum = 0;
	cur = cur->mit_بعدی;
    }
    // Must update this every time since highlight group override can change it.
    جستجو_hl->attr = HL_ATTR(HLF_L);

    جستجو_hl->buf = wp->w_میانگیر;
    جستجو_hl->lnum = 0;
    جستجو_hl->اولی_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ساكن صحيح
بعدی_جستجو_hl_pos(
    تطابق_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *تطابق,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    صحيح	    i;
    صحيح	    یافت = -1;

    لكل (i = تطابق->mit_pos_cur; i < تطابق->mit_pos_شمار; i++)
    {
	llpos_T	*pos = &تطابق->mit_pos_array[i];

	إذا (pos->lnum == 0)
	    اكسر;
	إذا (pos->len == 0 && pos->ستون < mincol)
	    استمر;
	إذا (pos->lnum == lnum)
	{
	    إذا (یافت >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		إذا (pos->ستون < تطابق->mit_pos_array[یافت].ستون)
		{
		    llpos_T	tmp = *pos;

		    *pos = تطابق->mit_pos_array[یافت];
		    تطابق->mit_pos_array[یافت] = tmp;
		}
	    }
	    وإلا
		یافت = i;
	}
    }
    تطابق->mit_pos_cur = 0;
    إذا (یافت >= 0)
    {
	colnr_T	شروع = تطابق->mit_pos_array[یافت].ستون == 0
				     ? 0 : تطابق->mit_pos_array[یافت].ستون - 1;
	colnr_T	انتها = تطابق->mit_pos_array[یافت].ستون == 0
			    ? MAXCOL : شروع + تطابق->mit_pos_array[یافت].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].ستون = شروع;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].ستون = انتها;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	تطابق->mit_pos_cur = یافت + 1;
	أرجع 1;
    }
    أرجع 0;
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    ساكن فراغ
بعدی_جستجو_hl(
    win_T	    *win,
    تطابق_T	    *جستجو_hl,
    تطابق_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    دراز	nmatched;
    صحيح		called_emsg_before = called_emsg;
    صحيح		timed_بیرون = FALSE;

    // for :{range}s/pat only highlight inside the range
    إذا ((lnum < جستجو_اولی_سطر || lnum > جستجو_آخرین_سطر) && cur == عدم)
    {
	shl->lnum = 0;
	أرجع;
    }

    إذا (shl->lnum != 0)
    {
	// Check for three situations:
	// 1. If the "lnum" is below a previous match, start a new search.
	// 2. If the previous match includes "mincol", use it.
	// 3. Continue after the previous match.
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	إذا (lnum > l)
	    shl->lnum = 0;
	وإلا إذا (lnum < l || shl->rm.endpos[0].ستون > mincol)
	    أرجع;
    }

    // Repeat searching for a match until one is found that includes "mincol"
    // or none is found in this line.
    لكل (;;)
    {
	// Three situations:
	// 1. No useful previous match: search from start of line.
	// 2. Not Vi compatible or empty match: continue at next character.
	//    Break the loop if this is beyond the end of the line.
	// 3. Vi compatible searching: continue at end of previous match.
	إذا (shl->lnum == 0)
	    matchcol = 0;
	وإلا إذا (vim_strchr(p_cpo, CPO_جستجو) == عدم
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].ستون <= shl->rm.startpos[0].ستون))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].ستون;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    إذا (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		اكسر;
	    }
	    إذا (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    وإلا
		++matchcol;
	}
	وإلا
	    matchcol = shl->rm.endpos[0].ستون;

	shl->lnum = lnum;
	إذا (shl->rm.regprog != عدم)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    صحيح regprog_is_copy = (shl != جستجو_hl && cur != عدم
			  && shl == &cur->mit_hl
			  && cur->mit_تطابق.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_بیرون);
	    // Copy the regprog, in case it got freed and recompiled.
	    إذا (regprog_is_copy)
		cur->mit_تطابق.regprog = cur->mit_hl.rm.regprog;

	    إذا (called_emsg > called_emsg_before || got_int || timed_بیرون)
	    {
		// Error while handling regexp: stop using this regexp.
		إذا (shl == جستجو_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    مجموعه_no_hlsearch(TRUE);
		}
		shl->rm.regprog = عدم;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		اكسر;
	    }
	}
	وإلا إذا (cur != عدم)
	    nmatched = بعدی_جستجو_hl_pos(shl, lnum, cur, matchcol);
	وإلا
	    nmatched = 0;
	إذا (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    اكسر;
	}
	إذا (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].ستون >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].ستون > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    اكسر;			// useful match found
	}
    }
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    فراغ
prepare_جستجو_hl(win_T *wp, تطابق_T *جستجو_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    تطابق_T	*shl;		// points to search_hl or a match
    صحيح		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    صحيح		pos_inprogress;	// marks that position match search is
				// in progress
    صحيح		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_تطابق_سر;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE)
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (shl->rm.regprog != عدم
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    إذا (shl->اولی_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		لكل (shl->اولی_lnum = lnum;
			   shl->اولی_lnum > wp->w_topline; --shl->اولی_lnum)
		    إذا (hasFoldingWin(wp, shl->اولی_lnum - 1,
						      عدم, عدم, TRUE, عدم))
			اكسر;
# وإلا
		shl->اولی_lnum = wp->w_topline;
# endif
	    }
	    إذا (cur != عدم)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    طالما (shl->اولی_lnum < lnum && (shl->rm.regprog != عدم
					  || (cur != عدم && pos_inprogress)))
	    {
		بعدی_جستجو_hl(wp, جستجو_hl, shl, shl->اولی_lnum, (colnr_T)n,
					       shl == جستجو_hl ? عدم : cur);
		pos_inprogress = cur == عدم || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		إذا (shl->lnum != 0)
		{
		    shl->اولی_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].ستون;
		}
		وإلا
		{
		    ++shl->اولی_lnum;
		    n = 0;
		}
	    }
	}
	إذا (shl != جستجو_hl && cur != عدم)
	    cur = cur->mit_بعدی;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ساكن فراغ
check_cur_جستجو_hl(win_T *wp, تطابق_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    إذا (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.ستون >= shl->rm.startpos[0].ستون)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.ستون < shl->rm.endpos[0].ستون))
	shl->has_cursor = TRUE;
    وإلا
	shl->has_cursor = FALSE;
}

/*
 * Prepare for 'hlsearch' and match highlighting in one window line.
 * Return TRUE if there is such highlighting and set "search_attr" to the
 * current highlight attribute.
 */
    صحيح
prepare_جستجو_hl_سطر(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **سطر,
	تطابق_T	    *جستجو_hl,
	صحيح	    *جستجو_attr)
{
    matchitem_T *cur;			// points to the match list
    تطابق_T	*shl;			// points to search_hl or a match
    صحيح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    صحيح		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_تطابق_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE)
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	shl->is_addpos = FALSE;
	shl->has_cursor = FALSE;
	إذا (cur != عدم)
	    cur->mit_pos_cur = 0;
	بعدی_جستجو_hl(wp, جستجو_hl, shl, lnum, mincol,
						shl == جستجو_hl ? عدم : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);

	إذا (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    إذا (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].ستون;
	    وإلا
		shl->startcol = 0;
	    إذا (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].ستون;
	    وإلا
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    إذا (shl == جستجو_hl)
		check_cur_جستجو_hl(wp, shl);

	    // Highlight one character for an empty match.
	    إذا (shl->startcol == shl->endcol)
	    {
		إذا (has_mbyte && (*سطر)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*سطر) + shl->endcol);
		وإلا
		    ++shl->endcol;
	    }
	    إذا ((دراز)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*جستجو_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	إذا (shl != جستجو_hl && cur != عدم)
	    cur = cur->mit_بعدی;
    }
    أرجع area_highlighting;
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
    صحيح
update_جستجو_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    ستون,
	char_u	    **سطر,
	تطابق_T	    *جستجو_hl,
	صحيح	    *has_تطابق_conc استفادهنشده,
	صحيح	    *تطابق_conc استفادهنشده,
	صحيح	    did_سطر_attr,
	صحيح	    lcs_eol_one,
	صحيح	    *on_آخرین_ستون)
{
    matchitem_T *cur;		    // points to the match list
    تطابق_T	*shl;		    // points to search_hl or a match
    صحيح		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    صحيح		pos_inprogress;	    // marks that position match search is in
				    //پیشرفت
    صحيح		جستجو_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_تطابق_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& (cur == عدم
			|| cur->mit_اولویت > جستجو_HL_اولویت))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (cur != عدم)
	    cur->mit_pos_cur = 0;
	pos_inprogress = TRUE;
	طالما (shl->rm.regprog != عدم || (cur != عدم && pos_inprogress))
	{
	    إذا (shl->startcol != MAXCOL
		    && ستون >= shl->startcol
		    && ستون < shl->endcol)
	    {
		صحيح بعدی_ستون = ستون + mb_ptr2len(*سطر + ستون);

		إذا (shl->endcol < بعدی_ستون)
		    shl->endcol = بعدی_ستون;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		إذا (cur != عدم
			&& shl != جستجو_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_تطابق_conc = ستون == shl->startcol ? 2 : 1;
		    *تطابق_conc = cur->mit_conceal_char;
		}
		وإلا
		    *has_تطابق_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		إذا (shl == جستجو_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    إذا (shl->attr_cur != shl->attr)
			جستجو_hl_has_cursor_lnum = lnum;
		}

	    }
	    وإلا إذا (ستون == shl->endcol)
	    {
		shl->attr_cur = 0;
		بعدی_جستجو_hl(wp, جستجو_hl, shl, lnum, ستون,
					       shl == جستجو_hl ? عدم : cur);
		pos_inprogress = !(cur == عدم || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);

		إذا (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].ستون;
		    إذا (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].ستون;
		    وإلا
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    إذا (shl == جستجو_hl)
			check_cur_جستجو_hl(wp, shl);

		    إذا (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			إذا (has_mbyte)
			{
			    char_u *p = *سطر + shl->endcol;

			    إذا (*p == NUL)
				// consistent with non-mbyte
				++shl->endcol;
			    وإلا
				shl->endcol += (*mb_ptr2len)(p);
			}
			وإلا
			    ++shl->endcol;
		    }

		    // Loop to check if the match starts at the
		    // current position
		    استمر;
		}
	    }
	    اكسر;
	}
	إذا (shl != جستجو_hl && cur != عدم)
	    cur = cur->mit_بعدی;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_تطابق_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& (cur == عدم ||
			cur->mit_اولویت > جستجو_HL_اولویت))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (shl->attr_cur != 0)
	{
	    جستجو_attr = shl->attr_cur;
	    *on_آخرین_ستون = ستون + 1 >= shl->endcol;
	}
	إذا (shl != جستجو_hl && cur != عدم)
	    cur = cur->mit_بعدی;
    }
    // Only highlight one character after the last column.
    إذا (*(*سطر + ستون) == NUL && (did_سطر_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	جستجو_attr = 0;
    أرجع جستجو_attr;
}

    صحيح
get_prevcol_hl_flag(win_T *wp, تطابق_T *جستجو_hl, دراز curcol)
{
    دراز	prevcol = curcol;
    صحيح		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# إذا defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    إذا (popup_is_popup(wp))
	أرجع FALSE;
# endif

    // we're not really at that column when skipping some text
    إذا ((دراز)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    إذا (!جستجو_hl->is_addpos && (prevcol == (دراز)جستجو_hl->startcol
		|| (prevcol > (دراز)جستجو_hl->startcol
					      && جستجو_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    وإلا
    {
	cur = wp->w_تطابق_سر;
	طالما (cur != عدم)
	{
	    إذا (!cur->mit_hl.is_addpos && (prevcol == (دراز)cur->mit_hl.startcol
			|| (prevcol > (دراز)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		اكسر;
	    }
	    cur = cur->mit_بعدی;
	}
    }
    أرجع prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    فراغ
get_جستجو_تطابق_hl(win_T *wp, تطابق_T *جستجو_hl, دراز ستون, صحيح *char_attr)
{
    matchitem_T *cur;			// points to the match list
    تطابق_T	*shl;			// points to search_hl or a match
    صحيح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_تطابق_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& ((cur != عدم
			&& cur->mit_اولویت > جستجو_HL_اولویت)
		    || cur == عدم))
	{
	    shl = جستجو_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (ستون - 1 == (دراز)shl->startcol
		&& (shl == جستجو_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	إذا (shl != جستجو_hl && cur != عدم)
	    cur = cur->mit_بعدی;
    }
}

#پایان_اگر // FEAT_SEARCH_EXTRA

#إذا defined(FEAT_EVAL)
# ifdef FEAT_جستجو_EXTRA
    ساكن صحيح
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    إذا (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	أرجع FAIL;
    }

    إذا (dict_has_کلید(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    إذا ((di = dict_پیدا_کن(tv->vval.v_dict, (char_u *)"window", -1)) == عدم)
	أرجع OK;

    *win = پیدا_کن_win_by_nr_or_id(&di->di_tv);
    إذا (*win == عدم)
    {
	emsg(_(e_invalid_پنجره_عدد));
	أرجع FAIL;
    }

    أرجع OK;
}
# endif

/*
 * "clearmatches()" function
 */
    فراغ
f_clearmatches(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    win_T   *win;

    إذا (درون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    win = get_optional_پنجره(argvars, 0);
    إذا (win != عدم)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    فراغ
f_getmatches(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    صحيح		i;
    win_T	*win;

    إذا (درون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    win = get_optional_پنجره(argvars, 0);
    إذا (rettv_list_alloc(rettv) == FAIL || win == عدم)
	أرجع;

    cur = win->w_تطابق_سر;
    طالما (cur != عدم)
    {
	dict = dict_alloc();
	إذا (dict == عدم)
	    أرجع;
	إذا (cur->mit_تطابق.regprog == عدم)
	{
	    // match added with matchaddpos()
	    لكل (i = 0; i < cur->mit_pos_شمار; ++i)
	    {
		llpos_T	*llpos;
		حرف	buf[30];  // use 30 to avoid compiler warning
		list_T	*l;

		llpos = &cur->mit_pos_array[i];
		إذا (llpos->lnum == 0)
		    اكسر;
		l = list_alloc();
		إذا (l == عدم)
		    اكسر;
		list_append_عدد(l, (varnumber_T)llpos->lnum);
		إذا (llpos->ستون > 0)
		{
		    list_append_عدد(l, (varnumber_T)llpos->ستون);
		    list_append_عدد(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		إذا (dict_جمع_کن_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	وإلا
	{
	    dict_جمع_کن_string(dict, "pattern", cur->mit_الگو);
	}
	dict_جمع_کن_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_جمع_کن_عدد(dict, "priority", (دراز)cur->mit_اولویت);
	dict_جمع_کن_عدد(dict, "id", (دراز)cur->mit_id);
#  إذا defined(FEAT_CONCEAL)
	إذا (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    صحيح	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_جمع_کن_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	إذا (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_بعدی;
    }
# endif
}

/*
 * "setmatches()" function
 */
    فراغ
f_setmatches(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = عدم;
    win_T	*win;

    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;
    win = get_optional_پنجره(argvars, 1);
    إذا (win == عدم)
	أرجع;

    إذا ((l = argvars[0].vval.v_list) != عدم)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_اولی;
	طالما (li != عدم)
	{
	    إذا (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == عدم)
	    {
		emsg(_(e_invalid_آرگومان));
		أرجع;
	    }
	    إذا (!(dict_has_کلید(d, "group")
			&& (dict_has_کلید(d, "pattern")
			    || dict_has_کلید(d, "pos1"))
			&& dict_has_کلید(d, "priority")
			&& dict_has_کلید(d, "id")))
	    {
		emsg(_(e_invalid_آرگومان));
		أرجع;
	    }
	    li = li->li_بعدی;
	}

	clear_matches(win);
	li = l->lv_اولی;
	طالما (li != عدم)
	{
	    صحيح		i = 0;
	    حرف	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*گروه;
	    صحيح		اولویت;
	    صحيح		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    إذا (!dict_has_کلید(d, "pattern"))
	    {
		إذا (s == عدم)
		{
		    s = list_alloc();
		    إذا (s == عدم)
			أرجع;
		    ++s->lv_refcount;
		}

		// match from matchaddpos()
		لكل (i = 1; i < 9; i++)
		{
		    sprintf((حرف *)buf, (حرف *)"pos%d", i);
		    إذا ((di = dict_پیدا_کن(d, (char_u *)buf, -1)) != عدم)
		    {
			إذا (di->di_tv.v_type != VAR_LIST)
			{
			    list_unref(s);
			    أرجع;
			}

			list_append_tv(s, &di->di_tv);
		    }
		    وإلا
			اكسر;
		}
	    }

	    گروه = dict_get_string(d, "group", TRUE);
	    اولویت = (صحيح)dict_get_عدد(d, "priority");
	    id = (صحيح)dict_get_عدد(d, "id");
	    conceal = dict_has_کلید(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : عدم;
	    إذا (i == 0)
	    {
		تطابق_جمع_کن(win, گروه,
		    dict_get_string(d, "pattern", FALSE),
		    اولویت, id, عدم, conceal);
	    }
	    وإلا
	    {
		تطابق_جمع_کن(win, گروه, عدم, اولویت, id, s, conceal);
		list_unref(s);
		s = عدم;
	    }
	    vim_free(گروه);
	    vim_free(conceal);

	    li = li->li_بعدی;
	}
	rettv->vval.v_عدد = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    فراغ
f_matchadd(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//گروه
    char_u	*pat;		//الگو
    صحيح		prio = 10;	// default priority
    صحيح		id = -1;
    صحيح		error = FALSE;
    char_u	*conceal_char = عدم;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	أرجع;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//گروه
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//الگو
    إذا (grp == عدم || pat == عدم)
	أرجع;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحيح)tv_get_عدد_chk(&argvars[2], &error);
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحيح)tv_get_عدد_chk(&argvars[3], &error);
	    إذا (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		أرجع;
	}
    }
    إذا (error == TRUE)
	أرجع;
    إذا (id >= 1 && id <= 3)
    {
	semsg(_(e_id_is_reserved_for_تطابق_nr), id);
	أرجع;
    }

    rettv->vval.v_عدد = تطابق_جمع_کن(win, grp, pat, prio, id, عدم,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    فراغ
f_matchaddpos(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*گروه;
    صحيح		prio = 10;
    صحيح		id = -1;
    صحيح		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = عدم;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	أرجع;

    گروه = tv_get_string_buf_chk(&argvars[0], buf);
    إذا (گروه == عدم)
	أرجع;

    إذا (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_آرگومان_از_str_must_be_list), "matchaddpos()");
	أرجع;
    }
    l = argvars[1].vval.v_list;
    إذا (l == عدم || l->lv_len == 0)
	أرجع;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	prio = (صحيح)tv_get_عدد_chk(&argvars[2], &error);
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	{
	    id = (صحيح)tv_get_عدد_chk(&argvars[3], &error);

	    إذا (argvars[4].v_type != VAR_UNKNOWN
		&& matchadd_dict_arg(&argvars[4], &conceal_char, &win) == FAIL)
		أرجع;
	}
    }
    إذا (error == TRUE)
	أرجع;

    // id == 3 is ok because matchaddpos() is supposed to substitute :3match
    إذا (id == 1 || id == 2)
    {
	semsg(_(e_id_is_reserved_for_تطابق_nr), id);
	أرجع;
    }

    rettv->vval.v_عدد = تطابق_جمع_کن(win, گروه, عدم, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    فراغ
f_matcharg(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) != OK)
	أرجع;

# ifdef FEAT_جستجو_EXTRA
    صحيح	    id;
    matchitem_T *m;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(&argvars[0]);
    إذا (id >= 1 && id <= 3)
    {
	إذا ((m = get_تطابق(curwin, id)) != عدم)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_الگو, -1);
	}
	وإلا
	{
	    list_append_string(rettv->vval.v_list, عدم, -1);
	    list_append_string(rettv->vval.v_list, عدم, -1);
	}
    }
# endif
}

/*
 * "matchdelete()" function
 */
    فراغ
f_matchdelete(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_جستجو_EXTRA
    win_T   *win;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    win = get_optional_پنجره(argvars, 1);
    إذا (win == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = تطابق_delete(win,
				       (صحيح)tv_get_عدد(&argvars[0]), TRUE);
# endif
}
#پایان_اگر

#إذا defined(FEAT_جستجو_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    فراغ
ex_تطابق(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = عدم;
    char_u	*انتها;
    صحيح		c;
    صحيح		id;

    إذا (eap->line2 <= 3)
	id = eap->line2;
    وإلا
    {
	emsg(_(e_invalid_command));
	أرجع;
    }

    // First clear any old pattern.
    إذا (!eap->skip)
	تطابق_delete(curwin, id, FALSE);

    إذا (دوسر_excmd2(eap->cmd, eap->arg))
	انتها = eap->arg;
    وإلا إذا ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || دوسر_excmd2(eap->arg, eap->arg + 4))))
	انتها = eap->arg + 4;
    وإلا
    {
	p = skiptowhite(eap->arg);
	إذا (!eap->skip)
	    g = vim_strnsave(eap->arg, p - eap->arg);
	p = skipwhite(p);
	إذا (*p == NUL)
	{
	    // There must be two arguments.
	    vim_free(g);
	    semsg(_(e_invalid_آرگومان_str), eap->arg);
	    أرجع;
	}
	انتها = skip_regexp(p + 1, *p, TRUE);
	إذا (!eap->skip)
	{
	    إذا (*انتها != NUL && !دوسر_excmd2(انتها, skipwhite(انتها + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, انتها);
		أرجع;
	    }
	    إذا (*انتها != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_آرگومان_str), p);
		أرجع;
	    }

	    c = *انتها;
	    *انتها = NUL;
	    تطابق_جمع_کن(curwin, g, p + 1, 10, id, عدم, عدم);
	    vim_free(g);
	    *انتها = c;
	}
    }
    eap->nextcmd = پیدا_کن_nextcmd(انتها);
}
#پایان_اگر
