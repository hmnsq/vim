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
स्थिर पूर्णाङ्क first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
स्थिर char_u *get_पङ्क्तिः_and_copy(linenr_T lnum, char_u *buf);
स्थिर शून्य show_pat_in_path(char_u *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, FILE *, linenr_T *, long);
#endif

प्रकारनिर्देश संरचना searchstat
{
    पूर्णाङ्क	    cur;	    // current position of found words
    पूर्णाङ्क	    cnt;	    // total count of found words
    पूर्णाङ्क	    exact_match;    // TRUE if matched exactly on specified position
    पूर्णाङ्क	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    पूर्णाङ्क	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_अन्वेषणम्_EXTRA
स्थिर शून्य save_incsearch_state(शून्य);
स्थिर शून्य restore_incsearch_state(शून्य);
#endif
स्थिर पूर्णाङ्क check_prevcol(char_u *linep, पूर्णाङ्क col, पूर्णाङ्क ch, पूर्णाङ्क *prevcol);
स्थिर पूर्णाङ्क find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
स्थिर शून्य find_mps_मानानि(पूर्णाङ्क *initc, पूर्णाङ्क *findc, पूर्णाङ्क *backwards, पूर्णाङ्क switchit);
स्थिर पूर्णाङ्क is_zero_width(char_u *pattern, size_t patternlen, पूर्णाङ्क move, pos_T *cur, पूर्णाङ्क direction);
स्थिर शून्य cmdline_अन्वेषणम्_stat(पूर्णाङ्क dirc, pos_T *pos, pos_T *cursor_pos, पूर्णाङ्क show_शिखरम्_bot_सन्देशः, char_u *msgbuf, size_t msgbuflen, पूर्णाङ्क recompute, पूर्णाङ्क maxcount, long timeout);
स्थिर शून्य update_अन्वेषणम्_stat(पूर्णाङ्क dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, पूर्णाङ्क recompute, पूर्णाङ्क maxcount, long timeout);

#define अन्वेषणम्_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define अन्वेषणम्_STAT_BUF_LEN 16

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

स्थिर पूर्णाङ्क last_idx = 0;	// index in spats[] for RE_LAST

स्थिर char_u lastc[2] = {NUL, NUL};	// last character searched for
स्थिर पूर्णाङ्क lastcdir = FORWARD;		// last direction of character search
स्थिर पूर्णाङ्क last_t_cmd = TRUE;		// last search t_cmd
स्थिर char_u	lastc_bytes[MB_MAXBYTES + 1];
स्थिर पूर्णाङ्क	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
स्थिर spat_T	    saved_spats[ARRAY_LENGTH(spats)];
स्थिर char_u	    *saved_mr_pattern = रिक्त;
स्थिर size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_अन्वेषणम्_EXTRA
स्थिर पूर्णाङ्क	    saved_spats_last_idx = 0;
स्थिर पूर्णाङ्क	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
स्थिर char_u	    *mr_pattern = रिक्त;
स्थिर size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
प्रकारनिर्देश संरचना SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    पूर्णाङ्क		matched;	// Found a match in this file
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
    पूर्णाङ्क
अन्वेषणम्_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    पूर्णाङ्क		pat_save,
    पूर्णाङ्क		pat_use,
    पूर्णाङ्क		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    पूर्णाङ्क		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    सचे (pat == रिक्त || *pat == NUL)
    {
	पूर्णाङ्क i;

	सचे (pat_use == RE_LAST)
	    i = last_idx;
	अञ्ञथा
	    i = pat_use;
	सचे (spats[i].pat == रिक्त)	// pattern was never defined
	{
	    सचे (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    अञ्ञथा
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    पटिनिवत्त FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    अञ्ञथा सचे (options & अन्वेषणम्_HIS)	// put new pattern in history
	add_to_history(HIST_अन्वेषणम्, pat, patlen, TRUE, NUL);

    सचे (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    सचे (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    अञ्ञथा
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    सचे (mr_pattern == रिक्त)
	mr_patternlen = 0;
    अञ्ञथा
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    सचे (!(options & अन्वेषणम्_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	सचे (pat_save == RE_अन्वेषणम् || pat_save == RE_BOTH)
	    save_re_pat(RE_अन्वेषणम्, pat, patlen, magic);
	// substitute or global command
	सचे (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    सचे (regmatch->regprog == रिक्त)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_अन्वेषणम्_pat(शून्य)
{
    पटिनिवत्त mr_pattern;
}

    शून्य
save_re_pat(पूर्णाङ्क idx, char_u *pat, size_t patlen, पूर्णाङ्क magic)
{
    सचे (spats[idx].pat == pat)
	पटिनिवत्त;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    सचे (spats[idx].pat == रिक्त)
	spats[idx].patlen = 0;
    अञ्ञथा
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_अन्वेषणम्_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    सचे (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
स्थिर पूर्णाङ्क save_level = 0;

    शून्य
save_अन्वेषणम्_patterns(शून्य)
{
    पूर्णाङ्क i;

    सचे (save_level++ != 0)
	पटिनिवत्त;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	सचे (spats[i].pat != रिक्त)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    सचे (saved_spats[i].pat == रिक्त)
		saved_spats[i].patlen = 0;
	    अञ्ञथा
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    सचे (mr_pattern == रिक्त)
	saved_mr_pattern = रिक्त;
    अञ्ञथा
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    सचे (saved_mr_pattern == रिक्त)
	saved_mr_patternlen = 0;
    अञ्ञथा
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_अन्वेषणम्_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    शून्य
restore_अन्वेषणम्_patterns(शून्य)
{
    पूर्णाङ्क i;

    सचे (--save_level != 0)
	पटिनिवत्त;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#सचे defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_अन्वेषणम्_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#सचे defined(EXITFREE)
    शून्य
free_अन्वेषणम्_patterns(शून्य)
{
    पूर्णाङ्क i;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_अन्वेषणम्_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
स्थिर spat_T	    saved_last_अन्वेषणम्_spat;
स्थिर पूर्णाङ्क	    did_save_last_अन्वेषणम्_spat = 0;
स्थिर पूर्णाङ्क	    saved_last_idx = 0;
स्थिर पूर्णाङ्क	    saved_no_hlsearch = 0;
स्थिर पूर्णाङ्क	    saved_अन्वेषणम्_match_endcol;
स्थिर पूर्णाङ्क	    saved_अन्वेषणम्_match_पङ्क्तयः;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    शून्य
save_last_अन्वेषणम्_pattern(शून्य)
{
    सचे (++did_save_last_अन्वेषणम्_spat != 1)
	// nested call, nothing to do
	पटिनिवत्त;

    saved_last_अन्वेषणम्_spat = spats[RE_अन्वेषणम्];
    सचे (spats[RE_अन्वेषणम्].pat != रिक्त)
    {
	saved_last_अन्वेषणम्_spat.pat = vim_strnsave(spats[RE_अन्वेषणम्].pat, spats[RE_अन्वेषणम्].patlen);
	सचे (saved_last_अन्वेषणम्_spat.pat == रिक्त)
	    saved_last_अन्वेषणम्_spat.patlen = 0;
	अञ्ञथा
	    saved_last_अन्वेषणम्_spat.patlen = spats[RE_अन्वेषणम्].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    शून्य
restore_last_अन्वेषणम्_pattern(शून्य)
{
    सचे (--did_save_last_अन्वेषणम्_spat > 0)
	// nested call, nothing to do
	पटिनिवत्त;
    सचे (did_save_last_अन्वेषणम्_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	पटिनिवत्त;
    }

    vim_free(spats[RE_अन्वेषणम्].pat);
    spats[RE_अन्वेषणम्] = saved_last_अन्वेषणम्_spat;
    saved_last_अन्वेषणम्_spat.pat = रिक्त;
    saved_last_अन्वेषणम्_spat.patlen = 0;
# सचे defined(FEAT_EVAL)
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
    saved_अन्वेषणम्_match_endcol = अन्वेषणम्_match_endcol;
    saved_अन्वेषणम्_match_पङ्क्तयः  = अन्वेषणम्_match_पङ्क्तयः;
}

    स्थिर शून्य
restore_incsearch_state(शून्य)
{
    अन्वेषणम्_match_endcol = saved_अन्वेषणम्_match_endcol;
    अन्वेषणम्_match_पङ्क्तयः  = saved_अन्वेषणम्_match_पङ्क्तयः;
}

    char_u *
last_अन्वेषणम्_pattern(शून्य)
{
    पटिनिवत्त spats[RE_अन्वेषणम्].pat;
}

    size_t
last_अन्वेषणम्_pattern_len(शून्य)
{
    पटिनिवत्त spats[RE_अन्वेषणम्].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    पूर्णाङ्क
ignorecase(char_u *pat)
{
    पटिनिवत्त ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    पूर्णाङ्क
ignorecase_opt(char_u *pat, पूर्णाङ्क ic_in, पूर्णाङ्क scs)
{
    पूर्णाङ्क		ic = ic_in;

    सचे (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    पटिनिवत्त ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    पूर्णाङ्क
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (शून्य)skip_regexp_ex(pat, NUL, magic_isset(), रिक्त, रिक्त, &magic_val);

    याव (*p != NUL)
    {
	पूर्णाङ्क		l;

	सचे (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    सचे (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		पटिनिवत्त TRUE;
	    p += l;
	}
	अञ्ञथा सचे (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    सचे (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    अञ्ञथा सचे (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    अञ्ञथा सचे (p[1] != NUL)  // skip "\X"
		p += 2;
	    अञ्ञथा
		p += 1;
	}
	अञ्ञथा सचे ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    सचे (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    अञ्ञथा
		p++;
	}
	अञ्ञथा सचे (MB_ISUPPER(*p))
	    पटिनिवत्त TRUE;
	अञ्ञथा
	    ++p;
    }
    पटिनिवत्त FALSE;
}

#सचे defined(FEAT_EVAL)
    char_u *
last_csearch(शून्य)
{
    पटिनिवत्त lastc_bytes;
}

    पूर्णाङ्क
last_csearch_forward(शून्य)
{
    पटिनिवत्त lastcdir == FORWARD;
}

    पूर्णाङ्क
last_csearch_until(शून्य)
{
    पटिनिवत्त last_t_cmd == TRUE;
}

    शून्य
set_last_csearch(पूर्णाङ्क c, char_u *s, पूर्णाङ्क len)
{
    *lastc = c;
    lastc_bytelen = len;
    सचे (len)
	memcpy(lastc_bytes, s, len);
    अञ्ञथा
	CLEAR_FIELD(lastc_bytes);
}
#endif

    शून्य
set_csearch_direction(पूर्णाङ्क cdir)
{
    lastcdir = cdir;
}

    शून्य
set_csearch_until(पूर्णाङ्क t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_अन्वेषणम्_pat(शून्य)
{
    पटिनिवत्त spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    शून्य
reset_अन्वेषणम्_dir(शून्य)
{
    spats[0].off.dir = '/';
#सचे defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#सचे defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    शून्य
set_last_अन्वेषणम्_pat(
    char_u	*s,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		magic,
    पूर्णाङ्क		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    सचे (*s == NUL)
	spats[idx].pat = रिक्त;
    अञ्ञथा
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    सचे (spats[idx].pat == रिक्त)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# सचे defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.पङ्क्तिः = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    सचे (setlast)
	last_idx = idx;
    सचे (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	सचे (spats[idx].pat == रिक्त)
	    saved_spats[idx].pat = रिक्त;
	अञ्ञथा
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	सचे (saved_spats[idx].pat == रिक्त)
	    saved_spats[idx].patlen = 0;
	अञ्ञथा
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_अन्वेषणम्_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_अन्वेषणम्_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    सचे (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_अन्वेषणम्_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    शून्य
last_pat_prog(regmmatch_T *regmatch)
{
    सचे (spats[last_idx].pat == रिक्त)
    {
	regmatch->regprog = रिक्त;
	पटिनिवत्त;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (शून्य)अन्वेषणम्_regcomp((char_u *)"", 0, रिक्त, 0, last_idx, अन्वेषणम्_KEEP, regmatch);
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
    पूर्णाङ्क
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    पूर्णाङ्क		dir,
    char_u	*pat,
    size_t	patlen,
    long	गणनम्,
    पूर्णाङ्क		options,
    पूर्णाङ्क		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    पूर्णाङ्क		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    पूर्णाङ्क		loop;
    pos_T	start_pos;
    पूर्णाङ्क		at_first_पङ्क्तिः;
    पूर्णाङ्क		extra_col;
    पूर्णाङ्क		start_char_len;
    पूर्णाङ्क		match_ok;
    long	nmatched;
    पूर्णाङ्क		submatch = 0;
    पूर्णाङ्क		first_match = TRUE;
    पूर्णाङ्क		called_emsg_before = called_emsg;
#ifdef FEAT_अन्वेषणम्_EXTRA
    पूर्णाङ्क		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    पूर्णाङ्क		unused_timeout_flag = FALSE;
    पूर्णाङ्क		*timed_out = &unused_timeout_flag;  // set when timed out.
    पूर्णाङ्क		अन्वेषणम्_from_match_end;		    // vi-compatible search?

    सचे (अन्वेषणम्_regcomp(pat, patlen, रिक्त, RE_अन्वेषणम्, pat_use,
		   (options & (अन्वेषणम्_HIS + अन्वेषणम्_KEEP)), &regmatch) == FAIL)
    {
	सचे ((options & अन्वेषणम्_सन्देशः) && !rc_did_emsg)
	    semsg(_(e_invalid_अन्वेषणम्_string_str), mr_pattern);
	पटिनिवत्त FAIL;
    }

    अन्वेषणम्_from_match_end = vim_strchr(p_cpo, CPO_अन्वेषणम्) != रिक्त;

    सचे (extra_arg != रिक्त)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	सचे (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    करोहि	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	सचे (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	अञ्ञथा सचे (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_पङ्क्तिः_गणनम्
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    सचे (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    अञ्ञथा
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	अञ्ञथा
	    start_char_len = 1;
	सचे (dir == FORWARD)
	{
	    सचे (options & अन्वेषणम्_START)
		extra_col = 0;
	    अञ्ञथा
		extra_col = start_char_len;
	}
	अञ्ञथा
	{
	    सचे (options & अन्वेषणम्_START)
		extra_col = start_char_len;
	    अञ्ञथा
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_पङ्क्तिः = TRUE;	// default: start in first line
	सचे (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_पङ्क्तिः = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	सचे (dir == BACKWARD && start_pos.col == 0
					     && (options & अन्वेषणम्_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_पङ्क्तिः = FALSE;
	}
	अञ्ञथा
	    lnum = pos->lnum;

	अत्थाय (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    अत्थाय ( ; lnum > 0 && lnum <= buf->b_ml.ml_पङ्क्तिः_गणनम्;
					   lnum += dir, at_first_पङ्क्तिः = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		सचे (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    भङ्ग;
		// Stop after passing the time limit.
		सचे (*timed_out)
		    भङ्ग;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_पङ्क्तिः && (options & अन्वेषणम्_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		सचे (regmatch.regprog == रिक्त)
		    भङ्ग;
		// Abort searching on an error (e.g., out of stack).
		सचे (called_emsg > called_emsg_before || *timed_out)
		    भङ्ग;
		सचे (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    सचे (lnum + matchpos.lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
			ptr = (char_u *)"";
		    अञ्ञथा
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    सचे (dir == FORWARD && at_first_पङ्क्तिः)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			याव (matchpos.lnum == 0
				&& ((options & अन्वेषणम्_END) && first_match
				    ?  (nmatched == 1
					&& (पूर्णाङ्क)endpos.col - 1
					     < (पूर्णाङ्क)start_pos.col + extra_col)
				    : ((पूर्णाङ्क)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (पूर्णाङ्क)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    सचे (अन्वेषणम्_from_match_end)
			    {
				सचे (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    भङ्ग;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				सचे (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    सचे (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अञ्ञथा
					++matchcol;
				}
			    }
			    अञ्ञथा
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				सचे (ptr[matchcol] != NUL)
				{
				    सचे (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    अञ्ञथा
					++matchcol;
				}
			    }
			    सचे (matchcol == 0 && (options & अन्वेषणम्_START))
				भङ्ग;
			    सचे (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				भङ्ग;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    सचे (regmatch.regprog == रिक्त)
				भङ्ग;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			सचे (!match_ok)
			    अनुवर्त;
		    }
		    सचे (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			अत्थाय (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    सचे (loop
				|| ((options & अन्वेषणम्_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (पूर्णाङ्क)regmatch.endpos[0].col - 1
							< (पूर्णाङ्क)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (पूर्णाङ्क)regmatch.startpos[0].col
						      < (पूर्णाङ्क)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    अञ्ञथा
				भङ्ग;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    सचे (अन्वेषणम्_from_match_end)
			    {
				सचे (nmatched > 1)
				    भङ्ग;
				matchcol = endpos.col;
				// for empty match: advance one char
				सचे (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    सचे (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अञ्ञथा
					++matchcol;
				}
			    }
			    अञ्ञथा
			    {
				// Stop when the match is in a next line.
				सचे (matchpos.lnum > 0)
				    भङ्ग;
				matchcol = matchpos.col;
				सचे (ptr[matchcol] != NUL)
				{
				    सचे (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    अञ्ञथा
					++matchcol;
				}
			    }
			    सचे (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				सचे (*timed_out)
				    match_ok = FALSE;
				भङ्ग;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    सचे (regmatch.regprog == रिक्त)
				भङ्ग;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			सचे (!match_ok)
			    अनुवर्त;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    सचे ((options & अन्वेषणम्_END) && !(options & अन्वेषणम्_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			सचे (endpos.col == 0)
			{
			    सचे (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			अञ्ञथा
			{
			    --pos->col;
			    सचे (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_पङ्क्तिः_गणनम्)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			सचे (end_pos != रिक्त)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    अञ्ञथा
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			सचे (end_pos != रिक्त)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    सचे (end_pos != रिक्त)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    अन्वेषणम्_match_पङ्क्तयः = endpos.lnum - matchpos.lnum;
		    अन्वेषणम्_match_endcol = endpos.col;
		    भङ्ग;
		}
		पङ्क्तिः_breakcheck();	// stop if ctrl-C typed
		सचे (got_int)
		    भङ्ग;

#ifdef FEAT_अन्वेषणम्_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		सचे ((options & अन्वेषणम्_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    भङ्ग;
		}
#endif

		सचे (loop && lnum == start_pos.lnum)
		    भङ्ग;	    // if second loop, stop where started
	    }
	    at_first_पङ्क्तिः = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    सचे (regmatch.regprog == रिक्त)
		भङ्ग;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    सचे (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_अन्वेषणम्_EXTRA
			      || break_loop
#endif
			      || found || loop)
		भङ्ग;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    सचे (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;
	    अञ्ञथा
		lnum = 1;
	    सचे (!shortmess(SHM_अन्वेषणम्)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & अन्वेषणम्_सन्देशः))
		give_warning((char_u *)_(dir == BACKWARD
					  ? शिखरम्_bot_सन्देशः : bot_शिखरम्_सन्देशः), TRUE);
	    सचे (extra_arg != रिक्त)
		extra_arg->sa_wrapped = TRUE;
	}
	सचे (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_अन्वेषणम्_EXTRA
		|| break_loop
#endif
		)
	    भङ्ग;
    }
    याव (--गणनम् > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    सचे (extra_arg != रिक्त && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    सचे (!found)		    // did not find it
    {
	सचे (got_int)
	    emsg(_(e_interrupted));
	अञ्ञथा सचे ((options & अन्वेषणम्_सन्देशः) == अन्वेषणम्_सन्देशः)
	{
	    सचे (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    अञ्ञथा सचे (lnum == 0)
		semsg(_(e_अन्वेषणम्_hit_शिखरम्_without_match_for_str), mr_pattern);
	    अञ्ञथा
		semsg(_(e_अन्वेषणम्_hit_bottom_without_match_for_str), mr_pattern);
	}
	पटिनिवत्त FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    सचे (pos->lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	pos->lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	सचे (pos->col > 0)
	    --pos->col;
    }

    पटिनिवत्त submatch + 1;
}

#सचे defined(FEAT_EVAL) || defined(FEAT_PROTO)
    शून्य
set_अन्वेषणम्_direction(पूर्णाङ्क cdir)
{
    spats[0].off.dir = cdir;
}

    स्थिर शून्य
set_vv_searchforward(शून्य)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    स्थिर पूर्णाङ्क
first_submatch(regmmatch_T *rp)
{
    पूर्णाङ्क		submatch;

    अत्थाय (submatch = 1; ; ++submatch)
    {
	सचे (rp->startpos[submatch].lnum >= 0)
	    भङ्ग;
	सचे (submatch == 9)
	{
	    submatch = 0;
	    भङ्ग;
	}
    }
    पटिनिवत्त submatch;
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
    पूर्णाङ्क
parse_अन्वेषणम्_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    पूर्णाङ्क		अन्वेषणम्_delim,
    पूर्णाङ्क		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    पूर्णाङ्क		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    सचे (*pat == रिक्त || **pat == NUL)
	पटिनिवत्त 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = रिक्त;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, अन्वेषणम्_delim, magic_isset(),
						    strcopy, रिक्त, रिक्त);
    सचे (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (पूर्णाङ्क)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    सचे (*p == अन्वेषणम्_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->पङ्क्तिः = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    सचे (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->पङ्क्तिः = TRUE;
    अञ्ञथा सचे ((options & अन्वेषणम्_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	सचे (*p == 'e')		// end
	    offset->end = अन्वेषणम्_END;
	++p;
    }
    सचे (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	सचे (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((वर्ण *)p);
	अञ्ञथा सचे (*p == '-')	    // single '-'
	    offset->off = -1;
	अञ्ञथा			    // single '+'
	    offset->off = 1;
	++p;
	याव (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (पूर्णाङ्क)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    पटिनिवत्त cmdlen;
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
    पूर्णाङ्क
do_अन्वेषणम्(
    oparg_T	    *oap,	// can be NULL
    पूर्णाङ्क		    dirc,	// '/' or '?'
    पूर्णाङ्क		    अन्वेषणम्_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    गणनम्,
    पूर्णाङ्क		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    पूर्णाङ्क		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = रिक्त;
    पूर्णाङ्क		    show_अन्वेषणम्_stats;
    char_u	    *msgbuf = रिक्त;
    size_t	    msgbuflen = 0;
    पूर्णाङ्क		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    सचे (spats[0].off.पङ्क्तिः && vim_strchr(p_cpo, CPO_LINEOFF) != रिक्त)
    {
	spats[0].off.पङ्क्तिः = FALSE;
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
    सचे (dirc == 0)
	dirc = spats[0].off.dir;
    अञ्ञथा
    {
	spats[0].off.dir = dirc;
#सचे defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    सचे (options & अन्वेषणम्_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#अञ्ञथा
	सचे (dirc == '/')
	    dirc = '?';
	अञ्ञथा
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    सचे (dirc == '/')
    {
	सचे (hasFolding(pos.lnum, रिक्त, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    अञ्ञथा
    {
	सचे (hasFolding(pos.lnum, &pos.lnum, रिक्त))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_अन्वेषणम्_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    सचे (no_hlsearch && !(options & अन्वेषणम्_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    अत्थाय (;;)
    {
	पूर्णाङ्क		show_शिखरम्_bot_सन्देशः = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = रिक्त;
					    // use previous pattern
	सचे (pat == रिक्त || *pat == NUL || *pat == अन्वेषणम्_delim)
	{
	    सचे (spats[RE_अन्वेषणम्].pat == रिक्त)	    // no previous pattern
	    {
		सचे (spats[RE_SUBST].pat == रिक्त)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    गच्छ end_do_अन्वेषणम्;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    अञ्ञथा
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	सचे (pat != रिक्त && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_अन्वेषणम्_pattern_offset(&pat, &patlen,
				अन्वेषणम्_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    सचे (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_अन्वेषणम्_stats = FALSE;
	सचे ((options & अन्वेषणम्_ECHO) && messaging()
		&& !सन्देशः_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    सन्देशः_start();

	    // Get the offset, so we know how long it is.
	    सचे (!cmd_silent &&
		    (spats[0].off.पङ्क्तिः || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		सचे (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		अञ्ञथा सचे (!spats[0].off.पङ्क्तिः)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		सचे (spats[0].off.off != 0 || spats[0].off.पङ्क्तिः)
		    off_len += vim_snprintf((वर्ण *)off_buf + off_len,
			   परिमाण(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    सचे (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    अञ्ञथा
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    सचे (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		सचे (सन्देशः_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (पूर्णाङ्क)(Rows - सन्देशः_row) * cmdline_width - 1;
		अञ्ञथा
		    // Use up to 'showcmd' column.
		    msgbufsize = (पूर्णाङ्क)(Rows - सन्देशः_row - 1) * cmdline_width
			+ sc_col - 1;
		सचे (msgbufsize < plen + off_len + अन्वेषणम्_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + अन्वेषणम्_STAT_BUF_LEN + 3;
	    }
	    अञ्ञथा
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    सचे (msgbuf == रिक्त)
	    {
		msgbuflen = 0;
	    }
	    अञ्ञथा
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		सचे (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    सचे (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    अञ्ञथा
			mch_memmove(msgbuf + 1, p, plen);
		    सचे (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = सन्देशः_strtrunc(msgbuf, TRUE);
		    सचे (trunc != रिक्त)
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
		    सचे (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			सचे (r != रिक्त)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    याव (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    सचे ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    अञ्ञथा
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    सन्देशः_outtrans(msgbuf);
		    सन्देशः_clr_eos();
		    सन्देशः_check();

		    gotocmdline(FALSE);
		    out_flush();
		    सन्देशः_nowait = TRUE;	    // don't wait for this message
		}

		सचे (!shortmess(SHM_SEARCHCOUNT))
		    show_अन्वेषणम्_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	सचे (!spats[0].off.पङ्क्तिः && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    सचे (spats[0].off.off > 0)
	    {
		अत्थाय (c = spats[0].off.off; c; --c)
		    सचे (decl(&pos) == -1)
			भङ्ग;
		सचे (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    अञ्ञथा
	    {
		अत्थाय (c = spats[0].off.off; c; ++c)
		    सचे (incl(&pos) == -1)
			भङ्ग;
		सचे (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, रिक्त,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, गणनम्, spats[0].off.end + (options &
		       (अन्वेषणम्_KEEP + अन्वेषणम्_PEEK + अन्वेषणम्_HIS
			+ अन्वेषणम्_सन्देशः + अन्वेषणम्_START
			+ ((pat != रिक्त && *pat == ';') ? 0 : अन्वेषणम्_NOOF))),
		RE_LAST, sia);

	सचे (dircp != रिक्त)
	    *dircp = अन्वेषणम्_delim; // restore second '/' or '?' for normal_cmd()

	सचे (!shortmess(SHM_अन्वेषणम्) && sia && sia->sa_wrapped)
	    show_शिखरम्_bot_सन्देशः = TRUE;

	सचे (c == FAIL)
	{
	    retval = 0;
	    गच्छ end_do_अन्वेषणम्;
	}
	सचे (spats[0].off.end && oap != रिक्त)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	सचे (!(options & अन्वेषणम्_NOOF) || (pat != रिक्त && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    सचे (spats[0].off.पङ्क्तिः)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		सचे (c < 1)
		    pos.lnum = 1;
		अञ्ञथा सचे (c > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		    pos.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
		अञ्ञथा
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    अञ्ञथा सचे (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		सचे (c > 0)
		{
		    याव (c-- > 0)
			सचे (incl(&pos) == -1)
			    भङ्ग;
		}
		// to the left, check for start of file
		अञ्ञथा
		{
		    याव (c++ < 0)
			सचे (decl(&pos) == -1)
			    भङ्ग;
		}
	    }
	    सचे (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	सचे (show_अन्वेषणम्_stats)
	     cmdline_अन्वेषणम्_stat(dirc, &pos, &curwin->w_cursor,
				show_शिखरम्_bot_सन्देशः, msgbuf, msgbuflen,
				(गणनम् != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_अन्वेषणम्)
				     && hasFolding(curwin->w_cursor.lnum,
								   रिक्त, रिक्त))
#endif
				),
				p_msc,
				अन्वेषणम्_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	सचे (!(options & अन्वेषणम्_OPT) || pat == रिक्त || *pat != ';')
	    भङ्ग;

	dirc = *++pat;
	--patlen;
	अन्वेषणम्_delim = dirc;
	सचे (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    गच्छ end_do_अन्वेषणम्;
	}
	++pat;
	--patlen;
    }

    सचे (options & अन्वेषणम्_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_अन्वेषणम्:
    सचे ((options & अन्वेषणम्_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    पटिनिवत्त retval;
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
    पूर्णाङ्क
अन्वेषणम्_for_exact_पङ्क्तिः(
    buf_T	*buf,
    pos_T	*pos,
    पूर्णाङ्क		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    पूर्णाङ्क		compl_len = ins_compl_len();

    सचे (buf->b_ml.ml_पङ्क्तिः_गणनम् == 0)
	पटिनिवत्त FAIL;
    अत्थाय (;;)
    {
	pos->lnum += dir;
	सचे (pos->lnum < 1)
	{
	    सचे (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;
		सचे (!shortmess(SHM_अन्वेषणम्))
		    give_warning((char_u *)_(शिखरम्_bot_सन्देशः), TRUE);
	    }
	    अञ्ञथा
	    {
		pos->lnum = 1;
		भङ्ग;
	    }
	}
	अञ्ञथा सचे (pos->lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    सचे (p_ws)
	    {
		pos->lnum = 1;
		सचे (!shortmess(SHM_अन्वेषणम्))
		    give_warning((char_u *)_(bot_शिखरम्_सन्देशः), TRUE);
	    }
	    अञ्ञथा
	    {
		pos->lnum = 1;
		भङ्ग;
	    }
	}
	सचे (pos->lnum == start)
	    भङ्ग;
	सचे (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	सचे (compl_status_adding() && !compl_status_sol())
	{
	    सचे ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		पटिनिवत्त OK;
	}
	अञ्ञथा सचे (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    सचे ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		पटिनिवत्त OK;
	}
    }
    पटिनिवत्त FAIL;
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
    पूर्णाङ्क
searchc(cmdarg_T *cap, पूर्णाङ्क t_cmd)
{
    पूर्णाङ्क			c = cap->nchar;	// char to search for
    पूर्णाङ्क			dir = cap->arg;	// TRUE for searching forward
    long		गणनम् = cap->count1;	// repeat count
    पूर्णाङ्क			col;
    char_u		*p;
    पूर्णाङ्क			len;
    पूर्णाङ्क			stop = TRUE;

    सचे (c != NUL)	// normal search: remember args for repeat
    {
	सचे (!कुञ्चिका_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    सचे (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		सचे (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    अञ्ञथा		// repeat previous search
    {
	सचे (*lastc == NUL && lastc_bytelen <= 1)
	    पटिनिवत्त FAIL;
	सचे (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	अञ्ञथा
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	सचे (vim_strchr(p_cpo, CPO_SCOLON) == रिक्त && गणनम् == 1 && t_cmd)
	    stop = FALSE;
    }

    सचे (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    अञ्ञथा
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    याव (गणनम्--)
    {
	सचे (has_mbyte)
	{
	    अत्थाय (;;)
	    {
		सचे (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    सचे (col >= len)
			पटिनिवत्त FAIL;
		}
		अञ्ञथा
		{
		    सचे (col == 0)
			पटिनिवत्त FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		सचे (lastc_bytelen <= 1)
		{
		    सचे (p[col] == c && stop)
			भङ्ग;
		}
		अञ्ञथा सचे (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    भङ्ग;
		stop = TRUE;
	    }
	}
	अञ्ञथा
	{
	    अत्थाय (;;)
	    {
		सचे ((col += dir) < 0 || col >= len)
		    पटिनिवत्त FAIL;
		सचे (p[col] == c && stop)
		    भङ्ग;
		stop = TRUE;
	    }
	}
    }

    सचे (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	सचे (has_mbyte)
	{
	    सचे (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    अञ्ञथा
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    पटिनिवत्त OK;
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
findmatch(oparg_T *oap, पूर्णाङ्क initc)
{
    पटिनिवत्त findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    स्थिर पूर्णाङ्क
check_prevcol(
    char_u	*linep,
    पूर्णाङ्क		col,
    पूर्णाङ्क		ch,
    पूर्णाङ्क		*prevcol)
{
    --col;
    सचे (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    सचे (prevcol)
	*prevcol = col;
    पटिनिवत्त (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    स्थिर पूर्णाङ्क
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    पूर्णाङ्क		found = FALSE;

    अत्थाय (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    सचे (delim_copy == रिक्त)
	पटिनिवत्त FALSE;
    अत्थाय (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *पङ्क्तिः = ml_get(lnum);

	अत्थाय (p = पङ्क्तिः + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    सचे (lnum == endpos->lnum && (colnr_T)(p - पङ्क्तिः) >= endpos->col)
		भङ्ग;
	    सचे (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		भङ्ग;
	    }
	}
	सचे (found)
	    भङ्ग;
    }
    vim_free(delim_copy);
    पटिनिवत्त found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    स्थिर शून्य
find_mps_मानानि(
    पूर्णाङ्क	    *initc,
    पूर्णाङ्क	    *findc,
    पूर्णाङ्क	    *backwards,
    पूर्णाङ्क	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    याव (*ptr != NUL)
    {
	सचे (has_mbyte)
	{
	    char_u *prev;

	    सचे (mb_ptr2char(ptr) == *initc)
	    {
		सचे (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		अञ्ञथा
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		पटिनिवत्त;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    सचे (mb_ptr2char(ptr) == *initc)
	    {
		सचे (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		अञ्ञथा
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		पटिनिवत्त;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	अञ्ञथा
	{
	    सचे (*ptr == *initc)
	    {
		सचे (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		अञ्ञथा
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		पटिनिवत्त;
	    }
	    ptr += 2;
	    सचे (*ptr == *initc)
	    {
		सचे (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		अञ्ञथा
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		पटिनिवत्त;
	    }
	    ++ptr;
	}
	सचे (*ptr == ',')
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
    पूर्णाङ्क		initc,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		maxtravel)
{
    स्थिर pos_T pos;			// current search position
    पूर्णाङ्क		findc = 0;		// matching brace
    पूर्णाङ्क		c;
    पूर्णाङ्क		गणनम् = 0;		// cumulative number of braces
    पूर्णाङ्क		backwards = FALSE;	// init for gcc
    पूर्णाङ्क		raw_string = FALSE;	// search for raw string
    पूर्णाङ्क		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    पूर्णाङ्क		do_quotes;		// check for quotes in current line
    पूर्णाङ्क		at_start;		// do_quotes value at start position
    पूर्णाङ्क		hash_dir = 0;		// Direction searched for # things
    पूर्णाङ्क		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    पूर्णाङ्क		start_in_quotes;	// start position is in quotes
    पूर्णाङ्क		traveled = 0;		// how far we've searched so far
    पूर्णाङ्क		ignore_cend = FALSE;    // ignore comment end
    पूर्णाङ्क		cpo_match;		// vi compatible matching
    पूर्णाङ्क		cpo_bsl;		// don't recognize backslashes
    पूर्णाङ्क		match_escaped = 0;	// search for escaped match
    पूर्णाङ्क		dir;			// Direction to search
    पूर्णाङ्क		comment_col = MAXCOL;   // start of / / comment
    पूर्णाङ्क		lispcomm = FALSE;	// inside of Lisp-style comment
    पूर्णाङ्क		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    पूर्णाङ्क		skip_comments = (flags & FM_SKIPCOMM) != 0;
    पूर्णाङ्क		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != रिक्त);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != रिक्त);

    // Direction to search when initc is '/', '*' or '#'
    सचे (flags & FM_BACKWARD)
	dir = BACKWARD;
    अञ्ञथा सचे (flags & FM_FORWARD)
	dir = FORWARD;
    अञ्ञथा
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    सचे (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	सचे (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    अञ्ञथा सचे (initc != '#' && initc != NUL)
    {
	find_mps_मानानि(&initc, &findc, &backwards, TRUE);
	सचे (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	सचे (findc == NUL)
	    पटिनिवत्त रिक्त;
    }
    अञ्ञथा
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	सचे (initc == '#')
	{
	    hash_dir = dir;
	}
	अञ्ञथा
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    सचे (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		सचे (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    सचे (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		अञ्ञथा सचे (linep[pos.col] == '/')
		{
		    सचे (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    अञ्ञथा सचे (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		अञ्ञथा सचे (linep[pos.col] == '*')
		{
		    सचे (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    अञ्ञथा सचे (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    सचे (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		सचे (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		अत्थाय (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    सचे (initc == NUL)
			भङ्ग;

		    find_mps_मानानि(&initc, &findc, &backwards, FALSE);
		    सचे (findc)
			भङ्ग;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		सचे (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    सचे (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    अञ्ञथा
			पटिनिवत्त रिक्त;
		}
		अञ्ञथा सचे (!cpo_bsl)
		{
		    पूर्णाङ्क col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    अत्थाय (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	सचे (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    सचे (oap != रिक्त)
		oap->motion_type = MLINE;   // Linewise for this case only
	    सचे (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		सचे (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		अञ्ञथा सचे (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		अञ्ञथा
		    पटिनिवत्त रिक्त;
	    }
	    pos.col = 0;
	    याव (!got_int)
	    {
		सचे (hash_dir > 0)
		{
		    सचे (pos.lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
			भङ्ग;
		}
		अञ्ञथा सचे (pos.lnum == 1)
		    भङ्ग;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		पङ्क्तिः_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		सचे (*ptr != '#')
		    अनुवर्त;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		सचे (hash_dir > 0)
		{
		    सचे (STRNCMP(ptr, "if", 2) == 0)
			गणनम्++;
		    अञ्ञथा सचे (STRNCMP(ptr, "el", 2) == 0)
		    {
			सचे (गणनम् == 0)
			    पटिनिवत्त &pos;
		    }
		    अञ्ञथा सचे (STRNCMP(ptr, "endif", 5) == 0)
		    {
			सचे (गणनम् == 0)
			    पटिनिवत्त &pos;
			गणनम्--;
		    }
		}
		अञ्ञथा
		{
		    सचे (STRNCMP(ptr, "if", 2) == 0)
		    {
			सचे (गणनम् == 0)
			    पटिनिवत्त &pos;
			गणनम्--;
		    }
		    अञ्ञथा सचे (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			सचे (गणनम् == 0)
			    पटिनिवत्त &pos;
		    }
		    अञ्ञथा सचे (STRNCMP(ptr, "endif", 5) == 0)
			गणनम्++;
		}
	    }
	    पटिनिवत्त रिक्त;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    सचे (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != रिक्त)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    सचे ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    सचे (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    सचे (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    याव (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	सचे (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    सचे (lispcomm && pos.col < (colnr_T)comment_col)
		भङ्ग;
	    सचे (pos.col == 0)		// at start of line, go to prev. one
	    {
		सचे (pos.lnum == 1)	// start of file
		    भङ्ग;
		--pos.lnum;

		सचे (maxtravel > 0 && ++traveled > maxtravel)
		    भङ्ग;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		पङ्क्तिः_breakcheck();

		// Check if this line contains a single-line comment
		सचे (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		सचे (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		अञ्ञथा सचे (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    अञ्ञथा
	    {
		--pos.col;
		सचे (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	अञ्ञथा				// forward search
	{
	    सचे (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		सचे (pos.lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    भङ्ग;
		++pos.lnum;

		सचे (maxtravel && traveled++ > maxtravel)
		    भङ्ग;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		पङ्क्तिः_breakcheck();
		सचे (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    अञ्ञथा
	    {
		सचे (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		अञ्ञथा
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	सचे (skip_comments && !comment_dir && !inquote)
	{
	    सचे (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		सचे (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (पूर्णाङ्क)pos.col < comment_col))
		    in_block_comment = TRUE;
		अञ्ञथा सचे (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    अञ्ञथा
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		सचे (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (पूर्णाङ्क)pos.col < comment_col))
		    in_block_comment = TRUE;
		अञ्ञथा सचे (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	सचे (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    सचे (linep[0] == findc && गणनम् == 0)	// match!
		पटिनिवत्त &pos;
	    भङ्ग;					// out of scope
	}

	सचे (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    सचे (comment_dir == FORWARD)
	    {
		सचे (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    पटिनिवत्त &pos;
		}
	    }
	    अञ्ञथा    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		सचे (pos.col == 0)
		    अनुवर्त;
		अञ्ञथा सचे (raw_string)
		{
		    सचे (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != रिक्त)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			सचे (!find_rawstring_end(linep, &pos,
				  गणनम् > 0 ? &match_pos : &curwin->w_cursor))
			{
			    गणनम्++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		अञ्ञथा सचे (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (पूर्णाङ्क)pos.col < comment_col)
		{
		    गणनम्++;
		    match_pos = pos;
		    match_pos.col--;
		}
		अञ्ञथा सचे (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    सचे (गणनम् > 0)
			pos = match_pos;
		    अञ्ञथा सचे (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (पूर्णाङ्क)pos.col <= comment_col)
			pos.col -= 2;
		    अञ्ञथा सचे (ignore_cend)
			अनुवर्त;
		    अञ्ञथा
			पटिनिवत्त रिक्त;
		    पटिनिवत्त &pos;
		}
	    }
	    अनुवर्त;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	सचे (cpo_match)
	    do_quotes = 0;
	अञ्ञथा सचे (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    अत्थाय (ptr = linep; *ptr; ++ptr)
	    {
		सचे (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		सचे (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		सचे (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    सचे (!do_quotes)
	    {
		inquote = FALSE;
		सचे (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    सचे (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			सचे (inquote)
			    start_in_quotes = TRUE;
		    }
		    अञ्ञथा सचे (backwards)
			inquote = TRUE;
		}
		सचे (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    सचे (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			सचे (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    सचे (inquote)
				start_in_quotes = TRUE;
			}
			अञ्ञथा सचे (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	सचे (start_in_quotes == MAYBE)
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
	विकल्प (c)
	{
	पक्ष NUL:
	    // at end of line without trailing backslash, reset inquote
	    सचे (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    भङ्ग;

	पक्ष '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    सचे (do_quotes)
	    {
		पूर्णाङ्क col;

		अत्थाय (col = pos.col - 1; col >= 0; --col)
		    सचे (linep[col] != '\\')
			भङ्ग;
		सचे ((((पूर्णाङ्क)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    भङ्ग;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	पक्ष '\'':
	    सचे (!cpo_match && initc != '\'' && findc != '\'')
	    {
		सचे (backwards)
		{
		    सचे (pos.col > 1)
		    {
			सचे (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    भङ्ग;
			}
			अञ्ञथा सचे (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    भङ्ग;
			}
		    }
		}
		अञ्ञथा सचे (linep[pos.col + 1])	// forward search
		{
		    सचे (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			भङ्ग;
		    }
		    अञ्ञथा सचे (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			भङ्ग;
		    }
		}
	    }
	    // FALLTHROUGH

	स्वभाव:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    सचे (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != रिक्त
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', रिक्त)
		    && check_prevcol(linep, pos.col - 1, '#', रिक्त))
		भङ्ग;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    सचे (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (पूर्णाङ्क)pos.col >= comment_col)))
		भङ्ग;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    सचे ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		पूर्णाङ्क	col, bslcnt = 0;

		सचे (!cpo_bsl)
		{
		    अत्थाय (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		सचे (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    सचे (c == initc)
			गणनम्++;
		    अञ्ञथा
		    {
			सचे (गणनम् == 0)
			    पटिनिवत्त &pos;
			गणनम्--;
		    }
		}
	    }
	}
    }

    सचे (comment_dir == BACKWARD && गणनम् > 0)
    {
	pos = match_pos;
	पटिनिवत्त &pos;
    }
    पटिनिवत्त (pos_T *)रिक्त;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    शून्य
showmatch(
    पूर्णाङ्क		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    पूर्णाङ्क		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    अत्थाय (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	सचे (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    भङ्ग;
#endif
	p += mb_ptr2len(p) + 1;
	सचे (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    भङ्ग;
	p += mb_ptr2len(p);
	सचे (*p == NUL)
	    पटिनिवत्त;
    }
    सचे (*p == NUL)
	पटिनिवत्त;

    सचे ((lpos = findmatch(रिक्त, NUL)) == रिक्त)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	पटिनिवत्त;
    }

    सचे (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	पटिनिवत्त;

    सचे (!curwin->w_p_wrap)
	getvcol(curwin, lpos, रिक्त, &vcol, रिक्त, 0);

    पूर्णाङ्क col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    सचे (!col_visible)
	पटिनिवत्त;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    सचे (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    सचे (vim_strchr(p_cpo, CPO_SHOWMATCH) != रिक्त)
	ui_delay(p_mat * 100L + 8, TRUE);
    अञ्ञथा सचे (!char_avail())
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
    स्थिर पूर्णाङ्क
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    पूर्णाङ्क		move,
    pos_T	*cur,
    पूर्णाङ्क		direction)
{
    regmmatch_T	regmatch;
    पूर्णाङ्क		nmatched = 0;
    पूर्णाङ्क		result = -1;
    pos_T	pos;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		flag = 0;

    सचे (pattern == रिक्त)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    सचे (अन्वेषणम्_regcomp(pattern, patternlen, रिक्त, RE_अन्वेषणम्, RE_अन्वेषणम्,
					      अन्वेषणम्_KEEP, &regmatch) == FAIL)
	पटिनिवत्त -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    सचे (move)
    {
	CLEAR_POS(&pos);
    }
    अञ्ञथा
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = अन्वेषणम्_START;
    }

    सचे (searchit(curwin, curbuf, &pos, रिक्त, direction, pattern, patternlen, 1,
				  अन्वेषणम्_KEEP + flag, RE_अन्वेषणम्, रिक्त) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	करोहि
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, रिक्त);
	    सचे (nmatched != 0)
		भङ्ग;
	} याव (regmatch.regprog != रिक्त
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	सचे (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    पटिनिवत्त result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    पूर्णाङ्क
current_अन्वेषणम्(
    long	गणनम्,
    पूर्णाङ्क		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    पूर्णाङ्क		i;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    पूर्णाङ्क		flags = 0;
    pos_T	save_VIsual = VIsual;
    पूर्णाङ्क		zero_width;
    पूर्णाङ्क		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    सचे (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    सचे (VIsual_active)
    {
	सचे (forward)
	    incl(&pos);
	अञ्ञथा
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    सचे (zero_width == -1)
	पटिनिवत्त FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    अत्थाय (i = 0; i < 2; i++)
    {
	सचे (forward)
	{
	    सचे (i == 0 && skip_first_backward)
		अनुवर्त;
	    dir = i;
	}
	अञ्ञथा
	    dir = !i;

	flags = 0;
	सचे (!dir && !zero_width)
	    flags = अन्वेषणम्_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	सचे (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? गणनम् : 1),
		अन्वेषणम्_KEEP | flags, RE_अन्वेषणम्, रिक्त);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	सचे (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    सचे (VIsual_active)
		VIsual = save_VIsual;
	    पटिनिवत्त FAIL;
	}
	अञ्ञथा सचे (i == 0 && !result)
	{
	    सचे (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    अञ्ञथा
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्);
	    }
	}
    }

    start_pos = pos;

    सचे (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    सचे (LT_POS(VIsual, end_pos) && forward)
    {
	सचे (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	अञ्ञथा
	    // put the cursor on last character of match
	    dec_cursor();
    }
    अञ्ञथा सचे (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    सचे (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	सचे (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	अञ्ञथा सचे (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    सचे (fdo_flags & FDO_अन्वेषणम् && कुञ्चिका_Typed)
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

    पटिनिवत्त OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    पूर्णाङ्क
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    पटिनिवत्त (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    स्थिर शून्य
cmdline_अन्वेषणम्_stat(
    पूर्णाङ्क		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    पूर्णाङ्क		show_शिखरम्_bot_सन्देशः,
    char_u	*msgbuf,
    size_t	msgbuflen,
    पूर्णाङ्क		recompute,
    पूर्णाङ्क		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_अन्वेषणम्_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    सचे (stat.cur <= 0)
	पटिनिवत्त;

    वर्ण	t[अन्वेषणम्_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    सचे (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	सचे (stat.incomplete == 1)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[?/??]");
	अञ्ञथा सचे (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	अञ्ञथा सचे (stat.cnt > maxcount)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	अञ्ञथा
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    अञ्ञथा
#endif
    {
	सचे (stat.incomplete == 1)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[?/??]");
	अञ्ञथा सचे (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	अञ्ञथा सचे (stat.cnt > maxcount)
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	अञ्ञथा
	    len = vim_snprintf(t, अन्वेषणम्_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    सचे (show_शिखरम्_bot_सन्देशः && len + 2 < अन्वेषणम्_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    सचे (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    सचे (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    सन्देशः_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    सन्देशः_hist_off = FALSE;
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
update_अन्वेषणम्_stat(
    पूर्णाङ्क			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    पूर्णाङ्क			recompute,
    पूर्णाङ्क			maxcount,
    long		timeout UNUSED)
{
    पूर्णाङ्क		    save_ws = p_ws;
    पूर्णाङ्क		    wraparound = FALSE;
    pos_T	    p = (*pos);
    स्थिर pos_T    lastpos = {0, 0, 0};
    स्थिर पूर्णाङ्क	    cur = 0;
    स्थिर पूर्णाङ्क	    cnt = 0;
    स्थिर पूर्णाङ्क	    exact_match = FALSE;
    स्थिर पूर्णाङ्क	    incomplete = 0;
    स्थिर पूर्णाङ्क	    last_maxcount = 0;
    स्थिर पूर्णाङ्क	    chgtick = 0;
    स्थिर char_u   *lastpat = रिक्त;
    स्थिर size_t   lastpatlen = 0;
    स्थिर buf_T    *lbuf = रिक्त;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    सचे (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	पटिनिवत्त;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    सचे (!(chgtick == CHANGEDTICK(curbuf)
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
    सचे (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    अञ्ञथा
    {
	पूर्णाङ्क	done_अन्वेषणम् = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	सचे (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	याव (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, रिक्त, 0, 1, अन्वेषणम्_KEEP, RE_LAST, रिक्त) != FAIL)
	{
	    done_अन्वेषणम् = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    सचे (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		भङ्ग;
	    }
#endif
	    cnt++;
	    सचे (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		सचे (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    सचे (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		भङ्ग;
	    }
	}
	सचे (got_int)
	    cur = -1; // abort
	सचे (done_अन्वेषणम्)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    सचे (lastpat == रिक्त)
		lastpatlen = 0;
	    अञ्ञथा
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

#सचे defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    स्थिर char_u *
get_पङ्क्तिः_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *पङ्क्तिः = ml_get(lnum);

    vim_strncpy(buf, पङ्क्तिः, LSIZE - 1);
    पटिनिवत्त buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    शून्य
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    पूर्णाङ्क		dir UNUSED,	// direction of expansion
    पूर्णाङ्क		len,		// length of search pattern
    पूर्णाङ्क		whole,		// match whole words only
    पूर्णाङ्क		skip_comments,	// don't match inside comments
    पूर्णाङ्क		type,		// Type of search; are we looking for a type?
				// a macro?
    long	गणनम्,
    पूर्णाङ्क		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    पूर्णाङ्क		forceit,	// If true, always switch to the found path
    पूर्णाङ्क		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    पूर्णाङ्क		max_path_depth = 50;
    long	match_गणनम् = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = रिक्त;
    linenr_T	lnum;
    पूर्णाङ्क		depth;
    पूर्णाङ्क		depth_displayed;	// For type==CHECK_PATH
    पूर्णाङ्क		old_files;
    पूर्णाङ्क		already_searched;
    char_u	*file_पङ्क्तिः;
    char_u	*पङ्क्तिः;
    char_u	*p;
    char_u	save_char;
    पूर्णाङ्क		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    पूर्णाङ्क		matched = FALSE;
    पूर्णाङ्क		did_show = FALSE;
    पूर्णाङ्क		found = FALSE;
    पूर्णाङ्क		i;
    char_u	*already = रिक्त;
    char_u	*startp = रिक्त;
    char_u	*inc_opt = रिक्त;
# सचे defined(FEAT_QUICKFIX)
    win_T	*curwin_save = रिक्त;
# endif

    regmatch.regprog = रिक्त;
    incl_regmatch.regprog = रिक्त;
    def_regmatch.regprog = रिक्त;

    file_पङ्क्तिः = alloc(LSIZE);
    सचे (file_पङ्क्तिः == रिक्त)
	पटिनिवत्त;

    सचे (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	सचे (pat == रिक्त)
	    गच्छ fpip_end;
	vim_snprintf((वर्ण *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	सचे (regmatch.regprog == रिक्त)
	    गच्छ fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    सचे (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	सचे (incl_regmatch.regprog == रिक्त)
	    गच्छ fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    सचे (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	सचे (def_regmatch.regprog == रिक्त)
	    गच्छ fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * परिमाण(SearchedFile), TRUE);
    सचे (files == रिक्त)
	गच्छ fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    सचे (end_lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	end_lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    सचे (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    पङ्क्तिः = get_पङ्क्तिः_and_copy(lnum, file_पङ्क्तिः);

    अत्थाय (;;)
    {
	सचे (incl_regmatch.regprog != रिक्त
		&& vim_regexec(&incl_regmatch, पङ्क्तिः, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    सचे (strstr((वर्ण *)inc_opt, "\\zs") != रिक्त)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (पूर्णाङ्क)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    अञ्ञथा
		// Use text after match with 'include'.
		new_fname = file_name_in_पङ्क्तिः(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, रिक्त);
	    already_searched = FALSE;
	    सचे (new_fname != रिक्त)
	    {
		// Check whether we have already searched in this file
		अत्थाय (i = 0;; i++)
		{
		    सचे (i == depth + 1)
			i = old_files;
		    सचे (i == max_path_depth)
			भङ्ग;
		    सचे (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			सचे (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    सन्देशः_putchar('\n');	    // cursor below last one
			    सचे (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				सन्देशः_home_replace_hl(new_fname);
				सन्देशः_puts(_(" (includes previously listed match)"));
				prev_fname = रिक्त;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			भङ्ग;
		    }
		}
	    }

	    सचे (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == रिक्त && !already_searched)))
	    {
		सचे (did_show)
		    सन्देशः_putchar('\n');	    // cursor below last one
		अञ्ञथा
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    सन्देशः_puts_title(_("--- Included files "));
		    सचे (action != ACTION_SHOW_ALL)
			सन्देशः_puts_title(_("not found "));
		    सन्देशः_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		याव (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    अत्थाय (i = 0; i < depth_displayed; i++)
			सन्देशः_puts("  ");
		    सन्देशः_home_replace(files[depth_displayed].name);
		    सन्देशः_puts(" -->\n");
		}
		सचे (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    अत्थाय (i = 0; i <= depth_displayed; i++)
			सन्देशः_puts("  ");
		    सचे (new_fname != रिक्त)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			सन्देशः_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    अञ्ञथा
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			सचे (strstr((वर्ण *)inc_opt, "\\zs") != रिक्त)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (पूर्णाङ्क)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			अञ्ञथा
			{
			    // find the file name after the end of the match
			    अत्थाय (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    अत्थाय (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			सचे (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (पूर्णाङ्क)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			अञ्ञथा सचे (p > पङ्क्तिः)
			{
			    सचे (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    सचे (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			सन्देशः_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    सचे (new_fname == रिक्त && action == ACTION_SHOW_ALL)
		    {
			सचे (already_searched)
			    सन्देशः_puts(_("  (Already listed)"));
			अञ्ञथा
			    सन्देशः_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    सचे (new_fname != रिक्त)
	    {
		// Push the new file onto the file stack
		सचे (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    सचे (bigger != रिक्त)
		    {
			अत्थाय (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			अत्थाय (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = रिक्त;
			    bigger[i].name = रिक्त;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			अत्थाय (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		सचे ((files[depth + 1].fp = mch_fopen((वर्ण *)new_fname, "r"))
								    == रिक्त)
		    vim_free(new_fname);
		अञ्ञथा
		{
		    सचे (++depth == old_files)
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
		    सचे (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			सन्देशः_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((वर्ण*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(वर्ण *)new_fname);
			सन्देशः_trunc_attr((वर्ण *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    अञ्ञथा सचे (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (वर्ण *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	अञ्ञथा
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = पङ्क्तिः;
अन्वेषणम्_पङ्क्तिः:
	    define_matched = FALSE;
	    सचे (def_regmatch.regprog != रिक्त
			      && vim_regexec(&def_regmatch, पङ्क्तिः, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		याव (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    सचे (def_regmatch.regprog == रिक्त || define_matched)
	    {
		सचे (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    सचे (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    अञ्ञथा
			matched = STRNCMP(startp, ptr, len) == 0;
		    सचे (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		अञ्ञथा सचे (regmatch.regprog != रिक्त
			 && vim_regexec(&regmatch, पङ्क्तिः, (colnr_T)(p - पङ्क्तिः)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    सचे (!define_matched && skip_comments)
		    {
			सचे ((*पङ्क्तिः != '#' ||
				STRNCMP(skipwhite(पङ्क्तिः + 1), "define", 6) != 0)
				&& get_leader_len(पङ्क्तिः, रिक्त, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(पङ्क्तिः);
			सचे (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    अत्थाय (p = पङ्क्तिः; *p && p < startp; ++p)
			    {
				सचे (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    सचे (p[1] == '/')
					भङ्ग;
				    ++p;
				}
				अञ्ञथा सचे (!matched && p[0] == '*' && p[1] == '/')
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
	सचे (matched)
	{
	    सचे (action == ACTION_EXPAND)
	    {
		पूर्णाङ्क	cont_s_ipos = FALSE;
		पूर्णाङ्क	add_r;
		char_u	*aux;

		सचे (depth == -1 && lnum == curwin->w_cursor.lnum)
		    भङ्ग;
		found = TRUE;
		aux = p = startp;
		सचे (compl_status_adding() && (पूर्णाङ्क)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    सचे (vim_iswordp(p))
			गच्छ exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (पूर्णाङ्क)(p - aux);

		सचे (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    सचे (depth < 0)
		    {
			सचे (lnum >= end_lnum)
			    गच्छ exit_matched;
			पङ्क्तिः = get_पङ्क्तिः_and_copy(++lnum, file_पङ्क्तिः);
		    }
		    अञ्ञथा सचे (vim_fgets(पङ्क्तिः = file_पङ्क्तिः,
						      LSIZE, files[depth].fp))
			गच्छ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(पङ्क्तिः);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    सचे (p > aux)
		    {
			सचे (*aux != ')' && IObuff[i-1] != TAB)
			{
			    सचे (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    सचे (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == रिक्त
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			सचे (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (पूर्णाङ्क)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    सचे (i == ins_compl_len())
			गच्छ exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? रिक्त
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		सचे (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		अञ्ञथा सचे (add_r == FAIL)
		    भङ्ग;
	    }
	    अञ्ञथा सचे (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		सचे (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		सचे (curr_fname != prev_fname)
		{
		    सचे (did_show)
			सन्देशः_putchar('\n');	// cursor below last one
		    सचे (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			सन्देशः_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		सचे (!got_int)
		    show_pat_in_path(पङ्क्तिः, type, TRUE, action,
			    (depth == -1) ? रिक्त : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_गणनम्++);

		// Set matched flag for this file and all the ones that
		// include it
		अत्थाय (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    अञ्ञथा सचे (--गणनम् <= 0)
	    {
		found = TRUE;
		सचे (depth == -1 && lnum == curwin->w_cursor.lnum
# सचे defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_पङ्क्तिः));
		अञ्ञथा सचे (action == ACTION_SHOW)
		{
		    show_pat_in_path(पङ्क्तिः, type, did_show, action,
			    (depth == -1) ? रिक्त : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		अञ्ञथा
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# सचे defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    सचे (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    सचे (action == ACTION_SPLIT)
		    {
			सचे (win_split(0, 0) == FAIL)
			    भङ्ग;
			RESET_BINDING(curwin);
		    }
		    सचे (depth == -1)
		    {
			// match in current file
# सचे defined(FEAT_QUICKFIX)
			सचे (g_do_tagpreview != 0)
			{
			    सचे (!win_valid(curwin_save))
				भङ्ग;
			    सचे (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, रिक्त,
						     रिक्त, TRUE, lnum, forceit)))
				भङ्ग;	// failed to jump to file
			}
			अञ्ञथा
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    अञ्ञथा
		    {
			सचे (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, रिक्त, TRUE,
						    files[depth].lnum, forceit)))
			    भङ्ग;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		सचे (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - पङ्क्तिः);
		    curwin->w_set_curswant = true;
		}

# सचे defined(FEAT_QUICKFIX)
		सचे (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		अञ्ञथा सचे (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		भङ्ग;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    सचे (def_regmatch.regprog == रिक्त
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		गच्छ अन्वेषणम्_पङ्क्तिः;
	}
	पङ्क्तिः_breakcheck();
	सचे (action == ACTION_EXPAND)
	    ins_compl_check_कुञ्चिकाः(30, FALSE);
	सचे (got_int || ins_compl_interrupted())
	    भङ्ग;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	याव (depth >= 0 && !already
		&& vim_fgets(पङ्क्तिः = file_पङ्क्तिः, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    सचे (depth < depth_displayed)
		depth_displayed = depth;
	}
	सचे (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (पूर्णाङ्क)STRLEN(पङ्क्तिः);
	    सचे (i > 0 && पङ्क्तिः[i - 1] == '\n')
		पङ्क्तिः[--i] = NUL;
	    सचे (i > 0 && पङ्क्तिः[i - 1] == '\r')
		पङ्क्तिः[--i] = NUL;
	}
	अञ्ञथा सचे (!already)
	{
	    सचे (++lnum > end_lnum)
		भङ्ग;
	    पङ्क्तिः = get_पङ्क्तिः_and_copy(lnum, file_पङ्क्तिः);
	}
	already = रिक्त;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    अत्थाय (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    अत्थाय (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    सचे (type == CHECK_PATH)
    {
	सचे (!did_show)
	{
	    सचे (action != ACTION_SHOW_ALL)
		सन्देशः(_("All included files were found"));
	    अञ्ञथा
		सन्देशः(_("No included files"));
	}
    }
    अञ्ञथा सचे (!found && action != ACTION_EXPAND && !silent)
    {
	सचे (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	अञ्ञथा सचे (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	अञ्ञथा
	    emsg(_(e_couldnt_find_pattern));
    }
    सचे (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	सन्देशः_end();

fpip_end:
    vim_free(file_पङ्क्तिः);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    स्थिर शून्य
show_pat_in_path(
    char_u  *पङ्क्तिः,
    पूर्णाङ्क	    type,
    पूर्णाङ्क	    did_show,
    पूर्णाङ्क	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    गणनम्)
{
    char_u  *p;
    size_t  linelen;

    सचे (did_show)
	सन्देशः_putchar('\n');	// cursor below last one
    अञ्ञथा सचे (!सन्देशः_silent)
	gotocmdline(TRUE);	// cursor at status line
    सचे (got_int)		// 'q' typed at "--more--" message
	पटिनिवत्त;
    linelen = STRLEN(पङ्क्तिः);
    अत्थाय (;;)
    {
	p = पङ्क्तिः + linelen - 1;
	सचे (fp != रिक्त)
	{
	    // We used fgets(), so get rid of newline at end
	    सचे (p >= पङ्क्तिः && *p == '\n')
		--p;
	    सचे (p >= पङ्क्तिः && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	सचे (action == ACTION_SHOW_ALL)
	{
	    sprintf((वर्ण *)IObuff, "%3ld: ", गणनम्);	// show match nr
	    सन्देशः_puts((वर्ण *)IObuff);
	    sprintf((वर्ण *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    सन्देशः_puts_attr((वर्ण *)IObuff, HL_ATTR(HLF_N));
	    सन्देशः_puts(" ");
	}
	सन्देशः_prt_पङ्क्तिः(पङ्क्तिः, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	सचे (got_int || type != FIND_DEFINE || p < पङ्क्तिः || *p != '\\')
	    भङ्ग;

	सचे (fp != रिक्त)
	{
	    सचे (vim_fgets(पङ्क्तिः, LSIZE, fp)) // end of file
		भङ्ग;
	    linelen = STRLEN(पङ्क्तिः);
	    ++*lnum;
	}
	अञ्ञथा
	{
	    सचे (++*lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		भङ्ग;
	    पङ्क्तिः = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	सन्देशः_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(पूर्णाङ्क idx)
{
    पटिनिवत्त &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    पूर्णाङ्क
get_spat_last_idx(शून्य)
{
    पटिनिवत्त last_idx;
}
#endif

#सचे defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    शून्य
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = रिक्त;
    पूर्णाङ्क			maxcount = p_msc;
    long		timeout = अन्वेषणम्_STAT_DEF_TIMEOUT;
    पूर्णाङ्क			recompute = TRUE;
    searchstat_T	stat;

    सचे (rettv_dict_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	पूर्णाङ्क		error = FALSE;

	सचे (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    पटिनिवत्त;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	सचे (di != रिक्त)
	{
	    timeout = (long)tv_get_सङ्ख्या_chk(&di->di_tv, &error);
	    सचे (error)
		पटिनिवत्त;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	सचे (di != रिक्त)
	{
	    maxcount = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&di->di_tv, &error);
	    सचे (error)
		पटिनिवत्त;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	सचे (di != रिक्त)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    सचे (pattern == रिक्त)
		पटिनिवत्त;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	सचे (di != रिक्त)
	{
	    सचे (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		पटिनिवत्त;
	    }
	    सचे (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		पटिनिवत्त;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    सचे (li != रिक्त)
	    {
		pos.lnum = tv_get_सङ्ख्या_chk(&li->li_tv, &error);
		सचे (error)
		    पटिनिवत्त;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    सचे (li != रिक्त)
	    {
		pos.col = tv_get_सङ्ख्या_chk(&li->li_tv, &error) - 1;
		सचे (error)
		    पटिनिवत्त;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    सचे (li != रिक्त)
	    {
		pos.coladd = tv_get_सङ्ख्या_chk(&li->li_tv, &error);
		सचे (error)
		    पटिनिवत्त;
	    }
	}
    }

    save_last_अन्वेषणम्_pattern();
# ifdef FEAT_अन्वेषणम्_EXTRA
    save_incsearch_state();
# endif
    सचे (pattern != रिक्त)
    {
	सचे (*pattern == NUL)
	    गच्छ the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	सचे (spats[last_idx].pat == रिक्त)
	    spats[last_idx].patlen = 0;
    }
    सचे (spats[last_idx].pat == रिक्त || *spats[last_idx].pat == NUL)
	गच्छ the_end;	// the previous pattern was never defined

    update_अन्वेषणम्_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_सङ्ख्या(rettv->vval.v_dict, "current", stat.cur);
    dict_add_सङ्ख्या(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_सङ्ख्या(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_सङ्ख्या(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_सङ्ख्या(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_अन्वेषणम्_pattern();
# ifdef FEAT_अन्वेषणम्_EXTRA
    restore_incsearch_state();
# endif
}
#endif
