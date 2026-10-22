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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_EVAL
ସ୍ଥିର ଶୂନ୍ୟ set_vv_searchforward(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
ସ୍ଥିର char_u *get_ଧାଡ଼ି_and_copy(linenr_T lnum, char_u *buf);
ସ୍ଥିର ଶୂନ୍ୟ show_pat_in_path(char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, FILE *, linenr_T *, long);
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା searchstat
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cur;	    // current position of found words
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cnt;	    // total count of found words
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    exact_match;    // TRUE if matched exactly on specified position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
ସ୍ଥିର ଶୂନ୍ୟ save_incsearch_state(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ restore_incsearch_state(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_prevcol(char_u *linep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *prevcol);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
ସ୍ଥିର ଶୂନ୍ୟ find_mps_ମୂଲ୍ୟଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *initc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *findc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *backwards, ପୂର୍ଣ୍ଣସଂଖ୍ୟା switchit);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_zero_width(char_u *pattern, size_t patternlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା move, pos_T *cur, ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction);
ସ୍ଥିର ଶୂନ୍ୟ cmdline_ସନ୍ଧାନ_stat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirc, pos_T *pos, pos_T *cursor_pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ, char_u *msgbuf, size_t msgbuflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା recompute, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxcount, long timeout);
ସ୍ଥିର ଶୂନ୍ୟ update_ସନ୍ଧାନ_stat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା recompute, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxcount, long timeout);

