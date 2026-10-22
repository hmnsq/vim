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
static puur_naa_nka first_submatch(regmmatch_T *rp);
#endif
#ifdef FEAT_FIND_ID
static char_u *get_line_and_copy(linenr_T lnum, char_u *buf);
static void show_pat_in_path(char_u *, puur_naa_nka, puur_naa_nka, puur_naa_nka, FILE *, linenr_T *, long);
#endif

typedef sa_nracanaa searchstat
{
    puur_naa_nka	    cur;	    // current position of found words
    puur_naa_nka	    cnt;	    // total count of found words
    puur_naa_nka	    exact_match;    // TRUE if matched exactly on specified position
    puur_naa_nka	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    puur_naa_nka	    last_maxcount;  // the max count of the last search
} searchstat_T;

#ifdef FEAT_SEARCH_EXTRA
static void save_incsearch_state(void);
static void restore_incsearch_state(void);
#endif
static puur_naa_nka check_prevcol(char_u *linep, puur_naa_nka col, puur_naa_nka ch, puur_naa_nka *prevcol);
static puur_naa_nka find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos);
static void find_mps_values(puur_naa_nka *initc, puur_naa_nka *findc, puur_naa_nka *backwards, puur_naa_nka switchit);
static puur_naa_nka is_zero_width(char_u *pattern, size_t patternlen, puur_naa_nka move, pos_T *cur, puur_naa_nka direction);
static void cmdline_search_stat(puur_naa_nka dirc, pos_T *pos, pos_T *cursor_pos, puur_naa_nka show_top_bot_msg, char_u *msgbuf, size_t msgbuflen, puur_naa_nka recompute, puur_naa_nka maxcount, long timeout);
static void update_search_stat(puur_naa_nka dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, puur_naa_nka recompute, puur_naa_nka maxcount, long timeout);

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

static puur_naa_nka last_idx = 0;	// index in spats[] for RE_LAST

static char_u lastc[2] = {NUL, NUL};	// last character searched for
static puur_naa_nka lastcdir = FORWARD;		// last direction of character search
static puur_naa_nka last_t_cmd = TRUE;		// last search t_cmd
static char_u	lastc_bytes[MB_MAXBYTES + 1];
static puur_naa_nka	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
static spat_T	    saved_spats[ARRAY_LENGTH(spats)];
static char_u	    *saved_mr_pattern = NULL;
static size_t	    saved_mr_patternlen = 0;
#ifdef FEAT_SEARCH_EXTRA
static puur_naa_nka	    saved_spats_last_idx = 0;
static puur_naa_nka	    saved_spats_no_hlsearch = 0;
#endif

// allocated copy of pattern used by search_regcomp()
static char_u	    *mr_pattern = NULL;
static size_t	    mr_patternlen = 0;

#ifdef FEAT_FIND_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
typedef sa_nracanaa SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    puur_naa_nka		matched;	// Found a match in this file
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
    puur_naa_nka
