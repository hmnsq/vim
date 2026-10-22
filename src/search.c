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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_vv_searchforward(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ_submatch(regmmatch_T *rp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
ਸਥਿਰ char_u *get_ਸਤਰ_and_copy(linenr_T lnum, char_u *buf);
ਸਥਿਰ ਖਾਲੀ show_pat_ਵਿੱਚ_ਰਸਤਾ(char_u *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, FILE *, linenr_T *, ਲੰਮਾ);
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ searchstat
{
    ਪੂਰਨ	    cur;	    // current position of found words
    ਪੂਰਨ	    cnt;	    // total count of found words
    ਪੂਰਨ	    exact_ਮੇਲ;    // TRUE if matched exactly on specified position
    ਪੂਰਨ	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    ਪੂਰਨ	    ਆਖਰੀ_maxcount;  // the max count of the last search
} searchstat_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
ਸਥਿਰ ਖਾਲੀ save_incsearch_state(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ restore_incsearch_state(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ check_prevcol(char_u *linep, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ch, ਪੂਰਨ *prevcol);
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_rawstring_ਸਿਰਾ(char_u *linep, pos_T *startpos, pos_T *endpos);
ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_mps_ਮੁੱਲਾਂ(ਪੂਰਨ *initc, ਪੂਰਨ *findc, ਪੂਰਨ *backwards, ਪੂਰਨ switchit);
ਸਥਿਰ ਪੂਰਨ is_zero_ਚੌੜਾਈ(char_u *ਨਮੂਨਾ, size_t patternlen, ਪੂਰਨ move, pos_T *cur, ਪੂਰਨ direction);
ਸਥਿਰ ਖਾਲੀ cmdline_ਖੋਜ_stat(ਪੂਰਨ dirc, pos_T *pos, pos_T *cursor_pos, ਪੂਰਨ show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ, char_u *msgbuf, size_t msgbuflen, ਪੂਰਨ recompute, ਪੂਰਨ maxcount, ਲੰਮਾ timeout);
ਸਥਿਰ ਖਾਲੀ update_ਖੋਜ_stat(ਪੂਰਨ dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, ਪੂਰਨ recompute, ਪੂਰਨ maxcount, ਲੰਮਾ timeout);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_STAT_BUF_LEN 16

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
ਸਥਿਰ spat_T spats[2] =
{
    {ਨਿਰਾ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ਨਿਰਾ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_idx = 0;	// index in spats[] for RE_LAST

ਸਥਿਰ char_u lastc[2] = {NUL, NUL};	// last character searched for
ਸਥਿਰ ਪੂਰਨ lastcdir = FORWARD;		// last direction of character search
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_t_cmd = TRUE;		// last search t_cmd
ਸਥਿਰ char_u	lastc_ਬਾਈਟਾਂ[MB_MAXBYTES + 1];
ਸਥਿਰ ਪੂਰਨ	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ਸਥਿਰ spat_T	    saved_spats[ARRAY_ਲੰਮਾਈ(spats)];
ਸਥਿਰ char_u	    *saved_mr_ਨਮੂਨਾ = ਨਿਰਾ;
ਸਥਿਰ size_t	    saved_mr_patternlen = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
ਸਥਿਰ ਪੂਰਨ	    saved_spats_ਆਖਰੀ_idx = 0;
ਸਥਿਰ ਪੂਰਨ	    saved_spats_no_hlsearch = 0;
#ਜੇ_ਅੰਤ

// allocated copy of pattern used by search_regcomp()
ਸਥਿਰ char_u	    *mr_ਨਮੂਨਾ = ਨਿਰਾ;
ਸਥਿਰ size_t	    mr_patternlen = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    ਪੂਰਨ		matched;	// Found a match in this file
} SearchedFile;
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
ਖੋਜ_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**ਵਰਤਿਆ_pat,
    ਪੂਰਨ		pat_save,
    ਪੂਰਨ		pat_use,
    ਪੂਰਨ		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    ਪੂਰਨ		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ਜੇ (pat == ਨਿਰਾ || *pat == NUL)
    {
	ਪੂਰਨ i;

	ਜੇ (pat_use == RE_ਆਖਰੀ)
	    i = ਆਖਰੀ_idx;
	ਨਹੀਂਤਾਂ
	    i = pat_use;
	ਜੇ (spats[i].pat == ਨਿਰਾ)	// pattern was never defined
	{
	    ਜੇ (pat_use == RE_SUBST)
		emsg(_(e_no_ਪਿਛਲਾ_substitute_regular_ਵਿਅੰਜਕ));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
	    rc_did_emsg = TRUE;
	    ਵਾਪਸ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ਨਹੀਂਤਾਂ ਜੇ (options & ਖੋਜ_HIS)	// put new pattern in history
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_history(HIST_ਖੋਜ, pat, patlen, TRUE, NUL);

    ਜੇ (ਵਰਤਿਆ_pat)
	*ਵਰਤਿਆ_pat = pat;

    vim_free(mr_ਨਮੂਨਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_ਨਮੂਨਾ = ਉਲਟ_ਪਾਠ(pat);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	mr_ਨਮੂਨਾ = vim_strnsave(pat, patlen);
    ਜੇ (mr_ਨਮੂਨਾ == ਨਿਰਾ)
	mr_patternlen = 0;
    ਨਹੀਂਤਾਂ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ਜੇ (!(options & ਖੋਜ_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ਜੇ (pat_save == RE_ਖੋਜ || pat_save == RE_BOTH)
	    save_re_pat(RE_ਖੋਜ, pat, patlen, magic);
	// substitute or global command
	ਜੇ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ਜੇ (regmatch->regprog == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_ਖੋਜ_pat(ਖਾਲੀ)
{
    ਵਾਪਸ mr_ਨਮੂਨਾ;
}

    ਖਾਲੀ
save_re_pat(ਪੂਰਨ idx, char_u *pat, size_t patlen, ਪੂਰਨ magic)
{
    ਜੇ (spats[idx].pat == pat)
	ਵਾਪਸ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ਜੇ (spats[idx].pat == ਨਿਰਾ)
	spats[idx].patlen = 0;
    ਨਹੀਂਤਾਂ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    ਆਖਰੀ_idx = idx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ਜੇ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ਸੈੱਟ_no_hlsearch(FALSE);
#ਜੇ_ਅੰਤ
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ਸਥਿਰ ਪੂਰਨ save_level = 0;

    ਖਾਲੀ
save_ਖੋਜ_patterns(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਜੇ (save_level++ != 0)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ਜੇ (spats[i].pat != ਨਿਰਾ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ਜੇ (saved_spats[i].pat == ਨਿਰਾ)
		saved_spats[i].patlen = 0;
	    ਨਹੀਂਤਾਂ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ਜੇ (mr_ਨਮੂਨਾ == ਨਿਰਾ)
	saved_mr_ਨਮੂਨਾ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	saved_mr_ਨਮੂਨਾ = vim_strnsave(mr_ਨਮੂਨਾ, mr_patternlen);
    ਜੇ (saved_mr_ਨਮੂਨਾ == ਨਿਰਾ)
	saved_mr_patternlen = 0;
    ਨਹੀਂਤਾਂ
	saved_mr_patternlen = mr_patternlen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    saved_spats_ਆਖਰੀ_idx = ਆਖਰੀ_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
restore_ਖੋਜ_patterns(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਜੇ (--save_level != 0)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ਜੇ defined(FEAT_EVAL)
    ਸੈੱਟ_vv_searchforward();
#ਜੇ_ਅੰਤ
    vim_free(mr_ਨਮੂਨਾ);
    mr_ਨਮੂਨਾ = saved_mr_ਨਮੂਨਾ;
    mr_patternlen = saved_mr_patternlen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਆਖਰੀ_idx = saved_spats_ਆਖਰੀ_idx;
    ਸੈੱਟ_no_hlsearch(saved_spats_no_hlsearch);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_ਖੋਜ_patterns(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_ਨਮੂਨਾ);
    mr_patternlen = 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ਸਥਿਰ spat_T	    saved_ਆਖਰੀ_ਖੋਜ_spat;
ਸਥਿਰ ਪੂਰਨ	    did_save_ਆਖਰੀ_ਖੋਜ_spat = 0;
ਸਥਿਰ ਪੂਰਨ	    saved_ਆਖਰੀ_idx = 0;
ਸਥਿਰ ਪੂਰਨ	    saved_no_hlsearch = 0;
ਸਥਿਰ ਪੂਰਨ	    saved_ਖੋਜ_ਮੇਲ_endcol;
ਸਥਿਰ ਪੂਰਨ	    saved_ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    ਖਾਲੀ
save_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ(ਖਾਲੀ)
{
    ਜੇ (++did_save_ਆਖਰੀ_ਖੋਜ_spat != 1)
	// nested call, nothing to do
	ਵਾਪਸ;

    saved_ਆਖਰੀ_ਖੋਜ_spat = spats[RE_ਖੋਜ];
    ਜੇ (spats[RE_ਖੋਜ].pat != ਨਿਰਾ)
    {
	saved_ਆਖਰੀ_ਖੋਜ_spat.pat = vim_strnsave(spats[RE_ਖੋਜ].pat, spats[RE_ਖੋਜ].patlen);
	ਜੇ (saved_ਆਖਰੀ_ਖੋਜ_spat.pat == ਨਿਰਾ)
	    saved_ਆਖਰੀ_ਖੋਜ_spat.patlen = 0;
	ਨਹੀਂਤਾਂ
	    saved_ਆਖਰੀ_ਖੋਜ_spat.patlen = spats[RE_ਖੋਜ].patlen;
    }
    saved_ਆਖਰੀ_idx = ਆਖਰੀ_idx;
    saved_no_hlsearch = no_hlsearch;
}

    ਖਾਲੀ
restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ(ਖਾਲੀ)
{
    ਜੇ (--did_save_ਆਖਰੀ_ਖੋਜ_spat > 0)
	// nested call, nothing to do
	ਵਾਪਸ;
    ਜੇ (did_save_ਆਖਰੀ_ਖੋਜ_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ਵਾਪਸ;
    }

    vim_free(spats[RE_ਖੋਜ].pat);
    spats[RE_ਖੋਜ] = saved_ਆਖਰੀ_ਖੋਜ_spat;
    saved_ਆਖਰੀ_ਖੋਜ_spat.pat = ਨਿਰਾ;
    saved_ਆਖਰੀ_ਖੋਜ_spat.patlen = 0;
# ਜੇ defined(FEAT_EVAL)
    ਸੈੱਟ_vv_searchforward();
# endif
    ਆਖਰੀ_idx = saved_ਆਖਰੀ_idx;
    ਸੈੱਟ_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ਸਥਿਰ ਖਾਲੀ
save_incsearch_state(ਖਾਲੀ)
{
    saved_ਖੋਜ_ਮੇਲ_endcol = ਖੋਜ_ਮੇਲ_endcol;
    saved_ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ  = ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ;
}

    ਸਥਿਰ ਖਾਲੀ
restore_incsearch_state(ਖਾਲੀ)
{
    ਖੋਜ_ਮੇਲ_endcol = saved_ਖੋਜ_ਮੇਲ_endcol;
    ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ  = saved_ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ;
}

    char_u *
ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ(ਖਾਲੀ)
{
    ਵਾਪਸ spats[RE_ਖੋਜ].pat;
}

    size_t
ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ_len(ਖਾਲੀ)
{
    ਵਾਪਸ spats[RE_ਖੋਜ].patlen;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    ਪੂਰਨ
ignorecase(char_u *pat)
{
    ਵਾਪਸ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    ਪੂਰਨ
ignorecase_opt(char_u *pat, ਪੂਰਨ ic_ਵਿੱਚ, ਪੂਰਨ scs)
{
    ਪੂਰਨ		ic = ic_ਵਿੱਚ;

    ਜੇ (ic && !no_smartcase && scs
			    && !(ctrl_x_ਬਹੁਲਕ_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ਵਾਪਸ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    ਪੂਰਨ
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (ਖਾਲੀ)skip_regexp_ex(pat, NUL, magic_isset(), ਨਿਰਾ, ਨਿਰਾ, &magic_val);

    ਜਦੋਂ (*p != NUL)
    {
	ਪੂਰਨ		l;

	ਜੇ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ਜੇ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ਵਾਪਸ TRUE;
	    p += l;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ਜੇ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ਨਹੀਂਤਾਂ ਜੇ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ਨਹੀਂਤਾਂ ਜੇ (p[1] != NUL)  // skip "\X"
		p += 2;
	    ਨਹੀਂਤਾਂ
		p += 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ਜੇ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ਨਹੀਂਤਾਂ
		p++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (MB_ISUPPER(*p))
	    ਵਾਪਸ TRUE;
	ਨਹੀਂਤਾਂ
	    ++p;
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_EVAL)
    char_u *
ਆਖਰੀ_csearch(ਖਾਲੀ)
{
    ਵਾਪਸ lastc_ਬਾਈਟਾਂ;
}

    ਪੂਰਨ
ਆਖਰੀ_csearch_forward(ਖਾਲੀ)
{
    ਵਾਪਸ lastcdir == FORWARD;
}

    ਪੂਰਨ
ਆਖਰੀ_csearch_until(ਖਾਲੀ)
{
    ਵਾਪਸ ਆਖਰੀ_t_cmd == TRUE;
}

    ਖਾਲੀ
ਸੈੱਟ_ਆਖਰੀ_csearch(ਪੂਰਨ c, char_u *s, ਪੂਰਨ len)
{
    *lastc = c;
    lastc_bytelen = len;
    ਜੇ (len)
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(lastc_ਬਾਈਟਾਂ, s, len);
    ਨਹੀਂਤਾਂ
	CLEAR_ਖੇਤਰ(lastc_ਬਾਈਟਾਂ);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
ਸੈੱਟ_csearch_direction(ਪੂਰਨ cdir)
{
    lastcdir = cdir;
}

    ਖਾਲੀ
ਸੈੱਟ_csearch_until(ਪੂਰਨ t_cmd)
{
    ਆਖਰੀ_t_cmd = t_cmd;
}

    char_u *
ਆਖਰੀ_ਖੋਜ_pat(ਖਾਲੀ)
{
    ਵਾਪਸ spats[ਆਖਰੀ_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    ਖਾਲੀ
reset_ਖੋਜ_dir(ਖਾਲੀ)
{
    spats[0].off.dir = '/';
#ਜੇ defined(FEAT_EVAL)
    ਸੈੱਟ_vv_searchforward();
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਆਖਰੀ_ਖੋਜ_pat(
    char_u	*s,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		magic,
    ਪੂਰਨ		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ਜੇ (*s == NUL)
	spats[idx].pat = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ਜੇ (spats[idx].pat == ਨਿਰਾ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ਜੇ defined(FEAT_EVAL)
    ਸੈੱਟ_vv_searchforward();
# endif
    spats[idx].off.ਸਤਰ = FALSE;
    spats[idx].off.ਸਿਰਾ = FALSE;
    spats[idx].off.off = 0;
    ਜੇ (setlast)
	ਆਖਰੀ_idx = idx;
    ਜੇ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ਜੇ (spats[idx].pat == ਨਿਰਾ)
	    saved_spats[idx].pat = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ਜੇ (saved_spats[idx].pat == ਨਿਰਾ)
	    saved_spats[idx].patlen = 0;
	ਨਹੀਂਤਾਂ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_ਖੋਜ_EXTRA
	saved_spats_ਆਖਰੀ_idx = ਆਖਰੀ_idx;
# endif
    }
# ifdef FEAT_ਖੋਜ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ਜੇ (p_hls && idx == ਆਖਰੀ_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    ਖਾਲੀ
ਆਖਰੀ_pat_prog(regmmatch_T *regmatch)
{
    ਜੇ (spats[ਆਖਰੀ_idx].pat == ਨਿਰਾ)
    {
	regmatch->regprog = ਨਿਰਾ;
	ਵਾਪਸ;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (ਖਾਲੀ)ਖੋਜ_regcomp((char_u *)"", 0, ਨਿਰਾ, 0, ਆਖਰੀ_idx, ਖੋਜ_KEEP, regmatch);
    --emsg_off;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*ਸਿਰਾ_pos,	// set to end of the match, unless NULL
    ਪੂਰਨ		dir,
    char_u	*pat,
    size_t	patlen,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		options,
    ਪੂਰਨ		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    ਪੂਰਨ		ਮਿਲਿਆ;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	ਖਾਨਾ;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    ਪੂਰਨ		loop;
    pos_T	ਸ਼ੁਰੂ_pos;
    ਪੂਰਨ		at_ਪਹਿਲਾ_ਸਤਰ;
    ਪੂਰਨ		extra_ਖਾਨਾ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_char_len;
    ਪੂਰਨ		ਮੇਲ_ok;
    ਲੰਮਾ	nmatched;
    ਪੂਰਨ		submatch = 0;
    ਪੂਰਨ		ਪਹਿਲਾ_ਮੇਲ = TRUE;
    ਪੂਰਨ		called_emsg_before = called_emsg;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ		break_loop = FALSE;
#ਜੇ_ਅੰਤ
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    ਪੂਰਨ		ਅਣਵਰਤਿਆ_timeout_flag = FALSE;
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ = &ਅਣਵਰਤਿਆ_timeout_flag;  // set when timed out.
    ਪੂਰਨ		ਖੋਜ_from_ਮੇਲ_ਸਿਰਾ;		    // vi-compatible search?

    ਜੇ (ਖੋਜ_regcomp(pat, patlen, ਨਿਰਾ, RE_ਖੋਜ, pat_use,
		   (options & (ਖੋਜ_HIS + ਖੋਜ_KEEP)), &regmatch) == FAIL)
    {
	ਜੇ ((options & ਖੋਜ_ਸੁਨੇਹਾ) && !rc_did_emsg)
	    semsg(_(e_invalid_ਖੋਜ_string_str), mr_ਨਮੂਨਾ);
	ਵਾਪਸ FAIL;
    }

    ਖੋਜ_from_ਮੇਲ_ਸਿਰਾ = vim_strchr(p_cpo, CPO_ਖੋਜ) != ਨਿਰਾ;

    ਜੇ (extra_arg != ਨਿਰਾ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	ਜੇ (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_ਬਾਹਰਲਾ = &extra_arg->sa_timed_ਬਾਹਰਲਾ;
#ਜੇ_ਅੰਤ
    }

    /*
     * find the string
     */
    ਕਰੋ	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	ਜੇ (pos->ਖਾਨਾ == MAXCOL)
	    ਸ਼ੁਰੂ_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
						    && pos->ਖਾਨਾ < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ਜੇ (ml_get_buf_len(buf, pos->lnum) <= pos->ਖਾਨਾ)
		ਸ਼ੁਰੂ_char_len = 1;
	    ਨਹੀਂਤਾਂ
		ਸ਼ੁਰੂ_char_len = (*mb_ptr2len)(ptr + pos->ਖਾਨਾ);
	}
	ਨਹੀਂਤਾਂ
	    ਸ਼ੁਰੂ_char_len = 1;
	ਜੇ (dir == FORWARD)
	{
	    ਜੇ (options & ਖੋਜ_ਸ਼ੁਰੂ)
		extra_ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ
		extra_ਖਾਨਾ = ਸ਼ੁਰੂ_char_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (options & ਖੋਜ_ਸ਼ੁਰੂ)
		extra_ਖਾਨਾ = ਸ਼ੁਰੂ_char_len;
	    ਨਹੀਂਤਾਂ
		extra_ਖਾਨਾ = 0;
	}

	ਸ਼ੁਰੂ_pos = *pos;	// remember start pos for detecting no match
	ਮਿਲਿਆ = 0;		// default: not found
	at_ਪਹਿਲਾ_ਸਤਰ = TRUE;	// default: start in first line
	ਜੇ (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->ਖਾਨਾ = 0;
	    at_ਪਹਿਲਾ_ਸਤਰ = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	ਜੇ (dir == BACKWARD && ਸ਼ੁਰੂ_pos.ਖਾਨਾ == 0
					     && (options & ਖੋਜ_ਸ਼ੁਰੂ) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_ਪਹਿਲਾ_ਸਤਰ = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    lnum = pos->lnum;

	ਲਈ (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    ਲਈ ( ; lnum > 0 && lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
					   lnum += dir, at_ਪਹਿਲਾ_ਸਤਰ = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ਜੇ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    ਰੋਕੋ;
		// Stop after passing the time limit.
		ਜੇ (*timed_ਬਾਹਰਲਾ)
		    ਰੋਕੋ;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		ਖਾਨਾ = at_ਪਹਿਲਾ_ਸਤਰ && (options & ਖੋਜ_ਖਾਨਾ) ? pos->ਖਾਨਾ
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
		// vim_regexec_multi() may clear "regprog"
		ਜੇ (regmatch.regprog == ਨਿਰਾ)
		    ਰੋਕੋ;
		// Abort searching on an error (e.g., out of stack).
		ਜੇ (called_emsg > called_emsg_before || *timed_ਬਾਹਰਲਾ)
		    ਰੋਕੋ;
		ਜੇ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    submatch = ਪਹਿਲਾ_submatch(&regmatch);
#ਜੇ_ਅੰਤ
		    // "lnum" may be past end of buffer for "\n\zs".
		    ਜੇ (lnum + matchpos.lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			ptr = (char_u *)"";
		    ਨਹੀਂਤਾਂ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ਜੇ (dir == FORWARD && at_ਪਹਿਲਾ_ਸਤਰ)
		    {
			ਮੇਲ_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ਜਦੋਂ (matchpos.lnum == 0
				&& ((options & ਖੋਜ_ਸਿਰਾ) && ਪਹਿਲਾ_ਮੇਲ
				    ?  (nmatched == 1
					&& (ਪੂਰਨ)endpos.ਖਾਨਾ - 1
					     < (ਪੂਰਨ)ਸ਼ੁਰੂ_pos.ਖਾਨਾ + extra_ਖਾਨਾ)
				    : ((ਪੂਰਨ)matchpos.ਖਾਨਾ
						  - (ptr[matchpos.ਖਾਨਾ] == NUL)
					    < (ਪੂਰਨ)ਸ਼ੁਰੂ_pos.ਖਾਨਾ + extra_ਖਾਨਾ)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ਜੇ (ਖੋਜ_from_ਮੇਲ_ਸਿਰਾ)
			    {
				ਜੇ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    ਮੇਲ_ok = FALSE;
				    ਰੋਕੋ;
				}
				matchcol = endpos.ਖਾਨਾ;
				// for empty match: advance one char
				ਜੇ (matchcol == matchpos.ਖਾਨਾ
						      && ptr[matchcol] != NUL)
				{
				    ਜੇ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ਨਹੀਂਤਾਂ
					++matchcol;
				}
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ਜੇ (ptr[matchcol] != NUL)
				{
				    ਜੇ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    ਨਹੀਂਤਾਂ
					++matchcol;
				}
			    }
			    ਜੇ (matchcol == 0 && (options & ਖੋਜ_ਸ਼ੁਰੂ))
				ਰੋਕੋ;
			    ਜੇ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_ਬਾਹਰਲਾ)) == 0)
			    {
				ਮੇਲ_ok = FALSE;
				ਰੋਕੋ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ਜੇ (regmatch.regprog == ਨਿਰਾ)
				ਰੋਕੋ;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    submatch = ਪਹਿਲਾ_submatch(&regmatch);
#ਜੇ_ਅੰਤ

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ਜੇ (!ਮੇਲ_ok)
			    ਜਾਰੀ;
		    }
		    ਜੇ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			ਮੇਲ_ok = FALSE;
			ਲਈ (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ਜੇ (loop
				|| ((options & ਖੋਜ_ਸਿਰਾ)
				    ? (lnum + regmatch.endpos[0].lnum
							      < ਸ਼ੁਰੂ_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == ਸ਼ੁਰੂ_pos.lnum
					     && (ਪੂਰਨ)regmatch.endpos[0].ਖਾਨਾ - 1
							< (ਪੂਰਨ)ਸ਼ੁਰੂ_pos.ਖਾਨਾ
								+ extra_ਖਾਨਾ))
				    : (lnum + regmatch.startpos[0].lnum
							      < ਸ਼ੁਰੂ_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == ਸ਼ੁਰੂ_pos.lnum
					     && (ਪੂਰਨ)regmatch.startpos[0].ਖਾਨਾ
						      < (ਪੂਰਨ)ਸ਼ੁਰੂ_pos.ਖਾਨਾ
							      + extra_ਖਾਨਾ))))
			    {
				ਮੇਲ_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				submatch = ਪਹਿਲਾ_submatch(&regmatch);
#ਜੇ_ਅੰਤ
			    }
			    ਨਹੀਂਤਾਂ
				ਰੋਕੋ;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ਜੇ (ਖੋਜ_from_ਮੇਲ_ਸਿਰਾ)
			    {
				ਜੇ (nmatched > 1)
				    ਰੋਕੋ;
				matchcol = endpos.ਖਾਨਾ;
				// for empty match: advance one char
				ਜੇ (matchcol == matchpos.ਖਾਨਾ
						      && ptr[matchcol] != NUL)
				{
				    ਜੇ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ਨਹੀਂਤਾਂ
					++matchcol;
				}
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// Stop when the match is in a next line.
				ਜੇ (matchpos.lnum > 0)
				    ਰੋਕੋ;
				matchcol = matchpos.ਖਾਨਾ;
				ਜੇ (ptr[matchcol] != NUL)
				{
				    ਜੇ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ਨਹੀਂਤਾਂ
					++matchcol;
				}
			    }
			    ਜੇ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_ਬਾਹਰਲਾ)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ਜੇ (*timed_ਬਾਹਰਲਾ)
				    ਮੇਲ_ok = FALSE;
				ਰੋਕੋ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ਜੇ (regmatch.regprog == ਨਿਰਾ)
				ਰੋਕੋ;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ਜੇ (!ਮੇਲ_ok)
			    ਜਾਰੀ;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ਜੇ ((options & ਖੋਜ_ਸਿਰਾ) && !(options & ਖੋਜ_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.ਖਾਨਾ == endpos.ਖਾਨਾ))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->ਖਾਨਾ = endpos.ਖਾਨਾ;
			ਜੇ (endpos.ਖਾਨਾ == 0)
			{
			    ਜੇ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->ਖਾਨਾ = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    --pos->ਖਾਨਾ;
			    ਜੇ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + pos->ਖਾਨਾ);
			    }
			}
			ਜੇ (ਸਿਰਾ_pos != ਨਿਰਾ)
			{
			    ਸਿਰਾ_pos->lnum = lnum + matchpos.lnum;
			    ਸਿਰਾ_pos->ਖਾਨਾ = matchpos.ਖਾਨਾ;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->ਖਾਨਾ = matchpos.ਖਾਨਾ;
			ਜੇ (ਸਿਰਾ_pos != ਨਿਰਾ)
			{
			    ਸਿਰਾ_pos->lnum = lnum + endpos.lnum;
			    ਸਿਰਾ_pos->ਖਾਨਾ = endpos.ਖਾਨਾ;
			}
		    }
		    pos->coladd = 0;
		    ਜੇ (ਸਿਰਾ_pos != ਨਿਰਾ)
			ਸਿਰਾ_pos->coladd = 0;
		    ਮਿਲਿਆ = 1;
		    ਪਹਿਲਾ_ਮੇਲ = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ = endpos.lnum - matchpos.lnum;
		    ਖੋਜ_ਮੇਲ_endcol = endpos.ਖਾਨਾ;
		    ਰੋਕੋ;
		}
		ਸਤਰ_breakcheck();	// stop if ctrl-C typed
		ਜੇ (got_int)
		    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ਜੇ ((options & ਖੋਜ_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    ਰੋਕੋ;
		}
#ਜੇ_ਅੰਤ

		ਜੇ (loop && lnum == ਸ਼ੁਰੂ_pos.lnum)
		    ਰੋਕੋ;	    // if second loop, stop where started
	    }
	    at_ਪਹਿਲਾ_ਸਤਰ = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		ਰੋਕੋ;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ਜੇ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_ਬਾਹਰਲਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
			      || break_loop
#ਜੇ_ਅੰਤ
			      || ਮਿਲਿਆ || loop)
		ਰੋਕੋ;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ਜੇ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
		lnum = 1;
	    ਜੇ (!shortmess(SHM_ਖੋਜ)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & ਖੋਜ_ਸੁਨੇਹਾ))
		give_warning((char_u *)_(dir == BACKWARD
					  ? ਸਿਖਰ_bot_ਸੁਨੇਹਾ : bot_ਸਿਖਰ_ਸੁਨੇਹਾ), TRUE);
	    ਜੇ (extra_arg != ਨਿਰਾ)
		extra_arg->sa_wrapped = TRUE;
	}
	ਜੇ (got_int || called_emsg > called_emsg_before || *timed_ਬਾਹਰਲਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		|| break_loop
#ਜੇ_ਅੰਤ
		)
	    ਰੋਕੋ;
    }
    ਜਦੋਂ (--ਗਿਣਤੀ_2 > 0 && ਮਿਲਿਆ);   // stop after count matches or no match

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (extra_arg != ਨਿਰਾ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#ਜੇ_ਅੰਤ
    vim_regfree(regmatch.regprog);

    ਜੇ (!ਮਿਲਿਆ)		    // did not find it
    {
	ਜੇ (got_int)
	    emsg(_(e_interrupted));
	ਨਹੀਂਤਾਂ ਜੇ ((options & ਖੋਜ_ਸੁਨੇਹਾ) == ਖੋਜ_ਸੁਨੇਹਾ)
	{
	    ਜੇ (p_ws)
		semsg(_(e_ਨਮੂਨਾ_not_ਮਿਲਿਆ_str), mr_ਨਮੂਨਾ);
	    ਨਹੀਂਤਾਂ ਜੇ (lnum == 0)
		semsg(_(e_ਖੋਜ_ਪ੍ਰਾਪਤੀ_ਸਿਖਰ_without_ਮੇਲ_for_str), mr_ਨਮੂਨਾ);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_ਖੋਜ_ਪ੍ਰਾਪਤੀ_bottom_without_ਮੇਲ_for_str), mr_ਨਮੂਨਾ);
	}
	ਵਾਪਸ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ਜੇ (pos->lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	pos->lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	pos->ਖਾਨਾ = ml_get_buf_len(buf, pos->lnum);
	ਜੇ (pos->ਖਾਨਾ > 0)
	    --pos->ਖਾਨਾ;
    }

    ਵਾਪਸ submatch + 1;
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    ਖਾਲੀ
ਸੈੱਟ_ਖੋਜ_direction(ਪੂਰਨ cdir)
{
    spats[0].off.dir = cdir;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_vv_searchforward(ਖਾਲੀ)
{
    ਸੈੱਟ_vim_var_nr(VV_SEARCHFORWARD, (ਲੰਮਾ)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਹਿਲਾ_submatch(regmmatch_T *rp)
{
    ਪੂਰਨ		submatch;

    ਲਈ (submatch = 1; ; ++submatch)
    {
	ਜੇ (rp->startpos[submatch].lnum >= 0)
	    ਰੋਕੋ;
	ਜੇ (submatch == 9)
	{
	    submatch = 0;
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ submatch;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
parse_ਖੋਜ_ਨਮੂਨਾ_ਵਿੱਥ(
    char_u	**pat,
    size_t	*patlen,
    ਪੂਰਨ		ਖੋਜ_delim,
    ਪੂਰਨ		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*ਵਿੱਥ)
{
    ਪੂਰਨ		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ਜੇ (*pat == ਨਿਰਾ || **pat == NUL)
	ਵਾਪਸ 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ਨਿਰਾ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, ਖੋਜ_delim, magic_isset(),
						    strcopy, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (ਪੂਰਨ)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ਜੇ (*p == ਖੋਜ_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    ਵਿੱਥ->ਸਤਰ = FALSE;
    ਵਿੱਥ->ਸਿਰਾ = FALSE;
    ਵਿੱਥ->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    ਜੇ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	ਵਿੱਥ->ਸਤਰ = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ ((options & ਖੋਜ_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ਜੇ (*p == 'e')		// end
	    ਵਿੱਥ->ਸਿਰਾ = ਖੋਜ_ਸਿਰਾ;
	++p;
    }
    ਜੇ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ਜੇ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    ਵਿੱਥ->off = atol((ਅੱਖਰ *)p);
	ਨਹੀਂਤਾਂ ਜੇ (*p == '-')	    // single '-'
	    ਵਿੱਥ->off = -1;
	ਨਹੀਂਤਾਂ			    // single '+'
	    ਵਿੱਥ->off = 1;
	++p;
	ਜਦੋਂ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (ਪੂਰਨ)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ਵਾਪਸ cmdlen;
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
    ਪੂਰਨ
do_ਖੋਜ(
    oparg_T	    *oap,	// can be NULL
    ਪੂਰਨ		    dirc,	// '/' or '?'
    ਪੂਰਨ		    ਖੋਜ_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    ਲੰਮਾ	    ਗਿਣਤੀ_2,
    ਪੂਰਨ		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    ਪੂਰਨ		    retval;	// Return value
    char_u	    *p;
    ਲੰਮਾ	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ਨਿਰਾ;
    ਪੂਰਨ		    show_ਖੋਜ_stats;
    char_u	    *msgbuf = ਨਿਰਾ;
    size_t	    msgbuflen = 0;
    ਪੂਰਨ		    has_ਵਿੱਥ = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ਜੇ (spats[0].off.ਸਤਰ && vim_strchr(p_cpo, CPO_LINEOFF) != ਨਿਰਾ)
    {
	spats[0].off.ਸਤਰ = FALSE;
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
    ਜੇ (dirc == 0)
	dirc = spats[0].off.dir;
    ਨਹੀਂਤਾਂ
    {
	spats[0].off.dir = dirc;
#ਜੇ defined(FEAT_EVAL)
	ਸੈੱਟ_vv_searchforward();
#ਜੇ_ਅੰਤ
    }
    ਜੇ (options & ਖੋਜ_REV)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ਨਹੀਂਤਾਂ
	ਜੇ (dirc == '/')
	    dirc = '?';
	ਨਹੀਂਤਾਂ
	    dirc = '/';
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ਜੇ (dirc == '/')
    {
	ਜੇ (hasFolding(pos.lnum, ਨਿਰਾ, &pos.lnum))
	    pos.ਖਾਨਾ = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (hasFolding(pos.lnum, &pos.lnum, ਨਿਰਾ))
	    pos.ਖਾਨਾ = 0;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ਜੇ (no_hlsearch && !(options & ਖੋਜ_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	ਸੈੱਟ_no_hlsearch(FALSE);
    }
#ਜੇ_ਅੰਤ

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ		show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ਨਿਰਾ;
					    // use previous pattern
	ਜੇ (pat == ਨਿਰਾ || *pat == NUL || *pat == ਖੋਜ_delim)
	{
	    ਜੇ (spats[RE_ਖੋਜ].pat == ਨਿਰਾ)	    // no previous pattern
	    {
		ਜੇ (spats[RE_SUBST].pat == ਨਿਰਾ)
		{
		    emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
		    retval = 0;
		    ਜਾਓ ਸਿਰਾ_do_ਖੋਜ;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ਜੇ (pat != ਨਿਰਾ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_ਖੋਜ_ਨਮੂਨਾ_ਵਿੱਥ(&pat, &patlen,
				ਖੋਜ_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ਜੇ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_ਖੋਜ_stats = FALSE;
	ਜੇ ((options & ਖੋਜ_ECHO) && messaging()
		&& !ਸੁਨੇਹਾ_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();

	    // Get the offset, so we know how long it is.
	    ਜੇ (!cmd_silent &&
		    (spats[0].off.ਸਤਰ || spats[0].off.ਸਿਰਾ || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ਜੇ (spats[0].off.ਸਿਰਾ)
		    off_buf[off_len++] = 'e';
		ਨਹੀਂਤਾਂ ਜੇ (!spats[0].off.ਸਤਰ)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ਜੇ (spats[0].off.off != 0 || spats[0].off.ਸਤਰ)
		    off_len += vim_snprintf((ਅੱਖਰ *)off_buf + off_len,
			   ਆਕਾਰ(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ਜੇ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ਜੇ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ਜੇ (ਸੁਨੇਹਾ_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - ਸੁਨੇਹਾ_ਪੰਕਤੀ) * cmdline_ਚੌੜਾਈ - 1;
		ਨਹੀਂਤਾਂ
		    // Use up to 'showcmd' column.
		    msgbufsize = (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - ਸੁਨੇਹਾ_ਪੰਕਤੀ - 1) * cmdline_ਚੌੜਾਈ
			+ sc_ਖਾਨਾ - 1;
		ਜੇ (msgbufsize < plen + off_len + ਖੋਜ_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + ਖੋਜ_STAT_BUF_LEN + 3;
	    }
	    ਨਹੀਂਤਾਂ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ਜੇ (msgbuf == ਨਿਰਾ)
	    {
		msgbuflen = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ਜੇ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ਜੇ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    ਨਹੀਂਤਾਂ
			mch_memmove(msgbuf + 1, p, plen);
		    ਜੇ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = ਸੁਨੇਹਾ_strtrunc(msgbuf, TRUE);
		    ਜੇ (trunc != ਨਿਰਾ)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    ਜੇ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = ਉਲਟ_ਪਾਠ(msgbuf);
			ਜੇ (r != ਨਿਰਾ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ਜਦੋਂ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ਜੇ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ਨਹੀਂਤਾਂ
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#ਜੇ_ਅੰਤ
		    ਸੁਨੇਹਾ_outtrans(msgbuf);
		    ਸੁਨੇਹਾ_clr_eos();
		    ਸੁਨੇਹਾ_check();

		    gotocmdline(FALSE);
		    ਬਾਹਰਲਾ_flush();
		    ਸੁਨੇਹਾ_nowait = TRUE;	    // don't wait for this message
		}

		ਜੇ (!shortmess(SHM_SEARCHCOUNT))
		    show_ਖੋਜ_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	ਜੇ (!spats[0].off.ਸਤਰ && spats[0].off.off && pos.ਖਾਨਾ < MAXCOL - 2)
	{
	    ਜੇ (spats[0].off.off > 0)
	    {
		ਲਈ (c = spats[0].off.off; c; --c)
		    ਜੇ (decl(&pos) == -1)
			ਰੋਕੋ;
		ਜੇ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.ਖਾਨਾ = MAXCOL;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (c = spats[0].off.off; c; ++c)
		    ਜੇ (incl(&pos) == -1)
			ਰੋਕੋ;
		ਜੇ (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
		    pos.ਖਾਨਾ = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ਨਿਰਾ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, ਗਿਣਤੀ_2, spats[0].off.ਸਿਰਾ + (options &
		       (ਖੋਜ_KEEP + ਖੋਜ_PEEK + ਖੋਜ_HIS
			+ ਖੋਜ_ਸੁਨੇਹਾ + ਖੋਜ_ਸ਼ੁਰੂ
			+ ((pat != ਨਿਰਾ && *pat == ';') ? 0 : ਖੋਜ_NOOF))),
		RE_ਆਖਰੀ, sia);

	ਜੇ (dircp != ਨਿਰਾ)
	    *dircp = ਖੋਜ_delim; // restore second '/' or '?' for normal_cmd()

	ਜੇ (!shortmess(SHM_ਖੋਜ) && sia && sia->sa_wrapped)
	    show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ = TRUE;

	ਜੇ (c == FAIL)
	{
	    retval = 0;
	    ਜਾਓ ਸਿਰਾ_do_ਖੋਜ;
	}
	ਜੇ (spats[0].off.ਸਿਰਾ && oap != ਨਿਰਾ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ਜੇ (!(options & ਖੋਜ_NOOF) || (pat != ਨਿਰਾ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ਜੇ (spats[0].off.ਸਤਰ)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ਜੇ (c < 1)
		    pos.lnum = 1;
		ਨਹੀਂਤਾਂ ਜੇ (c > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    pos.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਨਹੀਂਤਾਂ
		    pos.lnum = c;
		pos.ਖਾਨਾ = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (pos.ਖਾਨਾ < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ਜੇ (c > 0)
		{
		    ਜਦੋਂ (c-- > 0)
			ਜੇ (incl(&pos) == -1)
			    ਰੋਕੋ;
		}
		// to the left, check for start of file
		ਨਹੀਂਤਾਂ
		{
		    ਜਦੋਂ (c++ < 0)
			ਜੇ (decl(&pos) == -1)
			    ਰੋਕੋ;
		}
	    }
	    ਜੇ (!EQUAL_POS(pos, org_pos))
		has_ਵਿੱਥ = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ਜੇ (show_ਖੋਜ_stats)
	     cmdline_ਖੋਜ_stat(dirc, &pos, &curwin->w_cursor,
				show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ, msgbuf, msgbuflen,
				(ਗਿਣਤੀ_2 != 1 || has_ਵਿੱਥ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
				 || (!(fdo_flags & FDO_ਖੋਜ)
				     && hasFolding(curwin->w_cursor.lnum,
								   ਨਿਰਾ, ਨਿਰਾ))
#ਜੇ_ਅੰਤ
				),
				p_msc,
				ਖੋਜ_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	ਜੇ (!(options & ਖੋਜ_OPT) || pat == ਨਿਰਾ || *pat != ';')
	    ਰੋਕੋ;

	dirc = *++pat;
	--patlen;
	ਖੋਜ_delim = dirc;
	ਜੇ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    ਜਾਓ ਸਿਰਾ_do_ਖੋਜ;
	}
	++pat;
	--patlen;
    }

    ਜੇ (options & ਖੋਜ_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_ਸੈੱਟ_curswant = true;

ਸਿਰਾ_do_ਖੋਜ:
    ਜੇ ((options & ਖੋਜ_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ਵਾਪਸ retval;
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
    ਪੂਰਨ
ਖੋਜ_for_exact_ਸਤਰ(
    buf_T	*buf,
    pos_T	*pos,
    ਪੂਰਨ		dir,
    char_u	*pat)
{
    linenr_T	ਸ਼ੁਰੂ = 0;
    char_u	*ptr;
    char_u	*p;
    ਪੂਰਨ		compl_len = ins_compl_len();

    ਜੇ (buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 0)
	ਵਾਪਸ FAIL;
    ਲਈ (;;)
    {
	pos->lnum += dir;
	ਜੇ (pos->lnum < 1)
	{
	    ਜੇ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਜੇ (!shortmess(SHM_ਖੋਜ))
		    give_warning((char_u *)_(ਸਿਖਰ_bot_ਸੁਨੇਹਾ), TRUE);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		pos->lnum = 1;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (pos->lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਜੇ (p_ws)
	    {
		pos->lnum = 1;
		ਜੇ (!shortmess(SHM_ਖੋਜ))
		    give_warning((char_u *)_(bot_ਸਿਖਰ_ਸੁਨੇਹਾ), TRUE);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		pos->lnum = 1;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (pos->lnum == ਸ਼ੁਰੂ)
	    ਰੋਕੋ;
	ਜੇ (ਸ਼ੁਰੂ == 0)
	    ਸ਼ੁਰੂ = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->ਖਾਨਾ = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ਜੇ (compl_ਸਥਿਤੀ_adding() && !compl_ਸਥਿਤੀ_sol())
	{
	    ਜੇ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ਜੇ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ਵਾਪਸ OK;
	}
    }
    ਵਾਪਸ FAIL;
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
    ਪੂਰਨ
searchc(cmdarg_T *cap, ਪੂਰਨ t_cmd)
{
    ਪੂਰਨ			c = cap->nchar;	// char to search for
    ਪੂਰਨ			dir = cap->arg;	// TRUE for searching forward
    ਲੰਮਾ		ਗਿਣਤੀ_2 = cap->count1;	// repeat count
    ਪੂਰਨ			ਖਾਨਾ;
    char_u		*p;
    ਪੂਰਨ			len;
    ਪੂਰਨ			stop = TRUE;

    ਜੇ (c != NUL)	// normal search: remember args for repeat
    {
	ਜੇ (!ਕੁੰਜੀ_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    ਸੈੱਟ_csearch_direction(dir);
	    ਸੈੱਟ_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_ਬਾਈਟਾਂ);
	    ਜੇ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_ਬਾਈਟਾਂ + lastc_bytelen);
		ਜੇ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_ਬਾਈਟਾਂ + lastc_bytelen);
	    }
	}
    }
    ਨਹੀਂਤਾਂ		// repeat previous search
    {
	ਜੇ (*lastc == NUL && lastc_bytelen <= 1)
	    ਵਾਪਸ FAIL;
	ਜੇ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ਨਹੀਂਤਾਂ
	    dir = lastcdir;
	t_cmd = ਆਖਰੀ_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ਜੇ (vim_strchr(p_cpo, CPO_SCOLON) == ਨਿਰਾ && ਗਿਣਤੀ_2 == 1 && t_cmd)
	    stop = FALSE;
    }

    ਜੇ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ਨਹੀਂਤਾਂ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    len = ml_get_curline_len();

    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	ਜੇ (has_mbyte)
	{
	    ਲਈ (;;)
	    {
		ਜੇ (dir > 0)
		{
		    ਖਾਨਾ += (*mb_ptr2len)(p + ਖਾਨਾ);
		    ਜੇ (ਖਾਨਾ >= len)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (ਖਾਨਾ == 0)
			ਵਾਪਸ FAIL;
		    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + ਖਾਨਾ - 1) + 1;
		}
		ਜੇ (lastc_bytelen <= 1)
		{
		    ਜੇ (p[ਖਾਨਾ] == c && stop)
			ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p + ਖਾਨਾ, lastc_ਬਾਈਟਾਂ, lastc_bytelen) == 0
								       && stop)
		    ਰੋਕੋ;
		stop = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (;;)
	    {
		ਜੇ ((ਖਾਨਾ += dir) < 0 || ਖਾਨਾ >= len)
		    ਵਾਪਸ FAIL;
		ਜੇ (p[ਖਾਨਾ] == c && stop)
		    ਰੋਕੋ;
		stop = TRUE;
	    }
	}
    }

    ਜੇ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	ਖਾਨਾ -= dir;
	ਜੇ (has_mbyte)
	{
	    ਜੇ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		ਖਾਨਾ += lastc_bytelen - 1;
	    ਨਹੀਂਤਾਂ
		// To previous char, which may be multi-byte.
		ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + ਖਾਨਾ);
	}
    }
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;

    ਵਾਪਸ OK;
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
findmatch(oparg_T *oap, ਪੂਰਨ initc)
{
    ਵਾਪਸ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ਸਥਿਰ ਪੂਰਨ
check_prevcol(
    char_u	*linep,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		ch,
    ਪੂਰਨ		*prevcol)
{
    --ਖਾਨਾ;
    ਜੇ (ਖਾਨਾ > 0 && has_mbyte)
	ਖਾਨਾ -= (*mb_ਸਿਰ_off)(linep, linep + ਖਾਨਾ);
    ਜੇ (prevcol)
	*prevcol = ਖਾਨਾ;
    ਵਾਪਸ (ਖਾਨਾ >= 0 && linep[ਖਾਨਾ] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_rawstring_ਸਿਰਾ(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;

    ਲਈ (p = linep + startpos->ਖਾਨਾ + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->ਖਾਨਾ - 1;
    delim_copy = vim_strnsave(linep + startpos->ਖਾਨਾ + 1, delim_len);
    ਜੇ (delim_copy == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਲਈ (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *ਸਤਰ = ml_get(lnum);

	ਲਈ (p = ਸਤਰ + (lnum == startpos->lnum
					    ? startpos->ਖਾਨਾ + 1 : 0); *p; ++p)
	{
	    ਜੇ (lnum == endpos->lnum && (colnr_T)(p - ਸਤਰ) >= endpos->ਖਾਨਾ)
		ਰੋਕੋ;
	    ਜੇ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		ਮਿਲਿਆ = TRUE;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਮਿਲਿਆ)
	    ਰੋਕੋ;
    }
    vim_free(delim_copy);
    ਵਾਪਸ ਮਿਲਿਆ;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_mps_ਮੁੱਲਾਂ(
    ਪੂਰਨ	    *initc,
    ਪੂਰਨ	    *findc,
    ਪੂਰਨ	    *backwards,
    ਪੂਰਨ	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ਜਦੋਂ (*ptr != NUL)
    {
	ਜੇ (has_mbyte)
	{
	    char_u *prev;

	    ਜੇ (mb_ptr2char(ptr) == *initc)
	    {
		ਜੇ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ਵਾਪਸ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ਜੇ (mb_ptr2char(ptr) == *initc)
	    {
		ਜੇ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		ਨਹੀਂਤਾਂ
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		ਵਾਪਸ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*ptr == *initc)
	    {
		ਜੇ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ਨਹੀਂਤਾਂ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ਵਾਪਸ;
	    }
	    ptr += 2;
	    ਜੇ (*ptr == *initc)
	    {
		ਜੇ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		ਨਹੀਂਤਾਂ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ਵਾਪਸ;
	    }
	    ++ptr;
	}
	ਜੇ (*ptr == ',')
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
    ਪੂਰਨ		initc,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		maxtravel)
{
    ਸਥਿਰ pos_T pos;			// current search position
    ਪੂਰਨ		findc = 0;		// matching brace
    ਪੂਰਨ		c;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;		// cumulative number of braces
    ਪੂਰਨ		backwards = FALSE;	// init for gcc
    ਪੂਰਨ		raw_string = FALSE;	// search for raw string
    ਪੂਰਨ		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    ਪੂਰਨ		do_quotes;		// check for quotes in current line
    ਪੂਰਨ		at_ਸ਼ੁਰੂ;		// do_quotes value at start position
    ਪੂਰਨ		hash_dir = 0;		// Direction searched for # things
    ਪੂਰਨ		comment_dir = 0;	// Direction searched for comments
    pos_T	ਮੇਲ_pos;		// Where last slash-star was found
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਵਿੱਚ_quotes;	// start position is in quotes
    ਪੂਰਨ		traveled = 0;		// how far we've searched so far
    ਪੂਰਨ		ignore_cend = FALSE;    // ignore comment end
    ਪੂਰਨ		cpo_ਮੇਲ;		// vi compatible matching
    ਪੂਰਨ		cpo_bsl;		// don't recognize backslashes
    ਪੂਰਨ		ਮੇਲ_escaped = 0;	// search for escaped match
    ਪੂਰਨ		dir;			// Direction to search
    ਪੂਰਨ		comment_ਖਾਨਾ = MAXCOL;   // start of / / comment
    ਪੂਰਨ		lispcomm = FALSE;	// inside of Lisp-style comment
    ਪੂਰਨ		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    ਪੂਰਨ		skip_comments = (flags & FM_SKIPCOMM) != 0;
    ਪੂਰਨ		ਵਿੱਚ_ਖੰਡ_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_ਮੇਲ = (vim_strchr(p_cpo, CPO_ਮੇਲ) != ਨਿਰਾ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ਨਿਰਾ);

    // Direction to search when initc is '/', '*' or '#'
    ਜੇ (flags & FM_BACKWARD)
	dir = BACKWARD;
    ਨਹੀਂਤਾਂ ਜੇ (flags & FM_FORWARD)
	dir = FORWARD;
    ਨਹੀਂਤਾਂ
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ਜੇ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ਜੇ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (initc != '#' && initc != NUL)
    {
	ਲੱਭੋ_mps_ਮੁੱਲਾਂ(&initc, &findc, &backwards, TRUE);
	ਜੇ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ਜੇ (findc == NUL)
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ਜੇ (initc == '#')
	{
	    hash_dir = dir;
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ਜੇ (!cpo_ਮੇਲ)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ਜੇ (*ptr == '#' && pos.ਖਾਨਾ <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ਜੇ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ] == '/')
		{
		    ਜੇ (linep[pos.ਖਾਨਾ + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.ਖਾਨਾ++;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (pos.ਖਾਨਾ > 0 && linep[pos.ਖਾਨਾ - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.ਖਾਨਾ--;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ] == '*')
		{
		    ਜੇ (linep[pos.ਖਾਨਾ + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (pos.ਖਾਨਾ > 0 && linep[pos.ਖਾਨਾ - 1] == '/')
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
	    ਜੇ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ਜੇ (linep[pos.ਖਾਨਾ] == NUL && pos.ਖਾਨਾ)
		    --pos.ਖਾਨਾ;
		ਲਈ (;;)
		{
		    initc = PTR2CHAR(linep + pos.ਖਾਨਾ);
		    ਜੇ (initc == NUL)
			ਰੋਕੋ;

		    ਲੱਭੋ_mps_ਮੁੱਲਾਂ(&initc, &findc, &backwards, FALSE);
		    ਜੇ (findc)
			ਰੋਕੋ;
		    pos.ਖਾਨਾ += mb_ptr2len(linep + pos.ਖਾਨਾ);
		}
		ਜੇ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ਜੇ (!cpo_ਮੇਲ && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ਨਹੀਂਤਾਂ
			ਵਾਪਸ ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!cpo_bsl)
		{
		    ਪੂਰਨ ਖਾਨਾ, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    ਲਈ (ਖਾਨਾ = pos.ਖਾਨਾ; check_prevcol(linep, ਖਾਨਾ, '\\', &ਖਾਨਾ);)
			bslcnt++;
		    ਮੇਲ_escaped = (bslcnt & 1);
		}
	    }
	}
	ਜੇ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ਜੇ (oap != ਨਿਰਾ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ਜੇ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ਜੇ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ਨਹੀਂਤਾਂ
		    ਵਾਪਸ ਨਿਰਾ;
	    }
	    pos.ਖਾਨਾ = 0;
	    ਜਦੋਂ (!got_int)
	    {
		ਜੇ (hash_dir > 0)
		{
		    ਜੇ (pos.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (pos.lnum == 1)
		    ਰੋਕੋ;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		ਸਤਰ_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ਜੇ (*ptr != '#')
		    ਜਾਰੀ;
		pos.ਖਾਨਾ = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ਜੇ (hash_dir > 0)
		{
		    ਜੇ (STRNCMP(ptr, "if", 2) == 0)
			ਗਿਣਤੀ_2++;
		    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ptr, "el", 2) == 0)
		    {
			ਜੇ (ਗਿਣਤੀ_2 == 0)
			    ਵਾਪਸ &pos;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ਜੇ (ਗਿਣਤੀ_2 == 0)
			    ਵਾਪਸ &pos;
			ਗਿਣਤੀ_2--;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (STRNCMP(ptr, "if", 2) == 0)
		    {
			ਜੇ (ਗਿਣਤੀ_2 == 0)
			    ਵਾਪਸ &pos;
			ਗਿਣਤੀ_2--;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ਜੇ (ਗਿਣਤੀ_2 == 0)
			    ਵਾਪਸ &pos;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ptr, "endif", 5) == 0)
			ਗਿਣਤੀ_2++;
		}
	    }
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ਜੇ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ਨਿਰਾ)
	backwards = !backwards;
#ਜੇ_ਅੰਤ

    do_quotes = -1;
    ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = MAYBE;
    CLEAR_POS(&ਮੇਲ_pos);

    // backward search: Check if this line contains a single-line comment
    ਜੇ ((backwards && comment_dir) || lisp || skip_comments)
	comment_ਖਾਨਾ = check_linecomment(linep);
    ਜੇ (lisp && comment_ਖਾਨਾ != MAXCOL && pos.ਖਾਨਾ > (colnr_T)comment_ਖਾਨਾ)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ਜੇ (skip_comments && !ਵਿੱਚ_ਖੰਡ_comment && comment_ਖਾਨਾ != MAXCOL
	    && backwards && pos.ਖਾਨਾ > (colnr_T)comment_ਖਾਨਾ)
	pos.ਖਾਨਾ = comment_ਖਾਨਾ;

    ਜਦੋਂ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ਜੇ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ਜੇ (lispcomm && pos.ਖਾਨਾ < (colnr_T)comment_ਖਾਨਾ)
		ਰੋਕੋ;
	    ਜੇ (pos.ਖਾਨਾ == 0)		// at start of line, go to prev. one
	    {
		ਜੇ (pos.lnum == 1)	// start of file
		    ਰੋਕੋ;
		--pos.lnum;

		ਜੇ (maxtravel > 0 && ++traveled > maxtravel)
		    ਰੋਕੋ;

		linep = ml_get(pos.lnum);
		pos.ਖਾਨਾ = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		ਸਤਰ_breakcheck();

		// Check if this line contains a single-line comment
		ਜੇ (comment_dir || lisp || skip_comments)
		    comment_ਖਾਨਾ = check_linecomment(linep);
		// skip comment
		ਜੇ (lisp && comment_ਖਾਨਾ != MAXCOL)
		    pos.ਖਾਨਾ = comment_ਖਾਨਾ;
		ਨਹੀਂਤਾਂ ਜੇ (skip_comments && !ਵਿੱਚ_ਖੰਡ_comment
			&& comment_ਖਾਨਾ != MAXCOL
			&& pos.ਖਾਨਾ > (colnr_T)comment_ਖਾਨਾ)
		    pos.ਖਾਨਾ = comment_ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		--pos.ਖਾਨਾ;
		ਜੇ (has_mbyte)
		    pos.ਖਾਨਾ -= (*mb_ਸਿਰ_off)(linep, linep + pos.ਖਾਨਾ);
	    }
	}
	ਨਹੀਂਤਾਂ				// forward search
	{
	    ਜੇ (linep[pos.ਖਾਨਾ] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_ਖਾਨਾ != MAXCOL
					   && pos.ਖਾਨਾ == (colnr_T)comment_ਖਾਨਾ))
	    {
		ਜੇ (pos.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    ਰੋਕੋ;
		++pos.lnum;

		ਜੇ (maxtravel && traveled++ > maxtravel)
		    ਰੋਕੋ;

		linep = ml_get(pos.lnum);
		pos.ਖਾਨਾ = 0;
		do_quotes = -1;
		ਸਤਰ_breakcheck();
		ਜੇ (lisp || skip_comments)   // find comment pos in new line
		    comment_ਖਾਨਾ = check_linecomment(linep);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (has_mbyte)
		    pos.ਖਾਨਾ += (*mb_ptr2len)(linep + pos.ਖਾਨਾ);
		ਨਹੀਂਤਾਂ
		    ++pos.ਖਾਨਾ;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ਜੇ (skip_comments && !comment_dir && !inquote)
	{
	    ਜੇ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ਜੇ (!ਵਿੱਚ_ਖੰਡ_comment && pos.ਖਾਨਾ > 0
			&& linep[pos.ਖਾਨਾ - 1] == '*' && linep[pos.ਖਾਨਾ] == '/'
			&& (comment_ਖਾਨਾ == MAXCOL || (ਪੂਰਨ)pos.ਖਾਨਾ < comment_ਖਾਨਾ))
		    ਵਿੱਚ_ਖੰਡ_comment = TRUE;
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਖੰਡ_comment && pos.ਖਾਨਾ > 0
			&& linep[pos.ਖਾਨਾ - 1] == '/' && linep[pos.ਖਾਨਾ] == '*')
		    ਵਿੱਚ_ਖੰਡ_comment = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ਜੇ (!ਵਿੱਚ_ਖੰਡ_comment && linep[pos.ਖਾਨਾ] == '/'
			&& linep[pos.ਖਾਨਾ + 1] == '*'
			&& (comment_ਖਾਨਾ == MAXCOL || (ਪੂਰਨ)pos.ਖਾਨਾ < comment_ਖਾਨਾ))
		    ਵਿੱਚ_ਖੰਡ_comment = TRUE;
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਖੰਡ_comment && pos.ਖਾਨਾ > 0
			&& linep[pos.ਖਾਨਾ - 1] == '*' && linep[pos.ਖਾਨਾ] == '/')
		    ਵਿੱਚ_ਖੰਡ_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ਜੇ (pos.ਖਾਨਾ == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ਜੇ (linep[0] == findc && ਗਿਣਤੀ_2 == 0)	// match!
		ਵਾਪਸ &pos;
	    ਰੋਕੋ;					// out of scope
	}

	ਜੇ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ਜੇ (comment_dir == FORWARD)
	    {
		ਜੇ (linep[pos.ਖਾਨਾ] == '*' && linep[pos.ਖਾਨਾ + 1] == '/')
		{
		    pos.ਖਾਨਾ++;
		    ਵਾਪਸ &pos;
		}
	    }
	    ਨਹੀਂਤਾਂ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ਜੇ (pos.ਖਾਨਾ == 0)
		    ਜਾਰੀ;
		ਨਹੀਂਤਾਂ ਜੇ (raw_string)
		{
		    ਜੇ (linep[pos.ਖਾਨਾ - 1] == 'R'
			&& linep[pos.ਖਾਨਾ] == '"'
			&& vim_strchr(linep + pos.ਖਾਨਾ + 1, '(') != ਨਿਰਾ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ਜੇ (!ਲੱਭੋ_rawstring_ਸਿਰਾ(linep, &pos,
				  ਗਿਣਤੀ_2 > 0 ? &ਮੇਲ_pos : &curwin->w_cursor))
			{
			    ਗਿਣਤੀ_2++;
			    ਮੇਲ_pos = pos;
			    ਮੇਲ_pos.ਖਾਨਾ--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (  linep[pos.ਖਾਨਾ - 1] == '/'
			&& linep[pos.ਖਾਨਾ] == '*'
			&& (pos.ਖਾਨਾ == 1 || linep[pos.ਖਾਨਾ - 2] != '*')
			&& (ਪੂਰਨ)pos.ਖਾਨਾ < comment_ਖਾਨਾ)
		{
		    ਗਿਣਤੀ_2++;
		    ਮੇਲ_pos = pos;
		    ਮੇਲ_pos.ਖਾਨਾ--;
		}
		ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ - 1] == '*' && linep[pos.ਖਾਨਾ] == '/')
		{
		    ਜੇ (ਗਿਣਤੀ_2 > 0)
			pos = ਮੇਲ_pos;
		    ਨਹੀਂਤਾਂ ਜੇ (pos.ਖਾਨਾ > 1 && linep[pos.ਖਾਨਾ - 2] == '/'
					       && (ਪੂਰਨ)pos.ਖਾਨਾ <= comment_ਖਾਨਾ)
			pos.ਖਾਨਾ -= 2;
		    ਨਹੀਂਤਾਂ ਜੇ (ignore_cend)
			ਜਾਰੀ;
		    ਨਹੀਂਤਾਂ
			ਵਾਪਸ ਨਿਰਾ;
		    ਵਾਪਸ &pos;
		}
	    }
	    ਜਾਰੀ;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ਜੇ (cpo_ਮੇਲ)
	    do_quotes = 0;
	ਨਹੀਂਤਾਂ ਜੇ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_ਸ਼ੁਰੂ = do_quotes;
	    ਲਈ (ptr = linep; *ptr; ++ptr)
	    {
		ਜੇ (ptr == linep + pos.ਖਾਨਾ + backwards)
		    at_ਸ਼ੁਰੂ = (do_quotes & 1);
		ਜੇ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ਜੇ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ਜੇ (!do_quotes)
	    {
		inquote = FALSE;
		ਜੇ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ਜੇ (ਸ਼ੁਰੂ_ਵਿੱਚ_quotes == MAYBE)
		    {
			inquote = at_ਸ਼ੁਰੂ;
			ਜੇ (inquote)
			    ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = TRUE;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (backwards)
			inquote = TRUE;
		}
		ਜੇ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ਜੇ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ਜੇ (ਸ਼ੁਰੂ_ਵਿੱਚ_quotes == MAYBE)
			{
			    inquote = at_ਸ਼ੁਰੂ;
			    ਜੇ (inquote)
				ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = TRUE;
			}
			ਨਹੀਂਤਾਂ ਜੇ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ਜੇ (ਸ਼ੁਰੂ_ਵਿੱਚ_quotes == MAYBE)
	    ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = FALSE;

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
	c = PTR2CHAR(linep + pos.ਖਾਨਾ);
	ਚੋਣ (c)
	{
	ਹਾਲਤ NUL:
	    // at end of line without trailing backslash, reset inquote
	    ਜੇ (pos.ਖਾਨਾ == 0 || linep[pos.ਖਾਨਾ - 1] != '\\')
	    {
		inquote = FALSE;
		ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = FALSE;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ਜੇ (do_quotes)
	    {
		ਪੂਰਨ ਖਾਨਾ;

		ਲਈ (ਖਾਨਾ = pos.ਖਾਨਾ - 1; ਖਾਨਾ >= 0; --ਖਾਨਾ)
		    ਜੇ (linep[ਖਾਨਾ] != '\\')
			ਰੋਕੋ;
		ਜੇ ((((ਪੂਰਨ)pos.ਖਾਨਾ - 1 - ਖਾਨਾ) & 1) == 0)
		{
		    inquote = !inquote;
		    ਸ਼ੁਰੂ_ਵਿੱਚ_quotes = FALSE;
		}
	    }
	    ਰੋਕੋ;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	ਹਾਲਤ '\'':
	    ਜੇ (!cpo_ਮੇਲ && initc != '\'' && findc != '\'')
	    {
		ਜੇ (backwards)
		{
		    ਜੇ (pos.ਖਾਨਾ > 1)
		    {
			ਜੇ (linep[pos.ਖਾਨਾ - 2] == '\'')
			{
			    pos.ਖਾਨਾ -= 2;
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ - 2] == '\\'
				  && pos.ਖਾਨਾ > 2 && linep[pos.ਖਾਨਾ - 3] == '\'')
			{
			    pos.ਖਾਨਾ -= 3;
			    ਰੋਕੋ;
			}
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ + 1])	// forward search
		{
		    ਜੇ (linep[pos.ਖਾਨਾ + 1] == '\\'
			   && linep[pos.ਖਾਨਾ + 2] && linep[pos.ਖਾਨਾ + 3] == '\'')
		    {
			pos.ਖਾਨਾ += 3;
			ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (linep[pos.ਖਾਨਾ + 2] == '\'')
		    {
			pos.ਖਾਨਾ += 2;
			ਰੋਕੋ;
		    }
		}
	    }
	    // FALLTHROUGH

	ਮੂਲ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ਜੇ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ਨਿਰਾ
		    && pos.ਖਾਨਾ > 1
		    && check_prevcol(linep, pos.ਖਾਨਾ, '\\', ਨਿਰਾ)
		    && check_prevcol(linep, pos.ਖਾਨਾ - 1, '#', ਨਿਰਾ))
		ਰੋਕੋ;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ਜੇ (skip_comments && (ਵਿੱਚ_ਖੰਡ_comment
		    || (comment_ਖਾਨਾ != MAXCOL && (ਪੂਰਨ)pos.ਖਾਨਾ >= comment_ਖਾਨਾ)))
		ਰੋਕੋ;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ਜੇ ((!inquote || ਸ਼ੁਰੂ_ਵਿੱਚ_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		ਪੂਰਨ	ਖਾਨਾ, bslcnt = 0;

		ਜੇ (!cpo_bsl)
		{
		    ਲਈ (ਖਾਨਾ = pos.ਖਾਨਾ; check_prevcol(linep, ਖਾਨਾ, '\\', &ਖਾਨਾ);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ਜੇ (cpo_bsl || (bslcnt & 1) == ਮੇਲ_escaped)
		{
		    ਜੇ (c == initc)
			ਗਿਣਤੀ_2++;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (ਗਿਣਤੀ_2 == 0)
			    ਵਾਪਸ &pos;
			ਗਿਣਤੀ_2--;
		    }
		}
	    }
	}
    }

    ਜੇ (comment_dir == BACKWARD && ਗਿਣਤੀ_2 > 0)
    {
	pos = ਮੇਲ_pos;
	ਵਾਪਸ &pos;
    }
    ਵਾਪਸ (pos_T *)ਨਿਰਾ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    ਖਾਲੀ
showmatch(
    ਪੂਰਨ		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    ਲੰਮਾ	save_so;
    ਲੰਮਾ	save_siso;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ਪੂਰਨ		save_state;
#ਜੇ_ਅੰਤ
    colnr_T	save_dollar_vcol;
    char_u	*p;
    ਲੰਮਾ	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ਲੰਮਾ	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    ਲਈ (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	p += mb_ptr2len(p) + 1;
	ਜੇ (PTR2CHAR(p) == c
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#ਜੇ_ਅੰਤ
	   )
	    ਰੋਕੋ;
	p += mb_ptr2len(p);
	ਜੇ (*p == NUL)
	    ਵਾਪਸ;
    }
    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    ਜੇ ((lpos = findmatch(ਨਿਰਾ, NUL)) == ਨਿਰਾ)	    // no match, so beep
    {
	vim_beep(BO_ਮੇਲ);
	ਵਾਪਸ;
    }

    ਜੇ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ਵਾਪਸ;

    ਜੇ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ਨਿਰਾ, &vcol, ਨਿਰਾ, 0);

    ਪੂਰਨ ਖਾਨਾ_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_ਚੌੜਾਈ));
    ਜੇ (!ਖਾਨਾ_visible)
	ਵਾਪਸ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ਜੇ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
	dollar_vcol = -1;
    ++curwin->w_virtcol;	// do display ')' just before "$"
    update_screen(UPD_VALID);	// show the new char first

    save_dollar_vcol = dollar_vcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    save_state = State;
    State = ਬਹੁਲਕ_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    ਜੇ (vim_strchr(p_cpo, CPO_SHOWMATCH) != ਨਿਰਾ)
	ui_delay(p_mat * 100L + 8, TRUE);
    ਨਹੀਂਤਾਂ ਜੇ (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
is_zero_ਚੌੜਾਈ(
    char_u	*ਨਮੂਨਾ,
    size_t	patternlen,
    ਪੂਰਨ		move,
    pos_T	*cur,
    ਪੂਰਨ		direction)
{
    regmmatch_T	regmatch;
    ਪੂਰਨ		nmatched = 0;
    ਪੂਰਨ		ਨਤੀਜਾ = -1;
    pos_T	pos;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    ਪੂਰਨ		flag = 0;

    ਜੇ (ਨਮੂਨਾ == ਨਿਰਾ)
    {
	ਨਮੂਨਾ = spats[ਆਖਰੀ_idx].pat;
	patternlen = spats[ਆਖਰੀ_idx].patlen;
    }

    ਜੇ (ਖੋਜ_regcomp(ਨਮੂਨਾ, patternlen, ਨਿਰਾ, RE_ਖੋਜ, RE_ਖੋਜ,
					      ਖੋਜ_KEEP, &regmatch) == FAIL)
	ਵਾਪਸ -1;

    // init startcol correctly
    regmatch.startpos[0].ਖਾਨਾ = -1;
    // move to match
    ਜੇ (move)
    {
	CLEAR_POS(&pos);
    }
    ਨਹੀਂਤਾਂ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = ਖੋਜ_ਸ਼ੁਰੂ;
    }

    ਜੇ (searchit(curwin, curbuf, &pos, ਨਿਰਾ, direction, ਨਮੂਨਾ, patternlen, 1,
				  ਖੋਜ_KEEP + flag, RE_ਖੋਜ, ਨਿਰਾ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	ਕਰੋ
	{
	    regmatch.startpos[0].ਖਾਨਾ++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].ਖਾਨਾ, ਨਿਰਾ);
	    ਜੇ (nmatched != 0)
		ਰੋਕੋ;
	} ਜਦੋਂ (regmatch.regprog != ਨਿਰਾ
		&& (direction == FORWARD ? regmatch.startpos[0].ਖਾਨਾ < pos.ਖਾਨਾ
				      : regmatch.startpos[0].ਖਾਨਾ > pos.ਖਾਨਾ));

	ਜੇ (called_emsg == called_emsg_before)
	{
	    ਨਤੀਜਾ = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].ਖਾਨਾ == regmatch.endpos[0].ਖਾਨਾ);
	}
    }

    vim_regfree(regmatch.regprog);
    ਵਾਪਸ ਨਤੀਜਾ;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ਪੂਰਨ
current_ਖੋਜ(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	ਸ਼ੁਰੂ_pos;	// start position of the pattern match
    pos_T	ਸਿਰਾ_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    ਪੂਰਨ		i;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		ਨਤੀਜਾ;		// result of various function calls
    char_u	old_p_ws = p_ws;
    ਪੂਰਨ		flags = 0;
    pos_T	save_VIsual = VIsual;
    ਪੂਰਨ		zero_ਚੌੜਾਈ;
    ਪੂਰਨ		skip_ਪਹਿਲਾ_backward;

    // Correct cursor when 'selection' is exclusive
    ਜੇ (VIsual_ਸਰਗਰਮ && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_ਪਹਿਲਾ_backward = forward && VIsual_ਸਰਗਰਮ
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (forward)
	    incl(&pos);
	ਨਹੀਂਤਾਂ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_ਚੌੜਾਈ = is_zero_ਚੌੜਾਈ(spats[ਆਖਰੀ_idx].pat, spats[ਆਖਰੀ_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ਜੇ (zero_ਚੌੜਾਈ == -1)
	ਵਾਪਸ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    ਲਈ (i = 0; i < 2; i++)
    {
	ਜੇ (forward)
	{
	    ਜੇ (i == 0 && skip_ਪਹਿਲਾ_backward)
		ਜਾਰੀ;
	    dir = i;
	}
	ਨਹੀਂਤਾਂ
	    dir = !i;

	flags = 0;
	ਜੇ (!dir && !zero_ਚੌੜਾਈ)
	    flags = ਖੋਜ_ਸਿਰਾ;
	ਸਿਰਾ_pos = pos;

	// wrapping should not occur in the first round
	ਜੇ (i == 0)
	    p_ws = FALSE;

	ਨਤੀਜਾ = searchit(curwin, curbuf, &pos, &ਸਿਰਾ_pos,
		(dir ? FORWARD : BACKWARD),
		spats[ਆਖਰੀ_idx].pat, spats[ਆਖਰੀ_idx].patlen, (ਲੰਮਾ) (i ? ਗਿਣਤੀ_2 : 1),
		ਖੋਜ_KEEP | flags, RE_ਖੋਜ, ਨਿਰਾ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	ਜੇ (i == 1 && !ਨਤੀਜਾ) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ਜੇ (VIsual_ਸਰਗਰਮ)
		VIsual = save_VIsual;
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (i == 0 && !ਨਤੀਜਾ)
	{
	    ਜੇ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		pos.ਖਾਨਾ  = ml_get_len(curwin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
	    }
	}
    }

    ਸ਼ੁਰੂ_pos = pos;

    ਜੇ (!VIsual_ਸਰਗਰਮ)
	VIsual = ਸ਼ੁਰੂ_pos;

    // put the cursor after the match
    curwin->w_cursor = ਸਿਰਾ_pos;
    ਜੇ (LT_POS(VIsual, ਸਿਰਾ_pos) && forward)
    {
	ਜੇ (skip_ਪਹਿਲਾ_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ਨਹੀਂਤਾਂ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_ਸਰਗਰਮ = TRUE;
    VIsual_ਬਹੁਲਕ = 'v';

    ਜੇ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ਜੇ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ਨਹੀਂਤਾਂ ਜੇ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (fdo_flags & FDO_ਖੋਜ && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ

    may_ਸ਼ੁਰੂ_select('c');
    setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_ਤਾਰਾ.vmode = NUL;
#ਜੇ_ਅੰਤ
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    ਵਾਪਸ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    ਪੂਰਨ
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ਵਾਪਸ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
cmdline_ਖੋਜ_stat(
    ਪੂਰਨ		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    ਪੂਰਨ		show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ,
    char_u	*msgbuf,
    size_t	msgbuflen,
    ਪੂਰਨ		recompute,
    ਪੂਰਨ		maxcount,
    ਲੰਮਾ	timeout)
{
    searchstat_T stat;

    update_ਖੋਜ_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ਜੇ (stat.cur <= 0)
	ਵਾਪਸ;

    ਅੱਖਰ	t[ਖੋਜ_STAT_BUF_LEN];
    size_t	len;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ਜੇ (stat.incomplete == 1)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[?/??]");
	ਨਹੀਂਤਾਂ ਜੇ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ਨਹੀਂਤਾਂ ਜੇ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ਨਹੀਂਤਾਂ
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਜੇ (stat.incomplete == 1)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[?/??]");
	ਨਹੀਂਤਾਂ ਜੇ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ਨਹੀਂਤਾਂ ਜੇ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ਨਹੀਂਤਾਂ
	    len = vim_snprintf(t, ਖੋਜ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ਜੇ (show_ਸਿਖਰ_bot_ਸੁਨੇਹਾ && len + 2 < ਖੋਜ_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ਜੇ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ਜੇ (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    ਸੁਨੇਹਾ_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    ਸੁਨੇਹਾ_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    ਸਥਿਰ ਖਾਲੀ
update_ਖੋਜ_stat(
    ਪੂਰਨ			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    ਪੂਰਨ			recompute,
    ਪੂਰਨ			maxcount,
    ਲੰਮਾ		timeout ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		    save_ws = p_ws;
    ਪੂਰਨ		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ਸਥਿਰ pos_T    lastpos = {0, 0, 0};
    ਸਥਿਰ ਪੂਰਨ	    cur = 0;
    ਸਥਿਰ ਪੂਰਨ	    cnt = 0;
    ਸਥਿਰ ਪੂਰਨ	    exact_ਮੇਲ = FALSE;
    ਸਥਿਰ ਪੂਰਨ	    incomplete = 0;
    ਸਥਿਰ ਪੂਰਨ	    ਆਖਰੀ_maxcount = 0;
    ਸਥਿਰ ਪੂਰਨ	    chgtick = 0;
    ਸਥਿਰ char_u   *lastpat = ਨਿਰਾ;
    ਸਥਿਰ size_t   lastpatlen = 0;
    ਸਥਿਰ buf_T    *lbuf = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    proftime_T  ਸ਼ੁਰੂ;
#ਜੇ_ਅੰਤ

    CLEAR_ਸੂਚਕ(stat);

    ਜੇ (dirc == 0 && !recompute && !ਖਾਲੀ_ਥਾਵਾਂ_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_ਮੇਲ = exact_ਮੇਲ;
	stat->incomplete = incomplete;
	stat->ਆਖਰੀ_maxcount = ਆਖਰੀ_maxcount;
	ਵਾਪਸ;
    }
    ਆਖਰੀ_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ਜੇ (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ਨਿਰਾ
	    && STRNCMP(lastpat, spats[ਆਖਰੀ_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[ਆਖਰੀ_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_ਮੇਲ = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    ਜੇ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	ਪੂਰਾ_ਖੋਜ = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	ਜੇ (timeout > 0)
	    profile_setlimit(timeout, &ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ਨਿਰਾ, 0, 1, ਖੋਜ_KEEP, RE_ਆਖਰੀ, ਨਿਰਾ) != FAIL)
	{
	    ਪੂਰਾ_ਖੋਜ = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	    // Stop after passing the time limit.
	    ਜੇ (timeout > 0 && profile_passed_ਹੱਦਬੰਦੀ(&ਸ਼ੁਰੂ))
	    {
		incomplete = 1;
		ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	    cnt++;
	    ਜੇ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ਜੇ (LT_POS(p, endpos))
		    exact_ਮੇਲ = TRUE;
	    }
	    fast_breakcheck();
	    ਜੇ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		ਰੋਕੋ;
	    }
	}
	ਜੇ (got_int)
	    cur = -1; // abort
	ਜੇ (ਪੂਰਾ_ਖੋਜ)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[ਆਖਰੀ_idx].pat, spats[ਆਖਰੀ_idx].patlen);
	    ਜੇ (lastpat == ਨਿਰਾ)
		lastpatlen = 0;
	    ਨਹੀਂਤਾਂ
		lastpatlen = spats[ਆਖਰੀ_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_ਮੇਲ = exact_ਮੇਲ;
    stat->incomplete = incomplete;
    stat->ਆਖਰੀ_maxcount = ਆਖਰੀ_maxcount;
    p_ws = save_ws;
}

#ਜੇ defined(FEAT_ਲੱਭੋ_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ਸਥਿਰ char_u *
get_ਸਤਰ_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *ਸਤਰ = ml_get(lnum);

    vim_strncpy(buf, ਸਤਰ, LSIZE - 1);
    ਵਾਪਸ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    ਖਾਲੀ
ਲੱਭੋ_ਨਮੂਨਾ_ਵਿੱਚ_ਰਸਤਾ(
    char_u	*ptr,		// pointer to search pattern
    ਪੂਰਨ		dir ਅਣਵਰਤਿਆ,	// direction of expansion
    ਪੂਰਨ		len,		// length of search pattern
    ਪੂਰਨ		ਸਾਰਾ,		// match whole words only
    ਪੂਰਨ		skip_comments,	// don't match inside comments
    ਪੂਰਨ		type,		// Type of search; are we looking for a type?
				// a macro?
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		action,		// What to do when we find it
    linenr_T	ਸ਼ੁਰੂ_lnum,	// first line to start searching
    linenr_T	ਸਿਰਾ_lnum,	// last line for searching
    ਪੂਰਨ		forceit,	// If true, always switch to the found path
    ਪੂਰਨ		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    ਪੂਰਨ		max_ਰਸਤਾ_ਡੂੰਘਾਈ = 50;
    ਲੰਮਾ	ਮੇਲ_ਗਿਣਤੀ = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ਨਿਰਾ;
    linenr_T	lnum;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    ਪੂਰਨ		ਡੂੰਘਾਈ_displayed;	// For type==CHECK_PATH
    ਪੂਰਨ		old_files;
    ਪੂਰਨ		already_searched;
    char_u	*file_ਸਤਰ;
    char_u	*ਸਤਰ;
    char_u	*p;
    char_u	save_char;
    ਪੂਰਨ		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    ਪੂਰਨ		matched = FALSE;
    ਪੂਰਨ		did_show = FALSE;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;
    ਪੂਰਨ		i;
    char_u	*already = ਨਿਰਾ;
    char_u	*startp = ਨਿਰਾ;
    char_u	*inc_opt = ਨਿਰਾ;
# ਜੇ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ਨਿਰਾ;
# endif

    regmatch.regprog = ਨਿਰਾ;
    incl_regmatch.regprog = ਨਿਰਾ;
    def_regmatch.regprog = ਨਿਰਾ;

    file_ਸਤਰ = alloc(LSIZE);
    ਜੇ (file_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (type != CHECK_ਰਸਤਾ && type != ਲੱਭੋ_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_ਸਥਿਤੀ_sol())
    {
	pat = alloc(len + 5);
	ਜੇ (pat == ਨਿਰਾ)
	    ਜਾਓ fpip_ਸਿਰਾ;
	vim_snprintf((ਅੱਖਰ *)pat, len + 5, ਸਾਰਾ ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ fpip_ਸਿਰਾ;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ਜੇ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ਜੇ (incl_regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ fpip_ਸਿਰਾ;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ਜੇ (type == ਲੱਭੋ_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ਜੇ (def_regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ fpip_ਸਿਰਾ;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_ਰਸਤਾ_ਡੂੰਘਾਈ * ਆਕਾਰ(SearchedFile), TRUE);
    ਜੇ (files == ਨਿਰਾ)
	ਜਾਓ fpip_ਸਿਰਾ;
    old_files = max_ਰਸਤਾ_ਡੂੰਘਾਈ;
    ਡੂੰਘਾਈ = ਡੂੰਘਾਈ_displayed = -1;

    lnum = ਸ਼ੁਰੂ_lnum;
    ਜੇ (ਸਿਰਾ_lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਸਿਰਾ_lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (lnum > ਸਿਰਾ_lnum)		// do at least one line
	lnum = ਸਿਰਾ_lnum;
    ਸਤਰ = get_ਸਤਰ_and_copy(lnum, file_ਸਤਰ);

    ਲਈ (;;)
    {
	ਜੇ (incl_regmatch.regprog != ਨਿਰਾ
		&& vim_regexec(&incl_regmatch, ਸਤਰ, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)inc_opt, "\\zs") != ਨਿਰਾ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = ਲੱਭੋ_file_name_ਵਿੱਚ_ਰਸਤਾ(incl_regmatch.startp[0],
		       (ਪੂਰਨ)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ਨਹੀਂਤਾਂ
		// Use text after match with 'include'.
		new_fname = file_name_ਵਿੱਚ_ਸਤਰ(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ਨਿਰਾ);
	    already_searched = FALSE;
	    ਜੇ (new_fname != ਨਿਰਾ)
	    {
		// Check whether we have already searched in this file
		ਲਈ (i = 0;; i++)
		{
		    ਜੇ (i == ਡੂੰਘਾਈ + 1)
			i = old_files;
		    ਜੇ (i == max_ਰਸਤਾ_ਡੂੰਘਾਈ)
			ਰੋਕੋ;
		    ਜੇ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ਜੇ (type != CHECK_ਰਸਤਾ
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    ਸੁਨੇਹਾ_putchar('\n');	    // cursor below last one
			    ਜੇ (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //ਸੁਨੇਹਾ
			    {
				ਸੁਨੇਹਾ_home_replace_hl(new_fname);
				ਸੁਨੇਹਾ_puts(_(" (includes previously listed match)"));
				prev_fname = ਨਿਰਾ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			ਰੋਕੋ;
		    }
		}
	    }

	    ਜੇ (type == CHECK_ਰਸਤਾ && (action == ACTION_SHOW_ALL
				 || (new_fname == ਨਿਰਾ && !already_searched)))
	    {
		ਜੇ (did_show)
		    ਸੁਨੇਹਾ_putchar('\n');	    // cursor below last one
		ਨਹੀਂਤਾਂ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    ਸੁਨੇਹਾ_puts_title(_("--- Included files "));
		    ਜੇ (action != ACTION_SHOW_ALL)
			ਸੁਨੇਹਾ_puts_title(_("not found "));
		    ਸੁਨੇਹਾ_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ਜਦੋਂ (ਡੂੰਘਾਈ_displayed < ਡੂੰਘਾਈ && !got_int)
		{
		    ++ਡੂੰਘਾਈ_displayed;
		    ਲਈ (i = 0; i < ਡੂੰਘਾਈ_displayed; i++)
			ਸੁਨੇਹਾ_puts("  ");
		    ਸੁਨੇਹਾ_home_replace(files[ਡੂੰਘਾਈ_displayed].name);
		    ਸੁਨੇਹਾ_puts(" -->\n");
		}
		ਜੇ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    ਲਈ (i = 0; i <= ਡੂੰਘਾਈ_displayed; i++)
			ਸੁਨੇਹਾ_puts("  ");
		    ਜੇ (new_fname != ਨਿਰਾ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			ਸੁਨੇਹਾ_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ਨਹੀਂਤਾਂ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)inc_opt, "\\zs") != ਨਿਰਾ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (ਪੂਰਨ)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ਨਹੀਂਤਾਂ
			{
			    // find the file name after the end of the match
			    ਲਈ (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    ਲਈ (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ਜੇ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (ਪੂਰਨ)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			ਨਹੀਂਤਾਂ ਜੇ (p > ਸਤਰ)
			{
			    ਜੇ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ਜੇ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			ਸੁਨੇਹਾ_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ਜੇ (new_fname == ਨਿਰਾ && action == ACTION_SHOW_ALL)
		    {
			ਜੇ (already_searched)
			    ਸੁਨੇਹਾ_puts(_("  (Already listed)"));
			ਨਹੀਂਤਾਂ
			    ਸੁਨੇਹਾ_puts(_("  NOT FOUND"));
		    }
		}
		ਬਾਹਰਲਾ_flush();	    // output each line directly
	    }

	    ਜੇ (new_fname != ਨਿਰਾ)
	    {
		// Push the new file onto the file stack
		ਜੇ (ਡੂੰਘਾਈ + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_ਰਸਤਾ_ਡੂੰਘਾਈ * 2);
		    ਜੇ (bigger != ਨਿਰਾ)
		    {
			ਲਈ (i = 0; i <= ਡੂੰਘਾਈ; i++)
			    bigger[i] = files[i];
			ਲਈ (i = ਡੂੰਘਾਈ + 1; i < old_files + max_ਰਸਤਾ_ਡੂੰਘਾਈ; i++)
			{
			    bigger[i].fp = ਨਿਰਾ;
			    bigger[i].name = ਨਿਰਾ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			ਲਈ (i = old_files; i < max_ਰਸਤਾ_ਡੂੰਘਾਈ; i++)
			    bigger[i + max_ਰਸਤਾ_ਡੂੰਘਾਈ] = files[i];
			old_files += max_ਰਸਤਾ_ਡੂੰਘਾਈ;
			max_ਰਸਤਾ_ਡੂੰਘਾਈ *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ਜੇ ((files[ਡੂੰਘਾਈ + 1].fp = mch_fopen((ਅੱਖਰ *)new_fname, "r"))
								    == ਨਿਰਾ)
		    vim_free(new_fname);
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (++ਡੂੰਘਾਈ == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[ਡੂੰਘਾਈ].name = curr_fname = new_fname;
		    files[ਡੂੰਘਾਈ].lnum = 0;
		    files[ਡੂੰਘਾਈ].matched = FALSE;
		    ਜੇ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			ਸੁਨੇਹਾ_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((ਅੱਖਰ*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(ਅੱਖਰ *)new_fname);
			ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (ਅੱਖਰ *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = ਸਤਰ;
ਖੋਜ_ਸਤਰ:
	    define_matched = FALSE;
	    ਜੇ (def_regmatch.regprog != ਨਿਰਾ
			      && vim_regexec(&def_regmatch, ਸਤਰ, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ਜਦੋਂ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ਜੇ (def_regmatch.regprog == ਨਿਰਾ || define_matched)
	    {
		ਜੇ (define_matched || compl_ਸਥਿਤੀ_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ਜੇ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ਨਹੀਂਤਾਂ
			matched = STRNCMP(startp, ptr, len) == 0;
		    ਜੇ (matched && define_matched && ਸਾਰਾ
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (regmatch.regprog != ਨਿਰਾ
			 && vim_regexec(&regmatch, ਸਤਰ, (colnr_T)(p - ਸਤਰ)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ਜੇ (!define_matched && skip_comments)
		    {
			ਜੇ ((*ਸਤਰ != '#' ||
				STRNCMP(skipwhite(ਸਤਰ + 1), "define", 6) != 0)
				&& get_leader_len(ਸਤਰ, ਨਿਰਾ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(ਸਤਰ);
			ਜੇ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    ਲਈ (p = ਸਤਰ; *p && p < startp; ++p)
			    {
				ਜੇ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ਜੇ (p[1] == '/')
					ਰੋਕੋ;
				    ++p;
				}
				ਨਹੀਂਤਾਂ ਜੇ (!matched && p[0] == '*' && p[1] == '/')
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
	ਜੇ (matched)
	{
	    ਜੇ (action == ACTION_EXPAND)
	    {
		ਪੂਰਨ	cont_s_ipos = FALSE;
		ਪੂਰਨ	ਜੋੜੋ_r;
		char_u	*aux;

		ਜੇ (ਡੂੰਘਾਈ == -1 && lnum == curwin->w_cursor.lnum)
		    ਰੋਕੋ;
		ਮਿਲਿਆ = TRUE;
		aux = p = startp;
		ਜੇ (compl_ਸਥਿਤੀ_adding() && (ਪੂਰਨ)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ਜੇ (vim_iswordp(p))
			ਜਾਓ exit_matched;
		    p = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(p);
		}
		p = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(p);
		i = (ਪੂਰਨ)(p - aux);

		ਜੇ (compl_ਸਥਿਤੀ_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ਜੇ (ਡੂੰਘਾਈ < 0)
		    {
			ਜੇ (lnum >= ਸਿਰਾ_lnum)
			    ਜਾਓ exit_matched;
			ਸਤਰ = get_ਸਤਰ_and_copy(++lnum, file_ਸਤਰ);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (vim_fgets(ਸਤਰ = file_ਸਤਰ,
						      LSIZE, files[ਡੂੰਘਾਈ].fp))
			ਜਾਓ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(ਸਤਰ);
		    p = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(p);
		    p = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(p);
		    ਜੇ (p > aux)
		    {
			ਜੇ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ਜੇ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ਜੇ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ਨਿਰਾ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ਜੇ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (ਪੂਰਨ)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ਜੇ (i == ins_compl_len())
			ਜਾਓ exit_matched;
		}

		ਜੋੜੋ_r = ins_compl_ਜੋੜੋ_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ਨਿਰਾ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ਜੇ (ਜੋੜੋ_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ਨਹੀਂਤਾਂ ਜੇ (ਜੋੜੋ_r == FAIL)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (action == ACTION_SHOW_ALL)
	    {
		ਮਿਲਿਆ = TRUE;
		ਜੇ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ਜੇ (curr_fname != prev_fname)
		{
		    ਜੇ (did_show)
			ਸੁਨੇਹਾ_putchar('\n');	// cursor below last one
		    ਜੇ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			ਸੁਨੇਹਾ_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ਜੇ (!got_int)
		    show_pat_ਵਿੱਚ_ਰਸਤਾ(ਸਤਰ, type, TRUE, action,
			    (ਡੂੰਘਾਈ == -1) ? ਨਿਰਾ : files[ਡੂੰਘਾਈ].fp,
			    (ਡੂੰਘਾਈ == -1) ? &lnum : &files[ਡੂੰਘਾਈ].lnum,
			    ਮੇਲ_ਗਿਣਤੀ++);

		// Set matched flag for this file and all the ones that
		// include it
		ਲਈ (i = 0; i <= ਡੂੰਘਾਈ; ++i)
		    files[i].matched = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (--ਗਿਣਤੀ_2 <= 0)
	    {
		ਮਿਲਿਆ = TRUE;
		ਜੇ (ਡੂੰਘਾਈ == -1 && lnum == curwin->w_cursor.lnum
# ਜੇ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_ਮੇਲ_is_on_current_ਸਤਰ));
		ਨਹੀਂਤਾਂ ਜੇ (action == ACTION_SHOW)
		{
		    show_pat_ਵਿੱਚ_ਰਸਤਾ(ਸਤਰ, type, did_show, action,
			    (ਡੂੰਘਾਈ == -1) ? ਨਿਰਾ : files[ਡੂੰਘਾਈ].fp,
			    (ਡੂੰਘਾਈ == -1) ? &lnum : &files[ਡੂੰਘਾਈ].lnum, 1L);
		    did_show = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ਜੇ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ਜੇ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ਜੇ (action == ACTION_ਵੰਡੋ)
		    {
			ਜੇ (win_ਵੰਡੋ(0, 0) == FAIL)
			    ਰੋਕੋ;
			RESET_BINDING(curwin);
		    }
		    ਜੇ (ਡੂੰਘਾਈ == -1)
		    {
			// match in current file
# ਜੇ defined(FEAT_QUICKFIX)
			ਜੇ (g_do_tagpreview != 0)
			{
			    ਜੇ (!win_valid(curwin_save))
				ਰੋਕੋ;
			    ਜੇ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_ਬਫਰ->b_fnum, ਨਿਰਾ,
						     ਨਿਰਾ, TRUE, lnum, forceit)))
				ਰੋਕੋ;	// failed to jump to file
			}
			ਨਹੀਂਤਾਂ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (!GETFILE_SUCCESS(getfile(
					0, files[ਡੂੰਘਾਈ].name, ਨਿਰਾ, TRUE,
						    files[ਡੂੰਘਾਈ].lnum, forceit)))
			    ਰੋਕੋ;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[ਡੂੰਘਾਈ].lnum;
		    }
		}
		ਜੇ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.ਖਾਨਾ = (colnr_T)(startp - ਸਤਰ);
		    curwin->w_ਸੈੱਟ_curswant = true;
		}

# ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		ਰੋਕੋ;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ਜੇ (def_regmatch.regprog == ਨਿਰਾ
		    && action == ACTION_EXPAND
		    && !compl_ਸਥਿਤੀ_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		ਜਾਓ ਖੋਜ_ਸਤਰ;
	}
	ਸਤਰ_breakcheck();
	ਜੇ (action == ACTION_EXPAND)
	    ins_compl_check_ਕੁੰਜੀਆਂ(30, FALSE);
	ਜੇ (got_int || ins_compl_interrupted())
	    ਰੋਕੋ;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !already
		&& vim_fgets(ਸਤਰ = file_ਸਤਰ, LSIZE, files[ਡੂੰਘਾਈ].fp))
	{
	    fclose(files[ਡੂੰਘਾਈ].fp);
	    --old_files;
	    files[old_files].name = files[ਡੂੰਘਾਈ].name;
	    files[old_files].matched = files[ਡੂੰਘਾਈ].matched;
	    --ਡੂੰਘਾਈ;
	    curr_fname = (ਡੂੰਘਾਈ == -1) ? curbuf->b_fname
				       : files[ਡੂੰਘਾਈ].name;
	    ਜੇ (ਡੂੰਘਾਈ < ਡੂੰਘਾਈ_displayed)
		ਡੂੰਘਾਈ_displayed = ਡੂੰਘਾਈ;
	}
	ਜੇ (ਡੂੰਘਾਈ >= 0)		// we could read the line
	{
	    files[ਡੂੰਘਾਈ].lnum++;
	    // Remove any CR and LF from the line.
	    i = (ਪੂਰਨ)STRLEN(ਸਤਰ);
	    ਜੇ (i > 0 && ਸਤਰ[i - 1] == '\n')
		ਸਤਰ[--i] = NUL;
	    ਜੇ (i > 0 && ਸਤਰ[i - 1] == '\r')
		ਸਤਰ[--i] = NUL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!already)
	{
	    ਜੇ (++lnum > ਸਿਰਾ_lnum)
		ਰੋਕੋ;
	    ਸਤਰ = get_ਸਤਰ_and_copy(lnum, file_ਸਤਰ);
	}
	already = ਨਿਰਾ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    ਲਈ (i = 0; i <= ਡੂੰਘਾਈ; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    ਲਈ (i = old_files; i < max_ਰਸਤਾ_ਡੂੰਘਾਈ; i++)
	vim_free(files[i].name);
    vim_free(files);

    ਜੇ (type == CHECK_ਰਸਤਾ)
    {
	ਜੇ (!did_show)
	{
	    ਜੇ (action != ACTION_SHOW_ALL)
		ਸੁਨੇਹਾ(_("All included files were found"));
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ(_("No included files"));
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!ਮਿਲਿਆ && action != ACTION_EXPAND && !silent)
    {
	ਜੇ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ਨਹੀਂਤਾਂ ਜੇ (type == ਲੱਭੋ_DEFINE)
	    emsg(_(e_couldnt_ਲੱਭੋ_definition));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_couldnt_ਲੱਭੋ_ਨਮੂਨਾ));
    }
    ਜੇ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	ਸੁਨੇਹਾ_ਸਿਰਾ();

fpip_ਸਿਰਾ:
    vim_free(file_ਸਤਰ);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ਸਥਿਰ ਖਾਲੀ
show_pat_ਵਿੱਚ_ਰਸਤਾ(
    char_u  *ਸਤਰ,
    ਪੂਰਨ	    type,
    ਪੂਰਨ	    did_show,
    ਪੂਰਨ	    action,
    FILE    *fp,
    linenr_T *lnum,
    ਲੰਮਾ    ਗਿਣਤੀ_2)
{
    char_u  *p;
    size_t  linelen;

    ਜੇ (did_show)
	ਸੁਨੇਹਾ_putchar('\n');	// cursor below last one
    ਨਹੀਂਤਾਂ ਜੇ (!ਸੁਨੇਹਾ_silent)
	gotocmdline(TRUE);	// cursor at status line
    ਜੇ (got_int)		// 'q' typed at "--more--" message
	ਵਾਪਸ;
    linelen = STRLEN(ਸਤਰ);
    ਲਈ (;;)
    {
	p = ਸਤਰ + linelen - 1;
	ਜੇ (fp != ਨਿਰਾ)
	{
	    // We used fgets(), so get rid of newline at end
	    ਜੇ (p >= ਸਤਰ && *p == '\n')
		--p;
	    ਜੇ (p >= ਸਤਰ && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ਜੇ (action == ACTION_SHOW_ALL)
	{
	    sprintf((ਅੱਖਰ *)IObuff, "%3ld: ", ਗਿਣਤੀ_2);	// show match nr
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	    sprintf((ਅੱਖਰ *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)IObuff, HL_ATTR(HLF_N));
	    ਸੁਨੇਹਾ_puts(" ");
	}
	ਸੁਨੇਹਾ_prt_ਸਤਰ(ਸਤਰ, FALSE);
	ਬਾਹਰਲਾ_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ਜੇ (got_int || type != ਲੱਭੋ_DEFINE || p < ਸਤਰ || *p != '\\')
	    ਰੋਕੋ;

	ਜੇ (fp != ਨਿਰਾ)
	{
	    ਜੇ (vim_fgets(ਸਤਰ, LSIZE, fp)) // end of file
		ਰੋਕੋ;
	    linelen = STRLEN(ਸਤਰ);
	    ++*lnum;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (++*lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    ਸਤਰ = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	ਸੁਨੇਹਾ_putchar('\n');
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(ਪੂਰਨ idx)
{
    ਵਾਪਸ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    ਪੂਰਨ
get_spat_ਆਖਰੀ_idx(ਖਾਲੀ)
{
    ਵਾਪਸ ਆਖਰੀ_idx;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    ਖਾਲੀ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*ਨਮੂਨਾ = ਨਿਰਾ;
    ਪੂਰਨ			maxcount = p_msc;
    ਲੰਮਾ		timeout = ਖੋਜ_STAT_DEF_TIMEOUT;
    ਪੂਰਨ			recompute = TRUE;
    searchstat_T	stat;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	ਪੂਰਨ		error = FALSE;

	ਜੇ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ਵਾਪਸ;
	dict = argvars[0].vval.v_dict;
	di = dict_ਲੱਭੋ(dict, (char_u *)"timeout", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    timeout = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &error);
	    ਜੇ (error)
		ਵਾਪਸ;
	}
	di = dict_ਲੱਭੋ(dict, (char_u *)"maxcount", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    maxcount = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &error);
	    ਜੇ (error)
		ਵਾਪਸ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_ਲੱਭੋ(dict, (char_u *)"pattern", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਨਮੂਨਾ = tv_get_string_chk(&di->di_tv);
	    ਜੇ (ਨਮੂਨਾ == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	di = dict_ਲੱਭੋ(dict, (char_u *)"pos", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), "pos");
		ਵਾਪਸ;
	    }
	    ਜੇ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), "List format should be [lnum, col, off]");
		ਵਾਪਸ;
	    }
	    li = list_ਲੱਭੋ(di->di_tv.vval.v_list, 0L);
	    ਜੇ (li != ਨਿਰਾ)
	    {
		pos.lnum = tv_get_ਸੰਖਿਆ_chk(&li->li_tv, &error);
		ਜੇ (error)
		    ਵਾਪਸ;
	    }
	    li = list_ਲੱਭੋ(di->di_tv.vval.v_list, 1L);
	    ਜੇ (li != ਨਿਰਾ)
	    {
		pos.ਖਾਨਾ = tv_get_ਸੰਖਿਆ_chk(&li->li_tv, &error) - 1;
		ਜੇ (error)
		    ਵਾਪਸ;
	    }
	    li = list_ਲੱਭੋ(di->di_tv.vval.v_list, 2L);
	    ਜੇ (li != ਨਿਰਾ)
	    {
		pos.coladd = tv_get_ਸੰਖਿਆ_chk(&li->li_tv, &error);
		ਜੇ (error)
		    ਵਾਪਸ;
	    }
	}
    }

    save_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
# ifdef FEAT_ਖੋਜ_EXTRA
    save_incsearch_state();
# endif
    ਜੇ (ਨਮੂਨਾ != ਨਿਰਾ)
    {
	ਜੇ (*ਨਮੂਨਾ == NUL)
	    ਜਾਓ the_ਸਿਰਾ;
	vim_free(spats[ਆਖਰੀ_idx].pat);
	spats[ਆਖਰੀ_idx].patlen = STRLEN(ਨਮੂਨਾ);
	spats[ਆਖਰੀ_idx].pat = vim_strnsave(ਨਮੂਨਾ, spats[ਆਖਰੀ_idx].patlen);
	ਜੇ (spats[ਆਖਰੀ_idx].pat == ਨਿਰਾ)
	    spats[ਆਖਰੀ_idx].patlen = 0;
    }
    ਜੇ (spats[ਆਖਰੀ_idx].pat == ਨਿਰਾ || *spats[ਆਖਰੀ_idx].pat == NUL)
	ਜਾਓ the_ਸਿਰਾ;	// the previous pattern was never defined

    update_ਖੋਜ_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "current", stat.cur);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "total", stat.cnt);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "exact_match", stat.exact_ਮੇਲ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "maxcount", stat.ਆਖਰੀ_maxcount);

the_ਸਿਰਾ:
    restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
# ifdef FEAT_ਖੋਜ_EXTRA
    restore_incsearch_state();
# endif
}
#ਜੇ_ਅੰਤ
