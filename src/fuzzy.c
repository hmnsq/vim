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

#ܐܢ defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساكن صحيح fuzzy_match_item_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_match_in_list(list_T *l, char_u *str, صحيح matchseq, char_u *key, callback_T *item_cb, صحيح retmatchpos, list_T *fmatchlist, long max_matches);
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
    صحيح		numMatches = 0;
    صحيح		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    ܐܢ (save_pat == عدم)
	ܦܢܐ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ܟܡܐ (TRUE)
    {
	ܐܢ (matchseq)
	    complete = TRUE;
	ܐܚܪܢܐ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ܐܢ (*p == NUL)
		اكسر;
	    pat = p;
	    ܟܡܐ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ܐܢ (has_mbyte)
		    MB_PTR_ADV(p);
		ܐܚܪܢܐ
		    ++p;
	    }
	    ܐܢ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ܐܢ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ܐܢ (numMatches > maxMatches - pat_chars)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    اكسر;
	}

	score = FUZZY_SCORE_NONE;
	ܐܢ (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + numMatches);
	    ܐܢ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (صحيح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحيح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ܐܢ (score == FUZZY_SCORE_NONE)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    اكسر;
	}

	ܐܢ (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	ܐܚܪܢܐ ܐܢ (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	ܐܚܪܢܐ
	    *outScore += score;

	numMatches += pat_chars;

	ܐܢ (complete || numMatches >= maxMatches)
	    اكسر;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ܦܢܐ numMatches != 0;
}

#ܐܢ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساكن صحيح
fuzzy_match_item_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzzyItem_T *)s1)->score;
    صحيح		v2 = ((fuzzyItem_T *)s2)->score;

    ܐܢ (v1 == v2)
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

	ܐܢ (exact_match1 == exact_match2)
	{
	    صحيح idx1 = ((fuzzyItem_T *)s1)->idx;
	    صحيح idx2 = ((fuzzyItem_T *)s2)->idx;
	    ܦܢܐ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ܐܚܪܢܐ ܐܢ (exact_match2)
	    ܦܢܐ 1;
	ܦܢܐ -1;
    }
    ܐܚܪܢܐ
	ܦܢܐ v1 > v2 ? -1 : 1;
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
    char_u	*key,
    callback_T	*item_cb,
    صحيح		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *items = عدم;
    listitem_T	    *li;
    long	    match_count = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    ܐܢ (len == 0)
	ܦܢܐ;
    ܐܢ (max_matches > 0 && len > max_matches)
	len = max_matches;

    items = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    ܐܢ (items == عدم)
	ܦܢܐ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ITEMS(l, li)
    {
	صحيح		score;
	char_u		*itemstr = عدم;
	char_u		*itemstr_copy = عدم;
	typval_T	rettv;
	صحيح		itemstr_allocate = FALSE;
	list_T		*match_positions = عدم;

	ܐܢ (max_matches > 0 && match_count >= max_matches)
	    اكسر;

	rettv.v_type = VAR_UNKNOWN;
	ܐܢ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ܐܚܪܢܐ ܐܢ (li->li_tv.v_type == VAR_DICT
				&& (key != عدم || item_cb->cb_name != عدم))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ܐܢ (key != عدم)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (حرف *)key, FALSE);
	    ܐܚܪܢܐ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ܐܢ (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ܐܢ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ܐܢ (itemstr != عدم
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    ܐܢ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ܐܢ (itemstr_copy == عدم)
		{
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    ܐܚܪܢܐ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ܐܢ (retmatchpos)
	    {
		match_positions = list_alloc();
		ܐܢ (match_positions == عدم)
		{
		    ܐܢ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}

		// Fill position information
		صحيح	j = 0;
		char_u	*p = str;
		صحيح	success = TRUE;

		ܟܡܐ (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    ܐܢ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ܐܢ (list_append_number(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    اكسر;
			}
			j++;
		    }
		    ܐܢ (has_mbyte)
			MB_PTR_ADV(p);
		    ܐܚܪܢܐ
			++p;
		}

		ܐܢ (!success)
		{
		    list_free(match_positions);
		    ܐܢ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    items[match_count].idx = match_count;
	    items[match_count].item = li;
	    items[match_count].score = score;
	    items[match_count].pat = str;
	    items[match_count].startpos = matches[0];
	    items[match_count].itemstr = itemstr_copy;
	    items[match_count].itemstr_allocated = itemstr_allocate;
	    items[match_count].lmatchpos = match_positions;

	    ++match_count;
	}
	clear_tv(&rettv);
    }

    ܐܢ (match_count > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((فراغ *)items, (size_t)match_count, حجم(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ܐܢ (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    ܐܢ (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;
	}
	ܐܚܪܢܐ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	لكل (صحيح i = 0; i < match_count; i++)
	{
	    ܐܢ (list_append_tv(retlist, &items[i].item->li_tv) == FAIL)
		اذهب done;
	}

	// next copy the list of matching positions
	ܐܢ (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    ܐܢ (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;

	    لكل (صحيح i = 0; i < match_count; i++)
	    {
		ܐܢ (items[i].lmatchpos != عدم)
		{
		    ܐܢ (list_append_list(retlist, items[i].lmatchpos) == OK)
			items[i].lmatchpos = عدم;
		    ܐܚܪܢܐ
			اذهب done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    ܐܢ (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب done;
	    retlist = li->li_tv.vval.v_list;
	    لكل (صحيح i = 0; i < match_count; i++)
	    {
		ܐܢ (list_append_number(retlist, items[i].score) == FAIL)
		    اذهب done;
	    }
	}
    }

done:
    لكل (صحيح i = 0; i < match_count; i++)
    {
	ܐܢ (items[i].itemstr_allocated)
	    vim_free(items[i].itemstr);

	ܐܢ (items[i].lmatchpos)
	    list_free(items[i].lmatchpos);
    }
    vim_free(items);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ساكن فراغ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos)
{
    callback_T	cb;
    char_u	*key = عدم;
    صحيح		ret;
    صحيح		matchseq = FALSE;
    long	max_matches = 0;

    ܐܢ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    ܐܢ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ܦܢܐ;
    }
    ܐܢ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == عدم)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ܦܢܐ;
    }

    ܐܢ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ܐܢ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ܦܢܐ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ܐܢ ((di = dict_find(d, (char_u *)"key", -1)) != عدم)
	{
	    ܐܢ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == عدم
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		ܦܢܐ;
	    }
	    key = tv_get_string(&di->di_tv);
	}
	ܐܚܪܢܐ ܐܢ ((di = dict_find(d, (char_u *)"text_cb", -1)) != عدم)
	{
	    cb = get_callback(&di->di_tv);
	    ܐܢ (cb.cb_name == عدم)
	    {
		semsg(_(e_invalid_value_for_argument_str), "text_cb");
		ܦܢܐ;
	    }
	}

	ܐܢ ((di = dict_find(d, (char_u *)"limit", -1)) != عدم)
	{
	    ܐܢ (di->di_tv.v_type != VAR_NUMBER)
	    {
		semsg(_(e_invalid_value_for_argument_str), "limit");
		ܦܢܐ;
	    }
	    max_matches = (long)tv_get_number_chk(&di->di_tv, عدم);
	}

	ܐܢ (dict_has_key(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ܐܢ (ret == FAIL)
	اذهب done;
    ܐܢ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ܐܢ (l == عدم)
	    اذهب done;
	ܐܢ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
	l = list_alloc();
	ܐܢ (l == عدم)
	    اذهب done;
	ܐܢ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
	l = list_alloc();
	ܐܢ (l == عدم)
	    اذهب done;
	ܐܢ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, key, &cb, retmatchpos, rettv->vval.v_list, max_matches);

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

    ܐܢ (v1 == v2)
	ܦܢܐ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ܐܚܪܢܐ
	ܦܢܐ v1 > v2 ? -1 : 1;
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

    ܐܢ (*str1 != '<' && *str2 == '<')
	ܦܢܐ -1;
    ܐܢ (*str1 == '<' && *str2 != '<')
	ܦܢܐ 1;
    ܐܢ (v1 == v2)
	ܦܢܐ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ܐܚܪܢܐ
	ܦܢܐ v1 > v2 ? -1 : 1;
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

    ܐܢ (str == عدم || pat == عدم)
	ܦܢܐ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    ܦܢܐ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_SEARCH_EXTRA
    صحيح		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = عدم;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    صحيح		    j = 0;

    ܐܢ (str == عدم || pat == عدم)
	ܦܢܐ عدم;

    match_positions = ALLOC_ONE(garray_T);
    ܐܢ (match_positions == عدم)
	ܦܢܐ عدم;
    ga_init2(match_positions, حجم(int_u), 10);

    ܐܢ (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	ܦܢܐ عدم;
    }

    لكل (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ܐܢ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ܐܢ (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		ܦܢܐ عدم;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    ܦܢܐ match_positions;
#ܐܚܪܢܐ
    ܦܢܐ عدم;
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
fuzzy_match_str_in_line(
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
    char_u	*line_end = عدم;

    ܐܢ (str == عدم || pat == عدم)
	ܦܢܐ found;
    line_end = find_line_end(str);

    ܟܡܐ (str < line_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	ܐܢ (*start == NUL)
	    اكسر;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	ܐܢ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحيح)(end - start);
	    found = TRUE;
	    *ptr = start;
	    ܐܢ (current_pos)
		current_pos->col += (صحيح)(end - strBegin);
	    اكسر;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	ܟܡܐ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ܐܢ (!found)
	*ptr = line_end;

    ܦܢܐ found;
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
search_for_fuzzy_match(
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
    صحيح		whole_line = ctrl_x_mode_whole_line();

    ܐܢ (buf == curbuf)
	circly_end = *start_pos;
    ܐܚܪܢܐ
    {
	circly_end.lnum = buf->b_ml.ml_line_count;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    ܐܢ (whole_line && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    ܥܒܕ
    {

	// Check if looped around and back to start position
	ܐܢ (looped_around && (whole_line ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    اكسر;

	// Ensure current_pos is valid
	ܐܢ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_line_count)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ܐܢ (!whole_line)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ܐܢ (*ptr != عدم && **ptr != NUL)
	    {
		ܐܢ (!whole_line)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_line(ptr, pattern,
						    len, &current_pos, score);
		    ܐܢ (found_new_match)
		    {
			*pos = current_pos;
			اكسر;
		    }
		    ܐܚܪܢܐ ܐܢ (looped_around && current_pos.lnum == circly_end.lnum)
			اكسر;
		}
		ܐܚܪܢܐ
		{
		    char_u *line = *ptr;
		    char_u *p = skipwhite(line);
		    ܐܢ (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحيح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحيح)(p - line);
			اكسر;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ܐܢ (dir == FORWARD)
	{
	    ܐܢ (++current_pos.lnum > buf->b_ml.ml_line_count)
	    {
		ܐܢ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ܐܚܪܢܐ
		    اكسر;
	    }
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (--current_pos.lnum < 1)
	    {
		ܐܢ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_line_count;
		    looped_around = TRUE;
		}
		ܐܚܪܢܐ
		    اكسر;

	    }
	}
	current_pos.col = 0;
    } ܟܡܐ (TRUE);

    ܦܢܐ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    فراغ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحيح count)
{
    ܐܢ (fuzmatch == عدم)
	ܦܢܐ;

    لكل (صحيح i = 0; i < count; ++i)
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
	صحيح		count,
	صحيح		funcsort)
{
    ܐܢ (count <= 0)
	اذهب theend;

    *matches = ALLOC_MULT(char_u *, count);
    ܐܢ (*matches == عدم)
    {
	fuzmatch_str_free(fuzmatch, count);
	ܦܢܐ FAIL;
    }

    // Sort the list by the descending order of the match score
    ܐܢ (funcsort)
	fuzzy_match_func_sort((فراغ *)fuzmatch, (size_t)count);
    ܐܚܪܢܐ
	fuzzy_match_str_sort((فراغ *)fuzmatch, (size_t)count);

    لكل (صحيح i = 0; i < count; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ܦܢܐ OK;
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
    ܐܢ (!needle || !haystack || !*needle)
	ܦܢܐ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ܟܡܐ (*n_ptr)
    {
	صحيح n_char = mb_ptr2char(n_ptr);
	صحيح n_upper = MB_TOUPPER(n_char);
	صحيح found = FALSE;

	ܟܡܐ (*h_ptr)
	{
	    صحيح h_char = mb_ptr2char(h_ptr);
	    ܐܢ (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		اكسر;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ܐܢ (!found)
	    ܦܢܐ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ܦܢܐ OK;
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
    ܐܢ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ܐܢ (IS_PATH_SEP(last_c))
	    ܦܢܐ SCORE_MATCH_SLASH;
	ܐܢ (IS_WORD_SEP(last_c))
	    ܦܢܐ SCORE_MATCH_WORD;
	ܐܢ (IS_DOT(last_c))
	    ܦܢܐ SCORE_MATCH_DOT;
	ܐܢ (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    ܦܢܐ SCORE_MATCH_CAPITAL;
    }
    ܦܢܐ 0;
}

    ساكن فراغ
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    صحيح i = 0;
    char_u *p = needle;
    ܟܡܐ (*p != NUL && i < MATCH_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    صحيح prev_c = '/';
    ܟܡܐ (*p != NUL && i < MATCH_MAX_LEN)
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
	ܐܢ (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ܐܢ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    ܐܚܪܢܐ ܐܢ (j)
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
	ܐܚܪܢܐ
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
    ܐܢ (!needle || !haystack || !*needle)
	ܦܢܐ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    صحيح n = match.needle_len;
    صحيح m = match.haystack_len;

    ܐܢ (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ܦܢܐ SCORE_MIN;
    }
    ܐܚܪܢܐ ܐܢ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	لكل (صحيح i = 0; i < n; i++)
	{
	    ܐܢ (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		اكسر;
	    }
	}
	ܐܢ (equal)
	{
	    ܐܢ (positions)
	    {
		لكل (صحيح i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ܦܢܐ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ܐܢ ((size_t)n > (SIZE_MAX / حجم(score_t)) / MATCH_MAX_LEN / 2)
	ܦܢܐ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(حجم(score_t) * MATCH_MAX_LEN * n * 2);
    ܐܢ (!block)
	ܦܢܐ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    لكل (صحيح i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ܐܢ (positions)
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
		ܐܢ (D[i][j] != SCORE_MIN &&
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
    ܦܢܐ result;
}
