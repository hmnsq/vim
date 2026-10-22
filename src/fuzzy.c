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
#include "vim.h"

#ပေႃး defined(FEAT_EVAL) || defined(FEAT_PROTO)
static puur_naa_nka fuzzy_match_item_compare(const void *s1, const void *s2);
static void fuzzy_match_in_list(list_T *l, char_u *str, puur_naa_nka matchseq, char_u *key, callback_T *item_cb, puur_naa_nka retmatchpos, list_T *fmatchlist, long max_matches);
static void do_fuzzymatch(typval_T *argvars, typval_T *rettv, puur_naa_nka retmatchpos);
#endif
static puur_naa_nka fuzzy_match_str_compare(const void *s1, const void *s2);
static void fuzzy_match_str_sort(fuzmatch_str_T *fm, puur_naa_nka sz);
static puur_naa_nka fuzzy_match_func_compare(const void *s1, const void *s2);
static void fuzzy_match_func_sort(fuzmatch_str_T *fm, puur_naa_nka sz);

typedef dviguu_na score_t;
static score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
static puur_naa_nka has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

typedef sa_nracanaa
{
    puur_naa_nka		idx;		// used for stable sort
    listitem_T	*item;
    puur_naa_nka		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    puur_naa_nka		itemstr_allocated;
    puur_naa_nka		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    puur_naa_nka
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    puur_naa_nka		matchseq,
    puur_naa_nka		*outScore,
    int_u	*matches,
    puur_naa_nka		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    puur_naa_nka		complete = FALSE;
    puur_naa_nka		score = 0;
    puur_naa_nka		numMatches = 0;
    puur_naa_nka		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    ပေႃး (save_pat == NULL)
	ပွၵ်ႈ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ၶၢဝ်း (TRUE)
    {
	ပေႃး (matchseq)
	    complete = TRUE;
	ပေႃးဢမ်ႇ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ပေႃး (*p == NUL)
		bha_nga;
	    pat = p;
	    ၶၢဝ်း (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ပေႃး (has_mbyte)
		    MB_PTR_ADV(p);
		ပေႃးဢမ်ႇ
		    ++p;
	    }
	    ပေႃး (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ပေႃး (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ပေႃး (numMatches > maxMatches - pat_chars)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    bha_nga;
	}

	score = FUZZY_SCORE_NONE;
	ပေႃး (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + numMatches);
	    ပေႃး (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (puur_naa_nka)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (puur_naa_nka)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ပေႃး (score == FUZZY_SCORE_NONE)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    bha_nga;
	}

	ပေႃး (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	ပေႃးဢမ်ႇ ပေႃး (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	ပေႃးဢမ်ႇ
	    *outScore += score;

	numMatches += pat_chars;

	ပေႃး (complete || numMatches >= maxMatches)
	    bha_nga;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ပွၵ်ႈ numMatches != 0;
}

#ပေႃး defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    static puur_naa_nka
fuzzy_match_item_compare(const void *s1, const void *s2)
{
    puur_naa_nka		v1 = ((fuzzyItem_T *)s1)->score;
    puur_naa_nka		v2 = ((fuzzyItem_T *)s2)->score;

    ပေႃး (v1 == v2)
    {
	puur_naa_nka exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	puur_naa_nka patlen = (puur_naa_nka)STRLEN(pat);
	puur_naa_nka startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	ပေႃး (exact_match1 == exact_match2)
	{
	    puur_naa_nka idx1 = ((fuzzyItem_T *)s1)->idx;
	    puur_naa_nka idx2 = ((fuzzyItem_T *)s2)->idx;
	    ပွၵ်ႈ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ပေႃးဢမ်ႇ ပေႃး (exact_match2)
	    ပွၵ်ႈ 1;
	ပွၵ်ႈ -1;
    }
    ပေႃးဢမ်ႇ
	ပွၵ်ႈ v1 > v2 ? -1 : 1;
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
    static void
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    puur_naa_nka		matchseq,
    char_u	*key,
    callback_T	*item_cb,
    puur_naa_nka		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *items = NULL;
    listitem_T	    *li;
    long	    match_count = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    ပေႃး (len == 0)
	ပွၵ်ႈ;
    ပေႃး (max_matches > 0 && len > max_matches)
	len = max_matches;

    items = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    ပေႃး (items == NULL)
	ပွၵ်ႈ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ITEMS(l, li)
    {
	puur_naa_nka		score;
	char_u		*itemstr = NULL;
	char_u		*itemstr_copy = NULL;
	typval_T	rettv;
	puur_naa_nka		itemstr_allocate = FALSE;
	list_T		*match_positions = NULL;

	ပေႃး (max_matches > 0 && match_count >= max_matches)
	    bha_nga;

	rettv.v_type = VAR_UNKNOWN;
	ပေႃး (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ပေႃးဢမ်ႇ ပေႃး (li->li_tv.v_type == VAR_DICT
				&& (key != NULL || item_cb->cb_name != NULL))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ပေႃး (key != NULL)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (var_na *)key, FALSE);
	    ပေႃးဢမ်ႇ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ပေႃး (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ပေႃး (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ပေႃး (itemstr != NULL
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    ပေႃး (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ပေႃး (itemstr_copy == NULL)
		{
		    clear_tv(&rettv);
		    continue;
		}
	    }
	    ပေႃးဢမ်ႇ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ပေႃး (retmatchpos)
	    {
		match_positions = list_alloc();
		ပေႃး (match_positions == NULL)
		{
		    ပေႃး (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    continue;
		}

		// Fill position information
		puur_naa_nka	j = 0;
		char_u	*p = str;
		puur_naa_nka	success = TRUE;

		ၶၢဝ်း (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    ပေႃး (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ပေႃး (list_append_number(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    bha_nga;
			}
			j++;
		    }
		    ပေႃး (has_mbyte)
			MB_PTR_ADV(p);
		    ပေႃးဢမ်ႇ
			++p;
		}

		ပေႃး (!success)
		{
		    list_free(match_positions);
		    ပေႃး (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    continue;
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

    ပေႃး (match_count > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((void *)items, (size_t)match_count, parimaa_na(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ပေႃး (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    ပေႃး (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	}
	ပေႃးဢမ်ႇ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	for (puur_naa_nka i = 0; i < match_count; i++)
	{
	    ပေႃး (list_append_tv(retlist, &items[i].item->li_tv) == FAIL)
		goto done;
	}

	// next copy the list of matching positions
	ပေႃး (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    ပေႃး (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;

	    for (puur_naa_nka i = 0; i < match_count; i++)
	    {
		ပေႃး (items[i].lmatchpos != NULL)
		{
		    ပေႃး (list_append_list(retlist, items[i].lmatchpos) == OK)
			items[i].lmatchpos = NULL;
		    ပေႃးဢမ်ႇ
			goto done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    ပေႃး (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	    for (puur_naa_nka i = 0; i < match_count; i++)
	    {
		ပေႃး (list_append_number(retlist, items[i].score) == FAIL)
		    goto done;
	    }
	}
    }

done:
    for (puur_naa_nka i = 0; i < match_count; i++)
    {
	ပေႃး (items[i].itemstr_allocated)
	    vim_free(items[i].itemstr);

	ပေႃး (items[i].lmatchpos)
	    list_free(items[i].lmatchpos);
    }
    vim_free(items);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    static void
do_fuzzymatch(typval_T *argvars, typval_T *rettv, puur_naa_nka retmatchpos)
{
    callback_T	cb;
    char_u	*key = NULL;
    puur_naa_nka		ret;
    puur_naa_nka		matchseq = FALSE;
    long	max_matches = 0;

    ပေႃး (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ပွၵ်ႈ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    ပေႃး (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == NULL)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ပွၵ်ႈ;
    }
    ပေႃး (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == NULL)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ပွၵ်ႈ;
    }

    ပေႃး (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ပေႃး (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ပွၵ်ႈ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ပေႃး ((di = dict_find(d, (char_u *)"key", -1)) != NULL)
	{
	    ပေႃး (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == NULL
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		ပွၵ်ႈ;
	    }
	    key = tv_get_string(&di->di_tv);
	}
	ပေႃးဢမ်ႇ ပေႃး ((di = dict_find(d, (char_u *)"text_cb", -1)) != NULL)
	{
	    cb = get_callback(&di->di_tv);
	    ပေႃး (cb.cb_name == NULL)
	    {
		semsg(_(e_invalid_value_for_argument_str), "text_cb");
		ပွၵ်ႈ;
	    }
	}

	ပေႃး ((di = dict_find(d, (char_u *)"limit", -1)) != NULL)
	{
	    ပေႃး (di->di_tv.v_type != VAR_NUMBER)
	    {
		semsg(_(e_invalid_value_for_argument_str), "limit");
		ပွၵ်ႈ;
	    }
	    max_matches = (long)tv_get_number_chk(&di->di_tv, NULL);
	}

	ပေႃး (dict_has_key(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ပေႃး (ret == FAIL)
	goto done;
    ပေႃး (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ပေႃး (l == NULL)
	    goto done;
	ပေႃး (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	ပေႃး (l == NULL)
	    goto done;
	ပေႃး (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	ပေႃး (l == NULL)
	    goto done;
	ပေႃး (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
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
    void
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    void
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    static puur_naa_nka
fuzzy_match_str_compare(const void *s1, const void *s2)
{
    puur_naa_nka		v1 = ((fuzmatch_str_T *)s1)->score;
    puur_naa_nka		v2 = ((fuzmatch_str_T *)s2)->score;
    puur_naa_nka		idx1 = ((fuzmatch_str_T *)s1)->idx;
    puur_naa_nka		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ပေႃး (v1 == v2)
	ပွၵ်ႈ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ပေႃးဢမ်ႇ
	ပွၵ်ႈ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    static void
fuzzy_match_str_sort(fuzmatch_str_T *fm, puur_naa_nka sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, parimaa_na(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    static puur_naa_nka
fuzzy_match_func_compare(const void *s1, const void *s2)
{
    puur_naa_nka		v1 = ((fuzmatch_str_T *)s1)->score;
    puur_naa_nka		v2 = ((fuzmatch_str_T *)s2)->score;
    puur_naa_nka		idx1 = ((fuzmatch_str_T *)s1)->idx;
    puur_naa_nka		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ပေႃး (*str1 != '<' && *str2 == '<')
	ပွၵ်ႈ -1;
    ပေႃး (*str1 == '<' && *str2 != '<')
	ပွၵ်ႈ 1;
    ပေႃး (v1 == v2)
	ပွၵ်ႈ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ပေႃးဢမ်ႇ
	ပွၵ်ႈ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    static void
fuzzy_match_func_sort(fuzmatch_str_T *fm, puur_naa_nka sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, parimaa_na(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    puur_naa_nka
fuzzy_match_str(char_u *str, char_u *pat)
{
    puur_naa_nka		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    ပေႃး (str == NULL || pat == NULL)
	ပွၵ်ႈ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				parimaa_na(matchpos) / parimaa_na(matchpos[0]));

    ပွၵ်ႈ score;
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
    puur_naa_nka		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = NULL;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    puur_naa_nka		    j = 0;

    ပေႃး (str == NULL || pat == NULL)
	ပွၵ်ႈ NULL;

    match_positions = ALLOC_ONE(garray_T);
    ပေႃး (match_positions == NULL)
	ပွၵ်ႈ NULL;
    ga_init2(match_positions, parimaa_na(int_u), 10);

    ပေႃး (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	ပွၵ်ႈ NULL;
    }

    for (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ပေႃး (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ပေႃး (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		ပွၵ်ႈ NULL;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    ပွၵ်ႈ match_positions;
#ပေႃးဢမ်ႇ
    ပွၵ်ႈ NULL;
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
    puur_naa_nka
fuzzy_match_str_in_line(
    char_u	**ptr,
    char_u	*pat,
    puur_naa_nka		*len,
    pos_T	*current_pos,
    puur_naa_nka		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = NULL;
    char_u	*start = NULL;
    puur_naa_nka		found = FALSE;
    var_na	save_end;
    char_u	*line_end = NULL;

    ပေႃး (str == NULL || pat == NULL)
	ပွၵ်ႈ found;
    line_end = find_line_end(str);

    ၶၢဝ်း (str < line_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	ပေႃး (*start == NUL)
	    bha_nga;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	ပေႃး (*score != FUZZY_SCORE_NONE)
	{
	    *len = (puur_naa_nka)(end - start);
	    found = TRUE;
	    *ptr = start;
	    ပေႃး (current_pos)
		current_pos->col += (puur_naa_nka)(end - strBegin);
	    bha_nga;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	ၶၢဝ်း (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ပေႃး (!found)
	*ptr = line_end;

    ပွၵ်ႈ found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    puur_naa_nka
search_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    puur_naa_nka		dir,
    pos_T	*start_pos,
    puur_naa_nka		*len,
    char_u	**ptr,
    puur_naa_nka		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    puur_naa_nka		found_new_match = FALSE;
    puur_naa_nka		looped_around = FALSE;
    puur_naa_nka		whole_line = ctrl_x_mode_whole_line();

    ပေႃး (buf == curbuf)
	circly_end = *start_pos;
    ပေႃးဢမ်ႇ
    {
	circly_end.lnum = buf->b_ml.ml_line_count;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    ပေႃး (whole_line && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    do
    {

	// Check if looped around and back to start position
	ပေႃး (looped_around && (whole_line ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    bha_nga;

	// Ensure current_pos is valid
	ပေႃး (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_line_count)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ပေႃး (!whole_line)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ပေႃး (*ptr != NULL && **ptr != NUL)
	    {
		ပေႃး (!whole_line)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_line(ptr, pattern,
						    len, &current_pos, score);
		    ပေႃး (found_new_match)
		    {
			*pos = current_pos;
			bha_nga;
		    }
		    ပေႃးဢမ်ႇ ပေႃး (looped_around && current_pos.lnum == circly_end.lnum)
			bha_nga;
		}
		ပေႃးဢမ်ႇ
		{
		    char_u *line = *ptr;
		    char_u *p = skipwhite(line);
		    ပေႃး (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (puur_naa_nka)ml_get_buf_len(buf, current_pos.lnum) -
								(puur_naa_nka)(p - line);
			bha_nga;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ပေႃး (dir == FORWARD)
	{
	    ပေႃး (++current_pos.lnum > buf->b_ml.ml_line_count)
	    {
		ပေႃး (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ပေႃးဢမ်ႇ
		    bha_nga;
	    }
	}
	ပေႃးဢမ်ႇ
	{
	    ပေႃး (--current_pos.lnum < 1)
	    {
		ပေႃး (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_line_count;
		    looped_around = TRUE;
		}
		ပေႃးဢမ်ႇ
		    bha_nga;

	    }
	}
	current_pos.col = 0;
    } ၶၢဝ်း (TRUE);

    ပွၵ်ႈ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    void
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, puur_naa_nka count)
{
    ပေႃး (fuzmatch == NULL)
	ပွၵ်ႈ;

    for (puur_naa_nka i = 0; i < count; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    puur_naa_nka
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	puur_naa_nka		count,
	puur_naa_nka		funcsort)
{
    ပေႃး (count <= 0)
	goto theend;

    *matches = ALLOC_MULT(char_u *, count);
    ပေႃး (*matches == NULL)
    {
	fuzmatch_str_free(fuzmatch, count);
	ပွၵ်ႈ FAIL;
    }

    // Sort the list by the descending order of the match score
    ပေႃး (funcsort)
	fuzzy_match_func_sort((void *)fuzmatch, (size_t)count);
    ပေႃးဢမ်ႇ
	fuzzy_match_str_sort((void *)fuzmatch, (size_t)count);

    for (puur_naa_nka i = 0; i < count; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ပွၵ်ႈ OK;
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

    static puur_naa_nka
has_match(char_u *needle, char_u *haystack)
{
    ပေႃး (!needle || !haystack || !*needle)
	ပွၵ်ႈ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ၶၢဝ်း (*n_ptr)
    {
	puur_naa_nka n_char = mb_ptr2char(n_ptr);
	puur_naa_nka n_upper = MB_TOUPPER(n_char);
	puur_naa_nka found = FALSE;

	ၶၢဝ်း (*h_ptr)
	{
	    puur_naa_nka h_char = mb_ptr2char(h_ptr);
	    ပေႃး (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		bha_nga;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ပေႃး (!found)
	    ပွၵ်ႈ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ပွၵ်ႈ OK;
}

typedef sa_nracanaa match_struct
{
    puur_naa_nka needle_len;
    puur_naa_nka haystack_len;
    puur_naa_nka lower_needle[MATCH_MAX_LEN];     // stores codepoints
    puur_naa_nka lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    static score_t
compute_bonus_codepoint(puur_naa_nka last_c, puur_naa_nka c)
{
    ပေႃး (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ပေႃး (IS_PATH_SEP(last_c))
	    ပွၵ်ႈ SCORE_MATCH_SLASH;
	ပေႃး (IS_WORD_SEP(last_c))
	    ပွၵ်ႈ SCORE_MATCH_WORD;
	ပေႃး (IS_DOT(last_c))
	    ပွၵ်ႈ SCORE_MATCH_DOT;
	ပေႃး (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    ပွၵ်ႈ SCORE_MATCH_CAPITAL;
    }
    ပွၵ်ႈ 0;
}

    static void
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    puur_naa_nka i = 0;
    char_u *p = needle;
    ၶၢဝ်း (*p != NUL && i < MATCH_MAX_LEN)
    {
	puur_naa_nka c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    puur_naa_nka prev_c = '/';
    ၶၢဝ်း (*p != NUL && i < MATCH_MAX_LEN)
    {
	puur_naa_nka c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    static anta_hstha void
match_row(const match_struct *match, puur_naa_nka row, score_t *curr_D,
	score_t *curr_M, const score_t *last_D, const score_t *last_M)
{
    puur_naa_nka n = match->needle_len;
    puur_naa_nka m = match->haystack_len;
    puur_naa_nka i = row;

    const puur_naa_nka *lower_needle = match->lower_needle;
    const puur_naa_nka *lower_haystack = match->lower_haystack;
    const score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    for (puur_naa_nka j = 0; j < m; j++)
    {
	ပေႃး (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ပေႃး (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    ပေႃးဢမ်ႇ ပေႃး (j)
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
	ပေႃးဢမ်ႇ
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    static score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ပေႃး (!needle || !haystack || !*needle)
	ပွၵ်ႈ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    puur_naa_nka n = match.needle_len;
    puur_naa_nka m = match.haystack_len;

    ပေႃး (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ပွၵ်ႈ SCORE_MIN;
    }
    ပေႃးဢမ်ႇ ပေႃး (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	for (puur_naa_nka i = 0; i < n; i++)
	{
	    ပေႃး (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		bha_nga;
	    }
	}
	ပေႃး (equal)
	{
	    ပေႃး (positions)
	    {
		for (puur_naa_nka i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ပွၵ်ႈ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ပေႃး ((size_t)n > (SIZE_MAX / parimaa_na(score_t)) / MATCH_MAX_LEN / 2)
	ပွၵ်ႈ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(parimaa_na(score_t) * MATCH_MAX_LEN * n * 2);
    ပေႃး (!block)
	ပွၵ်ႈ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    for (puur_naa_nka i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ပေႃး (positions)
    {
	puur_naa_nka match_required = 0;
	for (puur_naa_nka i = n - 1, j = m - 1; i >= 0; i--)
	{
	    for (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ပေႃး (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    bha_nga;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    ပွၵ်ႈ result;
}
