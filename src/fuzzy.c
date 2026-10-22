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

#ако defined(FEAT_EVAL) || defined(FEAT_PROTO)
static цео fuzzy_match_item_compare(const void *s1, const void *s2);
static void fuzzy_match_in_list(list_T *l, char_u *str, цео matchseq, char_u *key, callback_T *item_cb, цео retmatchpos, list_T *fmatchlist, long max_matches);
static void do_fuzzymatch(typval_T *argvars, typval_T *rettv, цео retmatchpos);
#endif
static цео fuzzy_match_str_compare(const void *s1, const void *s2);
static void fuzzy_match_str_sort(fuzmatch_str_T *fm, цео sz);
static цео fuzzy_match_func_compare(const void *s1, const void *s2);
static void fuzzy_match_func_sort(fuzmatch_str_T *fm, цео sz);

typedef double score_t;
static score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
static цео has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

typedef struct
{
    цео		idx;		// used for stable sort
    listitem_T	*item;
    цео		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    цео		itemstr_allocated;
    цео		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    цео
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    цео		matchseq,
    цео		*outScore,
    int_u	*matches,
    цео		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    цео		complete = FALSE;
    цео		score = 0;
    цео		numMatches = 0;
    цео		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    ако (save_pat == NULL)
	врати FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    док (TRUE)
    {
	ако (matchseq)
	    complete = TRUE;
	инако
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ако (*p == NUL)
		break;
	    pat = p;
	    док (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ако (has_mbyte)
		    MB_PTR_ADV(p);
		инако
		    ++p;
	    }
	    ако (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ако (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ако (numMatches > maxMatches - pat_chars)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    break;
	}

	score = FUZZY_SCORE_NONE;
	ако (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + numMatches);
	    ако (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (цео)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (цео)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ако (score == FUZZY_SCORE_NONE)
	{
	    numMatches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    break;
	}

	ако (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	инако ако (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	инако
	    *outScore += score;

	numMatches += pat_chars;

	ако (complete || numMatches >= maxMatches)
	    break;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    врати numMatches != 0;
}

#ако defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    static цео
fuzzy_match_item_compare(const void *s1, const void *s2)
{
    цео		v1 = ((fuzzyItem_T *)s1)->score;
    цео		v2 = ((fuzzyItem_T *)s2)->score;

    ако (v1 == v2)
    {
	цео exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	цео patlen = (цео)STRLEN(pat);
	цео startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	ако (exact_match1 == exact_match2)
	{
	    цео idx1 = ((fuzzyItem_T *)s1)->idx;
	    цео idx2 = ((fuzzyItem_T *)s2)->idx;
	    врати idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	инако ако (exact_match2)
	    врати 1;
	врати -1;
    }
    инако
	врати v1 > v2 ? -1 : 1;
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
    цео		matchseq,
    char_u	*key,
    callback_T	*item_cb,
    цео		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *items = NULL;
    listitem_T	    *li;
    long	    match_count = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    ако (len == 0)
	врати;
    ако (max_matches > 0 && len > max_matches)
	len = max_matches;

    items = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    ако (items == NULL)
	врати;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ITEMS(l, li)
    {
	цео		score;
	char_u		*itemstr = NULL;
	char_u		*itemstr_copy = NULL;
	typval_T	rettv;
	цео		itemstr_allocate = FALSE;
	list_T		*match_positions = NULL;

	ако (max_matches > 0 && match_count >= max_matches)
	    break;

	rettv.v_type = VAR_UNKNOWN;
	ако (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	инако ако (li->li_tv.v_type == VAR_DICT
				&& (key != NULL || item_cb->cb_name != NULL))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ако (key != NULL)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (char *)key, FALSE);
	    инако
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ако (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ако (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ако (itemstr != NULL
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    ако (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ако (itemstr_copy == NULL)
		{
		    clear_tv(&rettv);
		    continue;
		}
	    }
	    инако
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ако (retmatchpos)
	    {
		match_positions = list_alloc();
		ако (match_positions == NULL)
		{
		    ако (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    continue;
		}

		// Fill position information
		цео	j = 0;
		char_u	*p = str;
		цео	success = TRUE;

		док (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    ако (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ако (list_append_number(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    break;
			}
			j++;
		    }
		    ако (has_mbyte)
			MB_PTR_ADV(p);
		    инако
			++p;
		}

		ако (!success)
		{
		    list_free(match_positions);
		    ако (itemstr_allocate && itemstr_copy)
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

    ако (match_count > 0)
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
	ако (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    ако (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	}
	инако
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	за (цео i = 0; i < match_count; i++)
	{
	    ако (list_append_tv(retlist, &items[i].item->li_tv) == FAIL)
		goto done;
	}

	// next copy the list of matching positions
	ако (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    ако (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;

	    за (цео i = 0; i < match_count; i++)
	    {
		ако (items[i].lmatchpos != NULL)
		{
		    ако (list_append_list(retlist, items[i].lmatchpos) == OK)
			items[i].lmatchpos = NULL;
		    инако
			goto done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    ако (li == NULL || li->li_tv.vval.v_list == NULL)
		goto done;
	    retlist = li->li_tv.vval.v_list;
	    за (цео i = 0; i < match_count; i++)
	    {
		ако (list_append_number(retlist, items[i].score) == FAIL)
		    goto done;
	    }
	}
    }

done:
    за (цео i = 0; i < match_count; i++)
    {
	ако (items[i].itemstr_allocated)
	    vim_free(items[i].itemstr);

	ако (items[i].lmatchpos)
	    list_free(items[i].lmatchpos);
    }
    vim_free(items);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    static void
do_fuzzymatch(typval_T *argvars, typval_T *rettv, цео retmatchpos)
{
    callback_T	cb;
    char_u	*key = NULL;
    цео		ret;
    цео		matchseq = FALSE;
    long	max_matches = 0;

    ако (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	врати;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    ако (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == NULL)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	врати;
    }
    ако (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == NULL)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	врати;
    }

    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ако (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    врати;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ако ((di = dict_find(d, (char_u *)"key", -1)) != NULL)
	{
	    ако (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == NULL
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		врати;
	    }
	    key = tv_get_string(&di->di_tv);
	}
	инако ако ((di = dict_find(d, (char_u *)"text_cb", -1)) != NULL)
	{
	    cb = get_callback(&di->di_tv);
	    ако (cb.cb_name == NULL)
	    {
		semsg(_(e_invalid_value_for_argument_str), "text_cb");
		врати;
	    }
	}

	ако ((di = dict_find(d, (char_u *)"limit", -1)) != NULL)
	{
	    ако (di->di_tv.v_type != VAR_NUMBER)
	    {
		semsg(_(e_invalid_value_for_argument_str), "limit");
		врати;
	    }
	    max_matches = (long)tv_get_number_chk(&di->di_tv, NULL);
	}

	ако (dict_has_key(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ако (ret == FAIL)
	goto done;
    ако (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ако (l == NULL)
	    goto done;
	ако (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	ако (l == NULL)
	    goto done;
	ако (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    goto done;
	}
	l = list_alloc();
	ако (l == NULL)
	    goto done;
	ако (list_append_list(rettv->vval.v_list, l) == FAIL)
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
    static цео
fuzzy_match_str_compare(const void *s1, const void *s2)
{
    цео		v1 = ((fuzmatch_str_T *)s1)->score;
    цео		v2 = ((fuzmatch_str_T *)s2)->score;
    цео		idx1 = ((fuzmatch_str_T *)s1)->idx;
    цео		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ако (v1 == v2)
	врати idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    инако
	врати v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    static void
fuzzy_match_str_sort(fuzmatch_str_T *fm, цео sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, sizeof(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    static цео
fuzzy_match_func_compare(const void *s1, const void *s2)
{
    цео		v1 = ((fuzmatch_str_T *)s1)->score;
    цео		v2 = ((fuzmatch_str_T *)s2)->score;
    цео		idx1 = ((fuzmatch_str_T *)s1)->idx;
    цео		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ако (*str1 != '<' && *str2 == '<')
	врати -1;
    ако (*str1 == '<' && *str2 != '<')
	врати 1;
    ако (v1 == v2)
	врати idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    инако
	врати v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    static void
fuzzy_match_func_sort(fuzmatch_str_T *fm, цео sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, sizeof(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    цео
fuzzy_match_str(char_u *str, char_u *pat)
{
    цео		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    ако (str == NULL || pat == NULL)
	врати score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				sizeof(matchpos) / sizeof(matchpos[0]));

    врати score;
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
    цео		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = NULL;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    цео		    j = 0;

    ако (str == NULL || pat == NULL)
	врати NULL;

    match_positions = ALLOC_ONE(garray_T);
    ако (match_positions == NULL)
	врати NULL;
    ga_init2(match_positions, sizeof(int_u), 10);

    ако (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	врати NULL;
    }

    за (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ако (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ако (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		врати NULL;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    врати match_positions;
#инако
    врати NULL;
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
    цео
fuzzy_match_str_in_line(
    char_u	**ptr,
    char_u	*pat,
    цео		*len,
    pos_T	*current_pos,
    цео		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = NULL;
    char_u	*start = NULL;
    цео		found = FALSE;
    char	save_end;
    char_u	*line_end = NULL;

    ако (str == NULL || pat == NULL)
	врати found;
    line_end = find_line_end(str);

    док (str < line_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	ако (*start == NUL)
	    break;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	ако (*score != FUZZY_SCORE_NONE)
	{
	    *len = (цео)(end - start);
	    found = TRUE;
	    *ptr = start;
	    ако (current_pos)
		current_pos->col += (цео)(end - strBegin);
	    break;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	док (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ако (!found)
	*ptr = line_end;

    врати found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    цео
search_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    цео		dir,
    pos_T	*start_pos,
    цео		*len,
    char_u	**ptr,
    цео		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    цео		found_new_match = FALSE;
    цео		looped_around = FALSE;
    цео		whole_line = ctrl_x_mode_whole_line();

    ако (buf == curbuf)
	circly_end = *start_pos;
    инако
    {
	circly_end.lnum = buf->b_ml.ml_line_count;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    ако (whole_line && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    do
    {

	// Check if looped around and back to start position
	ако (looped_around && (whole_line ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    break;

	// Ensure current_pos is valid
	ако (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_line_count)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ако (!whole_line)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ако (*ptr != NULL && **ptr != NUL)
	    {
		ако (!whole_line)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_line(ptr, pattern,
						    len, &current_pos, score);
		    ако (found_new_match)
		    {
			*pos = current_pos;
			break;
		    }
		    инако ако (looped_around && current_pos.lnum == circly_end.lnum)
			break;
		}
		инако
		{
		    char_u *line = *ptr;
		    char_u *p = skipwhite(line);
		    ако (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (цео)ml_get_buf_len(buf, current_pos.lnum) -
								(цео)(p - line);
			break;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ако (dir == FORWARD)
	{
	    ако (++current_pos.lnum > buf->b_ml.ml_line_count)
	    {
		ако (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		инако
		    break;
	    }
	}
	инако
	{
	    ако (--current_pos.lnum < 1)
	    {
		ако (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_line_count;
		    looped_around = TRUE;
		}
		инако
		    break;

	    }
	}
	current_pos.col = 0;
    } док (TRUE);

    врати found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    void
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, цео count)
{
    ако (fuzmatch == NULL)
	врати;

    за (цео i = 0; i < count; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    цео
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	цео		count,
	цео		funcsort)
{
    ако (count <= 0)
	goto theend;

    *matches = ALLOC_MULT(char_u *, count);
    ако (*matches == NULL)
    {
	fuzmatch_str_free(fuzmatch, count);
	врати FAIL;
    }

    // Sort the list by the descending order of the match score
    ако (funcsort)
	fuzzy_match_func_sort((void *)fuzmatch, (size_t)count);
    инако
	fuzzy_match_str_sort((void *)fuzmatch, (size_t)count);

    за (цео i = 0; i < count; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    врати OK;
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

    static цео
has_match(char_u *needle, char_u *haystack)
{
    ако (!needle || !haystack || !*needle)
	врати FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    док (*n_ptr)
    {
	цео n_char = mb_ptr2char(n_ptr);
	цео n_upper = MB_TOUPPER(n_char);
	цео found = FALSE;

	док (*h_ptr)
	{
	    цео h_char = mb_ptr2char(h_ptr);
	    ако (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		break;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ако (!found)
	    врати FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    врати OK;
}

typedef struct match_struct
{
    цео needle_len;
    цео haystack_len;
    цео lower_needle[MATCH_MAX_LEN];     // stores codepoints
    цео lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    static score_t
compute_bonus_codepoint(цео last_c, цео c)
{
    ако (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ако (IS_PATH_SEP(last_c))
	    врати SCORE_MATCH_SLASH;
	ако (IS_WORD_SEP(last_c))
	    врати SCORE_MATCH_WORD;
	ако (IS_DOT(last_c))
	    врати SCORE_MATCH_DOT;
	ако (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    врати SCORE_MATCH_CAPITAL;
    }
    врати 0;
}

    static void
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    цео i = 0;
    char_u *p = needle;
    док (*p != NUL && i < MATCH_MAX_LEN)
    {
	цео c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    цео prev_c = '/';
    док (*p != NUL && i < MATCH_MAX_LEN)
    {
	цео c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    static inline void
match_row(const match_struct *match, цео row, score_t *curr_D,
	score_t *curr_M, const score_t *last_D, const score_t *last_M)
{
    цео n = match->needle_len;
    цео m = match->haystack_len;
    цео i = row;

    const цео *lower_needle = match->lower_needle;
    const цео *lower_haystack = match->lower_haystack;
    const score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    за (цео j = 0; j < m; j++)
    {
	ако (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ако (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    инако ако (j)
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
	инако
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
    ако (!needle || !haystack || !*needle)
	врати SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    цео n = match.needle_len;
    цео m = match.haystack_len;

    ако (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	врати SCORE_MIN;
    }
    инако ако (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	за (цео i = 0; i < n; i++)
	{
	    ако (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		break;
	    }
	}
	ако (equal)
	{
	    ако (positions)
	    {
		за (цео i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    врати SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ако ((size_t)n > (SIZE_MAX / sizeof(score_t)) / MATCH_MAX_LEN / 2)
	врати SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(sizeof(score_t) * MATCH_MAX_LEN * n * 2);
    ако (!block)
	врати SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    за (цео i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ако (positions)
    {
	цео match_required = 0;
	за (цео i = n - 1, j = m - 1; i >= 0; i--)
	{
	    за (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ако (D[i][j] != SCORE_MIN &&
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
    врати result;
}
