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

#إذا defined(FEAT_ڳولا_EXTRA)

# define ڳولا_HL_ترجيح 0

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
ميلاپ_جوڙيو(
    win_T	*wp,
    char_u	*grp,
    char_u	*pat,
    صحيح		prio,
    صحيح		id,
    list_T	*pos_list,
    char_u      *conceal_char اڻ_استعمال_ٿيل) // pointer to conceal replacement char
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
	semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_منزل_one_1), id);
	أرجع -1;
    }
    إذا (id == -1)
    {
	// use the next available match ID
	id = wp->w_ايندڙ_ميلاپ_id++;
    }
    وإلا
    {
	// check the given ID is not already in use
	لكل (cur = wp->w_ميلاپ_سر; cur != عدم; cur = cur->mit_ايندڙ)
	    إذا (cur->mit_id == id)
	    {
		semsg(_(e_id_already_ورتل_nr), id);
		أرجع -1;
	    }

	// Make sure the next match ID is always higher than the highest
	// manually selected ID.  Add some extra in case a few more IDs are
	// added soon.
	إذا (wp->w_ايندڙ_ميلاپ_id < id + 100)
	    wp->w_ايندڙ_ميلاپ_id = id + 100;
    }

    إذا ((hlg_id = syn_namen2id(grp, (صحيح)STRLEN(grp))) == 0)
    {
	semsg(_(e_no_such_highlight_گروهه_name_str), grp);
	أرجع -1;
    }
    إذا (pat != عدم && (regprog = vim_regcomp(pat, RE_MAGIC)) == عدم)
    {
	semsg(_(e_invalid_دليل_str), pat);
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
	m->mit_pos_ڳڻپ = pos_list->lv_len;
    }
    m->mit_id = id;
    m->mit_ترجيح = prio;
    m->mit_نمونو = pat == عدم ? عدم : vim_strsave(pat);
    m->mit_hlg_id = hlg_id;
    m->mit_ميلاپ.regprog = regprog;
    m->mit_ميلاپ.rmm_ic = FALSE;
    m->mit_ميلاپ.rmm_maxcol = 0;
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
	لكل (i = 0, li = pos_list->lv_پهريون; li != عدم; i++, li = li->li_ايندڙ)
	{
	    linenr_T	lnum = 0;
	    colnr_T	کانو = 0;
	    صحيح		len = 1;
	    list_T	*subl;
	    listitem_T	*subli;
	    صحيح		error = FALSE;

	    إذا (li->li_tv.v_type == VAR_LIST)
	    {
		subl = li->li_tv.vval.v_list;
		إذا (subl == عدم)
		    اذهب fail;
		subli = subl->lv_پهريون;
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
		subli = subli->li_ايندڙ;
		إذا (subli != عدم)
		{
		    کانو = tv_get_عدد_chk(&subli->li_tv, &error);
		    إذا (error == TRUE)
			اذهب fail;
		    subli = subli->li_ايندڙ;
		    إذا (subli != عدم)
		    {
			len = tv_get_عدد_chk(&subli->li_tv, &error);
			إذا (error == TRUE)
			    اذهب fail;
		    }
		}
		m->mit_pos_array[i].کانو = کانو;
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
		m->mit_pos_array[i].کانو = 0;
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
    cur = wp->w_ميلاپ_سر;
    prev = cur;
    طالما (cur != عدم && prio >= cur->mit_ترجيح)
    {
	prev = cur;
	cur = cur->mit_ايندڙ;
    }
    إذا (cur == prev)
	wp->w_ميلاپ_سر = m;
    وإلا
	prev->mit_ايندڙ = m;
    m->mit_ايندڙ = cur;

    redraw_win_later(wp, rtype);
    أرجع id;

fail:
    vim_regfree(regprog);
    إذا (m != عدم)
    {
	vim_free(m->mit_نمونو);
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
ميلاپ_delete(win_T *wp, صحيح id, صحيح perr)
{
    matchitem_T	*cur = wp->w_ميلاپ_سر;
    matchitem_T	*prev = cur;
    صحيح		rtype = UPD_SOME_VALID;

    إذا (id < 1)
    {
	إذا (perr == TRUE)
	    semsg(_(e_invalid_id_nr_must_be_greater_than_or_equal_منزل_one_2),
									   id);
	أرجع -1;
    }
    طالما (cur != عدم && cur->mit_id != id)
    {
	prev = cur;
	cur = cur->mit_ايندڙ;
    }
    إذا (cur == عدم)
    {
	إذا (perr == TRUE)
	    semsg(_(e_id_not_مليو_nr), id);
	أرجع -1;
    }
    إذا (cur == prev)
	wp->w_ميلاپ_سر = cur->mit_ايندڙ;
    وإلا
	prev->mit_ايندڙ = cur->mit_ايندڙ;
    vim_regfree(cur->mit_ميلاپ.regprog);
    vim_free(cur->mit_نمونو);
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

    طالما (wp->w_ميلاپ_سر != عدم)
    {
	m = wp->w_ميلاپ_سر->mit_ايندڙ;
	vim_regfree(wp->w_ميلاپ_سر->mit_ميلاپ.regprog);
	vim_free(wp->w_ميلاپ_سر->mit_نمونو);
	vim_free(wp->w_ميلاپ_سر->mit_pos_array);
	vim_free(wp->w_ميلاپ_سر);
	wp->w_ميلاپ_سر = m;
    }
    redraw_win_later(wp, UPD_SOME_VALID);
}

/*
 * Get match from ID 'id' in window 'wp'.
 * Return NULL if match not found.
 */
    ساكن matchitem_T *
get_ميلاپ(win_T *wp, صحيح id)
{
    matchitem_T *cur = wp->w_ميلاپ_سر;

    طالما (cur != عدم && cur->mit_id != id)
	cur = cur->mit_ايندڙ;
    أرجع cur;
}

/*
 * Init for calling prepare_search_hl().
 */
    فراغ
init_ڳولا_hl(win_T *wp, ميلاپ_T *ڳولا_hl)
{
    matchitem_T *cur;

    // Setup for match and 'hlsearch' highlighting.  Disable any previous
    //ميلاپ
    cur = wp->w_ميلاپ_سر;
    طالما (cur != عدم)
    {
	cur->mit_hl.rm = cur->mit_ميلاپ;
	إذا (cur->mit_hlg_id == 0)
	    cur->mit_hl.attr = 0;
	وإلا
	    cur->mit_hl.attr = syn_id2attr(cur->mit_hlg_id);
	cur->mit_hl.buf = wp->w_بفر;
	cur->mit_hl.lnum = 0;
	cur->mit_hl.پهريون_lnum = 0;
	cur = cur->mit_ايندڙ;
    }
    // Must update this every time since highlight group override can change it.
    ڳولا_hl->attr = HL_ATTR(HLF_L);

    ڳولا_hl->buf = wp->w_بفر;
    ڳولا_hl->lnum = 0;
    ڳولا_hl->پهريون_lnum = 0;
    // time limit is set at the toplevel, for all windows
}

/*
 * If there is a match fill "shl" and return one.
 * Return zero otherwise.
 */
    ساكن صحيح
ايندڙ_ڳولا_hl_pos(
    ميلاپ_T	    *shl,	// points to a match
    linenr_T	    lnum,
    matchitem_T	    *ميلاپ,	// match item with positions
    colnr_T	    mincol)	// minimal column for a match
{
    صحيح	    i;
    صحيح	    مليو = -1;

    لكل (i = ميلاپ->mit_pos_cur; i < ميلاپ->mit_pos_ڳڻپ; i++)
    {
	llpos_T	*pos = &ميلاپ->mit_pos_array[i];

	إذا (pos->lnum == 0)
	    اكسر;
	إذا (pos->len == 0 && pos->کانو < mincol)
	    استمر;
	إذا (pos->lnum == lnum)
	{
	    إذا (مليو >= 0)
	    {
		// if this match comes before the one at "found" then swap them
		إذا (pos->کانو < ميلاپ->mit_pos_array[مليو].کانو)
		{
		    llpos_T	tmp = *pos;

		    *pos = ميلاپ->mit_pos_array[مليو];
		    ميلاپ->mit_pos_array[مليو] = tmp;
		}
	    }
	    وإلا
		مليو = i;
	}
    }
    ميلاپ->mit_pos_cur = 0;
    إذا (مليو >= 0)
    {
	colnr_T	شروعات = ميلاپ->mit_pos_array[مليو].کانو == 0
				     ? 0 : ميلاپ->mit_pos_array[مليو].کانو - 1;
	colnr_T	پڇاڙي = ميلاپ->mit_pos_array[مليو].کانو == 0
			    ? MAXCOL : شروعات + ميلاپ->mit_pos_array[مليو].len;

	shl->lnum = lnum;
	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].کانو = شروعات;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].کانو = پڇاڙي;
	shl->is_addpos = TRUE;
	shl->has_cursor = FALSE;
	ميلاپ->mit_pos_cur = مليو + 1;
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
ايندڙ_ڳولا_hl(
    win_T	    *win,
    ميلاپ_T	    *ڳولا_hl,
    ميلاپ_T	    *shl,	// points to search_hl or a match
    linenr_T	    lnum,
    colnr_T	    mincol,	// minimal column for a match
    matchitem_T	    *cur)	// to retrieve match positions if any
{
    linenr_T	l;
    colnr_T	matchcol;
    ڊگهو	nmatched;
    صحيح		called_emsg_before = called_emsg;
    صحيح		timed_ٻاهريون = FALSE;

    // for :{range}s/pat only highlight inside the range
    إذا ((lnum < ڳولا_پهريون_سٽ || lnum > ڳولا_آخري_سٽ) && cur == عدم)
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
	وإلا إذا (lnum < l || shl->rm.endpos[0].کانو > mincol)
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
	وإلا إذا (vim_strchr(p_cpo, CPO_ڳولا) == عدم
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].کانو <= shl->rm.startpos[0].کانو))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].کانو;
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
	    matchcol = shl->rm.endpos[0].کانو;

	shl->lnum = lnum;
	إذا (shl->rm.regprog != عدم)
	{
	    // Remember whether shl->rm is using a copy of the regprog in
	    // cur->mit_match.
	    صحيح regprog_is_copy = (shl != ڳولا_hl && cur != عدم
			  && shl == &cur->mit_hl
			  && cur->mit_ميلاپ.regprog == cur->mit_hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
							 matchcol, &timed_ٻاهريون);
	    // Copy the regprog, in case it got freed and recompiled.
	    إذا (regprog_is_copy)
		cur->mit_ميلاپ.regprog = cur->mit_hl.rm.regprog;

	    إذا (called_emsg > called_emsg_before || got_int || timed_ٻاهريون)
	    {
		// Error while handling regexp: stop using this regexp.
		إذا (shl == ڳولا_hl)
		{
		    // don't free regprog in the match list, it's a copy
		    vim_regfree(shl->rm.regprog);
		    سيٽ_no_hlsearch(TRUE);
		}
		shl->rm.regprog = عدم;
		shl->lnum = 0;
		got_int = FALSE;  // avoid the "Type :quit to exit Vim" message
		اكسر;
	    }
	}
	وإلا إذا (cur != عدم)
	    nmatched = ايندڙ_ڳولا_hl_pos(shl, lnum, cur, matchcol);
	وإلا
	    nmatched = 0;
	إذا (nmatched == 0)
	{
	    shl->lnum = 0;		// no match found
	    اكسر;
	}
	إذا (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].کانو >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].کانو > mincol)
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
prepare_ڳولا_hl(win_T *wp, ميلاپ_T *ڳولا_hl, linenr_T lnum)
{
    matchitem_T *cur;		// points to the match list
    ميلاپ_T	*shl;		// points to search_hl or a match
    صحيح		shl_flag;	// flag to indicate whether search_hl
				// has been processed or not
    صحيح		pos_inprogress;	// marks that position match search is
				// in progress
    صحيح		n;

    // When using a multi-line pattern, start searching at the top
    // of the window or just after a closed fold.
    // Do this both for search_hl and the match list.
    cur = wp->w_ميلاپ_سر;
    shl_flag = WIN_IS_POPUP(wp);  // skip search_hl in a popup window
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE)
	{
	    shl = ڳولا_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (shl->rm.regprog != عدم
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    إذا (shl->پهريون_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		لكل (shl->پهريون_lnum = lnum;
			   shl->پهريون_lnum > wp->w_topline; --shl->پهريون_lnum)
		    إذا (hasFoldingWin(wp, shl->پهريون_lnum - 1,
						      عدم, عدم, TRUE, عدم))
			اكسر;
# وإلا
		shl->پهريون_lnum = wp->w_topline;
# endif
	    }
	    إذا (cur != عدم)
		cur->mit_pos_cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    طالما (shl->پهريون_lnum < lnum && (shl->rm.regprog != عدم
					  || (cur != عدم && pos_inprogress)))
	    {
		ايندڙ_ڳولا_hl(wp, ڳولا_hl, shl, shl->پهريون_lnum, (colnr_T)n,
					       shl == ڳولا_hl ? عدم : cur);
		pos_inprogress = cur == عدم || cur->mit_pos_cur == 0
							      ? FALSE : TRUE;
		إذا (shl->lnum != 0)
		{
		    shl->پهريون_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].کانو;
		}
		وإلا
		{
		    ++shl->پهريون_lnum;
		    n = 0;
		}
	    }
	}
	إذا (shl != ڳولا_hl && cur != عدم)
	    cur = cur->mit_ايندڙ;
    }
}

