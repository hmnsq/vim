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
static số_nguyên first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
static char_u *get_line_and_copy(linenr_T lnum, char_u *buf);
static void show_pat_in_path(char_u *, số_nguyên, số_nguyên, số_nguyên, FILE *, linenr_T *, long);
#endif

typedef struct searchstat
{
    số_nguyên	    cur;	    // current position of found words
    số_nguyên	    cnt;	    // total count of found words
    số_nguyên	    exact_match;    // TRUE if matched exactly on specified position
    số_nguyên	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    số_nguyên	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_SEARCH_EXTRA
static void save_incsearch_state(void);
static void restore_incsearch_state(void);
#endif
static số_nguyên check_prevcol(char_u *linep, số_nguyên col, số_nguyên ch, số_nguyên *prevcol);
static số_nguyên find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
static void find_mps_values(số_nguyên *initc, số_nguyên *findc, số_nguyên *backwards, số_nguyên switchit);
static số_nguyên is_zero_width(char_u *pattern, size_t patternlen, số_nguyên move, pos_T *cur, số_nguyên direction);
static void cmdline_search_stat(số_nguyên dirc, pos_T *pos, pos_T *cursor_pos, số_nguyên show_top_bot_msg, char_u *msgbuf, size_t msgbuflen, số_nguyên recompute, số_nguyên maxcount, long timeout);
static void update_search_stat(số_nguyên dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, số_nguyên recompute, số_nguyên maxcount, long timeout);

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

static số_nguyên last_idx = 0;	// index in spats[] for RE_LAST

static char_u lastc[2] = {NUL, NUL};	// last character searched for
static số_nguyên lastcdir = FORWARD;		// last direction of character search
static số_nguyên last_t_cmd = TRUE;		// last search t_cmd
static char_u	lastc_bytes[MB_MAXBYTES + 1];
static số_nguyên	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
static spat_T	    saved_spats[ARRAY_LENGTH(spats)];
static char_u	    *saved_mr_pattern = NULL;
static size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_SEARCH_EXTRA
static số_nguyên	    saved_spats_last_idx = 0;
static số_nguyên	    saved_spats_no_hlsearch = 0;
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
    số_nguyên		matched;	// Found a match in this file
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
    số_nguyên
search_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    số_nguyên		pat_save,
    số_nguyên		pat_use,
    số_nguyên		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    số_nguyên		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    nếu (pat == NULL || *pat == NUL)
    {
	số_nguyên i;

	nếu (pat_use == RE_LAST)
	    i = last_idx;
	không_thì
	    i = pat_use;
	nếu (spats[i].pat == NULL)	// pattern was never defined
	{
	    nếu (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    không_thì
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    trả_về FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    không_thì nếu (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, patlen, TRUE, NUL);

    nếu (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    nếu (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    không_thì
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    nếu (mr_pattern == NULL)
	mr_patternlen = 0;
    không_thì
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    nếu (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	nếu (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, patlen, magic);
	// substitute or global command
	nếu (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    nếu (regmatch->regprog == NULL)
	trả_về FAIL;
    trả_về OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_search_pat(void)
{
    trả_về mr_pattern;
}

    void
save_re_pat(số_nguyên idx, char_u *pat, size_t patlen, số_nguyên magic)
{
    nếu (spats[idx].pat == pat)
	trả_về;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    nếu (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    không_thì
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    nếu (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
static số_nguyên save_level = 0;

    void
save_search_patterns(void)
{
    số_nguyên i;

    nếu (save_level++ != 0)
	trả_về;

    cho (i = 0; i < (số_nguyên)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	nếu (spats[i].pat != NULL)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    nếu (saved_spats[i].pat == NULL)
		saved_spats[i].patlen = 0;
	    không_thì
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    nếu (mr_pattern == NULL)
	saved_mr_pattern = NULL;
    không_thì
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    nếu (saved_mr_pattern == NULL)
	saved_mr_patternlen = 0;
    không_thì
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    void
restore_search_patterns(void)
{
    số_nguyên i;

    nếu (--save_level != 0)
	trả_về;

    cho (i = 0; i < (số_nguyên)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#nếu defined(FEAT_EVAL)
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

#nếu defined(EXITFREE)
    void
free_search_patterns(void)
{
    số_nguyên i;

    cho (i = 0; i < (số_nguyên)ARRAY_LENGTH(spats); ++i)
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
static số_nguyên	    did_save_last_search_spat = 0;
static số_nguyên	    saved_last_idx = 0;
static số_nguyên	    saved_no_hlsearch = 0;
static số_nguyên	    saved_search_match_endcol;
static số_nguyên	    saved_search_match_lines;

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
    nếu (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	trả_về;

    saved_last_search_spat = spats[RE_SEARCH];
    nếu (spats[RE_SEARCH].pat != NULL)
    {
	saved_last_search_spat.pat = vim_strnsave(spats[RE_SEARCH].pat, spats[RE_SEARCH].patlen);
	nếu (saved_last_search_spat.pat == NULL)
	    saved_last_search_spat.patlen = 0;
	không_thì
	    saved_last_search_spat.patlen = spats[RE_SEARCH].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    void
restore_last_search_pattern(void)
{
    nếu (--did_save_last_search_spat > 0)
	// nested call, nothing to do
	trả_về;
    nếu (did_save_last_search_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	trả_về;
    }

    vim_free(spats[RE_SEARCH].pat);
    spats[RE_SEARCH] = saved_last_search_spat;
    saved_last_search_spat.pat = NULL;
    saved_last_search_spat.patlen = 0;
# nếu defined(FEAT_EVAL)
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
    trả_về spats[RE_SEARCH].pat;
}

    size_t
last_search_pattern_len(void)
{
    trả_về spats[RE_SEARCH].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    số_nguyên
ignorecase(char_u *pat)
{
    trả_về ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    số_nguyên
ignorecase_opt(char_u *pat, số_nguyên ic_in, số_nguyên scs)
{
    số_nguyên		ic = ic_in;

    nếu (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    trả_về ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    số_nguyên
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (void)skip_regexp_ex(pat, NUL, magic_isset(), NULL, NULL, &magic_val);

    trong_khi (*p != NUL)
    {
	số_nguyên		l;

	nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    nếu (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		trả_về TRUE;
	    p += l;
	}
	không_thì nếu (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    nếu (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    không_thì nếu (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    không_thì nếu (p[1] != NUL)  // skip "\X"
		p += 2;
	    không_thì
		p += 1;
	}
	không_thì nếu ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    nếu (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    không_thì
		p++;
	}
	không_thì nếu (MB_ISUPPER(*p))
	    trả_về TRUE;
	không_thì
	    ++p;
    }
    trả_về FALSE;
}

#nếu defined(FEAT_EVAL)
    char_u *
last_csearch(void)
{
    trả_về lastc_bytes;
}

    số_nguyên
last_csearch_forward(void)
{
    trả_về lastcdir == FORWARD;
}

    số_nguyên
last_csearch_until(void)
{
    trả_về last_t_cmd == TRUE;
}

    void
set_last_csearch(số_nguyên c, char_u *s, số_nguyên len)
{
    *lastc = c;
    lastc_bytelen = len;
    nếu (len)
	memcpy(lastc_bytes, s, len);
    không_thì
	CLEAR_FIELD(lastc_bytes);
}
#endif

    void
set_csearch_direction(số_nguyên cdir)
{
    lastcdir = cdir;
}

    void
set_csearch_until(số_nguyên t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_search_pat(void)
{
    trả_về spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    void
reset_search_dir(void)
{
    spats[0].off.dir = '/';
#nếu defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#nếu defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    void
set_last_search_pat(
    char_u	*s,
    số_nguyên		idx,
    số_nguyên		magic,
    số_nguyên		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    nếu (*s == NUL)
	spats[idx].pat = NULL;
    không_thì
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    nếu (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# nếu defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    nếu (setlast)
	last_idx = idx;
    nếu (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	nếu (spats[idx].pat == NULL)
	    saved_spats[idx].pat = NULL;
	không_thì
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	nếu (saved_spats[idx].pat == NULL)
	    saved_spats[idx].patlen = 0;
	không_thì
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    nếu (p_hls && idx == last_idx && !no_hlsearch)
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
    nếu (spats[last_idx].pat == NULL)
    {
	regmatch->regprog = NULL;
	trả_về;
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
    số_nguyên
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    số_nguyên		dir,
    char_u	*pat,
    size_t	patlen,
    long	count,
    số_nguyên		options,
    số_nguyên		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    số_nguyên		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    số_nguyên		loop;
    pos_T	start_pos;
    số_nguyên		at_first_line;
    số_nguyên		extra_col;
    số_nguyên		start_char_len;
    số_nguyên		match_ok;
    long	nmatched;
    số_nguyên		submatch = 0;
    số_nguyên		first_match = TRUE;
    số_nguyên		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    số_nguyên		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    số_nguyên		unused_timeout_flag = FALSE;
    số_nguyên		*timed_out = &unused_timeout_flag;  // set when timed out.
    số_nguyên		search_from_match_end;		    // vi-compatible search?

    nếu (search_regcomp(pat, patlen, NULL, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	nếu ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	trả_về FAIL;
    }

    search_from_match_end = vim_strchr(p_cpo, CPO_SEARCH) != NULL;

    nếu (extra_arg != NULL)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	nếu (extra_arg->sa_tm > 0)
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
	nếu (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	không_thì nếu (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    nếu (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    không_thì
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	không_thì
	    start_char_len = 1;
	nếu (dir == FORWARD)
	{
	    nếu (options & SEARCH_START)
		extra_col = 0;
	    không_thì
		extra_col = start_char_len;
	}
	không_thì
	{
	    nếu (options & SEARCH_START)
		extra_col = start_char_len;
	    không_thì
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	nếu (pos->lnum == 0)	// correct lnum for when starting in line 0
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
	nếu (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	không_thì
	    lnum = pos->lnum;

	cho (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    cho ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		nếu (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    break;
		// Stop after passing the time limit.
		nếu (*timed_out)
		    break;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		nếu (regmatch.regprog == NULL)
		    break;
		// Abort searching on an error (e.g., out of stack).
		nếu (called_emsg > called_emsg_before || *timed_out)
		    break;
		nếu (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    nếu (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    không_thì
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    nếu (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			trong_khi (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (số_nguyên)endpos.col - 1
					     < (số_nguyên)start_pos.col + extra_col)
				    : ((số_nguyên)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (số_nguyên)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    nếu (search_from_match_end)
			    {
				nếu (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    break;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				nếu (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    nếu (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    không_thì
					++matchcol;
				}
			    }
			    không_thì
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				nếu (ptr[matchcol] != NUL)
				{
				    nếu (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    không_thì
					++matchcol;
				}
			    }
			    nếu (matchcol == 0 && (options & SEARCH_START))
				break;
			    nếu (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    nếu (regmatch.regprog == NULL)
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
			nếu (!match_ok)
			    continue;
		    }
		    nếu (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			cho (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    nếu (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (số_nguyên)regmatch.endpos[0].col - 1
							< (số_nguyên)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (số_nguyên)regmatch.startpos[0].col
						      < (số_nguyên)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    không_thì
				break;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    nếu (search_from_match_end)
			    {
				nếu (nmatched > 1)
				    break;
				matchcol = endpos.col;
				// for empty match: advance one char
				nếu (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    nếu (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    không_thì
					++matchcol;
				}
			    }
			    không_thì
			    {
				// Stop when the match is in a next line.
				nếu (matchpos.lnum > 0)
				    break;
				matchcol = matchpos.col;
				nếu (ptr[matchcol] != NUL)
				{
				    nếu (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    không_thì
					++matchcol;
				}
			    }
			    nếu (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				nếu (*timed_out)
				    match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    nếu (regmatch.regprog == NULL)
				break;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			nếu (!match_ok)
			    continue;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    nếu ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			nếu (endpos.col == 0)
			{
			    nếu (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			không_thì
			{
			    --pos->col;
			    nếu (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			nếu (end_pos != NULL)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    không_thì
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			nếu (end_pos != NULL)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    nếu (end_pos != NULL)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    break;
		}
		line_breakcheck();	// stop if ctrl-C typed
		nếu (got_int)
		    break;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		nếu ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    break;
		}
#endif

		nếu (loop && lnum == start_pos.lnum)
		    break;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    nếu (regmatch.regprog == NULL)
		break;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    nếu (!p_ws || stop_lnum != 0 || got_int
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
	    nếu (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    không_thì
		lnum = 1;
	    nếu (!shortmess(SHM_SEARCH)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    nếu (extra_arg != NULL)
		extra_arg->sa_wrapped = TRUE;
	}
	nếu (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    break;
    }
    trong_khi (--count > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    nếu (extra_arg != NULL && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    nếu (!found)		    // did not find it
    {
	nếu (got_int)
	    emsg(_(e_interrupted));
	không_thì nếu ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    nếu (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    không_thì nếu (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    không_thì
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	trả_về FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    nếu (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	nếu (pos->col > 0)
	    --pos->col;
    }

    trả_về submatch + 1;
}

#nếu defined(FEAT_EVAL) || defined(FEAT_PROTO)
    void
set_search_direction(số_nguyên cdir)
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
    static số_nguyên
first_submatch(regmmatch_T *rp)
{
    số_nguyên		submatch;

    cho (submatch = 1; ; ++submatch)
    {
	nếu (rp->startpos[submatch].lnum >= 0)
	    break;
	nếu (submatch == 9)
	{
	    submatch = 0;
	    break;
	}
    }
    trả_về submatch;
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
    số_nguyên
parse_search_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    số_nguyên		search_delim,
    số_nguyên		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    số_nguyên		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    nếu (*pat == NULL || **pat == NUL)
	trả_về 0;

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
    nếu (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (số_nguyên)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    nếu (*p == search_delim)
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
    nếu (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->line = TRUE;
    không_thì nếu ((options & SEARCH_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	nếu (*p == 'e')		// end
	    offset->end = SEARCH_END;
	++p;
    }
    nếu (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	nếu (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((char *)p);
	không_thì nếu (*p == '-')	    // single '-'
	    offset->off = -1;
	không_thì			    // single '+'
	    offset->off = 1;
	++p;
	trong_khi (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (số_nguyên)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    trả_về cmdlen;
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
    số_nguyên
do_search(
    oparg_T	    *oap,	// can be NULL
    số_nguyên		    dirc,	// '/' or '?'
    số_nguyên		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    count,
    số_nguyên		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    số_nguyên		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = NULL;
    số_nguyên		    show_search_stats;
    char_u	    *msgbuf = NULL;
    size_t	    msgbuflen = 0;
    số_nguyên		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    nếu (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != NULL)
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
    nếu (dirc == 0)
	dirc = spats[0].off.dir;
    không_thì
    {
	spats[0].off.dir = dirc;
#nếu defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    nếu (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#không_thì
	nếu (dirc == '/')
	    dirc = '?';
	không_thì
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    nếu (dirc == '/')
    {
	nếu (hasFolding(pos.lnum, NULL, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    không_thì
    {
	nếu (hasFolding(pos.lnum, &pos.lnum, NULL))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    nếu (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    cho (;;)
    {
	số_nguyên		show_top_bot_msg = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = NULL;
					    // use previous pattern
	nếu (pat == NULL || *pat == NUL || *pat == search_delim)
	{
	    nếu (spats[RE_SEARCH].pat == NULL)	    // no previous pattern
	    {
		nếu (spats[RE_SUBST].pat == NULL)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    goto end_do_search;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    không_thì
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	nếu (pat != NULL && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_search_pattern_offset(&pat, &patlen,
				search_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    nếu (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_search_stats = FALSE;
	nếu ((options & SEARCH_ECHO) && messaging()
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
	    nếu (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		nếu (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		không_thì nếu (!spats[0].off.line)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		nếu (spats[0].off.off != 0 || spats[0].off.line)
		    off_len += vim_snprintf((char *)off_buf + off_len,
			   sizeof(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    nếu (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    không_thì
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    nếu (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		nếu (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (số_nguyên)(Rows - msg_row) * cmdline_width - 1;
		không_thì
		    // Use up to 'showcmd' column.
		    msgbufsize = (số_nguyên)(Rows - msg_row - 1) * cmdline_width
			+ sc_col - 1;
		nếu (msgbufsize < plen + off_len + SEARCH_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    không_thì
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    nếu (msgbuf == NULL)
	    {
		msgbuflen = 0;
	    }
	    không_thì
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		nếu (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    nếu (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    không_thì
			mch_memmove(msgbuf + 1, p, plen);
		    nếu (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    nếu (trunc != NULL)
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
		    nếu (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			nếu (r != NULL)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    trong_khi (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    nếu ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    không_thì
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

		nếu (!shortmess(SHM_SEARCHCOUNT))
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
	nếu (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    nếu (spats[0].off.off > 0)
	    {
		cho (c = spats[0].off.off; c; --c)
		    nếu (decl(&pos) == -1)
			break;
		nếu (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    không_thì
	    {
		cho (c = spats[0].off.off; c; ++c)
		    nếu (incl(&pos) == -1)
			break;
		nếu (c)			// at end of buffer
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

	nếu (dircp != NULL)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	nếu (!shortmess(SHM_SEARCH) && sia && sia->sa_wrapped)
	    show_top_bot_msg = TRUE;

	nếu (c == FAIL)
	{
	    retval = 0;
	    goto end_do_search;
	}
	nếu (spats[0].off.end && oap != NULL)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	nếu (!(options & SEARCH_NOOF) || (pat != NULL && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    nếu (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		nếu (c < 1)
		    pos.lnum = 1;
		không_thì nếu (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		không_thì
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    không_thì nếu (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		nếu (c > 0)
		{
		    trong_khi (c-- > 0)
			nếu (incl(&pos) == -1)
			    break;
		}
		// to the left, check for start of file
		không_thì
		{
		    trong_khi (c++ < 0)
			nếu (decl(&pos) == -1)
			    break;
		}
	    }
	    nếu (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	nếu (show_search_stats)
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
	nếu (!(options & SEARCH_OPT) || pat == NULL || *pat != ';')
	    break;

	dirc = *++pat;
	--patlen;
	search_delim = dirc;
	nếu (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    goto end_do_search;
	}
	++pat;
	--patlen;
    }

    nếu (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_search:
    nếu ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    trả_về retval;
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
    số_nguyên
search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    số_nguyên		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    số_nguyên		compl_len = ins_compl_len();

    nếu (buf->b_ml.ml_line_count == 0)
	trả_về FAIL;
    cho (;;)
    {
	pos->lnum += dir;
	nếu (pos->lnum < 1)
	{
	    nếu (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		nếu (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    không_thì
	    {
		pos->lnum = 1;
		break;
	    }
	}
	không_thì nếu (pos->lnum > buf->b_ml.ml_line_count)
	{
	    nếu (p_ws)
	    {
		pos->lnum = 1;
		nếu (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    không_thì
	    {
		pos->lnum = 1;
		break;
	    }
	}
	nếu (pos->lnum == start)
	    break;
	nếu (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	nếu (compl_status_adding() && !compl_status_sol())
	{
	    nếu ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		trả_về OK;
	}
	không_thì nếu (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    nếu ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		trả_về OK;
	}
    }
    trả_về FAIL;
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
    số_nguyên
searchc(cmdarg_T *cap, số_nguyên t_cmd)
{
    số_nguyên			c = cap->nchar;	// char to search for
    số_nguyên			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    số_nguyên			col;
    char_u		*p;
    số_nguyên			len;
    số_nguyên			stop = TRUE;

    nếu (c != NUL)	// normal search: remember args for repeat
    {
	nếu (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    nếu (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		nếu (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    không_thì		// repeat previous search
    {
	nếu (*lastc == NUL && lastc_bytelen <= 1)
	    trả_về FAIL;
	nếu (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	không_thì
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	nếu (vim_strchr(p_cpo, CPO_SCOLON) == NULL && count == 1 && t_cmd)
	    stop = FALSE;
    }

    nếu (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    không_thì
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    trong_khi (count--)
    {
	nếu (has_mbyte)
	{
	    cho (;;)
	    {
		nếu (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    nếu (col >= len)
			trả_về FAIL;
		}
		không_thì
		{
		    nếu (col == 0)
			trả_về FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		nếu (lastc_bytelen <= 1)
		{
		    nếu (p[col] == c && stop)
			break;
		}
		không_thì nếu (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    break;
		stop = TRUE;
	    }
	}
	không_thì
	{
	    cho (;;)
	    {
		nếu ((col += dir) < 0 || col >= len)
		    trả_về FAIL;
		nếu (p[col] == c && stop)
		    break;
		stop = TRUE;
	    }
	}
    }

    nếu (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	nếu (has_mbyte)
	{
	    nếu (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    không_thì
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    trả_về OK;
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
findmatch(oparg_T *oap, số_nguyên initc)
{
    trả_về findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    static số_nguyên
check_prevcol(
    char_u	*linep,
    số_nguyên		col,
    số_nguyên		ch,
    số_nguyên		*prevcol)
{
    --col;
    nếu (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    nếu (prevcol)
	*prevcol = col;
    trả_về (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    static số_nguyên
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    số_nguyên		found = FALSE;

    cho (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    nếu (delim_copy == NULL)
	trả_về FALSE;
    cho (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *line = ml_get(lnum);

	cho (p = line + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    nếu (lnum == endpos->lnum && (colnr_T)(p - line) >= endpos->col)
		break;
	    nếu (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		break;
	    }
	}
	nếu (found)
	    break;
    }
    vim_free(delim_copy);
    trả_về found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    static void
find_mps_values(
    số_nguyên	    *initc,
    số_nguyên	    *findc,
    số_nguyên	    *backwards,
    số_nguyên	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    trong_khi (*ptr != NUL)
    {
	nếu (has_mbyte)
	{
	    char_u *prev;

	    nếu (mb_ptr2char(ptr) == *initc)
	    {
		nếu (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		không_thì
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		trả_về;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    nếu (mb_ptr2char(ptr) == *initc)
	    {
		nếu (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		không_thì
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		trả_về;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	không_thì
	{
	    nếu (*ptr == *initc)
	    {
		nếu (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		không_thì
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		trả_về;
	    }
	    ptr += 2;
	    nếu (*ptr == *initc)
	    {
		nếu (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		không_thì
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		trả_về;
	    }
	    ++ptr;
	}
	nếu (*ptr == ',')
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
    số_nguyên		initc,
    số_nguyên		flags,
    số_nguyên		maxtravel)
{
    static pos_T pos;			// current search position
    số_nguyên		findc = 0;		// matching brace
    số_nguyên		c;
    số_nguyên		count = 0;		// cumulative number of braces
    số_nguyên		backwards = FALSE;	// init for gcc
    số_nguyên		raw_string = FALSE;	// search for raw string
    số_nguyên		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    số_nguyên		do_quotes;		// check for quotes in current line
    số_nguyên		at_start;		// do_quotes value at start position
    số_nguyên		hash_dir = 0;		// Direction searched for # things
    số_nguyên		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    số_nguyên		start_in_quotes;	// start position is in quotes
    số_nguyên		traveled = 0;		// how far we've searched so far
    số_nguyên		ignore_cend = FALSE;    // ignore comment end
    số_nguyên		cpo_match;		// vi compatible matching
    số_nguyên		cpo_bsl;		// don't recognize backslashes
    số_nguyên		match_escaped = 0;	// search for escaped match
    số_nguyên		dir;			// Direction to search
    số_nguyên		comment_col = MAXCOL;   // start of / / comment
    số_nguyên		lispcomm = FALSE;	// inside of Lisp-style comment
    số_nguyên		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    số_nguyên		skip_comments = (flags & FM_SKIPCOMM) != 0;
    số_nguyên		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != NULL);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != NULL);

    // Direction to search when initc is '/', '*' or '#'
    nếu (flags & FM_BACKWARD)
	dir = BACKWARD;
    không_thì nếu (flags & FM_FORWARD)
	dir = FORWARD;
    không_thì
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    nếu (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	nếu (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    không_thì nếu (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	nếu (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	nếu (findc == NUL)
	    trả_về NULL;
    }
    không_thì
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	nếu (initc == '#')
	{
	    hash_dir = dir;
	}
	không_thì
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    nếu (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		nếu (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    nếu (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		không_thì nếu (linep[pos.col] == '/')
		{
		    nếu (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    không_thì nếu (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		không_thì nếu (linep[pos.col] == '*')
		{
		    nếu (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    không_thì nếu (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    nếu (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		nếu (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		cho (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    nếu (initc == NUL)
			break;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    nếu (findc)
			break;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		nếu (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    nếu (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    không_thì
			trả_về NULL;
		}
		không_thì nếu (!cpo_bsl)
		{
		    số_nguyên col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    cho (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	nếu (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    nếu (oap != NULL)
		oap->motion_type = MLINE;   // Linewise for this case only
	    nếu (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		nếu (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		không_thì nếu (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		không_thì
		    trả_về NULL;
	    }
	    pos.col = 0;
	    trong_khi (!got_int)
	    {
		nếu (hash_dir > 0)
		{
		    nếu (pos.lnum == curbuf->b_ml.ml_line_count)
			break;
		}
		không_thì nếu (pos.lnum == 1)
		    break;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		nếu (*ptr != '#')
		    continue;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		nếu (hash_dir > 0)
		{
		    nếu (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    không_thì nếu (STRNCMP(ptr, "el", 2) == 0)
		    {
			nếu (count == 0)
			    trả_về &pos;
		    }
		    không_thì nếu (STRNCMP(ptr, "endif", 5) == 0)
		    {
			nếu (count == 0)
			    trả_về &pos;
			count--;
		    }
		}
		không_thì
		{
		    nếu (STRNCMP(ptr, "if", 2) == 0)
		    {
			nếu (count == 0)
			    trả_về &pos;
			count--;
		    }
		    không_thì nếu (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			nếu (count == 0)
			    trả_về &pos;
		    }
		    không_thì nếu (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    trả_về NULL;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    nếu (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != NULL)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    nếu ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    nếu (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    nếu (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    trong_khi (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	nếu (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    nếu (lispcomm && pos.col < (colnr_T)comment_col)
		break;
	    nếu (pos.col == 0)		// at start of line, go to prev. one
	    {
		nếu (pos.lnum == 1)	// start of file
		    break;
		--pos.lnum;

		nếu (maxtravel > 0 && ++traveled > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		nếu (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		nếu (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		không_thì nếu (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    không_thì
	    {
		--pos.col;
		nếu (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	không_thì				// forward search
	{
	    nếu (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		nếu (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    break;
		++pos.lnum;

		nếu (maxtravel && traveled++ > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		nếu (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    không_thì
	    {
		nếu (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		không_thì
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	nếu (skip_comments && !comment_dir && !inquote)
	{
	    nếu (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		nếu (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (số_nguyên)pos.col < comment_col))
		    in_block_comment = TRUE;
		không_thì nếu (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    không_thì
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		nếu (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (số_nguyên)pos.col < comment_col))
		    in_block_comment = TRUE;
		không_thì nếu (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	nếu (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    nếu (linep[0] == findc && count == 0)	// match!
		trả_về &pos;
	    break;					// out of scope
	}

	nếu (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    nếu (comment_dir == FORWARD)
	    {
		nếu (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    trả_về &pos;
		}
	    }
	    không_thì    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		nếu (pos.col == 0)
		    continue;
		không_thì nếu (raw_string)
		{
		    nếu (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != NULL)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			nếu (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		không_thì nếu (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (số_nguyên)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		không_thì nếu (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    nếu (count > 0)
			pos = match_pos;
		    không_thì nếu (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (số_nguyên)pos.col <= comment_col)
			pos.col -= 2;
		    không_thì nếu (ignore_cend)
			continue;
		    không_thì
			trả_về NULL;
		    trả_về &pos;
		}
	    }
	    continue;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	nếu (cpo_match)
	    do_quotes = 0;
	không_thì nếu (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    cho (ptr = linep; *ptr; ++ptr)
	    {
		nếu (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		nếu (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		nếu (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    nếu (!do_quotes)
	    {
		inquote = FALSE;
		nếu (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    nếu (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			nếu (inquote)
			    start_in_quotes = TRUE;
		    }
		    không_thì nếu (backwards)
			inquote = TRUE;
		}
		nếu (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    nếu (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			nếu (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    nếu (inquote)
				start_in_quotes = TRUE;
			}
			không_thì nếu (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	nếu (start_in_quotes == MAYBE)
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
	    nếu (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    break;

	case '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    nếu (do_quotes)
	    {
		số_nguyên col;

		cho (col = pos.col - 1; col >= 0; --col)
		    nếu (linep[col] != '\\')
			break;
		nếu ((((số_nguyên)pos.col - 1 - col) & 1) == 0)
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
	    nếu (!cpo_match && initc != '\'' && findc != '\'')
	    {
		nếu (backwards)
		{
		    nếu (pos.col > 1)
		    {
			nếu (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    break;
			}
			không_thì nếu (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    break;
			}
		    }
		}
		không_thì nếu (linep[pos.col + 1])	// forward search
		{
		    nếu (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			break;
		    }
		    không_thì nếu (linep[pos.col + 2] == '\'')
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
	    nếu (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != NULL
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', NULL)
		    && check_prevcol(linep, pos.col - 1, '#', NULL))
		break;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    nếu (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (số_nguyên)pos.col >= comment_col)))
		break;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    nếu ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		số_nguyên	col, bslcnt = 0;

		nếu (!cpo_bsl)
		{
		    cho (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		nếu (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    nếu (c == initc)
			count++;
		    không_thì
		    {
			nếu (count == 0)
			    trả_về &pos;
			count--;
		    }
		}
	    }
	}
    }

    nếu (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	trả_về &pos;
    }
    trả_về (pos_T *)NULL;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    void
showmatch(
    số_nguyên		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    số_nguyên		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    cho (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	nếu (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    break;
#endif
	p += mb_ptr2len(p) + 1;
	nếu (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    break;
	p += mb_ptr2len(p);
	nếu (*p == NUL)
	    trả_về;
    }
    nếu (*p == NUL)
	trả_về;

    nếu ((lpos = findmatch(NULL, NUL)) == NULL)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	trả_về;
    }

    nếu (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	trả_về;

    nếu (!curwin->w_p_wrap)
	getvcol(curwin, lpos, NULL, &vcol, NULL, 0);

    số_nguyên col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    nếu (!col_visible)
	trả_về;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    nếu (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    nếu (vim_strchr(p_cpo, CPO_SHOWMATCH) != NULL)
	ui_delay(p_mat * 100L + 8, TRUE);
    không_thì nếu (!char_avail())
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
    static số_nguyên
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    số_nguyên		move,
    pos_T	*cur,
    số_nguyên		direction)
{
    regmmatch_T	regmatch;
    số_nguyên		nmatched = 0;
    số_nguyên		result = -1;
    pos_T	pos;
    số_nguyên		called_emsg_before = called_emsg;
    số_nguyên		flag = 0;

    nếu (pattern == NULL)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    nếu (search_regcomp(pattern, patternlen, NULL, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	trả_về -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    nếu (move)
    {
	CLEAR_POS(&pos);
    }
    không_thì
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    nếu (searchit(curwin, curbuf, &pos, NULL, direction, pattern, patternlen, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, NULL) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	do
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, NULL);
	    nếu (nmatched != 0)
		break;
	} trong_khi (regmatch.regprog != NULL
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	nếu (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    trả_về result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    số_nguyên
current_search(
    long	count,
    số_nguyên		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    số_nguyên		i;
    số_nguyên		dir;
    số_nguyên		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    số_nguyên		flags = 0;
    pos_T	save_VIsual = VIsual;
    số_nguyên		zero_width;
    số_nguyên		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    nếu (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    nếu (VIsual_active)
    {
	nếu (forward)
	    incl(&pos);
	không_thì
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    nếu (zero_width == -1)
	trả_về FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    cho (i = 0; i < 2; i++)
    {
	nếu (forward)
	{
	    nếu (i == 0 && skip_first_backward)
		continue;
	    dir = i;
	}
	không_thì
	    dir = !i;

	flags = 0;
	nếu (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	nếu (i == 0)
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
	nếu (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    nếu (VIsual_active)
		VIsual = save_VIsual;
	    trả_về FAIL;
	}
	không_thì nếu (i == 0 && !result)
	{
	    nếu (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    không_thì
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_line_count);
	    }
	}
    }

    start_pos = pos;

    nếu (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    nếu (LT_POS(VIsual, end_pos) && forward)
    {
	nếu (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	không_thì
	    // put the cursor on last character of match
	    dec_cursor();
    }
    không_thì nếu (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    nếu (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	nếu (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	không_thì nếu (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    nếu (fdo_flags & FDO_SEARCH && KeyTyped)
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

    trả_về OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    số_nguyên
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    trả_về (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    static void
cmdline_search_stat(
    số_nguyên		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    số_nguyên		show_top_bot_msg,
    char_u	*msgbuf,
    size_t	msgbuflen,
    số_nguyên		recompute,
    số_nguyên		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    nếu (stat.cur <= 0)
	trả_về;

    char	t[SEARCH_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    nếu (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	nếu (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	không_thì nếu (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	không_thì nếu (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	không_thì
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    không_thì
#endif
    {
	nếu (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	không_thì nếu (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	không_thì nếu (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	không_thì
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    nếu (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    nếu (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    nếu (dirc == '?' && stat.cur == maxcount + 1)
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
    số_nguyên			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    số_nguyên			recompute,
    số_nguyên			maxcount,
    long		timeout UNUSED)
{
    số_nguyên		    save_ws = p_ws;
    số_nguyên		    wraparound = FALSE;
    pos_T	    p = (*pos);
    static pos_T    lastpos = {0, 0, 0};
    static số_nguyên	    cur = 0;
    static số_nguyên	    cnt = 0;
    static số_nguyên	    exact_match = FALSE;
    static số_nguyên	    incomplete = 0;
    static số_nguyên	    last_maxcount = 0;
    static số_nguyên	    chgtick = 0;
    static char_u   *lastpat = NULL;
    static size_t   lastpatlen = 0;
    static buf_T    *lbuf = NULL;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    nếu (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	trả_về;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    nếu (!(chgtick == CHANGEDTICK(curbuf)
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
    nếu (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    không_thì
    {
	số_nguyên	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	nếu (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	trong_khi (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, NULL, 0, 1, SEARCH_KEEP, RE_LAST, NULL) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    nếu (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		break;
	    }
#endif
	    cnt++;
	    nếu (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		nếu (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    nếu (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		break;
	    }
	}
	nếu (got_int)
	    cur = -1; // abort
	nếu (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    nếu (lastpat == NULL)
		lastpatlen = 0;
	    không_thì
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

#nếu defined(FEAT_FIND_ID)

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
    trả_về buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    void
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    số_nguyên		dir UNUSED,	// direction of expansion
    số_nguyên		len,		// length of search pattern
    số_nguyên		whole,		// match whole words only
    số_nguyên		skip_comments,	// don't match inside comments
    số_nguyên		type,		// Type of search; are we looking for a type?
				// a macro?
    long	count,
    số_nguyên		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    số_nguyên		forceit,	// If true, always switch to the found path
    số_nguyên		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    số_nguyên		max_path_depth = 50;
    long	match_count = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = NULL;
    linenr_T	lnum;
    số_nguyên		depth;
    số_nguyên		depth_displayed;	// For type==CHECK_PATH
    số_nguyên		old_files;
    số_nguyên		already_searched;
    char_u	*file_line;
    char_u	*line;
    char_u	*p;
    char_u	save_char;
    số_nguyên		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    số_nguyên		matched = FALSE;
    số_nguyên		did_show = FALSE;
    số_nguyên		found = FALSE;
    số_nguyên		i;
    char_u	*already = NULL;
    char_u	*startp = NULL;
    char_u	*inc_opt = NULL;
# nếu defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
# endif

    regmatch.regprog = NULL;
    incl_regmatch.regprog = NULL;
    def_regmatch.regprog = NULL;

    file_line = alloc(LSIZE);
    nếu (file_line == NULL)
	trả_về;

    nếu (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	nếu (pat == NULL)
	    goto fpip_end;
	vim_snprintf((char *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	nếu (regmatch.regprog == NULL)
	    goto fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    nếu (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	nếu (incl_regmatch.regprog == NULL)
	    goto fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    nếu (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	nếu (def_regmatch.regprog == NULL)
	    goto fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * sizeof(SearchedFile), TRUE);
    nếu (files == NULL)
	goto fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    nếu (end_lnum > curbuf->b_ml.ml_line_count)
	end_lnum = curbuf->b_ml.ml_line_count;
    nếu (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    line = get_line_and_copy(lnum, file_line);

    cho (;;)
    {
	nếu (incl_regmatch.regprog != NULL
		&& vim_regexec(&incl_regmatch, line, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    nếu (strstr((char *)inc_opt, "\\zs") != NULL)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (số_nguyên)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    không_thì
		// Use text after match with 'include'.
		new_fname = file_name_in_line(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, NULL);
	    already_searched = FALSE;
	    nếu (new_fname != NULL)
	    {
		// Check whether we have already searched in this file
		cho (i = 0;; i++)
		{
		    nếu (i == depth + 1)
			i = old_files;
		    nếu (i == max_path_depth)
			break;
		    nếu (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			nếu (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    msg_putchar('\n');	    // cursor below last one
			    nếu (!got_int)	    // don't display if 'q'
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

	    nếu (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == NULL && !already_searched)))
	    {
		nếu (did_show)
		    msg_putchar('\n');	    // cursor below last one
		không_thì
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    msg_puts_title(_("--- Included files "));
		    nếu (action != ACTION_SHOW_ALL)
			msg_puts_title(_("not found "));
		    msg_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		trong_khi (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    cho (i = 0; i < depth_displayed; i++)
			msg_puts("  ");
		    msg_home_replace(files[depth_displayed].name);
		    msg_puts(" -->\n");
		}
		nếu (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    cho (i = 0; i <= depth_displayed; i++)
			msg_puts("  ");
		    nếu (new_fname != NULL)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			msg_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    không_thì
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			nếu (strstr((char *)inc_opt, "\\zs") != NULL)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (số_nguyên)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			không_thì
			{
			    // find the file name after the end of the match
			    cho (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    cho (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			nếu (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (số_nguyên)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			không_thì nếu (p > line)
			{
			    nếu (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    nếu (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			msg_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    nếu (new_fname == NULL && action == ACTION_SHOW_ALL)
		    {
			nếu (already_searched)
			    msg_puts(_("  (Already listed)"));
			không_thì
			    msg_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    nếu (new_fname != NULL)
	    {
		// Push the new file onto the file stack
		nếu (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    nếu (bigger != NULL)
		    {
			cho (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			cho (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = NULL;
			    bigger[i].name = NULL;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			cho (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		nếu ((files[depth + 1].fp = mch_fopen((char *)new_fname, "r"))
								    == NULL)
		    vim_free(new_fname);
		không_thì
		{
		    nếu (++depth == old_files)
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
		    nếu (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			msg_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((char*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(char *)new_fname);
			msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    không_thì nếu (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (char *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	không_thì
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = line;
search_line:
	    define_matched = FALSE;
	    nếu (def_regmatch.regprog != NULL
			      && vim_regexec(&def_regmatch, line, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		trong_khi (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    nếu (def_regmatch.regprog == NULL || define_matched)
	    {
		nếu (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    nếu (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    không_thì
			matched = STRNCMP(startp, ptr, len) == 0;
		    nếu (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		không_thì nếu (regmatch.regprog != NULL
			 && vim_regexec(&regmatch, line, (colnr_T)(p - line)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    nếu (!define_matched && skip_comments)
		    {
			nếu ((*line != '#' ||
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
			nếu (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    cho (p = line; *p && p < startp; ++p)
			    {
				nếu (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    nếu (p[1] == '/')
					break;
				    ++p;
				}
				không_thì nếu (!matched && p[0] == '*' && p[1] == '/')
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
	nếu (matched)
	{
	    nếu (action == ACTION_EXPAND)
	    {
		số_nguyên	cont_s_ipos = FALSE;
		số_nguyên	add_r;
		char_u	*aux;

		nếu (depth == -1 && lnum == curwin->w_cursor.lnum)
		    break;
		found = TRUE;
		aux = p = startp;
		nếu (compl_status_adding() && (số_nguyên)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    nếu (vim_iswordp(p))
			goto exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (số_nguyên)(p - aux);

		nếu (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    nếu (depth < 0)
		    {
			nếu (lnum >= end_lnum)
			    goto exit_matched;
			line = get_line_and_copy(++lnum, file_line);
		    }
		    không_thì nếu (vim_fgets(line = file_line,
						      LSIZE, files[depth].fp))
			goto exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(line);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    nếu (p > aux)
		    {
			nếu (*aux != ')' && IObuff[i-1] != TAB)
			{
			    nếu (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    nếu (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == NULL
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			nếu (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (số_nguyên)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    nếu (i == ins_compl_len())
			goto exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? NULL
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		nếu (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		không_thì nếu (add_r == FAIL)
		    break;
	    }
	    không_thì nếu (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		nếu (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		nếu (curr_fname != prev_fname)
		{
		    nếu (did_show)
			msg_putchar('\n');	// cursor below last one
		    nếu (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			msg_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		nếu (!got_int)
		    show_pat_in_path(line, type, TRUE, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_count++);

		// Set matched flag for this file and all the ones that
		// include it
		cho (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    không_thì nếu (--count <= 0)
	    {
		found = TRUE;
		nếu (depth == -1 && lnum == curwin->w_cursor.lnum
# nếu defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_line));
		không_thì nếu (action == ACTION_SHOW)
		{
		    show_pat_in_path(line, type, did_show, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		không_thì
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# nếu defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    nếu (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    nếu (action == ACTION_SPLIT)
		    {
			nếu (win_split(0, 0) == FAIL)
			    break;
			RESET_BINDING(curwin);
		    }
		    nếu (depth == -1)
		    {
			// match in current file
# nếu defined(FEAT_QUICKFIX)
			nếu (g_do_tagpreview != 0)
			{
			    nếu (!win_valid(curwin_save))
				break;
			    nếu (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, NULL,
						     NULL, TRUE, lnum, forceit)))
				break;	// failed to jump to file
			}
			không_thì
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    không_thì
		    {
			nếu (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, NULL, TRUE,
						    files[depth].lnum, forceit)))
			    break;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		nếu (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - line);
		    curwin->w_set_curswant = true;
		}

# nếu defined(FEAT_QUICKFIX)
		nếu (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		không_thì nếu (WIN_IS_POPUP(curwin))
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
	    nếu (def_regmatch.regprog == NULL
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		goto search_line;
	}
	line_breakcheck();
	nếu (action == ACTION_EXPAND)
	    ins_compl_check_keys(30, FALSE);
	nếu (got_int || ins_compl_interrupted())
	    break;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	trong_khi (depth >= 0 && !already
		&& vim_fgets(line = file_line, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    nếu (depth < depth_displayed)
		depth_displayed = depth;
	}
	nếu (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (số_nguyên)STRLEN(line);
	    nếu (i > 0 && line[i - 1] == '\n')
		line[--i] = NUL;
	    nếu (i > 0 && line[i - 1] == '\r')
		line[--i] = NUL;
	}
	không_thì nếu (!already)
	{
	    nếu (++lnum > end_lnum)
		break;
	    line = get_line_and_copy(lnum, file_line);
	}
	already = NULL;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    cho (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    cho (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    nếu (type == CHECK_PATH)
    {
	nếu (!did_show)
	{
	    nếu (action != ACTION_SHOW_ALL)
		msg(_("All included files were found"));
	    không_thì
		msg(_("No included files"));
	}
    }
    không_thì nếu (!found && action != ACTION_EXPAND && !silent)
    {
	nếu (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	không_thì nếu (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	không_thì
	    emsg(_(e_couldnt_find_pattern));
    }
    nếu (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
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
    số_nguyên	    type,
    số_nguyên	    did_show,
    số_nguyên	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;
    size_t  linelen;

    nếu (did_show)
	msg_putchar('\n');	// cursor below last one
    không_thì nếu (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    nếu (got_int)		// 'q' typed at "--more--" message
	trả_về;
    linelen = STRLEN(line);
    cho (;;)
    {
	p = line + linelen - 1;
	nếu (fp != NULL)
	{
	    // We used fgets(), so get rid of newline at end
	    nếu (p >= line && *p == '\n')
		--p;
	    nếu (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	nếu (action == ACTION_SHOW_ALL)
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
	nếu (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    break;

	nếu (fp != NULL)
	{
	    nếu (vim_fgets(line, LSIZE, fp)) // end of file
		break;
	    linelen = STRLEN(line);
	    ++*lnum;
	}
	không_thì
	{
	    nếu (++*lnum > curbuf->b_ml.ml_line_count)
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
get_spat(số_nguyên idx)
{
    trả_về &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    số_nguyên
get_spat_last_idx(void)
{
    trả_về last_idx;
}
#endif

#nếu defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    void
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = NULL;
    số_nguyên			maxcount = p_msc;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    số_nguyên			recompute = TRUE;
    searchstat_T	stat;

    nếu (rettv_dict_alloc(rettv) == FAIL)
	trả_về;

    nếu (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	trả_về;

    nếu (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    nếu (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	số_nguyên		error = FALSE;

	nếu (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    trả_về;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	nếu (di != NULL)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    nếu (error)
		trả_về;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	nếu (di != NULL)
	{
	    maxcount = (số_nguyên)tv_get_number_chk(&di->di_tv, &error);
	    nếu (error)
		trả_về;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	nếu (di != NULL)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    nếu (pattern == NULL)
		trả_về;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	nếu (di != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		trả_về;
	    }
	    nếu (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		trả_về;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    nếu (li != NULL)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		nếu (error)
		    trả_về;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    nếu (li != NULL)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		nếu (error)
		    trả_về;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    nếu (li != NULL)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		nếu (error)
		    trả_về;
	    }
	}
    }

    save_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
# endif
    nếu (pattern != NULL)
    {
	nếu (*pattern == NUL)
	    goto the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	nếu (spats[last_idx].pat == NULL)
	    spats[last_idx].patlen = 0;
    }
    nếu (spats[last_idx].pat == NULL || *spats[last_idx].pat == NUL)
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
