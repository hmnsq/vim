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
static 정수 first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
static char_u *get_line_and_copy(linenr_T lnum, char_u *buf);
static void show_pat_in_path(char_u *, 정수, 정수, 정수, FILE *, linenr_T *, long);
#endif

typedef struct searchstat
{
    정수	    cur;	    // current position of found words
    정수	    cnt;	    // total count of found words
    정수	    exact_match;    // TRUE if matched exactly on specified position
    정수	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    정수	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_SEARCH_EXTRA
static void save_incsearch_state(void);
static void restore_incsearch_state(void);
#endif
static 정수 check_prevcol(char_u *linep, 정수 col, 정수 ch, 정수 *prevcol);
static 정수 find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
static void find_mps_values(정수 *initc, 정수 *findc, 정수 *backwards, 정수 switchit);
static 정수 is_zero_width(char_u *pattern, size_t patternlen, 정수 move, pos_T *cur, 정수 direction);
static void cmdline_search_stat(정수 dirc, pos_T *pos, pos_T *cursor_pos, 정수 show_top_bot_msg, char_u *msgbuf, size_t msgbuflen, 정수 recompute, 정수 maxcount, long timeout);
static void update_search_stat(정수 dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, 정수 recompute, 정수 maxcount, long timeout);

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

static 정수 last_idx = 0;	// index in spats[] for RE_LAST

static char_u lastc[2] = {NUL, NUL};	// last character searched for
static 정수 lastcdir = FORWARD;		// last direction of character search
static 정수 last_t_cmd = TRUE;		// last search t_cmd
static char_u	lastc_bytes[MB_MAXBYTES + 1];
static 정수	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
static spat_T	    saved_spats[ARRAY_LENGTH(spats)];
static char_u	    *saved_mr_pattern = NULL;
static size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_SEARCH_EXTRA
static 정수	    saved_spats_last_idx = 0;
static 정수	    saved_spats_no_hlsearch = 0;
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
    정수		matched;	// Found a match in this file
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
    정수
search_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    정수		pat_save,
    정수		pat_use,
    정수		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    정수		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    만약 (pat == NULL || *pat == NUL)
    {
	정수 i;

	만약 (pat_use == RE_LAST)
	    i = last_idx;
	아니면
	    i = pat_use;
	만약 (spats[i].pat == NULL)	// pattern was never defined
	{
	    만약 (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    아니면
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    반환 FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    아니면 만약 (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, patlen, TRUE, NUL);

    만약 (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    만약 (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    아니면
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    만약 (mr_pattern == NULL)
	mr_patternlen = 0;
    아니면
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    만약 (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	만약 (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, patlen, magic);
	// substitute or global command
	만약 (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    만약 (regmatch->regprog == NULL)
	반환 FAIL;
    반환 OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_search_pat(void)
{
    반환 mr_pattern;
}

    void
save_re_pat(정수 idx, char_u *pat, size_t patlen, 정수 magic)
{
    만약 (spats[idx].pat == pat)
	반환;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    만약 (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    아니면
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    만약 (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
static 정수 save_level = 0;

    void
save_search_patterns(void)
{
    정수 i;

    만약 (save_level++ != 0)
	반환;

    위해 (i = 0; i < (정수)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	만약 (spats[i].pat != NULL)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    만약 (saved_spats[i].pat == NULL)
		saved_spats[i].patlen = 0;
	    아니면
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    만약 (mr_pattern == NULL)
	saved_mr_pattern = NULL;
    아니면
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    만약 (saved_mr_pattern == NULL)
	saved_mr_patternlen = 0;
    아니면
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    void
restore_search_patterns(void)
{
    정수 i;

    만약 (--save_level != 0)
	반환;

    위해 (i = 0; i < (정수)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#만약 defined(FEAT_EVAL)
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

#만약 defined(EXITFREE)
    void
free_search_patterns(void)
{
    정수 i;

    위해 (i = 0; i < (정수)ARRAY_LENGTH(spats); ++i)
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
static 정수	    did_save_last_search_spat = 0;
static 정수	    saved_last_idx = 0;
static 정수	    saved_no_hlsearch = 0;
static 정수	    saved_search_match_endcol;
static 정수	    saved_search_match_lines;

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
    만약 (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	반환;

    saved_last_search_spat = spats[RE_SEARCH];
    만약 (spats[RE_SEARCH].pat != NULL)
    {
	saved_last_search_spat.pat = vim_strnsave(spats[RE_SEARCH].pat, spats[RE_SEARCH].patlen);
	만약 (saved_last_search_spat.pat == NULL)
	    saved_last_search_spat.patlen = 0;
	아니면
	    saved_last_search_spat.patlen = spats[RE_SEARCH].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    void
restore_last_search_pattern(void)
{
    만약 (--did_save_last_search_spat > 0)
	// nested call, nothing to do
	반환;
    만약 (did_save_last_search_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	반환;
    }

    vim_free(spats[RE_SEARCH].pat);
    spats[RE_SEARCH] = saved_last_search_spat;
    saved_last_search_spat.pat = NULL;
    saved_last_search_spat.patlen = 0;
# 만약 defined(FEAT_EVAL)
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
    반환 spats[RE_SEARCH].pat;
}

    size_t
last_search_pattern_len(void)
{
    반환 spats[RE_SEARCH].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    정수
ignorecase(char_u *pat)
{
    반환 ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    정수
ignorecase_opt(char_u *pat, 정수 ic_in, 정수 scs)
{
    정수		ic = ic_in;

    만약 (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    반환 ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    정수
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (void)skip_regexp_ex(pat, NUL, magic_isset(), NULL, NULL, &magic_val);

    동안 (*p != NUL)
    {
	정수		l;

	만약 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    만약 (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		반환 TRUE;
	    p += l;
	}
	아니면 만약 (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    만약 (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    아니면 만약 (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    아니면 만약 (p[1] != NUL)  // skip "\X"
		p += 2;
	    아니면
		p += 1;
	}
	아니면 만약 ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    만약 (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    아니면
		p++;
	}
	아니면 만약 (MB_ISUPPER(*p))
	    반환 TRUE;
	아니면
	    ++p;
    }
    반환 FALSE;
}

#만약 defined(FEAT_EVAL)
    char_u *
last_csearch(void)
{
    반환 lastc_bytes;
}

    정수
last_csearch_forward(void)
{
    반환 lastcdir == FORWARD;
}

    정수
last_csearch_until(void)
{
    반환 last_t_cmd == TRUE;
}

    void
set_last_csearch(정수 c, char_u *s, 정수 len)
{
    *lastc = c;
    lastc_bytelen = len;
    만약 (len)
	memcpy(lastc_bytes, s, len);
    아니면
	CLEAR_FIELD(lastc_bytes);
}
#endif

    void
set_csearch_direction(정수 cdir)
{
    lastcdir = cdir;
}

    void
set_csearch_until(정수 t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_search_pat(void)
{
    반환 spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    void
reset_search_dir(void)
{
    spats[0].off.dir = '/';
#만약 defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#만약 defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    void
set_last_search_pat(
    char_u	*s,
    정수		idx,
    정수		magic,
    정수		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    만약 (*s == NUL)
	spats[idx].pat = NULL;
    아니면
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    만약 (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# 만약 defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    만약 (setlast)
	last_idx = idx;
    만약 (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	만약 (spats[idx].pat == NULL)
	    saved_spats[idx].pat = NULL;
	아니면
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	만약 (saved_spats[idx].pat == NULL)
	    saved_spats[idx].patlen = 0;
	아니면
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    만약 (p_hls && idx == last_idx && !no_hlsearch)
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
    만약 (spats[last_idx].pat == NULL)
    {
	regmatch->regprog = NULL;
	반환;
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
    정수
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    정수		dir,
    char_u	*pat,
    size_t	patlen,
    long	count,
    정수		options,
    정수		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    정수		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    정수		loop;
    pos_T	start_pos;
    정수		at_first_line;
    정수		extra_col;
    정수		start_char_len;
    정수		match_ok;
    long	nmatched;
    정수		submatch = 0;
    정수		first_match = TRUE;
    정수		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    정수		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    정수		unused_timeout_flag = FALSE;
    정수		*timed_out = &unused_timeout_flag;  // set when timed out.
    정수		search_from_match_end;		    // vi-compatible search?

    만약 (search_regcomp(pat, patlen, NULL, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	만약 ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	반환 FAIL;
    }

    search_from_match_end = vim_strchr(p_cpo, CPO_SEARCH) != NULL;

    만약 (extra_arg != NULL)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	만약 (extra_arg->sa_tm > 0)
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
	만약 (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	아니면 만약 (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    만약 (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    아니면
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	아니면
	    start_char_len = 1;
	만약 (dir == FORWARD)
	{
	    만약 (options & SEARCH_START)
		extra_col = 0;
	    아니면
		extra_col = start_char_len;
	}
	아니면
	{
	    만약 (options & SEARCH_START)
		extra_col = start_char_len;
	    아니면
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	만약 (pos->lnum == 0)	// correct lnum for when starting in line 0
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
	만약 (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	아니면
	    lnum = pos->lnum;

	위해 (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    위해 ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		만약 (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    break;
		// Stop after passing the time limit.
		만약 (*timed_out)
		    break;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		만약 (regmatch.regprog == NULL)
		    break;
		// Abort searching on an error (e.g., out of stack).
		만약 (called_emsg > called_emsg_before || *timed_out)
		    break;
		만약 (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    만약 (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    아니면
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    만약 (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			동안 (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (정수)endpos.col - 1
					     < (정수)start_pos.col + extra_col)
				    : ((정수)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (정수)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    만약 (search_from_match_end)
			    {
				만약 (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    break;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				만약 (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    만약 (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    아니면
					++matchcol;
				}
			    }
			    아니면
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				만약 (ptr[matchcol] != NUL)
				{
				    만약 (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    아니면
					++matchcol;
				}
			    }
			    만약 (matchcol == 0 && (options & SEARCH_START))
				break;
			    만약 (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    만약 (regmatch.regprog == NULL)
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
			만약 (!match_ok)
			    continue;
		    }
		    만약 (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			위해 (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    만약 (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (정수)regmatch.endpos[0].col - 1
							< (정수)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (정수)regmatch.startpos[0].col
						      < (정수)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    아니면
				break;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    만약 (search_from_match_end)
			    {
				만약 (nmatched > 1)
				    break;
				matchcol = endpos.col;
				// for empty match: advance one char
				만약 (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    만약 (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    아니면
					++matchcol;
				}
			    }
			    아니면
			    {
				// Stop when the match is in a next line.
				만약 (matchpos.lnum > 0)
				    break;
				matchcol = matchpos.col;
				만약 (ptr[matchcol] != NUL)
				{
				    만약 (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    아니면
					++matchcol;
				}
			    }
			    만약 (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				만약 (*timed_out)
				    match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    만약 (regmatch.regprog == NULL)
				break;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			만약 (!match_ok)
			    continue;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    만약 ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			만약 (endpos.col == 0)
			{
			    만약 (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			아니면
			{
			    --pos->col;
			    만약 (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			만약 (end_pos != NULL)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    아니면
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			만약 (end_pos != NULL)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    만약 (end_pos != NULL)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    break;
		}
		line_breakcheck();	// stop if ctrl-C typed
		만약 (got_int)
		    break;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		만약 ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    break;
		}
#endif

		만약 (loop && lnum == start_pos.lnum)
		    break;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    만약 (regmatch.regprog == NULL)
		break;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    만약 (!p_ws || stop_lnum != 0 || got_int
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
	    만약 (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    아니면
		lnum = 1;
	    만약 (!shortmess(SHM_SEARCH)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    만약 (extra_arg != NULL)
		extra_arg->sa_wrapped = TRUE;
	}
	만약 (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    break;
    }
    동안 (--count > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    만약 (extra_arg != NULL && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    만약 (!found)		    // did not find it
    {
	만약 (got_int)
	    emsg(_(e_interrupted));
	아니면 만약 ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    만약 (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    아니면 만약 (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    아니면
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	반환 FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    만약 (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	만약 (pos->col > 0)
	    --pos->col;
    }

    반환 submatch + 1;
}

#만약 defined(FEAT_EVAL) || defined(FEAT_PROTO)
    void
set_search_direction(정수 cdir)
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
    static 정수
first_submatch(regmmatch_T *rp)
{
    정수		submatch;

    위해 (submatch = 1; ; ++submatch)
    {
	만약 (rp->startpos[submatch].lnum >= 0)
	    break;
	만약 (submatch == 9)
	{
	    submatch = 0;
	    break;
	}
    }
    반환 submatch;
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
    정수
parse_search_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    정수		search_delim,
    정수		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    정수		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    만약 (*pat == NULL || **pat == NUL)
	반환 0;

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
    만약 (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (정수)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    만약 (*p == search_delim)
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
    만약 (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->line = TRUE;
    아니면 만약 ((options & SEARCH_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	만약 (*p == 'e')		// end
	    offset->end = SEARCH_END;
	++p;
    }
    만약 (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	만약 (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((char *)p);
	아니면 만약 (*p == '-')	    // single '-'
	    offset->off = -1;
	아니면			    // single '+'
	    offset->off = 1;
	++p;
	동안 (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (정수)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    반환 cmdlen;
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
    정수
do_search(
    oparg_T	    *oap,	// can be NULL
    정수		    dirc,	// '/' or '?'
    정수		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    count,
    정수		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    정수		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = NULL;
    정수		    show_search_stats;
    char_u	    *msgbuf = NULL;
    size_t	    msgbuflen = 0;
    정수		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    만약 (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != NULL)
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
    만약 (dirc == 0)
	dirc = spats[0].off.dir;
    아니면
    {
	spats[0].off.dir = dirc;
#만약 defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    만약 (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#아니면
	만약 (dirc == '/')
	    dirc = '?';
	아니면
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    만약 (dirc == '/')
    {
	만약 (hasFolding(pos.lnum, NULL, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    아니면
    {
	만약 (hasFolding(pos.lnum, &pos.lnum, NULL))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    만약 (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    위해 (;;)
    {
	정수		show_top_bot_msg = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = NULL;
					    // use previous pattern
	만약 (pat == NULL || *pat == NUL || *pat == search_delim)
	{
	    만약 (spats[RE_SEARCH].pat == NULL)	    // no previous pattern
	    {
		만약 (spats[RE_SUBST].pat == NULL)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    goto end_do_search;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    아니면
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	만약 (pat != NULL && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_search_pattern_offset(&pat, &patlen,
				search_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    만약 (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_search_stats = FALSE;
	만약 ((options & SEARCH_ECHO) && messaging()
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
	    만약 (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		만약 (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		아니면 만약 (!spats[0].off.line)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		만약 (spats[0].off.off != 0 || spats[0].off.line)
		    off_len += vim_snprintf((char *)off_buf + off_len,
			   sizeof(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    만약 (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    아니면
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    만약 (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		만약 (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (정수)(Rows - msg_row) * cmdline_width - 1;
		아니면
		    // Use up to 'showcmd' column.
		    msgbufsize = (정수)(Rows - msg_row - 1) * cmdline_width
			+ sc_col - 1;
		만약 (msgbufsize < plen + off_len + SEARCH_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    아니면
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    만약 (msgbuf == NULL)
	    {
		msgbuflen = 0;
	    }
	    아니면
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		만약 (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    만약 (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    아니면
			mch_memmove(msgbuf + 1, p, plen);
		    만약 (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    만약 (trunc != NULL)
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
		    만약 (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			만약 (r != NULL)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    동안 (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    만약 ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    아니면
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

		만약 (!shortmess(SHM_SEARCHCOUNT))
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
	만약 (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    만약 (spats[0].off.off > 0)
	    {
		위해 (c = spats[0].off.off; c; --c)
		    만약 (decl(&pos) == -1)
			break;
		만약 (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    아니면
	    {
		위해 (c = spats[0].off.off; c; ++c)
		    만약 (incl(&pos) == -1)
			break;
		만약 (c)			// at end of buffer
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

	만약 (dircp != NULL)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	만약 (!shortmess(SHM_SEARCH) && sia && sia->sa_wrapped)
	    show_top_bot_msg = TRUE;

	만약 (c == FAIL)
	{
	    retval = 0;
	    goto end_do_search;
	}
	만약 (spats[0].off.end && oap != NULL)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	만약 (!(options & SEARCH_NOOF) || (pat != NULL && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    만약 (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		만약 (c < 1)
		    pos.lnum = 1;
		아니면 만약 (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		아니면
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    아니면 만약 (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		만약 (c > 0)
		{
		    동안 (c-- > 0)
			만약 (incl(&pos) == -1)
			    break;
		}
		// to the left, check for start of file
		아니면
		{
		    동안 (c++ < 0)
			만약 (decl(&pos) == -1)
			    break;
		}
	    }
	    만약 (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	만약 (show_search_stats)
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
	만약 (!(options & SEARCH_OPT) || pat == NULL || *pat != ';')
	    break;

	dirc = *++pat;
	--patlen;
	search_delim = dirc;
	만약 (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    goto end_do_search;
	}
	++pat;
	--patlen;
    }

    만약 (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_search:
    만약 ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    반환 retval;
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
    정수
search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    정수		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    정수		compl_len = ins_compl_len();

    만약 (buf->b_ml.ml_line_count == 0)
	반환 FAIL;
    위해 (;;)
    {
	pos->lnum += dir;
	만약 (pos->lnum < 1)
	{
	    만약 (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		만약 (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    아니면
	    {
		pos->lnum = 1;
		break;
	    }
	}
	아니면 만약 (pos->lnum > buf->b_ml.ml_line_count)
	{
	    만약 (p_ws)
	    {
		pos->lnum = 1;
		만약 (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    아니면
	    {
		pos->lnum = 1;
		break;
	    }
	}
	만약 (pos->lnum == start)
	    break;
	만약 (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	만약 (compl_status_adding() && !compl_status_sol())
	{
	    만약 ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		반환 OK;
	}
	아니면 만약 (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    만약 ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		반환 OK;
	}
    }
    반환 FAIL;
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
    정수
searchc(cmdarg_T *cap, 정수 t_cmd)
{
    정수			c = cap->nchar;	// char to search for
    정수			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    정수			col;
    char_u		*p;
    정수			len;
    정수			stop = TRUE;

    만약 (c != NUL)	// normal search: remember args for repeat
    {
	만약 (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    만약 (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		만약 (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    아니면		// repeat previous search
    {
	만약 (*lastc == NUL && lastc_bytelen <= 1)
	    반환 FAIL;
	만약 (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	아니면
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	만약 (vim_strchr(p_cpo, CPO_SCOLON) == NULL && count == 1 && t_cmd)
	    stop = FALSE;
    }

    만약 (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    아니면
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    동안 (count--)
    {
	만약 (has_mbyte)
	{
	    위해 (;;)
	    {
		만약 (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    만약 (col >= len)
			반환 FAIL;
		}
		아니면
		{
		    만약 (col == 0)
			반환 FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		만약 (lastc_bytelen <= 1)
		{
		    만약 (p[col] == c && stop)
			break;
		}
		아니면 만약 (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    break;
		stop = TRUE;
	    }
	}
	아니면
	{
	    위해 (;;)
	    {
		만약 ((col += dir) < 0 || col >= len)
		    반환 FAIL;
		만약 (p[col] == c && stop)
		    break;
		stop = TRUE;
	    }
	}
    }

    만약 (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	만약 (has_mbyte)
	{
	    만약 (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    아니면
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    반환 OK;
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
findmatch(oparg_T *oap, 정수 initc)
{
    반환 findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    static 정수
check_prevcol(
    char_u	*linep,
    정수		col,
    정수		ch,
    정수		*prevcol)
{
    --col;
    만약 (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    만약 (prevcol)
	*prevcol = col;
    반환 (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    static 정수
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    정수		found = FALSE;

    위해 (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    만약 (delim_copy == NULL)
	반환 FALSE;
    위해 (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *line = ml_get(lnum);

	위해 (p = line + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    만약 (lnum == endpos->lnum && (colnr_T)(p - line) >= endpos->col)
		break;
	    만약 (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		break;
	    }
	}
	만약 (found)
	    break;
    }
    vim_free(delim_copy);
    반환 found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    static void
find_mps_values(
    정수	    *initc,
    정수	    *findc,
    정수	    *backwards,
    정수	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    동안 (*ptr != NUL)
    {
	만약 (has_mbyte)
	{
	    char_u *prev;

	    만약 (mb_ptr2char(ptr) == *initc)
	    {
		만약 (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		아니면
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		반환;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    만약 (mb_ptr2char(ptr) == *initc)
	    {
		만약 (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		아니면
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		반환;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	아니면
	{
	    만약 (*ptr == *initc)
	    {
		만약 (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		아니면
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		반환;
	    }
	    ptr += 2;
	    만약 (*ptr == *initc)
	    {
		만약 (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		아니면
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		반환;
	    }
	    ++ptr;
	}
	만약 (*ptr == ',')
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
    정수		initc,
    정수		flags,
    정수		maxtravel)
{
    static pos_T pos;			// current search position
    정수		findc = 0;		// matching brace
    정수		c;
    정수		count = 0;		// cumulative number of braces
    정수		backwards = FALSE;	// init for gcc
    정수		raw_string = FALSE;	// search for raw string
    정수		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    정수		do_quotes;		// check for quotes in current line
    정수		at_start;		// do_quotes value at start position
    정수		hash_dir = 0;		// Direction searched for # things
    정수		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    정수		start_in_quotes;	// start position is in quotes
    정수		traveled = 0;		// how far we've searched so far
    정수		ignore_cend = FALSE;    // ignore comment end
    정수		cpo_match;		// vi compatible matching
    정수		cpo_bsl;		// don't recognize backslashes
    정수		match_escaped = 0;	// search for escaped match
    정수		dir;			// Direction to search
    정수		comment_col = MAXCOL;   // start of / / comment
    정수		lispcomm = FALSE;	// inside of Lisp-style comment
    정수		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    정수		skip_comments = (flags & FM_SKIPCOMM) != 0;
    정수		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != NULL);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != NULL);

    // Direction to search when initc is '/', '*' or '#'
    만약 (flags & FM_BACKWARD)
	dir = BACKWARD;
    아니면 만약 (flags & FM_FORWARD)
	dir = FORWARD;
    아니면
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    만약 (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	만약 (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    아니면 만약 (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	만약 (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	만약 (findc == NUL)
	    반환 NULL;
    }
    아니면
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	만약 (initc == '#')
	{
	    hash_dir = dir;
	}
	아니면
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    만약 (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		만약 (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    만약 (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		아니면 만약 (linep[pos.col] == '/')
		{
		    만약 (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    아니면 만약 (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		아니면 만약 (linep[pos.col] == '*')
		{
		    만약 (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    아니면 만약 (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    만약 (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		만약 (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		위해 (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    만약 (initc == NUL)
			break;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    만약 (findc)
			break;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		만약 (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    만약 (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    아니면
			반환 NULL;
		}
		아니면 만약 (!cpo_bsl)
		{
		    정수 col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    위해 (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	만약 (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    만약 (oap != NULL)
		oap->motion_type = MLINE;   // Linewise for this case only
	    만약 (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		만약 (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		아니면 만약 (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		아니면
		    반환 NULL;
	    }
	    pos.col = 0;
	    동안 (!got_int)
	    {
		만약 (hash_dir > 0)
		{
		    만약 (pos.lnum == curbuf->b_ml.ml_line_count)
			break;
		}
		아니면 만약 (pos.lnum == 1)
		    break;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		만약 (*ptr != '#')
		    continue;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		만약 (hash_dir > 0)
		{
		    만약 (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    아니면 만약 (STRNCMP(ptr, "el", 2) == 0)
		    {
			만약 (count == 0)
			    반환 &pos;
		    }
		    아니면 만약 (STRNCMP(ptr, "endif", 5) == 0)
		    {
			만약 (count == 0)
			    반환 &pos;
			count--;
		    }
		}
		아니면
		{
		    만약 (STRNCMP(ptr, "if", 2) == 0)
		    {
			만약 (count == 0)
			    반환 &pos;
			count--;
		    }
		    아니면 만약 (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			만약 (count == 0)
			    반환 &pos;
		    }
		    아니면 만약 (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    반환 NULL;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    만약 (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != NULL)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    만약 ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    만약 (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    만약 (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    동안 (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	만약 (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    만약 (lispcomm && pos.col < (colnr_T)comment_col)
		break;
	    만약 (pos.col == 0)		// at start of line, go to prev. one
	    {
		만약 (pos.lnum == 1)	// start of file
		    break;
		--pos.lnum;

		만약 (maxtravel > 0 && ++traveled > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		만약 (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		만약 (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		아니면 만약 (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    아니면
	    {
		--pos.col;
		만약 (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	아니면				// forward search
	{
	    만약 (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		만약 (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    break;
		++pos.lnum;

		만약 (maxtravel && traveled++ > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		만약 (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    아니면
	    {
		만약 (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		아니면
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	만약 (skip_comments && !comment_dir && !inquote)
	{
	    만약 (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		만약 (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (정수)pos.col < comment_col))
		    in_block_comment = TRUE;
		아니면 만약 (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    아니면
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		만약 (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (정수)pos.col < comment_col))
		    in_block_comment = TRUE;
		아니면 만약 (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	만약 (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    만약 (linep[0] == findc && count == 0)	// match!
		반환 &pos;
	    break;					// out of scope
	}

	만약 (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    만약 (comment_dir == FORWARD)
	    {
		만약 (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    반환 &pos;
		}
	    }
	    아니면    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		만약 (pos.col == 0)
		    continue;
		아니면 만약 (raw_string)
		{
		    만약 (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != NULL)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			만약 (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		아니면 만약 (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (정수)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		아니면 만약 (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    만약 (count > 0)
			pos = match_pos;
		    아니면 만약 (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (정수)pos.col <= comment_col)
			pos.col -= 2;
		    아니면 만약 (ignore_cend)
			continue;
		    아니면
			반환 NULL;
		    반환 &pos;
		}
	    }
	    continue;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	만약 (cpo_match)
	    do_quotes = 0;
	아니면 만약 (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    위해 (ptr = linep; *ptr; ++ptr)
	    {
		만약 (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		만약 (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		만약 (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    만약 (!do_quotes)
	    {
		inquote = FALSE;
		만약 (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    만약 (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			만약 (inquote)
			    start_in_quotes = TRUE;
		    }
		    아니면 만약 (backwards)
			inquote = TRUE;
		}
		만약 (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    만약 (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			만약 (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    만약 (inquote)
				start_in_quotes = TRUE;
			}
			아니면 만약 (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	만약 (start_in_quotes == MAYBE)
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
	    만약 (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    break;

	case '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    만약 (do_quotes)
	    {
		정수 col;

		위해 (col = pos.col - 1; col >= 0; --col)
		    만약 (linep[col] != '\\')
			break;
		만약 ((((정수)pos.col - 1 - col) & 1) == 0)
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
	    만약 (!cpo_match && initc != '\'' && findc != '\'')
	    {
		만약 (backwards)
		{
		    만약 (pos.col > 1)
		    {
			만약 (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    break;
			}
			아니면 만약 (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    break;
			}
		    }
		}
		아니면 만약 (linep[pos.col + 1])	// forward search
		{
		    만약 (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			break;
		    }
		    아니면 만약 (linep[pos.col + 2] == '\'')
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
	    만약 (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != NULL
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', NULL)
		    && check_prevcol(linep, pos.col - 1, '#', NULL))
		break;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    만약 (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (정수)pos.col >= comment_col)))
		break;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    만약 ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		정수	col, bslcnt = 0;

		만약 (!cpo_bsl)
		{
		    위해 (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		만약 (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    만약 (c == initc)
			count++;
		    아니면
		    {
			만약 (count == 0)
			    반환 &pos;
			count--;
		    }
		}
	    }
	}
    }

    만약 (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	반환 &pos;
    }
    반환 (pos_T *)NULL;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    void
showmatch(
    정수		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    정수		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    위해 (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	만약 (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    break;
#endif
	p += mb_ptr2len(p) + 1;
	만약 (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    break;
	p += mb_ptr2len(p);
	만약 (*p == NUL)
	    반환;
    }
    만약 (*p == NUL)
	반환;

    만약 ((lpos = findmatch(NULL, NUL)) == NULL)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	반환;
    }

    만약 (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	반환;

    만약 (!curwin->w_p_wrap)
	getvcol(curwin, lpos, NULL, &vcol, NULL, 0);

    정수 col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    만약 (!col_visible)
	반환;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    만약 (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    만약 (vim_strchr(p_cpo, CPO_SHOWMATCH) != NULL)
	ui_delay(p_mat * 100L + 8, TRUE);
    아니면 만약 (!char_avail())
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
    static 정수
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    정수		move,
    pos_T	*cur,
    정수		direction)
{
    regmmatch_T	regmatch;
    정수		nmatched = 0;
    정수		result = -1;
    pos_T	pos;
    정수		called_emsg_before = called_emsg;
    정수		flag = 0;

    만약 (pattern == NULL)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    만약 (search_regcomp(pattern, patternlen, NULL, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	반환 -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    만약 (move)
    {
	CLEAR_POS(&pos);
    }
    아니면
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    만약 (searchit(curwin, curbuf, &pos, NULL, direction, pattern, patternlen, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, NULL) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	do
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, NULL);
	    만약 (nmatched != 0)
		break;
	} 동안 (regmatch.regprog != NULL
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	만약 (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    반환 result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    정수
current_search(
    long	count,
    정수		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    정수		i;
    정수		dir;
    정수		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    정수		flags = 0;
    pos_T	save_VIsual = VIsual;
    정수		zero_width;
    정수		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    만약 (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    만약 (VIsual_active)
    {
	만약 (forward)
	    incl(&pos);
	아니면
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    만약 (zero_width == -1)
	반환 FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    위해 (i = 0; i < 2; i++)
    {
	만약 (forward)
	{
	    만약 (i == 0 && skip_first_backward)
		continue;
	    dir = i;
	}
	아니면
	    dir = !i;

	flags = 0;
	만약 (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	만약 (i == 0)
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
	만약 (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    만약 (VIsual_active)
		VIsual = save_VIsual;
	    반환 FAIL;
	}
	아니면 만약 (i == 0 && !result)
	{
	    만약 (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    아니면
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_line_count);
	    }
	}
    }

    start_pos = pos;

    만약 (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    만약 (LT_POS(VIsual, end_pos) && forward)
    {
	만약 (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	아니면
	    // put the cursor on last character of match
	    dec_cursor();
    }
    아니면 만약 (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    만약 (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	만약 (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	아니면 만약 (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    만약 (fdo_flags & FDO_SEARCH && KeyTyped)
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

    반환 OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    정수
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    반환 (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    static void
cmdline_search_stat(
    정수		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    정수		show_top_bot_msg,
    char_u	*msgbuf,
    size_t	msgbuflen,
    정수		recompute,
    정수		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    만약 (stat.cur <= 0)
	반환;

    char	t[SEARCH_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    만약 (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	만약 (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	아니면 만약 (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	아니면 만약 (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	아니면
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    아니면
#endif
    {
	만약 (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	아니면 만약 (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	아니면 만약 (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	아니면
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    만약 (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    만약 (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    만약 (dirc == '?' && stat.cur == maxcount + 1)
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
    정수			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    정수			recompute,
    정수			maxcount,
    long		timeout UNUSED)
{
    정수		    save_ws = p_ws;
    정수		    wraparound = FALSE;
    pos_T	    p = (*pos);
    static pos_T    lastpos = {0, 0, 0};
    static 정수	    cur = 0;
    static 정수	    cnt = 0;
    static 정수	    exact_match = FALSE;
    static 정수	    incomplete = 0;
    static 정수	    last_maxcount = 0;
    static 정수	    chgtick = 0;
    static char_u   *lastpat = NULL;
    static size_t   lastpatlen = 0;
    static buf_T    *lbuf = NULL;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    만약 (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	반환;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    만약 (!(chgtick == CHANGEDTICK(curbuf)
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
    만약 (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    아니면
    {
	정수	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	만약 (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	동안 (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, NULL, 0, 1, SEARCH_KEEP, RE_LAST, NULL) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    만약 (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		break;
	    }
#endif
	    cnt++;
	    만약 (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		만약 (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    만약 (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		break;
	    }
	}
	만약 (got_int)
	    cur = -1; // abort
	만약 (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    만약 (lastpat == NULL)
		lastpatlen = 0;
	    아니면
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

#만약 defined(FEAT_FIND_ID)

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
    반환 buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    void
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    정수		dir UNUSED,	// direction of expansion
    정수		len,		// length of search pattern
    정수		whole,		// match whole words only
    정수		skip_comments,	// don't match inside comments
    정수		type,		// Type of search; are we looking for a type?
				// a macro?
    long	count,
    정수		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    정수		forceit,	// If true, always switch to the found path
    정수		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    정수		max_path_depth = 50;
    long	match_count = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = NULL;
    linenr_T	lnum;
    정수		depth;
    정수		depth_displayed;	// For type==CHECK_PATH
    정수		old_files;
    정수		already_searched;
    char_u	*file_line;
    char_u	*line;
    char_u	*p;
    char_u	save_char;
    정수		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    정수		matched = FALSE;
    정수		did_show = FALSE;
    정수		found = FALSE;
    정수		i;
    char_u	*already = NULL;
    char_u	*startp = NULL;
    char_u	*inc_opt = NULL;
# 만약 defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
# endif

    regmatch.regprog = NULL;
    incl_regmatch.regprog = NULL;
    def_regmatch.regprog = NULL;

    file_line = alloc(LSIZE);
    만약 (file_line == NULL)
	반환;

    만약 (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	만약 (pat == NULL)
	    goto fpip_end;
	vim_snprintf((char *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	만약 (regmatch.regprog == NULL)
	    goto fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    만약 (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	만약 (incl_regmatch.regprog == NULL)
	    goto fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    만약 (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	만약 (def_regmatch.regprog == NULL)
	    goto fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * sizeof(SearchedFile), TRUE);
    만약 (files == NULL)
	goto fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    만약 (end_lnum > curbuf->b_ml.ml_line_count)
	end_lnum = curbuf->b_ml.ml_line_count;
    만약 (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    line = get_line_and_copy(lnum, file_line);

    위해 (;;)
    {
	만약 (incl_regmatch.regprog != NULL
		&& vim_regexec(&incl_regmatch, line, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    만약 (strstr((char *)inc_opt, "\\zs") != NULL)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (정수)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    아니면
		// Use text after match with 'include'.
		new_fname = file_name_in_line(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, NULL);
	    already_searched = FALSE;
	    만약 (new_fname != NULL)
	    {
		// Check whether we have already searched in this file
		위해 (i = 0;; i++)
		{
		    만약 (i == depth + 1)
			i = old_files;
		    만약 (i == max_path_depth)
			break;
		    만약 (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			만약 (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    msg_putchar('\n');	    // cursor below last one
			    만약 (!got_int)	    // don't display if 'q'
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

	    만약 (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == NULL && !already_searched)))
	    {
		만약 (did_show)
		    msg_putchar('\n');	    // cursor below last one
		아니면
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    msg_puts_title(_("--- Included files "));
		    만약 (action != ACTION_SHOW_ALL)
			msg_puts_title(_("not found "));
		    msg_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		동안 (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    위해 (i = 0; i < depth_displayed; i++)
			msg_puts("  ");
		    msg_home_replace(files[depth_displayed].name);
		    msg_puts(" -->\n");
		}
		만약 (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    위해 (i = 0; i <= depth_displayed; i++)
			msg_puts("  ");
		    만약 (new_fname != NULL)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			msg_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    아니면
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			만약 (strstr((char *)inc_opt, "\\zs") != NULL)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (정수)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			아니면
			{
			    // find the file name after the end of the match
			    위해 (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    위해 (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			만약 (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (정수)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			아니면 만약 (p > line)
			{
			    만약 (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    만약 (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			msg_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    만약 (new_fname == NULL && action == ACTION_SHOW_ALL)
		    {
			만약 (already_searched)
			    msg_puts(_("  (Already listed)"));
			아니면
			    msg_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    만약 (new_fname != NULL)
	    {
		// Push the new file onto the file stack
		만약 (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    만약 (bigger != NULL)
		    {
			위해 (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			위해 (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = NULL;
			    bigger[i].name = NULL;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			위해 (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		만약 ((files[depth + 1].fp = mch_fopen((char *)new_fname, "r"))
								    == NULL)
		    vim_free(new_fname);
		아니면
		{
		    만약 (++depth == old_files)
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
		    만약 (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			msg_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((char*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(char *)new_fname);
			msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    아니면 만약 (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (char *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	아니면
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = line;
search_line:
	    define_matched = FALSE;
	    만약 (def_regmatch.regprog != NULL
			      && vim_regexec(&def_regmatch, line, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		동안 (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    만약 (def_regmatch.regprog == NULL || define_matched)
	    {
		만약 (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    만약 (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    아니면
			matched = STRNCMP(startp, ptr, len) == 0;
		    만약 (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		아니면 만약 (regmatch.regprog != NULL
			 && vim_regexec(&regmatch, line, (colnr_T)(p - line)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    만약 (!define_matched && skip_comments)
		    {
			만약 ((*line != '#' ||
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
			만약 (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    위해 (p = line; *p && p < startp; ++p)
			    {
				만약 (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    만약 (p[1] == '/')
					break;
				    ++p;
				}
				아니면 만약 (!matched && p[0] == '*' && p[1] == '/')
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
	만약 (matched)
	{
	    만약 (action == ACTION_EXPAND)
	    {
		정수	cont_s_ipos = FALSE;
		정수	add_r;
		char_u	*aux;

		만약 (depth == -1 && lnum == curwin->w_cursor.lnum)
		    break;
		found = TRUE;
		aux = p = startp;
		만약 (compl_status_adding() && (정수)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    만약 (vim_iswordp(p))
			goto exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (정수)(p - aux);

		만약 (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    만약 (depth < 0)
		    {
			만약 (lnum >= end_lnum)
			    goto exit_matched;
			line = get_line_and_copy(++lnum, file_line);
		    }
		    아니면 만약 (vim_fgets(line = file_line,
						      LSIZE, files[depth].fp))
			goto exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(line);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    만약 (p > aux)
		    {
			만약 (*aux != ')' && IObuff[i-1] != TAB)
			{
			    만약 (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    만약 (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == NULL
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			만약 (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (정수)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    만약 (i == ins_compl_len())
			goto exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? NULL
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		만약 (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		아니면 만약 (add_r == FAIL)
		    break;
	    }
	    아니면 만약 (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		만약 (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		만약 (curr_fname != prev_fname)
		{
		    만약 (did_show)
			msg_putchar('\n');	// cursor below last one
		    만약 (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			msg_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		만약 (!got_int)
		    show_pat_in_path(line, type, TRUE, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_count++);

		// Set matched flag for this file and all the ones that
		// include it
		위해 (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    아니면 만약 (--count <= 0)
	    {
		found = TRUE;
		만약 (depth == -1 && lnum == curwin->w_cursor.lnum
# 만약 defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_line));
		아니면 만약 (action == ACTION_SHOW)
		{
		    show_pat_in_path(line, type, did_show, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		아니면
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# 만약 defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    만약 (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    만약 (action == ACTION_SPLIT)
		    {
			만약 (win_split(0, 0) == FAIL)
			    break;
			RESET_BINDING(curwin);
		    }
		    만약 (depth == -1)
		    {
			// match in current file
# 만약 defined(FEAT_QUICKFIX)
			만약 (g_do_tagpreview != 0)
			{
			    만약 (!win_valid(curwin_save))
				break;
			    만약 (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, NULL,
						     NULL, TRUE, lnum, forceit)))
				break;	// failed to jump to file
			}
			아니면
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    아니면
		    {
			만약 (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, NULL, TRUE,
						    files[depth].lnum, forceit)))
			    break;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		만약 (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - line);
		    curwin->w_set_curswant = true;
		}

# 만약 defined(FEAT_QUICKFIX)
		만약 (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		아니면 만약 (WIN_IS_POPUP(curwin))
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
	    만약 (def_regmatch.regprog == NULL
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		goto search_line;
	}
	line_breakcheck();
	만약 (action == ACTION_EXPAND)
	    ins_compl_check_keys(30, FALSE);
	만약 (got_int || ins_compl_interrupted())
	    break;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	동안 (depth >= 0 && !already
		&& vim_fgets(line = file_line, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    만약 (depth < depth_displayed)
		depth_displayed = depth;
	}
	만약 (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (정수)STRLEN(line);
	    만약 (i > 0 && line[i - 1] == '\n')
		line[--i] = NUL;
	    만약 (i > 0 && line[i - 1] == '\r')
		line[--i] = NUL;
	}
	아니면 만약 (!already)
	{
	    만약 (++lnum > end_lnum)
		break;
	    line = get_line_and_copy(lnum, file_line);
	}
	already = NULL;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    위해 (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    위해 (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    만약 (type == CHECK_PATH)
    {
	만약 (!did_show)
	{
	    만약 (action != ACTION_SHOW_ALL)
		msg(_("All included files were found"));
	    아니면
		msg(_("No included files"));
	}
    }
    아니면 만약 (!found && action != ACTION_EXPAND && !silent)
    {
	만약 (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	아니면 만약 (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	아니면
	    emsg(_(e_couldnt_find_pattern));
    }
    만약 (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
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
    정수	    type,
    정수	    did_show,
    정수	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;
    size_t  linelen;

    만약 (did_show)
	msg_putchar('\n');	// cursor below last one
    아니면 만약 (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    만약 (got_int)		// 'q' typed at "--more--" message
	반환;
    linelen = STRLEN(line);
    위해 (;;)
    {
	p = line + linelen - 1;
	만약 (fp != NULL)
	{
	    // We used fgets(), so get rid of newline at end
	    만약 (p >= line && *p == '\n')
		--p;
	    만약 (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	만약 (action == ACTION_SHOW_ALL)
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
	만약 (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    break;

	만약 (fp != NULL)
	{
	    만약 (vim_fgets(line, LSIZE, fp)) // end of file
		break;
	    linelen = STRLEN(line);
	    ++*lnum;
	}
	아니면
	{
	    만약 (++*lnum > curbuf->b_ml.ml_line_count)
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
get_spat(정수 idx)
{
    반환 &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    정수
get_spat_last_idx(void)
{
    반환 last_idx;
}
#endif

#만약 defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    void
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = NULL;
    정수			maxcount = p_msc;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    정수			recompute = TRUE;
    searchstat_T	stat;

    만약 (rettv_dict_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	반환;

    만약 (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	정수		error = FALSE;

	만약 (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    반환;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	만약 (di != NULL)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    만약 (error)
		반환;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	만약 (di != NULL)
	{
	    maxcount = (정수)tv_get_number_chk(&di->di_tv, &error);
	    만약 (error)
		반환;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	만약 (di != NULL)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    만약 (pattern == NULL)
		반환;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	만약 (di != NULL)
	{
	    만약 (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		반환;
	    }
	    만약 (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		반환;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    만약 (li != NULL)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		만약 (error)
		    반환;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    만약 (li != NULL)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		만약 (error)
		    반환;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    만약 (li != NULL)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		만약 (error)
		    반환;
	    }
	}
    }

    save_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
# endif
    만약 (pattern != NULL)
    {
	만약 (*pattern == NUL)
	    goto the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	만약 (spats[last_idx].pat == NULL)
	    spats[last_idx].patlen = 0;
    }
    만약 (spats[last_idx].pat == NULL || *spats[last_idx].pat == NUL)
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
