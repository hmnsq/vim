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

#ضمن "vim.h"

#ifdef FEAT_EVAL
ساكن فراغ set_vv_searchforward(فراغ);
ساكن صحيح first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
ساكن char_u *get_line_and_copy(linenr_T lnum, char_u *buf);
ساكن فراغ show_pat_in_path(char_u *, صحيح, صحيح, صحيح, FILE *, linenr_T *, long);
#endif

تعريفنوع بنية searchstat
{
    صحيح	    cur;	    // current position of found words
    صحيح	    cnt;	    // total count of found words
    صحيح	    exact_match;    // TRUE if matched exactly on specified position
    صحيح	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    صحيح	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_SEARCH_EXTRA
ساكن فراغ save_incsearch_state(فراغ);
ساكن فراغ restore_incsearch_state(فراغ);
#endif
ساكن صحيح check_prevcol(char_u *linep, صحيح col, صحيح ch, صحيح *prevcol);
ساكن صحيح find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
ساكن فراغ find_mps_values(صحيح *initc, صحيح *findc, صحيح *backwards, صحيح switchit);
ساكن صحيح is_zero_width(char_u *pattern, size_t patternlen, صحيح move, pos_T *cur, صحيح direction);
ساكن فراغ cmdline_search_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, صحيح show_top_bot_msg, char_u *msgbuf, size_t msgbuflen, صحيح recompute, صحيح maxcount, long timeout);
ساكن فراغ update_search_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, صحيح recompute, صحيح maxcount, long timeout);

