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
#شامل "vim.h"

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
ایستا صحیح fuzzy_match_item_compare(ثابت تهی *s1, ثابت تهی *s2);
ایستا تهی fuzzy_match_in_list(list_T *l, char_u *str, صحیح matchseq, char_u *کلید, callback_T *item_cb, صحیح retmatchpos, list_T *fmatchlist, long max_matches);
ایستا تهی do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحیح retmatchpos);
#endif
ایستا صحیح fuzzy_match_str_compare(ثابت تهی *s1, ثابت تهی *s2);
ایستا تهی fuzzy_match_str_sort(fuzmatch_str_T *fm, صحیح sz);
ایستا صحیح fuzzy_match_func_compare(ثابت تهی *s1, ثابت تهی *s2);
ایستا تهی fuzzy_match_func_sort(fuzmatch_str_T *fm, صحیح sz);

تعریفنوع دوگانه score_t;
ایستا score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
ایستا صحیح has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

تعریفنوع ساختار
{
    صحیح		idx;		// used for stable sort
    listitem_T	*item;
    صحیح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحیح		itemstr_allocated;
    صحیح		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحیح
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    صحیح		matchseq,
    صحیح		*outScore,
    int_u	*matches,
    صحیح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحیح		complete = FALSE;
    صحیح		score = 0;
    صحیح		رقم_Matches = 0;
    صحیح		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    اگر (save_pat == پوچ)
	بازگشت FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    وقتی (TRUE)
    {
	اگر (matchseq)
	    complete = TRUE;
	وگرنه
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    اگر (*p == NUL)
		بشکن;
	    pat = p;
	    وقتی (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		اگر (has_mbyte)
		    MB_PTR_ADV(p);
		وگرنه
		    ++p;
	    }
	    اگر (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	اگر (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	اگر (رقم_Matches > maxMatches - pat_chars)
	{
	    رقم_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    بشکن;
	}

	score = FUZZY_SCORE_NONE;
	اگر (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + رقم_Matches);
	    اگر (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (صحیح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحیح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	اگر (score == FUZZY_SCORE_NONE)
	{
	    رقم_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    بشکن;
	}

	اگر (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	وگرنه اگر (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	وگرنه
	    *outScore += score;

	رقم_Matches += pat_chars;

	اگر (complete || رقم_Matches >= maxMatches)
	    بشکن;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    بازگشت رقم_Matches != 0;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ایستا صحیح
fuzzy_match_item_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    صحیح		v1 = ((fuzzyItem_T *)s1)->score;
    صحیح		v2 = ((fuzzyItem_T *)s2)->score;

    اگر (v1 == v2)
    {
	صحیح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	صحیح patlen = (صحیح)STRLEN(pat);
	صحیح startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	اگر (exact_match1 == exact_match2)
	{
	    صحیح idx1 = ((fuzzyItem_T *)s1)->idx;
	    صحیح idx2 = ((fuzzyItem_T *)s2)->idx;
	    بازگشت idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	وگرنه اگر (exact_match2)
	    بازگشت 1;
	بازگشت -1;
    }
    وگرنه
	بازگشت v1 > v2 ? -1 : 1;
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
    ایستا تهی
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    صحیح		matchseq,
    char_u	*کلید,
    callback_T	*item_cb,
    صحیح		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *اقلام = پوچ;
    listitem_T	    *li;
    long	    match_شمار = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    اگر (len == 0)
	بازگشت;
    اگر (max_matches > 0 && len > max_matches)
	len = max_matches;

    اقلام = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    اگر (اقلام == پوچ)
	بازگشت;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_اقلام(l, li)
    {
	صحیح		score;
	char_u		*itemstr = پوچ;
	char_u		*itemstr_copy = پوچ;
	typval_T	rettv;
	صحیح		itemstr_allocate = FALSE;
	list_T		*match_positions = پوچ;

	اگر (max_matches > 0 && match_شمار >= max_matches)
	    بشکن;

	rettv.v_type = VAR_UNKNOWN;
	اگر (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	وگرنه اگر (li->li_tv.v_type == VAR_DICT
				&& (کلید != پوچ || item_cb->cb_name != پوچ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    اگر (کلید != پوچ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (نویسه *)کلید, FALSE);
	    وگرنه
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		اگر (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    اگر (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	اگر (itemstr != پوچ
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    اگر (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		اگر (itemstr_copy == پوچ)
		{
		    clear_tv(&rettv);
		    ادامه;
		}
	    }
	    وگرنه
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    اگر (retmatchpos)
	    {
		match_positions = list_alloc();
		اگر (match_positions == پوچ)
		{
		    اگر (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ادامه;
		}

		// Fill position information
		صحیح	j = 0;
		char_u	*p = str;
		صحیح	success = TRUE;

		وقتی (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    اگر (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			اگر (list_append_عدد(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    بشکن;
			}
			j++;
		    }
		    اگر (has_mbyte)
			MB_PTR_ADV(p);
		    وگرنه
			++p;
		}

		اگر (!success)
		{
		    list_free(match_positions);
		    اگر (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ادامه;
		}
	    }
	    اقلام[match_شمار].idx = match_شمار;
	    اقلام[match_شمار].item = li;
	    اقلام[match_شمار].score = score;
	    اقلام[match_شمار].pat = str;
	    اقلام[match_شمار].startpos = matches[0];
	    اقلام[match_شمار].itemstr = itemstr_copy;
	    اقلام[match_شمار].itemstr_allocated = itemstr_allocate;
	    اقلام[match_شمار].lmatchpos = match_positions;

	    ++match_شمار;
	}
	clear_tv(&rettv);
    }

    اگر (match_شمار > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((تهی *)اقلام, (size_t)match_شمار, اندازه(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	اگر (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    اگر (li == پوچ || li->li_tv.vval.v_list == پوچ)
		برو done;
	    retlist = li->li_tv.vval.v_list;
	}
	وگرنه
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	برای (صحیح i = 0; i < match_شمار; i++)
	{
	    اگر (list_append_tv(retlist, &اقلام[i].item->li_tv) == FAIL)
		برو done;
	}

	// next copy the list of matching positions
	اگر (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    اگر (li == پوچ || li->li_tv.vval.v_list == پوچ)
		برو done;
	    retlist = li->li_tv.vval.v_list;

	    برای (صحیح i = 0; i < match_شمار; i++)
	    {
		اگر (اقلام[i].lmatchpos != پوچ)
		{
		    اگر (list_append_list(retlist, اقلام[i].lmatchpos) == OK)
			اقلام[i].lmatchpos = پوچ;
		    وگرنه
			برو done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    اگر (li == پوچ || li->li_tv.vval.v_list == پوچ)
		برو done;
	    retlist = li->li_tv.vval.v_list;
	    برای (صحیح i = 0; i < match_شمار; i++)
	    {
		اگر (list_append_عدد(retlist, اقلام[i].score) == FAIL)
		    برو done;
	    }
	}
    }

done:
    برای (صحیح i = 0; i < match_شمار; i++)
    {
	اگر (اقلام[i].itemstr_allocated)
	    vim_free(اقلام[i].itemstr);

	اگر (اقلام[i].lmatchpos)
	    list_free(اقلام[i].lmatchpos);
    }
    vim_free(اقلام);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ایستا تهی
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحیح retmatchpos)
{
    callback_T	cb;
    char_u	*کلید = پوچ;
    صحیح		ret;
    صحیح		matchseq = FALSE;
    long	max_matches = 0;

    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    اگر (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == پوچ)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	بازگشت;
    }
    اگر (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == پوچ)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	بازگشت;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    بازگشت;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	اگر ((di = dict_find(d, (char_u *)"key", -1)) != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == پوچ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		بازگشت;
	    }
	    کلید = tv_get_string(&di->di_tv);
	}
	وگرنه اگر ((di = dict_find(d, (char_u *)"text_cb", -1)) != پوچ)
	{
	    cb = get_callback(&di->di_tv);
	    اگر (cb.cb_name == پوچ)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str), "text_cb");
		بازگشت;
	    }
	}

	اگر ((di = dict_find(d, (char_u *)"limit", -1)) != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_عدد)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str), "limit");
		بازگشت;
	    }
	    max_matches = (long)tv_get_عدد_chk(&di->di_tv, پوچ);
	}

	اگر (dict_has_کلید(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    اگر (ret == FAIL)
	برو done;
    اگر (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	اگر (l == پوچ)
	    برو done;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    برو done;
	}
	l = list_alloc();
	اگر (l == پوچ)
	    برو done;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    برو done;
	}
	l = list_alloc();
	اگر (l == پوچ)
	    برو done;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    برو done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلید, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    تهی
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    تهی
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ایستا صحیح
fuzzy_match_str_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    صحیح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحیح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحیح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحیح		idx2 = ((fuzmatch_str_T *)s2)->idx;

    اگر (v1 == v2)
	بازگشت idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وگرنه
	بازگشت v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ایستا تهی
fuzzy_match_str_sort(fuzmatch_str_T *fm, صحیح sz)
{
    // Sort the list by the descending order of the match score
    qsort((تهی *)fm, (size_t)sz, اندازه(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ایستا صحیح
fuzzy_match_func_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    صحیح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحیح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحیح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحیح		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    اگر (*str1 != '<' && *str2 == '<')
	بازگشت -1;
    اگر (*str1 == '<' && *str2 != '<')
	بازگشت 1;
    اگر (v1 == v2)
	بازگشت idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وگرنه
	بازگشت v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ایستا تهی
fuzzy_match_func_sort(fuzmatch_str_T *fm, صحیح sz)
{
    // Sort the list by the descending order of the match score
    qsort((تهی *)fm, (size_t)sz, اندازه(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحیح
fuzzy_match_str(char_u *str, char_u *pat)
{
    صحیح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    اگر (str == پوچ || pat == پوچ)
	بازگشت score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				اندازه(matchpos) / اندازه(matchpos[0]));

    بازگشت score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_جستجو_EXTRA
    صحیح		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = پوچ;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    صحیح		    j = 0;

    اگر (str == پوچ || pat == پوچ)
	بازگشت پوچ;

    match_positions = ALLOC_ONE(garray_T);
    اگر (match_positions == پوچ)
	بازگشت پوچ;
    ga_init2(match_positions, اندازه(int_u), 10);

    اگر (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	بازگشت پوچ;
    }

    برای (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	اگر (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    اگر (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		بازگشت پوچ;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    بازگشت match_positions;
#وگرنه
    بازگشت پوچ;
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
    صحیح
fuzzy_match_str_in_سطر(
    char_u	**ptr,
    char_u	*pat,
    صحیح		*len,
    pos_T	*current_pos,
    صحیح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = پوچ;
    char_u	*start = پوچ;
    صحیح		found = FALSE;
    نویسه	save_end;
    char_u	*سطر_end = پوچ;

    اگر (str == پوچ || pat == پوچ)
	بازگشت found;
    سطر_end = find_سطر_end(str);

    وقتی (str < سطر_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	اگر (*start == NUL)
	    بشکن;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	اگر (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحیح)(end - start);
	    found = TRUE;
	    *ptr = start;
	    اگر (current_pos)
		current_pos->col += (صحیح)(end - strBegin);
	    بشکن;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	وقتی (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    اگر (!found)
	*ptr = سطر_end;

    بازگشت found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    صحیح
جستجو_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    صحیح		dir,
    pos_T	*start_pos,
    صحیح		*len,
    char_u	**ptr,
    صحیح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    صحیح		found_new_match = FALSE;
    صحیح		looped_around = FALSE;
    صحیح		whole_سطر = ctrl_x_mode_whole_سطر();

    اگر (buf == curbuf)
	circly_end = *start_pos;
    وگرنه
    {
	circly_end.lnum = buf->b_ml.ml_سطر_شمار;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    اگر (whole_سطر && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    انجامبده
    {

	// Check if looped around and back to start position
	اگر (looped_around && (whole_سطر ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    بشکن;

	// Ensure current_pos is valid
	اگر (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_سطر_شمار)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    اگر (!whole_سطر)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    اگر (*ptr != پوچ && **ptr != NUL)
	    {
		اگر (!whole_سطر)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_سطر(ptr, pattern,
						    len, &current_pos, score);
		    اگر (found_new_match)
		    {
			*pos = current_pos;
			بشکن;
		    }
		    وگرنه اگر (looped_around && current_pos.lnum == circly_end.lnum)
			بشکن;
		}
		وگرنه
		{
		    char_u *سطر = *ptr;
		    char_u *p = skipwhite(سطر);
		    اگر (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحیح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحیح)(p - سطر);
			بشکن;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	اگر (dir == FORWARD)
	{
	    اگر (++current_pos.lnum > buf->b_ml.ml_سطر_شمار)
	    {
		اگر (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		وگرنه
		    بشکن;
	    }
	}
	وگرنه
	{
	    اگر (--current_pos.lnum < 1)
	    {
		اگر (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_سطر_شمار;
		    looped_around = TRUE;
		}
		وگرنه
		    بشکن;

	    }
	}
	current_pos.col = 0;
    } وقتی (TRUE);

    بازگشت found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    تهی
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحیح شمار)
{
    اگر (fuzmatch == پوچ)
	بازگشت;

    برای (صحیح i = 0; i < شمار; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحیح
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحیح		شمار,
	صحیح		funcsort)
{
    اگر (شمار <= 0)
	برو theend;

    *matches = ALLOC_MULT(char_u *, شمار);
    اگر (*matches == پوچ)
    {
	fuzmatch_str_free(fuzmatch, شمار);
	بازگشت FAIL;
    }

    // Sort the list by the descending order of the match score
    اگر (funcsort)
	fuzzy_match_func_sort((تهی *)fuzmatch, (size_t)شمار);
    وگرنه
	fuzzy_match_str_sort((تهی *)fuzmatch, (size_t)شمار);

    برای (صحیح i = 0; i < شمار; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    بازگشت OK;
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

    ایستا صحیح
has_match(char_u *needle, char_u *haystack)
{
    اگر (!needle || !haystack || !*needle)
	بازگشت FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    وقتی (*n_ptr)
    {
	صحیح n_char = mb_ptr2char(n_ptr);
	صحیح n_upper = MB_TOUPPER(n_char);
	صحیح found = FALSE;

	وقتی (*h_ptr)
	{
	    صحیح h_char = mb_ptr2char(h_ptr);
	    اگر (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		بشکن;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	اگر (!found)
	    بازگشت FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    بازگشت OK;
}

تعریفنوع ساختار match_struct
{
    صحیح needle_len;
    صحیح haystack_len;
    صحیح lower_needle[MATCH_MAX_LEN];     // stores codepoints
    صحیح lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    ایستا score_t
compute_bonus_codepoint(صحیح last_c, صحیح c)
{
    اگر (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	اگر (IS_PATH_SEP(last_c))
	    بازگشت SCORE_MATCH_SLASH;
	اگر (IS_WORD_SEP(last_c))
	    بازگشت SCORE_MATCH_WORD;
	اگر (IS_DOT(last_c))
	    بازگشت SCORE_MATCH_DOT;
	اگر (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    بازگشت SCORE_MATCH_CAPITAL;
    }
    بازگشت 0;
}

    ایستا تهی
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    صحیح i = 0;
    char_u *p = needle;
    وقتی (*p != NUL && i < MATCH_MAX_LEN)
    {
	صحیح c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    صحیح prev_c = '/';
    وقتی (*p != NUL && i < MATCH_MAX_LEN)
    {
	صحیح c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    ایستا درونخطی تهی
match_row(ثابت match_struct *match, صحیح row, score_t *curr_D,
	score_t *curr_M, ثابت score_t *last_D, ثابت score_t *last_M)
{
    صحیح n = match->needle_len;
    صحیح m = match->haystack_len;
    صحیح i = row;

    ثابت صحیح *lower_needle = match->lower_needle;
    ثابت صحیح *lower_haystack = match->lower_haystack;
    ثابت score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    برای (صحیح j = 0; j < m; j++)
    {
	اگر (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    اگر (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    وگرنه اگر (j)
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
	وگرنه
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ایستا score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    اگر (!needle || !haystack || !*needle)
	بازگشت SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    صحیح n = match.needle_len;
    صحیح m = match.haystack_len;

    اگر (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	بازگشت SCORE_MIN;
    }
    وگرنه اگر (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	برای (صحیح i = 0; i < n; i++)
	{
	    اگر (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		بشکن;
	    }
	}
	اگر (equal)
	{
	    اگر (positions)
	    {
		برای (صحیح i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    بازگشت SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    اگر ((size_t)n > (SIZE_MAX / اندازه(score_t)) / MATCH_MAX_LEN / 2)
	بازگشت SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(اندازه(score_t) * MATCH_MAX_LEN * n * 2);
    اگر (!block)
	بازگشت SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    برای (صحیح i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    اگر (positions)
    {
	صحیح match_required = 0;
	برای (صحیح i = n - 1, j = m - 1; i >= 0; i--)
	{
	    برای (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		اگر (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    بشکن;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    بازگشت result;
}
