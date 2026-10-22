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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL) || defined(FEAT_PROTO)
স্থির পূর্ণসংখ্যা fuzzy_match_item_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);
স্থির শূন্য fuzzy_match_in_list(list_T *l, char_u *str, পূর্ণসংখ্যা matchseq, char_u *চাবি, callback_T *item_cb, পূর্ণসংখ্যা retmatchpos, list_T *fmatchlist, long max_matches);
স্থির শূন্য do_fuzzymatch(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা retmatchpos);
#endif
স্থির পূর্ণসংখ্যা fuzzy_match_str_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);
স্থির শূন্য fuzzy_match_str_sort(fuzmatch_str_T *fm, পূর্ণসংখ্যা sz);
স্থির পূর্ণসংখ্যা fuzzy_match_func_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);
স্থির শূন্য fuzzy_match_func_sort(fuzmatch_str_T *fm, পূর্ণসংখ্যা sz);

প্রকারলেখ দ্বিগুণ score_t;
স্থির score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
স্থির পূর্ণসংখ্যা has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা		idx;		// used for stable sort
    listitem_T	*item;
    পূর্ণসংখ্যা		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    পূর্ণসংখ্যা		itemstr_allocated;
    পূর্ণসংখ্যা		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    পূর্ণসংখ্যা
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    পূর্ণসংখ্যা		matchseq,
    পূর্ণসংখ্যা		*outScore,
    int_u	*matches,
    পূর্ণসংখ্যা		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    পূর্ণসংখ্যা		complete = FALSE;
    পূর্ণসংখ্যা		score = 0;
    পূর্ণসংখ্যা		অঙ্ক_Matches = 0;
    পূর্ণসংখ্যা		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    যদি (save_pat == ফাঁকা)
	ফেরত FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    যখন (TRUE)
    {
	যদি (matchseq)
	    complete = TRUE;
	অন্যথা
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    যদি (*p == NUL)
		বিরতি;
	    pat = p;
	    যখন (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		যদি (has_mbyte)
		    MB_PTR_ADV(p);
		অন্যথা
		    ++p;
	    }
	    যদি (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	যদি (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	যদি (অঙ্ক_Matches > maxMatches - pat_chars)
	{
	    অঙ্ক_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    বিরতি;
	}

	score = FUZZY_SCORE_NONE;
	যদি (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + অঙ্ক_Matches);
	    যদি (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (পূর্ণসংখ্যা)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (পূর্ণসংখ্যা)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	যদি (score == FUZZY_SCORE_NONE)
	{
	    অঙ্ক_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    বিরতি;
	}

	যদি (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	অন্যথা যদি (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	অন্যথা
	    *outScore += score;

	অঙ্ক_Matches += pat_chars;

	যদি (complete || অঙ্ক_Matches >= maxMatches)
	    বিরতি;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ফেরত অঙ্ক_Matches != 0;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    স্থির পূর্ণসংখ্যা
fuzzy_match_item_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    পূর্ণসংখ্যা		v1 = ((fuzzyItem_T *)s1)->score;
    পূর্ণসংখ্যা		v2 = ((fuzzyItem_T *)s2)->score;

    যদি (v1 == v2)
    {
	পূর্ণসংখ্যা exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	পূর্ণসংখ্যা patlen = (পূর্ণসংখ্যা)STRLEN(pat);
	পূর্ণসংখ্যা startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	যদি (exact_match1 == exact_match2)
	{
	    পূর্ণসংখ্যা idx1 = ((fuzzyItem_T *)s1)->idx;
	    পূর্ণসংখ্যা idx2 = ((fuzzyItem_T *)s2)->idx;
	    ফেরত idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	অন্যথা যদি (exact_match2)
	    ফেরত 1;
	ফেরত -1;
    }
    অন্যথা
	ফেরত v1 > v2 ? -1 : 1;
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
    স্থির শূন্য
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    পূর্ণসংখ্যা		matchseq,
    char_u	*চাবি,
    callback_T	*item_cb,
    পূর্ণসংখ্যা		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *বস্তুসমূহ = ফাঁকা;
    listitem_T	    *li;
    long	    match_গুনতি = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    যদি (len == 0)
	ফেরত;
    যদি (max_matches > 0 && len > max_matches)
	len = max_matches;

    বস্তুসমূহ = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    যদি (বস্তুসমূহ == ফাঁকা)
	ফেরত;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	পূর্ণসংখ্যা		score;
	char_u		*itemstr = ফাঁকা;
	char_u		*itemstr_copy = ফাঁকা;
	typval_T	rettv;
	পূর্ণসংখ্যা		itemstr_allocate = FALSE;
	list_T		*match_positions = ফাঁকা;

	যদি (max_matches > 0 && match_গুনতি >= max_matches)
	    বিরতি;

	rettv.v_type = VAR_UNKNOWN;
	যদি (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	অন্যথা যদি (li->li_tv.v_type == VAR_DICT
				&& (চাবি != ফাঁকা || item_cb->cb_name != ফাঁকা))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    যদি (চাবি != ফাঁকা)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (অক্ষর *)চাবি, FALSE);
	    অন্যথা
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		যদি (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    যদি (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	যদি (itemstr != ফাঁকা
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    যদি (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		যদি (itemstr_copy == ফাঁকা)
		{
		    clear_tv(&rettv);
		    চালু;
		}
	    }
	    অন্যথা
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    যদি (retmatchpos)
	    {
		match_positions = list_alloc();
		যদি (match_positions == ফাঁকা)
		{
		    যদি (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    চালু;
		}

		// Fill position information
		পূর্ণসংখ্যা	j = 0;
		char_u	*p = str;
		পূর্ণসংখ্যা	success = TRUE;

		যখন (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    যদি (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			যদি (list_append_সংখ্যা(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    বিরতি;
			}
			j++;
		    }
		    যদি (has_mbyte)
			MB_PTR_ADV(p);
		    অন্যথা
			++p;
		}

		যদি (!success)
		{
		    list_free(match_positions);
		    যদি (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    চালু;
		}
	    }
	    বস্তুসমূহ[match_গুনতি].idx = match_গুনতি;
	    বস্তুসমূহ[match_গুনতি].item = li;
	    বস্তুসমূহ[match_গুনতি].score = score;
	    বস্তুসমূহ[match_গুনতি].pat = str;
	    বস্তুসমূহ[match_গুনতি].startpos = matches[0];
	    বস্তুসমূহ[match_গুনতি].itemstr = itemstr_copy;
	    বস্তুসমূহ[match_গুনতি].itemstr_allocated = itemstr_allocate;
	    বস্তুসমূহ[match_গুনতি].lmatchpos = match_positions;

	    ++match_গুনতি;
	}
	clear_tv(&rettv);
    }

    যদি (match_গুনতি > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((শূন্য *)বস্তুসমূহ, (size_t)match_গুনতি, আকার(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	যদি (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    যদি (li == ফাঁকা || li->li_tv.vval.v_list == ফাঁকা)
		যাও done;
	    retlist = li->li_tv.vval.v_list;
	}
	অন্যথা
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	জন্য (পূর্ণসংখ্যা i = 0; i < match_গুনতি; i++)
	{
	    যদি (list_append_tv(retlist, &বস্তুসমূহ[i].item->li_tv) == FAIL)
		যাও done;
	}

	// next copy the list of matching positions
	যদি (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    যদি (li == ফাঁকা || li->li_tv.vval.v_list == ফাঁকা)
		যাও done;
	    retlist = li->li_tv.vval.v_list;

	    জন্য (পূর্ণসংখ্যা i = 0; i < match_গুনতি; i++)
	    {
		যদি (বস্তুসমূহ[i].lmatchpos != ফাঁকা)
		{
		    যদি (list_append_list(retlist, বস্তুসমূহ[i].lmatchpos) == OK)
			বস্তুসমূহ[i].lmatchpos = ফাঁকা;
		    অন্যথা
			যাও done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    যদি (li == ফাঁকা || li->li_tv.vval.v_list == ফাঁকা)
		যাও done;
	    retlist = li->li_tv.vval.v_list;
	    জন্য (পূর্ণসংখ্যা i = 0; i < match_গুনতি; i++)
	    {
		যদি (list_append_সংখ্যা(retlist, বস্তুসমূহ[i].score) == FAIL)
		    যাও done;
	    }
	}
    }

done:
    জন্য (পূর্ণসংখ্যা i = 0; i < match_গুনতি; i++)
    {
	যদি (বস্তুসমূহ[i].itemstr_allocated)
	    vim_free(বস্তুসমূহ[i].itemstr);

	যদি (বস্তুসমূহ[i].lmatchpos)
	    list_free(বস্তুসমূহ[i].lmatchpos);
    }
    vim_free(বস্তুসমূহ);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    স্থির শূন্য
do_fuzzymatch(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা retmatchpos)
{
    callback_T	cb;
    char_u	*চাবি = ফাঁকা;
    পূর্ণসংখ্যা		ret;
    পূর্ণসংখ্যা		matchseq = FALSE;
    long	max_matches = 0;

    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    যদি (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ফাঁকা)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ফেরত;
    }
    যদি (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ফাঁকা)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ফেরত;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	যদি (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ফেরত;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	যদি ((di = dict_find(d, (char_u *)"key", -1)) != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ফাঁকা
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_মান_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		ফেরত;
	    }
	    চাবি = tv_get_string(&di->di_tv);
	}
	অন্যথা যদি ((di = dict_find(d, (char_u *)"text_cb", -1)) != ফাঁকা)
	{
	    cb = get_callback(&di->di_tv);
	    যদি (cb.cb_name == ফাঁকা)
	    {
		semsg(_(e_invalid_মান_for_argument_str), "text_cb");
		ফেরত;
	    }
	}

	যদি ((di = dict_find(d, (char_u *)"limit", -1)) != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_সংখ্যা)
	    {
		semsg(_(e_invalid_মান_for_argument_str), "limit");
		ফেরত;
	    }
	    max_matches = (long)tv_get_সংখ্যা_chk(&di->di_tv, ফাঁকা);
	}

	যদি (dict_has_চাবি(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    যদি (ret == FAIL)
	যাও done;
    যদি (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	যদি (l == ফাঁকা)
	    যাও done;
	যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    যাও done;
	}
	l = list_alloc();
	যদি (l == ফাঁকা)
	    যাও done;
	যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    যাও done;
	}
	l = list_alloc();
	যদি (l == ফাঁকা)
	    যাও done;
	যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    যাও done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, চাবি, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    শূন্য
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    শূন্য
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    স্থির পূর্ণসংখ্যা
fuzzy_match_str_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    পূর্ণসংখ্যা		v1 = ((fuzmatch_str_T *)s1)->score;
    পূর্ণসংখ্যা		v2 = ((fuzmatch_str_T *)s2)->score;
    পূর্ণসংখ্যা		idx1 = ((fuzmatch_str_T *)s1)->idx;
    পূর্ণসংখ্যা		idx2 = ((fuzmatch_str_T *)s2)->idx;

    যদি (v1 == v2)
	ফেরত idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    অন্যথা
	ফেরত v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    স্থির শূন্য
fuzzy_match_str_sort(fuzmatch_str_T *fm, পূর্ণসংখ্যা sz)
{
    // Sort the list by the descending order of the match score
    qsort((শূন্য *)fm, (size_t)sz, আকার(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    স্থির পূর্ণসংখ্যা
fuzzy_match_func_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    পূর্ণসংখ্যা		v1 = ((fuzmatch_str_T *)s1)->score;
    পূর্ণসংখ্যা		v2 = ((fuzmatch_str_T *)s2)->score;
    পূর্ণসংখ্যা		idx1 = ((fuzmatch_str_T *)s1)->idx;
    পূর্ণসংখ্যা		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    যদি (*str1 != '<' && *str2 == '<')
	ফেরত -1;
    যদি (*str1 == '<' && *str2 != '<')
	ফেরত 1;
    যদি (v1 == v2)
	ফেরত idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    অন্যথা
	ফেরত v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    স্থির শূন্য
fuzzy_match_func_sort(fuzmatch_str_T *fm, পূর্ণসংখ্যা sz)
{
    // Sort the list by the descending order of the match score
    qsort((শূন্য *)fm, (size_t)sz, আকার(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    পূর্ণসংখ্যা
fuzzy_match_str(char_u *str, char_u *pat)
{
    পূর্ণসংখ্যা		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    যদি (str == ফাঁকা || pat == ফাঁকা)
	ফেরত score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				আকার(matchpos) / আকার(matchpos[0]));

    ফেরত score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণসংখ্যা		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = ফাঁকা;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    পূর্ণসংখ্যা		    j = 0;

    যদি (str == ফাঁকা || pat == ফাঁকা)
	ফেরত ফাঁকা;

    match_positions = ALLOC_ONE(garray_T);
    যদি (match_positions == ফাঁকা)
	ফেরত ফাঁকা;
    ga_init2(match_positions, আকার(int_u), 10);

    যদি (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	ফেরত ফাঁকা;
    }

    জন্য (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	যদি (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    যদি (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		ফেরত ফাঁকা;
	    }
	    ((int_u *)match_positions->ga_তথ্য)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    ফেরত match_positions;
#অন্যথা
    ফেরত ফাঁকা;
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
    পূর্ণসংখ্যা
fuzzy_match_str_in_সারি(
    char_u	**ptr,
    char_u	*pat,
    পূর্ণসংখ্যা		*len,
    pos_T	*current_pos,
    পূর্ণসংখ্যা		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = ফাঁকা;
    char_u	*start = ফাঁকা;
    পূর্ণসংখ্যা		found = FALSE;
    অক্ষর	save_end;
    char_u	*সারি_end = ফাঁকা;

    যদি (str == ফাঁকা || pat == ফাঁকা)
	ফেরত found;
    সারি_end = find_সারি_end(str);

    যখন (str < সারি_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	যদি (*start == NUL)
	    বিরতি;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	যদি (*score != FUZZY_SCORE_NONE)
	{
	    *len = (পূর্ণসংখ্যা)(end - start);
	    found = TRUE;
	    *ptr = start;
	    যদি (current_pos)
		current_pos->col += (পূর্ণসংখ্যা)(end - strBegin);
	    বিরতি;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	যখন (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    যদি (!found)
	*ptr = সারি_end;

    ফেরত found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    পূর্ণসংখ্যা
অনুসন্ধান_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    পূর্ণসংখ্যা		dir,
    pos_T	*start_pos,
    পূর্ণসংখ্যা		*len,
    char_u	**ptr,
    পূর্ণসংখ্যা		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    পূর্ণসংখ্যা		found_new_match = FALSE;
    পূর্ণসংখ্যা		looped_around = FALSE;
    পূর্ণসংখ্যা		whole_সারি = ctrl_x_mode_whole_সারি();

    যদি (buf == curbuf)
	circly_end = *start_pos;
    অন্যথা
    {
	circly_end.lnum = buf->b_ml.ml_সারি_গুনতি;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    যদি (whole_সারি && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    করো
    {

	// Check if looped around and back to start position
	যদি (looped_around && (whole_সারি ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    বিরতি;

	// Ensure current_pos is valid
	যদি (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_সারি_গুনতি)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    যদি (!whole_সারি)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    যদি (*ptr != ফাঁকা && **ptr != NUL)
	    {
		যদি (!whole_সারি)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_সারি(ptr, pattern,
						    len, &current_pos, score);
		    যদি (found_new_match)
		    {
			*pos = current_pos;
			বিরতি;
		    }
		    অন্যথা যদি (looped_around && current_pos.lnum == circly_end.lnum)
			বিরতি;
		}
		অন্যথা
		{
		    char_u *সারি = *ptr;
		    char_u *p = skipwhite(সারি);
		    যদি (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (পূর্ণসংখ্যা)ml_get_buf_len(buf, current_pos.lnum) -
								(পূর্ণসংখ্যা)(p - সারি);
			বিরতি;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	যদি (dir == FORWARD)
	{
	    যদি (++current_pos.lnum > buf->b_ml.ml_সারি_গুনতি)
	    {
		যদি (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		অন্যথা
		    বিরতি;
	    }
	}
	অন্যথা
	{
	    যদি (--current_pos.lnum < 1)
	    {
		যদি (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_সারি_গুনতি;
		    looped_around = TRUE;
		}
		অন্যথা
		    বিরতি;

	    }
	}
	current_pos.col = 0;
    } যখন (TRUE);

    ফেরত found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    শূন্য
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, পূর্ণসংখ্যা গুনতি)
{
    যদি (fuzmatch == ফাঁকা)
	ফেরত;

    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    পূর্ণসংখ্যা
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	পূর্ণসংখ্যা		গুনতি,
	পূর্ণসংখ্যা		funcsort)
{
    যদি (গুনতি <= 0)
	যাও theend;

    *matches = ALLOC_MULT(char_u *, গুনতি);
    যদি (*matches == ফাঁকা)
    {
	fuzmatch_str_free(fuzmatch, গুনতি);
	ফেরত FAIL;
    }

    // Sort the list by the descending order of the match score
    যদি (funcsort)
	fuzzy_match_func_sort((শূন্য *)fuzmatch, (size_t)গুনতি);
    অন্যথা
	fuzzy_match_str_sort((শূন্য *)fuzmatch, (size_t)গুনতি);

    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ফেরত OK;
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

    স্থির পূর্ণসংখ্যা
has_match(char_u *needle, char_u *haystack)
{
    যদি (!needle || !haystack || !*needle)
	ফেরত FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    যখন (*n_ptr)
    {
	পূর্ণসংখ্যা n_char = mb_ptr2char(n_ptr);
	পূর্ণসংখ্যা n_upper = MB_TOUPPER(n_char);
	পূর্ণসংখ্যা found = FALSE;

	যখন (*h_ptr)
	{
	    পূর্ণসংখ্যা h_char = mb_ptr2char(h_ptr);
	    যদি (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		বিরতি;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	যদি (!found)
	    ফেরত FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ফেরত OK;
}

প্রকারলেখ কাঠামো match_struct
{
    পূর্ণসংখ্যা needle_len;
    পূর্ণসংখ্যা haystack_len;
    পূর্ণসংখ্যা lower_needle[MATCH_MAX_LEN];     // stores codepoints
    পূর্ণসংখ্যা lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    স্থির score_t
compute_bonus_codepoint(পূর্ণসংখ্যা last_c, পূর্ণসংখ্যা c)
{
    যদি (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	যদি (IS_PATH_SEP(last_c))
	    ফেরত SCORE_MATCH_SLASH;
	যদি (IS_WORD_SEP(last_c))
	    ফেরত SCORE_MATCH_WORD;
	যদি (IS_DOT(last_c))
	    ফেরত SCORE_MATCH_DOT;
	যদি (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    ফেরত SCORE_MATCH_CAPITAL;
    }
    ফেরত 0;
}

    স্থির শূন্য
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    পূর্ণসংখ্যা i = 0;
    char_u *p = needle;
    যখন (*p != NUL && i < MATCH_MAX_LEN)
    {
	পূর্ণসংখ্যা c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    পূর্ণসংখ্যা পূর্ব_c = '/';
    যখন (*p != NUL && i < MATCH_MAX_LEN)
    {
	পূর্ণসংখ্যা c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(পূর্ব_c, c);
	পূর্ব_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    স্থির অন্তঃরেখ শূন্য
match_row(ধ্রুবক match_struct *match, পূর্ণসংখ্যা row, score_t *curr_D,
	score_t *curr_M, ধ্রুবক score_t *last_D, ধ্রুবক score_t *last_M)
{
    পূর্ণসংখ্যা n = match->needle_len;
    পূর্ণসংখ্যা m = match->haystack_len;
    পূর্ণসংখ্যা i = row;

    ধ্রুবক পূর্ণসংখ্যা *lower_needle = match->lower_needle;
    ধ্রুবক পূর্ণসংখ্যা *lower_haystack = match->lower_haystack;
    ধ্রুবক score_t *match_bonus = match->match_bonus;

    score_t পূর্ব_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t পূর্ব_M = SCORE_MIN, পূর্ব_D = SCORE_MIN;

    জন্য (পূর্ণসংখ্যা j = 0; j < m; j++)
    {
	যদি (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    যদি (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    অন্যথা যদি (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			পূর্ব_M + match_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			পূর্ব_D + SCORE_MATCH_CONSECUTIVE);
	    }
	    পূর্ব_D = last_D[j];
	    পূর্ব_M = last_M[j];
	    curr_D[j] = score;
	    curr_M[j] = পূর্ব_score = MAX(score, পূর্ব_score + gap_score);
	}
	অন্যথা
	{
	    পূর্ব_D = last_D[j];
	    পূর্ব_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = পূর্ব_score = পূর্ব_score + gap_score;
	}
    }
}

    স্থির score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    যদি (!needle || !haystack || !*needle)
	ফেরত SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    পূর্ণসংখ্যা n = match.needle_len;
    পূর্ণসংখ্যা m = match.haystack_len;

    যদি (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ফেরত SCORE_MIN;
    }
    অন্যথা যদি (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	জন্য (পূর্ণসংখ্যা i = 0; i < n; i++)
	{
	    যদি (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		বিরতি;
	    }
	}
	যদি (equal)
	{
	    যদি (positions)
	    {
		জন্য (পূর্ণসংখ্যা i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ফেরত SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    যদি ((size_t)n > (আকার_MAX / আকার(score_t)) / MATCH_MAX_LEN / 2)
	ফেরত SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(আকার(score_t) * MATCH_MAX_LEN * n * 2);
    যদি (!block)
	ফেরত SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    জন্য (পূর্ণসংখ্যা i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    যদি (positions)
    {
	পূর্ণসংখ্যা match_required = 0;
	জন্য (পূর্ণসংখ্যা i = n - 1, j = m - 1; i >= 0; i--)
	{
	    জন্য (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		যদি (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    বিরতি;
		}
	    }
	}
    }

    score_t ফলাফল = M[n - 1][m - 1];
    vim_free(block);
    ফেরত ফলাফল;
}
