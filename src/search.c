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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_EVAL
સ્થિર શૂન્ય set_vv_searchforward(શૂન્ય);
સ્થિર પૂર્ણાંક first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
સ્થિર char_u *get_લીટી_and_copy(linenr_T lnum, char_u *buf);
સ્થિર શૂન્ય show_pat_in_path(char_u *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, FILE *, linenr_T *, long);
#endif

પ્રકારવ્યાખ્યા સંરચના searchstat
{
    પૂર્ણાંક	    cur;	    // current position of found words
    પૂર્ણાંક	    cnt;	    // total count of found words
    પૂર્ણાંક	    exact_match;    // TRUE if matched exactly on specified position
    પૂર્ણાંક	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    પૂર્ણાંક	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_શોધ_EXTRA
સ્થિર શૂન્ય save_incsearch_state(શૂન્ય);
સ્થિર શૂન્ય restore_incsearch_state(શૂન્ય);
#endif
સ્થિર પૂર્ણાંક check_prevcol(char_u *linep, પૂર્ણાંક col, પૂર્ણાંક ch, પૂર્ણાંક *prevcol);
સ્થિર પૂર્ણાંક find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
સ્થિર શૂન્ય find_mps_કિંમતો(પૂર્ણાંક *initc, પૂર્ણાંક *findc, પૂર્ણાંક *backwards, પૂર્ણાંક switchit);
સ્થિર પૂર્ણાંક is_zero_width(char_u *pattern, size_t patternlen, પૂર્ણાંક move, pos_T *cur, પૂર્ણાંક direction);
સ્થિર શૂન્ય cmdline_શોધ_stat(પૂર્ણાંક dirc, pos_T *pos, pos_T *cursor_pos, પૂર્ણાંક show_ટોચ_bot_સંદેશ, char_u *msgbuf, size_t msgbuflen, પૂર્ણાંક recompute, પૂર્ણાંક maxcount, long timeout);
સ્થિર શૂન્ય update_શોધ_stat(પૂર્ણાંક dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, પૂર્ણાંક recompute, પૂર્ણાંક maxcount, long timeout);

#define શોધ_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define શોધ_STAT_BUF_LEN 16

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
સ્થિર spat_T spats[2] =
{
    {ખાલી, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ખાલી, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

સ્થિર પૂર્ણાંક last_idx = 0;	// index in spats[] for RE_LAST

સ્થિર char_u lastc[2] = {NUL, NUL};	// last character searched for
સ્થિર પૂર્ણાંક lastcdir = FORWARD;		// last direction of character search
સ્થિર પૂર્ણાંક last_t_cmd = TRUE;		// last search t_cmd
સ્થિર char_u	lastc_bytes[MB_MAXBYTES + 1];
સ્થિર પૂર્ણાંક	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
સ્થિર spat_T	    saved_spats[ARRAY_LENGTH(spats)];
સ્થિર char_u	    *saved_mr_pattern = ખાલી;
સ્થિર size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_શોધ_EXTRA
સ્થિર પૂર્ણાંક	    saved_spats_last_idx = 0;
સ્થિર પૂર્ણાંક	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
સ્થિર char_u	    *mr_pattern = ખાલી;
સ્થિર size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
પ્રકારવ્યાખ્યા સંરચના SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    પૂર્ણાંક		matched;	// Found a match in this file
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
    પૂર્ણાંક
શોધ_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    પૂર્ણાંક		pat_save,
    પૂર્ણાંક		pat_use,
    પૂર્ણાંક		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    પૂર્ણાંક		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    જો (pat == ખાલી || *pat == NUL)
    {
	પૂર્ણાંક i;

	જો (pat_use == RE_LAST)
	    i = last_idx;
	નહીંતર
	    i = pat_use;
	જો (spats[i].pat == ખાલી)	// pattern was never defined
	{
	    જો (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    નહીંતર
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    પાછા FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    નહીંતર જો (options & શોધ_HIS)	// put new pattern in history
	add_to_history(HIST_શોધ, pat, patlen, TRUE, NUL);

    જો (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    નહીંતર
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    જો (mr_pattern == ખાલી)
	mr_patternlen = 0;
    નહીંતર
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    જો (!(options & શોધ_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	જો (pat_save == RE_શોધ || pat_save == RE_BOTH)
	    save_re_pat(RE_શોધ, pat, patlen, magic);
	// substitute or global command
	જો (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    જો (regmatch->regprog == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_શોધ_pat(શૂન્ય)
{
    પાછા mr_pattern;
}

    શૂન્ય
save_re_pat(પૂર્ણાંક idx, char_u *pat, size_t patlen, પૂર્ણાંક magic)
{
    જો (spats[idx].pat == pat)
	પાછા;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    જો (spats[idx].pat == ખાલી)
	spats[idx].patlen = 0;
    નહીંતર
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_શોધ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    જો (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
સ્થિર પૂર્ણાંક save_level = 0;

    શૂન્ય
save_શોધ_patterns(શૂન્ય)
{
    પૂર્ણાંક i;

    જો (save_level++ != 0)
	પાછા;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	જો (spats[i].pat != ખાલી)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    જો (saved_spats[i].pat == ખાલી)
		saved_spats[i].patlen = 0;
	    નહીંતર
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    જો (mr_pattern == ખાલી)
	saved_mr_pattern = ખાલી;
    નહીંતર
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    જો (saved_mr_pattern == ખાલી)
	saved_mr_patternlen = 0;
    નહીંતર
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_શોધ_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    શૂન્ય
restore_શોધ_patterns(શૂન્ય)
{
    પૂર્ણાંક i;

    જો (--save_level != 0)
	પાછા;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#જો defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_શોધ_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#જો defined(EXITFREE)
    શૂન્ય
free_શોધ_patterns(શૂન્ય)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_શોધ_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
સ્થિર spat_T	    saved_last_શોધ_spat;
સ્થિર પૂર્ણાંક	    did_save_last_શોધ_spat = 0;
સ્થિર પૂર્ણાંક	    saved_last_idx = 0;
સ્થિર પૂર્ણાંક	    saved_no_hlsearch = 0;
સ્થિર પૂર્ણાંક	    saved_શોધ_match_endcol;
સ્થિર પૂર્ણાંક	    saved_શોધ_match_લીટીઓ;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    શૂન્ય
save_last_શોધ_pattern(શૂન્ય)
{
    જો (++did_save_last_શોધ_spat != 1)
	// nested call, nothing to do
	પાછા;

    saved_last_શોધ_spat = spats[RE_શોધ];
    જો (spats[RE_શોધ].pat != ખાલી)
    {
	saved_last_શોધ_spat.pat = vim_strnsave(spats[RE_શોધ].pat, spats[RE_શોધ].patlen);
	જો (saved_last_શોધ_spat.pat == ખાલી)
	    saved_last_શોધ_spat.patlen = 0;
	નહીંતર
	    saved_last_શોધ_spat.patlen = spats[RE_શોધ].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    શૂન્ય
restore_last_શોધ_pattern(શૂન્ય)
{
    જો (--did_save_last_શોધ_spat > 0)
	// nested call, nothing to do
	પાછા;
    જો (did_save_last_શોધ_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	પાછા;
    }

    vim_free(spats[RE_શોધ].pat);
    spats[RE_શોધ] = saved_last_શોધ_spat;
    saved_last_શોધ_spat.pat = ખાલી;
    saved_last_શોધ_spat.patlen = 0;
# જો defined(FEAT_EVAL)
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
    સ્થિર શૂન્ય
save_incsearch_state(શૂન્ય)
{
    saved_શોધ_match_endcol = શોધ_match_endcol;
    saved_શોધ_match_લીટીઓ  = શોધ_match_લીટીઓ;
}

    સ્થિર શૂન્ય
restore_incsearch_state(શૂન્ય)
{
    શોધ_match_endcol = saved_શોધ_match_endcol;
    શોધ_match_લીટીઓ  = saved_શોધ_match_લીટીઓ;
}

    char_u *
last_શોધ_pattern(શૂન્ય)
{
    પાછા spats[RE_શોધ].pat;
}

    size_t
last_શોધ_pattern_len(શૂન્ય)
{
    પાછા spats[RE_શોધ].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    પૂર્ણાંક
ignorecase(char_u *pat)
{
    પાછા ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    પૂર્ણાંક
ignorecase_opt(char_u *pat, પૂર્ણાંક ic_in, પૂર્ણાંક scs)
{
    પૂર્ણાંક		ic = ic_in;

    જો (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    પાછા ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    પૂર્ણાંક
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (શૂન્ય)skip_regexp_ex(pat, NUL, magic_isset(), ખાલી, ખાલી, &magic_val);

    જ્યારે (*p != NUL)
    {
	પૂર્ણાંક		l;

	જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    જો (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		પાછા TRUE;
	    p += l;
	}
	નહીંતર જો (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    જો (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    નહીંતર જો (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    નહીંતર જો (p[1] != NUL)  // skip "\X"
		p += 2;
	    નહીંતર
		p += 1;
	}
	નહીંતર જો ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    જો (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    નહીંતર
		p++;
	}
	નહીંતર જો (MB_ISUPPER(*p))
	    પાછા TRUE;
	નહીંતર
	    ++p;
    }
    પાછા FALSE;
}

#જો defined(FEAT_EVAL)
    char_u *
last_csearch(શૂન્ય)
{
    પાછા lastc_bytes;
}

    પૂર્ણાંક
last_csearch_forward(શૂન્ય)
{
    પાછા lastcdir == FORWARD;
}

    પૂર્ણાંક
last_csearch_until(શૂન્ય)
{
    પાછા last_t_cmd == TRUE;
}

    શૂન્ય
set_last_csearch(પૂર્ણાંક c, char_u *s, પૂર્ણાંક len)
{
    *lastc = c;
    lastc_bytelen = len;
    જો (len)
	memcpy(lastc_bytes, s, len);
    નહીંતર
	CLEAR_FIELD(lastc_bytes);
}
#endif

    શૂન્ય
set_csearch_direction(પૂર્ણાંક cdir)
{
    lastcdir = cdir;
}

    શૂન્ય
set_csearch_until(પૂર્ણાંક t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_શોધ_pat(શૂન્ય)
{
    પાછા spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    શૂન્ય
reset_શોધ_dir(શૂન્ય)
{
    spats[0].off.dir = '/';
#જો defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#જો defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    શૂન્ય
set_last_શોધ_pat(
    char_u	*s,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		magic,
    પૂર્ણાંક		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    જો (*s == NUL)
	spats[idx].pat = ખાલી;
    નહીંતર
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    જો (spats[idx].pat == ખાલી)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# જો defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.લીટી = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    જો (setlast)
	last_idx = idx;
    જો (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	જો (spats[idx].pat == ખાલી)
	    saved_spats[idx].pat = ખાલી;
	નહીંતર
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	જો (saved_spats[idx].pat == ખાલી)
	    saved_spats[idx].patlen = 0;
	નહીંતર
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_શોધ_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_શોધ_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    જો (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_શોધ_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    શૂન્ય
last_pat_prog(regmmatch_T *regmatch)
{
    જો (spats[last_idx].pat == ખાલી)
    {
	regmatch->regprog = ખાલી;
	પાછા;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (શૂન્ય)શોધ_regcomp((char_u *)"", 0, ખાલી, 0, last_idx, શોધ_KEEP, regmatch);
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
    પૂર્ણાંક
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    પૂર્ણાંક		dir,
    char_u	*pat,
    size_t	patlen,
    long	ગણતરી,
    પૂર્ણાંક		options,
    પૂર્ણાંક		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    પૂર્ણાંક		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    પૂર્ણાંક		loop;
    pos_T	start_pos;
    પૂર્ણાંક		at_first_લીટી;
    પૂર્ણાંક		extra_col;
    પૂર્ણાંક		start_char_len;
    પૂર્ણાંક		match_ok;
    long	nmatched;
    પૂર્ણાંક		submatch = 0;
    પૂર્ણાંક		first_match = TRUE;
    પૂર્ણાંક		called_emsg_before = called_emsg;
#ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    પૂર્ણાંક		unused_timeout_flag = FALSE;
    પૂર્ણાંક		*timed_out = &unused_timeout_flag;  // set when timed out.
    પૂર્ણાંક		શોધ_from_match_end;		    // vi-compatible search?

    જો (શોધ_regcomp(pat, patlen, ખાલી, RE_શોધ, pat_use,
		   (options & (શોધ_HIS + શોધ_KEEP)), &regmatch) == FAIL)
    {
	જો ((options & શોધ_સંદેશ) && !rc_did_emsg)
	    semsg(_(e_invalid_શોધ_string_str), mr_pattern);
	પાછા FAIL;
    }

    શોધ_from_match_end = vim_strchr(p_cpo, CPO_શોધ) != ખાલી;

    જો (extra_arg != ખાલી)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	જો (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    કરો	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	જો (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	નહીંતર જો (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_લીટી_ગણતરી
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    જો (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    નહીંતર
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	નહીંતર
	    start_char_len = 1;
	જો (dir == FORWARD)
	{
	    જો (options & શોધ_START)
		extra_col = 0;
	    નહીંતર
		extra_col = start_char_len;
	}
	નહીંતર
	{
	    જો (options & શોધ_START)
		extra_col = start_char_len;
	    નહીંતર
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_લીટી = TRUE;	// default: start in first line
	જો (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_લીટી = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	જો (dir == BACKWARD && start_pos.col == 0
					     && (options & શોધ_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_લીટી = FALSE;
	}
	નહીંતર
	    lnum = pos->lnum;

	માટે (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    માટે ( ; lnum > 0 && lnum <= buf->b_ml.ml_લીટી_ગણતરી;
					   lnum += dir, at_first_લીટી = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		જો (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    તોડો;
		// Stop after passing the time limit.
		જો (*timed_out)
		    તોડો;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_લીટી && (options & શોધ_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		જો (regmatch.regprog == ખાલી)
		    તોડો;
		// Abort searching on an error (e.g., out of stack).
		જો (called_emsg > called_emsg_before || *timed_out)
		    તોડો;
		જો (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    જો (lnum + matchpos.lnum > buf->b_ml.ml_લીટી_ગણતરી)
			ptr = (char_u *)"";
		    નહીંતર
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    જો (dir == FORWARD && at_first_લીટી)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			જ્યારે (matchpos.lnum == 0
				&& ((options & શોધ_END) && first_match
				    ?  (nmatched == 1
					&& (પૂર્ણાંક)endpos.col - 1
					     < (પૂર્ણાંક)start_pos.col + extra_col)
				    : ((પૂર્ણાંક)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (પૂર્ણાંક)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    જો (શોધ_from_match_end)
			    {
				જો (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    તોડો;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				જો (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    જો (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    નહીંતર
					++matchcol;
				}
			    }
			    નહીંતર
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				જો (ptr[matchcol] != NUL)
				{
				    જો (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    નહીંતર
					++matchcol;
				}
			    }
			    જો (matchcol == 0 && (options & શોધ_START))
				તોડો;
			    જો (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				તોડો;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    જો (regmatch.regprog == ખાલી)
				તોડો;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			જો (!match_ok)
			    ચાલુ;
		    }
		    જો (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			માટે (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    જો (loop
				|| ((options & શોધ_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (પૂર્ણાંક)regmatch.endpos[0].col - 1
							< (પૂર્ણાંક)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (પૂર્ણાંક)regmatch.startpos[0].col
						      < (પૂર્ણાંક)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    નહીંતર
				તોડો;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    જો (શોધ_from_match_end)
			    {
				જો (nmatched > 1)
				    તોડો;
				matchcol = endpos.col;
				// for empty match: advance one char
				જો (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    જો (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    નહીંતર
					++matchcol;
				}
			    }
			    નહીંતર
			    {
				// Stop when the match is in a next line.
				જો (matchpos.lnum > 0)
				    તોડો;
				matchcol = matchpos.col;
				જો (ptr[matchcol] != NUL)
				{
				    જો (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    નહીંતર
					++matchcol;
				}
			    }
			    જો (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				જો (*timed_out)
				    match_ok = FALSE;
				તોડો;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    જો (regmatch.regprog == ખાલી)
				તોડો;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			જો (!match_ok)
			    ચાલુ;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    જો ((options & શોધ_END) && !(options & શોધ_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			જો (endpos.col == 0)
			{
			    જો (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			નહીંતર
			{
			    --pos->col;
			    જો (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_લીટી_ગણતરી)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			જો (end_pos != ખાલી)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    નહીંતર
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			જો (end_pos != ખાલી)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    જો (end_pos != ખાલી)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    શોધ_match_લીટીઓ = endpos.lnum - matchpos.lnum;
		    શોધ_match_endcol = endpos.col;
		    તોડો;
		}
		લીટી_breakcheck();	// stop if ctrl-C typed
		જો (got_int)
		    તોડો;

#ifdef FEAT_શોધ_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		જો ((options & શોધ_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    તોડો;
		}
#endif

		જો (loop && lnum == start_pos.lnum)
		    તોડો;	    // if second loop, stop where started
	    }
	    at_first_લીટી = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    જો (regmatch.regprog == ખાલી)
		તોડો;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    જો (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_શોધ_EXTRA
			      || break_loop
#endif
			      || found || loop)
		તોડો;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    જો (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_લીટી_ગણતરી;
	    નહીંતર
		lnum = 1;
	    જો (!shortmess(SHM_શોધ)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & શોધ_સંદેશ))
		give_warning((char_u *)_(dir == BACKWARD
					  ? ટોચ_bot_સંદેશ : bot_ટોચ_સંદેશ), TRUE);
	    જો (extra_arg != ખાલી)
		extra_arg->sa_wrapped = TRUE;
	}
	જો (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_શોધ_EXTRA
		|| break_loop
#endif
		)
	    તોડો;
    }
    જ્યારે (--ગણતરી > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    જો (extra_arg != ખાલી && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    જો (!found)		    // did not find it
    {
	જો (got_int)
	    emsg(_(e_interrupted));
	નહીંતર જો ((options & શોધ_સંદેશ) == શોધ_સંદેશ)
	{
	    જો (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    નહીંતર જો (lnum == 0)
		semsg(_(e_શોધ_hit_ટોચ_without_match_for_str), mr_pattern);
	    નહીંતર
		semsg(_(e_શોધ_hit_bottom_without_match_for_str), mr_pattern);
	}
	પાછા FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    જો (pos->lnum > buf->b_ml.ml_લીટી_ગણતરી)
    {
	pos->lnum = buf->b_ml.ml_લીટી_ગણતરી;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	જો (pos->col > 0)
	    --pos->col;
    }

    પાછા submatch + 1;
}

#જો defined(FEAT_EVAL) || defined(FEAT_PROTO)
    શૂન્ય
set_શોધ_direction(પૂર્ણાંક cdir)
{
    spats[0].off.dir = cdir;
}

    સ્થિર શૂન્ય
set_vv_searchforward(શૂન્ય)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    સ્થિર પૂર્ણાંક
first_submatch(regmmatch_T *rp)
{
    પૂર્ણાંક		submatch;

    માટે (submatch = 1; ; ++submatch)
    {
	જો (rp->startpos[submatch].lnum >= 0)
	    તોડો;
	જો (submatch == 9)
	{
	    submatch = 0;
	    તોડો;
	}
    }
    પાછા submatch;
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
    પૂર્ણાંક
parse_શોધ_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    પૂર્ણાંક		શોધ_delim,
    પૂર્ણાંક		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    પૂર્ણાંક		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    જો (*pat == ખાલી || **pat == NUL)
	પાછા 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ખાલી;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, શોધ_delim, magic_isset(),
						    strcopy, ખાલી, ખાલી);
    જો (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (પૂર્ણાંક)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    જો (*p == શોધ_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->લીટી = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    જો (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->લીટી = TRUE;
    નહીંતર જો ((options & શોધ_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	જો (*p == 'e')		// end
	    offset->end = શોધ_END;
	++p;
    }
    જો (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	જો (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((અક્ષર *)p);
	નહીંતર જો (*p == '-')	    // single '-'
	    offset->off = -1;
	નહીંતર			    // single '+'
	    offset->off = 1;
	++p;
	જ્યારે (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (પૂર્ણાંક)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    પાછા cmdlen;
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
    પૂર્ણાંક
do_શોધ(
    oparg_T	    *oap,	// can be NULL
    પૂર્ણાંક		    dirc,	// '/' or '?'
    પૂર્ણાંક		    શોધ_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    ગણતરી,
    પૂર્ણાંક		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    પૂર્ણાંક		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ખાલી;
    પૂર્ણાંક		    show_શોધ_stats;
    char_u	    *msgbuf = ખાલી;
    size_t	    msgbuflen = 0;
    પૂર્ણાંક		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    જો (spats[0].off.લીટી && vim_strchr(p_cpo, CPO_LINEOFF) != ખાલી)
    {
	spats[0].off.લીટી = FALSE;
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
    જો (dirc == 0)
	dirc = spats[0].off.dir;
    નહીંતર
    {
	spats[0].off.dir = dirc;
#જો defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    જો (options & શોધ_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#નહીંતર
	જો (dirc == '/')
	    dirc = '?';
	નહીંતર
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    જો (dirc == '/')
    {
	જો (hasFolding(pos.lnum, ખાલી, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    નહીંતર
    {
	જો (hasFolding(pos.lnum, &pos.lnum, ખાલી))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_શોધ_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    જો (no_hlsearch && !(options & શોધ_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    માટે (;;)
    {
	પૂર્ણાંક		show_ટોચ_bot_સંદેશ = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ખાલી;
					    // use previous pattern
	જો (pat == ખાલી || *pat == NUL || *pat == શોધ_delim)
	{
	    જો (spats[RE_શોધ].pat == ખાલી)	    // no previous pattern
	    {
		જો (spats[RE_SUBST].pat == ખાલી)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    જાઓ end_do_શોધ;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    નહીંતર
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	જો (pat != ખાલી && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_શોધ_pattern_offset(&pat, &patlen,
				શોધ_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    જો (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_શોધ_stats = FALSE;
	જો ((options & શોધ_ECHO) && messaging()
		&& !સંદેશ_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    સંદેશ_start();

	    // Get the offset, so we know how long it is.
	    જો (!cmd_silent &&
		    (spats[0].off.લીટી || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		જો (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		નહીંતર જો (!spats[0].off.લીટી)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		જો (spats[0].off.off != 0 || spats[0].off.લીટી)
		    off_len += vim_snprintf((અક્ષર *)off_buf + off_len,
			   કદ(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    જો (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    નહીંતર
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    જો (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		જો (સંદેશ_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (પૂર્ણાંક)(Rows - સંદેશ_row) * cmdline_width - 1;
		નહીંતર
		    // Use up to 'showcmd' column.
		    msgbufsize = (પૂર્ણાંક)(Rows - સંદેશ_row - 1) * cmdline_width
			+ sc_col - 1;
		જો (msgbufsize < plen + off_len + શોધ_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + શોધ_STAT_BUF_LEN + 3;
	    }
	    નહીંતર
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    જો (msgbuf == ખાલી)
	    {
		msgbuflen = 0;
	    }
	    નહીંતર
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		જો (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    જો (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    નહીંતર
			mch_memmove(msgbuf + 1, p, plen);
		    જો (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = સંદેશ_strtrunc(msgbuf, TRUE);
		    જો (trunc != ખાલી)
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
		    જો (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			જો (r != ખાલી)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    જ્યારે (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    જો ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    નહીંતર
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    સંદેશ_outtrans(msgbuf);
		    સંદેશ_clr_eos();
		    સંદેશ_check();

		    gotocmdline(FALSE);
		    out_flush();
		    સંદેશ_nowait = TRUE;	    // don't wait for this message
		}

		જો (!shortmess(SHM_SEARCHCOUNT))
		    show_શોધ_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	જો (!spats[0].off.લીટી && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    જો (spats[0].off.off > 0)
	    {
		માટે (c = spats[0].off.off; c; --c)
		    જો (decl(&pos) == -1)
			તોડો;
		જો (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    નહીંતર
	    {
		માટે (c = spats[0].off.off; c; ++c)
		    જો (incl(&pos) == -1)
			તોડો;
		જો (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_લીટી_ગણતરી + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ખાલી,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, ગણતરી, spats[0].off.end + (options &
		       (શોધ_KEEP + શોધ_PEEK + શોધ_HIS
			+ શોધ_સંદેશ + શોધ_START
			+ ((pat != ખાલી && *pat == ';') ? 0 : શોધ_NOOF))),
		RE_LAST, sia);

	જો (dircp != ખાલી)
	    *dircp = શોધ_delim; // restore second '/' or '?' for normal_cmd()

	જો (!shortmess(SHM_શોધ) && sia && sia->sa_wrapped)
	    show_ટોચ_bot_સંદેશ = TRUE;

	જો (c == FAIL)
	{
	    retval = 0;
	    જાઓ end_do_શોધ;
	}
	જો (spats[0].off.end && oap != ખાલી)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	જો (!(options & શોધ_NOOF) || (pat != ખાલી && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    જો (spats[0].off.લીટી)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		જો (c < 1)
		    pos.lnum = 1;
		નહીંતર જો (c > curbuf->b_ml.ml_લીટી_ગણતરી)
		    pos.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
		નહીંતર
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    નહીંતર જો (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		જો (c > 0)
		{
		    જ્યારે (c-- > 0)
			જો (incl(&pos) == -1)
			    તોડો;
		}
		// to the left, check for start of file
		નહીંતર
		{
		    જ્યારે (c++ < 0)
			જો (decl(&pos) == -1)
			    તોડો;
		}
	    }
	    જો (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	જો (show_શોધ_stats)
	     cmdline_શોધ_stat(dirc, &pos, &curwin->w_cursor,
				show_ટોચ_bot_સંદેશ, msgbuf, msgbuflen,
				(ગણતરી != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_શોધ)
				     && hasFolding(curwin->w_cursor.lnum,
								   ખાલી, ખાલી))
#endif
				),
				p_msc,
				શોધ_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	જો (!(options & શોધ_OPT) || pat == ખાલી || *pat != ';')
	    તોડો;

	dirc = *++pat;
	--patlen;
	શોધ_delim = dirc;
	જો (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    જાઓ end_do_શોધ;
	}
	++pat;
	--patlen;
    }

    જો (options & શોધ_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_શોધ:
    જો ((options & શોધ_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    પાછા retval;
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
    પૂર્ણાંક
શોધ_for_exact_લીટી(
    buf_T	*buf,
    pos_T	*pos,
    પૂર્ણાંક		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    પૂર્ણાંક		compl_len = ins_compl_len();

    જો (buf->b_ml.ml_લીટી_ગણતરી == 0)
	પાછા FAIL;
    માટે (;;)
    {
	pos->lnum += dir;
	જો (pos->lnum < 1)
	{
	    જો (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_લીટી_ગણતરી;
		જો (!shortmess(SHM_શોધ))
		    give_warning((char_u *)_(ટોચ_bot_સંદેશ), TRUE);
	    }
	    નહીંતર
	    {
		pos->lnum = 1;
		તોડો;
	    }
	}
	નહીંતર જો (pos->lnum > buf->b_ml.ml_લીટી_ગણતરી)
	{
	    જો (p_ws)
	    {
		pos->lnum = 1;
		જો (!shortmess(SHM_શોધ))
		    give_warning((char_u *)_(bot_ટોચ_સંદેશ), TRUE);
	    }
	    નહીંતર
	    {
		pos->lnum = 1;
		તોડો;
	    }
	}
	જો (pos->lnum == start)
	    તોડો;
	જો (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	જો (compl_status_adding() && !compl_status_sol())
	{
	    જો ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		પાછા OK;
	}
	નહીંતર જો (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    જો ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		પાછા OK;
	}
    }
    પાછા FAIL;
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
    પૂર્ણાંક
searchc(cmdarg_T *cap, પૂર્ણાંક t_cmd)
{
    પૂર્ણાંક			c = cap->nchar;	// char to search for
    પૂર્ણાંક			dir = cap->arg;	// TRUE for searching forward
    long		ગણતરી = cap->count1;	// repeat count
    પૂર્ણાંક			col;
    char_u		*p;
    પૂર્ણાંક			len;
    પૂર્ણાંક			stop = TRUE;

    જો (c != NUL)	// normal search: remember args for repeat
    {
	જો (!ચાવી_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    જો (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		જો (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    નહીંતર		// repeat previous search
    {
	જો (*lastc == NUL && lastc_bytelen <= 1)
	    પાછા FAIL;
	જો (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	નહીંતર
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	જો (vim_strchr(p_cpo, CPO_SCOLON) == ખાલી && ગણતરી == 1 && t_cmd)
	    stop = FALSE;
    }

    જો (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    નહીંતર
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    જ્યારે (ગણતરી--)
    {
	જો (has_mbyte)
	{
	    માટે (;;)
	    {
		જો (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    જો (col >= len)
			પાછા FAIL;
		}
		નહીંતર
		{
		    જો (col == 0)
			પાછા FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		જો (lastc_bytelen <= 1)
		{
		    જો (p[col] == c && stop)
			તોડો;
		}
		નહીંતર જો (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    તોડો;
		stop = TRUE;
	    }
	}
	નહીંતર
	{
	    માટે (;;)
	    {
		જો ((col += dir) < 0 || col >= len)
		    પાછા FAIL;
		જો (p[col] == c && stop)
		    તોડો;
		stop = TRUE;
	    }
	}
    }

    જો (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	જો (has_mbyte)
	{
	    જો (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    નહીંતર
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    પાછા OK;
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
findmatch(oparg_T *oap, પૂર્ણાંક initc)
{
    પાછા findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    સ્થિર પૂર્ણાંક
check_prevcol(
    char_u	*linep,
    પૂર્ણાંક		col,
    પૂર્ણાંક		ch,
    પૂર્ણાંક		*prevcol)
{
    --col;
    જો (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    જો (prevcol)
	*prevcol = col;
    પાછા (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    સ્થિર પૂર્ણાંક
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    પૂર્ણાંક		found = FALSE;

    માટે (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    જો (delim_copy == ખાલી)
	પાછા FALSE;
    માટે (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *લીટી = ml_get(lnum);

	માટે (p = લીટી + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    જો (lnum == endpos->lnum && (colnr_T)(p - લીટી) >= endpos->col)
		તોડો;
	    જો (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		તોડો;
	    }
	}
	જો (found)
	    તોડો;
    }
    vim_free(delim_copy);
    પાછા found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    સ્થિર શૂન્ય
find_mps_કિંમતો(
    પૂર્ણાંક	    *initc,
    પૂર્ણાંક	    *findc,
    પૂર્ણાંક	    *backwards,
    પૂર્ણાંક	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    જ્યારે (*ptr != NUL)
    {
	જો (has_mbyte)
	{
	    char_u *prev;

	    જો (mb_ptr2char(ptr) == *initc)
	    {
		જો (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		નહીંતર
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		પાછા;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    જો (mb_ptr2char(ptr) == *initc)
	    {
		જો (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		નહીંતર
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		પાછા;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	નહીંતર
	{
	    જો (*ptr == *initc)
	    {
		જો (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		નહીંતર
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		પાછા;
	    }
	    ptr += 2;
	    જો (*ptr == *initc)
	    {
		જો (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		નહીંતર
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		પાછા;
	    }
	    ++ptr;
	}
	જો (*ptr == ',')
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
    પૂર્ણાંક		initc,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		maxtravel)
{
    સ્થિર pos_T pos;			// current search position
    પૂર્ણાંક		findc = 0;		// matching brace
    પૂર્ણાંક		c;
    પૂર્ણાંક		ગણતરી = 0;		// cumulative number of braces
    પૂર્ણાંક		backwards = FALSE;	// init for gcc
    પૂર્ણાંક		raw_string = FALSE;	// search for raw string
    પૂર્ણાંક		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    પૂર્ણાંક		do_quotes;		// check for quotes in current line
    પૂર્ણાંક		at_start;		// do_quotes value at start position
    પૂર્ણાંક		hash_dir = 0;		// Direction searched for # things
    પૂર્ણાંક		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    પૂર્ણાંક		start_in_quotes;	// start position is in quotes
    પૂર્ણાંક		traveled = 0;		// how far we've searched so far
    પૂર્ણાંક		ignore_cend = FALSE;    // ignore comment end
    પૂર્ણાંક		cpo_match;		// vi compatible matching
    પૂર્ણાંક		cpo_bsl;		// don't recognize backslashes
    પૂર્ણાંક		match_escaped = 0;	// search for escaped match
    પૂર્ણાંક		dir;			// Direction to search
    પૂર્ણાંક		comment_col = MAXCOL;   // start of / / comment
    પૂર્ણાંક		lispcomm = FALSE;	// inside of Lisp-style comment
    પૂર્ણાંક		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    પૂર્ણાંક		skip_comments = (flags & FM_SKIPCOMM) != 0;
    પૂર્ણાંક		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != ખાલી);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ખાલી);

    // Direction to search when initc is '/', '*' or '#'
    જો (flags & FM_BACKWARD)
	dir = BACKWARD;
    નહીંતર જો (flags & FM_FORWARD)
	dir = FORWARD;
    નહીંતર
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    જો (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	જો (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    નહીંતર જો (initc != '#' && initc != NUL)
    {
	find_mps_કિંમતો(&initc, &findc, &backwards, TRUE);
	જો (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	જો (findc == NUL)
	    પાછા ખાલી;
    }
    નહીંતર
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	જો (initc == '#')
	{
	    hash_dir = dir;
	}
	નહીંતર
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    જો (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		જો (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    જો (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		નહીંતર જો (linep[pos.col] == '/')
		{
		    જો (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    નહીંતર જો (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		નહીંતર જો (linep[pos.col] == '*')
		{
		    જો (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    નહીંતર જો (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    જો (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		જો (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		માટે (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    જો (initc == NUL)
			તોડો;

		    find_mps_કિંમતો(&initc, &findc, &backwards, FALSE);
		    જો (findc)
			તોડો;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		જો (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    જો (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    નહીંતર
			પાછા ખાલી;
		}
		નહીંતર જો (!cpo_bsl)
		{
		    પૂર્ણાંક col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    માટે (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	જો (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    જો (oap != ખાલી)
		oap->motion_type = MLINE;   // Linewise for this case only
	    જો (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		જો (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		નહીંતર જો (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		નહીંતર
		    પાછા ખાલી;
	    }
	    pos.col = 0;
	    જ્યારે (!got_int)
	    {
		જો (hash_dir > 0)
		{
		    જો (pos.lnum == curbuf->b_ml.ml_લીટી_ગણતરી)
			તોડો;
		}
		નહીંતર જો (pos.lnum == 1)
		    તોડો;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		લીટી_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		જો (*ptr != '#')
		    ચાલુ;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		જો (hash_dir > 0)
		{
		    જો (STRNCMP(ptr, "if", 2) == 0)
			ગણતરી++;
		    નહીંતર જો (STRNCMP(ptr, "el", 2) == 0)
		    {
			જો (ગણતરી == 0)
			    પાછા &pos;
		    }
		    નહીંતર જો (STRNCMP(ptr, "endif", 5) == 0)
		    {
			જો (ગણતરી == 0)
			    પાછા &pos;
			ગણતરી--;
		    }
		}
		નહીંતર
		{
		    જો (STRNCMP(ptr, "if", 2) == 0)
		    {
			જો (ગણતરી == 0)
			    પાછા &pos;
			ગણતરી--;
		    }
		    નહીંતર જો (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			જો (ગણતરી == 0)
			    પાછા &pos;
		    }
		    નહીંતર જો (STRNCMP(ptr, "endif", 5) == 0)
			ગણતરી++;
		}
	    }
	    પાછા ખાલી;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    જો (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ખાલી)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    જો ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    જો (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    જો (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    જ્યારે (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	જો (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    જો (lispcomm && pos.col < (colnr_T)comment_col)
		તોડો;
	    જો (pos.col == 0)		// at start of line, go to prev. one
	    {
		જો (pos.lnum == 1)	// start of file
		    તોડો;
		--pos.lnum;

		જો (maxtravel > 0 && ++traveled > maxtravel)
		    તોડો;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		લીટી_breakcheck();

		// Check if this line contains a single-line comment
		જો (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		જો (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		નહીંતર જો (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    નહીંતર
	    {
		--pos.col;
		જો (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	નહીંતર				// forward search
	{
	    જો (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		જો (pos.lnum == curbuf->b_ml.ml_લીટી_ગણતરી  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    તોડો;
		++pos.lnum;

		જો (maxtravel && traveled++ > maxtravel)
		    તોડો;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		લીટી_breakcheck();
		જો (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    નહીંતર
	    {
		જો (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		નહીંતર
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	જો (skip_comments && !comment_dir && !inquote)
	{
	    જો (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		જો (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (પૂર્ણાંક)pos.col < comment_col))
		    in_block_comment = TRUE;
		નહીંતર જો (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    નહીંતર
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		જો (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (પૂર્ણાંક)pos.col < comment_col))
		    in_block_comment = TRUE;
		નહીંતર જો (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	જો (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    જો (linep[0] == findc && ગણતરી == 0)	// match!
		પાછા &pos;
	    તોડો;					// out of scope
	}

	જો (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    જો (comment_dir == FORWARD)
	    {
		જો (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    પાછા &pos;
		}
	    }
	    નહીંતર    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		જો (pos.col == 0)
		    ચાલુ;
		નહીંતર જો (raw_string)
		{
		    જો (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != ખાલી)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			જો (!find_rawstring_end(linep, &pos,
				  ગણતરી > 0 ? &match_pos : &curwin->w_cursor))
			{
			    ગણતરી++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		નહીંતર જો (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (પૂર્ણાંક)pos.col < comment_col)
		{
		    ગણતરી++;
		    match_pos = pos;
		    match_pos.col--;
		}
		નહીંતર જો (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    જો (ગણતરી > 0)
			pos = match_pos;
		    નહીંતર જો (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (પૂર્ણાંક)pos.col <= comment_col)
			pos.col -= 2;
		    નહીંતર જો (ignore_cend)
			ચાલુ;
		    નહીંતર
			પાછા ખાલી;
		    પાછા &pos;
		}
	    }
	    ચાલુ;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	જો (cpo_match)
	    do_quotes = 0;
	નહીંતર જો (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    માટે (ptr = linep; *ptr; ++ptr)
	    {
		જો (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		જો (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		જો (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    જો (!do_quotes)
	    {
		inquote = FALSE;
		જો (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    જો (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			જો (inquote)
			    start_in_quotes = TRUE;
		    }
		    નહીંતર જો (backwards)
			inquote = TRUE;
		}
		જો (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    જો (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			જો (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    જો (inquote)
				start_in_quotes = TRUE;
			}
			નહીંતર જો (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	જો (start_in_quotes == MAYBE)
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
	પસંદગી (c)
	{
	સ્થિતિ NUL:
	    // at end of line without trailing backslash, reset inquote
	    જો (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    તોડો;

	સ્થિતિ '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    જો (do_quotes)
	    {
		પૂર્ણાંક col;

		માટે (col = pos.col - 1; col >= 0; --col)
		    જો (linep[col] != '\\')
			તોડો;
		જો ((((પૂર્ણાંક)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    તોડો;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	સ્થિતિ '\'':
	    જો (!cpo_match && initc != '\'' && findc != '\'')
	    {
		જો (backwards)
		{
		    જો (pos.col > 1)
		    {
			જો (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    તોડો;
			}
			નહીંતર જો (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    તોડો;
			}
		    }
		}
		નહીંતર જો (linep[pos.col + 1])	// forward search
		{
		    જો (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			તોડો;
		    }
		    નહીંતર જો (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			તોડો;
		    }
		}
	    }
	    // FALLTHROUGH

	મૂળભૂત:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    જો (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ખાલી
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', ખાલી)
		    && check_prevcol(linep, pos.col - 1, '#', ખાલી))
		તોડો;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    જો (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (પૂર્ણાંક)pos.col >= comment_col)))
		તોડો;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    જો ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		પૂર્ણાંક	col, bslcnt = 0;

		જો (!cpo_bsl)
		{
		    માટે (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		જો (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    જો (c == initc)
			ગણતરી++;
		    નહીંતર
		    {
			જો (ગણતરી == 0)
			    પાછા &pos;
			ગણતરી--;
		    }
		}
	    }
	}
    }

    જો (comment_dir == BACKWARD && ગણતરી > 0)
    {
	pos = match_pos;
	પાછા &pos;
    }
    પાછા (pos_T *)ખાલી;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    શૂન્ય
showmatch(
    પૂર્ણાંક		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    પૂર્ણાંક		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    માટે (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	જો (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    તોડો;
#endif
	p += mb_ptr2len(p) + 1;
	જો (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    તોડો;
	p += mb_ptr2len(p);
	જો (*p == NUL)
	    પાછા;
    }
    જો (*p == NUL)
	પાછા;

    જો ((lpos = findmatch(ખાલી, NUL)) == ખાલી)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	પાછા;
    }

    જો (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	પાછા;

    જો (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ખાલી, &vcol, ખાલી, 0);

    પૂર્ણાંક col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    જો (!col_visible)
	પાછા;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    જો (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    જો (vim_strchr(p_cpo, CPO_SHOWMATCH) != ખાલી)
	ui_delay(p_mat * 100L + 8, TRUE);
    નહીંતર જો (!char_avail())
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
    સ્થિર પૂર્ણાંક
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    પૂર્ણાંક		move,
    pos_T	*cur,
    પૂર્ણાંક		direction)
{
    regmmatch_T	regmatch;
    પૂર્ણાંક		nmatched = 0;
    પૂર્ણાંક		result = -1;
    pos_T	pos;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    પૂર્ણાંક		flag = 0;

    જો (pattern == ખાલી)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    જો (શોધ_regcomp(pattern, patternlen, ખાલી, RE_શોધ, RE_શોધ,
					      શોધ_KEEP, &regmatch) == FAIL)
	પાછા -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    જો (move)
    {
	CLEAR_POS(&pos);
    }
    નહીંતર
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = શોધ_START;
    }

    જો (searchit(curwin, curbuf, &pos, ખાલી, direction, pattern, patternlen, 1,
				  શોધ_KEEP + flag, RE_શોધ, ખાલી) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	કરો
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, ખાલી);
	    જો (nmatched != 0)
		તોડો;
	} જ્યારે (regmatch.regprog != ખાલી
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	જો (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    પાછા result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    પૂર્ણાંક
current_શોધ(
    long	ગણતરી,
    પૂર્ણાંક		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    પૂર્ણાંક		i;
    પૂર્ણાંક		dir;
    પૂર્ણાંક		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    પૂર્ણાંક		flags = 0;
    pos_T	save_VIsual = VIsual;
    પૂર્ણાંક		zero_width;
    પૂર્ણાંક		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    જો (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    જો (VIsual_active)
    {
	જો (forward)
	    incl(&pos);
	નહીંતર
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    જો (zero_width == -1)
	પાછા FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    માટે (i = 0; i < 2; i++)
    {
	જો (forward)
	{
	    જો (i == 0 && skip_first_backward)
		ચાલુ;
	    dir = i;
	}
	નહીંતર
	    dir = !i;

	flags = 0;
	જો (!dir && !zero_width)
	    flags = શોધ_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	જો (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? ગણતરી : 1),
		શોધ_KEEP | flags, RE_શોધ, ખાલી);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	જો (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    જો (VIsual_active)
		VIsual = save_VIsual;
	    પાછા FAIL;
	}
	નહીંતર જો (i == 0 && !result)
	{
	    જો (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    નહીંતર
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_લીટી_ગણતરી;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_લીટી_ગણતરી);
	    }
	}
    }

    start_pos = pos;

    જો (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    જો (LT_POS(VIsual, end_pos) && forward)
    {
	જો (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	નહીંતર
	    // put the cursor on last character of match
	    dec_cursor();
    }
    નહીંતર જો (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    જો (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	જો (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	નહીંતર જો (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    જો (fdo_flags & FDO_શોધ && ચાવી_Typed)
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

    પાછા OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    પૂર્ણાંક
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    પાછા (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    સ્થિર શૂન્ય
cmdline_શોધ_stat(
    પૂર્ણાંક		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    પૂર્ણાંક		show_ટોચ_bot_સંદેશ,
    char_u	*msgbuf,
    size_t	msgbuflen,
    પૂર્ણાંક		recompute,
    પૂર્ણાંક		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_શોધ_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    જો (stat.cur <= 0)
	પાછા;

    અક્ષર	t[શોધ_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	જો (stat.incomplete == 1)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[?/??]");
	નહીંતર જો (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	નહીંતર જો (stat.cnt > maxcount)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	નહીંતર
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    નહીંતર
#endif
    {
	જો (stat.incomplete == 1)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[?/??]");
	નહીંતર જો (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	નહીંતર જો (stat.cnt > maxcount)
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	નહીંતર
	    len = vim_snprintf(t, શોધ_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    જો (show_ટોચ_bot_સંદેશ && len + 2 < શોધ_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    જો (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    જો (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    સંદેશ_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    સંદેશ_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    સ્થિર શૂન્ય
update_શોધ_stat(
    પૂર્ણાંક			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    પૂર્ણાંક			recompute,
    પૂર્ણાંક			maxcount,
    long		timeout UNUSED)
{
    પૂર્ણાંક		    save_ws = p_ws;
    પૂર્ણાંક		    wraparound = FALSE;
    pos_T	    p = (*pos);
    સ્થિર pos_T    lastpos = {0, 0, 0};
    સ્થિર પૂર્ણાંક	    cur = 0;
    સ્થિર પૂર્ણાંક	    cnt = 0;
    સ્થિર પૂર્ણાંક	    exact_match = FALSE;
    સ્થિર પૂર્ણાંક	    incomplete = 0;
    સ્થિર પૂર્ણાંક	    last_maxcount = 0;
    સ્થિર પૂર્ણાંક	    chgtick = 0;
    સ્થિર char_u   *lastpat = ખાલી;
    સ્થિર size_t   lastpatlen = 0;
    સ્થિર buf_T    *lbuf = ખાલી;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    જો (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	પાછા;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    જો (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ખાલી
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
    જો (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    નહીંતર
    {
	પૂર્ણાંક	done_શોધ = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	જો (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	જ્યારે (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ખાલી, 0, 1, શોધ_KEEP, RE_LAST, ખાલી) != FAIL)
	{
	    done_શોધ = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    જો (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		તોડો;
	    }
#endif
	    cnt++;
	    જો (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		જો (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    જો (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		તોડો;
	    }
	}
	જો (got_int)
	    cur = -1; // abort
	જો (done_શોધ)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    જો (lastpat == ખાલી)
		lastpatlen = 0;
	    નહીંતર
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

#જો defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    સ્થિર char_u *
get_લીટી_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *લીટી = ml_get(lnum);

    vim_strncpy(buf, લીટી, LSIZE - 1);
    પાછા buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    શૂન્ય
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    પૂર્ણાંક		dir UNUSED,	// direction of expansion
    પૂર્ણાંક		len,		// length of search pattern
    પૂર્ણાંક		whole,		// match whole words only
    પૂર્ણાંક		skip_comments,	// don't match inside comments
    પૂર્ણાંક		type,		// Type of search; are we looking for a type?
				// a macro?
    long	ગણતરી,
    પૂર્ણાંક		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    પૂર્ણાંક		forceit,	// If true, always switch to the found path
    પૂર્ણાંક		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    પૂર્ણાંક		max_path_depth = 50;
    long	match_ગણતરી = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = ખાલી;
    linenr_T	lnum;
    પૂર્ણાંક		depth;
    પૂર્ણાંક		depth_displayed;	// For type==CHECK_PATH
    પૂર્ણાંક		old_files;
    પૂર્ણાંક		already_searched;
    char_u	*file_લીટી;
    char_u	*લીટી;
    char_u	*p;
    char_u	save_char;
    પૂર્ણાંક		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    પૂર્ણાંક		matched = FALSE;
    પૂર્ણાંક		did_show = FALSE;
    પૂર્ણાંક		found = FALSE;
    પૂર્ણાંક		i;
    char_u	*already = ખાલી;
    char_u	*startp = ખાલી;
    char_u	*inc_opt = ખાલી;
# જો defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ખાલી;
# endif

    regmatch.regprog = ખાલી;
    incl_regmatch.regprog = ખાલી;
    def_regmatch.regprog = ખાલી;

    file_લીટી = alloc(LSIZE);
    જો (file_લીટી == ખાલી)
	પાછા;

    જો (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	જો (pat == ખાલી)
	    જાઓ fpip_end;
	vim_snprintf((અક્ષર *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	જો (regmatch.regprog == ખાલી)
	    જાઓ fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    જો (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	જો (incl_regmatch.regprog == ખાલી)
	    જાઓ fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    જો (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	જો (def_regmatch.regprog == ખાલી)
	    જાઓ fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * કદ(SearchedFile), TRUE);
    જો (files == ખાલી)
	જાઓ fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    જો (end_lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	end_lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
    જો (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    લીટી = get_લીટી_and_copy(lnum, file_લીટી);

    માટે (;;)
    {
	જો (incl_regmatch.regprog != ખાલી
		&& vim_regexec(&incl_regmatch, લીટી, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    જો (strstr((અક્ષર *)inc_opt, "\\zs") != ખાલી)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (પૂર્ણાંક)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    નહીંતર
		// Use text after match with 'include'.
		new_fname = file_name_in_લીટી(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ખાલી);
	    already_searched = FALSE;
	    જો (new_fname != ખાલી)
	    {
		// Check whether we have already searched in this file
		માટે (i = 0;; i++)
		{
		    જો (i == depth + 1)
			i = old_files;
		    જો (i == max_path_depth)
			તોડો;
		    જો (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			જો (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    સંદેશ_putchar('\n');	    // cursor below last one
			    જો (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				સંદેશ_home_replace_hl(new_fname);
				સંદેશ_puts(_(" (includes previously listed match)"));
				prev_fname = ખાલી;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			તોડો;
		    }
		}
	    }

	    જો (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == ખાલી && !already_searched)))
	    {
		જો (did_show)
		    સંદેશ_putchar('\n');	    // cursor below last one
		નહીંતર
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    સંદેશ_puts_title(_("--- Included files "));
		    જો (action != ACTION_SHOW_ALL)
			સંદેશ_puts_title(_("not found "));
		    સંદેશ_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		જ્યારે (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    માટે (i = 0; i < depth_displayed; i++)
			સંદેશ_puts("  ");
		    સંદેશ_home_replace(files[depth_displayed].name);
		    સંદેશ_puts(" -->\n");
		}
		જો (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    માટે (i = 0; i <= depth_displayed; i++)
			સંદેશ_puts("  ");
		    જો (new_fname != ખાલી)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			સંદેશ_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    નહીંતર
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			જો (strstr((અક્ષર *)inc_opt, "\\zs") != ખાલી)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (પૂર્ણાંક)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			નહીંતર
			{
			    // find the file name after the end of the match
			    માટે (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    માટે (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			જો (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (પૂર્ણાંક)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			નહીંતર જો (p > લીટી)
			{
			    જો (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    જો (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			સંદેશ_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    જો (new_fname == ખાલી && action == ACTION_SHOW_ALL)
		    {
			જો (already_searched)
			    સંદેશ_puts(_("  (Already listed)"));
			નહીંતર
			    સંદેશ_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    જો (new_fname != ખાલી)
	    {
		// Push the new file onto the file stack
		જો (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    જો (bigger != ખાલી)
		    {
			માટે (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			માટે (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = ખાલી;
			    bigger[i].name = ખાલી;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			માટે (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		જો ((files[depth + 1].fp = mch_fopen((અક્ષર *)new_fname, "r"))
								    == ખાલી)
		    vim_free(new_fname);
		નહીંતર
		{
		    જો (++depth == old_files)
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
		    જો (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			સંદેશ_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((અક્ષર*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(અક્ષર *)new_fname);
			સંદેશ_trunc_attr((અક્ષર *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    નહીંતર જો (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (અક્ષર *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	નહીંતર
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = લીટી;
શોધ_લીટી:
	    define_matched = FALSE;
	    જો (def_regmatch.regprog != ખાલી
			      && vim_regexec(&def_regmatch, લીટી, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		જ્યારે (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    જો (def_regmatch.regprog == ખાલી || define_matched)
	    {
		જો (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    જો (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    નહીંતર
			matched = STRNCMP(startp, ptr, len) == 0;
		    જો (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		નહીંતર જો (regmatch.regprog != ખાલી
			 && vim_regexec(&regmatch, લીટી, (colnr_T)(p - લીટી)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    જો (!define_matched && skip_comments)
		    {
			જો ((*લીટી != '#' ||
				STRNCMP(skipwhite(લીટી + 1), "define", 6) != 0)
				&& get_leader_len(લીટી, ખાલી, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(લીટી);
			જો (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    માટે (p = લીટી; *p && p < startp; ++p)
			    {
				જો (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    જો (p[1] == '/')
					તોડો;
				    ++p;
				}
				નહીંતર જો (!matched && p[0] == '*' && p[1] == '/')
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
	જો (matched)
	{
	    જો (action == ACTION_EXPAND)
	    {
		પૂર્ણાંક	cont_s_ipos = FALSE;
		પૂર્ણાંક	add_r;
		char_u	*aux;

		જો (depth == -1 && lnum == curwin->w_cursor.lnum)
		    તોડો;
		found = TRUE;
		aux = p = startp;
		જો (compl_status_adding() && (પૂર્ણાંક)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    જો (vim_iswordp(p))
			જાઓ exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (પૂર્ણાંક)(p - aux);

		જો (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    જો (depth < 0)
		    {
			જો (lnum >= end_lnum)
			    જાઓ exit_matched;
			લીટી = get_લીટી_and_copy(++lnum, file_લીટી);
		    }
		    નહીંતર જો (vim_fgets(લીટી = file_લીટી,
						      LSIZE, files[depth].fp))
			જાઓ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(લીટી);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    જો (p > aux)
		    {
			જો (*aux != ')' && IObuff[i-1] != TAB)
			{
			    જો (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    જો (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ખાલી
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			જો (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (પૂર્ણાંક)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    જો (i == ins_compl_len())
			જાઓ exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ખાલી
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		જો (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		નહીંતર જો (add_r == FAIL)
		    તોડો;
	    }
	    નહીંતર જો (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		જો (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		જો (curr_fname != prev_fname)
		{
		    જો (did_show)
			સંદેશ_putchar('\n');	// cursor below last one
		    જો (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			સંદેશ_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		જો (!got_int)
		    show_pat_in_path(લીટી, type, TRUE, action,
			    (depth == -1) ? ખાલી : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_ગણતરી++);

		// Set matched flag for this file and all the ones that
		// include it
		માટે (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    નહીંતર જો (--ગણતરી <= 0)
	    {
		found = TRUE;
		જો (depth == -1 && lnum == curwin->w_cursor.lnum
# જો defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_લીટી));
		નહીંતર જો (action == ACTION_SHOW)
		{
		    show_pat_in_path(લીટી, type, did_show, action,
			    (depth == -1) ? ખાલી : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		નહીંતર
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# જો defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    જો (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    જો (action == ACTION_SPLIT)
		    {
			જો (win_split(0, 0) == FAIL)
			    તોડો;
			RESET_BINDING(curwin);
		    }
		    જો (depth == -1)
		    {
			// match in current file
# જો defined(FEAT_QUICKFIX)
			જો (g_do_tagpreview != 0)
			{
			    જો (!win_valid(curwin_save))
				તોડો;
			    જો (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, ખાલી,
						     ખાલી, TRUE, lnum, forceit)))
				તોડો;	// failed to jump to file
			}
			નહીંતર
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    નહીંતર
		    {
			જો (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, ખાલી, TRUE,
						    files[depth].lnum, forceit)))
			    તોડો;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		જો (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - લીટી);
		    curwin->w_set_curswant = true;
		}

# જો defined(FEAT_QUICKFIX)
		જો (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		નહીંતર જો (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		તોડો;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    જો (def_regmatch.regprog == ખાલી
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		જાઓ શોધ_લીટી;
	}
	લીટી_breakcheck();
	જો (action == ACTION_EXPAND)
	    ins_compl_check_ચાવીઓ(30, FALSE);
	જો (got_int || ins_compl_interrupted())
	    તોડો;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	જ્યારે (depth >= 0 && !already
		&& vim_fgets(લીટી = file_લીટી, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    જો (depth < depth_displayed)
		depth_displayed = depth;
	}
	જો (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (પૂર્ણાંક)STRLEN(લીટી);
	    જો (i > 0 && લીટી[i - 1] == '\n')
		લીટી[--i] = NUL;
	    જો (i > 0 && લીટી[i - 1] == '\r')
		લીટી[--i] = NUL;
	}
	નહીંતર જો (!already)
	{
	    જો (++lnum > end_lnum)
		તોડો;
	    લીટી = get_લીટી_and_copy(lnum, file_લીટી);
	}
	already = ખાલી;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    માટે (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    માટે (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    જો (type == CHECK_PATH)
    {
	જો (!did_show)
	{
	    જો (action != ACTION_SHOW_ALL)
		સંદેશ(_("All included files were found"));
	    નહીંતર
		સંદેશ(_("No included files"));
	}
    }
    નહીંતર જો (!found && action != ACTION_EXPAND && !silent)
    {
	જો (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	નહીંતર જો (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	નહીંતર
	    emsg(_(e_couldnt_find_pattern));
    }
    જો (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	સંદેશ_end();

fpip_end:
    vim_free(file_લીટી);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    સ્થિર શૂન્ય
show_pat_in_path(
    char_u  *લીટી,
    પૂર્ણાંક	    type,
    પૂર્ણાંક	    did_show,
    પૂર્ણાંક	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    ગણતરી)
{
    char_u  *p;
    size_t  linelen;

    જો (did_show)
	સંદેશ_putchar('\n');	// cursor below last one
    નહીંતર જો (!સંદેશ_silent)
	gotocmdline(TRUE);	// cursor at status line
    જો (got_int)		// 'q' typed at "--more--" message
	પાછા;
    linelen = STRLEN(લીટી);
    માટે (;;)
    {
	p = લીટી + linelen - 1;
	જો (fp != ખાલી)
	{
	    // We used fgets(), so get rid of newline at end
	    જો (p >= લીટી && *p == '\n')
		--p;
	    જો (p >= લીટી && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	જો (action == ACTION_SHOW_ALL)
	{
	    sprintf((અક્ષર *)IObuff, "%3ld: ", ગણતરી);	// show match nr
	    સંદેશ_puts((અક્ષર *)IObuff);
	    sprintf((અક્ષર *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    સંદેશ_puts_attr((અક્ષર *)IObuff, HL_ATTR(HLF_N));
	    સંદેશ_puts(" ");
	}
	સંદેશ_prt_લીટી(લીટી, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	જો (got_int || type != FIND_DEFINE || p < લીટી || *p != '\\')
	    તોડો;

	જો (fp != ખાલી)
	{
	    જો (vim_fgets(લીટી, LSIZE, fp)) // end of file
		તોડો;
	    linelen = STRLEN(લીટી);
	    ++*lnum;
	}
	નહીંતર
	{
	    જો (++*lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    લીટી = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	સંદેશ_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(પૂર્ણાંક idx)
{
    પાછા &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    પૂર્ણાંક
get_spat_last_idx(શૂન્ય)
{
    પાછા last_idx;
}
#endif

#જો defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    શૂન્ય
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = ખાલી;
    પૂર્ણાંક			maxcount = p_msc;
    long		timeout = શોધ_STAT_DEF_TIMEOUT;
    પૂર્ણાંક			recompute = TRUE;
    searchstat_T	stat;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	પૂર્ણાંક		error = FALSE;

	જો (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    પાછા;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	જો (di != ખાલી)
	{
	    timeout = (long)tv_get_સંખ્યા_chk(&di->di_tv, &error);
	    જો (error)
		પાછા;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	જો (di != ખાલી)
	{
	    maxcount = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&di->di_tv, &error);
	    જો (error)
		પાછા;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	જો (di != ખાલી)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    જો (pattern == ખાલી)
		પાછા;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	જો (di != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		પાછા;
	    }
	    જો (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		પાછા;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    જો (li != ખાલી)
	    {
		pos.lnum = tv_get_સંખ્યા_chk(&li->li_tv, &error);
		જો (error)
		    પાછા;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    જો (li != ખાલી)
	    {
		pos.col = tv_get_સંખ્યા_chk(&li->li_tv, &error) - 1;
		જો (error)
		    પાછા;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    જો (li != ખાલી)
	    {
		pos.coladd = tv_get_સંખ્યા_chk(&li->li_tv, &error);
		જો (error)
		    પાછા;
	    }
	}
    }

    save_last_શોધ_pattern();
# ifdef FEAT_શોધ_EXTRA
    save_incsearch_state();
# endif
    જો (pattern != ખાલી)
    {
	જો (*pattern == NUL)
	    જાઓ the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	જો (spats[last_idx].pat == ખાલી)
	    spats[last_idx].patlen = 0;
    }
    જો (spats[last_idx].pat == ખાલી || *spats[last_idx].pat == NUL)
	જાઓ the_end;	// the previous pattern was never defined

    update_શોધ_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_સંખ્યા(rettv->vval.v_dict, "current", stat.cur);
    dict_add_સંખ્યા(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_સંખ્યા(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_સંખ્યા(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_સંખ્યા(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_શોધ_pattern();
# ifdef FEAT_શોધ_EXTRA
    restore_incsearch_state();
# endif
}
#endif
