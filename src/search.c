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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_EVAL
நிலையான வெறுமை set_vv_searchforward(வெறுமை);
நிலையான முழு first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
நிலையான char_u *get_வரி_and_copy(linenr_T lnum, char_u *buf);
நிலையான வெறுமை show_pat_in_path(char_u *, முழு, முழு, முழு, FILE *, linenr_T *, long);
#endif

வகைவரை கட்டமைப்பு searchstat
{
    முழு	    cur;	    // current position of found words
    முழு	    cnt;	    // total count of found words
    முழு	    exact_match;    // TRUE if matched exactly on specified position
    முழு	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    முழு	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_தேடல்_EXTRA
நிலையான வெறுமை save_incsearch_state(வெறுமை);
நிலையான வெறுமை restore_incsearch_state(வெறுமை);
#endif
நிலையான முழு check_prevcol(char_u *linep, முழு col, முழு ch, முழு *prevcol);
நிலையான முழு find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
நிலையான வெறுமை find_mps_மதிப்புகள்(முழு *initc, முழு *findc, முழு *backwards, முழு switchit);
நிலையான முழு is_zero_width(char_u *pattern, size_t patternlen, முழு move, pos_T *cur, முழு direction);
நிலையான வெறுமை cmdline_தேடல்_stat(முழு dirc, pos_T *pos, pos_T *cursor_pos, முழு show_உச்சி_bot_செய்தி, char_u *msgbuf, size_t msgbuflen, முழு recompute, முழு maxcount, long timeout);
நிலையான வெறுமை update_தேடல்_stat(முழு dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, முழு recompute, முழு maxcount, long timeout);

#define தேடல்_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define தேடல்_STAT_BUF_LEN 16

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
நிலையான spat_T spats[2] =
{
    {வெற்று, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {வெற்று, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

நிலையான முழு last_idx = 0;	// index in spats[] for RE_LAST

நிலையான char_u lastc[2] = {NUL, NUL};	// last character searched for
நிலையான முழு lastcdir = FORWARD;		// last direction of character search
நிலையான முழு last_t_cmd = TRUE;		// last search t_cmd
நிலையான char_u	lastc_bytes[MB_MAXBYTES + 1];
நிலையான முழு	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
நிலையான spat_T	    saved_spats[ARRAY_LENGTH(spats)];
நிலையான char_u	    *saved_mr_pattern = வெற்று;
நிலையான size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_தேடல்_EXTRA
நிலையான முழு	    saved_spats_last_idx = 0;
நிலையான முழு	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
நிலையான char_u	    *mr_pattern = வெற்று;
நிலையான size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
வகைவரை கட்டமைப்பு SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    முழு		matched;	// Found a match in this file
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
    முழு
தேடல்_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    முழு		pat_save,
    முழு		pat_use,
    முழு		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    முழு		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    எனில் (pat == வெற்று || *pat == NUL)
    {
	முழு i;

	எனில் (pat_use == RE_LAST)
	    i = last_idx;
	இல்லையெனில்
	    i = pat_use;
	எனில் (spats[i].pat == வெற்று)	// pattern was never defined
	{
	    எனில் (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    இல்லையெனில்
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    திரும்பு FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    இல்லையெனில் எனில் (options & தேடல்_HIS)	// put new pattern in history
	add_to_history(HIST_தேடல், pat, patlen, TRUE, NUL);

    எனில் (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    இல்லையெனில்
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    எனில் (mr_pattern == வெற்று)
	mr_patternlen = 0;
    இல்லையெனில்
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    எனில் (!(options & தேடல்_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	எனில் (pat_save == RE_தேடல் || pat_save == RE_BOTH)
	    save_re_pat(RE_தேடல், pat, patlen, magic);
	// substitute or global command
	எனில் (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    எனில் (regmatch->regprog == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_தேடல்_pat(வெறுமை)
{
    திரும்பு mr_pattern;
}

    வெறுமை
save_re_pat(முழு idx, char_u *pat, size_t patlen, முழு magic)
{
    எனில் (spats[idx].pat == pat)
	திரும்பு;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    எனில் (spats[idx].pat == வெற்று)
	spats[idx].patlen = 0;
    இல்லையெனில்
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_தேடல்_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    எனில் (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
நிலையான முழு save_level = 0;

    வெறுமை
save_தேடல்_patterns(வெறுமை)
{
    முழு i;

    எனில் (save_level++ != 0)
	திரும்பு;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	எனில் (spats[i].pat != வெற்று)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    எனில் (saved_spats[i].pat == வெற்று)
		saved_spats[i].patlen = 0;
	    இல்லையெனில்
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    எனில் (mr_pattern == வெற்று)
	saved_mr_pattern = வெற்று;
    இல்லையெனில்
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    எனில் (saved_mr_pattern == வெற்று)
	saved_mr_patternlen = 0;
    இல்லையெனில்
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_தேடல்_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    வெறுமை
restore_தேடல்_patterns(வெறுமை)
{
    முழு i;

    எனில் (--save_level != 0)
	திரும்பு;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#எனில் defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_தேடல்_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#எனில் defined(EXITFREE)
    வெறுமை
free_தேடல்_patterns(வெறுமை)
{
    முழு i;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_தேடல்_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
நிலையான spat_T	    saved_last_தேடல்_spat;
நிலையான முழு	    did_save_last_தேடல்_spat = 0;
நிலையான முழு	    saved_last_idx = 0;
நிலையான முழு	    saved_no_hlsearch = 0;
நிலையான முழு	    saved_தேடல்_match_endcol;
நிலையான முழு	    saved_தேடல்_match_வரிகள்;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    வெறுமை
save_last_தேடல்_pattern(வெறுமை)
{
    எனில் (++did_save_last_தேடல்_spat != 1)
	// nested call, nothing to do
	திரும்பு;

    saved_last_தேடல்_spat = spats[RE_தேடல்];
    எனில் (spats[RE_தேடல்].pat != வெற்று)
    {
	saved_last_தேடல்_spat.pat = vim_strnsave(spats[RE_தேடல்].pat, spats[RE_தேடல்].patlen);
	எனில் (saved_last_தேடல்_spat.pat == வெற்று)
	    saved_last_தேடல்_spat.patlen = 0;
	இல்லையெனில்
	    saved_last_தேடல்_spat.patlen = spats[RE_தேடல்].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    வெறுமை
restore_last_தேடல்_pattern(வெறுமை)
{
    எனில் (--did_save_last_தேடல்_spat > 0)
	// nested call, nothing to do
	திரும்பு;
    எனில் (did_save_last_தேடல்_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	திரும்பு;
    }

    vim_free(spats[RE_தேடல்].pat);
    spats[RE_தேடல்] = saved_last_தேடல்_spat;
    saved_last_தேடல்_spat.pat = வெற்று;
    saved_last_தேடல்_spat.patlen = 0;
# எனில் defined(FEAT_EVAL)
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
    நிலையான வெறுமை
save_incsearch_state(வெறுமை)
{
    saved_தேடல்_match_endcol = தேடல்_match_endcol;
    saved_தேடல்_match_வரிகள்  = தேடல்_match_வரிகள்;
}

    நிலையான வெறுமை
restore_incsearch_state(வெறுமை)
{
    தேடல்_match_endcol = saved_தேடல்_match_endcol;
    தேடல்_match_வரிகள்  = saved_தேடல்_match_வரிகள்;
}

    char_u *
last_தேடல்_pattern(வெறுமை)
{
    திரும்பு spats[RE_தேடல்].pat;
}

    size_t
last_தேடல்_pattern_len(வெறுமை)
{
    திரும்பு spats[RE_தேடல்].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    முழு
ignorecase(char_u *pat)
{
    திரும்பு ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    முழு
ignorecase_opt(char_u *pat, முழு ic_in, முழு scs)
{
    முழு		ic = ic_in;

    எனில் (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    திரும்பு ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    முழு
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (வெறுமை)skip_regexp_ex(pat, NUL, magic_isset(), வெற்று, வெற்று, &magic_val);

    போது (*p != NUL)
    {
	முழு		l;

	எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    எனில் (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		திரும்பு TRUE;
	    p += l;
	}
	இல்லையெனில் எனில் (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    எனில் (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    இல்லையெனில் எனில் (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    இல்லையெனில் எனில் (p[1] != NUL)  // skip "\X"
		p += 2;
	    இல்லையெனில்
		p += 1;
	}
	இல்லையெனில் எனில் ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    எனில் (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    இல்லையெனில்
		p++;
	}
	இல்லையெனில் எனில் (MB_ISUPPER(*p))
	    திரும்பு TRUE;
	இல்லையெனில்
	    ++p;
    }
    திரும்பு FALSE;
}

#எனில் defined(FEAT_EVAL)
    char_u *
last_csearch(வெறுமை)
{
    திரும்பு lastc_bytes;
}

    முழு
last_csearch_forward(வெறுமை)
{
    திரும்பு lastcdir == FORWARD;
}

    முழு
last_csearch_until(வெறுமை)
{
    திரும்பு last_t_cmd == TRUE;
}

    வெறுமை
set_last_csearch(முழு c, char_u *s, முழு len)
{
    *lastc = c;
    lastc_bytelen = len;
    எனில் (len)
	memcpy(lastc_bytes, s, len);
    இல்லையெனில்
	CLEAR_FIELD(lastc_bytes);
}
#endif

    வெறுமை
set_csearch_direction(முழு cdir)
{
    lastcdir = cdir;
}

    வெறுமை
set_csearch_until(முழு t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_தேடல்_pat(வெறுமை)
{
    திரும்பு spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    வெறுமை
reset_தேடல்_dir(வெறுமை)
{
    spats[0].off.dir = '/';
#எனில் defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    வெறுமை
set_last_தேடல்_pat(
    char_u	*s,
    முழு		idx,
    முழு		magic,
    முழு		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    எனில் (*s == NUL)
	spats[idx].pat = வெற்று;
    இல்லையெனில்
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    எனில் (spats[idx].pat == வெற்று)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# எனில் defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.வரி = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    எனில் (setlast)
	last_idx = idx;
    எனில் (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	எனில் (spats[idx].pat == வெற்று)
	    saved_spats[idx].pat = வெற்று;
	இல்லையெனில்
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	எனில் (saved_spats[idx].pat == வெற்று)
	    saved_spats[idx].patlen = 0;
	இல்லையெனில்
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_தேடல்_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_தேடல்_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    எனில் (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_தேடல்_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    வெறுமை
last_pat_prog(regmmatch_T *regmatch)
{
    எனில் (spats[last_idx].pat == வெற்று)
    {
	regmatch->regprog = வெற்று;
	திரும்பு;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (வெறுமை)தேடல்_regcomp((char_u *)"", 0, வெற்று, 0, last_idx, தேடல்_KEEP, regmatch);
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
    முழு
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    முழு		dir,
    char_u	*pat,
    size_t	patlen,
    long	மொத்தம்,
    முழு		options,
    முழு		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    முழு		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    முழு		loop;
    pos_T	start_pos;
    முழு		at_first_வரி;
    முழு		extra_col;
    முழு		start_char_len;
    முழு		match_ok;
    long	nmatched;
    முழு		submatch = 0;
    முழு		first_match = TRUE;
    முழு		called_emsg_before = called_emsg;
#ifdef FEAT_தேடல்_EXTRA
    முழு		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    முழு		unused_timeout_flag = FALSE;
    முழு		*timed_out = &unused_timeout_flag;  // set when timed out.
    முழு		தேடல்_from_match_end;		    // vi-compatible search?

    எனில் (தேடல்_regcomp(pat, patlen, வெற்று, RE_தேடல், pat_use,
		   (options & (தேடல்_HIS + தேடல்_KEEP)), &regmatch) == FAIL)
    {
	எனில் ((options & தேடல்_செய்தி) && !rc_did_emsg)
	    semsg(_(e_invalid_தேடல்_string_str), mr_pattern);
	திரும்பு FAIL;
    }

    தேடல்_from_match_end = vim_strchr(p_cpo, CPO_தேடல்) != வெற்று;

    எனில் (extra_arg != வெற்று)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	எனில் (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    செய்	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	எனில் (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	இல்லையெனில் எனில் (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_வரி_மொத்தம்
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    எனில் (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    இல்லையெனில்
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	இல்லையெனில்
	    start_char_len = 1;
	எனில் (dir == FORWARD)
	{
	    எனில் (options & தேடல்_START)
		extra_col = 0;
	    இல்லையெனில்
		extra_col = start_char_len;
	}
	இல்லையெனில்
	{
	    எனில் (options & தேடல்_START)
		extra_col = start_char_len;
	    இல்லையெனில்
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_வரி = TRUE;	// default: start in first line
	எனில் (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_வரி = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	எனில் (dir == BACKWARD && start_pos.col == 0
					     && (options & தேடல்_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_வரி = FALSE;
	}
	இல்லையெனில்
	    lnum = pos->lnum;

	க்கு (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    க்கு ( ; lnum > 0 && lnum <= buf->b_ml.ml_வரி_மொத்தம்;
					   lnum += dir, at_first_வரி = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		எனில் (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    நிறுத்து;
		// Stop after passing the time limit.
		எனில் (*timed_out)
		    நிறுத்து;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_வரி && (options & தேடல்_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		எனில் (regmatch.regprog == வெற்று)
		    நிறுத்து;
		// Abort searching on an error (e.g., out of stack).
		எனில் (called_emsg > called_emsg_before || *timed_out)
		    நிறுத்து;
		எனில் (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    எனில் (lnum + matchpos.lnum > buf->b_ml.ml_வரி_மொத்தம்)
			ptr = (char_u *)"";
		    இல்லையெனில்
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    எனில் (dir == FORWARD && at_first_வரி)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			போது (matchpos.lnum == 0
				&& ((options & தேடல்_END) && first_match
				    ?  (nmatched == 1
					&& (முழு)endpos.col - 1
					     < (முழு)start_pos.col + extra_col)
				    : ((முழு)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (முழு)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    எனில் (தேடல்_from_match_end)
			    {
				எனில் (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    நிறுத்து;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				எனில் (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    எனில் (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    இல்லையெனில்
					++matchcol;
				}
			    }
			    இல்லையெனில்
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				எனில் (ptr[matchcol] != NUL)
				{
				    எனில் (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    இல்லையெனில்
					++matchcol;
				}
			    }
			    எனில் (matchcol == 0 && (options & தேடல்_START))
				நிறுத்து;
			    எனில் (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				நிறுத்து;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    எனில் (regmatch.regprog == வெற்று)
				நிறுத்து;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			எனில் (!match_ok)
			    தொடர்;
		    }
		    எனில் (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			க்கு (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    எனில் (loop
				|| ((options & தேடல்_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (முழு)regmatch.endpos[0].col - 1
							< (முழு)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (முழு)regmatch.startpos[0].col
						      < (முழு)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    இல்லையெனில்
				நிறுத்து;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    எனில் (தேடல்_from_match_end)
			    {
				எனில் (nmatched > 1)
				    நிறுத்து;
				matchcol = endpos.col;
				// for empty match: advance one char
				எனில் (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    எனில் (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    இல்லையெனில்
					++matchcol;
				}
			    }
			    இல்லையெனில்
			    {
				// Stop when the match is in a next line.
				எனில் (matchpos.lnum > 0)
				    நிறுத்து;
				matchcol = matchpos.col;
				எனில் (ptr[matchcol] != NUL)
				{
				    எனில் (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    இல்லையெனில்
					++matchcol;
				}
			    }
			    எனில் (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				எனில் (*timed_out)
				    match_ok = FALSE;
				நிறுத்து;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    எனில் (regmatch.regprog == வெற்று)
				நிறுத்து;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			எனில் (!match_ok)
			    தொடர்;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    எனில் ((options & தேடல்_END) && !(options & தேடல்_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			எனில் (endpos.col == 0)
			{
			    எனில் (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			இல்லையெனில்
			{
			    --pos->col;
			    எனில் (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_வரி_மொத்தம்)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			எனில் (end_pos != வெற்று)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    இல்லையெனில்
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			எனில் (end_pos != வெற்று)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    எனில் (end_pos != வெற்று)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    தேடல்_match_வரிகள் = endpos.lnum - matchpos.lnum;
		    தேடல்_match_endcol = endpos.col;
		    நிறுத்து;
		}
		வரி_breakcheck();	// stop if ctrl-C typed
		எனில் (got_int)
		    நிறுத்து;

#ifdef FEAT_தேடல்_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		எனில் ((options & தேடல்_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    நிறுத்து;
		}
#endif

		எனில் (loop && lnum == start_pos.lnum)
		    நிறுத்து;	    // if second loop, stop where started
	    }
	    at_first_வரி = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    எனில் (regmatch.regprog == வெற்று)
		நிறுத்து;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    எனில் (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_தேடல்_EXTRA
			      || break_loop
#endif
			      || found || loop)
		நிறுத்து;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    எனில் (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_வரி_மொத்தம்;
	    இல்லையெனில்
		lnum = 1;
	    எனில் (!shortmess(SHM_தேடல்)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & தேடல்_செய்தி))
		give_warning((char_u *)_(dir == BACKWARD
					  ? உச்சி_bot_செய்தி : bot_உச்சி_செய்தி), TRUE);
	    எனில் (extra_arg != வெற்று)
		extra_arg->sa_wrapped = TRUE;
	}
	எனில் (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_தேடல்_EXTRA
		|| break_loop
#endif
		)
	    நிறுத்து;
    }
    போது (--மொத்தம் > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    எனில் (extra_arg != வெற்று && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    எனில் (!found)		    // did not find it
    {
	எனில் (got_int)
	    emsg(_(e_interrupted));
	இல்லையெனில் எனில் ((options & தேடல்_செய்தி) == தேடல்_செய்தி)
	{
	    எனில் (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    இல்லையெனில் எனில் (lnum == 0)
		semsg(_(e_தேடல்_hit_உச்சி_without_match_for_str), mr_pattern);
	    இல்லையெனில்
		semsg(_(e_தேடல்_hit_bottom_without_match_for_str), mr_pattern);
	}
	திரும்பு FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    எனில் (pos->lnum > buf->b_ml.ml_வரி_மொத்தம்)
    {
	pos->lnum = buf->b_ml.ml_வரி_மொத்தம்;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	எனில் (pos->col > 0)
	    --pos->col;
    }

    திரும்பு submatch + 1;
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_PROTO)
    வெறுமை
set_தேடல்_direction(முழு cdir)
{
    spats[0].off.dir = cdir;
}

    நிலையான வெறுமை
set_vv_searchforward(வெறுமை)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    நிலையான முழு
first_submatch(regmmatch_T *rp)
{
    முழு		submatch;

    க்கு (submatch = 1; ; ++submatch)
    {
	எனில் (rp->startpos[submatch].lnum >= 0)
	    நிறுத்து;
	எனில் (submatch == 9)
	{
	    submatch = 0;
	    நிறுத்து;
	}
    }
    திரும்பு submatch;
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
    முழு
parse_தேடல்_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    முழு		தேடல்_delim,
    முழு		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    முழு		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    எனில் (*pat == வெற்று || **pat == NUL)
	திரும்பு 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = வெற்று;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, தேடல்_delim, magic_isset(),
						    strcopy, வெற்று, வெற்று);
    எனில் (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (முழு)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    எனில் (*p == தேடல்_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->வரி = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    எனில் (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->வரி = TRUE;
    இல்லையெனில் எனில் ((options & தேடல்_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	எனில் (*p == 'e')		// end
	    offset->end = தேடல்_END;
	++p;
    }
    எனில் (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	எனில் (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((எழுத்து *)p);
	இல்லையெனில் எனில் (*p == '-')	    // single '-'
	    offset->off = -1;
	இல்லையெனில்			    // single '+'
	    offset->off = 1;
	++p;
	போது (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (முழு)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    திரும்பு cmdlen;
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
    முழு
do_தேடல்(
    oparg_T	    *oap,	// can be NULL
    முழு		    dirc,	// '/' or '?'
    முழு		    தேடல்_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    மொத்தம்,
    முழு		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    முழு		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = வெற்று;
    முழு		    show_தேடல்_stats;
    char_u	    *msgbuf = வெற்று;
    size_t	    msgbuflen = 0;
    முழு		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    எனில் (spats[0].off.வரி && vim_strchr(p_cpo, CPO_LINEOFF) != வெற்று)
    {
	spats[0].off.வரி = FALSE;
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
    எனில் (dirc == 0)
	dirc = spats[0].off.dir;
    இல்லையெனில்
    {
	spats[0].off.dir = dirc;
#எனில் defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    எனில் (options & தேடல்_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#இல்லையெனில்
	எனில் (dirc == '/')
	    dirc = '?';
	இல்லையெனில்
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    எனில் (dirc == '/')
    {
	எனில் (hasFolding(pos.lnum, வெற்று, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    இல்லையெனில்
    {
	எனில் (hasFolding(pos.lnum, &pos.lnum, வெற்று))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_தேடல்_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    எனில் (no_hlsearch && !(options & தேடல்_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    க்கு (;;)
    {
	முழு		show_உச்சி_bot_செய்தி = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = வெற்று;
					    // use previous pattern
	எனில் (pat == வெற்று || *pat == NUL || *pat == தேடல்_delim)
	{
	    எனில் (spats[RE_தேடல்].pat == வெற்று)	    // no previous pattern
	    {
		எனில் (spats[RE_SUBST].pat == வெற்று)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    செல் end_do_தேடல்;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    இல்லையெனில்
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	எனில் (pat != வெற்று && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_தேடல்_pattern_offset(&pat, &patlen,
				தேடல்_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    எனில் (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_தேடல்_stats = FALSE;
	எனில் ((options & தேடல்_ECHO) && messaging()
		&& !செய்தி_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    செய்தி_start();

	    // Get the offset, so we know how long it is.
	    எனில் (!cmd_silent &&
		    (spats[0].off.வரி || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		எனில் (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		இல்லையெனில் எனில் (!spats[0].off.வரி)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		எனில் (spats[0].off.off != 0 || spats[0].off.வரி)
		    off_len += vim_snprintf((எழுத்து *)off_buf + off_len,
			   அளவு(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    எனில் (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    இல்லையெனில்
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    எனில் (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		எனில் (செய்தி_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (முழு)(Rows - செய்தி_row) * cmdline_width - 1;
		இல்லையெனில்
		    // Use up to 'showcmd' column.
		    msgbufsize = (முழு)(Rows - செய்தி_row - 1) * cmdline_width
			+ sc_col - 1;
		எனில் (msgbufsize < plen + off_len + தேடல்_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + தேடல்_STAT_BUF_LEN + 3;
	    }
	    இல்லையெனில்
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    எனில் (msgbuf == வெற்று)
	    {
		msgbuflen = 0;
	    }
	    இல்லையெனில்
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		எனில் (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    எனில் (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    இல்லையெனில்
			mch_memmove(msgbuf + 1, p, plen);
		    எனில் (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = செய்தி_strtrunc(msgbuf, TRUE);
		    எனில் (trunc != வெற்று)
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
		    எனில் (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			எனில் (r != வெற்று)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    போது (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    எனில் ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    இல்லையெனில்
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    செய்தி_outtrans(msgbuf);
		    செய்தி_clr_eos();
		    செய்தி_check();

		    gotocmdline(FALSE);
		    out_flush();
		    செய்தி_nowait = TRUE;	    // don't wait for this message
		}

		எனில் (!shortmess(SHM_SEARCHCOUNT))
		    show_தேடல்_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	எனில் (!spats[0].off.வரி && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    எனில் (spats[0].off.off > 0)
	    {
		க்கு (c = spats[0].off.off; c; --c)
		    எனில் (decl(&pos) == -1)
			நிறுத்து;
		எனில் (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    இல்லையெனில்
	    {
		க்கு (c = spats[0].off.off; c; ++c)
		    எனில் (incl(&pos) == -1)
			நிறுத்து;
		எனில் (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_வரி_மொத்தம் + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, வெற்று,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, மொத்தம், spats[0].off.end + (options &
		       (தேடல்_KEEP + தேடல்_PEEK + தேடல்_HIS
			+ தேடல்_செய்தி + தேடல்_START
			+ ((pat != வெற்று && *pat == ';') ? 0 : தேடல்_NOOF))),
		RE_LAST, sia);

	எனில் (dircp != வெற்று)
	    *dircp = தேடல்_delim; // restore second '/' or '?' for normal_cmd()

	எனில் (!shortmess(SHM_தேடல்) && sia && sia->sa_wrapped)
	    show_உச்சி_bot_செய்தி = TRUE;

	எனில் (c == FAIL)
	{
	    retval = 0;
	    செல் end_do_தேடல்;
	}
	எனில் (spats[0].off.end && oap != வெற்று)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	எனில் (!(options & தேடல்_NOOF) || (pat != வெற்று && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    எனில் (spats[0].off.வரி)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		எனில் (c < 1)
		    pos.lnum = 1;
		இல்லையெனில் எனில் (c > curbuf->b_ml.ml_வரி_மொத்தம்)
		    pos.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
		இல்லையெனில்
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    இல்லையெனில் எனில் (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		எனில் (c > 0)
		{
		    போது (c-- > 0)
			எனில் (incl(&pos) == -1)
			    நிறுத்து;
		}
		// to the left, check for start of file
		இல்லையெனில்
		{
		    போது (c++ < 0)
			எனில் (decl(&pos) == -1)
			    நிறுத்து;
		}
	    }
	    எனில் (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	எனில் (show_தேடல்_stats)
	     cmdline_தேடல்_stat(dirc, &pos, &curwin->w_cursor,
				show_உச்சி_bot_செய்தி, msgbuf, msgbuflen,
				(மொத்தம் != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_தேடல்)
				     && hasFolding(curwin->w_cursor.lnum,
								   வெற்று, வெற்று))
#endif
				),
				p_msc,
				தேடல்_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	எனில் (!(options & தேடல்_OPT) || pat == வெற்று || *pat != ';')
	    நிறுத்து;

	dirc = *++pat;
	--patlen;
	தேடல்_delim = dirc;
	எனில் (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    செல் end_do_தேடல்;
	}
	++pat;
	--patlen;
    }

    எனில் (options & தேடல்_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_தேடல்:
    எனில் ((options & தேடல்_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    திரும்பு retval;
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
    முழு
தேடல்_for_exact_வரி(
    buf_T	*buf,
    pos_T	*pos,
    முழு		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    முழு		compl_len = ins_compl_len();

    எனில் (buf->b_ml.ml_வரி_மொத்தம் == 0)
	திரும்பு FAIL;
    க்கு (;;)
    {
	pos->lnum += dir;
	எனில் (pos->lnum < 1)
	{
	    எனில் (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_வரி_மொத்தம்;
		எனில் (!shortmess(SHM_தேடல்))
		    give_warning((char_u *)_(உச்சி_bot_செய்தி), TRUE);
	    }
	    இல்லையெனில்
	    {
		pos->lnum = 1;
		நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (pos->lnum > buf->b_ml.ml_வரி_மொத்தம்)
	{
	    எனில் (p_ws)
	    {
		pos->lnum = 1;
		எனில் (!shortmess(SHM_தேடல்))
		    give_warning((char_u *)_(bot_உச்சி_செய்தி), TRUE);
	    }
	    இல்லையெனில்
	    {
		pos->lnum = 1;
		நிறுத்து;
	    }
	}
	எனில் (pos->lnum == start)
	    நிறுத்து;
	எனில் (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	எனில் (compl_status_adding() && !compl_status_sol())
	{
	    எனில் ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		திரும்பு OK;
	}
	இல்லையெனில் எனில் (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    எனில் ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		திரும்பு OK;
	}
    }
    திரும்பு FAIL;
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
    முழு
searchc(cmdarg_T *cap, முழு t_cmd)
{
    முழு			c = cap->nchar;	// char to search for
    முழு			dir = cap->arg;	// TRUE for searching forward
    long		மொத்தம் = cap->count1;	// repeat count
    முழு			col;
    char_u		*p;
    முழு			len;
    முழு			stop = TRUE;

    எனில் (c != NUL)	// normal search: remember args for repeat
    {
	எனில் (!சாவி_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    எனில் (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		எனில் (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    இல்லையெனில்		// repeat previous search
    {
	எனில் (*lastc == NUL && lastc_bytelen <= 1)
	    திரும்பு FAIL;
	எனில் (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	இல்லையெனில்
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	எனில் (vim_strchr(p_cpo, CPO_SCOLON) == வெற்று && மொத்தம் == 1 && t_cmd)
	    stop = FALSE;
    }

    எனில் (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    இல்லையெனில்
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    போது (மொத்தம்--)
    {
	எனில் (has_mbyte)
	{
	    க்கு (;;)
	    {
		எனில் (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    எனில் (col >= len)
			திரும்பு FAIL;
		}
		இல்லையெனில்
		{
		    எனில் (col == 0)
			திரும்பு FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		எனில் (lastc_bytelen <= 1)
		{
		    எனில் (p[col] == c && stop)
			நிறுத்து;
		}
		இல்லையெனில் எனில் (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    நிறுத்து;
		stop = TRUE;
	    }
	}
	இல்லையெனில்
	{
	    க்கு (;;)
	    {
		எனில் ((col += dir) < 0 || col >= len)
		    திரும்பு FAIL;
		எனில் (p[col] == c && stop)
		    நிறுத்து;
		stop = TRUE;
	    }
	}
    }

    எனில் (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	எனில் (has_mbyte)
	{
	    எனில் (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    இல்லையெனில்
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    திரும்பு OK;
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
findmatch(oparg_T *oap, முழு initc)
{
    திரும்பு findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    நிலையான முழு
check_prevcol(
    char_u	*linep,
    முழு		col,
    முழு		ch,
    முழு		*prevcol)
{
    --col;
    எனில் (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    எனில் (prevcol)
	*prevcol = col;
    திரும்பு (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    நிலையான முழு
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    முழு		found = FALSE;

    க்கு (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    எனில் (delim_copy == வெற்று)
	திரும்பு FALSE;
    க்கு (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *வரி = ml_get(lnum);

	க்கு (p = வரி + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    எனில் (lnum == endpos->lnum && (colnr_T)(p - வரி) >= endpos->col)
		நிறுத்து;
	    எனில் (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		நிறுத்து;
	    }
	}
	எனில் (found)
	    நிறுத்து;
    }
    vim_free(delim_copy);
    திரும்பு found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    நிலையான வெறுமை
find_mps_மதிப்புகள்(
    முழு	    *initc,
    முழு	    *findc,
    முழு	    *backwards,
    முழு	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    போது (*ptr != NUL)
    {
	எனில் (has_mbyte)
	{
	    char_u *prev;

	    எனில் (mb_ptr2char(ptr) == *initc)
	    {
		எனில் (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		இல்லையெனில்
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		திரும்பு;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    எனில் (mb_ptr2char(ptr) == *initc)
	    {
		எனில் (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		இல்லையெனில்
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		திரும்பு;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	இல்லையெனில்
	{
	    எனில் (*ptr == *initc)
	    {
		எனில் (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		இல்லையெனில்
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		திரும்பு;
	    }
	    ptr += 2;
	    எனில் (*ptr == *initc)
	    {
		எனில் (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		இல்லையெனில்
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		திரும்பு;
	    }
	    ++ptr;
	}
	எனில் (*ptr == ',')
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
    முழு		initc,
    முழு		flags,
    முழு		maxtravel)
{
    நிலையான pos_T pos;			// current search position
    முழு		findc = 0;		// matching brace
    முழு		c;
    முழு		மொத்தம் = 0;		// cumulative number of braces
    முழு		backwards = FALSE;	// init for gcc
    முழு		raw_string = FALSE;	// search for raw string
    முழு		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    முழு		do_quotes;		// check for quotes in current line
    முழு		at_start;		// do_quotes value at start position
    முழு		hash_dir = 0;		// Direction searched for # things
    முழு		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    முழு		start_in_quotes;	// start position is in quotes
    முழு		traveled = 0;		// how far we've searched so far
    முழு		ignore_cend = FALSE;    // ignore comment end
    முழு		cpo_match;		// vi compatible matching
    முழு		cpo_bsl;		// don't recognize backslashes
    முழு		match_escaped = 0;	// search for escaped match
    முழு		dir;			// Direction to search
    முழு		comment_col = MAXCOL;   // start of / / comment
    முழு		lispcomm = FALSE;	// inside of Lisp-style comment
    முழு		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    முழு		skip_comments = (flags & FM_SKIPCOMM) != 0;
    முழு		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != வெற்று);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != வெற்று);

    // Direction to search when initc is '/', '*' or '#'
    எனில் (flags & FM_BACKWARD)
	dir = BACKWARD;
    இல்லையெனில் எனில் (flags & FM_FORWARD)
	dir = FORWARD;
    இல்லையெனில்
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    எனில் (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	எனில் (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    இல்லையெனில் எனில் (initc != '#' && initc != NUL)
    {
	find_mps_மதிப்புகள்(&initc, &findc, &backwards, TRUE);
	எனில் (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	எனில் (findc == NUL)
	    திரும்பு வெற்று;
    }
    இல்லையெனில்
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	எனில் (initc == '#')
	{
	    hash_dir = dir;
	}
	இல்லையெனில்
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    எனில் (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		எனில் (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    எனில் (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		இல்லையெனில் எனில் (linep[pos.col] == '/')
		{
		    எனில் (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    இல்லையெனில் எனில் (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		இல்லையெனில் எனில் (linep[pos.col] == '*')
		{
		    எனில் (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    இல்லையெனில் எனில் (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    எனில் (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		எனில் (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		க்கு (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    எனில் (initc == NUL)
			நிறுத்து;

		    find_mps_மதிப்புகள்(&initc, &findc, &backwards, FALSE);
		    எனில் (findc)
			நிறுத்து;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		எனில் (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    எனில் (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    இல்லையெனில்
			திரும்பு வெற்று;
		}
		இல்லையெனில் எனில் (!cpo_bsl)
		{
		    முழு col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    க்கு (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	எனில் (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    எனில் (oap != வெற்று)
		oap->motion_type = MLINE;   // Linewise for this case only
	    எனில் (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		எனில் (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		இல்லையெனில் எனில் (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		இல்லையெனில்
		    திரும்பு வெற்று;
	    }
	    pos.col = 0;
	    போது (!got_int)
	    {
		எனில் (hash_dir > 0)
		{
		    எனில் (pos.lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
			நிறுத்து;
		}
		இல்லையெனில் எனில் (pos.lnum == 1)
		    நிறுத்து;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		வரி_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		எனில் (*ptr != '#')
		    தொடர்;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		எனில் (hash_dir > 0)
		{
		    எனில் (STRNCMP(ptr, "if", 2) == 0)
			மொத்தம்++;
		    இல்லையெனில் எனில் (STRNCMP(ptr, "el", 2) == 0)
		    {
			எனில் (மொத்தம் == 0)
			    திரும்பு &pos;
		    }
		    இல்லையெனில் எனில் (STRNCMP(ptr, "endif", 5) == 0)
		    {
			எனில் (மொத்தம் == 0)
			    திரும்பு &pos;
			மொத்தம்--;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (STRNCMP(ptr, "if", 2) == 0)
		    {
			எனில் (மொத்தம் == 0)
			    திரும்பு &pos;
			மொத்தம்--;
		    }
		    இல்லையெனில் எனில் (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			எனில் (மொத்தம் == 0)
			    திரும்பு &pos;
		    }
		    இல்லையெனில் எனில் (STRNCMP(ptr, "endif", 5) == 0)
			மொத்தம்++;
		}
	    }
	    திரும்பு வெற்று;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    எனில் (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != வெற்று)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    எனில் ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    எனில் (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    எனில் (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    போது (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	எனில் (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    எனில் (lispcomm && pos.col < (colnr_T)comment_col)
		நிறுத்து;
	    எனில் (pos.col == 0)		// at start of line, go to prev. one
	    {
		எனில் (pos.lnum == 1)	// start of file
		    நிறுத்து;
		--pos.lnum;

		எனில் (maxtravel > 0 && ++traveled > maxtravel)
		    நிறுத்து;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		வரி_breakcheck();

		// Check if this line contains a single-line comment
		எனில் (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		எனில் (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		இல்லையெனில் எனில் (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    இல்லையெனில்
	    {
		--pos.col;
		எனில் (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	இல்லையெனில்				// forward search
	{
	    எனில் (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		எனில் (pos.lnum == curbuf->b_ml.ml_வரி_மொத்தம்  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    நிறுத்து;
		++pos.lnum;

		எனில் (maxtravel && traveled++ > maxtravel)
		    நிறுத்து;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		வரி_breakcheck();
		எனில் (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    இல்லையெனில்
	    {
		எனில் (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		இல்லையெனில்
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	எனில் (skip_comments && !comment_dir && !inquote)
	{
	    எனில் (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		எனில் (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (முழு)pos.col < comment_col))
		    in_block_comment = TRUE;
		இல்லையெனில் எனில் (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    இல்லையெனில்
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		எனில் (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (முழு)pos.col < comment_col))
		    in_block_comment = TRUE;
		இல்லையெனில் எனில் (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	எனில் (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    எனில் (linep[0] == findc && மொத்தம் == 0)	// match!
		திரும்பு &pos;
	    நிறுத்து;					// out of scope
	}

	எனில் (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    எனில் (comment_dir == FORWARD)
	    {
		எனில் (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    திரும்பு &pos;
		}
	    }
	    இல்லையெனில்    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		எனில் (pos.col == 0)
		    தொடர்;
		இல்லையெனில் எனில் (raw_string)
		{
		    எனில் (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != வெற்று)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			எனில் (!find_rawstring_end(linep, &pos,
				  மொத்தம் > 0 ? &match_pos : &curwin->w_cursor))
			{
			    மொத்தம்++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		இல்லையெனில் எனில் (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (முழு)pos.col < comment_col)
		{
		    மொத்தம்++;
		    match_pos = pos;
		    match_pos.col--;
		}
		இல்லையெனில் எனில் (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    எனில் (மொத்தம் > 0)
			pos = match_pos;
		    இல்லையெனில் எனில் (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (முழு)pos.col <= comment_col)
			pos.col -= 2;
		    இல்லையெனில் எனில் (ignore_cend)
			தொடர்;
		    இல்லையெனில்
			திரும்பு வெற்று;
		    திரும்பு &pos;
		}
	    }
	    தொடர்;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	எனில் (cpo_match)
	    do_quotes = 0;
	இல்லையெனில் எனில் (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    க்கு (ptr = linep; *ptr; ++ptr)
	    {
		எனில் (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		எனில் (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		எனில் (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    எனில் (!do_quotes)
	    {
		inquote = FALSE;
		எனில் (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    எனில் (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			எனில் (inquote)
			    start_in_quotes = TRUE;
		    }
		    இல்லையெனில் எனில் (backwards)
			inquote = TRUE;
		}
		எனில் (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    எனில் (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			எனில் (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    எனில் (inquote)
				start_in_quotes = TRUE;
			}
			இல்லையெனில் எனில் (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	எனில் (start_in_quotes == MAYBE)
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
	தேர்வு (c)
	{
	நிலை NUL:
	    // at end of line without trailing backslash, reset inquote
	    எனில் (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    நிறுத்து;

	நிலை '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    எனில் (do_quotes)
	    {
		முழு col;

		க்கு (col = pos.col - 1; col >= 0; --col)
		    எனில் (linep[col] != '\\')
			நிறுத்து;
		எனில் ((((முழு)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    நிறுத்து;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	நிலை '\'':
	    எனில் (!cpo_match && initc != '\'' && findc != '\'')
	    {
		எனில் (backwards)
		{
		    எனில் (pos.col > 1)
		    {
			எனில் (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    நிறுத்து;
			}
			இல்லையெனில் எனில் (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    நிறுத்து;
			}
		    }
		}
		இல்லையெனில் எனில் (linep[pos.col + 1])	// forward search
		{
		    எனில் (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			நிறுத்து;
		    }
		    இல்லையெனில் எனில் (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			நிறுத்து;
		    }
		}
	    }
	    // FALLTHROUGH

	இயல்பு:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    எனில் (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != வெற்று
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', வெற்று)
		    && check_prevcol(linep, pos.col - 1, '#', வெற்று))
		நிறுத்து;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    எனில் (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (முழு)pos.col >= comment_col)))
		நிறுத்து;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    எனில் ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		முழு	col, bslcnt = 0;

		எனில் (!cpo_bsl)
		{
		    க்கு (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		எனில் (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    எனில் (c == initc)
			மொத்தம்++;
		    இல்லையெனில்
		    {
			எனில் (மொத்தம் == 0)
			    திரும்பு &pos;
			மொத்தம்--;
		    }
		}
	    }
	}
    }

    எனில் (comment_dir == BACKWARD && மொத்தம் > 0)
    {
	pos = match_pos;
	திரும்பு &pos;
    }
    திரும்பு (pos_T *)வெற்று;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    வெறுமை
showmatch(
    முழு		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    முழு		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    க்கு (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	எனில் (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    நிறுத்து;
#endif
	p += mb_ptr2len(p) + 1;
	எனில் (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    நிறுத்து;
	p += mb_ptr2len(p);
	எனில் (*p == NUL)
	    திரும்பு;
    }
    எனில் (*p == NUL)
	திரும்பு;

    எனில் ((lpos = findmatch(வெற்று, NUL)) == வெற்று)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	திரும்பு;
    }

    எனில் (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	திரும்பு;

    எனில் (!curwin->w_p_wrap)
	getvcol(curwin, lpos, வெற்று, &vcol, வெற்று, 0);

    முழு col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    எனில் (!col_visible)
	திரும்பு;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    எனில் (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    எனில் (vim_strchr(p_cpo, CPO_SHOWMATCH) != வெற்று)
	ui_delay(p_mat * 100L + 8, TRUE);
    இல்லையெனில் எனில் (!char_avail())
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
    நிலையான முழு
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    முழு		move,
    pos_T	*cur,
    முழு		direction)
{
    regmmatch_T	regmatch;
    முழு		nmatched = 0;
    முழு		result = -1;
    pos_T	pos;
    முழு		called_emsg_before = called_emsg;
    முழு		flag = 0;

    எனில் (pattern == வெற்று)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    எனில் (தேடல்_regcomp(pattern, patternlen, வெற்று, RE_தேடல், RE_தேடல்,
					      தேடல்_KEEP, &regmatch) == FAIL)
	திரும்பு -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    எனில் (move)
    {
	CLEAR_POS(&pos);
    }
    இல்லையெனில்
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = தேடல்_START;
    }

    எனில் (searchit(curwin, curbuf, &pos, வெற்று, direction, pattern, patternlen, 1,
				  தேடல்_KEEP + flag, RE_தேடல், வெற்று) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	செய்
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, வெற்று);
	    எனில் (nmatched != 0)
		நிறுத்து;
	} போது (regmatch.regprog != வெற்று
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	எனில் (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    திரும்பு result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    முழு
current_தேடல்(
    long	மொத்தம்,
    முழு		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    முழு		i;
    முழு		dir;
    முழு		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    முழு		flags = 0;
    pos_T	save_VIsual = VIsual;
    முழு		zero_width;
    முழு		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    எனில் (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    எனில் (VIsual_active)
    {
	எனில் (forward)
	    incl(&pos);
	இல்லையெனில்
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    எனில் (zero_width == -1)
	திரும்பு FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    க்கு (i = 0; i < 2; i++)
    {
	எனில் (forward)
	{
	    எனில் (i == 0 && skip_first_backward)
		தொடர்;
	    dir = i;
	}
	இல்லையெனில்
	    dir = !i;

	flags = 0;
	எனில் (!dir && !zero_width)
	    flags = தேடல்_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	எனில் (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? மொத்தம் : 1),
		தேடல்_KEEP | flags, RE_தேடல், வெற்று);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	எனில் (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    எனில் (VIsual_active)
		VIsual = save_VIsual;
	    திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (i == 0 && !result)
	{
	    எனில் (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    இல்லையெனில்
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_வரி_மொத்தம்;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_வரி_மொத்தம்);
	    }
	}
    }

    start_pos = pos;

    எனில் (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    எனில் (LT_POS(VIsual, end_pos) && forward)
    {
	எனில் (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	இல்லையெனில்
	    // put the cursor on last character of match
	    dec_cursor();
    }
    இல்லையெனில் எனில் (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    எனில் (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	எனில் (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	இல்லையெனில் எனில் (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    எனில் (fdo_flags & FDO_தேடல் && சாவி_Typed)
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

    திரும்பு OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    முழு
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    திரும்பு (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    நிலையான வெறுமை
cmdline_தேடல்_stat(
    முழு		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    முழு		show_உச்சி_bot_செய்தி,
    char_u	*msgbuf,
    size_t	msgbuflen,
    முழு		recompute,
    முழு		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_தேடல்_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    எனில் (stat.cur <= 0)
	திரும்பு;

    எழுத்து	t[தேடல்_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	எனில் (stat.incomplete == 1)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[?/??]");
	இல்லையெனில் எனில் (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	இல்லையெனில் எனில் (stat.cnt > maxcount)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	இல்லையெனில்
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    இல்லையெனில்
#endif
    {
	எனில் (stat.incomplete == 1)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[?/??]");
	இல்லையெனில் எனில் (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	இல்லையெனில் எனில் (stat.cnt > maxcount)
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	இல்லையெனில்
	    len = vim_snprintf(t, தேடல்_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    எனில் (show_உச்சி_bot_செய்தி && len + 2 < தேடல்_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    எனில் (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    எனில் (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    செய்தி_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    செய்தி_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    நிலையான வெறுமை
update_தேடல்_stat(
    முழு			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    முழு			recompute,
    முழு			maxcount,
    long		timeout UNUSED)
{
    முழு		    save_ws = p_ws;
    முழு		    wraparound = FALSE;
    pos_T	    p = (*pos);
    நிலையான pos_T    lastpos = {0, 0, 0};
    நிலையான முழு	    cur = 0;
    நிலையான முழு	    cnt = 0;
    நிலையான முழு	    exact_match = FALSE;
    நிலையான முழு	    incomplete = 0;
    நிலையான முழு	    last_maxcount = 0;
    நிலையான முழு	    chgtick = 0;
    நிலையான char_u   *lastpat = வெற்று;
    நிலையான size_t   lastpatlen = 0;
    நிலையான buf_T    *lbuf = வெற்று;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    எனில் (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	திரும்பு;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    எனில் (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != வெற்று
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
    எனில் (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    இல்லையெனில்
    {
	முழு	done_தேடல் = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	எனில் (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	போது (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, வெற்று, 0, 1, தேடல்_KEEP, RE_LAST, வெற்று) != FAIL)
	{
	    done_தேடல் = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    எனில் (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		நிறுத்து;
	    }
#endif
	    cnt++;
	    எனில் (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		எனில் (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    எனில் (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		நிறுத்து;
	    }
	}
	எனில் (got_int)
	    cur = -1; // abort
	எனில் (done_தேடல்)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    எனில் (lastpat == வெற்று)
		lastpatlen = 0;
	    இல்லையெனில்
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

#எனில் defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    நிலையான char_u *
get_வரி_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *வரி = ml_get(lnum);

    vim_strncpy(buf, வரி, LSIZE - 1);
    திரும்பு buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    வெறுமை
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    முழு		dir UNUSED,	// direction of expansion
    முழு		len,		// length of search pattern
    முழு		whole,		// match whole words only
    முழு		skip_comments,	// don't match inside comments
    முழு		type,		// Type of search; are we looking for a type?
				// a macro?
    long	மொத்தம்,
    முழு		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    முழு		forceit,	// If true, always switch to the found path
    முழு		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    முழு		max_path_depth = 50;
    long	match_மொத்தம் = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = வெற்று;
    linenr_T	lnum;
    முழு		depth;
    முழு		depth_displayed;	// For type==CHECK_PATH
    முழு		old_files;
    முழு		already_searched;
    char_u	*file_வரி;
    char_u	*வரி;
    char_u	*p;
    char_u	save_char;
    முழு		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    முழு		matched = FALSE;
    முழு		did_show = FALSE;
    முழு		found = FALSE;
    முழு		i;
    char_u	*already = வெற்று;
    char_u	*startp = வெற்று;
    char_u	*inc_opt = வெற்று;
# எனில் defined(FEAT_QUICKFIX)
    win_T	*curwin_save = வெற்று;
# endif

    regmatch.regprog = வெற்று;
    incl_regmatch.regprog = வெற்று;
    def_regmatch.regprog = வெற்று;

    file_வரி = alloc(LSIZE);
    எனில் (file_வரி == வெற்று)
	திரும்பு;

    எனில் (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	எனில் (pat == வெற்று)
	    செல் fpip_end;
	vim_snprintf((எழுத்து *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	எனில் (regmatch.regprog == வெற்று)
	    செல் fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    எனில் (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	எனில் (incl_regmatch.regprog == வெற்று)
	    செல் fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    எனில் (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	எனில் (def_regmatch.regprog == வெற்று)
	    செல் fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * அளவு(SearchedFile), TRUE);
    எனில் (files == வெற்று)
	செல் fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    எனில் (end_lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	end_lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
    எனில் (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    வரி = get_வரி_and_copy(lnum, file_வரி);

    க்கு (;;)
    {
	எனில் (incl_regmatch.regprog != வெற்று
		&& vim_regexec(&incl_regmatch, வரி, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    எனில் (strstr((எழுத்து *)inc_opt, "\\zs") != வெற்று)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (முழு)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    இல்லையெனில்
		// Use text after match with 'include'.
		new_fname = file_name_in_வரி(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, வெற்று);
	    already_searched = FALSE;
	    எனில் (new_fname != வெற்று)
	    {
		// Check whether we have already searched in this file
		க்கு (i = 0;; i++)
		{
		    எனில் (i == depth + 1)
			i = old_files;
		    எனில் (i == max_path_depth)
			நிறுத்து;
		    எனில் (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			எனில் (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    செய்தி_putchar('\n');	    // cursor below last one
			    எனில் (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				செய்தி_home_replace_hl(new_fname);
				செய்தி_puts(_(" (includes previously listed match)"));
				prev_fname = வெற்று;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			நிறுத்து;
		    }
		}
	    }

	    எனில் (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == வெற்று && !already_searched)))
	    {
		எனில் (did_show)
		    செய்தி_putchar('\n');	    // cursor below last one
		இல்லையெனில்
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    செய்தி_puts_title(_("--- Included files "));
		    எனில் (action != ACTION_SHOW_ALL)
			செய்தி_puts_title(_("not found "));
		    செய்தி_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		போது (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    க்கு (i = 0; i < depth_displayed; i++)
			செய்தி_puts("  ");
		    செய்தி_home_replace(files[depth_displayed].name);
		    செய்தி_puts(" -->\n");
		}
		எனில் (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    க்கு (i = 0; i <= depth_displayed; i++)
			செய்தி_puts("  ");
		    எனில் (new_fname != வெற்று)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			செய்தி_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    இல்லையெனில்
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			எனில் (strstr((எழுத்து *)inc_opt, "\\zs") != வெற்று)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (முழு)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			இல்லையெனில்
			{
			    // find the file name after the end of the match
			    க்கு (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    க்கு (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			எனில் (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (முழு)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			இல்லையெனில் எனில் (p > வரி)
			{
			    எனில் (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    எனில் (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			செய்தி_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    எனில் (new_fname == வெற்று && action == ACTION_SHOW_ALL)
		    {
			எனில் (already_searched)
			    செய்தி_puts(_("  (Already listed)"));
			இல்லையெனில்
			    செய்தி_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    எனில் (new_fname != வெற்று)
	    {
		// Push the new file onto the file stack
		எனில் (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    எனில் (bigger != வெற்று)
		    {
			க்கு (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			க்கு (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = வெற்று;
			    bigger[i].name = வெற்று;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			க்கு (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		எனில் ((files[depth + 1].fp = mch_fopen((எழுத்து *)new_fname, "r"))
								    == வெற்று)
		    vim_free(new_fname);
		இல்லையெனில்
		{
		    எனில் (++depth == old_files)
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
		    எனில் (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			செய்தி_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((எழுத்து*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(எழுத்து *)new_fname);
			செய்தி_trunc_attr((எழுத்து *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    இல்லையெனில் எனில் (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (எழுத்து *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	இல்லையெனில்
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = வரி;
தேடல்_வரி:
	    define_matched = FALSE;
	    எனில் (def_regmatch.regprog != வெற்று
			      && vim_regexec(&def_regmatch, வரி, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		போது (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    எனில் (def_regmatch.regprog == வெற்று || define_matched)
	    {
		எனில் (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    எனில் (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    இல்லையெனில்
			matched = STRNCMP(startp, ptr, len) == 0;
		    எனில் (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		இல்லையெனில் எனில் (regmatch.regprog != வெற்று
			 && vim_regexec(&regmatch, வரி, (colnr_T)(p - வரி)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    எனில் (!define_matched && skip_comments)
		    {
			எனில் ((*வரி != '#' ||
				STRNCMP(skipwhite(வரி + 1), "define", 6) != 0)
				&& get_leader_len(வரி, வெற்று, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(வரி);
			எனில் (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    க்கு (p = வரி; *p && p < startp; ++p)
			    {
				எனில் (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    எனில் (p[1] == '/')
					நிறுத்து;
				    ++p;
				}
				இல்லையெனில் எனில் (!matched && p[0] == '*' && p[1] == '/')
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
	எனில் (matched)
	{
	    எனில் (action == ACTION_EXPAND)
	    {
		முழு	cont_s_ipos = FALSE;
		முழு	add_r;
		char_u	*aux;

		எனில் (depth == -1 && lnum == curwin->w_cursor.lnum)
		    நிறுத்து;
		found = TRUE;
		aux = p = startp;
		எனில் (compl_status_adding() && (முழு)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    எனில் (vim_iswordp(p))
			செல் exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (முழு)(p - aux);

		எனில் (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    எனில் (depth < 0)
		    {
			எனில் (lnum >= end_lnum)
			    செல் exit_matched;
			வரி = get_வரி_and_copy(++lnum, file_வரி);
		    }
		    இல்லையெனில் எனில் (vim_fgets(வரி = file_வரி,
						      LSIZE, files[depth].fp))
			செல் exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(வரி);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    எனில் (p > aux)
		    {
			எனில் (*aux != ')' && IObuff[i-1] != TAB)
			{
			    எனில் (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    எனில் (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == வெற்று
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			எனில் (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (முழு)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    எனில் (i == ins_compl_len())
			செல் exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? வெற்று
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		எனில் (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		இல்லையெனில் எனில் (add_r == FAIL)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		எனில் (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		எனில் (curr_fname != prev_fname)
		{
		    எனில் (did_show)
			செய்தி_putchar('\n');	// cursor below last one
		    எனில் (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			செய்தி_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		எனில் (!got_int)
		    show_pat_in_path(வரி, type, TRUE, action,
			    (depth == -1) ? வெற்று : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_மொத்தம்++);

		// Set matched flag for this file and all the ones that
		// include it
		க்கு (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    இல்லையெனில் எனில் (--மொத்தம் <= 0)
	    {
		found = TRUE;
		எனில் (depth == -1 && lnum == curwin->w_cursor.lnum
# எனில் defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_வரி));
		இல்லையெனில் எனில் (action == ACTION_SHOW)
		{
		    show_pat_in_path(வரி, type, did_show, action,
			    (depth == -1) ? வெற்று : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		இல்லையெனில்
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# எனில் defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    எனில் (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    எனில் (action == ACTION_SPLIT)
		    {
			எனில் (win_split(0, 0) == FAIL)
			    நிறுத்து;
			RESET_BINDING(curwin);
		    }
		    எனில் (depth == -1)
		    {
			// match in current file
# எனில் defined(FEAT_QUICKFIX)
			எனில் (g_do_tagpreview != 0)
			{
			    எனில் (!win_valid(curwin_save))
				நிறுத்து;
			    எனில் (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, வெற்று,
						     வெற்று, TRUE, lnum, forceit)))
				நிறுத்து;	// failed to jump to file
			}
			இல்லையெனில்
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    இல்லையெனில்
		    {
			எனில் (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, வெற்று, TRUE,
						    files[depth].lnum, forceit)))
			    நிறுத்து;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		எனில் (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - வரி);
		    curwin->w_set_curswant = true;
		}

# எனில் defined(FEAT_QUICKFIX)
		எனில் (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		இல்லையெனில் எனில் (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		நிறுத்து;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    எனில் (def_regmatch.regprog == வெற்று
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		செல் தேடல்_வரி;
	}
	வரி_breakcheck();
	எனில் (action == ACTION_EXPAND)
	    ins_compl_check_சாவிகள்(30, FALSE);
	எனில் (got_int || ins_compl_interrupted())
	    நிறுத்து;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	போது (depth >= 0 && !already
		&& vim_fgets(வரி = file_வரி, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    எனில் (depth < depth_displayed)
		depth_displayed = depth;
	}
	எனில் (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (முழு)STRLEN(வரி);
	    எனில் (i > 0 && வரி[i - 1] == '\n')
		வரி[--i] = NUL;
	    எனில் (i > 0 && வரி[i - 1] == '\r')
		வரி[--i] = NUL;
	}
	இல்லையெனில் எனில் (!already)
	{
	    எனில் (++lnum > end_lnum)
		நிறுத்து;
	    வரி = get_வரி_and_copy(lnum, file_வரி);
	}
	already = வெற்று;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    க்கு (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    க்கு (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    எனில் (type == CHECK_PATH)
    {
	எனில் (!did_show)
	{
	    எனில் (action != ACTION_SHOW_ALL)
		செய்தி(_("All included files were found"));
	    இல்லையெனில்
		செய்தி(_("No included files"));
	}
    }
    இல்லையெனில் எனில் (!found && action != ACTION_EXPAND && !silent)
    {
	எனில் (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	இல்லையெனில் எனில் (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	இல்லையெனில்
	    emsg(_(e_couldnt_find_pattern));
    }
    எனில் (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	செய்தி_end();

fpip_end:
    vim_free(file_வரி);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    நிலையான வெறுமை
show_pat_in_path(
    char_u  *வரி,
    முழு	    type,
    முழு	    did_show,
    முழு	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    மொத்தம்)
{
    char_u  *p;
    size_t  linelen;

    எனில் (did_show)
	செய்தி_putchar('\n');	// cursor below last one
    இல்லையெனில் எனில் (!செய்தி_silent)
	gotocmdline(TRUE);	// cursor at status line
    எனில் (got_int)		// 'q' typed at "--more--" message
	திரும்பு;
    linelen = STRLEN(வரி);
    க்கு (;;)
    {
	p = வரி + linelen - 1;
	எனில் (fp != வெற்று)
	{
	    // We used fgets(), so get rid of newline at end
	    எனில் (p >= வரி && *p == '\n')
		--p;
	    எனில் (p >= வரி && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	எனில் (action == ACTION_SHOW_ALL)
	{
	    sprintf((எழுத்து *)IObuff, "%3ld: ", மொத்தம்);	// show match nr
	    செய்தி_puts((எழுத்து *)IObuff);
	    sprintf((எழுத்து *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    செய்தி_puts_attr((எழுத்து *)IObuff, HL_ATTR(HLF_N));
	    செய்தி_puts(" ");
	}
	செய்தி_prt_வரி(வரி, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	எனில் (got_int || type != FIND_DEFINE || p < வரி || *p != '\\')
	    நிறுத்து;

	எனில் (fp != வெற்று)
	{
	    எனில் (vim_fgets(வரி, LSIZE, fp)) // end of file
		நிறுத்து;
	    linelen = STRLEN(வரி);
	    ++*lnum;
	}
	இல்லையெனில்
	{
	    எனில் (++*lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    வரி = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	செய்தி_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(முழு idx)
{
    திரும்பு &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    முழு
get_spat_last_idx(வெறுமை)
{
    திரும்பு last_idx;
}
#endif

#எனில் defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    வெறுமை
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = வெற்று;
    முழு			maxcount = p_msc;
    long		timeout = தேடல்_STAT_DEF_TIMEOUT;
    முழு			recompute = TRUE;
    searchstat_T	stat;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	முழு		error = FALSE;

	எனில் (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    திரும்பு;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	எனில் (di != வெற்று)
	{
	    timeout = (long)tv_get_எண்_chk(&di->di_tv, &error);
	    எனில் (error)
		திரும்பு;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	எனில் (di != வெற்று)
	{
	    maxcount = (முழு)tv_get_எண்_chk(&di->di_tv, &error);
	    எனில் (error)
		திரும்பு;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	எனில் (di != வெற்று)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    எனில் (pattern == வெற்று)
		திரும்பு;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		திரும்பு;
	    }
	    எனில் (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		திரும்பு;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    எனில் (li != வெற்று)
	    {
		pos.lnum = tv_get_எண்_chk(&li->li_tv, &error);
		எனில் (error)
		    திரும்பு;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    எனில் (li != வெற்று)
	    {
		pos.col = tv_get_எண்_chk(&li->li_tv, &error) - 1;
		எனில் (error)
		    திரும்பு;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    எனில் (li != வெற்று)
	    {
		pos.coladd = tv_get_எண்_chk(&li->li_tv, &error);
		எனில் (error)
		    திரும்பு;
	    }
	}
    }

    save_last_தேடல்_pattern();
# ifdef FEAT_தேடல்_EXTRA
    save_incsearch_state();
# endif
    எனில் (pattern != வெற்று)
    {
	எனில் (*pattern == NUL)
	    செல் the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	எனில் (spats[last_idx].pat == வெற்று)
	    spats[last_idx].patlen = 0;
    }
    எனில் (spats[last_idx].pat == வெற்று || *spats[last_idx].pat == NUL)
	செல் the_end;	// the previous pattern was never defined

    update_தேடல்_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_எண்(rettv->vval.v_dict, "current", stat.cur);
    dict_add_எண்(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_எண்(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_எண்(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_எண்(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_தேடல்_pattern();
# ifdef FEAT_தேடல்_EXTRA
    restore_incsearch_state();
# endif
}
#endif
