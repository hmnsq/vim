/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * search.c: code for normal mode searching commands
 */

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_EVAL
స్థిర శూన్య సమితి_vv_searchforward(శూన్య);
స్థిర పూర్ణాంకం మొదటి_submatch(regmmatch_T *rp);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_కనుగొను_ID
స్థిర char_u *get_పంక్తి_and_copy(linenr_T lnum, char_u *buf);
స్థిర శూన్య show_pat_లోపల_మార్గం(char_u *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, FILE *, linenr_T *, దీర్ఘ);
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం searchstat
{
    పూర్ణాంకం	    cur;	    // current position of found words
    పూర్ణాంకం	    cnt;	    // total count of found words
    పూర్ణాంకం	    exact_సరిపోలిక;    // TRUE if matched exactly on specified position
    పూర్ణాంకం	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    పూర్ణాంకం	    చివరి_maxcount;  // the max count of the last search
} searchstat_T;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
స్థిర శూన్య save_incsearch_state(శూన్య);
స్థిర శూన్య restore_incsearch_state(శూన్య);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం check_prevcol(char_u *linep, పూర్ణాంకం నిలువు, పూర్ణాంకం ch, పూర్ణాంకం *prevcol);
స్థిర పూర్ణాంకం కనుగొను_rawstring_చివర(char_u *linep, pos_T *startpos, pos_T *endpos);
స్థిర శూన్య కనుగొను_mps_విలువలు(పూర్ణాంకం *initc, పూర్ణాంకం *findc, పూర్ణాంకం *backwards, పూర్ణాంకం switchit);
స్థిర పూర్ణాంకం is_zero_వెడల్పు(char_u *నమూనా, size_t patternlen, పూర్ణాంకం move, pos_T *cur, పూర్ణాంకం direction);
స్థిర శూన్య cmdline_శోధన_stat(పూర్ణాంకం dirc, pos_T *pos, pos_T *cursor_pos, పూర్ణాంకం show_శిఖరం_bot_సందేశం, char_u *msgbuf, size_t msgbuflen, పూర్ణాంకం recompute, పూర్ణాంకం maxcount, దీర్ఘ timeout);
స్థిర శూన్య update_శోధన_stat(పూర్ణాంకం dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, పూర్ణాంకం recompute, పూర్ణాంకం maxcount, దీర్ఘ timeout);

#నిర్వచించు శోధన_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#నిర్వచించు శోధన_STAT_BUF_LEN 16

/*
 * This file contains various searching-related routines. These fall into
 * three groups:
 * 1. string searches (for /, ?, n, and N)
 * 2. character searches within a single line (for f, F, t, T, etc)
 * 3. "other" kinds of searches like the '%' command, and 'word' searches.
 */

/*
 * String searches
 *
 * The string search functions are divided into two levels:
 * lowest:  searchit(); uses an pos_T for starting position and found match.
 * Highest: do_search(); uses curwin->w_cursor; calls searchit().
 *
 * The last search pattern is remembered for repeating the same search.
 * This pattern is shared between the :g, :s, ? and / commands.
 * This is in search_regcomp().
 *
 * The actual string matching is done using a heavily modified version of
 * Henry Spencer's regular expression library.  See regexp.c.
 */

/*
 * Two search patterns are remembered: One for the :substitute command and
 * one for other searches.  last_idx points to the one that was used the last
 * time.
 */
