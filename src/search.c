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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_EVAL
സ്ഥിതം ശൂന്യം set_vv_searchforward(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
സ്ഥിതം char_u *get_വരി_and_copy(linenr_T lnum, char_u *buf);
സ്ഥിതം ശൂന്യം show_pat_in_path(char_u *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, FILE *, linenr_T *, long);
#endif

തരനിർവചനം ഘടന searchstat
{
    പൂർണ്ണസംഖ്യ	    cur;	    // current position of found words
    പൂർണ്ണസംഖ്യ	    cnt;	    // total count of found words
    പൂർണ്ണസംഖ്യ	    exact_match;    // TRUE if matched exactly on specified position
    പൂർണ്ണസംഖ്യ	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    പൂർണ്ണസംഖ്യ	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_തിരയൽ_EXTRA
സ്ഥിതം ശൂന്യം save_incsearch_state(ശൂന്യം);
സ്ഥിതം ശൂന്യം restore_incsearch_state(ശൂന്യം);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ check_prevcol(char_u *linep, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ ch, പൂർണ്ണസംഖ്യ *prevcol);
സ്ഥിതം പൂർണ്ണസംഖ്യ find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
സ്ഥിതം ശൂന്യം find_mps_മൂല്യങ്ങൾ(പൂർണ്ണസംഖ്യ *initc, പൂർണ്ണസംഖ്യ *findc, പൂർണ്ണസംഖ്യ *backwards, പൂർണ്ണസംഖ്യ switchit);
സ്ഥിതം പൂർണ്ണസംഖ്യ is_zero_width(char_u *pattern, size_t patternlen, പൂർണ്ണസംഖ്യ move, pos_T *cur, പൂർണ്ണസംഖ്യ direction);
സ്ഥിതം ശൂന്യം cmdline_തിരയൽ_stat(പൂർണ്ണസംഖ്യ dirc, pos_T *pos, pos_T *cursor_pos, പൂർണ്ണസംഖ്യ show_മുകൾ_bot_സന്ദേശം, char_u *msgbuf, size_t msgbuflen, പൂർണ്ണസംഖ്യ recompute, പൂർണ്ണസംഖ്യ maxcount, long timeout);
സ്ഥിതം ശൂന്യം update_തിരയൽ_stat(പൂർണ്ണസംഖ്യ dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, പൂർണ്ണസംഖ്യ recompute, പൂർണ്ണസംഖ്യ maxcount, long timeout);

#define തിരയൽ_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define തിരയൽ_STAT_BUF_LEN 16

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
സ്ഥിതം spat_T spats[2] =
{
    {ശൂന്യ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ശൂന്യ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

സ്ഥിതം പൂർണ്ണസംഖ്യ last_idx = 0;	// index in spats[] for RE_LAST

സ്ഥിതം char_u lastc[2] = {NUL, NUL};	// last character searched for
സ്ഥിതം പൂർണ്ണസംഖ്യ lastcdir = FORWARD;		// last direction of character search
സ്ഥിതം പൂർണ്ണസംഖ്യ last_t_cmd = TRUE;		// last search t_cmd
സ്ഥിതം char_u	lastc_bytes[MB_MAXBYTES + 1];
സ്ഥിതം പൂർണ്ണസംഖ്യ	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
സ്ഥിതം spat_T	    saved_spats[ARRAY_LENGTH(spats)];
സ്ഥിതം char_u	    *saved_mr_pattern = ശൂന്യ;
സ്ഥിതം size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_തിരയൽ_EXTRA
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_spats_last_idx = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
സ്ഥിതം char_u	    *mr_pattern = ശൂന്യ;
സ്ഥിതം size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
തരനിർവചനം ഘടന SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    പൂർണ്ണസംഖ്യ		matched;	// Found a match in this file
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
    പൂർണ്ണസംഖ്യ
തിരയൽ_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    പൂർണ്ണസംഖ്യ		pat_save,
    പൂർണ്ണസംഖ്യ		pat_use,
    പൂർണ്ണസംഖ്യ		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    പൂർണ്ണസംഖ്യ		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    എങ്കിൽ (pat == ശൂന്യ || *pat == NUL)
    {
	പൂർണ്ണസംഖ്യ i;

	എങ്കിൽ (pat_use == RE_LAST)
	    i = last_idx;
	അല്ലെങ്കിൽ
	    i = pat_use;
	എങ്കിൽ (spats[i].pat == ശൂന്യ)	// pattern was never defined
	{
	    എങ്കിൽ (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    അല്ലെങ്കിൽ
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    തിരികെ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (options & തിരയൽ_HIS)	// put new pattern in history
	add_to_history(HIST_തിരയൽ, pat, patlen, TRUE, NUL);

    എങ്കിൽ (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    അല്ലെങ്കിൽ
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    എങ്കിൽ (mr_pattern == ശൂന്യ)
	mr_patternlen = 0;
    അല്ലെങ്കിൽ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    എങ്കിൽ (!(options & തിരയൽ_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	എങ്കിൽ (pat_save == RE_തിരയൽ || pat_save == RE_BOTH)
	    save_re_pat(RE_തിരയൽ, pat, patlen, magic);
	// substitute or global command
	എങ്കിൽ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    എങ്കിൽ (regmatch->regprog == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_തിരയൽ_pat(ശൂന്യം)
{
    തിരികെ mr_pattern;
}

    ശൂന്യം
save_re_pat(പൂർണ്ണസംഖ്യ idx, char_u *pat, size_t patlen, പൂർണ്ണസംഖ്യ magic)
{
    എങ്കിൽ (spats[idx].pat == pat)
	തിരികെ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    എങ്കിൽ (spats[idx].pat == ശൂന്യ)
	spats[idx].patlen = 0;
    അല്ലെങ്കിൽ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_തിരയൽ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    എങ്കിൽ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ save_level = 0;

    ശൂന്യം
save_തിരയൽ_patterns(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (save_level++ != 0)
	തിരികെ;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	എങ്കിൽ (spats[i].pat != ശൂന്യ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    എങ്കിൽ (saved_spats[i].pat == ശൂന്യ)
		saved_spats[i].patlen = 0;
	    അല്ലെങ്കിൽ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    എങ്കിൽ (mr_pattern == ശൂന്യ)
	saved_mr_pattern = ശൂന്യ;
    അല്ലെങ്കിൽ
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    എങ്കിൽ (saved_mr_pattern == ശൂന്യ)
	saved_mr_patternlen = 0;
    അല്ലെങ്കിൽ
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_തിരയൽ_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    ശൂന്യം
restore_തിരയൽ_patterns(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (--save_level != 0)
	തിരികെ;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#എങ്കിൽ defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_തിരയൽ_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_തിരയൽ_patterns(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_തിരയൽ_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
സ്ഥിതം spat_T	    saved_last_തിരയൽ_spat;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    did_save_last_തിരയൽ_spat = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_last_idx = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_no_hlsearch = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_തിരയൽ_match_endcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    saved_തിരയൽ_match_വരികൾ;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    ശൂന്യം
save_last_തിരയൽ_pattern(ശൂന്യം)
{
    എങ്കിൽ (++did_save_last_തിരയൽ_spat != 1)
	// nested call, nothing to do
	തിരികെ;

    saved_last_തിരയൽ_spat = spats[RE_തിരയൽ];
    എങ്കിൽ (spats[RE_തിരയൽ].pat != ശൂന്യ)
    {
	saved_last_തിരയൽ_spat.pat = vim_strnsave(spats[RE_തിരയൽ].pat, spats[RE_തിരയൽ].patlen);
	എങ്കിൽ (saved_last_തിരയൽ_spat.pat == ശൂന്യ)
	    saved_last_തിരയൽ_spat.patlen = 0;
	അല്ലെങ്കിൽ
	    saved_last_തിരയൽ_spat.patlen = spats[RE_തിരയൽ].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    ശൂന്യം
restore_last_തിരയൽ_pattern(ശൂന്യം)
{
    എങ്കിൽ (--did_save_last_തിരയൽ_spat > 0)
	// nested call, nothing to do
	തിരികെ;
    എങ്കിൽ (did_save_last_തിരയൽ_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	തിരികെ;
    }

    vim_free(spats[RE_തിരയൽ].pat);
    spats[RE_തിരയൽ] = saved_last_തിരയൽ_spat;
    saved_last_തിരയൽ_spat.pat = ശൂന്യ;
    saved_last_തിരയൽ_spat.patlen = 0;
# എങ്കിൽ defined(FEAT_EVAL)
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
    സ്ഥിതം ശൂന്യം
save_incsearch_state(ശൂന്യം)
{
    saved_തിരയൽ_match_endcol = തിരയൽ_match_endcol;
    saved_തിരയൽ_match_വരികൾ  = തിരയൽ_match_വരികൾ;
}

    സ്ഥിതം ശൂന്യം
restore_incsearch_state(ശൂന്യം)
{
    തിരയൽ_match_endcol = saved_തിരയൽ_match_endcol;
    തിരയൽ_match_വരികൾ  = saved_തിരയൽ_match_വരികൾ;
}

    char_u *
last_തിരയൽ_pattern(ശൂന്യം)
{
    തിരികെ spats[RE_തിരയൽ].pat;
}

    size_t
last_തിരയൽ_pattern_len(ശൂന്യം)
{
    തിരികെ spats[RE_തിരയൽ].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    പൂർണ്ണസംഖ്യ
ignorecase(char_u *pat)
{
    തിരികെ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    പൂർണ്ണസംഖ്യ
ignorecase_opt(char_u *pat, പൂർണ്ണസംഖ്യ ic_in, പൂർണ്ണസംഖ്യ scs)
{
    പൂർണ്ണസംഖ്യ		ic = ic_in;

    എങ്കിൽ (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    തിരികെ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    പൂർണ്ണസംഖ്യ
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (ശൂന്യം)skip_regexp_ex(pat, NUL, magic_isset(), ശൂന്യ, ശൂന്യ, &magic_val);

    ആകുമ്പോൾ (*p != NUL)
    {
	പൂർണ്ണസംഖ്യ		l;

	എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    എങ്കിൽ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		തിരികെ TRUE;
	    p += l;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    എങ്കിൽ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    അല്ലെങ്കിൽ എങ്കിൽ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    അല്ലെങ്കിൽ എങ്കിൽ (p[1] != NUL)  // skip "\X"
		p += 2;
	    അല്ലെങ്കിൽ
		p += 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    എങ്കിൽ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    അല്ലെങ്കിൽ
		p++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (MB_ISUPPER(*p))
	    തിരികെ TRUE;
	അല്ലെങ്കിൽ
	    ++p;
    }
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_EVAL)
    char_u *
last_csearch(ശൂന്യം)
{
    തിരികെ lastc_bytes;
}

    പൂർണ്ണസംഖ്യ
last_csearch_forward(ശൂന്യം)
{
    തിരികെ lastcdir == FORWARD;
}

    പൂർണ്ണസംഖ്യ
last_csearch_until(ശൂന്യം)
{
    തിരികെ last_t_cmd == TRUE;
}

    ശൂന്യം
set_last_csearch(പൂർണ്ണസംഖ്യ c, char_u *s, പൂർണ്ണസംഖ്യ len)
{
    *lastc = c;
    lastc_bytelen = len;
    എങ്കിൽ (len)
	memcpy(lastc_bytes, s, len);
    അല്ലെങ്കിൽ
	CLEAR_FIELD(lastc_bytes);
}
#endif

    ശൂന്യം
set_csearch_direction(പൂർണ്ണസംഖ്യ cdir)
{
    lastcdir = cdir;
}

    ശൂന്യം
set_csearch_until(പൂർണ്ണസംഖ്യ t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_തിരയൽ_pat(ശൂന്യം)
{
    തിരികെ spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    ശൂന്യം
reset_തിരയൽ_dir(ശൂന്യം)
{
    spats[0].off.dir = '/';
#എങ്കിൽ defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    ശൂന്യം
set_last_തിരയൽ_pat(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		magic,
    പൂർണ്ണസംഖ്യ		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    എങ്കിൽ (*s == NUL)
	spats[idx].pat = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    എങ്കിൽ (spats[idx].pat == ശൂന്യ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# എങ്കിൽ defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.വരി = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    എങ്കിൽ (setlast)
	last_idx = idx;
    എങ്കിൽ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	എങ്കിൽ (spats[idx].pat == ശൂന്യ)
	    saved_spats[idx].pat = ശൂന്യ;
	അല്ലെങ്കിൽ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	എങ്കിൽ (saved_spats[idx].pat == ശൂന്യ)
	    saved_spats[idx].patlen = 0;
	അല്ലെങ്കിൽ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_തിരയൽ_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_തിരയൽ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    എങ്കിൽ (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_തിരയൽ_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    ശൂന്യം
last_pat_prog(regmmatch_T *regmatch)
{
    എങ്കിൽ (spats[last_idx].pat == ശൂന്യ)
    {
	regmatch->regprog = ശൂന്യ;
	തിരികെ;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (ശൂന്യം)തിരയൽ_regcomp((char_u *)"", 0, ശൂന്യ, 0, last_idx, തിരയൽ_KEEP, regmatch);
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
    പൂർണ്ണസംഖ്യ
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    പൂർണ്ണസംഖ്യ		dir,
    char_u	*pat,
    size_t	patlen,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		options,
    പൂർണ്ണസംഖ്യ		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    പൂർണ്ണസംഖ്യ		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    പൂർണ്ണസംഖ്യ		loop;
    pos_T	start_pos;
    പൂർണ്ണസംഖ്യ		at_first_വരി;
    പൂർണ്ണസംഖ്യ		extra_col;
    പൂർണ്ണസംഖ്യ		start_char_len;
    പൂർണ്ണസംഖ്യ		match_ok;
    long	nmatched;
    പൂർണ്ണസംഖ്യ		submatch = 0;
    പൂർണ്ണസംഖ്യ		first_match = TRUE;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
#ifdef FEAT_തിരയൽ_EXTRA
    പൂർണ്ണസംഖ്യ		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    പൂർണ്ണസംഖ്യ		unused_timeout_flag = FALSE;
    പൂർണ്ണസംഖ്യ		*timed_out = &unused_timeout_flag;  // set when timed out.
    പൂർണ്ണസംഖ്യ		തിരയൽ_from_match_end;		    // vi-compatible search?

    എങ്കിൽ (തിരയൽ_regcomp(pat, patlen, ശൂന്യ, RE_തിരയൽ, pat_use,
		   (options & (തിരയൽ_HIS + തിരയൽ_KEEP)), &regmatch) == FAIL)
    {
	എങ്കിൽ ((options & തിരയൽ_സന്ദേശം) && !rc_did_emsg)
	    semsg(_(e_invalid_തിരയൽ_string_str), mr_pattern);
	തിരികെ FAIL;
    }

    തിരയൽ_from_match_end = vim_strchr(p_cpo, CPO_തിരയൽ) != ശൂന്യ;

    എങ്കിൽ (extra_arg != ശൂന്യ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	എങ്കിൽ (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    ചെയ്യുക	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	എങ്കിൽ (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_വരി_എണ്ണം
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    എങ്കിൽ (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    അല്ലെങ്കിൽ
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	അല്ലെങ്കിൽ
	    start_char_len = 1;
	എങ്കിൽ (dir == FORWARD)
	{
	    എങ്കിൽ (options & തിരയൽ_START)
		extra_col = 0;
	    അല്ലെങ്കിൽ
		extra_col = start_char_len;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (options & തിരയൽ_START)
		extra_col = start_char_len;
	    അല്ലെങ്കിൽ
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_വരി = TRUE;	// default: start in first line
	എങ്കിൽ (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_വരി = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	എങ്കിൽ (dir == BACKWARD && start_pos.col == 0
					     && (options & തിരയൽ_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_വരി = FALSE;
	}
	അല്ലെങ്കിൽ
	    lnum = pos->lnum;

	വേണ്ടി (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    വേണ്ടി ( ; lnum > 0 && lnum <= buf->b_ml.ml_വരി_എണ്ണം;
					   lnum += dir, at_first_വരി = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		എങ്കിൽ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    നിർത്തുക;
		// Stop after passing the time limit.
		എങ്കിൽ (*timed_out)
		    നിർത്തുക;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_വരി && (options & തിരയൽ_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		    നിർത്തുക;
		// Abort searching on an error (e.g., out of stack).
		എങ്കിൽ (called_emsg > called_emsg_before || *timed_out)
		    നിർത്തുക;
		എങ്കിൽ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    എങ്കിൽ (lnum + matchpos.lnum > buf->b_ml.ml_വരി_എണ്ണം)
			ptr = (char_u *)"";
		    അല്ലെങ്കിൽ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    എങ്കിൽ (dir == FORWARD && at_first_വരി)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ആകുമ്പോൾ (matchpos.lnum == 0
				&& ((options & തിരയൽ_END) && first_match
				    ?  (nmatched == 1
					&& (പൂർണ്ണസംഖ്യ)endpos.col - 1
					     < (പൂർണ്ണസംഖ്യ)start_pos.col + extra_col)
				    : ((പൂർണ്ണസംഖ്യ)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (പൂർണ്ണസംഖ്യ)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    എങ്കിൽ (തിരയൽ_from_match_end)
			    {
				എങ്കിൽ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    നിർത്തുക;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				എങ്കിൽ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    എങ്കിൽ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    അല്ലെങ്കിൽ
					++matchcol;
				}
			    }
			    അല്ലെങ്കിൽ
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				എങ്കിൽ (ptr[matchcol] != NUL)
				{
				    എങ്കിൽ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    അല്ലെങ്കിൽ
					++matchcol;
				}
			    }
			    എങ്കിൽ (matchcol == 0 && (options & തിരയൽ_START))
				നിർത്തുക;
			    എങ്കിൽ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				നിർത്തുക;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
				നിർത്തുക;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			എങ്കിൽ (!match_ok)
			    തുടരുക;
		    }
		    എങ്കിൽ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			വേണ്ടി (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    എങ്കിൽ (loop
				|| ((options & തിരയൽ_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (പൂർണ്ണസംഖ്യ)regmatch.endpos[0].col - 1
							< (പൂർണ്ണസംഖ്യ)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (പൂർണ്ണസംഖ്യ)regmatch.startpos[0].col
						      < (പൂർണ്ണസംഖ്യ)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    അല്ലെങ്കിൽ
				നിർത്തുക;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    എങ്കിൽ (തിരയൽ_from_match_end)
			    {
				എങ്കിൽ (nmatched > 1)
				    നിർത്തുക;
				matchcol = endpos.col;
				// for empty match: advance one char
				എങ്കിൽ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    എങ്കിൽ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    അല്ലെങ്കിൽ
					++matchcol;
				}
			    }
			    അല്ലെങ്കിൽ
			    {
				// Stop when the match is in a next line.
				എങ്കിൽ (matchpos.lnum > 0)
				    നിർത്തുക;
				matchcol = matchpos.col;
				എങ്കിൽ (ptr[matchcol] != NUL)
				{
				    എങ്കിൽ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    അല്ലെങ്കിൽ
					++matchcol;
				}
			    }
			    എങ്കിൽ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				എങ്കിൽ (*timed_out)
				    match_ok = FALSE;
				നിർത്തുക;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
				നിർത്തുക;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			എങ്കിൽ (!match_ok)
			    തുടരുക;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    എങ്കിൽ ((options & തിരയൽ_END) && !(options & തിരയൽ_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			എങ്കിൽ (endpos.col == 0)
			{
			    എങ്കിൽ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			അല്ലെങ്കിൽ
			{
			    --pos->col;
			    എങ്കിൽ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_വരി_എണ്ണം)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			എങ്കിൽ (end_pos != ശൂന്യ)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			എങ്കിൽ (end_pos != ശൂന്യ)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    എങ്കിൽ (end_pos != ശൂന്യ)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    തിരയൽ_match_വരികൾ = endpos.lnum - matchpos.lnum;
		    തിരയൽ_match_endcol = endpos.col;
		    നിർത്തുക;
		}
		വരി_breakcheck();	// stop if ctrl-C typed
		എങ്കിൽ (got_int)
		    നിർത്തുക;

#ifdef FEAT_തിരയൽ_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		എങ്കിൽ ((options & തിരയൽ_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    നിർത്തുക;
		}
#endif

		എങ്കിൽ (loop && lnum == start_pos.lnum)
		    നിർത്തുക;	    // if second loop, stop where started
	    }
	    at_first_വരി = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		നിർത്തുക;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    എങ്കിൽ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_തിരയൽ_EXTRA
			      || break_loop
#endif
			      || found || loop)
		നിർത്തുക;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    എങ്കിൽ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_വരി_എണ്ണം;
	    അല്ലെങ്കിൽ
		lnum = 1;
	    എങ്കിൽ (!shortmess(SHM_തിരയൽ)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & തിരയൽ_സന്ദേശം))
		give_warning((char_u *)_(dir == BACKWARD
					  ? മുകൾ_bot_സന്ദേശം : bot_മുകൾ_സന്ദേശം), TRUE);
	    എങ്കിൽ (extra_arg != ശൂന്യ)
		extra_arg->sa_wrapped = TRUE;
	}
	എങ്കിൽ (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_തിരയൽ_EXTRA
		|| break_loop
#endif
		)
	    നിർത്തുക;
    }
    ആകുമ്പോൾ (--എണ്ണം > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    എങ്കിൽ (extra_arg != ശൂന്യ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    എങ്കിൽ (!found)		    // did not find it
    {
	എങ്കിൽ (got_int)
	    emsg(_(e_interrupted));
	അല്ലെങ്കിൽ എങ്കിൽ ((options & തിരയൽ_സന്ദേശം) == തിരയൽ_സന്ദേശം)
	{
	    എങ്കിൽ (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    അല്ലെങ്കിൽ എങ്കിൽ (lnum == 0)
		semsg(_(e_തിരയൽ_hit_മുകൾ_without_match_for_str), mr_pattern);
	    അല്ലെങ്കിൽ
		semsg(_(e_തിരയൽ_hit_bottom_without_match_for_str), mr_pattern);
	}
	തിരികെ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    എങ്കിൽ (pos->lnum > buf->b_ml.ml_വരി_എണ്ണം)
    {
	pos->lnum = buf->b_ml.ml_വരി_എണ്ണം;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	എങ്കിൽ (pos->col > 0)
	    --pos->col;
    }

    തിരികെ submatch + 1;
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    ശൂന്യം
set_തിരയൽ_direction(പൂർണ്ണസംഖ്യ cdir)
{
    spats[0].off.dir = cdir;
}

    സ്ഥിതം ശൂന്യം
set_vv_searchforward(ശൂന്യം)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
first_submatch(regmmatch_T *rp)
{
    പൂർണ്ണസംഖ്യ		submatch;

    വേണ്ടി (submatch = 1; ; ++submatch)
    {
	എങ്കിൽ (rp->startpos[submatch].lnum >= 0)
	    നിർത്തുക;
	എങ്കിൽ (submatch == 9)
	{
	    submatch = 0;
	    നിർത്തുക;
	}
    }
    തിരികെ submatch;
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
    പൂർണ്ണസംഖ്യ
parse_തിരയൽ_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    പൂർണ്ണസംഖ്യ		തിരയൽ_delim,
    പൂർണ്ണസംഖ്യ		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    പൂർണ്ണസംഖ്യ		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    എങ്കിൽ (*pat == ശൂന്യ || **pat == NUL)
	തിരികെ 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ശൂന്യ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, തിരയൽ_delim, magic_isset(),
						    strcopy, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (പൂർണ്ണസംഖ്യ)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    എങ്കിൽ (*p == തിരയൽ_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->വരി = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    എങ്കിൽ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->വരി = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ ((options & തിരയൽ_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	എങ്കിൽ (*p == 'e')		// end
	    offset->end = തിരയൽ_END;
	++p;
    }
    എങ്കിൽ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	എങ്കിൽ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((അക്ഷരം *)p);
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '-')	    // single '-'
	    offset->off = -1;
	അല്ലെങ്കിൽ			    // single '+'
	    offset->off = 1;
	++p;
	ആകുമ്പോൾ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (പൂർണ്ണസംഖ്യ)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    തിരികെ cmdlen;
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
    പൂർണ്ണസംഖ്യ
do_തിരയൽ(
    oparg_T	    *oap,	// can be NULL
    പൂർണ്ണസംഖ്യ		    dirc,	// '/' or '?'
    പൂർണ്ണസംഖ്യ		    തിരയൽ_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    എണ്ണം,
    പൂർണ്ണസംഖ്യ		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    പൂർണ്ണസംഖ്യ		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    show_തിരയൽ_stats;
    char_u	    *msgbuf = ശൂന്യ;
    size_t	    msgbuflen = 0;
    പൂർണ്ണസംഖ്യ		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    എങ്കിൽ (spats[0].off.വരി && vim_strchr(p_cpo, CPO_LINEOFF) != ശൂന്യ)
    {
	spats[0].off.വരി = FALSE;
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
    എങ്കിൽ (dirc == 0)
	dirc = spats[0].off.dir;
    അല്ലെങ്കിൽ
    {
	spats[0].off.dir = dirc;
#എങ്കിൽ defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    എങ്കിൽ (options & തിരയൽ_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#അല്ലെങ്കിൽ
	എങ്കിൽ (dirc == '/')
	    dirc = '?';
	അല്ലെങ്കിൽ
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    എങ്കിൽ (dirc == '/')
    {
	എങ്കിൽ (hasFolding(pos.lnum, ശൂന്യ, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (hasFolding(pos.lnum, &pos.lnum, ശൂന്യ))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_തിരയൽ_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    എങ്കിൽ (no_hlsearch && !(options & തിരയൽ_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ		show_മുകൾ_bot_സന്ദേശം = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ശൂന്യ;
					    // use previous pattern
	എങ്കിൽ (pat == ശൂന്യ || *pat == NUL || *pat == തിരയൽ_delim)
	{
	    എങ്കിൽ (spats[RE_തിരയൽ].pat == ശൂന്യ)	    // no previous pattern
	    {
		എങ്കിൽ (spats[RE_SUBST].pat == ശൂന്യ)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    പോകുക end_do_തിരയൽ;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    അല്ലെങ്കിൽ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	എങ്കിൽ (pat != ശൂന്യ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_തിരയൽ_pattern_offset(&pat, &patlen,
				തിരയൽ_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    എങ്കിൽ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_തിരയൽ_stats = FALSE;
	എങ്കിൽ ((options & തിരയൽ_ECHO) && messaging()
		&& !സന്ദേശം_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    സന്ദേശം_start();

	    // Get the offset, so we know how long it is.
	    എങ്കിൽ (!cmd_silent &&
		    (spats[0].off.വരി || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		എങ്കിൽ (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		അല്ലെങ്കിൽ എങ്കിൽ (!spats[0].off.വരി)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		എങ്കിൽ (spats[0].off.off != 0 || spats[0].off.വരി)
		    off_len += vim_snprintf((അക്ഷരം *)off_buf + off_len,
			   വലുപ്പം(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    എങ്കിൽ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    അല്ലെങ്കിൽ
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    എങ്കിൽ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		എങ്കിൽ (സന്ദേശം_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (പൂർണ്ണസംഖ്യ)(Rows - സന്ദേശം_row) * cmdline_width - 1;
		അല്ലെങ്കിൽ
		    // Use up to 'showcmd' column.
		    msgbufsize = (പൂർണ്ണസംഖ്യ)(Rows - സന്ദേശം_row - 1) * cmdline_width
			+ sc_col - 1;
		എങ്കിൽ (msgbufsize < plen + off_len + തിരയൽ_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + തിരയൽ_STAT_BUF_LEN + 3;
	    }
	    അല്ലെങ്കിൽ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    എങ്കിൽ (msgbuf == ശൂന്യ)
	    {
		msgbuflen = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		എങ്കിൽ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    എങ്കിൽ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    അല്ലെങ്കിൽ
			mch_memmove(msgbuf + 1, p, plen);
		    എങ്കിൽ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = സന്ദേശം_strtrunc(msgbuf, TRUE);
		    എങ്കിൽ (trunc != ശൂന്യ)
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
		    എങ്കിൽ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			എങ്കിൽ (r != ശൂന്യ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ആകുമ്പോൾ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    എങ്കിൽ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    അല്ലെങ്കിൽ
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    സന്ദേശം_outtrans(msgbuf);
		    സന്ദേശം_clr_eos();
		    സന്ദേശം_check();

		    gotocmdline(FALSE);
		    out_flush();
		    സന്ദേശം_nowait = TRUE;	    // don't wait for this message
		}

		എങ്കിൽ (!shortmess(SHM_SEARCHCOUNT))
		    show_തിരയൽ_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	എങ്കിൽ (!spats[0].off.വരി && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    എങ്കിൽ (spats[0].off.off > 0)
	    {
		വേണ്ടി (c = spats[0].off.off; c; --c)
		    എങ്കിൽ (decl(&pos) == -1)
			നിർത്തുക;
		എങ്കിൽ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (c = spats[0].off.off; c; ++c)
		    എങ്കിൽ (incl(&pos) == -1)
			നിർത്തുക;
		എങ്കിൽ (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_വരി_എണ്ണം + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ശൂന്യ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, എണ്ണം, spats[0].off.end + (options &
		       (തിരയൽ_KEEP + തിരയൽ_PEEK + തിരയൽ_HIS
			+ തിരയൽ_സന്ദേശം + തിരയൽ_START
			+ ((pat != ശൂന്യ && *pat == ';') ? 0 : തിരയൽ_NOOF))),
		RE_LAST, sia);

	എങ്കിൽ (dircp != ശൂന്യ)
	    *dircp = തിരയൽ_delim; // restore second '/' or '?' for normal_cmd()

	എങ്കിൽ (!shortmess(SHM_തിരയൽ) && sia && sia->sa_wrapped)
	    show_മുകൾ_bot_സന്ദേശം = TRUE;

	എങ്കിൽ (c == FAIL)
	{
	    retval = 0;
	    പോകുക end_do_തിരയൽ;
	}
	എങ്കിൽ (spats[0].off.end && oap != ശൂന്യ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	എങ്കിൽ (!(options & തിരയൽ_NOOF) || (pat != ശൂന്യ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    എങ്കിൽ (spats[0].off.വരി)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		എങ്കിൽ (c < 1)
		    pos.lnum = 1;
		അല്ലെങ്കിൽ എങ്കിൽ (c > curbuf->b_ml.ml_വരി_എണ്ണം)
		    pos.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
		അല്ലെങ്കിൽ
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		എങ്കിൽ (c > 0)
		{
		    ആകുമ്പോൾ (c-- > 0)
			എങ്കിൽ (incl(&pos) == -1)
			    നിർത്തുക;
		}
		// to the left, check for start of file
		അല്ലെങ്കിൽ
		{
		    ആകുമ്പോൾ (c++ < 0)
			എങ്കിൽ (decl(&pos) == -1)
			    നിർത്തുക;
		}
	    }
	    എങ്കിൽ (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	എങ്കിൽ (show_തിരയൽ_stats)
	     cmdline_തിരയൽ_stat(dirc, &pos, &curwin->w_cursor,
				show_മുകൾ_bot_സന്ദേശം, msgbuf, msgbuflen,
				(എണ്ണം != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_തിരയൽ)
				     && hasFolding(curwin->w_cursor.lnum,
								   ശൂന്യ, ശൂന്യ))
#endif
				),
				p_msc,
				തിരയൽ_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	എങ്കിൽ (!(options & തിരയൽ_OPT) || pat == ശൂന്യ || *pat != ';')
	    നിർത്തുക;

	dirc = *++pat;
	--patlen;
	തിരയൽ_delim = dirc;
	എങ്കിൽ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    പോകുക end_do_തിരയൽ;
	}
	++pat;
	--patlen;
    }

    എങ്കിൽ (options & തിരയൽ_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_തിരയൽ:
    എങ്കിൽ ((options & തിരയൽ_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    തിരികെ retval;
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
    പൂർണ്ണസംഖ്യ
തിരയൽ_for_exact_വരി(
    buf_T	*buf,
    pos_T	*pos,
    പൂർണ്ണസംഖ്യ		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		compl_len = ins_compl_len();

    എങ്കിൽ (buf->b_ml.ml_വരി_എണ്ണം == 0)
	തിരികെ FAIL;
    വേണ്ടി (;;)
    {
	pos->lnum += dir;
	എങ്കിൽ (pos->lnum < 1)
	{
	    എങ്കിൽ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_വരി_എണ്ണം;
		എങ്കിൽ (!shortmess(SHM_തിരയൽ))
		    give_warning((char_u *)_(മുകൾ_bot_സന്ദേശം), TRUE);
	    }
	    അല്ലെങ്കിൽ
	    {
		pos->lnum = 1;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (pos->lnum > buf->b_ml.ml_വരി_എണ്ണം)
	{
	    എങ്കിൽ (p_ws)
	    {
		pos->lnum = 1;
		എങ്കിൽ (!shortmess(SHM_തിരയൽ))
		    give_warning((char_u *)_(bot_മുകൾ_സന്ദേശം), TRUE);
	    }
	    അല്ലെങ്കിൽ
	    {
		pos->lnum = 1;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (pos->lnum == start)
	    നിർത്തുക;
	എങ്കിൽ (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	എങ്കിൽ (compl_status_adding() && !compl_status_sol())
	{
	    എങ്കിൽ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		തിരികെ OK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    എങ്കിൽ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		തിരികെ OK;
	}
    }
    തിരികെ FAIL;
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
    പൂർണ്ണസംഖ്യ
searchc(cmdarg_T *cap, പൂർണ്ണസംഖ്യ t_cmd)
{
    പൂർണ്ണസംഖ്യ			c = cap->nchar;	// char to search for
    പൂർണ്ണസംഖ്യ			dir = cap->arg;	// TRUE for searching forward
    long		എണ്ണം = cap->count1;	// repeat count
    പൂർണ്ണസംഖ്യ			col;
    char_u		*p;
    പൂർണ്ണസംഖ്യ			len;
    പൂർണ്ണസംഖ്യ			stop = TRUE;

    എങ്കിൽ (c != NUL)	// normal search: remember args for repeat
    {
	എങ്കിൽ (!താക്കോൽ_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    എങ്കിൽ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		എങ്കിൽ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    അല്ലെങ്കിൽ		// repeat previous search
    {
	എങ്കിൽ (*lastc == NUL && lastc_bytelen <= 1)
	    തിരികെ FAIL;
	എങ്കിൽ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	അല്ലെങ്കിൽ
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	എങ്കിൽ (vim_strchr(p_cpo, CPO_SCOLON) == ശൂന്യ && എണ്ണം == 1 && t_cmd)
	    stop = FALSE;
    }

    എങ്കിൽ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    അല്ലെങ്കിൽ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    ആകുമ്പോൾ (എണ്ണം--)
    {
	എങ്കിൽ (has_mbyte)
	{
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    എങ്കിൽ (col >= len)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (col == 0)
			തിരികെ FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		എങ്കിൽ (lastc_bytelen <= 1)
		{
		    എങ്കിൽ (p[col] == c && stop)
			നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    നിർത്തുക;
		stop = TRUE;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ ((col += dir) < 0 || col >= len)
		    തിരികെ FAIL;
		എങ്കിൽ (p[col] == c && stop)
		    നിർത്തുക;
		stop = TRUE;
	    }
	}
    }

    എങ്കിൽ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	എങ്കിൽ (has_mbyte)
	{
	    എങ്കിൽ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    അല്ലെങ്കിൽ
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    തിരികെ OK;
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
findmatch(oparg_T *oap, പൂർണ്ണസംഖ്യ initc)
{
    തിരികെ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_prevcol(
    char_u	*linep,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		ch,
    പൂർണ്ണസംഖ്യ		*prevcol)
{
    --col;
    എങ്കിൽ (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    എങ്കിൽ (prevcol)
	*prevcol = col;
    തിരികെ (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		found = FALSE;

    വേണ്ടി (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    എങ്കിൽ (delim_copy == ശൂന്യ)
	തിരികെ FALSE;
    വേണ്ടി (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *വരി = ml_get(lnum);

	വേണ്ടി (p = വരി + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    എങ്കിൽ (lnum == endpos->lnum && (colnr_T)(p - വരി) >= endpos->col)
		നിർത്തുക;
	    എങ്കിൽ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (found)
	    നിർത്തുക;
    }
    vim_free(delim_copy);
    തിരികെ found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    സ്ഥിതം ശൂന്യം
find_mps_മൂല്യങ്ങൾ(
    പൂർണ്ണസംഖ്യ	    *initc,
    പൂർണ്ണസംഖ്യ	    *findc,
    പൂർണ്ണസംഖ്യ	    *backwards,
    പൂർണ്ണസംഖ്യ	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ആകുമ്പോൾ (*ptr != NUL)
    {
	എങ്കിൽ (has_mbyte)
	{
	    char_u *prev;

	    എങ്കിൽ (mb_ptr2char(ptr) == *initc)
	    {
		എങ്കിൽ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		തിരികെ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    എങ്കിൽ (mb_ptr2char(ptr) == *initc)
	    {
		എങ്കിൽ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		അല്ലെങ്കിൽ
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		തിരികെ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*ptr == *initc)
	    {
		എങ്കിൽ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		അല്ലെങ്കിൽ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		തിരികെ;
	    }
	    ptr += 2;
	    എങ്കിൽ (*ptr == *initc)
	    {
		എങ്കിൽ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		അല്ലെങ്കിൽ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		തിരികെ;
	    }
	    ++ptr;
	}
	എങ്കിൽ (*ptr == ',')
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
    പൂർണ്ണസംഖ്യ		initc,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		maxtravel)
{
    സ്ഥിതം pos_T pos;			// current search position
    പൂർണ്ണസംഖ്യ		findc = 0;		// matching brace
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;		// cumulative number of braces
    പൂർണ്ണസംഖ്യ		backwards = FALSE;	// init for gcc
    പൂർണ്ണസംഖ്യ		raw_string = FALSE;	// search for raw string
    പൂർണ്ണസംഖ്യ		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		do_quotes;		// check for quotes in current line
    പൂർണ്ണസംഖ്യ		at_start;		// do_quotes value at start position
    പൂർണ്ണസംഖ്യ		hash_dir = 0;		// Direction searched for # things
    പൂർണ്ണസംഖ്യ		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    പൂർണ്ണസംഖ്യ		start_in_quotes;	// start position is in quotes
    പൂർണ്ണസംഖ്യ		traveled = 0;		// how far we've searched so far
    പൂർണ്ണസംഖ്യ		ignore_cend = FALSE;    // ignore comment end
    പൂർണ്ണസംഖ്യ		cpo_match;		// vi compatible matching
    പൂർണ്ണസംഖ്യ		cpo_bsl;		// don't recognize backslashes
    പൂർണ്ണസംഖ്യ		match_escaped = 0;	// search for escaped match
    പൂർണ്ണസംഖ്യ		dir;			// Direction to search
    പൂർണ്ണസംഖ്യ		comment_col = MAXCOL;   // start of / / comment
    പൂർണ്ണസംഖ്യ		lispcomm = FALSE;	// inside of Lisp-style comment
    പൂർണ്ണസംഖ്യ		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    പൂർണ്ണസംഖ്യ		skip_comments = (flags & FM_SKIPCOMM) != 0;
    പൂർണ്ണസംഖ്യ		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != ശൂന്യ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ശൂന്യ);

    // Direction to search when initc is '/', '*' or '#'
    എങ്കിൽ (flags & FM_BACKWARD)
	dir = BACKWARD;
    അല്ലെങ്കിൽ എങ്കിൽ (flags & FM_FORWARD)
	dir = FORWARD;
    അല്ലെങ്കിൽ
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    എങ്കിൽ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	എങ്കിൽ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (initc != '#' && initc != NUL)
    {
	find_mps_മൂല്യങ്ങൾ(&initc, &findc, &backwards, TRUE);
	എങ്കിൽ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	എങ്കിൽ (findc == NUL)
	    തിരികെ ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	എങ്കിൽ (initc == '#')
	{
	    hash_dir = dir;
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    എങ്കിൽ (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		എങ്കിൽ (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    എങ്കിൽ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col] == '/')
		{
		    എങ്കിൽ (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col] == '*')
		{
		    എങ്കിൽ (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    എങ്കിൽ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		എങ്കിൽ (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		വേണ്ടി (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    എങ്കിൽ (initc == NUL)
			നിർത്തുക;

		    find_mps_മൂല്യങ്ങൾ(&initc, &findc, &backwards, FALSE);
		    എങ്കിൽ (findc)
			നിർത്തുക;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		എങ്കിൽ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    എങ്കിൽ (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    അല്ലെങ്കിൽ
			തിരികെ ശൂന്യ;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!cpo_bsl)
		{
		    പൂർണ്ണസംഖ്യ col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    വേണ്ടി (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	എങ്കിൽ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    എങ്കിൽ (oap != ശൂന്യ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    എങ്കിൽ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		എങ്കിൽ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		അല്ലെങ്കിൽ
		    തിരികെ ശൂന്യ;
	    }
	    pos.col = 0;
	    ആകുമ്പോൾ (!got_int)
	    {
		എങ്കിൽ (hash_dir > 0)
		{
		    എങ്കിൽ (pos.lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
			നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (pos.lnum == 1)
		    നിർത്തുക;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		വരി_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		എങ്കിൽ (*ptr != '#')
		    തുടരുക;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		എങ്കിൽ (hash_dir > 0)
		{
		    എങ്കിൽ (STRNCMP(ptr, "if", 2) == 0)
			എണ്ണം++;
		    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(ptr, "el", 2) == 0)
		    {
			എങ്കിൽ (എണ്ണം == 0)
			    തിരികെ &pos;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			എങ്കിൽ (എണ്ണം == 0)
			    തിരികെ &pos;
			എണ്ണം--;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (STRNCMP(ptr, "if", 2) == 0)
		    {
			എങ്കിൽ (എണ്ണം == 0)
			    തിരികെ &pos;
			എണ്ണം--;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			എങ്കിൽ (എണ്ണം == 0)
			    തിരികെ &pos;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(ptr, "endif", 5) == 0)
			എണ്ണം++;
		}
	    }
	    തിരികെ ശൂന്യ;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    എങ്കിൽ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ശൂന്യ)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    എങ്കിൽ ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    എങ്കിൽ (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    എങ്കിൽ (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    ആകുമ്പോൾ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	എങ്കിൽ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    എങ്കിൽ (lispcomm && pos.col < (colnr_T)comment_col)
		നിർത്തുക;
	    എങ്കിൽ (pos.col == 0)		// at start of line, go to prev. one
	    {
		എങ്കിൽ (pos.lnum == 1)	// start of file
		    നിർത്തുക;
		--pos.lnum;

		എങ്കിൽ (maxtravel > 0 && ++traveled > maxtravel)
		    നിർത്തുക;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		വരി_breakcheck();

		// Check if this line contains a single-line comment
		എങ്കിൽ (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		എങ്കിൽ (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		അല്ലെങ്കിൽ എങ്കിൽ (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    അല്ലെങ്കിൽ
	    {
		--pos.col;
		എങ്കിൽ (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	അല്ലെങ്കിൽ				// forward search
	{
	    എങ്കിൽ (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		എങ്കിൽ (pos.lnum == curbuf->b_ml.ml_വരി_എണ്ണം  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    നിർത്തുക;
		++pos.lnum;

		എങ്കിൽ (maxtravel && traveled++ > maxtravel)
		    നിർത്തുക;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		വരി_breakcheck();
		എങ്കിൽ (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		അല്ലെങ്കിൽ
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	എങ്കിൽ (skip_comments && !comment_dir && !inquote)
	{
	    എങ്കിൽ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		എങ്കിൽ (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (പൂർണ്ണസംഖ്യ)pos.col < comment_col))
		    in_block_comment = TRUE;
		അല്ലെങ്കിൽ എങ്കിൽ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		എങ്കിൽ (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (പൂർണ്ണസംഖ്യ)pos.col < comment_col))
		    in_block_comment = TRUE;
		അല്ലെങ്കിൽ എങ്കിൽ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	എങ്കിൽ (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    എങ്കിൽ (linep[0] == findc && എണ്ണം == 0)	// match!
		തിരികെ &pos;
	    നിർത്തുക;					// out of scope
	}

	എങ്കിൽ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    എങ്കിൽ (comment_dir == FORWARD)
	    {
		എങ്കിൽ (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    തിരികെ &pos;
		}
	    }
	    അല്ലെങ്കിൽ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		എങ്കിൽ (pos.col == 0)
		    തുടരുക;
		അല്ലെങ്കിൽ എങ്കിൽ (raw_string)
		{
		    എങ്കിൽ (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != ശൂന്യ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			എങ്കിൽ (!find_rawstring_end(linep, &pos,
				  എണ്ണം > 0 ? &match_pos : &curwin->w_cursor))
			{
			    എണ്ണം++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (പൂർണ്ണസംഖ്യ)pos.col < comment_col)
		{
		    എണ്ണം++;
		    match_pos = pos;
		    match_pos.col--;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    എങ്കിൽ (എണ്ണം > 0)
			pos = match_pos;
		    അല്ലെങ്കിൽ എങ്കിൽ (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (പൂർണ്ണസംഖ്യ)pos.col <= comment_col)
			pos.col -= 2;
		    അല്ലെങ്കിൽ എങ്കിൽ (ignore_cend)
			തുടരുക;
		    അല്ലെങ്കിൽ
			തിരികെ ശൂന്യ;
		    തിരികെ &pos;
		}
	    }
	    തുടരുക;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	എങ്കിൽ (cpo_match)
	    do_quotes = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    വേണ്ടി (ptr = linep; *ptr; ++ptr)
	    {
		എങ്കിൽ (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		എങ്കിൽ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		എങ്കിൽ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    എങ്കിൽ (!do_quotes)
	    {
		inquote = FALSE;
		എങ്കിൽ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    എങ്കിൽ (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			എങ്കിൽ (inquote)
			    start_in_quotes = TRUE;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (backwards)
			inquote = TRUE;
		}
		എങ്കിൽ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    എങ്കിൽ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			എങ്കിൽ (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    എങ്കിൽ (inquote)
				start_in_quotes = TRUE;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	എങ്കിൽ (start_in_quotes == MAYBE)
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
	തിരഞ്ഞെടുപ്പ് (c)
	{
	അവസ്ഥ NUL:
	    // at end of line without trailing backslash, reset inquote
	    എങ്കിൽ (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    നിർത്തുക;

	അവസ്ഥ '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    എങ്കിൽ (do_quotes)
	    {
		പൂർണ്ണസംഖ്യ col;

		വേണ്ടി (col = pos.col - 1; col >= 0; --col)
		    എങ്കിൽ (linep[col] != '\\')
			നിർത്തുക;
		എങ്കിൽ ((((പൂർണ്ണസംഖ്യ)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    നിർത്തുക;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	അവസ്ഥ '\'':
	    എങ്കിൽ (!cpo_match && initc != '\'' && findc != '\'')
	    {
		എങ്കിൽ (backwards)
		{
		    എങ്കിൽ (pos.col > 1)
		    {
			എങ്കിൽ (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    നിർത്തുക;
			}
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col + 1])	// forward search
		{
		    എങ്കിൽ (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			നിർത്തുക;
		    }
		}
	    }
	    // FALLTHROUGH

	സ്ഥിരസ്ഥിതി:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    എങ്കിൽ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ശൂന്യ
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', ശൂന്യ)
		    && check_prevcol(linep, pos.col - 1, '#', ശൂന്യ))
		നിർത്തുക;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    എങ്കിൽ (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (പൂർണ്ണസംഖ്യ)pos.col >= comment_col)))
		നിർത്തുക;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    എങ്കിൽ ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		പൂർണ്ണസംഖ്യ	col, bslcnt = 0;

		എങ്കിൽ (!cpo_bsl)
		{
		    വേണ്ടി (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		എങ്കിൽ (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    എങ്കിൽ (c == initc)
			എണ്ണം++;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (എണ്ണം == 0)
			    തിരികെ &pos;
			എണ്ണം--;
		    }
		}
	    }
	}
    }

    എങ്കിൽ (comment_dir == BACKWARD && എണ്ണം > 0)
    {
	pos = match_pos;
	തിരികെ &pos;
    }
    തിരികെ (pos_T *)ശൂന്യ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    ശൂന്യം
showmatch(
    പൂർണ്ണസംഖ്യ		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    പൂർണ്ണസംഖ്യ		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    വേണ്ടി (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    നിർത്തുക;
#endif
	p += mb_ptr2len(p) + 1;
	എങ്കിൽ (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    നിർത്തുക;
	p += mb_ptr2len(p);
	എങ്കിൽ (*p == NUL)
	    തിരികെ;
    }
    എങ്കിൽ (*p == NUL)
	തിരികെ;

    എങ്കിൽ ((lpos = findmatch(ശൂന്യ, NUL)) == ശൂന്യ)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	തിരികെ;
    }

    എങ്കിൽ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	തിരികെ;

    എങ്കിൽ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ശൂന്യ, &vcol, ശൂന്യ, 0);

    പൂർണ്ണസംഖ്യ col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    എങ്കിൽ (!col_visible)
	തിരികെ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    എങ്കിൽ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    എങ്കിൽ (vim_strchr(p_cpo, CPO_SHOWMATCH) != ശൂന്യ)
	ui_delay(p_mat * 100L + 8, TRUE);
    അല്ലെങ്കിൽ എങ്കിൽ (!char_avail())
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    പൂർണ്ണസംഖ്യ		move,
    pos_T	*cur,
    പൂർണ്ണസംഖ്യ		direction)
{
    regmmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		nmatched = 0;
    പൂർണ്ണസംഖ്യ		result = -1;
    pos_T	pos;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ		flag = 0;

    എങ്കിൽ (pattern == ശൂന്യ)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    എങ്കിൽ (തിരയൽ_regcomp(pattern, patternlen, ശൂന്യ, RE_തിരയൽ, RE_തിരയൽ,
					      തിരയൽ_KEEP, &regmatch) == FAIL)
	തിരികെ -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    എങ്കിൽ (move)
    {
	CLEAR_POS(&pos);
    }
    അല്ലെങ്കിൽ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = തിരയൽ_START;
    }

    എങ്കിൽ (searchit(curwin, curbuf, &pos, ശൂന്യ, direction, pattern, patternlen, 1,
				  തിരയൽ_KEEP + flag, RE_തിരയൽ, ശൂന്യ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	ചെയ്യുക
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, ശൂന്യ);
	    എങ്കിൽ (nmatched != 0)
		നിർത്തുക;
	} ആകുമ്പോൾ (regmatch.regprog != ശൂന്യ
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	എങ്കിൽ (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    തിരികെ result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    പൂർണ്ണസംഖ്യ
current_തിരയൽ(
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    പൂർണ്ണസംഖ്യ		flags = 0;
    pos_T	save_VIsual = VIsual;
    പൂർണ്ണസംഖ്യ		zero_width;
    പൂർണ്ണസംഖ്യ		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    എങ്കിൽ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (forward)
	    incl(&pos);
	അല്ലെങ്കിൽ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    എങ്കിൽ (zero_width == -1)
	തിരികെ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    വേണ്ടി (i = 0; i < 2; i++)
    {
	എങ്കിൽ (forward)
	{
	    എങ്കിൽ (i == 0 && skip_first_backward)
		തുടരുക;
	    dir = i;
	}
	അല്ലെങ്കിൽ
	    dir = !i;

	flags = 0;
	എങ്കിൽ (!dir && !zero_width)
	    flags = തിരയൽ_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	എങ്കിൽ (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? എണ്ണം : 1),
		തിരയൽ_KEEP | flags, RE_തിരയൽ, ശൂന്യ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	എങ്കിൽ (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    എങ്കിൽ (VIsual_active)
		VIsual = save_VIsual;
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (i == 0 && !result)
	{
	    എങ്കിൽ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    അല്ലെങ്കിൽ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_വരി_എണ്ണം;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_വരി_എണ്ണം);
	    }
	}
    }

    start_pos = pos;

    എങ്കിൽ (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    എങ്കിൽ (LT_POS(VIsual, end_pos) && forward)
    {
	എങ്കിൽ (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	അല്ലെങ്കിൽ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    എങ്കിൽ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	എങ്കിൽ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	അല്ലെങ്കിൽ എങ്കിൽ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    എങ്കിൽ (fdo_flags & FDO_തിരയൽ && താക്കോൽ_Typed)
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

    തിരികെ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    പൂർണ്ണസംഖ്യ
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    തിരികെ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    സ്ഥിതം ശൂന്യം
cmdline_തിരയൽ_stat(
    പൂർണ്ണസംഖ്യ		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    പൂർണ്ണസംഖ്യ		show_മുകൾ_bot_സന്ദേശം,
    char_u	*msgbuf,
    size_t	msgbuflen,
    പൂർണ്ണസംഖ്യ		recompute,
    പൂർണ്ണസംഖ്യ		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_തിരയൽ_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    എങ്കിൽ (stat.cur <= 0)
	തിരികെ;

    അക്ഷരം	t[തിരയൽ_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	എങ്കിൽ (stat.incomplete == 1)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[?/??]");
	അല്ലെങ്കിൽ എങ്കിൽ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	അല്ലെങ്കിൽ എങ്കിൽ (stat.cnt > maxcount)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	അല്ലെങ്കിൽ
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    അല്ലെങ്കിൽ
#endif
    {
	എങ്കിൽ (stat.incomplete == 1)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[?/??]");
	അല്ലെങ്കിൽ എങ്കിൽ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	അല്ലെങ്കിൽ എങ്കിൽ (stat.cnt > maxcount)
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	അല്ലെങ്കിൽ
	    len = vim_snprintf(t, തിരയൽ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    എങ്കിൽ (show_മുകൾ_bot_സന്ദേശം && len + 2 < തിരയൽ_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    എങ്കിൽ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    എങ്കിൽ (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    സന്ദേശം_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    സന്ദേശം_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    സ്ഥിതം ശൂന്യം
update_തിരയൽ_stat(
    പൂർണ്ണസംഖ്യ			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    പൂർണ്ണസംഖ്യ			recompute,
    പൂർണ്ണസംഖ്യ			maxcount,
    long		timeout UNUSED)
{
    പൂർണ്ണസംഖ്യ		    save_ws = p_ws;
    പൂർണ്ണസംഖ്യ		    wraparound = FALSE;
    pos_T	    p = (*pos);
    സ്ഥിതം pos_T    lastpos = {0, 0, 0};
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    cur = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    cnt = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    exact_match = FALSE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    incomplete = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    last_maxcount = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    chgtick = 0;
    സ്ഥിതം char_u   *lastpat = ശൂന്യ;
    സ്ഥിതം size_t   lastpatlen = 0;
    സ്ഥിതം buf_T    *lbuf = ശൂന്യ;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    എങ്കിൽ (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	തിരികെ;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    എങ്കിൽ (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ശൂന്യ
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
    എങ്കിൽ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	done_തിരയൽ = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	എങ്കിൽ (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	ആകുമ്പോൾ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ശൂന്യ, 0, 1, തിരയൽ_KEEP, RE_LAST, ശൂന്യ) != FAIL)
	{
	    done_തിരയൽ = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    എങ്കിൽ (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		നിർത്തുക;
	    }
#endif
	    cnt++;
	    എങ്കിൽ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		എങ്കിൽ (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    എങ്കിൽ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (got_int)
	    cur = -1; // abort
	എങ്കിൽ (done_തിരയൽ)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    എങ്കിൽ (lastpat == ശൂന്യ)
		lastpatlen = 0;
	    അല്ലെങ്കിൽ
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

#എങ്കിൽ defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    സ്ഥിതം char_u *
get_വരി_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *വരി = ml_get(lnum);

    vim_strncpy(buf, വരി, LSIZE - 1);
    തിരികെ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    ശൂന്യം
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    പൂർണ്ണസംഖ്യ		dir UNUSED,	// direction of expansion
    പൂർണ്ണസംഖ്യ		len,		// length of search pattern
    പൂർണ്ണസംഖ്യ		whole,		// match whole words only
    പൂർണ്ണസംഖ്യ		skip_comments,	// don't match inside comments
    പൂർണ്ണസംഖ്യ		type,		// Type of search; are we looking for a type?
				// a macro?
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    പൂർണ്ണസംഖ്യ		forceit,	// If true, always switch to the found path
    പൂർണ്ണസംഖ്യ		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    പൂർണ്ണസംഖ്യ		max_path_depth = 50;
    long	match_എണ്ണം = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ശൂന്യ;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		depth;
    പൂർണ്ണസംഖ്യ		depth_displayed;	// For type==CHECK_PATH
    പൂർണ്ണസംഖ്യ		old_files;
    പൂർണ്ണസംഖ്യ		already_searched;
    char_u	*file_വരി;
    char_u	*വരി;
    char_u	*p;
    char_u	save_char;
    പൂർണ്ണസംഖ്യ		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    പൂർണ്ണസംഖ്യ		matched = FALSE;
    പൂർണ്ണസംഖ്യ		did_show = FALSE;
    പൂർണ്ണസംഖ്യ		found = FALSE;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*already = ശൂന്യ;
    char_u	*startp = ശൂന്യ;
    char_u	*inc_opt = ശൂന്യ;
# എങ്കിൽ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ശൂന്യ;
# endif

    regmatch.regprog = ശൂന്യ;
    incl_regmatch.regprog = ശൂന്യ;
    def_regmatch.regprog = ശൂന്യ;

    file_വരി = alloc(LSIZE);
    എങ്കിൽ (file_വരി == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	എങ്കിൽ (pat == ശൂന്യ)
	    പോകുക fpip_end;
	vim_snprintf((അക്ഷരം *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	    പോകുക fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    എങ്കിൽ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	എങ്കിൽ (incl_regmatch.regprog == ശൂന്യ)
	    പോകുക fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    എങ്കിൽ (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	എങ്കിൽ (def_regmatch.regprog == ശൂന്യ)
	    പോകുക fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * വലുപ്പം(SearchedFile), TRUE);
    എങ്കിൽ (files == ശൂന്യ)
	പോകുക fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    എങ്കിൽ (end_lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	end_lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    എങ്കിൽ (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    വരി = get_വരി_and_copy(lnum, file_വരി);

    വേണ്ടി (;;)
    {
	എങ്കിൽ (incl_regmatch.regprog != ശൂന്യ
		&& vim_regexec(&incl_regmatch, വരി, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    എങ്കിൽ (strstr((അക്ഷരം *)inc_opt, "\\zs") != ശൂന്യ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (പൂർണ്ണസംഖ്യ)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    അല്ലെങ്കിൽ
		// Use text after match with 'include'.
		new_fname = file_name_in_വരി(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ശൂന്യ);
	    already_searched = FALSE;
	    എങ്കിൽ (new_fname != ശൂന്യ)
	    {
		// Check whether we have already searched in this file
		വേണ്ടി (i = 0;; i++)
		{
		    എങ്കിൽ (i == depth + 1)
			i = old_files;
		    എങ്കിൽ (i == max_path_depth)
			നിർത്തുക;
		    എങ്കിൽ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			എങ്കിൽ (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    സന്ദേശം_putchar('\n');	    // cursor below last one
			    എങ്കിൽ (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				സന്ദേശം_home_replace_hl(new_fname);
				സന്ദേശം_puts(_(" (includes previously listed match)"));
				prev_fname = ശൂന്യ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			നിർത്തുക;
		    }
		}
	    }

	    എങ്കിൽ (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == ശൂന്യ && !already_searched)))
	    {
		എങ്കിൽ (did_show)
		    സന്ദേശം_putchar('\n');	    // cursor below last one
		അല്ലെങ്കിൽ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    സന്ദേശം_puts_title(_("--- Included files "));
		    എങ്കിൽ (action != ACTION_SHOW_ALL)
			സന്ദേശം_puts_title(_("not found "));
		    സന്ദേശം_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ആകുമ്പോൾ (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    വേണ്ടി (i = 0; i < depth_displayed; i++)
			സന്ദേശം_puts("  ");
		    സന്ദേശം_home_replace(files[depth_displayed].name);
		    സന്ദേശം_puts(" -->\n");
		}
		എങ്കിൽ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    വേണ്ടി (i = 0; i <= depth_displayed; i++)
			സന്ദേശം_puts("  ");
		    എങ്കിൽ (new_fname != ശൂന്യ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			സന്ദേശം_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    അല്ലെങ്കിൽ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			എങ്കിൽ (strstr((അക്ഷരം *)inc_opt, "\\zs") != ശൂന്യ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (പൂർണ്ണസംഖ്യ)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			അല്ലെങ്കിൽ
			{
			    // find the file name after the end of the match
			    വേണ്ടി (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    വേണ്ടി (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			എങ്കിൽ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (പൂർണ്ണസംഖ്യ)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			അല്ലെങ്കിൽ എങ്കിൽ (p > വരി)
			{
			    എങ്കിൽ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    എങ്കിൽ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			സന്ദേശം_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    എങ്കിൽ (new_fname == ശൂന്യ && action == ACTION_SHOW_ALL)
		    {
			എങ്കിൽ (already_searched)
			    സന്ദേശം_puts(_("  (Already listed)"));
			അല്ലെങ്കിൽ
			    സന്ദേശം_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    എങ്കിൽ (new_fname != ശൂന്യ)
	    {
		// Push the new file onto the file stack
		എങ്കിൽ (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    എങ്കിൽ (bigger != ശൂന്യ)
		    {
			വേണ്ടി (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			വേണ്ടി (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = ശൂന്യ;
			    bigger[i].name = ശൂന്യ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			വേണ്ടി (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		എങ്കിൽ ((files[depth + 1].fp = mch_fopen((അക്ഷരം *)new_fname, "r"))
								    == ശൂന്യ)
		    vim_free(new_fname);
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (++depth == old_files)
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
		    എങ്കിൽ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			സന്ദേശം_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((അക്ഷരം*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(അക്ഷരം *)new_fname);
			സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (അക്ഷരം *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = വരി;
തിരയൽ_വരി:
	    define_matched = FALSE;
	    എങ്കിൽ (def_regmatch.regprog != ശൂന്യ
			      && vim_regexec(&def_regmatch, വരി, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ആകുമ്പോൾ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    എങ്കിൽ (def_regmatch.regprog == ശൂന്യ || define_matched)
	    {
		എങ്കിൽ (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    എങ്കിൽ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    അല്ലെങ്കിൽ
			matched = STRNCMP(startp, ptr, len) == 0;
		    എങ്കിൽ (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (regmatch.regprog != ശൂന്യ
			 && vim_regexec(&regmatch, വരി, (colnr_T)(p - വരി)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    എങ്കിൽ (!define_matched && skip_comments)
		    {
			എങ്കിൽ ((*വരി != '#' ||
				STRNCMP(skipwhite(വരി + 1), "define", 6) != 0)
				&& get_leader_len(വരി, ശൂന്യ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(വരി);
			എങ്കിൽ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    വേണ്ടി (p = വരി; *p && p < startp; ++p)
			    {
				എങ്കിൽ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    എങ്കിൽ (p[1] == '/')
					നിർത്തുക;
				    ++p;
				}
				അല്ലെങ്കിൽ എങ്കിൽ (!matched && p[0] == '*' && p[1] == '/')
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
	എങ്കിൽ (matched)
	{
	    എങ്കിൽ (action == ACTION_EXPAND)
	    {
		പൂർണ്ണസംഖ്യ	cont_s_ipos = FALSE;
		പൂർണ്ണസംഖ്യ	add_r;
		char_u	*aux;

		എങ്കിൽ (depth == -1 && lnum == curwin->w_cursor.lnum)
		    നിർത്തുക;
		found = TRUE;
		aux = p = startp;
		എങ്കിൽ (compl_status_adding() && (പൂർണ്ണസംഖ്യ)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    എങ്കിൽ (vim_iswordp(p))
			പോകുക exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (പൂർണ്ണസംഖ്യ)(p - aux);

		എങ്കിൽ (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    എങ്കിൽ (depth < 0)
		    {
			എങ്കിൽ (lnum >= end_lnum)
			    പോകുക exit_matched;
			വരി = get_വരി_and_copy(++lnum, file_വരി);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (vim_fgets(വരി = file_വരി,
						      LSIZE, files[depth].fp))
			പോകുക exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(വരി);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    എങ്കിൽ (p > aux)
		    {
			എങ്കിൽ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    എങ്കിൽ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    എങ്കിൽ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ശൂന്യ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			എങ്കിൽ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (പൂർണ്ണസംഖ്യ)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    എങ്കിൽ (i == ins_compl_len())
			പോകുക exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ശൂന്യ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		എങ്കിൽ (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		അല്ലെങ്കിൽ എങ്കിൽ (add_r == FAIL)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		എങ്കിൽ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		എങ്കിൽ (curr_fname != prev_fname)
		{
		    എങ്കിൽ (did_show)
			സന്ദേശം_putchar('\n');	// cursor below last one
		    എങ്കിൽ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			സന്ദേശം_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		എങ്കിൽ (!got_int)
		    show_pat_in_path(വരി, type, TRUE, action,
			    (depth == -1) ? ശൂന്യ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_എണ്ണം++);

		// Set matched flag for this file and all the ones that
		// include it
		വേണ്ടി (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (--എണ്ണം <= 0)
	    {
		found = TRUE;
		എങ്കിൽ (depth == -1 && lnum == curwin->w_cursor.lnum
# എങ്കിൽ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_വരി));
		അല്ലെങ്കിൽ എങ്കിൽ (action == ACTION_SHOW)
		{
		    show_pat_in_path(വരി, type, did_show, action,
			    (depth == -1) ? ശൂന്യ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		അല്ലെങ്കിൽ
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# എങ്കിൽ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    എങ്കിൽ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    എങ്കിൽ (action == ACTION_SPLIT)
		    {
			എങ്കിൽ (win_split(0, 0) == FAIL)
			    നിർത്തുക;
			RESET_BINDING(curwin);
		    }
		    എങ്കിൽ (depth == -1)
		    {
			// match in current file
# എങ്കിൽ defined(FEAT_QUICKFIX)
			എങ്കിൽ (g_do_tagpreview != 0)
			{
			    എങ്കിൽ (!win_valid(curwin_save))
				നിർത്തുക;
			    എങ്കിൽ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, ശൂന്യ,
						     ശൂന്യ, TRUE, lnum, forceit)))
				നിർത്തുക;	// failed to jump to file
			}
			അല്ലെങ്കിൽ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, ശൂന്യ, TRUE,
						    files[depth].lnum, forceit)))
			    നിർത്തുക;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		എങ്കിൽ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - വരി);
		    curwin->w_set_curswant = true;
		}

# എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		അല്ലെങ്കിൽ എങ്കിൽ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		നിർത്തുക;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    എങ്കിൽ (def_regmatch.regprog == ശൂന്യ
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		പോകുക തിരയൽ_വരി;
	}
	വരി_breakcheck();
	എങ്കിൽ (action == ACTION_EXPAND)
	    ins_compl_check_താക്കോലുകൾ(30, FALSE);
	എങ്കിൽ (got_int || ins_compl_interrupted())
	    നിർത്തുക;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ആകുമ്പോൾ (depth >= 0 && !already
		&& vim_fgets(വരി = file_വരി, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    എങ്കിൽ (depth < depth_displayed)
		depth_displayed = depth;
	}
	എങ്കിൽ (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (പൂർണ്ണസംഖ്യ)STRLEN(വരി);
	    എങ്കിൽ (i > 0 && വരി[i - 1] == '\n')
		വരി[--i] = NUL;
	    എങ്കിൽ (i > 0 && വരി[i - 1] == '\r')
		വരി[--i] = NUL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!already)
	{
	    എങ്കിൽ (++lnum > end_lnum)
		നിർത്തുക;
	    വരി = get_വരി_and_copy(lnum, file_വരി);
	}
	already = ശൂന്യ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    വേണ്ടി (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    വേണ്ടി (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    എങ്കിൽ (type == CHECK_PATH)
    {
	എങ്കിൽ (!did_show)
	{
	    എങ്കിൽ (action != ACTION_SHOW_ALL)
		സന്ദേശം(_("All included files were found"));
	    അല്ലെങ്കിൽ
		സന്ദേശം(_("No included files"));
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!found && action != ACTION_EXPAND && !silent)
    {
	എങ്കിൽ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	അല്ലെങ്കിൽ എങ്കിൽ (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	അല്ലെങ്കിൽ
	    emsg(_(e_couldnt_find_pattern));
    }
    എങ്കിൽ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	സന്ദേശം_end();

fpip_end:
    vim_free(file_വരി);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    സ്ഥിതം ശൂന്യം
show_pat_in_path(
    char_u  *വരി,
    പൂർണ്ണസംഖ്യ	    type,
    പൂർണ്ണസംഖ്യ	    did_show,
    പൂർണ്ണസംഖ്യ	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    എണ്ണം)
{
    char_u  *p;
    size_t  linelen;

    എങ്കിൽ (did_show)
	സന്ദേശം_putchar('\n');	// cursor below last one
    അല്ലെങ്കിൽ എങ്കിൽ (!സന്ദേശം_silent)
	gotocmdline(TRUE);	// cursor at status line
    എങ്കിൽ (got_int)		// 'q' typed at "--more--" message
	തിരികെ;
    linelen = STRLEN(വരി);
    വേണ്ടി (;;)
    {
	p = വരി + linelen - 1;
	എങ്കിൽ (fp != ശൂന്യ)
	{
	    // We used fgets(), so get rid of newline at end
	    എങ്കിൽ (p >= വരി && *p == '\n')
		--p;
	    എങ്കിൽ (p >= വരി && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	എങ്കിൽ (action == ACTION_SHOW_ALL)
	{
	    sprintf((അക്ഷരം *)IObuff, "%3ld: ", എണ്ണം);	// show match nr
	    സന്ദേശം_puts((അക്ഷരം *)IObuff);
	    sprintf((അക്ഷരം *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    സന്ദേശം_puts_attr((അക്ഷരം *)IObuff, HL_ATTR(HLF_N));
	    സന്ദേശം_puts(" ");
	}
	സന്ദേശം_prt_വരി(വരി, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	എങ്കിൽ (got_int || type != FIND_DEFINE || p < വരി || *p != '\\')
	    നിർത്തുക;

	എങ്കിൽ (fp != ശൂന്യ)
	{
	    എങ്കിൽ (vim_fgets(വരി, LSIZE, fp)) // end of file
		നിർത്തുക;
	    linelen = STRLEN(വരി);
	    ++*lnum;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (++*lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    വരി = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	സന്ദേശം_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    പൂർണ്ണസംഖ്യ
get_spat_last_idx(ശൂന്യം)
{
    തിരികെ last_idx;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    ശൂന്യം
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			maxcount = p_msc;
    long		timeout = തിരയൽ_STAT_DEF_TIMEOUT;
    പൂർണ്ണസംഖ്യ			recompute = TRUE;
    searchstat_T	stat;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	പൂർണ്ണസംഖ്യ		error = FALSE;

	എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    തിരികെ;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    timeout = (long)tv_get_സംഖ്യ_chk(&di->di_tv, &error);
	    എങ്കിൽ (error)
		തിരികെ;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    maxcount = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&di->di_tv, &error);
	    എങ്കിൽ (error)
		തിരികെ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    എങ്കിൽ (pattern == ശൂന്യ)
		തിരികെ;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		തിരികെ;
	    }
	    എങ്കിൽ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		തിരികെ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    എങ്കിൽ (li != ശൂന്യ)
	    {
		pos.lnum = tv_get_സംഖ്യ_chk(&li->li_tv, &error);
		എങ്കിൽ (error)
		    തിരികെ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    എങ്കിൽ (li != ശൂന്യ)
	    {
		pos.col = tv_get_സംഖ്യ_chk(&li->li_tv, &error) - 1;
		എങ്കിൽ (error)
		    തിരികെ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    എങ്കിൽ (li != ശൂന്യ)
	    {
		pos.coladd = tv_get_സംഖ്യ_chk(&li->li_tv, &error);
		എങ്കിൽ (error)
		    തിരികെ;
	    }
	}
    }

    save_last_തിരയൽ_pattern();
# ifdef FEAT_തിരയൽ_EXTRA
    save_incsearch_state();
# endif
    എങ്കിൽ (pattern != ശൂന്യ)
    {
	എങ്കിൽ (*pattern == NUL)
	    പോകുക the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	എങ്കിൽ (spats[last_idx].pat == ശൂന്യ)
	    spats[last_idx].patlen = 0;
    }
    എങ്കിൽ (spats[last_idx].pat == ശൂന്യ || *spats[last_idx].pat == NUL)
	പോകുക the_end;	// the previous pattern was never defined

    update_തിരയൽ_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_സംഖ്യ(rettv->vval.v_dict, "current", stat.cur);
    dict_add_സംഖ്യ(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_സംഖ്യ(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_സംഖ്യ(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_സംഖ്യ(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_തിരയൽ_pattern();
# ifdef FEAT_തിരയൽ_EXTRA
    restore_incsearch_state();
# endif
}
#endif
