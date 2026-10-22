/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 *
 * fuzzy.c: fuzzy matching algorithm and related functions
 *
 * Portions of this file are adapted from fzy (https://github.com/jhawthorn/fzy)
 * Original code:
 *   Copyright (c) 2014 John Hawthorn
 *   Licensed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#تعریف_کړه USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساكن صحيح fuzzy_سمون_توکی_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_سمون_دننه_list(list_T *l, char_u *str, صحيح matchseq, char_u *کلی, callback_T *توکی_cb, صحيح retmatchpos, list_T *fmatchlist, اوږد max_matches);
ساكن فراغ do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos);
#د_که_پای
ساكن صحيح fuzzy_سمون_str_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_سمون_str_ترتیبول(fuzmatch_str_T *fm, صحيح sz);
ساكن صحيح fuzzy_سمون_func_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_سمون_func_ترتیبول(fuzmatch_str_T *fm, صحيح sz);

تعريفنوع مزدوج score_t;
ساكن score_t سمون_positions(char_u *needle, char_u *haystack, int_u *positions);
ساكن صحيح has_سمون(char_u *needle, char_u *haystack);

#تعریف_کړه SCORE_MAX INFINITY
#تعریف_کړه SCORE_MIN (-INFINITY)
#تعریف_کړه SCORE_SCALE 1000

