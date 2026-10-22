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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_PROTO)
സ്ഥിതം പൂർണ്ണസംഖ്യ fuzzy_match_item_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);
സ്ഥിതം ശൂന്യം fuzzy_match_in_list(list_T *l, char_u *str, പൂർണ്ണസംഖ്യ matchseq, char_u *താക്കോൽ, callback_T *item_cb, പൂർണ്ണസംഖ്യ retmatchpos, list_T *fmatchlist, long max_matches);
സ്ഥിതം ശൂന്യം do_fuzzymatch(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ retmatchpos);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ fuzzy_match_str_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);
സ്ഥിതം ശൂന്യം fuzzy_match_str_sort(fuzmatch_str_T *fm, പൂർണ്ണസംഖ്യ sz);
സ്ഥിതം പൂർണ്ണസംഖ്യ fuzzy_match_func_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);
സ്ഥിതം ശൂന്യം fuzzy_match_func_sort(fuzmatch_str_T *fm, പൂർണ്ണസംഖ്യ sz);

തരനിർവചനം ഇരട്ട score_t;
സ്ഥിതം score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
സ്ഥിതം പൂർണ്ണസംഖ്യ has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ		idx;		// used for stable sort
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    പൂർണ്ണസംഖ്യ		itemstr_allocated;
    പൂർണ്ണസംഖ്യ		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    പൂർണ്ണസംഖ്യ
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    പൂർണ്ണസംഖ്യ		matchseq,
    പൂർണ്ണസംഖ്യ		*outScore,
    int_u	*matches,
    പൂർണ്ണസംഖ്യ		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		complete = FALSE;
    പൂർണ്ണസംഖ്യ		score = 0;
    പൂർണ്ണസംഖ്യ		അക്കം_Matches = 0;
    പൂർണ്ണസംഖ്യ		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    എങ്കിൽ (save_pat == ശൂന്യ)
	തിരികെ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ആകുമ്പോൾ (TRUE)
    {
	എങ്കിൽ (matchseq)
	    complete = TRUE;
	അല്ലെങ്കിൽ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	    pat = p;
	    ആകുമ്പോൾ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		എങ്കിൽ (has_mbyte)
		    MB_PTR_ADV(p);
		അല്ലെങ്കിൽ
		    ++p;
	    }
	    എങ്കിൽ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	എങ്കിൽ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	എങ്കിൽ (അക്കം_Matches > maxMatches - pat_chars)
	{
	    അക്കം_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    നിർത്തുക;
	}

	score = FUZZY_SCORE_NONE;
	എങ്കിൽ (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + അക്കം_Matches);
	    എങ്കിൽ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (പൂർണ്ണസംഖ്യ)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (പൂർണ്ണസംഖ്യ)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	എങ്കിൽ (score == FUZZY_SCORE_NONE)
	{
	    അക്കം_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    നിർത്തുക;
	}

	എങ്കിൽ (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	അല്ലെങ്കിൽ എങ്കിൽ (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	അല്ലെങ്കിൽ
	    *outScore += score;

	അക്കം_Matches += pat_chars;

	എങ്കിൽ (complete || അക്കം_Matches >= maxMatches)
	    നിർത്തുക;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    തിരികെ അക്കം_Matches != 0;
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fuzzy_match_item_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    പൂർണ്ണസംഖ്യ		v1 = ((fuzzyItem_T *)s1)->score;
    പൂർണ്ണസംഖ്യ		v2 = ((fuzzyItem_T *)s2)->score;

    എങ്കിൽ (v1 == v2)
    {
	പൂർണ്ണസംഖ്യ exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	പൂർണ്ണസംഖ്യ patlen = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
	പൂർണ്ണസംഖ്യ startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	എങ്കിൽ (exact_match1 == exact_match2)
	{
	    പൂർണ്ണസംഖ്യ idx1 = ((fuzzyItem_T *)s1)->idx;
	    പൂർണ്ണസംഖ്യ idx2 = ((fuzzyItem_T *)s2)->idx;
	    തിരികെ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (exact_match2)
	    തിരികെ 1;
	തിരികെ -1;
    }
    അല്ലെങ്കിൽ
	തിരികെ v1 > v2 ? -1 : 1;
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
    സ്ഥിതം ശൂന്യം
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		matchseq,
    char_u	*താക്കോൽ,
    callback_T	*item_cb,
    പൂർണ്ണസംഖ്യ		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *ഇനങ്ങൾ = ശൂന്യ;
    listitem_T	    *li;
    long	    match_എണ്ണം = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    എങ്കിൽ (len == 0)
	തിരികെ;
    എങ്കിൽ (max_matches > 0 && len > max_matches)
	len = max_matches;

    ഇനങ്ങൾ = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    എങ്കിൽ (ഇനങ്ങൾ == ശൂന്യ)
	തിരികെ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	പൂർണ്ണസംഖ്യ		score;
	char_u		*itemstr = ശൂന്യ;
	char_u		*itemstr_copy = ശൂന്യ;
	typval_T	rettv;
	പൂർണ്ണസംഖ്യ		itemstr_allocate = FALSE;
	list_T		*match_positions = ശൂന്യ;

	എങ്കിൽ (max_matches > 0 && match_എണ്ണം >= max_matches)
	    നിർത്തുക;

	rettv.v_type = VAR_UNKNOWN;
	എങ്കിൽ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	അല്ലെങ്കിൽ എങ്കിൽ (li->li_tv.v_type == VAR_DICT
				&& (താക്കോൽ != ശൂന്യ || item_cb->cb_name != ശൂന്യ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    എങ്കിൽ (താക്കോൽ != ശൂന്യ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (അക്ഷരം *)താക്കോൽ, FALSE);
	    അല്ലെങ്കിൽ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		എങ്കിൽ (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    എങ്കിൽ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	എങ്കിൽ (itemstr != ശൂന്യ
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    എങ്കിൽ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		എങ്കിൽ (itemstr_copy == ശൂന്യ)
		{
		    clear_tv(&rettv);
		    തുടരുക;
		}
	    }
	    അല്ലെങ്കിൽ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    എങ്കിൽ (retmatchpos)
	    {
		match_positions = list_alloc();
		എങ്കിൽ (match_positions == ശൂന്യ)
		{
		    എങ്കിൽ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    തുടരുക;
		}

		// Fill position information
		പൂർണ്ണസംഖ്യ	j = 0;
		char_u	*p = str;
		പൂർണ്ണസംഖ്യ	success = TRUE;

		ആകുമ്പോൾ (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    എങ്കിൽ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			എങ്കിൽ (list_append_സംഖ്യ(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    നിർത്തുക;
			}
			j++;
		    }
		    എങ്കിൽ (has_mbyte)
			MB_PTR_ADV(p);
		    അല്ലെങ്കിൽ
			++p;
		}

		എങ്കിൽ (!success)
		{
		    list_free(match_positions);
		    എങ്കിൽ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    തുടരുക;
		}
	    }
	    ഇനങ്ങൾ[match_എണ്ണം].idx = match_എണ്ണം;
	    ഇനങ്ങൾ[match_എണ്ണം].item = li;
	    ഇനങ്ങൾ[match_എണ്ണം].score = score;
	    ഇനങ്ങൾ[match_എണ്ണം].pat = str;
	    ഇനങ്ങൾ[match_എണ്ണം].startpos = matches[0];
	    ഇനങ്ങൾ[match_എണ്ണം].itemstr = itemstr_copy;
	    ഇനങ്ങൾ[match_എണ്ണം].itemstr_allocated = itemstr_allocate;
	    ഇനങ്ങൾ[match_എണ്ണം].lmatchpos = match_positions;

	    ++match_എണ്ണം;
	}
	clear_tv(&rettv);
    }

    എങ്കിൽ (match_എണ്ണം > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((ശൂന്യം *)ഇനങ്ങൾ, (size_t)match_എണ്ണം, വലുപ്പം(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	എങ്കിൽ (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    എങ്കിൽ (li == ശൂന്യ || li->li_tv.vval.v_list == ശൂന്യ)
		പോകുക done;
	    retlist = li->li_tv.vval.v_list;
	}
	അല്ലെങ്കിൽ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < match_എണ്ണം; i++)
	{
	    എങ്കിൽ (list_append_tv(retlist, &ഇനങ്ങൾ[i].item->li_tv) == FAIL)
		പോകുക done;
	}

	// next copy the list of matching positions
	എങ്കിൽ (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    എങ്കിൽ (li == ശൂന്യ || li->li_tv.vval.v_list == ശൂന്യ)
		പോകുക done;
	    retlist = li->li_tv.vval.v_list;

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < match_എണ്ണം; i++)
	    {
		എങ്കിൽ (ഇനങ്ങൾ[i].lmatchpos != ശൂന്യ)
		{
		    എങ്കിൽ (list_append_list(retlist, ഇനങ്ങൾ[i].lmatchpos) == OK)
			ഇനങ്ങൾ[i].lmatchpos = ശൂന്യ;
		    അല്ലെങ്കിൽ
			പോകുക done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    എങ്കിൽ (li == ശൂന്യ || li->li_tv.vval.v_list == ശൂന്യ)
		പോകുക done;
	    retlist = li->li_tv.vval.v_list;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < match_എണ്ണം; i++)
	    {
		എങ്കിൽ (list_append_സംഖ്യ(retlist, ഇനങ്ങൾ[i].score) == FAIL)
		    പോകുക done;
	    }
	}
    }

done:
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < match_എണ്ണം; i++)
    {
	എങ്കിൽ (ഇനങ്ങൾ[i].itemstr_allocated)
	    vim_free(ഇനങ്ങൾ[i].itemstr);

	എങ്കിൽ (ഇനങ്ങൾ[i].lmatchpos)
	    list_free(ഇനങ്ങൾ[i].lmatchpos);
    }
    vim_free(ഇനങ്ങൾ);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    സ്ഥിതം ശൂന്യം
do_fuzzymatch(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ retmatchpos)
{
    callback_T	cb;
    char_u	*താക്കോൽ = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		matchseq = FALSE;
    long	max_matches = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    എങ്കിൽ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ശൂന്യ)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	തിരികെ;
    }
    എങ്കിൽ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	തിരികെ;
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    തിരികെ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	എങ്കിൽ ((di = dict_find(d, (char_u *)"key", -1)) != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ശൂന്യ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		തിരികെ;
	    }
	    താക്കോൽ = tv_get_string(&di->di_tv);
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((di = dict_find(d, (char_u *)"text_cb", -1)) != ശൂന്യ)
	{
	    cb = get_callback(&di->di_tv);
	    എങ്കിൽ (cb.cb_name == ശൂന്യ)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str), "text_cb");
		തിരികെ;
	    }
	}

	എങ്കിൽ ((di = dict_find(d, (char_u *)"limit", -1)) != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_സംഖ്യ)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str), "limit");
		തിരികെ;
	    }
	    max_matches = (long)tv_get_സംഖ്യ_chk(&di->di_tv, ശൂന്യ);
	}

	എങ്കിൽ (dict_has_താക്കോൽ(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    എങ്കിൽ (ret == FAIL)
	പോകുക done;
    എങ്കിൽ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    പോകുക done;
	എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    പോകുക done;
	}
	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    പോകുക done;
	എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    പോകുക done;
	}
	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    പോകുക done;
	എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    പോകുക done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, താക്കോൽ, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    ശൂന്യം
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    ശൂന്യം
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fuzzy_match_str_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    പൂർണ്ണസംഖ്യ		v1 = ((fuzmatch_str_T *)s1)->score;
    പൂർണ്ണസംഖ്യ		v2 = ((fuzmatch_str_T *)s2)->score;
    പൂർണ്ണസംഖ്യ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    പൂർണ്ണസംഖ്യ		idx2 = ((fuzmatch_str_T *)s2)->idx;

    എങ്കിൽ (v1 == v2)
	തിരികെ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    അല്ലെങ്കിൽ
	തിരികെ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    സ്ഥിതം ശൂന്യം
