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
#define USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساكن صحيح fuzzy_match_item_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_match_in_list(list_T *l, char_u *str, صحيح matchseq, char_u *کلیل, callback_T *item_cb, صحيح retmatchpos, list_T *fmatchlist, long max_matches);
ساكن فراغ do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos);
#endif
ساكن صحيح fuzzy_match_str_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_match_str_sort(fuzmatch_str_T *fm, صحيح sz);
ساكن صحيح fuzzy_match_func_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_match_func_sort(fuzmatch_str_T *fm, صحيح sz);

تعريفنوع مزدوج score_t;
ساكن score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
ساكن صحيح has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

تعريفنوع بنية
{
    صحيح		idx;		// used for stable sort
    listitem_T	*item;
    صحيح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحيح		itemstr_allocated;
    صحيح		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحيح
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    صحيح		matchseq,
    صحيح		*outScore,
    int_u	*matches,
    صحيح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحيح		complete = FALSE;
    صحيح		score = 0;
    صحيح		ژمارە_Matches = 0;
    صحيح		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

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
	إذا (ژمارە_Matches > maxMatches - pat_chars)
	{
	    ژمارە_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    اكسر;
	}

	score = FUZZY_SCORE_NONE;
	إذا (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + ژمارە_Matches);
	    إذا (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (صحيح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحيح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	إذا (score == FUZZY_SCORE_NONE)
	{
	    ژمارە_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    اكسر;
	}

	إذا (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	وإلا إذا (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	وإلا
	    *outScore += score;

	ژمارە_Matches += pat_chars;

	إذا (complete || ژمارە_Matches >= maxMatches)
	    اكسر;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    أرجع ژمارە_Matches != 0;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساكن صحيح
fuzzy_match_item_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzzyItem_T *)s1)->score;
    صحيح		v2 = ((fuzzyItem_T *)s2)->score;

    إذا (v1 == v2)
    {
	صحيح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	صحيح patlen = (صحيح)STRLEN(pat);
	صحيح startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	إذا (exact_match1 == exact_match2)
	{
	    صحيح idx1 = ((fuzzyItem_T *)s1)->idx;
	    صحيح idx2 = ((fuzzyItem_T *)s2)->idx;
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
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    صحيح		matchseq,
    char_u	*کلیل,
    callback_T	*item_cb,
    صحيح		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *کاڵاکان = عدم;
    listitem_T	    *li;
    long	    match_ژماردن = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    إذا (len == 0)
	أرجع;
    إذا (max_matches > 0 && len > max_matches)
	len = max_matches;

    کاڵاکان = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    إذا (کاڵاکان == عدم)
	أرجع;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_کاڵاکان(l, li)
    {
	صحيح		score;
	char_u		*itemstr = عدم;
	char_u		*itemstr_copy = عدم;
	typval_T	rettv;
	صحيح		itemstr_allocate = FALSE;
	list_T		*match_positions = عدم;

	إذا (max_matches > 0 && match_ژماردن >= max_matches)
	    اكسر;

	rettv.v_type = VAR_UNKNOWN;
	إذا (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	وإلا إذا (li->li_tv.v_type == VAR_DICT
				&& (کلیل != عدم || item_cb->cb_name != عدم))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    إذا (کلیل != عدم)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (حرف *)کلیل, FALSE);
	    وإلا
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		إذا (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
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
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
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
		match_positions = list_alloc();
		إذا (match_positions == عدم)
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

		طالما (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    إذا (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			إذا (list_append_ژمار(match_positions, matches[j]) == FAIL)
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
		    list_free(match_positions);
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    کاڵاکان[match_ژماردن].idx = match_ژماردن;
	    کاڵاکان[match_ژماردن].item = li;
	    کاڵاکان[match_ژماردن].score = score;
	    کاڵاکان[match_ژماردن].pat = str;
	    کاڵاکان[match_ژماردن].startpos = matches[0];
	    کاڵاکان[match_ژماردن].itemstr = itemstr_copy;
	    کاڵاکان[match_ژماردن].itemstr_allocated = itemstr_allocate;
	    کاڵاکان[match_ژماردن].lmatchpos = match_positions;

	    ++match_ژماردن;
	}
	clear_tv(&rettv);
    }

    إذا (match_ژماردن > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((فراغ *)کاڵاکان, (size_t)match_ژماردن, حجم(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	إذا (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;
	}
	وإلا
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	لكل (صحيح i = 0; i < match_ژماردن; i++)
	{
	    إذا (list_append_tv(retlist, &کاڵاکان[i].item->li_tv) == FAIL)
		اذهب done;
	}

	// next copy the list of matching positions
	إذا (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;

	    لكل (صحيح i = 0; i < match_ژماردن; i++)
	    {
		إذا (کاڵاکان[i].lmatchpos != عدم)
		{
		    إذا (list_append_list(retlist, کاڵاکان[i].lmatchpos) == OK)
			کاڵاکان[i].lmatchpos = عدم;
		    وإلا
			اذهب done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;
	    لكل (صحيح i = 0; i < match_ژماردن; i++)
	    {
		إذا (list_append_ژمار(retlist, کاڵاکان[i].score) == FAIL)
		    اذهب done;
	    }
	}
    }

done:
    لكل (صحيح i = 0; i < match_ژماردن; i++)
    {
	إذا (کاڵاکان[i].itemstr_allocated)
	    vim_free(کاڵاکان[i].itemstr);

	إذا (کاڵاکان[i].lmatchpos)
	    list_free(کاڵاکان[i].lmatchpos);
    }
    vim_free(کاڵاکان);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ساكن فراغ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos)
{
    callback_T	cb;
    char_u	*کلیل = عدم;
    صحيح		ret;
    صحيح		matchseq = FALSE;
    long	max_matches = 0;

    إذا (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    إذا (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	أرجع;
    }
    إذا (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == عدم)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
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
	إذا ((di = dict_find(d, (char_u *)"key", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == عدم
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_بەها_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		أرجع;
	    }
	    کلیل = tv_get_string(&di->di_tv);
	}
	وإلا إذا ((di = dict_find(d, (char_u *)"text_cb", -1)) != عدم)
	{
	    cb = get_callback(&di->di_tv);
	    إذا (cb.cb_name == عدم)
	    {
		semsg(_(e_invalid_بەها_for_argument_str), "text_cb");
		أرجع;
	    }
	}

	إذا ((di = dict_find(d, (char_u *)"limit", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_ژمار)
	    {
		semsg(_(e_invalid_بەها_for_argument_str), "limit");
		أرجع;
	    }
	    max_matches = (long)tv_get_ژمار_chk(&di->di_tv, عدم);
	}

	إذا (dict_has_کلیل(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    إذا (ret == FAIL)
	اذهب done;
    إذا (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	إذا (l == عدم)
	    اذهب done;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب done;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب done;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلیل, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
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
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ساكن صحيح
fuzzy_match_str_compare(ثابت فراغ *s1, ثابت فراغ *s2)
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
fuzzy_match_str_sort(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ساكن صحيح
fuzzy_match_func_compare(ثابت فراغ *s1, ثابت فراغ *s2)
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
fuzzy_match_func_sort(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحيح
fuzzy_match_str(char_u *str, char_u *pat)
{
    صحيح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    إذا (str == عدم || pat == عدم)
	أرجع score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    أرجع score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_گەڕان_EXTRA
    صحيح		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = عدم;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    صحيح		    j = 0;

    إذا (str == عدم || pat == عدم)
	أرجع عدم;

    match_positions = ALLOC_ONE(garray_T);
    إذا (match_positions == عدم)
	أرجع عدم;
    ga_init2(match_positions, حجم(int_u), 10);

    إذا (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	أرجع عدم;
    }

    لكل (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	إذا (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    إذا (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		أرجع عدم;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    أرجع match_positions;
#وإلا
    أرجع عدم;
#endif
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
fuzzy_match_str_in_هێڵ(
    char_u	**ptr,
    char_u	*pat,
    صحيح		*len,
    pos_T	*current_pos,
    صحيح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = عدم;
    char_u	*start = عدم;
    صحيح		found = FALSE;
    حرف	save_end;
    char_u	*هێڵ_end = عدم;

    إذا (str == عدم || pat == عدم)
	أرجع found;
    هێڵ_end = find_هێڵ_end(str);

    طالما (str < هێڵ_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	إذا (*start == NUL)
	    اكسر;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	إذا (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحيح)(end - start);
	    found = TRUE;
	    *ptr = start;
	    إذا (current_pos)
		current_pos->col += (صحيح)(end - strBegin);
	    اكسر;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	طالما (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    إذا (!found)
	*ptr = هێڵ_end;

    أرجع found;
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
گەڕان_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    صحيح		dir,
    pos_T	*start_pos,
    صحيح		*len,
    char_u	**ptr,
    صحيح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    صحيح		found_new_match = FALSE;
    صحيح		looped_around = FALSE;
    صحيح		whole_هێڵ = ctrl_x_mode_whole_هێڵ();

    إذا (buf == curbuf)
	circly_end = *start_pos;
    وإلا
    {
	circly_end.lnum = buf->b_ml.ml_هێڵ_ژماردن;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    إذا (whole_هێڵ && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    افعل
    {

	// Check if looped around and back to start position
	إذا (looped_around && (whole_هێڵ ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    اكسر;

	// Ensure current_pos is valid
	إذا (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_هێڵ_ژماردن)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    إذا (!whole_هێڵ)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    إذا (*ptr != عدم && **ptr != NUL)
	    {
		إذا (!whole_هێڵ)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_هێڵ(ptr, pattern,
						    len, &current_pos, score);
		    إذا (found_new_match)
		    {
			*pos = current_pos;
			اكسر;
		    }
		    وإلا إذا (looped_around && current_pos.lnum == circly_end.lnum)
			اكسر;
		}
		وإلا
		{
		    char_u *هێڵ = *ptr;
		    char_u *p = skipwhite(هێڵ);
		    إذا (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحيح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحيح)(p - هێڵ);
			اكسر;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	إذا (dir == FORWARD)
	{
	    إذا (++current_pos.lnum > buf->b_ml.ml_هێڵ_ژماردن)
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
		    current_pos.lnum = buf->b_ml.ml_هێڵ_ژماردن;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;

	    }
	}
	current_pos.col = 0;
    } طالما (TRUE);

    أرجع found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    فراغ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحيح ژماردن)
{
    إذا (fuzmatch == عدم)
	أرجع;

    لكل (صحيح i = 0; i < ژماردن; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحيح
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحيح		ژماردن,
	صحيح		funcsort)
{
    إذا (ژماردن <= 0)
	اذهب theend;

    *matches = ALLOC_MULT(char_u *, ژماردن);
    إذا (*matches == عدم)
    {
	fuzmatch_str_free(fuzmatch, ژماردن);
	أرجع FAIL;
    }

    // Sort the list by the descending order of the match score
    إذا (funcsort)
	fuzzy_match_func_sort((فراغ *)fuzmatch, (size_t)ژماردن);
    وإلا
	fuzzy_match_str_sort((فراغ *)fuzmatch, (size_t)ژماردن);

    لكل (صحيح i = 0; i < ژماردن; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    أرجع OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#define MATCH_MAX_LEN FUZZY_MATCH_MAX_LEN

#define SCORE_GAP_LEADING -0.005
#define SCORE_GAP_TRAILING -0.005
#define SCORE_GAP_INNER -0.01
#define SCORE_MATCH_CONSECUTIVE 1.0
#define SCORE_MATCH_SLASH 0.9
#define SCORE_MATCH_WORD 0.8
#define SCORE_MATCH_CAPITAL 0.7
#define SCORE_MATCH_DOT 0.6

    ساكن صحيح
has_match(char_u *needle, char_u *haystack)
{
    إذا (!needle || !haystack || !*needle)
	أرجع FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    طالما (*n_ptr)
    {
	صحيح n_char = mb_ptr2char(n_ptr);
	صحيح n_upper = MB_TOUPPER(n_char);
	صحيح found = FALSE;

	طالما (*h_ptr)
	{
	    صحيح h_char = mb_ptr2char(h_ptr);
	    إذا (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		اكسر;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	إذا (!found)
	    أرجع FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    أرجع OK;
}

تعريفنوع بنية match_struct
{
    صحيح needle_len;
    صحيح haystack_len;
    صحيح lower_needle[MATCH_MAX_LEN];     // stores codepoints
    صحيح lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    ساكن score_t
compute_bonus_codepoint(صحيح last_c, صحيح c)
{
    إذا (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	إذا (IS_PATH_SEP(last_c))
	    أرجع SCORE_MATCH_SLASH;
	إذا (IS_WORD_SEP(last_c))
	    أرجع SCORE_MATCH_WORD;
	إذا (IS_DOT(last_c))
	    أرجع SCORE_MATCH_DOT;
	إذا (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    أرجع SCORE_MATCH_CAPITAL;
    }
    أرجع 0;
}

    ساكن فراغ
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    صحيح i = 0;
    char_u *p = needle;
    طالما (*p != NUL && i < MATCH_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    صحيح prev_c = '/';
    طالما (*p != NUL && i < MATCH_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    ساكن مضمن فراغ
match_row(ثابت match_struct *match, صحيح row, score_t *curr_D,
	score_t *curr_M, ثابت score_t *last_D, ثابت score_t *last_M)
{
    صحيح n = match->needle_len;
    صحيح m = match->haystack_len;
    صحيح i = row;

    ثابت صحيح *lower_needle = match->lower_needle;
    ثابت صحيح *lower_haystack = match->lower_haystack;
    ثابت score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    لكل (صحيح j = 0; j < m; j++)
    {
	إذا (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    إذا (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    وإلا إذا (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + match_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_MATCH_CONSECUTIVE);
	    }
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	وإلا
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ساكن score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    إذا (!needle || !haystack || !*needle)
	أرجع SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    صحيح n = match.needle_len;
    صحيح m = match.haystack_len;

    إذا (m > MATCH_MAX_LEN || n > m)
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
	    إذا (match.lower_needle[i] != match.lower_haystack[i])
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
    إذا ((size_t)n > (SIZE_MAX / حجم(score_t)) / MATCH_MAX_LEN / 2)
	أرجع SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(حجم(score_t) * MATCH_MAX_LEN * n * 2);
    إذا (!block)
	أرجع SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    لكل (صحيح i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    إذا (positions)
    {
	صحيح match_required = 0;
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
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    اكسر;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    أرجع result;
}
