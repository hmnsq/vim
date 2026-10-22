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

#include "vim.h"

#ifdef FEAT_EVAL
static void set_vv_searchforward(void);
static 整数 first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
static char_u *get_line_and_copy(linenr_T lnum, char_u *buf);
static void show_pat_in_path(char_u *, 整数, 整数, 整数, FILE *, linenr_T *, long);
#endif

typedef struct searchstat
{
    整数	    cur;	    // current position of found words
    整数	    cnt;	    // total count of found words
    整数	    exact_match;    // TRUE if matched exactly on specified position
    整数	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    整数	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_SEARCH_EXTRA
static void save_incsearch_state(void);
static void restore_incsearch_state(void);
#endif
static 整数 check_prevcol(char_u *linep, 整数 col, 整数 ch, 整数 *prevcol);
static 整数 find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
static void find_mps_values(整数 *initc, 整数 *findc, 整数 *backwards, 整数 switchit);
static 整数 is_zero_width(char_u *pattern, size_t patternlen, 整数 move, pos_T *cur, 整数 direction);
static void cmdline_search_stat(整数 dirc, pos_T *pos, pos_T *cursor_pos, 整数 show_top_bot_msg, char_u *msgbuf, size_t msgbuflen, 整数 recompute, 整数 maxcount, long timeout);
static void update_search_stat(整数 dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, 整数 recompute, 整数 maxcount, long timeout);

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
static spat_T spats[2] =
{
    {NULL, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {NULL, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

static 整数 last_idx = 0;	// index in spats[] for RE_LAST

static char_u lastc[2] = {NUL, NUL};	// last character searched for
static 整数 lastcdir = FORWARD;		// last direction of character search
static 整数 last_t_cmd = TRUE;		// last search t_cmd
static char_u	lastc_bytes[MB_MAXBYTES + 1];
static 整数	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
static spat_T	    saved_spats[ARRAY_LENGTH(spats)];
static char_u	    *saved_mr_pattern = NULL;
static size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_SEARCH_EXTRA
static 整数	    saved_spats_last_idx = 0;
static 整数	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
static char_u	    *mr_pattern = NULL;
static size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
typedef struct SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    整数		matched;	// Found a match in this file
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
    整数
search_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    整数		pat_save,
    整数		pat_use,
    整数		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    整数		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    もし (pat == NULL || *pat == NUL)
    {
	整数 i;

	もし (pat_use == RE_LAST)
	    i = last_idx;
	否则
	    i = pat_use;
	もし (spats[i].pat == NULL)	// pattern was never defined
	{
	    もし (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    否则
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    戻る FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    否则 もし (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, patlen, TRUE, NUL);

    もし (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    もし (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    否则
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    もし (mr_pattern == NULL)
	mr_patternlen = 0;
    否则
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    もし (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	もし (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, patlen, magic);
	// substitute or global command
	もし (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    もし (regmatch->regprog == NULL)
	戻る FAIL;
    戻る OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_search_pat(void)
{
    戻る mr_pattern;
}

    void
save_re_pat(整数 idx, char_u *pat, size_t patlen, 整数 magic)
{
    もし (spats[idx].pat == pat)
	戻る;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    もし (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    否则
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    もし (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
static 整数 save_level = 0;

    void
save_search_patterns(void)
{
    整数 i;

    もし (save_level++ != 0)
	戻る;

    ために (i = 0; i < (整数)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	もし (spats[i].pat != NULL)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    もし (saved_spats[i].pat == NULL)
		saved_spats[i].patlen = 0;
	    否则
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    もし (mr_pattern == NULL)
	saved_mr_pattern = NULL;
    否则
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    もし (saved_mr_pattern == NULL)
	saved_mr_patternlen = 0;
    否则
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    void
restore_search_patterns(void)
{
    整数 i;

    もし (--save_level != 0)
	戻る;

    ために (i = 0; i < (整数)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#もし defined(FEAT_EVAL)
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

#もし defined(EXITFREE)
    void
free_search_patterns(void)
{
    整数 i;

    ために (i = 0; i < (整数)ARRAY_LENGTH(spats); ++i)
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
static spat_T	    saved_last_search_spat;
static 整数	    did_save_last_search_spat = 0;
static 整数	    saved_last_idx = 0;
static 整数	    saved_no_hlsearch = 0;
static 整数	    saved_search_match_endcol;
static 整数	    saved_search_match_lines;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    void
save_last_search_pattern(void)
{
    もし (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	戻る;

    saved_last_search_spat = spats[RE_SEARCH];
    もし (spats[RE_SEARCH].pat != NULL)
    {
	saved_last_search_spat.pat = vim_strnsave(spats[RE_SEARCH].pat, spats[RE_SEARCH].patlen);
	もし (saved_last_search_spat.pat == NULL)
	    saved_last_search_spat.patlen = 0;
	否则
	    saved_last_search_spat.patlen = spats[RE_SEARCH].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    void
restore_last_search_pattern(void)
{
    もし (--did_save_last_search_spat > 0)
	// nested call, nothing to do
	戻る;
    もし (did_save_last_search_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	戻る;
    }

    vim_free(spats[RE_SEARCH].pat);
    spats[RE_SEARCH] = saved_last_search_spat;
    saved_last_search_spat.pat = NULL;
    saved_last_search_spat.patlen = 0;
# もし defined(FEAT_EVAL)
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
    static void
save_incsearch_state(void)
{
    saved_search_match_endcol = search_match_endcol;
    saved_search_match_lines  = search_match_lines;
}

    static void
restore_incsearch_state(void)
{
    search_match_endcol = saved_search_match_endcol;
    search_match_lines  = saved_search_match_lines;
}

    char_u *
last_search_pattern(void)
{
    戻る spats[RE_SEARCH].pat;
}

    size_t
last_search_pattern_len(void)
{
    戻る spats[RE_SEARCH].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    整数
ignorecase(char_u *pat)
{
    戻る ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    整数
ignorecase_opt(char_u *pat, 整数 ic_in, 整数 scs)
{
    整数		ic = ic_in;

    もし (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    戻る ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    整数
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (void)skip_regexp_ex(pat, NUL, magic_isset(), NULL, NULL, &magic_val);

    ながら (*p != NUL)
    {
	整数		l;

	もし (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    もし (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		戻る TRUE;
	    p += l;
	}
	否则 もし (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    もし (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    否则 もし (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    否则 もし (p[1] != NUL)  // skip "\X"
		p += 2;
	    否则
		p += 1;
	}
	否则 もし ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    もし (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    否则
		p++;
	}
	否则 もし (MB_ISUPPER(*p))
	    戻る TRUE;
	否则
	    ++p;
    }
    戻る FALSE;
}

#もし defined(FEAT_EVAL)
    char_u *
last_csearch(void)
{
    戻る lastc_bytes;
}

    整数
last_csearch_forward(void)
{
    戻る lastcdir == FORWARD;
}

    整数
last_csearch_until(void)
{
    戻る last_t_cmd == TRUE;
}

    void
set_last_csearch(整数 c, char_u *s, 整数 len)
{
    *lastc = c;
    lastc_bytelen = len;
    もし (len)
	memcpy(lastc_bytes, s, len);
    否则
	CLEAR_FIELD(lastc_bytes);
}
#endif

    void
set_csearch_direction(整数 cdir)
{
    lastcdir = cdir;
}

    void
set_csearch_until(整数 t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_search_pat(void)
{
    戻る spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    void
reset_search_dir(void)
{
    spats[0].off.dir = '/';
#もし defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#もし defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    void
set_last_search_pat(
    char_u	*s,
    整数		idx,
    整数		magic,
    整数		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    もし (*s == NUL)
	spats[idx].pat = NULL;
    否则
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    もし (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# もし defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    もし (setlast)
	last_idx = idx;
    もし (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	もし (spats[idx].pat == NULL)
	    saved_spats[idx].pat = NULL;
	否则
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	もし (saved_spats[idx].pat == NULL)
	    saved_spats[idx].patlen = 0;
	否则
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    もし (p_hls && idx == last_idx && !no_hlsearch)
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
    void
last_pat_prog(regmmatch_T *regmatch)
{
    もし (spats[last_idx].pat == NULL)
    {
	regmatch->regprog = NULL;
	戻る;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (void)search_regcomp((char_u *)"", 0, NULL, 0, last_idx, SEARCH_KEEP, regmatch);
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
    整数
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    整数		dir,
    char_u	*pat,
    size_t	patlen,
    long	count,
    整数		options,
    整数		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    整数		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    整数		loop;
    pos_T	start_pos;
    整数		at_first_line;
    整数		extra_col;
    整数		start_char_len;
    整数		match_ok;
    long	nmatched;
    整数		submatch = 0;
    整数		first_match = TRUE;
    整数		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    整数		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    整数		unused_timeout_flag = FALSE;
    整数		*timed_out = &unused_timeout_flag;  // set when timed out.
    整数		search_from_match_end;		    // vi-compatible search?

    もし (search_regcomp(pat, patlen, NULL, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	もし ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	戻る FAIL;
    }

    search_from_match_end = vim_strchr(p_cpo, CPO_SEARCH) != NULL;

    もし (extra_arg != NULL)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	もし (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    /*
     * find the string
     */
    do	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	もし (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	否则 もし (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    もし (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    否则
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	否则
	    start_char_len = 1;
	もし (dir == FORWARD)
	{
	    もし (options & SEARCH_START)
		extra_col = 0;
	    否则
		extra_col = start_char_len;
	}
	否则
	{
	    もし (options & SEARCH_START)
		extra_col = start_char_len;
	    否则
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	もし (pos->lnum == 0)	// correct lnum for when starting in line 0
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
	もし (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	否则
	    lnum = pos->lnum;

	ために (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    ために ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		もし (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    break;
		// Stop after passing the time limit.
		もし (*timed_out)
		    break;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		もし (regmatch.regprog == NULL)
		    break;
		// Abort searching on an error (e.g., out of stack).
		もし (called_emsg > called_emsg_before || *timed_out)
		    break;
		もし (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    もし (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    否则
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    もし (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ながら (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (整数)endpos.col - 1
					     < (整数)start_pos.col + extra_col)
				    : ((整数)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (整数)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    もし (search_from_match_end)
			    {
				もし (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    break;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				もし (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    もし (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    否则
					++matchcol;
				}
			    }
			    否则
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				もし (ptr[matchcol] != NUL)
				{
				    もし (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    否则
					++matchcol;
				}
			    }
			    もし (matchcol == 0 && (options & SEARCH_START))
				break;
			    もし (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    もし (regmatch.regprog == NULL)
				break;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			もし (!match_ok)
			    continue;
		    }
		    もし (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			ために (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    もし (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (整数)regmatch.endpos[0].col - 1
							< (整数)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (整数)regmatch.startpos[0].col
						      < (整数)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    否则
				break;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    もし (search_from_match_end)
			    {
				もし (nmatched > 1)
				    break;
				matchcol = endpos.col;
				// for empty match: advance one char
				もし (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    もし (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    否则
					++matchcol;
				}
			    }
			    否则
			    {
				// Stop when the match is in a next line.
				もし (matchpos.lnum > 0)
				    break;
				matchcol = matchpos.col;
				もし (ptr[matchcol] != NUL)
				{
				    もし (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    否则
					++matchcol;
				}
			    }
			    もし (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				もし (*timed_out)
				    match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    もし (regmatch.regprog == NULL)
				break;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			もし (!match_ok)
			    continue;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    もし ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			もし (endpos.col == 0)
			{
			    もし (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			否则
			{
			    --pos->col;
			    もし (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			もし (end_pos != NULL)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    否则
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			もし (end_pos != NULL)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    もし (end_pos != NULL)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    break;
		}
		line_breakcheck();	// stop if ctrl-C typed
		もし (got_int)
		    break;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		もし ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    break;
		}
#endif

		もし (loop && lnum == start_pos.lnum)
		    break;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    もし (regmatch.regprog == NULL)
		break;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    もし (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
			      || break_loop
#endif
			      || found || loop)
		break;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    もし (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    否则
		lnum = 1;
	    もし (!shortmess(SHM_SEARCH)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    もし (extra_arg != NULL)
		extra_arg->sa_wrapped = TRUE;
	}
	もし (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    break;
    }
    ながら (--count > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    もし (extra_arg != NULL && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    もし (!found)		    // did not find it
    {
	もし (got_int)
	    emsg(_(e_interrupted));
	否则 もし ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    もし (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    否则 もし (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    否则
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	戻る FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    もし (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	もし (pos->col > 0)
	    --pos->col;
    }

    戻る submatch + 1;
}

#もし defined(FEAT_EVAL) || defined(FEAT_PROTO)
    void
set_search_direction(整数 cdir)
{
    spats[0].off.dir = cdir;
}

    static void
set_vv_searchforward(void)
{
    set_vim_var_nr(VV_SEARCHFORWARD, (long)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    static 整数
first_submatch(regmmatch_T *rp)
{
    整数		submatch;

    ために (submatch = 1; ; ++submatch)
    {
	もし (rp->startpos[submatch].lnum >= 0)
	    break;
	もし (submatch == 9)
	{
	    submatch = 0;
	    break;
	}
    }
    戻る submatch;
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
    整数
parse_search_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    整数		search_delim,
    整数		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    整数		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    もし (*pat == NULL || **pat == NUL)
	戻る 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = NULL;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, search_delim, magic_isset(),
						    strcopy, NULL, NULL);
    もし (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (整数)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    もし (*p == search_delim)
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
    もし (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->line = TRUE;
    否则 もし ((options & SEARCH_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	もし (*p == 'e')		// end
	    offset->end = SEARCH_END;
	++p;
    }
    もし (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	もし (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((char *)p);
	否则 もし (*p == '-')	    // single '-'
	    offset->off = -1;
	否则			    // single '+'
	    offset->off = 1;
	++p;
	ながら (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (整数)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    戻る cmdlen;
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
    整数
do_search(
    oparg_T	    *oap,	// can be NULL
    整数		    dirc,	// '/' or '?'
    整数		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    count,
    整数		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    整数		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = NULL;
    整数		    show_search_stats;
    char_u	    *msgbuf = NULL;
    size_t	    msgbuflen = 0;
    整数		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    もし (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != NULL)
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
    もし (dirc == 0)
	dirc = spats[0].off.dir;
    否则
    {
	spats[0].off.dir = dirc;
#もし defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    もし (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#否则
	もし (dirc == '/')
	    dirc = '?';
	否则
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    もし (dirc == '/')
    {
	もし (hasFolding(pos.lnum, NULL, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    否则
    {
	もし (hasFolding(pos.lnum, &pos.lnum, NULL))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    もし (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    ために (;;)
    {
	整数		show_top_bot_msg = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = NULL;
					    // use previous pattern
	もし (pat == NULL || *pat == NUL || *pat == search_delim)
	{
	    もし (spats[RE_SEARCH].pat == NULL)	    // no previous pattern
	    {
		もし (spats[RE_SUBST].pat == NULL)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    goto end_do_search;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    否则
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	もし (pat != NULL && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_search_pattern_offset(&pat, &patlen,
				search_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    もし (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_search_stats = FALSE;
	もし ((options & SEARCH_ECHO) && messaging()
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
	    もし (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		もし (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		否则 もし (!spats[0].off.line)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		もし (spats[0].off.off != 0 || spats[0].off.line)
		    off_len += vim_snprintf((char *)off_buf + off_len,
			   sizeof(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    もし (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    否则
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    もし (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		もし (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (整数)(Rows - msg_row) * cmdline_width - 1;
		否则
		    // Use up to 'showcmd' column.
		    msgbufsize = (整数)(Rows - msg_row - 1) * cmdline_width
			+ sc_col - 1;
		もし (msgbufsize < plen + off_len + SEARCH_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    否则
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    もし (msgbuf == NULL)
	    {
		msgbuflen = 0;
	    }
	    否则
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		もし (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    もし (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    否则
			mch_memmove(msgbuf + 1, p, plen);
		    もし (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    もし (trunc != NULL)
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
		    もし (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			もし (r != NULL)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ながら (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    もし ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    否则
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

		もし (!shortmess(SHM_SEARCHCOUNT))
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
	もし (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    もし (spats[0].off.off > 0)
	    {
		ために (c = spats[0].off.off; c; --c)
		    もし (decl(&pos) == -1)
			break;
		もし (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    否则
	    {
		ために (c = spats[0].off.off; c; ++c)
		    もし (incl(&pos) == -1)
			break;
		もし (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_line_count + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, NULL,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, count, spats[0].off.end + (options &
		       (SEARCH_KEEP + SEARCH_PEEK + SEARCH_HIS
			+ SEARCH_MSG + SEARCH_START
			+ ((pat != NULL && *pat == ';') ? 0 : SEARCH_NOOF))),
		RE_LAST, sia);

	もし (dircp != NULL)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	もし (!shortmess(SHM_SEARCH) && sia && sia->sa_wrapped)
	    show_top_bot_msg = TRUE;

	もし (c == FAIL)
	{
	    retval = 0;
	    goto end_do_search;
	}
	もし (spats[0].off.end && oap != NULL)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	もし (!(options & SEARCH_NOOF) || (pat != NULL && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    もし (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		もし (c < 1)
		    pos.lnum = 1;
		否则 もし (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		否则
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    否则 もし (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		もし (c > 0)
		{
		    ながら (c-- > 0)
			もし (incl(&pos) == -1)
			    break;
		}
		// to the left, check for start of file
		否则
		{
		    ながら (c++ < 0)
			もし (decl(&pos) == -1)
			    break;
		}
	    }
	    もし (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	もし (show_search_stats)
	     cmdline_search_stat(dirc, &pos, &curwin->w_cursor,
				show_top_bot_msg, msgbuf, msgbuflen,
				(count != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_SEARCH)
				     && hasFolding(curwin->w_cursor.lnum,
								   NULL, NULL))
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
	もし (!(options & SEARCH_OPT) || pat == NULL || *pat != ';')
	    break;

	dirc = *++pat;
	--patlen;
	search_delim = dirc;
	もし (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    goto end_do_search;
	}
	++pat;
	--patlen;
    }

    もし (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_search:
    もし ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    戻る retval;
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
    整数
search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    整数		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    整数		compl_len = ins_compl_len();

    もし (buf->b_ml.ml_line_count == 0)
	戻る FAIL;
    ために (;;)
    {
	pos->lnum += dir;
	もし (pos->lnum < 1)
	{
	    もし (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		もし (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    否则
	    {
		pos->lnum = 1;
		break;
	    }
	}
	否则 もし (pos->lnum > buf->b_ml.ml_line_count)
	{
	    もし (p_ws)
	    {
		pos->lnum = 1;
		もし (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    否则
	    {
		pos->lnum = 1;
		break;
	    }
	}
	もし (pos->lnum == start)
	    break;
	もし (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	もし (compl_status_adding() && !compl_status_sol())
	{
	    もし ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		戻る OK;
	}
	否则 もし (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    もし ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		戻る OK;
	}
    }
    戻る FAIL;
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
    整数
searchc(cmdarg_T *cap, 整数 t_cmd)
{
    整数			c = cap->nchar;	// char to search for
    整数			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    整数			col;
    char_u		*p;
    整数			len;
    整数			stop = TRUE;

    もし (c != NUL)	// normal search: remember args for repeat
    {
	もし (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    もし (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		もし (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    否则		// repeat previous search
    {
	もし (*lastc == NUL && lastc_bytelen <= 1)
	    戻る FAIL;
	もし (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	否则
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	もし (vim_strchr(p_cpo, CPO_SCOLON) == NULL && count == 1 && t_cmd)
	    stop = FALSE;
    }

    もし (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    否则
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    ながら (count--)
    {
	もし (has_mbyte)
	{
	    ために (;;)
	    {
		もし (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    もし (col >= len)
			戻る FAIL;
		}
		否则
		{
		    もし (col == 0)
			戻る FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		もし (lastc_bytelen <= 1)
		{
		    もし (p[col] == c && stop)
			break;
		}
		否则 もし (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    break;
		stop = TRUE;
	    }
	}
	否则
	{
	    ために (;;)
	    {
		もし ((col += dir) < 0 || col >= len)
		    戻る FAIL;
		もし (p[col] == c && stop)
		    break;
		stop = TRUE;
	    }
	}
    }

    もし (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	もし (has_mbyte)
	{
	    もし (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    否则
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    戻る OK;
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
findmatch(oparg_T *oap, 整数 initc)
{
    戻る findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    static 整数
check_prevcol(
    char_u	*linep,
    整数		col,
    整数		ch,
    整数		*prevcol)
{
    --col;
    もし (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    もし (prevcol)
	*prevcol = col;
    戻る (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    static 整数
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    整数		found = FALSE;

    ために (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    もし (delim_copy == NULL)
	戻る FALSE;
    ために (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *line = ml_get(lnum);

	ために (p = line + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    もし (lnum == endpos->lnum && (colnr_T)(p - line) >= endpos->col)
		break;
	    もし (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		break;
	    }
	}
	もし (found)
	    break;
    }
    vim_free(delim_copy);
    戻る found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    static void
find_mps_values(
    整数	    *initc,
    整数	    *findc,
    整数	    *backwards,
    整数	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ながら (*ptr != NUL)
    {
	もし (has_mbyte)
	{
	    char_u *prev;

	    もし (mb_ptr2char(ptr) == *initc)
	    {
		もし (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		否则
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		戻る;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    もし (mb_ptr2char(ptr) == *initc)
	    {
		もし (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		否则
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		戻る;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	否则
	{
	    もし (*ptr == *initc)
	    {
		もし (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		否则
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		戻る;
	    }
	    ptr += 2;
	    もし (*ptr == *initc)
	    {
		もし (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		否则
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		戻る;
	    }
	    ++ptr;
	}
	もし (*ptr == ',')
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
    整数		initc,
    整数		flags,
    整数		maxtravel)
{
    static pos_T pos;			// current search position
    整数		findc = 0;		// matching brace
    整数		c;
    整数		count = 0;		// cumulative number of braces
    整数		backwards = FALSE;	// init for gcc
    整数		raw_string = FALSE;	// search for raw string
    整数		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    整数		do_quotes;		// check for quotes in current line
    整数		at_start;		// do_quotes value at start position
    整数		hash_dir = 0;		// Direction searched for # things
    整数		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    整数		start_in_quotes;	// start position is in quotes
    整数		traveled = 0;		// how far we've searched so far
    整数		ignore_cend = FALSE;    // ignore comment end
    整数		cpo_match;		// vi compatible matching
    整数		cpo_bsl;		// don't recognize backslashes
    整数		match_escaped = 0;	// search for escaped match
    整数		dir;			// Direction to search
    整数		comment_col = MAXCOL;   // start of / / comment
    整数		lispcomm = FALSE;	// inside of Lisp-style comment
    整数		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    整数		skip_comments = (flags & FM_SKIPCOMM) != 0;
    整数		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != NULL);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != NULL);

    // Direction to search when initc is '/', '*' or '#'
    もし (flags & FM_BACKWARD)
	dir = BACKWARD;
    否则 もし (flags & FM_FORWARD)
	dir = FORWARD;
    否则
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    もし (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	もし (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    否则 もし (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	もし (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	もし (findc == NUL)
	    戻る NULL;
    }
    否则
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	もし (initc == '#')
	{
	    hash_dir = dir;
	}
	否则
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    もし (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		もし (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    もし (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		否则 もし (linep[pos.col] == '/')
		{
		    もし (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    否则 もし (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		否则 もし (linep[pos.col] == '*')
		{
		    もし (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    否则 もし (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    もし (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		もし (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		ために (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    もし (initc == NUL)
			break;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    もし (findc)
			break;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		もし (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    もし (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    否则
			戻る NULL;
		}
		否则 もし (!cpo_bsl)
		{
		    整数 col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    ために (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	もし (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    もし (oap != NULL)
		oap->motion_type = MLINE;   // Linewise for this case only
	    もし (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		もし (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		否则 もし (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		否则
		    戻る NULL;
	    }
	    pos.col = 0;
	    ながら (!got_int)
	    {
		もし (hash_dir > 0)
		{
		    もし (pos.lnum == curbuf->b_ml.ml_line_count)
			break;
		}
		否则 もし (pos.lnum == 1)
		    break;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		もし (*ptr != '#')
		    continue;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		もし (hash_dir > 0)
		{
		    もし (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    否则 もし (STRNCMP(ptr, "el", 2) == 0)
		    {
			もし (count == 0)
			    戻る &pos;
		    }
		    否则 もし (STRNCMP(ptr, "endif", 5) == 0)
		    {
			もし (count == 0)
			    戻る &pos;
			count--;
		    }
		}
		否则
		{
		    もし (STRNCMP(ptr, "if", 2) == 0)
		    {
			もし (count == 0)
			    戻る &pos;
			count--;
		    }
		    否则 もし (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			もし (count == 0)
			    戻る &pos;
		    }
		    否则 もし (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    戻る NULL;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    もし (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != NULL)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    もし ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    もし (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    もし (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    ながら (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	もし (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    もし (lispcomm && pos.col < (colnr_T)comment_col)
		break;
	    もし (pos.col == 0)		// at start of line, go to prev. one
	    {
		もし (pos.lnum == 1)	// start of file
		    break;
		--pos.lnum;

		もし (maxtravel > 0 && ++traveled > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		もし (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		もし (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		否则 もし (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    否则
	    {
		--pos.col;
		もし (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	否则				// forward search
	{
	    もし (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		もし (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    break;
		++pos.lnum;

		もし (maxtravel && traveled++ > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		もし (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    否则
	    {
		もし (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		否则
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	もし (skip_comments && !comment_dir && !inquote)
	{
	    もし (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		もし (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (整数)pos.col < comment_col))
		    in_block_comment = TRUE;
		否则 もし (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    否则
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		もし (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (整数)pos.col < comment_col))
		    in_block_comment = TRUE;
		否则 もし (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	もし (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    もし (linep[0] == findc && count == 0)	// match!
		戻る &pos;
	    break;					// out of scope
	}

	もし (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    もし (comment_dir == FORWARD)
	    {
		もし (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    戻る &pos;
		}
	    }
	    否则    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		もし (pos.col == 0)
		    continue;
		否则 もし (raw_string)
		{
		    もし (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != NULL)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			もし (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		否则 もし (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (整数)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		否则 もし (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    もし (count > 0)
			pos = match_pos;
		    否则 もし (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (整数)pos.col <= comment_col)
			pos.col -= 2;
		    否则 もし (ignore_cend)
			continue;
		    否则
			戻る NULL;
		    戻る &pos;
		}
	    }
	    continue;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	もし (cpo_match)
	    do_quotes = 0;
	否则 もし (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    ために (ptr = linep; *ptr; ++ptr)
	    {
		もし (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		もし (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		もし (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    もし (!do_quotes)
	    {
		inquote = FALSE;
		もし (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    もし (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			もし (inquote)
			    start_in_quotes = TRUE;
		    }
		    否则 もし (backwards)
			inquote = TRUE;
		}
		もし (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    もし (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			もし (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    もし (inquote)
				start_in_quotes = TRUE;
			}
			否则 もし (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	もし (start_in_quotes == MAYBE)
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
	switch (c)
	{
	case NUL:
	    // at end of line without trailing backslash, reset inquote
	    もし (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    break;

	case '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    もし (do_quotes)
	    {
		整数 col;

		ために (col = pos.col - 1; col >= 0; --col)
		    もし (linep[col] != '\\')
			break;
		もし ((((整数)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    break;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	case '\'':
	    もし (!cpo_match && initc != '\'' && findc != '\'')
	    {
		もし (backwards)
		{
		    もし (pos.col > 1)
		    {
			もし (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    break;
			}
			否则 もし (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    break;
			}
		    }
		}
		否则 もし (linep[pos.col + 1])	// forward search
		{
		    もし (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			break;
		    }
		    否则 もし (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			break;
		    }
		}
	    }
	    // FALLTHROUGH

	default:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    もし (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != NULL
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', NULL)
		    && check_prevcol(linep, pos.col - 1, '#', NULL))
		break;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    もし (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (整数)pos.col >= comment_col)))
		break;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    もし ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		整数	col, bslcnt = 0;

		もし (!cpo_bsl)
		{
		    ために (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		もし (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    もし (c == initc)
			count++;
		    否则
		    {
			もし (count == 0)
			    戻る &pos;
			count--;
		    }
		}
	    }
	}
    }

    もし (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	戻る &pos;
    }
    戻る (pos_T *)NULL;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    void
showmatch(
    整数		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    整数		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    ために (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	もし (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    break;
#endif
	p += mb_ptr2len(p) + 1;
	もし (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    break;
	p += mb_ptr2len(p);
	もし (*p == NUL)
	    戻る;
    }
    もし (*p == NUL)
	戻る;

    もし ((lpos = findmatch(NULL, NUL)) == NULL)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	戻る;
    }

    もし (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	戻る;

    もし (!curwin->w_p_wrap)
	getvcol(curwin, lpos, NULL, &vcol, NULL, 0);

    整数 col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    もし (!col_visible)
	戻る;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    もし (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    もし (vim_strchr(p_cpo, CPO_SHOWMATCH) != NULL)
	ui_delay(p_mat * 100L + 8, TRUE);
    否则 もし (!char_avail())
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
    static 整数
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    整数		move,
    pos_T	*cur,
    整数		direction)
{
    regmmatch_T	regmatch;
    整数		nmatched = 0;
    整数		result = -1;
    pos_T	pos;
    整数		called_emsg_before = called_emsg;
    整数		flag = 0;

    もし (pattern == NULL)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    もし (search_regcomp(pattern, patternlen, NULL, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	戻る -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    もし (move)
    {
	CLEAR_POS(&pos);
    }
    否则
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    もし (searchit(curwin, curbuf, &pos, NULL, direction, pattern, patternlen, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, NULL) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	do
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, NULL);
	    もし (nmatched != 0)
		break;
	} ながら (regmatch.regprog != NULL
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	もし (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    戻る result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    整数
current_search(
    long	count,
    整数		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    整数		i;
    整数		dir;
    整数		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    整数		flags = 0;
    pos_T	save_VIsual = VIsual;
    整数		zero_width;
    整数		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    もし (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    もし (VIsual_active)
    {
	もし (forward)
	    incl(&pos);
	否则
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    もし (zero_width == -1)
	戻る FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    ために (i = 0; i < 2; i++)
    {
	もし (forward)
	{
	    もし (i == 0 && skip_first_backward)
		continue;
	    dir = i;
	}
	否则
	    dir = !i;

	flags = 0;
	もし (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	もし (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, spats[last_idx].patlen, (long) (i ? count : 1),
		SEARCH_KEEP | flags, RE_SEARCH, NULL);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	もし (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    もし (VIsual_active)
		VIsual = save_VIsual;
	    戻る FAIL;
	}
	否则 もし (i == 0 && !result)
	{
	    もし (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    否则
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_line_count);
	    }
	}
    }

    start_pos = pos;

    もし (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    もし (LT_POS(VIsual, end_pos) && forward)
    {
	もし (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	否则
	    // put the cursor on last character of match
	    dec_cursor();
    }
    否则 もし (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    もし (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	もし (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	否则 もし (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    もし (fdo_flags & FDO_SEARCH && KeyTyped)
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

    戻る OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    整数
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    戻る (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    static void
cmdline_search_stat(
    整数		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    整数		show_top_bot_msg,
    char_u	*msgbuf,
    size_t	msgbuflen,
    整数		recompute,
    整数		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    もし (stat.cur <= 0)
	戻る;

    char	t[SEARCH_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    もし (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	もし (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	否则 もし (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	否则 もし (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	否则
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    否则
#endif
    {
	もし (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	否则 もし (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	否则 もし (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	否则
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    もし (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    もし (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    もし (dirc == '?' && stat.cur == maxcount + 1)
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
    static void
update_search_stat(
    整数			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    整数			recompute,
    整数			maxcount,
    long		timeout UNUSED)
{
    整数		    save_ws = p_ws;
    整数		    wraparound = FALSE;
    pos_T	    p = (*pos);
    static pos_T    lastpos = {0, 0, 0};
    static 整数	    cur = 0;
    static 整数	    cnt = 0;
    static 整数	    exact_match = FALSE;
    static 整数	    incomplete = 0;
    static 整数	    last_maxcount = 0;
    static 整数	    chgtick = 0;
    static char_u   *lastpat = NULL;
    static size_t   lastpatlen = 0;
    static buf_T    *lbuf = NULL;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    もし (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	戻る;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    もし (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != NULL
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
    もし (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    否则
    {
	整数	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	もし (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	ながら (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, NULL, 0, 1, SEARCH_KEEP, RE_LAST, NULL) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    もし (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		break;
	    }
#endif
	    cnt++;
	    もし (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		もし (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    もし (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		break;
	    }
	}
	もし (got_int)
	    cur = -1; // abort
	もし (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    もし (lastpat == NULL)
		lastpatlen = 0;
	    否则
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

#もし defined(FEAT_FIND_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    static char_u *
get_line_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *line = ml_get(lnum);

    vim_strncpy(buf, line, LSIZE - 1);
    戻る buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    void
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    整数		dir UNUSED,	// direction of expansion
    整数		len,		// length of search pattern
    整数		whole,		// match whole words only
    整数		skip_comments,	// don't match inside comments
    整数		type,		// Type of search; are we looking for a type?
				// a macro?
    long	count,
    整数		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    整数		forceit,	// If true, always switch to the found path
    整数		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    整数		max_path_depth = 50;
    long	match_count = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = NULL;
    linenr_T	lnum;
    整数		depth;
    整数		depth_displayed;	// For type==CHECK_PATH
    整数		old_files;
    整数		already_searched;
    char_u	*file_line;
    char_u	*line;
    char_u	*p;
    char_u	save_char;
    整数		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    整数		matched = FALSE;
    整数		did_show = FALSE;
    整数		found = FALSE;
    整数		i;
    char_u	*already = NULL;
    char_u	*startp = NULL;
    char_u	*inc_opt = NULL;
# もし defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
# endif

    regmatch.regprog = NULL;
    incl_regmatch.regprog = NULL;
    def_regmatch.regprog = NULL;

    file_line = alloc(LSIZE);
    もし (file_line == NULL)
	戻る;

    もし (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	もし (pat == NULL)
	    goto fpip_end;
	vim_snprintf((char *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	もし (regmatch.regprog == NULL)
	    goto fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    もし (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	もし (incl_regmatch.regprog == NULL)
	    goto fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    もし (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	もし (def_regmatch.regprog == NULL)
	    goto fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * sizeof(SearchedFile), TRUE);
    もし (files == NULL)
	goto fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    もし (end_lnum > curbuf->b_ml.ml_line_count)
	end_lnum = curbuf->b_ml.ml_line_count;
    もし (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    line = get_line_and_copy(lnum, file_line);

    ために (;;)
    {
	もし (incl_regmatch.regprog != NULL
		&& vim_regexec(&incl_regmatch, line, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    もし (strstr((char *)inc_opt, "\\zs") != NULL)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (整数)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    否则
		// Use text after match with 'include'.
		new_fname = file_name_in_line(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, NULL);
	    already_searched = FALSE;
	    もし (new_fname != NULL)
	    {
		// Check whether we have already searched in this file
		ために (i = 0;; i++)
		{
		    もし (i == depth + 1)
			i = old_files;
		    もし (i == max_path_depth)
			break;
		    もし (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			もし (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    msg_putchar('\n');	    // cursor below last one
			    もし (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    // message
			    {
				msg_home_replace_hl(new_fname);
				msg_puts(_(" (includes previously listed match)"));
				prev_fname = NULL;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			break;
		    }
		}
	    }

	    もし (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == NULL && !already_searched)))
	    {
		もし (did_show)
		    msg_putchar('\n');	    // cursor below last one
		否则
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    msg_puts_title(_("--- Included files "));
		    もし (action != ACTION_SHOW_ALL)
			msg_puts_title(_("not found "));
		    msg_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ながら (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    ために (i = 0; i < depth_displayed; i++)
			msg_puts("  ");
		    msg_home_replace(files[depth_displayed].name);
		    msg_puts(" -->\n");
		}
		もし (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    ために (i = 0; i <= depth_displayed; i++)
			msg_puts("  ");
		    もし (new_fname != NULL)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			msg_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    否则
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			もし (strstr((char *)inc_opt, "\\zs") != NULL)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (整数)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			否则
			{
			    // find the file name after the end of the match
			    ために (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    ために (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			もし (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (整数)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			否则 もし (p > line)
			{
			    もし (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    もし (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			msg_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    もし (new_fname == NULL && action == ACTION_SHOW_ALL)
		    {
			もし (already_searched)
			    msg_puts(_("  (Already listed)"));
			否则
			    msg_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    もし (new_fname != NULL)
	    {
		// Push the new file onto the file stack
		もし (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    もし (bigger != NULL)
		    {
			ために (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			ために (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = NULL;
			    bigger[i].name = NULL;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			ために (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		もし ((files[depth + 1].fp = mch_fopen((char *)new_fname, "r"))
								    == NULL)
		    vim_free(new_fname);
		否则
		{
		    もし (++depth == old_files)
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
		    もし (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			msg_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((char*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(char *)new_fname);
			msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    否则 もし (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (char *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	否则
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = line;
search_line:
	    define_matched = FALSE;
	    もし (def_regmatch.regprog != NULL
			      && vim_regexec(&def_regmatch, line, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ながら (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    もし (def_regmatch.regprog == NULL || define_matched)
	    {
		もし (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    もし (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    否则
			matched = STRNCMP(startp, ptr, len) == 0;
		    もし (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		否则 もし (regmatch.regprog != NULL
			 && vim_regexec(&regmatch, line, (colnr_T)(p - line)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    もし (!define_matched && skip_comments)
		    {
			もし ((*line != '#' ||
				STRNCMP(skipwhite(line + 1), "define", 6) != 0)
				&& get_leader_len(line, NULL, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(line);
			もし (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    ために (p = line; *p && p < startp; ++p)
			    {
				もし (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    もし (p[1] == '/')
					break;
				    ++p;
				}
				否则 もし (!matched && p[0] == '*' && p[1] == '/')
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
	もし (matched)
	{
	    もし (action == ACTION_EXPAND)
	    {
		整数	cont_s_ipos = FALSE;
		整数	add_r;
		char_u	*aux;

		もし (depth == -1 && lnum == curwin->w_cursor.lnum)
		    break;
		found = TRUE;
		aux = p = startp;
		もし (compl_status_adding() && (整数)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    もし (vim_iswordp(p))
			goto exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (整数)(p - aux);

		もし (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    もし (depth < 0)
		    {
			もし (lnum >= end_lnum)
			    goto exit_matched;
			line = get_line_and_copy(++lnum, file_line);
		    }
		    否则 もし (vim_fgets(line = file_line,
						      LSIZE, files[depth].fp))
			goto exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(line);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    もし (p > aux)
		    {
			もし (*aux != ')' && IObuff[i-1] != TAB)
			{
			    もし (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    もし (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == NULL
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			もし (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (整数)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    もし (i == ins_compl_len())
			goto exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? NULL
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		もし (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		否则 もし (add_r == FAIL)
		    break;
	    }
	    否则 もし (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		もし (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		もし (curr_fname != prev_fname)
		{
		    もし (did_show)
			msg_putchar('\n');	// cursor below last one
		    もし (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			msg_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		もし (!got_int)
		    show_pat_in_path(line, type, TRUE, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_count++);

		// Set matched flag for this file and all the ones that
		// include it
		ために (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    否则 もし (--count <= 0)
	    {
		found = TRUE;
		もし (depth == -1 && lnum == curwin->w_cursor.lnum
# もし defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_line));
		否则 もし (action == ACTION_SHOW)
		{
		    show_pat_in_path(line, type, did_show, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		否则
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# もし defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    もし (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    もし (action == ACTION_SPLIT)
		    {
			もし (win_split(0, 0) == FAIL)
			    break;
			RESET_BINDING(curwin);
		    }
		    もし (depth == -1)
		    {
			// match in current file
# もし defined(FEAT_QUICKFIX)
			もし (g_do_tagpreview != 0)
			{
			    もし (!win_valid(curwin_save))
				break;
			    もし (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, NULL,
						     NULL, TRUE, lnum, forceit)))
				break;	// failed to jump to file
			}
			否则
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    否则
		    {
			もし (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, NULL, TRUE,
						    files[depth].lnum, forceit)))
			    break;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		もし (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - line);
		    curwin->w_set_curswant = true;
		}

# もし defined(FEAT_QUICKFIX)
		もし (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		否则 もし (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		break;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    もし (def_regmatch.regprog == NULL
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		goto search_line;
	}
	line_breakcheck();
	もし (action == ACTION_EXPAND)
	    ins_compl_check_keys(30, FALSE);
	もし (got_int || ins_compl_interrupted())
	    break;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ながら (depth >= 0 && !already
		&& vim_fgets(line = file_line, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    もし (depth < depth_displayed)
		depth_displayed = depth;
	}
	もし (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (整数)STRLEN(line);
	    もし (i > 0 && line[i - 1] == '\n')
		line[--i] = NUL;
	    もし (i > 0 && line[i - 1] == '\r')
		line[--i] = NUL;
	}
	否则 もし (!already)
	{
	    もし (++lnum > end_lnum)
		break;
	    line = get_line_and_copy(lnum, file_line);
	}
	already = NULL;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    ために (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    ために (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    もし (type == CHECK_PATH)
    {
	もし (!did_show)
	{
	    もし (action != ACTION_SHOW_ALL)
		msg(_("All included files were found"));
	    否则
		msg(_("No included files"));
	}
    }
    否则 もし (!found && action != ACTION_EXPAND && !silent)
    {
	もし (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	否则 もし (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	否则
	    emsg(_(e_couldnt_find_pattern));
    }
    もし (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	msg_end();

fpip_end:
    vim_free(file_line);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    static void
show_pat_in_path(
    char_u  *line,
    整数	    type,
    整数	    did_show,
    整数	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;
    size_t  linelen;

    もし (did_show)
	msg_putchar('\n');	// cursor below last one
    否则 もし (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    もし (got_int)		// 'q' typed at "--more--" message
	戻る;
    linelen = STRLEN(line);
    ために (;;)
    {
	p = line + linelen - 1;
	もし (fp != NULL)
	{
	    // We used fgets(), so get rid of newline at end
	    もし (p >= line && *p == '\n')
		--p;
	    もし (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	もし (action == ACTION_SHOW_ALL)
	{
	    sprintf((char *)IObuff, "%3ld: ", count);	// show match nr
	    msg_puts((char *)IObuff);
	    sprintf((char *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    msg_puts_attr((char *)IObuff, HL_ATTR(HLF_N));
	    msg_puts(" ");
	}
	msg_prt_line(line, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	もし (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    break;

	もし (fp != NULL)
	{
	    もし (vim_fgets(line, LSIZE, fp)) // end of file
		break;
	    linelen = STRLEN(line);
	    ++*lnum;
	}
	否则
	{
	    もし (++*lnum > curbuf->b_ml.ml_line_count)
		break;
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
get_spat(整数 idx)
{
    戻る &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    整数
get_spat_last_idx(void)
{
    戻る last_idx;
}
#endif

#もし defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    void
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = NULL;
    整数			maxcount = p_msc;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    整数			recompute = TRUE;
    searchstat_T	stat;

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	戻る;

    もし (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	整数		error = FALSE;

	もし (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    戻る;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	もし (di != NULL)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    もし (error)
		戻る;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	もし (di != NULL)
	{
	    maxcount = (整数)tv_get_number_chk(&di->di_tv, &error);
	    もし (error)
		戻る;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	もし (di != NULL)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    もし (pattern == NULL)
		戻る;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	もし (di != NULL)
	{
	    もし (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		戻る;
	    }
	    もし (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		戻る;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    もし (li != NULL)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		もし (error)
		    戻る;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    もし (li != NULL)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		もし (error)
		    戻る;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    もし (li != NULL)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		もし (error)
		    戻る;
	    }
	}
    }

    save_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
# endif
    もし (pattern != NULL)
    {
	もし (*pattern == NUL)
	    goto the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	もし (spats[last_idx].pat == NULL)
	    spats[last_idx].patlen = 0;
    }
    もし (spats[last_idx].pat == NULL || *spats[last_idx].pat == NUL)
	goto the_end;	// the previous pattern was never defined

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
