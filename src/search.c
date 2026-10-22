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

#اگر_تعریف_شده FEAT_EVAL
ساكن فراغ مجموعه_vv_searchforward(فراغ);
ساكن صحيح اولی_submatch(regmmatch_T *rp);
#پایان_اگر
#اگر_تعریف_شده FEAT_پیدا_کن_ID
ساكن char_u *get_سطر_and_copy(linenr_T lnum, char_u *buf);
ساكن فراغ show_pat_درون_مسیر(char_u *, صحيح, صحيح, صحيح, FILE *, linenr_T *, دراز);
#پایان_اگر

تعريفنوع بنية searchstat
{
    صحيح	    cur;	    // current position of found words
    صحيح	    cnt;	    // total count of found words
    صحيح	    exact_تطابق;    // TRUE if matched exactly on specified position
    صحيح	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    صحيح	    آخرین_maxcount;  // the max count of the last search
} searchstat_T;

#اگر_تعریف_شده FEAT_جستجو_EXTRA
ساكن فراغ save_incsearch_state(فراغ);
ساكن فراغ restore_incsearch_state(فراغ);
#پایان_اگر
ساكن صحيح check_prevcol(char_u *linep, صحيح ستون, صحيح ch, صحيح *prevcol);
ساكن صحيح پیدا_کن_rawstring_انتها(char_u *linep, pos_T *startpos, pos_T *endpos);
ساكن فراغ پیدا_کن_mps_مقادیر(صحيح *initc, صحيح *findc, صحيح *backwards, صحيح switchit);
ساكن صحيح is_zero_عرض(char_u *الگو, size_t patternlen, صحيح move, pos_T *cur, صحيح direction);
ساكن فراغ cmdline_جستجو_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, صحيح show_بالاترین_bot_پیام, char_u *msgbuf, size_t msgbuflen, صحيح recompute, صحيح maxcount, دراز timeout);
ساكن فراغ update_جستجو_stat(صحيح dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, صحيح recompute, صحيح maxcount, دراز timeout);

#تعریف_کن جستجو_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#تعریف_کن جستجو_STAT_BUF_LEN 16

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

ساكن صحيح آخرین_idx = 0;	// index in spats[] for RE_LAST

ساكن char_u lastc[2] = {NUL, NUL};	// last character searched for
ساكن صحيح lastcdir = FORWARD;		// last direction of character search
ساكن صحيح آخرین_t_cmd = TRUE;		// last search t_cmd
ساكن char_u	lastc_بایتها[MB_MAXBYTES + 1];
ساكن صحيح	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ساكن spat_T	    saved_spats[ARRAY_درازی(spats)];
ساكن char_u	    *saved_mr_الگو = عدم;
ساكن size_t	    saved_mr_patternlen = 0;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
ساكن صحيح	    saved_spats_آخرین_idx = 0;
ساكن صحيح	    saved_spats_no_hlsearch = 0;
#پایان_اگر

// allocated copy of pattern used by search_regcomp()
ساكن char_u	    *mr_الگو = عدم;
ساكن size_t	    mr_patternlen = 0;

