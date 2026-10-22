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

#सचे defined(FEAT_EVAL) || defined(FEAT_PROTO)
स्थिर पूर्णाङ्क fuzzy_match_item_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2);
स्थिर शून्य fuzzy_match_in_list(list_T *l, char_u *str, पूर्णाङ्क matchseq, char_u *कुञ्चिका, callback_T *item_cb, पूर्णाङ्क retmatchpos, list_T *fmatchlist, long max_matches);
स्थिर शून्य do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णाङ्क retmatchpos);
#endif
स्थिर पूर्णाङ्क fuzzy_match_str_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2);
स्थिर शून्य fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz);
स्थिर पूर्णाङ्क fuzzy_match_func_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2);
स्थिर शून्य fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz);

प्रकारनिर्देश द्विगुण score_t;
स्थिर score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
स्थिर पूर्णाङ्क has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		idx;		// used for stable sort
    listitem_T	*item;
    पूर्णाङ्क		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    पूर्णाङ्क		itemstr_allocated;
    पूर्णाङ्क		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    पूर्णाङ्क
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    पूर्णाङ्क		matchseq,
    पूर्णाङ्क		*outScore,
    int_u	*matches,
    पूर्णाङ्क		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    पूर्णाङ्क		complete = FALSE;
    पूर्णाङ्क		score = 0;
    पूर्णाङ्क		अङ्कः_Matches = 0;
    पूर्णाङ्क		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    सचे (save_pat == रिक्त)
	पटिनिवत्त FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    याव (TRUE)
    {
	सचे (matchseq)
	    complete = TRUE;
	अञ्ञथा
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    सचे (*p == NUL)
		भङ्ग;
	    pat = p;
	    याव (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		सचे (has_mbyte)
		    MB_PTR_ADV(p);
		अञ्ञथा
		    ++p;
	    }
	    सचे (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	सचे (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	सचे (अङ्कः_Matches > maxMatches - pat_chars)
	{
	    अङ्कः_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    भङ्ग;
	}

	score = FUZZY_SCORE_NONE;
	सचे (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + अङ्कः_Matches);
	    सचे (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (पूर्णाङ्क)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (पूर्णाङ्क)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	सचे (score == FUZZY_SCORE_NONE)
	{
	    अङ्कः_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    भङ्ग;
	}

	सचे (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	अञ्ञथा सचे (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	अञ्ञथा
	    *outScore += score;

	अङ्कः_Matches += pat_chars;

	सचे (complete || अङ्कः_Matches >= maxMatches)
	    भङ्ग;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    पटिनिवत्त अङ्कः_Matches != 0;
}

#सचे defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    स्थिर पूर्णाङ्क
fuzzy_match_item_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzzyItem_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzzyItem_T *)s2)->score;

    सचे (v1 == v2)
    {
	पूर्णाङ्क exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	पूर्णाङ्क patlen = (पूर्णाङ्क)STRLEN(pat);
	पूर्णाङ्क startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	सचे (exact_match1 == exact_match2)
	{
	    पूर्णाङ्क idx1 = ((fuzzyItem_T *)s1)->idx;
	    पूर्णाङ्क idx2 = ((fuzzyItem_T *)s2)->idx;
	    पटिनिवत्त idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	अञ्ञथा सचे (exact_match2)
	    पटिनिवत्त 1;
	पटिनिवत्त -1;
    }
    अञ्ञथा
	पटिनिवत्त v1 > v2 ? -1 : 1;
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
    पूर्णाङ्क		matchseq,
    char_u	*कुञ्चिका,
    callback_T	*item_cb,
    पूर्णाङ्क		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *वस्तूनि = रिक्त;
    listitem_T	    *li;
    long	    match_गणनम् = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    सचे (len == 0)
	पटिनिवत्त;
    सचे (max_matches > 0 && len > max_matches)
	len = max_matches;

    वस्तूनि = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    सचे (वस्तूनि == रिक्त)
	पटिनिवत्त;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_वस्तूनि(l, li)
    {
	पूर्णाङ्क		score;
	char_u		*itemstr = रिक्त;
	char_u		*itemstr_copy = रिक्त;
	typval_T	rettv;
	पूर्णाङ्क		itemstr_allocate = FALSE;
	list_T		*match_positions = रिक्त;

	सचे (max_matches > 0 && match_गणनम् >= max_matches)
	    भङ्ग;

	rettv.v_type = VAR_UNKNOWN;
	सचे (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	अञ्ञथा सचे (li->li_tv.v_type == VAR_DICT
				&& (कुञ्चिका != रिक्त || item_cb->cb_name != रिक्त))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    सचे (कुञ्चिका != रिक्त)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (वर्ण *)कुञ्चिका, FALSE);
	    अञ्ञथा
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		सचे (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    सचे (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	सचे (itemstr != रिक्त
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    सचे (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		सचे (itemstr_copy == रिक्त)
		{
		    clear_tv(&rettv);
		    अनुवर्त;
		}
	    }
	    अञ्ञथा
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    सचे (retmatchpos)
	    {
		match_positions = list_alloc();
		सचे (match_positions == रिक्त)
		{
		    सचे (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    अनुवर्त;
		}

		// Fill position information
		पूर्णाङ्क	j = 0;
		char_u	*p = str;
		पूर्णाङ्क	success = TRUE;

		याव (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    सचे (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			सचे (list_append_सङ्ख्या(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    भङ्ग;
			}
			j++;
		    }
		    सचे (has_mbyte)
			MB_PTR_ADV(p);
		    अञ्ञथा
			++p;
		}

		सचे (!success)
		{
		    list_free(match_positions);
		    सचे (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    अनुवर्त;
		}
	    }
	    वस्तूनि[match_गणनम्].idx = match_गणनम्;
	    वस्तूनि[match_गणनम्].item = li;
	    वस्तूनि[match_गणनम्].score = score;
	    वस्तूनि[match_गणनम्].pat = str;
	    वस्तूनि[match_गणनम्].startpos = matches[0];
	    वस्तूनि[match_गणनम्].itemstr = itemstr_copy;
	    वस्तूनि[match_गणनम्].itemstr_allocated = itemstr_allocate;
	    वस्तूनि[match_गणनम्].lmatchpos = match_positions;

	    ++match_गणनम्;
	}
	clear_tv(&rettv);
    }

    सचे (match_गणनम् > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((शून्य *)वस्तूनि, (size_t)match_गणनम्, परिमाण(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	सचे (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    सचे (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;
	}
	अञ्ञथा
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	अत्थाय (पूर्णाङ्क i = 0; i < match_गणनम्; i++)
	{
	    सचे (list_append_tv(retlist, &वस्तूनि[i].item->li_tv) == FAIL)
		गच्छ done;
	}

	// next copy the list of matching positions
	सचे (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    सचे (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;

	    अत्थाय (पूर्णाङ्क i = 0; i < match_गणनम्; i++)
	    {
		सचे (वस्तूनि[i].lmatchpos != रिक्त)
		{
		    सचे (list_append_list(retlist, वस्तूनि[i].lmatchpos) == OK)
			वस्तूनि[i].lmatchpos = रिक्त;
		    अञ्ञथा
			गच्छ done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    सचे (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;
	    अत्थाय (पूर्णाङ्क i = 0; i < match_गणनम्; i++)
	    {
		सचे (list_append_सङ्ख्या(retlist, वस्तूनि[i].score) == FAIL)
		    गच्छ done;
	    }
	}
    }

done:
    अत्थाय (पूर्णाङ्क i = 0; i < match_गणनम्; i++)
    {
	सचे (वस्तूनि[i].itemstr_allocated)
	    vim_free(वस्तूनि[i].itemstr);

	सचे (वस्तूनि[i].lmatchpos)
	    list_free(वस्तूनि[i].lmatchpos);
    }
    vim_free(वस्तूनि);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    स्थिर शून्य
do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णाङ्क retmatchpos)
{
    callback_T	cb;
    char_u	*कुञ्चिका = रिक्त;
    पूर्णाङ्क		ret;
    पूर्णाङ्क		matchseq = FALSE;
    long	max_matches = 0;

    सचे (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    सचे (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == रिक्त)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	पटिनिवत्त;
    }
    सचे (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == रिक्त)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	पटिनिवत्त;
    }

    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	सचे (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    पटिनिवत्त;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	सचे ((di = dict_find(d, (char_u *)"key", -1)) != रिक्त)
	{
	    सचे (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == रिक्त
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_मानम्_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		पटिनिवत्त;
	    }
	    कुञ्चिका = tv_get_string(&di->di_tv);
	}
	अञ्ञथा सचे ((di = dict_find(d, (char_u *)"text_cb", -1)) != रिक्त)
	{
	    cb = get_callback(&di->di_tv);
	    सचे (cb.cb_name == रिक्त)
	    {
		semsg(_(e_invalid_मानम्_for_argument_str), "text_cb");
		पटिनिवत्त;
	    }
	}

	सचे ((di = dict_find(d, (char_u *)"limit", -1)) != रिक्त)
	{
	    सचे (di->di_tv.v_type != VAR_सङ्ख्या)
	    {
		semsg(_(e_invalid_मानम्_for_argument_str), "limit");
		पटिनिवत्त;
	    }
	    max_matches = (long)tv_get_सङ्ख्या_chk(&di->di_tv, रिक्त);
	}

	सचे (dict_has_कुञ्चिका(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    सचे (ret == FAIL)
	गच्छ done;
    सचे (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	सचे (l == रिक्त)
	    गच्छ done;
	सचे (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
	l = list_alloc();
	सचे (l == रिक्त)
	    गच्छ done;
	सचे (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
	l = list_alloc();
	सचे (l == रिक्त)
	    गच्छ done;
	सचे (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, कुञ्चिका, &cb, retmatchpos, rettv->vval.v_list, max_matches);

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
    स्थिर पूर्णाङ्क
fuzzy_match_str_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णाङ्क		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णाङ्क		idx2 = ((fuzmatch_str_T *)s2)->idx;

    सचे (v1 == v2)
	पटिनिवत्त idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अञ्ञथा
	पटिनिवत्त v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    स्थिर शून्य
fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz)
{
    // Sort the list by the descending order of the match score
    qsort((शून्य *)fm, (size_t)sz, परिमाण(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    स्थिर पूर्णाङ्क
fuzzy_match_func_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णाङ्क		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णाङ्क		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    सचे (*str1 != '<' && *str2 == '<')
	पटिनिवत्त -1;
    सचे (*str1 == '<' && *str2 != '<')
	पटिनिवत्त 1;
    सचे (v1 == v2)
	पटिनिवत्त idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अञ्ञथा
	पटिनिवत्त v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    स्थिर शून्य
fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz)
{
    // Sort the list by the descending order of the match score
    qsort((शून्य *)fm, (size_t)sz, परिमाण(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    पूर्णाङ्क
fuzzy_match_str(char_u *str, char_u *pat)
{
    पूर्णाङ्क		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    सचे (str == रिक्त || pat == रिक्त)
	पटिनिवत्त score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				परिमाण(matchpos) / परिमाण(matchpos[0]));

    पटिनिवत्त score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_अन्वेषणम्_EXTRA
    पूर्णाङ्क		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = रिक्त;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    पूर्णाङ्क		    j = 0;

    सचे (str == रिक्त || pat == रिक्त)
	पटिनिवत्त रिक्त;

    match_positions = ALLOC_ONE(garray_T);
    सचे (match_positions == रिक्त)
	पटिनिवत्त रिक्त;
    ga_init2(match_positions, परिमाण(int_u), 10);

    सचे (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	पटिनिवत्त रिक्त;
    }

    अत्थाय (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	सचे (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    सचे (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		पटिनिवत्त रिक्त;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    पटिनिवत्त match_positions;
#अञ्ञथा
    पटिनिवत्त रिक्त;
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
    पूर्णाङ्क
fuzzy_match_str_in_पङ्क्तिः(
    char_u	**ptr,
    char_u	*pat,
    पूर्णाङ्क		*len,
    pos_T	*current_pos,
    पूर्णाङ्क		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = रिक्त;
    char_u	*start = रिक्त;
    पूर्णाङ्क		found = FALSE;
    वर्ण	save_end;
    char_u	*पङ्क्तिः_end = रिक्त;

    सचे (str == रिक्त || pat == रिक्त)
	पटिनिवत्त found;
    पङ्क्तिः_end = find_पङ्क्तिः_end(str);

    याव (str < पङ्क्तिः_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	सचे (*start == NUL)
	    भङ्ग;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	सचे (*score != FUZZY_SCORE_NONE)
	{
	    *len = (पूर्णाङ्क)(end - start);
	    found = TRUE;
	    *ptr = start;
	    सचे (current_pos)
		current_pos->col += (पूर्णाङ्क)(end - strBegin);
	    भङ्ग;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	याव (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    सचे (!found)
	*ptr = पङ्क्तिः_end;

    पटिनिवत्त found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    पूर्णाङ्क
अन्वेषणम्_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    पूर्णाङ्क		dir,
    pos_T	*start_pos,
    पूर्णाङ्क		*len,
    char_u	**ptr,
    पूर्णाङ्क		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    पूर्णाङ्क		found_new_match = FALSE;
    पूर्णाङ्क		looped_around = FALSE;
    पूर्णाङ्क		whole_पङ्क्तिः = ctrl_x_mode_whole_पङ्क्तिः();

    सचे (buf == curbuf)
	circly_end = *start_pos;
    अञ्ञथा
    {
	circly_end.lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    सचे (whole_पङ्क्तिः && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    करोहि
    {

	// Check if looped around and back to start position
	सचे (looped_around && (whole_पङ्क्तिः ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    भङ्ग;

	// Ensure current_pos is valid
	सचे (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    सचे (!whole_पङ्क्तिः)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    सचे (*ptr != रिक्त && **ptr != NUL)
	    {
		सचे (!whole_पङ्क्तिः)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_पङ्क्तिः(ptr, pattern,
						    len, &current_pos, score);
		    सचे (found_new_match)
		    {
			*pos = current_pos;
			भङ्ग;
		    }
		    अञ्ञथा सचे (looped_around && current_pos.lnum == circly_end.lnum)
			भङ्ग;
		}
		अञ्ञथा
		{
		    char_u *पङ्क्तिः = *ptr;
		    char_u *p = skipwhite(पङ्क्तिः);
		    सचे (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (पूर्णाङ्क)ml_get_buf_len(buf, current_pos.lnum) -
								(पूर्णाङ्क)(p - पङ्क्तिः);
			भङ्ग;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	सचे (dir == FORWARD)
	{
	    सचे (++current_pos.lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		सचे (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		अञ्ञथा
		    भङ्ग;
	    }
	}
	अञ्ञथा
	{
	    सचे (--current_pos.lnum < 1)
	    {
		सचे (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;
		    looped_around = TRUE;
		}
		अञ्ञथा
		    भङ्ग;

	    }
	}
	current_pos.col = 0;
    } याव (TRUE);

    पटिनिवत्त found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    शून्य
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, पूर्णाङ्क गणनम्)
{
    सचे (fuzmatch == रिक्त)
	पटिनिवत्त;

    अत्थाय (पूर्णाङ्क i = 0; i < गणनम्; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    पूर्णाङ्क
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	पूर्णाङ्क		गणनम्,
	पूर्णाङ्क		funcsort)
{
    सचे (गणनम् <= 0)
	गच्छ theend;

    *matches = ALLOC_MULT(char_u *, गणनम्);
    सचे (*matches == रिक्त)
    {
	fuzmatch_str_free(fuzmatch, गणनम्);
	पटिनिवत्त FAIL;
    }

    // Sort the list by the descending order of the match score
    सचे (funcsort)
	fuzzy_match_func_sort((शून्य *)fuzmatch, (size_t)गणनम्);
    अञ्ञथा
	fuzzy_match_str_sort((शून्य *)fuzmatch, (size_t)गणनम्);

    अत्थाय (पूर्णाङ्क i = 0; i < गणनम्; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    पटिनिवत्त OK;
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

    स्थिर पूर्णाङ्क
has_match(char_u *needle, char_u *haystack)
{
    सचे (!needle || !haystack || !*needle)
	पटिनिवत्त FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    याव (*n_ptr)
    {
	पूर्णाङ्क n_char = mb_ptr2char(n_ptr);
	पूर्णाङ्क n_upper = MB_TOUPPER(n_char);
	पूर्णाङ्क found = FALSE;

	याव (*h_ptr)
	{
	    पूर्णाङ्क h_char = mb_ptr2char(h_ptr);
	    सचे (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		भङ्ग;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	सचे (!found)
	    पटिनिवत्त FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    पटिनिवत्त OK;
}

प्रकारनिर्देश संरचना match_struct
{
    पूर्णाङ्क needle_len;
    पूर्णाङ्क haystack_len;
    पूर्णाङ्क lower_needle[MATCH_MAX_LEN];     // stores codepoints
    पूर्णाङ्क lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    स्थिर score_t
compute_bonus_codepoint(पूर्णाङ्क last_c, पूर्णाङ्क c)
{
    सचे (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	सचे (IS_PATH_SEP(last_c))
	    पटिनिवत्त SCORE_MATCH_SLASH;
	सचे (IS_WORD_SEP(last_c))
	    पटिनिवत्त SCORE_MATCH_WORD;
	सचे (IS_DOT(last_c))
	    पटिनिवत्त SCORE_MATCH_DOT;
	सचे (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    पटिनिवत्त SCORE_MATCH_CAPITAL;
    }
    पटिनिवत्त 0;
}

    स्थिर शून्य
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    पूर्णाङ्क i = 0;
    char_u *p = needle;
    याव (*p != NUL && i < MATCH_MAX_LEN)
    {
	पूर्णाङ्क c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    पूर्णाङ्क prev_c = '/';
    याव (*p != NUL && i < MATCH_MAX_LEN)
    {
	पूर्णाङ्क c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    स्थिर अन्तःस्थ शून्य
match_row(ध्रुव match_struct *match, पूर्णाङ्क row, score_t *curr_D,
	score_t *curr_M, ध्रुव score_t *last_D, ध्रुव score_t *last_M)
{
    पूर्णाङ्क n = match->needle_len;
    पूर्णाङ्क m = match->haystack_len;
    पूर्णाङ्क i = row;

    ध्रुव पूर्णाङ्क *lower_needle = match->lower_needle;
    ध्रुव पूर्णाङ्क *lower_haystack = match->lower_haystack;
    ध्रुव score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    अत्थाय (पूर्णाङ्क j = 0; j < m; j++)
    {
	सचे (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    सचे (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    अञ्ञथा सचे (j)
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
	अञ्ञथा
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
    सचे (!needle || !haystack || !*needle)
	पटिनिवत्त SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    पूर्णाङ्क n = match.needle_len;
    पूर्णाङ्क m = match.haystack_len;

    सचे (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	पटिनिवत्त SCORE_MIN;
    }
    अञ्ञथा सचे (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	अत्थाय (पूर्णाङ्क i = 0; i < n; i++)
	{
	    सचे (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		भङ्ग;
	    }
	}
	सचे (equal)
	{
	    सचे (positions)
	    {
		अत्थाय (पूर्णाङ्क i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    पटिनिवत्त SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    सचे ((size_t)n > (SIZE_MAX / परिमाण(score_t)) / MATCH_MAX_LEN / 2)
	पटिनिवत्त SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(परिमाण(score_t) * MATCH_MAX_LEN * n * 2);
    सचे (!block)
	पटिनिवत्त SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    अत्थाय (पूर्णाङ्क i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    सचे (positions)
    {
	पूर्णाङ्क match_required = 0;
	अत्थाय (पूर्णाङ्क i = n - 1, j = m - 1; i >= 0; i--)
	{
	    अत्थाय (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		सचे (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    भङ्ग;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    पटिनिवत्त result;
}