تعريفنوع بنية
{
    صحيح		idx;		// used for stable sort
    listitem_T	*توکی;
    صحيح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحيح		itemstr_allocated;
    صحيح		startpos;
} fuzzy_توکی_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحيح
fuzzy_سمون(
    char_u	*str,
    char_u	*pat_arg,
    صحيح		matchseq,
    صحيح		*بهر_Score,
    int_u	*matches,
    صحيح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحيح		complete = FALSE;
    صحيح		score = 0;
    صحيح		عدد_Matches = 0;
    صحيح		pat_chars = 0;
    score_t	fzy_score;

    *بهر_Score = 0;

    save_pat = vim_strsave(pat_arg);
    إذا (save_pat == عدم)
	أرجع FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    طالما (TRUE)
    {
	إذا (matchseq)
	    complete = TRUE;
	وإلا
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    إذا (*p == NUL)
		اكسر;
	    pat = p;
	    طالما (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		إذا (has_mbyte)
		    MB_PTR_ADV(p);
		وإلا
		    ++p;
	    }
	    إذا (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	إذا (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	إذا (عدد_Matches > maxMatches - pat_chars)
	{
	    عدد_Matches = 0;
	    *بهر_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	score = FUZZY_SCORE_NONE;
	إذا (has_سمون(pat, str))
	{
	    fzy_score = سمون_positions(pat, str, matches + عدد_Matches);
	    إذا (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? لوی_عدد
		    : (fzy_score < 0) ? (صحيح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحيح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	إذا (score == FUZZY_SCORE_NONE)
	{
	    عدد_Matches = 0;
	    *بهر_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	إذا (score > 0 && *بهر_Score > لوی_عدد - score)
	    *بهر_Score = لوی_عدد;
	وإلا إذا (score < 0 && *بهر_Score < INT_MIN + 1 - score)
	    *بهر_Score = INT_MIN + 1;
	وإلا
	    *بهر_Score += score;

	عدد_Matches += pat_chars;

	إذا (complete || عدد_Matches >= maxMatches)
	    اكسر;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    أرجع عدد_Matches != 0;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساكن صحيح
fuzzy_سمون_توکی_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzzy_توکی_T *)s1)->score;
    صحيح		v2 = ((fuzzy_توکی_T *)s2)->score;

    إذا (v1 == v2)
    {
	صحيح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_توکی_T *)s1)->pat;
	صحيح patlen = (صحيح)STRLEN(pat);
	صحيح startpos = ((fuzzy_توکی_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_توکی_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_توکی_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_توکی_T *)s2)->itemstr + startpos, patlen) == 0;

	إذا (exact_match1 == exact_match2)
	{
	    صحيح idx1 = ((fuzzy_توکی_T *)s1)->idx;
	    صحيح idx2 = ((fuzzy_توکی_T *)s2)->idx;
	    أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	وإلا إذا (exact_match2)
	    أرجع 1;
	أرجع -1;
    }
    وإلا
	أرجع v1 > v2 ? -1 : 1;
}

/*
 * Fuzzy search the string 'str' in a list of 'items' and return the matching
 * strings in 'fmatchlist'.
 * If 'matchseq' is TRUE, then for multi-word search strings, match all the
 * words in sequence.
 * If 'items' is a list of strings, then search for 'str' in the list.
 * If 'items' is a list of dicts, then either use 'key' to lookup the string
 * for each item or use 'item_cb' Funcref function to get the string.
 * If 'retmatchpos' is TRUE, then return a list of positions where 'str'
 * matches for each item.
 */
    ساكن فراغ
fuzzy_سمون_دننه_list(
    list_T	*l,
    char_u	*str,
    صحيح		matchseq,
    char_u	*کلی,
    callback_T	*توکی_cb,
    صحيح		retmatchpos,
    list_T	*fmatchlist,
    اوږد	max_matches)
{
    اوږد	    len;
    fuzzy_توکی_T	    *توکي = عدم;
    listitem_T	    *li;
    اوږد	    سمون_شمېر = 0;
    int_u	    matches[FUZZY_سمون_MAX_LEN];

    len = list_len(l);
    إذا (len == 0)
	أرجع;
    إذا (max_matches > 0 && len > max_matches)
	len = max_matches;

    توکي = ALLOC_CLEAR_MULT(fuzzy_توکی_T, len);
    إذا (توکي == عدم)
	أرجع;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_توکي(l, li)
    {
	صحيح		score;
	char_u		*itemstr = عدم;
	char_u		*itemstr_copy = عدم;
	typval_T	rettv;
	صحيح		itemstr_allocate = FALSE;
	list_T		*سمون_positions = عدم;

	إذا (max_matches > 0 && سمون_شمېر >= max_matches)
	    اكسر;

	rettv.v_type = VAR_UNKNOWN;
	إذا (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	وإلا إذا (li->li_tv.v_type == VAR_DICT
				&& (کلی != عدم || توکی_cb->cb_name != عدم))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    إذا (کلی != عدم)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (حرف *)کلی, FALSE);
	    وإلا
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		إذا (call_callback(توکی_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    إذا (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	إذا (itemstr != عدم
		&& fuzzy_سمون(itemstr, str, matchseq, &score, matches,
						FUZZY_سمون_MAX_LEN))
	{
	    إذا (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		إذا (itemstr_copy == عدم)
		{
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    وإلا
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    إذا (retmatchpos)
	    {
		سمون_positions = list_alloc();
		إذا (سمون_positions == عدم)
		{
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}

		// Fill position information
		صحيح	j = 0;
		char_u	*p = str;
		صحيح	success = TRUE;

		طالما (*p != NUL && j < FUZZY_سمون_MAX_LEN && success)
		{
		    إذا (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			إذا (list_append_شمېره(سمون_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    اكسر;
			}
			j++;
		    }
		    إذا (has_mbyte)
			MB_PTR_ADV(p);
		    وإلا
			++p;
		}

		إذا (!success)
		{
		    list_free(سمون_positions);
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    توکي[سمون_شمېر].idx = سمون_شمېر;
	    توکي[سمون_شمېر].توکی = li;
	    توکي[سمون_شمېر].score = score;
	    توکي[سمون_شمېر].pat = str;
	    توکي[سمون_شمېر].startpos = matches[0];
	    توکي[سمون_شمېر].itemstr = itemstr_copy;
	    توکي[سمون_شمېر].itemstr_allocated = itemstr_allocate;
	    توکي[سمون_شمېر].lmatchpos = سمون_positions;

	    ++سمون_شمېر;
	}
	clear_tv(&rettv);
    }

    إذا (سمون_شمېر > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((فراغ *)توکي, (size_t)سمون_شمېر, حجم(fuzzy_توکی_T),
		fuzzy_سمون_توکی_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	إذا (retmatchpos)
	{
	    li = list_پیدا_کړه(fmatchlist, 0);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب بشپړ;
	    retlist = li->li_tv.vval.v_list;
	}
	وإلا
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	لكل (صحيح i = 0; i < سمون_شمېر; i++)
	{
	    إذا (list_append_tv(retlist, &توکي[i].توکی->li_tv) == FAIL)
		اذهب بشپړ;
	}

	// next copy the list of matching positions
	إذا (retmatchpos)
	{
	    li = list_پیدا_کړه(fmatchlist, -2);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب بشپړ;
	    retlist = li->li_tv.vval.v_list;

	    لكل (صحيح i = 0; i < سمون_شمېر; i++)
	    {
		إذا (توکي[i].lmatchpos != عدم)
		{
		    إذا (list_append_list(retlist, توکي[i].lmatchpos) == OK)
			توکي[i].lmatchpos = عدم;
		    وإلا
			اذهب بشپړ;

		}
	    }

	    // copy the matching scores
	    li = list_پیدا_کړه(fmatchlist, -1);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب بشپړ;
	    retlist = li->li_tv.vval.v_list;
	    لكل (صحيح i = 0; i < سمون_شمېر; i++)
	    {
		إذا (list_append_شمېره(retlist, توکي[i].score) == FAIL)
		    اذهب بشپړ;
	    }
	}
    }

بشپړ:
    لكل (صحيح i = 0; i < سمون_شمېر; i++)
    {
	إذا (توکي[i].itemstr_allocated)
	    vim_free(توکي[i].itemstr);

	إذا (توکي[i].lmatchpos)
	    list_free(توکي[i].lmatchpos);
    }
    vim_free(توکي);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ساكن فراغ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos)
{
    callback_T	cb;
    char_u	*کلی = عدم;
    صحيح		ret;
    صحيح		matchseq = FALSE;
    اوږد	max_matches = 0;

    إذا (دننه_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    CLEAR_اشاره_ګر(&cb);

    // validate and get the arguments
    إذا (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_دلیل_د_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	أرجع;
    }
    إذا (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == عدم)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	أرجع;
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	إذا (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    أرجع;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	إذا ((di = dict_پیدا_کړه(d, (char_u *)"key", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == عدم
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "key",
						    tv_get_string(&di->di_tv));
		أرجع;
	    }
	    کلی = tv_get_string(&di->di_tv);
	}
	وإلا إذا ((di = dict_پیدا_کړه(d, (char_u *)"text_cb", -1)) != عدم)
	{
	    cb = get_callback(&di->di_tv);
	    إذا (cb.cb_name == عدم)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str), "text_cb");
		أرجع;
	    }
	}

	إذا ((di = dict_پیدا_کړه(d, (char_u *)"limit", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_شمېره)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str), "limit");
		أرجع;
	    }
	    max_matches = (اوږد)tv_get_شمېره_chk(&di->di_tv, عدم);
	}

	إذا (dict_has_کلی(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    إذا (ret == FAIL)
	اذهب بشپړ;
    إذا (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	إذا (l == عدم)
	    اذهب بشپړ;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب بشپړ;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب بشپړ;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب بشپړ;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب بشپړ;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب بشپړ;
	}
    }

    fuzzy_سمون_دننه_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلی, &cb, retmatchpos, rettv->vval.v_list, max_matches);

بشپړ:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    فراغ
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    فراغ
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#د_که_پای

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ساكن صحيح
fuzzy_سمون_str_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحيح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحيح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحيح		idx2 = ((fuzmatch_str_T *)s2)->idx;

    إذا (v1 == v2)
	أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وإلا
	أرجع v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ساكن فراغ
fuzzy_سمون_str_ترتیبول(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
	    fuzzy_سمون_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ساكن صحيح
fuzzy_سمون_func_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحيح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحيح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحيح		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    إذا (*str1 != '<' && *str2 == '<')
	أرجع -1;
    إذا (*str1 == '<' && *str2 != '<')
	أرجع 1;
    إذا (v1 == v2)
	أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وإلا
	أرجع v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ساكن فراغ
fuzzy_سمون_func_ترتیبول(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
		fuzzy_سمون_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحيح
fuzzy_سمون_str(char_u *str, char_u *pat)
{
    صحيح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_سمون_MAX_LEN];

    إذا (str == عدم || pat == عدم)
	أرجع score;

    fuzzy_سمون(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    أرجع score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_سمون_str_with_pos(char_u *str ناکارول_شوی, char_u *pat ناکارول_شوی)
{
#که_تعریف_شوی FEAT_پلټنه_EXTRA
    صحيح		    score = FUZZY_SCORE_NONE;
    garray_T	    *سمون_positions = عدم;
    int_u	    matches[FUZZY_سمون_MAX_LEN];
    صحيح		    j = 0;

    إذا (str == عدم || pat == عدم)
	أرجع عدم;

    سمون_positions = ALLOC_ONE(garray_T);
    إذا (سمون_positions == عدم)
	أرجع عدم;
    ga_init2(سمون_positions, حجم(int_u), 10);

    إذا (!fuzzy_سمون(str, pat, FALSE, &score, matches, FUZZY_سمون_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(سمون_positions);
	vim_free(سمون_positions);
	أرجع عدم;
    }

    لكل (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	إذا (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    إذا (ga_grow(سمون_positions, 1) == FAIL)
	    {
		ga_clear(سمون_positions);
		vim_free(سمون_positions);
		أرجع عدم;
	    }
	    ((int_u *)سمون_positions->ga_معلومات)[سمون_positions->ga_len] =
								    matches[j];
	    سمون_positions->ga_len++;
	    j++;
	}
    }

    أرجع سمون_positions;
#وإلا
    أرجع عدم;
#د_که_پای
}

/*
 * This function splits the line pointed to by `*ptr` into words and performs
 * a fuzzy match for the pattern `pat` on each word. It iterates through the
 * line, moving `*ptr` to the start of each word during the process.
 *
 * If a match is found:
 * - `*ptr` points to the start of the matched word.
 * - `*len` is set to the length of the matched word.
 * - `*score` contains the match score.
 *
 * If no match is found, `*ptr` is updated to the end of the line.
 */
    صحيح
fuzzy_سمون_str_دننه_کرښه(
    char_u	**ptr,
    char_u	*pat,
    صحيح		*len,
    pos_T	*current_pos,
    صحيح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*پای_ټکی = عدم;
    char_u	*پیل = عدم;
    صحيح		وموندل = FALSE;
    حرف	save_پای_ټکی;
    char_u	*کرښه_پای_ټکی = عدم;

    إذا (str == عدم || pat == عدم)
	أرجع وموندل;
    کرښه_پای_ټکی = پیدا_کړه_کرښه_پای_ټکی(str);

    طالما (str < کرښه_پای_ټکی)
    {
	// Skip non-word characters
	پیل = پیدا_کړه_کلمه_پیل(str);
	إذا (*پیل == NUL)
	    اكسر;
	پای_ټکی = پیدا_کړه_کلمه_پای_ټکی(پیل);

	// Extract the word from start to end
	save_پای_ټکی = *پای_ټکی;
	*پای_ټکی = NUL;

	// Perform fuzzy match
	*score = fuzzy_سمون_str(پیل, pat);
	*پای_ټکی = save_پای_ټکی;

	إذا (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحيح)(پای_ټکی - پیل);
	    وموندل = TRUE;
	    *ptr = پیل;
	    إذا (current_pos)
		current_pos->ستون += (صحيح)(پای_ټکی - strBegin);
	    اكسر;
	}

	// Move to the end of the current word for the next iteration
	str = پای_ټکی;
	// Ensure we continue searching after the current word
	طالما (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    إذا (!وموندل)
	*ptr = کرښه_پای_ټکی;

    أرجع وموندل;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    صحيح
پلټنه_for_fuzzy_سمون(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*نمونه,
    صحيح		dir,
    pos_T	*پیل_pos,
    صحيح		*len,
    char_u	**ptr,
    صحيح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_پای_ټکی;
    صحيح		وموندل_new_سمون = FALSE;
    صحيح		looped_around = FALSE;
    صحيح		ټولیز_کرښه = ctrl_x_ډېرپېښ_ټولیز_کرښه();

    إذا (buf == curbuf)
	circly_پای_ټکی = *پیل_pos;
    وإلا
    {
	circly_پای_ټکی.lnum = buf->b_ml.ml_کرښه_شمېر;
	circly_پای_ټکی.ستون = 0;
	circly_پای_ټکی.coladd = 0;
    }

    إذا (ټولیز_کرښه && پیل_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    افعل
    {

	// Check if looped around and back to start position
	إذا (looped_around && (ټولیز_کرښه ? current_pos.lnum == circly_پای_ټکی.lnum
					: EQUAL_POS(current_pos, circly_پای_ټکی)))
	    اكسر;

	// Ensure current_pos is valid
	إذا (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_کرښه_شمېر)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    إذا (!ټولیز_کرښه)
		*ptr += current_pos.ستون;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    إذا (*ptr != عدم && **ptr != NUL)
	    {
		إذا (!ټولیز_کرښه)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    وموندل_new_سمون = fuzzy_سمون_str_دننه_کرښه(ptr, نمونه,
						    len, &current_pos, score);
		    إذا (وموندل_new_سمون)
		    {
			*pos = current_pos;
			اكسر;
		    }
		    وإلا إذا (looped_around && current_pos.lnum == circly_پای_ټکی.lnum)
			اكسر;
		}
		وإلا
		{
		    char_u *کرښه = *ptr;
		    char_u *p = skipwhite(کرښه);
		    إذا (fuzzy_سمون_str(p, نمونه) != FUZZY_SCORE_NONE)
		    {
			وموندل_new_سمون = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحيح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحيح)(p - کرښه);
			اكسر;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	إذا (dir == FORWARD)
	{
	    إذا (++current_pos.lnum > buf->b_ml.ml_کرښه_شمېر)
	    {
		إذا (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;
	    }
	}
	وإلا
	{
	    إذا (--current_pos.lnum < 1)
	    {
		إذا (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_کرښه_شمېر;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;

	    }
	}
	current_pos.ستون = 0;
    } طالما (TRUE);

    أرجع وموندل_new_سمون;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    فراغ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحيح شمېر)
{
    إذا (fuzmatch == عدم)
	أرجع;

    لكل (صحيح i = 0; i < شمېر; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحيح
fuzzymatches_منزل_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحيح		شمېر,
	صحيح		funcsort)
{
    إذا (شمېر <= 0)
	اذهب theend;

    *matches = ALLOC_MULT(char_u *, شمېر);
    إذا (*matches == عدم)
    {
	fuzmatch_str_free(fuzmatch, شمېر);
	أرجع FAIL;
    }

    // Sort the list by the descending order of the match score
    إذا (funcsort)
	fuzzy_سمون_func_ترتیبول((فراغ *)fuzmatch, (size_t)شمېر);
    وإلا
	fuzzy_سمون_str_ترتیبول((فراغ *)fuzmatch, (size_t)شمېر);

    لكل (صحيح i = 0; i < شمېر; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    أرجع OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#تعریف_کړه سمون_MAX_LEN FUZZY_سمون_MAX_LEN

#تعریف_کړه SCORE_GAP_LEADING -0.005
#تعریف_کړه SCORE_GAP_TRAILING -0.005
#تعریف_کړه SCORE_GAP_INNER -0.01
#تعریف_کړه SCORE_سمون_CONSECUTIVE 1.0
#تعریف_کړه SCORE_سمون_SLASH 0.9
#تعریف_کړه SCORE_سمون_کلمه 0.8
#تعریف_کړه SCORE_سمون_CAPITAL 0.7
#تعریف_کړه SCORE_سمون_DOT 0.6

    ساكن صحيح
has_سمون(char_u *needle, char_u *haystack)
{
    إذا (!needle || !haystack || !*needle)
	أرجع FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    طالما (*n_ptr)
    {
	صحيح n_char = mb_ptr2char(n_ptr);
	صحيح n_پاسنی = MB_TOUPPER(n_char);
	صحيح وموندل = FALSE;

	طالما (*h_ptr)
	{
	    صحيح h_char = mb_ptr2char(h_ptr);
	    إذا (h_char == n_char || h_char == n_پاسنی)
	    {
		وموندل = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		اكسر;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	إذا (!وموندل)
	    أرجع FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    أرجع OK;
}

تعريفنوع بنية سمون_struct
{
    صحيح needle_len;
    صحيح haystack_len;
    صحيح ښکتنی_needle[سمون_MAX_LEN];     // stores codepoints
    صحيح ښکتنی_haystack[سمون_MAX_LEN];   // stores codepoints
    score_t سمون_bonus[سمون_MAX_LEN];
} سمون_struct;

#تعریف_کړه IS_کلمه_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#تعریف_کړه IS_لار_SEP(c) ((c) == '/')
#تعریف_کړه IS_DOT(c)      ((c) == '.')

    ساكن score_t
compute_bonus_codepoint(صحيح وروستی_c, صحيح c)
{
    إذا (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	إذا (IS_لار_SEP(وروستی_c))
	    أرجع SCORE_سمون_SLASH;
	إذا (IS_کلمه_SEP(وروستی_c))
	    أرجع SCORE_سمون_کلمه;
	إذا (IS_DOT(وروستی_c))
	    أرجع SCORE_سمون_DOT;
	إذا (MB_ISUPPER(c) && MB_ISLOWER(وروستی_c))
	    أرجع SCORE_سمون_CAPITAL;
    }
    أرجع 0;
}

    ساكن فراغ
تیاری_سمون_struct(سمون_struct *سمون, char_u *needle, char_u *haystack)
{
    صحيح i = 0;
    char_u *p = needle;
    طالما (*p != NUL && i < سمون_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	سمون->ښکتنی_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    سمون->needle_len = i;

    i = 0;
    p = haystack;
    صحيح prev_c = '/';
    طالما (*p != NUL && i < سمون_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	سمون->ښکتنی_haystack[i] = MB_TOLOWER(c);
	سمون->سمون_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    سمون->haystack_len = i;
}

    ساكن مضمن فراغ
سمون_لیکه(ثابت سمون_struct *سمون, صحيح لیکه, score_t *curr_D,
	score_t *curr_M, ثابت score_t *وروستی_D, ثابت score_t *وروستی_M)
{
    صحيح n = سمون->needle_len;
    صحيح m = سمون->haystack_len;
    صحيح i = لیکه;

    ثابت صحيح *ښکتنی_needle = سمون->ښکتنی_needle;
    ثابت صحيح *ښکتنی_haystack = سمون->ښکتنی_haystack;
    ثابت score_t *سمون_bonus = سمون->سمون_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    لكل (صحيح j = 0; j < m; j++)
    {
	إذا (ښکتنی_needle[i] == ښکتنی_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    إذا (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + سمون_bonus[j];
	    }
	    وإلا إذا (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + سمون_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_سمون_CONSECUTIVE);
	    }
	    prev_D = وروستی_D[j];
	    prev_M = وروستی_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	وإلا
	{
	    prev_D = وروستی_D[j];
	    prev_M = وروستی_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ساكن score_t
سمون_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    إذا (!needle || !haystack || !*needle)
	أرجع SCORE_MIN;

    سمون_struct سمون;
    تیاری_سمون_struct(&سمون, needle, haystack);

    صحيح n = سمون.needle_len;
    صحيح m = سمون.haystack_len;

    إذا (m > سمون_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	أرجع SCORE_MIN;
    }
    وإلا إذا (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	لكل (صحيح i = 0; i < n; i++)
	{
	    إذا (سمون.ښکتنی_needle[i] != سمون.ښکتنی_haystack[i])
	    {
		equal = false;
		اكسر;
	    }
	}
	إذا (equal)
	{
	    إذا (positions)
	    {
		لكل (صحيح i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    أرجع SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    إذا ((size_t)n > (اندازه_MAX / حجم(score_t)) / سمون_MAX_LEN / 2)
	أرجع SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *بلاک = (score_t*)alloc(حجم(score_t) * سمون_MAX_LEN * n * 2);
    إذا (!بلاک)
	أرجع SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[سمون_MAX_LEN] = (score_t(*)[سمون_MAX_LEN])بلاک;
    score_t (*M)[سمون_MAX_LEN] = (score_t(*)[سمون_MAX_LEN])(بلاک
							+ سمون_MAX_LEN * n);

    سمون_لیکه(&سمون, 0, D[0], M[0], D[0], M[0]);
    لكل (صحيح i = 1; i < n; i++)
	سمون_لیکه(&سمون, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    إذا (positions)
    {
	صحيح سمون_required = 0;
	لكل (صحيح i = n - 1, j = m - 1; i >= 0; i--)
	{
	    لكل (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		إذا (D[i][j] != SCORE_MIN &&
			(سمون_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    سمون_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_سمون_CONSECUTIVE;
		    positions[i] = j--;
		    اكسر;
		}
	    }
	}
    }

    score_t پایله = M[n - 1][m - 1];
    vim_free(بلاک);
    أرجع پایله;
}
