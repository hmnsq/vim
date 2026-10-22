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

#nếu defined(FEAT_EVAL) || defined(FEAT_PROTO)
static số_nguyên fuzzy_match_item_compare(const void *s1, const void *s2);
static void fuzzy_match_in_list(list_T *l, char_u *str, số_nguyên matchseq, char_u *key, callback_T *item_cb, số_nguyên retmatchpos, list_T *fmatchlist, long max_matches);
static void do_fuzzymatch(typval_T *argvars, typval_T *rettv, số_nguyên retmatchpos);
#endif
static số_nguyên fuzzy_match_str_compare(const void *s1, const void *s2);
static void fuzzy_match_str_sort(fuzmatch_str_T *fm, số_nguyên sz);
static số_nguyên fuzzy_match_func_compare(const void *s1, const void *s2);
static void fuzzy_match_func_sort(fuzmatch_str_T *fm, số_nguyên sz);

typedef double score_t;
static score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
static số_nguyên has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

typedef struct
{
    số_nguyên		idx;		// used for stable sort
    listitem_T	*item;
    số_nguyên		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    số_nguyên		itemstr_allocated;
    số_nguyên		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    số_nguyên
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    số_nguyên		matchseq,
    số_nguyên		*outScore,
    int_u	*matches,
    số_nguyên		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    số_nguyên		complete = FALSE;
    số_nguyên		score = 0;
    số_nguyên		numMatches = 0;
    số_nguyên		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    nếu (save_pat == NULL)
	trả_về FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    trong_khi (TRUE)
    {
	nếu (matchseq)
	    complete = TRUE;
	không_thì
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    nếu (*p == NUL)
		break;
	    pat = p;
	    trong_khi (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		nếu (has_mbyte)
		    MB_PTR_ADV(p);
		không_thì
		    ++p;
	    }
	    nếu (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	nếu (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	nếu (numMatches > maxMatches - pat_chars)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    break;
	}

	score = FUZZY_SCORE_NONE;
	nếu (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + numMatches);
	    nếu (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (số_nguyên)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (số_nguyên)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	nếu (score == FUZZY_SCORE_NONE)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    break;
	}

	nếu (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	không_thì nếu (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	không_thì
	    *outScore += score;

	numMatches += pat_chars;

	nếu (complete || numMatches >= maxMatches)
	    break;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    trả_về numMatches != 0;
}

#nếu defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    static số_nguyên
fuzzy_match_item_compare(const void *s1, const void *s2)
{
    số_nguyên		v1 = ((fuzzyItem_T *)s1)->score;
    số_nguyên		v2 = ((fuzzyItem_T *)s2)->score;

    nếu (v1 == v2)
    {
	số_nguyên exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	số_nguyên patlen = (số_nguyên)STRLEN(pat);
	số_nguyên startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	nếu (exact_match1 == exact_match2)
	{
	    số_nguyên idx1 = ((fuzzyItem_T *)s1)->idx;
	    số_nguyên idx2 = ((fuzzyItem_T *)s2)->idx;
	    trả_về idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	không_thì nếu (exact_match2)
	    trả_về 1;
	trả_về -1;
    }
    không_thì
	trả_về v1 > v2 ? -1 : 1;
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
    số_nguyên		matchseq,
    char_u	*key,
    callback_T	*item_cb,
    số_nguyên		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *items = NULL;
    listitem_T	    *li;
    long	    match_count = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    nếu (len == 0)
	trả_về;
    nếu (max_matches > 0 && len > max_matches)
	len = max_matches;

    items = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    nếu (items == NULL)
	trả_về;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ITEMS(l, li)
    {
	số_nguyên		score;
	char_u		*itemstr = NULL;
	char_u		*itemstr_copy = NULL;
	typval_T	rettv;
	số_nguyên		itemstr_allocate = FALSE;
	list_T		*match_positions = NULL;

	nếu (max_matches > 0 && match_count >= max_matches)
	    break;

	rettv.v_type = VAR_UNKNOWN;
	nếu (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	không_thì nếu (li->li_tv.v_type == VAR_DICT
				&& (key != NULL || item_cb->cb_name != NULL))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    nếu (key != NULL)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (char *)key, FALSE);
	    không_thì
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		nếu (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    nếu (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	nếu (itemstr != NULL
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    nếu (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		nếu (itemstr_copy == NULL)
		{
		    clear_tv(&rettv);
		    continue;
		}
	    }
	    không_thì
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    nếu (retmatchpos)
	    {
		match_positions = list_alloc();
		nếu (match_positions == NULL)
		{
		    nếu (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    continue;
		}

		// Fill position information
		số_nguyên	j = 0;
		char_u	*p = str;
		số_nguyên	success = TRUE;

		trong_khi (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    nếu (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			nếu (list_append_number(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    break;
			}
			j++;
		    }
		    nếu (has_mbyte)
			MB_PTR_ADV(p);
		    không_thì
			++p;
		}

		nếu (!success)
		{
		    list_free(match_positions);
		    nếu (itemstr_allocate && itemstr_copy)
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

    nếu (match_count > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((void *)items, (size_t)match_count, sizeof(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	nếu (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    nếu (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	}
	không_thì
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	cho (số_nguyên i = 0; i < match_count; i++)
	{
	    nếu (list_append_tv(retlist, &items[i].item->li_tv) == FAIL)
		goto done;
	}

	// next copy the list of matching positions
	nếu (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    nếu (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;

	    cho (số_nguyên i = 0; i < match_count; i++)
	    {
		nếu (items[i].lmatchpos != NULL)
		{
		    nếu (list_append_list(retlist, items[i].lmatchpos) == OK)
			items[i].lmatchpos = NULL;
		    không_thì
			goto done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    nếu (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	    cho (số_nguyên i = 0; i < match_count; i++)
	    {
		nếu (list_append_number(retlist, items[i].score) == FAIL)
		    goto done;
	    }
	}
    }

done:
    cho (số_nguyên i = 0; i < match_count; i++)
    {
	nếu (items[i].itemstr_allocated)
	    vim_free(items[i].itemstr);

	nếu (items[i].lmatchpos)
	    list_free(items[i].lmatchpos);
    }
    vim_free(items);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    static void
do_fuzzymatch(typval_T *argvars, typval_T *rettv, số_nguyên retmatchpos)
{
    callback_T	cb;
    char_u	*key = NULL;
    số_nguyên		ret;
    số_nguyên		matchseq = FALSE;
    long	max_matches = 0;

    nếu (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	trả_về;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    nếu (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == NULL)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	trả_về;
    }
    nếu (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == NULL)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	trả_về;
    }

    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	nếu (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    trả_về;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	nếu ((di = dict_find(d, (char_u *)"key", -1)) != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == NULL
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		trả_về;
	    }
	    key = tv_get_string(&di->di_tv);
	}
	không_thì nếu ((di = dict_find(d, (char_u *)"text_cb", -1)) != NULL)
	{
	    cb = get_callback(&di->di_tv);
	    nếu (cb.cb_name == NULL)
	    {
		semsg(_(e_invalid_value_for_argument_str), "text_cb");
		trả_về;
	    }
	}

	nếu ((di = dict_find(d, (char_u *)"limit", -1)) != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_NUMBER)
	    {
		semsg(_(e_invalid_value_for_argument_str), "limit");
		trả_về;
	    }
	    max_matches = (long)tv_get_number_chk(&di->di_tv, NULL);
	}

	nếu (dict_has_key(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    nếu (ret == FAIL)
	goto done;
    nếu (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	nếu (l == NULL)
	    goto done;
	nếu (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	nếu (l == NULL)
	    goto done;
	nếu (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	nếu (l == NULL)
	    goto done;
	nếu (list_append_list(rettv->vval.v_list, l) == FAIL)
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
    static số_nguyên
fuzzy_match_str_compare(const void *s1, const void *s2)
{
    số_nguyên		v1 = ((fuzmatch_str_T *)s1)->score;
    số_nguyên		v2 = ((fuzmatch_str_T *)s2)->score;
    số_nguyên		idx1 = ((fuzmatch_str_T *)s1)->idx;
    số_nguyên		idx2 = ((fuzmatch_str_T *)s2)->idx;

    nếu (v1 == v2)
	trả_về idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    không_thì
	trả_về v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    static void
fuzzy_match_str_sort(fuzmatch_str_T *fm, số_nguyên sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, sizeof(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    static số_nguyên
fuzzy_match_func_compare(const void *s1, const void *s2)
{
    số_nguyên		v1 = ((fuzmatch_str_T *)s1)->score;
    số_nguyên		v2 = ((fuzmatch_str_T *)s2)->score;
    số_nguyên		idx1 = ((fuzmatch_str_T *)s1)->idx;
    số_nguyên		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    nếu (*str1 != '<' && *str2 == '<')
	trả_về -1;
    nếu (*str1 == '<' && *str2 != '<')
	trả_về 1;
    nếu (v1 == v2)
	trả_về idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    không_thì
	trả_về v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    static void
fuzzy_match_func_sort(fuzmatch_str_T *fm, số_nguyên sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, sizeof(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    số_nguyên
fuzzy_match_str(char_u *str, char_u *pat)
{
    số_nguyên		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    nếu (str == NULL || pat == NULL)
	trả_về score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				sizeof(matchpos) / sizeof(matchpos[0]));

    trả_về score;
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
    số_nguyên		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = NULL;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    số_nguyên		    j = 0;

    nếu (str == NULL || pat == NULL)
	trả_về NULL;

    match_positions = ALLOC_ONE(garray_T);
    nếu (match_positions == NULL)
	trả_về NULL;
    ga_init2(match_positions, sizeof(int_u), 10);

    nếu (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	trả_về NULL;
    }

    cho (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	nếu (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    nếu (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		trả_về NULL;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    trả_về match_positions;
#không_thì
    trả_về NULL;
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
    số_nguyên
fuzzy_match_str_in_line(
    char_u	**ptr,
    char_u	*pat,
    số_nguyên		*len,
    pos_T	*current_pos,
    số_nguyên		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = NULL;
    char_u	*start = NULL;
    số_nguyên		found = FALSE;
    char	save_end;
    char_u	*line_end = NULL;

    nếu (str == NULL || pat == NULL)
	trả_về found;
    line_end = find_line_end(str);

    trong_khi (str < line_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	nếu (*start == NUL)
	    break;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	nếu (*score != FUZZY_SCORE_NONE)
	{
	    *len = (số_nguyên)(end - start);
	    found = TRUE;
	    *ptr = start;
	    nếu (current_pos)
		current_pos->col += (số_nguyên)(end - strBegin);
	    break;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	trong_khi (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    nếu (!found)
	*ptr = line_end;

    trả_về found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    số_nguyên
search_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    số_nguyên		dir,
    pos_T	*start_pos,
    số_nguyên		*len,
    char_u	**ptr,
    số_nguyên		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    số_nguyên		found_new_match = FALSE;
    số_nguyên		looped_around = FALSE;
    số_nguyên		whole_line = ctrl_x_mode_whole_line();

    nếu (buf == curbuf)
	circly_end = *start_pos;
    không_thì
    {
	circly_end.lnum = buf->b_ml.ml_line_count;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    nếu (whole_line && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    do
    {

	// Check if looped around and back to start position
	nếu (looped_around && (whole_line ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    break;

	// Ensure current_pos is valid
	nếu (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_line_count)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    nếu (!whole_line)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    nếu (*ptr != NULL && **ptr != NUL)
	    {
		nếu (!whole_line)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_line(ptr, pattern,
						    len, &current_pos, score);
		    nếu (found_new_match)
		    {
			*pos = current_pos;
			break;
		    }
		    không_thì nếu (looped_around && current_pos.lnum == circly_end.lnum)
			break;
		}
		không_thì
		{
		    char_u *line = *ptr;
		    char_u *p = skipwhite(line);
		    nếu (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (số_nguyên)ml_get_buf_len(buf, current_pos.lnum) -
								(số_nguyên)(p - line);
			break;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	nếu (dir == FORWARD)
	{
	    nếu (++current_pos.lnum > buf->b_ml.ml_line_count)
	    {
		nếu (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		không_thì
		    break;
	    }
	}
	không_thì
	{
	    nếu (--current_pos.lnum < 1)
	    {
		nếu (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_line_count;
		    looped_around = TRUE;
		}
		không_thì
		    break;

	    }
	}
	current_pos.col = 0;
    } trong_khi (TRUE);

    trả_về found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    void
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, số_nguyên count)
{
    nếu (fuzmatch == NULL)
	trả_về;

    cho (số_nguyên i = 0; i < count; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    số_nguyên
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	số_nguyên		count,
	số_nguyên		funcsort)
{
    nếu (count <= 0)
	goto theend;

    *matches = ALLOC_MULT(char_u *, count);
    nếu (*matches == NULL)
    {
	fuzmatch_str_free(fuzmatch, count);
	trả_về FAIL;
    }

    // Sort the list by the descending order of the match score
    nếu (funcsort)
	fuzzy_match_func_sort((void *)fuzmatch, (size_t)count);
    không_thì
	fuzzy_match_str_sort((void *)fuzmatch, (size_t)count);

    cho (số_nguyên i = 0; i < count; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    trả_về OK;
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

    static số_nguyên
has_match(char_u *needle, char_u *haystack)
{
    nếu (!needle || !haystack || !*needle)
	trả_về FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    trong_khi (*n_ptr)
    {
	số_nguyên n_char = mb_ptr2char(n_ptr);
	số_nguyên n_upper = MB_TOUPPER(n_char);
	số_nguyên found = FALSE;

	trong_khi (*h_ptr)
	{
	    số_nguyên h_char = mb_ptr2char(h_ptr);
	    nếu (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		break;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	nếu (!found)
	    trả_về FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    trả_về OK;
}

typedef struct match_struct
{
    số_nguyên needle_len;
    số_nguyên haystack_len;
    số_nguyên lower_needle[MATCH_MAX_LEN];     // stores codepoints
    số_nguyên lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    static score_t
compute_bonus_codepoint(số_nguyên last_c, số_nguyên c)
{
    nếu (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	nếu (IS_PATH_SEP(last_c))
	    trả_về SCORE_MATCH_SLASH;
	nếu (IS_WORD_SEP(last_c))
	    trả_về SCORE_MATCH_WORD;
	nếu (IS_DOT(last_c))
	    trả_về SCORE_MATCH_DOT;
	nếu (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    trả_về SCORE_MATCH_CAPITAL;
    }
    trả_về 0;
}

    static void
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    số_nguyên i = 0;
    char_u *p = needle;
    trong_khi (*p != NUL && i < MATCH_MAX_LEN)
    {
	số_nguyên c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    số_nguyên prev_c = '/';
    trong_khi (*p != NUL && i < MATCH_MAX_LEN)
    {
	số_nguyên c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    static inline void
match_row(const match_struct *match, số_nguyên row, score_t *curr_D,
	score_t *curr_M, const score_t *last_D, const score_t *last_M)
{
    số_nguyên n = match->needle_len;
    số_nguyên m = match->haystack_len;
    số_nguyên i = row;

    const số_nguyên *lower_needle = match->lower_needle;
    const số_nguyên *lower_haystack = match->lower_haystack;
    const score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    cho (số_nguyên j = 0; j < m; j++)
    {
	nếu (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    nếu (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    không_thì nếu (j)
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
	không_thì
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
    nếu (!needle || !haystack || !*needle)
	trả_về SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    số_nguyên n = match.needle_len;
    số_nguyên m = match.haystack_len;

    nếu (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	trả_về SCORE_MIN;
    }
    không_thì nếu (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	cho (số_nguyên i = 0; i < n; i++)
	{
	    nếu (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		break;
	    }
	}
	nếu (equal)
	{
	    nếu (positions)
	    {
		cho (số_nguyên i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    trả_về SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    nếu ((size_t)n > (SIZE_MAX / sizeof(score_t)) / MATCH_MAX_LEN / 2)
	trả_về SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(sizeof(score_t) * MATCH_MAX_LEN * n * 2);
    nếu (!block)
	trả_về SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    cho (số_nguyên i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    nếu (positions)
    {
	số_nguyên match_required = 0;
	cho (số_nguyên i = n - 1, j = m - 1; i >= 0; i--)
	{
	    cho (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		nếu (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    break;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    trả_về result;
}
