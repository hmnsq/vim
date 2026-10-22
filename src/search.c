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

#اگر_تعریف_شدہ FEAT_EVAL
ساکن خالی سیٹ_vv_searchforward(خالی);
ساکن صحیح پہلا_submatch(regmmatch_T *rp);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
ساکن char_u *get_سطر_and_copy(linenr_T lnum, char_u *buf);
ساکن خالی show_pat_اندر_راستہ(char_u *, صحیح, صحیح, صحیح, FILE *, linenr_T *, طویل);
#اگر_ختم

قسمتعریف ساخت searchstat
{
    صحیح	    cur;	    // current position of found words
    صحیح	    cnt;	    // total count of found words
    صحیح	    exact_مطابقت;    // TRUE if matched exactly on specified position
    صحیح	    incomplete;	    // 0: search was fully completed
			    // 1: recomputing was timed out
			    // 2: max count exceeded
    صحیح	    آخری_maxcount;  // the max count of the last search
} searchstat_T;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
ساکن خالی save_incsearch_state(خالی);
ساکن خالی restore_incsearch_state(خالی);
#اگر_ختم
ساکن صحیح check_prevcol(char_u *linep, صحیح ستون, صحیح ch, صحیح *prevcol);
ساکن صحیح ڈھونڈو_rawstring_انتہا(char_u *linep, pos_T *startpos, pos_T *endpos);
ساکن خالی ڈھونڈو_mps_اقدار(صحیح *initc, صحیح *findc, صحیح *backwards, صحیح switchit);
ساکن صحیح is_zero_چوڑائی(char_u *نمونہ, size_t patternlen, صحیح move, pos_T *cur, صحیح direction);
ساکن خالی cmdline_تلاش_stat(صحیح dirc, pos_T *pos, pos_T *cursor_pos, صحیح show_چوٹی_bot_پیغام, char_u *msgbuf, size_t msgbuflen, صحیح recompute, صحیح maxcount, طویل timeout);
ساکن خالی update_تلاش_stat(صحیح dirc, pos_T *pos, pos_T *cursor_pos, searchstat_T *stat, صحیح recompute, صحیح maxcount, طویل timeout);

#تعریف_کرو تلاش_STAT_DEF_TIMEOUT 40L
// 'W ':  2 +
// '[>9999/>9999]': 13 + 1 (NUL)
#تعریف_کرو تلاش_STAT_BUF_LEN 16

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
ساکن spat_T spats[2] =
{
    {تہی, 0, TRUE, FALSE, {'/', 0, 0, 0L}},	// last used search pat
    {تہی, 0, TRUE, FALSE, {'/', 0, 0, 0L}}	// last used substitute pat
};

ساکن صحیح آخری_idx = 0;	// index in spats[] for RE_LAST

ساکن char_u lastc[2] = {NUL, NUL};	// last character searched for
ساکن صحیح lastcdir = FORWARD;		// last direction of character search
ساکن صحیح آخری_t_cmd = TRUE;		// last search t_cmd
ساکن char_u	lastc_بائٹیں[MB_MAXBYTES + 1];
ساکن صحیح	lastc_bytelen = 1;	// >1 for multi-byte char

// copy of spats[], for keeping the search patterns while executing autocmds
ساکن spat_T	    saved_spats[ARRAY_لمبائی(spats)];
ساکن char_u	    *saved_mr_نمونہ = تہی;
ساکن size_t	    saved_mr_patternlen = 0;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
ساکن صحیح	    saved_spats_آخری_idx = 0;
ساکن صحیح	    saved_spats_no_hlsearch = 0;
#اگر_ختم

// allocated copy of pattern used by search_regcomp()
ساکن char_u	    *mr_نمونہ = تہی;
ساکن size_t	    mr_patternlen = 0;

#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
/*
 * Type used by find_pattern_in_path() to remember which included files have
 * been searched already.
 */
