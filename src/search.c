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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ set_vv_searchforward(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
ಸ್ಥಿರ char_u *get_ಸಾಲು_and_copy(linenr_T lnum, char_u *buf);
ಸ್ಥಿರ ಶೂನ್ಯ show_pat_in_path(char_u *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, FILE *, linenr_T *, long);
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ searchstat
{
    ಪೂರ್ಣಾಙ್ಕ	    cur;	    // current position of found words
    ಪೂರ್ಣಾಙ್ಕ	    cnt;	    // total count of found words
    ಪೂರ್ಣಾಙ್ಕ	    exact_match;    // TRUE if matched exactly on specified position
    ಪೂರ್ಣಾಙ್ಕ	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    ಪೂರ್ಣಾಙ್ಕ	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಶೂನ್ಯ save_incsearch_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ restore_incsearch_state(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_prevcol(char_u *linep, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ ch, ಪೂರ್ಣಾಙ್ಕ *prevcol);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
ಸ್ಥಿರ ಶೂನ್ಯ find_mps_ಮೌಲ್ಯಗಳು(ಪೂರ್ಣಾಙ್ಕ *initc, ಪೂರ್ಣಾಙ್ಕ *findc, ಪೂರ್ಣಾಙ್ಕ *backwards, ಪೂರ್ಣಾಙ್ಕ switchit);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_zero_width(char_u *pattern, size_t patternlen, ಪೂರ್ಣಾಙ್ಕ move, pos_T *cur, ಪೂರ್ಣಾಙ್ಕ direction);
ಸ್ಥಿರ ಶೂನ್ಯ cmdline_ಹುಡುಕು_stat(ಪೂರ್ಣಾಙ್ಕ dirc, pos_T *pos, pos_T *cursor_pos, ಪೂರ್ಣಾಙ್ಕ show_ತುದಿ_bot_ಸಂದೇಶ, char_u *msgbuf, size_t msgbuflen, ಪೂರ್ಣಾಙ್ಕ recompute, ಪೂರ್ಣಾಙ್ಕ maxcount, long timeout);
ಸ್ಥಿರ ಶೂನ್ಯ update_ಹುಡುಕು_stat(ಪೂರ್ಣಾಙ್ಕ dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, ಪೂರ್ಣಾಙ್ಕ recompute, ಪೂರ್ಣಾಙ್ಕ maxcount, long timeout);

#define ಹುಡುಕು_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define ಹುಡುಕು_STAT_BUF_LEN 16

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
ಸ್ಥಿರ spat_T spats[2] =
{
    {ರಿಕ್ತ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ರಿಕ್ತ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_idx = 0;	// index in spats[] for RE_LAST

ಸ್ಥಿರ char_u lastc[2] = {NUL, NUL};	// last character searched for
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lastcdir = FORWARD;		// last direction of character search
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_t_cmd = TRUE;		// last search t_cmd
ಸ್ಥಿರ char_u	lastc_bytes[MB_MAXBYTES + 1];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ಸ್ಥಿರ spat_T	    saved_spats[ARRAY_LENGTH(spats)];
ಸ್ಥಿರ char_u	    *saved_mr_pattern = ರಿಕ್ತ;
ಸ್ಥಿರ size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_spats_last_idx = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
ಸ್ಥಿರ char_u	    *mr_pattern = ರಿಕ್ತ;
ಸ್ಥಿರ size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    ಪೂರ್ಣಾಙ್ಕ		matched;	// Found a match in this file
} SearchedFile;
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
ಹುಡುಕು_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    ಪೂರ್ಣಾಙ್ಕ		pat_save,
    ಪೂರ್ಣಾಙ್ಕ		pat_use,
    ಪೂರ್ಣಾಙ್ಕ		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    ಪೂರ್ಣಾಙ್ಕ		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ಯದಿ (pat == ರಿಕ್ತ || *pat == NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ (pat_use == RE_LAST)
	    i = last_idx;
	ಅನ್ಯಥಾ
	    i = pat_use;
	ಯದಿ (spats[i].pat == ರಿಕ್ತ)	// pattern was never defined
	{
	    ಯದಿ (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    ಅನ್ಯಥಾ
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ಅನ್ಯಥಾ ಯದಿ (options & ಹುಡುಕು_HIS)	// put new pattern in history
	add_to_history(HIST_ಹುಡುಕು, pat, patlen, TRUE, NUL);

    ಯದಿ (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    ಅನ್ಯಥಾ
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    ಯದಿ (mr_pattern == ರಿಕ್ತ)
	mr_patternlen = 0;
    ಅನ್ಯಥಾ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ಯದಿ (!(options & ಹುಡುಕು_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ಯದಿ (pat_save == RE_ಹುಡುಕು || pat_save == RE_BOTH)
	    save_re_pat(RE_ಹುಡುಕು, pat, patlen, magic);
	// substitute or global command
	ಯದಿ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ಯದಿ (regmatch->regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_ಹುಡುಕು_pat(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ mr_pattern;
}

    ಶೂನ್ಯ
save_re_pat(ಪೂರ್ಣಾಙ್ಕ idx, char_u *pat, size_t patlen, ಪೂರ್ಣಾಙ್ಕ magic)
{
    ಯದಿ (spats[idx].pat == pat)
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ಯದಿ (spats[idx].pat == ರಿಕ್ತ)
	spats[idx].patlen = 0;
    ಅನ್ಯಥಾ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ಯದಿ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_level = 0;

    ಶೂನ್ಯ
save_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (save_level++ != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ಯದಿ (spats[i].pat != ರಿಕ್ತ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ಯದಿ (saved_spats[i].pat == ರಿಕ್ತ)
		saved_spats[i].patlen = 0;
	    ಅನ್ಯಥಾ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ಯದಿ (mr_pattern == ರಿಕ್ತ)
	saved_mr_pattern = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    ಯದಿ (saved_mr_pattern == ರಿಕ್ತ)
	saved_mr_patternlen = 0;
    ಅನ್ಯಥಾ
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    ಶೂನ್ಯ
restore_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (--save_level != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ಯದಿ defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_ಹುಡುಕು_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ಸ್ಥಿರ spat_T	    saved_last_ಹುಡುಕು_spat;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    did_save_last_ಹುಡುಕು_spat = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_last_idx = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_no_hlsearch = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_ಹುಡುಕು_match_endcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    saved_ಹುಡುಕು_match_ಸಾಲುಗಳು;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    ಶೂನ್ಯ
save_last_ಹುಡುಕು_pattern(ಶೂನ್ಯ)
{
    ಯದಿ (++did_save_last_ಹುಡುಕು_spat != 1)
	// nested call, nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    saved_last_ಹುಡುಕು_spat = spats[RE_ಹುಡುಕು];
    ಯದಿ (spats[RE_ಹುಡುಕು].pat != ರಿಕ್ತ)
    {
	saved_last_ಹುಡುಕು_spat.pat = vim_strnsave(spats[RE_ಹುಡುಕು].pat, spats[RE_ಹುಡುಕು].patlen);
	ಯದಿ (saved_last_ಹುಡುಕು_spat.pat == ರಿಕ್ತ)
	    saved_last_ಹುಡುಕು_spat.patlen = 0;
	ಅನ್ಯಥಾ
	    saved_last_ಹುಡುಕು_spat.patlen = spats[RE_ಹುಡುಕು].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    ಶೂನ್ಯ
restore_last_ಹುಡುಕು_pattern(ಶೂನ್ಯ)
{
    ಯದಿ (--did_save_last_ಹುಡುಕು_spat > 0)
	// nested call, nothing to do
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (did_save_last_ಹುಡುಕು_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ಪ್ರತ್ಯಾಗಮ;
    }

    vim_free(spats[RE_ಹುಡುಕು].pat);
    spats[RE_ಹುಡುಕು] = saved_last_ಹುಡುಕು_spat;
    saved_last_ಹುಡುಕು_spat.pat = ರಿಕ್ತ;
    saved_last_ಹುಡುಕು_spat.patlen = 0;
# ಯದಿ defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    last_idx = saved_last_idx;
    set_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_incsearch_state(ಶೂನ್ಯ)
{
    saved_ಹುಡುಕು_match_endcol = ಹುಡುಕು_match_endcol;
    saved_ಹುಡುಕು_match_ಸಾಲುಗಳು  = ಹುಡುಕು_match_ಸಾಲುಗಳು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_incsearch_state(ಶೂನ್ಯ)
{
    ಹುಡುಕು_match_endcol = saved_ಹುಡುಕು_match_endcol;
    ಹುಡುಕು_match_ಸಾಲುಗಳು  = saved_ಹುಡುಕು_match_ಸಾಲುಗಳು;
}

    char_u *
last_ಹುಡುಕು_pattern(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ spats[RE_ಹುಡುಕು].pat;
}

    size_t
last_ಹುಡುಕು_pattern_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ spats[RE_ಹುಡುಕು].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    ಪೂರ್ಣಾಙ್ಕ
ignorecase(char_u *pat)
{
    ಪ್ರತ್ಯಾಗಮ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    ಪೂರ್ಣಾಙ್ಕ
ignorecase_opt(char_u *pat, ಪೂರ್ಣಾಙ್ಕ ic_in, ಪೂರ್ಣಾಙ್ಕ scs)
{
    ಪೂರ್ಣಾಙ್ಕ		ic = ic_in;

    ಯದಿ (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ಪ್ರತ್ಯಾಗಮ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    ಪೂರ್ಣಾಙ್ಕ
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (ಶೂನ್ಯ)skip_regexp_ex(pat, NUL, magic_isset(), ರಿಕ್ತ, ರಿಕ್ತ, &magic_val);

    ಯಾವತ್ (*p != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ		l;

	ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ಯದಿ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    p += l;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ಯದಿ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ಅನ್ಯಥಾ ಯದಿ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ಅನ್ಯಥಾ ಯದಿ (p[1] != NUL)  // skip "\X"
		p += 2;
	    ಅನ್ಯಥಾ
		p += 1;
	}
	ಅನ್ಯಥಾ ಯದಿ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ಯದಿ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ಅನ್ಯಥಾ
		p++;
	}
	ಅನ್ಯಥಾ ಯದಿ (MB_ISUPPER(*p))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಅನ್ಯಥಾ
	    ++p;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_EVAL)
    char_u *
last_csearch(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ lastc_bytes;
}

    ಪೂರ್ಣಾಙ್ಕ
last_csearch_forward(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ lastcdir == FORWARD;
}

    ಪೂರ್ಣಾಙ್ಕ
last_csearch_until(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ last_t_cmd == TRUE;
}

    ಶೂನ್ಯ
set_last_csearch(ಪೂರ್ಣಾಙ್ಕ c, char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    *lastc = c;
    lastc_bytelen = len;
    ಯದಿ (len)
	memcpy(lastc_bytes, s, len);
    ಅನ್ಯಥಾ
	CLEAR_FIELD(lastc_bytes);
}
#endif

    ಶೂನ್ಯ
set_csearch_direction(ಪೂರ್ಣಾಙ್ಕ cdir)
{
    lastcdir = cdir;
}

    ಶೂನ್ಯ
set_csearch_until(ಪೂರ್ಣಾಙ್ಕ t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_ಹುಡುಕು_pat(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    ಶೂನ್ಯ
reset_ಹುಡುಕು_dir(ಶೂನ್ಯ)
{
    spats[0].off.dir = '/';
#ಯದಿ defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    ಶೂನ್ಯ
set_last_ಹುಡುಕು_pat(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		magic,
    ಪೂರ್ಣಾಙ್ಕ		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ಯದಿ (*s == NUL)
	spats[idx].pat = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ಯದಿ (spats[idx].pat == ರಿಕ್ತ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ಯದಿ defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.ಸಾಲು = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    ಯದಿ (setlast)
	last_idx = idx;
    ಯದಿ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ಯದಿ (spats[idx].pat == ರಿಕ್ತ)
	    saved_spats[idx].pat = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ಯದಿ (saved_spats[idx].pat == ರಿಕ್ತ)
	    saved_spats[idx].patlen = 0;
	ಅನ್ಯಥಾ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_ಹುಡುಕು_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ಯದಿ (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_ಹುಡುಕು_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    ಶೂನ್ಯ
last_pat_prog(regmmatch_T *regmatch)
{
    ಯದಿ (spats[last_idx].pat == ರಿಕ್ತ)
    {
	regmatch->regprog = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (ಶೂನ್ಯ)ಹುಡುಕು_regcomp((char_u *)"", 0, ರಿಕ್ತ, 0, last_idx, ಹುಡುಕು_KEEP, regmatch);
    --emsg_off;
}
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    ಪೂರ್ಣಾಙ್ಕ		dir,
    char_u	*pat,
    size_t	patlen,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		options,
    ಪೂರ್ಣಾಙ್ಕ		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    ಪೂರ್ಣಾಙ್ಕ		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    ಪೂರ್ಣಾಙ್ಕ		loop;
    pos_T	start_pos;
    ಪೂರ್ಣಾಙ್ಕ		at_first_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		extra_col;
    ಪೂರ್ಣಾಙ್ಕ		start_char_len;
    ಪೂರ್ಣಾಙ್ಕ		match_ok;
    long	nmatched;
    ಪೂರ್ಣಾಙ್ಕ		submatch = 0;
    ಪೂರ್ಣಾಙ್ಕ		first_match = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    ಪೂರ್ಣಾಙ್ಕ		unused_timeout_flag = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		*timed_out = &unused_timeout_flag;  // set when timed out.
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_from_match_end;		    // vi-compatible search?

    ಯದಿ (ಹುಡುಕು_regcomp(pat, patlen, ರಿಕ್ತ, RE_ಹುಡುಕು, pat_use,
		   (options & (ಹುಡುಕು_HIS + ಹುಡುಕು_KEEP)), &regmatch) == FAIL)
    {
	ಯದಿ ((options & ಹುಡುಕು_ಸಂದೇಶ) && !rc_did_emsg)
	    semsg(_(e_invalid_ಹುಡುಕು_string_str), mr_pattern);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಹುಡುಕು_from_match_end = vim_strchr(p_cpo, CPO_ಹುಡುಕು) != ರಿಕ್ತ;

    ಯದಿ (extra_arg != ರಿಕ್ತ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	ಯದಿ (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    ಕುರು	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	ಯದಿ (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ಯದಿ (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    ಅನ್ಯಥಾ
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	ಅನ್ಯಥಾ
	    start_char_len = 1;
	ಯದಿ (dir == FORWARD)
	{
	    ಯದಿ (options & ಹುಡುಕು_START)
		extra_col = 0;
	    ಅನ್ಯಥಾ
		extra_col = start_char_len;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (options & ಹುಡುಕು_START)
		extra_col = start_char_len;
	    ಅನ್ಯಥಾ
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_ಸಾಲು = TRUE;	// default: start in first line
	ಯದಿ (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_ಸಾಲು = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	ಯದಿ (dir == BACKWARD && start_pos.col == 0
					     && (options & ಹುಡುಕು_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_ಸಾಲು = FALSE;
	}
	ಅನ್ಯಥಾ
	    lnum = pos->lnum;

	ಪ್ರತಿ (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    ಪ್ರತಿ ( ; lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
					   lnum += dir, at_first_ಸಾಲು = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ಯದಿ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    ಭಙ್ಗ;
		// Stop after passing the time limit.
		ಯದಿ (*timed_out)
		    ಭಙ್ಗ;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_ಸಾಲು && (options & ಹುಡುಕು_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		// Abort searching on an error (e.g., out of stack).
		ಯದಿ (called_emsg > called_emsg_before || *timed_out)
		    ಭಙ್ಗ;
		ಯದಿ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    ಯದಿ (lnum + matchpos.lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ptr = (char_u *)"";
		    ಅನ್ಯಥಾ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ಯದಿ (dir == FORWARD && at_first_ಸಾಲು)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ಯಾವತ್ (matchpos.lnum == 0
				&& ((options & ಹುಡುಕು_END) && first_match
				    ?  (nmatched == 1
					&& (ಪೂರ್ಣಾಙ್ಕ)endpos.col - 1
					     < (ಪೂರ್ಣಾಙ್ಕ)start_pos.col + extra_col)
				    : ((ಪೂರ್ಣಾಙ್ಕ)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (ಪೂರ್ಣಾಙ್ಕ)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ಯದಿ (ಹುಡುಕು_from_match_end)
			    {
				ಯದಿ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    ಭಙ್ಗ;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				ಯದಿ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ಯದಿ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಅನ್ಯಥಾ
					++matchcol;
				}
			    }
			    ಅನ್ಯಥಾ
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ಯದಿ (ptr[matchcol] != NUL)
				{
				    ಯದಿ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    ಅನ್ಯಥಾ
					++matchcol;
				}
			    }
			    ಯದಿ (matchcol == 0 && (options & ಹುಡುಕು_START))
				ಭಙ್ಗ;
			    ಯದಿ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				ಭಙ್ಗ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
				ಭಙ್ಗ;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ಯದಿ (!match_ok)
			    ಅನುವರ್ತ;
		    }
		    ಯದಿ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			ಪ್ರತಿ (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ಯದಿ (loop
				|| ((options & ಹುಡುಕು_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (ಪೂರ್ಣಾಙ್ಕ)regmatch.endpos[0].col - 1
							< (ಪೂರ್ಣಾಙ್ಕ)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (ಪೂರ್ಣಾಙ್ಕ)regmatch.startpos[0].col
						      < (ಪೂರ್ಣಾಙ್ಕ)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    ಅನ್ಯಥಾ
				ಭಙ್ಗ;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ಯದಿ (ಹುಡುಕು_from_match_end)
			    {
				ಯದಿ (nmatched > 1)
				    ಭಙ್ಗ;
				matchcol = endpos.col;
				// for empty match: advance one char
				ಯದಿ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ಯದಿ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಅನ್ಯಥಾ
					++matchcol;
				}
			    }
			    ಅನ್ಯಥಾ
			    {
				// Stop when the match is in a next line.
				ಯದಿ (matchpos.lnum > 0)
				    ಭಙ್ಗ;
				matchcol = matchpos.col;
				ಯದಿ (ptr[matchcol] != NUL)
				{
				    ಯದಿ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಅನ್ಯಥಾ
					++matchcol;
				}
			    }
			    ಯದಿ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ಯದಿ (*timed_out)
				    match_ok = FALSE;
				ಭಙ್ಗ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
				ಭಙ್ಗ;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ಯದಿ (!match_ok)
			    ಅನುವರ್ತ;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ಯದಿ ((options & ಹುಡುಕು_END) && !(options & ಹುಡುಕು_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			ಯದಿ (endpos.col == 0)
			{
			    ಯದಿ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ಅನ್ಯಥಾ
			{
			    --pos->col;
			    ಯದಿ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			ಯದಿ (end_pos != ರಿಕ್ತ)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			ಯದಿ (end_pos != ರಿಕ್ತ)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    ಯದಿ (end_pos != ರಿಕ್ತ)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    ಹುಡುಕು_match_ಸಾಲುಗಳು = endpos.lnum - matchpos.lnum;
		    ಹುಡುಕು_match_endcol = endpos.col;
		    ಭಙ್ಗ;
		}
		ಸಾಲು_breakcheck();	// stop if ctrl-C typed
		ಯದಿ (got_int)
		    ಭಙ್ಗ;

#ifdef FEAT_ಹುಡುಕು_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ಯದಿ ((options & ಹುಡುಕು_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    ಭಙ್ಗ;
		}
#endif

		ಯದಿ (loop && lnum == start_pos.lnum)
		    ಭಙ್ಗ;	    // if second loop, stop where started
	    }
	    at_first_ಸಾಲು = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ಯದಿ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_ಹುಡುಕು_EXTRA
			      || break_loop
#endif
			      || found || loop)
		ಭಙ್ಗ;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ಯದಿ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ
		lnum = 1;
	    ಯದಿ (!shortmess(SHM_ಹುಡುಕು)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & ಹುಡುಕು_ಸಂದೇಶ))
		give_warning((char_u *)_(dir == BACKWARD
					  ? ತುದಿ_bot_ಸಂದೇಶ : bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    ಯದಿ (extra_arg != ರಿಕ್ತ)
		extra_arg->sa_wrapped = TRUE;
	}
	ಯದಿ (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_ಹುಡುಕು_EXTRA
		|| break_loop
#endif
		)
	    ಭಙ್ಗ;
    }
    ಯಾವತ್ (--ಎಣಿಕೆ > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    ಯದಿ (extra_arg != ರಿಕ್ತ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    ಯದಿ (!found)		    // did not find it
    {
	ಯದಿ (got_int)
	    emsg(_(e_interrupted));
	ಅನ್ಯಥಾ ಯದಿ ((options & ಹುಡುಕು_ಸಂದೇಶ) == ಹುಡುಕು_ಸಂದೇಶ)
	{
	    ಯದಿ (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    ಅನ್ಯಥಾ ಯದಿ (lnum == 0)
		semsg(_(e_ಹುಡುಕು_hit_ತುದಿ_without_match_for_str), mr_pattern);
	    ಅನ್ಯಥಾ
		semsg(_(e_ಹುಡುಕು_hit_bottom_without_match_for_str), mr_pattern);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ಯದಿ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	ಯದಿ (pos->col > 0)
	    --pos->col;
    }

    ಪ್ರತ್ಯಾಗಮ submatch + 1;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    ಶೂನ್ಯ
set_ಹುಡುಕು_direction(ಪೂರ್ಣಾಙ್ಕ cdir)
{
    spats[0].off.dir = cdir;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_vv_searchforward(ಶೂನ್ಯ)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
first_submatch(regmmatch_T *rp)
{
    ಪೂರ್ಣಾಙ್ಕ		submatch;

    ಪ್ರತಿ (submatch = 1; ; ++submatch)
    {
	ಯದಿ (rp->startpos[submatch].lnum >= 0)
	    ಭಙ್ಗ;
	ಯದಿ (submatch == 9)
	{
	    submatch = 0;
	    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ submatch;
}
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
parse_ಹುಡುಕು_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_delim,
    ಪೂರ್ಣಾಙ್ಕ		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    ಪೂರ್ಣಾಙ್ಕ		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ಯದಿ (*pat == ರಿಕ್ತ || **pat == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ರಿಕ್ತ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, ಹುಡುಕು_delim, magic_isset(),
						    strcopy, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (ಪೂರ್ಣಾಙ್ಕ)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ಯದಿ (*p == ಹುಡುಕು_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->ಸಾಲು = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    ಯದಿ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->ಸಾಲು = TRUE;
    ಅನ್ಯಥಾ ಯದಿ ((options & ಹುಡುಕು_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ಯದಿ (*p == 'e')		// end
	    offset->end = ಹುಡುಕು_END;
	++p;
    }
    ಯದಿ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ಯದಿ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((ವರ್ಣ *)p);
	ಅನ್ಯಥಾ ಯದಿ (*p == '-')	    // single '-'
	    offset->off = -1;
	ಅನ್ಯಥಾ			    // single '+'
	    offset->off = 1;
	++p;
	ಯಾವತ್ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (ಪೂರ್ಣಾಙ್ಕ)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ಪ್ರತ್ಯಾಗಮ cmdlen;
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
    ಪೂರ್ಣಾಙ್ಕ
do_ಹುಡುಕು(
    oparg_T	    *oap,	// can be NULL
    ಪೂರ್ಣಾಙ್ಕ		    dirc,	// '/' or '?'
    ಪೂರ್ಣಾಙ್ಕ		    ಹುಡುಕು_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    ಪೂರ್ಣಾಙ್ಕ		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    show_ಹುಡುಕು_stats;
    char_u	    *msgbuf = ರಿಕ್ತ;
    size_t	    msgbuflen = 0;
    ಪೂರ್ಣಾಙ್ಕ		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ಯದಿ (spats[0].off.ಸಾಲು && vim_strchr(p_cpo, CPO_LINEOFF) != ರಿಕ್ತ)
    {
	spats[0].off.ಸಾಲು = FALSE;
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
    ಯದಿ (dirc == 0)
	dirc = spats[0].off.dir;
    ಅನ್ಯಥಾ
    {
	spats[0].off.dir = dirc;
#ಯದಿ defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    ಯದಿ (options & ಹುಡುಕು_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ಅನ್ಯಥಾ
	ಯದಿ (dirc == '/')
	    dirc = '?';
	ಅನ್ಯಥಾ
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ಯದಿ (dirc == '/')
    {
	ಯದಿ (hasFolding(pos.lnum, ರಿಕ್ತ, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (hasFolding(pos.lnum, &pos.lnum, ರಿಕ್ತ))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_ಹುಡುಕು_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ಯದಿ (no_hlsearch && !(options & ಹುಡುಕು_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ		show_ತುದಿ_bot_ಸಂದೇಶ = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ರಿಕ್ತ;
					    // use previous pattern
	ಯದಿ (pat == ರಿಕ್ತ || *pat == NUL || *pat == ಹುಡುಕು_delim)
	{
	    ಯದಿ (spats[RE_ಹುಡುಕು].pat == ರಿಕ್ತ)	    // no previous pattern
	    {
		ಯದಿ (spats[RE_SUBST].pat == ರಿಕ್ತ)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    ಗಚ್ಛ end_do_ಹುಡುಕು;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ಅನ್ಯಥಾ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ಯದಿ (pat != ರಿಕ್ತ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_ಹುಡುಕು_pattern_offset(&pat, &patlen,
				ಹುಡುಕು_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ಯದಿ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_ಹುಡುಕು_stats = FALSE;
	ಯದಿ ((options & ಹುಡುಕು_ECHO) && messaging()
		&& !ಸಂದೇಶ_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    ಸಂದೇಶ_start();

	    // Get the offset, so we know how long it is.
	    ಯದಿ (!cmd_silent &&
		    (spats[0].off.ಸಾಲು || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ಯದಿ (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		ಅನ್ಯಥಾ ಯದಿ (!spats[0].off.ಸಾಲು)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ಯದಿ (spats[0].off.off != 0 || spats[0].off.ಸಾಲು)
		    off_len += vim_snprintf((ವರ್ಣ *)off_buf + off_len,
			   ಪರಿಮಾಣ(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ಯದಿ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ಅನ್ಯಥಾ
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ಯದಿ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ಯದಿ (ಸಂದೇಶ_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (ಪೂರ್ಣಾಙ್ಕ)(Rows - ಸಂದೇಶ_row) * cmdline_width - 1;
		ಅನ್ಯಥಾ
		    // Use up to 'showcmd' column.
		    msgbufsize = (ಪೂರ್ಣಾಙ್ಕ)(Rows - ಸಂದೇಶ_row - 1) * cmdline_width
			+ sc_col - 1;
		ಯದಿ (msgbufsize < plen + off_len + ಹುಡುಕು_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + ಹುಡುಕು_STAT_BUF_LEN + 3;
	    }
	    ಅನ್ಯಥಾ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ಯದಿ (msgbuf == ರಿಕ್ತ)
	    {
		msgbuflen = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ಯದಿ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ಯದಿ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    ಅನ್ಯಥಾ
			mch_memmove(msgbuf + 1, p, plen);
		    ಯದಿ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = ಸಂದೇಶ_strtrunc(msgbuf, TRUE);
		    ಯದಿ (trunc != ರಿಕ್ತ)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#ifdef FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    ಯದಿ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			ಯದಿ (r != ರಿಕ್ತ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ಯಾವತ್ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ಯದಿ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ಅನ್ಯಥಾ
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    ಸಂದೇಶ_outtrans(msgbuf);
		    ಸಂದೇಶ_clr_eos();
		    ಸಂದೇಶ_check();

		    gotocmdline(FALSE);
		    out_flush();
		    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
		}

		ಯದಿ (!shortmess(SHM_SEARCHCOUNT))
		    show_ಹುಡುಕು_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	ಯದಿ (!spats[0].off.ಸಾಲು && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    ಯದಿ (spats[0].off.off > 0)
	    {
		ಪ್ರತಿ (c = spats[0].off.off; c; --c)
		    ಯದಿ (decl(&pos) == -1)
			ಭಙ್ಗ;
		ಯದಿ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (c = spats[0].off.off; c; ++c)
		    ಯದಿ (incl(&pos) == -1)
			ಭಙ್ಗ;
		ಯದಿ (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ರಿಕ್ತ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, ಎಣಿಕೆ, spats[0].off.end + (options &
		       (ಹುಡುಕು_KEEP + ಹುಡುಕು_PEEK + ಹುಡುಕು_HIS
			+ ಹುಡುಕು_ಸಂದೇಶ + ಹುಡುಕು_START
			+ ((pat != ರಿಕ್ತ && *pat == ';') ? 0 : ಹುಡುಕು_NOOF))),
		RE_LAST, sia);

	ಯದಿ (dircp != ರಿಕ್ತ)
	    *dircp = ಹುಡುಕು_delim; // restore second '/' or '?' for normal_cmd()

	ಯದಿ (!shortmess(SHM_ಹುಡುಕು) && sia && sia->sa_wrapped)
	    show_ತುದಿ_bot_ಸಂದೇಶ = TRUE;

	ಯದಿ (c == FAIL)
	{
	    retval = 0;
	    ಗಚ್ಛ end_do_ಹುಡುಕು;
	}
	ಯದಿ (spats[0].off.end && oap != ರಿಕ್ತ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ಯದಿ (!(options & ಹುಡುಕು_NOOF) || (pat != ರಿಕ್ತ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ಯದಿ (spats[0].off.ಸಾಲು)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ಯದಿ (c < 1)
		    pos.lnum = 1;
		ಅನ್ಯಥಾ ಯದಿ (c > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಅನ್ಯಥಾ
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ಅನ್ಯಥಾ ಯದಿ (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ಯದಿ (c > 0)
		{
		    ಯಾವತ್ (c-- > 0)
			ಯದಿ (incl(&pos) == -1)
			    ಭಙ್ಗ;
		}
		// to the left, check for start of file
		ಅನ್ಯಥಾ
		{
		    ಯಾವತ್ (c++ < 0)
			ಯದಿ (decl(&pos) == -1)
			    ಭಙ್ಗ;
		}
	    }
	    ಯದಿ (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ಯದಿ (show_ಹುಡುಕು_stats)
	     cmdline_ಹುಡುಕು_stat(dirc, &pos, &curwin->w_cursor,
				show_ತುದಿ_bot_ಸಂದೇಶ, msgbuf, msgbuflen,
				(ಎಣಿಕೆ != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_ಹುಡುಕು)
				     && hasFolding(curwin->w_cursor.lnum,
								   ರಿಕ್ತ, ರಿಕ್ತ))
#endif
				),
				p_msc,
				ಹುಡುಕು_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	ಯದಿ (!(options & ಹುಡುಕು_OPT) || pat == ರಿಕ್ತ || *pat != ';')
	    ಭಙ್ಗ;

	dirc = *++pat;
	--patlen;
	ಹುಡುಕು_delim = dirc;
	ಯದಿ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    ಗಚ್ಛ end_do_ಹುಡುಕು;
	}
	++pat;
	--patlen;
    }

    ಯದಿ (options & ಹುಡುಕು_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_ಹುಡುಕು:
    ಯದಿ ((options & ಹುಡುಕು_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ಪ್ರತ್ಯಾಗಮ retval;
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
    ಪೂರ್ಣಾಙ್ಕ
ಹುಡುಕು_for_exact_ಸಾಲು(
    buf_T	*buf,
    pos_T	*pos,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		compl_len = ins_compl_len();

    ಯದಿ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತಿ (;;)
    {
	pos->lnum += dir;
	ಯದಿ (pos->lnum < 1)
	{
	    ಯದಿ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಯದಿ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(ತುದಿ_bot_ಸಂದೇಶ), TRUE);
	    }
	    ಅನ್ಯಥಾ
	    {
		pos->lnum = 1;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಯದಿ (p_ws)
	    {
		pos->lnum = 1;
		ಯದಿ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    }
	    ಅನ್ಯಥಾ
	    {
		pos->lnum = 1;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (pos->lnum == start)
	    ಭಙ್ಗ;
	ಯದಿ (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ಯದಿ (compl_status_adding() && !compl_status_sol())
	{
	    ಯದಿ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ಯದಿ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
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
    ಪೂರ್ಣಾಙ್ಕ
searchc(cmdarg_T *cap, ಪೂರ್ಣಾಙ್ಕ t_cmd)
{
    ಪೂರ್ಣಾಙ್ಕ			c = cap->nchar;	// char to search for
    ಪೂರ್ಣಾಙ್ಕ			dir = cap->arg;	// TRUE for searching forward
    long		ಎಣಿಕೆ = cap->count1;	// repeat count
    ಪೂರ್ಣಾಙ್ಕ			col;
    char_u		*p;
    ಪೂರ್ಣಾಙ್ಕ			len;
    ಪೂರ್ಣಾಙ್ಕ			stop = TRUE;

    ಯದಿ (c != NUL)	// normal search: remember args for repeat
    {
	ಯದಿ (!ಕೀಲಿ_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    ಯದಿ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		ಯದಿ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    ಅನ್ಯಥಾ		// repeat previous search
    {
	ಯದಿ (*lastc == NUL && lastc_bytelen <= 1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ಅನ್ಯಥಾ
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ಯದಿ (vim_strchr(p_cpo, CPO_SCOLON) == ರಿಕ್ತ && ಎಣಿಕೆ == 1 && t_cmd)
	    stop = FALSE;
    }

    ಯದಿ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ಅನ್ಯಥಾ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	ಯದಿ (has_mbyte)
	{
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    ಯದಿ (col >= len)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (col == 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		ಯದಿ (lastc_bytelen <= 1)
		{
		    ಯದಿ (p[col] == c && stop)
			ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    ಭಙ್ಗ;
		stop = TRUE;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ ((col += dir) < 0 || col >= len)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (p[col] == c && stop)
		    ಭಙ್ಗ;
		stop = TRUE;
	    }
	}
    }

    ಯದಿ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	ಯದಿ (has_mbyte)
	{
	    ಯದಿ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    ಅನ್ಯಥಾ
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    ಪ್ರತ್ಯಾಗಮ OK;
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
findmatch(oparg_T *oap, ಪೂರ್ಣಾಙ್ಕ initc)
{
    ಪ್ರತ್ಯಾಗಮ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_prevcol(
    char_u	*linep,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		ch,
    ಪೂರ್ಣಾಙ್ಕ		*prevcol)
{
    --col;
    ಯದಿ (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    ಯದಿ (prevcol)
	*prevcol = col;
    ಪ್ರತ್ಯಾಗಮ (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;

    ಪ್ರತಿ (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    ಯದಿ (delim_copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *ಸಾಲು = ml_get(lnum);

	ಪ್ರತಿ (p = ಸಾಲು + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    ಯದಿ (lnum == endpos->lnum && (colnr_T)(p - ಸಾಲು) >= endpos->col)
		ಭಙ್ಗ;
	    ಯದಿ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (found)
	    ಭಙ್ಗ;
    }
    vim_free(delim_copy);
    ಪ್ರತ್ಯಾಗಮ found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_mps_ಮೌಲ್ಯಗಳು(
    ಪೂರ್ಣಾಙ್ಕ	    *initc,
    ಪೂರ್ಣಾಙ್ಕ	    *findc,
    ಪೂರ್ಣಾಙ್ಕ	    *backwards,
    ಪೂರ್ಣಾಙ್ಕ	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ಯಾವತ್ (*ptr != NUL)
    {
	ಯದಿ (has_mbyte)
	{
	    char_u *prev;

	    ಯದಿ (mb_ptr2char(ptr) == *initc)
	    {
		ಯದಿ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ಅನ್ಯಥಾ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ಯದಿ (mb_ptr2char(ptr) == *initc)
	    {
		ಯದಿ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		ಅನ್ಯಥಾ
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*ptr == *initc)
	    {
		ಯದಿ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ಅನ್ಯಥಾ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ptr += 2;
	    ಯದಿ (*ptr == *initc)
	    {
		ಯದಿ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		ಅನ್ಯಥಾ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ++ptr;
	}
	ಯದಿ (*ptr == ',')
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
    ಪೂರ್ಣಾಙ್ಕ		initc,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		maxtravel)
{
    ಸ್ಥಿರ pos_T pos;			// current search position
    ಪೂರ್ಣಾಙ್ಕ		findc = 0;		// matching brace
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;		// cumulative number of braces
    ಪೂರ್ಣಾಙ್ಕ		backwards = FALSE;	// init for gcc
    ಪೂರ್ಣಾಙ್ಕ		raw_string = FALSE;	// search for raw string
    ಪೂರ್ಣಾಙ್ಕ		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		do_quotes;		// check for quotes in current line
    ಪೂರ್ಣಾಙ್ಕ		at_start;		// do_quotes value at start position
    ಪೂರ್ಣಾಙ್ಕ		hash_dir = 0;		// Direction searched for # things
    ಪೂರ್ಣಾಙ್ಕ		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    ಪೂರ್ಣಾಙ್ಕ		start_in_quotes;	// start position is in quotes
    ಪೂರ್ಣಾಙ್ಕ		traveled = 0;		// how far we've searched so far
    ಪೂರ್ಣಾಙ್ಕ		ignore_cend = FALSE;    // ignore comment end
    ಪೂರ್ಣಾಙ್ಕ		cpo_match;		// vi compatible matching
    ಪೂರ್ಣಾಙ್ಕ		cpo_bsl;		// don't recognize backslashes
    ಪೂರ್ಣಾಙ್ಕ		match_escaped = 0;	// search for escaped match
    ಪೂರ್ಣಾಙ್ಕ		dir;			// Direction to search
    ಪೂರ್ಣಾಙ್ಕ		comment_col = MAXCOL;   // start of / / comment
    ಪೂರ್ಣಾಙ್ಕ		lispcomm = FALSE;	// inside of Lisp-style comment
    ಪೂರ್ಣಾಙ್ಕ		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    ಪೂರ್ಣಾಙ್ಕ		skip_comments = (flags & FM_SKIPCOMM) != 0;
    ಪೂರ್ಣಾಙ್ಕ		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != ರಿಕ್ತ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ರಿಕ್ತ);

    // Direction to search when initc is '/', '*' or '#'
    ಯದಿ (flags & FM_BACKWARD)
	dir = BACKWARD;
    ಅನ್ಯಥಾ ಯದಿ (flags & FM_FORWARD)
	dir = FORWARD;
    ಅನ್ಯಥಾ
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ಯದಿ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ಯದಿ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    ಅನ್ಯಥಾ ಯದಿ (initc != '#' && initc != NUL)
    {
	find_mps_ಮೌಲ್ಯಗಳು(&initc, &findc, &backwards, TRUE);
	ಯದಿ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ಯದಿ (findc == NUL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ಯದಿ (initc == '#')
	{
	    hash_dir = dir;
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ಯದಿ (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ಯದಿ (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ಯದಿ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ಅನ್ಯಥಾ ಯದಿ (linep[pos.col] == '/')
		{
		    ಯದಿ (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (linep[pos.col] == '*')
		{
		    ಯದಿ (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    ಯದಿ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ಯದಿ (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		ಪ್ರತಿ (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    ಯದಿ (initc == NUL)
			ಭಙ್ಗ;

		    find_mps_ಮೌಲ್ಯಗಳು(&initc, &findc, &backwards, FALSE);
		    ಯದಿ (findc)
			ಭಙ್ಗ;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		ಯದಿ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ಯದಿ (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ (!cpo_bsl)
		{
		    ಪೂರ್ಣಾಙ್ಕ col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    ಪ್ರತಿ (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	ಯದಿ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ಯದಿ (oap != ರಿಕ್ತ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ಯದಿ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ಯದಿ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ಅನ್ಯಥಾ
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    pos.col = 0;
	    ಯಾವತ್ (!got_int)
	    {
		ಯದಿ (hash_dir > 0)
		{
		    ಯದಿ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (pos.lnum == 1)
		    ಭಙ್ಗ;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		ಸಾಲು_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ಯದಿ (*ptr != '#')
		    ಅನುವರ್ತ;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ಯದಿ (hash_dir > 0)
		{
		    ಯದಿ (STRNCMP(ptr, "if", 2) == 0)
			ಎಣಿಕೆ++;
		    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(ptr, "el", 2) == 0)
		    {
			ಯದಿ (ಎಣಿಕೆ == 0)
			    ಪ್ರತ್ಯಾಗಮ &pos;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ಯದಿ (ಎಣಿಕೆ == 0)
			    ಪ್ರತ್ಯಾಗಮ &pos;
			ಎಣಿಕೆ--;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (STRNCMP(ptr, "if", 2) == 0)
		    {
			ಯದಿ (ಎಣಿಕೆ == 0)
			    ಪ್ರತ್ಯಾಗಮ &pos;
			ಎಣಿಕೆ--;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ಯದಿ (ಎಣಿಕೆ == 0)
			    ಪ್ರತ್ಯಾಗಮ &pos;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(ptr, "endif", 5) == 0)
			ಎಣಿಕೆ++;
		}
	    }
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ಯದಿ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ರಿಕ್ತ)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    ಯದಿ ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    ಯದಿ (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ಯದಿ (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    ಯಾವತ್ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ಯದಿ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ಯದಿ (lispcomm && pos.col < (colnr_T)comment_col)
		ಭಙ್ಗ;
	    ಯದಿ (pos.col == 0)		// at start of line, go to prev. one
	    {
		ಯದಿ (pos.lnum == 1)	// start of file
		    ಭಙ್ಗ;
		--pos.lnum;

		ಯದಿ (maxtravel > 0 && ++traveled > maxtravel)
		    ಭಙ್ಗ;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		ಸಾಲು_breakcheck();

		// Check if this line contains a single-line comment
		ಯದಿ (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		ಯದಿ (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		ಅನ್ಯಥಾ ಯದಿ (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    ಅನ್ಯಥಾ
	    {
		--pos.col;
		ಯದಿ (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	ಅನ್ಯಥಾ				// forward search
	{
	    ಯದಿ (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		ಯದಿ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    ಭಙ್ಗ;
		++pos.lnum;

		ಯದಿ (maxtravel && traveled++ > maxtravel)
		    ಭಙ್ಗ;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		ಸಾಲು_breakcheck();
		ಯದಿ (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		ಅನ್ಯಥಾ
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ಯದಿ (skip_comments && !comment_dir && !inquote)
	{
	    ಯದಿ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ಯದಿ (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (ಪೂರ್ಣಾಙ್ಕ)pos.col < comment_col))
		    in_block_comment = TRUE;
		ಅನ್ಯಥಾ ಯದಿ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ಯದಿ (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (ಪೂರ್ಣಾಙ್ಕ)pos.col < comment_col))
		    in_block_comment = TRUE;
		ಅನ್ಯಥಾ ಯದಿ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ಯದಿ (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ಯದಿ (linep[0] == findc && ಎಣಿಕೆ == 0)	// match!
		ಪ್ರತ್ಯಾಗಮ &pos;
	    ಭಙ್ಗ;					// out of scope
	}

	ಯದಿ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ಯದಿ (comment_dir == FORWARD)
	    {
		ಯದಿ (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    ಪ್ರತ್ಯಾಗಮ &pos;
		}
	    }
	    ಅನ್ಯಥಾ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ಯದಿ (pos.col == 0)
		    ಅನುವರ್ತ;
		ಅನ್ಯಥಾ ಯದಿ (raw_string)
		{
		    ಯದಿ (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != ರಿಕ್ತ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ಯದಿ (!find_rawstring_end(linep, &pos,
				  ಎಣಿಕೆ > 0 ? &match_pos : &curwin->w_cursor))
			{
			    ಎಣಿಕೆ++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (ಪೂರ್ಣಾಙ್ಕ)pos.col < comment_col)
		{
		    ಎಣಿಕೆ++;
		    match_pos = pos;
		    match_pos.col--;
		}
		ಅನ್ಯಥಾ ಯದಿ (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    ಯದಿ (ಎಣಿಕೆ > 0)
			pos = match_pos;
		    ಅನ್ಯಥಾ ಯದಿ (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (ಪೂರ್ಣಾಙ್ಕ)pos.col <= comment_col)
			pos.col -= 2;
		    ಅನ್ಯಥಾ ಯದಿ (ignore_cend)
			ಅನುವರ್ತ;
		    ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ &pos;
		}
	    }
	    ಅನುವರ್ತ;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ಯದಿ (cpo_match)
	    do_quotes = 0;
	ಅನ್ಯಥಾ ಯದಿ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    ಪ್ರತಿ (ptr = linep; *ptr; ++ptr)
	    {
		ಯದಿ (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		ಯದಿ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ಯದಿ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ಯದಿ (!do_quotes)
	    {
		inquote = FALSE;
		ಯದಿ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ಯದಿ (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			ಯದಿ (inquote)
			    start_in_quotes = TRUE;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (backwards)
			inquote = TRUE;
		}
		ಯದಿ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ಯದಿ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ಯದಿ (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    ಯದಿ (inquote)
				start_in_quotes = TRUE;
			}
			ಅನ್ಯಥಾ ಯದಿ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ಯದಿ (start_in_quotes == MAYBE)
	    start_in_quotes = FALSE;

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
	c = PTR2CHAR(linep + pos.col);
	ವಿಕಲ್ಪ (c)
	{
	ಪಕ್ಷ NUL:
	    // at end of line without trailing backslash, reset inquote
	    ಯದಿ (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ಯದಿ (do_quotes)
	    {
		ಪೂರ್ಣಾಙ್ಕ col;

		ಪ್ರತಿ (col = pos.col - 1; col >= 0; --col)
		    ಯದಿ (linep[col] != '\\')
			ಭಙ್ಗ;
		ಯದಿ ((((ಪೂರ್ಣಾಙ್ಕ)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    ಭಙ್ಗ;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	ಪಕ್ಷ '\'':
	    ಯದಿ (!cpo_match && initc != '\'' && findc != '\'')
	    {
		ಯದಿ (backwards)
		{
		    ಯದಿ (pos.col > 1)
		    {
			ಯದಿ (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    ಭಙ್ಗ;
			}
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (linep[pos.col + 1])	// forward search
		{
		    ಯದಿ (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			ಭಙ್ಗ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			ಭಙ್ಗ;
		    }
		}
	    }
	    // FALLTHROUGH

	ಸ್ವಭಾವ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ಯದಿ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ರಿಕ್ತ
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', ರಿಕ್ತ)
		    && check_prevcol(linep, pos.col - 1, '#', ರಿಕ್ತ))
		ಭಙ್ಗ;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ಯದಿ (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (ಪೂರ್ಣಾಙ್ಕ)pos.col >= comment_col)))
		ಭಙ್ಗ;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ಯದಿ ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		ಪೂರ್ಣಾಙ್ಕ	col, bslcnt = 0;

		ಯದಿ (!cpo_bsl)
		{
		    ಪ್ರತಿ (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ಯದಿ (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    ಯದಿ (c == initc)
			ಎಣಿಕೆ++;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (ಎಣಿಕೆ == 0)
			    ಪ್ರತ್ಯಾಗಮ &pos;
			ಎಣಿಕೆ--;
		    }
		}
	    }
	}
    }

    ಯದಿ (comment_dir == BACKWARD && ಎಣಿಕೆ > 0)
    {
	pos = match_pos;
	ಪ್ರತ್ಯಾಗಮ &pos;
    }
    ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    ಶೂನ್ಯ
showmatch(
    ಪೂರ್ಣಾಙ್ಕ		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    ಪೂರ್ಣಾಙ್ಕ		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    ಪ್ರತಿ (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    ಭಙ್ಗ;
#endif
	p += mb_ptr2len(p) + 1;
	ಯದಿ (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    ಭಙ್ಗ;
	p += mb_ptr2len(p);
	ಯದಿ (*p == NUL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((lpos = findmatch(ರಿಕ್ತ, NUL)) == ರಿಕ್ತ)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ರಿಕ್ತ, &vcol, ರಿಕ್ತ, 0);

    ಪೂರ್ಣಾಙ್ಕ col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    ಯದಿ (!col_visible)
	ಪ್ರತ್ಯಾಗಮ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ಯದಿ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
	dollar_vcol = -1;
    ++curwin->w_virtcol;	// do display ')' just before "$"
    update_screen(UPD_VALID);	// show the new char first

    save_dollar_vcol = dollar_vcol;
#ifdef CURSOR_SHAPE
    save_state = State;
    State = MODE_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#endif
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    out_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    ಯದಿ (vim_strchr(p_cpo, CPO_SHOWMATCH) != ರಿಕ್ತ)
	ui_delay(p_mat * 100L + 8, TRUE);
    ಅನ್ಯಥಾ ಯದಿ (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#ifdef CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#endif
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    ಪೂರ್ಣಾಙ್ಕ		move,
    pos_T	*cur,
    ಪೂರ್ಣಾಙ್ಕ		direction)
{
    regmmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		nmatched = 0;
    ಪೂರ್ಣಾಙ್ಕ		result = -1;
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		flag = 0;

    ಯದಿ (pattern == ರಿಕ್ತ)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    ಯದಿ (ಹುಡುಕು_regcomp(pattern, patternlen, ರಿಕ್ತ, RE_ಹುಡುಕು, RE_ಹುಡುಕು,
					      ಹುಡುಕು_KEEP, &regmatch) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    ಯದಿ (move)
    {
	CLEAR_POS(&pos);
    }
    ಅನ್ಯಥಾ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = ಹುಡುಕು_START;
    }

    ಯದಿ (searchit(curwin, curbuf, &pos, ರಿಕ್ತ, direction, pattern, patternlen, 1,
				  ಹುಡುಕು_KEEP + flag, RE_ಹುಡುಕು, ರಿಕ್ತ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	ಕುರು
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, ರಿಕ್ತ);
	    ಯದಿ (nmatched != 0)
		ಭಙ್ಗ;
	} ಯಾವತ್ (regmatch.regprog != ರಿಕ್ತ
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	ಯದಿ (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    ಪ್ರತ್ಯಾಗಮ result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_ಹುಡುಕು(
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    pos_T	save_VIsual = VIsual;
    ಪೂರ್ಣಾಙ್ಕ		zero_width;
    ಪೂರ್ಣಾಙ್ಕ		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    ಯದಿ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ಯದಿ (VIsual_active)
    {
	ಯದಿ (forward)
	    incl(&pos);
	ಅನ್ಯಥಾ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ಯದಿ (zero_width == -1)
	ಪ್ರತ್ಯಾಗಮ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    ಪ್ರತಿ (i = 0; i < 2; i++)
    {
	ಯದಿ (forward)
	{
	    ಯದಿ (i == 0 && skip_first_backward)
		ಅನುವರ್ತ;
	    dir = i;
	}
	ಅನ್ಯಥಾ
	    dir = !i;

	flags = 0;
	ಯದಿ (!dir && !zero_width)
	    flags = ಹುಡುಕು_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	ಯದಿ (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? ಎಣಿಕೆ : 1),
		ಹುಡುಕು_KEEP | flags, RE_ಹುಡುಕು, ರಿಕ್ತ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	ಯದಿ (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ಯದಿ (VIsual_active)
		VIsual = save_VIsual;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (i == 0 && !result)
	{
	    ಯದಿ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ಅನ್ಯಥಾ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    }
	}
    }

    start_pos = pos;

    ಯದಿ (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    ಯದಿ (LT_POS(VIsual, end_pos) && forward)
    {
	ಯದಿ (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ಅನ್ಯಥಾ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ಅನ್ಯಥಾ ಯದಿ (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    ಯದಿ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ಯದಿ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ಅನ್ಯಥಾ ಯದಿ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    ಯದಿ (fdo_flags & FDO_ಹುಡುಕು && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif

    may_start_select('c');
    setmouse();
#ifdef FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_star.vmode = NUL;
#endif
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    ಪೂರ್ಣಾಙ್ಕ
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ಪ್ರತ್ಯಾಗಮ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_ಹುಡುಕು_stat(
    ಪೂರ್ಣಾಙ್ಕ		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    ಪೂರ್ಣಾಙ್ಕ		show_ತುದಿ_bot_ಸಂದೇಶ,
    char_u	*msgbuf,
    size_t	msgbuflen,
    ಪೂರ್ಣಾಙ್ಕ		recompute,
    ಪೂರ್ಣಾಙ್ಕ		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_ಹುಡುಕು_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ಯದಿ (stat.cur <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    ವರ್ಣ	t[ಹುಡುಕು_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ಯದಿ (stat.incomplete == 1)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[?/??]");
	ಅನ್ಯಥಾ ಯದಿ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ಅನ್ಯಥಾ ಯದಿ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ಅನ್ಯಥಾ
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ಅನ್ಯಥಾ
#endif
    {
	ಯದಿ (stat.incomplete == 1)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[?/??]");
	ಅನ್ಯಥಾ ಯದಿ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ಅನ್ಯಥಾ ಯದಿ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ಅನ್ಯಥಾ
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ಯದಿ (show_ತುದಿ_bot_ಸಂದೇಶ && len + 2 < ಹುಡುಕು_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ಯದಿ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ಯದಿ (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    ಸಂದೇಶ_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    ಸಂದೇಶ_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_ಹುಡುಕು_stat(
    ಪೂರ್ಣಾಙ್ಕ			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    ಪೂರ್ಣಾಙ್ಕ			recompute,
    ಪೂರ್ಣಾಙ್ಕ			maxcount,
    long		timeout UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		    save_ws = p_ws;
    ಪೂರ್ಣಾಙ್ಕ		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ಸ್ಥಿರ pos_T    lastpos = {0, 0, 0};
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cur = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cnt = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    exact_match = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    incomplete = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    last_maxcount = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    chgtick = 0;
    ಸ್ಥಿರ char_u   *lastpat = ರಿಕ್ತ;
    ಸ್ಥಿರ size_t   lastpatlen = 0;
    ಸ್ಥಿರ buf_T    *lbuf = ರಿಕ್ತ;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    ಯದಿ (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	ಪ್ರತ್ಯಾಗಮ;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ಯದಿ (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ರಿಕ್ತ
	    && STRNCMP(lastpat, spats[last_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[last_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_match = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    ಯದಿ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	done_ಹುಡುಕು = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	ಯದಿ (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	ಯಾವತ್ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ರಿಕ್ತ, 0, 1, ಹುಡುಕು_KEEP, RE_LAST, ರಿಕ್ತ) != FAIL)
	{
	    done_ಹುಡುಕು = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    ಯದಿ (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		ಭಙ್ಗ;
	    }
#endif
	    cnt++;
	    ಯದಿ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ಯದಿ (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    ಯದಿ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (got_int)
	    cur = -1; // abort
	ಯದಿ (done_ಹುಡುಕು)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    ಯದಿ (lastpat == ರಿಕ್ತ)
		lastpatlen = 0;
	    ಅನ್ಯಥಾ
		lastpatlen = spats[last_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_match = exact_match;
    stat->incomplete = incomplete;
    stat->last_maxcount = last_maxcount;
    p_ws = save_ws;
}

#ಯದಿ defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ಸ್ಥಿರ char_u *
get_ಸಾಲು_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *ಸಾಲು = ml_get(lnum);

    vim_strncpy(buf, ಸಾಲು, LSIZE - 1);
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    ಶೂನ್ಯ
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    ಪೂರ್ಣಾಙ್ಕ		dir UNUSED,	// direction of expansion
    ಪೂರ್ಣಾಙ್ಕ		len,		// length of search pattern
    ಪೂರ್ಣಾಙ್ಕ		whole,		// match whole words only
    ಪೂರ್ಣಾಙ್ಕ		skip_comments,	// don't match inside comments
    ಪೂರ್ಣಾಙ್ಕ		type,		// Type of search; are we looking for a type?
				// a macro?
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    ಪೂರ್ಣಾಙ್ಕ		forceit,	// If true, always switch to the found path
    ಪೂರ್ಣಾಙ್ಕ		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    ಪೂರ್ಣಾಙ್ಕ		max_path_depth = 50;
    long	match_ಎಣಿಕೆ = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ರಿಕ್ತ;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		depth;
    ಪೂರ್ಣಾಙ್ಕ		depth_displayed;	// For type==CHECK_PATH
    ಪೂರ್ಣಾಙ್ಕ		old_files;
    ಪೂರ್ಣಾಙ್ಕ		already_searched;
    char_u	*file_ಸಾಲು;
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	save_char;
    ಪೂರ್ಣಾಙ್ಕ		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    ಪೂರ್ಣಾಙ್ಕ		matched = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_show = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*already = ರಿಕ್ತ;
    char_u	*startp = ರಿಕ್ತ;
    char_u	*inc_opt = ರಿಕ್ತ;
# ಯದಿ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ರಿಕ್ತ;
# endif

    regmatch.regprog = ರಿಕ್ತ;
    incl_regmatch.regprog = ರಿಕ್ತ;
    def_regmatch.regprog = ರಿಕ್ತ;

    file_ಸಾಲು = alloc(LSIZE);
    ಯದಿ (file_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	ಯದಿ (pat == ರಿಕ್ತ)
	    ಗಚ್ಛ fpip_end;
	vim_snprintf((ವರ್ಣ *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	    ಗಚ್ಛ fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ಯದಿ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (incl_regmatch.regprog == ರಿಕ್ತ)
	    ಗಚ್ಛ fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ಯದಿ (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (def_regmatch.regprog == ರಿಕ್ತ)
	    ಗಚ್ಛ fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * ಪರಿಮಾಣ(SearchedFile), TRUE);
    ಯದಿ (files == ರಿಕ್ತ)
	ಗಚ್ಛ fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    ಯದಿ (end_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	end_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    ಸಾಲು = get_ಸಾಲು_and_copy(lnum, file_ಸಾಲು);

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (incl_regmatch.regprog != ರಿಕ್ತ
		&& vim_regexec(&incl_regmatch, ಸಾಲು, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ಯದಿ (strstr((ವರ್ಣ *)inc_opt, "\\zs") != ರಿಕ್ತ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (ಪೂರ್ಣಾಙ್ಕ)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ಅನ್ಯಥಾ
		// Use text after match with 'include'.
		new_fname = file_name_in_ಸಾಲು(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ರಿಕ್ತ);
	    already_searched = FALSE;
	    ಯದಿ (new_fname != ರಿಕ್ತ)
	    {
		// Check whether we have already searched in this file
		ಪ್ರತಿ (i = 0;; i++)
		{
		    ಯದಿ (i == depth + 1)
			i = old_files;
		    ಯದಿ (i == max_path_depth)
			ಭಙ್ಗ;
		    ಯದಿ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ಯದಿ (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
			    ಯದಿ (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				ಸಂದೇಶ_home_replace_hl(new_fname);
				ಸಂದೇಶ_puts(_(" (includes previously listed match)"));
				prev_fname = ರಿಕ್ತ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			ಭಙ್ಗ;
		    }
		}
	    }

	    ಯದಿ (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == ರಿಕ್ತ && !already_searched)))
	    {
		ಯದಿ (did_show)
		    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
		ಅನ್ಯಥಾ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    ಸಂದೇಶ_puts_title(_("--- Included files "));
		    ಯದಿ (action != ACTION_SHOW_ALL)
			ಸಂದೇಶ_puts_title(_("not found "));
		    ಸಂದೇಶ_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ಯಾವತ್ (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    ಪ್ರತಿ (i = 0; i < depth_displayed; i++)
			ಸಂದೇಶ_puts("  ");
		    ಸಂದೇಶ_home_replace(files[depth_displayed].name);
		    ಸಂದೇಶ_puts(" -->\n");
		}
		ಯದಿ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    ಪ್ರತಿ (i = 0; i <= depth_displayed; i++)
			ಸಂದೇಶ_puts("  ");
		    ಯದಿ (new_fname != ರಿಕ್ತ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			ಸಂದೇಶ_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ಅನ್ಯಥಾ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ಯದಿ (strstr((ವರ್ಣ *)inc_opt, "\\zs") != ರಿಕ್ತ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (ಪೂರ್ಣಾಙ್ಕ)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ಅನ್ಯಥಾ
			{
			    // find the file name after the end of the match
			    ಪ್ರತಿ (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    ಪ್ರತಿ (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ಯದಿ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			ಅನ್ಯಥಾ ಯದಿ (p > ಸಾಲು)
			{
			    ಯದಿ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ಯದಿ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			ಸಂದೇಶ_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ಯದಿ (new_fname == ರಿಕ್ತ && action == ACTION_SHOW_ALL)
		    {
			ಯದಿ (already_searched)
			    ಸಂದೇಶ_puts(_("  (Already listed)"));
			ಅನ್ಯಥಾ
			    ಸಂದೇಶ_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    ಯದಿ (new_fname != ರಿಕ್ತ)
	    {
		// Push the new file onto the file stack
		ಯದಿ (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    ಯದಿ (bigger != ರಿಕ್ತ)
		    {
			ಪ್ರತಿ (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			ಪ್ರತಿ (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = ರಿಕ್ತ;
			    bigger[i].name = ರಿಕ್ತ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			ಪ್ರತಿ (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ಯದಿ ((files[depth + 1].fp = mch_fopen((ವರ್ಣ *)new_fname, "r"))
								    == ರಿಕ್ತ)
		    vim_free(new_fname);
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (++depth == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[depth].name = curr_fname = new_fname;
		    files[depth].lnum = 0;
		    files[depth].matched = FALSE;
		    ಯದಿ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((ವರ್ಣ*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(ವರ್ಣ *)new_fname);
			ಸಂದೇಶ_trunc_attr((ವರ್ಣ *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ಅನ್ಯಥಾ ಯದಿ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (ವರ್ಣ *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = ಸಾಲು;
ಹುಡುಕು_ಸಾಲು:
	    define_matched = FALSE;
	    ಯದಿ (def_regmatch.regprog != ರಿಕ್ತ
			      && vim_regexec(&def_regmatch, ಸಾಲು, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ಯಾವತ್ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ಯದಿ (def_regmatch.regprog == ರಿಕ್ತ || define_matched)
	    {
		ಯದಿ (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ಯದಿ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ಅನ್ಯಥಾ
			matched = STRNCMP(startp, ptr, len) == 0;
		    ಯದಿ (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ಅನ್ಯಥಾ ಯದಿ (regmatch.regprog != ರಿಕ್ತ
			 && vim_regexec(&regmatch, ಸಾಲು, (colnr_T)(p - ಸಾಲು)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ಯದಿ (!define_matched && skip_comments)
		    {
			ಯದಿ ((*ಸಾಲು != '#' ||
				STRNCMP(skipwhite(ಸಾಲು + 1), "define", 6) != 0)
				&& get_leader_len(ಸಾಲು, ರಿಕ್ತ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(ಸಾಲು);
			ಯದಿ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    ಪ್ರತಿ (p = ಸಾಲು; *p && p < startp; ++p)
			    {
				ಯದಿ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ಯದಿ (p[1] == '/')
					ಭಙ್ಗ;
				    ++p;
				}
				ಅನ್ಯಥಾ ಯದಿ (!matched && p[0] == '*' && p[1] == '/')
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
	ಯದಿ (matched)
	{
	    ಯದಿ (action == ACTION_EXPAND)
	    {
		ಪೂರ್ಣಾಙ್ಕ	cont_s_ipos = FALSE;
		ಪೂರ್ಣಾಙ್ಕ	add_r;
		char_u	*aux;

		ಯದಿ (depth == -1 && lnum == curwin->w_cursor.lnum)
		    ಭಙ್ಗ;
		found = TRUE;
		aux = p = startp;
		ಯದಿ (compl_status_adding() && (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ಯದಿ (vim_iswordp(p))
			ಗಚ್ಛ exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (ಪೂರ್ಣಾಙ್ಕ)(p - aux);

		ಯದಿ (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ಯದಿ (depth < 0)
		    {
			ಯದಿ (lnum >= end_lnum)
			    ಗಚ್ಛ exit_matched;
			ಸಾಲು = get_ಸಾಲು_and_copy(++lnum, file_ಸಾಲು);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (vim_fgets(ಸಾಲು = file_ಸಾಲು,
						      LSIZE, files[depth].fp))
			ಗಚ್ಛ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(ಸಾಲು);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    ಯದಿ (p > aux)
		    {
			ಯದಿ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ಯದಿ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ಯದಿ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ರಿಕ್ತ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ಯದಿ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (ಪೂರ್ಣಾಙ್ಕ)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ಯದಿ (i == ins_compl_len())
			ಗಚ್ಛ exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ರಿಕ್ತ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ಯದಿ (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ಅನ್ಯಥಾ ಯದಿ (add_r == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		ಯದಿ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ಯದಿ (curr_fname != prev_fname)
		{
		    ಯದಿ (did_show)
			ಸಂದೇಶ_putchar('\n');	// cursor below last one
		    ಯದಿ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			ಸಂದೇಶ_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ಯದಿ (!got_int)
		    show_pat_in_path(ಸಾಲು, type, TRUE, action,
			    (depth == -1) ? ರಿಕ್ತ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_ಎಣಿಕೆ++);

		// Set matched flag for this file and all the ones that
		// include it
		ಪ್ರತಿ (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (--ಎಣಿಕೆ <= 0)
	    {
		found = TRUE;
		ಯದಿ (depth == -1 && lnum == curwin->w_cursor.lnum
# ಯದಿ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_ಸಾಲು));
		ಅನ್ಯಥಾ ಯದಿ (action == ACTION_SHOW)
		{
		    show_pat_in_path(ಸಾಲು, type, did_show, action,
			    (depth == -1) ? ರಿಕ್ತ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		ಅನ್ಯಥಾ
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ಯದಿ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ಯದಿ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ಯದಿ (action == ACTION_SPLIT)
		    {
			ಯದಿ (win_split(0, 0) == FAIL)
			    ಭಙ್ಗ;
			RESET_BINDING(curwin);
		    }
		    ಯದಿ (depth == -1)
		    {
			// match in current file
# ಯದಿ defined(FEAT_QUICKFIX)
			ಯದಿ (g_do_tagpreview != 0)
			{
			    ಯದಿ (!win_valid(curwin_save))
				ಭಙ್ಗ;
			    ಯದಿ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, ರಿಕ್ತ,
						     ರಿಕ್ತ, TRUE, lnum, forceit)))
				ಭಙ್ಗ;	// failed to jump to file
			}
			ಅನ್ಯಥಾ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, ರಿಕ್ತ, TRUE,
						    files[depth].lnum, forceit)))
			    ಭಙ್ಗ;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		ಯದಿ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - ಸಾಲು);
		    curwin->w_set_curswant = true;
		}

# ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		ಭಙ್ಗ;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ಯದಿ (def_regmatch.regprog == ರಿಕ್ತ
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		ಗಚ್ಛ ಹುಡುಕು_ಸಾಲು;
	}
	ಸಾಲು_breakcheck();
	ಯದಿ (action == ACTION_EXPAND)
	    ins_compl_check_ಕೀಲಿಗಳು(30, FALSE);
	ಯದಿ (got_int || ins_compl_interrupted())
	    ಭಙ್ಗ;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ಯಾವತ್ (depth >= 0 && !already
		&& vim_fgets(ಸಾಲು = file_ಸಾಲು, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    ಯದಿ (depth < depth_displayed)
		depth_displayed = depth;
	}
	ಯದಿ (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು);
	    ಯದಿ (i > 0 && ಸಾಲು[i - 1] == '\n')
		ಸಾಲು[--i] = NUL;
	    ಯದಿ (i > 0 && ಸಾಲು[i - 1] == '\r')
		ಸಾಲು[--i] = NUL;
	}
	ಅನ್ಯಥಾ ಯದಿ (!already)
	{
	    ಯದಿ (++lnum > end_lnum)
		ಭಙ್ಗ;
	    ಸಾಲು = get_ಸಾಲು_and_copy(lnum, file_ಸಾಲು);
	}
	already = ರಿಕ್ತ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    ಪ್ರತಿ (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    ಪ್ರತಿ (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    ಯದಿ (type == CHECK_PATH)
    {
	ಯದಿ (!did_show)
	{
	    ಯದಿ (action != ACTION_SHOW_ALL)
		ಸಂದೇಶ(_("All included files were found"));
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ(_("No included files"));
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!found && action != ACTION_EXPAND && !silent)
    {
	ಯದಿ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ಅನ್ಯಥಾ ಯದಿ (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	ಅನ್ಯಥಾ
	    emsg(_(e_couldnt_find_pattern));
    }
    ಯದಿ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	ಸಂದೇಶ_end();

fpip_end:
    vim_free(file_ಸಾಲು);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_pat_in_path(
    char_u  *ಸಾಲು,
    ಪೂರ್ಣಾಙ್ಕ	    type,
    ಪೂರ್ಣಾಙ್ಕ	    did_show,
    ಪೂರ್ಣಾಙ್ಕ	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    ಎಣಿಕೆ)
{
    char_u  *p;
    size_t  linelen;

    ಯದಿ (did_show)
	ಸಂದೇಶ_putchar('\n');	// cursor below last one
    ಅನ್ಯಥಾ ಯದಿ (!ಸಂದೇಶ_silent)
	gotocmdline(TRUE);	// cursor at status line
    ಯದಿ (got_int)		// 'q' typed at "--more--" message
	ಪ್ರತ್ಯಾಗಮ;
    linelen = STRLEN(ಸಾಲು);
    ಪ್ರತಿ (;;)
    {
	p = ಸಾಲು + linelen - 1;
	ಯದಿ (fp != ರಿಕ್ತ)
	{
	    // We used fgets(), so get rid of newline at end
	    ಯದಿ (p >= ಸಾಲು && *p == '\n')
		--p;
	    ಯದಿ (p >= ಸಾಲು && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ಯದಿ (action == ACTION_SHOW_ALL)
	{
	    sprintf((ವರ್ಣ *)IObuff, "%3ld: ", ಎಣಿಕೆ);	// show match nr
	    ಸಂದೇಶ_puts((ವರ್ಣ *)IObuff);
	    sprintf((ವರ್ಣ *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)IObuff, HL_ATTR(HLF_N));
	    ಸಂದೇಶ_puts(" ");
	}
	ಸಂದೇಶ_prt_ಸಾಲು(ಸಾಲು, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ಯದಿ (got_int || type != FIND_DEFINE || p < ಸಾಲು || *p != '\\')
	    ಭಙ್ಗ;

	ಯದಿ (fp != ರಿಕ್ತ)
	{
	    ಯದಿ (vim_fgets(ಸಾಲು, LSIZE, fp)) // end of file
		ಭಙ್ಗ;
	    linelen = STRLEN(ಸಾಲು);
	    ++*lnum;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (++*lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    ಸಾಲು = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	ಸಂದೇಶ_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_spat_last_idx(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ last_idx;
}
#endif

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    ಶೂನ್ಯ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			maxcount = p_msc;
    long		timeout = ಹುಡುಕು_STAT_DEF_TIMEOUT;
    ಪೂರ್ಣಾಙ್ಕ			recompute = TRUE;
    searchstat_T	stat;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	ಯದಿ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    timeout = (long)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    maxcount = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    ಯದಿ (pattern == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    ಯದಿ (li != ರಿಕ್ತ)
	    {
		pos.lnum = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    ಯದಿ (li != ರಿಕ್ತ)
	    {
		pos.col = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error) - 1;
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    ಯದಿ (li != ರಿಕ್ತ)
	    {
		pos.coladd = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }

    save_last_ಹುಡುಕು_pattern();
# ifdef FEAT_ಹುಡುಕು_EXTRA
    save_incsearch_state();
# endif
    ಯದಿ (pattern != ರಿಕ್ತ)
    {
	ಯದಿ (*pattern == NUL)
	    ಗಚ್ಛ the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	ಯದಿ (spats[last_idx].pat == ರಿಕ್ತ)
	    spats[last_idx].patlen = 0;
    }
    ಯದಿ (spats[last_idx].pat == ರಿಕ್ತ || *spats[last_idx].pat == NUL)
	ಗಚ್ಛ the_end;	// the previous pattern was never defined

    update_ಹುಡುಕು_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "current", stat.cur);
    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_ಹುಡುಕು_pattern();
# ifdef FEAT_ಹುಡುಕು_EXTRA
    restore_incsearch_state();
# endif
}
#endif
