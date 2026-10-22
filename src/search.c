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

#که_تعریف_شوی FEAT_EVAL
ساكن فراغ سېټ_vv_searchforward(فراغ);
ساكن صحيح لومړی_submatch(regmmatch_T *rp);
#د_که_پای
#که_تعریف_شوی FEAT_پیدا_کړه_ID
ساكن char_u *get_کرښه_and_copy(linenr_T lnum, char_u *buf);
ساكن فراغ show_pat_دننه_لار(char_u *, صحيح, صحيح, صحيح, FILE *, linenr_T *, اوږد);
#د_که_پای

تعريفنوع بنية searchstat
{
    صحيح	    cur;	    // current position of found words
    صحيح	    cnt;	    // total count of found words
    صحيح	    exact_سمون;    // TRUE if matched exactly on specified position
    صحيح	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    صحيح	    وروستی_maxcount;  // the max count of the last search
} searchstat_T;

#که_تعریف_شوی FEAT_پلټنه_EXTRA
ساكن فراغ save_incsearch_state(فراغ);
ساكن فراغ restore_incsearch_state(فراغ);
#د_که_پای
ساكن صحيح check_prevcol(char_u *linep, صحيح ستون, صحيح ch, صحيح *prevcol);
ساكن صحيح پیدا_کړه_rawstring_پای_ټکی(char_u *linep, pos_T *startpos, pos_T *endpos);
ساكن فراغ پیدا_کړه_mps_ارزښتونه(صحيح *initc, صحيح *findc, صحيح *backwards, صحيح switchit);
ساكن صحيح is_zero_پلنوالی(char_u *نمونه, size_t patternlen, صحيح move, pos_T *cur, صحيح direction);
ساكن فراغ cmdline_پلټنه_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, صحيح show_سر_bot_پیغام, char_u *msgbuf, size_t msgbuflen, صحيح recompute, صحيح maxcount, اوږد timeout);
ساكن فراغ update_پلټنه_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, صحيح recompute, صحيح maxcount, اوږد timeout);

#تعریف_کړه پلټنه_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#تعریف_کړه پلټنه_STAT_BUF_LEN 16

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

ساكن صحيح وروستی_idx = 0;	// index in spats[] for RE_LAST

ساكن char_u lastc[2] = {NUL, NUL};	// last character searched for
ساكن صحيح lastcdir = FORWARD;		// last direction of character search
ساكن صحيح وروستی_t_cmd = TRUE;		// last search t_cmd
ساكن char_u	lastc_بایټونه[MB_MAXBYTES + 1];
ساكن صحيح	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ساكن spat_T	    saved_spats[ARRAY_اوږدتیا(spats)];
ساكن char_u	    *saved_mr_نمونه = عدم;
ساكن size_t	    saved_mr_patternlen = 0;
#که_تعریف_شوی FEAT_پلټنه_EXTRA
ساكن صحيح	    saved_spats_وروستی_idx = 0;
ساكن صحيح	    saved_spats_no_hlsearch = 0;
#د_که_پای

// allocated copy of pattern used by search_regcomp()
ساكن char_u	    *mr_نمونه = عدم;
ساكن size_t	    mr_patternlen = 0;

