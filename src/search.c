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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_vv_searchforward(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ_submatch(regmmatch_T *rp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
ಸ್ಥಿರ char_u *get_ಸಾಲು_and_copy(linenr_T lnum, char_u *buf);
ಸ್ಥಿರ ಶೂನ್ಯ show_pat_ಒಳ_ಮಾರ್ಗ(char_u *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, FILE *, linenr_T *, ದೀರ್ಘ);
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ searchstat
{
    ಪೂರ್ಣಾಂಕ	    cur;	    // current position of found words
    ಪೂರ್ಣಾಂಕ	    cnt;	    // total count of found words
    ಪೂರ್ಣಾಂಕ	    exact_ಹೊಂದಿಕೆ;    // TRUE if matched exactly on specified position
    ಪೂರ್ಣಾಂಕ	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    ಪೂರ್ಣಾಂಕ	    ಕೊನೆಯ_maxcount;  // the max count of the last search
} searchstat_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಶೂನ್ಯ save_incsearch_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ restore_incsearch_state(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_prevcol(char_u *linep, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ch, ಪೂರ್ಣಾಂಕ *prevcol);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_rawstring_ಕೊನೆ(char_u *linep, pos_T *startpos, pos_T *endpos);
ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_mps_ಮೌಲ್ಯಗಳು(ಪೂರ್ಣಾಂಕ *initc, ಪೂರ್ಣಾಂಕ *findc, ಪೂರ್ಣಾಂಕ *backwards, ಪೂರ್ಣಾಂಕ switchit);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_zero_ಅಗಲ(char_u *ನಮೂನೆ, size_t patternlen, ಪೂರ್ಣಾಂಕ move, pos_T *cur, ಪೂರ್ಣಾಂಕ direction);
ಸ್ಥಿರ ಶೂನ್ಯ cmdline_ಹುಡುಕು_stat(ಪೂರ್ಣಾಂಕ dirc, pos_T *pos, pos_T *cursor_pos, ಪೂರ್ಣಾಂಕ show_ತುದಿ_bot_ಸಂದೇಶ, char_u *msgbuf, size_t msgbuflen, ಪೂರ್ಣಾಂಕ recompute, ಪೂರ್ಣಾಂಕ maxcount, ದೀರ್ಘ timeout);
ಸ್ಥಿರ ಶೂನ್ಯ update_ಹುಡುಕು_stat(ಪೂರ್ಣಾಂಕ dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, ಪೂರ್ಣಾಂಕ recompute, ಪೂರ್ಣಾಂಕ maxcount, ದೀರ್ಘ timeout);

#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_STAT_BUF_LEN 16

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
    {ಖಾಲಿ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ಖಾಲಿ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_idx = 0;	// index in spats[] for RE_LAST

ಸ್ಥಿರ char_u lastc[2] = {NUL, NUL};	// last character searched for
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lastcdir = FORWARD;		// last direction of character search
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_t_cmd = TRUE;		// last search t_cmd
ಸ್ಥಿರ char_u	lastc_ಬೈಟುಗಳು[MB_MAXBYTES + 1];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ಸ್ಥಿರ spat_T	    saved_spats[ARRAY_ಉದ್ದಳತೆ(spats)];
ಸ್ಥಿರ char_u	    *saved_mr_ನಮೂನೆ = ಖಾಲಿ;
ಸ್ಥಿರ size_t	    saved_mr_patternlen = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_spats_ಕೊನೆಯ_idx = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_spats_no_hlsearch = 0;
#ಆದರೆ_ಅಂತ್ಯ

// allocated copy of pattern used by search_regcomp()
ಸ್ಥಿರ char_u	    *mr_ನಮೂನೆ = ಖಾಲಿ;
ಸ್ಥಿರ size_t	    mr_patternlen = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    ಪೂರ್ಣಾಂಕ		matched;	// Found a match in this file
} SearchedFile;
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
ಹುಡುಕು_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**ಬಳಸಿದ_pat,
    ಪೂರ್ಣಾಂಕ		pat_save,
    ಪೂರ್ಣಾಂಕ		pat_use,
    ಪೂರ್ಣಾಂಕ		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    ಪೂರ್ಣಾಂಕ		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ಆದರೆ (pat == ಖಾಲಿ || *pat == NUL)
    {
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ (pat_use == RE_ಕೊನೆಯ)
	    i = ಕೊನೆಯ_idx;
	ಇಲ್ಲದಿದ್ದರೆ
	    i = pat_use;
	ಆದರೆ (spats[i].pat == ಖಾಲಿ)	// pattern was never defined
	{
	    ಆದರೆ (pat_use == RE_SUBST)
		emsg(_(e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    rc_did_emsg = TRUE;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (options & ಹುಡುಕು_HIS)	// put new pattern in history
	ಕೂಡಿಸು_ಗಮ್ಯ_history(HIST_ಹುಡುಕು, pat, patlen, TRUE, NUL);

    ಆದರೆ (ಬಳಸಿದ_pat)
	*ಬಳಸಿದ_pat = pat;

    vim_free(mr_ನಮೂನೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_ನಮೂನೆ = ಹಿಮ್ಮುಖ_ಪಠ್ಯ(pat);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	mr_ನಮೂನೆ = vim_strnsave(pat, patlen);
    ಆದರೆ (mr_ನಮೂನೆ == ಖಾಲಿ)
	mr_patternlen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ಆದರೆ (!(options & ಹುಡುಕು_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ಆದರೆ (pat_save == RE_ಹುಡುಕು || pat_save == RE_BOTH)
	    save_re_pat(RE_ಹುಡುಕು, pat, patlen, magic);
	// substitute or global command
	ಆದರೆ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ಆದರೆ (regmatch->regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_ಹುಡುಕು_pat(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ mr_ನಮೂನೆ;
}

    ಶೂನ್ಯ
save_re_pat(ಪೂರ್ಣಾಂಕ idx, char_u *pat, size_t patlen, ಪೂರ್ಣಾಂಕ magic)
{
    ಆದರೆ (spats[idx].pat == pat)
	ಹಿಂತಿರುಗಿ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ಆದರೆ (spats[idx].pat == ಖಾಲಿ)
	spats[idx].patlen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    ಕೊನೆಯ_idx = idx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ಆದರೆ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ಗಣ_no_hlsearch(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_level = 0;

    ಶೂನ್ಯ
save_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (save_level++ != 0)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ಆದರೆ (spats[i].pat != ಖಾಲಿ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ಆದರೆ (saved_spats[i].pat == ಖಾಲಿ)
		saved_spats[i].patlen = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ಆದರೆ (mr_ನಮೂನೆ == ಖಾಲಿ)
	saved_mr_ನಮೂನೆ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	saved_mr_ನಮೂನೆ = vim_strnsave(mr_ನಮೂನೆ, mr_patternlen);
    ಆದರೆ (saved_mr_ನಮೂನೆ == ಖಾಲಿ)
	saved_mr_patternlen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	saved_mr_patternlen = mr_patternlen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    saved_spats_ಕೊನೆಯ_idx = ಕೊನೆಯ_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
restore_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (--save_level != 0)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ಆದರೆ defined(FEAT_EVAL)
    ಗಣ_vv_searchforward();
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(mr_ನಮೂನೆ);
    mr_ನಮೂನೆ = saved_mr_ನಮೂನೆ;
    mr_patternlen = saved_mr_patternlen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಕೊನೆಯ_idx = saved_spats_ಕೊನೆಯ_idx;
    ಗಣ_no_hlsearch(saved_spats_no_hlsearch);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_ಹುಡುಕು_patterns(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_ನಮೂನೆ);
    mr_patternlen = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ಸ್ಥಿರ spat_T	    saved_ಕೊನೆಯ_ಹುಡುಕು_spat;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    did_save_ಕೊನೆಯ_ಹುಡುಕು_spat = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_ಕೊನೆಯ_idx = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_no_hlsearch = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    ಶೂನ್ಯ
save_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (++did_save_ಕೊನೆಯ_ಹುಡುಕು_spat != 1)
	// nested call, nothing to do
	ಹಿಂತಿರುಗಿ;

    saved_ಕೊನೆಯ_ಹುಡುಕು_spat = spats[RE_ಹುಡುಕು];
    ಆದರೆ (spats[RE_ಹುಡುಕು].pat != ಖಾಲಿ)
    {
	saved_ಕೊನೆಯ_ಹುಡುಕು_spat.pat = vim_strnsave(spats[RE_ಹುಡುಕು].pat, spats[RE_ಹುಡುಕು].patlen);
	ಆದರೆ (saved_ಕೊನೆಯ_ಹುಡುಕು_spat.pat == ಖಾಲಿ)
	    saved_ಕೊನೆಯ_ಹುಡುಕು_spat.patlen = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    saved_ಕೊನೆಯ_ಹುಡುಕು_spat.patlen = spats[RE_ಹುಡುಕು].patlen;
    }
    saved_ಕೊನೆಯ_idx = ಕೊನೆಯ_idx;
    saved_no_hlsearch = no_hlsearch;
}

    ಶೂನ್ಯ
restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (--did_save_ಕೊನೆಯ_ಹುಡುಕು_spat > 0)
	// nested call, nothing to do
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (did_save_ಕೊನೆಯ_ಹುಡುಕು_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ಹಿಂತಿರುಗಿ;
    }

    vim_free(spats[RE_ಹುಡುಕು].pat);
    spats[RE_ಹುಡುಕು] = saved_ಕೊನೆಯ_ಹುಡುಕು_spat;
    saved_ಕೊನೆಯ_ಹುಡುಕು_spat.pat = ಖಾಲಿ;
    saved_ಕೊನೆಯ_ಹುಡುಕು_spat.patlen = 0;
# ಆದರೆ defined(FEAT_EVAL)
    ಗಣ_vv_searchforward();
# endif
    ಕೊನೆಯ_idx = saved_ಕೊನೆಯ_idx;
    ಗಣ_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_incsearch_state(ಶೂನ್ಯ)
{
    saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol = ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;
    saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು  = ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_incsearch_state(ಶೂನ್ಯ)
{
    ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol = saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;
    ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು  = saved_ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು;
}

    char_u *
ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ spats[RE_ಹುಡುಕು].pat;
}

    size_t
ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ spats[RE_ಹುಡುಕು].patlen;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    ಪೂರ್ಣಾಂಕ
ignorecase(char_u *pat)
{
    ಹಿಂತಿರುಗಿ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    ಪೂರ್ಣಾಂಕ
ignorecase_opt(char_u *pat, ಪೂರ್ಣಾಂಕ ic_ಒಳ, ಪೂರ್ಣಾಂಕ scs)
{
    ಪೂರ್ಣಾಂಕ		ic = ic_ಒಳ;

    ಆದರೆ (ic && !no_smartcase && scs
			    && !(ctrl_x_ಬಹುಲಕ_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ಹಿಂತಿರುಗಿ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    ಪೂರ್ಣಾಂಕ
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (ಶೂನ್ಯ)skip_regexp_ex(pat, NUL, magic_isset(), ಖಾಲಿ, ಖಾಲಿ, &magic_val);

    ಆಗ (*p != NUL)
    {
	ಪೂರ್ಣಾಂಕ		l;

	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ಆದರೆ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ಹಿಂತಿರುಗಿ TRUE;
	    p += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ಆದರೆ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] != NUL)  // skip "\X"
		p += 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		p += 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ಆದರೆ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		p++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_ISUPPER(*p))
	    ಹಿಂತಿರುಗಿ TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++p;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_EVAL)
    char_u *
ಕೊನೆಯ_csearch(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ lastc_ಬೈಟುಗಳು;
}

    ಪೂರ್ಣಾಂಕ
ಕೊನೆಯ_csearch_forward(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ lastcdir == FORWARD;
}

    ಪೂರ್ಣಾಂಕ
ಕೊನೆಯ_csearch_until(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಕೊನೆಯ_t_cmd == TRUE;
}

    ಶೂನ್ಯ
ಗಣ_ಕೊನೆಯ_csearch(ಪೂರ್ಣಾಂಕ c, char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    *lastc = c;
    lastc_bytelen = len;
    ಆದರೆ (len)
	ಸ್ಮರಣೆ_ನಕಲು(lastc_ಬೈಟುಗಳು, s, len);
    ಇಲ್ಲದಿದ್ದರೆ
	CLEAR_ಕ್ಷೇತ್ರ(lastc_ಬೈಟುಗಳು);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
ಗಣ_csearch_direction(ಪೂರ್ಣಾಂಕ cdir)
{
    lastcdir = cdir;
}

    ಶೂನ್ಯ
ಗಣ_csearch_until(ಪೂರ್ಣಾಂಕ t_cmd)
{
    ಕೊನೆಯ_t_cmd = t_cmd;
}

    char_u *
ಕೊನೆಯ_ಹುಡುಕು_pat(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ spats[ಕೊನೆಯ_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    ಶೂನ್ಯ
reset_ಹುಡುಕು_dir(ಶೂನ್ಯ)
{
    spats[0].off.dir = '/';
#ಆದರೆ defined(FEAT_EVAL)
    ಗಣ_vv_searchforward();
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    ಶೂನ್ಯ
ಗಣ_ಕೊನೆಯ_ಹುಡುಕು_pat(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		magic,
    ಪೂರ್ಣಾಂಕ		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ಆದರೆ (*s == NUL)
	spats[idx].pat = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ಆದರೆ (spats[idx].pat == ಖಾಲಿ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ಆದರೆ defined(FEAT_EVAL)
    ಗಣ_vv_searchforward();
# endif
    spats[idx].off.ಸಾಲು = FALSE;
    spats[idx].off.ಕೊನೆ = FALSE;
    spats[idx].off.off = 0;
    ಆದರೆ (setlast)
	ಕೊನೆಯ_idx = idx;
    ಆದರೆ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ಆದರೆ (spats[idx].pat == ಖಾಲಿ)
	    saved_spats[idx].pat = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ಆದರೆ (saved_spats[idx].pat == ಖಾಲಿ)
	    saved_spats[idx].patlen = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	saved_spats_ಕೊನೆಯ_idx = ಕೊನೆಯ_idx;
# endif
    }
# ifdef FEAT_ಹುಡುಕು_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ಆದರೆ (p_hls && idx == ಕೊನೆಯ_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    ಶೂನ್ಯ
ಕೊನೆಯ_pat_prog(regmmatch_T *regmatch)
{
    ಆದರೆ (spats[ಕೊನೆಯ_idx].pat == ಖಾಲಿ)
    {
	regmatch->regprog = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (ಶೂನ್ಯ)ಹುಡುಕು_regcomp((char_u *)"", 0, ಖಾಲಿ, 0, ಕೊನೆಯ_idx, ಹುಡುಕು_KEEP, regmatch);
    --emsg_off;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*ಕೊನೆ_pos,	// set to end of the match, unless NULL
    ಪೂರ್ಣಾಂಕ		dir,
    char_u	*pat,
    size_t	patlen,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		options,
    ಪೂರ್ಣಾಂಕ		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	ಲಂಬಸಾಲು;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    ಪೂರ್ಣಾಂಕ		loop;
    pos_T	ಆರಂಭ_pos;
    ಪೂರ್ಣಾಂಕ		at_ಮೊದಲ_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		extra_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_char_len;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ_ok;
    ದೀರ್ಘ	nmatched;
    ಪೂರ್ಣಾಂಕ		submatch = 0;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_ಹೊಂದಿಕೆ = TRUE;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ		break_loop = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    ಪೂರ್ಣಾಂಕ		ಬಳಸದ_timeout_flag = FALSE;
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು = &ಬಳಸದ_timeout_flag;  // set when timed out.
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_from_ಹೊಂದಿಕೆ_ಕೊನೆ;		    // vi-compatible search?

    ಆದರೆ (ಹುಡುಕು_regcomp(pat, patlen, ಖಾಲಿ, RE_ಹುಡುಕು, pat_use,
		   (options & (ಹುಡುಕು_HIS + ಹುಡುಕು_KEEP)), &regmatch) == FAIL)
    {
	ಆದರೆ ((options & ಹುಡುಕು_ಸಂದೇಶ) && !rc_did_emsg)
	    semsg(_(e_invalid_ಹುಡುಕು_string_str), mr_ನಮೂನೆ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹುಡುಕು_from_ಹೊಂದಿಕೆ_ಕೊನೆ = vim_strchr(p_cpo, CPO_ಹುಡುಕು) != ಖಾಲಿ;

    ಆದರೆ (extra_arg != ಖಾಲಿ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	ಆದರೆ (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_ಹೊರಹೋಗು = &extra_arg->sa_timed_ಹೊರಹೋಗು;
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * find the string
     */
    ಮಾಡು	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	ಆದರೆ (pos->ಲಂಬಸಾಲು == MAXCOL)
	    ಆರಂಭ_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						    && pos->ಲಂಬಸಾಲು < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ಆದರೆ (ml_get_buf_len(buf, pos->lnum) <= pos->ಲಂಬಸಾಲು)
		ಆರಂಭ_char_len = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆರಂಭ_char_len = (*mb_ptr2len)(ptr + pos->ಲಂಬಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆರಂಭ_char_len = 1;
	ಆದರೆ (dir == FORWARD)
	{
	    ಆದರೆ (options & ಹುಡುಕು_ಆರಂಭ)
		extra_ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		extra_ಲಂಬಸಾಲು = ಆರಂಭ_char_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (options & ಹುಡುಕು_ಆರಂಭ)
		extra_ಲಂಬಸಾಲು = ಆರಂಭ_char_len;
	    ಇಲ್ಲದಿದ್ದರೆ
		extra_ಲಂಬಸಾಲು = 0;
	}

	ಆರಂಭ_pos = *pos;	// remember start pos for detecting no match
	ಸಿಕ್ಕಿತು = 0;		// default: not found
	at_ಮೊದಲ_ಸಾಲು = TRUE;	// default: start in first line
	ಆದರೆ (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->ಲಂಬಸಾಲು = 0;
	    at_ಮೊದಲ_ಸಾಲು = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	ಆದರೆ (dir == BACKWARD && ಆರಂಭ_pos.ಲಂಬಸಾಲು == 0
					     && (options & ಹುಡುಕು_ಆರಂಭ) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_ಮೊದಲ_ಸಾಲು = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    lnum = pos->lnum;

	ಗಾಗಿ (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    ಗಾಗಿ ( ; lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
					   lnum += dir, at_ಮೊದಲ_ಸಾಲು = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ಆದರೆ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    ನಿಲ್ಲಿಸು;
		// Stop after passing the time limit.
		ಆದರೆ (*timed_ಹೊರಹೋಗು)
		    ನಿಲ್ಲಿಸು;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		ಲಂಬಸಾಲು = at_ಮೊದಲ_ಸಾಲು && (options & ಹುಡುಕು_ಲಂಬಸಾಲು) ? pos->ಲಂಬಸಾಲು
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
		// vim_regexec_multi() may clear "regprog"
		ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		// Abort searching on an error (e.g., out of stack).
		ಆದರೆ (called_emsg > called_emsg_before || *timed_ಹೊರಹೋಗು)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    submatch = ಮೊದಲ_submatch(&regmatch);
#ಆದರೆ_ಅಂತ್ಯ
		    // "lnum" may be past end of buffer for "\n\zs".
		    ಆದರೆ (lnum + matchpos.lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ptr = (char_u *)"";
		    ಇಲ್ಲದಿದ್ದರೆ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ಆದರೆ (dir == FORWARD && at_ಮೊದಲ_ಸಾಲು)
		    {
			ಹೊಂದಿಕೆ_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ಆಗ (matchpos.lnum == 0
				&& ((options & ಹುಡುಕು_ಕೊನೆ) && ಮೊದಲ_ಹೊಂದಿಕೆ
				    ?  (nmatched == 1
					&& (ಪೂರ್ಣಾಂಕ)endpos.ಲಂಬಸಾಲು - 1
					     < (ಪೂರ್ಣಾಂಕ)ಆರಂಭ_pos.ಲಂಬಸಾಲು + extra_ಲಂಬಸಾಲು)
				    : ((ಪೂರ್ಣಾಂಕ)matchpos.ಲಂಬಸಾಲು
						  - (ptr[matchpos.ಲಂಬಸಾಲು] == NUL)
					    < (ಪೂರ್ಣಾಂಕ)ಆರಂಭ_pos.ಲಂಬಸಾಲು + extra_ಲಂಬಸಾಲು)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ಆದರೆ (ಹುಡುಕು_from_ಹೊಂದಿಕೆ_ಕೊನೆ)
			    {
				ಆದರೆ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    ಹೊಂದಿಕೆ_ok = FALSE;
				    ನಿಲ್ಲಿಸು;
				}
				matchcol = endpos.ಲಂಬಸಾಲು;
				// for empty match: advance one char
				ಆದರೆ (matchcol == matchpos.ಲಂಬಸಾಲು
						      && ptr[matchcol] != NUL)
				{
				    ಆದರೆ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಇಲ್ಲದಿದ್ದರೆ
					++matchcol;
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ಆದರೆ (ptr[matchcol] != NUL)
				{
				    ಆದರೆ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    ಇಲ್ಲದಿದ್ದರೆ
					++matchcol;
				}
			    }
			    ಆದರೆ (matchcol == 0 && (options & ಹುಡುಕು_ಆರಂಭ))
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_ಹೊರಹೋಗು)) == 0)
			    {
				ಹೊಂದಿಕೆ_ok = FALSE;
				ನಿಲ್ಲಿಸು;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
				ನಿಲ್ಲಿಸು;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    submatch = ಮೊದಲ_submatch(&regmatch);
#ಆದರೆ_ಅಂತ್ಯ

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ಆದರೆ (!ಹೊಂದಿಕೆ_ok)
			    ಮುಂದುವರಿ;
		    }
		    ಆದರೆ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			ಹೊಂದಿಕೆ_ok = FALSE;
			ಗಾಗಿ (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ಆದರೆ (loop
				|| ((options & ಹುಡುಕು_ಕೊನೆ)
				    ? (lnum + regmatch.endpos[0].lnum
							      < ಆರಂಭ_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == ಆರಂಭ_pos.lnum
					     && (ಪೂರ್ಣಾಂಕ)regmatch.endpos[0].ಲಂಬಸಾಲು - 1
							< (ಪೂರ್ಣಾಂಕ)ಆರಂಭ_pos.ಲಂಬಸಾಲು
								+ extra_ಲಂಬಸಾಲು))
				    : (lnum + regmatch.startpos[0].lnum
							      < ಆರಂಭ_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == ಆರಂಭ_pos.lnum
					     && (ಪೂರ್ಣಾಂಕ)regmatch.startpos[0].ಲಂಬಸಾಲು
						      < (ಪೂರ್ಣಾಂಕ)ಆರಂಭ_pos.ಲಂಬಸಾಲು
							      + extra_ಲಂಬಸಾಲು))))
			    {
				ಹೊಂದಿಕೆ_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				submatch = ಮೊದಲ_submatch(&regmatch);
#ಆದರೆ_ಅಂತ್ಯ
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				ನಿಲ್ಲಿಸು;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ಆದರೆ (ಹುಡುಕು_from_ಹೊಂದಿಕೆ_ಕೊನೆ)
			    {
				ಆದರೆ (nmatched > 1)
				    ನಿಲ್ಲಿಸು;
				matchcol = endpos.ಲಂಬಸಾಲು;
				// for empty match: advance one char
				ಆದರೆ (matchcol == matchpos.ಲಂಬಸಾಲು
						      && ptr[matchcol] != NUL)
				{
				    ಆದರೆ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಇಲ್ಲದಿದ್ದರೆ
					++matchcol;
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				// Stop when the match is in a next line.
				ಆದರೆ (matchpos.lnum > 0)
				    ನಿಲ್ಲಿಸು;
				matchcol = matchpos.ಲಂಬಸಾಲು;
				ಆದರೆ (ptr[matchcol] != NUL)
				{
				    ಆದರೆ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ಇಲ್ಲದಿದ್ದರೆ
					++matchcol;
				}
			    }
			    ಆದರೆ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_ಹೊರಹೋಗು)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ಆದರೆ (*timed_ಹೊರಹೋಗು)
				    ಹೊಂದಿಕೆ_ok = FALSE;
				ನಿಲ್ಲಿಸು;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
				ನಿಲ್ಲಿಸು;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ಆದರೆ (!ಹೊಂದಿಕೆ_ok)
			    ಮುಂದುವರಿ;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ಆದರೆ ((options & ಹುಡುಕು_ಕೊನೆ) && !(options & ಹುಡುಕು_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.ಲಂಬಸಾಲು == endpos.ಲಂಬಸಾಲು))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->ಲಂಬಸಾಲು = endpos.ಲಂಬಸಾಲು;
			ಆದರೆ (endpos.ಲಂಬಸಾಲು == 0)
			{
			    ಆದರೆ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->ಲಂಬಸಾಲು = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    --pos->ಲಂಬಸಾಲು;
			    ಆದರೆ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + pos->ಲಂಬಸಾಲು);
			    }
			}
			ಆದರೆ (ಕೊನೆ_pos != ಖಾಲಿ)
			{
			    ಕೊನೆ_pos->lnum = lnum + matchpos.lnum;
			    ಕೊನೆ_pos->ಲಂಬಸಾಲು = matchpos.ಲಂಬಸಾಲು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->ಲಂಬಸಾಲು = matchpos.ಲಂಬಸಾಲು;
			ಆದರೆ (ಕೊನೆ_pos != ಖಾಲಿ)
			{
			    ಕೊನೆ_pos->lnum = lnum + endpos.lnum;
			    ಕೊನೆ_pos->ಲಂಬಸಾಲು = endpos.ಲಂಬಸಾಲು;
			}
		    }
		    pos->coladd = 0;
		    ಆದರೆ (ಕೊನೆ_pos != ಖಾಲಿ)
			ಕೊನೆ_pos->coladd = 0;
		    ಸಿಕ್ಕಿತು = 1;
		    ಮೊದಲ_ಹೊಂದಿಕೆ = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು = endpos.lnum - matchpos.lnum;
		    ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol = endpos.ಲಂಬಸಾಲು;
		    ನಿಲ್ಲಿಸು;
		}
		ಸಾಲು_breakcheck();	// stop if ctrl-C typed
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ಆದರೆ ((options & ಹುಡುಕು_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (loop && lnum == ಆರಂಭ_pos.lnum)
		    ನಿಲ್ಲಿಸು;	    // if second loop, stop where started
	    }
	    at_ಮೊದಲ_ಸಾಲು = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ಆದರೆ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_ಹೊರಹೋಗು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
			      || break_loop
#ಆದರೆ_ಅಂತ್ಯ
			      || ಸಿಕ್ಕಿತು || loop)
		ನಿಲ್ಲಿಸು;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ಆದರೆ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		lnum = 1;
	    ಆದರೆ (!shortmess(SHM_ಹುಡುಕು)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & ಹುಡುಕು_ಸಂದೇಶ))
		give_warning((char_u *)_(dir == BACKWARD
					  ? ತುದಿ_bot_ಸಂದೇಶ : bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    ಆದರೆ (extra_arg != ಖಾಲಿ)
		extra_arg->sa_wrapped = TRUE;
	}
	ಆದರೆ (got_int || called_emsg > called_emsg_before || *timed_ಹೊರಹೋಗು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		|| break_loop
#ಆದರೆ_ಅಂತ್ಯ
		)
	    ನಿಲ್ಲಿಸು;
    }
    ಆಗ (--ಎಣಿಕೆ_2 > 0 && ಸಿಕ್ಕಿತು);   // stop after count matches or no match

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (extra_arg != ಖಾಲಿ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#ಆದರೆ_ಅಂತ್ಯ
    vim_regfree(regmatch.regprog);

    ಆದರೆ (!ಸಿಕ್ಕಿತು)		    // did not find it
    {
	ಆದರೆ (got_int)
	    emsg(_(e_interrupted));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((options & ಹುಡುಕು_ಸಂದೇಶ) == ಹುಡುಕು_ಸಂದೇಶ)
	{
	    ಆದರೆ (p_ws)
		semsg(_(e_ನಮೂನೆ_not_ಸಿಕ್ಕಿತು_str), mr_ನಮೂನೆ);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == 0)
		semsg(_(e_ಹುಡುಕು_ದೊರಕಿತು_ತುದಿ_without_ಹೊಂದಿಕೆ_for_str), mr_ನಮೂನೆ);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_ಹುಡುಕು_ದೊರಕಿತು_bottom_without_ಹೊಂದಿಕೆ_for_str), mr_ನಮೂನೆ);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ಆದರೆ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	pos->ಲಂಬಸಾಲು = ml_get_buf_len(buf, pos->lnum);
	ಆದರೆ (pos->ಲಂಬಸಾಲು > 0)
	    --pos->ಲಂಬಸಾಲು;
    }

    ಹಿಂತಿರುಗಿ submatch + 1;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    ಶೂನ್ಯ
ಗಣ_ಹುಡುಕು_direction(ಪೂರ್ಣಾಂಕ cdir)
{
    spats[0].off.dir = cdir;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_vv_searchforward(ಶೂನ್ಯ)
{
    ಗಣ_vim_var_nr(VV_SEARCHFORWARD, (ದೀರ್ಘ)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಮೊದಲ_submatch(regmmatch_T *rp)
{
    ಪೂರ್ಣಾಂಕ		submatch;

    ಗಾಗಿ (submatch = 1; ; ++submatch)
    {
	ಆದರೆ (rp->startpos[submatch].lnum >= 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (submatch == 9)
	{
	    submatch = 0;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ submatch;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
parse_ಹುಡುಕು_ನಮೂನೆ_ಸ್ಥಾನಾಂತರ(
    char_u	**pat,
    size_t	*patlen,
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_delim,
    ಪೂರ್ಣಾಂಕ		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ಆದರೆ (*pat == ಖಾಲಿ || **pat == NUL)
	ಹಿಂತಿರುಗಿ 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ಖಾಲಿ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, ಹುಡುಕು_delim, magic_isset(),
						    strcopy, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (ಪೂರ್ಣಾಂಕ)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ಆದರೆ (*p == ಹುಡುಕು_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    ಸ್ಥಾನಾಂತರ->ಸಾಲು = FALSE;
    ಸ್ಥಾನಾಂತರ->ಕೊನೆ = FALSE;
    ಸ್ಥಾನಾಂತರ->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    ಆದರೆ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	ಸ್ಥಾನಾಂತರ->ಸಾಲು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((options & ಹುಡುಕು_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ಆದರೆ (*p == 'e')		// end
	    ಸ್ಥಾನಾಂತರ->ಕೊನೆ = ಹುಡುಕು_ಕೊನೆ;
	++p;
    }
    ಆದರೆ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ಆದರೆ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    ಸ್ಥಾನಾಂತರ->off = atol((ಅಕ್ಷರ *)p);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '-')	    // single '-'
	    ಸ್ಥಾನಾಂತರ->off = -1;
	ಇಲ್ಲದಿದ್ದರೆ			    // single '+'
	    ಸ್ಥಾನಾಂತರ->off = 1;
	++p;
	ಆಗ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (ಪೂರ್ಣಾಂಕ)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ಹಿಂತಿರುಗಿ cmdlen;
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
    ಪೂರ್ಣಾಂಕ
do_ಹುಡುಕು(
    oparg_T	    *oap,	// can be NULL
    ಪೂರ್ಣಾಂಕ		    dirc,	// '/' or '?'
    ಪೂರ್ಣಾಂಕ		    ಹುಡುಕು_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    ದೀರ್ಘ	    ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    ಪೂರ್ಣಾಂಕ		    retval;	// Return value
    char_u	    *p;
    ದೀರ್ಘ	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    show_ಹುಡುಕು_stats;
    char_u	    *msgbuf = ಖಾಲಿ;
    size_t	    msgbuflen = 0;
    ಪೂರ್ಣಾಂಕ		    has_ಸ್ಥಾನಾಂತರ = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ಆದರೆ (spats[0].off.ಸಾಲು && vim_strchr(p_cpo, CPO_LINEOFF) != ಖಾಲಿ)
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
    ಆದರೆ (dirc == 0)
	dirc = spats[0].off.dir;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	spats[0].off.dir = dirc;
#ಆದರೆ defined(FEAT_EVAL)
	ಗಣ_vv_searchforward();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (options & ಹುಡುಕು_REV)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (dirc == '/')
	    dirc = '?';
	ಇಲ್ಲದಿದ್ದರೆ
	    dirc = '/';
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ಆದರೆ (dirc == '/')
    {
	ಆದರೆ (hasFolding(pos.lnum, ಖಾಲಿ, &pos.lnum))
	    pos.ಲಂಬಸಾಲು = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (hasFolding(pos.lnum, &pos.lnum, ಖಾಲಿ))
	    pos.ಲಂಬಸಾಲು = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ಆದರೆ (no_hlsearch && !(options & ಹುಡುಕು_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	ಗಣ_no_hlsearch(FALSE);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ		show_ತುದಿ_bot_ಸಂದೇಶ = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ಖಾಲಿ;
					    // use previous pattern
	ಆದರೆ (pat == ಖಾಲಿ || *pat == NUL || *pat == ಹುಡುಕು_delim)
	{
	    ಆದರೆ (spats[RE_ಹುಡುಕು].pat == ಖಾಲಿ)	    // no previous pattern
	    {
		ಆದರೆ (spats[RE_SUBST].pat == ಖಾಲಿ)
		{
		    emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
		    retval = 0;
		    ಹೋಗು ಕೊನೆ_do_ಹುಡುಕು;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ಆದರೆ (pat != ಖಾಲಿ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_ಹುಡುಕು_ನಮೂನೆ_ಸ್ಥಾನಾಂತರ(&pat, &patlen,
				ಹುಡುಕು_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ಆದರೆ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_ಹುಡುಕು_stats = FALSE;
	ಆದರೆ ((options & ಹುಡುಕು_ECHO) && messaging()
		&& !ಸಂದೇಶ_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    ಸಂದೇಶ_ಆರಂಭ();

	    // Get the offset, so we know how long it is.
	    ಆದರೆ (!cmd_silent &&
		    (spats[0].off.ಸಾಲು || spats[0].off.ಕೊನೆ || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ಆದರೆ (spats[0].off.ಕೊನೆ)
		    off_buf[off_len++] = 'e';
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!spats[0].off.ಸಾಲು)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ಆದರೆ (spats[0].off.off != 0 || spats[0].off.ಸಾಲು)
		    off_len += vim_snprintf((ಅಕ್ಷರ *)off_buf + off_len,
			   ಗಾತ್ರ(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ಆದರೆ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ಆದರೆ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ಆದರೆ (ಸಂದೇಶ_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - ಸಂದೇಶ_ಅಡ್ಡಸಾಲು) * cmdline_ಅಗಲ - 1;
		ಇಲ್ಲದಿದ್ದರೆ
		    // Use up to 'showcmd' column.
		    msgbufsize = (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - ಸಂದೇಶ_ಅಡ್ಡಸಾಲು - 1) * cmdline_ಅಗಲ
			+ sc_ಲಂಬಸಾಲು - 1;
		ಆದರೆ (msgbufsize < plen + off_len + ಹುಡುಕು_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + ಹುಡುಕು_STAT_BUF_LEN + 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ಆದರೆ (msgbuf == ಖಾಲಿ)
	    {
		msgbuflen = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ಆದರೆ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ಆದರೆ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mch_memmove(msgbuf + 1, p, plen);
		    ಆದರೆ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = ಸಂದೇಶ_strtrunc(msgbuf, TRUE);
		    ಆದರೆ (trunc != ಖಾಲಿ)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    ಆದರೆ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = ಹಿಮ್ಮುಖ_ಪಠ್ಯ(msgbuf);
			ಆದರೆ (r != ಖಾಲಿ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ಆಗ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ಆದರೆ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ಇಲ್ಲದಿದ್ದರೆ
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    ಸಂದೇಶ_outtrans(msgbuf);
		    ಸಂದೇಶ_clr_eos();
		    ಸಂದೇಶ_check();

		    gotocmdline(FALSE);
		    ಹೊರಹೋಗು_flush();
		    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
		}

		ಆದರೆ (!shortmess(SHM_SEARCHCOUNT))
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
	ಆದರೆ (!spats[0].off.ಸಾಲು && spats[0].off.off && pos.ಲಂಬಸಾಲು < MAXCOL - 2)
	{
	    ಆದರೆ (spats[0].off.off > 0)
	    {
		ಗಾಗಿ (c = spats[0].off.off; c; --c)
		    ಆದರೆ (decl(&pos) == -1)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.ಲಂಬಸಾಲು = MAXCOL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (c = spats[0].off.off; c; ++c)
		    ಆದರೆ (incl(&pos) == -1)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
		    pos.ಲಂಬಸಾಲು = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ಖಾಲಿ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, ಎಣಿಕೆ_2, spats[0].off.ಕೊನೆ + (options &
		       (ಹುಡುಕು_KEEP + ಹುಡುಕು_PEEK + ಹುಡುಕು_HIS
			+ ಹುಡುಕು_ಸಂದೇಶ + ಹುಡುಕು_ಆರಂಭ
			+ ((pat != ಖಾಲಿ && *pat == ';') ? 0 : ಹುಡುಕು_NOOF))),
		RE_ಕೊನೆಯ, sia);

	ಆದರೆ (dircp != ಖಾಲಿ)
	    *dircp = ಹುಡುಕು_delim; // restore second '/' or '?' for normal_cmd()

	ಆದರೆ (!shortmess(SHM_ಹುಡುಕು) && sia && sia->sa_wrapped)
	    show_ತುದಿ_bot_ಸಂದೇಶ = TRUE;

	ಆದರೆ (c == FAIL)
	{
	    retval = 0;
	    ಹೋಗು ಕೊನೆ_do_ಹುಡುಕು;
	}
	ಆದರೆ (spats[0].off.ಕೊನೆ && oap != ಖಾಲಿ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ಆದರೆ (!(options & ಹುಡುಕು_NOOF) || (pat != ಖಾಲಿ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ಆದರೆ (spats[0].off.ಸಾಲು)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ಆದರೆ (c < 1)
		    pos.lnum = 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಇಲ್ಲದಿದ್ದರೆ
		    pos.lnum = c;
		pos.ಲಂಬಸಾಲು = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.ಲಂಬಸಾಲು < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ಆದರೆ (c > 0)
		{
		    ಆಗ (c-- > 0)
			ಆದರೆ (incl(&pos) == -1)
			    ನಿಲ್ಲಿಸು;
		}
		// to the left, check for start of file
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆಗ (c++ < 0)
			ಆದರೆ (decl(&pos) == -1)
			    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ (!EQUAL_POS(pos, org_pos))
		has_ಸ್ಥಾನಾಂತರ = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ಆದರೆ (show_ಹುಡುಕು_stats)
	     cmdline_ಹುಡುಕು_stat(dirc, &pos, &curwin->w_cursor,
				show_ತುದಿ_bot_ಸಂದೇಶ, msgbuf, msgbuflen,
				(ಎಣಿಕೆ_2 != 1 || has_ಸ್ಥಾನಾಂತರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
				 || (!(fdo_flags & FDO_ಹುಡುಕು)
				     && hasFolding(curwin->w_cursor.lnum,
								   ಖಾಲಿ, ಖಾಲಿ))
#ಆದರೆ_ಅಂತ್ಯ
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
	ಆದರೆ (!(options & ಹುಡುಕು_OPT) || pat == ಖಾಲಿ || *pat != ';')
	    ನಿಲ್ಲಿಸು;

	dirc = *++pat;
	--patlen;
	ಹುಡುಕು_delim = dirc;
	ಆದರೆ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    ಹೋಗು ಕೊನೆ_do_ಹುಡುಕು;
	}
	++pat;
	--patlen;
    }

    ಆದರೆ (options & ಹುಡುಕು_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_ಗಣ_curswant = true;

ಕೊನೆ_do_ಹುಡುಕು:
    ಆದರೆ ((options & ಹುಡುಕು_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ
ಹುಡುಕು_for_exact_ಸಾಲು(
    buf_T	*buf,
    pos_T	*pos,
    ಪೂರ್ಣಾಂಕ		dir,
    char_u	*pat)
{
    linenr_T	ಆರಂಭ = 0;
    char_u	*ptr;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		compl_len = ins_compl_len();

    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (;;)
    {
	pos->lnum += dir;
	ಆದರೆ (pos->lnum < 1)
	{
	    ಆದರೆ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಆದರೆ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(ತುದಿ_bot_ಸಂದೇಶ), TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		pos->lnum = 1;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಆದರೆ (p_ws)
	    {
		pos->lnum = 1;
		ಆದರೆ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		pos->lnum = 1;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (pos->lnum == ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಆರಂಭ == 0)
	    ಆರಂಭ = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->ಲಂಬಸಾಲು = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && !compl_ಸ್ಥಿತಿ_sol())
	{
	    ಆದರೆ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ಆದರೆ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ಹಿಂತಿರುಗಿ OK;
	}
    }
    ಹಿಂತಿರುಗಿ FAIL;
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
    ಪೂರ್ಣಾಂಕ
searchc(cmdarg_T *cap, ಪೂರ್ಣಾಂಕ t_cmd)
{
    ಪೂರ್ಣಾಂಕ			c = cap->nchar;	// char to search for
    ಪೂರ್ಣಾಂಕ			dir = cap->arg;	// TRUE for searching forward
    ದೀರ್ಘ		ಎಣಿಕೆ_2 = cap->count1;	// repeat count
    ಪೂರ್ಣಾಂಕ			ಲಂಬಸಾಲು;
    char_u		*p;
    ಪೂರ್ಣಾಂಕ			len;
    ಪೂರ್ಣಾಂಕ			stop = TRUE;

    ಆದರೆ (c != NUL)	// normal search: remember args for repeat
    {
	ಆದರೆ (!ಕೀಲಿ_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    ಗಣ_csearch_direction(dir);
	    ಗಣ_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_ಬೈಟುಗಳು);
	    ಆದರೆ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_ಬೈಟುಗಳು + lastc_bytelen);
		ಆದರೆ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_ಬೈಟುಗಳು + lastc_bytelen);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ		// repeat previous search
    {
	ಆದರೆ (*lastc == NUL && lastc_bytelen <= 1)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ಇಲ್ಲದಿದ್ದರೆ
	    dir = lastcdir;
	t_cmd = ಕೊನೆಯ_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ಆದರೆ (vim_strchr(p_cpo, CPO_SCOLON) == ಖಾಲಿ && ಎಣಿಕೆ_2 == 1 && t_cmd)
	    stop = FALSE;
    }

    ಆದರೆ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    len = ml_get_curline_len();

    ಆಗ (ಎಣಿಕೆ_2--)
    {
	ಆದರೆ (has_mbyte)
	{
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (dir > 0)
		{
		    ಲಂಬಸಾಲು += (*mb_ptr2len)(p + ಲಂಬಸಾಲು);
		    ಆದರೆ (ಲಂಬಸಾಲು >= len)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (ಲಂಬಸಾಲು == 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + ಲಂಬಸಾಲು - 1) + 1;
		}
		ಆದರೆ (lastc_bytelen <= 1)
		{
		    ಆದರೆ (p[ಲಂಬಸಾಲು] == c && stop)
			ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p + ಲಂಬಸಾಲು, lastc_ಬೈಟುಗಳು, lastc_bytelen) == 0
								       && stop)
		    ನಿಲ್ಲಿಸು;
		stop = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ ((ಲಂಬಸಾಲು += dir) < 0 || ಲಂಬಸಾಲು >= len)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (p[ಲಂಬಸಾಲು] == c && stop)
		    ನಿಲ್ಲಿಸು;
		stop = TRUE;
	    }
	}
    }

    ಆದರೆ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	ಲಂಬಸಾಲು -= dir;
	ಆದರೆ (has_mbyte)
	{
	    ಆದರೆ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		ಲಂಬಸಾಲು += lastc_bytelen - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		// To previous char, which may be multi-byte.
		ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + ಲಂಬಸಾಲು);
	}
    }
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

    ಹಿಂತಿರುಗಿ OK;
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
findmatch(oparg_T *oap, ಪೂರ್ಣಾಂಕ initc)
{
    ಹಿಂತಿರುಗಿ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_prevcol(
    char_u	*linep,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ch,
    ಪೂರ್ಣಾಂಕ		*prevcol)
{
    --ಲಂಬಸಾಲು;
    ಆದರೆ (ಲಂಬಸಾಲು > 0 && has_mbyte)
	ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(linep, linep + ಲಂಬಸಾಲು);
    ಆದರೆ (prevcol)
	*prevcol = ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ (ಲಂಬಸಾಲು >= 0 && linep[ಲಂಬಸಾಲು] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_rawstring_ಕೊನೆ(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;

    ಗಾಗಿ (p = linep + startpos->ಲಂಬಸಾಲು + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->ಲಂಬಸಾಲು - 1;
    delim_copy = vim_strnsave(linep + startpos->ಲಂಬಸಾಲು + 1, delim_len);
    ಆದರೆ (delim_copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *ಸಾಲು = ml_get(lnum);

	ಗಾಗಿ (p = ಸಾಲು + (lnum == startpos->lnum
					    ? startpos->ಲಂಬಸಾಲು + 1 : 0); *p; ++p)
	{
	    ಆದರೆ (lnum == endpos->lnum && (colnr_T)(p - ಸಾಲು) >= endpos->ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ಸಿಕ್ಕಿತು)
	    ನಿಲ್ಲಿಸು;
    }
    vim_free(delim_copy);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_mps_ಮೌಲ್ಯಗಳು(
    ಪೂರ್ಣಾಂಕ	    *initc,
    ಪೂರ್ಣಾಂಕ	    *findc,
    ಪೂರ್ಣಾಂಕ	    *backwards,
    ಪೂರ್ಣಾಂಕ	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ಆಗ (*ptr != NUL)
    {
	ಆದರೆ (has_mbyte)
	{
	    char_u *prev;

	    ಆದರೆ (mb_ptr2char(ptr) == *initc)
	    {
		ಆದರೆ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ಹಿಂತಿರುಗಿ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ಆದರೆ (mb_ptr2char(ptr) == *initc)
	    {
		ಆದರೆ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		ಹಿಂತಿರುಗಿ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*ptr == *initc)
	    {
		ಆದರೆ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ಹಿಂತಿರುಗಿ;
	    }
	    ptr += 2;
	    ಆದರೆ (*ptr == *initc)
	    {
		ಆದರೆ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ಹಿಂತಿರುಗಿ;
	    }
	    ++ptr;
	}
	ಆದರೆ (*ptr == ',')
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
    ಪೂರ್ಣಾಂಕ		initc,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		maxtravel)
{
    ಸ್ಥಿರ pos_T pos;			// current search position
    ಪೂರ್ಣಾಂಕ		findc = 0;		// matching brace
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;		// cumulative number of braces
    ಪೂರ್ಣಾಂಕ		backwards = FALSE;	// init for gcc
    ಪೂರ್ಣಾಂಕ		raw_string = FALSE;	// search for raw string
    ಪೂರ್ಣಾಂಕ		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		do_quotes;		// check for quotes in current line
    ಪೂರ್ಣಾಂಕ		at_ಆರಂಭ;		// do_quotes value at start position
    ಪೂರ್ಣಾಂಕ		hash_dir = 0;		// Direction searched for # things
    ಪೂರ್ಣಾಂಕ		comment_dir = 0;	// Direction searched for comments
    pos_T	ಹೊಂದಿಕೆ_pos;		// Where last slash-star was found
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಒಳ_quotes;	// start position is in quotes
    ಪೂರ್ಣಾಂಕ		traveled = 0;		// how far we've searched so far
    ಪೂರ್ಣಾಂಕ		ignore_cend = FALSE;    // ignore comment end
    ಪೂರ್ಣಾಂಕ		cpo_ಹೊಂದಿಕೆ;		// vi compatible matching
    ಪೂರ್ಣಾಂಕ		cpo_bsl;		// don't recognize backslashes
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ_escaped = 0;	// search for escaped match
    ಪೂರ್ಣಾಂಕ		dir;			// Direction to search
    ಪೂರ್ಣಾಂಕ		comment_ಲಂಬಸಾಲು = MAXCOL;   // start of / / comment
    ಪೂರ್ಣಾಂಕ		lispcomm = FALSE;	// inside of Lisp-style comment
    ಪೂರ್ಣಾಂಕ		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    ಪೂರ್ಣಾಂಕ		skip_comments = (flags & FM_SKIPCOMM) != 0;
    ಪೂರ್ಣಾಂಕ		ಒಳ_ಖಂಡ_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_ಹೊಂದಿಕೆ = (vim_strchr(p_cpo, CPO_ಹೊಂದಿಕೆ) != ಖಾಲಿ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ಖಾಲಿ);

    // Direction to search when initc is '/', '*' or '#'
    ಆದರೆ (flags & FM_BACKWARD)
	dir = BACKWARD;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & FM_FORWARD)
	dir = FORWARD;
    ಇಲ್ಲದಿದ್ದರೆ
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ಆದರೆ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ಆದರೆ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (initc != '#' && initc != NUL)
    {
	ಕಂಡುಹಿಡಿ_mps_ಮೌಲ್ಯಗಳು(&initc, &findc, &backwards, TRUE);
	ಆದರೆ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ಆದರೆ (findc == NUL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ಆದರೆ (initc == '#')
	{
	    hash_dir = dir;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ಆದರೆ (!cpo_ಹೊಂದಿಕೆ)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ಆದರೆ (*ptr == '#' && pos.ಲಂಬಸಾಲು <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ಆದರೆ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು] == '/')
		{
		    ಆದರೆ (linep[pos.ಲಂಬಸಾಲು + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.ಲಂಬಸಾಲು++;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.ಲಂಬಸಾಲು > 0 && linep[pos.ಲಂಬಸಾಲು - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.ಲಂಬಸಾಲು--;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು] == '*')
		{
		    ಆದರೆ (linep[pos.ಲಂಬಸಾಲು + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.ಲಂಬಸಾಲು > 0 && linep[pos.ಲಂಬಸಾಲು - 1] == '/')
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
	    ಆದರೆ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ಆದರೆ (linep[pos.ಲಂಬಸಾಲು] == NUL && pos.ಲಂಬಸಾಲು)
		    --pos.ಲಂಬಸಾಲು;
		ಗಾಗಿ (;;)
		{
		    initc = PTR2CHAR(linep + pos.ಲಂಬಸಾಲು);
		    ಆದರೆ (initc == NUL)
			ನಿಲ್ಲಿಸು;

		    ಕಂಡುಹಿಡಿ_mps_ಮೌಲ್ಯಗಳು(&initc, &findc, &backwards, FALSE);
		    ಆದರೆ (findc)
			ನಿಲ್ಲಿಸು;
		    pos.ಲಂಬಸಾಲು += mb_ptr2len(linep + pos.ಲಂಬಸಾಲು);
		}
		ಆದರೆ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ಆದರೆ (!cpo_ಹೊಂದಿಕೆ && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!cpo_bsl)
		{
		    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    ಗಾಗಿ (ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು; check_prevcol(linep, ಲಂಬಸಾಲು, '\\', &ಲಂಬಸಾಲು);)
			bslcnt++;
		    ಹೊಂದಿಕೆ_escaped = (bslcnt & 1);
		}
	    }
	}
	ಆದರೆ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ಆದರೆ (oap != ಖಾಲಿ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ಆದರೆ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ಆದರೆ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    pos.ಲಂಬಸಾಲು = 0;
	    ಆಗ (!got_int)
	    {
		ಆದರೆ (hash_dir > 0)
		{
		    ಆದರೆ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.lnum == 1)
		    ನಿಲ್ಲಿಸು;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		ಸಾಲು_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ಆದರೆ (*ptr != '#')
		    ಮುಂದುವರಿ;
		pos.ಲಂಬಸಾಲು = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ಆದರೆ (hash_dir > 0)
		{
		    ಆದರೆ (STRNCMP(ptr, "if", 2) == 0)
			ಎಣಿಕೆ_2++;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ptr, "el", 2) == 0)
		    {
			ಆದರೆ (ಎಣಿಕೆ_2 == 0)
			    ಹಿಂತಿರುಗಿ &pos;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ಆದರೆ (ಎಣಿಕೆ_2 == 0)
			    ಹಿಂತಿರುಗಿ &pos;
			ಎಣಿಕೆ_2--;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (STRNCMP(ptr, "if", 2) == 0)
		    {
			ಆದರೆ (ಎಣಿಕೆ_2 == 0)
			    ಹಿಂತಿರುಗಿ &pos;
			ಎಣಿಕೆ_2--;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ಆದರೆ (ಎಣಿಕೆ_2 == 0)
			    ಹಿಂತಿರುಗಿ &pos;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ptr, "endif", 5) == 0)
			ಎಣಿಕೆ_2++;
		}
	    }
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ಆದರೆ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ಖಾಲಿ)
	backwards = !backwards;
#ಆದರೆ_ಅಂತ್ಯ

    do_quotes = -1;
    ಆರಂಭ_ಒಳ_quotes = MAYBE;
    CLEAR_POS(&ಹೊಂದಿಕೆ_pos);

    // backward search: Check if this line contains a single-line comment
    ಆದರೆ ((backwards && comment_dir) || lisp || skip_comments)
	comment_ಲಂಬಸಾಲು = check_linecomment(linep);
    ಆದರೆ (lisp && comment_ಲಂಬಸಾಲು != MAXCOL && pos.ಲಂಬಸಾಲು > (colnr_T)comment_ಲಂಬಸಾಲು)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ಆದರೆ (skip_comments && !ಒಳ_ಖಂಡ_comment && comment_ಲಂಬಸಾಲು != MAXCOL
	    && backwards && pos.ಲಂಬಸಾಲು > (colnr_T)comment_ಲಂಬಸಾಲು)
	pos.ಲಂಬಸಾಲು = comment_ಲಂಬಸಾಲು;

    ಆಗ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ಆದರೆ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ಆದರೆ (lispcomm && pos.ಲಂಬಸಾಲು < (colnr_T)comment_ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (pos.ಲಂಬಸಾಲು == 0)		// at start of line, go to prev. one
	    {
		ಆದರೆ (pos.lnum == 1)	// start of file
		    ನಿಲ್ಲಿಸು;
		--pos.lnum;

		ಆದರೆ (maxtravel > 0 && ++traveled > maxtravel)
		    ನಿಲ್ಲಿಸು;

		linep = ml_get(pos.lnum);
		pos.ಲಂಬಸಾಲು = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		ಸಾಲು_breakcheck();

		// Check if this line contains a single-line comment
		ಆದರೆ (comment_dir || lisp || skip_comments)
		    comment_ಲಂಬಸಾಲು = check_linecomment(linep);
		// skip comment
		ಆದರೆ (lisp && comment_ಲಂಬಸಾಲು != MAXCOL)
		    pos.ಲಂಬಸಾಲು = comment_ಲಂಬಸಾಲು;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (skip_comments && !ಒಳ_ಖಂಡ_comment
			&& comment_ಲಂಬಸಾಲು != MAXCOL
			&& pos.ಲಂಬಸಾಲು > (colnr_T)comment_ಲಂಬಸಾಲು)
		    pos.ಲಂಬಸಾಲು = comment_ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		--pos.ಲಂಬಸಾಲು;
		ಆದರೆ (has_mbyte)
		    pos.ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(linep, linep + pos.ಲಂಬಸಾಲು);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ				// forward search
	{
	    ಆದರೆ (linep[pos.ಲಂಬಸಾಲು] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_ಲಂಬಸಾಲು != MAXCOL
					   && pos.ಲಂಬಸಾಲು == (colnr_T)comment_ಲಂಬಸಾಲು))
	    {
		ಆದರೆ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    ನಿಲ್ಲಿಸು;
		++pos.lnum;

		ಆದರೆ (maxtravel && traveled++ > maxtravel)
		    ನಿಲ್ಲಿಸು;

		linep = ml_get(pos.lnum);
		pos.ಲಂಬಸಾಲು = 0;
		do_quotes = -1;
		ಸಾಲು_breakcheck();
		ಆದರೆ (lisp || skip_comments)   // find comment pos in new line
		    comment_ಲಂಬಸಾಲು = check_linecomment(linep);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (has_mbyte)
		    pos.ಲಂಬಸಾಲು += (*mb_ptr2len)(linep + pos.ಲಂಬಸಾಲು);
		ಇಲ್ಲದಿದ್ದರೆ
		    ++pos.ಲಂಬಸಾಲು;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ಆದರೆ (skip_comments && !comment_dir && !inquote)
	{
	    ಆದರೆ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ಆದರೆ (!ಒಳ_ಖಂಡ_comment && pos.ಲಂಬಸಾಲು > 0
			&& linep[pos.ಲಂಬಸಾಲು - 1] == '*' && linep[pos.ಲಂಬಸಾಲು] == '/'
			&& (comment_ಲಂಬಸಾಲು == MAXCOL || (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು < comment_ಲಂಬಸಾಲು))
		    ಒಳ_ಖಂಡ_comment = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಖಂಡ_comment && pos.ಲಂಬಸಾಲು > 0
			&& linep[pos.ಲಂಬಸಾಲು - 1] == '/' && linep[pos.ಲಂಬಸಾಲು] == '*')
		    ಒಳ_ಖಂಡ_comment = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ಆದರೆ (!ಒಳ_ಖಂಡ_comment && linep[pos.ಲಂಬಸಾಲು] == '/'
			&& linep[pos.ಲಂಬಸಾಲು + 1] == '*'
			&& (comment_ಲಂಬಸಾಲು == MAXCOL || (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು < comment_ಲಂಬಸಾಲು))
		    ಒಳ_ಖಂಡ_comment = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಖಂಡ_comment && pos.ಲಂಬಸಾಲು > 0
			&& linep[pos.ಲಂಬಸಾಲು - 1] == '*' && linep[pos.ಲಂಬಸಾಲು] == '/')
		    ಒಳ_ಖಂಡ_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ಆದರೆ (pos.ಲಂಬಸಾಲು == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ಆದರೆ (linep[0] == findc && ಎಣಿಕೆ_2 == 0)	// match!
		ಹಿಂತಿರುಗಿ &pos;
	    ನಿಲ್ಲಿಸು;					// out of scope
	}

	ಆದರೆ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ಆದರೆ (comment_dir == FORWARD)
	    {
		ಆದರೆ (linep[pos.ಲಂಬಸಾಲು] == '*' && linep[pos.ಲಂಬಸಾಲು + 1] == '/')
		{
		    pos.ಲಂಬಸಾಲು++;
		    ಹಿಂತಿರುಗಿ &pos;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ಆದರೆ (pos.ಲಂಬಸಾಲು == 0)
		    ಮುಂದುವರಿ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (raw_string)
		{
		    ಆದರೆ (linep[pos.ಲಂಬಸಾಲು - 1] == 'R'
			&& linep[pos.ಲಂಬಸಾಲು] == '"'
			&& vim_strchr(linep + pos.ಲಂಬಸಾಲು + 1, '(') != ಖಾಲಿ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ಆದರೆ (!ಕಂಡುಹಿಡಿ_rawstring_ಕೊನೆ(linep, &pos,
				  ಎಣಿಕೆ_2 > 0 ? &ಹೊಂದಿಕೆ_pos : &curwin->w_cursor))
			{
			    ಎಣಿಕೆ_2++;
			    ಹೊಂದಿಕೆ_pos = pos;
			    ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (  linep[pos.ಲಂಬಸಾಲು - 1] == '/'
			&& linep[pos.ಲಂಬಸಾಲು] == '*'
			&& (pos.ಲಂಬಸಾಲು == 1 || linep[pos.ಲಂಬಸಾಲು - 2] != '*')
			&& (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು < comment_ಲಂಬಸಾಲು)
		{
		    ಎಣಿಕೆ_2++;
		    ಹೊಂದಿಕೆ_pos = pos;
		    ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು--;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು - 1] == '*' && linep[pos.ಲಂಬಸಾಲು] == '/')
		{
		    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
			pos = ಹೊಂದಿಕೆ_pos;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.ಲಂಬಸಾಲು > 1 && linep[pos.ಲಂಬಸಾಲು - 2] == '/'
					       && (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು <= comment_ಲಂಬಸಾಲು)
			pos.ಲಂಬಸಾಲು -= 2;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ignore_cend)
			ಮುಂದುವರಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ &pos;
		}
	    }
	    ಮುಂದುವರಿ;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ಆದರೆ (cpo_ಹೊಂದಿಕೆ)
	    do_quotes = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_ಆರಂಭ = do_quotes;
	    ಗಾಗಿ (ptr = linep; *ptr; ++ptr)
	    {
		ಆದರೆ (ptr == linep + pos.ಲಂಬಸಾಲು + backwards)
		    at_ಆರಂಭ = (do_quotes & 1);
		ಆದರೆ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ಆದರೆ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ಆದರೆ (!do_quotes)
	    {
		inquote = FALSE;
		ಆದರೆ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ಆದರೆ (ಆರಂಭ_ಒಳ_quotes == MAYBE)
		    {
			inquote = at_ಆರಂಭ;
			ಆದರೆ (inquote)
			    ಆರಂಭ_ಒಳ_quotes = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (backwards)
			inquote = TRUE;
		}
		ಆದರೆ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ಆದರೆ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ಆದರೆ (ಆರಂಭ_ಒಳ_quotes == MAYBE)
			{
			    inquote = at_ಆರಂಭ;
			    ಆದರೆ (inquote)
				ಆರಂಭ_ಒಳ_quotes = TRUE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ಆದರೆ (ಆರಂಭ_ಒಳ_quotes == MAYBE)
	    ಆರಂಭ_ಒಳ_quotes = FALSE;

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
	c = PTR2CHAR(linep + pos.ಲಂಬಸಾಲು);
	ಆಯ್ಕೆ (c)
	{
	ಸ್ಥಿತಿ NUL:
	    // at end of line without trailing backslash, reset inquote
	    ಆದರೆ (pos.ಲಂಬಸಾಲು == 0 || linep[pos.ಲಂಬಸಾಲು - 1] != '\\')
	    {
		inquote = FALSE;
		ಆರಂಭ_ಒಳ_quotes = FALSE;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ಆದರೆ (do_quotes)
	    {
		ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

		ಗಾಗಿ (ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು - 1; ಲಂಬಸಾಲು >= 0; --ಲಂಬಸಾಲು)
		    ಆದರೆ (linep[ಲಂಬಸಾಲು] != '\\')
			ನಿಲ್ಲಿಸು;
		ಆದರೆ ((((ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು - 1 - ಲಂಬಸಾಲು) & 1) == 0)
		{
		    inquote = !inquote;
		    ಆರಂಭ_ಒಳ_quotes = FALSE;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	ಸ್ಥಿತಿ '\'':
	    ಆದರೆ (!cpo_ಹೊಂದಿಕೆ && initc != '\'' && findc != '\'')
	    {
		ಆದರೆ (backwards)
		{
		    ಆದರೆ (pos.ಲಂಬಸಾಲು > 1)
		    {
			ಆದರೆ (linep[pos.ಲಂಬಸಾಲು - 2] == '\'')
			{
			    pos.ಲಂಬಸಾಲು -= 2;
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು - 2] == '\\'
				  && pos.ಲಂಬಸಾಲು > 2 && linep[pos.ಲಂಬಸಾಲು - 3] == '\'')
			{
			    pos.ಲಂಬಸಾಲು -= 3;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು + 1])	// forward search
		{
		    ಆದರೆ (linep[pos.ಲಂಬಸಾಲು + 1] == '\\'
			   && linep[pos.ಲಂಬಸಾಲು + 2] && linep[pos.ಲಂಬಸಾಲು + 3] == '\'')
		    {
			pos.ಲಂಬಸಾಲು += 3;
			ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linep[pos.ಲಂಬಸಾಲು + 2] == '\'')
		    {
			pos.ಲಂಬಸಾಲು += 2;
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	    // FALLTHROUGH

	ಪೂರ್ವನಿಯೋಜಿತ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ಆದರೆ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ಖಾಲಿ
		    && pos.ಲಂಬಸಾಲು > 1
		    && check_prevcol(linep, pos.ಲಂಬಸಾಲು, '\\', ಖಾಲಿ)
		    && check_prevcol(linep, pos.ಲಂಬಸಾಲು - 1, '#', ಖಾಲಿ))
		ನಿಲ್ಲಿಸು;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ಆದರೆ (skip_comments && (ಒಳ_ಖಂಡ_comment
		    || (comment_ಲಂಬಸಾಲು != MAXCOL && (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು >= comment_ಲಂಬಸಾಲು)))
		ನಿಲ್ಲಿಸು;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ಆದರೆ ((!inquote || ಆರಂಭ_ಒಳ_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು, bslcnt = 0;

		ಆದರೆ (!cpo_bsl)
		{
		    ಗಾಗಿ (ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು; check_prevcol(linep, ಲಂಬಸಾಲು, '\\', &ಲಂಬಸಾಲು);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ಆದರೆ (cpo_bsl || (bslcnt & 1) == ಹೊಂದಿಕೆ_escaped)
		{
		    ಆದರೆ (c == initc)
			ಎಣಿಕೆ_2++;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (ಎಣಿಕೆ_2 == 0)
			    ಹಿಂತಿರುಗಿ &pos;
			ಎಣಿಕೆ_2--;
		    }
		}
	    }
	}
    }

    ಆದರೆ (comment_dir == BACKWARD && ಎಣಿಕೆ_2 > 0)
    {
	pos = ಹೊಂದಿಕೆ_pos;
	ಹಿಂತಿರುಗಿ &pos;
    }
    ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    ಶೂನ್ಯ
showmatch(
    ಪೂರ್ಣಾಂಕ		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    ದೀರ್ಘ	save_so;
    ದೀರ್ಘ	save_siso;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ಪೂರ್ಣಾಂಕ		save_state;
#ಆದರೆ_ಅಂತ್ಯ
    colnr_T	save_dollar_vcol;
    char_u	*p;
    ದೀರ್ಘ	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ದೀರ್ಘ	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    ಗಾಗಿ (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	p += mb_ptr2len(p) + 1;
	ಆದರೆ (PTR2CHAR(p) == c
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    ನಿಲ್ಲಿಸು;
	p += mb_ptr2len(p);
	ಆದರೆ (*p == NUL)
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((lpos = findmatch(ಖಾಲಿ, NUL)) == ಖಾಲಿ)	    // no match, so beep
    {
	vim_beep(BO_ಹೊಂದಿಕೆ);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ಖಾಲಿ, &vcol, ಖಾಲಿ, 0);

    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_ಅಗಲ));
    ಆದರೆ (!ಲಂಬಸಾಲು_visible)
	ಹಿಂತಿರುಗಿ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ಆದರೆ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
	dollar_vcol = -1;
    ++curwin->w_virtcol;	// do display ')' just before "$"
    update_screen(UPD_VALID);	// show the new char first

    save_dollar_vcol = dollar_vcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    save_state = State;
    State = ಬಹುಲಕ_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    ಆದರೆ (vim_strchr(p_cpo, CPO_SHOWMATCH) != ಖಾಲಿ)
	ui_delay(p_mat * 100L + 8, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_zero_ಅಗಲ(
    char_u	*ನಮೂನೆ,
    size_t	patternlen,
    ಪೂರ್ಣಾಂಕ		move,
    pos_T	*cur,
    ಪೂರ್ಣಾಂಕ		direction)
{
    regmmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		nmatched = 0;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = -1;
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ		flag = 0;

    ಆದರೆ (ನಮೂನೆ == ಖಾಲಿ)
    {
	ನಮೂನೆ = spats[ಕೊನೆಯ_idx].pat;
	patternlen = spats[ಕೊನೆಯ_idx].patlen;
    }

    ಆದರೆ (ಹುಡುಕು_regcomp(ನಮೂನೆ, patternlen, ಖಾಲಿ, RE_ಹುಡುಕು, RE_ಹುಡುಕು,
					      ಹುಡುಕು_KEEP, &regmatch) == FAIL)
	ಹಿಂತಿರುಗಿ -1;

    // init startcol correctly
    regmatch.startpos[0].ಲಂಬಸಾಲು = -1;
    // move to match
    ಆದರೆ (move)
    {
	CLEAR_POS(&pos);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = ಹುಡುಕು_ಆರಂಭ;
    }

    ಆದರೆ (searchit(curwin, curbuf, &pos, ಖಾಲಿ, direction, ನಮೂನೆ, patternlen, 1,
				  ಹುಡುಕು_KEEP + flag, RE_ಹುಡುಕು, ಖಾಲಿ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	ಮಾಡು
	{
	    regmatch.startpos[0].ಲಂಬಸಾಲು++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].ಲಂಬಸಾಲು, ಖಾಲಿ);
	    ಆದರೆ (nmatched != 0)
		ನಿಲ್ಲಿಸು;
	} ಆಗ (regmatch.regprog != ಖಾಲಿ
		&& (direction == FORWARD ? regmatch.startpos[0].ಲಂಬಸಾಲು < pos.ಲಂಬಸಾಲು
				      : regmatch.startpos[0].ಲಂಬಸಾಲು > pos.ಲಂಬಸಾಲು));

	ಆದರೆ (called_emsg == called_emsg_before)
	{
	    ಫಲಿತಾಂಶ = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].ಲಂಬಸಾಲು == regmatch.endpos[0].ಲಂಬಸಾಲು);
	}
    }

    vim_regfree(regmatch.regprog);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ಪೂರ್ಣಾಂಕ
current_ಹುಡುಕು(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	ಆರಂಭ_pos;	// start position of the pattern match
    pos_T	ಕೊನೆ_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;		// result of various function calls
    char_u	old_p_ws = p_ws;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    pos_T	save_VIsual = VIsual;
    ಪೂರ್ಣಾಂಕ		zero_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		skip_ಮೊದಲ_backward;

    // Correct cursor when 'selection' is exclusive
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_ಮೊದಲ_backward = forward && VIsual_ಸಕ್ರಿಯ
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (forward)
	    incl(&pos);
	ಇಲ್ಲದಿದ್ದರೆ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_ಅಗಲ = is_zero_ಅಗಲ(spats[ಕೊನೆಯ_idx].pat, spats[ಕೊನೆಯ_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ಆದರೆ (zero_ಅಗಲ == -1)
	ಹಿಂತಿರುಗಿ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    ಗಾಗಿ (i = 0; i < 2; i++)
    {
	ಆದರೆ (forward)
	{
	    ಆದರೆ (i == 0 && skip_ಮೊದಲ_backward)
		ಮುಂದುವರಿ;
	    dir = i;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    dir = !i;

	flags = 0;
	ಆದರೆ (!dir && !zero_ಅಗಲ)
	    flags = ಹುಡುಕು_ಕೊನೆ;
	ಕೊನೆ_pos = pos;

	// wrapping should not occur in the first round
	ಆದರೆ (i == 0)
	    p_ws = FALSE;

	ಫಲಿತಾಂಶ = searchit(curwin, curbuf, &pos, &ಕೊನೆ_pos,
		(dir ? FORWARD : BACKWARD),
		spats[ಕೊನೆಯ_idx].pat, spats[ಕೊನೆಯ_idx].patlen, (ದೀರ್ಘ) (i ? ಎಣಿಕೆ_2 : 1),
		ಹುಡುಕು_KEEP | flags, RE_ಹುಡುಕು, ಖಾಲಿ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	ಆದರೆ (i == 1 && !ಫಲಿತಾಂಶ) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		VIsual = save_VIsual;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 0 && !ಫಲಿತಾಂಶ)
	{
	    ಆದರೆ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		pos.ಲಂಬಸಾಲು  = ml_get_len(curwin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    }
	}
    }

    ಆರಂಭ_pos = pos;

    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ)
	VIsual = ಆರಂಭ_pos;

    // put the cursor after the match
    curwin->w_cursor = ಕೊನೆ_pos;
    ಆದರೆ (LT_POS(VIsual, ಕೊನೆ_pos) && forward)
    {
	ಆದರೆ (skip_ಮೊದಲ_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ಇಲ್ಲದಿದ್ದರೆ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_ಸಕ್ರಿಯ = TRUE;
    VIsual_ಬಹುಲಕ = 'v';

    ಆದರೆ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ಆದರೆ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (fdo_flags & FDO_ಹುಡುಕು && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    may_ಆರಂಭ_select('c');
    setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_ನಕ್ಷತ್ರ.vmode = NUL;
#ಆದರೆ_ಅಂತ್ಯ
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    ಪೂರ್ಣಾಂಕ
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ಹಿಂತಿರುಗಿ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_ಹುಡುಕು_stat(
    ಪೂರ್ಣಾಂಕ		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    ಪೂರ್ಣಾಂಕ		show_ತುದಿ_bot_ಸಂದೇಶ,
    char_u	*msgbuf,
    size_t	msgbuflen,
    ಪೂರ್ಣಾಂಕ		recompute,
    ಪೂರ್ಣಾಂಕ		maxcount,
    ದೀರ್ಘ	timeout)
{
    searchstat_T stat;

    update_ಹುಡುಕು_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ಆದರೆ (stat.cur <= 0)
	ಹಿಂತಿರುಗಿ;

    ಅಕ್ಷರ	t[ಹುಡುಕು_STAT_BUF_LEN];
    size_t	len;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ಆದರೆ (stat.incomplete == 1)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[?/??]");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ಇಲ್ಲದಿದ್ದರೆ
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (stat.incomplete == 1)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[?/??]");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stat.cnt > maxcount)
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ಇಲ್ಲದಿದ್ದರೆ
	    len = vim_snprintf(t, ಹುಡುಕು_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ಆದರೆ (show_ತುದಿ_bot_ಸಂದೇಶ && len + 2 < ಹುಡುಕು_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ಆದರೆ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ಆದರೆ (dirc == '?' && stat.cur == maxcount + 1)
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
    ಪೂರ್ಣಾಂಕ			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    ಪೂರ್ಣಾಂಕ			recompute,
    ಪೂರ್ಣಾಂಕ			maxcount,
    ದೀರ್ಘ		timeout ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		    save_ws = p_ws;
    ಪೂರ್ಣಾಂಕ		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ಸ್ಥಿರ pos_T    lastpos = {0, 0, 0};
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cur = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cnt = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    exact_ಹೊಂದಿಕೆ = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    incomplete = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಕೊನೆಯ_maxcount = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    chgtick = 0;
    ಸ್ಥಿರ char_u   *lastpat = ಖಾಲಿ;
    ಸ್ಥಿರ size_t   lastpatlen = 0;
    ಸ್ಥಿರ buf_T    *lbuf = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    proftime_T  ಆರಂಭ;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಸೂಚಕ(stat);

    ಆದರೆ (dirc == 0 && !recompute && !ಖಾಲಿಗಳು_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_ಹೊಂದಿಕೆ = exact_ಹೊಂದಿಕೆ;
	stat->incomplete = incomplete;
	stat->ಕೊನೆಯ_maxcount = ಕೊನೆಯ_maxcount;
	ಹಿಂತಿರುಗಿ;
    }
    ಕೊನೆಯ_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ಆದರೆ (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ಖಾಲಿ
	    && STRNCMP(lastpat, spats[ಕೊನೆಯ_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[ಕೊನೆಯ_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_ಹೊಂದಿಕೆ = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    ಆದರೆ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	ಮುಗಿದ_ಹುಡುಕು = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	ಆದರೆ (timeout > 0)
	    profile_setlimit(timeout, &ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ಖಾಲಿ, 0, 1, ಹುಡುಕು_KEEP, RE_ಕೊನೆಯ, ಖಾಲಿ) != FAIL)
	{
	    ಮುಗಿದ_ಹುಡುಕು = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	    // Stop after passing the time limit.
	    ಆದರೆ (timeout > 0 && profile_passed_ಎಲ್ಲೆ(&ಆರಂಭ))
	    {
		incomplete = 1;
		ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    cnt++;
	    ಆದರೆ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ಆದರೆ (LT_POS(p, endpos))
		    exact_ಹೊಂದಿಕೆ = TRUE;
	    }
	    fast_breakcheck();
	    ಆದರೆ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (got_int)
	    cur = -1; // abort
	ಆದರೆ (ಮುಗಿದ_ಹುಡುಕು)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[ಕೊನೆಯ_idx].pat, spats[ಕೊನೆಯ_idx].patlen);
	    ಆದರೆ (lastpat == ಖಾಲಿ)
		lastpatlen = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		lastpatlen = spats[ಕೊನೆಯ_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_ಹೊಂದಿಕೆ = exact_ಹೊಂದಿಕೆ;
    stat->incomplete = incomplete;
    stat->ಕೊನೆಯ_maxcount = ಕೊನೆಯ_maxcount;
    p_ws = save_ws;
}

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID)

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
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ನಮೂನೆ_ಒಳ_ಮಾರ್ಗ(
    char_u	*ptr,		// pointer to search pattern
    ಪೂರ್ಣಾಂಕ		dir ಬಳಸದ,	// direction of expansion
    ಪೂರ್ಣಾಂಕ		len,		// length of search pattern
    ಪೂರ್ಣಾಂಕ		ಸಂಪೂರ್ಣ,		// match whole words only
    ಪೂರ್ಣಾಂಕ		skip_comments,	// don't match inside comments
    ಪೂರ್ಣಾಂಕ		type,		// Type of search; are we looking for a type?
				// a macro?
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		action,		// What to do when we find it
    linenr_T	ಆರಂಭ_lnum,	// first line to start searching
    linenr_T	ಕೊನೆ_lnum,	// last line for searching
    ಪೂರ್ಣಾಂಕ		forceit,	// If true, always switch to the found path
    ಪೂರ್ಣಾಂಕ		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    ಪೂರ್ಣಾಂಕ		max_ಮಾರ್ಗ_ಆಳ = 50;
    ದೀರ್ಘ	ಹೊಂದಿಕೆ_ಎಣಿಕೆ = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ಖಾಲಿ;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    ಪೂರ್ಣಾಂಕ		ಆಳ_displayed;	// For type==CHECK_PATH
    ಪೂರ್ಣಾಂಕ		old_files;
    ಪೂರ್ಣಾಂಕ		already_searched;
    char_u	*file_ಸಾಲು;
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	save_char;
    ಪೂರ್ಣಾಂಕ		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    ಪೂರ್ಣಾಂಕ		matched = FALSE;
    ಪೂರ್ಣಾಂಕ		did_show = FALSE;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*already = ಖಾಲಿ;
    char_u	*startp = ಖಾಲಿ;
    char_u	*inc_opt = ಖಾಲಿ;
# ಆದರೆ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ಖಾಲಿ;
# endif

    regmatch.regprog = ಖಾಲಿ;
    incl_regmatch.regprog = ಖಾಲಿ;
    def_regmatch.regprog = ಖಾಲಿ;

    file_ಸಾಲು = alloc(LSIZE);
    ಆದರೆ (file_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (type != CHECK_ಮಾರ್ಗ && type != ಕಂಡುಹಿಡಿ_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_ಸ್ಥಿತಿ_sol())
    {
	pat = alloc(len + 5);
	ಆದರೆ (pat == ಖಾಲಿ)
	    ಹೋಗು fpip_ಕೊನೆ;
	vim_snprintf((ಅಕ್ಷರ *)pat, len + 5, ಸಂಪೂರ್ಣ ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು fpip_ಕೊನೆ;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ಆದರೆ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (incl_regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು fpip_ಕೊನೆ;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ಆದರೆ (type == ಕಂಡುಹಿಡಿ_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (def_regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು fpip_ಕೊನೆ;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_ಮಾರ್ಗ_ಆಳ * ಗಾತ್ರ(SearchedFile), TRUE);
    ಆದರೆ (files == ಖಾಲಿ)
	ಹೋಗು fpip_ಕೊನೆ;
    old_files = max_ಮಾರ್ಗ_ಆಳ;
    ಆಳ = ಆಳ_displayed = -1;

    lnum = ಆರಂಭ_lnum;
    ಆದರೆ (ಕೊನೆ_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆ_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (lnum > ಕೊನೆ_lnum)		// do at least one line
	lnum = ಕೊನೆ_lnum;
    ಸಾಲು = get_ಸಾಲು_and_copy(lnum, file_ಸಾಲು);

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (incl_regmatch.regprog != ಖಾಲಿ
		&& vim_regexec(&incl_regmatch, ಸಾಲು, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)inc_opt, "\\zs") != ಖಾಲಿ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = ಕಂಡುಹಿಡಿ_file_name_ಒಳ_ಮಾರ್ಗ(incl_regmatch.startp[0],
		       (ಪೂರ್ಣಾಂಕ)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ಇಲ್ಲದಿದ್ದರೆ
		// Use text after match with 'include'.
		new_fname = file_name_ಒಳ_ಸಾಲು(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ಖಾಲಿ);
	    already_searched = FALSE;
	    ಆದರೆ (new_fname != ಖಾಲಿ)
	    {
		// Check whether we have already searched in this file
		ಗಾಗಿ (i = 0;; i++)
		{
		    ಆದರೆ (i == ಆಳ + 1)
			i = old_files;
		    ಆದರೆ (i == max_ಮಾರ್ಗ_ಆಳ)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ಆದರೆ (type != CHECK_ಮಾರ್ಗ
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
			    ಆದರೆ (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //ಸಂದೇಶ
			    {
				ಸಂದೇಶ_home_replace_hl(new_fname);
				ಸಂದೇಶ_puts(_(" (includes previously listed match)"));
				prev_fname = ಖಾಲಿ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }

	    ಆದರೆ (type == CHECK_ಮಾರ್ಗ && (action == ACTION_SHOW_ALL
				 || (new_fname == ಖಾಲಿ && !already_searched)))
	    {
		ಆದರೆ (did_show)
		    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    ಸಂದೇಶ_puts_title(_("--- Included files "));
		    ಆದರೆ (action != ACTION_SHOW_ALL)
			ಸಂದೇಶ_puts_title(_("not found "));
		    ಸಂದೇಶ_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ಆಗ (ಆಳ_displayed < ಆಳ && !got_int)
		{
		    ++ಆಳ_displayed;
		    ಗಾಗಿ (i = 0; i < ಆಳ_displayed; i++)
			ಸಂದೇಶ_puts("  ");
		    ಸಂದೇಶ_home_replace(files[ಆಳ_displayed].name);
		    ಸಂದೇಶ_puts(" -->\n");
		}
		ಆದರೆ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    ಗಾಗಿ (i = 0; i <= ಆಳ_displayed; i++)
			ಸಂದೇಶ_puts("  ");
		    ಆದರೆ (new_fname != ಖಾಲಿ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			ಸಂದೇಶ_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)inc_opt, "\\zs") != ಖಾಲಿ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (ಪೂರ್ಣಾಂಕ)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // find the file name after the end of the match
			    ಗಾಗಿ (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    ಗಾಗಿ (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ಆದರೆ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p > ಸಾಲು)
			{
			    ಆದರೆ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ಆದರೆ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			ಸಂದೇಶ_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ಆದರೆ (new_fname == ಖಾಲಿ && action == ACTION_SHOW_ALL)
		    {
			ಆದರೆ (already_searched)
			    ಸಂದೇಶ_puts(_("  (Already listed)"));
			ಇಲ್ಲದಿದ್ದರೆ
			    ಸಂದೇಶ_puts(_("  NOT FOUND"));
		    }
		}
		ಹೊರಹೋಗು_flush();	    // output each line directly
	    }

	    ಆದರೆ (new_fname != ಖಾಲಿ)
	    {
		// Push the new file onto the file stack
		ಆದರೆ (ಆಳ + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_ಮಾರ್ಗ_ಆಳ * 2);
		    ಆದರೆ (bigger != ಖಾಲಿ)
		    {
			ಗಾಗಿ (i = 0; i <= ಆಳ; i++)
			    bigger[i] = files[i];
			ಗಾಗಿ (i = ಆಳ + 1; i < old_files + max_ಮಾರ್ಗ_ಆಳ; i++)
			{
			    bigger[i].fp = ಖಾಲಿ;
			    bigger[i].name = ಖಾಲಿ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			ಗಾಗಿ (i = old_files; i < max_ಮಾರ್ಗ_ಆಳ; i++)
			    bigger[i + max_ಮಾರ್ಗ_ಆಳ] = files[i];
			old_files += max_ಮಾರ್ಗ_ಆಳ;
			max_ಮಾರ್ಗ_ಆಳ *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ಆದರೆ ((files[ಆಳ + 1].fp = mch_fopen((ಅಕ್ಷರ *)new_fname, "r"))
								    == ಖಾಲಿ)
		    vim_free(new_fname);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (++ಆಳ == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[ಆಳ].name = curr_fname = new_fname;
		    files[ಆಳ].lnum = 0;
		    files[ಆಳ].matched = FALSE;
		    ಆದರೆ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((ಅಕ್ಷರ*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(ಅಕ್ಷರ *)new_fname);
			ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (ಅಕ್ಷರ *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = ಸಾಲು;
ಹುಡುಕು_ಸಾಲು:
	    define_matched = FALSE;
	    ಆದರೆ (def_regmatch.regprog != ಖಾಲಿ
			      && vim_regexec(&def_regmatch, ಸಾಲು, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ಆಗ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ಆದರೆ (def_regmatch.regprog == ಖಾಲಿ || define_matched)
	    {
		ಆದರೆ (define_matched || compl_ಸ್ಥಿತಿ_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ಆದರೆ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ಇಲ್ಲದಿದ್ದರೆ
			matched = STRNCMP(startp, ptr, len) == 0;
		    ಆದರೆ (matched && define_matched && ಸಂಪೂರ್ಣ
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regmatch.regprog != ಖಾಲಿ
			 && vim_regexec(&regmatch, ಸಾಲು, (colnr_T)(p - ಸಾಲು)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ಆದರೆ (!define_matched && skip_comments)
		    {
			ಆದರೆ ((*ಸಾಲು != '#' ||
				STRNCMP(skipwhite(ಸಾಲು + 1), "define", 6) != 0)
				&& get_leader_len(ಸಾಲು, ಖಾಲಿ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(ಸಾಲು);
			ಆದರೆ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    ಗಾಗಿ (p = ಸಾಲು; *p && p < startp; ++p)
			    {
				ಆದರೆ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ಆದರೆ (p[1] == '/')
					ನಿಲ್ಲಿಸು;
				    ++p;
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!matched && p[0] == '*' && p[1] == '/')
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
	ಆದರೆ (matched)
	{
	    ಆದರೆ (action == ACTION_EXPAND)
	    {
		ಪೂರ್ಣಾಂಕ	cont_s_ipos = FALSE;
		ಪೂರ್ಣಾಂಕ	ಕೂಡಿಸು_r;
		char_u	*aux;

		ಆದರೆ (ಆಳ == -1 && lnum == curwin->w_cursor.lnum)
		    ನಿಲ್ಲಿಸು;
		ಸಿಕ್ಕಿತು = TRUE;
		aux = p = startp;
		ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && (ಪೂರ್ಣಾಂಕ)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ಆದರೆ (vim_iswordp(p))
			ಹೋಗು exit_matched;
		    p = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(p);
		}
		p = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(p);
		i = (ಪೂರ್ಣಾಂಕ)(p - aux);

		ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ಆದರೆ (ಆಳ < 0)
		    {
			ಆದರೆ (lnum >= ಕೊನೆ_lnum)
			    ಹೋಗು exit_matched;
			ಸಾಲು = get_ಸಾಲು_and_copy(++lnum, file_ಸಾಲು);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_fgets(ಸಾಲು = file_ಸಾಲು,
						      LSIZE, files[ಆಳ].fp))
			ಹೋಗು exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(ಸಾಲು);
		    p = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(p);
		    p = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(p);
		    ಆದರೆ (p > aux)
		    {
			ಆದರೆ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ಆದರೆ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ಆದರೆ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ಖಾಲಿ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ಆದರೆ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (ಪೂರ್ಣಾಂಕ)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ಆದರೆ (i == ins_compl_len())
			ಹೋಗು exit_matched;
		}

		ಕೂಡಿಸು_r = ins_compl_ಕೂಡಿಸು_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ಖಾಲಿ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ಆದರೆ (ಕೂಡಿಸು_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೂಡಿಸು_r == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ACTION_SHOW_ALL)
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ಆದರೆ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ಆದರೆ (curr_fname != prev_fname)
		{
		    ಆದರೆ (did_show)
			ಸಂದೇಶ_putchar('\n');	// cursor below last one
		    ಆದರೆ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			ಸಂದೇಶ_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ಆದರೆ (!got_int)
		    show_pat_ಒಳ_ಮಾರ್ಗ(ಸಾಲು, type, TRUE, action,
			    (ಆಳ == -1) ? ಖಾಲಿ : files[ಆಳ].fp,
			    (ಆಳ == -1) ? &lnum : &files[ಆಳ].lnum,
			    ಹೊಂದಿಕೆ_ಎಣಿಕೆ++);

		// Set matched flag for this file and all the ones that
		// include it
		ಗಾಗಿ (i = 0; i <= ಆಳ; ++i)
		    files[i].matched = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (--ಎಣಿಕೆ_2 <= 0)
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ಆದರೆ (ಆಳ == -1 && lnum == curwin->w_cursor.lnum
# ಆದರೆ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_ಹೊಂದಿಕೆ_is_on_current_ಸಾಲು));
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ACTION_SHOW)
		{
		    show_pat_ಒಳ_ಮಾರ್ಗ(ಸಾಲು, type, did_show, action,
			    (ಆಳ == -1) ? ಖಾಲಿ : files[ಆಳ].fp,
			    (ಆಳ == -1) ? &lnum : &files[ಆಳ].lnum, 1L);
		    did_show = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ಆದರೆ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ಆದರೆ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ಆದರೆ (action == ACTION_ವಿಭಜಿಸು)
		    {
			ಆದರೆ (win_ವಿಭಜಿಸು(0, 0) == FAIL)
			    ನಿಲ್ಲಿಸು;
			RESET_BINDING(curwin);
		    }
		    ಆದರೆ (ಆಳ == -1)
		    {
			// match in current file
# ಆದರೆ defined(FEAT_QUICKFIX)
			ಆದರೆ (g_do_tagpreview != 0)
			{
			    ಆದರೆ (!win_valid(curwin_save))
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_ಬಫರ್->b_fnum, ಖಾಲಿ,
						     ಖಾಲಿ, TRUE, lnum, forceit)))
				ನಿಲ್ಲಿಸು;	// failed to jump to file
			}
			ಇಲ್ಲದಿದ್ದರೆ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (!GETFILE_SUCCESS(getfile(
					0, files[ಆಳ].name, ಖಾಲಿ, TRUE,
						    files[ಆಳ].lnum, forceit)))
			    ನಿಲ್ಲಿಸು;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[ಆಳ].lnum;
		    }
		}
		ಆದರೆ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(startp - ಸಾಲು);
		    curwin->w_ಗಣ_curswant = true;
		}

# ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		ನಿಲ್ಲಿಸು;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ಆದರೆ (def_regmatch.regprog == ಖಾಲಿ
		    && action == ACTION_EXPAND
		    && !compl_ಸ್ಥಿತಿ_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		ಹೋಗು ಹುಡುಕು_ಸಾಲು;
	}
	ಸಾಲು_breakcheck();
	ಆದರೆ (action == ACTION_EXPAND)
	    ins_compl_check_ಕೀಲಿಗಳು(30, FALSE);
	ಆದರೆ (got_int || ins_compl_interrupted())
	    ನಿಲ್ಲಿಸು;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ಆಗ (ಆಳ >= 0 && !already
		&& vim_fgets(ಸಾಲು = file_ಸಾಲು, LSIZE, files[ಆಳ].fp))
	{
	    fclose(files[ಆಳ].fp);
	    --old_files;
	    files[old_files].name = files[ಆಳ].name;
	    files[old_files].matched = files[ಆಳ].matched;
	    --ಆಳ;
	    curr_fname = (ಆಳ == -1) ? curbuf->b_fname
				       : files[ಆಳ].name;
	    ಆದರೆ (ಆಳ < ಆಳ_displayed)
		ಆಳ_displayed = ಆಳ;
	}
	ಆದರೆ (ಆಳ >= 0)		// we could read the line
	{
	    files[ಆಳ].lnum++;
	    // Remove any CR and LF from the line.
	    i = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು);
	    ಆದರೆ (i > 0 && ಸಾಲು[i - 1] == '\n')
		ಸಾಲು[--i] = NUL;
	    ಆದರೆ (i > 0 && ಸಾಲು[i - 1] == '\r')
		ಸಾಲು[--i] = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!already)
	{
	    ಆದರೆ (++lnum > ಕೊನೆ_lnum)
		ನಿಲ್ಲಿಸು;
	    ಸಾಲು = get_ಸಾಲು_and_copy(lnum, file_ಸಾಲು);
	}
	already = ಖಾಲಿ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    ಗಾಗಿ (i = 0; i <= ಆಳ; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    ಗಾಗಿ (i = old_files; i < max_ಮಾರ್ಗ_ಆಳ; i++)
	vim_free(files[i].name);
    vim_free(files);

    ಆದರೆ (type == CHECK_ಮಾರ್ಗ)
    {
	ಆದರೆ (!did_show)
	{
	    ಆದರೆ (action != ACTION_SHOW_ALL)
		ಸಂದೇಶ(_("All included files were found"));
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ(_("No included files"));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಸಿಕ್ಕಿತು && action != ACTION_EXPAND && !silent)
    {
	ಆದರೆ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಕಂಡುಹಿಡಿ_DEFINE)
	    emsg(_(e_couldnt_ಕಂಡುಹಿಡಿ_definition));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_couldnt_ಕಂಡುಹಿಡಿ_ನಮೂನೆ));
    }
    ಆದರೆ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	ಸಂದೇಶ_ಕೊನೆ();

fpip_ಕೊನೆ:
    vim_free(file_ಸಾಲು);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_pat_ಒಳ_ಮಾರ್ಗ(
    char_u  *ಸಾಲು,
    ಪೂರ್ಣಾಂಕ	    type,
    ಪೂರ್ಣಾಂಕ	    did_show,
    ಪೂರ್ಣಾಂಕ	    action,
    FILE    *fp,
    linenr_T *lnum,
    ದೀರ್ಘ    ಎಣಿಕೆ_2)
{
    char_u  *p;
    size_t  linelen;

    ಆದರೆ (did_show)
	ಸಂದೇಶ_putchar('\n');	// cursor below last one
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಸಂದೇಶ_silent)
	gotocmdline(TRUE);	// cursor at status line
    ಆದರೆ (got_int)		// 'q' typed at "--more--" message
	ಹಿಂತಿರುಗಿ;
    linelen = STRLEN(ಸಾಲು);
    ಗಾಗಿ (;;)
    {
	p = ಸಾಲು + linelen - 1;
	ಆದರೆ (fp != ಖಾಲಿ)
	{
	    // We used fgets(), so get rid of newline at end
	    ಆದರೆ (p >= ಸಾಲು && *p == '\n')
		--p;
	    ಆದರೆ (p >= ಸಾಲು && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ಆದರೆ (action == ACTION_SHOW_ALL)
	{
	    sprintf((ಅಕ್ಷರ *)IObuff, "%3ld: ", ಎಣಿಕೆ_2);	// show match nr
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	    sprintf((ಅಕ್ಷರ *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)IObuff, HL_ATTR(HLF_N));
	    ಸಂದೇಶ_puts(" ");
	}
	ಸಂದೇಶ_prt_ಸಾಲು(ಸಾಲು, FALSE);
	ಹೊರಹೋಗು_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ಆದರೆ (got_int || type != ಕಂಡುಹಿಡಿ_DEFINE || p < ಸಾಲು || *p != '\\')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (fp != ಖಾಲಿ)
	{
	    ಆದರೆ (vim_fgets(ಸಾಲು, LSIZE, fp)) // end of file
		ನಿಲ್ಲಿಸು;
	    linelen = STRLEN(ಸಾಲು);
	    ++*lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (++*lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    ಸಾಲು = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	ಸಂದೇಶ_putchar('\n');
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    ಪೂರ್ಣಾಂಕ
get_spat_ಕೊನೆಯ_idx(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಕೊನೆಯ_idx;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    ಶೂನ್ಯ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*ನಮೂನೆ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			maxcount = p_msc;
    ದೀರ್ಘ		timeout = ಹುಡುಕು_STAT_DEF_TIMEOUT;
    ಪೂರ್ಣಾಂಕ			recompute = TRUE;
    searchstat_T	stat;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	ಪೂರ್ಣಾಂಕ		error = FALSE;

	ಆದರೆ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	dict = argvars[0].vval.v_dict;
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"timeout", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    timeout = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	}
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"maxcount", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    maxcount = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"pattern", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ನಮೂನೆ = tv_get_string_chk(&di->di_tv);
	    ಆದರೆ (ನಮೂನೆ == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"pos", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_ವಾದ_str), "pos");
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_ವಾದ_str), "List format should be [lnum, col, off]");
		ಹಿಂತಿರುಗಿ;
	    }
	    li = list_ಕಂಡುಹಿಡಿ(di->di_tv.vval.v_list, 0L);
	    ಆದರೆ (li != ಖಾಲಿ)
	    {
		pos.lnum = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ;
	    }
	    li = list_ಕಂಡುಹಿಡಿ(di->di_tv.vval.v_list, 1L);
	    ಆದರೆ (li != ಖಾಲಿ)
	    {
		pos.ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error) - 1;
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ;
	    }
	    li = list_ಕಂಡುಹಿಡಿ(di->di_tv.vval.v_list, 2L);
	    ಆದರೆ (li != ಖಾಲಿ)
	    {
		pos.coladd = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ;
	    }
	}
    }

    save_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
# ifdef FEAT_ಹುಡುಕು_EXTRA
    save_incsearch_state();
# endif
    ಆದರೆ (ನಮೂನೆ != ಖಾಲಿ)
    {
	ಆದರೆ (*ನಮೂನೆ == NUL)
	    ಹೋಗು the_ಕೊನೆ;
	vim_free(spats[ಕೊನೆಯ_idx].pat);
	spats[ಕೊನೆಯ_idx].patlen = STRLEN(ನಮೂನೆ);
	spats[ಕೊನೆಯ_idx].pat = vim_strnsave(ನಮೂನೆ, spats[ಕೊನೆಯ_idx].patlen);
	ಆದರೆ (spats[ಕೊನೆಯ_idx].pat == ಖಾಲಿ)
	    spats[ಕೊನೆಯ_idx].patlen = 0;
    }
    ಆದರೆ (spats[ಕೊನೆಯ_idx].pat == ಖಾಲಿ || *spats[ಕೊನೆಯ_idx].pat == NUL)
	ಹೋಗು the_ಕೊನೆ;	// the previous pattern was never defined

    update_ಹುಡುಕು_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "current", stat.cur);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "total", stat.cnt);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "exact_match", stat.exact_ಹೊಂದಿಕೆ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "maxcount", stat.ಕೊನೆಯ_maxcount);

the_ಕೊನೆ:
    restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
# ifdef FEAT_ಹುಡುಕು_EXTRA
    restore_incsearch_state();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ
