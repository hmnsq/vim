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
#अन्तर्भाव "vim.h"

#जर defined(FEAT_EVAL) || defined(FEAT_PROTO)
स्थिर पूर्णांक fuzzy_match_item_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2);
स्थिर शून्य fuzzy_match_in_list(list_T *l, char_u *str, पूर्णांक matchseq, char_u *किल्ली, callback_T *item_cb, पूर्णांक retmatchpos, list_T *fmatchlist, दीर्घ max_matches);
स्थिर शून्य do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णांक retmatchpos);
#endif
स्थिर पूर्णांक fuzzy_match_str_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2);
स्थिर शून्य fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णांक sz);
स्थिर पूर्णांक fuzzy_match_func_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2);
स्थिर शून्य fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णांक sz);

प्रकारव्याख्या द्विगुण score_t;
स्थिर score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
स्थिर पूर्णांक has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX अनन्त
#define SCORE_MIN (-अनन्त)
#define SCORE_SCALE 1000

प्रकारव्याख्या संरचना
{
    पूर्णांक		idx;		// used for stable sort
    listitem_T	*item;
    पूर्णांक		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    पूर्णांक		itemstr_allocated;
    पूर्णांक		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    पूर्णांक
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    पूर्णांक		matchseq,
    पूर्णांक		*outScore,
    int_u	*matches,
    पूर्णांक		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    पूर्णांक		complete = FALSE;
    पूर्णांक		score = 0;
    पूर्णांक		अंक_Matches = 0;
    पूर्णांक		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    जर (save_pat == रिक्त)
	परत FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    पर्यंत (TRUE)
    {
	जर (matchseq)
	    complete = TRUE;
	अन्यथा
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    जर (*p == NUL)
		थांबा;
	    pat = p;
	    पर्यंत (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		जर (has_mbyte)
		    MB_PTR_ADV(p);
		अन्यथा
		    ++p;
	    }
	    जर (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	जर (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	जर (अंक_Matches > maxMatches - pat_chars)
	{
	    अंक_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    थांबा;
	}

	score = FUZZY_SCORE_NONE;
	जर (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + अंक_Matches);
	    जर (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? पूर्णांक_उच्च
		    : (fzy_score < 0) ? (पूर्णांक)उच्चमान(fzy_score * SCORE_SCALE - 0.5)
		    : (पूर्णांक)न्यूनमान(fzy_score * SCORE_SCALE + 0.5);
	}

	जर (score == FUZZY_SCORE_NONE)
	{
	    अंक_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    थांबा;
	}

	जर (score > 0 && *outScore > पूर्णांक_उच्च - score)
	    *outScore = पूर्णांक_उच्च;
	अन्यथा जर (score < 0 && *outScore < पूर्णांक_न्यून + 1 - score)
	    *outScore = पूर्णांक_न्यून + 1;
	अन्यथा
	    *outScore += score;

	अंक_Matches += pat_chars;

	जर (complete || अंक_Matches >= maxMatches)
	    थांबा;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    परत अंक_Matches != 0;
}

#जर defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    स्थिर पूर्णांक
fuzzy_match_item_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    पूर्णांक		v1 = ((fuzzyItem_T *)s1)->score;
    पूर्णांक		v2 = ((fuzzyItem_T *)s2)->score;

    जर (v1 == v2)
    {
	पूर्णांक exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	पूर्णांक patlen = (पूर्णांक)STRLEN(pat);
	पूर्णांक startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	जर (exact_match1 == exact_match2)
	{
	    पूर्णांक idx1 = ((fuzzyItem_T *)s1)->idx;
	    पूर्णांक idx2 = ((fuzzyItem_T *)s2)->idx;
	    परत idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	अन्यथा जर (exact_match2)
	    परत 1;
	परत -1;
    }
    अन्यथा
	परत v1 > v2 ? -1 : 1;
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
    स्थिर शून्य
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    पूर्णांक		matchseq,
    char_u	*किल्ली,
    callback_T	*item_cb,
    पूर्णांक		retmatchpos,
    list_T	*fmatchlist,
    दीर्घ	max_matches)
{
    दीर्घ	    len;
    fuzzyItem_T	    *वस्तुसंच = रिक्त;
    listitem_T	    *li;
    दीर्घ	    match_मोजणी = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    जर (len == 0)
	परत;
    जर (max_matches > 0 && len > max_matches)
	len = max_matches;

    वस्तुसंच = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    जर (वस्तुसंच == रिक्त)
	परत;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	पूर्णांक		score;
	char_u		*itemstr = रिक्त;
	char_u		*itemstr_copy = रिक्त;
	typval_T	rettv;
	पूर्णांक		itemstr_allocate = FALSE;
	list_T		*match_positions = रिक्त;

	जर (max_matches > 0 && match_मोजणी >= max_matches)
	    थांबा;

	rettv.v_type = VAR_UNKNOWN;
	जर (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	अन्यथा जर (li->li_tv.v_type == VAR_DICT
				&& (किल्ली != रिक्त || item_cb->cb_name != रिक्त))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    जर (किल्ली != रिक्त)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (अक्षर *)किल्ली, FALSE);
	    अन्यथा
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		जर (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    जर (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	जर (itemstr != रिक्त
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    जर (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		जर (itemstr_copy == रिक्त)
		{
		    clear_tv(&rettv);
		    सुरूठेवा;
		}
	    }
	    अन्यथा
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    जर (retmatchpos)
	    {
		match_positions = list_alloc();
		जर (match_positions == रिक्त)
		{
		    जर (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    सुरूठेवा;
		}

		// Fill position information
		पूर्णांक	j = 0;
		char_u	*p = str;
		पूर्णांक	success = TRUE;

		पर्यंत (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    जर (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			जर (list_append_संख्या(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    थांबा;
			}
			j++;
		    }
		    जर (has_mbyte)
			MB_PTR_ADV(p);
		    अन्यथा
			++p;
		}

		जर (!success)
		{
		    list_free(match_positions);
		    जर (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    सुरूठेवा;
		}
	    }
	    वस्तुसंच[match_मोजणी].idx = match_मोजणी;
	    वस्तुसंच[match_मोजणी].item = li;
	    वस्तुसंच[match_मोजणी].score = score;
	    वस्तुसंच[match_मोजणी].pat = str;
	    वस्तुसंच[match_मोजणी].startpos = matches[0];
	    वस्तुसंच[match_मोजणी].itemstr = itemstr_copy;
	    वस्तुसंच[match_मोजणी].itemstr_allocated = itemstr_allocate;
	    वस्तुसंच[match_मोजणी].lmatchpos = match_positions;

	    ++match_मोजणी;
	}
	clear_tv(&rettv);
    }

    जर (match_मोजणी > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	क्विक((शून्य *)वस्तुसंच, (माप_प्रकार)match_मोजणी, आकार(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	जर (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    जर (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		जा done;
	    retlist = li->li_tv.vval.v_list;
	}
	अन्यथा
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	साठी (पूर्णांक i = 0; i < match_मोजणी; i++)
	{
	    जर (list_append_tv(retlist, &वस्तुसंच[i].item->li_tv) == FAIL)
		जा done;
	}

	// next copy the list of matching positions
	जर (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    जर (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		जा done;
	    retlist = li->li_tv.vval.v_list;

	    साठी (पूर्णांक i = 0; i < match_मोजणी; i++)
	    {
		जर (वस्तुसंच[i].lmatchpos != रिक्त)
		{
		    जर (list_append_list(retlist, वस्तुसंच[i].lmatchpos) == OK)
			वस्तुसंच[i].lmatchpos = रिक्त;
		    अन्यथा
			जा done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    जर (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		जा done;
	    retlist = li->li_tv.vval.v_list;
	    साठी (पूर्णांक i = 0; i < match_मोजणी; i++)
	    {
		जर (list_append_संख्या(retlist, वस्तुसंच[i].score) == FAIL)
		    जा done;
	    }
	}
    }

done:
    साठी (पूर्णांक i = 0; i < match_मोजणी; i++)
    {
	जर (वस्तुसंच[i].itemstr_allocated)
	    vim_free(वस्तुसंच[i].itemstr);

	जर (वस्तुसंच[i].lmatchpos)
	    list_free(वस्तुसंच[i].lmatchpos);
    }
    vim_free(वस्तुसंच);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    स्थिर शून्य
do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णांक retmatchpos)
{
    callback_T	cb;
    char_u	*किल्ली = रिक्त;
    पूर्णांक		ret;
    पूर्णांक		matchseq = FALSE;
    दीर्घ	max_matches = 0;

    जर (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    जर (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == रिक्त)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	परत;
    }
    जर (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == रिक्त)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	परत;
    }

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	जर (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    परत;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	जर ((di = dict_find(d, (char_u *)"key", -1)) != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == रिक्त
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		परत;
	    }
	    किल्ली = tv_get_string(&di->di_tv);
	}
	अन्यथा जर ((di = dict_find(d, (char_u *)"text_cb", -1)) != रिक्त)
	{
	    cb = get_callback(&di->di_tv);
	    जर (cb.cb_name == रिक्त)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str), "text_cb");
		परत;
	    }
	}

	जर ((di = dict_find(d, (char_u *)"limit", -1)) != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_संख्या)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str), "limit");
		परत;
	    }
	    max_matches = (दीर्घ)tv_get_संख्या_chk(&di->di_tv, रिक्त);
	}

	जर (dict_has_किल्ली(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    जर (ret == FAIL)
	जा done;
    जर (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	जर (l == रिक्त)
	    जा done;
	जर (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    जा done;
	}
	l = list_alloc();
	जर (l == रिक्त)
	    जा done;
	जर (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    जा done;
	}
	l = list_alloc();
	जर (l == रिक्त)
	    जा done;
	जर (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    जा done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, किल्ली, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    शून्य
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    शून्य
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    स्थिर पूर्णांक
fuzzy_match_str_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    पूर्णांक		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णांक		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णांक		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णांक		idx2 = ((fuzmatch_str_T *)s2)->idx;

    जर (v1 == v2)
	परत idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अन्यथा
	परत v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    स्थिर शून्य
fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णांक sz)
{
    // Sort the list by the descending order of the match score
    क्विक((शून्य *)fm, (माप_प्रकार)sz, आकार(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    स्थिर पूर्णांक
fuzzy_match_func_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    पूर्णांक		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णांक		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णांक		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णांक		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    जर (*str1 != '<' && *str2 == '<')
	परत -1;
    जर (*str1 == '<' && *str2 != '<')
	परत 1;
    जर (v1 == v2)
	परत idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अन्यथा
	परत v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    स्थिर शून्य
fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णांक sz)
{
    // Sort the list by the descending order of the match score
    क्विक((शून्य *)fm, (माप_प्रकार)sz, आकार(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    पूर्णांक
fuzzy_match_str(char_u *str, char_u *pat)
{
    पूर्णांक		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    जर (str == रिक्त || pat == रिक्त)
	परत score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				आकार(matchpos) / आकार(matchpos[0]));

    परत score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_शोध_EXTRA
    पूर्णांक		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = रिक्त;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    पूर्णांक		    j = 0;

    जर (str == रिक्त || pat == रिक्त)
	परत रिक्त;

    match_positions = ALLOC_ONE(garray_T);
    जर (match_positions == रिक्त)
	परत रिक्त;
    ga_init2(match_positions, आकार(int_u), 10);

    जर (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	परत रिक्त;
    }

    साठी (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	जर (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    जर (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		परत रिक्त;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    परत match_positions;
#अन्यथा
    परत रिक्त;
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
    पूर्णांक
fuzzy_match_str_in_ओळ(
    char_u	**ptr,
    char_u	*pat,
    पूर्णांक		*len,
    pos_T	*current_pos,
    पूर्णांक		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = रिक्त;
    char_u	*start = रिक्त;
    पूर्णांक		found = FALSE;
    अक्षर	save_end;
    char_u	*ओळ_end = रिक्त;

    जर (str == रिक्त || pat == रिक्त)
	परत found;
    ओळ_end = find_ओळ_end(str);

    पर्यंत (str < ओळ_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	जर (*start == NUL)
	    थांबा;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	जर (*score != FUZZY_SCORE_NONE)
	{
	    *len = (पूर्णांक)(end - start);
	    found = TRUE;
	    *ptr = start;
	    जर (current_pos)
		current_pos->col += (पूर्णांक)(end - strBegin);
	    थांबा;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	पर्यंत (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    जर (!found)
	*ptr = ओळ_end;

    परत found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    पूर्णांक
शोध_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    पूर्णांक		dir,
    pos_T	*start_pos,
    पूर्णांक		*len,
    char_u	**ptr,
    पूर्णांक		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    पूर्णांक		found_new_match = FALSE;
    पूर्णांक		looped_around = FALSE;
    पूर्णांक		whole_ओळ = ctrl_x_mode_whole_ओळ();

    जर (buf == curbuf)
	circly_end = *start_pos;
    अन्यथा
    {
	circly_end.lnum = buf->b_ml.ml_ओळ_मोजणी;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    जर (whole_ओळ && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    करा
    {

	// Check if looped around and back to start position
	जर (looped_around && (whole_ओळ ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    थांबा;

	// Ensure current_pos is valid
	जर (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_ओळ_मोजणी)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    जर (!whole_ओळ)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    जर (*ptr != रिक्त && **ptr != NUL)
	    {
		जर (!whole_ओळ)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_ओळ(ptr, pattern,
						    len, &current_pos, score);
		    जर (found_new_match)
		    {
			*pos = current_pos;
			थांबा;
		    }
		    अन्यथा जर (looped_around && current_pos.lnum == circly_end.lnum)
			थांबा;
		}
		अन्यथा
		{
		    char_u *ओळ = *ptr;
		    char_u *p = skipwhite(ओळ);
		    जर (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (पूर्णांक)ml_get_buf_len(buf, current_pos.lnum) -
								(पूर्णांक)(p - ओळ);
			थांबा;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	जर (dir == FORWARD)
	{
	    जर (++current_pos.lnum > buf->b_ml.ml_ओळ_मोजणी)
	    {
		जर (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		अन्यथा
		    थांबा;
	    }
	}
	अन्यथा
	{
	    जर (--current_pos.lnum < 1)
	    {
		जर (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_ओळ_मोजणी;
		    looped_around = TRUE;
		}
		अन्यथा
		    थांबा;

	    }
	}
	current_pos.col = 0;
    } पर्यंत (TRUE);

    परत found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    शून्य
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, पूर्णांक मोजणी)
{
    जर (fuzmatch == रिक्त)
	परत;

    साठी (पूर्णांक i = 0; i < मोजणी; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    पूर्णांक
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	पूर्णांक		मोजणी,
	पूर्णांक		funcsort)
{
    जर (मोजणी <= 0)
	जा theend;

    *matches = ALLOC_MULT(char_u *, मोजणी);
    जर (*matches == रिक्त)
    {
	fuzmatch_str_free(fuzmatch, मोजणी);
	परत FAIL;
    }

    // Sort the list by the descending order of the match score
    जर (funcsort)
	fuzzy_match_func_sort((शून्य *)fuzmatch, (माप_प्रकार)मोजणी);
    अन्यथा
	fuzzy_match_str_sort((शून्य *)fuzmatch, (माप_प्रकार)मोजणी);

    साठी (पूर्णांक i = 0; i < मोजणी; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    परत OK;
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

    स्थिर पूर्णांक
has_match(char_u *needle, char_u *haystack)
{
    जर (!needle || !haystack || !*needle)
	परत FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    पर्यंत (*n_ptr)
    {
	पूर्णांक n_char = mb_ptr2char(n_ptr);
	पूर्णांक n_upper = MB_TOUPPER(n_char);
	पूर्णांक found = FALSE;

	पर्यंत (*h_ptr)
	{
	    पूर्णांक h_char = mb_ptr2char(h_ptr);
	    जर (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		थांबा;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	जर (!found)
	    परत FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    परत OK;
}

प्रकारव्याख्या संरचना match_struct
{
    पूर्णांक needle_len;
    पूर्णांक haystack_len;
    पूर्णांक lower_needle[MATCH_MAX_LEN];     // stores codepoints
    पूर्णांक lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    स्थिर score_t
compute_bonus_codepoint(पूर्णांक last_c, पूर्णांक c)
{
    जर (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	जर (IS_PATH_SEP(last_c))
	    परत SCORE_MATCH_SLASH;
	जर (IS_WORD_SEP(last_c))
	    परत SCORE_MATCH_WORD;
	जर (IS_DOT(last_c))
	    परत SCORE_MATCH_DOT;
	जर (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    परत SCORE_MATCH_CAPITAL;
    }
    परत 0;
}

    स्थिर शून्य
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    पूर्णांक i = 0;
    char_u *p = needle;
    पर्यंत (*p != NUL && i < MATCH_MAX_LEN)
    {
	पूर्णांक c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    पूर्णांक prev_c = '/';
    पर्यंत (*p != NUL && i < MATCH_MAX_LEN)
    {
	पूर्णांक c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    स्थिर ओळीत शून्य
match_row(स्थिरांक match_struct *match, पूर्णांक row, score_t *curr_D,
	score_t *curr_M, स्थिरांक score_t *last_D, स्थिरांक score_t *last_M)
{
    पूर्णांक n = match->needle_len;
    पूर्णांक m = match->haystack_len;
    पूर्णांक i = row;

    स्थिरांक पूर्णांक *lower_needle = match->lower_needle;
    स्थिरांक पूर्णांक *lower_haystack = match->lower_haystack;
    स्थिरांक score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    साठी (पूर्णांक j = 0; j < m; j++)
    {
	जर (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    जर (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    अन्यथा जर (j)
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
	अन्यथा
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    स्थिर score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    जर (!needle || !haystack || !*needle)
	परत SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    पूर्णांक n = match.needle_len;
    पूर्णांक m = match.haystack_len;

    जर (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	परत SCORE_MIN;
    }
    अन्यथा जर (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	साठी (पूर्णांक i = 0; i < n; i++)
	{
	    जर (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		थांबा;
	    }
	}
	जर (equal)
	{
	    जर (positions)
	    {
		साठी (पूर्णांक i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    परत SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    जर ((माप_प्रकार)n > (SIZE_MAX / आकार(score_t)) / MATCH_MAX_LEN / 2)
	परत SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(आकार(score_t) * MATCH_MAX_LEN * n * 2);
    जर (!block)
	परत SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    साठी (पूर्णांक i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    जर (positions)
    {
	पूर्णांक match_required = 0;
	साठी (पूर्णांक i = n - 1, j = m - 1; i >= 0; i--)
	{
	    साठी (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		जर (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    थांबा;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    परत result;
}
