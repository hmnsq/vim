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

#شامل "vim.h"

#ifdef FEAT_EVAL
ایستا تهی set_vv_searchforward(تهی);
ایستا صحیح first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
ایستا char_u *get_سطر_and_copy(linenr_T lnum, char_u *buf);
ایستا تهی show_pat_in_path(char_u *, صحیح, صحیح, صحیح, FILE *, linenr_T *, long);
#endif

تعریفنوع ساختار searchstat
{
    صحیح	    cur;	    // current position of found words
    صحیح	    cnt;	    // total count of found words
    صحیح	    exact_match;    // TRUE if matched exactly on specified position
    صحیح	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    صحیح	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_جستجو_EXTRA
ایستا تهی save_incsearch_state(تهی);
ایستا تهی restore_incsearch_state(تهی);
#endif
ایستا صحیح check_prevcol(char_u *linep, صحیح col, صحیح ch, صحیح *prevcol);
ایستا صحیح find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
ایستا تهی find_mps_مقادیر(صحیح *initc, صحیح *findc, صحیح *backwards, صحیح switchit);
ایستا صحیح is_zero_width(char_u *pattern, size_t patternlen, صحیح move, pos_T *cur, صحیح direction);
ایستا تهی cmdline_جستجو_stat(صحیح dirc, pos_T *pos, pos_T *cursor_pos, صحیح show_بالاترین_bot_پیام, char_u *msgbuf, size_t msgbuflen, صحیح recompute, صحیح maxcount, long timeout);
ایستا تهی update_جستجو_stat(صحیح dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, صحیح recompute, صحیح maxcount, long timeout);

#define جستجو_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define جستجو_STAT_BUF_LEN 16

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
ایستا spat_T spats[2] =
{
    {پوچ, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {پوچ, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ایستا صحیح last_idx = 0;	// index in spats[] for RE_LAST

ایستا char_u lastc[2] = {NUL, NUL};	// last character searched for
ایستا صحیح lastcdir = FORWARD;		// last direction of character search
ایستا صحیح last_t_cmd = TRUE;		// last search t_cmd
ایستا char_u	lastc_bytes[MB_MAXBYTES + 1];
ایستا صحیح	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ایستا spat_T	    saved_spats[ARRAY_LENGTH(spats)];
ایستا char_u	    *saved_mr_pattern = پوچ;
ایستا size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_جستجو_EXTRA
ایستا صحیح	    saved_spats_last_idx = 0;
ایستا صحیح	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
ایستا char_u	    *mr_pattern = پوچ;
ایستا size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
تعریفنوع ساختار SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    صحیح		matched;	// Found a match in this file
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
    صحیح
جستجو_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    صحیح		pat_save,
    صحیح		pat_use,
    صحیح		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    صحیح		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    اگر (pat == پوچ || *pat == NUL)
    {
	صحیح i;

	اگر (pat_use == RE_LAST)
	    i = last_idx;
	وگرنه
	    i = pat_use;
	اگر (spats[i].pat == پوچ)	// pattern was never defined
	{
	    اگر (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    وگرنه
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    بازگشت FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    وگرنه اگر (options & جستجو_HIS)	// put new pattern in history
	add_to_history(HIST_جستجو, pat, patlen, TRUE, NUL);

    اگر (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    وگرنه
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    اگر (mr_pattern == پوچ)
	mr_patternlen = 0;
    وگرنه
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    اگر (!(options & جستجو_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	اگر (pat_save == RE_جستجو || pat_save == RE_BOTH)
	    save_re_pat(RE_جستجو, pat, patlen, magic);
	// substitute or global command
	اگر (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    اگر (regmatch->regprog == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_جستجو_pat(تهی)
{
    بازگشت mr_pattern;
}

    تهی
save_re_pat(صحیح idx, char_u *pat, size_t patlen, صحیح magic)
{
    اگر (spats[idx].pat == pat)
	بازگشت;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    اگر (spats[idx].pat == پوچ)
	spats[idx].patlen = 0;
    وگرنه
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_جستجو_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    اگر (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ایستا صحیح save_level = 0;

    تهی
save_جستجو_patterns(تهی)
{
    صحیح i;

    اگر (save_level++ != 0)
	بازگشت;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	اگر (spats[i].pat != پوچ)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    اگر (saved_spats[i].pat == پوچ)
		saved_spats[i].patlen = 0;
	    وگرنه
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    اگر (mr_pattern == پوچ)
	saved_mr_pattern = پوچ;
    وگرنه
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    اگر (saved_mr_pattern == پوچ)
	saved_mr_patternlen = 0;
    وگرنه
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_جستجو_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    تهی
restore_جستجو_patterns(تهی)
{
    صحیح i;

    اگر (--save_level != 0)
	بازگشت;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#اگر defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_جستجو_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#اگر defined(EXITFREE)
    تهی
free_جستجو_patterns(تهی)
{
    صحیح i;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_جستجو_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ایستا spat_T	    saved_last_جستجو_spat;
ایستا صحیح	    did_save_last_جستجو_spat = 0;
ایستا صحیح	    saved_last_idx = 0;
ایستا صحیح	    saved_no_hlsearch = 0;
ایستا صحیح	    saved_جستجو_match_endcol;
ایستا صحیح	    saved_جستجو_match_سطرها;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    تهی
save_last_جستجو_pattern(تهی)
{
    اگر (++did_save_last_جستجو_spat != 1)
	// nested call, nothing to do
	بازگشت;

    saved_last_جستجو_spat = spats[RE_جستجو];
    اگر (spats[RE_جستجو].pat != پوچ)
    {
	saved_last_جستجو_spat.pat = vim_strnsave(spats[RE_جستجو].pat, spats[RE_جستجو].patlen);
	اگر (saved_last_جستجو_spat.pat == پوچ)
	    saved_last_جستجو_spat.patlen = 0;
	وگرنه
	    saved_last_جستجو_spat.patlen = spats[RE_جستجو].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    تهی
restore_last_جستجو_pattern(تهی)
{
    اگر (--did_save_last_جستجو_spat > 0)
	// nested call, nothing to do
	بازگشت;
    اگر (did_save_last_جستجو_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	بازگشت;
    }

    vim_free(spats[RE_جستجو].pat);
    spats[RE_جستجو] = saved_last_جستجو_spat;
    saved_last_جستجو_spat.pat = پوچ;
    saved_last_جستجو_spat.patlen = 0;
# اگر defined(FEAT_EVAL)
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
    ایستا تهی
save_incsearch_state(تهی)
{
    saved_جستجو_match_endcol = جستجو_match_endcol;
    saved_جستجو_match_سطرها  = جستجو_match_سطرها;
}

    ایستا تهی
restore_incsearch_state(تهی)
{
    جستجو_match_endcol = saved_جستجو_match_endcol;
    جستجو_match_سطرها  = saved_جستجو_match_سطرها;
}

    char_u *
last_جستجو_pattern(تهی)
{
    بازگشت spats[RE_جستجو].pat;
}

    size_t
last_جستجو_pattern_len(تهی)
{
    بازگشت spats[RE_جستجو].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    صحیح
ignorecase(char_u *pat)
{
    بازگشت ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    صحیح
ignorecase_opt(char_u *pat, صحیح ic_in, صحیح scs)
{
    صحیح		ic = ic_in;

    اگر (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    بازگشت ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    صحیح
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (تهی)skip_regexp_ex(pat, NUL, magic_isset(), پوچ, پوچ, &magic_val);

    وقتی (*p != NUL)
    {
	صحیح		l;

	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    اگر (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		بازگشت TRUE;
	    p += l;
	}
	وگرنه اگر (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    اگر (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    وگرنه اگر (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    وگرنه اگر (p[1] != NUL)  // skip "\X"
		p += 2;
	    وگرنه
		p += 1;
	}
	وگرنه اگر ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    اگر (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    وگرنه
		p++;
	}
	وگرنه اگر (MB_ISUPPER(*p))
	    بازگشت TRUE;
	وگرنه
	    ++p;
    }
    بازگشت FALSE;
}

#اگر defined(FEAT_EVAL)
    char_u *
last_csearch(تهی)
{
    بازگشت lastc_bytes;
}

    صحیح
last_csearch_forward(تهی)
{
    بازگشت lastcdir == FORWARD;
}

    صحیح
last_csearch_until(تهی)
{
    بازگشت last_t_cmd == TRUE;
}

    تهی
set_last_csearch(صحیح c, char_u *s, صحیح len)
{
    *lastc = c;
    lastc_bytelen = len;
    اگر (len)
	memcpy(lastc_bytes, s, len);
    وگرنه
	CLEAR_FIELD(lastc_bytes);
}
#endif

    تهی
set_csearch_direction(صحیح cdir)
{
    lastcdir = cdir;
}

    تهی
set_csearch_until(صحیح t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_جستجو_pat(تهی)
{
    بازگشت spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    تهی
reset_جستجو_dir(تهی)
{
    spats[0].off.dir = '/';
#اگر defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#اگر defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    تهی
set_last_جستجو_pat(
    char_u	*s,
    صحیح		idx,
    صحیح		magic,
    صحیح		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    اگر (*s == NUL)
	spats[idx].pat = پوچ;
    وگرنه
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    اگر (spats[idx].pat == پوچ)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# اگر defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.سطر = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    اگر (setlast)
	last_idx = idx;
    اگر (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	اگر (spats[idx].pat == پوچ)
	    saved_spats[idx].pat = پوچ;
	وگرنه
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	اگر (saved_spats[idx].pat == پوچ)
	    saved_spats[idx].patlen = 0;
	وگرنه
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_جستجو_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_جستجو_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    اگر (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_جستجو_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    تهی
last_pat_prog(regmmatch_T *regmatch)
{
    اگر (spats[last_idx].pat == پوچ)
    {
	regmatch->regprog = پوچ;
	بازگشت;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (تهی)جستجو_regcomp((char_u *)"", 0, پوچ, 0, last_idx, جستجو_KEEP, regmatch);
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
    صحیح
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    صحیح		dir,
    char_u	*pat,
    size_t	patlen,
    long	شمار,
    صحیح		options,
    صحیح		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    صحیح		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    صحیح		loop;
    pos_T	start_pos;
    صحیح		at_first_سطر;
    صحیح		extra_col;
    صحیح		start_char_len;
    صحیح		match_ok;
    long	nmatched;
    صحیح		submatch = 0;
    صحیح		first_match = TRUE;
    صحیح		called_emsg_before = called_emsg;
#ifdef FEAT_جستجو_EXTRA
    صحیح		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    صحیح		unused_timeout_flag = FALSE;
    صحیح		*timed_out = &unused_timeout_flag;  // set when timed out.
    صحیح		جستجو_from_match_end;		    // vi-compatible search?

    اگر (جستجو_regcomp(pat, patlen, پوچ, RE_جستجو, pat_use,
		   (options & (جستجو_HIS + جستجو_KEEP)), &regmatch) == FAIL)
    {
	اگر ((options & جستجو_پیام) && !rc_did_emsg)
	    semsg(_(e_invalid_جستجو_string_str), mr_pattern);
	بازگشت FAIL;
    }

    جستجو_from_match_end = vim_strchr(p_cpo, CPO_جستجو) != پوچ;

    اگر (extra_arg != پوچ)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	اگر (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    انجامبده	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	اگر (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	وگرنه اگر (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_سطر_شمار
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    اگر (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    وگرنه
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	وگرنه
	    start_char_len = 1;
	اگر (dir == FORWARD)
	{
	    اگر (options & جستجو_START)
		extra_col = 0;
	    وگرنه
		extra_col = start_char_len;
	}
	وگرنه
	{
	    اگر (options & جستجو_START)
		extra_col = start_char_len;
	    وگرنه
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_سطر = TRUE;	// default: start in first line
	اگر (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_سطر = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	اگر (dir == BACKWARD && start_pos.col == 0
					     && (options & جستجو_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_سطر = FALSE;
	}
	وگرنه
	    lnum = pos->lnum;

	برای (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    برای ( ; lnum > 0 && lnum <= buf->b_ml.ml_سطر_شمار;
					   lnum += dir, at_first_سطر = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		اگر (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    بشکن;
		// Stop after passing the time limit.
		اگر (*timed_out)
		    بشکن;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_سطر && (options & جستجو_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		اگر (regmatch.regprog == پوچ)
		    بشکن;
		// Abort searching on an error (e.g., out of stack).
		اگر (called_emsg > called_emsg_before || *timed_out)
		    بشکن;
		اگر (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    اگر (lnum + matchpos.lnum > buf->b_ml.ml_سطر_شمار)
			ptr = (char_u *)"";
		    وگرنه
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    اگر (dir == FORWARD && at_first_سطر)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			وقتی (matchpos.lnum == 0
				&& ((options & جستجو_END) && first_match
				    ?  (nmatched == 1
					&& (صحیح)endpos.col - 1
					     < (صحیح)start_pos.col + extra_col)
				    : ((صحیح)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (صحیح)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    اگر (جستجو_from_match_end)
			    {
				اگر (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    بشکن;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				اگر (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وگرنه
					++matchcol;
				}
			    }
			    وگرنه
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				اگر (ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    وگرنه
					++matchcol;
				}
			    }
			    اگر (matchcol == 0 && (options & جستجو_START))
				بشکن;
			    اگر (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				بشکن;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    اگر (regmatch.regprog == پوچ)
				بشکن;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			اگر (!match_ok)
			    ادامه;
		    }
		    اگر (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			برای (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    اگر (loop
				|| ((options & جستجو_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (صحیح)regmatch.endpos[0].col - 1
							< (صحیح)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (صحیح)regmatch.startpos[0].col
						      < (صحیح)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    وگرنه
				بشکن;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    اگر (جستجو_from_match_end)
			    {
				اگر (nmatched > 1)
				    بشکن;
				matchcol = endpos.col;
				// for empty match: advance one char
				اگر (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وگرنه
					++matchcol;
				}
			    }
			    وگرنه
			    {
				// Stop when the match is in a next line.
				اگر (matchpos.lnum > 0)
				    بشکن;
				matchcol = matchpos.col;
				اگر (ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وگرنه
					++matchcol;
				}
			    }
			    اگر (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				اگر (*timed_out)
				    match_ok = FALSE;
				بشکن;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    اگر (regmatch.regprog == پوچ)
				بشکن;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			اگر (!match_ok)
			    ادامه;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    اگر ((options & جستجو_END) && !(options & جستجو_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			اگر (endpos.col == 0)
			{
			    اگر (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			وگرنه
			{
			    --pos->col;
			    اگر (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_سطر_شمار)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			اگر (end_pos != پوچ)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    وگرنه
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			اگر (end_pos != پوچ)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    اگر (end_pos != پوچ)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    جستجو_match_سطرها = endpos.lnum - matchpos.lnum;
		    جستجو_match_endcol = endpos.col;
		    بشکن;
		}
		سطر_breakcheck();	// stop if ctrl-C typed
		اگر (got_int)
		    بشکن;

#ifdef FEAT_جستجو_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		اگر ((options & جستجو_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    بشکن;
		}
#endif

		اگر (loop && lnum == start_pos.lnum)
		    بشکن;	    // if second loop, stop where started
	    }
	    at_first_سطر = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    اگر (regmatch.regprog == پوچ)
		بشکن;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    اگر (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_جستجو_EXTRA
			      || break_loop
#endif
			      || found || loop)
		بشکن;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    اگر (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_سطر_شمار;
	    وگرنه
		lnum = 1;
	    اگر (!shortmess(SHM_جستجو)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & جستجو_پیام))
		give_warning((char_u *)_(dir == BACKWARD
					  ? بالاترین_bot_پیام : bot_بالاترین_پیام), TRUE);
	    اگر (extra_arg != پوچ)
		extra_arg->sa_wrapped = TRUE;
	}
	اگر (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_جستجو_EXTRA
		|| break_loop
#endif
		)
	    بشکن;
    }
    وقتی (--شمار > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    اگر (extra_arg != پوچ && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    اگر (!found)		    // did not find it
    {
	اگر (got_int)
	    emsg(_(e_interrupted));
	وگرنه اگر ((options & جستجو_پیام) == جستجو_پیام)
	{
	    اگر (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    وگرنه اگر (lnum == 0)
		semsg(_(e_جستجو_hit_بالاترین_without_match_for_str), mr_pattern);
	    وگرنه
		semsg(_(e_جستجو_hit_bottom_without_match_for_str), mr_pattern);
	}
	بازگشت FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    اگر (pos->lnum > buf->b_ml.ml_سطر_شمار)
    {
	pos->lnum = buf->b_ml.ml_سطر_شمار;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	اگر (pos->col > 0)
	    --pos->col;
    }

    بازگشت submatch + 1;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
    تهی
set_جستجو_direction(صحیح cdir)
{
    spats[0].off.dir = cdir;
}

    ایستا تهی
set_vv_searchforward(تهی)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ایستا صحیح
first_submatch(regmmatch_T *rp)
{
    صحیح		submatch;

    برای (submatch = 1; ; ++submatch)
    {
	اگر (rp->startpos[submatch].lnum >= 0)
	    بشکن;
	اگر (submatch == 9)
	{
	    submatch = 0;
	    بشکن;
	}
    }
    بازگشت submatch;
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
    صحیح
parse_جستجو_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    صحیح		جستجو_delim,
    صحیح		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    صحیح		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    اگر (*pat == پوچ || **pat == NUL)
	بازگشت 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = پوچ;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, جستجو_delim, magic_isset(),
						    strcopy, پوچ, پوچ);
    اگر (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (صحیح)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    اگر (*p == جستجو_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->سطر = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    اگر (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->سطر = TRUE;
    وگرنه اگر ((options & جستجو_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	اگر (*p == 'e')		// end
	    offset->end = جستجو_END;
	++p;
    }
    اگر (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	اگر (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((نویسه *)p);
	وگرنه اگر (*p == '-')	    // single '-'
	    offset->off = -1;
	وگرنه			    // single '+'
	    offset->off = 1;
	++p;
	وقتی (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (صحیح)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    بازگشت cmdlen;
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
    صحیح
do_جستجو(
    oparg_T	    *oap,	// can be NULL
    صحیح		    dirc,	// '/' or '?'
    صحیح		    جستجو_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    شمار,
    صحیح		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    صحیح		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = پوچ;
    صحیح		    show_جستجو_stats;
    char_u	    *msgbuf = پوچ;
    size_t	    msgbuflen = 0;
    صحیح		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    اگر (spats[0].off.سطر && vim_strchr(p_cpo, CPO_LINEOFF) != پوچ)
    {
	spats[0].off.سطر = FALSE;
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
    اگر (dirc == 0)
	dirc = spats[0].off.dir;
    وگرنه
    {
	spats[0].off.dir = dirc;
#اگر defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    اگر (options & جستجو_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#وگرنه
	اگر (dirc == '/')
	    dirc = '?';
	وگرنه
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    اگر (dirc == '/')
    {
	اگر (hasFolding(pos.lnum, پوچ, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    وگرنه
    {
	اگر (hasFolding(pos.lnum, &pos.lnum, پوچ))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_جستجو_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    اگر (no_hlsearch && !(options & جستجو_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    برای (;;)
    {
	صحیح		show_بالاترین_bot_پیام = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = پوچ;
					    // use previous pattern
	اگر (pat == پوچ || *pat == NUL || *pat == جستجو_delim)
	{
	    اگر (spats[RE_جستجو].pat == پوچ)	    // no previous pattern
	    {
		اگر (spats[RE_SUBST].pat == پوچ)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    برو end_do_جستجو;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    وگرنه
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	اگر (pat != پوچ && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_جستجو_pattern_offset(&pat, &patlen,
				جستجو_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    اگر (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_جستجو_stats = FALSE;
	اگر ((options & جستجو_ECHO) && messaging()
		&& !پیام_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    پیام_start();

	    // Get the offset, so we know how long it is.
	    اگر (!cmd_silent &&
		    (spats[0].off.سطر || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		اگر (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		وگرنه اگر (!spats[0].off.سطر)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		اگر (spats[0].off.off != 0 || spats[0].off.سطر)
		    off_len += vim_snprintf((نویسه *)off_buf + off_len,
			   اندازه(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    اگر (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    وگرنه
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    اگر (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		اگر (پیام_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (صحیح)(Rows - پیام_row) * cmdline_width - 1;
		وگرنه
		    // Use up to 'showcmd' column.
		    msgbufsize = (صحیح)(Rows - پیام_row - 1) * cmdline_width
			+ sc_col - 1;
		اگر (msgbufsize < plen + off_len + جستجو_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + جستجو_STAT_BUF_LEN + 3;
	    }
	    وگرنه
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    اگر (msgbuf == پوچ)
	    {
		msgbuflen = 0;
	    }
	    وگرنه
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		اگر (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    اگر (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    وگرنه
			mch_memmove(msgbuf + 1, p, plen);
		    اگر (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = پیام_strtrunc(msgbuf, TRUE);
		    اگر (trunc != پوچ)
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
		    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			اگر (r != پوچ)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    وقتی (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    اگر ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    وگرنه
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    پیام_outtrans(msgbuf);
		    پیام_clr_eos();
		    پیام_check();

		    gotocmdline(FALSE);
		    out_flush();
		    پیام_nowait = TRUE;	    // don't wait for this message
		}

		اگر (!shortmess(SHM_SEARCHCOUNT))
		    show_جستجو_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	اگر (!spats[0].off.سطر && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    اگر (spats[0].off.off > 0)
	    {
		برای (c = spats[0].off.off; c; --c)
		    اگر (decl(&pos) == -1)
			بشکن;
		اگر (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    وگرنه
	    {
		برای (c = spats[0].off.off; c; ++c)
		    اگر (incl(&pos) == -1)
			بشکن;
		اگر (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_سطر_شمار + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, پوچ,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, شمار, spats[0].off.end + (options &
		       (جستجو_KEEP + جستجو_PEEK + جستجو_HIS
			+ جستجو_پیام + جستجو_START
			+ ((pat != پوچ && *pat == ';') ? 0 : جستجو_NOOF))),
		RE_LAST, sia);

	اگر (dircp != پوچ)
	    *dircp = جستجو_delim; // restore second '/' or '?' for normal_cmd()

	اگر (!shortmess(SHM_جستجو) && sia && sia->sa_wrapped)
	    show_بالاترین_bot_پیام = TRUE;

	اگر (c == FAIL)
	{
	    retval = 0;
	    برو end_do_جستجو;
	}
	اگر (spats[0].off.end && oap != پوچ)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	اگر (!(options & جستجو_NOOF) || (pat != پوچ && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    اگر (spats[0].off.سطر)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		اگر (c < 1)
		    pos.lnum = 1;
		وگرنه اگر (c > curbuf->b_ml.ml_سطر_شمار)
		    pos.lnum = curbuf->b_ml.ml_سطر_شمار;
		وگرنه
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    وگرنه اگر (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		اگر (c > 0)
		{
		    وقتی (c-- > 0)
			اگر (incl(&pos) == -1)
			    بشکن;
		}
		// to the left, check for start of file
		وگرنه
		{
		    وقتی (c++ < 0)
			اگر (decl(&pos) == -1)
			    بشکن;
		}
	    }
	    اگر (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	اگر (show_جستجو_stats)
	     cmdline_جستجو_stat(dirc, &pos, &curwin->w_cursor,
				show_بالاترین_bot_پیام, msgbuf, msgbuflen,
				(شمار != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_جستجو)
				     && hasFolding(curwin->w_cursor.lnum,
								   پوچ, پوچ))
#endif
				),
				p_msc,
				جستجو_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	اگر (!(options & جستجو_OPT) || pat == پوچ || *pat != ';')
	    بشکن;

	dirc = *++pat;
	--patlen;
	جستجو_delim = dirc;
	اگر (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    برو end_do_جستجو;
	}
	++pat;
	--patlen;
    }

    اگر (options & جستجو_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_جستجو:
    اگر ((options & جستجو_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    بازگشت retval;
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
    صحیح
جستجو_for_exact_سطر(
    buf_T	*buf,
    pos_T	*pos,
    صحیح		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    صحیح		compl_len = ins_compl_len();

    اگر (buf->b_ml.ml_سطر_شمار == 0)
	بازگشت FAIL;
    برای (;;)
    {
	pos->lnum += dir;
	اگر (pos->lnum < 1)
	{
	    اگر (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_سطر_شمار;
		اگر (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(بالاترین_bot_پیام), TRUE);
	    }
	    وگرنه
	    {
		pos->lnum = 1;
		بشکن;
	    }
	}
	وگرنه اگر (pos->lnum > buf->b_ml.ml_سطر_شمار)
	{
	    اگر (p_ws)
	    {
		pos->lnum = 1;
		اگر (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(bot_بالاترین_پیام), TRUE);
	    }
	    وگرنه
	    {
		pos->lnum = 1;
		بشکن;
	    }
	}
	اگر (pos->lnum == start)
	    بشکن;
	اگر (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	اگر (compl_status_adding() && !compl_status_sol())
	{
	    اگر ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		بازگشت OK;
	}
	وگرنه اگر (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    اگر ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		بازگشت OK;
	}
    }
    بازگشت FAIL;
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
    صحیح
searchc(cmdarg_T *cap, صحیح t_cmd)
{
    صحیح			c = cap->nchar;	// char to search for
    صحیح			dir = cap->arg;	// TRUE for searching forward
    long		شمار = cap->count1;	// repeat count
    صحیح			col;
    char_u		*p;
    صحیح			len;
    صحیح			stop = TRUE;

    اگر (c != NUL)	// normal search: remember args for repeat
    {
	اگر (!کلید_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    اگر (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		اگر (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    وگرنه		// repeat previous search
    {
	اگر (*lastc == NUL && lastc_bytelen <= 1)
	    بازگشت FAIL;
	اگر (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	وگرنه
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	اگر (vim_strchr(p_cpo, CPO_SCOLON) == پوچ && شمار == 1 && t_cmd)
	    stop = FALSE;
    }

    اگر (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    وگرنه
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    وقتی (شمار--)
    {
	اگر (has_mbyte)
	{
	    برای (;;)
	    {
		اگر (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    اگر (col >= len)
			بازگشت FAIL;
		}
		وگرنه
		{
		    اگر (col == 0)
			بازگشت FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		اگر (lastc_bytelen <= 1)
		{
		    اگر (p[col] == c && stop)
			بشکن;
		}
		وگرنه اگر (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    بشکن;
		stop = TRUE;
	    }
	}
	وگرنه
	{
	    برای (;;)
	    {
		اگر ((col += dir) < 0 || col >= len)
		    بازگشت FAIL;
		اگر (p[col] == c && stop)
		    بشکن;
		stop = TRUE;
	    }
	}
    }

    اگر (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	اگر (has_mbyte)
	{
	    اگر (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    وگرنه
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    بازگشت OK;
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
findmatch(oparg_T *oap, صحیح initc)
{
    بازگشت findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ایستا صحیح
check_prevcol(
    char_u	*linep,
    صحیح		col,
    صحیح		ch,
    صحیح		*prevcol)
{
    --col;
    اگر (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    اگر (prevcol)
	*prevcol = col;
    بازگشت (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ایستا صحیح
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    صحیح		found = FALSE;

    برای (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    اگر (delim_copy == پوچ)
	بازگشت FALSE;
    برای (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *سطر = ml_get(lnum);

	برای (p = سطر + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    اگر (lnum == endpos->lnum && (colnr_T)(p - سطر) >= endpos->col)
		بشکن;
	    اگر (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		بشکن;
	    }
	}
	اگر (found)
	    بشکن;
    }
    vim_free(delim_copy);
    بازگشت found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ایستا تهی
find_mps_مقادیر(
    صحیح	    *initc,
    صحیح	    *findc,
    صحیح	    *backwards,
    صحیح	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    وقتی (*ptr != NUL)
    {
	اگر (has_mbyte)
	{
	    char_u *prev;

	    اگر (mb_ptr2char(ptr) == *initc)
	    {
		اگر (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		وگرنه
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		بازگشت;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    اگر (mb_ptr2char(ptr) == *initc)
	    {
		اگر (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		وگرنه
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		بازگشت;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	وگرنه
	{
	    اگر (*ptr == *initc)
	    {
		اگر (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		وگرنه
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		بازگشت;
	    }
	    ptr += 2;
	    اگر (*ptr == *initc)
	    {
		اگر (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		وگرنه
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		بازگشت;
	    }
	    ++ptr;
	}
	اگر (*ptr == ',')
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
    صحیح		initc,
    صحیح		flags,
    صحیح		maxtravel)
{
    ایستا pos_T pos;			// current search position
    صحیح		findc = 0;		// matching brace
    صحیح		c;
    صحیح		شمار = 0;		// cumulative number of braces
    صحیح		backwards = FALSE;	// init for gcc
    صحیح		raw_string = FALSE;	// search for raw string
    صحیح		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    صحیح		do_quotes;		// check for quotes in current line
    صحیح		at_start;		// do_quotes value at start position
    صحیح		hash_dir = 0;		// Direction searched for # things
    صحیح		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    صحیح		start_in_quotes;	// start position is in quotes
    صحیح		traveled = 0;		// how far we've searched so far
    صحیح		ignore_cend = FALSE;    // ignore comment end
    صحیح		cpo_match;		// vi compatible matching
    صحیح		cpo_bsl;		// don't recognize backslashes
    صحیح		match_escaped = 0;	// search for escaped match
    صحیح		dir;			// Direction to search
    صحیح		comment_col = MAXCOL;   // start of / / comment
    صحیح		lispcomm = FALSE;	// inside of Lisp-style comment
    صحیح		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    صحیح		skip_comments = (flags & FM_SKIPCOMM) != 0;
    صحیح		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != پوچ);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != پوچ);

    // Direction to search when initc is '/', '*' or '#'
    اگر (flags & FM_BACKWARD)
	dir = BACKWARD;
    وگرنه اگر (flags & FM_FORWARD)
	dir = FORWARD;
    وگرنه
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    اگر (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	اگر (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    وگرنه اگر (initc != '#' && initc != NUL)
    {
	find_mps_مقادیر(&initc, &findc, &backwards, TRUE);
	اگر (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	اگر (findc == NUL)
	    بازگشت پوچ;
    }
    وگرنه
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	اگر (initc == '#')
	{
	    hash_dir = dir;
	}
	وگرنه
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    اگر (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		اگر (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    اگر (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		وگرنه اگر (linep[pos.col] == '/')
		{
		    اگر (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    وگرنه اگر (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		وگرنه اگر (linep[pos.col] == '*')
		{
		    اگر (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    وگرنه اگر (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    اگر (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		اگر (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		برای (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    اگر (initc == NUL)
			بشکن;

		    find_mps_مقادیر(&initc, &findc, &backwards, FALSE);
		    اگر (findc)
			بشکن;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		اگر (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    اگر (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    وگرنه
			بازگشت پوچ;
		}
		وگرنه اگر (!cpo_bsl)
		{
		    صحیح col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    برای (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	اگر (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    اگر (oap != پوچ)
		oap->motion_type = MLINE;   // Linewise for this case only
	    اگر (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		اگر (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		وگرنه اگر (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		وگرنه
		    بازگشت پوچ;
	    }
	    pos.col = 0;
	    وقتی (!got_int)
	    {
		اگر (hash_dir > 0)
		{
		    اگر (pos.lnum == curbuf->b_ml.ml_سطر_شمار)
			بشکن;
		}
		وگرنه اگر (pos.lnum == 1)
		    بشکن;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		سطر_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		اگر (*ptr != '#')
		    ادامه;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		اگر (hash_dir > 0)
		{
		    اگر (STRNCMP(ptr, "if", 2) == 0)
			شمار++;
		    وگرنه اگر (STRNCMP(ptr, "el", 2) == 0)
		    {
			اگر (شمار == 0)
			    بازگشت &pos;
		    }
		    وگرنه اگر (STRNCMP(ptr, "endif", 5) == 0)
		    {
			اگر (شمار == 0)
			    بازگشت &pos;
			شمار--;
		    }
		}
		وگرنه
		{
		    اگر (STRNCMP(ptr, "if", 2) == 0)
		    {
			اگر (شمار == 0)
			    بازگشت &pos;
			شمار--;
		    }
		    وگرنه اگر (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			اگر (شمار == 0)
			    بازگشت &pos;
		    }
		    وگرنه اگر (STRNCMP(ptr, "endif", 5) == 0)
			شمار++;
		}
	    }
	    بازگشت پوچ;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    اگر (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != پوچ)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    اگر ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    اگر (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    اگر (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    وقتی (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	اگر (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    اگر (lispcomm && pos.col < (colnr_T)comment_col)
		بشکن;
	    اگر (pos.col == 0)		// at start of line, go to prev. one
	    {
		اگر (pos.lnum == 1)	// start of file
		    بشکن;
		--pos.lnum;

		اگر (maxtravel > 0 && ++traveled > maxtravel)
		    بشکن;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		سطر_breakcheck();

		// Check if this line contains a single-line comment
		اگر (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		اگر (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		وگرنه اگر (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    وگرنه
	    {
		--pos.col;
		اگر (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	وگرنه				// forward search
	{
	    اگر (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		اگر (pos.lnum == curbuf->b_ml.ml_سطر_شمار  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    بشکن;
		++pos.lnum;

		اگر (maxtravel && traveled++ > maxtravel)
		    بشکن;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		سطر_breakcheck();
		اگر (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    وگرنه
	    {
		اگر (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		وگرنه
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	اگر (skip_comments && !comment_dir && !inquote)
	{
	    اگر (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		اگر (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (صحیح)pos.col < comment_col))
		    in_block_comment = TRUE;
		وگرنه اگر (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    وگرنه
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		اگر (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (صحیح)pos.col < comment_col))
		    in_block_comment = TRUE;
		وگرنه اگر (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	اگر (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    اگر (linep[0] == findc && شمار == 0)	// match!
		بازگشت &pos;
	    بشکن;					// out of scope
	}

	اگر (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    اگر (comment_dir == FORWARD)
	    {
		اگر (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    بازگشت &pos;
		}
	    }
	    وگرنه    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		اگر (pos.col == 0)
		    ادامه;
		وگرنه اگر (raw_string)
		{
		    اگر (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != پوچ)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			اگر (!find_rawstring_end(linep, &pos,
				  شمار > 0 ? &match_pos : &curwin->w_cursor))
			{
			    شمار++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		وگرنه اگر (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (صحیح)pos.col < comment_col)
		{
		    شمار++;
		    match_pos = pos;
		    match_pos.col--;
		}
		وگرنه اگر (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    اگر (شمار > 0)
			pos = match_pos;
		    وگرنه اگر (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (صحیح)pos.col <= comment_col)
			pos.col -= 2;
		    وگرنه اگر (ignore_cend)
			ادامه;
		    وگرنه
			بازگشت پوچ;
		    بازگشت &pos;
		}
	    }
	    ادامه;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	اگر (cpo_match)
	    do_quotes = 0;
	وگرنه اگر (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    برای (ptr = linep; *ptr; ++ptr)
	    {
		اگر (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		اگر (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		اگر (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    اگر (!do_quotes)
	    {
		inquote = FALSE;
		اگر (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    اگر (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			اگر (inquote)
			    start_in_quotes = TRUE;
		    }
		    وگرنه اگر (backwards)
			inquote = TRUE;
		}
		اگر (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    اگر (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			اگر (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    اگر (inquote)
				start_in_quotes = TRUE;
			}
			وگرنه اگر (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	اگر (start_in_quotes == MAYBE)
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
	انتخاب (c)
	{
	حالت NUL:
	    // at end of line without trailing backslash, reset inquote
	    اگر (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    بشکن;

	حالت '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    اگر (do_quotes)
	    {
		صحیح col;

		برای (col = pos.col - 1; col >= 0; --col)
		    اگر (linep[col] != '\\')
			بشکن;
		اگر ((((صحیح)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    بشکن;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	حالت '\'':
	    اگر (!cpo_match && initc != '\'' && findc != '\'')
	    {
		اگر (backwards)
		{
		    اگر (pos.col > 1)
		    {
			اگر (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    بشکن;
			}
			وگرنه اگر (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    بشکن;
			}
		    }
		}
		وگرنه اگر (linep[pos.col + 1])	// forward search
		{
		    اگر (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			بشکن;
		    }
		    وگرنه اگر (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			بشکن;
		    }
		}
	    }
	    // FALLTHROUGH

	پیشفرض:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    اگر (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != پوچ
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', پوچ)
		    && check_prevcol(linep, pos.col - 1, '#', پوچ))
		بشکن;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    اگر (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (صحیح)pos.col >= comment_col)))
		بشکن;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    اگر ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		صحیح	col, bslcnt = 0;

		اگر (!cpo_bsl)
		{
		    برای (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		اگر (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    اگر (c == initc)
			شمار++;
		    وگرنه
		    {
			اگر (شمار == 0)
			    بازگشت &pos;
			شمار--;
		    }
		}
	    }
	}
    }

    اگر (comment_dir == BACKWARD && شمار > 0)
    {
	pos = match_pos;
	بازگشت &pos;
    }
    بازگشت (pos_T *)پوچ;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    تهی
showmatch(
    صحیح		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    صحیح		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    برای (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	اگر (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    بشکن;
#endif
	p += mb_ptr2len(p) + 1;
	اگر (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    بشکن;
	p += mb_ptr2len(p);
	اگر (*p == NUL)
	    بازگشت;
    }
    اگر (*p == NUL)
	بازگشت;

    اگر ((lpos = findmatch(پوچ, NUL)) == پوچ)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	بازگشت;
    }

    اگر (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	بازگشت;

    اگر (!curwin->w_p_wrap)
	getvcol(curwin, lpos, پوچ, &vcol, پوچ, 0);

    صحیح col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    اگر (!col_visible)
	بازگشت;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    اگر (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    اگر (vim_strchr(p_cpo, CPO_SHOWMATCH) != پوچ)
	ui_delay(p_mat * 100L + 8, TRUE);
    وگرنه اگر (!char_avail())
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
    ایستا صحیح
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    صحیح		move,
    pos_T	*cur,
    صحیح		direction)
{
    regmmatch_T	regmatch;
    صحیح		nmatched = 0;
    صحیح		result = -1;
    pos_T	pos;
    صحیح		called_emsg_before = called_emsg;
    صحیح		flag = 0;

    اگر (pattern == پوچ)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    اگر (جستجو_regcomp(pattern, patternlen, پوچ, RE_جستجو, RE_جستجو,
					      جستجو_KEEP, &regmatch) == FAIL)
	بازگشت -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    اگر (move)
    {
	CLEAR_POS(&pos);
    }
    وگرنه
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = جستجو_START;
    }

    اگر (searchit(curwin, curbuf, &pos, پوچ, direction, pattern, patternlen, 1,
				  جستجو_KEEP + flag, RE_جستجو, پوچ) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	انجامبده
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, پوچ);
	    اگر (nmatched != 0)
		بشکن;
	} وقتی (regmatch.regprog != پوچ
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	اگر (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    بازگشت result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحیح
current_جستجو(
    long	شمار,
    صحیح		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    صحیح		i;
    صحیح		dir;
    صحیح		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    صحیح		flags = 0;
    pos_T	save_VIsual = VIsual;
    صحیح		zero_width;
    صحیح		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    اگر (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    اگر (VIsual_active)
    {
	اگر (forward)
	    incl(&pos);
	وگرنه
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    اگر (zero_width == -1)
	بازگشت FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    برای (i = 0; i < 2; i++)
    {
	اگر (forward)
	{
	    اگر (i == 0 && skip_first_backward)
		ادامه;
	    dir = i;
	}
	وگرنه
	    dir = !i;

	flags = 0;
	اگر (!dir && !zero_width)
	    flags = جستجو_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	اگر (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? شمار : 1),
		جستجو_KEEP | flags, RE_جستجو, پوچ);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	اگر (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    اگر (VIsual_active)
		VIsual = save_VIsual;
	    بازگشت FAIL;
	}
	وگرنه اگر (i == 0 && !result)
	{
	    اگر (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    وگرنه
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_سطر_شمار;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_سطر_شمار);
	    }
	}
    }

    start_pos = pos;

    اگر (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    اگر (LT_POS(VIsual, end_pos) && forward)
    {
	اگر (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	وگرنه
	    // put the cursor on last character of match
	    dec_cursor();
    }
    وگرنه اگر (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    اگر (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	اگر (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	وگرنه اگر (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    اگر (fdo_flags & FDO_جستجو && کلید_Typed)
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

    بازگشت OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    صحیح
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    بازگشت (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ایستا تهی
cmdline_جستجو_stat(
    صحیح		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    صحیح		show_بالاترین_bot_پیام,
    char_u	*msgbuf,
    size_t	msgbuflen,
    صحیح		recompute,
    صحیح		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_جستجو_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    اگر (stat.cur <= 0)
	بازگشت;

    نویسه	t[جستجو_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	اگر (stat.incomplete == 1)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[?/??]");
	وگرنه اگر (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وگرنه اگر (stat.cnt > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	وگرنه
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    وگرنه
#endif
    {
	اگر (stat.incomplete == 1)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[?/??]");
	وگرنه اگر (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وگرنه اگر (stat.cnt > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	وگرنه
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    اگر (show_بالاترین_bot_پیام && len + 2 < جستجو_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    اگر (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    اگر (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    پیام_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    پیام_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    ایستا تهی
update_جستجو_stat(
    صحیح			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    صحیح			recompute,
    صحیح			maxcount,
    long		timeout UNUSED)
{
    صحیح		    save_ws = p_ws;
    صحیح		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ایستا pos_T    lastpos = {0, 0, 0};
    ایستا صحیح	    cur = 0;
    ایستا صحیح	    cnt = 0;
    ایستا صحیح	    exact_match = FALSE;
    ایستا صحیح	    incomplete = 0;
    ایستا صحیح	    last_maxcount = 0;
    ایستا صحیح	    chgtick = 0;
    ایستا char_u   *lastpat = پوچ;
    ایستا size_t   lastpatlen = 0;
    ایستا buf_T    *lbuf = پوچ;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    اگر (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	بازگشت;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    اگر (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != پوچ
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
    اگر (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    وگرنه
    {
	صحیح	done_جستجو = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	اگر (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	وقتی (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, پوچ, 0, 1, جستجو_KEEP, RE_LAST, پوچ) != FAIL)
	{
	    done_جستجو = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    اگر (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		بشکن;
	    }
#endif
	    cnt++;
	    اگر (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		اگر (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    اگر (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		بشکن;
	    }
	}
	اگر (got_int)
	    cur = -1; // abort
	اگر (done_جستجو)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    اگر (lastpat == پوچ)
		lastpatlen = 0;
	    وگرنه
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

#اگر defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ایستا char_u *
get_سطر_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *سطر = ml_get(lnum);

    vim_strncpy(buf, سطر, LSIZE - 1);
    بازگشت buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    تهی
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    صحیح		dir UNUSED,	// direction of expansion
    صحیح		len,		// length of search pattern
    صحیح		whole,		// match whole words only
    صحیح		skip_comments,	// don't match inside comments
    صحیح		type,		// Type of search; are we looking for a type?
				// a macro?
    long	شمار,
    صحیح		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    صحیح		forceit,	// If true, always switch to the found path
    صحیح		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    صحیح		max_path_depth = 50;
    long	match_شمار = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = پوچ;
    linenr_T	lnum;
    صحیح		depth;
    صحیح		depth_displayed;	// For type==CHECK_PATH
    صحیح		old_files;
    صحیح		already_searched;
    char_u	*file_سطر;
    char_u	*سطر;
    char_u	*p;
    char_u	save_char;
    صحیح		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    صحیح		matched = FALSE;
    صحیح		did_show = FALSE;
    صحیح		found = FALSE;
    صحیح		i;
    char_u	*already = پوچ;
    char_u	*startp = پوچ;
    char_u	*inc_opt = پوچ;
# اگر defined(FEAT_QUICKFIX)
    win_T	*curwin_save = پوچ;
# endif

    regmatch.regprog = پوچ;
    incl_regmatch.regprog = پوچ;
    def_regmatch.regprog = پوچ;

    file_سطر = alloc(LSIZE);
    اگر (file_سطر == پوچ)
	بازگشت;

    اگر (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	اگر (pat == پوچ)
	    برو fpip_end;
	vim_snprintf((نویسه *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	اگر (regmatch.regprog == پوچ)
	    برو fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    اگر (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	اگر (incl_regmatch.regprog == پوچ)
	    برو fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    اگر (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	اگر (def_regmatch.regprog == پوچ)
	    برو fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * اندازه(SearchedFile), TRUE);
    اگر (files == پوچ)
	برو fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    اگر (end_lnum > curbuf->b_ml.ml_سطر_شمار)
	end_lnum = curbuf->b_ml.ml_سطر_شمار;
    اگر (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    سطر = get_سطر_and_copy(lnum, file_سطر);

    برای (;;)
    {
	اگر (incl_regmatch.regprog != پوچ
		&& vim_regexec(&incl_regmatch, سطر, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    اگر (strstr((نویسه *)inc_opt, "\\zs") != پوچ)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (صحیح)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    وگرنه
		// Use text after match with 'include'.
		new_fname = file_name_in_سطر(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, پوچ);
	    already_searched = FALSE;
	    اگر (new_fname != پوچ)
	    {
		// Check whether we have already searched in this file
		برای (i = 0;; i++)
		{
		    اگر (i == depth + 1)
			i = old_files;
		    اگر (i == max_path_depth)
			بشکن;
		    اگر (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			اگر (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    پیام_putchar('\n');	    // cursor below last one
			    اگر (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				پیام_home_replace_hl(new_fname);
				پیام_puts(_(" (includes previously listed match)"));
				prev_fname = پوچ;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			بشکن;
		    }
		}
	    }

	    اگر (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == پوچ && !already_searched)))
	    {
		اگر (did_show)
		    پیام_putchar('\n');	    // cursor below last one
		وگرنه
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    پیام_puts_title(_("--- Included files "));
		    اگر (action != ACTION_SHOW_ALL)
			پیام_puts_title(_("not found "));
		    پیام_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		وقتی (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    برای (i = 0; i < depth_displayed; i++)
			پیام_puts("  ");
		    پیام_home_replace(files[depth_displayed].name);
		    پیام_puts(" -->\n");
		}
		اگر (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    برای (i = 0; i <= depth_displayed; i++)
			پیام_puts("  ");
		    اگر (new_fname != پوچ)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			پیام_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    وگرنه
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			اگر (strstr((نویسه *)inc_opt, "\\zs") != پوچ)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (صحیح)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			وگرنه
			{
			    // find the file name after the end of the match
			    برای (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    برای (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			اگر (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (صحیح)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			وگرنه اگر (p > سطر)
			{
			    اگر (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    اگر (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			پیام_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    اگر (new_fname == پوچ && action == ACTION_SHOW_ALL)
		    {
			اگر (already_searched)
			    پیام_puts(_("  (Already listed)"));
			وگرنه
			    پیام_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    اگر (new_fname != پوچ)
	    {
		// Push the new file onto the file stack
		اگر (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    اگر (bigger != پوچ)
		    {
			برای (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			برای (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = پوچ;
			    bigger[i].name = پوچ;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			برای (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		اگر ((files[depth + 1].fp = mch_fopen((نویسه *)new_fname, "r"))
								    == پوچ)
		    vim_free(new_fname);
		وگرنه
		{
		    اگر (++depth == old_files)
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
		    اگر (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			پیام_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((نویسه*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(نویسه *)new_fname);
			پیام_trunc_attr((نویسه *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    وگرنه اگر (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (نویسه *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	وگرنه
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = سطر;
جستجو_سطر:
	    define_matched = FALSE;
	    اگر (def_regmatch.regprog != پوچ
			      && vim_regexec(&def_regmatch, سطر, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		وقتی (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    اگر (def_regmatch.regprog == پوچ || define_matched)
	    {
		اگر (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    اگر (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    وگرنه
			matched = STRNCMP(startp, ptr, len) == 0;
		    اگر (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		وگرنه اگر (regmatch.regprog != پوچ
			 && vim_regexec(&regmatch, سطر, (colnr_T)(p - سطر)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    اگر (!define_matched && skip_comments)
		    {
			اگر ((*سطر != '#' ||
				STRNCMP(skipwhite(سطر + 1), "define", 6) != 0)
				&& get_leader_len(سطر, پوچ, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(سطر);
			اگر (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    برای (p = سطر; *p && p < startp; ++p)
			    {
				اگر (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    اگر (p[1] == '/')
					بشکن;
				    ++p;
				}
				وگرنه اگر (!matched && p[0] == '*' && p[1] == '/')
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
	اگر (matched)
	{
	    اگر (action == ACTION_EXPAND)
	    {
		صحیح	cont_s_ipos = FALSE;
		صحیح	add_r;
		char_u	*aux;

		اگر (depth == -1 && lnum == curwin->w_cursor.lnum)
		    بشکن;
		found = TRUE;
		aux = p = startp;
		اگر (compl_status_adding() && (صحیح)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    اگر (vim_iswordp(p))
			برو exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (صحیح)(p - aux);

		اگر (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    اگر (depth < 0)
		    {
			اگر (lnum >= end_lnum)
			    برو exit_matched;
			سطر = get_سطر_and_copy(++lnum, file_سطر);
		    }
		    وگرنه اگر (vim_fgets(سطر = file_سطر,
						      LSIZE, files[depth].fp))
			برو exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(سطر);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    اگر (p > aux)
		    {
			اگر (*aux != ')' && IObuff[i-1] != TAB)
			{
			    اگر (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    اگر (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == پوچ
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			اگر (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (صحیح)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    اگر (i == ins_compl_len())
			برو exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? پوچ
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		اگر (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		وگرنه اگر (add_r == FAIL)
		    بشکن;
	    }
	    وگرنه اگر (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		اگر (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		اگر (curr_fname != prev_fname)
		{
		    اگر (did_show)
			پیام_putchar('\n');	// cursor below last one
		    اگر (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			پیام_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		اگر (!got_int)
		    show_pat_in_path(سطر, type, TRUE, action,
			    (depth == -1) ? پوچ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_شمار++);

		// Set matched flag for this file and all the ones that
		// include it
		برای (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    وگرنه اگر (--شمار <= 0)
	    {
		found = TRUE;
		اگر (depth == -1 && lnum == curwin->w_cursor.lnum
# اگر defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_سطر));
		وگرنه اگر (action == ACTION_SHOW)
		{
		    show_pat_in_path(سطر, type, did_show, action,
			    (depth == -1) ? پوچ : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		وگرنه
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# اگر defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    اگر (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    اگر (action == ACTION_SPLIT)
		    {
			اگر (win_split(0, 0) == FAIL)
			    بشکن;
			RESET_BINDING(curwin);
		    }
		    اگر (depth == -1)
		    {
			// match in current file
# اگر defined(FEAT_QUICKFIX)
			اگر (g_do_tagpreview != 0)
			{
			    اگر (!win_valid(curwin_save))
				بشکن;
			    اگر (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, پوچ,
						     پوچ, TRUE, lnum, forceit)))
				بشکن;	// failed to jump to file
			}
			وگرنه
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    وگرنه
		    {
			اگر (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, پوچ, TRUE,
						    files[depth].lnum, forceit)))
			    بشکن;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		اگر (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - سطر);
		    curwin->w_set_curswant = true;
		}

# اگر defined(FEAT_QUICKFIX)
		اگر (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		وگرنه اگر (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		بشکن;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    اگر (def_regmatch.regprog == پوچ
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		برو جستجو_سطر;
	}
	سطر_breakcheck();
	اگر (action == ACTION_EXPAND)
	    ins_compl_check_کلیدها(30, FALSE);
	اگر (got_int || ins_compl_interrupted())
	    بشکن;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	وقتی (depth >= 0 && !already
		&& vim_fgets(سطر = file_سطر, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    اگر (depth < depth_displayed)
		depth_displayed = depth;
	}
	اگر (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (صحیح)STRLEN(سطر);
	    اگر (i > 0 && سطر[i - 1] == '\n')
		سطر[--i] = NUL;
	    اگر (i > 0 && سطر[i - 1] == '\r')
		سطر[--i] = NUL;
	}
	وگرنه اگر (!already)
	{
	    اگر (++lnum > end_lnum)
		بشکن;
	    سطر = get_سطر_and_copy(lnum, file_سطر);
	}
	already = پوچ;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    برای (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    برای (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    اگر (type == CHECK_PATH)
    {
	اگر (!did_show)
	{
	    اگر (action != ACTION_SHOW_ALL)
		پیام(_("All included files were found"));
	    وگرنه
		پیام(_("No included files"));
	}
    }
    وگرنه اگر (!found && action != ACTION_EXPAND && !silent)
    {
	اگر (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	وگرنه اگر (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	وگرنه
	    emsg(_(e_couldnt_find_pattern));
    }
    اگر (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	پیام_end();

fpip_end:
    vim_free(file_سطر);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ایستا تهی
show_pat_in_path(
    char_u  *سطر,
    صحیح	    type,
    صحیح	    did_show,
    صحیح	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    شمار)
{
    char_u  *p;
    size_t  linelen;

    اگر (did_show)
	پیام_putchar('\n');	// cursor below last one
    وگرنه اگر (!پیام_silent)
	gotocmdline(TRUE);	// cursor at status line
    اگر (got_int)		// 'q' typed at "--more--" message
	بازگشت;
    linelen = STRLEN(سطر);
    برای (;;)
    {
	p = سطر + linelen - 1;
	اگر (fp != پوچ)
	{
	    // We used fgets(), so get rid of newline at end
	    اگر (p >= سطر && *p == '\n')
		--p;
	    اگر (p >= سطر && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	اگر (action == ACTION_SHOW_ALL)
	{
	    sprintf((نویسه *)IObuff, "%3ld: ", شمار);	// show match nr
	    پیام_puts((نویسه *)IObuff);
	    sprintf((نویسه *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    پیام_puts_attr((نویسه *)IObuff, HL_ATTR(HLF_N));
	    پیام_puts(" ");
	}
	پیام_prt_سطر(سطر, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	اگر (got_int || type != FIND_DEFINE || p < سطر || *p != '\\')
	    بشکن;

	اگر (fp != پوچ)
	{
	    اگر (vim_fgets(سطر, LSIZE, fp)) // end of file
		بشکن;
	    linelen = STRLEN(سطر);
	    ++*lnum;
	}
	وگرنه
	{
	    اگر (++*lnum > curbuf->b_ml.ml_سطر_شمار)
		بشکن;
	    سطر = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	پیام_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(صحیح idx)
{
    بازگشت &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    صحیح
get_spat_last_idx(تهی)
{
    بازگشت last_idx;
}
#endif

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    تهی
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = پوچ;
    صحیح			maxcount = p_msc;
    long		timeout = جستجو_STAT_DEF_TIMEOUT;
    صحیح			recompute = TRUE;
    searchstat_T	stat;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	صحیح		error = FALSE;

	اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    بازگشت;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	اگر (di != پوچ)
	{
	    timeout = (long)tv_get_عدد_chk(&di->di_tv, &error);
	    اگر (error)
		بازگشت;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	اگر (di != پوچ)
	{
	    maxcount = (صحیح)tv_get_عدد_chk(&di->di_tv, &error);
	    اگر (error)
		بازگشت;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	اگر (di != پوچ)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    اگر (pattern == پوچ)
		بازگشت;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	اگر (di != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		بازگشت;
	    }
	    اگر (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		بازگشت;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    اگر (li != پوچ)
	    {
		pos.lnum = tv_get_عدد_chk(&li->li_tv, &error);
		اگر (error)
		    بازگشت;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    اگر (li != پوچ)
	    {
		pos.col = tv_get_عدد_chk(&li->li_tv, &error) - 1;
		اگر (error)
		    بازگشت;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    اگر (li != پوچ)
	    {
		pos.coladd = tv_get_عدد_chk(&li->li_tv, &error);
		اگر (error)
		    بازگشت;
	    }
	}
    }

    save_last_جستجو_pattern();
# ifdef FEAT_جستجو_EXTRA
    save_incsearch_state();
# endif
    اگر (pattern != پوچ)
    {
	اگر (*pattern == NUL)
	    برو the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	اگر (spats[last_idx].pat == پوچ)
	    spats[last_idx].patlen = 0;
    }
    اگر (spats[last_idx].pat == پوچ || *spats[last_idx].pat == NUL)
	برو the_end;	// the previous pattern was never defined

    update_جستجو_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_عدد(rettv->vval.v_dict, "current", stat.cur);
    dict_add_عدد(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_عدد(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_عدد(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_عدد(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_جستجو_pattern();
# ifdef FEAT_جستجو_EXTRA
    restore_incsearch_state();
# endif
}
#endif