قسمتعریف ساخت SearchedFile
{
    FILE	*fp;		// File pointer
    char_u	*name;		// Full name of file
    linenr_T	lnum;		// Line we were up to in file
    صحیح		matched;	// Found a match in this file
} SearchedFile;
#اگر_ختم

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
تلاش_regcomp(
    char_u	*pat,
    size_t	patlen,
    char_u	**مستعمل_pat,
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
    اگر (pat == تہی || *pat == NUL)
    {
	صحیح i;

	اگر (pat_use == RE_آخری)
	    i = آخری_idx;
	ورنہ
	    i = pat_use;
	اگر (spats[i].pat == تہی)	// pattern was never defined
	{
	    اگر (pat_use == RE_SUBST)
		emsg(_(e_no_پچھلا_substitute_regular_عبارت));
	    ورنہ
		emsg(_(e_no_پچھلا_regular_عبارت));
	    rc_did_emsg = TRUE;
	    واپس FAIL;
	}
	pat = spats[i].pat;
	patlen = spats[i].patlen;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    ورنہ اگر (options & تلاش_HIS)	// put new pattern in history
	جوڑو_مقصد_history(HIST_تلاش, pat, patlen, TRUE, NUL);

    اگر (مستعمل_pat)
	*مستعمل_pat = pat;

    vim_free(mr_نمونہ);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_نمونہ = الٹ_متن(pat);
    ورنہ
#اگر_ختم
	mr_نمونہ = vim_strnsave(pat, patlen);
    اگر (mr_نمونہ == تہی)
	mr_patternlen = 0;
    ورنہ
	mr_patternlen = patlen;

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    اگر (!(options & تلاش_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	اگر (pat_save == RE_تلاش || pat_save == RE_BOTH)
	    save_re_pat(RE_تلاش, pat, patlen, magic);
	// substitute or global command
	اگر (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, patlen, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    اگر (regmatch->regprog == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * Get search pattern used by search_regcomp().
 */
    char_u *
get_تلاش_pat(خالی)
{
    واپس mr_نمونہ;
}

    خالی
save_re_pat(صحیح idx, char_u *pat, size_t patlen, صحیح magic)
{
    اگر (spats[idx].pat == pat)
	واپس;

    vim_free(spats[idx].pat);
    spats[idx].pat = vim_strnsave(pat, patlen);
    اگر (spats[idx].pat == تہی)
	spats[idx].patlen = 0;
    ورنہ
	spats[idx].patlen = patlen;
    spats[idx].magic = magic;
    spats[idx].no_scs = no_smartcase;
    آخری_idx = idx;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    اگر (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    سیٹ_no_hlsearch(FALSE);
#اگر_ختم
}

/*
 * Save the search patterns, so they can be restored later.
 * Used before/after executing autocommands and user functions.
 */
ساکن صحیح save_level = 0;

    خالی
save_تلاش_patterns(خالی)
{
    صحیح i;

    اگر (save_level++ != 0)
	واپس;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(spats); ++i)
    {
	saved_spats[i] = spats[i];
	اگر (spats[i].pat != تہی)
	{
	    saved_spats[i].pat = vim_strnsave(spats[i].pat, spats[i].patlen);
	    اگر (saved_spats[i].pat == تہی)
		saved_spats[i].patlen = 0;
	    ورنہ
		saved_spats[i].patlen = spats[i].patlen;
	}
    }
    اگر (mr_نمونہ == تہی)
	saved_mr_نمونہ = تہی;
    ورنہ
	saved_mr_نمونہ = vim_strnsave(mr_نمونہ, mr_patternlen);
    اگر (saved_mr_نمونہ == تہی)
	saved_mr_patternlen = 0;
    ورنہ
	saved_mr_patternlen = mr_patternlen;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    saved_spats_آخری_idx = آخری_idx;
    saved_spats_no_hlsearch = no_hlsearch;
#اگر_ختم
}

    خالی
restore_تلاش_patterns(خالی)
{
    صحیح i;

    اگر (--save_level != 0)
	واپس;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(spats); ++i)
    {
	vim_free(spats[i].pat);
	spats[i] = saved_spats[i];
    }
#اگر defined(FEAT_EVAL)
    سیٹ_vv_searchforward();
#اگر_ختم
    vim_free(mr_نمونہ);
    mr_نمونہ = saved_mr_نمونہ;
    mr_patternlen = saved_mr_patternlen;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    آخری_idx = saved_spats_آخری_idx;
    سیٹ_no_hlsearch(saved_spats_no_hlsearch);
#اگر_ختم
}

#اگر defined(EXITFREE)
    خالی
free_تلاش_patterns(خالی)
{
    صحیح i;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(spats); ++i)
    {
	VIM_CLEAR(spats[i].pat);
	spats[i].patlen = 0;
    }
    VIM_CLEAR(mr_نمونہ);
    mr_patternlen = 0;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
// copy of spats[RE_SEARCH], for keeping the search patterns while incremental
// searching
ساکن spat_T	    saved_آخری_تلاش_spat;
ساکن صحیح	    did_save_آخری_تلاش_spat = 0;
ساکن صحیح	    saved_آخری_idx = 0;
ساکن صحیح	    saved_no_hlsearch = 0;
ساکن صحیح	    saved_تلاش_مطابقت_endcol;
ساکن صحیح	    saved_تلاش_مطابقت_سطریں;

/*
 * Save and restore the search pattern for incremental highlight search
 * feature.
 *
 * It's similar to but different from save_search_patterns() and
 * restore_search_patterns(), because the search pattern must be restored when
 * canceling incremental searching even if it's called inside user functions.
 */
    خالی
save_آخری_تلاش_نمونہ(خالی)
{
    اگر (++did_save_آخری_تلاش_spat != 1)
	// nested call, nothing to do
	واپس;

    saved_آخری_تلاش_spat = spats[RE_تلاش];
    اگر (spats[RE_تلاش].pat != تہی)
    {
	saved_آخری_تلاش_spat.pat = vim_strnsave(spats[RE_تلاش].pat, spats[RE_تلاش].patlen);
	اگر (saved_آخری_تلاش_spat.pat == تہی)
	    saved_آخری_تلاش_spat.patlen = 0;
	ورنہ
	    saved_آخری_تلاش_spat.patlen = spats[RE_تلاش].patlen;
    }
    saved_آخری_idx = آخری_idx;
    saved_no_hlsearch = no_hlsearch;
}

    خالی
restore_آخری_تلاش_نمونہ(خالی)
{
    اگر (--did_save_آخری_تلاش_spat > 0)
	// nested call, nothing to do
	واپس;
    اگر (did_save_آخری_تلاش_spat != 0)
    {
	iemsg("restore_last_search_pattern() called more often than save_last_search_pattern()");
	واپس;
    }

    vim_free(spats[RE_تلاش].pat);
    spats[RE_تلاش] = saved_آخری_تلاش_spat;
    saved_آخری_تلاش_spat.pat = تہی;
    saved_آخری_تلاش_spat.patlen = 0;
# اگر defined(FEAT_EVAL)
    سیٹ_vv_searchforward();
# endif
    آخری_idx = saved_آخری_idx;
    سیٹ_no_hlsearch(saved_no_hlsearch);
}

/*
 * Save and restore the incsearch highlighting variables.
 * This is required so that calling searchcount() at does not invalidate the
 * incsearch highlighting.
 */
    ساکن خالی
save_incsearch_state(خالی)
{
    saved_تلاش_مطابقت_endcol = تلاش_مطابقت_endcol;
    saved_تلاش_مطابقت_سطریں  = تلاش_مطابقت_سطریں;
}

    ساکن خالی
restore_incsearch_state(خالی)
{
    تلاش_مطابقت_endcol = saved_تلاش_مطابقت_endcol;
    تلاش_مطابقت_سطریں  = saved_تلاش_مطابقت_سطریں;
}

    char_u *
آخری_تلاش_نمونہ(خالی)
{
    واپس spats[RE_تلاش].pat;
}

    size_t
آخری_تلاش_نمونہ_len(خالی)
{
    واپس spats[RE_تلاش].patlen;
}
#اگر_ختم

/*
 * Return TRUE when case should be ignored for search pattern "pat".
 * Uses the 'ignorecase' and 'smartcase' options.
 */
    صحیح
ignorecase(char_u *pat)
{
    واپس ignorecase_opt(pat, p_ic, p_scs);
}

/*
 * As ignorecase() but pass the "ic" and "scs" flags.
 */
    صحیح
ignorecase_opt(char_u *pat, صحیح ic_اندر, صحیح scs)
{
    صحیح		ic = ic_اندر;

    اگر (ic && !no_smartcase && scs
			    && !(ctrl_x_منوال_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    واپس ic;
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
    (خالی)skip_regexp_ex(pat, NUL, magic_isset(), تہی, تہی, &magic_val);

    جب (*p != NUL)
    {
	صحیح		l;

	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    اگر (enc_utf8 && utf_isupper(utf_ptr2char(p)))
		واپس TRUE;
	    p += l;
	}
	ورنہ اگر (*p == '\\' && magic_val <= MAGIC_ON)
	{
	    اگر (p[1] == '_' && p[2] != NUL)  // skip "\_X"
		p += 3;
	    ورنہ اگر (p[1] == '%' && p[2] != NUL)  // skip "\%X"
		p += 3;
	    ورنہ اگر (p[1] != NUL)  // skip "\X"
		p += 2;
	    ورنہ
		p += 1;
	}
	ورنہ اگر ((*p == '%' || *p == '_') && magic_val == MAGIC_ALL)
	{
	    اگر (p[1] != NUL)  // skip "_X" and %X
		p += 2;
	    ورنہ
		p++;
	}
	ورنہ اگر (MB_ISUPPER(*p))
	    واپس TRUE;
	ورنہ
	    ++p;
    }
    واپس FALSE;
}

#اگر defined(FEAT_EVAL)
    char_u *
آخری_csearch(خالی)
{
    واپس lastc_بائٹیں;
}

    صحیح
آخری_csearch_forward(خالی)
{
    واپس lastcdir == FORWARD;
}

    صحیح
آخری_csearch_until(خالی)
{
    واپس آخری_t_cmd == TRUE;
}

    خالی
سیٹ_آخری_csearch(صحیح c, char_u *s, صحیح len)
{
    *lastc = c;
    lastc_bytelen = len;
    اگر (len)
	حافظہ_نقل(lastc_بائٹیں, s, len);
    ورنہ
	CLEAR_میدان(lastc_بائٹیں);
}
#اگر_ختم

    خالی
سیٹ_csearch_direction(صحیح cdir)
{
    lastcdir = cdir;
}

    خالی
سیٹ_csearch_until(صحیح t_cmd)
{
    آخری_t_cmd = t_cmd;
}

    char_u *
آخری_تلاش_pat(خالی)
{
    واپس spats[آخری_idx].pat;
}

/*
 * Reset search direction to forward.  For "gd" and "gD" commands.
 */
    خالی
reset_تلاش_dir(خالی)
{
    spats[0].off.dir = '/';
#اگر defined(FEAT_EVAL)
    سیٹ_vv_searchforward();
#اگر_ختم
}

#اگر defined(FEAT_EVAL) || defined(FEAT_VIMINFO)
/*
 * Set the last search pattern.  For ":let @/ =" and viminfo.
 * Also set the saved search pattern, so that this works in an autocommand.
 */
    خالی
سیٹ_آخری_تلاش_pat(
    char_u	*s,
    صحیح		idx,
    صحیح		magic,
    صحیح		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    اگر (*s == NUL)
	spats[idx].pat = تہی;
    ورنہ
    {
	spats[idx].patlen = STRLEN(s);
	spats[idx].pat = vim_strnsave(s, spats[idx].patlen);
    }
    اگر (spats[idx].pat == تہی)
	spats[idx].patlen = 0;
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
# اگر defined(FEAT_EVAL)
    سیٹ_vv_searchforward();
# endif
    spats[idx].off.سطر = FALSE;
    spats[idx].off.انتہا = FALSE;
    spats[idx].off.off = 0;
    اگر (setlast)
	آخری_idx = idx;
    اگر (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	اگر (spats[idx].pat == تہی)
	    saved_spats[idx].pat = تہی;
	ورنہ
	    saved_spats[idx].pat = vim_strnsave(spats[idx].pat, spats[idx].patlen);
	اگر (saved_spats[idx].pat == تہی)
	    saved_spats[idx].patlen = 0;
	ورنہ
	    saved_spats[idx].patlen = spats[idx].patlen;
# ifdef FEAT_تلاش_EXTRA
	saved_spats_آخری_idx = آخری_idx;
# endif
    }
# ifdef FEAT_تلاش_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    اگر (p_hls && idx == آخری_idx && !no_hlsearch)
	redraw_all_later(UPD_SOME_VALID);
# endif
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
/*
 * Get a regexp program for the last used search pattern.
 * This is used for highlighting all matches in a window.
 * Values returned in regmatch->regprog and regmatch->rmm_ic.
 */
    خالی
آخری_pat_prog(regmmatch_T *regmatch)
{
    اگر (spats[آخری_idx].pat == تہی)
    {
	regmatch->regprog = تہی;
	واپس;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (خالی)تلاش_regcomp((char_u *)"", 0, تہی, 0, آخری_idx, تلاش_KEEP, regmatch);
    --emsg_off;
}
#اگر_ختم

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
    pos_T	*انتہا_pos,	// set to end of the match, unless NULL
    صحیح		dir,
    char_u	*pat,
    size_t	patlen,
    طویل	گنتی,
    صحیح		options,
    صحیح		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    صحیح		ملا;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	ستون;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    صحیح		loop;
    pos_T	آغاز_pos;
    صحیح		at_پہلا_سطر;
    صحیح		extra_ستون;
    صحیح		آغاز_char_len;
    صحیح		مطابقت_ok;
    طویل	nmatched;
    صحیح		submatch = 0;
    صحیح		پہلا_مطابقت = TRUE;
    صحیح		called_emsg_before = called_emsg;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    صحیح		break_loop = FALSE;
#اگر_ختم
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
    صحیح		غیر_مستعمل_timeout_flag = FALSE;
    صحیح		*timed_برآمد = &غیر_مستعمل_timeout_flag;  // set when timed out.
    صحیح		تلاش_from_مطابقت_انتہا;		    // vi-compatible search?

    اگر (تلاش_regcomp(pat, patlen, تہی, RE_تلاش, pat_use,
		   (options & (تلاش_HIS + تلاش_KEEP)), &regmatch) == FAIL)
    {
	اگر ((options & تلاش_پیغام) && !rc_did_emsg)
	    semsg(_(e_invalid_تلاش_string_str), mr_نمونہ);
	واپس FAIL;
    }

    تلاش_from_مطابقت_انتہا = vim_strchr(p_cpo, CPO_تلاش) != تہی;

    اگر (extra_arg != تہی)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#اگر_تعریف_شدہ FEAT_RELTIME
	اگر (extra_arg->sa_tm > 0)
	    init_regexp_timeout(extra_arg->sa_tm);
	// Also set the pointer when sa_tm is zero, the caller may have set the
	// timeout.
	timed_برآمد = &extra_arg->sa_timed_برآمد;
#اگر_ختم
    }

    /*
     * find the string
     */
    کرو	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	اگر (pos->ستون == MAXCOL)
	    آغاز_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	ورنہ اگر (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_سطر_گنتی
						    && pos->ستون < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    اگر (ml_get_buf_len(buf, pos->lnum) <= pos->ستون)
		آغاز_char_len = 1;
	    ورنہ
		آغاز_char_len = (*mb_ptr2len)(ptr + pos->ستون);
	}
	ورنہ
	    آغاز_char_len = 1;
	اگر (dir == FORWARD)
	{
	    اگر (options & تلاش_آغاز)
		extra_ستون = 0;
	    ورنہ
		extra_ستون = آغاز_char_len;
	}
	ورنہ
	{
	    اگر (options & تلاش_آغاز)
		extra_ستون = آغاز_char_len;
	    ورنہ
		extra_ستون = 0;
	}

	آغاز_pos = *pos;	// remember start pos for detecting no match
	ملا = 0;		// default: not found
	at_پہلا_سطر = TRUE;	// default: start in first line
	اگر (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->ستون = 0;
	    at_پہلا_سطر = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	اگر (dir == BACKWARD && آغاز_pos.ستون == 0
					     && (options & تلاش_آغاز) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_پہلا_سطر = FALSE;
	}
	ورنہ
	    lnum = pos->lnum;

	برائے (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    برائے ( ; lnum > 0 && lnum <= buf->b_ml.ml_سطر_گنتی;
					   lnum += dir, at_پہلا_سطر = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		اگر (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    توڑو;
		// Stop after passing the time limit.
		اگر (*timed_برآمد)
		    توڑو;

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		ستون = at_پہلا_سطر && (options & تلاش_ستون) ? pos->ستون
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
							 lnum, ستون, timed_برآمد);
		// vim_regexec_multi() may clear "regprog"
		اگر (regmatch.regprog == تہی)
		    توڑو;
		// Abort searching on an error (e.g., out of stack).
		اگر (called_emsg > called_emsg_before || *timed_برآمد)
		    توڑو;
		اگر (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#اگر_تعریف_شدہ FEAT_EVAL
		    submatch = پہلا_submatch(&regmatch);
#اگر_ختم
		    // "lnum" may be past end of buffer for "\n\zs".
		    اگر (lnum + matchpos.lnum > buf->b_ml.ml_سطر_گنتی)
			ptr = (char_u *)"";
		    ورنہ
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    اگر (dir == FORWARD && at_پہلا_سطر)
		    {
			مطابقت_ok = TRUE;

			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			جب (matchpos.lnum == 0
				&& ((options & تلاش_انتہا) && پہلا_مطابقت
				    ?  (nmatched == 1
					&& (صحیح)endpos.ستون - 1
					     < (صحیح)آغاز_pos.ستون + extra_ستون)
				    : ((صحیح)matchpos.ستون
						  - (ptr[matchpos.ستون] == NUL)
					    < (صحیح)آغاز_pos.ستون + extra_ستون)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    اگر (تلاش_from_مطابقت_انتہا)
			    {
				اگر (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    مطابقت_ok = FALSE;
				    توڑو;
				}
				matchcol = endpos.ستون;
				// for empty match: advance one char
				اگر (matchcol == matchpos.ستون
						      && ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ورنہ
					++matchcol;
				}
			    }
			    ورنہ
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
				    ورنہ
					++matchcol;
				}
			    }
			    اگر (matchcol == 0 && (options & تلاش_آغاز))
				توڑو;
			    اگر (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_برآمد)) == 0)
			    {
				مطابقت_ok = FALSE;
				توڑو;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    اگر (regmatch.regprog == تہی)
				توڑو;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
#اگر_تعریف_شدہ FEAT_EVAL
			    submatch = پہلا_submatch(&regmatch);
#اگر_ختم

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			اگر (!مطابقت_ok)
			    جاری;
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
			مطابقت_ok = FALSE;
			برائے (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    اگر (loop
				|| ((options & تلاش_انتہا)
				    ? (lnum + regmatch.endpos[0].lnum
							      < آغاز_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == آغاز_pos.lnum
					     && (صحیح)regmatch.endpos[0].ستون - 1
							< (صحیح)آغاز_pos.ستون
								+ extra_ستون))
				    : (lnum + regmatch.startpos[0].lnum
							      < آغاز_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == آغاز_pos.lnum
					     && (صحیح)regmatch.startpos[0].ستون
						      < (صحیح)آغاز_pos.ستون
							      + extra_ستون))))
			    {
				مطابقت_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
#اگر_تعریف_شدہ FEAT_EVAL
				submatch = پہلا_submatch(&regmatch);
#اگر_ختم
			    }
			    ورنہ
				توڑو;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    اگر (تلاش_from_مطابقت_انتہا)
			    {
				اگر (nmatched > 1)
				    توڑو;
				matchcol = endpos.ستون;
				// for empty match: advance one char
				اگر (matchcol == matchpos.ستون
						      && ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ورنہ
					++matchcol;
				}
			    }
			    ورنہ
			    {
				// Stop when the match is in a next line.
				اگر (matchpos.lnum > 0)
				    توڑو;
				matchcol = matchpos.ستون;
				اگر (ptr[matchcol] != NUL)
				{
				    اگر (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    ورنہ
					++matchcol;
				}
			    }
			    اگر (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol, timed_برآمد)) == 0)
			    {
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				اگر (*timed_برآمد)
				    مطابقت_ok = FALSE;
				توڑو;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    اگر (regmatch.regprog == تہی)
				توڑو;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			اگر (!مطابقت_ok)
			    جاری;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    اگر ((options & تلاش_انتہا) && !(options & تلاش_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.ستون == endpos.ستون))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->ستون = endpos.ستون;
			اگر (endpos.ستون == 0)
			{
			    اگر (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->ستون = ml_get_buf_len(buf, pos->lnum);
			    }
			}
			ورنہ
			{
			    --pos->ستون;
			    اگر (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_سطر_گنتی)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->ستون -= (*mb_سر_off)(ptr, ptr + pos->ستون);
			    }
			}
			اگر (انتہا_pos != تہی)
			{
			    انتہا_pos->lnum = lnum + matchpos.lnum;
			    انتہا_pos->ستون = matchpos.ستون;
			}
		    }
		    ورنہ
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->ستون = matchpos.ستون;
			اگر (انتہا_pos != تہی)
			{
			    انتہا_pos->lnum = lnum + endpos.lnum;
			    انتہا_pos->ستون = endpos.ستون;
			}
		    }
		    pos->coladd = 0;
		    اگر (انتہا_pos != تہی)
			انتہا_pos->coladd = 0;
		    ملا = 1;
		    پہلا_مطابقت = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    تلاش_مطابقت_سطریں = endpos.lnum - matchpos.lnum;
		    تلاش_مطابقت_endcol = endpos.ستون;
		    توڑو;
		}
		سطر_breakcheck();	// stop if ctrl-C typed
		اگر (got_int)
		    توڑو;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		اگر ((options & تلاش_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    توڑو;
		}
#اگر_ختم

		اگر (loop && lnum == آغاز_pos.lnum)
		    توڑو;	    // if second loop, stop where started
	    }
	    at_پہلا_سطر = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    اگر (regmatch.regprog == تہی)
		توڑو;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    اگر (!p_ws || stop_lnum != 0 || got_int
			      || called_emsg > called_emsg_before || *timed_برآمد
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
			      || break_loop
#اگر_ختم
			      || ملا || loop)
		توڑو;

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
		lnum = buf->b_ml.ml_سطر_گنتی;
	    ورنہ
		lnum = 1;
	    اگر (!shortmess(SHM_تلاش)
		    && shortmess(SHM_SEARCHCOUNT)
		    && (options & تلاش_پیغام))
		give_warning((char_u *)_(dir == BACKWARD
					  ? چوٹی_bot_پیغام : bot_چوٹی_پیغام), TRUE);
	    اگر (extra_arg != تہی)
		extra_arg->sa_wrapped = TRUE;
	}
	اگر (got_int || called_emsg > called_emsg_before || *timed_برآمد
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		|| break_loop
#اگر_ختم
		)
	    توڑو;
    }
    جب (--گنتی > 0 && ملا);   // stop after count matches or no match

#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (extra_arg != تہی && extra_arg->sa_tm > 0)
	disable_regexp_timeout();
#اگر_ختم
    vim_regfree(regmatch.regprog);

    اگر (!ملا)		    // did not find it
    {
	اگر (got_int)
	    emsg(_(e_interrupted));
	ورنہ اگر ((options & تلاش_پیغام) == تلاش_پیغام)
	{
	    اگر (p_ws)
		semsg(_(e_نمونہ_not_ملا_str), mr_نمونہ);
	    ورنہ اگر (lnum == 0)
		semsg(_(e_تلاش_کامیابی_چوٹی_without_مطابقت_for_str), mr_نمونہ);
	    ورنہ
		semsg(_(e_تلاش_کامیابی_bottom_without_مطابقت_for_str), mr_نمونہ);
	}
	واپس FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    اگر (pos->lnum > buf->b_ml.ml_سطر_گنتی)
    {
	pos->lnum = buf->b_ml.ml_سطر_گنتی;
	pos->ستون = ml_get_buf_len(buf, pos->lnum);
	اگر (pos->ستون > 0)
	    --pos->ستون;
    }

    واپس submatch + 1;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
    خالی
سیٹ_تلاش_direction(صحیح cdir)
{
    spats[0].off.dir = cdir;
}

    ساکن خالی
سیٹ_vv_searchforward(خالی)
{
    سیٹ_vim_var_nr(VV_SEARCHFORWARD, (طویل)(spats[0].off.dir == '/'));
}

/*
 * Return the number of the first subpat that matched.
 * Return zero if none of them matched.
 */
    ساکن صحیح
پہلا_submatch(regmmatch_T *rp)
{
    صحیح		submatch;

    برائے (submatch = 1; ; ++submatch)
    {
	اگر (rp->startpos[submatch].lnum >= 0)
	    توڑو;
	اگر (submatch == 9)
	{
	    submatch = 0;
	    توڑو;
	}
    }
    واپس submatch;
}
#اگر_ختم

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
parse_تلاش_نمونہ_انحراف(
    char_u	**pat,
    size_t	*patlen,
    صحیح		تلاش_delim,
    صحیح		options,
    char_u	**strcopy,
    char_u	**searchstr,
    size_t	*searchstrlen,
    char_u	**dircp,
    soffset_T	*انحراف)
{
    صحیح		cmdlen = 0;
    char_u	*p;
    char_u	*ps;

    اگر (*pat == تہی || **pat == NUL)
	واپس 0;

    ps = *strcopy;
    *searchstr = *pat;
    *searchstrlen = *patlen;
    *dircp = تہی;

    /*
     * Find end of regular expression.
     * If there is a matching '/' or '?', toss it.
     */
    p = skip_regexp_ex(*pat, تلاش_delim, magic_isset(),
						    strcopy, تہی, تہی);
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
    اگر (*p == تلاش_delim)
    {
	*searchstrlen = p - *pat;
	*dircp = p;	// remember where we put the NUL
	*p++ = NUL;
    }

    انحراف->سطر = FALSE;
    انحراف->انتہا = FALSE;
    انحراف->off = 0;
    /*
     * Check for a line offset or a character offset.
     * For get_address (echo off) we don't check for a character
     * offset, because it is meaningless and the 's' could be a
     * substitute command.
     */
    اگر (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
	انحراف->سطر = TRUE;
    ورنہ اگر ((options & تلاش_OPT)
				  && (*p == 'e' || *p == 's' || *p == 'b'))
    {
	اگر (*p == 'e')		// end
	    انحراف->انتہا = تلاش_انتہا;
	++p;
    }
    اگر (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
    {
				    // 'nr' or '+nr' or '-nr'
	اگر (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
	    انحراف->off = atol((حرف *)p);
	ورنہ اگر (*p == '-')	    // single '-'
	    انحراف->off = -1;
	ورنہ			    // single '+'
	    انحراف->off = 1;
	++p;
	جب (VIM_ISDIGIT(*p))	    // skip number
	    ++p;
    }

    // compute length of search command for get_address()
    cmdlen += (صحیح)(p - *pat);
    *patlen -= p - *pat;
    *pat = p;			    // put pat after search command

    واپس cmdlen;
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
do_تلاش(
    oparg_T	    *oap,	// can be NULL
    صحیح		    dirc,	// '/' or '?'
    صحیح		    تلاش_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    size_t	    patlen,
    طویل	    گنتی,
    صحیح		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    size_t	    searchstrlen;
    soffset_T	    old_off;
    صحیح		    retval;	// Return value
    char_u	    *p;
    طویل	    c;
    char_u	    *dircp;
    char_u	    *strcopy = تہی;
    صحیح		    show_تلاش_stats;
    char_u	    *msgbuf = تہی;
    size_t	    msgbuflen = 0;
    صحیح		    has_انحراف = FALSE;

    searchcmdlen = 0;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    اگر (spats[0].off.سطر && vim_strchr(p_cpo, CPO_LINEOFF) != تہی)
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
    ورنہ
    {
	spats[0].off.dir = dirc;
#اگر defined(FEAT_EVAL)
	سیٹ_vv_searchforward();
#اگر_ختم
    }
    اگر (options & تلاش_REV)
    {
#اگر_تعریف_شدہ MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#ورنہ
	اگر (dirc == '/')
	    dirc = '?';
	ورنہ
	    dirc = '/';
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    اگر (dirc == '/')
    {
	اگر (hasFolding(pos.lnum, تہی, &pos.lnum))
	    pos.ستون = MAXCOL - 2;	// avoid overflow when adding 1
    }
    ورنہ
    {
	اگر (hasFolding(pos.lnum, &pos.lnum, تہی))
	    pos.ستون = 0;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    اگر (no_hlsearch && !(options & تلاش_KEEP))
    {
	redraw_all_later(UPD_SOME_VALID);
	سیٹ_no_hlsearch(FALSE);
    }
#اگر_ختم

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    برائے (;;)
    {
	صحیح		show_چوٹی_bot_پیغام = FALSE;

	searchstr = pat;
	searchstrlen = patlen;

	dircp = تہی;
					    // use previous pattern
	اگر (pat == تہی || *pat == NUL || *pat == تلاش_delim)
	{
	    اگر (spats[RE_تلاش].pat == تہی)	    // no previous pattern
	    {
		اگر (spats[RE_SUBST].pat == تہی)
		{
		    emsg(_(e_no_پچھلا_regular_عبارت));
		    retval = 0;
		    جاؤ انتہا_do_تلاش;
		}
		searchstr = spats[RE_SUBST].pat;
		searchstrlen = spats[RE_SUBST].patlen;
	    }
	    ورنہ
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
		searchstrlen = 0;
	    }
	}

	اگر (pat != تہی && *pat != NUL)	// look for (new) offset
	{
	    char_u	*پچھلا_searchstr = searchstr;
	    size_t	پچھلا_searchstrlen = searchstrlen;

	    searchcmdlen += parse_تلاش_نمونہ_انحراف(&pat, &patlen,
				تلاش_delim, options, &strcopy, &searchstr,
				&searchstrlen, &dircp, &spats[0].off);
	    // An empty pattern (e.g. "//e") uses the previous pattern selected
	    // above.
	    اگر (*searchstr == NUL)
	    {
		searchstr = پچھلا_searchstr;
		searchstrlen = پچھلا_searchstrlen;
	    }
	}

	show_تلاش_stats = FALSE;
	اگر ((options & تلاش_ECHO) && messaging()
		&& !پیغام_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	off_buf[40];
	    size_t	off_len = 0;
	    size_t	plen;
	    size_t	msgbufsize;

	    // Compute msg_row early.
	    پیغام_آغاز();

	    // Get the offset, so we know how long it is.
	    اگر (!cmd_silent &&
		    (spats[0].off.سطر || spats[0].off.انتہا || spats[0].off.off))
	    {
		off_buf[off_len++] = dirc;
		اگر (spats[0].off.انتہا)
		    off_buf[off_len++] = 'e';
		ورنہ اگر (!spats[0].off.سطر)
		    off_buf[off_len++] = 's';
		off_buf[off_len] = NUL;
		اگر (spats[0].off.off != 0 || spats[0].off.سطر)
		    off_len += vim_snprintf((حرف *)off_buf + off_len,
			   حجم(off_buf) - off_len, "%+ld", spats[0].off.off);
	    }

	    اگر (*searchstr == NUL)
	    {
		p = spats[0].pat;
		plen = spats[0].patlen;
	    }
	    ورنہ
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
		اگر (پیغام_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    msgbufsize = (صحیح)(صفیں - پیغام_صف) * cmdline_چوڑائی - 1;
		ورنہ
		    // Use up to 'showcmd' column.
		    msgbufsize = (صحیح)(صفیں - پیغام_صف - 1) * cmdline_چوڑائی
			+ sc_ستون - 1;
		اگر (msgbufsize < plen + off_len + تلاش_STAT_BUF_LEN + 3)
		    msgbufsize = plen + off_len + تلاش_STAT_BUF_LEN + 3;
	    }
	    ورنہ
		// Reserve enough space for the search pattern + offset.
		msgbufsize = plen + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(msgbufsize);
	    اگر (msgbuf == تہی)
	    {
		msgbuflen = 0;
	    }
	    ورنہ
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
		    ورنہ
			mch_memmove(msgbuf + 1, p, plen);
		    اگر (off_len > 0)
			mch_memmove(msgbuf + plen + 1, off_buf, off_len);

		    trunc = پیغام_strtrunc(msgbuf, TRUE);
		    اگر (trunc != تہی)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
			msgbuflen = STRLEN(msgbuf);
		    }

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = الٹ_متن(msgbuf);
			اگر (r != تہی)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    msgbuflen = STRLEN(msgbuf);
			    // move reversed text to beginning of buffer
			    جب (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + msgbuflen - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    اگر ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    ورنہ
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#اگر_ختم
		    پیغام_outtrans(msgbuf);
		    پیغام_clr_eos();
		    پیغام_check();

		    gotocmdline(FALSE);
		    برآمد_flush();
		    پیغام_nowait = TRUE;	    // don't wait for this message
		}

		اگر (!shortmess(SHM_SEARCHCOUNT))
		    show_تلاش_stats = TRUE;
	    }	// msgbuf != NULL
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	اگر (!spats[0].off.سطر && spats[0].off.off && pos.ستون < MAXCOL - 2)
	{
	    اگر (spats[0].off.off > 0)
	    {
		برائے (c = spats[0].off.off; c; --c)
		    اگر (decl(&pos) == -1)
			توڑو;
		اگر (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.ستون = MAXCOL;
		}
	    }
	    ورنہ
	    {
		برائے (c = spats[0].off.off; c; ++c)
		    اگر (incl(&pos) == -1)
			توڑو;
		اگر (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_سطر_گنتی + 1;
		    pos.ستون = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, تہی,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, searchstrlen, گنتی, spats[0].off.انتہا + (options &
		       (تلاش_KEEP + تلاش_PEEK + تلاش_HIS
			+ تلاش_پیغام + تلاش_آغاز
			+ ((pat != تہی && *pat == ';') ? 0 : تلاش_NOOF))),
		RE_آخری, sia);

	اگر (dircp != تہی)
	    *dircp = تلاش_delim; // restore second '/' or '?' for normal_cmd()

	اگر (!shortmess(SHM_تلاش) && sia && sia->sa_wrapped)
	    show_چوٹی_bot_پیغام = TRUE;

	اگر (c == FAIL)
	{
	    retval = 0;
	    جاؤ انتہا_do_تلاش;
	}
	اگر (spats[0].off.انتہا && oap != تہی)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	اگر (!(options & تلاش_NOOF) || (pat != تہی && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    اگر (spats[0].off.سطر)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		اگر (c < 1)
		    pos.lnum = 1;
		ورنہ اگر (c > curbuf->b_ml.ml_سطر_گنتی)
		    pos.lnum = curbuf->b_ml.ml_سطر_گنتی;
		ورنہ
		    pos.lnum = c;
		pos.ستون = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    ورنہ اگر (pos.ستون < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		اگر (c > 0)
		{
		    جب (c-- > 0)
			اگر (incl(&pos) == -1)
			    توڑو;
		}
		// to the left, check for start of file
		ورنہ
		{
		    جب (c++ < 0)
			اگر (decl(&pos) == -1)
			    توڑو;
		}
	    }
	    اگر (!EQUAL_POS(pos, org_pos))
		has_انحراف = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	اگر (show_تلاش_stats)
	     cmdline_تلاش_stat(dirc, &pos, &curwin->w_cursor,
				show_چوٹی_bot_پیغام, msgbuf, msgbuflen,
				(گنتی != 1 || has_انحراف
#اگر_تعریف_شدہ FEAT_FOLDING
				 || (!(fdo_flags & FDO_تلاش)
				     && hasFolding(curwin->w_cursor.lnum,
								   تہی, تہی))
#اگر_ختم
				),
				p_msc,
				تلاش_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	اگر (!(options & تلاش_OPT) || pat == تہی || *pat != ';')
	    توڑو;

	dirc = *++pat;
	--patlen;
	تلاش_delim = dirc;
	اگر (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    جاؤ انتہا_do_تلاش;
	}
	++pat;
	--patlen;
    }

    اگر (options & تلاش_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_سیٹ_curswant = true;

انتہا_do_تلاش:
    اگر ((options & تلاش_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    واپس retval;
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
تلاش_for_exact_سطر(
    buf_T	*buf,
    pos_T	*pos,
    صحیح		dir,
    char_u	*pat)
{
    linenr_T	آغاز = 0;
    char_u	*ptr;
    char_u	*p;
    صحیح		compl_len = ins_compl_len();

    اگر (buf->b_ml.ml_سطر_گنتی == 0)
	واپس FAIL;
    برائے (;;)
    {
	pos->lnum += dir;
	اگر (pos->lnum < 1)
	{
	    اگر (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_سطر_گنتی;
		اگر (!shortmess(SHM_تلاش))
		    give_warning((char_u *)_(چوٹی_bot_پیغام), TRUE);
	    }
	    ورنہ
	    {
		pos->lnum = 1;
		توڑو;
	    }
	}
	ورنہ اگر (pos->lnum > buf->b_ml.ml_سطر_گنتی)
	{
	    اگر (p_ws)
	    {
		pos->lnum = 1;
		اگر (!shortmess(SHM_تلاش))
		    give_warning((char_u *)_(bot_چوٹی_پیغام), TRUE);
	    }
	    ورنہ
	    {
		pos->lnum = 1;
		توڑو;
	    }
	}
	اگر (pos->lnum == آغاز)
	    توڑو;
	اگر (آغاز == 0)
	    آغاز = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->ستون = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	اگر (compl_حالت_adding() && !compl_حالت_sol())
	{
	    اگر ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		واپس OK;
	}
	ورنہ اگر (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    اگر ((p_ic ? MB_STRNICMP(p, pat, compl_len)
				   : STRNCMP(p, pat, compl_len)) == 0)
		واپس OK;
	}
    }
    واپس FAIL;
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
    طویل		گنتی = cap->count1;	// repeat count
    صحیح			ستون;
    char_u		*p;
    صحیح			len;
    صحیح			stop = TRUE;

    اگر (c != NUL)	// normal search: remember args for repeat
    {
	اگر (!کلید_Stuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    سیٹ_csearch_direction(dir);
	    سیٹ_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_بائٹیں);
	    اگر (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_بائٹیں + lastc_bytelen);
		اگر (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_بائٹیں + lastc_bytelen);
	    }
	}
    }
    ورنہ		// repeat previous search
    {
	اگر (*lastc == NUL && lastc_bytelen <= 1)
	    واپس FAIL;
	اگر (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	ورنہ
	    dir = lastcdir;
	t_cmd = آخری_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	اگر (vim_strchr(p_cpo, CPO_SCOLON) == تہی && گنتی == 1 && t_cmd)
	    stop = FALSE;
    }

    اگر (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    ورنہ
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    ستون = curwin->w_cursor.ستون;
    len = ml_get_curline_len();

    جب (گنتی--)
    {
	اگر (has_mbyte)
	{
	    برائے (;;)
	    {
		اگر (dir > 0)
		{
		    ستون += (*mb_ptr2len)(p + ستون);
		    اگر (ستون >= len)
			واپس FAIL;
		}
		ورنہ
		{
		    اگر (ستون == 0)
			واپس FAIL;
		    ستون -= (*mb_سر_off)(p, p + ستون - 1) + 1;
		}
		اگر (lastc_bytelen <= 1)
		{
		    اگر (p[ستون] == c && stop)
			توڑو;
		}
		ورنہ اگر (STRNCMP(p + ستون, lastc_بائٹیں, lastc_bytelen) == 0
								       && stop)
		    توڑو;
		stop = TRUE;
	    }
	}
	ورنہ
	{
	    برائے (;;)
	    {
		اگر ((ستون += dir) < 0 || ستون >= len)
		    واپس FAIL;
		اگر (p[ستون] == c && stop)
		    توڑو;
		stop = TRUE;
	    }
	}
    }

    اگر (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	ستون -= dir;
	اگر (has_mbyte)
	{
	    اگر (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		ستون += lastc_bytelen - 1;
	    ورنہ
		// To previous char, which may be multi-byte.
		ستون -= (*mb_سر_off)(p, p + ستون);
	}
    }
    curwin->w_cursor.ستون = ستون;

    واپس OK;
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
    واپس findmatchlimit(oap, initc, 0, 0);
}

/*
 * Return TRUE if the character before "linep[col]" equals "ch".
 * Return FALSE if "col" is zero.
 * Update "*prevcol" to the column of the previous character, unless "prevcol"
 * is NULL.
 * Handles multibyte string correctly.
 */
    ساکن صحیح
check_prevcol(
    char_u	*linep,
    صحیح		ستون,
    صحیح		ch,
    صحیح		*prevcol)
{
    --ستون;
    اگر (ستون > 0 && has_mbyte)
	ستون -= (*mb_سر_off)(linep, linep + ستون);
    اگر (prevcol)
	*prevcol = ستون;
    واپس (ستون >= 0 && linep[ستون] == ch) ? TRUE : FALSE;
}

/*
 * Raw string start is found at linep[startpos.col - 1].
 * Return TRUE if the matching end can be found between startpos and endpos.
 */
    ساکن صحیح
ڈھونڈو_rawstring_انتہا(char_u *linep, pos_T *startpos, pos_T *endpos)
{
    char_u	*p;
    char_u	*delim_copy;
    size_t	delim_len;
    linenr_T	lnum;
    صحیح		ملا = FALSE;

    برائے (p = linep + startpos->ستون + 1; *p && *p != '('; ++p)
	;
    delim_len = (p - linep) - startpos->ستون - 1;
    delim_copy = vim_strnsave(linep + startpos->ستون + 1, delim_len);
    اگر (delim_copy == تہی)
	واپس FALSE;
    برائے (lnum = startpos->lnum; lnum <= endpos->lnum; ++lnum)
    {
	char_u *سطر = ml_get(lnum);

	برائے (p = سطر + (lnum == startpos->lnum
					    ? startpos->ستون + 1 : 0); *p; ++p)
	{
	    اگر (lnum == endpos->lnum && (colnr_T)(p - سطر) >= endpos->ستون)
		توڑو;
	    اگر (*p == ')' && STRNCMP(delim_copy, p + 1, delim_len) == 0
			  && p[delim_len + 1] == '"')
	    {
		ملا = TRUE;
		توڑو;
	    }
	}
	اگر (ملا)
	    توڑو;
    }
    vim_free(delim_copy);
    واپس ملا;
}

/*
 * Check matchpairs option for "*initc".
 * If there is a match set "*initc" to the matching character and "*findc" to
 * the opposite character.  Set "*backwards" to the direction.
 * When "switchit" is TRUE swap the direction.
 */
    ساکن خالی
ڈھونڈو_mps_اقدار(
    صحیح	    *initc,
    صحیح	    *findc,
    صحیح	    *backwards,
    صحیح	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    جب (*ptr != NUL)
    {
	اگر (has_mbyte)
	{
	    char_u *پچھلا;

	    اگر (mb_ptr2char(ptr) == *initc)
	    {
		اگر (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		ورنہ
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		واپس;
	    }
	    پچھلا = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    اگر (mb_ptr2char(ptr) == *initc)
	    {
		اگر (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(پچھلا);
		    *backwards = FALSE;
		}
		ورنہ
		{
		    *findc = mb_ptr2char(پچھلا);
		    *backwards = TRUE;
		}
		واپس;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	ورنہ
	{
	    اگر (*ptr == *initc)
	    {
		اگر (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		ورنہ
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		واپس;
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
		ورنہ
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		واپس;
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
    ساکن pos_T pos;			// current search position
    صحیح		findc = 0;		// matching brace
    صحیح		c;
    صحیح		گنتی = 0;		// cumulative number of braces
    صحیح		backwards = FALSE;	// init for gcc
    صحیح		raw_string = FALSE;	// search for raw string
    صحیح		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    صحیح		do_quotes;		// check for quotes in current line
    صحیح		at_آغاز;		// do_quotes value at start position
    صحیح		hash_dir = 0;		// Direction searched for # things
    صحیح		comment_dir = 0;	// Direction searched for comments
    pos_T	مطابقت_pos;		// Where last slash-star was found
    صحیح		آغاز_اندر_quotes;	// start position is in quotes
    صحیح		traveled = 0;		// how far we've searched so far
    صحیح		ignore_cend = FALSE;    // ignore comment end
    صحیح		cpo_مطابقت;		// vi compatible matching
    صحیح		cpo_bsl;		// don't recognize backslashes
    صحیح		مطابقت_escaped = 0;	// search for escaped match
    صحیح		dir;			// Direction to search
    صحیح		comment_ستون = MAXCOL;   // start of / / comment
    صحیح		lispcomm = FALSE;	// inside of Lisp-style comment
    صحیح		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)
    صحیح		skip_comments = (flags & FM_SKIPCOMM) != 0;
    صحیح		اندر_بلاک_comment = FALSE; // inside /* */ block comment

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_مطابقت = (vim_strchr(p_cpo, CPO_مطابقت) != تہی);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != تہی);

    // Direction to search when initc is '/', '*' or '#'
    اگر (flags & FM_BACKWARD)
	dir = BACKWARD;
    ورنہ اگر (flags & FM_FORWARD)
	dir = FORWARD;
    ورنہ
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
    ورنہ اگر (initc != '#' && initc != NUL)
    {
	ڈھونڈو_mps_اقدار(&initc, &findc, &backwards, TRUE);
	اگر (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	اگر (findc == NUL)
	    واپس تہی;
    }
    ورنہ
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	اگر (initc == '#')
	{
	    hash_dir = dir;
	}
	ورنہ
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    اگر (!cpo_مطابقت)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		اگر (*ptr == '#' && pos.ستون <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    اگر (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		ورنہ اگر (linep[pos.ستون] == '/')
		{
		    اگر (linep[pos.ستون + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.ستون++;
		    }
		    ورنہ اگر (pos.ستون > 0 && linep[pos.ستون - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.ستون--;
		    }
		}
		ورنہ اگر (linep[pos.ستون] == '*')
		{
		    اگر (linep[pos.ستون + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    ورنہ اگر (pos.ستون > 0 && linep[pos.ستون - 1] == '/')
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
		اگر (linep[pos.ستون] == NUL && pos.ستون)
		    --pos.ستون;
		برائے (;;)
		{
		    initc = PTR2CHAR(linep + pos.ستون);
		    اگر (initc == NUL)
			توڑو;

		    ڈھونڈو_mps_اقدار(&initc, &findc, &backwards, FALSE);
		    اگر (findc)
			توڑو;
		    pos.ستون += mb_ptr2len(linep + pos.ستون);
		}
		اگر (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    اگر (!cpo_مطابقت && *skipwhite(linep) == '#')
			hash_dir = 1;
		    ورنہ
			واپس تہی;
		}
		ورنہ اگر (!cpo_bsl)
		{
		    صحیح ستون, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    برائے (ستون = pos.ستون; check_prevcol(linep, ستون, '\\', &ستون);)
			bslcnt++;
		    مطابقت_escaped = (bslcnt & 1);
		}
	    }
	}
	اگر (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    اگر (oap != تہی)
		oap->motion_type = MLINE;   // Linewise for this case only
	    اگر (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		اگر (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		ورنہ اگر (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		ورنہ
		    واپس تہی;
	    }
	    pos.ستون = 0;
	    جب (!got_int)
	    {
		اگر (hash_dir > 0)
		{
		    اگر (pos.lnum == curbuf->b_ml.ml_سطر_گنتی)
			توڑو;
		}
		ورنہ اگر (pos.lnum == 1)
		    توڑو;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		سطر_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		اگر (*ptr != '#')
		    جاری;
		pos.ستون = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		اگر (hash_dir > 0)
		{
		    اگر (STRNCMP(ptr, "if", 2) == 0)
			گنتی++;
		    ورنہ اگر (STRNCMP(ptr, "el", 2) == 0)
		    {
			اگر (گنتی == 0)
			    واپس &pos;
		    }
		    ورنہ اگر (STRNCMP(ptr, "endif", 5) == 0)
		    {
			اگر (گنتی == 0)
			    واپس &pos;
			گنتی--;
		    }
		}
		ورنہ
		{
		    اگر (STRNCMP(ptr, "if", 2) == 0)
		    {
			اگر (گنتی == 0)
			    واپس &pos;
			گنتی--;
		    }
		    ورنہ اگر (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			اگر (گنتی == 0)
			    واپس &pos;
		    }
		    ورنہ اگر (STRNCMP(ptr, "endif", 5) == 0)
			گنتی++;
		}
	    }
	    واپس تہی;
	}
    }

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    اگر (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != تہی)
	backwards = !backwards;
#اگر_ختم

    do_quotes = -1;
    آغاز_اندر_quotes = MAYBE;
    CLEAR_POS(&مطابقت_pos);

    // backward search: Check if this line contains a single-line comment
    اگر ((backwards && comment_dir) || lisp || skip_comments)
	comment_ستون = check_linecomment(linep);
    اگر (lisp && comment_ستون != MAXCOL && pos.ستون > (colnr_T)comment_ستون)
	lispcomm = TRUE;    // find match inside this comment
    // skip // comment portion at starting position
    اگر (skip_comments && !اندر_بلاک_comment && comment_ستون != MAXCOL
	    && backwards && pos.ستون > (colnr_T)comment_ستون)
	pos.ستون = comment_ستون;

    جب (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	اگر (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    اگر (lispcomm && pos.ستون < (colnr_T)comment_ستون)
		توڑو;
	    اگر (pos.ستون == 0)		// at start of line, go to prev. one
	    {
		اگر (pos.lnum == 1)	// start of file
		    توڑو;
		--pos.lnum;

		اگر (maxtravel > 0 && ++traveled > maxtravel)
		    توڑو;

		linep = ml_get(pos.lnum);
		pos.ستون = ml_get_len(pos.lnum); // pos.col on trailing NUL
		do_quotes = -1;
		سطر_breakcheck();

		// Check if this line contains a single-line comment
		اگر (comment_dir || lisp || skip_comments)
		    comment_ستون = check_linecomment(linep);
		// skip comment
		اگر (lisp && comment_ستون != MAXCOL)
		    pos.ستون = comment_ستون;
		ورنہ اگر (skip_comments && !اندر_بلاک_comment
			&& comment_ستون != MAXCOL
			&& pos.ستون > (colnr_T)comment_ستون)
		    pos.ستون = comment_ستون;
	    }
	    ورنہ
	    {
		--pos.ستون;
		اگر (has_mbyte)
		    pos.ستون -= (*mb_سر_off)(linep, linep + pos.ستون);
	    }
	}
	ورنہ				// forward search
	{
	    اگر (linep[pos.ستون] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_ستون != MAXCOL
					   && pos.ستون == (colnr_T)comment_ستون))
	    {
		اگر (pos.lnum == curbuf->b_ml.ml_سطر_گنتی  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    توڑو;
		++pos.lnum;

		اگر (maxtravel && traveled++ > maxtravel)
		    توڑو;

		linep = ml_get(pos.lnum);
		pos.ستون = 0;
		do_quotes = -1;
		سطر_breakcheck();
		اگر (lisp || skip_comments)   // find comment pos in new line
		    comment_ستون = check_linecomment(linep);
	    }
	    ورنہ
	    {
		اگر (has_mbyte)
		    pos.ستون += (*mb_ptr2len)(linep + pos.ستون);
		ورنہ
		    ++pos.ستون;
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
		اگر (!اندر_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/'
			&& (comment_ستون == MAXCOL || (صحیح)pos.ستون < comment_ستون))
		    اندر_بلاک_comment = TRUE;
		ورنہ اگر (اندر_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '/' && linep[pos.ستون] == '*')
		    اندر_بلاک_comment = FALSE;
	    }
	    ورنہ
	    {
		// Guard pos.col < comment_col: don't treat '/*' inside a '//'
		// comment as a block-comment start-marker.
		اگر (!اندر_بلاک_comment && linep[pos.ستون] == '/'
			&& linep[pos.ستون + 1] == '*'
			&& (comment_ستون == MAXCOL || (صحیح)pos.ستون < comment_ستون))
		    اندر_بلاک_comment = TRUE;
		ورنہ اگر (اندر_بلاک_comment && pos.ستون > 0
			&& linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		    اندر_بلاک_comment = FALSE;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	اگر (pos.ستون == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    اگر (linep[0] == findc && گنتی == 0)	// match!
		واپس &pos;
	    توڑو;					// out of scope
	}

	اگر (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    اگر (comment_dir == FORWARD)
	    {
		اگر (linep[pos.ستون] == '*' && linep[pos.ستون + 1] == '/')
		{
		    pos.ستون++;
		    واپس &pos;
		}
	    }
	    ورنہ    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		اگر (pos.ستون == 0)
		    جاری;
		ورنہ اگر (raw_string)
		{
		    اگر (linep[pos.ستون - 1] == 'R'
			&& linep[pos.ستون] == '"'
			&& vim_strchr(linep + pos.ستون + 1, '(') != تہی)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			اگر (!ڈھونڈو_rawstring_انتہا(linep, &pos,
				  گنتی > 0 ? &مطابقت_pos : &curwin->w_cursor))
			{
			    گنتی++;
			    مطابقت_pos = pos;
			    مطابقت_pos.ستون--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		ورنہ اگر (  linep[pos.ستون - 1] == '/'
			&& linep[pos.ستون] == '*'
			&& (pos.ستون == 1 || linep[pos.ستون - 2] != '*')
			&& (صحیح)pos.ستون < comment_ستون)
		{
		    گنتی++;
		    مطابقت_pos = pos;
		    مطابقت_pos.ستون--;
		}
		ورنہ اگر (linep[pos.ستون - 1] == '*' && linep[pos.ستون] == '/')
		{
		    اگر (گنتی > 0)
			pos = مطابقت_pos;
		    ورنہ اگر (pos.ستون > 1 && linep[pos.ستون - 2] == '/'
					       && (صحیح)pos.ستون <= comment_ستون)
			pos.ستون -= 2;
		    ورنہ اگر (ignore_cend)
			جاری;
		    ورنہ
			واپس تہی;
		    واپس &pos;
		}
	    }
	    جاری;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	اگر (cpo_مطابقت)
	    do_quotes = 0;
	ورنہ اگر (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_آغاز = do_quotes;
	    برائے (ptr = linep; *ptr; ++ptr)
	    {
		اگر (ptr == linep + pos.ستون + backwards)
		    at_آغاز = (do_quotes & 1);
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
		    اگر (آغاز_اندر_quotes == MAYBE)
		    {
			inquote = at_آغاز;
			اگر (inquote)
			    آغاز_اندر_quotes = TRUE;
		    }
		    ورنہ اگر (backwards)
			inquote = TRUE;
		}
		اگر (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    اگر (*ptr && *(ptr + ml_get_len(pos.lnum - 1) - 1) == '\\')
		    {
			do_quotes = 1;
			اگر (آغاز_اندر_quotes == MAYBE)
			{
			    inquote = at_آغاز;
			    اگر (inquote)
				آغاز_اندر_quotes = TRUE;
			}
			ورنہ اگر (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	اگر (آغاز_اندر_quotes == MAYBE)
	    آغاز_اندر_quotes = FALSE;

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
	انتخاب (c)
	{
	حالت NUL:
	    // at end of line without trailing backslash, reset inquote
	    اگر (pos.ستون == 0 || linep[pos.ستون - 1] != '\\')
	    {
		inquote = FALSE;
		آغاز_اندر_quotes = FALSE;
	    }
	    توڑو;

	حالت '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    اگر (do_quotes)
	    {
		صحیح ستون;

		برائے (ستون = pos.ستون - 1; ستون >= 0; --ستون)
		    اگر (linep[ستون] != '\\')
			توڑو;
		اگر ((((صحیح)pos.ستون - 1 - ستون) & 1) == 0)
		{
		    inquote = !inquote;
		    آغاز_اندر_quotes = FALSE;
		}
	    }
	    توڑو;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	حالت '\'':
	    اگر (!cpo_مطابقت && initc != '\'' && findc != '\'')
	    {
		اگر (backwards)
		{
		    اگر (pos.ستون > 1)
		    {
			اگر (linep[pos.ستون - 2] == '\'')
			{
			    pos.ستون -= 2;
			    توڑو;
			}
			ورنہ اگر (linep[pos.ستون - 2] == '\\'
				  && pos.ستون > 2 && linep[pos.ستون - 3] == '\'')
			{
			    pos.ستون -= 3;
			    توڑو;
			}
		    }
		}
		ورنہ اگر (linep[pos.ستون + 1])	// forward search
		{
		    اگر (linep[pos.ستون + 1] == '\\'
			   && linep[pos.ستون + 2] && linep[pos.ستون + 3] == '\'')
		    {
			pos.ستون += 3;
			توڑو;
		    }
		    ورنہ اگر (linep[pos.ستون + 2] == '\'')
		    {
			pos.ستون += 2;
			توڑو;
		    }
		}
	    }
	    // FALLTHROUGH

	طےشدہ:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    اگر (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"{}()[]", c) != تہی
		    && pos.ستون > 1
		    && check_prevcol(linep, pos.ستون, '\\', تہی)
		    && check_prevcol(linep, pos.ستون - 1, '#', تہی))
		توڑو;

	    // Skip matches inside comments when FM_SKIPCOMM is set.
	    اگر (skip_comments && (اندر_بلاک_comment
		    || (comment_ستون != MAXCOL && (صحیح)pos.ستون >= comment_ستون)))
		توڑو;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    اگر ((!inquote || آغاز_اندر_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		صحیح	ستون, bslcnt = 0;

		اگر (!cpo_bsl)
		{
		    برائے (ستون = pos.ستون; check_prevcol(linep, ستون, '\\', &ستون);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		اگر (cpo_bsl || (bslcnt & 1) == مطابقت_escaped)
		{
		    اگر (c == initc)
			گنتی++;
		    ورنہ
		    {
			اگر (گنتی == 0)
			    واپس &pos;
			گنتی--;
		    }
		}
	    }
	}
    }

    اگر (comment_dir == BACKWARD && گنتی > 0)
    {
	pos = مطابقت_pos;
	واپس &pos;
    }
    واپس (pos_T *)تہی;	// never found it
}

/*
 * Move cursor briefly to character matching the one under the cursor.
 * Used for Insert mode and "r" command.
 * Show the match only if it is visible on the screen.
 * If there isn't a match, then beep.
 */
    خالی
showmatch(
    صحیح		c)	    // char to show match for
{
    pos_T	*lpos, save_cursor;
    pos_T	mpos;
    colnr_T	vcol;
    طویل	save_so;
    طویل	save_siso;
#اگر_تعریف_شدہ CURSOR_SHAPE
    صحیح		save_state;
#اگر_ختم
    colnr_T	save_dollar_vcol;
    char_u	*p;
    طویل	*so = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    طویل	*siso = curwin->w_p_siso >= 0 ? &curwin->w_p_siso : &p_siso;

    /*
     * Only show match for chars in the 'matchpairs' option.
     */
    // 'matchpairs' is "x:y,x:y"
    برائے (p = curbuf->b_p_mps; *p != NUL; ++p)
    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (PTR2CHAR(p) == c && (curwin->w_p_rl ^ p_ri))
	    توڑو;
#اگر_ختم
	p += mb_ptr2len(p) + 1;
	اگر (PTR2CHAR(p) == c
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		&& !(curwin->w_p_rl ^ p_ri)
#اگر_ختم
	   )
	    توڑو;
	p += mb_ptr2len(p);
	اگر (*p == NUL)
	    واپس;
    }
    اگر (*p == NUL)
	واپس;

    اگر ((lpos = findmatch(تہی, NUL)) == تہی)	    // no match, so beep
    {
	vim_beep(BO_مطابقت);
	واپس;
    }

    اگر (lpos->lnum < curwin->w_topline || lpos->lnum >= curwin->w_botline)
	واپس;

    اگر (!curwin->w_p_wrap)
	getvcol(curwin, lpos, تہی, &vcol, تہی, 0);

    صحیح ستون_visible = (curwin->w_p_wrap
	    || (vcol >= curwin->w_leftcol
		&& vcol < curwin->w_leftcol + curwin->w_چوڑائی));
    اگر (!ستون_visible)
	واپس;

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
#اگر_تعریف_شدہ CURSOR_SHAPE
    save_state = State;
    State = منوال_SHOWMATCH;
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
    curwin->w_cursor = mpos;	// move to matching char
    *so = 0;			// don't use 'scrolloff' here
    *siso = 0;			// don't use 'sidescrolloff' here
    showruler(FALSE);
    setcursor();
    cursor_on();		// make sure that the cursor is shown
    برآمد_flush_cursor(TRUE, FALSE);

    // Restore dollar_vcol(), because setcursor() may call curs_rows()
    // which resets it if the matching position is in a previous line
    // and has a higher column number.
    dollar_vcol = save_dollar_vcol;

    /*
     * brief pause, unless 'm' is present in 'cpo' and a character is
     * available.
     */
    اگر (vim_strchr(p_cpo, CPO_SHOWMATCH) != تہی)
	ui_delay(p_mat * 100L + 8, TRUE);
    ورنہ اگر (!char_avail())
	ui_delay(p_mat * 100L + 9, FALSE);
    curwin->w_cursor = save_cursor;	// restore cursor position
    *so = save_so;
    *siso = save_siso;
#اگر_تعریف_شدہ CURSOR_SHAPE
    State = save_state;
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
}

/*
 * Check if the pattern is zero-width.
 * If move is TRUE, check from the beginning of the buffer, else from position
 * "cur".
 * "direction" is FORWARD or BACKWARD.
 * Returns TRUE, FALSE or -1 for failure.
 */
    ساکن صحیح
is_zero_چوڑائی(
    char_u	*نمونہ,
    size_t	patternlen,
    صحیح		move,
    pos_T	*cur,
    صحیح		direction)
{
    regmmatch_T	regmatch;
    صحیح		nmatched = 0;
    صحیح		نتیجہ = -1;
    pos_T	pos;
    صحیح		called_emsg_before = called_emsg;
    صحیح		flag = 0;

    اگر (نمونہ == تہی)
    {
	نمونہ = spats[آخری_idx].pat;
	patternlen = spats[آخری_idx].patlen;
    }

    اگر (تلاش_regcomp(نمونہ, patternlen, تہی, RE_تلاش, RE_تلاش,
					      تلاش_KEEP, &regmatch) == FAIL)
	واپس -1;

    // init startcol correctly
    regmatch.startpos[0].ستون = -1;
    // move to match
    اگر (move)
    {
	CLEAR_POS(&pos);
    }
    ورنہ
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = تلاش_آغاز;
    }

    اگر (searchit(curwin, curbuf, &pos, تہی, direction, نمونہ, patternlen, 1,
				  تلاش_KEEP + flag, RE_تلاش, تہی) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	کرو
	{
	    regmatch.startpos[0].ستون++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].ستون, تہی);
	    اگر (nmatched != 0)
		توڑو;
	} جب (regmatch.regprog != تہی
		&& (direction == FORWARD ? regmatch.startpos[0].ستون < pos.ستون
				      : regmatch.startpos[0].ستون > pos.ستون));

	اگر (called_emsg == called_emsg_before)
	{
	    نتیجہ = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].ستون == regmatch.endpos[0].ستون);
	}
    }

    vim_regfree(regmatch.regprog);
    واپس نتیجہ;
}


/*
 * Find next search match under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحیح
current_تلاش(
    طویل	گنتی,
    صحیح		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	آغاز_pos;	// start position of the pattern match
    pos_T	انتہا_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    صحیح		i;
    صحیح		dir;
    صحیح		نتیجہ;		// result of various function calls
    char_u	old_p_ws = p_ws;
    صحیح		flags = 0;
    pos_T	save_VIsual = VIsual;
    صحیح		zero_چوڑائی;
    صحیح		skip_پہلا_backward;

    // Correct cursor when 'selection' is exclusive
    اگر (VIsual_فعال && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_پہلا_backward = forward && VIsual_فعال
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    اگر (VIsual_فعال)
    {
	اگر (forward)
	    incl(&pos);
	ورنہ
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_چوڑائی = is_zero_چوڑائی(spats[آخری_idx].pat, spats[آخری_idx].patlen,
						TRUE, &curwin->w_cursor, FORWARD);
    اگر (zero_چوڑائی == -1)
	واپس FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    برائے (i = 0; i < 2; i++)
    {
	اگر (forward)
	{
	    اگر (i == 0 && skip_پہلا_backward)
		جاری;
	    dir = i;
	}
	ورنہ
	    dir = !i;

	flags = 0;
	اگر (!dir && !zero_چوڑائی)
	    flags = تلاش_انتہا;
	انتہا_pos = pos;

	// wrapping should not occur in the first round
	اگر (i == 0)
	    p_ws = FALSE;

	نتیجہ = searchit(curwin, curbuf, &pos, &انتہا_pos,
		(dir ? FORWARD : BACKWARD),
		spats[آخری_idx].pat, spats[آخری_idx].patlen, (طویل) (i ? گنتی : 1),
		تلاش_KEEP | flags, RE_تلاش, تہی);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	اگر (i == 1 && !نتیجہ) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    اگر (VIsual_فعال)
		VIsual = save_VIsual;
	    واپس FAIL;
	}
	ورنہ اگر (i == 0 && !نتیجہ)
	{
	    اگر (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    ورنہ
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_بفر->b_ml.ml_سطر_گنتی;
		pos.ستون  = ml_get_len(curwin->w_بفر->b_ml.ml_سطر_گنتی);
	    }
	}
    }

    آغاز_pos = pos;

    اگر (!VIsual_فعال)
	VIsual = آغاز_pos;

    // put the cursor after the match
    curwin->w_cursor = انتہا_pos;
    اگر (LT_POS(VIsual, انتہا_pos) && forward)
    {
	اگر (skip_پہلا_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	ورنہ
	    // put the cursor on last character of match
	    dec_cursor();
    }
    ورنہ اگر (VIsual_فعال && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_فعال = TRUE;
    VIsual_منوال = 'v';

    اگر (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	اگر (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ورنہ اگر (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (fdo_flags & FDO_تلاش && کلید_Typed)
	foldOpenCursor();
#اگر_ختم

    may_آغاز_select('c');
    setmouse();
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_ستارہ.vmode = NUL;
#اگر_ختم
    redraw_curbuf_later(UPD_INVERTED);
    showmode();

    واپس OK;
}

/*
 * return TRUE if line 'lnum' is empty or has white chars only.
 */
    صحیح
linewhite(linenr_T lnum)
{
    char_u  *p;

    p = skipwhite(ml_get(lnum));
    واپس (*p == NUL);
}

/*
 * Add the search count "[3/19]" to "msgbuf".
 * See update_search_stat() for other arguments.
 */
    ساکن خالی
cmdline_تلاش_stat(
    صحیح		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    صحیح		show_چوٹی_bot_پیغام,
    char_u	*msgbuf,
    size_t	msgbuflen,
    صحیح		recompute,
    صحیح		maxcount,
    طویل	timeout)
{
    searchstat_T stat;

    update_تلاش_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    اگر (stat.cur <= 0)
	واپس;

    حرف	t[تلاش_STAT_BUF_LEN];
    size_t	len;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's')
    {
	اگر (stat.incomplete == 1)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[?/??]");
	ورنہ اگر (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ورنہ اگر (stat.cnt > maxcount)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[>%d/%d]",
		    maxcount, stat.cur);
	ورنہ
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[%d/%d]",
		    stat.cnt, stat.cur);
    }
    ورنہ
#اگر_ختم
    {
	اگر (stat.incomplete == 1)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[?/??]");
	ورنہ اگر (stat.cnt > maxcount && stat.cur > maxcount)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[>%d/>%d]",
		    maxcount, maxcount);
	ورنہ اگر (stat.cnt > maxcount)
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[%d/>%d]",
		    stat.cur, maxcount);
	ورنہ
	    len = vim_snprintf(t, تلاش_STAT_BUF_LEN, "[%d/%d]",
		    stat.cur, stat.cnt);
    }

    اگر (show_چوٹی_bot_پیغام && len + 2 < تلاش_STAT_BUF_LEN)
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
    ساکن خالی
update_تلاش_stat(
    صحیح			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    صحیح			recompute,
    صحیح			maxcount,
    طویل		timeout غیر_مستعمل)
{
    صحیح		    save_ws = p_ws;
    صحیح		    wraparound = FALSE;
    pos_T	    p = (*pos);
    ساکن pos_T    lastpos = {0, 0, 0};
    ساکن صحیح	    cur = 0;
    ساکن صحیح	    cnt = 0;
    ساکن صحیح	    exact_مطابقت = FALSE;
    ساکن صحیح	    incomplete = 0;
    ساکن صحیح	    آخری_maxcount = 0;
    ساکن صحیح	    chgtick = 0;
    ساکن char_u   *lastpat = تہی;
    ساکن size_t   lastpatlen = 0;
    ساکن buf_T    *lbuf = تہی;
#اگر_تعریف_شدہ FEAT_RELTIME
    proftime_T  آغاز;
#اگر_ختم

    CLEAR_اشارہ_گر(stat);

    اگر (dirc == 0 && !recompute && !خالی_خانے_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_مطابقت = exact_مطابقت;
	stat->incomplete = incomplete;
	stat->آخری_maxcount = آخری_maxcount;
	واپس;
    }
    آخری_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    اگر (!(chgtick == CHANGEDTICK(curbuf)
	&& (lastpat != تہی
	    && STRNCMP(lastpat, spats[آخری_idx].pat, lastpatlen) == 0
	    && lastpatlen == spats[آخری_idx].patlen
	)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_مطابقت = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    // when searching backwards and having jumped to the first occurrence,
    // cur must remain greater than 1
    اگر (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 1))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    ورنہ
    {
	صحیح	مکمل_تلاش = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#اگر_تعریف_شدہ FEAT_RELTIME
	اگر (timeout > 0)
	    profile_setlimit(timeout, &آغاز);
#اگر_ختم
	جب (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, تہی, 0, 1, تلاش_KEEP, RE_آخری, تہی) != FAIL)
	{
	    مکمل_تلاش = TRUE;
#اگر_تعریف_شدہ FEAT_RELTIME
	    // Stop after passing the time limit.
	    اگر (timeout > 0 && profile_passed_حد_بندی(&آغاز))
	    {
		incomplete = 1;
		توڑو;
	    }
#اگر_ختم
	    cnt++;
	    اگر (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		اگر (LT_POS(p, endpos))
		    exact_مطابقت = TRUE;
	    }
	    fast_breakcheck();
	    اگر (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		توڑو;
	    }
	}
	اگر (got_int)
	    cur = -1; // abort
	اگر (مکمل_تلاش)
	{
	    vim_free(lastpat);
	    lastpat = vim_strnsave(spats[آخری_idx].pat, spats[آخری_idx].patlen);
	    اگر (lastpat == تہی)
		lastpatlen = 0;
	    ورنہ
		lastpatlen = spats[آخری_idx].patlen;
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_مطابقت = exact_مطابقت;
    stat->incomplete = incomplete;
    stat->آخری_maxcount = آخری_maxcount;
    p_ws = save_ws;
}

#اگر defined(FEAT_ڈھونڈو_ID)

/*
 * Get line "lnum" and copy it into "buf[LSIZE]".
 * The copy is made because the regexp may make the line invalid when using a
 * mark.
 */
    ساکن char_u *
get_سطر_and_copy(linenr_T lnum, char_u *buf)
{
    char_u *سطر = ml_get(lnum);

    vim_strncpy(buf, سطر, LSIZE - 1);
    واپس buf;
}

/*
 * Find identifiers or defines in included files.
 * If p_ic && compl_status_sol() then ptr must be in lowercase.
 */
    خالی
ڈھونڈو_نمونہ_اندر_راستہ(
    char_u	*ptr,		// pointer to search pattern
    صحیح		dir غیر_مستعمل,	// direction of expansion
    صحیح		len,		// length of search pattern
    صحیح		پورا,		// match whole words only
    صحیح		skip_comments,	// don't match inside comments
    صحیح		type,		// Type of search; are we looking for a type?
				// a macro?
    طویل	گنتی,
    صحیح		action,		// What to do when we find it
    linenr_T	آغاز_lnum,	// first line to start searching
    linenr_T	انتہا_lnum,	// last line for searching
    صحیح		forceit,	// If true, always switch to the found path
    صحیح		silent)		// Do not print messages when ACTION_EXPAND
{
    SearchedFile *files;		// Stack of included files
    SearchedFile *bigger;		// When we need more space
    صحیح		max_راستہ_گہرائی = 50;
    طویل	مطابقت_گنتی = 1;

    char_u	*pat;
    char_u	*new_fname;
    char_u	*curr_fname = curbuf->b_fname;
    char_u	*پچھلا_fname = تہی;
    linenr_T	lnum;
    صحیح		گہرائی;
    صحیح		گہرائی_displayed;	// For type==CHECK_PATH
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
    صحیح		ملا = FALSE;
    صحیح		i;
    char_u	*already = تہی;
    char_u	*startp = تہی;
    char_u	*inc_opt = تہی;
# اگر defined(FEAT_QUICKFIX)
    win_T	*curwin_save = تہی;
# endif

    regmatch.regprog = تہی;
    incl_regmatch.regprog = تہی;
    def_regmatch.regprog = تہی;

    file_سطر = alloc(LSIZE);
    اگر (file_سطر == تہی)
	واپس;

    اگر (type != CHECK_راستہ && type != ڈھونڈو_DEFINE
	    // when CONT_SOL is set compare "ptr" with the beginning of the
	    // line is faster than quote_meta/regcomp/regexec "ptr" -- Acevedo
	    && !compl_حالت_sol())
    {
	pat = alloc(len + 5);
	اگر (pat == تہی)
	    جاؤ fpip_انتہا;
	vim_snprintf((حرف *)pat, len + 5, پورا ? "\\<%.*s\\>" : "%.*s", len, ptr);
	// ignore case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	vim_free(pat);
	اگر (regmatch.regprog == تہی)
	    جاؤ fpip_انتہا;
    }
    inc_opt = (*curbuf->b_p_inc == NUL) ? p_inc : curbuf->b_p_inc;
    اگر (*inc_opt != NUL)
    {
	incl_regmatch.regprog = vim_regcomp(inc_opt,
						 magic_isset() ? RE_MAGIC : 0);
	اگر (incl_regmatch.regprog == تہی)
	    جاؤ fpip_انتہا;
	incl_regmatch.rm_ic = FALSE;	// don't ignore case in incl. pat.
    }
    اگر (type == ڈھونڈو_DEFINE && (*curbuf->b_p_def != NUL || *p_def != NUL))
    {
	def_regmatch.regprog = vim_regcomp(*curbuf->b_p_def == NUL ? p_def : curbuf->b_p_def,
						 magic_isset() ? RE_MAGIC : 0);
	اگر (def_regmatch.regprog == تہی)
	    جاؤ fpip_انتہا;
	def_regmatch.rm_ic = FALSE;	// don't ignore case in define pat.
    }
    files = lalloc_clear(max_راستہ_گہرائی * حجم(SearchedFile), TRUE);
    اگر (files == تہی)
	جاؤ fpip_انتہا;
    old_files = max_راستہ_گہرائی;
    گہرائی = گہرائی_displayed = -1;

    lnum = آغاز_lnum;
    اگر (انتہا_lnum > curbuf->b_ml.ml_سطر_گنتی)
	انتہا_lnum = curbuf->b_ml.ml_سطر_گنتی;
    اگر (lnum > انتہا_lnum)		// do at least one line
	lnum = انتہا_lnum;
    سطر = get_سطر_and_copy(lnum, file_سطر);

    برائے (;;)
    {
	اگر (incl_regmatch.regprog != تہی
		&& vim_regexec(&incl_regmatch, سطر, (colnr_T)0))
	{
	    char_u *p_fname = (curr_fname == curbuf->b_fname)
					      ? curbuf->b_ffname : curr_fname;

	    اگر (لڑی_تلاش((حرف *)inc_opt, "\\zs") != تہی)
		// Use text from '\zs' to '\ze' (or end) of 'include'.
		new_fname = ڈھونڈو_file_name_اندر_راستہ(incl_regmatch.startp[0],
		       (صحیح)(incl_regmatch.endp[0] - incl_regmatch.startp[0]),
				 FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname);
	    ورنہ
		// Use text after match with 'include'.
		new_fname = file_name_اندر_سطر(incl_regmatch.endp[0], 0,
			     FNAME_EXP|FNAME_INCL|FNAME_REL, 1L, p_fname, تہی);
	    already_searched = FALSE;
	    اگر (new_fname != تہی)
	    {
		// Check whether we have already searched in this file
		برائے (i = 0;; i++)
		{
		    اگر (i == گہرائی + 1)
			i = old_files;
		    اگر (i == max_راستہ_گہرائی)
			توڑو;
		    اگر (fullpathcmp(new_fname, files[i].name, TRUE, TRUE)
								    & FPC_SAME)
		    {
			اگر (type != CHECK_راستہ
				&& action == ACTION_SHOW_ALL
				&& files[i].matched)
			{
			    پیغام_putchar('\n');	    // cursor below last one
			    اگر (!got_int)	    // don't display if 'q'
						    // typed at "--more--"
						    //پیغام
			    {
				پیغام_home_replace_hl(new_fname);
				پیغام_puts(_(" (includes previously listed match)"));
				پچھلا_fname = تہی;
			    }
			}
			VIM_CLEAR(new_fname);
			already_searched = TRUE;
			توڑو;
		    }
		}
	    }

	    اگر (type == CHECK_راستہ && (action == ACTION_SHOW_ALL
				 || (new_fname == تہی && !already_searched)))
	    {
		اگر (did_show)
		    پیغام_putchar('\n');	    // cursor below last one
		ورنہ
		{
		    gotocmdline(TRUE);	    // cursor at status line
		    پیغام_puts_title(_("--- Included files "));
		    اگر (action != ACTION_SHOW_ALL)
			پیغام_puts_title(_("not found "));
		    پیغام_puts_title(_("in path ---\n"));
		}
		did_show = TRUE;
		جب (گہرائی_displayed < گہرائی && !got_int)
		{
		    ++گہرائی_displayed;
		    برائے (i = 0; i < گہرائی_displayed; i++)
			پیغام_puts("  ");
		    پیغام_home_replace(files[گہرائی_displayed].name);
		    پیغام_puts(" -->\n");
		}
		اگر (!got_int)		    // don't display if 'q' typed
					    // for "--more--" message
		{
		    برائے (i = 0; i <= گہرائی_displayed; i++)
			پیغام_puts("  ");
		    اگر (new_fname != تہی)
		    {
			// using "new_fname" is more reliable, e.g., when
			// 'includeexpr' is set.
			پیغام_outtrans_attr(new_fname, HL_ATTR(HLF_D));
		    }
		    ورنہ
		    {
			/*
			 * Isolate the file name.
			 * Include the surrounding "" or <> if present.
			 */
			اگر (لڑی_تلاش((حرف *)inc_opt, "\\zs") != تہی)
			{
			    // pattern contains \zs, use the match
			    p = incl_regmatch.startp[0];
			    i = (صحیح)(incl_regmatch.endp[0]
						   - incl_regmatch.startp[0]);
			}
			ورنہ
			{
			    // find the file name after the end of the match
			    برائے (p = incl_regmatch.endp[0];
						  *p && !vim_isfilec(*p); p++)
				;
			    برائے (i = 0; vim_isfilec(p[i]); i++)
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
			ورنہ اگر (p > سطر)
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
			پیغام_outtrans_attr(p, HL_ATTR(HLF_D));
			p[i] = save_char;
		    }

		    اگر (new_fname == تہی && action == ACTION_SHOW_ALL)
		    {
			اگر (already_searched)
			    پیغام_puts(_("  (Already listed)"));
			ورنہ
			    پیغام_puts(_("  NOT FOUND"));
		    }
		}
		برآمد_flush();	    // output each line directly
	    }

	    اگر (new_fname != تہی)
	    {
		// Push the new file onto the file stack
		اگر (گہرائی + 1 == old_files)
		{
		    bigger = ALLOC_MULT(SearchedFile, max_راستہ_گہرائی * 2);
		    اگر (bigger != تہی)
		    {
			برائے (i = 0; i <= گہرائی; i++)
			    bigger[i] = files[i];
			برائے (i = گہرائی + 1; i < old_files + max_راستہ_گہرائی; i++)
			{
			    bigger[i].fp = تہی;
			    bigger[i].name = تہی;
			    bigger[i].lnum = 0;
			    bigger[i].matched = FALSE;
			}
			برائے (i = old_files; i < max_راستہ_گہرائی; i++)
			    bigger[i + max_راستہ_گہرائی] = files[i];
			old_files += max_راستہ_گہرائی;
			max_راستہ_گہرائی *= 2;
			vim_free(files);
			files = bigger;
		    }
		}
		اگر ((files[گہرائی + 1].fp = mch_fopen((حرف *)new_fname, "r"))
								    == تہی)
		    vim_free(new_fname);
		ورنہ
		{
		    اگر (++گہرائی == old_files)
		    {
			/*
			 * lalloc() for 'bigger' must have failed above.  We
			 * will forget one of our already visited files now.
			 */
			vim_free(files[old_files].name);
			++old_files;
		    }
		    files[گہرائی].name = curr_fname = new_fname;
		    files[گہرائی].lnum = 0;
		    files[گہرائی].matched = FALSE;
		    اگر (action == ACTION_EXPAND
			    && !shortmess(SHM_COMPLETIONSCAN) && !silent)
		    {
			پیغام_hist_off = TRUE;	// reset in msg_trunc_attr()
			vim_snprintf((حرف*)IObuff, IOSIZE,
				_("Scanning included file: %s"),
				(حرف *)new_fname);
			پیغام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
		    }
		    ورنہ اگر (p_verbose >= 5)
		    {
			verbose_enter();
			smsg(_("Searching included file %s"),
							   (حرف *)new_fname);
			verbose_leave();
		    }

		}
	    }
	}
	ورنہ
	{
	    /*
	     * Check if the line is a define (type == FIND_DEFINE)
	     */
	    p = سطر;
تلاش_سطر:
	    define_matched = FALSE;
	    اگر (def_regmatch.regprog != تہی
			      && vim_regexec(&def_regmatch, سطر, (colnr_T)0))
	    {
		/*
		 * Pattern must be first identifier after 'define', so skip
		 * to that position before checking for match of pattern.  Also
		 * don't let it match beyond the end of this identifier.
		 */
		p = def_regmatch.endp[0];
		جب (*p && !vim_iswordc(*p))
		    p++;
		define_matched = TRUE;
	    }

	    /*
	     * Look for a match.  Don't do this if we are looking for a
	     * define and this line didn't match define_prog above.
	     */
	    اگر (def_regmatch.regprog == تہی || define_matched)
	    {
		اگر (define_matched || compl_حالت_sol())
		{
		    // compare the first "len" chars from "ptr"
		    startp = skipwhite(p);
		    اگر (p_ic)
			matched = MB_STRNICMP(startp, ptr, len) == 0;
		    ورنہ
			matched = STRNCMP(startp, ptr, len) == 0;
		    اگر (matched && define_matched && پورا
						  && vim_iswordc(startp[len]))
			matched = FALSE;
		}
		ورنہ اگر (regmatch.regprog != تہی
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
				&& get_leader_len(سطر, تہی, FALSE, TRUE))
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
			    برائے (p = سطر; *p && p < startp; ++p)
			    {
				اگر (matched
					&& p[0] == '/'
					&& (p[1] == '*' || p[1] == '/'))
				{
				    matched = FALSE;
				    // After "//" all text is comment
				    اگر (p[1] == '/')
					توڑو;
				    ++p;
				}
				ورنہ اگر (!matched && p[0] == '*' && p[1] == '/')
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
		صحیح	جوڑو_r;
		char_u	*aux;

		اگر (گہرائی == -1 && lnum == curwin->w_cursor.lnum)
		    توڑو;
		ملا = TRUE;
		aux = p = startp;
		اگر (compl_حالت_adding() && (صحیح)STRLEN(p) >= ins_compl_len())
		{
		    p += ins_compl_len();
		    اگر (vim_iswordp(p))
			جاؤ exit_matched;
		    p = ڈھونڈو_لفظ_آغاز(p);
		}
		p = ڈھونڈو_لفظ_انتہا(p);
		i = (صحیح)(p - aux);

		اگر (compl_حالت_adding() && i == ins_compl_len())
		{
		    // IOSIZE > compl_length, so the STRNCPY works
		    STRNCPY(IObuff, aux, i);

		    // Get the next line: when "depth" < 0  from the current
		    // buffer, otherwise from the included file.  Jump to
		    // exit_matched when past the last line.
		    اگر (گہرائی < 0)
		    {
			اگر (lnum >= انتہا_lnum)
			    جاؤ exit_matched;
			سطر = get_سطر_and_copy(++lnum, file_سطر);
		    }
		    ورنہ اگر (vim_fgets(سطر = file_سطر,
						      LSIZE, files[گہرائی].fp))
			جاؤ exit_matched;

		    // we read a line, set "already" to check this "line" later
		    // if depth >= 0 we'll increase files[depth].lnum far
		    // below  -- Acevedo
		    already = aux = p = skipwhite(سطر);
		    p = ڈھونڈو_لفظ_آغاز(p);
		    p = ڈھونڈو_لفظ_انتہا(p);
		    اگر (p > aux)
		    {
			اگر (*aux != ')' && IObuff[i-1] != TAB)
			{
			    اگر (IObuff[i-1] != ' ')
				IObuff[i++] = ' ';
			    // IObuf =~ "\(\k\|\i\).* ", thus i >= 2
			    اگر (p_js
				&& (IObuff[i-2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP) == تہی
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
			جاؤ exit_matched;
		}

		جوڑو_r = ins_compl_جوڑو_infercase(aux, i, p_ic,
			curr_fname == curbuf->b_fname ? تہی
						  : shorten_fname1(curr_fname),
			dir, cont_s_ipos, 0);
		اگر (جوڑو_r == OK)
		    // if dir was BACKWARD then honor it just once
		    dir = FORWARD;
		ورنہ اگر (جوڑو_r == FAIL)
		    توڑو;
	    }
	    ورنہ اگر (action == ACTION_SHOW_ALL)
	    {
		ملا = TRUE;
		اگر (!did_show)
		    gotocmdline(TRUE);		// cursor at status line
		اگر (curr_fname != پچھلا_fname)
		{
		    اگر (did_show)
			پیغام_putchar('\n');	// cursor below last one
		    اگر (!got_int)		// don't display if 'q' typed
						// at "--more--" message
			پیغام_home_replace_hl(curr_fname);
		    پچھلا_fname = curr_fname;
		}
		did_show = TRUE;
		اگر (!got_int)
		    show_pat_اندر_راستہ(سطر, type, TRUE, action,
			    (گہرائی == -1) ? تہی : files[گہرائی].fp,
			    (گہرائی == -1) ? &lnum : &files[گہرائی].lnum,
			    مطابقت_گنتی++);

		// Set matched flag for this file and all the ones that
		// include it
		برائے (i = 0; i <= گہرائی; ++i)
		    files[i].matched = TRUE;
	    }
	    ورنہ اگر (--گنتی <= 0)
	    {
		ملا = TRUE;
		اگر (گہرائی == -1 && lnum == curwin->w_cursor.lnum
# اگر defined(FEAT_QUICKFIX)
						      && g_do_tagpreview == 0
# endif
						      )
		    emsg(_(e_مطابقت_is_on_current_سطر));
		ورنہ اگر (action == ACTION_SHOW)
		{
		    show_pat_اندر_راستہ(سطر, type, did_show, action,
			    (گہرائی == -1) ? تہی : files[گہرائی].fp,
			    (گہرائی == -1) ? &lnum : &files[گہرائی].lnum, 1L);
		    did_show = TRUE;
		}
		ورنہ
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
		    اگر (action == ACTION_بانٹو)
		    {
			اگر (win_بانٹو(0, 0) == FAIL)
			    توڑو;
			RESET_BINDING(curwin);
		    }
		    اگر (گہرائی == -1)
		    {
			// match in current file
# اگر defined(FEAT_QUICKFIX)
			اگر (g_do_tagpreview != 0)
			{
			    اگر (!win_valid(curwin_save))
				توڑو;
			    اگر (!GETFILE_SUCCESS(getfile(
					   curwin_save->w_بفر->b_fnum, تہی,
						     تہی, TRUE, lnum, forceit)))
				توڑو;	// failed to jump to file
			}
			ورنہ
# endif
			    setpcmark();
			curwin->w_cursor.lnum = lnum;
			check_cursor();
		    }
		    ورنہ
		    {
			اگر (!GETFILE_SUCCESS(getfile(
					0, files[گہرائی].name, تہی, TRUE,
						    files[گہرائی].lnum, forceit)))
			    توڑو;	// failed to jump to file
			// autocommands may have changed the lnum, we don't
			// want that here
			curwin->w_cursor.lnum = files[گہرائی].lnum;
		    }
		}
		اگر (action != ACTION_SHOW)
		{
		    curwin->w_cursor.ستون = (colnr_T)(startp - سطر);
		    curwin->w_سیٹ_curswant = true;
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
		ورنہ اگر (WIN_IS_POPUP(curwin))
		    // can't keep focus in popup window
		    win_enter(firstwin, TRUE);
#  endif
# endif
		توڑو;
	    }
exit_matched:
	    matched = FALSE;
	    // look for other matches in the rest of the line if we
	    // are not at the end of it already
	    اگر (def_regmatch.regprog == تہی
		    && action == ACTION_EXPAND
		    && !compl_حالت_sol()
		    && *startp != NUL
		    && *(startp + mb_ptr2len(startp)) != NUL)
		جاؤ تلاش_سطر;
	}
	سطر_breakcheck();
	اگر (action == ACTION_EXPAND)
	    ins_compl_check_کلیدیں(30, FALSE);
	اگر (got_int || ins_compl_interrupted())
	    توڑو;

	/*
	 * Read the next line.  When reading an included file and encountering
	 * end-of-file, close the file and continue in the file that included
	 * it.
	 */
	جب (گہرائی >= 0 && !already
		&& vim_fgets(سطر = file_سطر, LSIZE, files[گہرائی].fp))
	{
	    fclose(files[گہرائی].fp);
	    --old_files;
	    files[old_files].name = files[گہرائی].name;
	    files[old_files].matched = files[گہرائی].matched;
	    --گہرائی;
	    curr_fname = (گہرائی == -1) ? curbuf->b_fname
				       : files[گہرائی].name;
	    اگر (گہرائی < گہرائی_displayed)
		گہرائی_displayed = گہرائی;
	}
	اگر (گہرائی >= 0)		// we could read the line
	{
	    files[گہرائی].lnum++;
	    // Remove any CR and LF from the line.
	    i = (صحیح)STRLEN(سطر);
	    اگر (i > 0 && سطر[i - 1] == '\n')
		سطر[--i] = NUL;
	    اگر (i > 0 && سطر[i - 1] == '\r')
		سطر[--i] = NUL;
	}
	ورنہ اگر (!already)
	{
	    اگر (++lnum > انتہا_lnum)
		توڑو;
	    سطر = get_سطر_and_copy(lnum, file_سطر);
	}
	already = تہی;
    }
    // End of big for (;;) loop.

    // Close any files that are still open.
    برائے (i = 0; i <= گہرائی; i++)
    {
	fclose(files[i].fp);
	vim_free(files[i].name);
    }
    برائے (i = old_files; i < max_راستہ_گہرائی; i++)
	vim_free(files[i].name);
    vim_free(files);

    اگر (type == CHECK_راستہ)
    {
	اگر (!did_show)
	{
	    اگر (action != ACTION_SHOW_ALL)
		پیغام(_("All included files were found"));
	    ورنہ
		پیغام(_("No included files"));
	}
    }
    ورنہ اگر (!ملا && action != ACTION_EXPAND && !silent)
    {
	اگر (got_int || ins_compl_interrupted())
	    emsg(_(e_interrupted));
	ورنہ اگر (type == ڈھونڈو_DEFINE)
	    emsg(_(e_couldnt_ڈھونڈو_definition));
	ورنہ
	    emsg(_(e_couldnt_ڈھونڈو_نمونہ));
    }
    اگر (action == ACTION_SHOW || action == ACTION_SHOW_ALL)
	پیغام_انتہا();

fpip_انتہا:
    vim_free(file_سطر);
    vim_regfree(regmatch.regprog);
    vim_regfree(incl_regmatch.regprog);
    vim_regfree(def_regmatch.regprog);
}

    ساکن خالی
show_pat_اندر_راستہ(
    char_u  *سطر,
    صحیح	    type,
    صحیح	    did_show,
    صحیح	    action,
    FILE    *fp,
    linenr_T *lnum,
    طویل    گنتی)
{
    char_u  *p;
    size_t  linelen;

    اگر (did_show)
	پیغام_putchar('\n');	// cursor below last one
    ورنہ اگر (!پیغام_silent)
	gotocmdline(TRUE);	// cursor at status line
    اگر (got_int)		// 'q' typed at "--more--" message
	واپس;
    linelen = STRLEN(سطر);
    برائے (;;)
    {
	p = سطر + linelen - 1;
	اگر (fp != تہی)
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
	    sprintf((حرف *)IObuff, "%3ld: ", گنتی);	// show match nr
	    پیغام_puts((حرف *)IObuff);
	    sprintf((حرف *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    پیغام_puts_attr((حرف *)IObuff, HL_ATTR(HLF_N));
	    پیغام_puts(" ");
	}
	پیغام_prt_سطر(سطر, FALSE);
	برآمد_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	اگر (got_int || type != ڈھونڈو_DEFINE || p < سطر || *p != '\\')
	    توڑو;

	اگر (fp != تہی)
	{
	    اگر (vim_fgets(سطر, LSIZE, fp)) // end of file
		توڑو;
	    linelen = STRLEN(سطر);
	    ++*lnum;
	}
	ورنہ
	{
	    اگر (++*lnum > curbuf->b_ml.ml_سطر_گنتی)
		توڑو;
	    سطر = ml_get(*lnum);
	    linelen = ml_get_len(*lnum);
	}
	پیغام_putchar('\n');
    }
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_VIMINFO
/*
 * Return the last used search pattern at "idx".
 */
    spat_T *
get_spat(صحیح idx)
{
    واپس &spats[idx];
}

/*
 * Return the last used search pattern index.
 */
    صحیح
get_spat_آخری_idx(خالی)
{
    واپس آخری_idx;
}
#اگر_ختم

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * "searchcount()" function
 */
    خالی
f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*نمونہ = تہی;
    صحیح			maxcount = p_msc;
    طویل		timeout = تلاش_STAT_DEF_TIMEOUT;
    صحیح			recompute = TRUE;
    searchstat_T	stat;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	صحیح		error = FALSE;

	اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	    واپس;
	dict = argvars[0].vval.v_dict;
	di = dict_ڈھونڈو(dict, (char_u *)"timeout", -1);
	اگر (di != تہی)
	{
	    timeout = (طویل)tv_get_نمبر_chk(&di->di_tv, &error);
	    اگر (error)
		واپس;
	}
	di = dict_ڈھونڈو(dict, (char_u *)"maxcount", -1);
	اگر (di != تہی)
	{
	    maxcount = (صحیح)tv_get_نمبر_chk(&di->di_tv, &error);
	    اگر (error)
		واپس;
	}
	recompute = dict_get_bool(dict, "recompute", recompute);
	di = dict_ڈھونڈو(dict, (char_u *)"pattern", -1);
	اگر (di != تہی)
	{
	    نمونہ = tv_get_string_chk(&di->di_tv);
	    اگر (نمونہ == تہی)
		واپس;
	}
	di = dict_ڈھونڈو(dict, (char_u *)"pos", -1);
	اگر (di != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_دلیل_str), "pos");
		واپس;
	    }
	    اگر (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_دلیل_str), "List format should be [lnum, col, off]");
		واپس;
	    }
	    li = list_ڈھونڈو(di->di_tv.vval.v_list, 0L);
	    اگر (li != تہی)
	    {
		pos.lnum = tv_get_نمبر_chk(&li->li_tv, &error);
		اگر (error)
		    واپس;
	    }
	    li = list_ڈھونڈو(di->di_tv.vval.v_list, 1L);
	    اگر (li != تہی)
	    {
		pos.ستون = tv_get_نمبر_chk(&li->li_tv, &error) - 1;
		اگر (error)
		    واپس;
	    }
	    li = list_ڈھونڈو(di->di_tv.vval.v_list, 2L);
	    اگر (li != تہی)
	    {
		pos.coladd = tv_get_نمبر_chk(&li->li_tv, &error);
		اگر (error)
		    واپس;
	    }
	}
    }

    save_آخری_تلاش_نمونہ();
# ifdef FEAT_تلاش_EXTRA
    save_incsearch_state();
# endif
    اگر (نمونہ != تہی)
    {
	اگر (*نمونہ == NUL)
	    جاؤ the_انتہا;
	vim_free(spats[آخری_idx].pat);
	spats[آخری_idx].patlen = STRLEN(نمونہ);
	spats[آخری_idx].pat = vim_strnsave(نمونہ, spats[آخری_idx].patlen);
	اگر (spats[آخری_idx].pat == تہی)
	    spats[آخری_idx].patlen = 0;
    }
    اگر (spats[آخری_idx].pat == تہی || *spats[آخری_idx].pat == NUL)
	جاؤ the_انتہا;	// the previous pattern was never defined

    update_تلاش_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_جوڑو_نمبر(rettv->vval.v_dict, "current", stat.cur);
    dict_جوڑو_نمبر(rettv->vval.v_dict, "total", stat.cnt);
    dict_جوڑو_نمبر(rettv->vval.v_dict, "exact_match", stat.exact_مطابقت);
    dict_جوڑو_نمبر(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_جوڑو_نمبر(rettv->vval.v_dict, "maxcount", stat.آخری_maxcount);

the_انتہا:
    restore_آخری_تلاش_نمونہ();
# ifdef FEAT_تلاش_EXTRA
    restore_incsearch_state();
# endif
}
#اگر_ختم