#define SEARCH_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#define SEARCH_STAT_BUF_LEN 16

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
ساكن spat_T spats[2] =
{
    {عدم, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {عدم, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ساكن صحيح last_idx = 0;	// index in spats[] for RE_LAST

ساكن char_u lastc[2] = {NUL, NUL};	// last character searched for
ساكن صحيح lastcdir = FORWARD;		// last direction of character search
ساكن صحيح last_t_cmd = TRUE;		// last search t_cmd
ساكن char_u	lastc_bytes[MB_MAXBYTES + 1];
ساكن صحيح	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ساكن spat_T	    saved_spats[ARRAY_LENGTH(spats)];
ساكن char_u	    *saved_mr_pattern = عدم;
ساكن size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_SEARCH_EXTRA
ساكن صحيح	    saved_spats_last_idx = 0;
ساكن صحيح	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
ساكن char_u	    *mr_pattern = عدم;
ساكن size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
تعريفنوع بنية SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    صحيح		matched;	// Found a match in this file
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
    صحيح
search_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    صحيح		pat_save,
    صحيح		pat_use,
    صحيح		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    صحيح		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    אם (pat == عدم || *pat == NUL)
    {
	صحيح i;

	אם (pat_use == RE_LAST)
	    i = last_idx;
	אחרת
	    i = pat_use;
	אם (spats[i].pat == عدم)	// pattern was never defined
	{
	    אם (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    אחרת
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    החזר FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    אחרת אם (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, patlen, TRUE, NUL);

    אם (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    אם (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    אחרת
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    אם (mr_pattern == عدم)
	mr_patternlen = 0;
    אחרת
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    אם (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	אם (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, patlen, magic);
	// substitute or global command
	אם (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    אם (regmatch->regprog == عدم)
	החזר FAIL;
    החזר OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_search_pat(فراغ)
{
    החזר mr_pattern;
}

    فراغ
save_re_pat(صحيح idx, char_u *pat, size_t patlen, صحيح magic)
{
    אם (spats[idx].pat == pat)
	החזר;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    אם (spats[idx].pat == عدم)
	spats[idx].patlen = 0;
    אחרת
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    אם (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ساكن صحيح save_level = 0;

    فراغ
save_search_patterns(فراغ)
{
    صحيح i;

    אם (save_level++ != 0)
	החזר;

    עבור (i = 0; i < (صحيح)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	אם (spats[i].pat != عدم)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    אם (saved_spats[i].pat == عدم)
		saved_spats[i].patlen = 0;
	    אחרת
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    אם (mr_pattern == عدم)
	saved_mr_pattern = عدم;
    אחרת
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    אם (saved_mr_pattern == عدم)
	saved_mr_patternlen = 0;
    אחרת
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    فراغ
restore_search_patterns(فراغ)
{
    صحيح i;

    אם (--save_level != 0)
	החזר;

    עבור (i = 0; i < (صحيح)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#אם defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    vim_free(mr_pattern);
    mr_pattern = saved_mr_pattern;
    mr_patternlen = saved_mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    last_idx = saved_spats_last_idx;
    set_no_hlsearch(saved_spats_no_hlsearch);
#endif
}

#אם defined(EXITFREE)
    فراغ
free_search_patterns(فراغ)
{
    صحيح i;

    עבור (i = 0; i < (صحيح)ARRAY_LENGTH(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_pattern);
    mr_patternlen = 0;
}
#endif

#ifdef FEAT_SEARCH_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ساكن spat_T	    saved_last_search_spat;
ساكن صحيح	    did_save_last_search_spat = 0;
ساكن صحيح	    saved_last_idx = 0;
ساكن صحيح	    saved_no_hlsearch = 0;
ساكن صحيح	    saved_search_match_endcol;
ساكن صحيح	    saved_search_match_lines;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    فراغ
save_last_search_pattern(فراغ)
{
    אם (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	החזר;

    saved_last_search_spat = spats[RE_SEARCH];
    אם (spats[RE_SEARCH].pat != عدم)
    {
	saved_last_search_spat.pat = vim_strnsave(spats[RE_SEARCH].pat, spats[RE_SEARCH].patlen);
	אם (saved_last_search_spat.pat == عدم)
	    saved_last_search_spat.patlen = 0;
	אחרת
	    saved_last_search_spat.patlen = spats[RE_SEARCH].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    فراغ
restore_last_search_pattern(فراغ)
{
    אם (--did_save_last_search_spat > 0)
	// nested call, nothing to do
	החזר;
    אם (did_save_last_search_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	החזר;
    }

    vim_free(spats[RE_SEARCH].pat);
    spats[RE_SEARCH] = saved_last_search_spat;
    saved_last_search_spat.pat = عدم;
    saved_last_search_spat.patlen = 0;
# אם defined(FEAT_EVAL)
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
    ساكن فراغ
save_incsearch_state(فراغ)
{
    saved_search_match_endcol = search_match_endcol;
    saved_search_match_lines  = search_match_lines;
}

    ساكن فراغ
restore_incsearch_state(فراغ)
{
    search_match_endcol = saved_search_match_endcol;
    search_match_lines  = saved_search_match_lines;
}

    char_u *
last_search_pattern(فراغ)
{
    החזר spats[RE_SEARCH].pat;
}

    size_t
last_search_pattern_len(فراغ)
{
    החזר spats[RE_SEARCH].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    صحيح
ignorecase(char_u *pat)
{
    החזר ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    صحيح
ignorecase_opt(char_u *pat, صحيح ic_in, صحيح scs)
{
    صحيح		ic = ic_in;

    אם (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    החזר ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    صحيح
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (فراغ)skip_regexp_ex(pat, NUL, magic_isset(), عدم, عدم, &magic_val);

    כלעוד (*p != NUL)
    {
	صحيح		l;

	אם (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    אם (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		החזר TRUE;
	    p += l;
	}
	אחרת אם (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    אם (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    אחרת אם (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    אחרת אם (p[1] != NUL)  // skip "\X"
		p += 2;
	    אחרת
		p += 1;
	}
	אחרת אם ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    אם (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    אחרת
		p++;
	}
	אחרת אם (MB_ISUPPER(*p))
	    החזר TRUE;
	אחרת
	    ++p;
    }
    החזר FALSE;
}

#אם defined(FEAT_EVAL)
    char_u *
last_csearch(فراغ)
{
    החזר lastc_bytes;
}

    صحيح
last_csearch_forward(فراغ)
{
    החזר lastcdir == FORWARD;
}

    صحيح
last_csearch_until(فراغ)
{
    החזר last_t_cmd == TRUE;
}

    فراغ
set_last_csearch(صحيح c, char_u *s, صحيح len)
{
    *lastc = c;
    lastc_bytelen = len;
    אם (len)
	memcpy(lastc_bytes, s, len);
    אחרת
	CLEAR_FIELD(lastc_bytes);
}
#endif

    فراغ
set_csearch_direction(صحيح cdir)
{
    lastcdir = cdir;
}

    فراغ
set_csearch_until(صحيح t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_search_pat(فراغ)
{
    החזר spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    فراغ
reset_search_dir(فراغ)
{
    spats[0].off.dir = '/';
#אם defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#אם defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    فراغ
set_last_search_pat(
    char_u	*s,
    صحيح		idx,
    صحيح		magic,
    صحيح		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    אם (*s == NUL)
	spats[idx].pat = عدم;
    אחרת
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    אם (spats[idx].pat == عدم)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# אם defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    אם (setlast)
	last_idx = idx;
    אם (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	אם (spats[idx].pat == عدم)
	    saved_spats[idx].pat = عدم;
	אחרת
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	אם (saved_spats[idx].pat == عدم)
	    saved_spats[idx].patlen = 0;
	אחרת
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    אם (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#endif

#ifdef FEAT_SEARCH_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    فراغ
last_pat_prog(regmmatch_T *regmatch)
{
    אם (spats[last_idx].pat == عدم)
    {
	regmatch->regprog = عدم;
	החזר;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (فراغ)search_regcomp((char_u *)"", 0, عدم, 0, last_idx, SEARCH_KEEP, regmatch);
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
    صحيح
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    صحيح		dir,
    char_u	*pat,
    size_t	patlen,
    long	count,
    صحيح		options,
    صحيح		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    صحيح		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    صحيح		loop;
    pos_T	start_pos;
    صحيح		at_first_line;
    صحيح		extra_col;
    صحيح		start_char_len;
    صحيح		match_ok;
    long	nmatched;
    صحيح		submatch = 0;
    صحيح		first_match = TRUE;
    صحيح		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    صحيح		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    صحيح		unused_timeout_flag = FALSE;
    صحيح		*timed_out = &unused_timeout_flag;  // set when timed out.
    صحيح		search_from_match_end;		    // vi-compatible search?

    אם (search_regcomp(pat, patlen, عدم, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	אם ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	החזר FAIL;
    }

    search_from_match_end = vim_strchr(p_cpo, CPO_SEARCH) != عدم;

    אם (extra_arg != عدم)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	אם (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    עשה	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	אם (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	אחרת אם (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    אם (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    אחרת
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	אחרת
	    start_char_len = 1;
	אם (dir == FORWARD)
	{
	    אם (options & SEARCH_START)
		extra_col = 0;
	    אחרת
		extra_col = start_char_len;
	}
	אחרת
	{
	    אם (options & SEARCH_START)
		extra_col = start_char_len;
	    אחרת
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	אם (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_line = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	אם (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	אחרת
	    lnum = pos->lnum;

	עבור (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    עבור ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		אם (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    اكسر;
		// Stop after passing the time limit.
		אם (*timed_out)
		    اكسر;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		אם (regmatch.regprog == عدم)
		    اكسر;
		// Abort searching on an error (e.g., out of stack).
		אם (called_emsg > called_emsg_before || *timed_out)
		    اكسر;
		אם (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    אם (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    אחרת
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    אם (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			כלעוד (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (صحيح)endpos.col - 1
					     < (صحيح)start_pos.col + extra_col)
				    : ((صحيح)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (صحيح)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    אם (search_from_match_end)
			    {
				אם (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    اكسر;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				אם (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    אם (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    אחרת
					++matchcol;
				}
			    }
			    אחרת
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				אם (ptr[matchcol] != NUL)
				{
				    אם (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    אחרת
					++matchcol;
				}
			    }
			    אם (matchcol == 0 && (options & SEARCH_START))
				اكسر;
			    אם (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				اكسر;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    אם (regmatch.regprog == عدم)
				اكسر;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			אם (!match_ok)
			    استمر;
		    }
		    אם (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			עבור (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    אם (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (صحيح)regmatch.endpos[0].col - 1
							< (صحيح)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (صحيح)regmatch.startpos[0].col
						      < (صحيح)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    אחרת
				اكسر;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    אם (search_from_match_end)
			    {
				אם (nmatched > 1)
				    اكسر;
				matchcol = endpos.col;
				// for empty match: advance one char
				אם (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    אם (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    אחרת
					++matchcol;
				}
			    }
			    אחרת
			    {
				// Stop when the match is in a next line.
				אם (matchpos.lnum > 0)
				    اكسر;
				matchcol = matchpos.col;
				אם (ptr[matchcol] != NUL)
				{
				    אם (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    אחרת
					++matchcol;
				}
			    }
			    אם (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				אם (*timed_out)
				    match_ok = FALSE;
				اكسر;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    אם (regmatch.regprog == عدم)
				اكسر;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			אם (!match_ok)
			    استمر;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    אם ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			אם (endpos.col == 0)
			{
			    אם (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			אחרת
			{
			    --pos->col;
			    אם (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			אם (end_pos != عدم)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    אחרת
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			אם (end_pos != عدم)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    אם (end_pos != عدم)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    اكسر;
		}
		line_breakcheck();	// stop if ctrl-C typed
		אם (got_int)
		    اكسر;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		אם ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    اكسر;
		}
#endif

		אם (loop && lnum == start_pos.lnum)
		    اكسر;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    אם (regmatch.regprog == عدم)
		اكسر;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    אם (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
			      || break_loop
#endif
			      || found || loop)
		اكسر;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    אם (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    אחרת
		lnum = 1;
	    אם (!shortmess(SHM_SEARCH)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    אם (extra_arg != عدم)
		extra_arg->sa_wrapped = TRUE;
	}
	אם (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    اكسر;
    }
    כלעוד (--count > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    אם (extra_arg != عدم && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    אם (!found)		    // did not find it
    {
	אם (got_int)
	    emsg(_(e_interrupted));
	אחרת אם ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    אם (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    אחרת אם (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    אחרת
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	החזר FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    אם (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	אם (pos->col > 0)
	    --pos->col;
    }

    החזר submatch + 1;
}

#אם defined(FEAT_EVAL) || defined(FEAT_PROTO)
    فراغ
set_search_direction(صحيح cdir)
{
    spats[0].off.dir = cdir;
}

    ساكن فراغ
set_vv_searchforward(فراغ)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ساكن صحيح
first_submatch(regmmatch_T *rp)
{
    صحيح		submatch;

    עבור (submatch = 1; ; ++submatch)
    {
	אם (rp->startpos[submatch].lnum >= 0)
	    اكسر;
	אם (submatch == 9)
	{
	    submatch = 0;
	    اكسر;
	}
    }
    החזר submatch;
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
    صحيح
parse_search_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    صحيح		search_delim,
    صحيح		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    صحيح		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    אם (*pat == عدم || **pat == NUL)
	החזר 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = عدم;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, search_delim, magic_isset(),
						    strcopy, عدم, عدم);
    אם (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (صحيح)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    אם (*p == search_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    offset->line = FALSE;
    offset->end = FALSE;
    offset->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    אם (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->line = TRUE;
    אחרת אם ((options & SEARCH_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	אם (*p == 'e')		// end
	    offset->end = SEARCH_END;
	++p;
    }
    אם (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	אם (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((حرف *)p);
	אחרת אם (*p == '-')	    // single '-'
	    offset->off = -1;
	אחרת			    // single '+'
	    offset->off = 1;
	++p;
	כלעוד (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (صحيح)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    החזר cmdlen;
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
    صحيح
do_search(
    oparg_T	    *oap,	// can be NULL
    صحيح		    dirc,	// '/' or '?'
    صحيح		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    count,
    صحيح		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    صحيح		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = عدم;
    صحيح		    show_search_stats;
    char_u	    *msgbuf = عدم;
    size_t	    msgbuflen = 0;
    صحيح		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    אם (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != عدم)
    {
	spats[0].off.line = FALSE;
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
    אם (dirc == 0)
	dirc = spats[0].off.dir;
    אחרת
    {
	spats[0].off.dir = dirc;
#אם defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    אם (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#אחרת
	אם (dirc == '/')
	    dirc = '?';
	אחרת
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    אם (dirc == '/')
    {
	אם (hasFolding(pos.lnum, عدم, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    אחרת
    {
	אם (hasFolding(pos.lnum, &pos.lnum, عدم))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    אם (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    עבור (;;)
    {
	صحيح		show_top_bot_msg = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = عدم;
					    // use previous pattern
	אם (pat == عدم || *pat == NUL || *pat == search_delim)
	{
	    אם (spats[RE_SEARCH].pat == عدم)	    // no previous pattern
	    {
		אם (spats[RE_SUBST].pat == عدم)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    اذهب end_do_search;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    אחרת
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	אם (pat != عدم && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_search_pattern_offset(&pat, &patlen,
				search_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    אם (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_search_stats = FALSE;
	אם ((options & SEARCH_ECHO) && messaging()
		&& !msg_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    msg_start();

	    // Get the offset, so we know how long it is.
	    אם (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		אם (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		אחרת אם (!spats[0].off.line)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		אם (spats[0].off.off != 0 || spats[0].off.line)
		    off_len += vim_snprintf((حرف *)off_buf + off_len,
			   حجم(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    אם (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    אחרת
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    אם (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		אם (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (صحيح)(Rows - msg_row) * cmdline_width - 1;
		אחרת
		    // Use up to 'showcmd' column.
		    msgbufsize = (صحيح)(Rows - msg_row - 1) * cmdline_width
			+ sc_col - 1;
		אם (msgbufsize < plen + off_len + SEARCH_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    אחרת
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    אם (msgbuf == عدم)
	    {
		msgbuflen = 0;
	    }
	    אחרת
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		אם (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    אם (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    אחרת
			mch_memmove(msgbuf + 1, p, plen);
		    אם (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    אם (trunc != عدم)
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
		    אם (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			אם (r != عدم)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    כלעוד (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    אם ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    אחרת
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    msg_outtrans(msgbuf);
		    msg_clr_eos();
		    msg_check();

		    gotocmdline(FALSE);
		    out_flush();
		    msg_nowait = TRUE;	    // don't wait for this message
		}

		אם (!shortmess(SHM_SEARCHCOUNT))
		    show_search_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	אם (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    אם (spats[0].off.off > 0)
	    {
		עבור (c = spats[0].off.off; c; --c)
		    אם (decl(&pos) == -1)
			اكسر;
		אם (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    אחרת
	    {
		עבור (c = spats[0].off.off; c; ++c)
		    אם (incl(&pos) == -1)
			اكسر;
		אם (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_line_count + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, عدم,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, count, spats[0].off.end + (options &
		       (SEARCH_KEEP + SEARCH_PEEK + SEARCH_HIS
			+ SEARCH_MSG + SEARCH_START
			+ ((pat != عدم && *pat == ';') ? 0 : SEARCH_NOOF))),
		RE_LAST, sia);

	אם (dircp != عدم)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	אם (!shortmess(SHM_SEARCH) && sia && sia->sa_wrapped)
	    show_top_bot_msg = TRUE;

	אם (c == FAIL)
	{
	    retval = 0;
	    اذهب end_do_search;
	}
	אם (spats[0].off.end && oap != عدم)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	אם (!(options & SEARCH_NOOF) || (pat != عدم && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    אם (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		אם (c < 1)
		    pos.lnum = 1;
		אחרת אם (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		אחרת
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    אחרת אם (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		אם (c > 0)
		{
		    כלעוד (c-- > 0)
			אם (incl(&pos) == -1)
			    اكسر;
		}
		// to the left, check for start of file
		אחרת
		{
		    כלעוד (c++ < 0)
			אם (decl(&pos) == -1)
			    اكسر;
		}
	    }
	    אם (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	אם (show_search_stats)
	     cmdline_search_stat(dirc, &pos, &curwin->w_cursor,
				show_top_bot_msg, msgbuf, msgbuflen,
				(count != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_SEARCH)
				     && hasFolding(curwin->w_cursor.lnum,
								   عدم, عدم))
#endif
				),
				p_msc,
				SEARCH_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	אם (!(options & SEARCH_OPT) || pat == عدم || *pat != ';')
	    اكسر;

	dirc = *++pat;
	--patlen;
	search_delim = dirc;
	אם (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    اذهب end_do_search;
	}
	++pat;
	--patlen;
    }

    אם (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_search:
    אם ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    החזר retval;
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
    صحيح
search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    صحيح		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    صحيح		compl_len = ins_compl_len();

    אם (buf->b_ml.ml_line_count == 0)
	החזר FAIL;
    עבור (;;)
    {
	pos->lnum += dir;
	אם (pos->lnum < 1)
	{
	    אם (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		אם (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    אחרת
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	אחרת אם (pos->lnum > buf->b_ml.ml_line_count)
	{
	    אם (p_ws)
	    {
		pos->lnum = 1;
		אם (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    אחרת
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	אם (pos->lnum == start)
	    اكسر;
	אם (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	אם (compl_status_adding() && !compl_status_sol())
	{
	    אם ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		החזר OK;
	}
	אחרת אם (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    אם ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		החזר OK;
	}
    }
    החזר FAIL;
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
    صحيح
searchc(cmdarg_T *cap, صحيح t_cmd)
{
    صحيح			c = cap->nchar;	// char to search for
    صحيح			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    صحيح			col;
    char_u		*p;
    صحيح			len;
    صحيح			stop = TRUE;

    אם (c != NUL)	// normal search: remember args for repeat
    {
	אם (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    אם (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		אם (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    אחרת		// repeat previous search
    {
	אם (*lastc == NUL && lastc_bytelen <= 1)
	    החזר FAIL;
	אם (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	אחרת
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	אם (vim_strchr(p_cpo, CPO_SCOLON) == عدم && count == 1 && t_cmd)
	    stop = FALSE;
    }

    אם (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    אחרת
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    כלעוד (count--)
    {
	אם (has_mbyte)
	{
	    עבור (;;)
	    {
		אם (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    אם (col >= len)
			החזר FAIL;
		}
		אחרת
		{
		    אם (col == 0)
			החזר FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		אם (lastc_bytelen <= 1)
		{
		    אם (p[col] == c && stop)
			اكسر;
		}
		אחרת אם (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    اكسر;
		stop = TRUE;
	    }
	}
	אחרת
	{
	    עבור (;;)
	    {
		אם ((col += dir) < 0 || col >= len)
		    החזר FAIL;
		אם (p[col] == c && stop)
		    اكسر;
		stop = TRUE;
	    }
	}
    }

    אם (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	אם (has_mbyte)
	{
	    אם (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    אחרת
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    החזר OK;
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
findmatch(oparg_T *oap, صحيح initc)
{
    החזר findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ساكن صحيح
check_prevcol(
    char_u	*linep,
    صحيح		col,
    صحيح		ch,
    صحيح		*prevcol)
{
    --col;
    אם (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    אם (prevcol)
	*prevcol = col;
    החזר (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ساكن صحيح
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    صحيح		found = FALSE;

    עבור (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    אם (delim_copy == عدم)
	החזר FALSE;
    עבור (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *line = ml_get(lnum);

	עבור (p = line + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    אם (lnum == endpos->lnum && (colnr_T)(p - line) >= endpos->col)
		اكسر;
	    אם (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		اكسر;
	    }
	}
	אם (found)
	    اكسر;
    }
    vim_free(delim_copy);
    החזר found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ساكن فراغ
find_mps_values(
    صحيح	    *initc,
    صحيح	    *findc,
    صحيح	    *backwards,
    صحيح	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    כלעוד (*ptr != NUL)
    {
	אם (has_mbyte)
	{
	    char_u *prev;

	    אם (mb_ptr2char(ptr) == *initc)
	    {
		אם (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		אחרת
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		החזר;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    אם (mb_ptr2char(ptr) == *initc)
	    {
		אם (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		אחרת
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		החזר;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	אחרת
	{
	    אם (*ptr == *initc)
	    {
		אם (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		אחרת
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		החזר;
	    }
	    ptr += 2;
	    אם (*ptr == *initc)
	    {
		אם (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		אחרת
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		החזר;
	    }
	    ++ptr;
	}
	אם (*ptr == ',')
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
    صحيح		initc,
    صحيح		flags,
    صحيح		maxtravel)
{
    ساكن pos_T pos;			// current search position
    صحيح		findc = 0;		// matching brace
    صحيح		c;
    صحيح		count = 0;		// cumulative number of braces
    صحيح		backwards = FALSE;	// init for gcc
    صحيح		raw_string = FALSE;	// search for raw string
    صحيح		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    صحيح		do_quotes;		// check for quotes in current line
    صحيح		at_start;		// do_quotes value at start position
    صحيح		hash_dir = 0;		// Direction searched for # things
    صحيح		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    صحيح		start_in_quotes;	// start position is in quotes
    صحيح		traveled = 0;		// how far we've searched so far
    صحيح		ignore_cend = FALSE;    // ignore comment end
    صحيح		cpo_match;		// vi compatible matching
    صحيح		cpo_bsl;		// don't recognize backslashes
    صحيح		match_escaped = 0;	// search for escaped match
    صحيح		dir;			// Direction to search
    صحيح		comment_col = MAXCOL;   // start of / / comment
    صحيح		lispcomm = FALSE;	// inside of Lisp-style comment
    صحيح		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    صحيح		skip_comments = (flags & FM_SKIPCOMM) != 0;
    صحيح		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != عدم);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != عدم);

    // Direction to search when initc is '/', '*' or '#'
    אם (flags & FM_BACKWARD)
	dir = BACKWARD;
    אחרת אם (flags & FM_FORWARD)
	dir = FORWARD;
    אחרת
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    אם (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	אם (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    אחרת אם (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	אם (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	אם (findc == NUL)
	    החזר عدم;
    }
    אחרת
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	אם (initc == '#')
	{
	    hash_dir = dir;
	}
	אחרת
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    אם (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		אם (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    אם (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		אחרת אם (linep[pos.col] == '/')
		{
		    אם (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    אחרת אם (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		אחרת אם (linep[pos.col] == '*')
		{
		    אם (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    אחרת אם (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    אם (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		אם (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		עבור (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    אם (initc == NUL)
			اكسر;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    אם (findc)
			اكسر;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		אם (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    אם (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    אחרת
			החזר عدم;
		}
		אחרת אם (!cpo_bsl)
		{
		    صحيح col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    עבור (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	אם (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    אם (oap != عدم)
		oap->motion_type = MLINE;   // Linewise for this case only
	    אם (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		אם (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		אחרת אם (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		אחרת
		    החזר عدم;
	    }
	    pos.col = 0;
	    כלעוד (!got_int)
	    {
		אם (hash_dir > 0)
		{
		    אם (pos.lnum == curbuf->b_ml.ml_line_count)
			اكسر;
		}
		אחרת אם (pos.lnum == 1)
		    اكسر;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		אם (*ptr != '#')
		    استمر;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		אם (hash_dir > 0)
		{
		    אם (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    אחרת אם (STRNCMP(ptr, "el", 2) == 0)
		    {
			אם (count == 0)
			    החזר &pos;
		    }
		    אחרת אם (STRNCMP(ptr, "endif", 5) == 0)
		    {
			אם (count == 0)
			    החזר &pos;
			count--;
		    }
		}
		אחרת
		{
		    אם (STRNCMP(ptr, "if", 2) == 0)
		    {
			אם (count == 0)
			    החזר &pos;
			count--;
		    }
		    אחרת אם (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			אם (count == 0)
			    החזר &pos;
		    }
		    אחרת אם (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    החזר عدم;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    אם (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != عدم)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    אם ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    אם (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    אם (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    כלעוד (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	אם (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    אם (lispcomm && pos.col < (colnr_T)comment_col)
		اكسر;
	    אם (pos.col == 0)		// at start of line, go to prev. one
	    {
		אם (pos.lnum == 1)	// start of file
		    اكسر;
		--pos.lnum;

		אם (maxtravel > 0 && ++traveled > maxtravel)
		    اكسر;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		אם (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		אם (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		אחרת אם (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    אחרת
	    {
		--pos.col;
		אם (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	אחרת				// forward search
	{
	    אם (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		אם (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    اكسر;
		++pos.lnum;

		אם (maxtravel && traveled++ > maxtravel)
		    اكسر;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		אם (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    אחרת
	    {
		אם (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		אחרת
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	אם (skip_comments && !comment_dir && !inquote)
	{
	    אם (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		אם (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (صحيح)pos.col < comment_col))
		    in_block_comment = TRUE;
		אחרת אם (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    אחרת
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		אם (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (صحيح)pos.col < comment_col))
		    in_block_comment = TRUE;
		אחרת אם (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	אם (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    אם (linep[0] == findc && count == 0)	// match!
		החזר &pos;
	    اكسر;					// out of scope
	}

	אם (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    אם (comment_dir == FORWARD)
	    {
		אם (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    החזר &pos;
		}
	    }
	    אחרת    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		אם (pos.col == 0)
		    استمر;
		אחרת אם (raw_string)
		{
		    אם (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != عدم)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			אם (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		אחרת אם (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (صحيح)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		אחרת אם (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    אם (count > 0)
			pos = match_pos;
		    אחרת אם (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (صحيح)pos.col <= comment_col)
			pos.col -= 2;
		    אחרת אם (ignore_cend)
			استمر;
		    אחרת
			החזר عدم;
		    החזר &pos;
		}
	    }
	    استمر;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	אם (cpo_match)
	    do_quotes = 0;
	אחרת אם (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    עבור (ptr = linep; *ptr; ++ptr)
	    {
		אם (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		אם (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		אם (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    אם (!do_quotes)
	    {
		inquote = FALSE;
		אם (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    אם (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			אם (inquote)
			    start_in_quotes = TRUE;
		    }
		    אחרת אם (backwards)
			inquote = TRUE;
		}
		אם (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    אם (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			אם (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    אם (inquote)
				start_in_quotes = TRUE;
			}
			אחרת אם (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	אם (start_in_quotes == MAYBE)
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
	بدل (c)
	{
	حالة NUL:
	    // at end of line without trailing backslash, reset inquote
	    אם (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    اكسر;

	حالة '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    אם (do_quotes)
	    {
		صحيح col;

		עבור (col = pos.col - 1; col >= 0; --col)
		    אם (linep[col] != '\\')
			اكسر;
		אם ((((صحيح)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    اكسر;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	حالة '\'':
	    אם (!cpo_match && initc != '\'' && findc != '\'')
	    {
		אם (backwards)
		{
		    אם (pos.col > 1)
		    {
			אם (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    اكسر;
			}
			אחרת אם (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    اكسر;
			}
		    }
		}
		אחרת אם (linep[pos.col + 1])	// forward search
		{
		    אם (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			اكسر;
		    }
		    אחרת אם (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			اكسر;
		    }
		}
	    }
	    // FALLTHROUGH

	افتراضي:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    אם (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != عدم
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', عدم)
		    && check_prevcol(linep, pos.col - 1, '#', عدم))
		اكسر;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    אם (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (صحيح)pos.col >= comment_col)))
		اكسر;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    אם ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		صحيح	col, bslcnt = 0;

		אם (!cpo_bsl)
		{
		    עבור (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		אם (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    אם (c == initc)
			count++;
		    אחרת
		    {
			אם (count == 0)
			    החזר &pos;
			count--;
		    }
		}
	    }
	}
    }

    אם (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	החזר &pos;
    }
    החזר (pos_T *)عدم;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    فراغ
showmatch(
    صحيح		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    صحيح		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    עבור (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	אם (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    اكسر;
#endif
	p += mb_ptr2len(p) + 1;
	אם (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    اكسر;
	p += mb_ptr2len(p);
	אם (*p == NUL)
	    החזר;
    }
    אם (*p == NUL)
	החזר;

    אם ((lpos = findmatch(عدم, NUL)) == عدم)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	החזר;
    }

    אם (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	החזר;

    אם (!curwin->w_p_wrap)
	getvcol(curwin, lpos, عدم, &vcol, عدم, 0);

    صحيح col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    אם (!col_visible)
	החזר;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    אם (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    אם (vim_strchr(p_cpo, CPO_SHOWMATCH) != عدم)
	ui_delay(p_mat * 100L + 8, TRUE);
    אחרת אם (!char_avail())
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
    ساكن صحيح
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    صحيح		move,
    pos_T	*cur,
    صحيح		direction)
{
    regmmatch_T	regmatch;
    صحيح		nmatched = 0;
    صحيح		result = -1;
    pos_T	pos;
    صحيح		called_emsg_before = called_emsg;
    صحيح		flag = 0;

    אם (pattern == عدم)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    אם (search_regcomp(pattern, patternlen, عدم, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	החזר -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    אם (move)
    {
	CLEAR_POS(&pos);
    }
    אחרת
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    אם (searchit(curwin, curbuf, &pos, عدم, direction, pattern, patternlen, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, عدم) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	עשה
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, عدم);
	    אם (nmatched != 0)
		اكسر;
	} כלעוד (regmatch.regprog != عدم
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	אם (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    החזר result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحيح
current_search(
    long	count,
    صحيح		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    صحيح		i;
    صحيح		dir;
    صحيح		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    صحيح		flags = 0;
    pos_T	save_VIsual = VIsual;
    صحيح		zero_width;
    صحيح		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    אם (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    אם (VIsual_active)
    {
	אם (forward)
	    incl(&pos);
	אחרת
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    אם (zero_width == -1)
	החזר FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    עבור (i = 0; i < 2; i++)
    {
	אם (forward)
	{
	    אם (i == 0 && skip_first_backward)
		استمر;
	    dir = i;
	}
	אחרת
	    dir = !i;

	flags = 0;
	אם (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	אם (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? count : 1),
		SEARCH_KEEP | flags, RE_SEARCH, عدم);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	אם (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    אם (VIsual_active)
		VIsual = save_VIsual;
	    החזר FAIL;
	}
	אחרת אם (i == 0 && !result)
	{
	    אם (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    אחרת
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_line_count);
	    }
	}
    }

    start_pos = pos;

    אם (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    אם (LT_POS(VIsual, end_pos) && forward)
    {
	אם (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	אחרת
	    // put the cursor on last character of match
	    dec_cursor();
    }
    אחרת אם (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    אם (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	אם (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	אחרת אם (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    אם (fdo_flags & FDO_SEARCH && KeyTyped)
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

    החזר OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    صحيح
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    החזר (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ساكن فراغ
cmdline_search_stat(
    صحيح		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    صحيح		show_top_bot_msg,
    char_u	*msgbuf,
    size_t	msgbuflen,
    صحيح		recompute,
    صحيح		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    אם (stat.cur <= 0)
	החזר;

    حرف	t[SEARCH_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    אם (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	אם (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	אחרת אם (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	אחרת אם (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	אחרת
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    אחרת
#endif
    {
	אם (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	אחרת אם (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	אחרת אם (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	אחרת
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    אם (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    אם (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    אם (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    msg_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    msg_hist_off = FALSE;
}

/*
 * Add the search count information to "stat".
 * "stat" must not be NULL.
 * When "recompute" is TRUE always recompute the numbers.
 * dirc == 0: don't find the next/previous match (only set the result to "stat")
 * dirc == '/': find the next match
 * dirc == '?': find the previous match
 */
    ساكن فراغ
update_search_stat(
    صحيح			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    صحيح			recompute,
    صحيح			maxcount,
    long		timeout UNUSED)
{
    صحيح		    save_ws = p_ws;
    صحيح		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ساكن pos_T    lastpos = {0, 0, 0};
    ساكن صحيح	    cur = 0;
    ساكن صحيح	    cnt = 0;
    ساكن صحيح	    exact_match = FALSE;
    ساكن صحيح	    incomplete = 0;
    ساكن صحيح	    last_maxcount = 0;
    ساكن صحيح	    chgtick = 0;
    ساكن char_u   *lastpat = عدم;
    ساكن size_t   lastpatlen = 0;
    ساكن buf_T    *lbuf = عدم;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    אם (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	החזר;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    אם (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != عدم
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
    אם (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    אחרת
    {
	صحيح	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	אם (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	כלעוד (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, عدم, 0, 1, SEARCH_KEEP, RE_LAST, عدم) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    אם (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		اكسر;
	    }
#endif
	    cnt++;
	    אם (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		אם (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    אם (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		اكسر;
	    }
	}
	אם (got_int)
	    cur = -1; // abort
	אם (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    אם (lastpat == عدم)
		lastpatlen = 0;
	    אחרת
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

#אם defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ساكن char_u *
get_line_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *line = ml_get(lnum);

    vim_strncpy(buf, line, LSIZE - 1);
    החזר buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    فراغ
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    صحيح		dir UNUSED,	// direction of expansion
    صحيح		len,		// length of search pattern
    صحيح		whole,		// match whole words only
    صحيح		skip_comments,	// don't match inside comments
    صحيح		type,		// Type of search; are we looking for a type?
				// a macro?
    long	count,
    صحيح		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    صحيح		forceit,	// If true, always switch to the found path
    صحيح		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    صحيح		max_path_depth = 50;
    long	match_count = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = عدم;
    linenr_T	lnum;
    صحيح		depth;
    صحيح		depth_displayed;	// For type==CHECK_PATH
    صحيح		old_files;
    صحيح		already_searched;
    char_u	*file_line;
    char_u	*line;
    char_u	*p;
    char_u	save_char;
    صحيح		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    صحيح		matched = FALSE;
    صحيح		did_show = FALSE;
    صحيح		found = FALSE;
    صحيح		i;
    char_u	*already = عدم;
    char_u	*startp = عدم;
    char_u	*inc_opt = عدم;
# אם defined(FEAT_QUICKFIX)
    win_T	*curwin_save = عدم;
# endif

    regmatch.regprog = عدم;
    incl_regmatch.regprog = عدم;
    def_regmatch.regprog = عدم;

    file_line = alloc(LSIZE);
    אם (file_line == عدم)
	החזר;

    אם (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	אם (pat == عدم)
	    اذهب fpip_end;
	vim_snprintf((حرف *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	אם (regmatch.regprog == عدم)
	    اذهب fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    אם (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	אם (incl_regmatch.regprog == عدم)
	    اذهب fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    אם (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	אם (def_regmatch.regprog == عدم)
	    اذهب fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * حجم(SearchedFile), TRUE);
    אם (files == عدم)
	اذهب fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    אם (end_lnum > curbuf->b_ml.ml_line_count)
	end_lnum = curbuf->b_ml.ml_line_count;
    אם (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    line = get_line_and_copy(lnum, file_line);

    עבור (;;)
    {
	אם (incl_regmatch.regprog != عدم
		&& vim_regexec(&incl_regmatch, line, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    אם (strstr((حرف *)inc_opt, "\\zs") != عدم)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (صحيح)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    אחרת
		// Use text after match with 'include'.
		new_fname = file_name_in_line(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, عدم);
	    already_searched = FALSE;
	    אם (new_fname != عدم)
	    {
		// Check whether we have already searched in this file
		עבור (i = 0;; i++)
		{
		    אם (i == depth + 1)
			i = old_files;
		    אם (i == max_path_depth)
			اكسر;
		    אם (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			אם (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    msg_putchar('\n');	    // cursor below last one
			    אם (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				msg_home_replace_hl(new_fname);
				msg_puts(_(" (includes previously listed match)"));
				prev_fname = عدم;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			اكسر;
		    }
		}
	    }

	    אם (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == عدم && !already_searched)))
	    {
		אם (did_show)
		    msg_putchar('\n');	    // cursor below last one
		אחרת
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    msg_puts_title(_("--- Included files "));
		    אם (action != ACTION_SHOW_ALL)
			msg_puts_title(_("not found "));
		    msg_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		כלעוד (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    עבור (i = 0; i < depth_displayed; i++)
			msg_puts("  ");
		    msg_home_replace(files[depth_displayed].name);
		    msg_puts(" -->\n");
		}
		אם (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    עבור (i = 0; i <= depth_displayed; i++)
			msg_puts("  ");
		    אם (new_fname != عدم)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			msg_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    אחרת
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			אם (strstr((حرف *)inc_opt, "\\zs") != عدم)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (صحيح)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			אחרת
			{
			    // find the file name after the end of the match
			    עבור (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    עבור (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			אם (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (صحيح)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			אחרת אם (p > line)
			{
			    אם (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    אם (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			msg_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    אם (new_fname == عدم && action == ACTION_SHOW_ALL)
		    {
			אם (already_searched)
			    msg_puts(_("  (Already listed)"));
			אחרת
			    msg_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    אם (new_fname != عدم)
	    {
		// Push the new file onto the file stack
		אם (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    אם (bigger != عدم)
		    {
			עבור (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			עבור (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = عدم;
			    bigger[i].name = عدم;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			עבור (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		אם ((files[depth + 1].fp = mch_fopen((حرف *)new_fname, "r"))
								    == عدم)
		    vim_free(new_fname);
		אחרת
		{
		    אם (++depth == old_files)
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
		    אם (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			msg_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((حرف*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(حرف *)new_fname);
			msg_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    אחרת אם (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (حرف *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	אחרת
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = line;
search_line:
	    define_matched = FALSE;
	    אם (def_regmatch.regprog != عدم
			      && vim_regexec(&def_regmatch, line, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		כלעוד (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    אם (def_regmatch.regprog == عدم || define_matched)
	    {
		אם (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    אם (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    אחרת
			matched = STRNCMP(startp, ptr, len) == 0;
		    אם (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		אחרת אם (regmatch.regprog != عدم
			 && vim_regexec(&regmatch, line, (colnr_T)(p - line)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    אם (!define_matched && skip_comments)
		    {
			אם ((*line != '#' ||
				STRNCMP(skipwhite(line + 1), "define", 6) != 0)
				&& get_leader_len(line, عدم, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(line);
			אם (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    עבור (p = line; *p && p < startp; ++p)
			    {
				אם (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    אם (p[1] == '/')
					اكسر;
				    ++p;
				}
				אחרת אם (!matched && p[0] == '*' && p[1] == '/')
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
	אם (matched)
	{
	    אם (action == ACTION_EXPAND)
	    {
		صحيح	cont_s_ipos = FALSE;
		صحيح	add_r;
		char_u	*aux;

		אם (depth == -1 && lnum == curwin->w_cursor.lnum)
		    اكسر;
		found = TRUE;
		aux = p = startp;
		אם (compl_status_adding() && (صحيح)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    אם (vim_iswordp(p))
			اذهب exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (صحيح)(p - aux);

		אם (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    אם (depth < 0)
		    {
			אם (lnum >= end_lnum)
			    اذهب exit_matched;
			line = get_line_and_copy(++lnum, file_line);
		    }
		    אחרת אם (vim_fgets(line = file_line,
						      LSIZE, files[depth].fp))
			اذهب exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(line);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    אם (p > aux)
		    {
			אם (*aux != ')' && IObuff[i-1] != TAB)
			{
			    אם (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    אם (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == عدم
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			אם (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (صحيح)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    אם (i == ins_compl_len())
			اذهب exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? عدم
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		אם (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		אחרת אם (add_r == FAIL)
		    اكسر;
	    }
	    אחרת אם (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		אם (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		אם (curr_fname != prev_fname)
		{
		    אם (did_show)
			msg_putchar('\n');	// cursor below last one
		    אם (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			msg_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		אם (!got_int)
		    show_pat_in_path(line, type, TRUE, action,
			    (depth == -1) ? عدم : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_count++);

		// Set matched flag for this file and all the ones that
		// include it
		עבור (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    אחרת אם (--count <= 0)
	    {
		found = TRUE;
		אם (depth == -1 && lnum == curwin->w_cursor.lnum
# אם defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_line));
		אחרת אם (action == ACTION_SHOW)
		{
		    show_pat_in_path(line, type, did_show, action,
			    (depth == -1) ? عدم : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		אחרת
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# אם defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    אם (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    אם (action == ACTION_SPLIT)
		    {
			אם (win_split(0, 0) == FAIL)
			    اكسر;
			RESET_BINDING(curwin);
		    }
		    אם (depth == -1)
		    {
			// match in current file
# אם defined(FEAT_QUICKFIX)
			אם (g_do_tagpreview != 0)
			{
			    אם (!win_valid(curwin_save))
				اكسر;
			    אם (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, عدم,
						     عدم, TRUE, lnum, forceit)))
				اكسر;	// failed to jump to file
			}
			אחרת
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    אחרת
		    {
			אם (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, عدم, TRUE,
						    files[depth].lnum, forceit)))
			    اكسر;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		אם (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - line);
		    curwin->w_set_curswant = true;
		}

# אם defined(FEAT_QUICKFIX)
		אם (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		אחרת אם (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		اكسر;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    אם (def_regmatch.regprog == عدم
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		اذهب search_line;
	}
	line_breakcheck();
	אם (action == ACTION_EXPAND)
	    ins_compl_check_keys(30, FALSE);
	אם (got_int || ins_compl_interrupted())
	    اكسر;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	כלעוד (depth >= 0 && !already
		&& vim_fgets(line = file_line, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    אם (depth < depth_displayed)
		depth_displayed = depth;
	}
	אם (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (صحيح)STRLEN(line);
	    אם (i > 0 && line[i - 1] == '\n')
		line[--i] = NUL;
	    אם (i > 0 && line[i - 1] == '\r')
		line[--i] = NUL;
	}
	אחרת אם (!already)
	{
	    אם (++lnum > end_lnum)
		اكسر;
	    line = get_line_and_copy(lnum, file_line);
	}
	already = عدم;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    עבור (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    עבור (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    אם (type == CHECK_PATH)
    {
	אם (!did_show)
	{
	    אם (action != ACTION_SHOW_ALL)
		msg(_("All included files were found"));
	    אחרת
		msg(_("No included files"));
	}
    }
    אחרת אם (!found && action != ACTION_EXPAND && !silent)
    {
	אם (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	אחרת אם (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	אחרת
	    emsg(_(e_couldnt_find_pattern));
    }
    אם (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	msg_end();

fpip_end:
    vim_free(file_line);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ساكن فراغ
show_pat_in_path(
    char_u  *line,
    صحيح	    type,
    صحيح	    did_show,
    صحيح	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;
    size_t  linelen;

    אם (did_show)
	msg_putchar('\n');	// cursor below last one
    אחרת אם (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    אם (got_int)		// 'q' typed at "--more--" message
	החזר;
    linelen = STRLEN(line);
    עבור (;;)
    {
	p = line + linelen - 1;
	אם (fp != عدم)
	{
	    // We used fgets(), so get rid of newline at end
	    אם (p >= line && *p == '\n')
		--p;
	    אם (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	אם (action == ACTION_SHOW_ALL)
	{
	    sprintf((حرف *)IObuff, "%3ld: ", count);	// show match nr
	    msg_puts((حرف *)IObuff);
	    sprintf((حرف *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    msg_puts_attr((حرف *)IObuff, HL_ATTR(HLF_N));
	    msg_puts(" ");
	}
	msg_prt_line(line, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	אם (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    اكسر;

	אם (fp != عدم)
	{
	    אם (vim_fgets(line, LSIZE, fp)) // end of file
		اكسر;
	    linelen = STRLEN(line);
	    ++*lnum;
	}
	אחרת
	{
	    אם (++*lnum > curbuf->b_ml.ml_line_count)
		اكسر;
	    line = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	msg_putchar('\n');
    }
}
#endif

#ifdef FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(صحيح idx)
{
    החזר &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    صحيح
get_spat_last_idx(فراغ)
{
    החזר last_idx;
}
#endif

#אם defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    فراغ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = عدم;
    صحيح			maxcount = p_msc;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    صحيح			recompute = TRUE;
    searchstat_T	stat;

    אם (rettv_dict_alloc(rettv) == FAIL)
	החזר;

    אם (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	החזר;

    אם (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    אם (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	صحيح		error = FALSE;

	אם (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    החזר;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	אם (di != عدم)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    אם (error)
		החזר;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	אם (di != عدم)
	{
	    maxcount = (صحيح)tv_get_number_chk(&di->di_tv, &error);
	    אם (error)
		החזר;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	אם (di != عدم)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    אם (pattern == عدم)
		החזר;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	אם (di != عدم)
	{
	    אם (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		החזר;
	    }
	    אם (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		החזר;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    אם (li != عدم)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		אם (error)
		    החזר;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    אם (li != عدم)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		אם (error)
		    החזר;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    אם (li != عدم)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		אם (error)
		    החזר;
	    }
	}
    }

    save_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
# endif
    אם (pattern != عدم)
    {
	אם (*pattern == NUL)
	    اذهب the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	אם (spats[last_idx].pat == عدم)
	    spats[last_idx].patlen = 0;
    }
    אם (spats[last_idx].pat == عدم || *spats[last_idx].pat == NUL)
	اذهب the_end;	// the previous pattern was never defined

    update_search_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_number(rettv->vval.v_dict, "current", stat.cur);
    dict_add_number(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_number(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_number(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_number(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    restore_incsearch_state();
# endif
}
#endif
