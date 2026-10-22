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

#यदि defined(FEAT_EVAL) || defined(FEAT_PROTO)
स्थिर पूर्णाङ्क fuzzy_match_item_compare(अचर शून्य *s1, अचर शून्य *s2);
स्थिर शून्य fuzzy_match_in_list(list_T *l, char_u *str, पूर्णाङ्क matchseq, char_u *साँचो, callback_T *item_cb, पूर्णाङ्क retmatchpos, list_T *fmatchlist, दीर्घ max_matches);
स्थिर शून्य do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णाङ्क retmatchpos);
#endif
स्थिर पूर्णाङ्क fuzzy_match_str_compare(अचर शून्य *s1, अचर शून्य *s2);
स्थिर शून्य fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz);
स्थिर पूर्णाङ्क fuzzy_match_func_compare(अचर शून्य *s1, अचर शून्य *s2);
स्थिर शून्य fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz);

प्रकारनिर्देश द्विगुण score_t;
स्थिर score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
स्थिर पूर्णाङ्क has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX अनन्त
#define SCORE_MIN (-अनन्त)
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
    पूर्णाङ्क		अङ्क_Matches = 0;
    पूर्णाङ्क		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    यदि (save_pat == रिक्त)
	फर्काऊ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    जबसम्म (TRUE)
    {
	यदि (matchseq)
	    complete = TRUE;
	अन्यथा
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    यदि (*p == NUL)
		रोक;
	    pat = p;
	    जबसम्म (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		यदि (has_mbyte)
		    MB_PTR_ADV(p);
		अन्यथा
		    ++p;
	    }
	    यदि (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	यदि (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	यदि (अङ्क_Matches > maxMatches - pat_chars)
	{
	    अङ्क_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    रोक;
	}

	score = FUZZY_SCORE_NONE;
	यदि (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + अङ्क_Matches);
	    यदि (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? पूर्णांक_उच्च
		    : (fzy_score < 0) ? (पूर्णाङ्क)उच्चमान(fzy_score * SCORE_SCALE - 0.5)
		    : (पूर्णाङ्क)न्यूनमान(fzy_score * SCORE_SCALE + 0.5);
	}

	यदि (score == FUZZY_SCORE_NONE)
	{
	    अङ्क_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    रोक;
	}

	यदि (score > 0 && *outScore > पूर्णांक_उच्च - score)
	    *outScore = पूर्णांक_उच्च;
	अन्यथा यदि (score < 0 && *outScore < पूर्णांक_न्यून + 1 - score)
	    *outScore = पूर्णांक_न्यून + 1;
	अन्यथा
	    *outScore += score;

	अङ्क_Matches += pat_chars;

	यदि (complete || अङ्क_Matches >= maxMatches)
	    रोक;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    फर्काऊ अङ्क_Matches != 0;
}

#यदि defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    स्थिर पूर्णाङ्क
fuzzy_match_item_compare(अचर शून्य *s1, अचर शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzzyItem_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzzyItem_T *)s2)->score;

    यदि (v1 == v2)
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

	यदि (exact_match1 == exact_match2)
	{
	    पूर्णाङ्क idx1 = ((fuzzyItem_T *)s1)->idx;
	    पूर्णाङ्क idx2 = ((fuzzyItem_T *)s2)->idx;
	    फर्काऊ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	अन्यथा यदि (exact_match2)
	    फर्काऊ 1;
	फर्काऊ -1;
    }
    अन्यथा
	फर्काऊ v1 > v2 ? -1 : 1;
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
    char_u	*साँचो,
    callback_T	*item_cb,
    पूर्णाङ्क		retmatchpos,
    list_T	*fmatchlist,
    दीर्घ	max_matches)
{
    दीर्घ	    len;
    fuzzyItem_T	    *वस्तुहरू = रिक्त;
    listitem_T	    *li;
    दीर्घ	    match_गन्ती = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    यदि (len == 0)
	फर्काऊ;
    यदि (max_matches > 0 && len > max_matches)
	len = max_matches;

    वस्तुहरू = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    यदि (वस्तुहरू == रिक्त)
	फर्काऊ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_वस्तुहरू(l, li)
    {
	पूर्णाङ्क		score;
	char_u		*itemstr = रिक्त;
	char_u		*itemstr_copy = रिक्त;
	typval_T	rettv;
	पूर्णाङ्क		itemstr_allocate = FALSE;
	list_T		*match_positions = रिक्त;

	यदि (max_matches > 0 && match_गन्ती >= max_matches)
	    रोक;

	rettv.v_type = VAR_UNKNOWN;
	यदि (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	अन्यथा यदि (li->li_tv.v_type == VAR_DICT
				&& (साँचो != रिक्त || item_cb->cb_name != रिक्त))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    यदि (साँचो != रिक्त)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (वर्ण *)साँचो, FALSE);
	    अन्यथा
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		यदि (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    यदि (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	यदि (itemstr != रिक्त
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    यदि (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		यदि (itemstr_copy == रिक्त)
		{
		    clear_tv(&rettv);
		    जारी;
		}
	    }
	    अन्यथा
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    यदि (retmatchpos)
	    {
		match_positions = list_alloc();
		यदि (match_positions == रिक्त)
		{
		    यदि (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    जारी;
		}

		// Fill position information
		पूर्णाङ्क	j = 0;
		char_u	*p = str;
		पूर्णाङ्क	success = TRUE;

		जबसम्म (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    यदि (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			यदि (list_append_सङ्ख्या(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    रोक;
			}
			j++;
		    }
		    यदि (has_mbyte)
			MB_PTR_ADV(p);
		    अन्यथा
			++p;
		}

		यदि (!success)
		{
		    list_free(match_positions);
		    यदि (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    जारी;
		}
	    }
	    वस्तुहरू[match_गन्ती].idx = match_गन्ती;
	    वस्तुहरू[match_गन्ती].item = li;
	    वस्तुहरू[match_गन्ती].score = score;
	    वस्तुहरू[match_गन्ती].pat = str;
	    वस्तुहरू[match_गन्ती].startpos = matches[0];
	    वस्तुहरू[match_गन्ती].itemstr = itemstr_copy;
	    वस्तुहरू[match_गन्ती].itemstr_allocated = itemstr_allocate;
	    वस्तुहरू[match_गन्ती].lmatchpos = match_positions;

	    ++match_गन्ती;
	}
	clear_tv(&rettv);
    }

    यदि (match_गन्ती > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	क्विक((शून्य *)वस्तुहरू, (माप_प्रकार)match_गन्ती, परिमाण(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	यदि (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    यदि (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;
	}
	अन्यथा
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	लागि (पूर्णाङ्क i = 0; i < match_गन्ती; i++)
	{
	    यदि (list_append_tv(retlist, &वस्तुहरू[i].item->li_tv) == FAIL)
		गच्छ done;
	}

	// next copy the list of matching positions
	यदि (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    यदि (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;

	    लागि (पूर्णाङ्क i = 0; i < match_गन्ती; i++)
	    {
		यदि (वस्तुहरू[i].lmatchpos != रिक्त)
		{
		    यदि (list_append_list(retlist, वस्तुहरू[i].lmatchpos) == OK)
			वस्तुहरू[i].lmatchpos = रिक्त;
		    अन्यथा
			गच्छ done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    यदि (li == रिक्त || li->li_tv.vval.v_list == रिक्त)
		गच्छ done;
	    retlist = li->li_tv.vval.v_list;
	    लागि (पूर्णाङ्क i = 0; i < match_गन्ती; i++)
	    {
		यदि (list_append_सङ्ख्या(retlist, वस्तुहरू[i].score) == FAIL)
		    गच्छ done;
	    }
	}
    }

done:
    लागि (पूर्णाङ्क i = 0; i < match_गन्ती; i++)
    {
	यदि (वस्तुहरू[i].itemstr_allocated)
	    vim_free(वस्तुहरू[i].itemstr);

	यदि (वस्तुहरू[i].lmatchpos)
	    list_free(वस्तुहरू[i].lmatchpos);
    }
    vim_free(वस्तुहरू);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    स्थिर शून्य
do_fuzzymatch(typval_T *argvars, typval_T *rettv, पूर्णाङ्क retmatchpos)
{
    callback_T	cb;
    char_u	*साँचो = रिक्त;
    पूर्णाङ्क		ret;
    पूर्णाङ्क		matchseq = FALSE;
    दीर्घ	max_matches = 0;

    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	फर्काऊ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    यदि (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == रिक्त)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	फर्काऊ;
    }
    यदि (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == रिक्त)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	फर्काऊ;
    }

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	यदि (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    फर्काऊ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	यदि ((di = dict_find(d, (char_u *)"key", -1)) != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == रिक्त
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_मान_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		फर्काऊ;
	    }
	    साँचो = tv_get_string(&di->di_tv);
	}
	अन्यथा यदि ((di = dict_find(d, (char_u *)"text_cb", -1)) != रिक्त)
	{
	    cb = get_callback(&di->di_tv);
	    यदि (cb.cb_name == रिक्त)
	    {
		semsg(_(e_invalid_मान_for_argument_str), "text_cb");
		फर्काऊ;
	    }
	}

	यदि ((di = dict_find(d, (char_u *)"limit", -1)) != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_सङ्ख्या)
	    {
		semsg(_(e_invalid_मान_for_argument_str), "limit");
		फर्काऊ;
	    }
	    max_matches = (दीर्घ)tv_get_सङ्ख्या_chk(&di->di_tv, रिक्त);
	}

	यदि (dict_has_साँचो(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    यदि (ret == FAIL)
	गच्छ done;
    यदि (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	यदि (l == रिक्त)
	    गच्छ done;
	यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
	l = list_alloc();
	यदि (l == रिक्त)
	    गच्छ done;
	यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
	l = list_alloc();
	यदि (l == रिक्त)
	    गच्छ done;
	यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    गच्छ done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, साँचो, &cb, retmatchpos, rettv->vval.v_list, max_matches);

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
fuzzy_match_str_compare(अचर शून्य *s1, अचर शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णाङ्क		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णाङ्क		idx2 = ((fuzmatch_str_T *)s2)->idx;

    यदि (v1 == v2)
	फर्काऊ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अन्यथा
	फर्काऊ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    स्थिर शून्य
fuzzy_match_str_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz)
{
    // Sort the list by the descending order of the match score
    क्विक((शून्य *)fm, (माप_प्रकार)sz, परिमाण(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    स्थिर पूर्णाङ्क
fuzzy_match_func_compare(अचर शून्य *s1, अचर शून्य *s2)
{
    पूर्णाङ्क		v1 = ((fuzmatch_str_T *)s1)->score;
    पूर्णाङ्क		v2 = ((fuzmatch_str_T *)s2)->score;
    पूर्णाङ्क		idx1 = ((fuzmatch_str_T *)s1)->idx;
    पूर्णाङ्क		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    यदि (*str1 != '<' && *str2 == '<')
	फर्काऊ -1;
    यदि (*str1 == '<' && *str2 != '<')
	फर्काऊ 1;
    यदि (v1 == v2)
	फर्काऊ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    अन्यथा
	फर्काऊ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    स्थिर शून्य
fuzzy_match_func_sort(fuzmatch_str_T *fm, पूर्णाङ्क sz)
{
    // Sort the list by the descending order of the match score
    क्विक((शून्य *)fm, (माप_प्रकार)sz, परिमाण(fuzmatch_str_T),
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

    यदि (str == रिक्त || pat == रिक्त)
	फर्काऊ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				परिमाण(matchpos) / परिमाण(matchpos[0]));

    फर्काऊ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_खोज_EXTRA
    पूर्णाङ्क		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = रिक्त;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    पूर्णाङ्क		    j = 0;

    यदि (str == रिक्त || pat == रिक्त)
	फर्काऊ रिक्त;

    match_positions = ALLOC_ONE(garray_T);
    यदि (match_positions == रिक्त)
	फर्काऊ रिक्त;
    ga_init2(match_positions, परिमाण(int_u), 10);

    यदि (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	फर्काऊ रिक्त;
    }

    लागि (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	यदि (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    यदि (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		फर्काऊ रिक्त;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    फर्काऊ match_positions;
#अन्यथा
    फर्काऊ रिक्त;
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
fuzzy_match_str_in_पङ्क्ति(
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
    char_u	*पङ्क्ति_end = रिक्त;

    यदि (str == रिक्त || pat == रिक्त)
	फर्काऊ found;
    पङ्क्ति_end = find_पङ्क्ति_end(str);

    जबसम्म (str < पङ्क्ति_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	यदि (*start == NUL)
	    रोक;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	यदि (*score != FUZZY_SCORE_NONE)
	{
	    *len = (पूर्णाङ्क)(end - start);
	    found = TRUE;
	    *ptr = start;
	    यदि (current_pos)
		current_pos->col += (पूर्णाङ्क)(end - strBegin);
	    रोक;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	जबसम्म (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    यदि (!found)
	*ptr = पङ्क्ति_end;

    फर्काऊ found;
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
खोज_for_fuzzy_match(
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
    पूर्णाङ्क		whole_पङ्क्ति = ctrl_x_mode_whole_पङ्क्ति();

    यदि (buf == curbuf)
	circly_end = *start_pos;
    अन्यथा
    {
	circly_end.lnum = buf->b_ml.ml_पङ्क्ति_गन्ती;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    यदि (whole_पङ्क्ति && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    गर
    {

	// Check if looped around and back to start position
	यदि (looped_around && (whole_पङ्क्ति ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    रोक;

	// Ensure current_pos is valid
	यदि (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_पङ्क्ति_गन्ती)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    यदि (!whole_पङ्क्ति)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    यदि (*ptr != रिक्त && **ptr != NUL)
	    {
		यदि (!whole_पङ्क्ति)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_पङ्क्ति(ptr, pattern,
						    len, &current_pos, score);
		    यदि (found_new_match)
		    {
			*pos = current_pos;
			रोक;
		    }
		    अन्यथा यदि (looped_around && current_pos.lnum == circly_end.lnum)
			रोक;
		}
		अन्यथा
		{
		    char_u *पङ्क्ति = *ptr;
		    char_u *p = skipwhite(पङ्क्ति);
		    यदि (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (पूर्णाङ्क)ml_get_buf_len(buf, current_pos.lnum) -
								(पूर्णाङ्क)(p - पङ्क्ति);
			रोक;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	यदि (dir == FORWARD)
	{
	    यदि (++current_pos.lnum > buf->b_ml.ml_पङ्क्ति_गन्ती)
	    {
		यदि (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		अन्यथा
		    रोक;
	    }
	}
	अन्यथा
	{
	    यदि (--current_pos.lnum < 1)
	    {
		यदि (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_पङ्क्ति_गन्ती;
		    looped_around = TRUE;
		}
		अन्यथा
		    रोक;

	    }
	}
	current_pos.col = 0;
    } जबसम्म (TRUE);

    फर्काऊ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    शून्य
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, पूर्णाङ्क गन्ती)
{
    यदि (fuzmatch == रिक्त)
	फर्काऊ;

    लागि (पूर्णाङ्क i = 0; i < गन्ती; ++i)
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
	पूर्णाङ्क		गन्ती,
	पूर्णाङ्क		funcsort)
{
    यदि (गन्ती <= 0)
	गच्छ theend;

    *matches = ALLOC_MULT(char_u *, गन्ती);
    यदि (*matches == रिक्त)
    {
	fuzmatch_str_free(fuzmatch, गन्ती);
	फर्काऊ FAIL;
    }

    // Sort the list by the descending order of the match score
    यदि (funcsort)
	fuzzy_match_func_sort((शून्य *)fuzmatch, (माप_प्रकार)गन्ती);
    अन्यथा
	fuzzy_match_str_sort((शून्य *)fuzmatch, (माप_प्रकार)गन्ती);

    लागि (पूर्णाङ्क i = 0; i < गन्ती; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    फर्काऊ OK;
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
    यदि (!needle || !haystack || !*needle)
	फर्काऊ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    जबसम्म (*n_ptr)
    {
	पूर्णाङ्क n_char = mb_ptr2char(n_ptr);
	पूर्णाङ्क n_upper = MB_TOUPPER(n_char);
	पूर्णाङ्क found = FALSE;

	जबसम्म (*h_ptr)
	{
	    पूर्णाङ्क h_char = mb_ptr2char(h_ptr);
	    यदि (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		रोक;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	यदि (!found)
	    फर्काऊ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    फर्काऊ OK;
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
    यदि (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	यदि (IS_PATH_SEP(last_c))
	    फर्काऊ SCORE_MATCH_SLASH;
	यदि (IS_WORD_SEP(last_c))
	    फर्काऊ SCORE_MATCH_WORD;
	यदि (IS_DOT(last_c))
	    फर्काऊ SCORE_MATCH_DOT;
	यदि (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    फर्काऊ SCORE_MATCH_CAPITAL;
    }
    फर्काऊ 0;
}

    स्थिर शून्य
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    पूर्णाङ्क i = 0;
    char_u *p = needle;
    जबसम्म (*p != NUL && i < MATCH_MAX_LEN)
    {
	पूर्णाङ्क c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    पूर्णाङ्क prev_c = '/';
    जबसम्म (*p != NUL && i < MATCH_MAX_LEN)
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
match_row(अचर match_struct *match, पूर्णाङ्क row, score_t *curr_D,
	score_t *curr_M, अचर score_t *last_D, अचर score_t *last_M)
{
    पूर्णाङ्क n = match->needle_len;
    पूर्णाङ्क m = match->haystack_len;
    पूर्णाङ्क i = row;

    अचर पूर्णाङ्क *lower_needle = match->lower_needle;
    अचर पूर्णाङ्क *lower_haystack = match->lower_haystack;
    अचर score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    लागि (पूर्णाङ्क j = 0; j < m; j++)
    {
	यदि (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    यदि (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    अन्यथा यदि (j)
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
    यदि (!needle || !haystack || !*needle)
	फर्काऊ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    पूर्णाङ्क n = match.needle_len;
    पूर्णाङ्क m = match.haystack_len;

    यदि (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	फर्काऊ SCORE_MIN;
    }
    अन्यथा यदि (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	लागि (पूर्णाङ्क i = 0; i < n; i++)
	{
	    यदि (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		रोक;
	    }
	}
	यदि (equal)
	{
	    यदि (positions)
	    {
		लागि (पूर्णाङ्क i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    फर्काऊ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    यदि ((माप_प्रकार)n > (SIZE_MAX / परिमाण(score_t)) / MATCH_MAX_LEN / 2)
	फर्काऊ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(परिमाण(score_t) * MATCH_MAX_LEN * n * 2);
    यदि (!block)
	फर्काऊ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    लागि (पूर्णाङ्क i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    यदि (positions)
    {
	पूर्णाङ्क match_required = 0;
	लागि (पूर्णाङ्क i = n - 1, j = m - 1; i >= 0; i--)
	{
	    लागि (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		यदि (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    रोक;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    फर्काऊ result;
}