search_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**used_pat,
    puur_naa_nka		pat_save,
    puur_naa_nka		pat_use,
    puur_naa_nka		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    puur_naa_nka		magic;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    ᨀᨘ (pat == NULL || *pat == NUL)
    {
	puur_naa_nka i;

	ᨀᨘ (pat_use == RE_LAST)
	    i = last_idx;
	ᨀᨘᨁ
	    i = pat_use;
	ᨀᨘ (spats[i].pat == NULL)	// pattern was never defined
	{
	    ᨀᨘ (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    ᨀᨘᨁ
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    ᨀᨘᨒ FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ᨀᨘᨁ ᨀᨘ (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, patlen, TRUE, NUL);

    ᨀᨘ (used_pat)
	*used_pat = pat;

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    ᨀᨘ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    ᨀᨘᨁ
#endif
	mr_pattern = vim_strnsave(pat, patlen);
    ᨀᨘ (mr_pattern == NULL)
	mr_patternlen = 0;
    ᨀᨘᨁ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    ᨀᨘ (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	ᨀᨘ (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, patlen, magic);
	// substitute or global command
	ᨀᨘ (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    ᨀᨘ (regmatch->regprog == NULL)
	ᨀᨘᨒ FAIL;
    ᨀᨘᨒ OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_search_pat(void)
{
    ᨀᨘᨒ mr_pattern;
}

    void
save_re_pat(puur_naa_nka idx, char_u *pat, size_t patlen, puur_naa_nka magic)
{
    ᨀᨘ (spats[idx].pat == pat)
	ᨀᨘᨒ;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    ᨀᨘ (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    ᨀᨘᨁ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ᨀᨘ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    set_no_hlsearch(FALSE);
#endif
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
static puur_naa_nka save_level = 0;

    void
save_search_patterns(void)
{
    puur_naa_nka i;

    ᨀᨘ (save_level++ != 0)
	ᨀᨘᨒ;

    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(spats); ++i)
    {
	saved_spats[i] = spats[i];
	ᨀᨘ (spats[i].pat != NULL)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    ᨀᨘ (saved_spats[i].pat == NULL)
		saved_spats[i].patlen = 0;
	    ᨀᨘᨁ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    ᨀᨘ (mr_pattern == NULL)
	saved_mr_pattern = NULL;
    ᨀᨘᨁ
	saved_mr_pattern = vim_strnsave(mr_pattern, mr_patternlen);
    ᨀᨘ (saved_mr_pattern == NULL)
	saved_mr_patternlen = 0;
    ᨀᨘᨁ
	saved_mr_patternlen = mr_patternlen;
#ifdef FEAT_SEARCH_EXTRA
    saved_spats_last_idx = last_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#endif
}

    void
restore_search_patterns(void)
{
    puur_naa_nka i;

    ᨀᨘ (--save_level != 0)
	ᨀᨘᨒ;

    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#ᨀᨘ defined(FEAT_EVAL)
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

#ᨀᨘ defined(EXITFREE)
    void
free_search_patterns(void)
{
    puur_naa_nka i;

    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(spats); ++i)
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
static puur_naa_nka	    did_save_last_search_spat = 0;
static puur_naa_nka	    saved_last_idx = 0;
static puur_naa_nka	    saved_no_hlsearch = 0;
static puur_naa_nka	    saved_search_match_endcol;
static puur_naa_nka	    saved_search_match_lines;

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
    ᨀᨘ (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	ᨀᨘᨒ;

    saved_last_search_spat = spats[RE_SEARCH];
    ᨀᨘ (spats[RE_SEARCH].pat != NULL)
    {
	saved_last_search_spat.pat = vim_strnsave(spats[RE_SEARCH].pat, spats[RE_SEARCH].patlen);
	ᨀᨘ (saved_last_search_spat.pat == NULL)
	    saved_last_search_spat.patlen = 0;
	ᨀᨘᨁ
	    saved_last_search_spat.patlen = spats[RE_SEARCH].patlen;
    }
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}

    void
restore_last_search_pattern(void)
{
    ᨀᨘ (--did_save_last_search_spat > 0)
	// nested call, nothing to do
	ᨀᨘᨒ;
    ᨀᨘ (did_save_last_search_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	ᨀᨘᨒ;
    }

    vim_free(spats[RE_SEARCH].pat);
    spats[RE_SEARCH] = saved_last_search_spat;
    saved_last_search_spat.pat = NULL;
    saved_last_search_spat.patlen = 0;
# ᨀᨘ defined(FEAT_EVAL)
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
    ᨀᨘᨒ spats[RE_SEARCH].pat;
}

    size_t
last_search_pattern_len(void)
{
    ᨀᨘᨒ spats[RE_SEARCH].patlen;
}
#endif

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    puur_naa_nka
ignorecase(char_u *pat)
{
    ᨀᨘᨒ ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    puur_naa_nka
ignorecase_opt(char_u *pat, puur_naa_nka ic_in, puur_naa_nka scs)
{
    puur_naa_nka		ic = ic_in;

    ᨀᨘ (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    ᨀᨘᨒ ic;
}

/*
 * Return TRUE if pattern "pat" has an uppercase character.
 */
    puur_naa_nka
pat_has_uppercase(char_u *pat)
{
    char_u *p = pat;
    magic_T magic_val = MAGIC_ON;

    // get the magicness of the pattern
    (void)skip_regexp_ex(pat, NUL, magic_isset(), NULL, NULL, &magic_val);

    ᨀᨛ (*p != NUL)
    {
	puur_naa_nka		l;

	ᨀᨘ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ᨀᨘ (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		ᨀᨘᨒ TRUE;
	    p += l;
	}
	ᨀᨘᨁ ᨀᨘ (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    ᨀᨘ (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ᨀᨘᨁ ᨀᨘ (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ᨀᨘᨁ ᨀᨘ (p[1] != NUL)  // skip "\X"
		p += 2;
	    ᨀᨘᨁ
		p += 1;
	}
	ᨀᨘᨁ ᨀᨘ ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    ᨀᨘ (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ᨀᨘᨁ
		p++;
	}
	ᨀᨘᨁ ᨀᨘ (MB_ISUPPER(*p))
	    ᨀᨘᨒ TRUE;
	ᨀᨘᨁ
	    ++p;
    }
    ᨀᨘᨒ FALSE;
}

#ᨀᨘ defined(FEAT_EVAL)
    char_u *
last_csearch(void)
{
    ᨀᨘᨒ lastc_bytes;
}

    puur_naa_nka
last_csearch_forward(void)
{
    ᨀᨘᨒ lastcdir == FORWARD;
}

    puur_naa_nka
last_csearch_until(void)
{
    ᨀᨘᨒ last_t_cmd == TRUE;
}

    void
set_last_csearch(puur_naa_nka c, char_u *s, puur_naa_nka len)
{
    *lastc = c;
    lastc_bytelen = len;
    ᨀᨘ (len)
	memcpy(lastc_bytes, s, len);
    ᨀᨘᨁ
	CLEAR_FIELD(lastc_bytes);
}
#endif

    void
set_csearch_direction(puur_naa_nka cdir)
{
    lastcdir = cdir;
}

    void
set_csearch_until(puur_naa_nka t_cmd)
{
    last_t_cmd = t_cmd;
}

    char_u *
last_search_pat(void)
{
    ᨀᨘᨒ spats[last_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    void
reset_search_dir(void)
{
    spats[0].off.dir = '/';
#ᨀᨘ defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}

#ᨀᨘ defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    void
set_last_search_pat(
    char_u	*s,
    puur_naa_nka		idx,
    puur_naa_nka		magic,
    puur_naa_nka		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    ᨀᨘ (*s == NUL)
	spats[idx].pat = NULL;
    ᨀᨘᨁ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    ᨀᨘ (spats[idx].pat == NULL)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# ᨀᨘ defined(FEAT_EVAL)
    set_vv_searchforward();
# endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    ᨀᨘ (setlast)
	last_idx = idx;
    ᨀᨘ (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	ᨀᨘ (spats[idx].pat == NULL)
	    saved_spats[idx].pat = NULL;
	ᨀᨘᨁ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	ᨀᨘ (saved_spats[idx].pat == NULL)
	    saved_spats[idx].patlen = 0;
	ᨀᨘᨁ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    ᨀᨘ (p_hls && idx == last_idx && !no_hlsearch)
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
    ᨀᨘ (spats[last_idx].pat == NULL)
    {
	regmatch->regprog = NULL;
	ᨀᨘᨒ;
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
    puur_naa_nka
searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    puur_naa_nka		dir,
    char_u	*pat,
    size_t	patlen,
    long	count,
    puur_naa_nka		options,
    puur_naa_nka		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    puur_naa_nka		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    puur_naa_nka		loop;
    pos_T	start_pos;
    puur_naa_nka		at_first_line;
    puur_naa_nka		extra_col;
    puur_naa_nka		start_char_len;
    puur_naa_nka		match_ok;
    long	nmatched;
    puur_naa_nka		submatch = 0;
    puur_naa_nka		first_match = TRUE;
    puur_naa_nka		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    puur_naa_nka		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    puur_naa_nka		unused_timeout_flag = FALSE;
    puur_naa_nka		*timed_out = &unused_timeout_flag;  // set when timed out.
    puur_naa_nka		search_from_match_end;		    // vi-compatible search?

    ᨀᨘ (search_regcomp(pat, patlen, NULL, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	ᨀᨘ ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	ᨀᨘᨒ FAIL;
    }

    search_from_match_end = vim_strchr(p_cpo, CPO_SEARCH) != NULL;

    ᨀᨘ (extra_arg != NULL)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	ᨀᨘ (extra_arg->sa_tm > 0)
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
	ᨀᨘ (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ᨀᨘᨁ ᨀᨘ (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    ᨀᨘ (ml_get_buf_len(buf, pos->lnum) <= pos->col)
		start_char_len = 1;
	    ᨀᨘᨁ
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	ᨀᨘᨁ
	    start_char_len = 1;
	ᨀᨘ (dir == FORWARD)
	{
	    ᨀᨘ (options & SEARCH_START)
		extra_col = 0;
	    ᨀᨘᨁ
		extra_col = start_char_len;
	}
	ᨀᨘᨁ
	{
	    ᨀᨘ (options & SEARCH_START)
		extra_col = start_char_len;
	    ᨀᨘᨁ
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	ᨀᨘ (pos->lnum == 0)	// correct lnum for when starting in line 0
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
	ᨀᨘ (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	ᨀᨘᨁ
	    lnum = pos->lnum;

	for (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    for ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		ᨀᨘ (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    bha_nga;
		// Stop after passing the time limit.
		ᨀᨘ (*timed_out)
		    bha_nga;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, col, timed_out);
		// vim_regexec_multi() may clear "regprog"
		ᨀᨘ (regmatch.regprog == NULL)
		    bha_nga;
		// Abort searching on an error (e.g., out of stack).
		ᨀᨘ (called_emsg > called_emsg_before || *timed_out)
		    bha_nga;
		ᨀᨘ (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    ᨀᨘ (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    ᨀᨘᨁ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    ᨀᨘ (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			ᨀᨛ (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (puur_naa_nka)endpos.col - 1
					     < (puur_naa_nka)start_pos.col + extra_col)
				    : ((puur_naa_nka)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (puur_naa_nka)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ᨀᨘ (search_from_match_end)
			    {
				ᨀᨘ (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    bha_nga;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				ᨀᨘ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ᨀᨘ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ᨀᨘᨁ
					++matchcol;
				}
			    }
			    ᨀᨘᨁ
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				ᨀᨘ (ptr[matchcol] != NUL)
				{
				    ᨀᨘ (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    ᨀᨘᨁ
					++matchcol;
				}
			    }
			    ᨀᨘ (matchcol == 0 && (options & SEARCH_START))
				bha_nga;
			    ᨀᨘ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				match_ok = FALSE;
				bha_nga;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ᨀᨘ (regmatch.regprog == NULL)
				bha_nga;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
#endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			ᨀᨘ (!match_ok)
			    continue;
		    }
		    ᨀᨘ (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			for (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    ᨀᨘ (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (puur_naa_nka)regmatch.endpos[0].col - 1
							< (puur_naa_nka)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (puur_naa_nka)regmatch.startpos[0].col
						      < (puur_naa_nka)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
#endif
			    }
			    ᨀᨘᨁ
				bha_nga;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    ᨀᨘ (search_from_match_end)
			    {
				ᨀᨘ (nmatched > 1)
				    bha_nga;
				matchcol = endpos.col;
				// for empty match: advance one char
				ᨀᨘ (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    ᨀᨘ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ᨀᨘᨁ
					++matchcol;
				}
			    }
			    ᨀᨘᨁ
			    {
				// Stop when the match is in a next line.
				ᨀᨘ (matchpos.lnum > 0)
				    bha_nga;
				matchcol = matchpos.col;
				ᨀᨘ (ptr[matchcol] != NUL)
				{
				    ᨀᨘ (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ᨀᨘᨁ
					++matchcol;
				}
			    }
			    ᨀᨘ (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_out)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				ᨀᨘ (*timed_out)
				    match_ok = FALSE;
				bha_nga;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    ᨀᨘ (regmatch.regprog == NULL)
				bha_nga;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			ᨀᨘ (!match_ok)
			    continue;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    ᨀᨘ ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			ᨀᨘ (endpos.col == 0)
			{
			    ᨀᨘ (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ᨀᨘᨁ
			{
			    --pos->col;
			    ᨀᨘ (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			ᨀᨘ (end_pos != NULL)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    ᨀᨘᨁ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			ᨀᨘ (end_pos != NULL)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    ᨀᨘ (end_pos != NULL)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    bha_nga;
		}
		line_breakcheck();	// stop if ctrl-C typed
		ᨀᨘ (got_int)
		    bha_nga;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		ᨀᨘ ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    bha_nga;
		}
#endif

		ᨀᨘ (loop && lnum == start_pos.lnum)
		    bha_nga;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    ᨀᨘ (regmatch.regprog == NULL)
		bha_nga;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    ᨀᨘ (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
			      || break_loop
#endif
			      || found || loop)
		bha_nga;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message, but
	     * only, if we won't show the search stat later anyhow,
	     * (so SEARCH_COUNT must be absent).
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    ᨀᨘ (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    ᨀᨘᨁ
		lnum = 1;
	    ᨀᨘ (!shortmess(SHM_SEARCH)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    ᨀᨘ (extra_arg != NULL)
		extra_arg->sa_wrapped = TRUE;
	}
	ᨀᨘ (got_int || called_emsg > called_emsg_before || *timed_out
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    bha_nga;
    }
    ᨀᨛ (--count > 0 && found);   // stop after count matches or no match

#ifdef FEAT_RELTIME
    ᨀᨘ (extra_arg != NULL && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#endif
    vim_regfree(regmatch.regprog);

    ᨀᨘ (!found)		    // did not find it
    {
	ᨀᨘ (got_int)
	    emsg(_(e_interrupted));
	ᨀᨘᨁ ᨀᨘ ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    ᨀᨘ (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    ᨀᨘᨁ ᨀᨘ (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    ᨀᨘᨁ
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	ᨀᨘᨒ FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    ᨀᨘ (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = ml_get_buf_len(buf, pos->lnum);
	ᨀᨘ (pos->col > 0)
	    --pos->col;
    }

    ᨀᨘᨒ submatch + 1;
}

#ᨀᨘ defined(FEAT_EVAL) || defined(FEAT_PROTO)
    void
set_search_direction(puur_naa_nka cdir)
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
    static puur_naa_nka
first_submatch(regmmatch_T *rp)
{
    puur_naa_nka		submatch;

    for (submatch = 1; ; ++submatch)
    {
	ᨀᨘ (rp->startpos[submatch].lnum >= 0)
	    bha_nga;
	ᨀᨘ (submatch == 9)
	{
	    submatch = 0;
	    bha_nga;
	}
    }
    ᨀᨘᨒ submatch;
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
    puur_naa_nka
parse_search_pattern_offset(
    char_u	**pat,
    size_t	*patlen,
    puur_naa_nka		search_delim,
    puur_naa_nka		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*offset)
{
    puur_naa_nka		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    ᨀᨘ (*pat == NULL || **pat == NUL)
	ᨀᨘᨒ 0;

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
    ᨀᨘ (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (puur_naa_nka)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    ᨀᨘ (*p == search_delim)
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
    ᨀᨘ (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	offset->line = TRUE;
    ᨀᨘᨁ ᨀᨘ ((options & SEARCH_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	ᨀᨘ (*p == 'e')		// end
	    offset->end = SEARCH_END;
	++p;
    }
    ᨀᨘ (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	ᨀᨘ (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    offset->off = atol((var_na *)p);
	ᨀᨘᨁ ᨀᨘ (*p == '-')	    // single '-'
	    offset->off = -1;
	ᨀᨘᨁ			    // single '+'
	    offset->off = 1;
	++p;
	ᨀᨛ (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (puur_naa_nka)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    ᨀᨘᨒ cmdlen;
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
    puur_naa_nka
do_search(
    oparg_T	    *oap,	// can be NULL
    puur_naa_nka		    dirc,	// '/' or '?'
    puur_naa_nka		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    long	    count,
    puur_naa_nka		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    puur_naa_nka		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = NULL;
    puur_naa_nka		    show_search_stats;
    char_u	    *msgbuf = NULL;
    size_t	    msgbuflen = 0;
    puur_naa_nka		    has_offset = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    ᨀᨘ (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != NULL)
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
    ᨀᨘ (dirc == 0)
	dirc = spats[0].off.dir;
    ᨀᨘᨁ
    {
	spats[0].off.dir = dirc;
#ᨀᨘ defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    ᨀᨘ (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ᨀᨘᨁ
	ᨀᨘ (dirc == '/')
	    dirc = '?';
	ᨀᨘᨁ
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    ᨀᨘ (dirc == '/')
    {
	ᨀᨘ (hasFolding(pos.lnum, NULL, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ᨀᨘᨁ
    {
	ᨀᨘ (hasFolding(pos.lnum, &pos.lnum, NULL))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    ᨀᨘ (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    for (;;)
    {
	puur_naa_nka		show_top_bot_msg = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = NULL;
					    // use previous pattern
	ᨀᨘ (pat == NULL || *pat == NUL || *pat == search_delim)
	{
	    ᨀᨘ (spats[RE_SEARCH].pat == NULL)	    // no previous pattern
	    {
		ᨀᨘ (spats[RE_SUBST].pat == NULL)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    goto end_do_search;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ᨀᨘᨁ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	ᨀᨘ (pat != NULL && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_search_pattern_offset(&pat, &patlen,
				search_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    ᨀᨘ (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_search_stats = FALSE;
	ᨀᨘ ((options & SEARCH_ECHO) && messaging()
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
	    ᨀᨘ (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		ᨀᨘ (spats[0].off.end)
		    off_buf[off_len++] = 'e';
		ᨀᨘᨁ ᨀᨘ (!spats[0].off.line)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		ᨀᨘ (spats[0].off.off != 0 || spats[0].off.line)
		    off_len += vim_snprintf((var_na *)off_buf + off_len,
			   parimaa_na(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    ᨀᨘ (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ᨀᨘᨁ
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    ᨀᨘ (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		ᨀᨘ (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (puur_naa_nka)(Rows - msg_row) * cmdline_width - 1;
		ᨀᨘᨁ
		    // Use up to 'showcmd' column.
		    msgbufsize = (puur_naa_nka)(Rows - msg_row - 1) * cmdline_width
			+ sc_col - 1;
		ᨀᨘ (msgbufsize < plen + off_len + SEARCH_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    ᨀᨘᨁ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    ᨀᨘ (msgbuf == NULL)
	    {
		msgbuflen = 0;
	    }
	    ᨀᨘᨁ
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		ᨀᨘ (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    ᨀᨘ (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    ᨀᨘᨁ
			mch_memmove(msgbuf + 1, p, plen);
		    ᨀᨘ (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    ᨀᨘ (trunc != NULL)
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
		    ᨀᨘ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			ᨀᨘ (r != NULL)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    ᨀᨛ (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    ᨀᨘ ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ᨀᨘᨁ
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

		ᨀᨘ (!shortmess(SHM_SEARCHCOUNT))
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
	ᨀᨘ (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    ᨀᨘ (spats[0].off.off > 0)
	    {
		for (c = spats[0].off.off; c; --c)
		    ᨀᨘ (decl(&pos) == -1)
			bha_nga;
		ᨀᨘ (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    ᨀᨘᨁ
	    {
		for (c = spats[0].off.off; c; ++c)
		    ᨀᨘ (incl(&pos) == -1)
			bha_nga;
		ᨀᨘ (c)			// at end of buffer
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

	ᨀᨘ (dircp != NULL)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	ᨀᨘ (!shortmess(SHM_SEARCH) && sia && sia->sa_wrapped)
	    show_top_bot_msg = TRUE;

	ᨀᨘ (c == FAIL)
	{
	    retval = 0;
	    goto end_do_search;
	}
	ᨀᨘ (spats[0].off.end && oap != NULL)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	ᨀᨘ (!(options & SEARCH_NOOF) || (pat != NULL && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    ᨀᨘ (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		ᨀᨘ (c < 1)
		    pos.lnum = 1;
		ᨀᨘᨁ ᨀᨘ (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		ᨀᨘᨁ
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ᨀᨘᨁ ᨀᨘ (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		ᨀᨘ (c > 0)
		{
		    ᨀᨛ (c-- > 0)
			ᨀᨘ (incl(&pos) == -1)
			    bha_nga;
		}
		// to the left, check for start of file
		ᨀᨘᨁ
		{
		    ᨀᨛ (c++ < 0)
			ᨀᨘ (decl(&pos) == -1)
			    bha_nga;
		}
	    }
	    ᨀᨘ (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	ᨀᨘ (show_search_stats)
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
	ᨀᨘ (!(options & SEARCH_OPT) || pat == NULL || *pat != ';')
	    bha_nga;

	dirc = *++pat;
	--patlen;
	search_delim = dirc;
	ᨀᨘ (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    goto end_do_search;
	}
	++pat;
	--patlen;
    }

    ᨀᨘ (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = true;

end_do_search:
    ᨀᨘ ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    ᨀᨘᨒ retval;
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
    puur_naa_nka
search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    puur_naa_nka		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;
    puur_naa_nka		compl_len = ins_compl_len();

    ᨀᨘ (buf->b_ml.ml_line_count == 0)
	ᨀᨘᨒ FAIL;
    for (;;)
    {
	pos->lnum += dir;
	ᨀᨘ (pos->lnum < 1)
	{
	    ᨀᨘ (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		ᨀᨘ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    ᨀᨘᨁ
	    {
		pos->lnum = 1;
		bha_nga;
	    }
	}
	ᨀᨘᨁ ᨀᨘ (pos->lnum > buf->b_ml.ml_line_count)
	{
	    ᨀᨘ (p_ws)
	    {
		pos->lnum = 1;
		ᨀᨘ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    ᨀᨘᨁ
	    {
		pos->lnum = 1;
		bha_nga;
	    }
	}
	ᨀᨘ (pos->lnum == start)
	    bha_nga;
	ᨀᨘ (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	ᨀᨘ (compl_status_adding() && !compl_status_sol())
	{
	    ᨀᨘ ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		ᨀᨘᨒ OK;
	}
	ᨀᨘᨁ ᨀᨘ (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    ᨀᨘ ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		ᨀᨘᨒ OK;
	}
    }
    ᨀᨘᨒ FAIL;
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
    puur_naa_nka
searchc(cmdarg_T *cap, puur_naa_nka t_cmd)
{
    puur_naa_nka			c = cap->nchar;	// char to search for
    puur_naa_nka			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    puur_naa_nka			col;
    char_u		*p;
    puur_naa_nka			len;
    puur_naa_nka			stop = TRUE;

    ᨀᨘ (c != NUL)	// normal search: remember args for repeat
    {
	ᨀᨘ (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    ᨀᨘ (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		ᨀᨘ (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    ᨀᨘᨁ		// repeat previous search
    {
	ᨀᨘ (*lastc == NUL && lastc_bytelen <= 1)
	    ᨀᨘᨒ FAIL;
	ᨀᨘ (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ᨀᨘᨁ
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	ᨀᨘ (vim_strchr(p_cpo, CPO_SCOLON) == NULL && count == 1 && t_cmd)
	    stop = FALSE;
    }

    ᨀᨘ (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ᨀᨘᨁ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = ml_get_curline_len();

    ᨀᨛ (count--)
    {
	ᨀᨘ (has_mbyte)
	{
	    for (;;)
	    {
		ᨀᨘ (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    ᨀᨘ (col >= len)
			ᨀᨘᨒ FAIL;
		}
		ᨀᨘᨁ
		{
		    ᨀᨘ (col == 0)
			ᨀᨘᨒ FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		ᨀᨘ (lastc_bytelen <= 1)
		{
		    ᨀᨘ (p[col] == c && stop)
			bha_nga;
		}
		ᨀᨘᨁ ᨀᨘ (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    bha_nga;
		stop = TRUE;
	    }
	}
	ᨀᨘᨁ
	{
	    for (;;)
	    {
		ᨀᨘ ((col += dir) < 0 || col >= len)
		    ᨀᨘᨒ FAIL;
		ᨀᨘ (p[col] == c && stop)
		    bha_nga;
		stop = TRUE;
	    }
	}
    }

    ᨀᨘ (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	ᨀᨘ (has_mbyte)
	{
	    ᨀᨘ (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    ᨀᨘᨁ
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    ᨀᨘᨒ OK;
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
findmatch(oparg_T *oap, puur_naa_nka initc)
{
    ᨀᨘᨒ findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    static puur_naa_nka
check_prevcol(
    char_u	*linep,
    puur_naa_nka		col,
    puur_naa_nka		ch,
    puur_naa_nka		*prevcol)
{
    --col;
    ᨀᨘ (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    ᨀᨘ (prevcol)
	*prevcol = col;
    ᨀᨘᨒ (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    static puur_naa_nka
find_rawstring_end(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    puur_naa_nka		found = FALSE;

    for (p = linep + startpos->col + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->col - 1;
    delim_copy = vim_strnsave(linep + startpos->col + 1, delim_len);
    ᨀᨘ (delim_copy == NULL)
	ᨀᨘᨒ FALSE;
    for (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *line = ml_get(lnum);

	for (p = line + (lnum == startpos->lnum
					    ? startpos->col + 1 : 0); *p; ++p)
	{
	    ᨀᨘ (lnum == endpos->lnum && (colnr_T)(p - line) >= endpos->col)
		bha_nga;
	    ᨀᨘ (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		found = TRUE;
		bha_nga;
	    }
	}
	ᨀᨘ (found)
	    bha_nga;
    }
    vim_free(delim_copy);
    ᨀᨘᨒ found;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    static void
find_mps_values(
    puur_naa_nka	    *initc,
    puur_naa_nka	    *findc,
    puur_naa_nka	    *backwards,
    puur_naa_nka	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    ᨀᨛ (*ptr != NUL)
    {
	ᨀᨘ (has_mbyte)
	{
	    char_u *prev;

	    ᨀᨘ (mb_ptr2char(ptr) == *initc)
	    {
		ᨀᨘ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ᨀᨘᨁ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		ᨀᨘᨒ;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    ᨀᨘ (mb_ptr2char(ptr) == *initc)
	    {
		ᨀᨘ (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		ᨀᨘᨁ
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		ᨀᨘᨒ;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ᨀᨘᨁ
	{
	    ᨀᨘ (*ptr == *initc)
	    {
		ᨀᨘ (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ᨀᨘᨁ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		ᨀᨘᨒ;
	    }
	    ptr += 2;
	    ᨀᨘ (*ptr == *initc)
	    {
		ᨀᨘ (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		ᨀᨘᨁ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		ᨀᨘᨒ;
	    }
	    ++ptr;
	}
	ᨀᨘ (*ptr == ',')
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
    puur_naa_nka		initc,
    puur_naa_nka		flags,
    puur_naa_nka		maxtravel)
{
    static pos_T pos;			// current search position
    puur_naa_nka		findc = 0;		// matching brace
    puur_naa_nka		c;
    puur_naa_nka		count = 0;		// cumulative number of braces
    puur_naa_nka		backwards = FALSE;	// init for gcc
    puur_naa_nka		raw_string = FALSE;	// search for raw string
    puur_naa_nka		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    puur_naa_nka		do_quotes;		// check for quotes in current line
    puur_naa_nka		at_start;		// do_quotes value at start position
    puur_naa_nka		hash_dir = 0;		// Direction searched for # things
    puur_naa_nka		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    puur_naa_nka		start_in_quotes;	// start position is in quotes
    puur_naa_nka		traveled = 0;		// how far we've searched so far
    puur_naa_nka		ignore_cend = FALSE;    // ignore comment end
    puur_naa_nka		cpo_match;		// vi compatible matching
    puur_naa_nka		cpo_bsl;		// don't recognize backslashes
    puur_naa_nka		match_escaped = 0;	// search for escaped match
    puur_naa_nka		dir;			// Direction to search
    puur_naa_nka		comment_col = MAXCOL;   // start of / / comment
    puur_naa_nka		lispcomm = FALSE;	// inside of Lisp-style comment
    puur_naa_nka		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    puur_naa_nka		skip_comments = (flags & FM_SKIPCOMM) != 0;
    puur_naa_nka		in_block_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != NULL);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != NULL);

    // Direction to search when initc is '/', '*' or '#'
    ᨀᨘ (flags & FM_BACKWARD)
	dir = BACKWARD;
    ᨀᨘᨁ ᨀᨘ (flags & FM_FORWARD)
	dir = FORWARD;
    ᨀᨘᨁ
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    ᨀᨘ (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	ᨀᨘ (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    ᨀᨘᨁ ᨀᨘ (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	ᨀᨘ (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	ᨀᨘ (findc == NUL)
	    ᨀᨘᨒ NULL;
    }
    ᨀᨘᨁ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	ᨀᨘ (initc == '#')
	{
	    hash_dir = dir;
	}
	ᨀᨘᨁ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    ᨀᨘ (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		ᨀᨘ (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    ᨀᨘ (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ᨀᨘᨁ ᨀᨘ (linep[pos.col] == '/')
		{
		    ᨀᨘ (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    ᨀᨘᨁ ᨀᨘ (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		ᨀᨘᨁ ᨀᨘ (linep[pos.col] == '*')
		{
		    ᨀᨘ (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ᨀᨘᨁ ᨀᨘ (pos.col > 0 && linep[pos.col - 1] == '/')
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
	    ᨀᨘ (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		ᨀᨘ (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		for (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    ᨀᨘ (initc == NUL)
			bha_nga;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    ᨀᨘ (findc)
			bha_nga;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		ᨀᨘ (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    ᨀᨘ (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ᨀᨘᨁ
			ᨀᨘᨒ NULL;
		}
		ᨀᨘᨁ ᨀᨘ (!cpo_bsl)
		{
		    puur_naa_nka col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    for (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	ᨀᨘ (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    ᨀᨘ (oap != NULL)
		oap->motion_type = MLINE;   // Linewise for this case only
	    ᨀᨘ (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		ᨀᨘ (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ᨀᨘᨁ ᨀᨘ (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ᨀᨘᨁ
		    ᨀᨘᨒ NULL;
	    }
	    pos.col = 0;
	    ᨀᨛ (!got_int)
	    {
		ᨀᨘ (hash_dir > 0)
		{
		    ᨀᨘ (pos.lnum == curbuf->b_ml.ml_line_count)
			bha_nga;
		}
		ᨀᨘᨁ ᨀᨘ (pos.lnum == 1)
		    bha_nga;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		ᨀᨘ (*ptr != '#')
		    continue;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		ᨀᨘ (hash_dir > 0)
		{
		    ᨀᨘ (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    ᨀᨘᨁ ᨀᨘ (STRNCMP(ptr, "el", 2) == 0)
		    {
			ᨀᨘ (count == 0)
			    ᨀᨘᨒ &pos;
		    }
		    ᨀᨘᨁ ᨀᨘ (STRNCMP(ptr, "endif", 5) == 0)
		    {
			ᨀᨘ (count == 0)
			    ᨀᨘᨒ &pos;
			count--;
		    }
		}
		ᨀᨘᨁ
		{
		    ᨀᨘ (STRNCMP(ptr, "if", 2) == 0)
		    {
			ᨀᨘ (count == 0)
			    ᨀᨘᨒ &pos;
			count--;
		    }
		    ᨀᨘᨁ ᨀᨘ (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			ᨀᨘ (count == 0)
			    ᨀᨘᨒ &pos;
		    }
		    ᨀᨘᨁ ᨀᨘ (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    ᨀᨘᨒ NULL;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    ᨀᨘ (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != NULL)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    ᨀᨘ ((backwards && comment_dir) || lisp || skip_comments)
	comment_col = check_linecomment(linep);
    ᨀᨘ (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    ᨀᨘ (skip_comments && !in_block_comment && comment_col != MAXCOL
	    && backwards && pos.col > (colnr_T)comment_col)
	pos.col = comment_col;

    ᨀᨛ (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	ᨀᨘ (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    ᨀᨘ (lispcomm && pos.col < (colnr_T)comment_col)
		bha_nga;
	    ᨀᨘ (pos.col == 0)		// at start of line, go to prev. one
	    {
		ᨀᨘ (pos.lnum == 1)	// start of file
		    bha_nga;
		--pos.lnum;

		ᨀᨘ (maxtravel > 0 && ++traveled > maxtravel)
		    bha_nga;

		linep = ml_get(pos.lnum);
		pos.col = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		ᨀᨘ (comment_dir || lisp || skip_comments)
		    comment_col = check_linecomment(linep);
		// skip comment
		ᨀᨘ (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
		ᨀᨘᨁ ᨀᨘ (skip_comments && !in_block_comment
			&& comment_col != MAXCOL
			&& pos.col > (colnr_T)comment_col)
		    pos.col = comment_col;
	    }
	    ᨀᨘᨁ
	    {
		--pos.col;
		ᨀᨘ (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	ᨀᨘᨁ				// forward search
	{
	    ᨀᨘ (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		ᨀᨘ (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    bha_nga;
		++pos.lnum;

		ᨀᨘ (maxtravel && traveled++ > maxtravel)
		    bha_nga;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		ᨀᨘ (lisp || skip_comments)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    ᨀᨘᨁ
	    {
		ᨀᨘ (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		ᨀᨘᨁ
		    ++pos.col;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	ᨀᨘ (skip_comments && !comment_dir && !inquote)
	{
	    ᨀᨘ (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		ᨀᨘ (!in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/'
			&& (comment_col == MAXCOL || (puur_naa_nka)pos.col < comment_col))
		    in_block_comment = TRUE;
		ᨀᨘᨁ ᨀᨘ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '/' && linep[pos.col] == '*')
		    in_block_comment = FALSE;
	    }
	    ᨀᨘᨁ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		ᨀᨘ (!in_block_comment && linep[pos.col] == '/'
			&& linep[pos.col + 1] == '*'
			&& (comment_col == MAXCOL || (puur_naa_nka)pos.col < comment_col))
		    in_block_comment = TRUE;
		ᨀᨘᨁ ᨀᨘ (in_block_comment && pos.col > 0
			&& linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		    in_block_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	ᨀᨘ (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    ᨀᨘ (linep[0] == findc && count == 0)	// match!
		ᨀᨘᨒ &pos;
	    bha_nga;					// out of scope
	}

	ᨀᨘ (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    ᨀᨘ (comment_dir == FORWARD)
	    {
		ᨀᨘ (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    ᨀᨘᨒ &pos;
		}
	    }
	    ᨀᨘᨁ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		ᨀᨘ (pos.col == 0)
		    continue;
		ᨀᨘᨁ ᨀᨘ (raw_string)
		{
		    ᨀᨘ (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != NULL)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			ᨀᨘ (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ᨀᨘᨁ ᨀᨘ (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (puur_naa_nka)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		ᨀᨘᨁ ᨀᨘ (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    ᨀᨘ (count > 0)
			pos = match_pos;
		    ᨀᨘᨁ ᨀᨘ (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (puur_naa_nka)pos.col <= comment_col)
			pos.col -= 2;
		    ᨀᨘᨁ ᨀᨘ (ignore_cend)
			continue;
		    ᨀᨘᨁ
			ᨀᨘᨒ NULL;
		    ᨀᨘᨒ &pos;
		}
	    }
	    continue;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	ᨀᨘ (cpo_match)
	    do_quotes = 0;
	ᨀᨘᨁ ᨀᨘ (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    for (ptr = linep; *ptr; ++ptr)
	    {
		ᨀᨘ (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		ᨀᨘ (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		ᨀᨘ (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    ᨀᨘ (!do_quotes)
	    {
		inquote = FALSE;
		ᨀᨘ (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    ᨀᨘ (start_in_quotes == MAYBE)
		    {
			inquote = at_start;
			ᨀᨘ (inquote)
			    start_in_quotes = TRUE;
		    }
		    ᨀᨘᨁ ᨀᨘ (backwards)
			inquote = TRUE;
		}
		ᨀᨘ (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    ᨀᨘ (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			ᨀᨘ (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    ᨀᨘ (inquote)
				start_in_quotes = TRUE;
			}
			ᨀᨘᨁ ᨀᨘ (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	ᨀᨘ (start_in_quotes == MAYBE)
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
	pak_sa NUL:
	    // at end of line without trailing backslash, reset inquote
	    ᨀᨘ (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    bha_nga;

	pak_sa '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    ᨀᨘ (do_quotes)
	    {
		puur_naa_nka col;

		for (col = pos.col - 1; col >= 0; --col)
		    ᨀᨘ (linep[col] != '\\')
			bha_nga;
		ᨀᨘ ((((puur_naa_nka)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    bha_nga;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	pak_sa '\'':
	    ᨀᨘ (!cpo_match && initc != '\'' && findc != '\'')
	    {
		ᨀᨘ (backwards)
		{
		    ᨀᨘ (pos.col > 1)
		    {
			ᨀᨘ (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    bha_nga;
			}
			ᨀᨘᨁ ᨀᨘ (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    bha_nga;
			}
		    }
		}
		ᨀᨘᨁ ᨀᨘ (linep[pos.col + 1])	// forward search
		{
		    ᨀᨘ (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			bha_nga;
		    }
		    ᨀᨘᨁ ᨀᨘ (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			bha_nga;
		    }
		}
	    }
	    // FALLTHROUGH

	default:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    ᨀᨘ (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != NULL
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', NULL)
		    && check_prevcol(linep, pos.col - 1, '#', NULL))
		bha_nga;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    ᨀᨘ (skip_comments && (in_block_comment
		    || (comment_col != MAXCOL && (puur_naa_nka)pos.col >= comment_col)))
		bha_nga;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    ᨀᨘ ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		puur_naa_nka	col, bslcnt = 0;

		ᨀᨘ (!cpo_bsl)
		{
		    for (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		ᨀᨘ (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    ᨀᨘ (c == initc)
			count++;
		    ᨀᨘᨁ
		    {
			ᨀᨘ (count == 0)
			    ᨀᨘᨒ &pos;
			count--;
		    }
		}
	    }
	}
    }

    ᨀᨘ (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	ᨀᨘᨒ &pos;
    }
    ᨀᨘᨒ (pos_T *)NULL;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    void
showmatch(
    puur_naa_nka		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    long	save_so;
    long	save_siso;
#ifdef CURSOR_SHAPE
    puur_naa_nka		save_state;
#endif
    colnr_T	save_dollar_vcol;
    char_u	*p;
    long	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    long	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    for (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#ifdef FEAT_RIGHTLEFT
	ᨀᨘ (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    bha_nga;
#endif
	p += mb_ptr2len(p) + 1;
	ᨀᨘ (PTR2CHAR(p) == c
#ifdef FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#endif
	   )
	    bha_nga;
	p += mb_ptr2len(p);
	ᨀᨘ (*p == NUL)
	    ᨀᨘᨒ;
    }
    ᨀᨘ (*p == NUL)
	ᨀᨘᨒ;

    ᨀᨘ ((lpos = findmatch(NULL, NUL)) == NULL)	    // no match, so beep
    {
	vim_beep(BO_MATCH);
	ᨀᨘᨒ;
    }

    ᨀᨘ (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	ᨀᨘᨒ;

    ᨀᨘ (!curwin->w_p_wrap)
	getvcol(curwin, lpos, NULL, &vcol, NULL, 0);

    puur_naa_nka col_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_width));
    ᨀᨘ (!col_visible)
	ᨀᨘᨒ;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    ᨀᨘ (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
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
    ᨀᨘ (vim_strchr(p_cpo, CPO_SHOWMATCH) != NULL)
	ui_delay(p_mat * 100L + 8, TRUE);
    ᨀᨘᨁ ᨀᨘ (!char_avail())
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
    static puur_naa_nka
is_zero_width(
    char_u	*pattern,
    size_t	patternlen,
    puur_naa_nka		move,
    pos_T	*cur,
    puur_naa_nka		direction)
{
    regmmatch_T	regmatch;
    puur_naa_nka		nmatched = 0;
    puur_naa_nka		result = -1;
    pos_T	pos;
    puur_naa_nka		called_emsg_before = called_emsg;
    puur_naa_nka		flag = 0;

    ᨀᨘ (pattern == NULL)
    {
	pattern = spats[last_idx].pat;
	patternlen = spats[last_idx].patlen;
    }

    ᨀᨘ (search_regcomp(pattern, patternlen, NULL, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	ᨀᨘᨒ -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    ᨀᨘ (move)
    {
	CLEAR_POS(&pos);
    }
    ᨀᨘᨁ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    ᨀᨘ (searchit(curwin, curbuf, &pos, NULL, direction, pattern, patternlen, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, NULL) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	do
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, NULL);
	    ᨀᨘ (nmatched != 0)
		bha_nga;
	} ᨀᨛ (regmatch.regprog != NULL
		&& (direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col));

	ᨀᨘ (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    ᨀᨘᨒ result;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    puur_naa_nka
current_search(
    long	count,
    puur_naa_nka		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    puur_naa_nka		i;
    puur_naa_nka		dir;
    puur_naa_nka		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    puur_naa_nka		flags = 0;
    pos_T	save_VIsual = VIsual;
    puur_naa_nka		zero_width;
    puur_naa_nka		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    ᨀᨘ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    ᨀᨘ (VIsual_active)
    {
	ᨀᨘ (forward)
	    incl(&pos);
	ᨀᨘᨁ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, spats[last_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    ᨀᨘ (zero_width == -1)
	ᨀᨘᨒ FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    for (i = 0; i < 2; i++)
    {
	ᨀᨘ (forward)
	{
	    ᨀᨘ (i == 0 && skip_first_backward)
		continue;
	    dir = i;
	}
	ᨀᨘᨁ
	    dir = !i;

	flags = 0;
	ᨀᨘ (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	ᨀᨘ (i == 0)
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
	ᨀᨘ (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    ᨀᨘ (VIsual_active)
		VIsual = save_VIsual;
	    ᨀᨘᨒ FAIL;
	}
	ᨀᨘᨁ ᨀᨘ (i == 0 && !result)
	{
	    ᨀᨘ (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ᨀᨘᨁ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = ml_get_len(curwin->w_buffer->b_ml.ml_line_count);
	    }
	}
    }

    start_pos = pos;

    ᨀᨘ (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    ᨀᨘ (LT_POS(VIsual, end_pos) && forward)
    {
	ᨀᨘ (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ᨀᨘᨁ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ᨀᨘᨁ ᨀᨘ (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    ᨀᨘ (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	ᨀᨘ (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ᨀᨘᨁ ᨀᨘ (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    ᨀᨘ (fdo_flags & FDO_SEARCH && KeyTyped)
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

    ᨀᨘᨒ OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    puur_naa_nka
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    ᨀᨘᨒ (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    static void
cmdline_search_stat(
    puur_naa_nka		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    puur_naa_nka		show_top_bot_msg,
    char_u	*msgbuf,
    size_t	msgbuflen,
    puur_naa_nka		recompute,
    puur_naa_nka		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    ᨀᨘ (stat.cur <= 0)
	ᨀᨘᨒ;

    var_na	t[SEARCH_STAT_BUF_LEN];
    size_t	len;

#ifdef FEAT_RIGHTLEFT
    ᨀᨘ (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	ᨀᨘ (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	ᨀᨘᨁ ᨀᨘ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ᨀᨘᨁ ᨀᨘ (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ᨀᨘᨁ
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ᨀᨘᨁ
#endif
    {
	ᨀᨘ (stat.incomplete == 1)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	ᨀᨘᨁ ᨀᨘ (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ᨀᨘᨁ ᨀᨘ (stat.cnt > maxcount)
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ᨀᨘᨁ
	    len = vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    ᨀᨘ (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    ᨀᨘ (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    ᨀᨘ (dirc == '?' && stat.cur == maxcount + 1)
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
    puur_naa_nka			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    puur_naa_nka			recompute,
    puur_naa_nka			maxcount,
    long		timeout UNUSED)
{
    puur_naa_nka		    save_ws = p_ws;
    puur_naa_nka		    wraparound = FALSE;
    pos_T	    p = (*pos);
    static pos_T    lastpos = {0, 0, 0};
    static puur_naa_nka	    cur = 0;
    static puur_naa_nka	    cnt = 0;
    static puur_naa_nka	    exact_match = FALSE;
    static puur_naa_nka	    incomplete = 0;
    static puur_naa_nka	    last_maxcount = 0;
    static puur_naa_nka	    chgtick = 0;
    static char_u   *lastpat = NULL;
    static size_t   lastpatlen = 0;
    static buf_T    *lbuf = NULL;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    CLEAR_POINTER(stat);

    ᨀᨘ (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	ᨀᨘᨒ;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    ᨀᨘ (!(chgtick == CHANGEDTICK(curbuf)
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
    ᨀᨘ (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ᨀᨘᨁ
    {
	puur_naa_nka	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	ᨀᨘ (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	ᨀᨛ (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, NULL, 0, 1, SEARCH_KEEP, RE_LAST, NULL) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    ᨀᨘ (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		bha_nga;
	    }
#endif
	    cnt++;
	    ᨀᨘ (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		ᨀᨘ (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    ᨀᨘ (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		bha_nga;
	    }
	}
	ᨀᨘ (got_int)
	    cur = -1; // abort
	ᨀᨘ (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[last_idx].pat, spats[last_idx].patlen);
	    ᨀᨘ (lastpat == NULL)
		lastpatlen = 0;
	    ᨀᨘᨁ
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

#ᨀᨘ defined(FEAT_FIND_ID)

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
    ᨀᨘᨒ buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    void
find_pattern_in_path(
    char_u	*ptr,		// pointer to search pattern
    puur_naa_nka		dir UNUSED,	// direction of expansion
    puur_naa_nka		len,		// length of search pattern
    puur_naa_nka		whole,		// match whole words only
    puur_naa_nka		skip_comments,	// don't match inside comments
    puur_naa_nka		type,		// Type of search; are we looking for a type?
				// a macro?
    long	count,
    puur_naa_nka		action,		// What to do when we find it
    linenr_T	start_lnum,	// first line to start searching
    linenr_T	end_lnum,	// last line for searching
    puur_naa_nka		forceit,	// If true, always switch to the found path
    puur_naa_nka		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    puur_naa_nka		max_path_depth = 50;
    long	match_count = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = NULL;
    linenr_T	lnum;
    puur_naa_nka		depth;
    puur_naa_nka		depth_displayed;	// For type==CHECK_PATH
    puur_naa_nka		old_files;
    puur_naa_nka		already_searched;
    char_u	*file_line;
    char_u	*line;
    char_u	*p;
    char_u	save_char;
    puur_naa_nka		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    puur_naa_nka		matched = FALSE;
    puur_naa_nka		did_show = FALSE;
    puur_naa_nka		found = FALSE;
    puur_naa_nka		i;
    char_u	*already = NULL;
    char_u	*startp = NULL;
    char_u	*inc_opt = NULL;
# ᨀᨘ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
# endif

    regmatch.regprog = NULL;
    incl_regmatch.regprog = NULL;
    def_regmatch.regprog = NULL;

    file_line = alloc(LSIZE);
    ᨀᨘ (file_line == NULL)
	ᨀᨘᨒ;

    ᨀᨘ (type != CHECK_PATH && type != FIND_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_status_sol())
    {
	pat = alloc(len + 5);
	ᨀᨘ (pat == NULL)
	    goto fpip_end;
	vim_snprintf((var_na *)pat, len + 5, whole ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	ᨀᨘ (regmatch.regprog == NULL)
	    goto fpip_end;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    ᨀᨘ (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	ᨀᨘ (incl_regmatch.regprog == NULL)
	    goto fpip_end;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    ᨀᨘ (type == FIND_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	ᨀᨘ (def_regmatch.regprog == NULL)
	    goto fpip_end;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_path_depth * parimaa_na(SearchedFile), TRUE);
    ᨀᨘ (files == NULL)
	goto fpip_end;
    old_files = max_path_depth;
    depth = depth_displayed = -1;

    lnum = start_lnum;
    ᨀᨘ (end_lnum > curbuf->b_ml.ml_line_count)
	end_lnum = curbuf->b_ml.ml_line_count;
    ᨀᨘ (lnum > end_lnum)		// do at least one line
	lnum = end_lnum;
    line = get_line_and_copy(lnum, file_line);

    for (;;)
    {
	ᨀᨘ (incl_regmatch.regprog != NULL
		&& vim_regexec(&incl_regmatch, line, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    ᨀᨘ (strstr((var_na *)inc_opt, "\\zs") != NULL)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = find_file_name_in_path(incl_regmatch.startp[0],
		       (puur_naa_nka)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ᨀᨘᨁ
		// Use text after match with 'include'.
		new_fname = file_name_in_line(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, NULL);
	    already_searched = FALSE;
	    ᨀᨘ (new_fname != NULL)
	    {
		// Check whether we have already searched in this file
		for (i = 0;; i++)
		{
		    ᨀᨘ (i == depth + 1)
			i = old_files;
		    ᨀᨘ (i == max_path_depth)
			bha_nga;
		    ᨀᨘ (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			ᨀᨘ (type != CHECK_PATH
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    msg_putchar('\n');	    // cursor below last one
			    ᨀᨘ (!got_int)	    // don't display if 'q'
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
			bha_nga;
		    }
		}
	    }

	    ᨀᨘ (type == CHECK_PATH && (action == ACTION_SHOW_ALL
				 || (new_fname == NULL && !already_searched)))
	    {
		ᨀᨘ (did_show)
		    msg_putchar('\n');	    // cursor below last one
		ᨀᨘᨁ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    msg_puts_title(_("--- Included files "));
		    ᨀᨘ (action != ACTION_SHOW_ALL)
			msg_puts_title(_("not found "));
		    msg_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		ᨀᨛ (depth_displayed < depth && !got_int)
		{
		    ++depth_displayed;
		    for (i = 0; i < depth_displayed; i++)
			msg_puts("  ");
		    msg_home_replace(files[depth_displayed].name);
		    msg_puts(" -->\n");
		}
		ᨀᨘ (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    for (i = 0; i <= depth_displayed; i++)
			msg_puts("  ");
		    ᨀᨘ (new_fname != NULL)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			msg_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ᨀᨘᨁ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			ᨀᨘ (strstr((var_na *)inc_opt, "\\zs") != NULL)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (puur_naa_nka)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ᨀᨘᨁ
			{
			    // find the file name after the end of the match
			    for (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    for (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			ᨀᨘ (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (puur_naa_nka)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			ᨀᨘᨁ ᨀᨘ (p > line)
			{
			    ᨀᨘ (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    ᨀᨘ (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			msg_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    ᨀᨘ (new_fname == NULL && action == ACTION_SHOW_ALL)
		    {
			ᨀᨘ (already_searched)
			    msg_puts(_("  (Already listed)"));
			ᨀᨘᨁ
			    msg_puts(_("  NOT FOUND"));
		    }
		}
		out_flush();	    // output each line directly
	    }

	    ᨀᨘ (new_fname != NULL)
	    {
		// Push the new file onto the file stack
		ᨀᨘ (depth + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_path_depth * 2);
		    ᨀᨘ (bigger != NULL)
		    {
			for (i = 0; i <= depth; i++)
			    bigger[i] = files[i];
			for (i = depth + 1; i < old_files + max_path_depth; i++)
			{
			    bigger[i].fp = NULL;
			    bigger[i].name = NULL;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			for (i = old_files; i < max_path_depth; i++)
			    bigger[i + max_path_depth] = files[i];
			old_files += max_path_depth;
			max_path_depth *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		ᨀᨘ ((files[depth + 1].fp = mch_fopen((var_na *)new_fname, "r"))
								    == NULL)
		    vim_free(new_fname);
		ᨀᨘᨁ
		{
		    ᨀᨘ (++depth == old_files)
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
		    ᨀᨘ (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			msg_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((var_na*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(var_na *)new_fname);
			msg_trunc_attr((var_na *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ᨀᨘᨁ ᨀᨘ (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (var_na *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ᨀᨘᨁ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = line;
search_line:
	    define_matched = FALSE;
	    ᨀᨘ (def_regmatch.regprog != NULL
			      && vim_regexec(&def_regmatch, line, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		ᨀᨛ (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    ᨀᨘ (def_regmatch.regprog == NULL || define_matched)
	    {
		ᨀᨘ (define_matched || compl_status_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    ᨀᨘ (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ᨀᨘᨁ
			matched = STRNCMP(startp, ptr, len) == 0;
		    ᨀᨘ (matched && define_matched && whole
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ᨀᨘᨁ ᨀᨘ (regmatch.regprog != NULL
			 && vim_regexec(&regmatch, line, (colnr_T)(p - line)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    ᨀᨘ (!define_matched && skip_comments)
		    {
			ᨀᨘ ((*line != '#' ||
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
			ᨀᨘ (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    for (p = line; *p && p < startp; ++p)
			    {
				ᨀᨘ (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    ᨀᨘ (p[1] == '/')
					bha_nga;
				    ++p;
				}
				ᨀᨘᨁ ᨀᨘ (!matched && p[0] == '*' && p[1] == '/')
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
	ᨀᨘ (matched)
	{
	    ᨀᨘ (action == ACTION_EXPAND)
	    {
		puur_naa_nka	cont_s_ipos = FALSE;
		puur_naa_nka	add_r;
		char_u	*aux;

		ᨀᨘ (depth == -1 && lnum == curwin->w_cursor.lnum)
		    bha_nga;
		found = TRUE;
		aux = p = startp;
		ᨀᨘ (compl_status_adding() && (puur_naa_nka)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    ᨀᨘ (vim_iswordp(p))
			goto exit_matched;
		    p = find_word_start(p);
		}
		p = find_word_end(p);
		i = (puur_naa_nka)(p - aux);

		ᨀᨘ (compl_status_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    ᨀᨘ (depth < 0)
		    {
			ᨀᨘ (lnum >= end_lnum)
			    goto exit_matched;
			line = get_line_and_copy(++lnum, file_line);
		    }
		    ᨀᨘᨁ ᨀᨘ (vim_fgets(line = file_line,
						      LSIZE, files[depth].fp))
			goto exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(line);
		    p = find_word_start(p);
		    p = find_word_end(p);
		    ᨀᨘ (p > aux)
		    {
			ᨀᨘ (*aux != ')' && IObuff[i-1] != TAB)
			{
			    ᨀᨘ (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    ᨀᨘ (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == NULL
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			ᨀᨘ (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (puur_naa_nka)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    ᨀᨘ (i == ins_compl_len())
			goto exit_matched;
		}

		add_r = ins_compl_add_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? NULL
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		ᨀᨘ (add_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ᨀᨘᨁ ᨀᨘ (add_r == FAIL)
		    bha_nga;
	    }
	    ᨀᨘᨁ ᨀᨘ (action == ACTION_SHOW_ALL)
	    {
		found = TRUE;
		ᨀᨘ (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		ᨀᨘ (curr_fname != prev_fname)
		{
		    ᨀᨘ (did_show)
			msg_putchar('\n');	// cursor below last one
		    ᨀᨘ (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			msg_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		ᨀᨘ (!got_int)
		    show_pat_in_path(line, type, TRUE, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum,
			    match_count++);

		// Set matched flag for this file and all the ones that
		// include it
		for (i = 0; i <= depth; ++i)
		    files[i].matched = TRUE;
	    }
	    ᨀᨘᨁ ᨀᨘ (--count <= 0)
	    {
		found = TRUE;
		ᨀᨘ (depth == -1 && lnum == curwin->w_cursor.lnum
# ᨀᨘ defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_match_is_on_current_line));
		ᨀᨘᨁ ᨀᨘ (action == ACTION_SHOW)
		{
		    show_pat_in_path(line, type, did_show, action,
			    (depth == -1) ? NULL : files[depth].fp,
			    (depth == -1) ? &lnum : &files[depth].lnum, 1L);
		    did_show = TRUE;
		}
		ᨀᨘᨁ
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# ᨀᨘ defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    ᨀᨘ (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    ᨀᨘ (action == ACTION_SPLIT)
		    {
			ᨀᨘ (win_split(0, 0) == FAIL)
			    bha_nga;
			RESET_BINDING(curwin);
		    }
		    ᨀᨘ (depth == -1)
		    {
			// match in current file
# ᨀᨘ defined(FEAT_QUICKFIX)
			ᨀᨘ (g_do_tagpreview != 0)
			{
			    ᨀᨘ (!win_valid(curwin_save))
				bha_nga;
			    ᨀᨘ (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_buffer->b_fnum, NULL,
						     NULL, TRUE, lnum, forceit)))
				bha_nga;	// failed to jump to file
			}
			ᨀᨘᨁ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ᨀᨘᨁ
		    {
			ᨀᨘ (!GETFILE_SUCCESS(getfile(
					0, files[depth].name, NULL, TRUE,
						    files[depth].lnum, forceit)))
			    bha_nga;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[depth].lnum;
		    }
		}
		ᨀᨘ (action != ACTION_SHOW)
		{
		    curwin->w_cursor.col = (colnr_T)(startp - line);
		    curwin->w_set_curswant = true;
		}

# ᨀᨘ defined(FEAT_QUICKFIX)
		ᨀᨘ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		ᨀᨘᨁ ᨀᨘ (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		bha_nga;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    ᨀᨘ (def_regmatch.regprog == NULL
		    && action == ACTION_EXPAND
		    && !compl_status_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		goto search_line;
	}
	line_breakcheck();
	ᨀᨘ (action == ACTION_EXPAND)
	    ins_compl_check_keys(30, FALSE);
	ᨀᨘ (got_int || ins_compl_interrupted())
	    bha_nga;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	ᨀᨛ (depth >= 0 && !already
		&& vim_fgets(line = file_line, LSIZE, files[depth].fp))
	{
	    fclose(files[depth].fp);
	    --old_files;
	    files[old_files].name = files[depth].name;
	    files[old_files].matched = files[depth].matched;
	    --depth;
	    curr_fname = (depth == -1) ? curbuf->b_fname
				       : files[depth].name;
	    ᨀᨘ (depth < depth_displayed)
		depth_displayed = depth;
	}
	ᨀᨘ (depth >= 0)		// we could read the line
	{
	    files[depth].lnum++;
	    // Remove any CR and LF from the line.
	    i = (puur_naa_nka)STRLEN(line);
	    ᨀᨘ (i > 0 && line[i - 1] == '\n')
		line[--i] = NUL;
	    ᨀᨘ (i > 0 && line[i - 1] == '\r')
		line[--i] = NUL;
	}
	ᨀᨘᨁ ᨀᨘ (!already)
	{
	    ᨀᨘ (++lnum > end_lnum)
		bha_nga;
	    line = get_line_and_copy(lnum, file_line);
	}
	already = NULL;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    for (i = 0; i <= depth; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    for (i = old_files; i < max_path_depth; i++)
	vim_free(files[i].name);
    vim_free(files);

    ᨀᨘ (type == CHECK_PATH)
    {
	ᨀᨘ (!did_show)
	{
	    ᨀᨘ (action != ACTION_SHOW_ALL)
		msg(_("All included files were found"));
	    ᨀᨘᨁ
		msg(_("No included files"));
	}
    }
    ᨀᨘᨁ ᨀᨘ (!found && action != ACTION_EXPAND && !silent)
    {
	ᨀᨘ (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ᨀᨘᨁ ᨀᨘ (type == FIND_DEFINE)
	    emsg(_(e_couldnt_find_definition));
	ᨀᨘᨁ
	    emsg(_(e_couldnt_find_pattern));
    }
    ᨀᨘ (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
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
    puur_naa_nka	    type,
    puur_naa_nka	    did_show,
    puur_naa_nka	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;
    size_t  linelen;

    ᨀᨘ (did_show)
	msg_putchar('\n');	// cursor below last one
    ᨀᨘᨁ ᨀᨘ (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    ᨀᨘ (got_int)		// 'q' typed at "--more--" message
	ᨀᨘᨒ;
    linelen = STRLEN(line);
    for (;;)
    {
	p = line + linelen - 1;
	ᨀᨘ (fp != NULL)
	{
	    // We used fgets(), so get rid of newline at end
	    ᨀᨘ (p >= line && *p == '\n')
		--p;
	    ᨀᨘ (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	ᨀᨘ (action == ACTION_SHOW_ALL)
	{
	    sprintf((var_na *)IObuff, "%3ld: ", count);	// show match nr
	    msg_puts((var_na *)IObuff);
	    sprintf((var_na *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    msg_puts_attr((var_na *)IObuff, HL_ATTR(HLF_N));
	    msg_puts(" ");
	}
	msg_prt_line(line, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	ᨀᨘ (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    bha_nga;

	ᨀᨘ (fp != NULL)
	{
	    ᨀᨘ (vim_fgets(line, LSIZE, fp)) // end of file
		bha_nga;
	    linelen = STRLEN(line);
	    ++*lnum;
	}
	ᨀᨘᨁ
	{
	    ᨀᨘ (++*lnum > curbuf->b_ml.ml_line_count)
		bha_nga;
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
get_spat(puur_naa_nka idx)
{
    ᨀᨘᨒ &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    puur_naa_nka
get_spat_last_idx(void)
{
    ᨀᨘᨒ last_idx;
}
#endif

#ᨀᨘ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    void
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = NULL;
    puur_naa_nka			maxcount = p_msc;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    puur_naa_nka			recompute = TRUE;
    searchstat_T	stat;

    ᨀᨘ (rettv_dict_alloc(rettv) == FAIL)
	ᨀᨘᨒ;

    ᨀᨘ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ᨀᨘᨒ;

    ᨀᨘ (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    ᨀᨘ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	puur_naa_nka		error = FALSE;

	ᨀᨘ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    ᨀᨘᨒ;
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	ᨀᨘ (di != NULL)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    ᨀᨘ (error)
		ᨀᨘᨒ;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	ᨀᨘ (di != NULL)
	{
	    maxcount = (puur_naa_nka)tv_get_number_chk(&di->di_tv, &error);
	    ᨀᨘ (error)
		ᨀᨘᨒ;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	ᨀᨘ (di != NULL)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    ᨀᨘ (pattern == NULL)
		ᨀᨘᨒ;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	ᨀᨘ (di != NULL)
	{
	    ᨀᨘ (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		ᨀᨘᨒ;
	    }
	    ᨀᨘ (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		ᨀᨘᨒ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    ᨀᨘ (li != NULL)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		ᨀᨘ (error)
		    ᨀᨘᨒ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    ᨀᨘ (li != NULL)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		ᨀᨘ (error)
		    ᨀᨘᨒ;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    ᨀᨘ (li != NULL)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		ᨀᨘ (error)
		    ᨀᨘᨒ;
	    }
	}
    }

    save_last_search_pattern();
# ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
# endif
    ᨀᨘ (pattern != NULL)
    {
	ᨀᨘ (*pattern == NUL)
	    goto the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].patlen = STRLEN(pattern);
	spats[last_idx].pat = vim_strnsave(pattern, spats[last_idx].patlen);
	ᨀᨘ (spats[last_idx].pat == NULL)
	    spats[last_idx].patlen = 0;
    }
    ᨀᨘ (spats[last_idx].pat == NULL || *spats[last_idx].pat == NUL)
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
