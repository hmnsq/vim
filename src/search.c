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

#অন্তর্ভাব "vim.h"

#ifdef FEAT_EVAL
স্থির শূন্য set_vv_searchforward(শূন্য);
স্থির পূর্ণসংখ্যা first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
স্থির char_u *get_সারি_and_copy(linenr_T lnum, char_u *buf);
স্থির শূন্য show_pat_in_path(char_u *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, FILE *, linenr_T *, long);
#endif

প্রকারলেখ কাঠামো searchstat
{
    পূর্ণসংখ্যা	    cur;	    // current position of found words
    পূর্ণসংখ্যা	    cnt;	    // total count of found words
    পূর্ণসংখ্যা	    exact_match;    // TRUE if matched exactly on specified position
    পূর্ণসংখ্যা	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    পূর্ণসংখ্যা	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_অনুসন্ধান_EXTRA
স্থির শূন্য save_incsearch_state(শূন্য);
স্থির শূন্য restore_incsearch_state(শূন্য);
#endif
স্থির পূর্ণসংখ্যা check_prevcol(char_u *linep, পূর্ণসংখ্যা col, পূর্ণসংখ্যা ch, পূর্ণসংখ্যা *prevcol);
স্থির পূর্ণসংখ্যা find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
স্থির শূন্য find_mps_মানসমূহ(পূর্ণসংখ্যা *initc, পূর্ণসংখ্যা *findc, পূর্ণসংখ্যা *backwards, পূর্ণসংখ্যা switchit);
স্থির পূর্ণসংখ্যা is_zero_width(char_u *pattern, size_t patternlen, পূর্ণসংখ্যা move, pos_T *cur, পূর্ণসংখ্যা direction);
স্থির শূন্য cmdline_অনুসন্ধান_stat(পূর্ণসংখ্যা dirc, pos_T *pos, pos_T *cursor_pos, পূর্ণসংখ্যা show_শীর্ষ_bot_বার্তা, char_u *msgbuf, size_t msgbuflen, পূর্ণসংখ্যা recompute, পূর্ণসংখ্যা maxcount, long timeout);
স্থির শূন্য update_অনুসন্ধান_stat(পূর্ণসংখ্যা dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, পূর্ণসংখ্যা recompute, পূর্ণসংখ্যা maxcount, long timeout);

#define অনুসন্ধান_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define অনুসন্ধান_STAT_BUF_LEN 16

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
স্থির spat_T spats[2] =
{
    {ফাঁকা, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {ফাঁকা, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

স্থির পূর্ণসংখ্যা last_idx = 0;	// index in spats[] for RE_LAST

স্থির char_u lastc[2] = {NUL, NUL};	// last character searched for
স্থির পূর্ণসংখ্যা lastcdir = FORWARD;		// last direction of character search
স্থির পূর্ণসংখ্যা last_t_cmd = TRUE;		// last search t_cmd
স্থির char_u	lastc_bytes[MB_MAXBYTES + 1];
স্থির পূর্ণসংখ্যা	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
স্থির spat_T	    saved_spats[ARRAY_দৈর্ঘ্য(spats)];
স্থির char_u	    *saved_mr_pattern = ফাঁকা;
স্থির size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_অনুসন্ধান_EXTRA
স্থির পূর্ণসংখ্যা	    saved_spats_last_idx = 0;
স্থির পূর্ণসংখ্যা	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
স্থির char_u	    *mr_pattern = ফাঁকা;
স্থির size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
প্রকারলেখ কাঠামো SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    পূর্ণসংখ্যা		matched;	// Found a match in this file
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
    পূর্ণসংখ্যা
অনুসন্ধান_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    পূর্ণসংখ্যা		pat_save,
    পূর্ণসংখ্যা		pat_use,
    পূর্ণসংখ্যা		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    পূর্ণসংখ্যা		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    যদি (pat == ফাঁকা || *pat == NUL)
    {
	পূর্ণসংখ্যা i;

	যদি (pat_use == RE_LAST)
	    i = last_idx;
	অন্যথা
	    i = pat_use;
	যদি (spats[i].pat == ফাঁকা)	// pattern was never defined
	{
	    যদি (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    অন্যথা
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    ফেরত FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    অন্যথা যদি (options & অনুসন্ধান_HIS)	// put new pattern in history
	add_to_history(HIST_অনুসন্ধান, pat, patlen, TRUE, NUL);

    যদি (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    অন্যথা
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    যদি (mr_pattern == ফাঁকা)
	mr_patternlen = 0;
    অন্যথা
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    যদি (!(options & অনুসন্ধান_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	যদি (pat_save == RE_অনুসন্ধান || pat_save == RE_BOTH)
	    save_re_pat(RE_অনুসন্ধান, pat, patlen, magic);
	// substitute or global command
	যদি (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    যদি (regmatch->regprog == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_অনুসন্ধান_pat(শূন্য)
{
    ফেরত mr_pattern;
}

    শূন্য
save_re_pat(পূর্ণসংখ্যা idx, char_u *pat, size_t patlen, পূর্ণসংখ্যা magic)
{
    যদি (spats[idx].pat == pat)
	ফেরত;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    যদি (spats[idx].pat == ফাঁকা)
	spats[idx].patlen = 0;
    অন্যথা
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_অনুসন্ধান_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    যদি (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
স্থির পূর্ণসংখ্যা save_level = 0;

    শূন্য
save_অনুসন্ধান_patterns(শূন্য)
{
    পূর্ণসংখ্যা i;

    যদি (save_level++ != 0)
	ফেরত;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(spats); ++i)
    {
	saved_spats[i] = spats[i];
	যদি (spats[i].pat != ফাঁকা)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    যদি (saved_spats[i].pat == ফাঁকা)
		saved_spats[i].patlen = 0;
	    অন্যথা
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    যদি (mr_pattern == ফাঁকা)
	saved_mr_pattern = ফাঁকা;
    অন্যথা
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    যদি (saved_mr_pattern == ফাঁকা)
	saved_mr_patternlen = 0;
    অন্যথা
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_অনুসন্ধান_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    শূন্য
restore_অনুসন্ধান_patterns(শূন্য)
{
    পূর্ণসংখ্যা i;

    যদি (--save_level != 0)
	ফেরত;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#যদি defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_অনুসন্ধান_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#যদি defined(EXITFREE)
    শূন্য
free_অনুসন্ধান_patterns(শূন্য)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_অনুসন্ধান_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
স্থির spat_T	    saved_last_অনুসন্ধান_spat;
স্থির পূর্ণসংখ্যা	    did_save_last_অনুসন্ধান_spat = 0;
স্থির পূর্ণসংখ্যা	    saved_last_idx = 0;
স্থির পূর্ণসংখ্যা	    saved_no_hlsearch = 0;
স্থির পূর্ণসংখ্যা	    saved_অনুসন্ধান_match_endcol;
স্থির পূর্ণসংখ্যা	    saved_অনুসন্ধান_match_সারিগুলি;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    শূন্য
save_last_অনুসন্ধান_pattern(শূন্য)
{
    যদি (++did_save_last_অনুসন্ধান_spat != 1)
	// nested call, nothing to do
	ফেরত;

    saved_last_অনুসন্ধান_spat = spats[RE_অনুসন্ধান];
    যদি (spats[RE_অনুসন্ধান].pat != ফাঁকা)
    {
	saved_last_অনুসন্ধান_spat.pat = vim_strnsave(spats[RE_অনুসন্ধান].pat, spats[RE_অনুসন্ধান].patlen);
	যদি (saved_last_অনুসন্ধান_spat.pat == ফাঁকা)
	    saved_last_অনুসন্ধান_spat.patlen = 0;
	অন্যথা
	    saved_last_অনুসন্ধান_spat.patlen = spats[RE_অনুসন্ধান].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    শূন্য
restore_last_অনুসন্ধান_pattern(শূন্য)
{
    যদি (--did_save_last_অনুসন্ধান_spat > 0)
	// nested call, nothing to do
	ফেরত;
    যদি (did_save_last_অনুসন্ধান_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ফেরত;
    }

    vim_free(spats[RE_অনুসন্ধান].pat);
    spats[RE_অনুসন্ধান] = saved_last_অনুসন্ধান_spat;
    saved_last_অনুসন্ধান_spat.pat = ফাঁকা;
    saved_last_অনুসন্ধান_spat.patlen = 0;
# যদি defined(FEAT_EVAL)
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
    স্থির শূন্য
save_incsearch_state(শূন্য)
{
    saved_অনুসন্ধান_match_endcol = অনুসন্ধান_match_endcol;
    saved_অনুসন্ধান_match_সারিগুলি  = অনুসন্ধান_match_সারিগুলি;
}

    স্থির শূন্য
restore_incsearch_state(শূন্য)
{
    অনুসন্ধান_match_endcol = saved_অনুসন্ধান_match_endcol;
    অনুসন্ধান_match_সারিগুলি  = saved_অনুসন্ধান_match_সারিগুলি;
}

    char_u *
last_অনুসন্ধান_pattern(শূন্য)
{
    ফেরত spats[RE_অনুসন্ধান].pat;
}

    size_t
last_অনুসন্ধান_pattern_len(শূন্য)
{
    ফেরত spats[RE_অনুসন্ধান].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    পূর্ণসংখ্যা
ignorecase(char_u *pat)
{
    ফেরত ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    পূর্ণসংখ্যা
ignorecase_opt(char_u *pat, পূর্ণসংখ্যা ic_in, পূর্ণসংখ্যা scs)
{
    পূর্ণসংখ্যা		ic = ic_in;

    যদি (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ফেরত ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    পূর্ণসংখ্যা
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (শূন্য)skip_regexp_ex(pat, NUL, magic_isset(), ফাঁকা, ফাঁকা, &magic_val);

    যখন (*p != NUL)
    {
	পূর্ণসংখ্যা		l;

	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    যদি (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ফেরত TRUE;
	    p += l;
	}
	অন্যথা যদি (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    যদি (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    অন্যথা যদি (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    অন্যথা যদি (p[1] != NUL)  // skip "\X"
		p += 2;
	    অন্যথা
		p += 1;
	}
	অন্যথা যদি ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    যদি (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    অন্যথা
		p++;
	}
	অন্যথা যদি (MB_ISUPPER(*p))
	    ফেরত TRUE;
	অন্যথা
	    ++p;
    }
    ফেরত FALSE;
}

#যদি defined(FEAT_EVAL)
    char_u *
last_csearch(শূন্য)
{
    ফেরত lastc_bytes;
}

    পূর্ণসংখ্যা
last_csearch_forward(শূন্য)
{
    ফেরত lastcdir == FORWARD;
}

    পূর্ণসংখ্যা
last_csearch_until(শূন্য)
{
    ফেরত last_t_cmd == TRUE;
}

    শূন্য
set_last_csearch(পূর্ণসংখ্যা c, char_u *s, পূর্ণসংখ্যা len)
{
    *lastc = c;
    lastc_bytelen = len;
    যদি (len)
	memcpy(lastc_bytes, s, len);
    অন্যথা
	CLEAR_FIELD(lastc_bytes);
}
#endif

    শূন্য
set_csearch_direction(পূর্ণসংখ্যা cdir)
{
    lastcdir = cdir;
}

    শূন্য
set_csearch_until(পূর্ণসংখ্যা t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_অনুসন্ধান_pat(শূন্য)
{
    ফেরত spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    শূন্য
reset_অনুসন্ধান_dir(শূন্য)
{
    spats[0].off.dir = '/';
#যদি defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#যদি defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    শূন্য
set_last_অনুসন্ধান_pat(
    char_u	*s,
    পূর্ণসংখ্যা		idx,
    পূর্ণসংখ্যা		magic,
    পূর্ণসংখ্যা		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    যদি (*s == NUL)
	spats[idx].pat = ফাঁকা;
    অন্যথা
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    যদি (spats[idx].pat == ফাঁকা)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# যদি defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.সারি = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    যদি (setlast)
	last_idx = idx;
    যদি (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	যদি (spats[idx].pat == ফাঁকা)
	    saved_spats[idx].pat = ফাঁকা;
	অন্যথা
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	যদি (saved_spats[idx].pat == ফাঁকা)
	    saved_spats[idx].patlen = 0;
	অন্যথা
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_অনুসন্ধান_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_অনুসন্ধান_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    যদি (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_অনুসন্ধান_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    শূন্য
last_pat_prog(regmmatch_T *regmatch)
{
    যদি (spats[last_idx].pat == ফাঁকা)
    {
	regmatch->regprog = ফাঁকা;
	ফেরত;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (শূন্য)অনুসন্ধান_regcomp((char_u *)"", 0, ফাঁকা, 0, last_idx, অনুসন্ধান_KEEP, regmatch);
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
    পূর্ণসংখ্যা
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    পূর্ণসংখ্যা		dir,
    char_u	*pat,
    size_t	patlen,
    long	গুনতি,
    পূর্ণসংখ্যা		options,
    পূর্ণসংখ্যা		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    পূর্ণসংখ্যা		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    পূর্ণসংখ্যা		loop;
    pos_T	start_pos;
    পূর্ণসংখ্যা		at_first_সারি;
    পূর্ণসংখ্যা		extra_col;
    পূর্ণসংখ্যা		start_char_len;
    পূর্ণসংখ্যা		match_ok;
    long	nmatched;
    পূর্ণসংখ্যা		submatch = 0;
    পূর্ণসংখ্যা		first_match = TRUE;
    পূর্ণসংখ্যা		called_emsg_before = called_emsg;
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণসংখ্যা		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    পূর্ণসংখ্যা		unused_timeout_flag = FALSE;
    পূর্ণসংখ্যা		*timed_out = &unused_timeout_flag;  // set when timed out.
    পূর্ণসংখ্যা		অনুসন্ধান_from_match_end;		    // vi-compatible search?

    যদি (অনুসন্ধান_regcomp(pat, patlen, ফাঁকা, RE_অনুসন্ধান, pat_use,
		   (options & (অনুসন্ধান_HIS + অনুসন্ধান_KEEP)), &regmatch) == FAIL)
    {
	যদি ((options & অনুসন্ধান_বার্তা) && !rc_did_emsg)
	    semsg(_(e_invalid_অনুসন্ধান_string_str), mr_pattern);
	ফেরত FAIL;
    }

    অনুসন্ধান_from_match_end = vim_strchr(p_cpo, CPO_অনুসন্ধান) != ফাঁকা;

    যদি (extra_arg != ফাঁকা)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	যদি (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    করো	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	যদি (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	অন্যথা যদি (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_সারি_গুনতি
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    যদি (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    অন্যথা
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	অন্যথা
	    start_char_len = 1;
	যদি (dir == FORWARD)
	{
	    যদি (options & অনুসন্ধান_START)
		extra_col = 0;
	    অন্যথা
		extra_col = start_char_len;
	}
	অন্যথা
	{
	    যদি (options & অনুসন্ধান_START)
		extra_col = start_char_len;
	    অন্যথা
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_সারি = TRUE;	// default: start in first line
	যদি (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_সারি = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	যদি (dir == BACKWARD && start_pos.col == 0
					     && (options & অনুসন্ধান_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_সারি = FALSE;
	}
	অন্যথা
	    lnum = pos->lnum;

	জন্য (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    জন্য ( ; lnum > 0 && lnum <= buf->b_ml.ml_সারি_গুনতি;
					   lnum += dir, at_first_সারি = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		যদি (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    বিরতি;
		// Stop after passing the time limit.
		যদি (*timed_out)
		    বিরতি;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_সারি && (options & অনুসন্ধান_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		যদি (regmatch.regprog == ফাঁকা)
		    বিরতি;
		// Abort searching on an error (e.g., out of stack).
		যদি (called_emsg > called_emsg_before || *timed_out)
		    বিরতি;
		যদি (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    যদি (lnum + matchpos.lnum > buf->b_ml.ml_সারি_গুনতি)
			ptr = (char_u *)"";
		    অন্যথা
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    যদি (dir == FORWARD && at_first_সারি)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			যখন (matchpos.lnum == 0
				&& ((options & অনুসন্ধান_END) && first_match
				    ?  (nmatched == 1
					&& (পূর্ণসংখ্যা)endpos.col - 1
					     < (পূর্ণসংখ্যা)start_pos.col + extra_col)
				    : ((পূর্ণসংখ্যা)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (পূর্ণসংখ্যা)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    যদি (অনুসন্ধান_from_match_end)
			    {
				যদি (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    বিরতি;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				যদি (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    যদি (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    অন্যথা
					++matchcol;
				}
			    }
			    অন্যথা
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				যদি (ptr[matchcol] != NUL)
				{
				    যদি (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    অন্যথা
					++matchcol;
				}
			    }
			    যদি (matchcol == 0 && (options & অনুসন্ধান_START))
				বিরতি;
			    যদি (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				বিরতি;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    যদি (regmatch.regprog == ফাঁকা)
				বিরতি;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			যদি (!match_ok)
			    চালু;
		    }
		    যদি (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			জন্য (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    যদি (loop
				|| ((options & অনুসন্ধান_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (পূর্ণসংখ্যা)regmatch.endpos[0].col - 1
							< (পূর্ণসংখ্যা)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (পূর্ণসংখ্যা)regmatch.startpos[0].col
						      < (পূর্ণসংখ্যা)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    অন্যথা
				বিরতি;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    যদি (অনুসন্ধান_from_match_end)
			    {
				যদি (nmatched > 1)
				    বিরতি;
				matchcol = endpos.col;
				// for empty match: advance one char
				যদি (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    যদি (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    অন্যথা
					++matchcol;
				}
			    }
			    অন্যথা
			    {
				// Stop when the match is in a next line.
				যদি (matchpos.lnum > 0)
				    বিরতি;
				matchcol = matchpos.col;
				যদি (ptr[matchcol] != NUL)
				{
				    যদি (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    অন্যথা
					++matchcol;
				}
			    }
			    যদি (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				যদি (*timed_out)
				    match_ok = FALSE;
				বিরতি;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    যদি (regmatch.regprog == ফাঁকা)
				বিরতি;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			যদি (!match_ok)
			    চালু;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    যদি ((options & অনুসন্ধান_END) && !(options & অনুসন্ধান_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			যদি (endpos.col == 0)
			{
			    যদি (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			অন্যথা
			{
			    --pos->col;
			    যদি (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_সারি_গুনতি)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_শীর্ষ_off)(ptr, ptr + pos->col);
			    }
			}
			যদি (end_pos != ফাঁকা)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    অন্যথা
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			যদি (end_pos != ফাঁকা)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    যদি (end_pos != ফাঁকা)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    অনুসন্ধান_match_সারিগুলি = endpos.lnum - matchpos.lnum;
		    অনুসন্ধান_match_endcol = endpos.col;
		    বিরতি;
		}
		সারি_breakcheck();	// stop if ctrl-C typed
		যদি (got_int)
		    বিরতি;

#ifdef FEAT_অনুসন্ধান_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		যদি ((options & অনুসন্ধান_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    বিরতি;
		}
#endif

		যদি (loop && lnum == start_pos.lnum)
		    বিরতি;	    // if second loop, stop where started
	    }
	    at_first_সারি = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    যদি (regmatch.regprog == ফাঁকা)
		বিরতি;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    যদি (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_অনুসন্ধান_EXTRA
			      || break_loop
#endif
			      || found || loop)
		বিরতি;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    যদি (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_সারি_গুনতি;
	    অন্যথা
		lnum = 1;
	    যদি (!shortmess(SHM_অনুসন্ধান)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & অনুসন্ধান_বার্তা))
		give_warning((char_u *)_(dir == BACKWARD
					  ? শীর্ষ_bot_বার্তা : bot_শীর্ষ_বার্তা), TRUE);
	    যদি (extra_arg != ফাঁকা)
		extra_arg->sa_wrapped = TRUE;
	}
	যদি (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_অনুসন্ধান_EXTRA
		|| break_loop
#endif
		)
	    বিরতি;
    }
    যখন (--গুনতি > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    যদি (extra_arg != ফাঁকা && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    যদি (!found)		    // did not find it
    {
	যদি (got_int)
	    emsg(_(e_interrupted));
	অন্যথা যদি ((options & অনুসন্ধান_বার্তা) == অনুসন্ধান_বার্তা)
	{
	    যদি (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    অন্যথা যদি (lnum == 0)
		semsg(_(e_অনুসন্ধান_hit_শীর্ষ_without_match_for_str), mr_pattern);
	    অন্যথা
		semsg(_(e_অনুসন্ধান_hit_bottom_without_match_for_str), mr_pattern);
	}
	ফেরত FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    যদি (pos->lnum > buf->b_ml.ml_সারি_গুনতি)
    {
	pos->lnum = buf->b_ml.ml_সারি_গুনতি;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	যদি (pos->col > 0)
	    --pos->col;
    }

    ফেরত submatch + 1;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_PROTO)
    শূন্য
set_অনুসন্ধান_direction(পূর্ণসংখ্যা cdir)
{
    spats[0].off.dir = cdir;
}

    স্থির শূন্য
set_vv_searchforward(শূন্য)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    স্থির পূর্ণসংখ্যা
first_submatch(regmmatch_T *rp)
{
    পূর্ণসংখ্যা		submatch;

    জন্য (submatch = 1; ; ++submatch)
    {
	যদি (rp->startpos[submatch].lnum >= 0)
	    বিরতি;
	যদি (submatch == 9)
	{
	    submatch = 0;
	    বিরতি;
	}
    }
    ফেরত submatch;
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
    পূর্ণসংখ্যা
parse_অনুসন্ধান_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    পূর্ণসংখ্যা		অনুসন্ধান_delim,
    পূর্ণসংখ্যা		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    পূর্ণসংখ্যা		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    যদি (*pat == ফাঁকা || **pat == NUL)
	ফেরত 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = ফাঁকা;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, অনুসন্ধান_delim, magic_isset(),
						    strcopy, ফাঁকা, ফাঁকা);
    যদি (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (পূর্ণসংখ্যা)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    যদি (*p == অনুসন্ধান_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->সারি = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    যদি (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->সারি = TRUE;
    অন্যথা যদি ((options & অনুসন্ধান_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	যদি (*p == 'e')		// end
	    offset->end = অনুসন্ধান_END;
	++p;
    }
    যদি (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	যদি (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((অক্ষর *)p);
	অন্যথা যদি (*p == '-')	    // single '-'
	    offset->off = -1;
	অন্যথা			    // single '+'
	    offset->off = 1;
	++p;
	যখন (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (পূর্ণসংখ্যা)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ফেরত cmdlen;
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
    পূর্ণসংখ্যা
do_অনুসন্ধান(
    oparg_T	    *oap,	// can be NULL
    পূর্ণসংখ্যা		    dirc,	// '/' or '?'
    পূর্ণসংখ্যা		    অনুসন্ধান_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    গুনতি,
    পূর্ণসংখ্যা		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    পূর্ণসংখ্যা		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = ফাঁকা;
    পূর্ণসংখ্যা		    show_অনুসন্ধান_stats;
    char_u	    *msgbuf = ফাঁকা;
    size_t	    msgbuflen = 0;
    পূর্ণসংখ্যা		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    যদি (spats[0].off.সারি && vim_strchr(p_cpo, CPO_LINEOFF) != ফাঁকা)
    {
	spats[0].off.সারি = FALSE;
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
    যদি (dirc == 0)
	dirc = spats[0].off.dir;
    অন্যথা
    {
	spats[0].off.dir = dirc;
#যদি defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    যদি (options & অনুসন্ধান_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#অন্যথা
	যদি (dirc == '/')
	    dirc = '?';
	অন্যথা
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    যদি (dirc == '/')
    {
	যদি (hasFolding(pos.lnum, ফাঁকা, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    অন্যথা
    {
	যদি (hasFolding(pos.lnum, &pos.lnum, ফাঁকা))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_অনুসন্ধান_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    যদি (no_hlsearch && !(options & অনুসন্ধান_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    জন্য (;;)
    {
	পূর্ণসংখ্যা		show_শীর্ষ_bot_বার্তা = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = ফাঁকা;
					    // use previous pattern
	যদি (pat == ফাঁকা || *pat == NUL || *pat == অনুসন্ধান_delim)
	{
	    যদি (spats[RE_অনুসন্ধান].pat == ফাঁকা)	    // no previous pattern
	    {
		যদি (spats[RE_SUBST].pat == ফাঁকা)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    যাও end_do_অনুসন্ধান;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    অন্যথা
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	যদি (pat != ফাঁকা && *pat != NUL)	// look for (new) offset
	{
	    char_u	*পূর্ব_searchstr = searchstr;
	    size_t	পূর্ব_searchstrlen = searchstrlen;

	    searchcmdlen += parse_অনুসন্ধান_pattern_offset(&pat, &patlen,
				অনুসন্ধান_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    যদি (*searchstr == NUL)
	    {
		searchstr = পূর্ব_searchstr;
		searchstrlen = পূর্ব_searchstrlen;
	    }
	}

	show_অনুসন্ধান_stats = FALSE;
	যদি ((options & অনুসন্ধান_ECHO) && messaging()
		&& !বার্তা_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    বার্তা_start();

	    // Get the offset, so we know how long it is.
	    যদি (!cmd_silent &&
		    (spats[0].off.সারি || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		যদি (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		অন্যথা যদি (!spats[0].off.সারি)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		যদি (spats[0].off.off != 0 || spats[0].off.সারি)
		    off_len += vim_snprintf((অক্ষর *)off_buf + off_len,
			   আকার(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    যদি (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    অন্যথা
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    যদি (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		যদি (বার্তা_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (পূর্ণসংখ্যা)(Rows - বার্তা_row) * cmdline_width - 1;
		অন্যথা
		    // Use up to 'showcmd' column.
		    msgbufsize = (পূর্ণসংখ্যা)(Rows - বার্তা_row - 1) * cmdline_width
			+ sc_col - 1;
		যদি (msgbufsize < plen + off_len + অনুসন্ধান_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + অনুসন্ধান_STAT_BUF_LEN + 3;
	    }
	    অন্যথা
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    যদি (msgbuf == ফাঁকা)
	    {
		msgbuflen = 0;
	    }
	    অন্যথা
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		যদি (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    যদি (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    অন্যথা
			mch_memmove(msgbuf + 1, p, plen);
		    যদি (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = বার্তা_strtrunc(msgbuf, TRUE);
		    যদি (trunc != ফাঁকা)
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
		    যদি (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			যদি (r != ফাঁকা)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    যখন (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    যদি ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    অন্যথা
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    বার্তা_outtrans(msgbuf);
		    বার্তা_clr_eos();
		    বার্তা_check();

		    gotocmdline(FALSE);
		    out_flush();
		    বার্তা_nowait = TRUE;	    // don't wait for this message
		}

		যদি (!shortmess(SHM_SEARCHCOUNT))
		    show_অনুসন্ধান_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	যদি (!spats[0].off.সারি && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    যদি (spats[0].off.off > 0)
	    {
		জন্য (c = spats[0].off.off; c; --c)
		    যদি (decl(&pos) == -1)
			বিরতি;
		যদি (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    অন্যথা
	    {
		জন্য (c = spats[0].off.off; c; ++c)
		    যদি (incl(&pos) == -1)
			বিরতি;
		যদি (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_সারি_গুনতি + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, ফাঁকা,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, গুনতি, spats[0].off.end + (options &
		       (অনুসন্ধান_KEEP + অনুসন্ধান_PEEK + অনুসন্ধান_HIS
			+ অনুসন্ধান_বার্তা + অনুসন্ধান_START
			+ ((pat != ফাঁকা && *pat == ';') ? 0 : অনুসন্ধান_NOOF))),
		RE_LAST, sia);

	যদি (dircp != ফাঁকা)
	    *dircp = অনুসন্ধান_delim; // restore second '/' or '?' for normal_cmd()

	যদি (!shortmess(SHM_অনুসন্ধান) && sia && sia->sa_wrapped)
	    show_শীর্ষ_bot_বার্তা = TRUE;

	যদি (c == FAIL)
	{
	    retval = 0;
	    যাও end_do_অনুসন্ধান;
	}
	যদি (spats[0].off.end && oap != ফাঁকা)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	যদি (!(options & অনুসন্ধান_NOOF) || (pat != ফাঁকা && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    যদি (spats[0].off.সারি)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		যদি (c < 1)
		    pos.lnum = 1;
		অন্যথা যদি (c > curbuf->b_ml.ml_সারি_গুনতি)
		    pos.lnum = curbuf->b_ml.ml_সারি_গুনতি;
		অন্যথা
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    অন্যথা যদি (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		যদি (c > 0)
		{
		    যখন (c-- > 0)
			যদি (incl(&pos) == -1)
			    বিরতি;
		}
		// to the left, check for start of file
		অন্যথা
		{
		    যখন (c++ < 0)
			যদি (decl(&pos) == -1)
			    বিরতি;
		}
	    }
	    যদি (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	যদি (show_অনুসন্ধান_stats)
	     cmdline_অনুসন্ধান_stat(dirc, &pos, &curwin->w_cursor,
				show_শীর্ষ_bot_বার্তা, msgbuf, msgbuflen,
				(গুনতি != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_অনুসন্ধান)
				     && hasFolding(curwin->w_cursor.lnum,
								   ফাঁকা, ফাঁকা))
#endif
				),
				p_msc,
				অনুসন্ধান_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	যদি (!(options & অনুসন্ধান_OPT) || pat == ফাঁকা || *pat != ';')
	    বিরতি;

	dirc = *++pat;
	--patlen;
	অনুসন্ধান_delim = dirc;
	যদি (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    যাও end_do_অনুসন্ধান;
	}
	++pat;
	--patlen;
    }

    যদি (options & অনুসন্ধান_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_অনুসন্ধান:
    যদি ((options & অনুসন্ধান_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ফেরত retval;
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
    পূর্ণসংখ্যা
অনুসন্ধান_for_exact_সারি(
    buf_T	*buf,
    pos_T	*pos,
    পূর্ণসংখ্যা		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    পূর্ণসংখ্যা		compl_len = ins_compl_len();

    যদি (buf->b_ml.ml_সারি_গুনতি == 0)
	ফেরত FAIL;
    জন্য (;;)
    {
	pos->lnum += dir;
	যদি (pos->lnum < 1)
	{
	    যদি (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_সারি_গুনতি;
		যদি (!shortmess(SHM_অনুসন্ধান))
		    give_warning((char_u *)_(শীর্ষ_bot_বার্তা), TRUE);
	    }
	    অন্যথা
	    {
		pos->lnum = 1;
		বিরতি;
	    }
	}
	অন্যথা যদি (pos->lnum > buf->b_ml.ml_সারি_গুনতি)
	{
	    যদি (p_ws)
	    {
		pos->lnum = 1;
		যদি (!shortmess(SHM_অনুসন্ধান))
		    give_warning((char_u *)_(bot_শীর্ষ_বার্তা), TRUE);
	    }
	    অন্যথা
	    {
		pos->lnum = 1;
		বিরতি;
	    }
	}
	যদি (pos->lnum == start)
	    বিরতি;
	যদি (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	যদি (compl_status_adding() && !compl_status_sol())
	{
	    যদি ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ফেরত OK;
	}
	অন্যথা যদি (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    যদি ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ফেরত OK;
	}
    }
    ফেরত FAIL;
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
    পূর্ণসংখ্যা
searchc(cmdarg_T *cap, পূর্ণসংখ্যা t_cmd)
{
    পূর্ণসংখ্যা			c = cap->nchar;	// char to search for
    পূর্ণসংখ্যা			dir = cap->arg;	// TRUE for searching forward
    long		গুনতি = cap->count1;	// repeat count
    পূর্ণসংখ্যা			col;
    char_u		*p;
    পূর্ণসংখ্যা			len;
    পূর্ণসংখ্যা			stop = TRUE;

    যদি (c != NUL)	// normal search: remember args for repeat
    {
	যদি (!চাবি_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    যদি (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		যদি (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    অন্যথা		// repeat previous search
    {
	যদি (*lastc == NUL && lastc_bytelen <= 1)
	    ফেরত FAIL;
	যদি (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	অন্যথা
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	যদি (vim_strchr(p_cpo, CPO_SCOLON) == ফাঁকা && গুনতি == 1 && t_cmd)
	    stop = FALSE;
    }

    যদি (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    অন্যথা
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    যখন (গুনতি--)
    {
	যদি (has_mbyte)
	{
	    জন্য (;;)
	    {
		যদি (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    যদি (col >= len)
			ফেরত FAIL;
		}
		অন্যথা
		{
		    যদি (col == 0)
			ফেরত FAIL;
		    col -= (*mb_শীর্ষ_off)(p, p + col - 1) + 1;
		}
		যদি (lastc_bytelen <= 1)
		{
		    যদি (p[col] == c && stop)
			বিরতি;
		}
		অন্যথা যদি (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    বিরতি;
		stop = TRUE;
	    }
	}
	অন্যথা
	{
	    জন্য (;;)
	    {
		যদি ((col += dir) < 0 || col >= len)
		    ফেরত FAIL;
		যদি (p[col] == c && stop)
		    বিরতি;
		stop = TRUE;
	    }
	}
    }

    যদি (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	যদি (has_mbyte)
	{
	    যদি (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    অন্যথা
		// To previous char, which may be multi-byte.
		col -= (*mb_শীর্ষ_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    ফেরত OK;
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
findmatch(oparg_T *oap, পূর্ণসংখ্যা initc)
{
    ফেরত findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    স্থির পূর্ণসংখ্যা
check_prevcol(
    char_u	*linep,
    পূর্ণসংখ্যা		col,
    পূর্ণসংখ্যা		ch,
    পূর্ণসংখ্যা		*prevcol)
{
    --col;
    যদি (col > 0 && has_mbyte)
	col -= (*mb_শীর্ষ_off)(linep, linep + col);
    যদি (prevcol)
	*prevcol = col;
    ফেরত (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    স্থির পূর্ণসংখ্যা
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    পূর্ণসংখ্যা		found = FALSE;

    জন্য (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    যদি (delim_copy == ফাঁকা)
	ফেরত FALSE;
    জন্য (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *সারি = ml_get(lnum);

	জন্য (p = সারি + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    যদি (lnum == endpos->lnum && (colnr_T)(p - সারি) >= endpos->col)
		বিরতি;
	    যদি (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		বিরতি;
	    }
	}
	যদি (found)
	    বিরতি;
    }
    vim_free(delim_copy);
    ফেরত found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    স্থির শূন্য
find_mps_মানসমূহ(
    পূর্ণসংখ্যা	    *initc,
    পূর্ণসংখ্যা	    *findc,
    পূর্ণসংখ্যা	    *backwards,
    পূর্ণসংখ্যা	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    যখন (*ptr != NUL)
    {
	যদি (has_mbyte)
	{
	    char_u *পূর্ব;

	    যদি (mb_ptr2char(ptr) == *initc)
	    {
		যদি (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		অন্যথা
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ফেরত;
	    }
	    পূর্ব = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    যদি (mb_ptr2char(ptr) == *initc)
	    {
		যদি (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(পূর্ব);
		    *backwards = FALSE;
		}
		অন্যথা
		{
		    *findc = mb_ptr2char(পূর্ব);
		    *backwards = TRUE;
		}
		ফেরত;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	অন্যথা
	{
	    যদি (*ptr == *initc)
	    {
		যদি (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		অন্যথা
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ফেরত;
	    }
	    ptr += 2;
	    যদি (*ptr == *initc)
	    {
		যদি (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		অন্যথা
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ফেরত;
	    }
	    ++ptr;
	}
	যদি (*ptr == ',')
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
    পূর্ণসংখ্যা		initc,
    পূর্ণসংখ্যা		flags,
    পূর্ণসংখ্যা		maxtravel)
{
    স্থির pos_T pos;			// current search position
    পূর্ণসংখ্যা		findc = 0;		// matching brace
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		গুনতি = 0;		// cumulative number of braces
    পূর্ণসংখ্যা		backwards = FALSE;	// init for gcc
    পূর্ণসংখ্যা		raw_string = FALSE;	// search for raw string
    পূর্ণসংখ্যা		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    পূর্ণসংখ্যা		do_quotes;		// check for quotes in current line
    পূর্ণসংখ্যা		at_start;		// do_quotes value at start position
    পূর্ণসংখ্যা		hash_dir = 0;		// Direction searched for # things
    পূর্ণসংখ্যা		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    পূর্ণসংখ্যা		start_in_quotes;	// start position is in quotes
    পূর্ণসংখ্যা		traveled = 0;		// how far we've searched so far
    পূর্ণসংখ্যা		ignore_cend = FALSE;    // ignore comment end
    পূর্ণসংখ্যা		cpo_match;		// vi compatible matching
    পূর্ণসংখ্যা		cpo_bsl;		// don't recognize backslashes
    পূর্ণসংখ্যা		match_escaped = 0;	// search for escaped match
    পূর্ণসংখ্যা		dir;			// Direction to search
    পূর্ণসংখ্যা		comment_col = MAXCOL;   // start of / / comment
    পূর্ণসংখ্যা		lispcomm = FALSE;	// inside of Lisp-style comment
    পূর্ণসংখ্যা		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    পূর্ণসংখ্যা		skip_comments = (flags & FM_SKIPCOMM) != 0;
    পূর্ণসংখ্যা		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != ফাঁকা);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != ফাঁকা);

    // Direction to search when initc is '/', '*' or '#'
    যদি (flags & FM_BACKWARD)
	dir = BACKWARD;
    অন্যথা যদি (flags & FM_FORWARD)
	dir = FORWARD;
    অন্যথা
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    যদি (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	যদি (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    অন্যথা যদি (initc != '#' && initc != NUL)
    {
	find_mps_মানসমূহ(&initc, &findc, &backwards, TRUE);
	যদি (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	যদি (findc == NUL)
	    ফেরত ফাঁকা;
    }
    অন্যথা
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	যদি (initc == '#')
	{
	    hash_dir = dir;
	}
	অন্যথা
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    যদি (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		যদি (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    যদি (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		অন্যথা যদি (linep[pos.col] == '/')
		{
		    যদি (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    অন্যথা যদি (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		অন্যথা যদি (linep[pos.col] == '*')
		{
		    যদি (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    অন্যথা যদি (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    যদি (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		যদি (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		জন্য (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    যদি (initc == NUL)
			বিরতি;

		    find_mps_মানসমূহ(&initc, &findc, &backwards, FALSE);
		    যদি (findc)
			বিরতি;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		যদি (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    যদি (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    অন্যথা
			ফেরত ফাঁকা;
		}
		অন্যথা যদি (!cpo_bsl)
		{
		    পূর্ণসংখ্যা col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    জন্য (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	যদি (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    যদি (oap != ফাঁকা)
		oap->motion_type = MLINE;   // Linewise for this case only
	    যদি (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		যদি (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		অন্যথা যদি (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		অন্যথা
		    ফেরত ফাঁকা;
	    }
	    pos.col = 0;
	    যখন (!got_int)
	    {
		যদি (hash_dir > 0)
		{
		    যদি (pos.lnum == curbuf->b_ml.ml_সারি_গুনতি)
			বিরতি;
		}
		অন্যথা যদি (pos.lnum == 1)
		    বিরতি;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		সারি_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		যদি (*ptr != '#')
		    চালু;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		যদি (hash_dir > 0)
		{
		    যদি (STRNCMP(ptr, "if", 2) == 0)
			গুনতি++;
		    অন্যথা যদি (STRNCMP(ptr, "el", 2) == 0)
		    {
			যদি (গুনতি == 0)
			    ফেরত &pos;
		    }
		    অন্যথা যদি (STRNCMP(ptr, "endif", 5) == 0)
		    {
			যদি (গুনতি == 0)
			    ফেরত &pos;
			গুনতি--;
		    }
		}
		অন্যথা
		{
		    যদি (STRNCMP(ptr, "if", 2) == 0)
		    {
			যদি (গুনতি == 0)
			    ফেরত &pos;
			গুনতি--;
		    }
		    অন্যথা যদি (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			যদি (গুনতি == 0)
			    ফেরত &pos;
		    }
		    অন্যথা যদি (STRNCMP(ptr, "endif", 5) == 0)
			গুনতি++;
		}
	    }
	    ফেরত ফাঁকা;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    যদি (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != ফাঁকা)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    যদি ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    যদি (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    যদি (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    যখন (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	যদি (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    যদি (lispcomm && pos.col < (colnr_T)comment_col)
		বিরতি;
	    যদি (pos.col == 0)		// at start of line, go to prev. one
	    {
		যদি (pos.lnum == 1)	// start of file
		    বিরতি;
		--pos.lnum;

		যদি (maxtravel > 0 && ++traveled > maxtravel)
		    বিরতি;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		সারি_breakcheck();

		// Check if this line contains a single-line comment
		যদি (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		যদি (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		অন্যথা যদি (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    অন্যথা
	    {
		--pos.col;
		যদি (has_mbyte)
		    pos.col -= (*mb_শীর্ষ_off)(linep, linep + pos.col);
	    }
	}
	অন্যথা				// forward search
	{
	    যদি (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		যদি (pos.lnum == curbuf->b_ml.ml_সারি_গুনতি  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    বিরতি;
		++pos.lnum;

		যদি (maxtravel && traveled++ > maxtravel)
		    বিরতি;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		সারি_breakcheck();
		যদি (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    অন্যথা
	    {
		যদি (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		অন্যথা
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	যদি (skip_comments && !comment_dir && !inquote)
	{
	    যদি (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		যদি (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (পূর্ণসংখ্যা)pos.col < comment_col))
		    in_block_comment = TRUE;
		অন্যথা যদি (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    অন্যথা
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		যদি (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (পূর্ণসংখ্যা)pos.col < comment_col))
		    in_block_comment = TRUE;
		অন্যথা যদি (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	যদি (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    যদি (linep[0] == findc && গুনতি == 0)	// match!
		ফেরত &pos;
	    বিরতি;					// out of scope
	}

	যদি (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    যদি (comment_dir == FORWARD)
	    {
		যদি (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    ফেরত &pos;
		}
	    }
	    অন্যথা    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		যদি (pos.col == 0)
		    চালু;
		অন্যথা যদি (raw_string)
		{
		    যদি (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != ফাঁকা)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			যদি (!find_rawstring_end(linep, &pos,
				  গুনতি > 0 ? &match_pos : &curwin->w_cursor))
			{
			    গুনতি++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		অন্যথা যদি (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (পূর্ণসংখ্যা)pos.col < comment_col)
		{
		    গুনতি++;
		    match_pos = pos;
		    match_pos.col--;
		}
		অন্যথা যদি (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    যদি (গুনতি > 0)
			pos = match_pos;
		    অন্যথা যদি (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (পূর্ণসংখ্যা)pos.col <= comment_col)
			pos.col -= 2;
		    অন্যথা যদি (ignore_cend)
			চালু;
		    অন্যথা
			ফেরত ফাঁকা;
		    ফেরত &pos;
		}
	    }
	    চালু;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	যদি (cpo_match)
	    do_quotes = 0;
	অন্যথা যদি (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    জন্য (ptr = linep; *ptr; ++ptr)
	    {
		যদি (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		যদি (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		যদি (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    যদি (!do_quotes)
	    {
		inquote = FALSE;
		যদি (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    যদি (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			যদি (inquote)
			    start_in_quotes = TRUE;
		    }
		    অন্যথা যদি (backwards)
			inquote = TRUE;
		}
		যদি (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    যদি (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			যদি (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    যদি (inquote)
				start_in_quotes = TRUE;
			}
			অন্যথা যদি (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	যদি (start_in_quotes == MAYBE)
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
	নির্বাচন (c)
	{
	ক্ষেত্র NUL:
	    // at end of line without trailing backslash, reset inquote
	    যদি (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    বিরতি;

	ক্ষেত্র '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    যদি (do_quotes)
	    {
		পূর্ণসংখ্যা col;

		জন্য (col = pos.col - 1; col >= 0; --col)
		    যদি (linep[col] != '\\')
			বিরতি;
		যদি ((((পূর্ণসংখ্যা)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    বিরতি;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	ক্ষেত্র '\'':
	    যদি (!cpo_match && initc != '\'' && findc != '\'')
	    {
		যদি (backwards)
		{
		    যদি (pos.col > 1)
		    {
			যদি (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    বিরতি;
			}
			অন্যথা যদি (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    বিরতি;
			}
		    }
		}
		অন্যথা যদি (linep[pos.col + 1])	// forward search
		{
		    যদি (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			বিরতি;
		    }
		    অন্যথা যদি (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			বিরতি;
		    }
		}
	    }
	    // FALLTHROUGH

	পূর্বনির্ধারিত:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    যদি (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != ফাঁকা
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', ফাঁকা)
		    && check_prevcol(linep, pos.col - 1, '#', ফাঁকা))
		বিরতি;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    যদি (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (পূর্ণসংখ্যা)pos.col >= comment_col)))
		বিরতি;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    যদি ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		পূর্ণসংখ্যা	col, bslcnt = 0;

		যদি (!cpo_bsl)
		{
		    জন্য (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		যদি (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    যদি (c == initc)
			গুনতি++;
		    অন্যথা
		    {
			যদি (গুনতি == 0)
			    ফেরত &pos;
			গুনতি--;
		    }
		}
	    }
	}
    }

    যদি (comment_dir == BACKWARD && গুনতি > 0)
    {
	pos = match_pos;
	ফেরত &pos;
    }
    ফেরত (pos_T *)ফাঁকা;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    শূন্য
showmatch(
    পূর্ণসংখ্যা		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    পূর্ণসংখ্যা		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    জন্য (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	যদি (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    বিরতি;
#endif
	p += mb_ptr2len(p) + 1;
	যদি (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    বিরতি;
	p += mb_ptr2len(p);
	যদি (*p == NUL)
	    ফেরত;
    }
    যদি (*p == NUL)
	ফেরত;

    যদি ((lpos = findmatch(ফাঁকা, NUL)) == ফাঁকা)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	ফেরত;
    }

    যদি (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ফেরত;

    যদি (!curwin->w_p_wrap)
	getvcol(curwin, lpos, ফাঁকা, &vcol, ফাঁকা, 0);

    পূর্ণসংখ্যা col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    যদি (!col_visible)
	ফেরত;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    যদি (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    যদি (vim_strchr(p_cpo, CPO_SHOWMATCH) != ফাঁকা)
	ui_delay(p_mat * 100L + 8, TRUE);
    অন্যথা যদি (!char_avail())
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
    স্থির পূর্ণসংখ্যা
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    পূর্ণসংখ্যা		move,
    pos_T	*cur,
    পূর্ণসংখ্যা		direction)
{
    regmmatch_T	regmatch;
    পূর্ণসংখ্যা		nmatched = 0;
    পূর্ণসংখ্যা		ফলাফল = -1;
    pos_T	pos;
    পূর্ণসংখ্যা		called_emsg_before = called_emsg;
    পূর্ণসংখ্যা		flag = 0;

    যদি (pattern == ফাঁকা)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    যদি (অনুসন্ধান_regcomp(pattern, patternlen, ফাঁকা, RE_অনুসন্ধান, RE_অনুসন্ধান,
					      অনুসন্ধান_KEEP, &regmatch) == FAIL)
	ফেরত -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    যদি (move)
    {
	CLEAR_POS(&pos);
    }
    অন্যথা
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = অনুসন্ধান_START;
    }

    যদি (searchit(curwin, curbuf, &pos, ফাঁকা, direction, pattern, patternlen, 1,
				  অনুসন্ধান_KEEP + flag, RE_অনুসন্ধান, ফাঁকা) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	করো
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, ফাঁকা);
	    যদি (nmatched != 0)
		বিরতি;
	} যখন (regmatch.regprog != ফাঁকা
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	যদি (called_emsg == called_emsg_before)
	{
	    ফলাফল = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    ফেরত ফলাফল;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    পূর্ণসংখ্যা
current_অনুসন্ধান(
    long	গুনতি,
    পূর্ণসংখ্যা		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		ফলাফল;		// result of various function calls
    char_u	old_p_ws = p_ws;
    পূর্ণসংখ্যা		flags = 0;
    pos_T	save_VIsual = VIsual;
    পূর্ণসংখ্যা		zero_width;
    পূর্ণসংখ্যা		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    যদি (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    যদি (VIsual_active)
    {
	যদি (forward)
	    incl(&pos);
	অন্যথা
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    যদি (zero_width == -1)
	ফেরত FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    জন্য (i = 0; i < 2; i++)
    {
	যদি (forward)
	{
	    যদি (i == 0 && skip_first_backward)
		চালু;
	    dir = i;
	}
	অন্যথা
	    dir = !i;

	flags = 0;
	যদি (!dir && !zero_width)
	    flags = অনুসন্ধান_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	যদি (i == 0)
	    p_ws = FALSE;

	ফলাফল = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? গুনতি : 1),
		অনুসন্ধান_KEEP | flags, RE_অনুসন্ধান, ফাঁকা);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	যদি (i == 1 && !ফলাফল) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    যদি (VIsual_active)
		VIsual = save_VIsual;
	    ফেরত FAIL;
	}
	অন্যথা যদি (i == 0 && !ফলাফল)
	{
	    যদি (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    অন্যথা
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_বাফার->b_ml.ml_সারি_গুনতি;
		pos.col  = ml_get_len(curwin->w_বাফার->b_ml.ml_সারি_গুনতি);
	    }
	}
    }

    start_pos = pos;

    যদি (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    যদি (LT_POS(VIsual, end_pos) && forward)
    {
	যদি (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	অন্যথা
	    // put the cursor on last character of match
	    dec_cursor();
    }
    অন্যথা যদি (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    যদি (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	যদি (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	অন্যথা যদি (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    যদি (fdo_flags & FDO_অনুসন্ধান && চাবি_Typed)
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

    ফেরত OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    পূর্ণসংখ্যা
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ফেরত (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    স্থির শূন্য
cmdline_অনুসন্ধান_stat(
    পূর্ণসংখ্যা		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    পূর্ণসংখ্যা		show_শীর্ষ_bot_বার্তা,
    char_u	*msgbuf,
    size_t	msgbuflen,
    পূর্ণসংখ্যা		recompute,
    পূর্ণসংখ্যা		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_অনুসন্ধান_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    যদি (stat.cur <= 0)
	ফেরত;

    অক্ষর	t[অনুসন্ধান_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	যদি (stat.incomplete == 1)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[?/??]");
	অন্যথা যদি (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	অন্যথা যদি (stat.cnt > maxcount)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	অন্যথা
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    অন্যথা
#endif
    {
	যদি (stat.incomplete == 1)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[?/??]");
	অন্যথা যদি (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	অন্যথা যদি (stat.cnt > maxcount)
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	অন্যথা
	    len = vim_snprintf(t, অনুসন্ধান_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    যদি (show_শীর্ষ_bot_বার্তা && len + 2 < অনুসন্ধান_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    যদি (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    যদি (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    বার্তা_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    বার্তা_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    স্থির শূন্য
update_অনুসন্ধান_stat(
    পূর্ণসংখ্যা			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    পূর্ণসংখ্যা			recompute,
    পূর্ণসংখ্যা			maxcount,
    long		timeout UNUSED)
{
    পূর্ণসংখ্যা		    save_ws = p_ws;
    পূর্ণসংখ্যা		    wraparound = FALSE;
    pos_T	    p = (*pos);
    স্থির pos_T    lastpos = {0, 0, 0};
    স্থির পূর্ণসংখ্যা	    cur = 0;
    স্থির পূর্ণসংখ্যা	    cnt = 0;
    স্থির পূর্ণসংখ্যা	    exact_match = FALSE;
    স্থির পূর্ণসংখ্যা	    incomplete = 0;
    স্থির পূর্ণসংখ্যা	    last_maxcount = 0;
    স্থির পূর্ণসংখ্যা	    chgtick = 0;
    স্থির char_u   *lastpat = ফাঁকা;
    স্থির size_t   lastpatlen = 0;
    স্থির buf_T    *lbuf = ফাঁকা;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    যদি (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	ফেরত;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    যদি (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != ফাঁকা
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
    যদি (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    অন্যথা
    {
	পূর্ণসংখ্যা	done_অনুসন্ধান = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	যদি (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	যখন (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, ফাঁকা, 0, 1, অনুসন্ধান_KEEP, RE_LAST, ফাঁকা) != FAIL)
	{
	    done_অনুসন্ধান = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    যদি (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		বিরতি;
	    }
#endif
	    cnt++;
	    যদি (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		যদি (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    যদি (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		বিরতি;
	    }
	}
	যদি (got_int)
	    cur = -1; // abort
	যদি (done_অনুসন্ধান)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    যদি (lastpat == ফাঁকা)
		lastpatlen = 0;
	    অন্যথা
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

#যদি defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    স্থির char_u *
get_সারি_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *সারি = ml_get(lnum);

    vim_strncpy(buf, সারি, LSIZE - 1);
    ফেরত buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    শূন্য
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    পূর্ণসংখ্যা		dir UNUSED,	// direction of expansion
    পূর্ণসংখ্যা		len,		// length of search pattern
    পূর্ণসংখ্যা		whole,		// match whole words only
    পূর্ণসংখ্যা		skip_comments,	// don't match inside comments
    পূর্ণসংখ্যা		type,		// Type of search; are we looking for a type?
				// a macro?
    long	গুনতি,
    পূর্ণসংখ্যা		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    পূর্ণসংখ্যা		forceit,	// If true, always switch to the found path
    পূর্ণসংখ্যা		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    পূর্ণসংখ্যা		max_path_depth = 50;
    long	match_গুনতি = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*পূর্ব_fname = ফাঁকা;
    linenr_T	lnum;
    পূর্ণসংখ্যা		depth;
    পূর্ণসংখ্যা		depth_displayed;	// For type==CHECK_PATH
    পূর্ণসংখ্যা		old_files;
    পূর্ণসংখ্যা		already_searched;
    char_u	*file_সারি;
    char_u	*সারি;
    char_u	*p;
    char_u	save_char;
    পূর্ণসংখ্যা		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    পূর্ণসংখ্যা		matched = FALSE;
    পূর্ণসংখ্যা		did_show = FALSE;
    পূর্ণসংখ্যা		found = FALSE;
    পূর্ণসংখ্যা		i;
    char_u	*already = ফাঁকা;
    char_u	*startp = ফাঁকা;
    char_u	*inc_opt = ফাঁকা;
# যদি defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ফাঁকা;
# endif

    regmatch.regprog = ফাঁকা;
    incl_regmatch.regprog = ফাঁকা;
    def_regmatch.regprog = ফাঁকা;

    file_সারি = alloc(LSIZE);
    যদি (file_সারি == ফাঁকা)
	ফেরত;

    যদি (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	যদি (pat == ফাঁকা)
	    যাও fpip_end;
	vim_snprintf((অক্ষর *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	যদি (regmatch.regprog == ফাঁকা)
	    যাও fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    যদি (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	যদি (incl_regmatch.regprog == ফাঁকা)
	    যাও fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    যদি (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	যদি (def_regmatch.regprog == ফাঁকা)
	    যাও fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * আকার(SearchedFile), TRUE);
    যদি (files == ফাঁকা)
	যাও fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    যদি (end_lnum > curbuf->b_ml.ml_সারি_গুনতি)
	end_lnum = curbuf->b_ml.ml_সারি_গুনতি;
    যদি (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    সারি = get_সারি_and_copy(lnum, file_সারি);

    জন্য (;;)
    {
	যদি (incl_regmatch.regprog != ফাঁকা
		&& vim_regexec(&incl_regmatch, সারি, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    যদি (strstr((অক্ষর *)inc_opt, "\\zs") != ফাঁকা)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (পূর্ণসংখ্যা)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    অন্যথা
		// Use text after match with 'include'.
		new_fname = file_name_in_সারি(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, ফাঁকা);
	    already_searched = FALSE;
	    যদি (new_fname != ফাঁকা)
	    {
		// Check whether we have already searched in this file
		জন্য (i = 0;; i++)
		{
		    যদি (i == depth + 1)
			i = old_files;
		    যদি (i == max_path_depth)
			বিরতি;
		    যদি (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			যদি (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    বার্তা_putchar('\n');	    // cursor below last one
			    যদি (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				বার্তা_home_replace_hl(new_fname);
				বার্তা_puts(_(" (includes previously listed match)"));
				পূর্ব_fname = ফাঁকা;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			বিরতি;
		    }
		}
	    }

	    যদি (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == ফাঁকা && !already_searched)))
	    {
		যদি (did_show)
		    বার্তা_putchar('\n');	    // cursor below last one
		অন্যথা
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    বার্তা_puts_title(_("--- Included files "));
		    যদি (action != ACTION_SHOW_ALL)
			বার্তা_puts_title(_("not found "));
		    বার্তা_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		যখন (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    জন্য (i = 0; i < depth_displayed; i++)
			বার্তা_puts("  ");
		    বার্তা_home_replace(files[depth_displayed].name);
		    বার্তা_puts(" -->\n");
		}
		যদি (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    জন্য (i = 0; i <= depth_displayed; i++)
			বার্তা_puts("  ");
		    যদি (new_fname != ফাঁকা)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			বার্তা_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    অন্যথা
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			যদি (strstr((অক্ষর *)inc_opt, "\\zs") != ফাঁকা)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (পূর্ণসংখ্যা)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			অন্যথা
			{
			    // find the file name after the end of the match
			    জন্য (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    জন্য (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			যদি (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (পূর্ণসংখ্যা)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			অন্যথা যদি (p > সারি)
			{
			    যদি (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    যদি (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			বার্তা_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    যদি (new_fname == ফাঁকা && action == ACTION_SHOW_ALL)
		    {
			যদি (already_searched)
			    বার্তা_puts(_("  (Already listed)"));
			অন্যথা
			    বার্তা_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    যদি (new_fname != ফাঁকা)
	    {
		// Push the new file onto the file stack
		যদি (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    যদি (bigger != ফাঁকা)
		    {
			জন্য (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			জন্য (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = ফাঁকা;
			    bigger[i].name = ফাঁকা;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			জন্য (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		যদি ((files[depth + 1].fp = mch_fopen((অক্ষর *)new_fname, "r"))
								    == ফাঁকা)
		    vim_free(new_fname);
		অন্যথা
		{
		    যদি (++depth == old_files)
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
		    যদি (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			বার্তা_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((অক্ষর*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(অক্ষর *)new_fname);
			বার্তা_trunc_attr((অক্ষর *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    অন্যথা যদি (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (অক্ষর *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	অন্যথা
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = সারি;
অনুসন্ধান_সারি:
	    define_matched = FALSE;
	    যদি (def_regmatch.regprog != ফাঁকা
			      && vim_regexec(&def_regmatch, সারি, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		যখন (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    যদি (def_regmatch.regprog == ফাঁকা || define_matched)
	    {
		যদি (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    যদি (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    অন্যথা
			matched = STRNCMP(startp, ptr, len) == 0;
		    যদি (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		অন্যথা যদি (regmatch.regprog != ফাঁকা
			 && vim_regexec(&regmatch, সারি, (colnr_T)(p - সারি)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    যদি (!define_matched && skip_comments)
		    {
			যদি ((*সারি != '#' ||
				STRNCMP(skipwhite(সারি + 1), "define", 6) != 0)
				&& get_leader_len(সারি, ফাঁকা, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(সারি);
			যদি (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    জন্য (p = সারি; *p && p < startp; ++p)
			    {
				যদি (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    যদি (p[1] == '/')
					বিরতি;
				    ++p;
				}
				অন্যথা যদি (!matched && p[0] == '*' && p[1] == '/')
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
	যদি (matched)
	{
	    যদি (action == ACTION_EXPAND)
	    {
		পূর্ণসংখ্যা	cont_s_ipos = FALSE;
		পূর্ণসংখ্যা	add_r;
		char_u	*aux;

		যদি (depth == -1 && lnum == curwin->w_cursor.lnum)
		    বিরতি;
		found = TRUE;
		aux = p = startp;
		যদি (compl_status_adding() && (পূর্ণসংখ্যা)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    যদি (vim_iswordp(p))
			যাও exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (পূর্ণসংখ্যা)(p - aux);

		যদি (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    যদি (depth < 0)
		    {
			যদি (lnum >= end_lnum)
			    যাও exit_matched;
			সারি = get_সারি_and_copy(++lnum, file_সারি);
		    }
		    অন্যথা যদি (vim_fgets(সারি = file_সারি,
						      LSIZE, files[depth].fp))
			যাও exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(সারি);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    যদি (p > aux)
		    {
			যদি (*aux != ')' && IObuff[i-1] != TAB)
			{
			    যদি (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    যদি (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == ফাঁকা
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			যদি (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (পূর্ণসংখ্যা)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    যদি (i == ins_compl_len())
			যাও exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? ফাঁকা
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		যদি (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		অন্যথা যদি (add_r == FAIL)
		    বিরতি;
	    }
	    অন্যথা যদি (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		যদি (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		যদি (curr_fname != পূর্ব_fname)
		{
		    যদি (did_show)
			বার্তা_putchar('\n');	// cursor below last one
		    যদি (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			বার্তা_home_replace_hl(curr_fname);
		    পূর্ব_fname = curr_fname;
		}
		did_show = TRUE;
		যদি (!got_int)
		    show_pat_in_path(সারি, type, TRUE, action,
			    (depth == -1) ? ফাঁকা : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_গুনতি++);

		// Set matched flag for this file and all the ones that
		// include it
		জন্য (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    অন্যথা যদি (--গুনতি <= 0)
	    {
		found = TRUE;
		যদি (depth == -1 && lnum == curwin->w_cursor.lnum
# যদি defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_সারি));
		অন্যথা যদি (action == ACTION_SHOW)
		{
		    show_pat_in_path(সারি, type, did_show, action,
			    (depth == -1) ? ফাঁকা : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		অন্যথা
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# যদি defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    যদি (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    যদি (action == ACTION_SPLIT)
		    {
			যদি (win_split(0, 0) == FAIL)
			    বিরতি;
			RESET_BINDING(curwin);
		    }
		    যদি (depth == -1)
		    {
			// match in current file
# যদি defined(FEAT_QUICKFIX)
			যদি (g_do_tagpreview != 0)
			{
			    যদি (!win_valid(curwin_save))
				বিরতি;
			    যদি (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_বাফার->b_fnum, ফাঁকা,
						     ফাঁকা, TRUE, lnum, forceit)))
				বিরতি;	// failed to jump to file
			}
			অন্যথা
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    অন্যথা
		    {
			যদি (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, ফাঁকা, TRUE,
						    files[depth].lnum, forceit)))
			    বিরতি;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		যদি (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - সারি);
		    curwin->w_set_curswant = true;
		}

# যদি defined(FEAT_QUICKFIX)
		যদি (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		অন্যথা যদি (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		বিরতি;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    যদি (def_regmatch.regprog == ফাঁকা
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		যাও অনুসন্ধান_সারি;
	}
	সারি_breakcheck();
	যদি (action == ACTION_EXPAND)
	    ins_compl_check_চাবিগুলি(30, FALSE);
	যদি (got_int || ins_compl_interrupted())
	    বিরতি;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	যখন (depth >= 0 && !already
		&& vim_fgets(সারি = file_সারি, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    যদি (depth < depth_displayed)
		depth_displayed = depth;
	}
	যদি (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (পূর্ণসংখ্যা)STRLEN(সারি);
	    যদি (i > 0 && সারি[i - 1] == '\n')
		সারি[--i] = NUL;
	    যদি (i > 0 && সারি[i - 1] == '\r')
		সারি[--i] = NUL;
	}
	অন্যথা যদি (!already)
	{
	    যদি (++lnum > end_lnum)
		বিরতি;
	    সারি = get_সারি_and_copy(lnum, file_সারি);
	}
	already = ফাঁকা;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    জন্য (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    জন্য (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    যদি (type == CHECK_PATH)
    {
	যদি (!did_show)
	{
	    যদি (action != ACTION_SHOW_ALL)
		বার্তা(_("All included files were found"));
	    অন্যথা
		বার্তা(_("No included files"));
	}
    }
    অন্যথা যদি (!found && action != ACTION_EXPAND && !silent)
    {
	যদি (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	অন্যথা যদি (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	অন্যথা
	    emsg(_(e_couldnt_find_pattern));
    }
    যদি (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	বার্তা_end();

fpip_end:
    vim_free(file_সারি);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    স্থির শূন্য
show_pat_in_path(
    char_u  *সারি,
    পূর্ণসংখ্যা	    type,
    পূর্ণসংখ্যা	    did_show,
    পূর্ণসংখ্যা	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    গুনতি)
{
    char_u  *p;
    size_t  linelen;

    যদি (did_show)
	বার্তা_putchar('\n');	// cursor below last one
    অন্যথা যদি (!বার্তা_silent)
	gotocmdline(TRUE);	// cursor at status line
    যদি (got_int)		// 'q' typed at "--more--" message
	ফেরত;
    linelen = STRLEN(সারি);
    জন্য (;;)
    {
	p = সারি + linelen - 1;
	যদি (fp != ফাঁকা)
	{
	    // We used fgets(), so get rid of newline at end
	    যদি (p >= সারি && *p == '\n')
		--p;
	    যদি (p >= সারি && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	যদি (action == ACTION_SHOW_ALL)
	{
	    sprintf((অক্ষর *)IObuff, "%3ld: ", গুনতি);	// show match nr
	    বার্তা_puts((অক্ষর *)IObuff);
	    sprintf((অক্ষর *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    বার্তা_puts_attr((অক্ষর *)IObuff, HL_ATTR(HLF_N));
	    বার্তা_puts(" ");
	}
	বার্তা_prt_সারি(সারি, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	যদি (got_int || type != FIND_DEFINE || p < সারি || *p != '\\')
	    বিরতি;

	যদি (fp != ফাঁকা)
	{
	    যদি (vim_fgets(সারি, LSIZE, fp)) // end of file
		বিরতি;
	    linelen = STRLEN(সারি);
	    ++*lnum;
	}
	অন্যথা
	{
	    যদি (++*lnum > curbuf->b_ml.ml_সারি_গুনতি)
		বিরতি;
	    সারি = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	বার্তা_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(পূর্ণসংখ্যা idx)
{
    ফেরত &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    পূর্ণসংখ্যা
get_spat_last_idx(শূন্য)
{
    ফেরত last_idx;
}
#endif

#যদি defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    শূন্য
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = ফাঁকা;
    পূর্ণসংখ্যা			maxcount = p_msc;
    long		timeout = অনুসন্ধান_STAT_DEF_TIMEOUT;
    পূর্ণসংখ্যা			recompute = TRUE;
    searchstat_T	stat;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	পূর্ণসংখ্যা		error = FALSE;

	যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ফেরত;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	যদি (di != ফাঁকা)
	{
	    timeout = (long)tv_get_সংখ্যা_chk(&di->di_tv, &error);
	    যদি (error)
		ফেরত;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	যদি (di != ফাঁকা)
	{
	    maxcount = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&di->di_tv, &error);
	    যদি (error)
		ফেরত;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	যদি (di != ফাঁকা)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    যদি (pattern == ফাঁকা)
		ফেরত;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	যদি (di != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		ফেরত;
	    }
	    যদি (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		ফেরত;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    যদি (li != ফাঁকা)
	    {
		pos.lnum = tv_get_সংখ্যা_chk(&li->li_tv, &error);
		যদি (error)
		    ফেরত;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    যদি (li != ফাঁকা)
	    {
		pos.col = tv_get_সংখ্যা_chk(&li->li_tv, &error) - 1;
		যদি (error)
		    ফেরত;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    যদি (li != ফাঁকা)
	    {
		pos.coladd = tv_get_সংখ্যা_chk(&li->li_tv, &error);
		যদি (error)
		    ফেরত;
	    }
	}
    }

    save_last_অনুসন্ধান_pattern();
# ifdef FEAT_অনুসন্ধান_EXTRA
    save_incsearch_state();
# endif
    যদি (pattern != ফাঁকা)
    {
	যদি (*pattern == NUL)
	    যাও the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	যদি (spats[last_idx].pat == ফাঁকা)
	    spats[last_idx].patlen = 0;
    }
    যদি (spats[last_idx].pat == ফাঁকা || *spats[last_idx].pat == NUL)
	যাও the_end;	// the previous pattern was never defined

    update_অনুসন্ধান_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_সংখ্যা(rettv->vval.v_dict, "current", stat.cur);
    dict_add_সংখ্যা(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_সংখ্যা(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_সংখ্যা(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_সংখ্যা(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_অনুসন্ধান_pattern();
# ifdef FEAT_অনুসন্ধান_EXTRA
    restore_incsearch_state();
# endif
}
#endif