#define ସନ୍ଧାନ_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define ସନ୍ଧାନ_STAT_BUF_LEN 16

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
ସ୍ଥିର spat_T spats[2] =
{
    {ଖାଲି, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ଖାଲି, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_idx = 0;	// index in spats[] for RE_LAST

ସ୍ଥିର char_u lastc[2] = {NUL, NUL};	// last character searched for
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastcdir = FORWARD;		// last direction of character search
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_t_cmd = TRUE;		// last search t_cmd
ସ୍ଥିର char_u	lastc_bytes[MB_MAXBYTES + 1];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ସ୍ଥିର spat_T	    saved_spats[ARRAY_LENGTH(spats)];
ସ୍ଥିର char_u	    *saved_mr_pattern = ଖାଲି;
ସ୍ଥିର size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_spats_last_idx = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
ସ୍ଥିର char_u	    *mr_pattern = ଖାଲି;
ସ୍ଥିର size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matched;	// Found a match in this file
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଧାନ_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_save,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_use,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ଯଦି (pat == ଖାଲି || *pat == NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି (pat_use == RE_LAST)
	    i = last_idx;
	ଅନ୍ୟଥା
	    i = pat_use;
	ଯଦି (spats[i].pat == ଖାଲି)	// pattern was never defined
	{
	    ଯଦି (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    ଅନ୍ୟଥା
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    ଫେରନ୍ତୁ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ଅନ୍ୟଥା ଯଦି (options & ସନ୍ଧାନ_HIS)	// put new pattern in history
	add_to_history(HIST_ସନ୍ଧାନ, pat, patlen, TRUE, NUL);

    ଯଦି (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    ଅନ୍ୟଥା
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    ଯଦି (mr_pattern == ଖାଲି)
	mr_patternlen = 0;
    ଅନ୍ୟଥା
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ଯଦି (!(options & ସନ୍ଧାନ_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ଯଦି (pat_save == RE_ସନ୍ଧାନ || pat_save == RE_BOTH)
	    save_re_pat(RE_ସନ୍ଧାନ, pat, patlen, magic);
	// substitute or global command
	ଯଦି (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ଯଦି (regmatch->regprog == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_ସନ୍ଧାନ_pat(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ mr_pattern;
}

    ଶୂନ୍ୟ
save_re_pat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *pat, size_t patlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା magic)
{
    ଯଦି (spats[idx].pat == pat)
	ଫେରନ୍ତୁ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ଯଦି (spats[idx].pat == ଖାଲି)
	spats[idx].patlen = 0;
    ଅନ୍ୟଥା
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ଯଦି (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_level = 0;

    ଶୂନ୍ୟ
save_ସନ୍ଧାନ_patterns(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (save_level++ != 0)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ଯଦି (spats[i].pat != ଖାଲି)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ଯଦି (saved_spats[i].pat == ଖାଲି)
		saved_spats[i].patlen = 0;
	    ଅନ୍ୟଥା
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ଯଦି (mr_pattern == ଖାଲି)
	saved_mr_pattern = ଖାଲି;
    ଅନ୍ୟଥା
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    ଯଦି (saved_mr_pattern == ଖାଲି)
	saved_mr_patternlen = 0;
    ଅନ୍ୟଥା
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    ଶୂନ୍ୟ
restore_ସନ୍ଧାନ_patterns(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (--save_level != 0)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ଯଦି defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_ସନ୍ଧାନ_patterns(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ସ୍ଥିର spat_T	    saved_last_ସନ୍ଧାନ_spat;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_save_last_ସନ୍ଧାନ_spat = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_last_idx = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_no_hlsearch = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_ସନ୍ଧାନ_match_endcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    ଶୂନ୍ୟ
save_last_ସନ୍ଧାନ_pattern(ଶୂନ୍ୟ)
{
    ଯଦି (++did_save_last_ସନ୍ଧାନ_spat != 1)
	// nested call, nothing to do
	ଫେରନ୍ତୁ;

    saved_last_ସନ୍ଧାନ_spat = spats[RE_ସନ୍ଧାନ];
    ଯଦି (spats[RE_ସନ୍ଧାନ].pat != ଖାଲି)
    {
	saved_last_ସନ୍ଧାନ_spat.pat = vim_strnsave(spats[RE_ସନ୍ଧାନ].pat, spats[RE_ସନ୍ଧାନ].patlen);
	ଯଦି (saved_last_ସନ୍ଧାନ_spat.pat == ଖାଲି)
	    saved_last_ସନ୍ଧାନ_spat.patlen = 0;
	ଅନ୍ୟଥା
	    saved_last_ସନ୍ଧାନ_spat.patlen = spats[RE_ସନ୍ଧାନ].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    ଶୂନ୍ୟ
restore_last_ସନ୍ଧାନ_pattern(ଶୂନ୍ୟ)
{
    ଯଦି (--did_save_last_ସନ୍ଧାନ_spat > 0)
	// nested call, nothing to do
	ଫେରନ୍ତୁ;
    ଯଦି (did_save_last_ସନ୍ଧାନ_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ଫେରନ୍ତୁ;
    }

    vim_free(spats[RE_ସନ୍ଧାନ].pat);
    spats[RE_ସନ୍ଧାନ] = saved_last_ସନ୍ଧାନ_spat;
    saved_last_ସନ୍ଧାନ_spat.pat = ଖାଲି;
    saved_last_ସନ୍ଧାନ_spat.patlen = 0;
# ଯଦି defined(FEAT_EVAL)
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
    ସ୍ଥିର ଶୂନ୍ୟ
save_incsearch_state(ଶୂନ୍ୟ)
{
    saved_ସନ୍ଧାନ_match_endcol = ସନ୍ଧାନ_match_endcol;
    saved_ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ  = ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ;
}

    ସ୍ଥିର ଶୂନ୍ୟ
restore_incsearch_state(ଶୂନ୍ୟ)
{
    ସନ୍ଧାନ_match_endcol = saved_ସନ୍ଧାନ_match_endcol;
    ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ  = saved_ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ;
}

    char_u *
last_ସନ୍ଧାନ_pattern(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ spats[RE_ସନ୍ଧାନ].pat;
}

    size_t
last_ସନ୍ଧାନ_pattern_len(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ spats[RE_ସନ୍ଧାନ].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ignorecase(char_u *pat)
{
    ଫେରନ୍ତୁ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ignorecase_opt(char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = ic_in;

    ଯଦି (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ଫେରନ୍ତୁ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (ଶୂନ୍ୟ)skip_regexp_ex(pat, NUL, magic_isset(), ଖାଲି, ଖାଲି, &magic_val);

    ଯେତେବେଳେ (*p != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ଯଦି (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ଫେରନ୍ତୁ TRUE;
	    p += l;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ଯଦି (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ଅନ୍ୟଥା ଯଦି (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ଅନ୍ୟଥା ଯଦି (p[1] != NUL)  // skip "\X"
		p += 2;
	    ଅନ୍ୟଥା
		p += 1;
	}
	ଅନ୍ୟଥା ଯଦି ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ଯଦି (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ଅନ୍ୟଥା
		p++;
	}
	ଅନ୍ୟଥା ଯଦି (MB_ISUPPER(*p))
	    ଫେରନ୍ତୁ TRUE;
	ଅନ୍ୟଥା
	    ++p;
    }
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_EVAL)
    char_u *
last_csearch(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ lastc_bytes;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
last_csearch_forward(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ lastcdir == FORWARD;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
last_csearch_until(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ last_t_cmd == TRUE;
}

    ଶୂନ୍ୟ
set_last_csearch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    *lastc = c;
    lastc_bytelen = len;
    ଯଦି (len)
	memcpy(lastc_bytes, s, len);
    ଅନ୍ୟଥା
	CLEAR_FIELD(lastc_bytes);
}
#endif

    ଶୂନ୍ୟ
set_csearch_direction(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cdir)
{
    lastcdir = cdir;
}

    ଶୂନ୍ୟ
set_csearch_until(ପୂର୍ଣ୍ଣସଂଖ୍ୟା t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_ସନ୍ଧାନ_pat(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    ଶୂନ୍ୟ
reset_ସନ୍ଧାନ_dir(ଶୂନ୍ୟ)
{
    spats[0].off.dir = '/';
#ଯଦି defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    ଶୂନ୍ୟ
set_last_ସନ୍ଧାନ_pat(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ଯଦି (*s == NUL)
	spats[idx].pat = ଖାଲି;
    ଅନ୍ୟଥା
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ଯଦି (spats[idx].pat == ଖାଲି)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ଯଦି defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.ଧାଡ଼ି = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    ଯଦି (setlast)
	last_idx = idx;
    ଯଦି (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ଯଦି (spats[idx].pat == ଖାଲି)
	    saved_spats[idx].pat = ଖାଲି;
	ଅନ୍ୟଥା
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ଯଦି (saved_spats[idx].pat == ଖାଲି)
	    saved_spats[idx].patlen = 0;
	ଅନ୍ୟଥା
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ଯଦି (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    ଶୂନ୍ୟ
last_pat_prog(regmmatch_T *regmatch)
{
    ଯଦି (spats[last_idx].pat == ଖାଲି)
    {
	regmatch->regprog = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (ଶୂନ୍ୟ)ସନ୍ଧାନ_regcomp((char_u *)"", 0, ଖାଲି, 0, last_idx, ସନ୍ଧାନ_KEEP, regmatch);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    char_u	*pat,
    size_t	patlen,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop;
    pos_T	start_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		at_first_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_char_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match_ok;
    long	nmatched;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		submatch = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_match = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unused_timeout_flag = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out = &unused_timeout_flag;  // set when timed out.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_from_match_end;		    // vi-compatible search?

    ଯଦି (ସନ୍ଧାନ_regcomp(pat, patlen, ଖାଲି, RE_ସନ୍ଧାନ, pat_use,
		   (options & (ସନ୍ଧାନ_HIS + ସନ୍ଧାନ_KEEP)), &regmatch) == FAIL)
    {
	ଯଦି ((options & ସନ୍ଧାନ_ସନ୍ଦେଶ) && !rc_did_emsg)
	    semsg(_(e_invalid_ସନ୍ଧାନ_string_str), mr_pattern);
	ଫେରନ୍ତୁ FAIL;
    }

    ସନ୍ଧାନ_from_match_end = vim_strchr(p_cpo, CPO_ସନ୍ଧାନ) != ଖାଲି;

    ଯଦି (extra_arg != ଖାଲି)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	ଯଦି (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    କର	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	ଯଦି (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ଅନ୍ୟଥା ଯଦି (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ଯଦି (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    ଅନ୍ୟଥା
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	ଅନ୍ୟଥା
	    start_char_len = 1;
	ଯଦି (dir == FORWARD)
	{
	    ଯଦି (options & ସନ୍ଧାନ_START)
		extra_col = 0;
	    ଅନ୍ୟଥା
		extra_col = start_char_len;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (options & ସନ୍ଧାନ_START)
		extra_col = start_char_len;
	    ଅନ୍ୟଥା
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_ଧାଡ଼ି = TRUE;	// default: start in first line
	ଯଦି (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_ଧାଡ଼ି = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	ଯଦି (dir == BACKWARD && start_pos.col == 0
					     && (options & ସନ୍ଧାନ_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_ଧାଡ଼ି = FALSE;
	}
	ଅନ୍ୟଥା
	    lnum = pos->lnum;

	ପାଇଁ (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    ପାଇଁ ( ; lnum > 0 && lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
					   lnum += dir, at_first_ଧାଡ଼ି = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ଯଦି (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    ଭାଙ୍ଗ;
		// Stop after passing the time limit.
		ଯଦି (*timed_out)
		    ଭାଙ୍ଗ;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_ଧାଡ଼ି && (options & ସନ୍ଧାନ_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		ଯଦି (regmatch.regprog == ଖାଲି)
		    ଭାଙ୍ଗ;
		// Abort searching on an error (e.g., out of stack).
		ଯଦି (called_emsg > called_emsg_before || *timed_out)
		    ଭାଙ୍ଗ;
		ଯଦି (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    ଯଦି (lnum + matchpos.lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			ptr = (char_u *)"";
		    ଅନ୍ୟଥା
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ଯଦି (dir == FORWARD && at_first_ଧାଡ଼ି)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ଯେତେବେଳେ (matchpos.lnum == 0
				&& ((options & ସନ୍ଧାନ_END) && first_match
				    ?  (nmatched == 1
					&& (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)endpos.col - 1
					     < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)start_pos.col + extra_col)
				    : ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ଯଦି (ସନ୍ଧାନ_from_match_end)
			    {
				ଯଦି (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    ଭାଙ୍ଗ;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				ଯଦି (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ଯଦି (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ଅନ୍ୟଥା
					++matchcol;
				}
			    }
			    ଅନ୍ୟଥା
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ଯଦି (ptr[matchcol] != NUL)
				{
				    ଯଦି (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    ଅନ୍ୟଥା
					++matchcol;
				}
			    }
			    ଯଦି (matchcol == 0 && (options & ସନ୍ଧାନ_START))
				ଭାଙ୍ଗ;
			    ଯଦି (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				ଭାଙ୍ଗ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ଯଦି (regmatch.regprog == ଖାଲି)
				ଭାଙ୍ଗ;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ଯଦି (!match_ok)
			    ଜାରି;
		    }
		    ଯଦି (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			ପାଇଁ (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ଯଦି (loop
				|| ((options & ସନ୍ଧାନ_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)regmatch.endpos[0].col - 1
							< (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)regmatch.startpos[0].col
						      < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    ଅନ୍ୟଥା
				ଭାଙ୍ଗ;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ଯଦି (ସନ୍ଧାନ_from_match_end)
			    {
				ଯଦି (nmatched > 1)
				    ଭାଙ୍ଗ;
				matchcol = endpos.col;
				// for empty match: advance one char
				ଯଦି (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ଯଦି (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ଅନ୍ୟଥା
					++matchcol;
				}
			    }
			    ଅନ୍ୟଥା
			    {
				// Stop when the match is in a next line.
				ଯଦି (matchpos.lnum > 0)
				    ଭାଙ୍ଗ;
				matchcol = matchpos.col;
				ଯଦି (ptr[matchcol] != NUL)
				{
				    ଯଦି (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ଅନ୍ୟଥା
					++matchcol;
				}
			    }
			    ଯଦି (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ଯଦି (*timed_out)
				    match_ok = FALSE;
				ଭାଙ୍ଗ;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ଯଦି (regmatch.regprog == ଖାଲି)
				ଭାଙ୍ଗ;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ଯଦି (!match_ok)
			    ଜାରି;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ଯଦି ((options & ସନ୍ଧାନ_END) && !(options & ସନ୍ଧାନ_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			ଯଦି (endpos.col == 0)
			{
			    ଯଦି (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ଅନ୍ୟଥା
			{
			    --pos->col;
			    ଯଦି (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			ଯଦି (end_pos != ଖାଲି)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			ଯଦି (end_pos != ଖାଲି)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    ଯଦି (end_pos != ଖାଲି)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ = endpos.lnum - matchpos.lnum;
		    ସନ୍ଧାନ_match_endcol = endpos.col;
		    ଭାଙ୍ଗ;
		}
		ଧାଡ଼ି_breakcheck();	// stop if ctrl-C typed
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ଯଦି ((options & ସନ୍ଧାନ_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    ଭାଙ୍ଗ;
		}
#endif

		ଯଦି (loop && lnum == start_pos.lnum)
		    ଭାଙ୍ଗ;	    // if second loop, stop where started
	    }
	    at_first_ଧାଡ଼ି = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ଯଦି (regmatch.regprog == ଖାଲି)
		ଭାଙ୍ଗ;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ଯଦି (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
			      || break_loop
#endif
			      || found || loop)
		ଭାଙ୍ଗ;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ଯଦି (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଅନ୍ୟଥା
		lnum = 1;
	    ଯଦି (!shortmess(SHM_ସନ୍ଧାନ)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & ସନ୍ଧାନ_ସନ୍ଦେଶ))
		give_warning((char_u *)_(dir == BACKWARD
					  ? ଶୀର୍ଷ_bot_ସନ୍ଦେଶ : bot_ଶୀର୍ଷ_ସନ୍ଦେଶ), TRUE);
	    ଯଦି (extra_arg != ଖାଲି)
		extra_arg->sa_wrapped = TRUE;
	}
	ଯଦି (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		|| break_loop
#endif
		)
	    ଭାଙ୍ଗ;
    }
    ଯେତେବେଳେ (--ଗଣନା_2 > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    ଯଦି (extra_arg != ଖାଲି && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    ଯଦି (!found)		    // did not find it
    {
	ଯଦି (got_int)
	    emsg(_(e_interrupted));
	ଅନ୍ୟଥା ଯଦି ((options & ସନ୍ଧାନ_ସନ୍ଦେଶ) == ସନ୍ଧାନ_ସନ୍ଦେଶ)
	{
	    ଯଦି (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    ଅନ୍ୟଥା ଯଦି (lnum == 0)
		semsg(_(e_ସନ୍ଧାନ_hit_ଶୀର୍ଷ_without_match_for_str), mr_pattern);
	    ଅନ୍ୟଥା
		semsg(_(e_ସନ୍ଧାନ_hit_bottom_without_match_for_str), mr_pattern);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ଯଦି (pos->lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	pos->lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	ଯଦି (pos->col > 0)
	    --pos->col;
    }

    ଫେରନ୍ତୁ submatch + 1;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PROTO)
    ଶୂନ୍ୟ
set_ସନ୍ଧାନ_direction(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cdir)
{
    spats[0].off.dir = cdir;
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_vv_searchforward(ଶୂନ୍ୟ)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
first_submatch(regmmatch_T *rp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		submatch;

    ପାଇଁ (submatch = 1; ; ++submatch)
    {
	ଯଦି (rp->startpos[submatch].lnum >= 0)
	    ଭାଙ୍ଗ;
	ଯଦି (submatch == 9)
	{
	    submatch = 0;
	    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ submatch;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_ସନ୍ଧାନ_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_delim,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ଯଦି (*pat == ଖାଲି || **pat == NUL)
	ଫେରନ୍ତୁ 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ଖାଲି;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, ସନ୍ଧାନ_delim, magic_isset(),
						    strcopy, ଖାଲି, ଖାଲି);
    ଯଦି (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ଯଦି (*p == ସନ୍ଧାନ_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->ଧାଡ଼ି = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    ଯଦି (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->ଧାଡ଼ି = TRUE;
    ଅନ୍ୟଥା ଯଦି ((options & ସନ୍ଧାନ_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ଯଦି (*p == 'e')		// end
	    offset->end = ସନ୍ଧାନ_END;
	++p;
    }
    ଯଦି (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ଯଦି (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((ଅକ୍ଷର *)p);
	ଅନ୍ୟଥା ଯଦି (*p == '-')	    // single '-'
	    offset->off = -1;
	ଅନ୍ୟଥା			    // single '+'
	    offset->off = 1;
	++p;
	ଯେତେବେଳେ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ଫେରନ୍ତୁ cmdlen;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_ସନ୍ଧାନ(
    oparg_T	    *oap,	// can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dirc,	// '/' or '?'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ସନ୍ଧାନ_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    show_ସନ୍ଧାନ_stats;
    char_u	    *msgbuf = ଖାଲି;
    size_t	    msgbuflen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ଯଦି (spats[0].off.ଧାଡ଼ି && vim_strchr(p_cpo, CPO_LINEOFF) != ଖାଲି)
    {
	spats[0].off.ଧାଡ଼ି = FALSE;
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
    ଯଦି (dirc == 0)
	dirc = spats[0].off.dir;
    ଅନ୍ୟଥା
    {
	spats[0].off.dir = dirc;
#ଯଦି defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    ଯଦି (options & ସନ୍ଧାନ_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ଅନ୍ୟଥା
	ଯଦି (dirc == '/')
	    dirc = '?';
	ଅନ୍ୟଥା
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ଯଦି (dirc == '/')
    {
	ଯଦି (hasFolding(pos.lnum, ଖାଲି, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (hasFolding(pos.lnum, &pos.lnum, ଖାଲି))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ଯଦି (no_hlsearch && !(options & ସନ୍ଧାନ_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ଖାଲି;
					    // use previous pattern
	ଯଦି (pat == ଖାଲି || *pat == NUL || *pat == ସନ୍ଧାନ_delim)
	{
	    ଯଦି (spats[RE_ସନ୍ଧାନ].pat == ଖାଲି)	    // no previous pattern
	    {
		ଯଦି (spats[RE_SUBST].pat == ଖାଲି)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    ଯାଅ end_do_ସନ୍ଧାନ;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ଅନ୍ୟଥା
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ଯଦି (pat != ଖାଲି && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_ସନ୍ଧାନ_pattern_offset(&pat, &patlen,
				ସନ୍ଧାନ_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ଯଦି (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_ସନ୍ଧାନ_stats = FALSE;
	ଯଦି ((options & ସନ୍ଧାନ_ECHO) && messaging()
		&& !ସନ୍ଦେଶ_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    ସନ୍ଦେଶ_start();

	    // Get the offset, so we know how long it is.
	    ଯଦି (!cmd_silent &&
		    (spats[0].off.ଧାଡ଼ି || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ଯଦି (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		ଅନ୍ୟଥା ଯଦି (!spats[0].off.ଧାଡ଼ି)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ଯଦି (spats[0].off.off != 0 || spats[0].off.ଧାଡ଼ି)
		    off_len += vim_snprintf((ଅକ୍ଷର *)off_buf + off_len,
			   ଆକାର(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ଯଦି (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ଅନ୍ୟଥା
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ଯଦି (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ଯଦି (ସନ୍ଦେଶ_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - ସନ୍ଦେଶ_row) * cmdline_width - 1;
		ଅନ୍ୟଥା
		    // Use up to 'showcmd' column.
		    msgbufsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - ସନ୍ଦେଶ_row - 1) * cmdline_width
			+ sc_col - 1;
		ଯଦି (msgbufsize < plen + off_len + ସନ୍ଧାନ_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + ସନ୍ଧାନ_STAT_BUF_LEN + 3;
	    }
	    ଅନ୍ୟଥା
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ଯଦି (msgbuf == ଖାଲି)
	    {
		msgbuflen = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ଯଦି (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ଯଦି (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    ଅନ୍ୟଥା
			mch_memmove(msgbuf + 1, p, plen);
		    ଯଦି (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = ସନ୍ଦେଶ_strtrunc(msgbuf, TRUE);
		    ଯଦି (trunc != ଖାଲି)
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
		    ଯଦି (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			ଯଦି (r != ଖାଲି)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ଯେତେବେଳେ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ଯଦି ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ଅନ୍ୟଥା
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    ସନ୍ଦେଶ_outtrans(msgbuf);
		    ସନ୍ଦେଶ_clr_eos();
		    ସନ୍ଦେଶ_check();

		    gotocmdline(FALSE);
		    out_flush();
		    ସନ୍ଦେଶ_nowait = TRUE;	    // don't wait for this message
		}

		ଯଦି (!shortmess(SHM_SEARCHCOUNT))
		    show_ସନ୍ଧାନ_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	ଯଦି (!spats[0].off.ଧାଡ଼ି && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    ଯଦି (spats[0].off.off > 0)
	    {
		ପାଇଁ (c = spats[0].off.off; c; --c)
		    ଯଦି (decl(&pos) == -1)
			ଭାଙ୍ଗ;
		ଯଦି (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (c = spats[0].off.off; c; ++c)
		    ଯଦି (incl(&pos) == -1)
			ଭାଙ୍ଗ;
		ଯଦି (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ଖାଲି,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, ଗଣନା_2, spats[0].off.end + (options &
		       (ସନ୍ଧାନ_KEEP + ସନ୍ଧାନ_PEEK + ସନ୍ଧାନ_HIS
			+ ସନ୍ଧାନ_ସନ୍ଦେଶ + ସନ୍ଧାନ_START
			+ ((pat != ଖାଲି && *pat == ';') ? 0 : ସନ୍ଧାନ_NOOF))),
		RE_LAST, sia);

	ଯଦି (dircp != ଖାଲି)
	    *dircp = ସନ୍ଧାନ_delim; // restore second '/' or '?' for normal_cmd()

	ଯଦି (!shortmess(SHM_ସନ୍ଧାନ) && sia && sia->sa_wrapped)
	    show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ = TRUE;

	ଯଦି (c == FAIL)
	{
	    retval = 0;
	    ଯାଅ end_do_ସନ୍ଧାନ;
	}
	ଯଦି (spats[0].off.end && oap != ଖାଲି)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ଯଦି (!(options & ସନ୍ଧାନ_NOOF) || (pat != ଖାଲି && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ଯଦି (spats[0].off.ଧାଡ଼ି)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ଯଦି (c < 1)
		    pos.lnum = 1;
		ଅନ୍ୟଥା ଯଦି (c > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		    pos.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		ଅନ୍ୟଥା
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ଅନ୍ୟଥା ଯଦି (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ଯଦି (c > 0)
		{
		    ଯେତେବେଳେ (c-- > 0)
			ଯଦି (incl(&pos) == -1)
			    ଭାଙ୍ଗ;
		}
		// to the left, check for start of file
		ଅନ୍ୟଥା
		{
		    ଯେତେବେଳେ (c++ < 0)
			ଯଦି (decl(&pos) == -1)
			    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ଯଦି (show_ସନ୍ଧାନ_stats)
	     cmdline_ସନ୍ଧାନ_stat(dirc, &pos, &curwin->w_cursor,
				show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ, msgbuf, msgbuflen,
				(ଗଣନା_2 != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_ସନ୍ଧାନ)
				     && hasFolding(curwin->w_cursor.lnum,
								   ଖାଲି, ଖାଲି))
#endif
				),
				p_msc,
				ସନ୍ଧାନ_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	ଯଦି (!(options & ସନ୍ଧାନ_OPT) || pat == ଖାଲି || *pat != ';')
	    ଭାଙ୍ଗ;

	dirc = *++pat;
	--patlen;
	ସନ୍ଧାନ_delim = dirc;
	ଯଦି (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    ଯାଅ end_do_ସନ୍ଧାନ;
	}
	++pat;
	--patlen;
    }

    ଯଦି (options & ସନ୍ଧାନ_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_ସନ୍ଧାନ:
    ଯଦି ((options & ସନ୍ଧାନ_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ଫେରନ୍ତୁ retval;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଧାନ_for_exact_ଧାଡ଼ି(
    buf_T	*buf,
    pos_T	*pos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_len = ins_compl_len();

    ଯଦି (buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା == 0)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (;;)
    {
	pos->lnum += dir;
	ଯଦି (pos->lnum < 1)
	{
	    ଯଦି (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		ଯଦି (!shortmess(SHM_ସନ୍ଧାନ))
		    give_warning((char_u *)_(ଶୀର୍ଷ_bot_ସନ୍ଦେଶ), TRUE);
	    }
	    ଅନ୍ୟଥା
	    {
		pos->lnum = 1;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (pos->lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    ଯଦି (p_ws)
	    {
		pos->lnum = 1;
		ଯଦି (!shortmess(SHM_ସନ୍ଧାନ))
		    give_warning((char_u *)_(bot_ଶୀର୍ଷ_ସନ୍ଦେଶ), TRUE);
	    }
	    ଅନ୍ୟଥା
	    {
		pos->lnum = 1;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (pos->lnum == start)
	    ଭାଙ୍ଗ;
	ଯଦି (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ଯଦି (compl_status_adding() && !compl_status_sol())
	{
	    ଯଦି ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ଫେରନ୍ତୁ OK;
	}
	ଅନ୍ୟଥା ଯଦି (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ଯଦି ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ଫେରନ୍ତୁ OK;
	}
    }
    ଫେରନ୍ତୁ FAIL;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
searchc(cmdarg_T *cap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା t_cmd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c = cap->nchar;	// char to search for
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			dir = cap->arg;	// TRUE for searching forward
    long		ଗଣନା_2 = cap->count1;	// repeat count
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			col;
    char_u		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			stop = TRUE;

    ଯଦି (c != NUL)	// normal search: remember args for repeat
    {
	ଯଦି (!ଚାବି_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    ଯଦି (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		ଯଦି (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    ଅନ୍ୟଥା		// repeat previous search
    {
	ଯଦି (*lastc == NUL && lastc_bytelen <= 1)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ଅନ୍ୟଥା
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ଯଦି (vim_strchr(p_cpo, CPO_SCOLON) == ଖାଲି && ଗଣନା_2 == 1 && t_cmd)
	    stop = FALSE;
    }

    ଯଦି (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ଅନ୍ୟଥା
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	ଯଦି (has_mbyte)
	{
	    ପାଇଁ (;;)
	    {
		ଯଦି (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    ଯଦି (col >= len)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (col == 0)
			ଫେରନ୍ତୁ FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		ଯଦି (lastc_bytelen <= 1)
		{
		    ଯଦି (p[col] == c && stop)
			ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    ଭାଙ୍ଗ;
		stop = TRUE;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (;;)
	    {
		ଯଦି ((col += dir) < 0 || col >= len)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (p[col] == c && stop)
		    ଭାଙ୍ଗ;
		stop = TRUE;
	    }
	}
    }

    ଯଦି (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	ଯଦି (has_mbyte)
	{
	    ଯଦି (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    ଅନ୍ୟଥା
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    ଫେରନ୍ତୁ OK;
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
findmatch(oparg_T *oap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା initc)
{
    ଫେରନ୍ତୁ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_prevcol(
    char_u	*linep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prevcol)
{
    --col;
    ଯଦି (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    ଯଦି (prevcol)
	*prevcol = col;
    ଫେରନ୍ତୁ (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;

    ପାଇଁ (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    ଯଦି (delim_copy == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *ଧାଡ଼ି = ml_get(lnum);

	ପାଇଁ (p = ଧାଡ଼ି + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    ଯଦି (lnum == endpos->lnum && (colnr_T)(p - ଧାଡ଼ି) >= endpos->col)
		ଭାଙ୍ଗ;
	    ଯଦି (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (found)
	    ଭାଙ୍ଗ;
    }
    vim_free(delim_copy);
    ଫେରନ୍ତୁ found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_mps_ମୂଲ୍ୟଗୁଡ଼ିକ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *initc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *findc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *backwards,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ଯେତେବେଳେ (*ptr != NUL)
    {
	ଯଦି (has_mbyte)
	{
	    char_u *prev;

	    ଯଦି (mb_ptr2char(ptr) == *initc)
	    {
		ଯଦି (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ଫେରନ୍ତୁ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ଯଦି (mb_ptr2char(ptr) == *initc)
	    {
		ଯଦି (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		ଅନ୍ୟଥା
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		ଫେରନ୍ତୁ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*ptr == *initc)
	    {
		ଯଦି (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ଅନ୍ୟଥା
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ଫେରନ୍ତୁ;
	    }
	    ptr += 2;
	    ଯଦି (*ptr == *initc)
	    {
		ଯଦି (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		ଅନ୍ୟଥା
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ଫେରନ୍ତୁ;
	    }
	    ++ptr;
	}
	ଯଦି (*ptr == ',')
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		initc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxtravel)
{
    ସ୍ଥିର pos_T pos;			// current search position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		findc = 0;		// matching brace
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;		// cumulative number of braces
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		backwards = FALSE;	// init for gcc
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		raw_string = FALSE;	// search for raw string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_quotes;		// check for quotes in current line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		at_start;		// do_quotes value at start position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash_dir = 0;		// Direction searched for # things
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_in_quotes;	// start position is in quotes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		traveled = 0;		// how far we've searched so far
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_cend = FALSE;    // ignore comment end
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpo_match;		// vi compatible matching
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpo_bsl;		// don't recognize backslashes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match_escaped = 0;	// search for escaped match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;			// Direction to search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		comment_col = MAXCOL;   // start of / / comment
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lispcomm = FALSE;	// inside of Lisp-style comment
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_comments = (flags & FM_SKIPCOMM) != 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != ଖାଲି);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ଖାଲି);

    // Direction to search when initc is '/', '*' or '#'
    ଯଦି (flags & FM_BACKWARD)
	dir = BACKWARD;
    ଅନ୍ୟଥା ଯଦି (flags & FM_FORWARD)
	dir = FORWARD;
    ଅନ୍ୟଥା
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ଯଦି (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ଯଦି (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    ଅନ୍ୟଥା ଯଦି (initc != '#' && initc != NUL)
    {
	find_mps_ମୂଲ୍ୟଗୁଡ଼ିକ(&initc, &findc, &backwards, TRUE);
	ଯଦି (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ଯଦି (findc == NUL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ଯଦି (initc == '#')
	{
	    hash_dir = dir;
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ଯଦି (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ଯଦି (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ଯଦି (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ଅନ୍ୟଥା ଯଦି (linep[pos.col] == '/')
		{
		    ଯଦି (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    ଅନ୍ୟଥା ଯଦି (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (linep[pos.col] == '*')
		{
		    ଯଦି (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ଅନ୍ୟଥା ଯଦି (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    ଯଦି (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ଯଦି (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		ପାଇଁ (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    ଯଦି (initc == NUL)
			ଭାଙ୍ଗ;

		    find_mps_ମୂଲ୍ୟଗୁଡ଼ିକ(&initc, &findc, &backwards, FALSE);
		    ଯଦି (findc)
			ଭାଙ୍ଗ;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		ଯଦି (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ଯଦି (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ଅନ୍ୟଥା
			ଫେରନ୍ତୁ ଖାଲି;
		}
		ଅନ୍ୟଥା ଯଦି (!cpo_bsl)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    ପାଇଁ (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	ଯଦି (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ଯଦି (oap != ଖାଲି)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ଯଦି (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ଯଦି (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ଅନ୍ୟଥା ଯଦି (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ଅନ୍ୟଥା
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	    pos.col = 0;
	    ଯେତେବେଳେ (!got_int)
	    {
		ଯଦି (hash_dir > 0)
		{
		    ଯଦି (pos.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (pos.lnum == 1)
		    ଭାଙ୍ଗ;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		ଧାଡ଼ି_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ଯଦି (*ptr != '#')
		    ଜାରି;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ଯଦି (hash_dir > 0)
		{
		    ଯଦି (STRNCMP(ptr, "if", 2) == 0)
			ଗଣନା_2++;
		    ଅନ୍ୟଥା ଯଦି (STRNCMP(ptr, "el", 2) == 0)
		    {
			ଯଦି (ଗଣନା_2 == 0)
			    ଫେରନ୍ତୁ &pos;
		    }
		    ଅନ୍ୟଥା ଯଦି (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ଯଦି (ଗଣନା_2 == 0)
			    ଫେରନ୍ତୁ &pos;
			ଗଣନା_2--;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (STRNCMP(ptr, "if", 2) == 0)
		    {
			ଯଦି (ଗଣନା_2 == 0)
			    ଫେରନ୍ତୁ &pos;
			ଗଣନା_2--;
		    }
		    ଅନ୍ୟଥା ଯଦି (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ଯଦି (ଗଣନା_2 == 0)
			    ଫେରନ୍ତୁ &pos;
		    }
		    ଅନ୍ୟଥା ଯଦି (STRNCMP(ptr, "endif", 5) == 0)
			ଗଣନା_2++;
		}
	    }
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ଯଦି (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ଖାଲି)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    ଯଦି ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    ଯଦି (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ଯଦି (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    ଯେତେବେଳେ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ଯଦି (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ଯଦି (lispcomm && pos.col < (colnr_T)comment_col)
		ଭାଙ୍ଗ;
	    ଯଦି (pos.col == 0)		// at start of line, go to prev. one
	    {
		ଯଦି (pos.lnum == 1)	// start of file
		    ଭାଙ୍ଗ;
		--pos.lnum;

		ଯଦି (maxtravel > 0 && ++traveled > maxtravel)
		    ଭାଙ୍ଗ;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		ଧାଡ଼ି_breakcheck();

		// Check if this line contains a single-line comment
		ଯଦି (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		ଯଦି (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		ଅନ୍ୟଥା ଯଦି (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    ଅନ୍ୟଥା
	    {
		--pos.col;
		ଯଦି (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	ଅନ୍ୟଥା				// forward search
	{
	    ଯଦି (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		ଯଦି (pos.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    ଭାଙ୍ଗ;
		++pos.lnum;

		ଯଦି (maxtravel && traveled++ > maxtravel)
		    ଭାଙ୍ଗ;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		ଧାଡ଼ି_breakcheck();
		ଯଦି (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		ଅନ୍ୟଥା
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ଯଦି (skip_comments && !comment_dir && !inquote)
	{
	    ଯଦି (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ଯଦି (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col < comment_col))
		    in_block_comment = TRUE;
		ଅନ୍ୟଥା ଯଦି (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ଯଦି (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col < comment_col))
		    in_block_comment = TRUE;
		ଅନ୍ୟଥା ଯଦି (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ଯଦି (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ଯଦି (linep[0] == findc && ଗଣନା_2 == 0)	// match!
		ଫେରନ୍ତୁ &pos;
	    ଭାଙ୍ଗ;					// out of scope
	}

	ଯଦି (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ଯଦି (comment_dir == FORWARD)
	    {
		ଯଦି (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    ଫେରନ୍ତୁ &pos;
		}
	    }
	    ଅନ୍ୟଥା    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ଯଦି (pos.col == 0)
		    ଜାରି;
		ଅନ୍ୟଥା ଯଦି (raw_string)
		{
		    ଯଦି (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != ଖାଲି)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ଯଦି (!find_rawstring_end(linep, &pos,
				  ଗଣନା_2 > 0 ? &match_pos : &curwin->w_cursor))
			{
			    ଗଣନା_2++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ଅନ୍ୟଥା ଯଦି (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col < comment_col)
		{
		    ଗଣନା_2++;
		    match_pos = pos;
		    match_pos.col--;
		}
		ଅନ୍ୟଥା ଯଦି (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    ଯଦି (ଗଣନା_2 > 0)
			pos = match_pos;
		    ଅନ୍ୟଥା ଯଦି (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col <= comment_col)
			pos.col -= 2;
		    ଅନ୍ୟଥା ଯଦି (ignore_cend)
			ଜାରି;
		    ଅନ୍ୟଥା
			ଫେରନ୍ତୁ ଖାଲି;
		    ଫେରନ୍ତୁ &pos;
		}
	    }
	    ଜାରି;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ଯଦି (cpo_match)
	    do_quotes = 0;
	ଅନ୍ୟଥା ଯଦି (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    ପାଇଁ (ptr = linep; *ptr; ++ptr)
	    {
		ଯଦି (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		ଯଦି (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ଯଦି (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ଯଦି (!do_quotes)
	    {
		inquote = FALSE;
		ଯଦି (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ଯଦି (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			ଯଦି (inquote)
			    start_in_quotes = TRUE;
		    }
		    ଅନ୍ୟଥା ଯଦି (backwards)
			inquote = TRUE;
		}
		ଯଦି (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ଯଦି (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ଯଦି (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    ଯଦି (inquote)
				start_in_quotes = TRUE;
			}
			ଅନ୍ୟଥା ଯଦି (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ଯଦି (start_in_quotes == MAYBE)
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
	ବାଛ (c)
	{
	ସ୍ଥିତି NUL:
	    // at end of line without trailing backslash, reset inquote
	    ଯଦି (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ଯଦି (do_quotes)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

		ପାଇଁ (col = pos.col - 1; col >= 0; --col)
		    ଯଦି (linep[col] != '\\')
			ଭାଙ୍ଗ;
		ଯଦି ((((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    ଭାଙ୍ଗ;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	ସ୍ଥିତି '\'':
	    ଯଦି (!cpo_match && initc != '\'' && findc != '\'')
	    {
		ଯଦି (backwards)
		{
		    ଯଦି (pos.col > 1)
		    {
			ଯଦି (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    ଭାଙ୍ଗ;
			}
		    }
		}
		ଅନ୍ୟଥା ଯଦି (linep[pos.col + 1])	// forward search
		{
		    ଯଦି (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା ଯଦି (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			ଭାଙ୍ଗ;
		    }
		}
	    }
	    // FALLTHROUGH

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ଯଦି (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ଖାଲି
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', ଖାଲି)
		    && check_prevcol(linep, pos.col - 1, '#', ଖାଲି))
		ଭାଙ୍ଗ;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ଯଦି (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col >= comment_col)))
		ଭାଙ୍ଗ;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ଯଦି ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col, bslcnt = 0;

		ଯଦି (!cpo_bsl)
		{
		    ପାଇଁ (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ଯଦି (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    ଯଦି (c == initc)
			ଗଣନା_2++;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (ଗଣନା_2 == 0)
			    ଫେରନ୍ତୁ &pos;
			ଗଣନା_2--;
		    }
		}
	    }
	}
    }

    ଯଦି (comment_dir == BACKWARD && ଗଣନା_2 > 0)
    {
	pos = match_pos;
	ଫେରନ୍ତୁ &pos;
    }
    ଫେରନ୍ତୁ (pos_T *)ଖାଲି;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    ଶୂନ୍ୟ
showmatch(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    ପାଇଁ (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	ଯଦି (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    ଭାଙ୍ଗ;
#endif
	p += mb_ptr2len(p) + 1;
	ଯଦି (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    ଭାଙ୍ଗ;
	p += mb_ptr2len(p);
	ଯଦି (*p == NUL)
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    ଯଦି ((lpos = findmatch(ଖାଲି, NUL)) == ଖାଲି)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ଫେରନ୍ତୁ;

    ଯଦି (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ଖାଲି, &vcol, ଖାଲି, 0);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    ଯଦି (!col_visible)
	ଫେରନ୍ତୁ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ଯଦି (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    ଯଦି (vim_strchr(p_cpo, CPO_SHOWMATCH) != ଖାଲି)
	ui_delay(p_mat * 100L + 8, TRUE);
    ଅନ୍ୟଥା ଯଦି (!char_avail())
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		move,
    pos_T	*cur,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		direction)
{
    regmmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nmatched = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = -1;
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flag = 0;

    ଯଦି (pattern == ଖାଲି)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    ଯଦି (ସନ୍ଧାନ_regcomp(pattern, patternlen, ଖାଲି, RE_ସନ୍ଧାନ, RE_ସନ୍ଧାନ,
					      ସନ୍ଧାନ_KEEP, &regmatch) == FAIL)
	ଫେରନ୍ତୁ -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    ଯଦି (move)
    {
	CLEAR_POS(&pos);
    }
    ଅନ୍ୟଥା
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = ସନ୍ଧାନ_START;
    }

    ଯଦି (searchit(curwin, curbuf, &pos, ଖାଲି, direction, pattern, patternlen, 1,
				  ସନ୍ଧାନ_KEEP + flag, RE_ସନ୍ଧାନ, ଖାଲି) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	କର
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, ଖାଲି);
	    ଯଦି (nmatched != 0)
		ଭାଙ୍ଗ;
	} ଯେତେବେଳେ (regmatch.regprog != ଖାଲି
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	ଯଦି (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    ଫେରନ୍ତୁ result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_ସନ୍ଧାନ(
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    pos_T	save_VIsual = VIsual;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zero_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    ଯଦି (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ଯଦି (VIsual_active)
    {
	ଯଦି (forward)
	    incl(&pos);
	ଅନ୍ୟଥା
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ଯଦି (zero_width == -1)
	ଫେରନ୍ତୁ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    ପାଇଁ (i = 0; i < 2; i++)
    {
	ଯଦି (forward)
	{
	    ଯଦି (i == 0 && skip_first_backward)
		ଜାରି;
	    dir = i;
	}
	ଅନ୍ୟଥା
	    dir = !i;

	flags = 0;
	ଯଦି (!dir && !zero_width)
	    flags = ସନ୍ଧାନ_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	ଯଦି (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? ଗଣନା_2 : 1),
		ସନ୍ଧାନ_KEEP | flags, RE_ସନ୍ଧାନ, ଖାଲି);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	ଯଦି (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ଯଦି (VIsual_active)
		VIsual = save_VIsual;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (i == 0 && !result)
	{
	    ଯଦି (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ଅନ୍ୟଥା
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
	    }
	}
    }

    start_pos = pos;

    ଯଦି (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    ଯଦି (LT_POS(VIsual, end_pos) && forward)
    {
	ଯଦି (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ଅନ୍ୟଥା
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ଅନ୍ୟଥା ଯଦି (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    ଯଦି (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ଯଦି (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ଅନ୍ୟଥା ଯଦି (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    ଯଦି (fdo_flags & FDO_ସନ୍ଧାନ && ଚାବି_Typed)
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

    ଫେରନ୍ତୁ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ଫେରନ୍ତୁ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cmdline_ସନ୍ଧାନ_stat(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ,
    char_u	*msgbuf,
    size_t	msgbuflen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		recompute,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_ସନ୍ଧାନ_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ଯଦି (stat.cur <= 0)
	ଫେରନ୍ତୁ;

    ଅକ୍ଷର	t[ସନ୍ଧାନ_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ଯଦି (stat.incomplete == 1)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[?/??]");
	ଅନ୍ୟଥା ଯଦି (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ଅନ୍ୟଥା ଯଦି (stat.cnt > maxcount)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ଅନ୍ୟଥା
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ଅନ୍ୟଥା
#endif
    {
	ଯଦି (stat.incomplete == 1)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[?/??]");
	ଅନ୍ୟଥା ଯଦି (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ଅନ୍ୟଥା ଯଦି (stat.cnt > maxcount)
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ଅନ୍ୟଥା
	    len = vim_snprintf(t, ସନ୍ଧାନ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ଯଦି (show_ଶୀର୍ଷ_bot_ସନ୍ଦେଶ && len + 2 < ସନ୍ଧାନ_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ଯଦି (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ଯଦି (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    ସନ୍ଦେଶ_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    ସନ୍ଦେଶ_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_ସନ୍ଧାନ_stat(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			recompute,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			maxcount,
    long		timeout UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    save_ws = p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ସ୍ଥିର pos_T    lastpos = {0, 0, 0};
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cur = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cnt = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    exact_match = FALSE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    incomplete = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    last_maxcount = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    chgtick = 0;
    ସ୍ଥିର char_u   *lastpat = ଖାଲି;
    ସ୍ଥିର size_t   lastpatlen = 0;
    ସ୍ଥିର buf_T    *lbuf = ଖାଲି;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    ଯଦି (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	ଫେରନ୍ତୁ;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ଯଦି (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ଖାଲି
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
    ଯଦି (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done_ସନ୍ଧାନ = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	ଯଦି (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	ଯେତେବେଳେ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ଖାଲି, 0, 1, ସନ୍ଧାନ_KEEP, RE_LAST, ଖାଲି) != FAIL)
	{
	    done_ସନ୍ଧାନ = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    ଯଦି (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		ଭାଙ୍ଗ;
	    }
#endif
	    cnt++;
	    ଯଦି (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ଯଦି (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    ଯଦି (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (got_int)
	    cur = -1; // abort
	ଯଦି (done_ସନ୍ଧାନ)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    ଯଦି (lastpat == ଖାଲି)
		lastpatlen = 0;
	    ଅନ୍ୟଥା
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

#ଯଦି defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ସ୍ଥିର char_u *
get_ଧାଡ଼ି_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *ଧାଡ଼ି = ml_get(lnum);

    vim_strncpy(buf, ଧାଡ଼ି, LSIZE - 1);
    ଫେରନ୍ତୁ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    ଶୂନ୍ୟ
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir UNUSED,	// direction of expansion
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// length of search pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whole,		// match whole words only
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_comments,	// don't match inside comments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,		// Type of search; are we looking for a type?
				// a macro?
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,	// If true, always switch to the found path
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_path_depth = 50;
    long	match_ଗଣନା = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ଖାଲି;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth_displayed;	// For type==CHECK_PATH
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_files;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		already_searched;
    char_u	*file_ଧାଡ଼ି;
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    char_u	save_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matched = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_show = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*already = ଖାଲି;
    char_u	*startp = ଖାଲି;
    char_u	*inc_opt = ଖାଲି;
# ଯଦି defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ଖାଲି;
# endif

    regmatch.regprog = ଖାଲି;
    incl_regmatch.regprog = ଖାଲି;
    def_regmatch.regprog = ଖାଲି;

    file_ଧାଡ଼ି = alloc(LSIZE);
    ଯଦି (file_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	ଯଦି (pat == ଖାଲି)
	    ଯାଅ fpip_end;
	vim_snprintf((ଅକ୍ଷର *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ଯଦି (regmatch.regprog == ଖାଲି)
	    ଯାଅ fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ଯଦି (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ଯଦି (incl_regmatch.regprog == ଖାଲି)
	    ଯାଅ fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ଯଦି (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ଯଦି (def_regmatch.regprog == ଖାଲି)
	    ଯାଅ fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * ଆକାର(SearchedFile), TRUE);
    ଯଦି (files == ଖାଲି)
	ଯାଅ fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    ଯଦି (end_lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	end_lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯଦି (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    ଧାଡ଼ି = get_ଧାଡ଼ି_and_copy(lnum, file_ଧାଡ଼ି);

    ପାଇଁ (;;)
    {
	ଯଦି (incl_regmatch.regprog != ଖାଲି
		&& vim_regexec(&incl_regmatch, ଧାଡ଼ି, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ଯଦି (strstr((ଅକ୍ଷର *)inc_opt, "\\zs") != ଖାଲି)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ଅନ୍ୟଥା
		// Use text after match with 'include'.
		new_fname = file_name_in_ଧାଡ଼ି(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ଖାଲି);
	    already_searched = FALSE;
	    ଯଦି (new_fname != ଖାଲି)
	    {
		// Check whether we have already searched in this file
		ପାଇଁ (i = 0;; i++)
		{
		    ଯଦି (i == depth + 1)
			i = old_files;
		    ଯଦି (i == max_path_depth)
			ଭାଙ୍ଗ;
		    ଯଦି (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ଯଦି (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    ସନ୍ଦେଶ_putchar('\n');	    // cursor below last one
			    ଯଦି (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				ସନ୍ଦେଶ_home_replace_hl(new_fname);
				ସନ୍ଦେଶ_puts(_(" (includes previously listed match)"));
				prev_fname = ଖାଲି;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			ଭାଙ୍ଗ;
		    }
		}
	    }

	    ଯଦି (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == ଖାଲି && !already_searched)))
	    {
		ଯଦି (did_show)
		    ସନ୍ଦେଶ_putchar('\n');	    // cursor below last one
		ଅନ୍ୟଥା
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    ସନ୍ଦେଶ_puts_title(_("--- Included files "));
		    ଯଦି (action != ACTION_SHOW_ALL)
			ସନ୍ଦେଶ_puts_title(_("not found "));
		    ସନ୍ଦେଶ_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ଯେତେବେଳେ (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    ପାଇଁ (i = 0; i < depth_displayed; i++)
			ସନ୍ଦେଶ_puts("  ");
		    ସନ୍ଦେଶ_home_replace(files[depth_displayed].name);
		    ସନ୍ଦେଶ_puts(" -->\n");
		}
		ଯଦି (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    ପାଇଁ (i = 0; i <= depth_displayed; i++)
			ସନ୍ଦେଶ_puts("  ");
		    ଯଦି (new_fname != ଖାଲି)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			ସନ୍ଦେଶ_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ଅନ୍ୟଥା
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ଯଦି (strstr((ଅକ୍ଷର *)inc_opt, "\\zs") != ଖାଲି)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ଅନ୍ୟଥା
			{
			    // find the file name after the end of the match
			    ପାଇଁ (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    ପାଇଁ (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ଯଦି (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			ଅନ୍ୟଥା ଯଦି (p > ଧାଡ଼ି)
			{
			    ଯଦି (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ଯଦି (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			ସନ୍ଦେଶ_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ଯଦି (new_fname == ଖାଲି && action == ACTION_SHOW_ALL)
		    {
			ଯଦି (already_searched)
			    ସନ୍ଦେଶ_puts(_("  (Already listed)"));
			ଅନ୍ୟଥା
			    ସନ୍ଦେଶ_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    ଯଦି (new_fname != ଖାଲି)
	    {
		// Push the new file onto the file stack
		ଯଦି (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    ଯଦି (bigger != ଖାଲି)
		    {
			ପାଇଁ (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			ପାଇଁ (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = ଖାଲି;
			    bigger[i].name = ଖାଲି;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			ପାଇଁ (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ଯଦି ((files[depth + 1].fp = mch_fopen((ଅକ୍ଷର *)new_fname, "r"))
								    == ଖାଲି)
		    vim_free(new_fname);
		ଅନ୍ୟଥା
		{
		    ଯଦି (++depth == old_files)
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
		    ଯଦି (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			ସନ୍ଦେଶ_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((ଅକ୍ଷର*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(ଅକ୍ଷର *)new_fname);
			ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ଅନ୍ୟଥା ଯଦି (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (ଅକ୍ଷର *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = ଧାଡ଼ି;
ସନ୍ଧାନ_ଧାଡ଼ି:
	    define_matched = FALSE;
	    ଯଦି (def_regmatch.regprog != ଖାଲି
			      && vim_regexec(&def_regmatch, ଧାଡ଼ି, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ଯେତେବେଳେ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ଯଦି (def_regmatch.regprog == ଖାଲି || define_matched)
	    {
		ଯଦି (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ଯଦି (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ଅନ୍ୟଥା
			matched = STRNCMP(startp, ptr, len) == 0;
		    ଯଦି (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ଅନ୍ୟଥା ଯଦି (regmatch.regprog != ଖାଲି
			 && vim_regexec(&regmatch, ଧାଡ଼ି, (colnr_T)(p - ଧାଡ଼ି)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ଯଦି (!define_matched && skip_comments)
		    {
			ଯଦି ((*ଧାଡ଼ି != '#' ||
				STRNCMP(skipwhite(ଧାଡ଼ି + 1), "define", 6) != 0)
				&& get_leader_len(ଧାଡ଼ି, ଖାଲି, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(ଧାଡ଼ି);
			ଯଦି (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    ପାଇଁ (p = ଧାଡ଼ି; *p && p < startp; ++p)
			    {
				ଯଦି (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ଯଦି (p[1] == '/')
					ଭାଙ୍ଗ;
				    ++p;
				}
				ଅନ୍ୟଥା ଯଦି (!matched && p[0] == '*' && p[1] == '/')
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
	ଯଦି (matched)
	{
	    ଯଦି (action == ACTION_EXPAND)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cont_s_ipos = FALSE;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	add_r;
		char_u	*aux;

		ଯଦି (depth == -1 && lnum == curwin->w_cursor.lnum)
		    ଭାଙ୍ଗ;
		found = TRUE;
		aux = p = startp;
		ଯଦି (compl_status_adding() && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ଯଦି (vim_iswordp(p))
			ଯାଅ exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - aux);

		ଯଦି (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ଯଦି (depth < 0)
		    {
			ଯଦି (lnum >= end_lnum)
			    ଯାଅ exit_matched;
			ଧାଡ଼ି = get_ଧାଡ଼ି_and_copy(++lnum, file_ଧାଡ଼ି);
		    }
		    ଅନ୍ୟଥା ଯଦି (vim_fgets(ଧାଡ଼ି = file_ଧାଡ଼ି,
						      LSIZE, files[depth].fp))
			ଯାଅ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(ଧାଡ଼ି);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    ଯଦି (p > aux)
		    {
			ଯଦି (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ଯଦି (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ଯଦି (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ଖାଲି
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ଯଦି (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ଯଦି (i == ins_compl_len())
			ଯାଅ exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ଖାଲି
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ଯଦି (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ଅନ୍ୟଥା ଯଦି (add_r == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		ଯଦି (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ଯଦି (curr_fname != prev_fname)
		{
		    ଯଦି (did_show)
			ସନ୍ଦେଶ_putchar('\n');	// cursor below last one
		    ଯଦି (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			ସନ୍ଦେଶ_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ଯଦି (!got_int)
		    show_pat_in_path(ଧାଡ଼ି, type, TRUE, action,
			    (depth == -1) ? ଖାଲି : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_ଗଣନା++);

		// Set matched flag for this file and all the ones that
		// include it
		ପାଇଁ (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (--ଗଣନା_2 <= 0)
	    {
		found = TRUE;
		ଯଦି (depth == -1 && lnum == curwin->w_cursor.lnum
# ଯଦି defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_ଧାଡ଼ି));
		ଅନ୍ୟଥା ଯଦି (action == ACTION_SHOW)
		{
		    show_pat_in_path(ଧାଡ଼ି, type, did_show, action,
			    (depth == -1) ? ଖାଲି : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		ଅନ୍ୟଥା
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ଯଦି defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ଯଦି (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ଯଦି (action == ACTION_SPLIT)
		    {
			ଯଦି (win_split(0, 0) == FAIL)
			    ଭାଙ୍ଗ;
			RESET_BINDING(curwin);
		    }
		    ଯଦି (depth == -1)
		    {
			// match in current file
# ଯଦି defined(FEAT_QUICKFIX)
			ଯଦି (g_do_tagpreview != 0)
			{
			    ଯଦି (!win_valid(curwin_save))
				ଭାଙ୍ଗ;
			    ଯଦି (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, ଖାଲି,
						     ଖାଲି, TRUE, lnum, forceit)))
				ଭାଙ୍ଗ;	// failed to jump to file
			}
			ଅନ୍ୟଥା
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, ଖାଲି, TRUE,
						    files[depth].lnum, forceit)))
			    ଭାଙ୍ଗ;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		ଯଦି (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - ଧାଡ଼ି);
		    curwin->w_set_curswant = true;
		}

# ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		ଅନ୍ୟଥା ଯଦି (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		ଭାଙ୍ଗ;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ଯଦି (def_regmatch.regprog == ଖାଲି
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		ଯାଅ ସନ୍ଧାନ_ଧାଡ଼ି;
	}
	ଧାଡ଼ି_breakcheck();
	ଯଦି (action == ACTION_EXPAND)
	    ins_compl_check_ଚାବିଗୁଡ଼ିକ(30, FALSE);
	ଯଦି (got_int || ins_compl_interrupted())
	    ଭାଙ୍ଗ;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ଯେତେବେଳେ (depth >= 0 && !already
		&& vim_fgets(ଧାଡ଼ି = file_ଧାଡ଼ି, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    ଯଦି (depth < depth_displayed)
		depth_displayed = depth;
	}
	ଯଦି (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି);
	    ଯଦି (i > 0 && ଧାଡ଼ି[i - 1] == '\n')
		ଧାଡ଼ି[--i] = NUL;
	    ଯଦି (i > 0 && ଧାଡ଼ି[i - 1] == '\r')
		ଧାଡ଼ି[--i] = NUL;
	}
	ଅନ୍ୟଥା ଯଦି (!already)
	{
	    ଯଦି (++lnum > end_lnum)
		ଭାଙ୍ଗ;
	    ଧାଡ଼ି = get_ଧାଡ଼ି_and_copy(lnum, file_ଧାଡ଼ି);
	}
	already = ଖାଲି;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    ପାଇଁ (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    ପାଇଁ (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    ଯଦି (type == CHECK_PATH)
    {
	ଯଦି (!did_show)
	{
	    ଯଦି (action != ACTION_SHOW_ALL)
		ସନ୍ଦେଶ(_("All included files were found"));
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ(_("No included files"));
	}
    }
    ଅନ୍ୟଥା ଯଦି (!found && action != ACTION_EXPAND && !silent)
    {
	ଯଦି (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ଅନ୍ୟଥା ଯଦି (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	ଅନ୍ୟଥା
	    emsg(_(e_couldnt_find_pattern));
    }
    ଯଦି (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	ସନ୍ଦେଶ_end();

fpip_end:
    vim_free(file_ଧାଡ଼ି);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ସ୍ଥିର ଶୂନ୍ୟ
show_pat_in_path(
    char_u  *ଧାଡ଼ି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_show,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    ଗଣନା_2)
{
    char_u  *p;
    size_t  linelen;

    ଯଦି (did_show)
	ସନ୍ଦେଶ_putchar('\n');	// cursor below last one
    ଅନ୍ୟଥା ଯଦି (!ସନ୍ଦେଶ_silent)
	gotocmdline(TRUE);	// cursor at status line
    ଯଦି (got_int)		// 'q' typed at "--more--" message
	ଫେରନ୍ତୁ;
    linelen = STRLEN(ଧାଡ଼ି);
    ପାଇଁ (;;)
    {
	p = ଧାଡ଼ି + linelen - 1;
	ଯଦି (fp != ଖାଲି)
	{
	    // We used fgets(), so get rid of newline at end
	    ଯଦି (p >= ଧାଡ଼ି && *p == '\n')
		--p;
	    ଯଦି (p >= ଧାଡ଼ି && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ଯଦି (action == ACTION_SHOW_ALL)
	{
	    sprintf((ଅକ୍ଷର *)IObuff, "%3ld: ", ଗଣନା_2);	// show match nr
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)IObuff);
	    sprintf((ଅକ୍ଷର *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)IObuff, HL_ATTR(HLF_N));
	    ସନ୍ଦେଶ_puts(" ");
	}
	ସନ୍ଦେଶ_prt_ଧାଡ଼ି(ଧାଡ଼ି, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ଯଦି (got_int || type != FIND_DEFINE || p < ଧାଡ଼ି || *p != '\\')
	    ଭାଙ୍ଗ;

	ଯଦି (fp != ଖାଲି)
	{
	    ଯଦି (vim_fgets(ଧାଡ଼ି, LSIZE, fp)) // end of file
		ଭାଙ୍ଗ;
	    linelen = STRLEN(ଧାଡ଼ି);
	    ++*lnum;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (++*lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	    ଧାଡ଼ି = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	ସନ୍ଦେଶ_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_spat_last_idx(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ last_idx;
}
#endif

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    ଶୂନ୍ୟ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			maxcount = p_msc;
    long		timeout = ସନ୍ଧାନ_STAT_DEF_TIMEOUT;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			recompute = TRUE;
    searchstat_T	stat;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	ଯଦି (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ଫେରନ୍ତୁ;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	ଯଦି (di != ଖାଲି)
	{
	    timeout = (long)tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	ଯଦି (di != ଖାଲି)
	{
	    maxcount = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	ଯଦି (di != ଖାଲି)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    ଯଦି (pattern == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		ଫେରନ୍ତୁ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    ଯଦି (li != ଖାଲି)
	    {
		pos.lnum = tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, &error);
		ଯଦି (error)
		    ଫେରନ୍ତୁ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    ଯଦି (li != ଖାଲି)
	    {
		pos.col = tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, &error) - 1;
		ଯଦି (error)
		    ଫେରନ୍ତୁ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    ଯଦି (li != ଖାଲି)
	    {
		pos.coladd = tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, &error);
		ଯଦି (error)
		    ଫେରନ୍ତୁ;
	    }
	}
    }

    save_last_ସନ୍ଧାନ_pattern();
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    save_incsearch_state();
# endif
    ଯଦି (pattern != ଖାଲି)
    {
	ଯଦି (*pattern == NUL)
	    ଯାଅ the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	ଯଦି (spats[last_idx].pat == ଖାଲି)
	    spats[last_idx].patlen = 0;
    }
    ଯଦି (spats[last_idx].pat == ଖାଲି || *spats[last_idx].pat == NUL)
	ଯାଅ the_end;	// the previous pattern was never defined

    update_ସନ୍ଧାନ_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "current", stat.cur);
    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_ସନ୍ଧାନ_pattern();
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
    restore_incsearch_state();
# endif
}
#endif