#اگر_تعریف_شده FEAT_پیدا_کن_ID
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
#پایان_اگر

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
جستجو_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**استفاده_شده_pat,
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

	إذا (pat_use == RE_آخرین)
	    i = آخرین_idx;
	وإلا
	    i = pat_use;
	إذا (spats[i].pat == عدم)	// pattern was never defined
	{
	    إذا (pat_use == RE_SUBST)
		emsg(_(e_no_قبلی_substitute_regular_عبارت));
	    وإلا
		emsg(_(e_no_قبلی_regular_عبارت));
	    rc_did_emsg = TRUE;
	    أرجع FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    وإلا إذا (options & جستجو_HIS)	// put new pattern in history
	جمع_کن_مقصد_history(HIST_جستجو, pat, patlen, TRUE, NUL);

    إذا (استفاده_شده_pat)
	*استفاده_شده_pat = pat;

    vim_free(mr_الگو);
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_الگو = معکوس_متن(pat);
    وإلا
#پایان_اگر
	mr_الگو = vim_strnsave(pat, patlen);
    إذا (mr_الگو == عدم)
	mr_patternlen = 0;
    وإلا
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    إذا (!(options & جستجو_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	إذا (pat_save == RE_جستجو || pat_save == RE_BOTH)
	    save_re_pat(RE_جستجو, pat, patlen, magic);
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
get_جستجو_pat(فراغ)
{
    أرجع mr_الگو;
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
    آخرین_idx = idx;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    إذا (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    مجموعه_no_hlsearch(FALSE);
#پایان_اگر
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ساكن صحيح save_level = 0;

    فراغ
save_جستجو_patterns(فراغ)
{
    صحيح i;

    إذا (save_level++ != 0)
	أرجع;

    لكل (i = 0; i < (صحيح)ARRAY_درازی(spats); ++i)
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
    إذا (mr_الگو == عدم)
	saved_mr_الگو = عدم;
    وإلا
	saved_mr_الگو = vim_strnsave(mr_الگو, mr_patternlen);
    إذا (saved_mr_الگو == عدم)
	saved_mr_patternlen = 0;
    وإلا
	saved_mr_patternlen = mr_patternlen;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    saved_spats_آخرین_idx = آخرین_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#پایان_اگر
}

    فراغ
restore_جستجو_patterns(فراغ)
{
    صحيح i;

    إذا (--save_level != 0)
	أرجع;

    لكل (i = 0; i < (صحيح)ARRAY_درازی(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#إذا defined(FEAT_EVAL)
    مجموعه_vv_searchforward();
#پایان_اگر
    vim_free(mr_الگو);
    mr_الگو = saved_mr_الگو;
    mr_patternlen = saved_mr_patternlen;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    آخرین_idx = saved_spats_آخرین_idx;
    مجموعه_no_hlsearch(saved_spats_no_hlsearch);
#پایان_اگر
}

#إذا defined(EXITFREE)
    فراغ
free_جستجو_patterns(فراغ)
{
    صحيح i;

    لكل (i = 0; i < (صحيح)ARRAY_درازی(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_الگو);
    mr_patternlen = 0;
}
#پایان_اگر

#اگر_تعریف_شده FEAT_جستجو_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ساكن spat_T	    saved_آخرین_جستجو_spat;
ساكن صحيح	    did_save_آخرین_جستجو_spat = 0;
ساكن صحيح	    saved_آخرین_idx = 0;
ساكن صحيح	    saved_no_hlsearch = 0;
ساكن صحيح	    saved_جستجو_تطابق_endcol;
ساكن صحيح	    saved_جستجو_تطابق_سطرها;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    فراغ
save_آخرین_جستجو_الگو(فراغ)
{
    إذا (++did_save_آخرین_جستجو_spat != 1)
	// nested call, nothing to do
	أرجع;

    saved_آخرین_جستجو_spat = spats[RE_جستجو];
    إذا (spats[RE_جستجو].pat != عدم)
    {
	saved_آخرین_جستجو_spat.pat = vim_strnsave(spats[RE_جستجو].pat, spats[RE_جستجو].patlen);
	إذا (saved_آخرین_جستجو_spat.pat == عدم)
	    saved_آخرین_جستجو_spat.patlen = 0;
	وإلا
	    saved_آخرین_جستجو_spat.patlen = spats[RE_جستجو].patlen;
    }
    saved_آخرین_idx = آخرین_idx;
    saved_no_hlsearch = no_hlsearch;
}

    فراغ
restore_آخرین_جستجو_الگو(فراغ)
{
    إذا (--did_save_آخرین_جستجو_spat > 0)
	// nested call, nothing to do
	أرجع;
    إذا (did_save_آخرین_جستجو_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	أرجع;
    }

    vim_free(spats[RE_جستجو].pat);
    spats[RE_جستجو] = saved_آخرین_جستجو_spat;
    saved_آخرین_جستجو_spat.pat = عدم;
    saved_آخرین_جستجو_spat.patlen = 0;
# إذا defined(FEAT_EVAL)
    مجموعه_vv_searchforward();
# endif
    آخرین_idx = saved_آخرین_idx;
    مجموعه_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ساكن فراغ
save_incsearch_state(فراغ)
{
    saved_جستجو_تطابق_endcol = جستجو_تطابق_endcol;
    saved_جستجو_تطابق_سطرها  = جستجو_تطابق_سطرها;
}

    ساكن فراغ
restore_incsearch_state(فراغ)
{
    جستجو_تطابق_endcol = saved_جستجو_تطابق_endcol;
    جستجو_تطابق_سطرها  = saved_جستجو_تطابق_سطرها;
}

    char_u *
آخرین_جستجو_الگو(فراغ)
{
    أرجع spats[RE_جستجو].pat;
}

    size_t
آخرین_جستجو_الگو_len(فراغ)
{
    أرجع spats[RE_جستجو].patlen;
}
#پایان_اگر

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
ignorecase_opt(char_u *pat, صحيح ic_درون, صحيح scs)
{
    صحيح		ic = ic_درون;

    إذا (ic && !no_smartcase && scs
			    && !(ctrl_x_نما_not_default() && curbuf->b_p_inf))
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
آخرین_csearch(فراغ)
{
    أرجع lastc_بایتها;
}

    صحيح
آخرین_csearch_forward(فراغ)
{
    أرجع lastcdir == FORWARD;
}

    صحيح
آخرین_csearch_until(فراغ)
{
    أرجع آخرین_t_cmd == TRUE;
}

    فراغ
مجموعه_آخرین_csearch(صحيح c, char_u *s, صحيح len)
{
    *lastc = c;
    lastc_bytelen = len;
    إذا (len)
	کپی_حافظه(lastc_بایتها, s, len);
    وإلا
	CLEAR_فیلد(lastc_بایتها);
}
#پایان_اگر

    فراغ
مجموعه_csearch_direction(صحيح cdir)
{
    lastcdir = cdir;
}

    فراغ
مجموعه_csearch_until(صحيح t_cmd)
{
    آخرین_t_cmd = t_cmd;
}

    char_u *
آخرین_جستجو_pat(فراغ)
{
    أرجع spats[آخرین_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    فراغ
reset_جستجو_dir(فراغ)
{
    spats[0].off.dir = '/';
#إذا defined(FEAT_EVAL)
    مجموعه_vv_searchforward();
#پایان_اگر
}

#إذا defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    فراغ
مجموعه_آخرین_جستجو_pat(
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
    مجموعه_vv_searchforward();
# endif
    spats[idx].off.سطر = FALSE;
    spats[idx].off.انتها = FALSE;
    spats[idx].off.off = 0;
    إذا (setlast)
	آخرین_idx = idx;
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
# ifdef FEAT_جستجو_EXTRA
	saved_spats_آخرین_idx = آخرین_idx;
# endif
    }
# ifdef FEAT_جستجو_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    إذا (p_hls && idx == آخرین_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#پایان_اگر

#اگر_تعریف_شده FEAT_جستجو_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    فراغ
آخرین_pat_prog(regmmatch_T *regmatch)
{
    إذا (spats[آخرین_idx].pat == عدم)
    {
	regmatch->regprog = عدم;
	أرجع;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (فراغ)جستجو_regcomp((char_u *)"", 0, عدم, 0, آخرین_idx, جستجو_KEEP, regmatch);
    --emsg_off;
}
#پایان_اگر

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
    pos_T	*انتها_pos,	// set to end of the match, unless NULL
    صحيح		dir,
    char_u	*pat,
    size_t	patlen,
    دراز	شمار,
    صحيح		options,
    صحيح		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    صحيح		یافت;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	ستون;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    صحيح		loop;
    pos_T	شروع_pos;
    صحيح		at_اولی_سطر;
    صحيح		extra_ستون;
    صحيح		شروع_char_len;
    صحيح		تطابق_ok;
    دراز	nmatched;
    صحيح		submatch = 0;
    صحيح		اولی_تطابق = TRUE;
    صحيح		called_emsg_before = called_emsg;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    صحيح		break_loop = FALSE;
#پایان_اگر
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    صحيح		استفادهنشده_timeout_flag = FALSE;
    صحيح		*timed_بیرون = &استفادهنشده_timeout_flag;  // set when timed out.
    صحيح		جستجو_from_تطابق_انتها;		    // vi-compatible search?

    إذا (جستجو_regcomp(pat, patlen, عدم, RE_جستجو, pat_use,
		   (options & (جستجو_HIS + جستجو_KEEP)), &regmatch) == FAIL)
    {
	إذا ((options & جستجو_پیام) && !rc_did_emsg)
	    semsg(_(e_invalid_جستجو_string_str), mr_الگو);
	أرجع FAIL;
    }

    جستجو_from_تطابق_انتها = vim_strchr(p_cpo, CPO_جستجو) != عدم;

    إذا (extra_arg != عدم)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#اگر_تعریف_شده FEAT_RELTIME
	إذا (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_بیرون = &extra_arg->sa_timed_بیرون;
#پایان_اگر
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
	    شروع_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	وإلا إذا (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_سطر_شمار
						    && pos->ستون < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    إذا (ml_get_buf_len(buf, pos->lnum) <= pos->ستون)
		شروع_char_len = 1;
	    وإلا
		شروع_char_len = (*mb_ptr2len)(ptr + pos->ستون);
	}
	وإلا
	    شروع_char_len = 1;
	إذا (dir == FORWARD)
	{
	    إذا (options & جستجو_شروع)
		extra_ستون = 0;
	    وإلا
		extra_ستون = شروع_char_len;
	}
	وإلا
	{
	    إذا (options & جستجو_شروع)
		extra_ستون = شروع_char_len;
	    وإلا
		extra_ستون = 0;
	}

	شروع_pos = *pos;	// remember start pos for detecting no match
	یافت = 0;		// default: not found
	at_اولی_سطر = TRUE;	// default: start in first line
	إذا (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->ستون = 0;
	    at_اولی_سطر = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	إذا (dir == BACKWARD && شروع_pos.ستون == 0
					     && (options & جستجو_شروع) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_اولی_سطر = FALSE;
	}
	وإلا
	    lnum = pos->lnum;

	لكل (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    لكل ( ; lnum > 0 && lnum <= buf->b_ml.ml_سطر_شمار;
					   lnum += dir, at_اولی_سطر = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		إذا (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    اكسر;
		// Stop after passing the time limit.
		إذا (*timed_بیرون)
		    اكسر;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		ستون = at_اولی_سطر && (options & جستجو_ستون) ? pos->ستون
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, ستون, timed_بیرون);
		// vim_regexec_multi() may clear "regprog"
		إذا (regmatch.regprog == عدم)
		    اكسر;
		// Abort searching on an error (e.g., out of stack).
		إذا (called_emsg > called_emsg_before || *timed_بیرون)
		    اكسر;
		إذا (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#اگر_تعریف_شده FEAT_EVAL
		    submatch = اولی_submatch(&regmatch);
#پایان_اگر
		    // "lnum" may be past end of buffer for "\n\zs".
		    إذا (lnum + matchpos.lnum > buf->b_ml.ml_سطر_شمار)
			ptr = (char_u *)"";
		    وإلا
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    إذا (dir == FORWARD && at_اولی_سطر)
		    {
			تطابق_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			طالما (matchpos.lnum == 0
				&& ((options & جستجو_انتها) && اولی_تطابق
				    ?  (nmatched == 1
					&& (صحيح)endpos.ستون - 1
					     < (صحيح)شروع_pos.ستون + extra_ستون)
				    : ((صحيح)matchpos.ستون
						  - (ptr[matchpos.ستون] == NUL)
					    < (صحيح)شروع_pos.ستون + extra_ستون)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    إذا (جستجو_from_تطابق_انتها)
			    {
				إذا (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    تطابق_ok = FALSE;
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
			    إذا (matchcol == 0 && (options & جستجو_شروع))
				اكسر;
			    إذا (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_بیرون)) == 0)
			    {
				تطابق_ok = FALSE;
				اكسر;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    إذا (regmatch.regprog == عدم)
				اكسر;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#اگر_تعریف_شده FEAT_EVAL
			    submatch = اولی_submatch(&regmatch);
#پایان_اگر

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			إذا (!تطابق_ok)
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
			تطابق_ok = FALSE;
			لكل (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    إذا (loop
				|| ((options & جستجو_انتها)
				    ? (lnum + regmatch.endpos[0].lnum
							      < شروع_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == شروع_pos.lnum
					     && (صحيح)regmatch.endpos[0].ستون - 1
							< (صحيح)شروع_pos.ستون
								+ extra_ستون))
				    : (lnum + regmatch.startpos[0].lnum
							      < شروع_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == شروع_pos.lnum
					     && (صحيح)regmatch.startpos[0].ستون
						      < (صحيح)شروع_pos.ستون
							      + extra_ستون))))
			    {
				تطابق_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#اگر_تعریف_شده FEAT_EVAL
				submatch = اولی_submatch(&regmatch);
#پایان_اگر
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
			    إذا (جستجو_from_تطابق_انتها)
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
					      matchcol, timed_بیرون)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				إذا (*timed_بیرون)
				    تطابق_ok = FALSE;
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
			إذا (!تطابق_ok)
			    استمر;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    إذا ((options & جستجو_انتها) && !(options & جستجو_NOOF)
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
				    && pos->lnum <= buf->b_ml.ml_سطر_شمار)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->ستون -= (*mb_سر_off)(ptr, ptr + pos->ستون);
			    }
			}
			إذا (انتها_pos != عدم)
			{
			    انتها_pos->lnum = lnum + matchpos.lnum;
			    انتها_pos->ستون = matchpos.ستون;
			}
		    }
		    وإلا
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->ستون = matchpos.ستون;
			إذا (انتها_pos != عدم)
			{
			    انتها_pos->lnum = lnum + endpos.lnum;
			    انتها_pos->ستون = endpos.ستون;
			}
		    }
		    pos->coladd = 0;
		    إذا (انتها_pos != عدم)
			انتها_pos->coladd = 0;
		    یافت = 1;
		    اولی_تطابق = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    جستجو_تطابق_سطرها = endpos.lnum - matchpos.lnum;
		    جستجو_تطابق_endcol = endpos.ستون;
		    اكسر;
		}
		سطر_breakcheck();	// stop if ctrl-C typed
		إذا (got_int)
		    اكسر;

#اگر_تعریف_شده FEAT_جستجو_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		إذا ((options & جستجو_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    اكسر;
		}
#پایان_اگر

		إذا (loop && lnum == شروع_pos.lnum)
		    اكسر;	    // if second loop, stop where started
	    }
	    at_اولی_سطر = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    إذا (regmatch.regprog == عدم)
		اكسر;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    إذا (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_بیرون
#اگر_تعریف_شده FEAT_جستجو_EXTRA
			      || break_loop
#پایان_اگر
			      || یافت || loop)
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
		lnum = buf->b_ml.ml_سطر_شمار;
	    وإلا
		lnum = 1;
	    إذا (!shortmess(SHM_جستجو)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & جستجو_پیام))
		give_warning((char_u *)_(dir == BACKWARD
					  ? بالاترین_bot_پیام : bot_بالاترین_پیام), TRUE);
	    إذا (extra_arg != عدم)
		extra_arg->sa_wrapped = TRUE;
	}
	إذا (got_int || called_emsg > called_emsg_before || *timed_بیرون
#اگر_تعریف_شده FEAT_جستجو_EXTRA
		|| break_loop
#پایان_اگر
		)
	    اكسر;
    }
    طالما (--شمار > 0 && یافت);   // stop after count matches or no match

#اگر_تعریف_شده FEAT_RELTIME
    إذا (extra_arg != عدم && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#پایان_اگر
    vim_regfree(regmatch.regprog);

    إذا (!یافت)		    // did not find it
    {
	إذا (got_int)
	    emsg(_(e_interrupted));
	وإلا إذا ((options & جستجو_پیام) == جستجو_پیام)
	{
	    إذا (p_ws)
		semsg(_(e_الگو_not_یافت_str), mr_الگو);
	    وإلا إذا (lnum == 0)
		semsg(_(e_جستجو_اصابت_بالاترین_without_تطابق_for_str), mr_الگو);
	    وإلا
		semsg(_(e_جستجو_اصابت_bottom_without_تطابق_for_str), mr_الگو);
	}
	أرجع FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    إذا (pos->lnum > buf->b_ml.ml_سطر_شمار)
    {
	pos->lnum = buf->b_ml.ml_سطر_شمار;
	pos->ستون = ml_get_buf_len(buf, pos->lnum);
	إذا (pos->ستون > 0)
	    --pos->ستون;
    }

    أرجع submatch + 1;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
    فراغ
مجموعه_جستجو_direction(صحيح cdir)
{
    spats[0].off.dir = cdir;
}

    ساكن فراغ
مجموعه_vv_searchforward(فراغ)
{
    مجموعه_vim_var_nr(VV_SEARCHFORWARD, (دراز)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ساكن صحيح
اولی_submatch(regmmatch_T *rp)
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
#پایان_اگر

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
parse_جستجو_الگو_جابه_جایی(
    char_u	**pat,
    size_t	*patlen,
    صحيح		جستجو_delim,
    صحيح		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*جابه_جایی)
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
    p = skip_regexp_ex(*pat, جستجو_delim, magic_isset(),
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
    إذا (*p == جستجو_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    جابه_جایی->سطر = FALSE;
    جابه_جایی->انتها = FALSE;
    جابه_جایی->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    إذا (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	جابه_جایی->سطر = TRUE;
    وإلا إذا ((options & جستجو_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	إذا (*p == 'e')		// end
	    جابه_جایی->انتها = جستجو_انتها;
	++p;
    }
    إذا (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	إذا (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    جابه_جایی->off = atol((حرف *)p);
	وإلا إذا (*p == '-')	    // single '-'
	    جابه_جایی->off = -1;
	وإلا			    // single '+'
	    جابه_جایی->off = 1;
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
do_جستجو(
    oparg_T	    *oap,	// can be NULL
    صحيح		    dirc,	// '/' or '?'
    صحيح		    جستجو_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    دراز	    شمار,
    صحيح		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    صحيح		    retval;	// Return value
    char_u	    *p;
    دراز	    c;
    char_u	    *dircp;
    char_u	    *strcopy = عدم;
    صحيح		    show_جستجو_stats;
    char_u	    *msgbuf = عدم;
    size_t	    msgbuflen = 0;
    صحيح		    has_جابه_جایی = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    إذا (spats[0].off.سطر && vim_strchr(p_cpo, CPO_LINEOFF) != عدم)
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
    إذا (dirc == 0)
	dirc = spats[0].off.dir;
    وإلا
    {
	spats[0].off.dir = dirc;
#إذا defined(FEAT_EVAL)
	مجموعه_vv_searchforward();
#پایان_اگر
    }
    إذا (options & جستجو_REV)
    {
#اگر_تعریف_شده MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#وإلا
	إذا (dirc == '/')
	    dirc = '?';
	وإلا
	    dirc = '/';
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_FOLDING
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
#پایان_اگر

#اگر_تعریف_شده FEAT_جستجو_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    إذا (no_hlsearch && !(options & جستجو_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	مجموعه_no_hlsearch(FALSE);
    }
#پایان_اگر

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    لكل (;;)
    {
	صحيح		show_بالاترین_bot_پیام = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = عدم;
					    // use previous pattern
	إذا (pat == عدم || *pat == NUL || *pat == جستجو_delim)
	{
	    إذا (spats[RE_جستجو].pat == عدم)	    // no previous pattern
	    {
		إذا (spats[RE_SUBST].pat == عدم)
		{
		    emsg(_(e_no_قبلی_regular_عبارت));
		    retval = 0;
		    اذهب انتها_do_جستجو;
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

	    searchcmdlen += parse_جستجو_الگو_جابه_جایی(&pat, &patlen,
				جستجو_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    إذا (*searchstr == NUL)
	    {
		searchstr = prev_searchstr;
		searchstrlen = prev_searchstrlen;
	    }
	}

	show_جستجو_stats = FALSE;
	إذا ((options & جستجو_ECHO) && messaging()
		&& !پیام_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    پیام_شروع();

	    // Get the offset, so we know how long it is.
	    إذا (!cmd_silent &&
		    (spats[0].off.سطر || spats[0].off.انتها || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		إذا (spats[0].off.انتها)
		    off_buf[off_len++] = 'e';
		وإلا إذا (!spats[0].off.سطر)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		إذا (spats[0].off.off != 0 || spats[0].off.سطر)
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
		إذا (پیام_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (صحيح)(ردیفها - پیام_ردیف) * cmdline_عرض - 1;
		وإلا
		    // Use up to 'showcmd' column.
		    msgbufsize = (صحيح)(ردیفها - پیام_ردیف - 1) * cmdline_عرض
			+ sc_ستون - 1;
		إذا (msgbufsize < plen + off_len + جستجو_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + جستجو_STAT_BUF_LEN + 3;
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

		    trunc = پیام_strtrunc(msgbuf, TRUE);
		    إذا (trunc != عدم)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#اگر_تعریف_شده FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = معکوس_متن(msgbuf);
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
#پایان_اگر
		    پیام_outtrans(msgbuf);
		    پیام_clr_eos();
		    پیام_check();

		    gotocmdline(FALSE);
		    بیرون_flush();
		    پیام_nowait = TRUE;	    // don't wait for this message
		}

		إذا (!shortmess(SHM_SEARCHCOUNT))
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
	إذا (!spats[0].off.سطر && spats[0].off.off && pos.ستون < MAXCOL - 2)
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
		    pos.lnum = curbuf->b_ml.ml_سطر_شمار + 1;
		    pos.ستون = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, عدم,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, شمار, spats[0].off.انتها + (options &
		       (جستجو_KEEP + جستجو_PEEK + جستجو_HIS
			+ جستجو_پیام + جستجو_شروع
			+ ((pat != عدم && *pat == ';') ? 0 : جستجو_NOOF))),
		RE_آخرین, sia);

	إذا (dircp != عدم)
	    *dircp = جستجو_delim; // restore second '/' or '?' for normal_cmd()

	إذا (!shortmess(SHM_جستجو) && sia && sia->sa_wrapped)
	    show_بالاترین_bot_پیام = TRUE;

	إذا (c == FAIL)
	{
	    retval = 0;
	    اذهب انتها_do_جستجو;
	}
	إذا (spats[0].off.انتها && oap != عدم)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	إذا (!(options & جستجو_NOOF) || (pat != عدم && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    إذا (spats[0].off.سطر)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		إذا (c < 1)
		    pos.lnum = 1;
		وإلا إذا (c > curbuf->b_ml.ml_سطر_شمار)
		    pos.lnum = curbuf->b_ml.ml_سطر_شمار;
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
		has_جابه_جایی = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	إذا (show_جستجو_stats)
	     cmdline_جستجو_stat(dirc, &pos, &curwin->w_cursor,
				show_بالاترین_bot_پیام, msgbuf, msgbuflen,
				(شمار != 1 || has_جابه_جایی
#اگر_تعریف_شده FEAT_FOLDING
				 || (!(fdo_flags & FDO_جستجو)
				     && hasFolding(curwin->w_cursor.lnum,
								   عدم, عدم))
#پایان_اگر
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
	إذا (!(options & جستجو_OPT) || pat == عدم || *pat != ';')
	    اكسر;

	dirc = *++pat;
	--patlen;
	جستجو_delim = dirc;
	إذا (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    اذهب انتها_do_جستجو;
	}
	++pat;
	--patlen;
    }

    إذا (options & جستجو_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_مجموعه_curswant = true;

انتها_do_جستجو:
    إذا ((options & جستجو_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
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
جستجو_for_exact_سطر(
    buf_T	*buf,
    pos_T	*pos,
    صحيح		dir,
    char_u	*pat)
{
    linenr_T	شروع = 0;
    char_u	*ptr;
    char_u	*p;
    صحيح		compl_len = ins_compl_len();

    إذا (buf->b_ml.ml_سطر_شمار == 0)
	أرجع FAIL;
    لكل (;;)
    {
	pos->lnum += dir;
	إذا (pos->lnum < 1)
	{
	    إذا (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_سطر_شمار;
		إذا (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(بالاترین_bot_پیام), TRUE);
	    }
	    وإلا
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	وإلا إذا (pos->lnum > buf->b_ml.ml_سطر_شمار)
	{
	    إذا (p_ws)
	    {
		pos->lnum = 1;
		إذا (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(bot_بالاترین_پیام), TRUE);
	    }
	    وإلا
	    {
		pos->lnum = 1;
		اكسر;
	    }
	}
	إذا (pos->lnum == شروع)
	    اكسر;
	إذا (شروع == 0)
	    شروع = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->ستون = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	إذا (compl_وضعیت_adding() && !compl_وضعیت_sol())
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
    دراز		شمار = cap->count1;	// repeat count
    صحيح			ستون;
    char_u		*p;
    صحيح			len;
    صحيح			stop = TRUE;

    إذا (c != NUL)	// normal search: remember args for repeat
    {
	إذا (!کلید_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    مجموعه_csearch_direction(dir);
	    مجموعه_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_بایتها);
	    إذا (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_بایتها + lastc_bytelen);
		إذا (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_بایتها + lastc_bytelen);
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
	t_cmd = آخرین_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	إذا (vim_strchr(p_cpo, CPO_SCOLON) == عدم && شمار == 1 && t_cmd)
	    stop = FALSE;
    }

    إذا (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    وإلا
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    ستون = curwin->w_cursor.ستون;
    len = ml_get_curline_len();

    طالما (شمار--)
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
		وإلا إذا (STRNCMP(p + ستون, lastc_بایتها, lastc_bytelen) == 0
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
پیدا_کن_rawstring_انتها(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    صحيح		یافت = FALSE;

    لكل (p = linep + startpos->ستون + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->ستون - 1;
    delim_copy = vim_strnsave(linep + startpos->ستون + 1, delim_len);
    إذا (delim_copy == عدم)
	أرجع FALSE;
    لكل (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *سطر = ml_get(lnum);

	لكل (p = سطر + (lnum == startpos->lnum
					    ? startpos->ستون + 1 : 0); *p; ++p)
	{
	    إذا (lnum == endpos->lnum && (colnr_T)(p - سطر) >= endpos->ستون)
		اكسر;
	    إذا (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		یافت = TRUE;
		اكسر;
	    }
	}
	إذا (یافت)
	    اكسر;
    }
    vim_free(delim_copy);
    أرجع یافت;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ساكن فراغ
پیدا_کن_mps_مقادیر(
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
    صحيح		شمار = 0;		// cumulative number of braces
    صحيح		backwards = FALSE;	// init for gcc
    صحيح		raw_string = FALSE;	// search for raw string
    صحيح		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    صحيح		do_quotes;		// check for quotes in current line
    صحيح		at_شروع;		// do_quotes value at start position
    صحيح		hash_dir = 0;		// Direction searched for # things
    صحيح		comment_dir = 0;	// Direction searched for comments
    pos_T	تطابق_pos;		// Where last slash-star was found
    صحيح		شروع_درون_quotes;	// start position is in quotes
    صحيح		traveled = 0;		// how far we've searched so far
    صحيح		ignore_cend = FALSE;    // ignore comment end
    صحيح		cpo_تطابق;		// vi compatible matching
    صحيح		cpo_bsl;		// don't recognize backslashes
    صحيح		تطابق_escaped = 0;	// search for escaped match
    صحيح		dir;			// Direction to search
    صحيح		comment_ستون = MAXCOL;   // start of / / comment
    صحيح		lispcomm = FALSE;	// inside of Lisp-style comment
    صحيح		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    صحيح		skip_comments = (flags & FM_SKIPCOMM) != 0;
    صحيح		درون_بلوک_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_تطابق = (vim_strchr(p_cpo, CPO_تطابق) != عدم);
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
	پیدا_کن_mps_مقادیر(&initc, &findc, &backwards, TRUE);
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
	    إذا (!cpo_تطابق)
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

		    پیدا_کن_mps_مقادیر(&initc, &findc, &backwards, FALSE);
		    إذا (findc)
			اكسر;
		    pos.ستون += mb_ptr2len(linep + pos.ستون);
		}
		إذا (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    إذا (!cpo_تطابق && *skipwhite(linep) == '#')
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
		    تطابق_escaped = (bslcnt & 1);
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
		    إذا (pos.lnum == curbuf->b_ml.ml_سطر_شمار)
			اكسر;
		}
		وإلا إذا (pos.lnum == 1)
		    اكسر;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		سطر_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		إذا (*ptr != '#')
		    استمر;
		pos.ستون = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		إذا (hash_dir > 0)
		{
		    إذا (STRNCMP(ptr, "if", 2) == 0)
			شمار++;
		    وإلا إذا (STRNCMP(ptr, "el", 2) == 0)
		    {
			إذا (شمار == 0)
			    أرجع &pos;
		    }
		    وإلا إذا (STRNCMP(ptr, "endif", 5) == 0)
		    {
			إذا (شمار == 0)
			    أرجع &pos;
			شمار--;
		    }
		}
		وإلا
		{
		    إذا (STRNCMP(ptr, "if", 2) == 0)
		    {
			إذا (شمار == 0)
			    أرجع &pos;
			شمار--;
		    }
		    وإلا إذا (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			إذا (شمار == 0)
			    أرجع &pos;
		    }
		    وإلا إذا (STRNCMP(ptr, "endif", 5) == 0)
			شمار++;
		}
	    }
	    أرجع عدم;
	}
    }

#اگر_تعریف_شده FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    إذا (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != عدم)
	backwards = !backwards;
#پایان_اگر

    do_quotes = -1;
    شروع_درون_quotes = MAYBE;
    CLEAR_POS(&تطابق_pos);

    // backward search: Check if this line contains a single-line comment
    إذا ((backwards && comment_dir) || lisp || skip_comments)
	comment_ستون = check_linecomment(linep);
    إذا (lisp && comment_ستون != MAXCOL && pos.ستون > (colnr_T)comment_ستون)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    إذا (skip_comments && !درون_بلوک_comment && comment_ستون != MAXCOL
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
		سطر_breakcheck();

		// Check if this line contains a single-line comment
		إذا (comment_dir || lisp || skip_comments)
		    comment_ستون = check_linecomment(linep);
		// skip comment
		إذا (lisp && comment_ستون != MAXCOL)
		    pos.ستون = comment_ستون;
		وإلا إذا (skip_comments && !درون_بلوک_comment
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
		إذا (pos.lnum == curbuf->b_ml.ml_سطر_شمار  // end of file
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
		سطر_breakcheck();
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
		إذا (!درون_بلوک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/'
			&& (comment_ستون == MAXCOL || (صحيح)pos.ستون < comment_ستون))
		    درون_بلوک_comment = TRUE;
		وإلا إذا (درون_بلوک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '/' && linep[pos.ستون] == '*')
		    درون_بلوک_comment = FALSE;
	    }
	    وإلا
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		إذا (!درون_بلوک_comment && linep[pos.ستون] == '/'
			&& linep[pos.ستون + 1] == '*'
			&& (comment_ستون == MAXCOL || (صحيح)pos.ستون < comment_ستون))
		    درون_بلوک_comment = TRUE;
		وإلا إذا (درون_بلوک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		    درون_بلوک_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	إذا (pos.ستون == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    إذا (linep[0] == findc && شمار == 0)	// match!
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
			إذا (!پیدا_کن_rawstring_انتها(linep, &pos,
				  شمار > 0 ? &تطابق_pos : &curwin->w_cursor))
			{
			    شمار++;
			    تطابق_pos = pos;
			    تطابق_pos.ستون--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		وإلا إذا (  linep[pos.ستون - 1] == '/'
			&& linep[pos.ستون] == '*'
			&& (pos.ستون == 1 || linep[pos.ستون - 2] != '*')
			&& (صحيح)pos.ستون < comment_ستون)
		{
		    شمار++;
		    تطابق_pos = pos;
		    تطابق_pos.ستون--;
		}
		وإلا إذا (linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		{
		    إذا (شمار > 0)
			pos = تطابق_pos;
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
	إذا (cpo_تطابق)
	    do_quotes = 0;
	وإلا إذا (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_شروع = do_quotes;
	    لكل (ptr = linep; *ptr; ++ptr)
	    {
		إذا (ptr == linep + pos.ستون + backwards)
		    at_شروع = (do_quotes & 1);
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
		    إذا (شروع_درون_quotes == MAYBE)
		    {
			inquote = at_شروع;
			إذا (inquote)
			    شروع_درون_quotes = TRUE;
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
			إذا (شروع_درون_quotes == MAYBE)
			{
			    inquote = at_شروع;
			    إذا (inquote)
				شروع_درون_quotes = TRUE;
			}
			وإلا إذا (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	إذا (شروع_درون_quotes == MAYBE)
	    شروع_درون_quotes = FALSE;

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
		شروع_درون_quotes = FALSE;
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
		    شروع_درون_quotes = FALSE;
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
	    إذا (!cpo_تطابق && initc != '\'' && findc != '\'')
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
	    إذا (skip_comments && (درون_بلوک_comment
		    || (comment_ستون != MAXCOL && (صحيح)pos.ستون >= comment_ستون)))
		اكسر;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    إذا ((!inquote || شروع_درون_quotes == TRUE)
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
		إذا (cpo_bsl || (bslcnt & 1) == تطابق_escaped)
		{
		    إذا (c == initc)
			شمار++;
		    وإلا
		    {
			إذا (شمار == 0)
			    أرجع &pos;
			شمار--;
		    }
		}
	    }
	}
    }

    إذا (comment_dir == BACKWARD && شمار > 0)
    {
	pos = تطابق_pos;
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
    دراز	save_so;
    دراز	save_siso;
#اگر_تعریف_شده CURSOR_SHAPE
    صحيح		save_state;
#پایان_اگر
    colnr_T	save_dollar_vcol;
    char_u	*p;
    دراز	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    دراز	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    لكل (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    اكسر;
#پایان_اگر
	p += mb_ptr2len(p) + 1;
	إذا (PTR2CHAR(p) == c
#اگر_تعریف_شده FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#پایان_اگر
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
	vim_beep(BO_تطابق);
	أرجع;
    }

    إذا (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	أرجع;

    إذا (!curwin->w_p_wrap)
	getvcol(curwin, lpos, عدم, &vcol, عدم, 0);

    صحيح ستون_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_عرض));
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
#اگر_تعریف_شده CURSOR_SHAPE
    save_state = State;
    State = نما_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#پایان_اگر
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    بیرون_flush_cursor(TRUE, FALSE);

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
#اگر_تعریف_شده CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#پایان_اگر
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ساكن صحيح
is_zero_عرض(
    char_u	*الگو,
    size_t	patternlen,
    صحيح		move,
    pos_T	*cur,
    صحيح		direction)
{
    regmmatch_T	regmatch;
    صحيح		nmatched = 0;
    صحيح		نتیجه = -1;
    pos_T	pos;
    صحيح		called_emsg_before = called_emsg;
    صحيح		flag = 0;

    إذا (الگو == عدم)
    {
	الگو = spats[آخرین_idx].pat;
	patternlen = spats[آخرین_idx].patlen;
    }

    إذا (جستجو_regcomp(الگو, patternlen, عدم, RE_جستجو, RE_جستجو,
					      جستجو_KEEP, &regmatch) == FAIL)
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
	flag = جستجو_شروع;
    }

    إذا (searchit(curwin, curbuf, &pos, عدم, direction, الگو, patternlen, 1,
				  جستجو_KEEP + flag, RE_جستجو, عدم) != FAIL)
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
	    نتیجه = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].ستون == regmatch.endpos[0].ستون);
	}
    }

    vim_regfree(regmatch.regprog);
    أرجع نتیجه;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحيح
current_جستجو(
    دراز	شمار,
    صحيح		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	شروع_pos;	// start position of the pattern match
    pos_T	انتها_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    صحيح		i;
    صحيح		dir;
    صحيح		نتیجه;		// result of various function calls
    char_u	old_p_ws = p_ws;
    صحيح		flags = 0;
    pos_T	save_VIsual = VIsual;
    صحيح		zero_عرض;
    صحيح		skip_اولی_backward;

    // Correct cursor when 'selection' is exclusive
    إذا (VIsual_فعال && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_اولی_backward = forward && VIsual_فعال
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
    zero_عرض = is_zero_عرض(spats[آخرین_idx].pat, spats[آخرین_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    إذا (zero_عرض == -1)
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
	    إذا (i == 0 && skip_اولی_backward)
		استمر;
	    dir = i;
	}
	وإلا
	    dir = !i;

	flags = 0;
	إذا (!dir && !zero_عرض)
	    flags = جستجو_انتها;
	انتها_pos = pos;

	// wrapping should not occur in the first round
	إذا (i == 0)
	    p_ws = FALSE;

	نتیجه = searchit(curwin, curbuf, &pos, &انتها_pos,
		(dir ? FORWARD : BACKWARD),
		spats[آخرین_idx].pat, spats[آخرین_idx].patlen, (دراز) (i ? شمار : 1),
		جستجو_KEEP | flags, RE_جستجو, عدم);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	إذا (i == 1 && !نتیجه) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    إذا (VIsual_فعال)
		VIsual = save_VIsual;
	    أرجع FAIL;
	}
	وإلا إذا (i == 0 && !نتیجه)
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
		pos.lnum = curwin->w_میانگیر->b_ml.ml_سطر_شمار;
		pos.ستون  = ml_get_len(curwin->w_میانگیر->b_ml.ml_سطر_شمار);
	    }
	}
    }

    شروع_pos = pos;

    إذا (!VIsual_فعال)
	VIsual = شروع_pos;

    // put the cursor after the match
    curwin->w_cursor = انتها_pos;
    إذا (LT_POS(VIsual, انتها_pos) && forward)
    {
	إذا (skip_اولی_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	وإلا
	    // put the cursor on last character of match
	    dec_cursor();
    }
    وإلا إذا (VIsual_فعال && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_فعال = TRUE;
    VIsual_نما = 'v';

    إذا (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	إذا (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	وإلا إذا (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#اگر_تعریف_شده FEAT_FOLDING
    إذا (fdo_flags & FDO_جستجو && کلید_Typed)
	foldOpenCursor();
#پایان_اگر

    may_شروع_select('c');
    setmouse();
#اگر_تعریف_شده FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_ستاره.vmode = NUL;
#پایان_اگر
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
cmdline_جستجو_stat(
    صحيح		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    صحيح		show_بالاترین_bot_پیام,
    char_u	*msgbuf,
    size_t	msgbuflen,
    صحيح		recompute,
    صحيح		maxcount,
    دراز	timeout)
{
    searchstat_T stat;

    update_جستجو_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    إذا (stat.cur <= 0)
	أرجع;

    حرف	t[جستجو_STAT_BUF_LEN];
    size_t	len;

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	إذا (stat.incomplete == 1)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[?/??]");
	وإلا إذا (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وإلا إذا (stat.cnt > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	وإلا
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    وإلا
#پایان_اگر
    {
	إذا (stat.incomplete == 1)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[?/??]");
	وإلا إذا (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	وإلا إذا (stat.cnt > maxcount)
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	وإلا
	    len = vim_snprintf(t, جستجو_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    إذا (show_بالاترین_bot_پیام && len + 2 < جستجو_STAT_BUF_LEN)
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
    ساكن فراغ
update_جستجو_stat(
    صحيح			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    صحيح			recompute,
    صحيح			maxcount,
    دراز		timeout استفادهنشده)
{
    صحيح		    save_ws = p_ws;
    صحيح		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ساكن pos_T    lastpos = {0, 0, 0};
    ساكن صحيح	    cur = 0;
    ساكن صحيح	    cnt = 0;
    ساكن صحيح	    exact_تطابق = FALSE;
    ساكن صحيح	    incomplete = 0;
    ساكن صحيح	    آخرین_maxcount = 0;
    ساكن صحيح	    chgtick = 0;
    ساكن char_u   *lastpat = عدم;
    ساكن size_t   lastpatlen = 0;
    ساكن buf_T    *lbuf = عدم;
#اگر_تعریف_شده FEAT_RELTIME
    proftime_T  شروع;
#پایان_اگر

    CLEAR_اشاره_گر(stat);

    إذا (dirc == 0 && !recompute && !خالی_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_تطابق = exact_تطابق;
	stat->incomplete = incomplete;
	stat->آخرین_maxcount = آخرین_maxcount;
	أرجع;
    }
    آخرین_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    إذا (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != عدم
	    && STRNCMP(lastpat, spats[آخرین_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[آخرین_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_تطابق = FALSE;
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
	صحيح	انجامشده_جستجو = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#اگر_تعریف_شده FEAT_RELTIME
	إذا (timeout > 0)
	    profile_setlimit(timeout, &شروع);
#پایان_اگر
	طالما (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, عدم, 0, 1, جستجو_KEEP, RE_آخرین, عدم) != FAIL)
	{
	    انجامشده_جستجو = TRUE;
#اگر_تعریف_شده FEAT_RELTIME
	    // Stop after passing the time limit.
	    إذا (timeout > 0 && profile_passed_حد(&شروع))
	    {
		incomplete = 1;
		اكسر;
	    }
#پایان_اگر
	    cnt++;
	    إذا (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		إذا (LT_POS(p, endpos))
		    exact_تطابق = TRUE;
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
	إذا (انجامشده_جستجو)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[آخرین_idx].pat, spats[آخرین_idx].patlen);
	    إذا (lastpat == عدم)
		lastpatlen = 0;
	    وإلا
		lastpatlen = spats[آخرین_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_تطابق = exact_تطابق;
    stat->incomplete = incomplete;
    stat->آخرین_maxcount = آخرین_maxcount;
    p_ws = save_ws;
}

#إذا defined(FEAT_پیدا_کن_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ساكن char_u *
get_سطر_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *سطر = ml_get(lnum);

    vim_strncpy(buf, سطر, LSIZE - 1);
    أرجع buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    فراغ
پیدا_کن_الگو_درون_مسیر(
    char_u	*ptr,		// pointer to search pattern
    صحيح		dir استفادهنشده,	// direction of expansion
    صحيح		len,		// length of search pattern
    صحيح		کامل,		// match whole words only
    صحيح		skip_comments,	// don't match inside comments
    صحيح		type,		// Type of search; are we looking for a type?
				// a macro?
    دراز	شمار,
    صحيح		action,		// What to do when we find it
    linenr_T	شروع_lnum,	// first line to start searching
    linenr_T	انتها_lnum,	// last line for searching
    صحيح		forceit,	// If true, always switch to the found path
    صحيح		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    صحيح		max_مسیر_عمق = 50;
    دراز	تطابق_شمار = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*prev_fname = عدم;
    linenr_T	lnum;
    صحيح		عمق;
    صحيح		عمق_displayed;	// For type==CHECK_PATH
    صحيح		old_files;
    صحيح		already_searched;
    char_u	*file_سطر;
    char_u	*سطر;
    char_u	*p;
    char_u	save_char;
    صحيح		define_matched;
    regmatch_T	regmatch;
    regmatch_T	incl_regmatch;
    regmatch_T	def_regmatch;
    صحيح		matched = FALSE;
    صحيح		did_show = FALSE;
    صحيح		یافت = FALSE;
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

    file_سطر = alloc(LSIZE);
    إذا (file_سطر == عدم)
	أرجع;

    إذا (type != CHECK_مسیر && type != پیدا_کن_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_وضعیت_sol())
    {
	pat = alloc(len + 5);
	إذا (pat == عدم)
	    اذهب fpip_انتها;
	vim_snprintf((حرف *)pat, len + 5, کامل ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	إذا (regmatch.regprog == عدم)
	    اذهب fpip_انتها;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    إذا (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	إذا (incl_regmatch.regprog == عدم)
	    اذهب fpip_انتها;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    إذا (type == پیدا_کن_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	إذا (def_regmatch.regprog == عدم)
	    اذهب fpip_انتها;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_مسیر_عمق * حجم(SearchedFile), TRUE);
    إذا (files == عدم)
	اذهب fpip_انتها;
    old_files = max_مسیر_عمق;
    عمق = عمق_displayed = -1;

    lnum = شروع_lnum;
    إذا (انتها_lnum > curbuf->b_ml.ml_سطر_شمار)
	انتها_lnum = curbuf->b_ml.ml_سطر_شمار;
    إذا (lnum > انتها_lnum)		// do at least one line
	lnum = انتها_lnum;
    سطر = get_سطر_and_copy(lnum, file_سطر);

    لكل (;;)
    {
	إذا (incl_regmatch.regprog != عدم
		&& vim_regexec(&incl_regmatch, سطر, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    إذا (جستجوی_رشته((حرف *)inc_opt, "\\zs") != عدم)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = پیدا_کن_file_name_درون_مسیر(incl_regmatch.startp[0],
		       (صحيح)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    وإلا
		// Use text after match with 'include'.
		new_fname = file_name_درون_سطر(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, عدم);
	    already_searched = FALSE;
	    إذا (new_fname != عدم)
	    {
		// Check whether we have already searched in this file
		لكل (i = 0;; i++)
		{
		    إذا (i == عمق + 1)
			i = old_files;
		    إذا (i == max_مسیر_عمق)
			اكسر;
		    إذا (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			إذا (type != CHECK_مسیر
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    پیام_putchar('\n');	    // cursor below last one
			    إذا (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //پیام
			    {
				پیام_home_replace_hl(new_fname);
				پیام_puts(_(" (includes previously listed match)"));
				prev_fname = عدم;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			اكسر;
		    }
		}
	    }

	    إذا (type == CHECK_مسیر && (action == ACTION_SHOW_ALL
				 || (new_fname == عدم && !already_searched)))
	    {
		إذا (did_show)
		    پیام_putchar('\n');	    // cursor below last one
		وإلا
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    پیام_puts_title(_("--- Included files "));
		    إذا (action != ACTION_SHOW_ALL)
			پیام_puts_title(_("not found "));
		    پیام_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		طالما (عمق_displayed < عمق && !got_int)
		{
		    ++عمق_displayed;
		    لكل (i = 0; i < عمق_displayed; i++)
			پیام_puts("  ");
		    پیام_home_replace(files[عمق_displayed].name);
		    پیام_puts(" -->\n");
		}
		إذا (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    لكل (i = 0; i <= عمق_displayed; i++)
			پیام_puts("  ");
		    إذا (new_fname != عدم)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			پیام_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    وإلا
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			إذا (جستجوی_رشته((حرف *)inc_opt, "\\zs") != عدم)
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
			وإلا إذا (p > سطر)
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
			پیام_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    إذا (new_fname == عدم && action == ACTION_SHOW_ALL)
		    {
			إذا (already_searched)
			    پیام_puts(_("  (Already listed)"));
			وإلا
			    پیام_puts(_("  NOT FOUND"));
		    }
		}
		بیرون_flush();	    // output each line directly
	    }

	    إذا (new_fname != عدم)
	    {
		// Push the new file onto the file stack
		إذا (عمق + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_مسیر_عمق * 2);
		    إذا (bigger != عدم)
		    {
			لكل (i = 0; i <= عمق; i++)
			    bigger[i] = files[i];
			لكل (i = عمق + 1; i < old_files + max_مسیر_عمق; i++)
			{
			    bigger[i].fp = عدم;
			    bigger[i].name = عدم;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			لكل (i = old_files; i < max_مسیر_عمق; i++)
			    bigger[i + max_مسیر_عمق] = files[i];
			old_files += max_مسیر_عمق;
			max_مسیر_عمق *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		إذا ((files[عمق + 1].fp = mch_fopen((حرف *)new_fname, "r"))
								    == عدم)
		    vim_free(new_fname);
		وإلا
		{
		    إذا (++عمق == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[عمق].name = curr_fname = new_fname;
		    files[عمق].lnum = 0;
		    files[عمق].matched = FALSE;
		    إذا (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			پیام_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((حرف*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(حرف *)new_fname);
			پیام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
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
	    p = سطر;
جستجو_سطر:
	    define_matched = FALSE;
	    إذا (def_regmatch.regprog != عدم
			      && vim_regexec(&def_regmatch, سطر, (colnr_T)0))
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
		إذا (define_matched || compl_وضعیت_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    إذا (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    وإلا
			matched = STRNCMP(startp, ptr, len) == 0;
		    إذا (matched && define_matched && کامل
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		وإلا إذا (regmatch.regprog != عدم
			 && vim_regexec(&regmatch, سطر, (colnr_T)(p - سطر)))
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
			إذا ((*سطر != '#' ||
				STRNCMP(skipwhite(سطر + 1), "define", 6) != 0)
				&& get_leader_len(سطر, عدم, FALSE, TRUE))
			    matched = FALSE;

			/*
			 * Also check for a "/ *" or "/ /" before the match.
			 * Skips lines like "int backwards;  / * normal index
			 * * /" when looking for "normal".
			 * Note: Doesn't skip "/ *" in comments.
			 */
			p = skipwhite(سطر);
			إذا (matched
				|| (p[0] == '/' && p[1] == '*') || p[0] == '*')
			    لكل (p = سطر; *p && p < startp; ++p)
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
		صحيح	جمع_کن_r;
		char_u	*aux;

		إذا (عمق == -1 && lnum == curwin->w_cursor.lnum)
		    اكسر;
		یافت = TRUE;
		aux = p = startp;
		إذا (compl_وضعیت_adding() && (صحيح)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    إذا (vim_iswordp(p))
			اذهب exit_matched;
		    p = پیدا_کن_کلمه_شروع(p);
		}
		p = پیدا_کن_کلمه_انتها(p);
		i = (صحيح)(p - aux);

		إذا (compl_وضعیت_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    إذا (عمق < 0)
		    {
			إذا (lnum >= انتها_lnum)
			    اذهب exit_matched;
			سطر = get_سطر_and_copy(++lnum, file_سطر);
		    }
		    وإلا إذا (vim_fgets(سطر = file_سطر,
						      LSIZE, files[عمق].fp))
			اذهب exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(سطر);
		    p = پیدا_کن_کلمه_شروع(p);
		    p = پیدا_کن_کلمه_انتها(p);
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

		جمع_کن_r = ins_compl_جمع_کن_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? عدم
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		إذا (جمع_کن_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		وإلا إذا (جمع_کن_r == FAIL)
		    اكسر;
	    }
	    وإلا إذا (action == ACTION_SHOW_ALL)
	    {
		یافت = TRUE;
		إذا (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		إذا (curr_fname != prev_fname)
		{
		    إذا (did_show)
			پیام_putchar('\n');	// cursor below last one
		    إذا (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			پیام_home_replace_hl(curr_fname);
		    prev_fname = curr_fname;
		}
		did_show = TRUE;
		إذا (!got_int)
		    show_pat_درون_مسیر(سطر, type, TRUE, action,
			    (عمق == -1) ? عدم : files[عمق].fp,
			    (عمق == -1) ? &lnum : &files[عمق].lnum,
			    تطابق_شمار++);

		// Set matched flag for this file and all the ones that
		// include it
		لكل (i = 0; i <= عمق; ++i)
		    files[i].matched = TRUE;
	    }
	    وإلا إذا (--شمار <= 0)
	    {
		یافت = TRUE;
		إذا (عمق == -1 && lnum == curwin->w_cursor.lnum
# إذا defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_تطابق_is_on_current_سطر));
		وإلا إذا (action == ACTION_SHOW)
		{
		    show_pat_درون_مسیر(سطر, type, did_show, action,
			    (عمق == -1) ? عدم : files[عمق].fp,
			    (عمق == -1) ? &lnum : &files[عمق].lnum, 1L);
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
		    إذا (action == ACTION_تقسیم)
		    {
			إذا (win_تقسیم(0, 0) == FAIL)
			    اكسر;
			RESET_BINDING(curwin);
		    }
		    إذا (عمق == -1)
		    {
			// match in current file
# إذا defined(FEAT_QUICKFIX)
			إذا (g_do_tagpreview != 0)
			{
			    إذا (!win_valid(curwin_save))
				اكسر;
			    إذا (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_میانگیر->b_fnum, عدم,
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
					0, files[عمق].name, عدم, TRUE,
						    files[عمق].lnum, forceit)))
			    اكسر;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[عمق].lnum;
		    }
		}
		إذا (action != ACTION_SHOW)
		{
		    curwin->w_cursor.ستون = (colnr_T)(startp - سطر);
		    curwin->w_مجموعه_curswant = true;
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
		    && !compl_وضعیت_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		اذهب جستجو_سطر;
	}
	سطر_breakcheck();
	إذا (action == ACTION_EXPAND)
	    ins_compl_check_کلیدها(30, FALSE);
	إذا (got_int || ins_compl_interrupted())
	    اكسر;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	طالما (عمق >= 0 && !already
		&& vim_fgets(سطر = file_سطر, LSIZE, files[عمق].fp))
	{
	    fclose(files[عمق].fp);
	    --old_files;
	    files[old_files].name = files[عمق].name;
	    files[old_files].matched = files[عمق].matched;
	    --عمق;
	    curr_fname = (عمق == -1) ? curbuf->b_fname
				       : files[عمق].name;
	    إذا (عمق < عمق_displayed)
		عمق_displayed = عمق;
	}
	إذا (عمق >= 0)		// we could read the line
	{
	    files[عمق].lnum++;
	    // Remove any CR and LF from the line.
	    i = (صحيح)STRLEN(سطر);
	    إذا (i > 0 && سطر[i - 1] == '\n')
		سطر[--i] = NUL;
	    إذا (i > 0 && سطر[i - 1] == '\r')
		سطر[--i] = NUL;
	}
	وإلا إذا (!already)
	{
	    إذا (++lnum > انتها_lnum)
		اكسر;
	    سطر = get_سطر_and_copy(lnum, file_سطر);
	}
	already = عدم;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    لكل (i = 0; i <= عمق; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    لكل (i = old_files; i < max_مسیر_عمق; i++)
	vim_free(files[i].name);
    vim_free(files);

    إذا (type == CHECK_مسیر)
    {
	إذا (!did_show)
	{
	    إذا (action != ACTION_SHOW_ALL)
		پیام(_("All included files were found"));
	    وإلا
		پیام(_("No included files"));
	}
    }
    وإلا إذا (!یافت && action != ACTION_EXPAND && !silent)
    {
	إذا (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	وإلا إذا (type == پیدا_کن_DEFINE)
	    emsg(_(e_couldnt_پیدا_کن_definition));
	وإلا
	    emsg(_(e_couldnt_پیدا_کن_الگو));
    }
    إذا (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	پیام_انتها();

fpip_انتها:
    vim_free(file_سطر);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ساكن فراغ
show_pat_درون_مسیر(
    char_u  *سطر,
    صحيح	    type,
    صحيح	    did_show,
    صحيح	    action,
    FILE    *fp,
    linenr_T *lnum,
    دراز    شمار)
{
    char_u  *p;
    size_t  linelen;

    إذا (did_show)
	پیام_putchar('\n');	// cursor below last one
    وإلا إذا (!پیام_silent)
	gotocmdline(TRUE);	// cursor at status line
    إذا (got_int)		// 'q' typed at "--more--" message
	أرجع;
    linelen = STRLEN(سطر);
    لكل (;;)
    {
	p = سطر + linelen - 1;
	إذا (fp != عدم)
	{
	    // We used fgets(), so get rid of newline at end
	    إذا (p >= سطر && *p == '\n')
		--p;
	    إذا (p >= سطر && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	إذا (action == ACTION_SHOW_ALL)
	{
	    sprintf((حرف *)IObuff, "%3ld: ", شمار);	// show match nr
	    پیام_puts((حرف *)IObuff);
	    sprintf((حرف *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    پیام_puts_attr((حرف *)IObuff, HL_ATTR(HLF_N));
	    پیام_puts(" ");
	}
	پیام_prt_سطر(سطر, FALSE);
	بیرون_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	إذا (got_int || type != پیدا_کن_DEFINE || p < سطر || *p != '\\')
	    اكسر;

	إذا (fp != عدم)
	{
	    إذا (vim_fgets(سطر, LSIZE, fp)) // end of file
		اكسر;
	    linelen = STRLEN(سطر);
	    ++*lnum;
	}
	وإلا
	{
	    إذا (++*lnum > curbuf->b_ml.ml_سطر_شمار)
		اكسر;
	    سطر = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	پیام_putchar('\n');
    }
}
#پایان_اگر

#اگر_تعریف_شده FEAT_VIMINFO
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
get_spat_آخرین_idx(فراغ)
{
    أرجع آخرین_idx;
}
#پایان_اگر

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    فراغ
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*الگو = عدم;
    صحيح			maxcount = p_msc;
    دراز		timeout = جستجو_STAT_DEF_TIMEOUT;
    صحيح			recompute = TRUE;
    searchstat_T	stat;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
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
	di = dict_پیدا_کن(dict, (char_u *)"timeout", -1);
	إذا (di != عدم)
	{
	    timeout = (دراز)tv_get_عدد_chk(&di->di_tv, &error);
	    إذا (error)
		أرجع;
	}
	di = dict_پیدا_کن(dict, (char_u *)"maxcount", -1);
	إذا (di != عدم)
	{
	    maxcount = (صحيح)tv_get_عدد_chk(&di->di_tv, &error);
	    إذا (error)
		أرجع;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_پیدا_کن(dict, (char_u *)"pattern", -1);
	إذا (di != عدم)
	{
	    الگو = tv_get_string_chk(&di->di_tv);
	    إذا (الگو == عدم)
		أرجع;
	}
	di = dict_پیدا_کن(dict, (char_u *)"pos", -1);
	إذا (di != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_آرگومان_str), "pos");
		أرجع;
	    }
	    إذا (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_آرگومان_str), "List format should be [lnum, col, off]");
		أرجع;
	    }
	    li = list_پیدا_کن(di->di_tv.vval.v_list, 0L);
	    إذا (li != عدم)
	    {
		pos.lnum = tv_get_عدد_chk(&li->li_tv, &error);
		إذا (error)
		    أرجع;
	    }
	    li = list_پیدا_کن(di->di_tv.vval.v_list, 1L);
	    إذا (li != عدم)
	    {
		pos.ستون = tv_get_عدد_chk(&li->li_tv, &error) - 1;
		إذا (error)
		    أرجع;
	    }
	    li = list_پیدا_کن(di->di_tv.vval.v_list, 2L);
	    إذا (li != عدم)
	    {
		pos.coladd = tv_get_عدد_chk(&li->li_tv, &error);
		إذا (error)
		    أرجع;
	    }
	}
    }

    save_آخرین_جستجو_الگو();
# ifdef FEAT_جستجو_EXTRA
    save_incsearch_state();
# endif
    إذا (الگو != عدم)
    {
	إذا (*الگو == NUL)
	    اذهب the_انتها;
	vim_free(spats[آخرین_idx].pat);
	spats[آخرین_idx].patlen = STRLEN(الگو);
	spats[آخرین_idx].pat = vim_strnsave(الگو, spats[آخرین_idx].patlen);
	إذا (spats[آخرین_idx].pat == عدم)
	    spats[آخرین_idx].patlen = 0;
    }
    إذا (spats[آخرین_idx].pat == عدم || *spats[آخرین_idx].pat == NUL)
	اذهب the_انتها;	// the previous pattern was never defined

    update_جستجو_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_جمع_کن_عدد(rettv->vval.v_dict, "current", stat.cur);
    dict_جمع_کن_عدد(rettv->vval.v_dict, "total", stat.cnt);
    dict_جمع_کن_عدد(rettv->vval.v_dict, "exact_match", stat.exact_تطابق);
    dict_جمع_کن_عدد(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_جمع_کن_عدد(rettv->vval.v_dict, "maxcount", stat.آخرین_maxcount);

the_انتها:
    restore_آخرین_جستجو_الگو();
# ifdef FEAT_جستجو_EXTRA
    restore_incsearch_state();
# endif
}
#پایان_اگر