/*
 * Update "shl->has_cursor" based on the match in "shl" and the cursor
 * position.
 */
    ساكن فراغ
check_cur_ڳولا_hl(win_T *wp, ميلاپ_T *shl)
{
    linenr_T linecount = shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;

    إذا (wp->w_cursor.lnum >= shl->lnum
	    && wp->w_cursor.lnum <= shl->lnum + linecount
	    && (wp->w_cursor.lnum > shl->lnum
				|| wp->w_cursor.کانو >= shl->rm.startpos[0].کانو)
	    && (wp->w_cursor.lnum < shl->lnum + linecount
				  || wp->w_cursor.کانو < shl->rm.endpos[0].کانو))
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
prepare_ڳولا_hl_سٽ(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    mincol,
	char_u	    **سٽ,
	ميلاپ_T	    *ڳولا_hl,
	صحيح	    *ڳولا_attr)
{
    matchitem_T *cur;			// points to the match list
    ميلاپ_T	*shl;			// points to search_hl or a match
    صحيح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not
    صحيح		area_highlighting = FALSE;

    // Handle highlighting the last used search pattern and matches.
    // Do this for both search_hl and the match list.
    // Do not use search_hl in a popup window.
    cur = wp->w_ميلاپ_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE)
	{
	    shl = ڳولا_hl;
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
	ايندڙ_ڳولا_hl(wp, ڳولا_hl, shl, lnum, mincol,
						shl == ڳولا_hl ? عدم : cur);

	// Need to get the line again, a multi-line regexp may have made it
	// invalid.
	*سٽ = ml_get_buf(wp->w_بفر, lnum, FALSE);

	إذا (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    إذا (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].کانو;
	    وإلا
		shl->startcol = 0;
	    إذا (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].کانو;
	    وإلا
		shl->endcol = MAXCOL;

	    // check if the cursor is in the match before changing the columns
	    إذا (shl == ڳولا_hl)
		check_cur_ڳولا_hl(wp, shl);

	    // Highlight one character for an empty match.
	    إذا (shl->startcol == shl->endcol)
	    {
		إذا (has_mbyte && (*سٽ)[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)((*سٽ) + shl->endcol);
		وإلا
		    ++shl->endcol;
	    }
	    إذا ((ڊگهو)shl->startcol < mincol)  // match at leftcol
	    {
		shl->attr_cur = shl->attr;
		*ڳولا_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	إذا (shl != ڳولا_hl && cur != عدم)
	    cur = cur->mit_ايندڙ;
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
update_ڳولا_hl(
	win_T	    *wp,
	linenr_T    lnum,
	colnr_T	    کانو,
	char_u	    **سٽ,
	ميلاپ_T	    *ڳولا_hl,
	صحيح	    *has_ميلاپ_conc اڻ_استعمال_ٿيل,
	صحيح	    *ميلاپ_conc اڻ_استعمال_ٿيل,
	صحيح	    did_سٽ_attr,
	صحيح	    lcs_eol_one,
	صحيح	    *on_آخري_کانو)
{
    matchitem_T *cur;		    // points to the match list
    ميلاپ_T	*shl;		    // points to search_hl or a match
    صحيح		shl_flag;	    // flag to indicate whether search_hl
				    // has been processed or not
    صحيح		pos_inprogress;	    // marks that position match search is in
				    //ترقي
    صحيح		ڳولا_attr = 0;


    // Do this for 'search_hl' and the match list (ordered by priority).
    cur = wp->w_ميلاپ_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& (cur == عدم
			|| cur->mit_ترجيح > ڳولا_HL_ترجيح))
	{
	    shl = ڳولا_hl;
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
		    && کانو >= shl->startcol
		    && کانو < shl->endcol)
	    {
		صحيح ايندڙ_کانو = کانو + mb_ptr2len(*سٽ + کانو);

		إذا (shl->endcol < ايندڙ_کانو)
		    shl->endcol = ايندڙ_کانو;
		shl->attr_cur = shl->attr;
# ifdef FEAT_CONCEAL
		// Match with the "Conceal" group results in hiding
		// the match.
		إذا (cur != عدم
			&& shl != ڳولا_hl
			&& syn_name2id((char_u *)"Conceal") == cur->mit_hlg_id)
		{
		    *has_ميلاپ_conc = کانو == shl->startcol ? 2 : 1;
		    *ميلاپ_conc = cur->mit_conceal_char;
		}
		وإلا
		    *has_ميلاپ_conc = 0;
# endif
		// Highlight the match were the cursor is using the CurSearch
		// group.
		إذا (shl == ڳولا_hl && shl->has_cursor)
		{
		    shl->attr_cur = HL_ATTR(HLF_LC);
		    إذا (shl->attr_cur != shl->attr)
			ڳولا_hl_has_cursor_lnum = lnum;
		}

	    }
	    وإلا إذا (کانو == shl->endcol)
	    {
		shl->attr_cur = 0;
		ايندڙ_ڳولا_hl(wp, ڳولا_hl, shl, lnum, کانو,
					       shl == ڳولا_hl ? عدم : cur);
		pos_inprogress = !(cur == عدم || cur->mit_pos_cur == 0);

		// Need to get the line again, a multi-line regexp may have
		// made it invalid.
		*سٽ = ml_get_buf(wp->w_بفر, lnum, FALSE);

		إذا (shl->lnum == lnum)
		{
		    shl->startcol = shl->rm.startpos[0].کانو;
		    إذا (shl->rm.endpos[0].lnum == 0)
			shl->endcol = shl->rm.endpos[0].کانو;
		    وإلا
			shl->endcol = MAXCOL;

		    // check if the cursor is in the match
		    إذا (shl == ڳولا_hl)
			check_cur_ڳولا_hl(wp, shl);

		    إذا (shl->startcol == shl->endcol)
		    {
			// highlight empty match, try again after
			// it
			إذا (has_mbyte)
			{
			    char_u *p = *سٽ + shl->endcol;

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
	إذا (shl != ڳولا_hl && cur != عدم)
	    cur = cur->mit_ايندڙ;
    }

    // Use attributes from match with highest priority among 'search_hl' and
    // the match list.
    cur = wp->w_ميلاپ_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& (cur == عدم ||
			cur->mit_ترجيح > ڳولا_HL_ترجيح))
	{
	    shl = ڳولا_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (shl->attr_cur != 0)
	{
	    ڳولا_attr = shl->attr_cur;
	    *on_آخري_کانو = کانو + 1 >= shl->endcol;
	}
	إذا (shl != ڳولا_hl && cur != عدم)
	    cur = cur->mit_ايندڙ;
    }
    // Only highlight one character after the last column.
    إذا (*(*سٽ + کانو) == NUL && (did_سٽ_attr >= 1
				       || (wp->w_p_list && lcs_eol_one == -1)))
	ڳولا_attr = 0;
    أرجع ڳولا_attr;
}

    صحيح
get_prevcol_hl_flag(win_T *wp, ميلاپ_T *ڳولا_hl, ڊگهو curcol)
{
    ڊگهو	prevcol = curcol;
    صحيح		prevcol_hl_flag = FALSE;
    matchitem_T *cur;			// points to the match list

# إذا defined(FEAT_PROP_POPUP)
    // don't do this in a popup window
    إذا (popup_is_popup(wp))
	أرجع FALSE;
# endif

    // we're not really at that column when skipping some text
    إذا ((ڊگهو)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
	++prevcol;

    // Highlight a character after the end of the line if the match started
    // at the end of the line or when the match continues in the next line
    // (match includes the line break).
    إذا (!ڳولا_hl->is_addpos && (prevcol == (ڊگهو)ڳولا_hl->startcol
		|| (prevcol > (ڊگهو)ڳولا_hl->startcol
					      && ڳولا_hl->endcol == MAXCOL)))
	prevcol_hl_flag = TRUE;
    وإلا
    {
	cur = wp->w_ميلاپ_سر;
	طالما (cur != عدم)
	{
	    إذا (!cur->mit_hl.is_addpos && (prevcol == (ڊگهو)cur->mit_hl.startcol
			|| (prevcol > (ڊگهو)cur->mit_hl.startcol
					     && cur->mit_hl.endcol == MAXCOL)))
	    {
		prevcol_hl_flag = TRUE;
		اكسر;
	    }
	    cur = cur->mit_ايندڙ;
	}
    }
    أرجع prevcol_hl_flag;
}

/*
 * Get highlighting for the char after the text in "char_attr" from 'hlsearch'
 * or match highlighting.
 */
    فراغ
get_ڳولا_ميلاپ_hl(win_T *wp, ميلاپ_T *ڳولا_hl, ڊگهو کانو, صحيح *char_attr)
{
    matchitem_T *cur;			// points to the match list
    ميلاپ_T	*shl;			// points to search_hl or a match
    صحيح		shl_flag;		// flag to indicate whether search_hl
					// has been processed or not

    cur = wp->w_ميلاپ_سر;
    shl_flag = WIN_IS_POPUP(wp);
    طالما (cur != عدم || shl_flag == FALSE)
    {
	إذا (shl_flag == FALSE
		&& ((cur != عدم
			&& cur->mit_ترجيح > ڳولا_HL_ترجيح)
		    || cur == عدم))
	{
	    shl = ڳولا_hl;
	    shl_flag = TRUE;
	}
	وإلا
	    shl = &cur->mit_hl;
	إذا (کانو - 1 == (ڊگهو)shl->startcol
		&& (shl == ڳولا_hl || !shl->is_addpos))
	    *char_attr = shl->attr;
	إذا (shl != ڳولا_hl && cur != عدم)
	    cur = cur->mit_ايندڙ;
    }
}

#جيڪڏهن_پورو // FEAT_SEARCH_EXTRA

#إذا defined(FEAT_EVAL)
# ifdef FEAT_ڳولا_EXTRA
    ساكن صحيح
matchadd_dict_arg(typval_T *tv, char_u **conceal_char, win_T **win)
{
    dictitem_T *di;

    إذا (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	أرجع FAIL;
    }

    إذا (dict_has_ڪنجي(tv->vval.v_dict, "conceal"))
	*conceal_char = dict_get_string(tv->vval.v_dict, "conceal", FALSE);

    إذا ((di = dict_ڳوليو(tv->vval.v_dict, (char_u *)"window", -1)) == عدم)
	أرجع OK;

    *win = ڳوليو_win_by_nr_or_id(&di->di_tv);
    إذا (*win == عدم)
    {
	emsg(_(e_invalid_دري_عدد));
	أرجع FAIL;
    }

    أرجع OK;
}
# endif

/*
 * "clearmatches()" function
 */
    فراغ
f_clearmatches(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    win_T   *win;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    win = get_optional_دري(argvars, 0);
    إذا (win != عدم)
	clear_matches(win);
# endif
}

/*
 * "getmatches()" function
 */
    فراغ
f_getmatches(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    dict_T	*dict;
    matchitem_T	*cur;
    صحيح		i;
    win_T	*win;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    win = get_optional_دري(argvars, 0);
    إذا (rettv_list_alloc(rettv) == FAIL || win == عدم)
	أرجع;

    cur = win->w_ميلاپ_سر;
    طالما (cur != عدم)
    {
	dict = dict_alloc();
	إذا (dict == عدم)
	    أرجع;
	إذا (cur->mit_ميلاپ.regprog == عدم)
	{
	    // match added with matchaddpos()
	    لكل (i = 0; i < cur->mit_pos_ڳڻپ; ++i)
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
		إذا (llpos->کانو > 0)
		{
		    list_append_عدد(l, (varnumber_T)llpos->کانو);
		    list_append_عدد(l, (varnumber_T)llpos->len);
		}
		sprintf(buf, "pos%d", i + 1);
		إذا (dict_جوڙيو_list(dict, buf, l) == FAIL)
		    list_unref(l);
	    }
	}
	وإلا
	{
	    dict_جوڙيو_string(dict, "pattern", cur->mit_نمونو);
	}
	dict_جوڙيو_string(dict, "group", syn_id2name(cur->mit_hlg_id));
	dict_جوڙيو_عدد(dict, "priority", (ڊگهو)cur->mit_ترجيح);
	dict_جوڙيو_عدد(dict, "id", (ڊگهو)cur->mit_id);
#  إذا defined(FEAT_CONCEAL)
	إذا (cur->mit_conceal_char)
	{
	    char_u  buf[MB_MAXBYTES + 1];
	    صحيح	    buflen;

	    buflen = (*mb_char2bytes)(cur->mit_conceal_char, buf);
	    buf[buflen] = NUL;
	    dict_جوڙيو_string_len(dict, "conceal", (char_u *)&buf, buflen);
	}
#  endif
	إذا (list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	cur = cur->mit_ايندڙ;
    }
# endif
}

/*
 * "setmatches()" function
 */
    فراغ
f_setmatches(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    list_T	*s = عدم;
    win_T	*win;

    rettv->vval.v_عدد = -1;

    إذا (اندريون_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;
    win = get_optional_دري(argvars, 1);
    إذا (win == عدم)
	أرجع;

    إذا ((l = argvars[0].vval.v_list) != عدم)
    {
	// To some extent make sure that we are dealing with a list from
	// "getmatches()".
	li = l->lv_پهريون;
	طالما (li != عدم)
	{
	    إذا (li->li_tv.v_type != VAR_DICT
		    || (d = li->li_tv.vval.v_dict) == عدم)
	    {
		emsg(_(e_invalid_دليل));
		أرجع;
	    }
	    إذا (!(dict_has_ڪنجي(d, "group")
			&& (dict_has_ڪنجي(d, "pattern")
			    || dict_has_ڪنجي(d, "pos1"))
			&& dict_has_ڪنجي(d, "priority")
			&& dict_has_ڪنجي(d, "id")))
	    {
		emsg(_(e_invalid_دليل));
		أرجع;
	    }
	    li = li->li_ايندڙ;
	}

	clear_matches(win);
	li = l->lv_پهريون;
	طالما (li != عدم)
	{
	    صحيح		i = 0;
	    حرف	buf[30];  // use 30 to avoid compiler warning
	    dictitem_T  *di;
	    char_u	*گروهه;
	    صحيح		ترجيح;
	    صحيح		id;
	    char_u	*conceal;

	    d = li->li_tv.vval.v_dict;
	    إذا (!dict_has_ڪنجي(d, "pattern"))
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
		    إذا ((di = dict_ڳوليو(d, (char_u *)buf, -1)) != عدم)
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

	    گروهه = dict_get_string(d, "group", TRUE);
	    ترجيح = (صحيح)dict_get_عدد(d, "priority");
	    id = (صحيح)dict_get_عدد(d, "id");
	    conceal = dict_has_ڪنجي(d, "conceal")
			      ? dict_get_string(d, "conceal", TRUE)
			      : عدم;
	    إذا (i == 0)
	    {
		ميلاپ_جوڙيو(win, گروهه,
		    dict_get_string(d, "pattern", FALSE),
		    ترجيح, id, عدم, conceal);
	    }
	    وإلا
	    {
		ميلاپ_جوڙيو(win, گروهه, عدم, ترجيح, id, s, conceal);
		list_unref(s);
		s = عدم;
	    }
	    vim_free(گروهه);
	    vim_free(conceal);

	    li = li->li_ايندڙ;
	}
	rettv->vval.v_عدد = 0;
    }
# endif
}

/*
 * "matchadd()" function
 */
    فراغ
f_matchadd(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*grp;		//گروهه
    char_u	*pat;		//نمونو
    صحيح		prio = 10;	// default priority
    صحيح		id = -1;
    صحيح		error = FALSE;
    char_u	*conceal_char = عدم;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	أرجع;

    grp = tv_get_string_buf_chk(&argvars[0], buf);	//گروهه
    pat = tv_get_string_buf_chk(&argvars[1], buf);	//نمونو
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
	semsg(_(e_id_is_reserved_for_ميلاپ_nr), id);
	أرجع;
    }

    rettv->vval.v_عدد = ميلاپ_جوڙيو(win, grp, pat, prio, id, عدم,
								conceal_char);
# endif
}

/*
 * "matchaddpos()" function
 */
    فراغ
f_matchaddpos(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    char_u	buf[NUMBUFLEN];
    char_u	*گروهه;
    صحيح		prio = 10;
    صحيح		id = -1;
    صحيح		error = FALSE;
    list_T	*l;
    char_u	*conceal_char = عدم;
    win_T	*win = curwin;

    rettv->vval.v_عدد = -1;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_dict_arg(argvars, 4) == FAIL)))))
	أرجع;

    گروهه = tv_get_string_buf_chk(&argvars[0], buf);
    إذا (گروهه == عدم)
	أرجع;

    إذا (argvars[1].v_type != VAR_LIST)
    {
	semsg(_(e_دليل_جو_str_must_be_list), "matchaddpos()");
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
	semsg(_(e_id_is_reserved_for_ميلاپ_nr), id);
	أرجع;
    }

    rettv->vval.v_عدد = ميلاپ_جوڙيو(win, گروهه, عدم, prio, id, l,
								conceal_char);
# endif
}

/*
 * "matcharg()" function
 */
    فراغ
f_matcharg(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) != OK)
	أرجع;

# ifdef FEAT_ڳولا_EXTRA
    صحيح	    id;
    matchitem_T *m;

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(&argvars[0]);
    إذا (id >= 1 && id <= 3)
    {
	إذا ((m = get_ميلاپ(curwin, id)) != عدم)
	{
	    list_append_string(rettv->vval.v_list,
		    syn_id2name(m->mit_hlg_id), -1);
	    list_append_string(rettv->vval.v_list, m->mit_نمونو, -1);
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
f_matchdelete(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_ڳولا_EXTRA
    win_T   *win;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    win = get_optional_دري(argvars, 1);
    إذا (win == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = ميلاپ_delete(win,
				       (صحيح)tv_get_عدد(&argvars[0]), TRUE);
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_ڳولا_EXTRA)
/*
 * ":[N]match {group} {pattern}"
 * Sets nextcmd to the start of the next command, if any.  Also called when
 * skipping commands to find the next command.
 */
    فراغ
ex_ميلاپ(exarg_T *eap)
{
    char_u	*p;
    char_u	*g = عدم;
    char_u	*پڇاڙي;
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
	ميلاپ_delete(curwin, id, FALSE);

    إذا (پڇاڙيون_excmd2(eap->cmd, eap->arg))
	پڇاڙي = eap->arg;
    وإلا إذا ((STRNICMP(eap->arg, "none", 4) == 0
		&& (VIM_ISWHITE(eap->arg[4])
				      || پڇاڙيون_excmd2(eap->arg, eap->arg + 4))))
	پڇاڙي = eap->arg + 4;
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
	    semsg(_(e_invalid_دليل_str), eap->arg);
	    أرجع;
	}
	پڇاڙي = skip_regexp(p + 1, *p, TRUE);
	إذا (!eap->skip)
	{
	    إذا (*پڇاڙي != NUL && !پڇاڙيون_excmd2(پڇاڙي, skipwhite(پڇاڙي + 1)))
	    {
		vim_free(g);
		eap->errmsg = ex_errmsg(e_trailing_characters_str, پڇاڙي);
		أرجع;
	    }
	    إذا (*پڇاڙي != *p)
	    {
		vim_free(g);
		semsg(_(e_invalid_دليل_str), p);
		أرجع;
	    }

	    c = *پڇاڙي;
	    *پڇاڙي = NUL;
	    ميلاپ_جوڙيو(curwin, g, p + 1, 10, id, عدم, عدم);
	    vim_free(g);
	    *پڇاڙي = c;
	}
    }
    eap->nextcmd = ڳوليو_nextcmd(پڇاڙي);
}
#جيڪڏهن_پورو
