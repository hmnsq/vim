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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL) || defined(FEAT_PROTO)
સ્થિર પૂર્ણાંક fuzzy_match_item_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);
સ્થિર શૂન્ય fuzzy_match_in_list(list_T *l, char_u *str, પૂર્ણાંક matchseq, char_u *ચાવી, callback_T *item_cb, પૂર્ણાંક retmatchpos, list_T *fmatchlist, long max_matches);
સ્થિર શૂન્ય do_fuzzymatch(typval_T *argvars, typval_T *rettv, પૂર્ણાંક retmatchpos);
#endif
સ્થિર પૂર્ણાંક fuzzy_match_str_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);
સ્થિર શૂન્ય fuzzy_match_str_sort(fuzmatch_str_T *fm, પૂર્ણાંક sz);
સ્થિર પૂર્ણાંક fuzzy_match_func_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);
સ્થિર શૂન્ય fuzzy_match_func_sort(fuzmatch_str_T *fm, પૂર્ણાંક sz);

પ્રકારવ્યાખ્યા દ્વિગુણ score_t;
સ્થિર score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
સ્થિર પૂર્ણાંક has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		idx;		// used for stable sort
    listitem_T	*item;
    પૂર્ણાંક		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    પૂર્ણાંક		itemstr_allocated;
    પૂર્ણાંક		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    પૂર્ણાંક
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    પૂર્ણાંક		matchseq,
    પૂર્ણાંક		*outScore,
    int_u	*matches,
    પૂર્ણાંક		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    પૂર્ણાંક		complete = FALSE;
    પૂર્ણાંક		score = 0;
    પૂર્ણાંક		અંક_Matches = 0;
    પૂર્ણાંક		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    જો (save_pat == ખાલી)
	પાછા FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    જ્યારે (TRUE)
    {
	જો (matchseq)
	    complete = TRUE;
	નહીંતર
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    જો (*p == NUL)
		તોડો;
	    pat = p;
	    જ્યારે (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		જો (has_mbyte)
		    MB_PTR_ADV(p);
		નહીંતર
		    ++p;
	    }
	    જો (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	જો (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	જો (અંક_Matches > maxMatches - pat_chars)
	{
	    અંક_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    તોડો;
	}

	score = FUZZY_SCORE_NONE;
	જો (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + અંક_Matches);
	    જો (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (પૂર્ણાંક)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (પૂર્ણાંક)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	જો (score == FUZZY_SCORE_NONE)
	{
	    અંક_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    તોડો;
	}

	જો (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	નહીંતર જો (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	નહીંતર
	    *outScore += score;

	અંક_Matches += pat_chars;

	જો (complete || અંક_Matches >= maxMatches)
	    તોડો;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    પાછા અંક_Matches != 0;
}

#જો defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    સ્થિર પૂર્ણાંક
fuzzy_match_item_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    પૂર્ણાંક		v1 = ((fuzzyItem_T *)s1)->score;
    પૂર્ણાંક		v2 = ((fuzzyItem_T *)s2)->score;

    જો (v1 == v2)
    {
	પૂર્ણાંક exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	પૂર્ણાંક patlen = (પૂર્ણાંક)STRLEN(pat);
	પૂર્ણાંક startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	જો (exact_match1 == exact_match2)
	{
	    પૂર્ણાંક idx1 = ((fuzzyItem_T *)s1)->idx;
	    પૂર્ણાંક idx2 = ((fuzzyItem_T *)s2)->idx;
	    પાછા idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	નહીંતર જો (exact_match2)
	    પાછા 1;
	પાછા -1;
    }
    નહીંતર
	પાછા v1 > v2 ? -1 : 1;
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
    સ્થિર શૂન્ય
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    પૂર્ણાંક		matchseq,
    char_u	*ચાવી,
    callback_T	*item_cb,
    પૂર્ણાંક		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *વસ્તુઓ = ખાલી;
    listitem_T	    *li;
    long	    match_ગણતરી = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    જો (len == 0)
	પાછા;
    જો (max_matches > 0 && len > max_matches)
	len = max_matches;

    વસ્તુઓ = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    જો (વસ્તુઓ == ખાલી)
	પાછા;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	પૂર્ણાંક		score;
	char_u		*itemstr = ખાલી;
	char_u		*itemstr_copy = ખાલી;
	typval_T	rettv;
	પૂર્ણાંક		itemstr_allocate = FALSE;
	list_T		*match_positions = ખાલી;

	જો (max_matches > 0 && match_ગણતરી >= max_matches)
	    તોડો;

	rettv.v_type = VAR_UNKNOWN;
	જો (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	નહીંતર જો (li->li_tv.v_type == VAR_DICT
				&& (ચાવી != ખાલી || item_cb->cb_name != ખાલી))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    જો (ચાવી != ખાલી)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (અક્ષર *)ચાવી, FALSE);
	    નહીંતર
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		જો (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    જો (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	જો (itemstr != ખાલી
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    જો (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		જો (itemstr_copy == ખાલી)
		{
		    clear_tv(&rettv);
		    ચાલુ;
		}
	    }
	    નહીંતર
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    જો (retmatchpos)
	    {
		match_positions = list_alloc();
		જો (match_positions == ખાલી)
		{
		    જો (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ચાલુ;
		}

		// Fill position information
		પૂર્ણાંક	j = 0;
		char_u	*p = str;
		પૂર્ણાંક	success = TRUE;

		જ્યારે (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    જો (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			જો (list_append_સંખ્યા(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    તોડો;
			}
			j++;
		    }
		    જો (has_mbyte)
			MB_PTR_ADV(p);
		    નહીંતર
			++p;
		}

		જો (!success)
		{
		    list_free(match_positions);
		    જો (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ચાલુ;
		}
	    }
	    વસ્તુઓ[match_ગણતરી].idx = match_ગણતરી;
	    વસ્તુઓ[match_ગણતરી].item = li;
	    વસ્તુઓ[match_ગણતરી].score = score;
	    વસ્તુઓ[match_ગણતરી].pat = str;
	    વસ્તુઓ[match_ગણતરી].startpos = matches[0];
	    વસ્તુઓ[match_ગણતરી].itemstr = itemstr_copy;
	    વસ્તુઓ[match_ગણતરી].itemstr_allocated = itemstr_allocate;
	    વસ્તુઓ[match_ગણતરી].lmatchpos = match_positions;

	    ++match_ગણતરી;
	}
	clear_tv(&rettv);
    }

    જો (match_ગણતરી > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((શૂન્ય *)વસ્તુઓ, (size_t)match_ગણતરી, કદ(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	જો (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    જો (li == ખાલી || li->li_tv.vval.v_list == ખાલી)
		જાઓ done;
	    retlist = li->li_tv.vval.v_list;
	}
	નહીંતર
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	માટે (પૂર્ણાંક i = 0; i < match_ગણતરી; i++)
	{
	    જો (list_append_tv(retlist, &વસ્તુઓ[i].item->li_tv) == FAIL)
		જાઓ done;
	}

	// next copy the list of matching positions
	જો (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    જો (li == ખાલી || li->li_tv.vval.v_list == ખાલી)
		જાઓ done;
	    retlist = li->li_tv.vval.v_list;

	    માટે (પૂર્ણાંક i = 0; i < match_ગણતરી; i++)
	    {
		જો (વસ્તુઓ[i].lmatchpos != ખાલી)
		{
		    જો (list_append_list(retlist, વસ્તુઓ[i].lmatchpos) == OK)
			વસ્તુઓ[i].lmatchpos = ખાલી;
		    નહીંતર
			જાઓ done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    જો (li == ખાલી || li->li_tv.vval.v_list == ખાલી)
		જાઓ done;
	    retlist = li->li_tv.vval.v_list;
	    માટે (પૂર્ણાંક i = 0; i < match_ગણતરી; i++)
	    {
		જો (list_append_સંખ્યા(retlist, વસ્તુઓ[i].score) == FAIL)
		    જાઓ done;
	    }
	}
    }

done:
    માટે (પૂર્ણાંક i = 0; i < match_ગણતરી; i++)
    {
	જો (વસ્તુઓ[i].itemstr_allocated)
	    vim_free(વસ્તુઓ[i].itemstr);

	જો (વસ્તુઓ[i].lmatchpos)
	    list_free(વસ્તુઓ[i].lmatchpos);
    }
    vim_free(વસ્તુઓ);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    સ્થિર શૂન્ય
do_fuzzymatch(typval_T *argvars, typval_T *rettv, પૂર્ણાંક retmatchpos)
{
    callback_T	cb;
    char_u	*ચાવી = ખાલી;
    પૂર્ણાંક		ret;
    પૂર્ણાંક		matchseq = FALSE;
    long	max_matches = 0;

    જો (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    જો (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ખાલી)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	પાછા;
    }
    જો (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ખાલી)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	પાછા;
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	જો (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    પાછા;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	જો ((di = dict_find(d, (char_u *)"key", -1)) != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ખાલી
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		પાછા;
	    }
	    ચાવી = tv_get_string(&di->di_tv);
	}
	નહીંતર જો ((di = dict_find(d, (char_u *)"text_cb", -1)) != ખાલી)
	{
	    cb = get_callback(&di->di_tv);
	    જો (cb.cb_name == ખાલી)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str), "text_cb");
		પાછા;
	    }
	}

	જો ((di = dict_find(d, (char_u *)"limit", -1)) != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_સંખ્યા)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str), "limit");
		પાછા;
	    }
	    max_matches = (long)tv_get_સંખ્યા_chk(&di->di_tv, ખાલી);
	}

	જો (dict_has_ચાવી(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    જો (ret == FAIL)
	જાઓ done;
    જો (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	જો (l == ખાલી)
	    જાઓ done;
	જો (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    જાઓ done;
	}
	l = list_alloc();
	જો (l == ખાલી)
	    જાઓ done;
	જો (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    જાઓ done;
	}
	l = list_alloc();
	જો (l == ખાલી)
	    જાઓ done;
	જો (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    જાઓ done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, ચાવી, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    શૂન્ય
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    શૂન્ય
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    સ્થિર પૂર્ણાંક
fuzzy_match_str_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    પૂર્ણાંક		v1 = ((fuzmatch_str_T *)s1)->score;
    પૂર્ણાંક		v2 = ((fuzmatch_str_T *)s2)->score;
    પૂર્ણાંક		idx1 = ((fuzmatch_str_T *)s1)->idx;
    પૂર્ણાંક		idx2 = ((fuzmatch_str_T *)s2)->idx;

    જો (v1 == v2)
	પાછા idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    નહીંતર
	પાછા v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    સ્થિર શૂન્ય
fuzzy_match_str_sort(fuzmatch_str_T *fm, પૂર્ણાંક sz)
{
    // Sort the list by the descending order of the match score
    qsort((શૂન્ય *)fm, (size_t)sz, કદ(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    સ્થિર પૂર્ણાંક
fuzzy_match_func_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    પૂર્ણાંક		v1 = ((fuzmatch_str_T *)s1)->score;
    પૂર્ણાંક		v2 = ((fuzmatch_str_T *)s2)->score;
    પૂર્ણાંક		idx1 = ((fuzmatch_str_T *)s1)->idx;
    પૂર્ણાંક		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    જો (*str1 != '<' && *str2 == '<')
	પાછા -1;
    જો (*str1 == '<' && *str2 != '<')
	પાછા 1;
    જો (v1 == v2)
	પાછા idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    નહીંતર
	પાછા v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    સ્થિર શૂન્ય
fuzzy_match_func_sort(fuzmatch_str_T *fm, પૂર્ણાંક sz)
{
    // Sort the list by the descending order of the match score
    qsort((શૂન્ય *)fm, (size_t)sz, કદ(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    પૂર્ણાંક
fuzzy_match_str(char_u *str, char_u *pat)
{
    પૂર્ણાંક		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    જો (str == ખાલી || pat == ખાલી)
	પાછા score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				કદ(matchpos) / કદ(matchpos[0]));

    પાછા score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = ખાલી;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    પૂર્ણાંક		    j = 0;

    જો (str == ખાલી || pat == ખાલી)
	પાછા ખાલી;

    match_positions = ALLOC_ONE(garray_T);
    જો (match_positions == ખાલી)
	પાછા ખાલી;
    ga_init2(match_positions, કદ(int_u), 10);

    જો (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	પાછા ખાલી;
    }

    માટે (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	જો (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    જો (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		પાછા ખાલી;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    પાછા match_positions;
#નહીંતર
    પાછા ખાલી;
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
    પૂર્ણાંક
fuzzy_match_str_in_લીટી(
    char_u	**ptr,
    char_u	*pat,
    પૂર્ણાંક		*len,
    pos_T	*current_pos,
    પૂર્ણાંક		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = ખાલી;
    char_u	*start = ખાલી;
    પૂર્ણાંક		found = FALSE;
    અક્ષર	save_end;
    char_u	*લીટી_end = ખાલી;

    જો (str == ખાલી || pat == ખાલી)
	પાછા found;
    લીટી_end = find_લીટી_end(str);

    જ્યારે (str < લીટી_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	જો (*start == NUL)
	    તોડો;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	જો (*score != FUZZY_SCORE_NONE)
	{
	    *len = (પૂર્ણાંક)(end - start);
	    found = TRUE;
	    *ptr = start;
	    જો (current_pos)
		current_pos->col += (પૂર્ણાંક)(end - strBegin);
	    તોડો;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	જ્યારે (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    જો (!found)
	*ptr = લીટી_end;

    પાછા found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    પૂર્ણાંક
શોધ_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    પૂર્ણાંક		dir,
    pos_T	*start_pos,
    પૂર્ણાંક		*len,
    char_u	**ptr,
    પૂર્ણાંક		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    પૂર્ણાંક		found_new_match = FALSE;
    પૂર્ણાંક		looped_around = FALSE;
    પૂર્ણાંક		whole_લીટી = ctrl_x_mode_whole_લીટી();

    જો (buf == curbuf)
	circly_end = *start_pos;
    નહીંતર
    {
	circly_end.lnum = buf->b_ml.ml_લીટી_ગણતરી;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    જો (whole_લીટી && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    કરો
    {

	// Check if looped around and back to start position
	જો (looped_around && (whole_લીટી ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    તોડો;

	// Ensure current_pos is valid
	જો (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_લીટી_ગણતરી)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    જો (!whole_લીટી)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    જો (*ptr != ખાલી && **ptr != NUL)
	    {
		જો (!whole_લીટી)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_લીટી(ptr, pattern,
						    len, &current_pos, score);
		    જો (found_new_match)
		    {
			*pos = current_pos;
			તોડો;
		    }
		    નહીંતર જો (looped_around && current_pos.lnum == circly_end.lnum)
			તોડો;
		}
		નહીંતર
		{
		    char_u *લીટી = *ptr;
		    char_u *p = skipwhite(લીટી);
		    જો (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (પૂર્ણાંક)ml_get_buf_len(buf, current_pos.lnum) -
								(પૂર્ણાંક)(p - લીટી);
			તોડો;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	જો (dir == FORWARD)
	{
	    જો (++current_pos.lnum > buf->b_ml.ml_લીટી_ગણતરી)
	    {
		જો (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		નહીંતર
		    તોડો;
	    }
	}
	નહીંતર
	{
	    જો (--current_pos.lnum < 1)
	    {
		જો (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_લીટી_ગણતરી;
		    looped_around = TRUE;
		}
		નહીંતર
		    તોડો;

	    }
	}
	current_pos.col = 0;
    } જ્યારે (TRUE);

    પાછા found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    શૂન્ય
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, પૂર્ણાંક ગણતરી)
{
    જો (fuzmatch == ખાલી)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    પૂર્ણાંક
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	પૂર્ણાંક		ગણતરી,
	પૂર્ણાંક		funcsort)
{
    જો (ગણતરી <= 0)
	જાઓ theend;

    *matches = ALLOC_MULT(char_u *, ગણતરી);
    જો (*matches == ખાલી)
    {
	fuzmatch_str_free(fuzmatch, ગણતરી);
	પાછા FAIL;
    }

    // Sort the list by the descending order of the match score
    જો (funcsort)
	fuzzy_match_func_sort((શૂન્ય *)fuzmatch, (size_t)ગણતરી);
    નહીંતર
	fuzzy_match_str_sort((શૂન્ય *)fuzmatch, (size_t)ગણતરી);

    માટે (પૂર્ણાંક i = 0; i < ગણતરી; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    પાછા OK;
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

    સ્થિર પૂર્ણાંક
has_match(char_u *needle, char_u *haystack)
{
    જો (!needle || !haystack || !*needle)
	પાછા FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    જ્યારે (*n_ptr)
    {
	પૂર્ણાંક n_char = mb_ptr2char(n_ptr);
	પૂર્ણાંક n_upper = MB_TOUPPER(n_char);
	પૂર્ણાંક found = FALSE;

	જ્યારે (*h_ptr)
	{
	    પૂર્ણાંક h_char = mb_ptr2char(h_ptr);
	    જો (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		તોડો;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	જો (!found)
	    પાછા FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    પાછા OK;
}

પ્રકારવ્યાખ્યા સંરચના match_struct
{
    પૂર્ણાંક needle_len;
    પૂર્ણાંક haystack_len;
    પૂર્ણાંક lower_needle[MATCH_MAX_LEN];     // stores codepoints
    પૂર્ણાંક lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    સ્થિર score_t
compute_bonus_codepoint(પૂર્ણાંક last_c, પૂર્ણાંક c)
{
    જો (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	જો (IS_PATH_SEP(last_c))
	    પાછા SCORE_MATCH_SLASH;
	જો (IS_WORD_SEP(last_c))
	    પાછા SCORE_MATCH_WORD;
	જો (IS_DOT(last_c))
	    પાછા SCORE_MATCH_DOT;
	જો (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    પાછા SCORE_MATCH_CAPITAL;
    }
    પાછા 0;
}

    સ્થિર શૂન્ય
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    પૂર્ણાંક i = 0;
    char_u *p = needle;
    જ્યારે (*p != NUL && i < MATCH_MAX_LEN)
    {
	પૂર્ણાંક c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    પૂર્ણાંક prev_c = '/';
    જ્યારે (*p != NUL && i < MATCH_MAX_LEN)
    {
	પૂર્ણાંક c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    સ્થિર અંતઃરેખ શૂન્ય
match_row(અચલ match_struct *match, પૂર્ણાંક row, score_t *curr_D,
	score_t *curr_M, અચલ score_t *last_D, અચલ score_t *last_M)
{
    પૂર્ણાંક n = match->needle_len;
    પૂર્ણાંક m = match->haystack_len;
    પૂર્ણાંક i = row;

    અચલ પૂર્ણાંક *lower_needle = match->lower_needle;
    અચલ પૂર્ણાંક *lower_haystack = match->lower_haystack;
    અચલ score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    માટે (પૂર્ણાંક j = 0; j < m; j++)
    {
	જો (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    જો (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    નહીંતર જો (j)
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
	નહીંતર
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    સ્થિર score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    જો (!needle || !haystack || !*needle)
	પાછા SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    પૂર્ણાંક n = match.needle_len;
    પૂર્ણાંક m = match.haystack_len;

    જો (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	પાછા SCORE_MIN;
    }
    નહીંતર જો (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	માટે (પૂર્ણાંક i = 0; i < n; i++)
	{
	    જો (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		તોડો;
	    }
	}
	જો (equal)
	{
	    જો (positions)
	    {
		માટે (પૂર્ણાંક i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    પાછા SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    જો ((size_t)n > (SIZE_MAX / કદ(score_t)) / MATCH_MAX_LEN / 2)
	પાછા SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(કદ(score_t) * MATCH_MAX_LEN * n * 2);
    જો (!block)
	પાછા SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    માટે (પૂર્ણાંક i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    જો (positions)
    {
	પૂર્ણાંક match_required = 0;
	માટે (પૂર્ણાંક i = n - 1, j = m - 1; i >= 0; i--)
	{
	    માટે (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		જો (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    તોડો;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    પાછા result;
}
