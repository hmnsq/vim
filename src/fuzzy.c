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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL) || defined(FEAT_PROTO)
நிலையான முழு fuzzy_match_item_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);
நிலையான வெறுமை fuzzy_match_in_list(list_T *l, char_u *str, முழு matchseq, char_u *சாவி, callback_T *item_cb, முழு retmatchpos, list_T *fmatchlist, long max_matches);
நிலையான வெறுமை do_fuzzymatch(typval_T *argvars, typval_T *rettv, முழு retmatchpos);
#endif
நிலையான முழு fuzzy_match_str_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);
நிலையான வெறுமை fuzzy_match_str_sort(fuzmatch_str_T *fm, முழு sz);
நிலையான முழு fuzzy_match_func_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);
நிலையான வெறுமை fuzzy_match_func_sort(fuzmatch_str_T *fm, முழு sz);

வகைவரை இரட்டை score_t;
நிலையான score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
நிலையான முழு has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

வகைவரை கட்டமைப்பு
{
    முழு		idx;		// used for stable sort
    listitem_T	*item;
    முழு		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    முழு		itemstr_allocated;
    முழு		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    முழு
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    முழு		matchseq,
    முழு		*outScore,
    int_u	*matches,
    முழு		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    முழு		complete = FALSE;
    முழு		score = 0;
    முழு		இலக்கம்_Matches = 0;
    முழு		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    எனில் (save_pat == வெற்று)
	திரும்பு FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    போது (TRUE)
    {
	எனில் (matchseq)
	    complete = TRUE;
	இல்லையெனில்
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    எனில் (*p == NUL)
		நிறுத்து;
	    pat = p;
	    போது (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		எனில் (has_mbyte)
		    MB_PTR_ADV(p);
		இல்லையெனில்
		    ++p;
	    }
	    எனில் (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	எனில் (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	எனில் (இலக்கம்_Matches > maxMatches - pat_chars)
	{
	    இலக்கம்_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    நிறுத்து;
	}

	score = FUZZY_SCORE_NONE;
	எனில் (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + இலக்கம்_Matches);
	    எனில் (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (முழு)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (முழு)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	எனில் (score == FUZZY_SCORE_NONE)
	{
	    இலக்கம்_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    நிறுத்து;
	}

	எனில் (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	இல்லையெனில் எனில் (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	இல்லையெனில்
	    *outScore += score;

	இலக்கம்_Matches += pat_chars;

	எனில் (complete || இலக்கம்_Matches >= maxMatches)
	    நிறுத்து;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    திரும்பு இலக்கம்_Matches != 0;
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    நிலையான முழு
fuzzy_match_item_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    முழு		v1 = ((fuzzyItem_T *)s1)->score;
    முழு		v2 = ((fuzzyItem_T *)s2)->score;

    எனில் (v1 == v2)
    {
	முழு exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	முழு patlen = (முழு)STRLEN(pat);
	முழு startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	எனில் (exact_match1 == exact_match2)
	{
	    முழு idx1 = ((fuzzyItem_T *)s1)->idx;
	    முழு idx2 = ((fuzzyItem_T *)s2)->idx;
	    திரும்பு idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	இல்லையெனில் எனில் (exact_match2)
	    திரும்பு 1;
	திரும்பு -1;
    }
    இல்லையெனில்
	திரும்பு v1 > v2 ? -1 : 1;
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
    நிலையான வெறுமை
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    முழு		matchseq,
    char_u	*சாவி,
    callback_T	*item_cb,
    முழு		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *பொருட்கள் = வெற்று;
    listitem_T	    *li;
    long	    match_மொத்தம் = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    எனில் (len == 0)
	திரும்பு;
    எனில் (max_matches > 0 && len > max_matches)
	len = max_matches;

    பொருட்கள் = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    எனில் (பொருட்கள் == வெற்று)
	திரும்பு;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	முழு		score;
	char_u		*itemstr = வெற்று;
	char_u		*itemstr_copy = வெற்று;
	typval_T	rettv;
	முழு		itemstr_allocate = FALSE;
	list_T		*match_positions = வெற்று;

	எனில் (max_matches > 0 && match_மொத்தம் >= max_matches)
	    நிறுத்து;

	rettv.v_type = VAR_UNKNOWN;
	எனில் (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	இல்லையெனில் எனில் (li->li_tv.v_type == VAR_DICT
				&& (சாவி != வெற்று || item_cb->cb_name != வெற்று))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    எனில் (சாவி != வெற்று)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (எழுத்து *)சாவி, FALSE);
	    இல்லையெனில்
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		எனில் (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    எனில் (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	எனில் (itemstr != வெற்று
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    எனில் (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		எனில் (itemstr_copy == வெற்று)
		{
		    clear_tv(&rettv);
		    தொடர்;
		}
	    }
	    இல்லையெனில்
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    எனில் (retmatchpos)
	    {
		match_positions = list_alloc();
		எனில் (match_positions == வெற்று)
		{
		    எனில் (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    தொடர்;
		}

		// Fill position information
		முழு	j = 0;
		char_u	*p = str;
		முழு	success = TRUE;

		போது (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    எனில் (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			எனில் (list_append_எண்(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    நிறுத்து;
			}
			j++;
		    }
		    எனில் (has_mbyte)
			MB_PTR_ADV(p);
		    இல்லையெனில்
			++p;
		}

		எனில் (!success)
		{
		    list_free(match_positions);
		    எனில் (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    தொடர்;
		}
	    }
	    பொருட்கள்[match_மொத்தம்].idx = match_மொத்தம்;
	    பொருட்கள்[match_மொத்தம்].item = li;
	    பொருட்கள்[match_மொத்தம்].score = score;
	    பொருட்கள்[match_மொத்தம்].pat = str;
	    பொருட்கள்[match_மொத்தம்].startpos = matches[0];
	    பொருட்கள்[match_மொத்தம்].itemstr = itemstr_copy;
	    பொருட்கள்[match_மொத்தம்].itemstr_allocated = itemstr_allocate;
	    பொருட்கள்[match_மொத்தம்].lmatchpos = match_positions;

	    ++match_மொத்தம்;
	}
	clear_tv(&rettv);
    }

    எனில் (match_மொத்தம் > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((வெறுமை *)பொருட்கள், (size_t)match_மொத்தம், அளவு(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	எனில் (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    எனில் (li == வெற்று || li->li_tv.vval.v_list == வெற்று)
		செல் done;
	    retlist = li->li_tv.vval.v_list;
	}
	இல்லையெனில்
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	க்கு (முழு i = 0; i < match_மொத்தம்; i++)
	{
	    எனில் (list_append_tv(retlist, &பொருட்கள்[i].item->li_tv) == FAIL)
		செல் done;
	}

	// next copy the list of matching positions
	எனில் (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    எனில் (li == வெற்று || li->li_tv.vval.v_list == வெற்று)
		செல் done;
	    retlist = li->li_tv.vval.v_list;

	    க்கு (முழு i = 0; i < match_மொத்தம்; i++)
	    {
		எனில் (பொருட்கள்[i].lmatchpos != வெற்று)
		{
		    எனில் (list_append_list(retlist, பொருட்கள்[i].lmatchpos) == OK)
			பொருட்கள்[i].lmatchpos = வெற்று;
		    இல்லையெனில்
			செல் done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    எனில் (li == வெற்று || li->li_tv.vval.v_list == வெற்று)
		செல் done;
	    retlist = li->li_tv.vval.v_list;
	    க்கு (முழு i = 0; i < match_மொத்தம்; i++)
	    {
		எனில் (list_append_எண்(retlist, பொருட்கள்[i].score) == FAIL)
		    செல் done;
	    }
	}
    }

done:
    க்கு (முழு i = 0; i < match_மொத்தம்; i++)
    {
	எனில் (பொருட்கள்[i].itemstr_allocated)
	    vim_free(பொருட்கள்[i].itemstr);

	எனில் (பொருட்கள்[i].lmatchpos)
	    list_free(பொருட்கள்[i].lmatchpos);
    }
    vim_free(பொருட்கள்);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    நிலையான வெறுமை
do_fuzzymatch(typval_T *argvars, typval_T *rettv, முழு retmatchpos)
{
    callback_T	cb;
    char_u	*சாவி = வெற்று;
    முழு		ret;
    முழு		matchseq = FALSE;
    long	max_matches = 0;

    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    எனில் (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == வெற்று)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	திரும்பு;
    }
    எனில் (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == வெற்று)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	திரும்பு;
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	எனில் (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    திரும்பு;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	எனில் ((di = dict_find(d, (char_u *)"key", -1)) != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == வெற்று
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		திரும்பு;
	    }
	    சாவி = tv_get_string(&di->di_tv);
	}
	இல்லையெனில் எனில் ((di = dict_find(d, (char_u *)"text_cb", -1)) != வெற்று)
	{
	    cb = get_callback(&di->di_tv);
	    எனில் (cb.cb_name == வெற்று)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str), "text_cb");
		திரும்பு;
	    }
	}

	எனில் ((di = dict_find(d, (char_u *)"limit", -1)) != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_எண்)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str), "limit");
		திரும்பு;
	    }
	    max_matches = (long)tv_get_எண்_chk(&di->di_tv, வெற்று);
	}

	எனில் (dict_has_சாவி(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    எனில் (ret == FAIL)
	செல் done;
    எனில் (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	எனில் (l == வெற்று)
	    செல் done;
	எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    செல் done;
	}
	l = list_alloc();
	எனில் (l == வெற்று)
	    செல் done;
	எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    செல் done;
	}
	l = list_alloc();
	எனில் (l == வெற்று)
	    செல் done;
	எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    செல் done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, சாவி, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    வெறுமை
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    வெறுமை
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    நிலையான முழு
fuzzy_match_str_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    முழு		v1 = ((fuzmatch_str_T *)s1)->score;
    முழு		v2 = ((fuzmatch_str_T *)s2)->score;
    முழு		idx1 = ((fuzmatch_str_T *)s1)->idx;
    முழு		idx2 = ((fuzmatch_str_T *)s2)->idx;

    எனில் (v1 == v2)
	திரும்பு idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    இல்லையெனில்
	திரும்பு v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    நிலையான வெறுமை
fuzzy_match_str_sort(fuzmatch_str_T *fm, முழு sz)
{
    // Sort the list by the descending order of the match score
    qsort((வெறுமை *)fm, (size_t)sz, அளவு(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    நிலையான முழு
fuzzy_match_func_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    முழு		v1 = ((fuzmatch_str_T *)s1)->score;
    முழு		v2 = ((fuzmatch_str_T *)s2)->score;
    முழு		idx1 = ((fuzmatch_str_T *)s1)->idx;
    முழு		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    எனில் (*str1 != '<' && *str2 == '<')
	திரும்பு -1;
    எனில் (*str1 == '<' && *str2 != '<')
	திரும்பு 1;
    எனில் (v1 == v2)
	திரும்பு idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    இல்லையெனில்
	திரும்பு v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    நிலையான வெறுமை
fuzzy_match_func_sort(fuzmatch_str_T *fm, முழு sz)
{
    // Sort the list by the descending order of the match score
    qsort((வெறுமை *)fm, (size_t)sz, அளவு(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    முழு
fuzzy_match_str(char_u *str, char_u *pat)
{
    முழு		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    எனில் (str == வெற்று || pat == வெற்று)
	திரும்பு score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				அளவு(matchpos) / அளவு(matchpos[0]));

    திரும்பு score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_தேடல்_EXTRA
    முழு		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = வெற்று;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    முழு		    j = 0;

    எனில் (str == வெற்று || pat == வெற்று)
	திரும்பு வெற்று;

    match_positions = ALLOC_ONE(garray_T);
    எனில் (match_positions == வெற்று)
	திரும்பு வெற்று;
    ga_init2(match_positions, அளவு(int_u), 10);

    எனில் (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	திரும்பு வெற்று;
    }

    க்கு (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	எனில் (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    எனில் (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		திரும்பு வெற்று;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    திரும்பு match_positions;
#இல்லையெனில்
    திரும்பு வெற்று;
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
    முழு
fuzzy_match_str_in_வரி(
    char_u	**ptr,
    char_u	*pat,
    முழு		*len,
    pos_T	*current_pos,
    முழு		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = வெற்று;
    char_u	*start = வெற்று;
    முழு		found = FALSE;
    எழுத்து	save_end;
    char_u	*வரி_end = வெற்று;

    எனில் (str == வெற்று || pat == வெற்று)
	திரும்பு found;
    வரி_end = find_வரி_end(str);

    போது (str < வரி_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	எனில் (*start == NUL)
	    நிறுத்து;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	எனில் (*score != FUZZY_SCORE_NONE)
	{
	    *len = (முழு)(end - start);
	    found = TRUE;
	    *ptr = start;
	    எனில் (current_pos)
		current_pos->col += (முழு)(end - strBegin);
	    நிறுத்து;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	போது (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    எனில் (!found)
	*ptr = வரி_end;

    திரும்பு found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    முழு
தேடல்_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    முழு		dir,
    pos_T	*start_pos,
    முழு		*len,
    char_u	**ptr,
    முழு		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    முழு		found_new_match = FALSE;
    முழு		looped_around = FALSE;
    முழு		whole_வரி = ctrl_x_mode_whole_வரி();

    எனில் (buf == curbuf)
	circly_end = *start_pos;
    இல்லையெனில்
    {
	circly_end.lnum = buf->b_ml.ml_வரி_மொத்தம்;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    எனில் (whole_வரி && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    செய்
    {

	// Check if looped around and back to start position
	எனில் (looped_around && (whole_வரி ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    நிறுத்து;

	// Ensure current_pos is valid
	எனில் (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_வரி_மொத்தம்)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    எனில் (!whole_வரி)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    எனில் (*ptr != வெற்று && **ptr != NUL)
	    {
		எனில் (!whole_வரி)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_வரி(ptr, pattern,
						    len, &current_pos, score);
		    எனில் (found_new_match)
		    {
			*pos = current_pos;
			நிறுத்து;
		    }
		    இல்லையெனில் எனில் (looped_around && current_pos.lnum == circly_end.lnum)
			நிறுத்து;
		}
		இல்லையெனில்
		{
		    char_u *வரி = *ptr;
		    char_u *p = skipwhite(வரி);
		    எனில் (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (முழு)ml_get_buf_len(buf, current_pos.lnum) -
								(முழு)(p - வரி);
			நிறுத்து;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	எனில் (dir == FORWARD)
	{
	    எனில் (++current_pos.lnum > buf->b_ml.ml_வரி_மொத்தம்)
	    {
		எனில் (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		இல்லையெனில்
		    நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (--current_pos.lnum < 1)
	    {
		எனில் (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_வரி_மொத்தம்;
		    looped_around = TRUE;
		}
		இல்லையெனில்
		    நிறுத்து;

	    }
	}
	current_pos.col = 0;
    } போது (TRUE);

    திரும்பு found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    வெறுமை
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, முழு மொத்தம்)
{
    எனில் (fuzmatch == வெற்று)
	திரும்பு;

    க்கு (முழு i = 0; i < மொத்தம்; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    முழு
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	முழு		மொத்தம்,
	முழு		funcsort)
{
    எனில் (மொத்தம் <= 0)
	செல் theend;

    *matches = ALLOC_MULT(char_u *, மொத்தம்);
    எனில் (*matches == வெற்று)
    {
	fuzmatch_str_free(fuzmatch, மொத்தம்);
	திரும்பு FAIL;
    }

    // Sort the list by the descending order of the match score
    எனில் (funcsort)
	fuzzy_match_func_sort((வெறுமை *)fuzmatch, (size_t)மொத்தம்);
    இல்லையெனில்
	fuzzy_match_str_sort((வெறுமை *)fuzmatch, (size_t)மொத்தம்);

    க்கு (முழு i = 0; i < மொத்தம்; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    திரும்பு OK;
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

    நிலையான முழு
has_match(char_u *needle, char_u *haystack)
{
    எனில் (!needle || !haystack || !*needle)
	திரும்பு FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    போது (*n_ptr)
    {
	முழு n_char = mb_ptr2char(n_ptr);
	முழு n_upper = MB_TOUPPER(n_char);
	முழு found = FALSE;

	போது (*h_ptr)
	{
	    முழு h_char = mb_ptr2char(h_ptr);
	    எனில் (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		நிறுத்து;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	எனில் (!found)
	    திரும்பு FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    திரும்பு OK;
}

வகைவரை கட்டமைப்பு match_struct
{
    முழு needle_len;
    முழு haystack_len;
    முழு lower_needle[MATCH_MAX_LEN];     // stores codepoints
    முழு lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    நிலையான score_t
compute_bonus_codepoint(முழு last_c, முழு c)
{
    எனில் (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	எனில் (IS_PATH_SEP(last_c))
	    திரும்பு SCORE_MATCH_SLASH;
	எனில் (IS_WORD_SEP(last_c))
	    திரும்பு SCORE_MATCH_WORD;
	எனில் (IS_DOT(last_c))
	    திரும்பு SCORE_MATCH_DOT;
	எனில் (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    திரும்பு SCORE_MATCH_CAPITAL;
    }
    திரும்பு 0;
}

    நிலையான வெறுமை
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    முழு i = 0;
    char_u *p = needle;
    போது (*p != NUL && i < MATCH_MAX_LEN)
    {
	முழு c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    முழு prev_c = '/';
    போது (*p != NUL && i < MATCH_MAX_LEN)
    {
	முழு c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    நிலையான உள்வரி வெறுமை
match_row(மாறிலி match_struct *match, முழு row, score_t *curr_D,
	score_t *curr_M, மாறிலி score_t *last_D, மாறிலி score_t *last_M)
{
    முழு n = match->needle_len;
    முழு m = match->haystack_len;
    முழு i = row;

    மாறிலி முழு *lower_needle = match->lower_needle;
    மாறிலி முழு *lower_haystack = match->lower_haystack;
    மாறிலி score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    க்கு (முழு j = 0; j < m; j++)
    {
	எனில் (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    எனில் (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    இல்லையெனில் எனில் (j)
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
	இல்லையெனில்
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    நிலையான score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    எனில் (!needle || !haystack || !*needle)
	திரும்பு SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    முழு n = match.needle_len;
    முழு m = match.haystack_len;

    எனில் (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	திரும்பு SCORE_MIN;
    }
    இல்லையெனில் எனில் (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	க்கு (முழு i = 0; i < n; i++)
	{
	    எனில் (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		நிறுத்து;
	    }
	}
	எனில் (equal)
	{
	    எனில் (positions)
	    {
		க்கு (முழு i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    திரும்பு SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    எனில் ((size_t)n > (SIZE_MAX / அளவு(score_t)) / MATCH_MAX_LEN / 2)
	திரும்பு SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(அளவு(score_t) * MATCH_MAX_LEN * n * 2);
    எனில் (!block)
	திரும்பு SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    க்கு (முழு i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    எனில் (positions)
    {
	முழு match_required = 0;
	க்கு (முழு i = n - 1, j = m - 1; i >= 0; i--)
	{
	    க்கு (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		எனில் (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    நிறுத்து;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    திரும்பு result;
}