fuzzy_match_str_sort(fuzmatch_str_T *fm, പൂർണ്ണസംഖ്യ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ശൂന്യം *)fm, (size_t)sz, വലുപ്പം(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fuzzy_match_func_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    പൂർണ്ണസംഖ്യ		v1 = ((fuzmatch_str_T *)s1)->score;
    പൂർണ്ണസംഖ്യ		v2 = ((fuzmatch_str_T *)s2)->score;
    പൂർണ്ണസംഖ്യ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    പൂർണ്ണസംഖ്യ		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    എങ്കിൽ (*str1 != '<' && *str2 == '<')
	തിരികെ -1;
    എങ്കിൽ (*str1 == '<' && *str2 != '<')
	തിരികെ 1;
    എങ്കിൽ (v1 == v2)
	തിരികെ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    അല്ലെങ്കിൽ
	തിരികെ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    സ്ഥിതം ശൂന്യം
fuzzy_match_func_sort(fuzmatch_str_T *fm, പൂർണ്ണസംഖ്യ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ശൂന്യം *)fm, (size_t)sz, വലുപ്പം(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    പൂർണ്ണസംഖ്യ
fuzzy_match_str(char_u *str, char_u *pat)
{
    പൂർണ്ണസംഖ്യ		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    എങ്കിൽ (str == ശൂന്യ || pat == ശൂന്യ)
	തിരികെ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				വലുപ്പം(matchpos) / വലുപ്പം(matchpos[0]));

    തിരികെ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_തിരയൽ_EXTRA
    പൂർണ്ണസംഖ്യ		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = ശൂന്യ;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    പൂർണ്ണസംഖ്യ		    j = 0;

    എങ്കിൽ (str == ശൂന്യ || pat == ശൂന്യ)
	തിരികെ ശൂന്യ;

    match_positions = ALLOC_ONE(garray_T);
    എങ്കിൽ (match_positions == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ga_init2(match_positions, വലുപ്പം(int_u), 10);

    എങ്കിൽ (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	തിരികെ ശൂന്യ;
    }

    വേണ്ടി (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	എങ്കിൽ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    എങ്കിൽ (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		തിരികെ ശൂന്യ;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    തിരികെ match_positions;
#അല്ലെങ്കിൽ
    തിരികെ ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
fuzzy_match_str_in_വരി(
    char_u	**ptr,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		*len,
    pos_T	*current_pos,
    പൂർണ്ണസംഖ്യ		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = ശൂന്യ;
    char_u	*start = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		found = FALSE;
    അക്ഷരം	save_end;
    char_u	*വരി_end = ശൂന്യ;

    എങ്കിൽ (str == ശൂന്യ || pat == ശൂന്യ)
	തിരികെ found;
    വരി_end = find_വരി_end(str);

    ആകുമ്പോൾ (str < വരി_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	എങ്കിൽ (*start == NUL)
	    നിർത്തുക;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	എങ്കിൽ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (പൂർണ്ണസംഖ്യ)(end - start);
	    found = TRUE;
	    *ptr = start;
	    എങ്കിൽ (current_pos)
		current_pos->col += (പൂർണ്ണസംഖ്യ)(end - strBegin);
	    നിർത്തുക;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	ആകുമ്പോൾ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    എങ്കിൽ (!found)
	*ptr = വരി_end;

    തിരികെ found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    പൂർണ്ണസംഖ്യ
തിരയൽ_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    പൂർണ്ണസംഖ്യ		dir,
    pos_T	*start_pos,
    പൂർണ്ണസംഖ്യ		*len,
    char_u	**ptr,
    പൂർണ്ണസംഖ്യ		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    പൂർണ്ണസംഖ്യ		found_new_match = FALSE;
    പൂർണ്ണസംഖ്യ		looped_around = FALSE;
    പൂർണ്ണസംഖ്യ		whole_വരി = ctrl_x_mode_whole_വരി();

    എങ്കിൽ (buf == curbuf)
	circly_end = *start_pos;
    അല്ലെങ്കിൽ
    {
	circly_end.lnum = buf->b_ml.ml_വരി_എണ്ണം;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    എങ്കിൽ (whole_വരി && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    ചെയ്യുക
    {

	// Check if looped around and back to start position
	എങ്കിൽ (looped_around && (whole_വരി ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    നിർത്തുക;

	// Ensure current_pos is valid
	എങ്കിൽ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_വരി_എണ്ണം)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    എങ്കിൽ (!whole_വരി)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    എങ്കിൽ (*ptr != ശൂന്യ && **ptr != NUL)
	    {
		എങ്കിൽ (!whole_വരി)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_വരി(ptr, pattern,
						    len, &current_pos, score);
		    എങ്കിൽ (found_new_match)
		    {
			*pos = current_pos;
			നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (looped_around && current_pos.lnum == circly_end.lnum)
			നിർത്തുക;
		}
		അല്ലെങ്കിൽ
		{
		    char_u *വരി = *ptr;
		    char_u *p = skipwhite(വരി);
		    എങ്കിൽ (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (പൂർണ്ണസംഖ്യ)ml_get_buf_len(buf, current_pos.lnum) -
								(പൂർണ്ണസംഖ്യ)(p - വരി);
			നിർത്തുക;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	എങ്കിൽ (dir == FORWARD)
	{
	    എങ്കിൽ (++current_pos.lnum > buf->b_ml.ml_വരി_എണ്ണം)
	    {
		എങ്കിൽ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (--current_pos.lnum < 1)
	    {
		എങ്കിൽ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_വരി_എണ്ണം;
		    looped_around = TRUE;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;

	    }
	}
	current_pos.col = 0;
    } ആകുമ്പോൾ (TRUE);

    തിരികെ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    ശൂന്യം
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (fuzmatch == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    പൂർണ്ണസംഖ്യ
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	പൂർണ്ണസംഖ്യ		എണ്ണം,
	പൂർണ്ണസംഖ്യ		funcsort)
{
    എങ്കിൽ (എണ്ണം <= 0)
	പോകുക theend;

    *matches = ALLOC_MULT(char_u *, എണ്ണം);
    എങ്കിൽ (*matches == ശൂന്യ)
    {
	fuzmatch_str_free(fuzmatch, എണ്ണം);
	തിരികെ FAIL;
    }

    // Sort the list by the descending order of the match score
    എങ്കിൽ (funcsort)
	fuzzy_match_func_sort((ശൂന്യം *)fuzmatch, (size_t)എണ്ണം);
    അല്ലെങ്കിൽ
	fuzzy_match_str_sort((ശൂന്യം *)fuzmatch, (size_t)എണ്ണം);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    തിരികെ OK;
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_match(char_u *needle, char_u *haystack)
{
    എങ്കിൽ (!needle || !haystack || !*needle)
	തിരികെ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ആകുമ്പോൾ (*n_ptr)
    {
	പൂർണ്ണസംഖ്യ n_char = mb_ptr2char(n_ptr);
	പൂർണ്ണസംഖ്യ n_upper = MB_TOUPPER(n_char);
	പൂർണ്ണസംഖ്യ found = FALSE;

	ആകുമ്പോൾ (*h_ptr)
	{
	    പൂർണ്ണസംഖ്യ h_char = mb_ptr2char(h_ptr);
	    എങ്കിൽ (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		നിർത്തുക;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	എങ്കിൽ (!found)
	    തിരികെ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    തിരികെ OK;
}

തരനിർവചനം ഘടന match_struct
{
    പൂർണ്ണസംഖ്യ needle_len;
    പൂർണ്ണസംഖ്യ haystack_len;
    പൂർണ്ണസംഖ്യ lower_needle[MATCH_MAX_LEN];     // stores codepoints
    പൂർണ്ണസംഖ്യ lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    സ്ഥിതം score_t
compute_bonus_codepoint(പൂർണ്ണസംഖ്യ last_c, പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	എങ്കിൽ (IS_PATH_SEP(last_c))
	    തിരികെ SCORE_MATCH_SLASH;
	എങ്കിൽ (IS_WORD_SEP(last_c))
	    തിരികെ SCORE_MATCH_WORD;
	എങ്കിൽ (IS_DOT(last_c))
	    തിരികെ SCORE_MATCH_DOT;
	എങ്കിൽ (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    തിരികെ SCORE_MATCH_CAPITAL;
    }
    തിരികെ 0;
}

    സ്ഥിതം ശൂന്യം
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    പൂർണ്ണസംഖ്യ i = 0;
    char_u *p = needle;
    ആകുമ്പോൾ (*p != NUL && i < MATCH_MAX_LEN)
    {
	പൂർണ്ണസംഖ്യ c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    പൂർണ്ണസംഖ്യ prev_c = '/';
    ആകുമ്പോൾ (*p != NUL && i < MATCH_MAX_LEN)
    {
	പൂർണ്ണസംഖ്യ c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    സ്ഥിതം വരിയിൽ ശൂന്യം
match_row(സ്ഥിരം match_struct *match, പൂർണ്ണസംഖ്യ row, score_t *curr_D,
	score_t *curr_M, സ്ഥിരം score_t *last_D, സ്ഥിരം score_t *last_M)
{
    പൂർണ്ണസംഖ്യ n = match->needle_len;
    പൂർണ്ണസംഖ്യ m = match->haystack_len;
    പൂർണ്ണസംഖ്യ i = row;

    സ്ഥിരം പൂർണ്ണസംഖ്യ *lower_needle = match->lower_needle;
    സ്ഥിരം പൂർണ്ണസംഖ്യ *lower_haystack = match->lower_haystack;
    സ്ഥിരം score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < m; j++)
    {
	എങ്കിൽ (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    എങ്കിൽ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (j)
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
	അല്ലെങ്കിൽ
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    സ്ഥിതം score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    എങ്കിൽ (!needle || !haystack || !*needle)
	തിരികെ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    പൂർണ്ണസംഖ്യ n = match.needle_len;
    പൂർണ്ണസംഖ്യ m = match.haystack_len;

    എങ്കിൽ (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	തിരികെ SCORE_MIN;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n; i++)
	{
	    എങ്കിൽ (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (equal)
	{
	    എങ്കിൽ (positions)
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    തിരികെ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    എങ്കിൽ ((size_t)n > (SIZE_MAX / വലുപ്പം(score_t)) / MATCH_MAX_LEN / 2)
	തിരികെ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(വലുപ്പം(score_t) * MATCH_MAX_LEN * n * 2);
    എങ്കിൽ (!block)
	തിരികെ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    എങ്കിൽ (positions)
    {
	പൂർണ്ണസംഖ്യ match_required = 0;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = n - 1, j = m - 1; i >= 0; i--)
	{
	    വേണ്ടി (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		എങ്കിൽ (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    നിർത്തുക;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    തിരികെ result;
}