#که_تعریف_شوی FEAT_پیدا_کړه_ID
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
#د_که_پای

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
پلټنه_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**کارول_شوی_pat,
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
    إذا (pat == عدم || *pat == NUL)
    {
	صحيح i;

	إذا (pat_use == RE_وروستی)
	    i = وروستی_idx;
	وإلا
	    i = pat_use;
	إذا (spats[i].pat == عدم)	// pattern was never defined
	{
	    إذا (pat_use == RE_SUBST)
		emsg(_(e_no_پخوانی_substitute_regular_افاده));
	    وإلا
		emsg(_(e_no_پخوانی_regular_افاده));
	    rc_did_emsg = TRUE;
	    أرجع FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    وإلا إذا (options & پلټنه_HIS)	// put new pattern in history
	جمع_کړه_منزل_history(HIST_پلټنه, pat, patlen, TRUE, NUL);

    إذا (کارول_شوی_pat)
	*کارول_شوی_pat = pat;

    vim_free(mr_نمونه);
#که_تعریف_شوی FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_نمونه = سرچپه_متن(pat);
    وإلا
#د_که_پای
	mr_نمونه = vim_strnsave(pat, patlen);
    إذا (mr_نمونه == عدم)
	mr_patternlen = 0;
    وإلا
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    إذا (!(options & پلټنه_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	إذا (pat_save == RE_پلټنه || pat_save == RE_BOTH)
	    save_re_pat(RE_پلټنه, pat, patlen, magic);
	// substitute or global command
	إذا (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    إذا (regmatch->regprog == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_پلټنه_pat(فراغ)
{
    أرجع mr_نمونه;
}

    فراغ
save_re_pat(صحيح idx, char_u *pat, size_t patlen, صحيح magic)
{
    إذا (spats[idx].pat == pat)
	أرجع;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    إذا (spats[idx].pat == عدم)
	spats[idx].patlen = 0;
    وإلا
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    وروستی_idx = idx;
#که_تعریف_شوی FEAT_پلټنه_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    إذا (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    سېټ_no_hlsearch(FALSE);
#د_که_پای
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ساكن صحيح save_level = 0;

    فراغ
save_پلټنه_patterns(فراغ)
{
    صحيح i;

    إذا (save_level++ != 0)
	أرجع;

    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(spats); ++i)
    {
	saved_spats[i] = spats[i];
	إذا (spats[i].pat != عدم)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    إذا (saved_spats[i].pat == عدم)
		saved_spats[i].patlen = 0;
	    وإلا
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    إذا (mr_نمونه == عدم)
	saved_mr_نمونه = عدم;
    وإلا
	saved_mr_نمونه = vim_strnsave(mr_نمونه, mr_patternlen);
    إذا (saved_mr_نمونه == عدم)
	saved_mr_patternlen = 0;
    وإلا
	saved_mr_patternlen = mr_patternlen;
#که_تعریف_شوی FEAT_پلټنه_EXTRA
    saved_spats_وروستی_idx = وروستی_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#د_که_پای
}

    فراغ
restore_پلټنه_patterns(فراغ)
{
    صحيح i;

    إذا (--save_level != 0)
	أرجع;

    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#إذا defined(FEAT_EVAL)
    سېټ_vv_searchforward();
#د_که_پای
    vim_free(mr_نمونه);
    mr_نمونه = saved_mr_نمونه;
    mr_patternlen = saved_mr_patternlen;
#که_تعریف_شوی FEAT_پلټنه_EXTRA
    وروستی_idx = saved_spats_وروستی_idx;
    سېټ_no_hlsearch(saved_spats_no_hlsearch);
#د_که_پای
}

#إذا defined(EXITFREE)
    فراغ
free_پلټنه_patterns(فراغ)
{
    صحيح i;

    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_نمونه);
    mr_patternlen = 0;
}
#د_که_پای

#که_تعریف_شوی FEAT_پلټنه_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ساكن spat_T	    saved_وروستی_پلټنه_spat;
ساكن صحيح	    did_save_وروستی_پلټنه_spat = 0;
ساكن صحيح	    saved_وروستی_idx = 0;
ساكن صحيح	    saved_no_hlsearch = 0;
ساكن صحيح	    saved_پلټنه_سمون_endcol;
ساكن صحيح	    saved_پلټنه_سمون_کرښې;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    فراغ
save_وروستی_پلټنه_نمونه(فراغ)
{
    إذا (++did_save_وروستی_پلټنه_spat != 1)
	// nested call, nothing to do
	أرجع;

    saved_وروستی_پلټنه_spat = spats[RE_پلټنه];
    إذا (spats[RE_پلټنه].pat != عدم)
    {
	saved_وروستی_پلټنه_spat.pat = vim_strnsave(spats[RE_پلټنه].pat, spats[RE_پلټنه].patlen);
	إذا (saved_وروستی_پلټنه_spat.pat == عدم)
	    saved_وروستی_پلټنه_spat.patlen = 0;
	وإلا
	    saved_وروستی_پلټنه_spat.patlen = spats[RE_پلټنه].patlen;
    }
    saved_وروستی_idx = وروستی_idx;
    saved_no_hlsearch = no_hlsearch;
}

    فراغ
restore_وروستی_پلټنه_نمونه(فراغ)
{
    إذا (--did_save_وروستی_پلټنه_spat > 0)
	// nested call, nothing to do
	أرجع;
    إذا (did_save_وروستی_پلټنه_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	أرجع;
    }

    vim_free(spats[RE_پلټنه].pat);
    spats[RE_پلټنه] = saved_وروستی_پلټنه_spat;
    saved_وروستی_پلټنه_spat.pat = عدم;
    saved_وروستی_پلټنه_spat.patlen = 0;
# إذا defined(FEAT_EVAL)
    سېټ_vv_searchforward();
# endif
    وروستی_idx = saved_وروستی_idx;
    سېټ_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ساكن فراغ
save_incsearch_state(فراغ)
{
    saved_پلټنه_سمون_endcol = پلټنه_سمون_endcol;
    saved_پلټنه_سمون_کرښې  = پلټنه_سمون_کرښې;
}

    ساكن فراغ
restore_incsearch_state(فراغ)
{
    پلټنه_سمون_endcol = saved_پلټنه_سمون_endcol;
    پلټنه_سمون_کرښې  = saved_پلټنه_سمون_کرښې;
}

    char_u *
وروستی_پلټنه_نمونه(فراغ)
{
    أرجع spats[RE_پلټنه].pat;
}

    size_t
وروستی_پلټنه_نمونه_len(فراغ)
{
    أرجع spats[RE_پلټنه].patlen;
}
#د_که_پای

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    صحيح
ignorecase(char_u *pat)
{
    أرجع ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    صحيح
ignorecase_opt(char_u *pat, صحيح ic_دننه, صحيح scs)
{
    صحيح		ic = ic_دننه;

    إذا (ic && !no_smartcase && scs
			    && !(ctrl_x_ډېرپېښ_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    أرجع ic;
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

    طالما (*p != NUL)
    {
	صحيح		l;

	إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    إذا (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		أرجع TRUE;
	    p += l;
	}
	وإلا إذا (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    إذا (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    وإلا إذا (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    وإلا إذا (p[1] != NUL)  // skip "\X"
		p += 2;
	    وإلا
		p += 1;
	}
	وإلا إذا ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    إذا (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    وإلا
		p++;
	}
	وإلا إذا (MB_ISUPPER(*p))
	    أرجع TRUE;
	وإلا
	    ++p;
    }
    أرجع FALSE;
}

#إذا defined(FEAT_EVAL)
    char_u *
وروستی_csearch(فراغ)
{
    أرجع lastc_بایټونه;
}

    صحيح
وروستی_csearch_forward(فراغ)
{
    أرجع lastcdir == FORWARD;
}

    صحيح
وروستی_csearch_until(فراغ)
{
    أرجع وروستی_t_cmd == TRUE;
}

    فراغ
سېټ_وروستی_csearch(صحيح c, char_u *s, صحيح len)
{
    *lastc = c;
    lastc_bytelen = len;
    إذا (len)
	حافظه_نقل(lastc_بایټونه, s, len);
    وإلا
	CLEAR_ډګر(lastc_بایټونه);
}
#د_که_پای

    فراغ
سېټ_csearch_direction(صحيح cdir)
{
    lastcdir = cdir;
}

    فراغ
سېټ_csearch_until(صحيح t_cmd)
{
    وروستی_t_cmd = t_cmd;
}

    char_u *
وروستی_پلټنه_pat(فراغ)
{
    أرجع spats[وروستی_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    فراغ
reset_پلټنه_dir(فراغ)
{
    spats[0].off.dir = '/';
#إذا defined(FEAT_EVAL)
    سېټ_vv_searchforward();
#د_که_پای
}

#إذا defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    فراغ
سېټ_وروستی_پلټنه_pat(
    char_u	*s,
    صحيح		idx,
    صحيح		magic,
    صحيح		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    إذا (*s == NUL)
	spats[idx].pat = عدم;
    وإلا
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    إذا (spats[idx].pat == عدم)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# إذا defined(FEAT_EVAL)
    سېټ_vv_searchforward();
# endif
    spats[idx].off.کرښه = FALSE;
    spats[idx].off.پای_ټکی = FALSE;
    spats[idx].off.off = 0;
    إذا (setlast)
	وروستی_idx = idx;
    إذا (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	إذا (spats[idx].pat == عدم)
	    saved_spats[idx].pat = عدم;
	وإلا
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	إذا (saved_spats[idx].pat == عدم)
	    saved_spats[idx].patlen = 0;
	وإلا
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_پلټنه_EXTRA
	saved_spats_وروستی_idx = وروستی_idx;
# endif
    }
# ifdef FEAT_پلټنه_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    إذا (p_hls && idx == وروستی_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#د_که_پای

#که_تعریف_شوی FEAT_پلټنه_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    فراغ
وروستی_pat_prog(regmmatch_T *regmatch)
{
    إذا (spats[وروستی_idx].pat == عدم)
    {
	regmatch->regprog = عدم;
	أرجع;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (فراغ)پلټنه_regcomp((char_u *)"", 0, عدم, 0, وروستی_idx, پلټنه_KEEP, regmatch);
    --emsg_off;
}
#د_که_پای

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
    pos_T	*پای_ټکی_pos,	// set to end of the match, unless NULL
    صحيح		dir,
    char_u	*pat,
    size_t	patlen,
    اوږد	شمېر,
    صحيح		options,
    صحيح		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    صحيح		وموندل;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	ستون;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    صحيح		loop;
    pos_T	پیل_pos;
    صحيح		at_لومړی_کرښه;
    صحيح		extra_ستون;
    صحيح		پیل_char_len;
    صحيح		سمون_ok;
    اوږد	nmatched;
    صحيح		submatch = 0;
    صحيح		لومړی_سمون = TRUE;
    صحيح		called_emsg_before = called_emsg;
#که_تعریف_شوی FEAT_پلټنه_EXTRA
    صحيح		break_loop = FALSE;
#د_که_پای
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    صحيح		ناکارول_شوی_timeout_flag = FALSE;
    صحيح		*timed_بهر = &ناکارول_شوی_timeout_flag;  // set when timed out.
    صحيح		پلټنه_from_سمون_پای_ټکی;		    // vi-compatible search?

    إذا (پلټنه_regcomp(pat, patlen, عدم, RE_پلټنه, pat_use,
		   (options & (پلټنه_HIS + پلټنه_KEEP)), &regmatch) == FAIL)
    {
	إذا ((options & پلټنه_پیغام) && !rc_did_emsg)
	    semsg(_(e_invalid_پلټنه_string_str), mr_نمونه);
	أرجع FAIL;
    }

    پلټنه_from_سمون_پای_ټکی = vim_strchr(p_cpo, CPO_پلټنه) != عدم;

    إذا (extra_arg != عدم)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#که_تعریف_شوی FEAT_RELTIME
	إذا (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_بهر = &extra_arg->sa_timed_بهر;
#د_که_پای
    }

    /*
     * find the string
     */
    افعل	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	إذا (pos->ستون == MAXCOL)
	    پیل_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	وإلا إذا (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_کرښه_شمېر
						    && pos->ستون < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    إذا (ml_get_buf_len(buf, pos->lnum) <= pos->ستون)
		پیل_char_len = 1;
	    وإلا
		پیل_char_len = (*mb_ptr2len)(ptr + pos->ستون);
	}
	وإلا
	    پیل_char_len = 1;
	إذا (dir == FORWARD)
	{
	    إذا (options & پلټنه_پیل)
		extra_ستون = 0;
	    وإلا
		extra_ستون = پیل_char_len;
	}
	وإلا
	{
	    إذا (options & پلټنه_پیل)
		extra_ستون = پیل_char_len;
	    وإلا
		extra_ستون = 0;
	}

	پیل_pos = *pos;	// remember start pos for detecting no match
	وموندل = 0;		// default: not found
	at_لومړی_کرښه = TRUE;	// default: start in first line
	إذا (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->ستون = 0;
	    at_لومړی_کرښه = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	إذا (dir == BACKWARD && پیل_pos.ستون == 0
					     && (options & پلټنه_پیل) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_لومړی_کرښه = FALSE;
	}
	وإلا
	    lnum = pos->lnum;

	لكل (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    لكل ( ; lnum > 0 && lnum <= buf->b_ml.ml_کرښه_شمېر;
					   lnum += dir, at_لومړی_کرښه = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		إذا (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    اكسر;
		// Stop after passing the time limit.
		إذا (*timed_بهر)
		    اكسر;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		ستون = at_لومړی_کرښه && (options & پلټنه_ستون) ? pos->ستون
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, ستون, timed_بهر);
		// vim_regexec_multi() may clear "regprog"
		إذا (regmatch.regprog == عدم)
		    اكسر;
		// Abort searching on an error (e.g., out of stack).
		إذا (called_emsg > called_emsg_before || *timed_بهر)
		    اكسر;
		إذا (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#که_تعریف_شوی FEAT_EVAL
		    submatch = لومړی_submatch(&regmatch);
#د_که_پای
		    // "lnum" may be past end of buffer for "\n\zs".
		    إذا (lnum + matchpos.lnum > buf->b_ml.ml_کرښه_شمېر)
			ptr = (char_u *)"";
		    وإلا
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    إذا (dir == FORWARD && at_لومړی_کرښه)
		    {
			سمون_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			طالما (matchpos.lnum == 0
				&& ((options & پلټنه_پای_ټکی) && لومړی_سمون
				    ?  (nmatched == 1
					&& (صحيح)endpos.ستون - 1
					     < (صحيح)پیل_pos.ستون + extra_ستون)
				    : ((صحيح)matchpos.ستون
						  - (ptr[matchpos.ستون] == NUL)
					    < (صحيح)پیل_pos.ستون + extra_ستون)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    إذا (پلټنه_from_سمون_پای_ټکی)
			    {
				إذا (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    سمون_ok = FALSE;
				    اكسر;
				}
				matchcol = endpos.ستون;
				// for empty match: advance one char
				إذا (matchcol == matchpos.ستون
						      && ptr[matchcol] != NUL)
				{
				    إذا (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وإلا
					++matchcol;
				}
			    }
			    وإلا
			    {
				// Advance "matchcol" to the next character.
				// This uses rmm_matchcol, the actual start of
				// the match, ignoring "\zs".
				matchcol = regmatch.rmm_matchcol;
				إذا (ptr[matchcol] != NUL)
				{
				    إذا (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    وإلا
					++matchcol;
				}
			    }
			    إذا (matchcol == 0 && (options & پلټنه_پیل))
				اكسر;
			    إذا (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_بهر)) == 0)
			    {
				سمون_ok = FALSE;
				اكسر;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    إذا (regmatch.regprog == عدم)
				اكسر;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#که_تعریف_شوی FEAT_EVAL
			    submatch = لومړی_submatch(&regmatch);
#د_که_پای

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			إذا (!سمون_ok)
			    استمر;
		    }
		    إذا (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			سمون_ok = FALSE;
			لكل (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    إذا (loop
				|| ((options & پلټنه_پای_ټکی)
				    ? (lnum + regmatch.endpos[0].lnum
							      < پیل_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == پیل_pos.lnum
					     && (صحيح)regmatch.endpos[0].ستون - 1
							< (صحيح)پیل_pos.ستون
								+ extra_ستون))
				    : (lnum + regmatch.startpos[0].lnum
							      < پیل_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == پیل_pos.lnum
					     && (صحيح)regmatch.startpos[0].ستون
						      < (صحيح)پیل_pos.ستون
							      + extra_ستون))))
			    {
				سمون_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#که_تعریف_شوی FEAT_EVAL
				submatch = لومړی_submatch(&regmatch);
#د_که_پای
			    }
			    وإلا
				اكسر;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    إذا (پلټنه_from_سمون_پای_ټکی)
			    {
				إذا (nmatched > 1)
				    اكسر;
				matchcol = endpos.ستون;
				// for empty match: advance one char
				إذا (matchcol == matchpos.ستون
						      && ptr[matchcol] != NUL)
				{
				    إذا (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وإلا
					++matchcol;
				}
			    }
			    وإلا
			    {
				// Stop when the match is in a next line.
				إذا (matchpos.lnum > 0)
				    اكسر;
				matchcol = matchpos.ستون;
				إذا (ptr[matchcol] != NUL)
				{
				    إذا (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    وإلا
					++matchcol;
				}
			    }
			    إذا (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_بهر)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				إذا (*timed_بهر)
				    سمون_ok = FALSE;
				اكسر;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    إذا (regmatch.regprog == عدم)
				اكسر;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			إذا (!سمون_ok)
			    استمر;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    إذا ((options & پلټنه_پای_ټکی) && !(options & پلټنه_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.ستون == endpos.ستون))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->ستون = endpos.ستون;
			إذا (endpos.ستون == 0)
			{
			    إذا (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->ستون = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			وإلا
			{
			    --pos->ستون;
			    إذا (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_کرښه_شمېر)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->ستون -= (*mb_سر_off)(ptr, ptr + pos->ستون);
			    }
			}
			إذا (پای_ټکی_pos != عدم)
			{
			    پای_ټکی_pos->lnum = lnum + matchpos.lnum;
			    پای_ټکی_pos->ستون = matchpos.ستون;
			}
		    }
		    وإلا
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->ستون = matchpos.ستون;
			إذا (پای_ټکی_pos != عدم)
			{
			    پای_ټکی_pos->lnum = lnum + endpos.lnum;
			    پای_ټکی_pos->ستون = endpos.ستون;
			}
		    }
		    pos->coladd = 0;
		    إذا (پای_ټکی_pos != عدم)
			پای_ټکی_pos->coladd = 0;
		    وموندل = 1;
		    لومړی_سمون = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    پلټنه_سمون_کرښې = endpos.lnum - matchpos.lnum;
		    پلټنه_سمون_endcol = endpos.ستون;
		    اكسر;
		}
		کرښه_breakcheck();	// stop if ctrl-C typed
		إذا (got_int)
		    اكسر;

#که_تعریف_شوی FEAT_پلټنه_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		إذا ((options & پلټنه_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    اكسر;
		}
#د_که_پای

		إذا (loop && lnum == پیل_pos.lnum)
		    اكسر;	    // if second loop, stop where started
	    }
	    at_لومړی_کرښه = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    إذا (regmatch.regprog == عدم)
		اكسر;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    إذا (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_بهر
#که_تعریف_شوی FEAT_پلټنه_EXTRA
			      || break_loop
#د_که_پای
			      || وموندل || loop)
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
	    إذا (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_کرښه_شمېر;
	    وإلا
		lnum = 1;
	    إذا (!shortmess(SHM_پلټنه)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & پلټنه_پیغام))
		give_warning((char_u *)_(dir == BACKWARD
					  ? سر_bot_پیغام : bot_سر_پیغام), TRUE);
	    إذا (extra_arg != عدم)
		extra_arg->sa_wrapped = TRUE;
	}
	إذا (got_int || called_emsg > called_emsg_before || *timed_بهر
#که_تعریف_شوی FEAT_پلټنه_EXTRA
		|| break_loop
#د_که_پای
		)
	    اكسر;
    }
    طالما (--شمېر > 0 && وموندل);   // stop after count matches or no match

#که_تعریف_شوی FEAT_RELTIME
    إذا (extra_arg != عدم && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#د_که_پای
    vim_regfree(regmatch.regprog);

    إذا (!وموندل)		    // did not find it
    {
	إذا (got_int)
	    emsg(_(e_interrupted));
	وإلا إذا ((options & پلټنه_پیغام) == پلټنه_پیغام)
	{
	    إذا (p_ws)
		semsg(_(e_نمونه_not_وموندل_str), mr_نمونه);
	    وإلا إذا (lnum == 0)
		semsg(_(e_پلټنه_موندنه_سر_without_سمون_for_str), mr_نمونه);
	    وإلا
		semsg(_(e_پلټنه_موندنه_bottom_without_سمون_for_str), mr_نمونه);
	}
	أرجع FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    إذا (pos->lnum > buf->b_ml.ml_کرښه_شمېر)
    {
	pos->lnum = buf->b_ml.ml_کرښه_شمېر;
	pos->ستون = ml_get_buf_len(buf, pos->lnum);
	إذا (pos->ستون > 0)
	    --pos->ستون;
    }

    أرجع submatch + 1;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
    فراغ
سېټ_پلټنه_direction(صحيح cdir)
{
    spats[0].off.dir = cdir;
}

    ساكن فراغ
سېټ_vv_searchforward(فراغ)
{
    سېټ_vim_var_nr(VV_SEARCHFORWARD, (اوږد)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ساكن صحيح
لومړی_submatch(regmmatch_T *rp)
{
    صحيح		submatch;

    لكل (submatch = 1; ; ++submatch)
    {
	إذا (rp->startpos[submatch].lnum >= 0)
	    اكسر;
	إذا (submatch == 9)
	{
	    submatch = 0;
	    اكسر;
	}
    }
    أرجع submatch;
}
#د_که_پای

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
parse_پلټنه_نمونه_دننه_واټن(
    char_u	**pat,
    size_t	*patlen,
    صحيح		پلټنه_delim,
    صحيح		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*دننه_واټن)
{
    صحيح		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    إذا (*pat == عدم || **pat == NUL)
	أرجع 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = عدم;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, پلټنه_delim, magic_isset(),
						    strcopy, عدم, عدم);
    إذا (*strcopy != ps)
    {
	size_t len = STRLEN(*strcopy);
	// made a copy of "pat" to change "\?" to "?"
	cmdlen += (صحيح)(*patlen - len);
	*pat = *strcopy;
	*patlen = len;
	*searchstr = *strcopy;
	*searchstrlen = len;
    }
    إذا (*p == پلټنه_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    دننه_واټن->کرښه = FALSE;
    دننه_واټن->پای_ټکی = FALSE;
    دننه_واټن->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    إذا (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	دننه_واټن->کرښه = TRUE;
    وإلا إذا ((options & پلټنه_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	إذا (*p == 'e')		// end
	    دننه_واټن->پای_ټکی = پلټنه_پای_ټکی;
	++p;
    }
    إذا (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	إذا (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    دننه_واټن->off = atol((حرف *)p);
	وإلا إذا (*p == '-')	    // single '-'
	    دننه_واټن->off = -1;
	وإلا			    // single '+'
	    دننه_واټن->off = 1;
	++p;
	طالما (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (صحيح)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    أرجع cmdlen;
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
do_پلټنه(
    oparg_T	    *oap,	// can be NULL
    صحيح		    dirc,	// '/' or '?'
    صحيح		    پلټنه_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    اوږد	    شمېر,
    صحيح		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    صحيح		    retval;	// Return value
    char_u	    *p;
    اوږد	    c;
    char_u	    *dircp;
    char_u	    *strcopy = عدم;
    صحيح		    show_پلټنه_stats;
    char_u	    *msgbuf = عدم;
    size_t	    msgbuflen = 0;
    صحيح		    has_دننه_واټن = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    إذا (spats[0].off.کرښه && vim_strchr(p_cpo, CPO_LINEOFF) != عدم)
    {
	spats[0].off.کرښه = FALSE;
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
    إذا (dirc == 0)
	dirc = spats[0].off.dir;
    وإلا
    {
	spats[0].off.dir = dirc;
#إذا defined(FEAT_EVAL)
	سېټ_vv_searchforward();
#د_که_پای
    }
    إذا (options & پلټنه_REV)
    {
#که_تعریف_شوی MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#وإلا
	إذا (dirc == '/')
	    dirc = '?';
	وإلا
	    dirc = '/';
#د_که_پای
    }

#که_تعریف_شوی FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    إذا (dirc == '/')
    {
	إذا (hasFolding(pos.lnum, عدم, &pos.lnum))
	    pos.ستون = MAXCOL - 2;	// avoid overflow when adding 1
    }
    وإلا
    {
	إذا (hasFolding(pos.lnum, &pos.lnum, عدم))
	    pos.ستون = 0;
    }
#د_که_پای

#که_تعریف_شوی FEAT_پلټنه_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    إذا (no_hlsearch && !(options & پلټنه_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	سېټ_no_hlsearch(FALSE);
    }
#د_که_پای

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    لكل (;;)
    {
	صحيح		show_سر_bot_پیغام = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = عدم;
					    // use previous pattern
	إذا (pat == عدم || *pat == NUL || *pat == پلټنه_delim)
	{
	    إذا (spats[RE_پلټنه].pat == عدم)	    // no previous pattern
	    {
		إذا (spats[RE_SUBST].pat == عدم)
		{
		    emsg(_(e_no_پخوانی_regular_افاده));
		    retval = 0;
		    اذهب پای_ټکی_do_پلټنه;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    وإلا
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	إذا (pat != عدم && *pat != NUL)	// look for (new) offset
	{
	    char_u	*prev_searchstr = searchstr;
	    size_t	prev_searchstrlen = searchstrlen;

	    searchcmdlen += parse_پلټنه_نمونه_دننه_واټن(&pat, &patlen,
				پلټنه_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    إذا (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_پلټنه_stats = FALSE;
	إذا ((options & پلټنه_ECHO) && messaging()
		&& !پیغام_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    پیغام_پیل();

	    // Get the offset, so we know how long it is.
	    إذا (!cmd_silent &&
		    (spats[0].off.کرښه || spats[0].off.پای_ټکی || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		إذا (spats[0].off.پای_ټکی)
		    off_buf[off_len++] = 'e';
		وإلا إذا (!spats[0].off.کرښه)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		إذا (spats[0].off.off != 0 || spats[0].off.کرښه)
		    off_len += vim_snprintf((حرف *)off_buf + off_len,
			   حجم(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    إذا (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    وإلا
	    {
		p = searchstr;
		plen = searchstrlen;
	    }

	    إذا (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		إذا (پیغام_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (صحيح)(لیکې - پیغام_لیکه) * cmdline_پلنوالی - 1;
		وإلا
		    // Use up to 'showcmd' column.
		    msgbufsize = (صحيح)(لیکې - پیغام_لیکه - 1) * cmdline_پلنوالی
			+ sc_ستون - 1;
		إذا (msgbufsize < plen + off_len + پلټنه_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + پلټنه_STAT_BUF_LEN + 3;
	    }
	    وإلا
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    إذا (msgbuf == عدم)
	    {
		msgbuflen = 0;
	    }
	    وإلا
	    {
		vim_memset(msgbuf, ' ', msgbufsize);
		msgbuflen = msgbufsize - 1;
		msgbuf[msgbuflen] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		إذا (!cmd_silent)
		{
		    char_u	*trunc;

		    msgbuf[0] = dirc;

		    إذا (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, plen);
		    }
		    وإلا
			mch_memmove(msgbuf + 1, p, plen);
		    إذا (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = پیغام_strtrunc(msgbuf, TRUE);
		    إذا (trunc != عدم)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#که_تعریف_شوی FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = سرچپه_متن(msgbuf);
			إذا (r != عدم)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    طالما (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    إذا ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    وإلا
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#د_که_پای
		    پیغام_outtrans(msgbuf);
		    پیغام_clr_eos();
		    پیغام_check();

		    gotocmdline(FALSE);
		    بهر_flush();
		    پیغام_nowait = TRUE;	    // don't wait for this message
		}

		إذا (!shortmess(SHM_SEARCHCOUNT))
		    show_پلټنه_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	إذا (!spats[0].off.کرښه && spats[0].off.off && pos.ستون < MAXCOL - 2)
	{
	    إذا (spats[0].off.off > 0)
	    {
		لكل (c = spats[0].off.off; c; --c)
		    إذا (decl(&pos) == -1)
			اكسر;
		إذا (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.ستون = MAXCOL;
		}
	    }
	    وإلا
	    {
		لكل (c = spats[0].off.off; c; ++c)
		    إذا (incl(&pos) == -1)
			اكسر;
		إذا (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_کرښه_شمېر + 1;
		    pos.ستون = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, عدم,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, شمېر, spats[0].off.پای_ټکی + (options &
		       (پلټنه_KEEP + پلټنه_PEEK + پلټنه_HIS
			+ پلټنه_پیغام + پلټنه_پیل
			+ ((pat != عدم && *pat == ';') ? 0 : پلټنه_NOOF))),
		RE_وروستی, sia);

	إذا (dircp != عدم)
	    *dircp = پلټنه_delim; // restore second '/' or '?' for normal_cmd()

	إذا (!shortmess(SHM_پلټنه) && sia && sia->sa_wrapped)
	    show_سر_bot_پیغام = TRUE;

	إذا (c == FAIL)
	{
	    retval = 0;
	    اذهب پای_ټکی_do_پلټنه;
	}
	إذا (spats[0].off.پای_ټکی && oap != عدم)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	إذا (!(options & پلټنه_NOOF) || (pat != عدم && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    إذا (spats[0].off.کرښه)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		إذا (c < 1)
		    pos.lnum = 1;
		وإلا إذا (c > curbuf->b_ml.ml_کرښه_شمېر)
		    pos.lnum = curbuf->b_ml.ml_کرښه_شمېر;
		وإلا
		    pos.lnum = c;
		pos.ستون = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    وإلا إذا (pos.ستون < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		إذا (c > 0)
		{
		    طالما (c-- > 0)
			إذا (incl(&pos) == -1)
			    اكسر;
		}
		// to the left, check for start of file
		وإلا
		{
		    طالما (c++ < 0)
			إذا (decl(&pos) == -1)
			    اكسر;
		}
	    }
	    إذا (!EQUAL_POS(pos, org_pos))
		has_دننه_واټن = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	إذا (show_پلټنه_stats)
	     cmdline_پلټنه_stat(dirc, &pos, &curwin->w_cursor,
				show_سر_bot_پیغام, msgbuf, msgbuflen,
				(شمېر != 1 || has_دننه_واټن
#که_تعریف_شوی FEAT_FOLDING
				 || (!(fdo_flags & FDO_پلټنه)
				     && hasFolding(curwin->w_cursor.lnum,
								   عدم, عدم))
#د_که_پای
				),
				p_msc,
				پلټنه_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	إذا (!(options & پلټنه_OPT) || pat == عدم || *pat != ';')
	    اكسر;

	dirc = *++pat;
	--patlen;
	پلټنه_delim = dirc;
	إذا (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    اذهب پای_ټکی_do_پلټنه;
	}
	++pat;
	--patlen;
    }

    إذا (options & پلټنه_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_سېټ_curswant = true;

پای_ټکی_do_پلټنه:
    إذا ((options & پلټنه_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    أرجع retval;
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
پلټنه_for_exact_کرښه(
    buf_T	*buf,
    pos_T	*pos,
    صحيح		dir,
    char_u	*pat)
{
    linenr_T	پیل = 0;
    char_u	*ptr;
    char_u	*p;
    صحيح		compl_len = ins_compl_len();

    إذا (buf->b_ml.ml_کرښه_شمېر == 0)
	أرجع FAIL;
    لكل (;;)
    {
	pos->lnum += dir;
	إذا (pos->lnum < 1)
	{
	    إذا (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_کرښه_شمېر;
		إذا (!shortmess(SHM_پلټنه))
		    give_warning((char_u *)_(سر_bot_پیغام), TRUE);
	    }
	    وإلا
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	وإلا إذا (pos->lnum > buf->b_ml.ml_کرښه_شمېر)
	{
	    إذا (p_ws)
	    {
		pos->lnum = 1;
		إذا (!shortmess(SHM_پلټنه))
		    give_warning((char_u *)_(bot_سر_پیغام), TRUE);
	    }
	    وإلا
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	إذا (pos->lnum == پیل)
	    اكسر;
	إذا (پیل == 0)
	    پیل = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->ستون = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	إذا (compl_حالت_adding() && !compl_حالت_sol())
	{
	    إذا ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		أرجع OK;
	}
	وإلا إذا (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    إذا ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		أرجع OK;
	}
    }
    أرجع FAIL;
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
    اوږد		شمېر = cap->count1;	// repeat count
    صحيح			ستون;
    char_u		*p;
    صحيح			len;
    صحيح			stop = TRUE;

    إذا (c != NUL)	// normal search: remember args for repeat
    {
	إذا (!کلی_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    سېټ_csearch_direction(dir);
	    سېټ_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_بایټونه);
	    إذا (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_بایټونه + lastc_bytelen);
		إذا (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_بایټونه + lastc_bytelen);
	    }
	}
    }
    وإلا		// repeat previous search
    {
	إذا (*lastc == NUL && lastc_bytelen <= 1)
	    أرجع FAIL;
	إذا (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	وإلا
	    dir = lastcdir;
	t_cmd = وروستی_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	إذا (vim_strchr(p_cpo, CPO_SCOLON) == عدم && شمېر == 1 && t_cmd)
	    stop = FALSE;
    }

    إذا (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    وإلا
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    ستون = curwin->w_cursor.ستون;
    len = ml_get_curline_len();

    طالما (شمېر--)
    {
	إذا (has_mbyte)
	{
	    لكل (;;)
	    {
		إذا (dir > 0)
		{
		    ستون += (*mb_ptr2len)(p + ستون);
		    إذا (ستون >= len)
			أرجع FAIL;
		}
		وإلا
		{
		    إذا (ستون == 0)
			أرجع FAIL;
		    ستون -= (*mb_سر_off)(p, p + ستون - 1) + 1;
		}
		إذا (lastc_bytelen <= 1)
		{
		    إذا (p[ستون] == c && stop)
			اكسر;
		}
		وإلا إذا (STRNCMP(p + ستون, lastc_بایټونه, lastc_bytelen) == 0
								       && stop)
		    اكسر;
		stop = TRUE;
	    }
	}
	وإلا
	{
	    لكل (;;)
	    {
		إذا ((ستون += dir) < 0 || ستون >= len)
		    أرجع FAIL;
		إذا (p[ستون] == c && stop)
		    اكسر;
		stop = TRUE;
	    }
	}
    }

    إذا (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	ستون -= dir;
	إذا (has_mbyte)
	{
	    إذا (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		ستون += lastc_bytelen - 1;
	    وإلا
		// To previous char, which may be multi-byte.
		ستون -= (*mb_سر_off)(p, p + ستون);
	}
    }
    curwin->w_cursor.ستون = ستون;

    أرجع OK;
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
    أرجع findmatchlimit(oap, initc, 0, 0);
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
    صحيح		ستون,
    صحيح		ch,
    صحيح		*prevcol)
{
    --ستون;
    إذا (ستون > 0 && has_mbyte)
	ستون -= (*mb_سر_off)(linep, linep + ستون);
    إذا (prevcol)
	*prevcol = ستون;
    أرجع (ستون >= 0 && linep[ستون] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ساكن صحيح
پیدا_کړه_rawstring_پای_ټکی(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    صحيح		وموندل = FALSE;

    لكل (p = linep + startpos->ستون + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->ستون - 1;
    delim_copy = vim_strnsave(linep + startpos->ستون + 1, delim_len);
    إذا (delim_copy == عدم)
	أرجع FALSE;
    لكل (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *کرښه = ml_get(lnum);

	لكل (p = کرښه + (lnum == startpos->lnum
					    ? startpos->ستون + 1 : 0); *p; ++p)
	{
	    إذا (lnum == endpos->lnum && (colnr_T)(p - کرښه) >= endpos->ستون)
		اكسر;
	    إذا (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		وموندل = TRUE;
		اكسر;
	    }
	}
	إذا (وموندل)
	    اكسر;
    }
    vim_free(delim_copy);
    أرجع وموندل;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ساكن فراغ
پیدا_کړه_mps_ارزښتونه(
    صحيح	    *initc,
    صحيح	    *findc,
    صحيح	    *backwards,
    صحيح	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    طالما (*ptr != NUL)
    {
	إذا (has_mbyte)
	{
	    char_u *prev;

	    إذا (mb_ptr2char(ptr) == *initc)
	    {
		إذا (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		وإلا
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		أرجع;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    إذا (mb_ptr2char(ptr) == *initc)
	    {
		إذا (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		وإلا
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		أرجع;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	وإلا
	{
	    إذا (*ptr == *initc)
	    {
		إذا (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		وإلا
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		أرجع;
	    }
	    ptr += 2;
	    إذا (*ptr == *initc)
	    {
		إذا (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		وإلا
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		أرجع;
	    }
	    ++ptr;
	}
	إذا (*ptr == ',')
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
    صحيح		شمېر = 0;		// cumulative number of braces
    صحيح		backwards = FALSE;	// init for gcc
    صحيح		raw_string = FALSE;	// search for raw string
    صحيح		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    صحيح		do_quotes;		// check for quotes in current line
    صحيح		at_پیل;		// do_quotes value at start position
    صحيح		hash_dir = 0;		// Direction searched for # things
    صحيح		comment_dir = 0;	// Direction searched for comments
    pos_T	سمون_pos;		// Where last slash-star was found
    صحيح		پیل_دننه_quotes;	// start position is in quotes
    صحيح		traveled = 0;		// how far we've searched so far
    صحيح		ignore_cend = FALSE;    // ignore comment end
    صحيح		cpo_سمون;		// vi compatible matching
    صحيح		cpo_bsl;		// don't recognize backslashes
    صحيح		سمون_escaped = 0;	// search for escaped match
    صحيح		dir;			// Direction to search
    صحيح		comment_ستون = MAXCOL;   // start of / / comment
    صحيح		lispcomm = FALSE;	// inside of Lisp-style comment
    صحيح		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    صحيح		skip_comments = (flags & FM_SKIPCOMM) != 0;
    صحيح		دننه_بلاک_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_سمون = (vim_strchr(p_cpo, CPO_سمون) != عدم);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != عدم);

    // Direction to search when initc is '/', '*' or '#'
    إذا (flags & FM_BACKWARD)
	dir = BACKWARD;
    وإلا إذا (flags & FM_FORWARD)
	dir = FORWARD;
    وإلا
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    إذا (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	إذا (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    وإلا إذا (initc != '#' && initc != NUL)
    {
	پیدا_کړه_mps_ارزښتونه(&initc, &findc, &backwards, TRUE);
	إذا (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	إذا (findc == NUL)
	    أرجع عدم;
    }
    وإلا
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	إذا (initc == '#')
	{
	    hash_dir = dir;
	}
	وإلا
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    إذا (!cpo_سمون)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		إذا (*ptr == '#' && pos.ستون <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    إذا (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		وإلا إذا (linep[pos.ستون] == '/')
		{
		    إذا (linep[pos.ستون + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.ستون++;
		    }
		    وإلا إذا (pos.ستون > 0 && linep[pos.ستون - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.ستون--;
		    }
		}
		وإلا إذا (linep[pos.ستون] == '*')
		{
		    إذا (linep[pos.ستون + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    وإلا إذا (pos.ستون > 0 && linep[pos.ستون - 1] == '/')
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
	    إذا (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		إذا (linep[pos.ستون] == NUL && pos.ستون)
		    --pos.ستون;
		لكل (;;)
		{
		    initc = PTR2CHAR(linep + pos.ستون);
		    إذا (initc == NUL)
			اكسر;

		    پیدا_کړه_mps_ارزښتونه(&initc, &findc, &backwards, FALSE);
		    إذا (findc)
			اكسر;
		    pos.ستون += mb_ptr2len(linep + pos.ستون);
		}
		إذا (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    إذا (!cpo_سمون && *skipwhite(linep) == '#')
			hash_dir = 1;
		    وإلا
			أرجع عدم;
		}
		وإلا إذا (!cpo_bsl)
		{
		    صحيح ستون, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    لكل (ستون = pos.ستون; check_prevcol(linep, ستون, '\\', &ستون);)
			bslcnt++;
		    سمون_escaped = (bslcnt & 1);
		}
	    }
	}
	إذا (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    إذا (oap != عدم)
		oap->motion_type = MLINE;   // Linewise for this case only
	    إذا (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		إذا (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		وإلا إذا (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		وإلا
		    أرجع عدم;
	    }
	    pos.ستون = 0;
	    طالما (!got_int)
	    {
		إذا (hash_dir > 0)
		{
		    إذا (pos.lnum == curbuf->b_ml.ml_کرښه_شمېر)
			اكسر;
		}
		وإلا إذا (pos.lnum == 1)
		    اكسر;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		کرښه_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		إذا (*ptr != '#')
		    استمر;
		pos.ستون = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		إذا (hash_dir > 0)
		{
		    إذا (STRNCMP(ptr, "if", 2) == 0)
			شمېر++;
		    وإلا إذا (STRNCMP(ptr, "el", 2) == 0)
		    {
			إذا (شمېر == 0)
			    أرجع &pos;
		    }
		    وإلا إذا (STRNCMP(ptr, "endif", 5) == 0)
		    {
			إذا (شمېر == 0)
			    أرجع &pos;
			شمېر--;
		    }
		}
		وإلا
		{
		    إذا (STRNCMP(ptr, "if", 2) == 0)
		    {
			إذا (شمېر == 0)
			    أرجع &pos;
			شمېر--;
		    }
		    وإلا إذا (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			إذا (شمېر == 0)
			    أرجع &pos;
		    }
		    وإلا إذا (STRNCMP(ptr, "endif", 5) == 0)
			شمېر++;
		}
	    }
	    أرجع عدم;
	}
    }

#که_تعریف_شوی FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    إذا (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != عدم)
	backwards = !backwards;
#د_که_پای

    do_quotes = -1;
    پیل_دننه_quotes = MAYBE;
    CLEAR_POS(&سمون_pos);

    // backward search: Check if this line contains a single-line comment
    إذا ((backwards && comment_dir) || lisp || skip_comments)
	comment_ستون = check_linecomment(linep);
    إذا (lisp && comment_ستون != MAXCOL && pos.ستون > (colnr_T)comment_ستون)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    إذا (skip_comments && !دننه_بلاک_comment && comment_ستون != MAXCOL
	    && backwards && pos.ستون > (colnr_T)comment_ستون)
	pos.ستون = comment_ستون;

    طالما (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	إذا (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    إذا (lispcomm && pos.ستون < (colnr_T)comment_ستون)
		اكسر;
	    إذا (pos.ستون == 0)		// at start of line, go to prev. one
	    {
		إذا (pos.lnum == 1)	// start of file
		    اكسر;
		--pos.lnum;

		إذا (maxtravel > 0 && ++traveled > maxtravel)
		    اكسر;

		linep = ml_get(pos.lnum);
		pos.ستون = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		کرښه_breakcheck();

		// Check if this line contains a single-line comment
		إذا (comment_dir || lisp || skip_comments)
		    comment_ستون = check_linecomment(linep);
		// skip comment
		إذا (lisp && comment_ستون != MAXCOL)
		    pos.ستون = comment_ستون;
		وإلا إذا (skip_comments && !دننه_بلاک_comment
			&& comment_ستون != MAXCOL
			&& pos.ستون > (colnr_T)comment_ستون)
		    pos.ستون = comment_ستون;
	    }
	    وإلا
	    {
		--pos.ستون;
		إذا (has_mbyte)
		    pos.ستون -= (*mb_سر_off)(linep, linep + pos.ستون);
	    }
	}
	وإلا				// forward search
	{
	    إذا (linep[pos.ستون] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_ستون != MAXCOL
					   && pos.ستون == (colnr_T)comment_ستون))
	    {
		إذا (pos.lnum == curbuf->b_ml.ml_کرښه_شمېر  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    اكسر;
		++pos.lnum;

		إذا (maxtravel && traveled++ > maxtravel)
		    اكسر;

		linep = ml_get(pos.lnum);
		pos.ستون = 0;
		do_quotes = -1;
		کرښه_breakcheck();
		إذا (lisp || skip_comments)   // find comment pos in new line
		    comment_ستون = check_linecomment(linep);
	    }
	    وإلا
	    {
		إذا (has_mbyte)
		    pos.ستون += (*mb_ptr2len)(linep + pos.ستون);
		وإلا
		    ++pos.ستون;
	    }
	}

	// Track block comment state when FM_SKIPCOMM is set.  Markers inside a
	// string are not comments, so skip them while "inquote" is set.
	// Backward: '/' of end-marker enters comment; '*' of start-marker exits.
	// Forward:  '/' of start-marker enters comment; '/' of end-marker exits.
	إذا (skip_comments && !comment_dir && !inquote)
	{
	    إذا (backwards)
	    {
		// Guard pos.col < comment_col: don't misread '*/' at the '//'
		// position as a block-comment end-marker.
		إذا (!دننه_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/'
			&& (comment_ستون == MAXCOL || (صحيح)pos.ستون < comment_ستون))
		    دننه_بلاک_comment = TRUE;
		وإلا إذا (دننه_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '/' && linep[pos.ستون] == '*')
		    دننه_بلاک_comment = FALSE;
	    }
	    وإلا
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		إذا (!دننه_بلاک_comment && linep[pos.ستون] == '/'
			&& linep[pos.ستون + 1] == '*'
			&& (comment_ستون == MAXCOL || (صحيح)pos.ستون < comment_ستون))
		    دننه_بلاک_comment = TRUE;
		وإلا إذا (دننه_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		    دننه_بلاک_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	إذا (pos.ستون == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    إذا (linep[0] == findc && شمېر == 0)	// match!
		أرجع &pos;
	    اكسر;					// out of scope
	}

	إذا (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    إذا (comment_dir == FORWARD)
	    {
		إذا (linep[pos.ستون] == '*' && linep[pos.ستون + 1] == '/')
		{
		    pos.ستون++;
		    أرجع &pos;
		}
	    }
	    وإلا    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		إذا (pos.ستون == 0)
		    استمر;
		وإلا إذا (raw_string)
		{
		    إذا (linep[pos.ستون - 1] == 'R'
			&& linep[pos.ستون] == '"'
			&& vim_strchr(linep + pos.ستون + 1, '(') != عدم)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			إذا (!پیدا_کړه_rawstring_پای_ټکی(linep, &pos,
				  شمېر > 0 ? &سمون_pos : &curwin->w_cursor))
			{
			    شمېر++;
			    سمون_pos = pos;
			    سمون_pos.ستون--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		وإلا إذا (  linep[pos.ستون - 1] == '/'
			&& linep[pos.ستون] == '*'
			&& (pos.ستون == 1 || linep[pos.ستون - 2] != '*')
			&& (صحيح)pos.ستون < comment_ستون)
		{
		    شمېر++;
		    سمون_pos = pos;
		    سمون_pos.ستون--;
		}
		وإلا إذا (linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		{
		    إذا (شمېر > 0)
			pos = سمون_pos;
		    وإلا إذا (pos.ستون > 1 && linep[pos.ستون - 2] == '/'
					       && (صحيح)pos.ستون <= comment_ستون)
			pos.ستون -= 2;
		    وإلا إذا (ignore_cend)
			استمر;
		    وإلا
			أرجع عدم;
		    أرجع &pos;
		}
	    }
	    استمر;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	إذا (cpo_سمون)
	    do_quotes = 0;
	وإلا إذا (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_پیل = do_quotes;
	    لكل (ptr = linep; *ptr; ++ptr)
	    {
		إذا (ptr == linep + pos.ستون + backwards)
		    at_پیل = (do_quotes & 1);
		إذا (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		إذا (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    إذا (!do_quotes)
	    {
		inquote = FALSE;
		إذا (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    إذا (پیل_دننه_quotes == MAYBE)
		    {
			inquote = at_پیل;
			إذا (inquote)
			    پیل_دننه_quotes = TRUE;
		    }
		    وإلا إذا (backwards)
			inquote = TRUE;
		}
		إذا (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    إذا (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			إذا (پیل_دننه_quotes == MAYBE)
			{
			    inquote = at_پیل;
			    إذا (inquote)
				پیل_دننه_quotes = TRUE;
			}
			وإلا إذا (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	إذا (پیل_دننه_quotes == MAYBE)
	    پیل_دننه_quotes = FALSE;

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
	c = PTR2CHAR(linep + pos.ستون);
	بدل (c)
	{
	حالة NUL:
	    // at end of line without trailing backslash, reset inquote
	    إذا (pos.ستون == 0 || linep[pos.ستون - 1] != '\\')
	    {
		inquote = FALSE;
		پیل_دننه_quotes = FALSE;
	    }
	    اكسر;

	حالة '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    إذا (do_quotes)
	    {
		صحيح ستون;

		لكل (ستون = pos.ستون - 1; ستون >= 0; --ستون)
		    إذا (linep[ستون] != '\\')
			اكسر;
		إذا ((((صحيح)pos.ستون - 1 - ستون) & 1) == 0)
		{
		    inquote = !inquote;
		    پیل_دننه_quotes = FALSE;
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
	    إذا (!cpo_سمون && initc != '\'' && findc != '\'')
	    {
		إذا (backwards)
		{
		    إذا (pos.ستون > 1)
		    {
			إذا (linep[pos.ستون - 2] == '\'')
			{
			    pos.ستون -= 2;
			    اكسر;
			}
			وإلا إذا (linep[pos.ستون - 2] == '\\'
				  && pos.ستون > 2 && linep[pos.ستون - 3] == '\'')
			{
			    pos.ستون -= 3;
			    اكسر;
			}
		    }
		}
		وإلا إذا (linep[pos.ستون + 1])	// forward search
		{
		    إذا (linep[pos.ستون + 1] == '\\'
			   && linep[pos.ستون + 2] && linep[pos.ستون + 3] == '\'')
		    {
			pos.ستون += 3;
			اكسر;
		    }
		    وإلا إذا (linep[pos.ستون + 2] == '\'')
		    {
			pos.ستون += 2;
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
	    إذا (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != عدم
		    && pos.ستون > 1
		    && check_prevcol(linep, pos.ستون, '\\', عدم)
		    && check_prevcol(linep, pos.ستون - 1, '#', عدم))
		اكسر;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    إذا (skip_comments && (دننه_بلاک_comment
		    || (comment_ستون != MAXCOL && (صحيح)pos.ستون >= comment_ستون)))
		اكسر;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    إذا ((!inquote || پیل_دننه_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		صحيح	ستون, bslcnt = 0;

		إذا (!cpo_bsl)
		{
		    لكل (ستون = pos.ستون; check_prevcol(linep, ستون, '\\', &ستون);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		إذا (cpo_bsl || (bslcnt & 1) == سمون_escaped)
		{
		    إذا (c == initc)
			شمېر++;
		    وإلا
		    {
			إذا (شمېر == 0)
			    أرجع &pos;
			شمېر--;
		    }
		}
	    }
	}
    }

    إذا (comment_dir == BACKWARD && شمېر > 0)
    {
	pos = سمون_pos;
	أرجع &pos;
    }
    أرجع (pos_T *)عدم;	// never found it
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
    اوږد	save_so;
    اوږد	save_siso;
#که_تعریف_شوی CURSOR_SHAPE
    صحيح		save_state;
#د_که_پای
    colnr_T	save_dollar_vcol;
    char_u	*p;
    اوږد	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    اوږد	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    لكل (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#که_تعریف_شوی FEAT_RIGHTLEFT
	إذا (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    اكسر;
#د_که_پای
	p += mb_ptr2len(p) + 1;
	إذا (PTR2CHAR(p) == c
#که_تعریف_شوی FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#د_که_پای
	   )
	    اكسر;
	p += mb_ptr2len(p);
	إذا (*p == NUL)
	    أرجع;
    }
    إذا (*p == NUL)
	أرجع;

    إذا ((lpos = findmatch(عدم, NUL)) == عدم)	    // no match, so beep
    {
	vim_beep(BO_سمون);
	أرجع;
    }

    إذا (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	أرجع;

    إذا (!curwin->w_p_wrap)
	getvcol(curwin, lpos, عدم, &vcol, عدم, 0);

    صحيح ستون_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_پلنوالی));
    إذا (!ستون_visible)
	أرجع;

    mpos = *lpos;    // save the pos, update_screen() may change it
    save_cursor = curwin->w_cursor;
    save_so = *so;
    save_siso = *siso;
    // Handle "$" in 'cpo': If the ')' is typed on top of the "$",
    // stop displaying the "$".
    إذا (dollar_vcol >= 0 && dollar_vcol == curwin->w_virtcol)
	dollar_vcol = -1;
    ++curwin->w_virtcol;	// do display ')' just before "$"
    update_screen(UPD_VALID);	// show the new char first

    save_dollar_vcol = dollar_vcol;
#که_تعریف_شوی CURSOR_SHAPE
    save_state = State;
    State = ډېرپېښ_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#د_که_پای
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    بهر_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    إذا (vim_strchr(p_cpo, CPO_SHOWMATCH) != عدم)
	ui_delay(p_mat * 100L + 8, TRUE);
    وإلا إذا (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#که_تعریف_شوی CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#د_که_پای
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ساكن صحيح
is_zero_پلنوالی(
    char_u	*نمونه,
    size_t	patternlen,
    صحيح		move,
    pos_T	*cur,
    صحيح		direction)
{
    regmmatch_T	regmatch;
    صحيح		nmatched = 0;
    صحيح		پایله = -1;
    pos_T	pos;
    صحيح		called_emsg_before = called_emsg;
    صحيح		flag = 0;

    إذا (نمونه == عدم)
    {
	نمونه = spats[وروستی_idx].pat;
	patternlen = spats[وروستی_idx].patlen;
    }

    إذا (پلټنه_regcomp(نمونه, patternlen, عدم, RE_پلټنه, RE_پلټنه,
					      پلټنه_KEEP, &regmatch) == FAIL)
	أرجع -1;

    // init startcol correctly
    regmatch.startpos[0].ستون = -1;
    // move to match
    إذا (move)
    {
	CLEAR_POS(&pos);
    }
    وإلا
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = پلټنه_پیل;
    }

    إذا (searchit(curwin, curbuf, &pos, عدم, direction, نمونه, patternlen, 1,
				  پلټنه_KEEP + flag, RE_پلټنه, عدم) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	افعل
	{
	    regmatch.startpos[0].ستون++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].ستون, عدم);
	    إذا (nmatched != 0)
		اكسر;
	} طالما (regmatch.regprog != عدم
		&& (direction == FORWARD ? regmatch.startpos[0].ستون < pos.ستون
				      : regmatch.startpos[0].ستون > pos.ستون));

	إذا (called_emsg == called_emsg_before)
	{
	    پایله = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].ستون == regmatch.endpos[0].ستون);
	}
    }

    vim_regfree(regmatch.regprog);
    أرجع پایله;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحيح
current_پلټنه(
    اوږد	شمېر,
    صحيح		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	پیل_pos;	// start position of the pattern match
    pos_T	پای_ټکی_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    صحيح		i;
    صحيح		dir;
    صحيح		پایله;		// result of various function calls
    char_u	old_p_ws = p_ws;
    صحيح		flags = 0;
    pos_T	save_VIsual = VIsual;
    صحيح		zero_پلنوالی;
    صحيح		skip_لومړی_backward;

    // Correct cursor when 'selection' is exclusive
    إذا (VIsual_فعال && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_لومړی_backward = forward && VIsual_فعال
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    إذا (VIsual_فعال)
    {
	إذا (forward)
	    incl(&pos);
	وإلا
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_پلنوالی = is_zero_پلنوالی(spats[وروستی_idx].pat, spats[وروستی_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    إذا (zero_پلنوالی == -1)
	أرجع FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    لكل (i = 0; i < 2; i++)
    {
	إذا (forward)
	{
	    إذا (i == 0 && skip_لومړی_backward)
		استمر;
	    dir = i;
	}
	وإلا
	    dir = !i;

	flags = 0;
	إذا (!dir && !zero_پلنوالی)
	    flags = پلټنه_پای_ټکی;
	پای_ټکی_pos = pos;

	// wrapping should not occur in the first round
	إذا (i == 0)
	    p_ws = FALSE;

	پایله = searchit(curwin, curbuf, &pos, &پای_ټکی_pos,
		(dir ? FORWARD : BACKWARD),
		spats[وروستی_idx].pat, spats[وروستی_idx].patlen, (اوږد) (i ? شمېر : 1),
		پلټنه_KEEP | flags, RE_پلټنه, عدم);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	إذا (i == 1 && !پایله) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    إذا (VIsual_فعال)
		VIsual = save_VIsual;
	    أرجع FAIL;
	}
	وإلا إذا (i == 0 && !پایله)
	{
	    إذا (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    وإلا
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_بفر->b_ml.ml_کرښه_شمېر;
		pos.ستون  = ml_get_len(curwin->w_بفر->b_ml.ml_کرښه_شمېر);
	    }
	}
    }

    پیل_pos = pos;

    إذا (!VIsual_فعال)
	VIsual = پیل_pos;

    // put the cursor after the match
    curwin->w_cursor = پای_ټکی_pos;
    إذا (LT_POS(VIsual, پای_ټکی_pos) && forward)
    {
	إذا (skip_لومړی_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	وإلا
	    // put the cursor on last character of match
	    dec_cursor();
    }
    وإلا إذا (VIsual_فعال && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_فعال = TRUE;
    VIsual_ډېرپېښ = 'v';

    إذا (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	إذا (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	وإلا إذا (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#که_تعریف_شوی FEAT_FOLDING
    إذا (fdo_flags & FDO_پلټنه && کلی_Typed)
	foldOpenCursor();
#د_که_پای

    may_پیل_select('c');
    setmouse();
#که_تعریف_شوی FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_ستوری.vmode = NUL;
#د_که_پای
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    أرجع OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    صحيح
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    أرجع (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ساكن فراغ
cmdline_پلټنه_stat(
    صحيح		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    صحيح		show_سر_bot_پیغام,
    char_u	*msgbuf,
    size_t	msgbuflen,
    صحيح		recompute,
    صحيح		maxcount,
    اوږد	timeout)
{
    searchstat_T stat;

    update_پلټنه_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    إذا (stat.cur <= 0)
	أرجع;

    حرف	t[پلټنه_STAT_BUF_LEN];
    size_t	len;

#که_تعریف_شوی FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	إذا (stat.incomplete == 1)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[?/??]");
	وإلا إذا (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وإلا إذا (stat.cnt > maxcount)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	وإلا
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    وإلا
#د_که_پای
    {
	إذا (stat.incomplete == 1)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[?/??]");
	وإلا إذا (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وإلا إذا (stat.cnt > maxcount)
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	وإلا
	    len = vim_snprintf(t, پلټنه_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    إذا (show_سر_bot_پیغام && len + 2 < پلټنه_STAT_BUF_LEN)
    {
	mch_memmove(t + 2, t, len);
	t[0] = 'W';
	t[1] = ' ';
	len += 2;
    }

    إذا (len > msgbuflen)
	len = msgbuflen;
    mch_memmove(msgbuf + msgbuflen - len, t, len);

    إذا (dirc == '?' && stat.cur == maxcount + 1)
	stat.cur = -1;

    // keep the message even after redraw, but don't put in history
    پیغام_hist_off = TRUE;
    give_warning(msgbuf, FALSE);
    پیغام_hist_off = FALSE;
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
update_پلټنه_stat(
    صحيح			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    صحيح			recompute,
    صحيح			maxcount,
    اوږد		timeout ناکارول_شوی)
{
    صحيح		    save_ws = p_ws;
    صحيح		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ساكن pos_T    lastpos = {0, 0, 0};
    ساكن صحيح	    cur = 0;
    ساكن صحيح	    cnt = 0;
    ساكن صحيح	    exact_سمون = FALSE;
    ساكن صحيح	    incomplete = 0;
    ساكن صحيح	    وروستی_maxcount = 0;
    ساكن صحيح	    chgtick = 0;
    ساكن char_u   *lastpat = عدم;
    ساكن size_t   lastpatlen = 0;
    ساكن buf_T    *lbuf = عدم;
#که_تعریف_شوی FEAT_RELTIME
    proftime_T  پیل;
#د_که_پای

    CLEAR_اشاره_ګر(stat);

    إذا (dirc == 0 && !recompute && !تشه_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_سمون = exact_سمون;
	stat->incomplete = incomplete;
	stat->وروستی_maxcount = وروستی_maxcount;
	أرجع;
    }
    وروستی_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    إذا (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != عدم
	    && STRNCMP(lastpat, spats[وروستی_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[وروستی_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_سمون = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    إذا (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    وإلا
    {
	صحيح	بشپړ_پلټنه = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#که_تعریف_شوی FEAT_RELTIME
	إذا (timeout > 0)
	    profile_setlimit(timeout, &پیل);
#د_که_پای
	طالما (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, عدم, 0, 1, پلټنه_KEEP, RE_وروستی, عدم) != FAIL)
	{
	    بشپړ_پلټنه = TRUE;
#که_تعریف_شوی FEAT_RELTIME
	    // Stop after passing the time limit.
	    إذا (timeout > 0 && profile_passed_حد(&پیل))
	    {
		incomplete = 1;
		اكسر;
	    }
#د_که_پای
	    cnt++;
	    إذا (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		إذا (LT_POS(p, endpos))
		    exact_سمون = TRUE;
	    }
	    fast_breakcheck();
	    إذا (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		اكسر;
	    }
	}
	إذا (got_int)
	    cur = -1; // abort
	إذا (بشپړ_پلټنه)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[وروستی_idx].pat, spats[وروستی_idx].patlen);
	    إذا (lastpat == عدم)
		lastpatlen = 0;
	    وإلا
		lastpatlen = spats[وروستی_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_سمون = exact_سمون;
    stat->incomplete = incomplete;
    stat->وروستی_maxcount = وروستی_maxcount;
    p_ws = save_ws;
}

#إذا defined(FEAT_پیدا_کړه_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ساكن char_u *
get_کرښه_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *کرښه = ml_get(lnum);

    vim_strncpy(buf, کرښه, LSIZE - 1);
    أرجع buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    فراغ
پیدا_کړه_نمونه_دننه_لار(
    char_u	*ptr,		// pointer to search pattern
    صحيح		dir ناکارول_شوی,	// direction of expansion
    صحيح		len,		// length of search pattern
    صحيح		ټولیز,		// match whole words only
    صحيح		skip_comments,	// don't match inside comments
    صحيح		type,		// Type of search; are we looking for a type?
				// a macro?
    اوږد	شمېر,
    صحيح		action,		// What to do when we find it
    linenr_T	پیل_lnum,	// first line to start searching
    linenr_T	پای_ټکی_lnum,	// last line for searching
    صحيح		forceit,	// If true, always switch to the found path
    صحيح		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    صحيح		max_لار_ژوروالی = 50;
    اوږد	سمون_شمېر = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = عدم;
    linenr_T	lnum;
    صحيح		ژوروالی;
    صحيح		ژوروالی_displayed;	// For type==CHECK_PATH
    صحيح		old_files;
    صحيح		already_searched;
    char_u	*file_کرښه;
    char_u	*کرښه;
    char_u	*p;
    char_u	save_char;
    صحيح		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    صحيح		matched = FALSE;
    صحيح		did_show = FALSE;
    صحيح		وموندل = FALSE;
    صحيح		i;
    char_u	*already = عدم;
    char_u	*startp = عدم;
    char_u	*inc_opt = عدم;
# إذا defined(FEAT_QUICKFIX)
    win_T	*curwin_save = عدم;
# endif

    regmatch.regprog = عدم;
    incl_regmatch.regprog = عدم;
    def_regmatch.regprog = عدم;

    file_کرښه = alloc(LSIZE);
    إذا (file_کرښه == عدم)
	أرجع;

    إذا (type != CHECK_لار && type != پیدا_کړه_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_حالت_sol())
    {
	pat = alloc(len + 5);
	إذا (pat == عدم)
	    اذهب fpip_پای_ټکی;
	vim_snprintf((حرف *)pat, len + 5, ټولیز ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	إذا (regmatch.regprog == عدم)
	    اذهب fpip_پای_ټکی;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    إذا (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	إذا (incl_regmatch.regprog == عدم)
	    اذهب fpip_پای_ټکی;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    إذا (type == پیدا_کړه_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	إذا (def_regmatch.regprog == عدم)
	    اذهب fpip_پای_ټکی;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_لار_ژوروالی * حجم(SearchedFile), TRUE);
    إذا (files == عدم)
	اذهب fpip_پای_ټکی;
    old_files = max_لار_ژوروالی;
    ژوروالی = ژوروالی_displayed = -1;

    lnum = پیل_lnum;
    إذا (پای_ټکی_lnum > curbuf->b_ml.ml_کرښه_شمېر)
	پای_ټکی_lnum = curbuf->b_ml.ml_کرښه_شمېر;
    إذا (lnum > پای_ټکی_lnum)		// do at least one line
	lnum = پای_ټکی_lnum;
    کرښه = get_کرښه_and_copy(lnum, file_کرښه);

    لكل (;;)
    {
	إذا (incl_regmatch.regprog != عدم
		&& vim_regexec(&incl_regmatch, کرښه, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    إذا (مزي_پلټنه((حرف *)inc_opt, "\\zs") != عدم)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = پیدا_کړه_file_name_دننه_لار(incl_regmatch.startp[0],
		       (صحيح)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    وإلا
		// Use text after match with 'include'.
		new_fname = file_name_دننه_کرښه(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, عدم);
	    already_searched = FALSE;
	    إذا (new_fname != عدم)
	    {
		// Check whether we have already searched in this file
		لكل (i = 0;; i++)
		{
		    إذا (i == ژوروالی + 1)
			i = old_files;
		    إذا (i == max_لار_ژوروالی)
			اكسر;
		    إذا (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			إذا (type != CHECK_لار
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    پیغام_putchar('\n');	    // cursor below last one
			    إذا (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //پیغام
			    {
				پیغام_home_replace_hl(new_fname);
				پیغام_puts(_(" (includes previously listed match)"));
				prev_fname = عدم;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			اكسر;
		    }
		}
	    }

	    إذا (type == CHECK_لار && (action == ACTION_SHOW_ALL
				 || (new_fname == عدم && !already_searched)))
	    {
		إذا (did_show)
		    پیغام_putchar('\n');	    // cursor below last one
		وإلا
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    پیغام_puts_title(_("--- Included files "));
		    إذا (action != ACTION_SHOW_ALL)
			پیغام_puts_title(_("not found "));
		    پیغام_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		طالما (ژوروالی_displayed < ژوروالی && !got_int)
		{
		    ++ژوروالی_displayed;
		    لكل (i = 0; i < ژوروالی_displayed; i++)
			پیغام_puts("  ");
		    پیغام_home_replace(files[ژوروالی_displayed].name);
		    پیغام_puts(" -->\n");
		}
		إذا (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    لكل (i = 0; i <= ژوروالی_displayed; i++)
			پیغام_puts("  ");
		    إذا (new_fname != عدم)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			پیغام_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    وإلا
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			إذا (مزي_پلټنه((حرف *)inc_opt, "\\zs") != عدم)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (صحيح)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			وإلا
			{
			    // find the file name after the end of the match
			    لكل (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    لكل (i = 0; vim_isfilec(p[i]); i++)
				;
			}

			إذا (i == 0)
			{
			    // Nothing found, use the rest of the line.
			    p = incl_regmatch.endp[0];
			    i = (صحيح)STRLEN(p);
			}
			// Avoid checking before the start of the line, can
			// happen if \zs appears in the regexp.
			وإلا إذا (p > کرښه)
			{
			    إذا (p[-1] == '"' || p[-1] == '<')
			    {
				--p;
				++i;
			    }
			    إذا (p[i] == '"' || p[i] == '>')
				++i;
			}
			save_char = p[i];
			p[i] = NUL;
			پیغام_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    إذا (new_fname == عدم && action == ACTION_SHOW_ALL)
		    {
			إذا (already_searched)
			    پیغام_puts(_("  (Already listed)"));
			وإلا
			    پیغام_puts(_("  NOT FOUND"));
		    }
		}
		بهر_flush();	    // output each line directly
	    }

	    إذا (new_fname != عدم)
	    {
		// Push the new file onto the file stack
		إذا (ژوروالی + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_لار_ژوروالی * 2);
		    إذا (bigger != عدم)
		    {
			لكل (i = 0; i <= ژوروالی; i++)
			    bigger[i] = files[i];
			لكل (i = ژوروالی + 1; i < old_files + max_لار_ژوروالی; i++)
			{
			    bigger[i].fp = عدم;
			    bigger[i].name = عدم;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			لكل (i = old_files; i < max_لار_ژوروالی; i++)
			    bigger[i + max_لار_ژوروالی] = files[i];
			old_files += max_لار_ژوروالی;
			max_لار_ژوروالی *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		إذا ((files[ژوروالی + 1].fp = mch_fopen((حرف *)new_fname, "r"))
								    == عدم)
		    vim_free(new_fname);
		وإلا
		{
		    إذا (++ژوروالی == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[ژوروالی].name = curr_fname = new_fname;
		    files[ژوروالی].lnum = 0;
		    files[ژوروالی].matched = FALSE;
		    إذا (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			پیغام_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((حرف*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(حرف *)new_fname);
			پیغام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    وإلا إذا (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (حرف *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	وإلا
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = کرښه;
پلټنه_کرښه:
	    define_matched = FALSE;
	    إذا (def_regmatch.regprog != عدم
			      && vim_regexec(&def_regmatch, کرښه, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		طالما (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    إذا (def_regmatch.regprog == عدم || define_matched)
	    {
		إذا (define_matched || compl_حالت_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    إذا (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    وإلا
			matched = STRNCMP(startp, ptr, len) == 0;
		    إذا (matched && define_matched && ټولیز
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		وإلا إذا (regmatch.regprog != عدم
			 && vim_regexec(&regmatch, کرښه, (colnr_T)(p - کرښه)))
		{
		    matched = TRUE;
		    startp = regmatch.startp[0];
		    /*
		     * Check if the line is not a comment line (unless we are
		     * looking for a define).  A line starting with "# define"
		     * is not considered to be a comment line.
		     */
		    إذا (!define_matched && skip_comments)
		    {
			إذا ((*کرښه != '#' ||
				STRNCMP(skipwhite(کرښه + 1), "define", 6) != 0)
				&& get_leader_len(کرښه, عدم, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(کرښه);
			إذا (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    لكل (p = کرښه; *p && p < startp; ++p)
			    {
				إذا (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    إذا (p[1] == '/')
					اكسر;
				    ++p;
				}
				وإلا إذا (!matched && p[0] == '*' && p[1] == '/')
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
	إذا (matched)
	{
	    إذا (action == ACTION_EXPAND)
	    {
		صحيح	cont_s_ipos = FALSE;
		صحيح	جمع_کړه_r;
		char_u	*aux;

		إذا (ژوروالی == -1 && lnum == curwin->w_cursor.lnum)
		    اكسر;
		وموندل = TRUE;
		aux = p = startp;
		إذا (compl_حالت_adding() && (صحيح)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    إذا (vim_iswordp(p))
			اذهب exit_matched;
		    p = پیدا_کړه_کلمه_پیل(p);
		}
		p = پیدا_کړه_کلمه_پای_ټکی(p);
		i = (صحيح)(p - aux);

		إذا (compl_حالت_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    إذا (ژوروالی < 0)
		    {
			إذا (lnum >= پای_ټکی_lnum)
			    اذهب exit_matched;
			کرښه = get_کرښه_and_copy(++lnum, file_کرښه);
		    }
		    وإلا إذا (vim_fgets(کرښه = file_کرښه,
						      LSIZE, files[ژوروالی].fp))
			اذهب exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(کرښه);
		    p = پیدا_کړه_کلمه_پیل(p);
		    p = پیدا_کړه_کلمه_پای_ټکی(p);
		    إذا (p > aux)
		    {
			إذا (*aux != ')' && IObuff[i-1] != TAB)
			{
			    إذا (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    إذا (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == عدم
					&& (IObuff[i-2] == '?'
					    || IObuff[i-2] == '!'))))
				IObuff[i++] = ' ';
			}
			// copy as much as possible of the new word
			إذا (p - aux >= IOSIZE - i)
			    p = aux + IOSIZE - i - 1;
			STRNCPY(IObuff + i, aux, p - aux);
			i += (صحيح)(p - aux);
			cont_s_ipos = TRUE;
		    }
		    IObuff[i] = NUL;
		    aux = IObuff;

		    إذا (i == ins_compl_len())
			اذهب exit_matched;
		}

		جمع_کړه_r = ins_compl_جمع_کړه_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? عدم
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		إذا (جمع_کړه_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		وإلا إذا (جمع_کړه_r == FAIL)
		    اكسر;
	    }
	    وإلا إذا (action == ACTION_SHOW_ALL)
	    {
		وموندل = TRUE;
		إذا (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		إذا (curr_fname != prev_fname)
		{
		    إذا (did_show)
			پیغام_putchar('\n');	// cursor below last one
		    إذا (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			پیغام_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		إذا (!got_int)
		    show_pat_دننه_لار(کرښه, type, TRUE, action,
			    (ژوروالی == -1) ? عدم : files[ژوروالی].fp,
			    (ژوروالی == -1) ? &lnum : &files[ژوروالی].lnum,
			    سمون_شمېر++);

		// Set matched flag for this file and all the ones that
		// include it
		لكل (i = 0; i <= ژوروالی; ++i)
		    files[i].matched = TRUE;
	    }
	    وإلا إذا (--شمېر <= 0)
	    {
		وموندل = TRUE;
		إذا (ژوروالی == -1 && lnum == curwin->w_cursor.lnum
# إذا defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_سمون_is_on_current_کرښه));
		وإلا إذا (action == ACTION_SHOW)
		{
		    show_pat_دننه_لار(کرښه, type, did_show, action,
			    (ژوروالی == -1) ? عدم : files[ژوروالی].fp,
			    (ژوروالی == -1) ? &lnum : &files[ژوروالی].lnum, 1L);
		    did_show = TRUE;
		}
		وإلا
		{
# ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
# endif
# إذا defined(FEAT_QUICKFIX)
		    // ":psearch" uses the preview window
		    إذا (g_do_tagpreview != 0)
		    {
			curwin_save = curwin;
			prepare_tagpreview(TRUE, TRUE, FALSE);
		    }
# endif
		    إذا (action == ACTION_وېشل)
		    {
			إذا (win_وېشل(0, 0) == FAIL)
			    اكسر;
			RESET_BINDING(curwin);
		    }
		    إذا (ژوروالی == -1)
		    {
			// match in current file
# إذا defined(FEAT_QUICKFIX)
			إذا (g_do_tagpreview != 0)
			{
			    إذا (!win_valid(curwin_save))
				اكسر;
			    إذا (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_بفر->b_fnum, عدم,
						     عدم, TRUE, lnum, forceit)))
				اكسر;	// failed to jump to file
			}
			وإلا
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    وإلا
		    {
			إذا (!GETFILE_SUCCESS(getfile(
					0, files[ژوروالی].name, عدم, TRUE,
						    files[ژوروالی].lnum, forceit)))
			    اكسر;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[ژوروالی].lnum;
		    }
		}
		إذا (action != ACTION_SHOW)
		{
		    curwin->w_cursor.ستون = (colnr_T)(startp - کرښه);
		    curwin->w_سېټ_curswant = true;
		}

# إذا defined(FEAT_QUICKFIX)
		إذا (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
		{
		    // Return cursor to where we were
		    validate_cursor();
		    redraw_later(UPD_VALID);
		    win_enter(curwin_save, TRUE);
		}
#  ifdef FEAT_PROP_POPUP
		وإلا إذا (WIN_IS_POPUP(curwin))
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
	    إذا (def_regmatch.regprog == عدم
		    && action == ACTION_EXPAND
		    && !compl_حالت_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		اذهب پلټنه_کرښه;
	}
	کرښه_breakcheck();
	إذا (action == ACTION_EXPAND)
	    ins_compl_check_کلیګانې(30, FALSE);
	إذا (got_int || ins_compl_interrupted())
	    اكسر;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	طالما (ژوروالی >= 0 && !already
		&& vim_fgets(کرښه = file_کرښه, LSIZE, files[ژوروالی].fp))
	{
	    fclose(files[ژوروالی].fp);
	    --old_files;
	    files[old_files].name = files[ژوروالی].name;
	    files[old_files].matched = files[ژوروالی].matched;
	    --ژوروالی;
	    curr_fname = (ژوروالی == -1) ? curbuf->b_fname
				       : files[ژوروالی].name;
	    إذا (ژوروالی < ژوروالی_displayed)
		ژوروالی_displayed = ژوروالی;
	}
	إذا (ژوروالی >= 0)		// we could read the line
	{
	    files[ژوروالی].lnum++;
	    // Remove any CR and LF from the line.
	    i = (صحيح)STRLEN(کرښه);
	    إذا (i > 0 && کرښه[i - 1] == '\n')
		کرښه[--i] = NUL;
	    إذا (i > 0 && کرښه[i - 1] == '\r')
		کرښه[--i] = NUL;
	}
	وإلا إذا (!already)
	{
	    إذا (++lnum > پای_ټکی_lnum)
		اكسر;
	    کرښه = get_کرښه_and_copy(lnum, file_کرښه);
	}
	already = عدم;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    لكل (i = 0; i <= ژوروالی; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    لكل (i = old_files; i < max_لار_ژوروالی; i++)
	vim_free(files[i].name);
    vim_free(files);

    إذا (type == CHECK_لار)
    {
	إذا (!did_show)
	{
	    إذا (action != ACTION_SHOW_ALL)
		پیغام(_("All included files were found"));
	    وإلا
		پیغام(_("No included files"));
	}
    }
    وإلا إذا (!وموندل && action != ACTION_EXPAND && !silent)
    {
	إذا (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	وإلا إذا (type == پیدا_کړه_DEFINE)
	    emsg(_(e_couldnt_پیدا_کړه_definition));
	وإلا
	    emsg(_(e_couldnt_پیدا_کړه_نمونه));
    }
    إذا (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	پیغام_پای_ټکی();

fpip_پای_ټکی:
    vim_free(file_کرښه);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ساكن فراغ
show_pat_دننه_لار(
    char_u  *کرښه,
    صحيح	    type,
    صحيح	    did_show,
    صحيح	    action,
    FILE    *fp,
    linenr_T *lnum,
    اوږد    شمېر)
{
    char_u  *p;
    size_t  linelen;

    إذا (did_show)
	پیغام_putchar('\n');	// cursor below last one
    وإلا إذا (!پیغام_silent)
	gotocmdline(TRUE);	// cursor at status line
    إذا (got_int)		// 'q' typed at "--more--" message
	أرجع;
    linelen = STRLEN(کرښه);
    لكل (;;)
    {
	p = کرښه + linelen - 1;
	إذا (fp != عدم)
	{
	    // We used fgets(), so get rid of newline at end
	    إذا (p >= کرښه && *p == '\n')
		--p;
	    إذا (p >= کرښه && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	إذا (action == ACTION_SHOW_ALL)
	{
	    sprintf((حرف *)IObuff, "%3ld: ", شمېر);	// show match nr
	    پیغام_puts((حرف *)IObuff);
	    sprintf((حرف *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    پیغام_puts_attr((حرف *)IObuff, HL_ATTR(HLF_N));
	    پیغام_puts(" ");
	}
	پیغام_prt_کرښه(کرښه, FALSE);
	بهر_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	إذا (got_int || type != پیدا_کړه_DEFINE || p < کرښه || *p != '\\')
	    اكسر;

	إذا (fp != عدم)
	{
	    إذا (vim_fgets(کرښه, LSIZE, fp)) // end of file
		اكسر;
	    linelen = STRLEN(کرښه);
	    ++*lnum;
	}
	وإلا
	{
	    إذا (++*lnum > curbuf->b_ml.ml_کرښه_شمېر)
		اكسر;
	    کرښه = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	پیغام_putchar('\n');
    }
}
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(صحيح idx)
{
    أرجع &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    صحيح
get_spat_وروستی_idx(فراغ)
{
    أرجع وروستی_idx;
}
#د_که_پای

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    فراغ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*نمونه = عدم;
    صحيح			maxcount = p_msc;
    اوږد		timeout = پلټنه_STAT_DEF_TIMEOUT;
    صحيح			recompute = TRUE;
    searchstat_T	stat;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	صحيح		error = FALSE;

	إذا (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    أرجع;
	dict = argvars[0].vval.v_dict;
	di = dict_پیدا_کړه(dict, (char_u *)"timeout", -1);
	إذا (di != عدم)
	{
	    timeout = (اوږد)tv_get_شمېره_chk(&di->di_tv, &error);
	    إذا (error)
		أرجع;
	}
	di = dict_پیدا_کړه(dict, (char_u *)"maxcount", -1);
	إذا (di != عدم)
	{
	    maxcount = (صحيح)tv_get_شمېره_chk(&di->di_tv, &error);
	    إذا (error)
		أرجع;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_پیدا_کړه(dict, (char_u *)"pattern", -1);
	إذا (di != عدم)
	{
	    نمونه = tv_get_string_chk(&di->di_tv);
	    إذا (نمونه == عدم)
		أرجع;
	}
	di = dict_پیدا_کړه(dict, (char_u *)"pos", -1);
	إذا (di != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_دلیل_str), "pos");
		أرجع;
	    }
	    إذا (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_دلیل_str), "List format should be [lnum, col, off]");
		أرجع;
	    }
	    li = list_پیدا_کړه(di->di_tv.vval.v_list, 0L);
	    إذا (li != عدم)
	    {
		pos.lnum = tv_get_شمېره_chk(&li->li_tv, &error);
		إذا (error)
		    أرجع;
	    }
	    li = list_پیدا_کړه(di->di_tv.vval.v_list, 1L);
	    إذا (li != عدم)
	    {
		pos.ستون = tv_get_شمېره_chk(&li->li_tv, &error) - 1;
		إذا (error)
		    أرجع;
	    }
	    li = list_پیدا_کړه(di->di_tv.vval.v_list, 2L);
	    إذا (li != عدم)
	    {
		pos.coladd = tv_get_شمېره_chk(&li->li_tv, &error);
		إذا (error)
		    أرجع;
	    }
	}
    }

    save_وروستی_پلټنه_نمونه();
# ifdef FEAT_پلټنه_EXTRA
    save_incsearch_state();
# endif
    إذا (نمونه != عدم)
    {
	إذا (*نمونه == NUL)
	    اذهب the_پای_ټکی;
	vim_free(spats[وروستی_idx].pat);
	spats[وروستی_idx].patlen = STRLEN(نمونه);
	spats[وروستی_idx].pat = vim_strnsave(نمونه, spats[وروستی_idx].patlen);
	إذا (spats[وروستی_idx].pat == عدم)
	    spats[وروستی_idx].patlen = 0;
    }
    إذا (spats[وروستی_idx].pat == عدم || *spats[وروستی_idx].pat == NUL)
	اذهب the_پای_ټکی;	// the previous pattern was never defined

    update_پلټنه_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_جمع_کړه_شمېره(rettv->vval.v_dict, "current", stat.cur);
    dict_جمع_کړه_شمېره(rettv->vval.v_dict, "total", stat.cnt);
    dict_جمع_کړه_شمېره(rettv->vval.v_dict, "exact_match", stat.exact_سمون);
    dict_جمع_کړه_شمېره(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_جمع_کړه_شمېره(rettv->vval.v_dict, "maxcount", stat.وروستی_maxcount);

the_پای_ټکی:
    restore_وروستی_پلټنه_نمونه();
# ifdef FEAT_پلټنه_EXTRA
    restore_incsearch_state();
# endif
}
#د_که_پای
