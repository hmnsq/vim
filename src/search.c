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

#अन्तर्भाव "vim.h"

#ifdef FEAT_EVAL
स्थिर शून्य set_vv_searchforward(शून्य);
स्थिर पूर्णांक first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
स्थिर char_u *get_ओळ_and_copy(linenr_T lnum, char_u *buf);
स्थिर शून्य show_pat_in_path(char_u *, पूर्णांक, पूर्णांक, पूर्णांक, खाता *, linenr_T *, दीर्घ);
#endif

प्रकारव्याख्या संरचना searchstat
{
    पूर्णांक	    cur;	    // current position of found words
    पूर्णांक	    cnt;	    // total count of found words
    पूर्णांक	    exact_match;    // TRUE if matched exactly on specified position
    पूर्णांक	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    पूर्णांक	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_शोध_EXTRA
स्थिर शून्य save_incsearch_state(शून्य);
स्थिर शून्य restore_incsearch_state(शून्य);
#endif
स्थिर पूर्णांक check_prevcol(char_u *linep, पूर्णांक col, पूर्णांक ch, पूर्णांक *prevcol);
स्थिर पूर्णांक find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
स्थिर शून्य find_mps_मूल्ये(पूर्णांक *initc, पूर्णांक *findc, पूर्णांक *backwards, पूर्णांक switchit);
स्थिर पूर्णांक is_zero_width(char_u *pattern, माप_प्रकार patternlen, पूर्णांक move, pos_T *cur, पूर्णांक direction);
स्थिर शून्य cmdline_शोध_stat(पूर्णांक dirc, pos_T *pos, pos_T *cursor_pos, पूर्णांक show_शीर्ष_bot_संदेश, char_u *msgbuf, माप_प्रकार msgbuflen, पूर्णांक recompute, पूर्णांक maxcount, दीर्घ timeout);
स्थिर शून्य update_शोध_stat(पूर्णांक dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, पूर्णांक recompute, पूर्णांक maxcount, दीर्घ timeout);

#define शोध_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define शोध_STAT_BUF_LEN 16

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
स्थिर spat_T spats[2] =
{
    {रिक्त, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {रिक्त, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

स्थिर पूर्णांक last_idx = 0;	// index in spats[] for RE_LAST

स्थिर char_u lastc[2] = {NUL, NUL};	// last character searched for
स्थिर पूर्णांक lastcdir = FORWARD;		// last direction of character search
स्थिर पूर्णांक last_t_cmd = TRUE;		// last search t_cmd
स्थिर char_u	lastc_bytes[MB_MAXBYTES + 1];
स्थिर पूर्णांक	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
स्थिर spat_T	    saved_spats[ARRAY_LENGTH(spats)];
स्थिर char_u	    *saved_mr_pattern = रिक्त;
स्थिर माप_प्रकार	    saved_mr_patternlen = 0;
#ifdef FEAT_शोध_EXTRA
स्थिर पूर्णांक	    saved_spats_last_idx = 0;
स्थिर पूर्णांक	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
स्थिर char_u	    *mr_pattern = रिक्त;
स्थिर माप_प्रकार	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
प्रकारव्याख्या संरचना SearchedFile
{
    खाता	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    पूर्णांक		matched;	// Found a match in this file
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
    पूर्णांक
शोध_regcomp(
    char_u	*pat,
    माप_प्रकार	patlen,
    char_u	**used_pat,
    पूर्णांक		pat_save,
    पूर्णांक		pat_use,
    पूर्णांक		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    पूर्णांक		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    जर (pat == रिक्त || *pat == NUL)
    {
	पूर्णांक i;

	जर (pat_use == RE_LAST)
	    i = last_idx;
	अन्यथा
	    i = pat_use;
	जर (spats[i].pat == रिक्त)	// pattern was never defined
	{
	    जर (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    अन्यथा
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    परत FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    अन्यथा जर (options & शोध_HIS)	// put new pattern in history
	add_to_history(HIST_शोध, pat, patlen, TRUE, NUL);

    जर (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    जर (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    अन्यथा
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    जर (mr_pattern == रिक्त)
	mr_patternlen = 0;
    अन्यथा
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    जर (!(options & शोध_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	जर (pat_save == RE_शोध || pat_save == RE_BOTH)
	    save_re_pat(RE_शोध, pat, patlen, magic);
	// substitute or global command
	जर (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    जर (regmatch->regprog == रिक्त)
	परत FAIL;
    परत OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_शोध_pat(शून्य)
{
    परत mr_pattern;
}

    शून्य
save_re_pat(पूर्णांक idx, char_u *pat, माप_प्रकार patlen, पूर्णांक magic)
{
    जर (spats[idx].pat == pat)
	परत;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    जर (spats[idx].pat == रिक्त)
	spats[idx].patlen = 0;
    अन्यथा
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_शोध_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    जर (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
स्थिर पूर्णांक save_level = 0;

    शून्य
save_शोध_patterns(शून्य)
{
    पूर्णांक i;

    जर (save_level++ != 0)
	परत;

    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	जर (spats[i].pat != रिक्त)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    जर (saved_spats[i].pat == रिक्त)
		saved_spats[i].patlen = 0;
	    अन्यथा
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    जर (mr_pattern == रिक्त)
	saved_mr_pattern = रिक्त;
    अन्यथा
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    जर (saved_mr_pattern == रिक्त)
	saved_mr_patternlen = 0;
    अन्यथा
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_शोध_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    शून्य
restore_शोध_patterns(शून्य)
{
    पूर्णांक i;

    जर (--save_level != 0)
	परत;

    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#जर defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_शोध_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#जर defined(EXITFREE)
    शून्य
free_शोध_patterns(शून्य)
{
    पूर्णांक i;

    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_शोध_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
स्थिर spat_T	    saved_last_शोध_spat;
स्थिर पूर्णांक	    did_save_last_शोध_spat = 0;
स्थिर पूर्णांक	    saved_last_idx = 0;
स्थिर पूर्णांक	    saved_no_hlsearch = 0;
स्थिर पूर्णांक	    saved_शोध_match_endcol;
स्थिर पूर्णांक	    saved_शोध_match_ओळी;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    शून्य
save_last_शोध_pattern(शून्य)
{
    जर (++did_save_last_शोध_spat != 1)
	// nested call, nothing to do
	परत;

    saved_last_शोध_spat = spats[RE_शोध];
    जर (spats[RE_शोध].pat != रिक्त)
    {
	saved_last_शोध_spat.pat = vim_strnsave(spats[RE_शोध].pat, spats[RE_शोध].patlen);
	जर (saved_last_शोध_spat.pat == रिक्त)
	    saved_last_शोध_spat.patlen = 0;
	अन्यथा
	    saved_last_शोध_spat.patlen = spats[RE_शोध].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    शून्य
restore_last_शोध_pattern(शून्य)
{
    जर (--did_save_last_शोध_spat > 0)
	// nested call, nothing to do
	परत;
    जर (did_save_last_शोध_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	परत;
    }

    vim_free(spats[RE_शोध].pat);
    spats[RE_शोध] = saved_last_शोध_spat;
    saved_last_शोध_spat.pat = रिक्त;
    saved_last_शोध_spat.patlen = 0;
# जर defined(FEAT_EVAL)
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
    स्थिर शून्य
save_incsearch_state(शून्य)
{
    saved_शोध_match_endcol = शोध_match_endcol;
    saved_शोध_match_ओळी  = शोध_match_ओळी;
}

    स्थिर शून्य
restore_incsearch_state(शून्य)
{
    शोध_match_endcol = saved_शोध_match_endcol;
    शोध_match_ओळी  = saved_शोध_match_ओळी;
}

    char_u *
last_शोध_pattern(शून्य)
{
    परत spats[RE_शोध].pat;
}

    माप_प्रकार
last_शोध_pattern_len(शून्य)
{
    परत spats[RE_शोध].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    पूर्णांक
ignorecase(char_u *pat)
{
    परत ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    पूर्णांक
ignorecase_opt(char_u *pat, पूर्णांक ic_in, पूर्णांक scs)
{
    पूर्णांक		ic = ic_in;

    जर (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    परत ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    पूर्णांक
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (शून्य)skip_regexp_ex(pat, NUL, magic_isset(), रिक्त, रिक्त, &magic_val);

    पर्यंत (*p != NUL)
    {
	पूर्णांक		l;

	जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    जर (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		परत TRUE;
	    p += l;
	}
	अन्यथा जर (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    जर (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    अन्यथा जर (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    अन्यथा जर (p[1] != NUL)  // skip "\X"
		p += 2;
	    अन्यथा
		p += 1;
	}
	अन्यथा जर ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    जर (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    अन्यथा
		p++;
	}
	अन्यथा जर (MB_ISUPPER(*p))
	    परत TRUE;
	अन्यथा
	    ++p;
    }
    परत FALSE;
}

#जर defined(FEAT_EVAL)
    char_u *
last_csearch(शून्य)
{
    परत lastc_bytes;
}

    पूर्णांक
last_csearch_forward(शून्य)
{
    परत lastcdir == FORWARD;
}

    पूर्णांक
last_csearch_until(शून्य)
{
    परत last_t_cmd == TRUE;
}

    शून्य
set_last_csearch(पूर्णांक c, char_u *s, पूर्णांक len)
{
    *lastc = c;
    lastc_bytelen = len;
    जर (len)
	स_नकल(lastc_bytes, s, len);
    अन्यथा
	CLEAR_FIELD(lastc_bytes);
}
#endif

    शून्य
set_csearch_direction(पूर्णांक cdir)
{
    lastcdir = cdir;
}

    शून्य
set_csearch_until(पूर्णांक t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_शोध_pat(शून्य)
{
    परत spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    शून्य
reset_शोध_dir(शून्य)
{
    spats[0].off.dir = '/';
#जर defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#जर defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    शून्य
set_last_शोध_pat(
    char_u	*s,
    पूर्णांक		idx,
    पूर्णांक		magic,
    पूर्णांक		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    जर (*s == NUL)
	spats[idx].pat = रिक्त;
    अन्यथा
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    जर (spats[idx].pat == रिक्त)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# जर defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.ओळ = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    जर (setlast)
	last_idx = idx;
    जर (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	जर (spats[idx].pat == रिक्त)
	    saved_spats[idx].pat = रिक्त;
	अन्यथा
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	जर (saved_spats[idx].pat == रिक्त)
	    saved_spats[idx].patlen = 0;
	अन्यथा
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_शोध_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_शोध_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    जर (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_शोध_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    शून्य
last_pat_prog(regmmatch_T *regmatch)
{
    जर (spats[last_idx].pat == रिक्त)
    {
	regmatch->regprog = रिक्त;
	परत;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (शून्य)शोध_regcomp((char_u *)"", 0, रिक्त, 0, last_idx, शोध_KEEP, regmatch);
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
    पूर्णांक
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    पूर्णांक		dir,
    char_u	*pat,
    माप_प्रकार	patlen,
    दीर्घ	मोजणी,
    पूर्णांक		options,
    पूर्णांक		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    पूर्णांक		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    पूर्णांक		loop;
    pos_T	start_pos;
    पूर्णांक		at_first_ओळ;
    पूर्णांक		extra_col;
    पूर्णांक		start_char_len;
    पूर्णांक		match_ok;
    दीर्घ	nmatched;
    पूर्णांक		submatch = 0;
    पूर्णांक		first_match = TRUE;
    पूर्णांक		called_emsg_before = called_emsg;
#ifdef FEAT_शोध_EXTRA
    पूर्णांक		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    पूर्णांक		unused_timeout_flag = FALSE;
    पूर्णांक		*timed_out = &unused_timeout_flag;  // set when timed out.
    पूर्णांक		शोध_from_match_end;		    // vi-compatible search?

    जर (शोध_regcomp(pat, patlen, रिक्त, RE_शोध, pat_use,
		   (options & (शोध_HIS + शोध_KEEP)), &regmatch) == FAIL)
    {
	जर ((options & शोध_संदेश) && !rc_did_emsg)
	    semsg(_(e_invalid_शोध_string_str), mr_pattern);
	परत FAIL;
    }

    शोध_from_match_end = vim_strchr(p_cpo, CPO_शोध) != रिक्त;

    जर (extra_arg != रिक्त)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	जर (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    करा	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	जर (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	अन्यथा जर (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_ओळ_मोजणी
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    जर (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    अन्यथा
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	अन्यथा
	    start_char_len = 1;
	जर (dir == FORWARD)
	{
	    जर (options & शोध_START)
		extra_col = 0;
	    अन्यथा
		extra_col = start_char_len;
	}
	अन्यथा
	{
	    जर (options & शोध_START)
		extra_col = start_char_len;
	    अन्यथा
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_ओळ = TRUE;	// default: start in first line
	जर (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_ओळ = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	जर (dir == BACKWARD && start_pos.col == 0
					     && (options & शोध_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_ओळ = FALSE;
	}
	अन्यथा
	    lnum = pos->lnum;

	साठी (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    साठी ( ; lnum > 0 && lnum <= buf->b_ml.ml_ओळ_मोजणी;
					   lnum += dir, at_first_ओळ = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		जर (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    थांबा;
		// Stop after passing the time limit.
		जर (*timed_out)
		    थांबा;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_ओळ && (options & शोध_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		जर (regmatch.regprog == रिक्त)
		    थांबा;
		// Abort searching on an error (e.g., out of stack).
		जर (called_emsg > called_emsg_before || *timed_out)
		    थांबा;
		जर (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    जर (lnum + matchpos.lnum > buf->b_ml.ml_ओळ_मोजणी)
			ptr = (char_u *)"";
		    अन्यथा
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    जर (dir == FORWARD && at_first_ओळ)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			पर्यंत (matchpos.lnum == 0
				&& ((options & शोध_END) && first_match
				    ?  (nmatched == 1
					&& (पूर्णांक)endpos.col - 1
					     < (पूर्णांक)start_pos.col + extra_col)
				    : ((पूर्णांक)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (पूर्णांक)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    जर (शोध_from_match_end)
			    {
				जर (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    थांबा;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				जर (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    जर (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अन्यथा
					++matchcol;
				}
			    }
			    अन्यथा
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				जर (ptr[matchcol] != NUL)
				{
				    जर (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    अन्यथा
					++matchcol;
				}
			    }
			    जर (matchcol == 0 && (options & शोध_START))
				थांबा;
			    जर (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				थांबा;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    जर (regmatch.regprog == रिक्त)
				थांबा;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			जर (!match_ok)
			    सुरूठेवा;
		    }
		    जर (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			साठी (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    जर (loop
				|| ((options & शोध_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (पूर्णांक)regmatch.endpos[0].col - 1
							< (पूर्णांक)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (पूर्णांक)regmatch.startpos[0].col
						      < (पूर्णांक)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    अन्यथा
				थांबा;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    जर (शोध_from_match_end)
			    {
				जर (nmatched > 1)
				    थांबा;
				matchcol = endpos.col;
				// for empty match: advance one char
				जर (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    जर (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अन्यथा
					++matchcol;
				}
			    }
			    अन्यथा
			    {
				// Stop when the match is in a next line.
				जर (matchpos.lnum > 0)
				    थांबा;
				matchcol = matchpos.col;
				जर (ptr[matchcol] != NUL)
				{
				    जर (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अन्यथा
					++matchcol;
				}
			    }
			    जर (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				जर (*timed_out)
				    match_ok = FALSE;
				थांबा;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    जर (regmatch.regprog == रिक्त)
				थांबा;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			जर (!match_ok)
			    सुरूठेवा;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    जर ((options & शोध_END) && !(options & शोध_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			जर (endpos.col == 0)
			{
			    जर (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			अन्यथा
			{
			    --pos->col;
			    जर (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_ओळ_मोजणी)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			जर (end_pos != रिक्त)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    अन्यथा
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			जर (end_pos != रिक्त)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    जर (end_pos != रिक्त)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    शोध_match_ओळी = endpos.lnum - matchpos.lnum;
		    शोध_match_endcol = endpos.col;
		    थांबा;
		}
		ओळ_breakcheck();	// stop if ctrl-C typed
		जर (got_int)
		    थांबा;

#ifdef FEAT_शोध_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		जर ((options & शोध_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    थांबा;
		}
#endif

		जर (loop && lnum == start_pos.lnum)
		    थांबा;	    // if second loop, stop where started
	    }
	    at_first_ओळ = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    जर (regmatch.regprog == रिक्त)
		थांबा;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    जर (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_शोध_EXTRA
			      || break_loop
#endif
			      || found || loop)
		थांबा;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    जर (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_ओळ_मोजणी;
	    अन्यथा
		lnum = 1;
	    जर (!shortmess(SHM_शोध)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & शोध_संदेश))
		give_warning((char_u *)_(dir == BACKWARD
					  ? शीर्ष_bot_संदेश : bot_शीर्ष_संदेश), TRUE);
	    जर (extra_arg != रिक्त)
		extra_arg->sa_wrapped = TRUE;
	}
	जर (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_शोध_EXTRA
		|| break_loop
#endif
		)
	    थांबा;
    }
    पर्यंत (--मोजणी > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    जर (extra_arg != रिक्त && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    जर (!found)		    // did not find it
    {
	जर (got_int)
	    emsg(_(e_interrupted));
	अन्यथा जर ((options & शोध_संदेश) == शोध_संदेश)
	{
	    जर (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    अन्यथा जर (lnum == 0)
		semsg(_(e_शोध_hit_शीर्ष_without_match_for_str), mr_pattern);
	    अन्यथा
		semsg(_(e_शोध_hit_bottom_without_match_for_str), mr_pattern);
	}
	परत FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    जर (pos->lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	pos->lnum = buf->b_ml.ml_ओळ_मोजणी;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	जर (pos->col > 0)
	    --pos->col;
    }

    परत submatch + 1;
}

#जर defined(FEAT_EVAL) || defined(FEAT_PROTO)
    शून्य
set_शोध_direction(पूर्णांक cdir)
{
    spats[0].off.dir = cdir;
}

    स्थिर शून्य
set_vv_searchforward(शून्य)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (दीर्घ)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    स्थिर पूर्णांक
first_submatch(regmmatch_T *rp)
{
    पूर्णांक		submatch;

    साठी (submatch = 1; ; ++submatch)
    {
	जर (rp->startpos[submatch].lnum >= 0)
	    थांबा;
	जर (submatch == 9)
	{
	    submatch = 0;
	    थांबा;
	}
    }
    परत submatch;
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
    पूर्णांक
parse_शोध_pattern_offset(
    char_u	**pat,
    माप_प्रकार	*patlen,
    पूर्णांक		शोध_delim,
    पूर्णांक		options,
    char_u	**strcopy,
    char_u	**searchstr,
    माप_प्रकार	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    पूर्णांक		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    जर (*pat == रिक्त || **pat == NUL)
	परत 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = रिक्त;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, शोध_delim, magic_isset(),
						    strcopy, रिक्त, रिक्त);
    जर (*strcopy != ps)
    {
	माप_प्रकार len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (पूर्णांक)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    जर (*p == शोध_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->ओळ = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    जर (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->ओळ = TRUE;
    अन्यथा जर ((options & शोध_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	जर (*p == 'e')		// end
	    offset->end = शोध_END;
	++p;
    }
    जर (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	जर (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = म_से_द((अक्षर *)p);
	अन्यथा जर (*p == '-')	    // single '-'
	    offset->off = -1;
	अन्यथा			    // single '+'
	    offset->off = 1;
	++p;
	पर्यंत (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (पूर्णांक)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    परत cmdlen;
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
    पूर्णांक
do_शोध(
    oparg_T	    *oap,	// can be NULL
    पूर्णांक		    dirc,	// '/' or '?'
    पूर्णांक		    शोध_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    माप_प्रकार	    patlen,
    दीर्घ	    मोजणी,
    पूर्णांक		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    माप_प्रकार	    searchstrlen;
    soffset_T	    old_off;
    पूर्णांक		    retval;	// Return value
    char_u	    *p;
    दीर्घ	    c;
    char_u	    *dircp;
    char_u	    *strcopy = रिक्त;
    पूर्णांक		    show_शोध_stats;
    char_u	    *msgbuf = रिक्त;
    माप_प्रकार	    msgbuflen = 0;
    पूर्णांक		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    जर (spats[0].off.ओळ && vim_strchr(p_cpo, CPO_LINEOFF) != रिक्त)
    {
	spats[0].off.ओळ = FALSE;
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
    जर (dirc == 0)
	dirc = spats[0].off.dir;
    अन्यथा
    {
	spats[0].off.dir = dirc;
#जर defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    जर (options & शोध_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#अन्यथा
	जर (dirc == '/')
	    dirc = '?';
	अन्यथा
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    जर (dirc == '/')
    {
	जर (hasFolding(pos.lnum, रिक्त, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    अन्यथा
    {
	जर (hasFolding(pos.lnum, &pos.lnum, रिक्त))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_शोध_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    जर (no_hlsearch && !(options & शोध_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    साठी (;;)
    {
	पूर्णांक		show_शीर्ष_bot_संदेश = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = रिक्त;
					    // use previous pattern
	जर (pat == रिक्त || *pat == NUL || *pat == शोध_delim)
	{
	    जर (spats[RE_शोध].pat == रिक्त)	    // no previous pattern
	    {
		जर (spats[RE_SUBST].pat == रिक्त)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    जा end_do_शोध;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    अन्यथा
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	जर (pat != रिक्त && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    माप_प्रकार	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_शोध_pattern_offset(&pat, &patlen,
				शोध_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    जर (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_शोध_stats = FALSE;
	जर ((options & शोध_ECHO) && messaging()
		&& !संदेश_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    माप_प्रकार	off_len = 0;
	    माप_प्रकार	plen;
	    माप_प्रकार	msgbufsize;

	    // Compute msg_row early.
	    संदेश_start();

	    // Get the offset, so we know how long it is.
	    जर (!cmd_silent &&
		    (spats[0].off.ओळ || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		जर (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		अन्यथा जर (!spats[0].off.ओळ)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		जर (spats[0].off.off != 0 || spats[0].off.ओळ)
		    off_len += vim_snprintf((अक्षर *)off_buf + off_len,
			   आकार(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    जर (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    अन्यथा
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    जर (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		जर (संदेश_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (पूर्णांक)(Rows - संदेश_row) * cmdline_width - 1;
		अन्यथा
		    // Use up to 'showcmd' column.
		    msgbufsize = (पूर्णांक)(Rows - संदेश_row - 1) * cmdline_width
			+ sc_col - 1;
		जर (msgbufsize < plen + off_len + शोध_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + शोध_STAT_BUF_LEN + 3;
	    }
	    अन्यथा
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    जर (msgbuf == रिक्त)
	    {
		msgbuflen = 0;
	    }
	    अन्यथा
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		जर (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    जर (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    अन्यथा
			mch_memmove(msgbuf + 1, p, plen);
		    जर (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = संदेश_strtrunc(msgbuf, TRUE);
		    जर (trunc != रिक्त)
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
		    जर (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			माप_प्रकार pat_len;

			r = reverse_text(msgbuf);
			जर (r != रिक्त)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    पर्यंत (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    जर ((माप_प्रकार)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    अन्यथा
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    संदेश_outtrans(msgbuf);
		    संदेश_clr_eos();
		    संदेश_check();

		    gotocmdline(FALSE);
		    out_flush();
		    संदेश_nowait = TRUE;	    // don't wait for this message
		}

		जर (!shortmess(SHM_SEARCHCOUNT))
		    show_शोध_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	जर (!spats[0].off.ओळ && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    जर (spats[0].off.off > 0)
	    {
		साठी (c = spats[0].off.off; c; --c)
		    जर (decl(&pos) == -1)
			थांबा;
		जर (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    अन्यथा
	    {
		साठी (c = spats[0].off.off; c; ++c)
		    जर (incl(&pos) == -1)
			थांबा;
		जर (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_ओळ_मोजणी + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, रिक्त,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, मोजणी, spats[0].off.end + (options &
		       (शोध_KEEP + शोध_PEEK + शोध_HIS
			+ शोध_संदेश + शोध_START
			+ ((pat != रिक्त && *pat == ';') ? 0 : शोध_NOOF))),
		RE_LAST, sia);

	जर (dircp != रिक्त)
	    *dircp = शोध_delim; // restore second '/' or '?' for normal_cmd()

	जर (!shortmess(SHM_शोध) && sia && sia->sa_wrapped)
	    show_शीर्ष_bot_संदेश = TRUE;

	जर (c == FAIL)
	{
	    retval = 0;
	    जा end_do_शोध;
	}
	जर (spats[0].off.end && oap != रिक्त)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	जर (!(options & शोध_NOOF) || (pat != रिक्त && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    जर (spats[0].off.ओळ)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		जर (c < 1)
		    pos.lnum = 1;
		अन्यथा जर (c > curbuf->b_ml.ml_ओळ_मोजणी)
		    pos.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
		अन्यथा
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    अन्यथा जर (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		जर (c > 0)
		{
		    पर्यंत (c-- > 0)
			जर (incl(&pos) == -1)
			    थांबा;
		}
		// to the left, check for start of file
		अन्यथा
		{
		    पर्यंत (c++ < 0)
			जर (decl(&pos) == -1)
			    थांबा;
		}
	    }
	    जर (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	जर (show_शोध_stats)
	     cmdline_शोध_stat(dirc, &pos, &curwin->w_cursor,
				show_शीर्ष_bot_संदेश, msgbuf, msgbuflen,
				(मोजणी != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_शोध)
				     && hasFolding(curwin->w_cursor.lnum,
								   रिक्त, रिक्त))
#endif
				),
				p_msc,
				शोध_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	जर (!(options & शोध_OPT) || pat == रिक्त || *pat != ';')
	    थांबा;

	dirc = *++pat;
	--patlen;
	शोध_delim = dirc;
	जर (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    जा end_do_शोध;
	}
	++pat;
	--patlen;
    }

    जर (options & शोध_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_शोध:
    जर ((options & शोध_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    परत retval;
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
    पूर्णांक
शोध_for_exact_ओळ(
    buf_T	*buf,
    pos_T	*pos,
    पूर्णांक		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    पूर्णांक		compl_len = ins_compl_len();

    जर (buf->b_ml.ml_ओळ_मोजणी == 0)
	परत FAIL;
    साठी (;;)
    {
	pos->lnum += dir;
	जर (pos->lnum < 1)
	{
	    जर (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_ओळ_मोजणी;
		जर (!shortmess(SHM_शोध))
		    give_warning((char_u *)_(शीर्ष_bot_संदेश), TRUE);
	    }
	    अन्यथा
	    {
		pos->lnum = 1;
		थांबा;
	    }
	}
	अन्यथा जर (pos->lnum > buf->b_ml.ml_ओळ_मोजणी)
	{
	    जर (p_ws)
	    {
		pos->lnum = 1;
		जर (!shortmess(SHM_शोध))
		    give_warning((char_u *)_(bot_शीर्ष_संदेश), TRUE);
	    }
	    अन्यथा
	    {
		pos->lnum = 1;
		थांबा;
	    }
	}
	जर (pos->lnum == start)
	    थांबा;
	जर (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	जर (compl_status_adding() && !compl_status_sol())
	{
	    जर ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		परत OK;
	}
	अन्यथा जर (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    जर ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		परत OK;
	}
    }
    परत FAIL;
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
    पूर्णांक
searchc(cmdarg_T *cap, पूर्णांक t_cmd)
{
    पूर्णांक			c = cap->nchar;	// char to search for
    पूर्णांक			dir = cap->arg;	// TRUE for searching forward
    दीर्घ		मोजणी = cap->count1;	// repeat count
    पूर्णांक			col;
    char_u		*p;
    पूर्णांक			len;
    पूर्णांक			stop = TRUE;

    जर (c != NUL)	// normal search: remember args for repeat
    {
	जर (!किल्ली_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    जर (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		जर (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    अन्यथा		// repeat previous search
    {
	जर (*lastc == NUL && lastc_bytelen <= 1)
	    परत FAIL;
	जर (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	अन्यथा
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	जर (vim_strchr(p_cpo, CPO_SCOLON) == रिक्त && मोजणी == 1 && t_cmd)
	    stop = FALSE;
    }

    जर (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    अन्यथा
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    पर्यंत (मोजणी--)
    {
	जर (has_mbyte)
	{
	    साठी (;;)
	    {
		जर (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    जर (col >= len)
			परत FAIL;
		}
		अन्यथा
		{
		    जर (col == 0)
			परत FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		जर (lastc_bytelen <= 1)
		{
		    जर (p[col] == c && stop)
			थांबा;
		}
		अन्यथा जर (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    थांबा;
		stop = TRUE;
	    }
	}
	अन्यथा
	{
	    साठी (;;)
	    {
		जर ((col += dir) < 0 || col >= len)
		    परत FAIL;
		जर (p[col] == c && stop)
		    थांबा;
		stop = TRUE;
	    }
	}
    }

    जर (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	जर (has_mbyte)
	{
	    जर (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    अन्यथा
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    परत OK;
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
findmatch(oparg_T *oap, पूर्णांक initc)
{
    परत findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    स्थिर पूर्णांक
check_prevcol(
    char_u	*linep,
    पूर्णांक		col,
    पूर्णांक		ch,
    पूर्णांक		*prevcol)
{
    --col;
    जर (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    जर (prevcol)
	*prevcol = col;
    परत (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    स्थिर पूर्णांक
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    माप_प्रकार	delim_len;
    linenr_T	lnum;
    पूर्णांक		found = FALSE;

    साठी (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    जर (delim_copy == रिक्त)
	परत FALSE;
    साठी (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *ओळ = ml_get(lnum);

	साठी (p = ओळ + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    जर (lnum == endpos->lnum && (colnr_T)(p - ओळ) >= endpos->col)
		थांबा;
	    जर (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		थांबा;
	    }
	}
	जर (found)
	    थांबा;
    }
    vim_free(delim_copy);
    परत found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    स्थिर शून्य
find_mps_मूल्ये(
    पूर्णांक	    *initc,
    पूर्णांक	    *findc,
    पूर्णांक	    *backwards,
    पूर्णांक	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    पर्यंत (*ptr != NUL)
    {
	जर (has_mbyte)
	{
	    char_u *prev;

	    जर (mb_ptr2char(ptr) == *initc)
	    {
		जर (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		अन्यथा
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		परत;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    जर (mb_ptr2char(ptr) == *initc)
	    {
		जर (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		अन्यथा
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		परत;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	अन्यथा
	{
	    जर (*ptr == *initc)
	    {
		जर (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		अन्यथा
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		परत;
	    }
	    ptr += 2;
	    जर (*ptr == *initc)
	    {
		जर (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		अन्यथा
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		परत;
	    }
	    ++ptr;
	}
	जर (*ptr == ',')
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
    पूर्णांक		initc,
    पूर्णांक		flags,
    पूर्णांक		maxtravel)
{
    स्थिर pos_T pos;			// current search position
    पूर्णांक		findc = 0;		// matching brace
    पूर्णांक		c;
    पूर्णांक		मोजणी = 0;		// cumulative number of braces
    पूर्णांक		backwards = FALSE;	// init for gcc
    पूर्णांक		raw_string = FALSE;	// search for raw string
    पूर्णांक		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    पूर्णांक		do_quotes;		// check for quotes in current line
    पूर्णांक		at_start;		// do_quotes value at start position
    पूर्णांक		hash_dir = 0;		// Direction searched for # things
    पूर्णांक		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    पूर्णांक		start_in_quotes;	// start position is in quotes
    पूर्णांक		traveled = 0;		// how far we've searched so far
    पूर्णांक		ignore_cend = FALSE;    // ignore comment end
    पूर्णांक		cpo_match;		// vi compatible matching
    पूर्णांक		cpo_bsl;		// don't recognize backslashes
    पूर्णांक		match_escaped = 0;	// search for escaped match
    पूर्णांक		dir;			// Direction to search
    पूर्णांक		comment_col = MAXCOL;   // start of / / comment
    पूर्णांक		lispcomm = FALSE;	// inside of Lisp-style comment
    पूर्णांक		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    पूर्णांक		skip_comments = (flags & FM_SKIPCOMM) != 0;
    पूर्णांक		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != रिक्त);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != रिक्त);

    // Direction to search when initc is '/', '*' or '#'
    जर (flags & FM_BACKWARD)
	dir = BACKWARD;
    अन्यथा जर (flags & FM_FORWARD)
	dir = FORWARD;
    अन्यथा
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    जर (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	जर (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    अन्यथा जर (initc != '#' && initc != NUL)
    {
	find_mps_मूल्ये(&initc, &findc, &backwards, TRUE);
	जर (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	जर (findc == NUL)
	    परत रिक्त;
    }
    अन्यथा
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	जर (initc == '#')
	{
	    hash_dir = dir;
	}
	अन्यथा
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    जर (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		जर (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    जर (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		अन्यथा जर (linep[pos.col] == '/')
		{
		    जर (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    अन्यथा जर (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		अन्यथा जर (linep[pos.col] == '*')
		{
		    जर (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    अन्यथा जर (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    जर (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		जर (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		साठी (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    जर (initc == NUL)
			थांबा;

		    find_mps_मूल्ये(&initc, &findc, &backwards, FALSE);
		    जर (findc)
			थांबा;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		जर (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    जर (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    अन्यथा
			परत रिक्त;
		}
		अन्यथा जर (!cpo_bsl)
		{
		    पूर्णांक col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    साठी (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	जर (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    जर (oap != रिक्त)
		oap->motion_type = MLINE;   // Linewise for this case only
	    जर (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		जर (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		अन्यथा जर (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		अन्यथा
		    परत रिक्त;
	    }
	    pos.col = 0;
	    पर्यंत (!got_int)
	    {
		जर (hash_dir > 0)
		{
		    जर (pos.lnum == curbuf->b_ml.ml_ओळ_मोजणी)
			थांबा;
		}
		अन्यथा जर (pos.lnum == 1)
		    थांबा;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		ओळ_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		जर (*ptr != '#')
		    सुरूठेवा;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		जर (hash_dir > 0)
		{
		    जर (STRNCMP(ptr, "if", 2) == 0)
			मोजणी++;
		    अन्यथा जर (STRNCMP(ptr, "el", 2) == 0)
		    {
			जर (मोजणी == 0)
			    परत &pos;
		    }
		    अन्यथा जर (STRNCMP(ptr, "endif", 5) == 0)
		    {
			जर (मोजणी == 0)
			    परत &pos;
			मोजणी--;
		    }
		}
		अन्यथा
		{
		    जर (STRNCMP(ptr, "if", 2) == 0)
		    {
			जर (मोजणी == 0)
			    परत &pos;
			मोजणी--;
		    }
		    अन्यथा जर (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			जर (मोजणी == 0)
			    परत &pos;
		    }
		    अन्यथा जर (STRNCMP(ptr, "endif", 5) == 0)
			मोजणी++;
		}
	    }
	    परत रिक्त;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    जर (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != रिक्त)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    जर ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    जर (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    जर (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    पर्यंत (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	जर (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    जर (lispcomm && pos.col < (colnr_T)comment_col)
		थांबा;
	    जर (pos.col == 0)		// at start of line, go to prev. one
	    {
		जर (pos.lnum == 1)	// start of file
		    थांबा;
		--pos.lnum;

		जर (maxtravel > 0 && ++traveled > maxtravel)
		    थांबा;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		ओळ_breakcheck();

		// Check if this line contains a single-line comment
		जर (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		जर (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		अन्यथा जर (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    अन्यथा
	    {
		--pos.col;
		जर (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	अन्यथा				// forward search
	{
	    जर (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		जर (pos.lnum == curbuf->b_ml.ml_ओळ_मोजणी  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    थांबा;
		++pos.lnum;

		जर (maxtravel && traveled++ > maxtravel)
		    थांबा;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		ओळ_breakcheck();
		जर (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    अन्यथा
	    {
		जर (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		अन्यथा
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	जर (skip_comments && !comment_dir && !inquote)
	{
	    जर (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		जर (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (पूर्णांक)pos.col < comment_col))
		    in_block_comment = TRUE;
		अन्यथा जर (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    अन्यथा
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		जर (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (पूर्णांक)pos.col < comment_col))
		    in_block_comment = TRUE;
		अन्यथा जर (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	जर (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    जर (linep[0] == findc && मोजणी == 0)	// match!
		परत &pos;
	    थांबा;					// out of scope
	}

	जर (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    जर (comment_dir == FORWARD)
	    {
		जर (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    परत &pos;
		}
	    }
	    अन्यथा    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		जर (pos.col == 0)
		    सुरूठेवा;
		अन्यथा जर (raw_string)
		{
		    जर (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != रिक्त)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			जर (!find_rawstring_end(linep, &pos,
				  मोजणी > 0 ? &match_pos : &curwin->w_cursor))
			{
			    मोजणी++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		अन्यथा जर (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (पूर्णांक)pos.col < comment_col)
		{
		    मोजणी++;
		    match_pos = pos;
		    match_pos.col--;
		}
		अन्यथा जर (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    जर (मोजणी > 0)
			pos = match_pos;
		    अन्यथा जर (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (पूर्णांक)pos.col <= comment_col)
			pos.col -= 2;
		    अन्यथा जर (ignore_cend)
			सुरूठेवा;
		    अन्यथा
			परत रिक्त;
		    परत &pos;
		}
	    }
	    सुरूठेवा;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	जर (cpo_match)
	    do_quotes = 0;
	अन्यथा जर (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    साठी (ptr = linep; *ptr; ++ptr)
	    {
		जर (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		जर (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		जर (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    जर (!do_quotes)
	    {
		inquote = FALSE;
		जर (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    जर (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			जर (inquote)
			    start_in_quotes = TRUE;
		    }
		    अन्यथा जर (backwards)
			inquote = TRUE;
		}
		जर (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    जर (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			जर (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    जर (inquote)
				start_in_quotes = TRUE;
			}
			अन्यथा जर (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	जर (start_in_quotes == MAYBE)
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
	निवड (c)
	{
	स्थिती NUL:
	    // at end of line without trailing backslash, reset inquote
	    जर (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    थांबा;

	स्थिती '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    जर (do_quotes)
	    {
		पूर्णांक col;

		साठी (col = pos.col - 1; col >= 0; --col)
		    जर (linep[col] != '\\')
			थांबा;
		जर ((((पूर्णांक)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    थांबा;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	स्थिती '\'':
	    जर (!cpo_match && initc != '\'' && findc != '\'')
	    {
		जर (backwards)
		{
		    जर (pos.col > 1)
		    {
			जर (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    थांबा;
			}
			अन्यथा जर (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    थांबा;
			}
		    }
		}
		अन्यथा जर (linep[pos.col + 1])	// forward search
		{
		    जर (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			थांबा;
		    }
		    अन्यथा जर (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			थांबा;
		    }
		}
	    }
	    // FALLTHROUGH

	पूर्वनिर्धारित:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    जर (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != रिक्त
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', रिक्त)
		    && check_prevcol(linep, pos.col - 1, '#', रिक्त))
		थांबा;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    जर (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (पूर्णांक)pos.col >= comment_col)))
		थांबा;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    जर ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		पूर्णांक	col, bslcnt = 0;

		जर (!cpo_bsl)
		{
		    साठी (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		जर (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    जर (c == initc)
			मोजणी++;
		    अन्यथा
		    {
			जर (मोजणी == 0)
			    परत &pos;
			मोजणी--;
		    }
		}
	    }
	}
    }

    जर (comment_dir == BACKWARD && मोजणी > 0)
    {
	pos = match_pos;
	परत &pos;
    }
    परत (pos_T *)रिक्त;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    शून्य
showmatch(
    पूर्णांक		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    दीर्घ	save_so;
    दीर्घ	save_siso;
#ifdef CURSOR_SHAPE
    पूर्णांक		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    दीर्घ	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    दीर्घ	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    साठी (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	जर (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    थांबा;
#endif
	p += mb_ptr2len(p) + 1;
	जर (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    थांबा;
	p += mb_ptr2len(p);
	जर (*p == NUL)
	    परत;
    }
    जर (*p == NUL)
	परत;

    जर ((lpos = findmatch(रिक्त, NUL)) == रिक्त)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	परत;
    }

    जर (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	परत;

    जर (!curwin->w_p_wrap)
	getvcol(curwin, lpos, रिक्त, &vcol, रिक्त, 0);

    पूर्णांक col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    जर (!col_visible)
	परत;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    जर (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    जर (vim_strchr(p_cpo, CPO_SHOWMATCH) != रिक्त)
	ui_delay(p_mat * 100L + 8, TRUE);
    अन्यथा जर (!char_avail())
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
    स्थिर पूर्णांक
is_zero_width(
    char_u	*pattern,
    माप_प्रकार	patternlen,
    पूर्णांक		move,
    pos_T	*cur,
    पूर्णांक		direction)
{
    regmmatch_T	regmatch;
    पूर्णांक		nmatched = 0;
    पूर्णांक		result = -1;
    pos_T	pos;
    पूर्णांक		called_emsg_before = called_emsg;
    पूर्णांक		flag = 0;

    जर (pattern == रिक्त)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    जर (शोध_regcomp(pattern, patternlen, रिक्त, RE_शोध, RE_शोध,
					      शोध_KEEP, &regmatch) == FAIL)
	परत -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    जर (move)
    {
	CLEAR_POS(&pos);
    }
    अन्यथा
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = शोध_START;
    }

    जर (searchit(curwin, curbuf, &pos, रिक्त, direction, pattern, patternlen, 1,
				  शोध_KEEP + flag, RE_शोध, रिक्त) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	करा
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, रिक्त);
	    जर (nmatched != 0)
		थांबा;
	} पर्यंत (regmatch.regprog != रिक्त
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	जर (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    परत result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    पूर्णांक
current_शोध(
    दीर्घ	मोजणी,
    पूर्णांक		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    पूर्णांक		i;
    पूर्णांक		dir;
    पूर्णांक		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    पूर्णांक		flags = 0;
    pos_T	save_VIsual = VIsual;
    पूर्णांक		zero_width;
    पूर्णांक		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    जर (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    जर (VIsual_active)
    {
	जर (forward)
	    incl(&pos);
	अन्यथा
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    जर (zero_width == -1)
	परत FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    साठी (i = 0; i < 2; i++)
    {
	जर (forward)
	{
	    जर (i == 0 && skip_first_backward)
		सुरूठेवा;
	    dir = i;
	}
	अन्यथा
	    dir = !i;

	flags = 0;
	जर (!dir && !zero_width)
	    flags = शोध_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	जर (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (दीर्घ) (i ? मोजणी : 1),
		शोध_KEEP | flags, RE_शोध, रिक्त);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	जर (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    जर (VIsual_active)
		VIsual = save_VIsual;
	    परत FAIL;
	}
	अन्यथा जर (i == 0 && !result)
	{
	    जर (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    अन्यथा
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_ओळ_मोजणी;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_ओळ_मोजणी);
	    }
	}
    }

    start_pos = pos;

    जर (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    जर (LT_POS(VIsual, end_pos) && forward)
    {
	जर (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	अन्यथा
	    // put the cursor on last character of match
	    dec_cursor();
    }
    अन्यथा जर (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    जर (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	जर (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	अन्यथा जर (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    जर (fdo_flags & FDO_शोध && किल्ली_Typed)
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

    परत OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    पूर्णांक
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    परत (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    स्थिर शून्य
cmdline_शोध_stat(
    पूर्णांक		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    पूर्णांक		show_शीर्ष_bot_संदेश,
    char_u	*msgbuf,
    माप_प्रकार	msgbuflen,
    पूर्णांक		recompute,
    पूर्णांक		maxcount,
    दीर्घ	timeout)
{
    searchstat_T stat;

    update_शोध_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    जर (stat.cur <= 0)
	परत;

    अक्षर	t[शोध_STAT_BUF_LEN];
    माप_प्रकार	len;

#ifdef FEAT_RIGHTLEFT
    जर (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	जर (stat.incomplete == 1)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[?/??]");
	अन्यथा जर (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	अन्यथा जर (stat.cnt > maxcount)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	अन्यथा
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    अन्यथा
#endif
    {
	जर (stat.incomplete == 1)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[?/??]");
	अन्यथा जर (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	अन्यथा जर (stat.cnt > maxcount)
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	अन्यथा
	    len = vim_snprintf(t, शोध_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    जर (show_शीर्ष_bot_संदेश && len + 2 < शोध_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    जर (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    जर (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    संदेश_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    संदेश_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    स्थिर शून्य
update_शोध_stat(
    पूर्णांक			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    पूर्णांक			recompute,
    पूर्णांक			maxcount,
    दीर्घ		timeout UNUSED)
{
    पूर्णांक		    save_ws = p_ws;
    पूर्णांक		    wraparound = FALSE;
    pos_T	    p = (*pos);
    स्थिर pos_T    lastpos = {0, 0, 0};
    स्थिर पूर्णांक	    cur = 0;
    स्थिर पूर्णांक	    cnt = 0;
    स्थिर पूर्णांक	    exact_match = FALSE;
    स्थिर पूर्णांक	    incomplete = 0;
    स्थिर पूर्णांक	    last_maxcount = 0;
    स्थिर पूर्णांक	    chgtick = 0;
    स्थिर char_u   *lastpat = रिक्त;
    स्थिर माप_प्रकार   lastpatlen = 0;
    स्थिर buf_T    *lbuf = रिक्त;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    जर (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	परत;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    जर (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != रिक्त
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
    जर (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    अन्यथा
    {
	पूर्णांक	done_शोध = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	जर (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	पर्यंत (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, रिक्त, 0, 1, शोध_KEEP, RE_LAST, रिक्त) != FAIL)
	{
	    done_शोध = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    जर (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		थांबा;
	    }
#endif
	    cnt++;
	    जर (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		जर (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    जर (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		थांबा;
	    }
	}
	जर (got_int)
	    cur = -1; // abort
	जर (done_शोध)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    जर (lastpat == रिक्त)
		lastpatlen = 0;
	    अन्यथा
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

#जर defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    स्थिर char_u *
get_ओळ_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *ओळ = ml_get(lnum);

    vim_strncpy(buf, ओळ, LSIZE - 1);
    परत buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    शून्य
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    पूर्णांक		dir UNUSED,	// direction of expansion
    पूर्णांक		len,		// length of search pattern
    पूर्णांक		whole,		// match whole words only
    पूर्णांक		skip_comments,	// don't match inside comments
    पूर्णांक		type,		// Type of search; are we looking for a type?
				// a macro?
    दीर्घ	मोजणी,
    पूर्णांक		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    पूर्णांक		forceit,	// If true, always switch to the found path
    पूर्णांक		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    पूर्णांक		max_path_depth = 50;
    दीर्घ	match_मोजणी = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = रिक्त;
    linenr_T	lnum;
    पूर्णांक		depth;
    पूर्णांक		depth_displayed;	// For type==CHECK_PATH
    पूर्णांक		old_files;
    पूर्णांक		already_searched;
    char_u	*file_ओळ;
    char_u	*ओळ;
    char_u	*p;
    char_u	save_char;
    पूर्णांक		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    पूर्णांक		matched = FALSE;
    पूर्णांक		did_show = FALSE;
    पूर्णांक		found = FALSE;
    पूर्णांक		i;
    char_u	*already = रिक्त;
    char_u	*startp = रिक्त;
    char_u	*inc_opt = रिक्त;
# जर defined(FEAT_QUICKFIX)
    win_T	*curwin_save = रिक्त;
# endif

    regmatch.regprog = रिक्त;
    incl_regmatch.regprog = रिक्त;
    def_regmatch.regprog = रिक्त;

    file_ओळ = alloc(LSIZE);
    जर (file_ओळ == रिक्त)
	परत;

    जर (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	जर (pat == रिक्त)
	    जा fpip_end;
	vim_snprintf((अक्षर *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	जर (regmatch.regprog == रिक्त)
	    जा fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    जर (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	जर (incl_regmatch.regprog == रिक्त)
	    जा fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    जर (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	जर (def_regmatch.regprog == रिक्त)
	    जा fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * आकार(SearchedFile), TRUE);
    जर (files == रिक्त)
	जा fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    जर (end_lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	end_lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    जर (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    ओळ = get_ओळ_and_copy(lnum, file_ओळ);

    साठी (;;)
    {
	जर (incl_regmatch.regprog != रिक्त
		&& vim_regexec(&incl_regmatch, ओळ, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    जर (म_माला((अक्षर *)inc_opt, "\\zs") != रिक्त)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (पूर्णांक)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    अन्यथा
		// Use text after match with 'include'.
		new_fname = file_name_in_ओळ(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, रिक्त);
	    already_searched = FALSE;
	    जर (new_fname != रिक्त)
	    {
		// Check whether we have already searched in this file
		साठी (i = 0;; i++)
		{
		    जर (i == depth + 1)
			i = old_files;
		    जर (i == max_path_depth)
			थांबा;
		    जर (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			जर (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    संदेश_putchar('\n');	    // cursor below last one
			    जर (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				संदेश_home_replace_hl(new_fname);
				संदेश_puts(_(" (includes previously listed match)"));
				prev_fname = रिक्त;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			थांबा;
		    }
		}
	    }

	    जर (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == रिक्त && !already_searched)))
	    {
		जर (did_show)
		    संदेश_putchar('\n');	    // cursor below last one
		अन्यथा
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    संदेश_puts_title(_("--- Included files "));
		    जर (action != ACTION_SHOW_ALL)
			संदेश_puts_title(_("not found "));
		    संदेश_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		पर्यंत (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    साठी (i = 0; i < depth_displayed; i++)
			संदेश_puts("  ");
		    संदेश_home_replace(files[depth_displayed].name);
		    संदेश_puts(" -->\n");
		}
		जर (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    साठी (i = 0; i <= depth_displayed; i++)
			संदेश_puts("  ");
		    जर (new_fname != रिक्त)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			संदेश_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    अन्यथा
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			जर (म_माला((अक्षर *)inc_opt, "\\zs") != रिक्त)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (पूर्णांक)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			अन्यथा
			{
			    // find the file name after the end of the match
			    साठी (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    साठी (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			जर (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (पूर्णांक)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			अन्यथा जर (p > ओळ)
			{
			    जर (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    जर (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			संदेश_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    जर (new_fname == रिक्त && action == ACTION_SHOW_ALL)
		    {
			जर (already_searched)
			    संदेश_puts(_("  (Already listed)"));
			अन्यथा
			    संदेश_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    जर (new_fname != रिक्त)
	    {
		// Push the new file onto the file stack
		जर (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    जर (bigger != रिक्त)
		    {
			साठी (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			साठी (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = रिक्त;
			    bigger[i].name = रिक्त;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			साठी (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		जर ((files[depth + 1].fp = mch_fopen((अक्षर *)new_fname, "r"))
								    == रिक्त)
		    vim_free(new_fname);
		अन्यथा
		{
		    जर (++depth == old_files)
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
		    जर (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			संदेश_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((अक्षर*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(अक्षर *)new_fname);
			संदेश_trunc_attr((अक्षर *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    अन्यथा जर (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (अक्षर *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	अन्यथा
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = ओळ;
शोध_ओळ:
	    define_matched = FALSE;
	    जर (def_regmatch.regprog != रिक्त
			      && vim_regexec(&def_regmatch, ओळ, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		पर्यंत (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    जर (def_regmatch.regprog == रिक्त || define_matched)
	    {
		जर (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    जर (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    अन्यथा
			matched = STRNCMP(startp, ptr, len) == 0;
		    जर (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		अन्यथा जर (regmatch.regprog != रिक्त
			 && vim_regexec(&regmatch, ओळ, (colnr_T)(p - ओळ)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    जर (!define_matched && skip_comments)
		    {
			जर ((*ओळ != '#' ||
				STRNCMP(skipwhite(ओळ + 1), "define", 6) != 0)
				&& get_leader_len(ओळ, रिक्त, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(ओळ);
			जर (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    साठी (p = ओळ; *p && p < startp; ++p)
			    {
				जर (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    जर (p[1] == '/')
					थांबा;
				    ++p;
				}
				अन्यथा जर (!matched && p[0] == '*' && p[1] == '/')
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
	जर (matched)
	{
	    जर (action == ACTION_EXPAND)
	    {
		पूर्णांक	cont_s_ipos = FALSE;
		पूर्णांक	add_r;
		char_u	*aux;

		जर (depth == -1 && lnum == curwin->w_cursor.lnum)
		    थांबा;
		found = TRUE;
		aux = p = startp;
		जर (compl_status_adding() && (पूर्णांक)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    जर (vim_iswordp(p))
			जा exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (पूर्णांक)(p - aux);

		जर (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    जर (depth < 0)
		    {
			जर (lnum >= end_lnum)
			    जा exit_matched;
			ओळ = get_ओळ_and_copy(++lnum, file_ओळ);
		    }
		    अन्यथा जर (vim_fgets(ओळ = file_ओळ,
						      LSIZE, files[depth].fp))
			जा exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(ओळ);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    जर (p > aux)
		    {
			जर (*aux != ')' && IObuff[i-1] != TAB)
			{
			    जर (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    जर (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == रिक्त
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			जर (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (पूर्णांक)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    जर (i == ins_compl_len())
			जा exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? रिक्त
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		जर (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		अन्यथा जर (add_r == FAIL)
		    थांबा;
	    }
	    अन्यथा जर (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		जर (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		जर (curr_fname != prev_fname)
		{
		    जर (did_show)
			संदेश_putchar('\n');	// cursor below last one
		    जर (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			संदेश_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		जर (!got_int)
		    show_pat_in_path(ओळ, type, TRUE, action,
			    (depth == -1) ? रिक्त : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_मोजणी++);

		// Set matched flag for this file and all the ones that
		// include it
		साठी (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    अन्यथा जर (--मोजणी <= 0)
	    {
		found = TRUE;
		जर (depth == -1 && lnum == curwin->w_cursor.lnum
# जर defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_ओळ));
		अन्यथा जर (action == ACTION_SHOW)
		{
		    show_pat_in_path(ओळ, type, did_show, action,
			    (depth == -1) ? रिक्त : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		अन्यथा
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# जर defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    जर (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    जर (action == ACTION_SPLIT)
		    {
			जर (win_split(0, 0) == FAIL)
			    थांबा;
			RESET_BINDING(curwin);
		    }
		    जर (depth == -1)
		    {
			// match in current file
# जर defined(FEAT_QUICKFIX)
			जर (g_do_tagpreview != 0)
			{
			    जर (!win_valid(curwin_save))
				थांबा;
			    जर (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, रिक्त,
						     रिक्त, TRUE, lnum, forceit)))
				थांबा;	// failed to jump to file
			}
			अन्यथा
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    अन्यथा
		    {
			जर (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, रिक्त, TRUE,
						    files[depth].lnum, forceit)))
			    थांबा;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		जर (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - ओळ);
		    curwin->w_set_curswant = true;
		}

# जर defined(FEAT_QUICKFIX)
		जर (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		अन्यथा जर (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		थांबा;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    जर (def_regmatch.regprog == रिक्त
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		जा शोध_ओळ;
	}
	ओळ_breakcheck();
	जर (action == ACTION_EXPAND)
	    ins_compl_check_किल्ल्या(30, FALSE);
	जर (got_int || ins_compl_interrupted())
	    थांबा;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	पर्यंत (depth >= 0 && !already
		&& vim_fgets(ओळ = file_ओळ, LSIZE, files[depth].fp))
	{
	    ख_बंद(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    जर (depth < depth_displayed)
		depth_displayed = depth;
	}
	जर (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (पूर्णांक)STRLEN(ओळ);
	    जर (i > 0 && ओळ[i - 1] == '\n')
		ओळ[--i] = NUL;
	    जर (i > 0 && ओळ[i - 1] == '\r')
		ओळ[--i] = NUL;
	}
	अन्यथा जर (!already)
	{
	    जर (++lnum > end_lnum)
		थांबा;
	    ओळ = get_ओळ_and_copy(lnum, file_ओळ);
	}
	already = रिक्त;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    साठी (i = 0; i <= depth; i++)
    {
	ख_बंद(files[i].fp);
	vim_free(files[i].name);
    }
    साठी (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    जर (type == CHECK_PATH)
    {
	जर (!did_show)
	{
	    जर (action != ACTION_SHOW_ALL)
		संदेश(_("All included files were found"));
	    अन्यथा
		संदेश(_("No included files"));
	}
    }
    अन्यथा जर (!found && action != ACTION_EXPAND && !silent)
    {
	जर (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	अन्यथा जर (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	अन्यथा
	    emsg(_(e_couldnt_find_pattern));
    }
    जर (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	संदेश_end();

fpip_end:
    vim_free(file_ओळ);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    स्थिर शून्य
show_pat_in_path(
    char_u  *ओळ,
    पूर्णांक	    type,
    पूर्णांक	    did_show,
    पूर्णांक	    action,
    खाता    *fp,
    linenr_T *lnum,
    दीर्घ    मोजणी)
{
    char_u  *p;
    माप_प्रकार  linelen;

    जर (did_show)
	संदेश_putchar('\n');	// cursor below last one
    अन्यथा जर (!संदेश_silent)
	gotocmdline(TRUE);	// cursor at status line
    जर (got_int)		// 'q' typed at "--more--" message
	परत;
    linelen = STRLEN(ओळ);
    साठी (;;)
    {
	p = ओळ + linelen - 1;
	जर (fp != रिक्त)
	{
	    // We used fgets(), so get rid of newline at end
	    जर (p >= ओळ && *p == '\n')
		--p;
	    जर (p >= ओळ && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	जर (action == ACTION_SHOW_ALL)
	{
	    प्र_लिखो((अक्षर *)IObuff, "%3ld: ", मोजणी);	// show match nr
	    संदेश_puts((अक्षर *)IObuff);
	    प्र_लिखो((अक्षर *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    संदेश_puts_attr((अक्षर *)IObuff, HL_ATTR(HLF_N));
	    संदेश_puts(" ");
	}
	संदेश_prt_ओळ(ओळ, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	जर (got_int || type != FIND_DEFINE || p < ओळ || *p != '\\')
	    थांबा;

	जर (fp != रिक्त)
	{
	    जर (vim_fgets(ओळ, LSIZE, fp)) // end of file
		थांबा;
	    linelen = STRLEN(ओळ);
	    ++*lnum;
	}
	अन्यथा
	{
	    जर (++*lnum > curbuf->b_ml.ml_ओळ_मोजणी)
		थांबा;
	    ओळ = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	संदेश_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(पूर्णांक idx)
{
    परत &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    पूर्णांक
get_spat_last_idx(शून्य)
{
    परत last_idx;
}
#endif

#जर defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    शून्य
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = रिक्त;
    पूर्णांक			maxcount = p_msc;
    दीर्घ		timeout = शोध_STAT_DEF_TIMEOUT;
    पूर्णांक			recompute = TRUE;
    searchstat_T	stat;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	परत;

    जर (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	पूर्णांक		error = FALSE;

	जर (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    परत;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	जर (di != रिक्त)
	{
	    timeout = (दीर्घ)tv_get_संख्या_chk(&di->di_tv, &error);
	    जर (error)
		परत;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	जर (di != रिक्त)
	{
	    maxcount = (पूर्णांक)tv_get_संख्या_chk(&di->di_tv, &error);
	    जर (error)
		परत;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	जर (di != रिक्त)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    जर (pattern == रिक्त)
		परत;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	जर (di != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		परत;
	    }
	    जर (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		परत;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    जर (li != रिक्त)
	    {
		pos.lnum = tv_get_संख्या_chk(&li->li_tv, &error);
		जर (error)
		    परत;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    जर (li != रिक्त)
	    {
		pos.col = tv_get_संख्या_chk(&li->li_tv, &error) - 1;
		जर (error)
		    परत;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    जर (li != रिक्त)
	    {
		pos.coladd = tv_get_संख्या_chk(&li->li_tv, &error);
		जर (error)
		    परत;
	    }
	}
    }

    save_last_शोध_pattern();
# ifdef FEAT_शोध_EXTRA
    save_incsearch_state();
# endif
    जर (pattern != रिक्त)
    {
	जर (*pattern == NUL)
	    जा the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	जर (spats[last_idx].pat == रिक्त)
	    spats[last_idx].patlen = 0;
    }
    जर (spats[last_idx].pat == रिक्त || *spats[last_idx].pat == NUL)
	जा the_end;	// the previous pattern was never defined

    update_शोध_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_संख्या(rettv->vval.v_dict, "current", stat.cur);
    dict_add_संख्या(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_संख्या(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_संख्या(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_संख्या(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_शोध_pattern();
# ifdef FEAT_शोध_EXTRA
    restore_incsearch_state();
# endif
}
#endif