స్థిర spat_T spats[2] =
{
    {ఖాళీ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ఖాళీ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

స్థిర పూర్ణాంకం చివరి_idx = 0;	// index in spats[] for RE_LAST

స్థిర char_u lastc[2] = {NUL, NUL};	// last character searched for
స్థిర పూర్ణాంకం lastcdir = FORWARD;		// last direction of character search
స్థిర పూర్ణాంకం చివరి_t_cmd = TRUE;		// last search t_cmd
స్థిర char_u	lastc_బైట్లు[MB_MAXBYTES + 1];
స్థిర పూర్ణాంకం	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
స్థిర spat_T	    saved_spats[ARRAY_నిడివి(spats)];
స్థిర char_u	    *saved_mr_నమూనా = ఖాళీ;
స్థిర size_t	    saved_mr_patternlen = 0;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
స్థిర పూర్ణాంకం	    saved_spats_చివరి_idx = 0;
స్థిర పూర్ణాంకం	    saved_spats_no_hlsearch = 0;
#అయితే_ముగింపు

// allocated copy of pattern used by search_regcomp()
స్థిర char_u	    *mr_నమూనా = ఖాళీ;
స్థిర size_t	    mr_patternlen = 0;

#నిర్వచితమైతే FEAT_కనుగొను_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
రకనిర్వచనం నిర్మాణం SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    పూర్ణాంకం		matched;	// Found a match in this file
} SearchedFile;
#అయితే_ముగింపు

/*
 * translate search pattern for vim_regcomp()
 *
 * pat_save == RE_SEARCH: save pat in spats[RE_SEARCH].pat (normal search cmd)
 * pat_save == RE_SUBST: save pat in spats[RE_SUBST].pat (:substitute command)
 * pat_save == RE_BOTH: save pat in both patterns (:global command)
 * pat_use  == RE_SEARCH: use previous search pattern if "pat" is NULL
 * pat_use  == RE_SUBST: use previous substitute pattern if "pat" is NULL
 * pat_use  == RE_LAST: use last used pattern if "pat" is NULL
 * options & SEARCH_HIS: put search string in history
 * options & SEARCH_KEEP: keep previous search pattern
 *
 * returns FAIL if failed, OK otherwise.
 */
    పూర్ణాంకం
శోధన_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**వాడిన_pat,
    పూర్ణాంకం		pat_save,
    పూర్ణాంకం		pat_use,
    పూర్ణాంకం		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    పూర్ణాంకం		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ఒకవేళ (pat == ఖాళీ || *pat == NUL)
    {
	పూర్ణాంకం i;

	ఒకవేళ (pat_use == RE_చివరి)
	    i = చివరి_idx;
	లేకపోతే
	    i = pat_use;
	ఒకవేళ (spats[i].pat == ఖాళీ)	// pattern was never defined
	{
	    ఒకవేళ (pat_use == RE_SUBST)
		emsg(_(e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ));
	    లేకపోతే
		emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
	    rc_did_emsg = TRUE;
	    తిరిగి FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    లేకపోతే ఒకవేళ (options & శోధన_HIS)	// put new pattern in history
	కూడు_గమ్యం_history(HIST_శోధన, pat, patlen, TRUE, NUL);

    ఒకవేళ (వాడిన_pat)
	*వాడిన_pat = pat;

    vim_free(mr_నమూనా);
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_నమూనా = తిరగేయి_పాఠ్యం(pat);
    లేకపోతే
#అయితే_ముగింపు
	mr_నమూనా = vim_strnsave(pat, patlen);
    ఒకవేళ (mr_నమూనా == ఖాళీ)
	mr_patternlen = 0;
    లేకపోతే
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ఒకవేళ (!(options & శోధన_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ఒకవేళ (pat_save == RE_శోధన || pat_save == RE_BOTH)
	    save_re_pat(RE_శోధన, pat, patlen, magic);
	// substitute or global command
	ఒకవేళ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ఒకవేళ (regmatch->regprog == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_శోధన_pat(శూన్య)
{
    తిరిగి mr_నమూనా;
}

    శూన్య
save_re_pat(పూర్ణాంకం idx, char_u *pat, size_t patlen, పూర్ణాంకం magic)
{
    ఒకవేళ (spats[idx].pat == pat)
	తిరిగి;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ఒకవేళ (spats[idx].pat == ఖాళీ)
	spats[idx].patlen = 0;
    లేకపోతే
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    చివరి_idx = idx;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ఒకవేళ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    సమితి_no_hlsearch(FALSE);
#అయితే_ముగింపు
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
స్థిర పూర్ణాంకం save_level = 0;

    శూన్య
save_శోధన_patterns(శూన్య)
{
    పూర్ణాంకం i;

    ఒకవేళ (save_level++ != 0)
	తిరిగి;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ఒకవేళ (spats[i].pat != ఖాళీ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ఒకవేళ (saved_spats[i].pat == ఖాళీ)
		saved_spats[i].patlen = 0;
	    లేకపోతే
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ఒకవేళ (mr_నమూనా == ఖాళీ)
	saved_mr_నమూనా = ఖాళీ;
    లేకపోతే
	saved_mr_నమూనా = vim_strnsave(mr_నమూనా, mr_patternlen);
    ఒకవేళ (saved_mr_నమూనా == ఖాళీ)
	saved_mr_patternlen = 0;
    లేకపోతే
	saved_mr_patternlen = mr_patternlen;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    saved_spats_చివరి_idx = చివరి_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#అయితే_ముగింపు
}

    శూన్య
restore_శోధన_patterns(శూన్య)
{
    పూర్ణాంకం i;

    ఒకవేళ (--save_level != 0)
	తిరిగి;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ఒకవేళ defined(FEAT_EVAL)
    సమితి_vv_searchforward();
#అయితే_ముగింపు
    vim_free(mr_నమూనా);
    mr_నమూనా = saved_mr_నమూనా;
    mr_patternlen = saved_mr_patternlen;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    చివరి_idx = saved_spats_చివరి_idx;
    సమితి_no_hlsearch(saved_spats_no_hlsearch);
#అయితే_ముగింపు
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_శోధన_patterns(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_నమూనా);
    mr_patternlen = 0;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_శోధన_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
స్థిర spat_T	    saved_చివరి_శోధన_spat;
స్థిర పూర్ణాంకం	    did_save_చివరి_శోధన_spat = 0;
స్థిర పూర్ణాంకం	    saved_చివరి_idx = 0;
స్థిర పూర్ణాంకం	    saved_no_hlsearch = 0;
స్థిర పూర్ణాంకం	    saved_శోధన_సరిపోలిక_endcol;
స్థిర పూర్ణాంకం	    saved_శోధన_సరిపోలిక_పంక్తులు;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    శూన్య
save_చివరి_శోధన_నమూనా(శూన్య)
{
    ఒకవేళ (++did_save_చివరి_శోధన_spat != 1)
	// nested call, nothing to do
	తిరిగి;

    saved_చివరి_శోధన_spat = spats[RE_శోధన];
    ఒకవేళ (spats[RE_శోధన].pat != ఖాళీ)
    {
	saved_చివరి_శోధన_spat.pat = vim_strnsave(spats[RE_శోధన].pat, spats[RE_శోధన].patlen);
	ఒకవేళ (saved_చివరి_శోధన_spat.pat == ఖాళీ)
	    saved_చివరి_శోధన_spat.patlen = 0;
	లేకపోతే
	    saved_చివరి_శోధన_spat.patlen = spats[RE_శోధన].patlen;
    }
    saved_చివరి_idx = చివరి_idx;
    saved_no_hlsearch = no_hlsearch;
}

    శూన్య
restore_చివరి_శోధన_నమూనా(శూన్య)
{
    ఒకవేళ (--did_save_చివరి_శోధన_spat > 0)
	// nested call, nothing to do
	తిరిగి;
    ఒకవేళ (did_save_చివరి_శోధన_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	తిరిగి;
    }

    vim_free(spats[RE_శోధన].pat);
    spats[RE_శోధన] = saved_చివరి_శోధన_spat;
    saved_చివరి_శోధన_spat.pat = ఖాళీ;
    saved_చివరి_శోధన_spat.patlen = 0;
# ఒకవేళ defined(FEAT_EVAL)
    సమితి_vv_searchforward();
# endif
    చివరి_idx = saved_చివరి_idx;
    సమితి_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    స్థిర శూన్య
save_incsearch_state(శూన్య)
{
    saved_శోధన_సరిపోలిక_endcol = శోధన_సరిపోలిక_endcol;
    saved_శోధన_సరిపోలిక_పంక్తులు  = శోధన_సరిపోలిక_పంక్తులు;
}

    స్థిర శూన్య
restore_incsearch_state(శూన్య)
{
    శోధన_సరిపోలిక_endcol = saved_శోధన_సరిపోలిక_endcol;
    శోధన_సరిపోలిక_పంక్తులు  = saved_శోధన_సరిపోలిక_పంక్తులు;
}

    char_u *
చివరి_శోధన_నమూనా(శూన్య)
{
    తిరిగి spats[RE_శోధన].pat;
}

    size_t
చివరి_శోధన_నమూనా_len(శూన్య)
{
    తిరిగి spats[RE_శోధన].patlen;
}
#అయితే_ముగింపు

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    పూర్ణాంకం
ignorecase(char_u *pat)
{
    తిరిగి ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    పూర్ణాంకం
ignorecase_opt(char_u *pat, పూర్ణాంకం ic_లోపల, పూర్ణాంకం scs)
{
    పూర్ణాంకం		ic = ic_లోపల;

    ఒకవేళ (ic && !no_smartcase && scs
			    && !(ctrl_x_బహుళకం_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    తిరిగి ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    పూర్ణాంకం
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (శూన్య)skip_regexp_ex(pat, NUL, magic_isset(), ఖాళీ, ఖాళీ, &magic_val);

    అయితే (*p != NUL)
    {
	పూర్ణాంకం		l;

	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ఒకవేళ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		తిరిగి TRUE;
	    p += l;
	}
	లేకపోతే ఒకవేళ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ఒకవేళ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    లేకపోతే ఒకవేళ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    లేకపోతే ఒకవేళ (p[1] != NUL)  // skip "\X"
		p += 2;
	    లేకపోతే
		p += 1;
	}
	లేకపోతే ఒకవేళ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ఒకవేళ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    లేకపోతే
		p++;
	}
	లేకపోతే ఒకవేళ (MB_ISUPPER(*p))
	    తిరిగి TRUE;
	లేకపోతే
	    ++p;
    }
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_EVAL)
    char_u *
చివరి_csearch(శూన్య)
{
    తిరిగి lastc_బైట్లు;
}

    పూర్ణాంకం
చివరి_csearch_forward(శూన్య)
{
    తిరిగి lastcdir == FORWARD;
}

    పూర్ణాంకం
చివరి_csearch_until(శూన్య)
{
    తిరిగి చివరి_t_cmd == TRUE;
}

    శూన్య
సమితి_చివరి_csearch(పూర్ణాంకం c, char_u *s, పూర్ణాంకం len)
{
    *lastc = c;
    lastc_bytelen = len;
    ఒకవేళ (len)
	జ్ఞాపకం_నకలు(lastc_బైట్లు, s, len);
    లేకపోతే
	CLEAR_క్షేత్రం(lastc_బైట్లు);
}
#అయితే_ముగింపు

    శూన్య
సమితి_csearch_direction(పూర్ణాంకం cdir)
{
    lastcdir = cdir;
}

    శూన్య
సమితి_csearch_until(పూర్ణాంకం t_cmd)
{
    చివరి_t_cmd = t_cmd;
}

    char_u *
చివరి_శోధన_pat(శూన్య)
{
    తిరిగి spats[చివరి_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    శూన్య
reset_శోధన_dir(శూన్య)
{
    spats[0].off.dir = '/';
#ఒకవేళ defined(FEAT_EVAL)
    సమితి_vv_searchforward();
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    శూన్య
సమితి_చివరి_శోధన_pat(
    char_u	*s,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		magic,
    పూర్ణాంకం		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ఒకవేళ (*s == NUL)
	spats[idx].pat = ఖాళీ;
    లేకపోతే
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ఒకవేళ (spats[idx].pat == ఖాళీ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ఒకవేళ defined(FEAT_EVAL)
    సమితి_vv_searchforward();
# endif
    spats[idx].off.పంక్తి = FALSE;
    spats[idx].off.చివర = FALSE;
    spats[idx].off.off = 0;
    ఒకవేళ (setlast)
	చివరి_idx = idx;
    ఒకవేళ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ఒకవేళ (spats[idx].pat == ఖాళీ)
	    saved_spats[idx].pat = ఖాళీ;
	లేకపోతే
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ఒకవేళ (saved_spats[idx].pat == ఖాళీ)
	    saved_spats[idx].patlen = 0;
	లేకపోతే
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_శోధన_EXTRA
	saved_spats_చివరి_idx = చివరి_idx;
# endif
    }
# ifdef FEAT_శోధన_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ఒకవేళ (p_hls && idx == చివరి_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_శోధన_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    శూన్య
చివరి_pat_prog(regmmatch_T *regmatch)
{
    ఒకవేళ (spats[చివరి_idx].pat == ఖాళీ)
    {
	regmatch->regprog = ఖాళీ;
	తిరిగి;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (శూన్య)శోధన_regcomp((char_u *)"", 0, ఖాళీ, 0, చివరి_idx, శోధన_KEEP, regmatch);
    --emsg_off;
}
#అయితే_ముగింపు

/*
 * Lowest level search function.
 * Search for 'count'th occurrence of pattern "pat" in direction "dir".
 * Start at position "pos" and return the found position in "pos".
 *
 * if (options & SEARCH_MSG) == 0 don't give any messages
 * if (options & SEARCH_MSG) == SEARCH_NFMSG don't give 'notfound' messages
 * if (options & SEARCH_MSG) == SEARCH_MSG give all messages
 * if (options & SEARCH_HIS) put search pattern in history
 * if (options & SEARCH_END) return position at end of match
 * if (options & SEARCH_START) accept match at pos itself
 * if (options & SEARCH_KEEP) keep previous search pattern
 * if (options & SEARCH_FOLD) match only once in a closed fold
 * if (options & SEARCH_PEEK) check for typed char, cancel search
 * if (options & SEARCH_COL) start at pos->col instead of zero
 *
 * Return FAIL (zero) for failure, non-zero for success.
 * When FEAT_EVAL is defined, returns the index of the first matching
 * subpattern plus one; one if there was none.
 */
    పూర్ణాంకం
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*చివర_pos,	// set to end of the match, unless NULL
    పూర్ణాంకం		dir,
    char_u	*pat,
    size_t	patlen,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		options,
    పూర్ణాంకం		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    పూర్ణాంకం		దొరికింది;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	నిలువు;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    పూర్ణాంకం		loop;
    pos_T	ప్రారంభం_pos;
    పూర్ణాంకం		at_మొదటి_పంక్తి;
    పూర్ణాంకం		extra_నిలువు;
    పూర్ణాంకం		ప్రారంభం_char_len;
    పూర్ణాంకం		సరిపోలిక_ok;
    దీర్ఘ	nmatched;
    పూర్ణాంకం		submatch = 0;
    పూర్ణాంకం		మొదటి_సరిపోలిక = TRUE;
    పూర్ణాంకం		called_emsg_before = called_emsg;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    పూర్ణాంకం		break_loop = FALSE;
#అయితే_ముగింపు
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    పూర్ణాంకం		వాడని_timeout_flag = FALSE;
    పూర్ణాంకం		*timed_బహిర్గతం = &వాడని_timeout_flag;  // set when timed out.
    పూర్ణాంకం		శోధన_from_సరిపోలిక_చివర;		    // vi-compatible search?

    ఒకవేళ (శోధన_regcomp(pat, patlen, ఖాళీ, RE_శోధన, pat_use,
		   (options & (శోధన_HIS + శోధన_KEEP)), &regmatch) == FAIL)
    {
	ఒకవేళ ((options & శోధన_సందేశం) && !rc_did_emsg)
	    semsg(_(e_invalid_శోధన_string_str), mr_నమూనా);
	తిరిగి FAIL;
    }

    శోధన_from_సరిపోలిక_చివర = vim_strchr(p_cpo, CPO_శోధన) != ఖాళీ;

    ఒకవేళ (extra_arg != ఖాళీ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#నిర్వచితమైతే FEAT_RELTIME
	ఒకవేళ (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_బహిర్గతం = &extra_arg->sa_timed_బహిర్గతం;
#అయితే_ముగింపు
    }

    /*
     * find the string
     */
    చేయి	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	ఒకవేళ (pos->నిలువు == MAXCOL)
	    ప్రారంభం_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	లేకపోతే ఒకవేళ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_పంక్తి_లెక్క
						    && pos->నిలువు < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ఒకవేళ (ml_get_buf_len(buf, pos->lnum) <= pos->నిలువు)
		ప్రారంభం_char_len = 1;
	    లేకపోతే
		ప్రారంభం_char_len = (*mb_ptr2len)(ptr + pos->నిలువు);
	}
	లేకపోతే
	    ప్రారంభం_char_len = 1;
	ఒకవేళ (dir == FORWARD)
	{
	    ఒకవేళ (options & శోధన_ప్రారంభం)
		extra_నిలువు = 0;
	    లేకపోతే
		extra_నిలువు = ప్రారంభం_char_len;
	}
	లేకపోతే
	{
	    ఒకవేళ (options & శోధన_ప్రారంభం)
		extra_నిలువు = ప్రారంభం_char_len;
	    లేకపోతే
		extra_నిలువు = 0;
	}

	ప్రారంభం_pos = *pos;	// remember start pos for detecting no match
	దొరికింది = 0;		// default: not found
	at_మొదటి_పంక్తి = TRUE;	// default: start in first line
	ఒకవేళ (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->నిలువు = 0;
	    at_మొదటి_పంక్తి = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	ఒకవేళ (dir == BACKWARD && ప్రారంభం_pos.నిలువు == 0
					     && (options & శోధన_ప్రారంభం) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_మొదటి_పంక్తి = FALSE;
	}
	లేకపోతే
	    lnum = pos->lnum;

	కొరకు (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    కొరకు ( ; lnum > 0 && lnum <= buf->b_ml.ml_పంక్తి_లెక్క;
					   lnum += dir, at_మొదటి_పంక్తి = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ఒకవేళ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    ఆపు;
		// Stop after passing the time limit.
		ఒకవేళ (*timed_బహిర్గతం)
		    ఆపు;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		నిలువు = at_మొదటి_పంక్తి && (options & శోధన_నిలువు) ? pos->నిలువు
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, నిలువు, timed_బహిర్గతం);
		// vim_regexec_multi() may clear "regprog"
		ఒకవేళ (regmatch.regprog == ఖాళీ)
		    ఆపు;
		// Abort searching on an error (e.g., out of stack).
		ఒకవేళ (called_emsg > called_emsg_before || *timed_బహిర్గతం)
		    ఆపు;
		ఒకవేళ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#నిర్వచితమైతే FEAT_EVAL
		    submatch = మొదటి_submatch(&regmatch);
#అయితే_ముగింపు
		    // "lnum" may be past end of buffer for "\n\zs".
		    ఒకవేళ (lnum + matchpos.lnum > buf->b_ml.ml_పంక్తి_లెక్క)
			ptr = (char_u *)"";
		    లేకపోతే
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ఒకవేళ (dir == FORWARD && at_మొదటి_పంక్తి)
		    {
			సరిపోలిక_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			అయితే (matchpos.lnum == 0
				&& ((options & శోధన_చివర) && మొదటి_సరిపోలిక
				    ?  (nmatched == 1
					&& (పూర్ణాంకం)endpos.నిలువు - 1
					     < (పూర్ణాంకం)ప్రారంభం_pos.నిలువు + extra_నిలువు)
				    : ((పూర్ణాంకం)matchpos.నిలువు
						  - (ptr[matchpos.నిలువు] == NUL)
					    < (పూర్ణాంకం)ప్రారంభం_pos.నిలువు + extra_నిలువు)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ఒకవేళ (శోధన_from_సరిపోలిక_చివర)
			    {
				ఒకవేళ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    సరిపోలిక_ok = FALSE;
				    ఆపు;
				}
				matchcol = endpos.నిలువు;
				// for empty match: advance one char
				ఒకవేళ (matchcol == matchpos.నిలువు
						      && ptr[matchcol] != NUL)
				{
				    ఒకవేళ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    లేకపోతే
					++matchcol;
				}
			    }
			    లేకపోతే
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ఒకవేళ (ptr[matchcol] != NUL)
				{
				    ఒకవేళ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    లేకపోతే
					++matchcol;
				}
			    }
			    ఒకవేళ (matchcol == 0 && (options & శోధన_ప్రారంభం))
				ఆపు;
			    ఒకవేళ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_బహిర్గతం)) == 0)
			    {
				సరిపోలిక_ok = FALSE;
				ఆపు;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ఒకవేళ (regmatch.regprog == ఖాళీ)
				ఆపు;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#నిర్వచితమైతే FEAT_EVAL
			    submatch = మొదటి_submatch(&regmatch);
#అయితే_ముగింపు

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ఒకవేళ (!సరిపోలిక_ok)
			    కొనసాగు;
		    }
		    ఒకవేళ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			సరిపోలిక_ok = FALSE;
			కొరకు (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ఒకవేళ (loop
				|| ((options & శోధన_చివర)
				    ? (lnum + regmatch.endpos[0].lnum
							      < ప్రారంభం_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == ప్రారంభం_pos.lnum
					     && (పూర్ణాంకం)regmatch.endpos[0].నిలువు - 1
							< (పూర్ణాంకం)ప్రారంభం_pos.నిలువు
								+ extra_నిలువు))
				    : (lnum + regmatch.startpos[0].lnum
							      < ప్రారంభం_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == ప్రారంభం_pos.lnum
					     && (పూర్ణాంకం)regmatch.startpos[0].నిలువు
						      < (పూర్ణాంకం)ప్రారంభం_pos.నిలువు
							      + extra_నిలువు))))
			    {
				సరిపోలిక_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#నిర్వచితమైతే FEAT_EVAL
				submatch = మొదటి_submatch(&regmatch);
#అయితే_ముగింపు
			    }
			    లేకపోతే
				ఆపు;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ఒకవేళ (శోధన_from_సరిపోలిక_చివర)
			    {
				ఒకవేళ (nmatched > 1)
				    ఆపు;
				matchcol = endpos.నిలువు;
				// for empty match: advance one char
				ఒకవేళ (matchcol == matchpos.నిలువు
						      && ptr[matchcol] != NUL)
				{
				    ఒకవేళ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    లేకపోతే
					++matchcol;
				}
			    }
			    లేకపోతే
			    {
				// Stop when the match is in a next line.
				ఒకవేళ (matchpos.lnum > 0)
				    ఆపు;
				matchcol = matchpos.నిలువు;
				ఒకవేళ (ptr[matchcol] != NUL)
				{
				    ఒకవేళ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    లేకపోతే
					++matchcol;
				}
			    }
			    ఒకవేళ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_బహిర్గతం)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ఒకవేళ (*timed_బహిర్గతం)
				    సరిపోలిక_ok = FALSE;
				ఆపు;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ఒకవేళ (regmatch.regprog == ఖాళీ)
				ఆపు;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ఒకవేళ (!సరిపోలిక_ok)
			    కొనసాగు;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ఒకవేళ ((options & శోధన_చివర) && !(options & శోధన_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.నిలువు == endpos.నిలువు))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->నిలువు = endpos.నిలువు;
			ఒకవేళ (endpos.నిలువు == 0)
			{
			    ఒకవేళ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->నిలువు = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			లేకపోతే
			{
			    --pos->నిలువు;
			    ఒకవేళ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_పంక్తి_లెక్క)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->నిలువు -= (*mb_తల_off)(ptr, ptr + pos->నిలువు);
			    }
			}
			ఒకవేళ (చివర_pos != ఖాళీ)
			{
			    చివర_pos->lnum = lnum + matchpos.lnum;
			    చివర_pos->నిలువు = matchpos.నిలువు;
			}
		    }
		    లేకపోతే
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->నిలువు = matchpos.నిలువు;
			ఒకవేళ (చివర_pos != ఖాళీ)
			{
			    చివర_pos->lnum = lnum + endpos.lnum;
			    చివర_pos->నిలువు = endpos.నిలువు;
			}
		    }
		    pos->coladd = 0;
		    ఒకవేళ (చివర_pos != ఖాళీ)
			చివర_pos->coladd = 0;
		    దొరికింది = 1;
		    మొదటి_సరిపోలిక = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    శోధన_సరిపోలిక_పంక్తులు = endpos.lnum - matchpos.lnum;
		    శోధన_సరిపోలిక_endcol = endpos.నిలువు;
		    ఆపు;
		}
		పంక్తి_breakcheck();	// stop if ctrl-C typed
		ఒకవేళ (got_int)
		    ఆపు;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ఒకవేళ ((options & శోధన_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    ఆపు;
		}
#అయితే_ముగింపు

		ఒకవేళ (loop && lnum == ప్రారంభం_pos.lnum)
		    ఆపు;	    // if second loop, stop where started
	    }
	    at_మొదటి_పంక్తి = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ఒకవేళ (regmatch.regprog == ఖాళీ)
		ఆపు;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ఒకవేళ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_బహిర్గతం
#నిర్వచితమైతే FEAT_శోధన_EXTRA
			      || break_loop
#అయితే_ముగింపు
			      || దొరికింది || loop)
		ఆపు;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ఒకవేళ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_పంక్తి_లెక్క;
	    లేకపోతే
		lnum = 1;
	    ఒకవేళ (!shortmess(SHM_శోధన)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & శోధన_సందేశం))
		give_warning((char_u *)_(dir == BACKWARD
					  ? శిఖరం_bot_సందేశం : bot_శిఖరం_సందేశం), TRUE);
	    ఒకవేళ (extra_arg != ఖాళీ)
		extra_arg->sa_wrapped = TRUE;
	}
	ఒకవేళ (got_int || called_emsg > called_emsg_before || *timed_బహిర్గతం
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		|| break_loop
#అయితే_ముగింపు
		)
	    ఆపు;
    }
    అయితే (--లెక్క > 0 && దొరికింది);   // stop after count matches or no match

#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (extra_arg != ఖాళీ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#అయితే_ముగింపు
    vim_regfree(regmatch.regprog);

    ఒకవేళ (!దొరికింది)		    // did not find it
    {
	ఒకవేళ (got_int)
	    emsg(_(e_interrupted));
	లేకపోతే ఒకవేళ ((options & శోధన_సందేశం) == శోధన_సందేశం)
	{
	    ఒకవేళ (p_ws)
		semsg(_(e_నమూనా_not_దొరికింది_str), mr_నమూనా);
	    లేకపోతే ఒకవేళ (lnum == 0)
		semsg(_(e_శోధన_తగిలింది_శిఖరం_without_సరిపోలిక_for_str), mr_నమూనా);
	    లేకపోతే
		semsg(_(e_శోధన_తగిలింది_bottom_without_సరిపోలిక_for_str), mr_నమూనా);
	}
	తిరిగి FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ఒకవేళ (pos->lnum > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	pos->lnum = buf->b_ml.ml_పంక్తి_లెక్క;
	pos->నిలువు = ml_get_buf_len(buf, pos->lnum);
	ఒకవేళ (pos->నిలువు > 0)
	    --pos->నిలువు;
    }

    తిరిగి submatch + 1;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    శూన్య
సమితి_శోధన_direction(పూర్ణాంకం cdir)
{
    spats[0].off.dir = cdir;
}

    స్థిర శూన్య
సమితి_vv_searchforward(శూన్య)
{
    సమితి_vim_var_nr(VV_SEARCHFORWARD, (దీర్ఘ)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    స్థిర పూర్ణాంకం
మొదటి_submatch(regmmatch_T *rp)
{
    పూర్ణాంకం		submatch;

    కొరకు (submatch = 1; ; ++submatch)
    {
	ఒకవేళ (rp->startpos[submatch].lnum >= 0)
	    ఆపు;
	ఒకవేళ (submatch == 9)
	{
	    submatch = 0;
	    ఆపు;
	}
    }
    తిరిగి submatch;
}
#అయితే_ముగింపు

/*
 * Parse a search pattern followed by an optional offset (e.g. "pat/e+1").
 * On entry "*pat" points at the start of the pattern and "*patlen" is its
 * length.  Updates the in/out parameters:
 *    *pat / *patlen	   - moved past the pattern and offset
 *    *strcopy		   - allocated copy if "\?" or "\/" was unescaped
 *			     (caller must vim_free() it)
 *    *searchstr and *searchstrlen - pointer/length of the search pattern only
 *    *dircp		   - location of the trailing delimiter that was
 *			     replaced with NUL (or NULL); caller may restore
 *			     it
 *    *offset		   - parsed offset (line/end/off)
 *
 * Returns the length of the parsed pattern + offset (used by get_address()
 * to know how much of the command line was consumed).
 */
    పూర్ణాంకం
parse_శోధన_నమూనా_స్థానభ్రంశం(
    char_u	**pat,
    size_t	*patlen,
    పూర్ణాంకం		శోధన_delim,
    పూర్ణాంకం		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*స్థానభ్రంశం)
{
    పూర్ణాంకం		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ఒకవేళ (*pat == ఖాళీ || **pat == NUL)
	తిరిగి 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ఖాళీ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, శోధన_delim, magic_isset(),
						    strcopy, ఖాళీ, ఖాళీ);
    ఒకవేళ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (పూర్ణాంకం)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ఒకవేళ (*p == శోధన_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    స్థానభ్రంశం->పంక్తి = FALSE;
    స్థానభ్రంశం->చివర = FALSE;
    స్థానభ్రంశం->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    ఒకవేళ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	స్థానభ్రంశం->పంక్తి = TRUE;
    లేకపోతే ఒకవేళ ((options & శోధన_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ఒకవేళ (*p == 'e')		// end
	    స్థానభ్రంశం->చివర = శోధన_చివర;
	++p;
    }
    ఒకవేళ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ఒకవేళ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    స్థానభ్రంశం->off = atol((అక్షరం *)p);
	లేకపోతే ఒకవేళ (*p == '-')	    // single '-'
	    స్థానభ్రంశం->off = -1;
	లేకపోతే			    // single '+'
	    స్థానభ్రంశం->off = 1;
	++p;
	అయితే (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (పూర్ణాంకం)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    తిరిగి cmdlen;
}

/*
 * Highest level string search function.
 * Search for the 'count'th occurrence of pattern 'pat' in direction 'dirc'
 *		  If 'dirc' is 0: use previous dir.
 *    If 'pat' is NULL or empty : use previous string.
 *    If 'options & SEARCH_REV' : go in reverse of previous dir.
 *    If 'options & SEARCH_ECHO': echo the search command and handle options
 *    If 'options & SEARCH_MSG' : may give error message
 *    If 'options & SEARCH_OPT' : interpret optional flags
 *    If 'options & SEARCH_HIS' : put search pattern in history
 *    If 'options & SEARCH_NOOF': don't add offset to position
 *    If 'options & SEARCH_MARK': set previous context mark
 *    If 'options & SEARCH_KEEP': keep previous search pattern
 *    If 'options & SEARCH_START': accept match at curpos itself
 *    If 'options & SEARCH_PEEK': check for typed char, cancel search
 *
 * Careful: If spats[0].off.line == TRUE and spats[0].off.off == 0 this
 * makes the movement linewise without moving the match position.
 *
 * Return 0 for failure, 1 for found, 2 for found and line offset added.
 */
    పూర్ణాంకం
do_శోధన(
    oparg_T	    *oap,	// can be NULL
    పూర్ణాంకం		    dirc,	// '/' or '?'
    పూర్ణాంకం		    శోధన_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    దీర్ఘ	    లెక్క,
    పూర్ణాంకం		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    పూర్ణాంకం		    retval;	// Return value
    char_u	    *p;
    దీర్ఘ	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ఖాళీ;
    పూర్ణాంకం		    show_శోధన_stats;
    char_u	    *msgbuf = ఖాళీ;
    size_t	    msgbuflen = 0;
    పూర్ణాంకం		    has_స్థానభ్రంశం = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ఒకవేళ (spats[0].off.పంక్తి && vim_strchr(p_cpo, CPO_LINEOFF) != ఖాళీ)
    {
	spats[0].off.పంక్తి = FALSE;
	spats[0].off.off = 0;
    }

    /*
     * Save the values for when (options & SEARCH_KEEP) is used.
     * (there is no "if ()" around this because gcc wants them initialized)
     */
    old_off = spats[0].off;

    pos = curwin->w_cursor;	// start searching at the cursor position

    /*
     * Find out the direction of the search.
     */
    ఒకవేళ (dirc == 0)
	dirc = spats[0].off.dir;
    లేకపోతే
    {
	spats[0].off.dir = dirc;
#ఒకవేళ defined(FEAT_EVAL)
	సమితి_vv_searchforward();
#అయితే_ముగింపు
    }
    ఒకవేళ (options & శోధన_REV)
    {
#నిర్వచితమైతే MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#లేకపోతే
	ఒకవేళ (dirc == '/')
	    dirc = '?';
	లేకపోతే
	    dirc = '/';
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ఒకవేళ (dirc == '/')
    {
	ఒకవేళ (hasFolding(pos.lnum, ఖాళీ, &pos.lnum))
	    pos.నిలువు = MAXCOL - 2;	// avoid overflow when adding 1
    }
    లేకపోతే
    {
	ఒకవేళ (hasFolding(pos.lnum, &pos.lnum, ఖాళీ))
	    pos.నిలువు = 0;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ఒకవేళ (no_hlsearch && !(options & శోధన_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	సమితి_no_hlsearch(FALSE);
    }
#అయితే_ముగింపు

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    కొరకు (;;)
    {
	పూర్ణాంకం		show_శిఖరం_bot_సందేశం = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ఖాళీ;
					    // use previous pattern
	ఒకవేళ (pat == ఖాళీ || *pat == NUL || *pat == శోధన_delim)
	{
	    ఒకవేళ (spats[RE_శోధన].pat == ఖాళీ)	    // no previous pattern
	    {
		ఒకవేళ (spats[RE_SUBST].pat == ఖాళీ)
		{
		    emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
		    retval = 0;
		    వెళ్ళు చివర_do_శోధన;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    లేకపోతే
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ఒకవేళ (pat != ఖాళీ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_శోధన_నమూనా_స్థానభ్రంశం(&pat, &patlen,
				శోధన_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ఒకవేళ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_శోధన_stats = FALSE;
	ఒకవేళ ((options & శోధన_ECHO) && messaging()
		&& !సందేశం_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    సందేశం_ప్రారంభం();

	    // Get the offset, so we know how long it is.
	    ఒకవేళ (!cmd_silent &&
		    (spats[0].off.పంక్తి || spats[0].off.చివర || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ఒకవేళ (spats[0].off.చివర)
		    off_buf[off_len++] = 'e';
		లేకపోతే ఒకవేళ (!spats[0].off.పంక్తి)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ఒకవేళ (spats[0].off.off != 0 || spats[0].off.పంక్తి)
		    off_len += vim_snprintf((అక్షరం *)off_buf + off_len,
			   పరిమాణం(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ఒకవేళ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    లేకపోతే
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ఒకవేళ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ఒకవేళ (సందేశం_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (పూర్ణాంకం)(అడ్డువరుసలు - సందేశం_అడ్డువరుస) * cmdline_వెడల్పు - 1;
		లేకపోతే
		    // Use up to 'showcmd' column.
		    msgbufsize = (పూర్ణాంకం)(అడ్డువరుసలు - సందేశం_అడ్డువరుస - 1) * cmdline_వెడల్పు
			+ sc_నిలువు - 1;
		ఒకవేళ (msgbufsize < plen + off_len + శోధన_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + శోధన_STAT_BUF_LEN + 3;
	    }
	    లేకపోతే
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ఒకవేళ (msgbuf == ఖాళీ)
	    {
		msgbuflen = 0;
	    }
	    లేకపోతే
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ఒకవేళ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ఒకవేళ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    లేకపోతే
			mch_memmove(msgbuf + 1, p, plen);
		    ఒకవేళ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = సందేశం_strtrunc(msgbuf, TRUE);
		    ఒకవేళ (trunc != ఖాళీ)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#నిర్వచితమైతే FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    ఒకవేళ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = తిరగేయి_పాఠ్యం(msgbuf);
			ఒకవేళ (r != ఖాళీ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    అయితే (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ఒకవేళ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    లేకపోతే
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#అయితే_ముగింపు
		    సందేశం_outtrans(msgbuf);
		    సందేశం_clr_eos();
		    సందేశం_check();

		    gotocmdline(FALSE);
		    బహిర్గతం_flush();
		    సందేశం_nowait = TRUE;	    // don't wait for this message
		}

		ఒకవేళ (!shortmess(SHM_SEARCHCOUNT))
		    show_శోధన_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	ఒకవేళ (!spats[0].off.పంక్తి && spats[0].off.off && pos.నిలువు < MAXCOL - 2)
	{
	    ఒకవేళ (spats[0].off.off > 0)
	    {
		కొరకు (c = spats[0].off.off; c; --c)
		    ఒకవేళ (decl(&pos) == -1)
			ఆపు;
		ఒకవేళ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.నిలువు = MAXCOL;
		}
	    }
	    లేకపోతే
	    {
		కొరకు (c = spats[0].off.off; c; ++c)
		    ఒకవేళ (incl(&pos) == -1)
			ఆపు;
		ఒకవేళ (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క + 1;
		    pos.నిలువు = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ఖాళీ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, లెక్క, spats[0].off.చివర + (options &
		       (శోధన_KEEP + శోధన_PEEK + శోధన_HIS
			+ శోధన_సందేశం + శోధన_ప్రారంభం
			+ ((pat != ఖాళీ && *pat == ';') ? 0 : శోధన_NOOF))),
		RE_చివరి, sia);

	ఒకవేళ (dircp != ఖాళీ)
	    *dircp = శోధన_delim; // restore second '/' or '?' for normal_cmd()

	ఒకవేళ (!shortmess(SHM_శోధన) && sia && sia->sa_wrapped)
	    show_శిఖరం_bot_సందేశం = TRUE;

	ఒకవేళ (c == FAIL)
	{
	    retval = 0;
	    వెళ్ళు చివర_do_శోధన;
	}
	ఒకవేళ (spats[0].off.చివర && oap != ఖాళీ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ఒకవేళ (!(options & శోధన_NOOF) || (pat != ఖాళీ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ఒకవేళ (spats[0].off.పంక్తి)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ఒకవేళ (c < 1)
		    pos.lnum = 1;
		లేకపోతే ఒకవేళ (c > curbuf->b_ml.ml_పంక్తి_లెక్క)
		    pos.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
		లేకపోతే
		    pos.lnum = c;
		pos.నిలువు = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    లేకపోతే ఒకవేళ (pos.నిలువు < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ఒకవేళ (c > 0)
		{
		    అయితే (c-- > 0)
			ఒకవేళ (incl(&pos) == -1)
			    ఆపు;
		}
		// to the left, check for start of file
		లేకపోతే
		{
		    అయితే (c++ < 0)
			ఒకవేళ (decl(&pos) == -1)
			    ఆపు;
		}
	    }
	    ఒకవేళ (!EQUAL_POS(pos, org_pos))
		has_స్థానభ్రంశం = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ఒకవేళ (show_శోధన_stats)
	     cmdline_శోధన_stat(dirc, &pos, &curwin->w_cursor,
				show_శిఖరం_bot_సందేశం, msgbuf, msgbuflen,
				(లెక్క != 1 || has_స్థానభ్రంశం
#నిర్వచితమైతే FEAT_FOLDING
				 || (!(fdo_flags & FDO_శోధన)
				     && hasFolding(curwin->w_cursor.lnum,
								   ఖాళీ, ఖాళీ))
#అయితే_ముగింపు
				),
				p_msc,
				శోధన_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	ఒకవేళ (!(options & శోధన_OPT) || pat == ఖాళీ || *pat != ';')
	    ఆపు;

	dirc = *++pat;
	--patlen;
	శోధన_delim = dirc;
	ఒకవేళ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    వెళ్ళు చివర_do_శోధన;
	}
	++pat;
	--patlen;
    }

    ఒకవేళ (options & శోధన_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_సమితి_curswant = true;

చివర_do_శోధన:
    ఒకవేళ ((options & శోధన_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    తిరిగి retval;
}

/*
 * search_for_exact_line(buf, pos, dir, pat)
 *
 * Search for a line starting with the given pattern (ignoring leading
 * white-space), starting from pos and going in direction "dir". "pos" will
 * contain the position of the match found.    Blank lines match only if
 * ADDING is set.  If p_ic is set then the pattern must be in lowercase.
 * Return OK for success, or FAIL if no line found.
 */
    పూర్ణాంకం
శోధన_for_exact_పంక్తి(
    buf_T	*buf,
    pos_T	*pos,
    పూర్ణాంకం		dir,
    char_u	*pat)
{
    linenr_T	ప్రారంభం = 0;
    char_u	*ptr;
    char_u	*p;
    పూర్ణాంకం		compl_len = ins_compl_len();

    ఒకవేళ (buf->b_ml.ml_పంక్తి_లెక్క == 0)
	తిరిగి FAIL;
    కొరకు (;;)
    {
	pos->lnum += dir;
	ఒకవేళ (pos->lnum < 1)
	{
	    ఒకవేళ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_పంక్తి_లెక్క;
		ఒకవేళ (!shortmess(SHM_శోధన))
		    give_warning((char_u *)_(శిఖరం_bot_సందేశం), TRUE);
	    }
	    లేకపోతే
	    {
		pos->lnum = 1;
		ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (pos->lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	{
	    ఒకవేళ (p_ws)
	    {
		pos->lnum = 1;
		ఒకవేళ (!shortmess(SHM_శోధన))
		    give_warning((char_u *)_(bot_శిఖరం_సందేశం), TRUE);
	    }
	    లేకపోతే
	    {
		pos->lnum = 1;
		ఆపు;
	    }
	}
	ఒకవేళ (pos->lnum == ప్రారంభం)
	    ఆపు;
	ఒకవేళ (ప్రారంభం == 0)
	    ప్రారంభం = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->నిలువు = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ఒకవేళ (compl_స్థితి_adding() && !compl_స్థితి_sol())
	{
	    ఒకవేళ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		తిరిగి OK;
	}
	లేకపోతే ఒకవేళ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ఒకవేళ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		తిరిగి OK;
	}
    }
    తిరిగి FAIL;
}

/*
 * Character Searches
 */

/*
 * Search for a character in a line.  If "t_cmd" is FALSE, move to the
 * position of the character, otherwise move to just before the char.
 * Do this "cap->count1" times.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
searchc(cmdarg_T *cap, పూర్ణాంకం t_cmd)
{
    పూర్ణాంకం			c = cap->nchar;	// char to search for
    పూర్ణాంకం			dir = cap->arg;	// TRUE for searching forward
    దీర్ఘ		లెక్క = cap->count1;	// repeat count
    పూర్ణాంకం			నిలువు;
    char_u		*p;
    పూర్ణాంకం			len;
    పూర్ణాంకం			stop = TRUE;

    ఒకవేళ (c != NUL)	// normal search: remember args for repeat
    {
	ఒకవేళ (!కీలకం_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    సమితి_csearch_direction(dir);
	    సమితి_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_బైట్లు);
	    ఒకవేళ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_బైట్లు + lastc_bytelen);
		ఒకవేళ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_బైట్లు + lastc_bytelen);
	    }
	}
    }
    లేకపోతే		// repeat previous search
    {
	ఒకవేళ (*lastc == NUL && lastc_bytelen <= 1)
	    తిరిగి FAIL;
	ఒకవేళ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	లేకపోతే
	    dir = lastcdir;
	t_cmd = చివరి_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ఒకవేళ (vim_strchr(p_cpo, CPO_SCOLON) == ఖాళీ && లెక్క == 1 && t_cmd)
	    stop = FALSE;
    }

    ఒకవేళ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    లేకపోతే
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    నిలువు = curwin->w_cursor.నిలువు;
    len = ml_get_curline_len();

    అయితే (లెక్క--)
    {
	ఒకవేళ (has_mbyte)
	{
	    కొరకు (;;)
	    {
		ఒకవేళ (dir > 0)
		{
		    నిలువు += (*mb_ptr2len)(p + నిలువు);
		    ఒకవేళ (నిలువు >= len)
			తిరిగి FAIL;
		}
		లేకపోతే
		{
		    ఒకవేళ (నిలువు == 0)
			తిరిగి FAIL;
		    నిలువు -= (*mb_తల_off)(p, p + నిలువు - 1) + 1;
		}
		ఒకవేళ (lastc_bytelen <= 1)
		{
		    ఒకవేళ (p[నిలువు] == c && stop)
			ఆపు;
		}
		లేకపోతే ఒకవేళ (STRNCMP(p + నిలువు, lastc_బైట్లు, lastc_bytelen) == 0
								       && stop)
		    ఆపు;
		stop = TRUE;
	    }
	}
	లేకపోతే
	{
	    కొరకు (;;)
	    {
		ఒకవేళ ((నిలువు += dir) < 0 || నిలువు >= len)
		    తిరిగి FAIL;
		ఒకవేళ (p[నిలువు] == c && stop)
		    ఆపు;
		stop = TRUE;
	    }
	}
    }

    ఒకవేళ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	నిలువు -= dir;
	ఒకవేళ (has_mbyte)
	{
	    ఒకవేళ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		నిలువు += lastc_bytelen - 1;
	    లేకపోతే
		// To previous char, which may be multi-byte.
		నిలువు -= (*mb_తల_off)(p, p + నిలువు);
	}
    }
    curwin->w_cursor.నిలువు = నిలువు;

    తిరిగి OK;
}

/*
 * "Other" Searches
 */

/*
 * findmatch - find the matching paren or brace
 *
 * Improvement over vi: Braces inside quotes are ignored.
 */
    pos_T *
findmatch(oparg_T *oap, పూర్ణాంకం initc)
{
    తిరిగి findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    స్థిర పూర్ణాంకం
check_prevcol(
    char_u	*linep,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		ch,
    పూర్ణాంకం		*prevcol)
{
    --నిలువు;
    ఒకవేళ (నిలువు > 0 && has_mbyte)
	నిలువు -= (*mb_తల_off)(linep, linep + నిలువు);
    ఒకవేళ (prevcol)
	*prevcol = నిలువు;
    తిరిగి (నిలువు >= 0 && linep[నిలువు] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    స్థిర పూర్ణాంకం
కనుగొను_rawstring_చివర(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    పూర్ణాంకం		దొరికింది = FALSE;

    కొరకు (p = linep + startpos->నిలువు + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->నిలువు - 1;
    delim_copy = vim_strnsave(linep + startpos->నిలువు + 1, delim_len);
    ఒకవేళ (delim_copy == ఖాళీ)
	తిరిగి FALSE;
    కొరకు (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *పంక్తి = ml_get(lnum);

	కొరకు (p = పంక్తి + (lnum == startpos->lnum
					    ? startpos->నిలువు + 1 : 0); *p; ++p)
	{
	    ఒకవేళ (lnum == endpos->lnum && (colnr_T)(p - పంక్తి) >= endpos->నిలువు)
		ఆపు;
	    ఒకవేళ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		దొరికింది = TRUE;
		ఆపు;
	    }
	}
	ఒకవేళ (దొరికింది)
	    ఆపు;
    }
    vim_free(delim_copy);
    తిరిగి దొరికింది;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    స్థిర శూన్య
కనుగొను_mps_విలువలు(
    పూర్ణాంకం	    *initc,
    పూర్ణాంకం	    *findc,
    పూర్ణాంకం	    *backwards,
    పూర్ణాంకం	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    అయితే (*ptr != NUL)
    {
	ఒకవేళ (has_mbyte)
	{
	    char_u *prev;

	    ఒకవేళ (mb_ptr2char(ptr) == *initc)
	    {
		ఒకవేళ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		లేకపోతే
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		తిరిగి;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ఒకవేళ (mb_ptr2char(ptr) == *initc)
	    {
		ఒకవేళ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		లేకపోతే
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		తిరిగి;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	లేకపోతే
	{
	    ఒకవేళ (*ptr == *initc)
	    {
		ఒకవేళ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		లేకపోతే
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		తిరిగి;
	    }
	    ptr += 2;
	    ఒకవేళ (*ptr == *initc)
	    {
		ఒకవేళ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		లేకపోతే
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		తిరిగి;
	    }
	    ++ptr;
	}
	ఒకవేళ (*ptr == ',')
	    ++ptr;
    }
}

/*
 * findmatchlimit -- find the matching paren or brace, if it exists within
 * maxtravel lines of the cursor.  A maxtravel of 0 means search until falling
 * off the edge of the file.
 *
 * "initc" is the character to find a match for.  NUL means to find the
 * character at or after the cursor. Special values:
 * '*'  look for C-style comment / *
 * '/'  look for C-style comment / *, ignoring comment-end
 * '#'  look for preprocessor directives
 * 'R'  look for raw string start: R"delim(text)delim" (only backwards)
 *
 * flags: FM_BACKWARD	search backwards (when initc is '/', '*' or '#')
 *	  FM_FORWARD	search forwards (when initc is '/', '*' or '#')
 *	  FM_BLOCKSTOP	stop at start/end of block ({ or } in column 0)
 *	  FM_SKIPCOMM	skip over comments (cursor must start outside a block comment)
 *
 * "oap" is only used to set oap->motion_type for a linewise motion, it can be
 * NULL
 */
    pos_T *
findmatchlimit(
    oparg_T	*oap,
    పూర్ణాంకం		initc,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		maxtravel)
{
    స్థిర pos_T pos;			// current search position
    పూర్ణాంకం		findc = 0;		// matching brace
    పూర్ణాంకం		c;
    పూర్ణాంకం		లెక్క = 0;		// cumulative number of braces
    పూర్ణాంకం		backwards = FALSE;	// init for gcc
    పూర్ణాంకం		raw_string = FALSE;	// search for raw string
    పూర్ణాంకం		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    పూర్ణాంకం		do_quotes;		// check for quotes in current line
    పూర్ణాంకం		at_ప్రారంభం;		// do_quotes value at start position
    పూర్ణాంకం		hash_dir = 0;		// Direction searched for # things
    పూర్ణాంకం		comment_dir = 0;	// Direction searched for comments
    pos_T	సరిపోలిక_pos;		// Where last slash-star was found
    పూర్ణాంకం		ప్రారంభం_లోపల_quotes;	// start position is in quotes
    పూర్ణాంకం		traveled = 0;		// how far we've searched so far
    పూర్ణాంకం		ignore_cend = FALSE;    // ignore comment end
    పూర్ణాంకం		cpo_సరిపోలిక;		// vi compatible matching
    పూర్ణాంకం		cpo_bsl;		// don't recognize backslashes
    పూర్ణాంకం		సరిపోలిక_escaped = 0;	// search for escaped match
    పూర్ణాంకం		dir;			// Direction to search
    పూర్ణాంకం		comment_నిలువు = MAXCOL;   // start of / / comment
    పూర్ణాంకం		lispcomm = FALSE;	// inside of Lisp-style comment
    పూర్ణాంకం		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    పూర్ణాంకం		skip_comments = (flags & FM_SKIPCOMM) != 0;
    పూర్ణాంకం		లోపల_ఖండం_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_సరిపోలిక = (vim_strchr(p_cpo, CPO_సరిపోలిక) != ఖాళీ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ఖాళీ);

    // Direction to search when initc is '/', '*' or '#'
    ఒకవేళ (flags & FM_BACKWARD)
	dir = BACKWARD;
    లేకపోతే ఒకవేళ (flags & FM_FORWARD)
	dir = FORWARD;
    లేకపోతే
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ఒకవేళ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ఒకవేళ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    లేకపోతే ఒకవేళ (initc != '#' && initc != NUL)
    {
	కనుగొను_mps_విలువలు(&initc, &findc, &backwards, TRUE);
	ఒకవేళ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ఒకవేళ (findc == NUL)
	    తిరిగి ఖాళీ;
    }
    లేకపోతే
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ఒకవేళ (initc == '#')
	{
	    hash_dir = dir;
	}
	లేకపోతే
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ఒకవేళ (!cpo_సరిపోలిక)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ఒకవేళ (*ptr == '#' && pos.నిలువు <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ఒకవేళ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		లేకపోతే ఒకవేళ (linep[pos.నిలువు] == '/')
		{
		    ఒకవేళ (linep[pos.నిలువు + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.నిలువు++;
		    }
		    లేకపోతే ఒకవేళ (pos.నిలువు > 0 && linep[pos.నిలువు - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.నిలువు--;
		    }
		}
		లేకపోతే ఒకవేళ (linep[pos.నిలువు] == '*')
		{
		    ఒకవేళ (linep[pos.నిలువు + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    లేకపోతే ఒకవేళ (pos.నిలువు > 0 && linep[pos.నిలువు - 1] == '/')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
		    }
		}
	    }

	    /*
	     * If we are not on a comment or the # at the start of a line, then
	     * look for brace anywhere on this line after the cursor.
	     */
	    ఒకవేళ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ఒకవేళ (linep[pos.నిలువు] == NUL && pos.నిలువు)
		    --pos.నిలువు;
		కొరకు (;;)
		{
		    initc = PTR2CHAR(linep + pos.నిలువు);
		    ఒకవేళ (initc == NUL)
			ఆపు;

		    కనుగొను_mps_విలువలు(&initc, &findc, &backwards, FALSE);
		    ఒకవేళ (findc)
			ఆపు;
		    pos.నిలువు += mb_ptr2len(linep + pos.నిలువు);
		}
		ఒకవేళ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ఒకవేళ (!cpo_సరిపోలిక && *skipwhite(linep) == '#')
			hash_dir = 1;
		    లేకపోతే
			తిరిగి ఖాళీ;
		}
		లేకపోతే ఒకవేళ (!cpo_bsl)
		{
		    పూర్ణాంకం నిలువు, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    కొరకు (నిలువు = pos.నిలువు; check_prevcol(linep, నిలువు, '\\', &నిలువు);)
			bslcnt++;
		    సరిపోలిక_escaped = (bslcnt & 1);
		}
	    }
	}
	ఒకవేళ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ఒకవేళ (oap != ఖాళీ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ఒకవేళ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ఒకవేళ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		లేకపోతే ఒకవేళ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		లేకపోతే
		    తిరిగి ఖాళీ;
	    }
	    pos.నిలువు = 0;
	    అయితే (!got_int)
	    {
		ఒకవేళ (hash_dir > 0)
		{
		    ఒకవేళ (pos.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
			ఆపు;
		}
		లేకపోతే ఒకవేళ (pos.lnum == 1)
		    ఆపు;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		పంక్తి_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ఒకవేళ (*ptr != '#')
		    కొనసాగు;
		pos.నిలువు = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ఒకవేళ (hash_dir > 0)
		{
		    ఒకవేళ (STRNCMP(ptr, "if", 2) == 0)
			లెక్క++;
		    లేకపోతే ఒకవేళ (STRNCMP(ptr, "el", 2) == 0)
		    {
			ఒకవేళ (లెక్క == 0)
			    తిరిగి &pos;
		    }
		    లేకపోతే ఒకవేళ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ఒకవేళ (లెక్క == 0)
			    తిరిగి &pos;
			లెక్క--;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (STRNCMP(ptr, "if", 2) == 0)
		    {
			ఒకవేళ (లెక్క == 0)
			    తిరిగి &pos;
			లెక్క--;
		    }
		    లేకపోతే ఒకవేళ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ఒకవేళ (లెక్క == 0)
			    తిరిగి &pos;
		    }
		    లేకపోతే ఒకవేళ (STRNCMP(ptr, "endif", 5) == 0)
			లెక్క++;
		}
	    }
	    తిరిగి ఖాళీ;
	}
    }

#నిర్వచితమైతే FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ఒకవేళ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ఖాళీ)
	backwards = !backwards;
#అయితే_ముగింపు

    do_quotes = -1;
    ప్రారంభం_లోపల_quotes = MAYBE;
    CLEAR_POS(&సరిపోలిక_pos);

    // backward search: Check if this line contains a single-line comment
    ఒకవేళ ((backwards && comment_dir) || lisp || skip_comments)
	comment_నిలువు = check_linecomment(linep);
    ఒకవేళ (lisp && comment_నిలువు != MAXCOL && pos.నిలువు > (colnr_T)comment_నిలువు)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ఒకవేళ (skip_comments && !లోపల_ఖండం_comment && comment_నిలువు != MAXCOL
	    && backwards && pos.నిలువు > (colnr_T)comment_నిలువు)
	pos.నిలువు = comment_నిలువు;

    అయితే (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ఒకవేళ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ఒకవేళ (lispcomm && pos.నిలువు < (colnr_T)comment_నిలువు)
		ఆపు;
	    ఒకవేళ (pos.నిలువు == 0)		// at start of line, go to prev. one
	    {
		ఒకవేళ (pos.lnum == 1)	// start of file
		    ఆపు;
		--pos.lnum;

		ఒకవేళ (maxtravel > 0 && ++traveled > maxtravel)
		    ఆపు;

		linep = ml_get(pos.lnum);
		pos.నిలువు = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		పంక్తి_breakcheck();

		// Check if this line contains a single-line comment
		ఒకవేళ (comment_dir || lisp || skip_comments)
		    comment_నిలువు = check_linecomment(linep);
		// skip comment
		ఒకవేళ (lisp && comment_నిలువు != MAXCOL)
		    pos.నిలువు = comment_నిలువు;
		లేకపోతే ఒకవేళ (skip_comments && !లోపల_ఖండం_comment
			&& comment_నిలువు != MAXCOL
			&& pos.నిలువు > (colnr_T)comment_నిలువు)
		    pos.నిలువు = comment_నిలువు;
	    }
	    లేకపోతే
	    {
		--pos.నిలువు;
		ఒకవేళ (has_mbyte)
		    pos.నిలువు -= (*mb_తల_off)(linep, linep + pos.నిలువు);
	    }
	}
	లేకపోతే				// forward search
	{
	    ఒకవేళ (linep[pos.నిలువు] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_నిలువు != MAXCOL
					   && pos.నిలువు == (colnr_T)comment_నిలువు))
	    {
		ఒకవేళ (pos.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    ఆపు;
		++pos.lnum;

		ఒకవేళ (maxtravel && traveled++ > maxtravel)
		    ఆపు;

		linep = ml_get(pos.lnum);
		pos.నిలువు = 0;
		do_quotes = -1;
		పంక్తి_breakcheck();
		ఒకవేళ (lisp || skip_comments)   // find comment pos in new line
		    comment_నిలువు = check_linecomment(linep);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (has_mbyte)
		    pos.నిలువు += (*mb_ptr2len)(linep + pos.నిలువు);
		లేకపోతే
		    ++pos.నిలువు;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ఒకవేళ (skip_comments && !comment_dir && !inquote)
	{
	    ఒకవేళ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ఒకవేళ (!లోపల_ఖండం_comment && pos.నిలువు > 0
			&& linep[pos.నిలువు - 1] == '*' && linep[pos.నిలువు] == '/'
			&& (comment_నిలువు == MAXCOL || (పూర్ణాంకం)pos.నిలువు < comment_నిలువు))
		    లోపల_ఖండం_comment = TRUE;
		లేకపోతే ఒకవేళ (లోపల_ఖండం_comment && pos.నిలువు > 0
			&& linep[pos.నిలువు - 1] == '/' && linep[pos.నిలువు] == '*')
		    లోపల_ఖండం_comment = FALSE;
	    }
	    లేకపోతే
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ఒకవేళ (!లోపల_ఖండం_comment && linep[pos.నిలువు] == '/'
			&& linep[pos.నిలువు + 1] == '*'
			&& (comment_నిలువు == MAXCOL || (పూర్ణాంకం)pos.నిలువు < comment_నిలువు))
		    లోపల_ఖండం_comment = TRUE;
		లేకపోతే ఒకవేళ (లోపల_ఖండం_comment && pos.నిలువు > 0
			&& linep[pos.నిలువు - 1] == '*' && linep[pos.నిలువు] == '/')
		    లోపల_ఖండం_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ఒకవేళ (pos.నిలువు == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ఒకవేళ (linep[0] == findc && లెక్క == 0)	// match!
		తిరిగి &pos;
	    ఆపు;					// out of scope
	}

	ఒకవేళ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ఒకవేళ (comment_dir == FORWARD)
	    {
		ఒకవేళ (linep[pos.నిలువు] == '*' && linep[pos.నిలువు + 1] == '/')
		{
		    pos.నిలువు++;
		    తిరిగి &pos;
		}
	    }
	    లేకపోతే    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ఒకవేళ (pos.నిలువు == 0)
		    కొనసాగు;
		లేకపోతే ఒకవేళ (raw_string)
		{
		    ఒకవేళ (linep[pos.నిలువు - 1] == 'R'
			&& linep[pos.నిలువు] == '"'
			&& vim_strchr(linep + pos.నిలువు + 1, '(') != ఖాళీ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ఒకవేళ (!కనుగొను_rawstring_చివర(linep, &pos,
				  లెక్క > 0 ? &సరిపోలిక_pos : &curwin->w_cursor))
			{
			    లెక్క++;
			    సరిపోలిక_pos = pos;
			    సరిపోలిక_pos.నిలువు--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		లేకపోతే ఒకవేళ (  linep[pos.నిలువు - 1] == '/'
			&& linep[pos.నిలువు] == '*'
			&& (pos.నిలువు == 1 || linep[pos.నిలువు - 2] != '*')
			&& (పూర్ణాంకం)pos.నిలువు < comment_నిలువు)
		{
		    లెక్క++;
		    సరిపోలిక_pos = pos;
		    సరిపోలిక_pos.నిలువు--;
		}
		లేకపోతే ఒకవేళ (linep[pos.నిలువు - 1] == '*' && linep[pos.నిలువు] == '/')
		{
		    ఒకవేళ (లెక్క > 0)
			pos = సరిపోలిక_pos;
		    లేకపోతే ఒకవేళ (pos.నిలువు > 1 && linep[pos.నిలువు - 2] == '/'
					       && (పూర్ణాంకం)pos.నిలువు <= comment_నిలువు)
			pos.నిలువు -= 2;
		    లేకపోతే ఒకవేళ (ignore_cend)
			కొనసాగు;
		    లేకపోతే
			తిరిగి ఖాళీ;
		    తిరిగి &pos;
		}
	    }
	    కొనసాగు;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ఒకవేళ (cpo_సరిపోలిక)
	    do_quotes = 0;
	లేకపోతే ఒకవేళ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_ప్రారంభం = do_quotes;
	    కొరకు (ptr = linep; *ptr; ++ptr)
	    {
		ఒకవేళ (ptr == linep + pos.నిలువు + backwards)
		    at_ప్రారంభం = (do_quotes & 1);
		ఒకవేళ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ఒకవేళ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ఒకవేళ (!do_quotes)
	    {
		inquote = FALSE;
		ఒకవేళ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ఒకవేళ (ప్రారంభం_లోపల_quotes == MAYBE)
		    {
			inquote = at_ప్రారంభం;
			ఒకవేళ (inquote)
			    ప్రారంభం_లోపల_quotes = TRUE;
		    }
		    లేకపోతే ఒకవేళ (backwards)
			inquote = TRUE;
		}
		ఒకవేళ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ఒకవేళ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ఒకవేళ (ప్రారంభం_లోపల_quotes == MAYBE)
			{
			    inquote = at_ప్రారంభం;
			    ఒకవేళ (inquote)
				ప్రారంభం_లోపల_quotes = TRUE;
			}
			లేకపోతే ఒకవేళ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ఒకవేళ (ప్రారంభం_లోపల_quotes == MAYBE)
	    ప్రారంభం_లోపల_quotes = FALSE;

	/*
	 * If 'smartmatch' is set:
	 *   Things inside quotes are ignored by setting 'inquote'.  If we
	 *   find a quote without a preceding '\' invert 'inquote'.  At the
	 *   end of a line not ending in '\' we reset 'inquote'.
	 *
	 *   In lines with an uneven number of quotes (without preceding '\')
	 *   we do not know which part to ignore. Therefore we only set
	 *   inquote if the number of quotes in a line is even, unless this
	 *   line or the previous one ends in a '\'.  Complicated, isn't it?
	 */
	c = PTR2CHAR(linep + pos.నిలువు);
	ఎంపిక (c)
	{
	స్థితి NUL:
	    // at end of line without trailing backslash, reset inquote
	    ఒకవేళ (pos.నిలువు == 0 || linep[pos.నిలువు - 1] != '\\')
	    {
		inquote = FALSE;
		ప్రారంభం_లోపల_quotes = FALSE;
	    }
	    ఆపు;

	స్థితి '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ఒకవేళ (do_quotes)
	    {
		పూర్ణాంకం నిలువు;

		కొరకు (నిలువు = pos.నిలువు - 1; నిలువు >= 0; --నిలువు)
		    ఒకవేళ (linep[నిలువు] != '\\')
			ఆపు;
		ఒకవేళ ((((పూర్ణాంకం)pos.నిలువు - 1 - నిలువు) & 1) == 0)
		{
		    inquote = !inquote;
		    ప్రారంభం_లోపల_quotes = FALSE;
		}
	    }
	    ఆపు;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	స్థితి '\'':
	    ఒకవేళ (!cpo_సరిపోలిక && initc != '\'' && findc != '\'')
	    {
		ఒకవేళ (backwards)
		{
		    ఒకవేళ (pos.నిలువు > 1)
		    {
			ఒకవేళ (linep[pos.నిలువు - 2] == '\'')
			{
			    pos.నిలువు -= 2;
			    ఆపు;
			}
			లేకపోతే ఒకవేళ (linep[pos.నిలువు - 2] == '\\'
				  && pos.నిలువు > 2 && linep[pos.నిలువు - 3] == '\'')
			{
			    pos.నిలువు -= 3;
			    ఆపు;
			}
		    }
		}
		లేకపోతే ఒకవేళ (linep[pos.నిలువు + 1])	// forward search
		{
		    ఒకవేళ (linep[pos.నిలువు + 1] == '\\'
			   && linep[pos.నిలువు + 2] && linep[pos.నిలువు + 3] == '\'')
		    {
			pos.నిలువు += 3;
			ఆపు;
		    }
		    లేకపోతే ఒకవేళ (linep[pos.నిలువు + 2] == '\'')
		    {
			pos.నిలువు += 2;
			ఆపు;
		    }
		}
	    }
	    // FALLTHROUGH

	అప్రమేయ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ఒకవేళ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ఖాళీ
		    && pos.నిలువు > 1
		    && check_prevcol(linep, pos.నిలువు, '\\', ఖాళీ)
		    && check_prevcol(linep, pos.నిలువు - 1, '#', ఖాళీ))
		ఆపు;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ఒకవేళ (skip_comments && (లోపల_ఖండం_comment
		    || (comment_నిలువు != MAXCOL && (పూర్ణాంకం)pos.నిలువు >= comment_నిలువు)))
		ఆపు;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ఒకవేళ ((!inquote || ప్రారంభం_లోపల_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		పూర్ణాంకం	నిలువు, bslcnt = 0;

		ఒకవేళ (!cpo_bsl)
		{
		    కొరకు (నిలువు = pos.నిలువు; check_prevcol(linep, నిలువు, '\\', &నిలువు);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ఒకవేళ (cpo_bsl || (bslcnt & 1) == సరిపోలిక_escaped)
		{
		    ఒకవేళ (c == initc)
			లెక్క++;
		    లేకపోతే
		    {
			ఒకవేళ (లెక్క == 0)
			    తిరిగి &pos;
			లెక్క--;
		    }
		}
	    }
	}
    }

    ఒకవేళ (comment_dir == BACKWARD && లెక్క > 0)
    {
	pos = సరిపోలిక_pos;
	తిరిగి &pos;
    }
    తిరిగి (pos_T *)ఖాళీ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    శూన్య
showmatch(
    పూర్ణాంకం		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    దీర్ఘ	save_so;
    దీర్ఘ	save_siso;
#నిర్వచితమైతే CURSOR_SHAPE
    పూర్ణాంకం		save_state;
#అయితే_ముగింపు
    colnr_T	save_dollar_vcol;
    char_u	*p;
    దీర్ఘ	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    దీర్ఘ	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    కొరకు (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    ఆపు;
#అయితే_ముగింపు
	p += mb_ptr2len(p) + 1;
	ఒకవేళ (PTR2CHAR(p) == c
#నిర్వచితమైతే FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#అయితే_ముగింపు
	   )
	    ఆపు;
	p += mb_ptr2len(p);
	ఒకవేళ (*p == NUL)
	    తిరిగి;
    }
    ఒకవేళ (*p == NUL)
	తిరిగి;

    ఒకవేళ ((lpos = findmatch(ఖాళీ, NUL)) == ఖాళీ)	    // no match, so beep
    {
	vim_beep(BO_సరిపోలిక);
	తిరిగి;
    }

    ఒకవేళ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	తిరిగి;

    ఒకవేళ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ఖాళీ, &vcol, ఖాళీ, 0);

    పూర్ణాంకం నిలువు_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_వెడల్పు));
    ఒకవేళ (!నిలువు_visible)
	తిరిగి;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ఒకవేళ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
	dollar_vcol = -1;
    ++curwin->w_virtcol;	// do display ')' just before "$"
    update_screen(UPD_VALID);	// show the new char first

    save_dollar_vcol = dollar_vcol;
#నిర్వచితమైతే CURSOR_SHAPE
    save_state = State;
    State = బహుళకం_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    బహిర్గతం_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    ఒకవేళ (vim_strchr(p_cpo, CPO_SHOWMATCH) != ఖాళీ)
	ui_delay(p_mat * 100L + 8, TRUE);
    లేకపోతే ఒకవేళ (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#నిర్వచితమైతే CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    స్థిర పూర్ణాంకం
is_zero_వెడల్పు(
    char_u	*నమూనా,
    size_t	patternlen,
    పూర్ణాంకం		move,
    pos_T	*cur,
    పూర్ణాంకం		direction)
{
    regmmatch_T	regmatch;
    పూర్ణాంకం		nmatched = 0;
    పూర్ణాంకం		ఫలితం = -1;
    pos_T	pos;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    పూర్ణాంకం		flag = 0;

    ఒకవేళ (నమూనా == ఖాళీ)
    {
	నమూనా = spats[చివరి_idx].pat;
	patternlen = spats[చివరి_idx].patlen;
    }

    ఒకవేళ (శోధన_regcomp(నమూనా, patternlen, ఖాళీ, RE_శోధన, RE_శోధన,
					      శోధన_KEEP, &regmatch) == FAIL)
	తిరిగి -1;

    // init startcol correctly
    regmatch.startpos[0].నిలువు = -1;
    // move to match
    ఒకవేళ (move)
    {
	CLEAR_POS(&pos);
    }
    లేకపోతే
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = శోధన_ప్రారంభం;
    }

    ఒకవేళ (searchit(curwin, curbuf, &pos, ఖాళీ, direction, నమూనా, patternlen, 1,
				  శోధన_KEEP + flag, RE_శోధన, ఖాళీ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	చేయి
	{
	    regmatch.startpos[0].నిలువు++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].నిలువు, ఖాళీ);
	    ఒకవేళ (nmatched != 0)
		ఆపు;
	} అయితే (regmatch.regprog != ఖాళీ
		&& (direction == FORWARD ? regmatch.startpos[0].నిలువు < pos.నిలువు
				      : regmatch.startpos[0].నిలువు > pos.నిలువు));

	ఒకవేళ (called_emsg == called_emsg_before)
	{
	    ఫలితం = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].నిలువు == regmatch.endpos[0].నిలువు);
	}
    }

    vim_regfree(regmatch.regprog);
    తిరిగి ఫలితం;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    పూర్ణాంకం
current_శోధన(
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	ప్రారంభం_pos;	// start position of the pattern match
    pos_T	చివర_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    పూర్ణాంకం		i;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		ఫలితం;		// result of various function calls
    char_u	old_p_ws = p_ws;
    పూర్ణాంకం		flags = 0;
    pos_T	save_VIsual = VIsual;
    పూర్ణాంకం		zero_వెడల్పు;
    పూర్ణాంకం		skip_మొదటి_backward;

    // Correct cursor when 'selection' is exclusive
    ఒకవేళ (VIsual_చురుకైన && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_మొదటి_backward = forward && VIsual_చురుకైన
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (forward)
	    incl(&pos);
	లేకపోతే
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_వెడల్పు = is_zero_వెడల్పు(spats[చివరి_idx].pat, spats[చివరి_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ఒకవేళ (zero_వెడల్పు == -1)
	తిరిగి FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    కొరకు (i = 0; i < 2; i++)
    {
	ఒకవేళ (forward)
	{
	    ఒకవేళ (i == 0 && skip_మొదటి_backward)
		కొనసాగు;
	    dir = i;
	}
	లేకపోతే
	    dir = !i;

	flags = 0;
	ఒకవేళ (!dir && !zero_వెడల్పు)
	    flags = శోధన_చివర;
	చివర_pos = pos;

	// wrapping should not occur in the first round
	ఒకవేళ (i == 0)
	    p_ws = FALSE;

	ఫలితం = searchit(curwin, curbuf, &pos, &చివర_pos,
		(dir ? FORWARD : BACKWARD),
		spats[చివరి_idx].pat, spats[చివరి_idx].patlen, (దీర్ఘ) (i ? లెక్క : 1),
		శోధన_KEEP | flags, RE_శోధన, ఖాళీ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	ఒకవేళ (i == 1 && !ఫలితం) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ఒకవేళ (VIsual_చురుకైన)
		VIsual = save_VIsual;
	    తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (i == 0 && !ఫలితం)
	{
	    ఒకవేళ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    లేకపోతే
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
		pos.నిలువు  = ml_get_len(curwin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క);
	    }
	}
    }

    ప్రారంభం_pos = pos;

    ఒకవేళ (!VIsual_చురుకైన)
	VIsual = ప్రారంభం_pos;

    // put the cursor after the match
    curwin->w_cursor = చివర_pos;
    ఒకవేళ (LT_POS(VIsual, చివర_pos) && forward)
    {
	ఒకవేళ (skip_మొదటి_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	లేకపోతే
	    // put the cursor on last character of match
	    dec_cursor();
    }
    లేకపోతే ఒకవేళ (VIsual_చురుకైన && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_చురుకైన = TRUE;
    VIsual_బహుళకం = 'v';

    ఒకవేళ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ఒకవేళ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	లేకపోతే ఒకవేళ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (fdo_flags & FDO_శోధన && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు

    may_ప్రారంభం_select('c');
    setmouse();
#నిర్వచితమైతే FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_నక్షత్రం.vmode = NUL;
#అయితే_ముగింపు
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    తిరిగి OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    పూర్ణాంకం
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    తిరిగి (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    స్థిర శూన్య
cmdline_శోధన_stat(
    పూర్ణాంకం		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    పూర్ణాంకం		show_శిఖరం_bot_సందేశం,
    char_u	*msgbuf,
    size_t	msgbuflen,
    పూర్ణాంకం		recompute,
    పూర్ణాంకం		maxcount,
    దీర్ఘ	timeout)
{
    searchstat_T stat;

    update_శోధన_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ఒకవేళ (stat.cur <= 0)
	తిరిగి;

    అక్షరం	t[శోధన_STAT_BUF_LEN];
    size_t	len;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ఒకవేళ (stat.incomplete == 1)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[?/??]");
	లేకపోతే ఒకవేళ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	లేకపోతే ఒకవేళ (stat.cnt > maxcount)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	లేకపోతే
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	ఒకవేళ (stat.incomplete == 1)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[?/??]");
	లేకపోతే ఒకవేళ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	లేకపోతే ఒకవేళ (stat.cnt > maxcount)
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	లేకపోతే
	    len = vim_snprintf(t, శోధన_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ఒకవేళ (show_శిఖరం_bot_సందేశం && len + 2 < శోధన_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ఒకవేళ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ఒకవేళ (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    సందేశం_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    సందేశం_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    స్థిర శూన్య
update_శోధన_stat(
    పూర్ణాంకం			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    పూర్ణాంకం			recompute,
    పూర్ణాంకం			maxcount,
    దీర్ఘ		timeout వాడని)
{
    పూర్ణాంకం		    save_ws = p_ws;
    పూర్ణాంకం		    wraparound = FALSE;
    pos_T	    p = (*pos);
    స్థిర pos_T    lastpos = {0, 0, 0};
    స్థిర పూర్ణాంకం	    cur = 0;
    స్థిర పూర్ణాంకం	    cnt = 0;
    స్థిర పూర్ణాంకం	    exact_సరిపోలిక = FALSE;
    స్థిర పూర్ణాంకం	    incomplete = 0;
    స్థిర పూర్ణాంకం	    చివరి_maxcount = 0;
    స్థిర పూర్ణాంకం	    chgtick = 0;
    స్థిర char_u   *lastpat = ఖాళీ;
    స్థిర size_t   lastpatlen = 0;
    స్థిర buf_T    *lbuf = ఖాళీ;
#నిర్వచితమైతే FEAT_RELTIME
    proftime_T  ప్రారంభం;
#అయితే_ముగింపు

    CLEAR_సూచిక(stat);

    ఒకవేళ (dirc == 0 && !recompute && !ఖాళీలు_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_సరిపోలిక = exact_సరిపోలిక;
	stat->incomplete = incomplete;
	stat->చివరి_maxcount = చివరి_maxcount;
	తిరిగి;
    }
    చివరి_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ఒకవేళ (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ఖాళీ
	    && STRNCMP(lastpat, spats[చివరి_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[చివరి_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_సరిపోలిక = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    ఒకవేళ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    లేకపోతే
    {
	పూర్ణాంకం	పూర్తి_శోధన = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#నిర్వచితమైతే FEAT_RELTIME
	ఒకవేళ (timeout > 0)
	    profile_setlimit(timeout, &ప్రారంభం);
#అయితే_ముగింపు
	అయితే (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ఖాళీ, 0, 1, శోధన_KEEP, RE_చివరి, ఖాళీ) != FAIL)
	{
	    పూర్తి_శోధన = TRUE;
#నిర్వచితమైతే FEAT_RELTIME
	    // Stop after passing the time limit.
	    ఒకవేళ (timeout > 0 && profile_passed_హద్దు(&ప్రారంభం))
	    {
		incomplete = 1;
		ఆపు;
	    }
#అయితే_ముగింపు
	    cnt++;
	    ఒకవేళ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ఒకవేళ (LT_POS(p, endpos))
		    exact_సరిపోలిక = TRUE;
	    }
	    fast_breakcheck();
	    ఒకవేళ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		ఆపు;
	    }
	}
	ఒకవేళ (got_int)
	    cur = -1; // abort
	ఒకవేళ (పూర్తి_శోధన)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[చివరి_idx].pat, spats[చివరి_idx].patlen);
	    ఒకవేళ (lastpat == ఖాళీ)
		lastpatlen = 0;
	    లేకపోతే
		lastpatlen = spats[చివరి_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_సరిపోలిక = exact_సరిపోలిక;
    stat->incomplete = incomplete;
    stat->చివరి_maxcount = చివరి_maxcount;
    p_ws = save_ws;
}

#ఒకవేళ defined(FEAT_కనుగొను_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    స్థిర char_u *
get_పంక్తి_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *పంక్తి = ml_get(lnum);

    vim_strncpy(buf, పంక్తి, LSIZE - 1);
    తిరిగి buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    శూన్య
కనుగొను_నమూనా_లోపల_మార్గం(
    char_u	*ptr,		// pointer to search pattern
    పూర్ణాంకం		dir వాడని,	// direction of expansion
    పూర్ణాంకం		len,		// length of search pattern
    పూర్ణాంకం		సంపూర్ణ,		// match whole words only
    పూర్ణాంకం		skip_comments,	// don't match inside comments
    పూర్ణాంకం		type,		// Type of search; are we looking for a type?
				// a macro?
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		action,		// What to do when we find it
    linenr_T	ప్రారంభం_lnum,	// first line to start searching
    linenr_T	చివర_lnum,	// last line for searching
    పూర్ణాంకం		forceit,	// If true, always switch to the found path
    పూర్ణాంకం		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    పూర్ణాంకం		max_మార్గం_లోతు = 50;
    దీర్ఘ	సరిపోలిక_లెక్క = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ఖాళీ;
    linenr_T	lnum;
    పూర్ణాంకం		లోతు;
    పూర్ణాంకం		లోతు_displayed;	// For type==CHECK_PATH
    పూర్ణాంకం		old_files;
    పూర్ణాంకం		already_searched;
    char_u	*file_పంక్తి;
    char_u	*పంక్తి;
    char_u	*p;
    char_u	save_char;
    పూర్ణాంకం		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    పూర్ణాంకం		matched = FALSE;
    పూర్ణాంకం		did_show = FALSE;
    పూర్ణాంకం		దొరికింది = FALSE;
    పూర్ణాంకం		i;
    char_u	*already = ఖాళీ;
    char_u	*startp = ఖాళీ;
    char_u	*inc_opt = ఖాళీ;
# ఒకవేళ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ఖాళీ;
# endif

    regmatch.regprog = ఖాళీ;
    incl_regmatch.regprog = ఖాళీ;
    def_regmatch.regprog = ఖాళీ;

    file_పంక్తి = alloc(LSIZE);
    ఒకవేళ (file_పంక్తి == ఖాళీ)
	తిరిగి;

    ఒకవేళ (type != CHECK_మార్గం && type != కనుగొను_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_స్థితి_sol())
    {
	pat = alloc(len + 5);
	ఒకవేళ (pat == ఖాళీ)
	    వెళ్ళు fpip_చివర;
	vim_snprintf((అక్షరం *)pat, len + 5, సంపూర్ణ ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	    వెళ్ళు fpip_చివర;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ఒకవేళ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (incl_regmatch.regprog == ఖాళీ)
	    వెళ్ళు fpip_చివర;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ఒకవేళ (type == కనుగొను_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (def_regmatch.regprog == ఖాళీ)
	    వెళ్ళు fpip_చివర;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_మార్గం_లోతు * పరిమాణం(SearchedFile), TRUE);
    ఒకవేళ (files == ఖాళీ)
	వెళ్ళు fpip_చివర;
    old_files = max_మార్గం_లోతు;
    లోతు = లోతు_displayed = -1;

    lnum = ప్రారంభం_lnum;
    ఒకవేళ (చివర_lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	చివర_lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (lnum > చివర_lnum)		// do at least one line
	lnum = చివర_lnum;
    పంక్తి = get_పంక్తి_and_copy(lnum, file_పంక్తి);

    కొరకు (;;)
    {
	ఒకవేళ (incl_regmatch.regprog != ఖాళీ
		&& vim_regexec(&incl_regmatch, పంక్తి, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)inc_opt, "\\zs") != ఖాళీ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = కనుగొను_file_name_లోపల_మార్గం(incl_regmatch.startp[0],
		       (పూర్ణాంకం)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    లేకపోతే
		// Use text after match with 'include'.
		new_fname = file_name_లోపల_పంక్తి(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ఖాళీ);
	    already_searched = FALSE;
	    ఒకవేళ (new_fname != ఖాళీ)
	    {
		// Check whether we have already searched in this file
		కొరకు (i = 0;; i++)
		{
		    ఒకవేళ (i == లోతు + 1)
			i = old_files;
		    ఒకవేళ (i == max_మార్గం_లోతు)
			ఆపు;
		    ఒకవేళ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ఒకవేళ (type != CHECK_మార్గం
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    సందేశం_putchar('\n');	    // cursor below last one
			    ఒకవేళ (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //సందేశం
			    {
				సందేశం_home_replace_hl(new_fname);
				సందేశం_puts(_(" (includes previously listed match)"));
				prev_fname = ఖాళీ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			ఆపు;
		    }
		}
	    }

	    ఒకవేళ (type == CHECK_మార్గం && (action == ACTION_SHOW_ALL
				 || (new_fname == ఖాళీ && !already_searched)))
	    {
		ఒకవేళ (did_show)
		    సందేశం_putchar('\n');	    // cursor below last one
		లేకపోతే
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    సందేశం_puts_title(_("--- Included files "));
		    ఒకవేళ (action != ACTION_SHOW_ALL)
			సందేశం_puts_title(_("not found "));
		    సందేశం_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		అయితే (లోతు_displayed < లోతు && !got_int)
		{
		    ++లోతు_displayed;
		    కొరకు (i = 0; i < లోతు_displayed; i++)
			సందేశం_puts("  ");
		    సందేశం_home_replace(files[లోతు_displayed].name);
		    సందేశం_puts(" -->\n");
		}
		ఒకవేళ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    కొరకు (i = 0; i <= లోతు_displayed; i++)
			సందేశం_puts("  ");
		    ఒకవేళ (new_fname != ఖాళీ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			సందేశం_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    లేకపోతే
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)inc_opt, "\\zs") != ఖాళీ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (పూర్ణాంకం)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			లేకపోతే
			{
			    // find the file name after the end of the match
			    కొరకు (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    కొరకు (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ఒకవేళ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (పూర్ణాంకం)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			లేకపోతే ఒకవేళ (p > పంక్తి)
			{
			    ఒకవేళ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ఒకవేళ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			సందేశం_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ఒకవేళ (new_fname == ఖాళీ && action == ACTION_SHOW_ALL)
		    {
			ఒకవేళ (already_searched)
			    సందేశం_puts(_("  (Already listed)"));
			లేకపోతే
			    సందేశం_puts(_("  NOT FOUND"));
		    }
		}
		బహిర్గతం_flush();	    // output each line directly
	    }

	    ఒకవేళ (new_fname != ఖాళీ)
	    {
		// Push the new file onto the file stack
		ఒకవేళ (లోతు + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_మార్గం_లోతు * 2);
		    ఒకవేళ (bigger != ఖాళీ)
		    {
			కొరకు (i = 0; i <= లోతు; i++)
			    bigger[i] = files[i];
			కొరకు (i = లోతు + 1; i < old_files + max_మార్గం_లోతు; i++)
			{
			    bigger[i].fp = ఖాళీ;
			    bigger[i].name = ఖాళీ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			కొరకు (i = old_files; i < max_మార్గం_లోతు; i++)
			    bigger[i + max_మార్గం_లోతు] = files[i];
			old_files += max_మార్గం_లోతు;
			max_మార్గం_లోతు *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ఒకవేళ ((files[లోతు + 1].fp = mch_fopen((అక్షరం *)new_fname, "r"))
								    == ఖాళీ)
		    vim_free(new_fname);
		లేకపోతే
		{
		    ఒకవేళ (++లోతు == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[లోతు].name = curr_fname = new_fname;
		    files[లోతు].lnum = 0;
		    files[లోతు].matched = FALSE;
		    ఒకవేళ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			సందేశం_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((అక్షరం*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(అక్షరం *)new_fname);
			సందేశం_trunc_attr((అక్షరం *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    లేకపోతే ఒకవేళ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (అక్షరం *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	లేకపోతే
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = పంక్తి;
శోధన_పంక్తి:
	    define_matched = FALSE;
	    ఒకవేళ (def_regmatch.regprog != ఖాళీ
			      && vim_regexec(&def_regmatch, పంక్తి, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		అయితే (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ఒకవేళ (def_regmatch.regprog == ఖాళీ || define_matched)
	    {
		ఒకవేళ (define_matched || compl_స్థితి_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ఒకవేళ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    లేకపోతే
			matched = STRNCMP(startp, ptr, len) == 0;
		    ఒకవేళ (matched && define_matched && సంపూర్ణ
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		లేకపోతే ఒకవేళ (regmatch.regprog != ఖాళీ
			 && vim_regexec(&regmatch, పంక్తి, (colnr_T)(p - పంక్తి)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ఒకవేళ (!define_matched && skip_comments)
		    {
			ఒకవేళ ((*పంక్తి != '#' ||
				STRNCMP(skipwhite(పంక్తి + 1), "define", 6) != 0)
				&& get_leader_len(పంక్తి, ఖాళీ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(పంక్తి);
			ఒకవేళ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    కొరకు (p = పంక్తి; *p && p < startp; ++p)
			    {
				ఒకవేళ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ఒకవేళ (p[1] == '/')
					ఆపు;
				    ++p;
				}
				లేకపోతే ఒకవేళ (!matched && p[0] == '*' && p[1] == '/')
				{
				    // Can find match after "* /".
				    matched = TRUE;
				    ++p;
				}
			    }
		    }
		}
	    }
	}
	ఒకవేళ (matched)
	{
	    ఒకవేళ (action == ACTION_EXPAND)
	    {
		పూర్ణాంకం	cont_s_ipos = FALSE;
		పూర్ణాంకం	కూడు_r;
		char_u	*aux;

		ఒకవేళ (లోతు == -1 && lnum == curwin->w_cursor.lnum)
		    ఆపు;
		దొరికింది = TRUE;
		aux = p = startp;
		ఒకవేళ (compl_స్థితి_adding() && (పూర్ణాంకం)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ఒకవేళ (vim_iswordp(p))
			వెళ్ళు exit_matched;
		    p = కనుగొను_మాట_ప్రారంభం(p);
		}
		p = కనుగొను_మాట_చివర(p);
		i = (పూర్ణాంకం)(p - aux);

		ఒకవేళ (compl_స్థితి_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ఒకవేళ (లోతు < 0)
		    {
			ఒకవేళ (lnum >= చివర_lnum)
			    వెళ్ళు exit_matched;
			పంక్తి = get_పంక్తి_and_copy(++lnum, file_పంక్తి);
		    }
		    లేకపోతే ఒకవేళ (vim_fgets(పంక్తి = file_పంక్తి,
						      LSIZE, files[లోతు].fp))
			వెళ్ళు exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(పంక్తి);
		    p = కనుగొను_మాట_ప్రారంభం(p);
		    p = కనుగొను_మాట_చివర(p);
		    ఒకవేళ (p > aux)
		    {
			ఒకవేళ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ఒకవేళ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ఒకవేళ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ఖాళీ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ఒకవేళ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (పూర్ణాంకం)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ఒకవేళ (i == ins_compl_len())
			వెళ్ళు exit_matched;
		}

		కూడు_r = ins_compl_కూడు_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ఖాళీ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ఒకవేళ (కూడు_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		లేకపోతే ఒకవేళ (కూడు_r == FAIL)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (action == ACTION_SHOW_ALL)
	    {
		దొరికింది = TRUE;
		ఒకవేళ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ఒకవేళ (curr_fname != prev_fname)
		{
		    ఒకవేళ (did_show)
			సందేశం_putchar('\n');	// cursor below last one
		    ఒకవేళ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			సందేశం_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ఒకవేళ (!got_int)
		    show_pat_లోపల_మార్గం(పంక్తి, type, TRUE, action,
			    (లోతు == -1) ? ఖాళీ : files[లోతు].fp,
			    (లోతు == -1) ? &lnum : &files[లోతు].lnum,
			    సరిపోలిక_లెక్క++);

		// Set matched flag for this file and all the ones that
		// include it
		కొరకు (i = 0; i <= లోతు; ++i)
		    files[i].matched = TRUE;
	    }
	    లేకపోతే ఒకవేళ (--లెక్క <= 0)
	    {
		దొరికింది = TRUE;
		ఒకవేళ (లోతు == -1 && lnum == curwin->w_cursor.lnum
# ఒకవేళ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_సరిపోలిక_is_on_current_పంక్తి));
		లేకపోతే ఒకవేళ (action == ACTION_SHOW)
		{
		    show_pat_లోపల_మార్గం(పంక్తి, type, did_show, action,
			    (లోతు == -1) ? ఖాళీ : files[లోతు].fp,
			    (లోతు == -1) ? &lnum : &files[లోతు].lnum, 1L);
		    did_show = TRUE;
		}
		లేకపోతే
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ఒకవేళ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ఒకవేళ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ఒకవేళ (action == ACTION_విభజించు)
		    {
			ఒకవేళ (win_విభజించు(0, 0) == FAIL)
			    ఆపు;
			RESET_BINDING(curwin);
		    }
		    ఒకవేళ (లోతు == -1)
		    {
			// match in current file
# ఒకవేళ defined(FEAT_QUICKFIX)
			ఒకవేళ (g_do_tagpreview != 0)
			{
			    ఒకవేళ (!win_valid(curwin_save))
				ఆపు;
			    ఒకవేళ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_బఫరు->b_fnum, ఖాళీ,
						     ఖాళీ, TRUE, lnum, forceit)))
				ఆపు;	// failed to jump to file
			}
			లేకపోతే
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    లేకపోతే
		    {
			ఒకవేళ (!GETFILE_SUCCESS(getfile(
					0, files[లోతు].name, ఖాళీ, TRUE,
						    files[లోతు].lnum, forceit)))
			    ఆపు;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[లోతు].lnum;
		    }
		}
		ఒకవేళ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.నిలువు = (colnr_T)(startp - పంక్తి);
		    curwin->w_సమితి_curswant = true;
		}

# ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		లేకపోతే ఒకవేళ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		ఆపు;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ఒకవేళ (def_regmatch.regprog == ఖాళీ
		    && action == ACTION_EXPAND
		    && !compl_స్థితి_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		వెళ్ళు శోధన_పంక్తి;
	}
	పంక్తి_breakcheck();
	ఒకవేళ (action == ACTION_EXPAND)
	    ins_compl_check_కీలకాలు(30, FALSE);
	ఒకవేళ (got_int || ins_compl_interrupted())
	    ఆపు;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	అయితే (లోతు >= 0 && !already
		&& vim_fgets(పంక్తి = file_పంక్తి, LSIZE, files[లోతు].fp))
	{
	    fclose(files[లోతు].fp);
	    --old_files;
	    files[old_files].name = files[లోతు].name;
	    files[old_files].matched = files[లోతు].matched;
	    --లోతు;
	    curr_fname = (లోతు == -1) ? curbuf->b_fname
				       : files[లోతు].name;
	    ఒకవేళ (లోతు < లోతు_displayed)
		లోతు_displayed = లోతు;
	}
	ఒకవేళ (లోతు >= 0)		// we could read the line
	{
	    files[లోతు].lnum++;
	    // Remove any CR and LF from the line.
	    i = (పూర్ణాంకం)STRLEN(పంక్తి);
	    ఒకవేళ (i > 0 && పంక్తి[i - 1] == '\n')
		పంక్తి[--i] = NUL;
	    ఒకవేళ (i > 0 && పంక్తి[i - 1] == '\r')
		పంక్తి[--i] = NUL;
	}
	లేకపోతే ఒకవేళ (!already)
	{
	    ఒకవేళ (++lnum > చివర_lnum)
		ఆపు;
	    పంక్తి = get_పంక్తి_and_copy(lnum, file_పంక్తి);
	}
	already = ఖాళీ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    కొరకు (i = 0; i <= లోతు; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    కొరకు (i = old_files; i < max_మార్గం_లోతు; i++)
	vim_free(files[i].name);
    vim_free(files);

    ఒకవేళ (type == CHECK_మార్గం)
    {
	ఒకవేళ (!did_show)
	{
	    ఒకవేళ (action != ACTION_SHOW_ALL)
		సందేశం(_("All included files were found"));
	    లేకపోతే
		సందేశం(_("No included files"));
	}
    }
    లేకపోతే ఒకవేళ (!దొరికింది && action != ACTION_EXPAND && !silent)
    {
	ఒకవేళ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	లేకపోతే ఒకవేళ (type == కనుగొను_DEFINE)
	    emsg(_(e_couldnt_కనుగొను_definition));
	లేకపోతే
	    emsg(_(e_couldnt_కనుగొను_నమూనా));
    }
    ఒకవేళ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	సందేశం_చివర();

fpip_చివర:
    vim_free(file_పంక్తి);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    స్థిర శూన్య
show_pat_లోపల_మార్గం(
    char_u  *పంక్తి,
    పూర్ణాంకం	    type,
    పూర్ణాంకం	    did_show,
    పూర్ణాంకం	    action,
    FILE    *fp,
    linenr_T *lnum,
    దీర్ఘ    లెక్క)
{
    char_u  *p;
    size_t  linelen;

    ఒకవేళ (did_show)
	సందేశం_putchar('\n');	// cursor below last one
    లేకపోతే ఒకవేళ (!సందేశం_silent)
	gotocmdline(TRUE);	// cursor at status line
    ఒకవేళ (got_int)		// 'q' typed at "--more--" message
	తిరిగి;
    linelen = STRLEN(పంక్తి);
    కొరకు (;;)
    {
	p = పంక్తి + linelen - 1;
	ఒకవేళ (fp != ఖాళీ)
	{
	    // We used fgets(), so get rid of newline at end
	    ఒకవేళ (p >= పంక్తి && *p == '\n')
		--p;
	    ఒకవేళ (p >= పంక్తి && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ఒకవేళ (action == ACTION_SHOW_ALL)
	{
	    sprintf((అక్షరం *)IObuff, "%3ld: ", లెక్క);	// show match nr
	    సందేశం_puts((అక్షరం *)IObuff);
	    sprintf((అక్షరం *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    సందేశం_puts_attr((అక్షరం *)IObuff, HL_ATTR(HLF_N));
	    సందేశం_puts(" ");
	}
	సందేశం_prt_పంక్తి(పంక్తి, FALSE);
	బహిర్గతం_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ఒకవేళ (got_int || type != కనుగొను_DEFINE || p < పంక్తి || *p != '\\')
	    ఆపు;

	ఒకవేళ (fp != ఖాళీ)
	{
	    ఒకవేళ (vim_fgets(పంక్తి, LSIZE, fp)) // end of file
		ఆపు;
	    linelen = STRLEN(పంక్తి);
	    ++*lnum;
	}
	లేకపోతే
	{
	    ఒకవేళ (++*lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    పంక్తి = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	సందేశం_putchar('\n');
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(పూర్ణాంకం idx)
{
    తిరిగి &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    పూర్ణాంకం
get_spat_చివరి_idx(శూన్య)
{
    తిరిగి చివరి_idx;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    శూన్య
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*నమూనా = ఖాళీ;
    పూర్ణాంకం			maxcount = p_msc;
    దీర్ఘ		timeout = శోధన_STAT_DEF_TIMEOUT;
    పూర్ణాంకం			recompute = TRUE;
    searchstat_T	stat;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	పూర్ణాంకం		error = FALSE;

	ఒకవేళ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    తిరిగి;
	dict = argvars[0].vval.v_dict;
	di = dict_కనుగొను(dict, (char_u *)"timeout", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    timeout = (దీర్ఘ)tv_get_సంఖ్య_chk(&di->di_tv, &error);
	    ఒకవేళ (error)
		తిరిగి;
	}
	di = dict_కనుగొను(dict, (char_u *)"maxcount", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    maxcount = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&di->di_tv, &error);
	    ఒకవేళ (error)
		తిరిగి;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_కనుగొను(dict, (char_u *)"pattern", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    నమూనా = tv_get_string_chk(&di->di_tv);
	    ఒకవేళ (నమూనా == ఖాళీ)
		తిరిగి;
	}
	di = dict_కనుగొను(dict, (char_u *)"pos", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_వాదం_str), "pos");
		తిరిగి;
	    }
	    ఒకవేళ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_వాదం_str), "List format should be [lnum, col, off]");
		తిరిగి;
	    }
	    li = list_కనుగొను(di->di_tv.vval.v_list, 0L);
	    ఒకవేళ (li != ఖాళీ)
	    {
		pos.lnum = tv_get_సంఖ్య_chk(&li->li_tv, &error);
		ఒకవేళ (error)
		    తిరిగి;
	    }
	    li = list_కనుగొను(di->di_tv.vval.v_list, 1L);
	    ఒకవేళ (li != ఖాళీ)
	    {
		pos.నిలువు = tv_get_సంఖ్య_chk(&li->li_tv, &error) - 1;
		ఒకవేళ (error)
		    తిరిగి;
	    }
	    li = list_కనుగొను(di->di_tv.vval.v_list, 2L);
	    ఒకవేళ (li != ఖాళీ)
	    {
		pos.coladd = tv_get_సంఖ్య_chk(&li->li_tv, &error);
		ఒకవేళ (error)
		    తిరిగి;
	    }
	}
    }

    save_చివరి_శోధన_నమూనా();
# ifdef FEAT_శోధన_EXTRA
    save_incsearch_state();
# endif
    ఒకవేళ (నమూనా != ఖాళీ)
    {
	ఒకవేళ (*నమూనా == NUL)
	    వెళ్ళు the_చివర;
	vim_free(spats[చివరి_idx].pat);
	spats[చివరి_idx].patlen = STRLEN(నమూనా);
	spats[చివరి_idx].pat = vim_strnsave(నమూనా, spats[చివరి_idx].patlen);
	ఒకవేళ (spats[చివరి_idx].pat == ఖాళీ)
	    spats[చివరి_idx].patlen = 0;
    }
    ఒకవేళ (spats[చివరి_idx].pat == ఖాళీ || *spats[చివరి_idx].pat == NUL)
	వెళ్ళు the_చివర;	// the previous pattern was never defined

    update_శోధన_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "current", stat.cur);
    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "total", stat.cnt);
    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "exact_match", stat.exact_సరిపోలిక);
    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "maxcount", stat.చివరి_maxcount);

the_చివర:
    restore_చివరి_శోధన_నమూనా();
# ifdef FEAT_శోధన_EXTRA
    restore_incsearch_state();
# endif
}
#అయితే_ముగింపు
