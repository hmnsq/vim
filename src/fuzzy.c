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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PROTO)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fuzzy_match_item_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_match_in_list(list_T *l, char_u *str, ಪೂರ್ಣಾಙ್ಕ matchseq, char_u *ಕೀಲಿ, callback_T *item_cb, ಪೂರ್ಣಾಙ್ಕ retmatchpos, list_T *fmatchlist, long max_matches);
ಸ್ಥಿರ ಶೂನ್ಯ do_fuzzymatch(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ retmatchpos);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fuzzy_match_str_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_match_str_sort(fuzmatch_str_T *fm, ಪೂರ್ಣಾಙ್ಕ sz);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fuzzy_match_func_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_match_func_sort(fuzmatch_str_T *fm, ಪೂರ್ಣಾಙ್ಕ sz);

ಪ್ರಕಾರನಿರ್ದೇಶ ದ್ವಿಗುಣ score_t;
ಸ್ಥಿರ score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		idx;		// used for stable sort
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    ಪೂರ್ಣಾಙ್ಕ		itemstr_allocated;
    ಪೂರ್ಣಾಙ್ಕ		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    ಪೂರ್ಣಾಙ್ಕ
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    ಪೂರ್ಣಾಙ್ಕ		matchseq,
    ಪೂರ್ಣಾಙ್ಕ		*outScore,
    int_u	*matches,
    ಪೂರ್ಣಾಙ್ಕ		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		complete = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Matches = 0;
    ಪೂರ್ಣಾಙ್ಕ		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    ಯದಿ (save_pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ಯಾವತ್ (TRUE)
    {
	ಯದಿ (matchseq)
	    complete = TRUE;
	ಅನ್ಯಥಾ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    pat = p;
	    ಯಾವತ್ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ಯದಿ (has_mbyte)
		    MB_PTR_ADV(p);
		ಅನ್ಯಥಾ
		    ++p;
	    }
	    ಯದಿ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ಯದಿ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ಯದಿ (ಅಂಕೆ_Matches > maxMatches - pat_chars)
	{
	    ಅಂಕೆ_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    ಭಙ್ಗ;
	}

	score = FUZZY_SCORE_NONE;
	ಯದಿ (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + ಅಂಕೆ_Matches);
	    ಯದಿ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (ಪೂರ್ಣಾಙ್ಕ)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (ಪೂರ್ಣಾಙ್ಕ)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ಯದಿ (score == FUZZY_SCORE_NONE)
	{
	    ಅಂಕೆ_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    ಭಙ್ಗ;
	}

	ಯದಿ (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	ಅನ್ಯಥಾ ಯದಿ (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	ಅನ್ಯಥಾ
	    *outScore += score;

	ಅಂಕೆ_Matches += pat_chars;

	ಯದಿ (complete || ಅಂಕೆ_Matches >= maxMatches)
	    ಭಙ್ಗ;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_Matches != 0;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fuzzy_match_item_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ		v1 = ((fuzzyItem_T *)s1)->score;
    ಪೂರ್ಣಾಙ್ಕ		v2 = ((fuzzyItem_T *)s2)->score;

    ಯದಿ (v1 == v2)
    {
	ಪೂರ್ಣಾಙ್ಕ exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	ಪೂರ್ಣಾಙ್ಕ patlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
	ಪೂರ್ಣಾಙ್ಕ startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	ಯದಿ (exact_match1 == exact_match2)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx1 = ((fuzzyItem_T *)s1)->idx;
	    ಪೂರ್ಣಾಙ್ಕ idx2 = ((fuzzyItem_T *)s2)->idx;
	    ಪ್ರತ್ಯಾಗಮ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (exact_match2)
	    ಪ್ರತ್ಯಾಗಮ 1;
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ v1 > v2 ? -1 : 1;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		matchseq,
    char_u	*ಕೀಲಿ,
    callback_T	*item_cb,
    ಪೂರ್ಣಾಙ್ಕ		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *ವಸ್ತುಗಳು = ರಿಕ್ತ;
    listitem_T	    *li;
    long	    match_ಎಣಿಕೆ = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (max_matches > 0 && len > max_matches)
	len = max_matches;

    ವಸ್ತುಗಳು = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    ಯದಿ (ವಸ್ತುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಪೂರ್ಣಾಙ್ಕ		score;
	char_u		*itemstr = ರಿಕ್ತ;
	char_u		*itemstr_copy = ರಿಕ್ತ;
	typval_T	rettv;
	ಪೂರ್ಣಾಙ್ಕ		itemstr_allocate = FALSE;
	list_T		*match_positions = ರಿಕ್ತ;

	ಯದಿ (max_matches > 0 && match_ಎಣಿಕೆ >= max_matches)
	    ಭಙ್ಗ;

	rettv.v_type = VAR_UNKNOWN;
	ಯದಿ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ಅನ್ಯಥಾ ಯದಿ (li->li_tv.v_type == VAR_DICT
				&& (ಕೀಲಿ != ರಿಕ್ತ || item_cb->cb_name != ರಿಕ್ತ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ಯದಿ (ಕೀಲಿ != ರಿಕ್ತ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (ವರ್ಣ *)ಕೀಲಿ, FALSE);
	    ಅನ್ಯಥಾ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ಯದಿ (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ಯದಿ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ಯದಿ (itemstr != ರಿಕ್ತ
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    ಯದಿ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ಯದಿ (itemstr_copy == ರಿಕ್ತ)
		{
		    clear_tv(&rettv);
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ಯದಿ (retmatchpos)
	    {
		match_positions = list_alloc();
		ಯದಿ (match_positions == ರಿಕ್ತ)
		{
		    ಯದಿ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ಅನುವರ್ತ;
		}

		// Fill position information
		ಪೂರ್ಣಾಙ್ಕ	j = 0;
		char_u	*p = str;
		ಪೂರ್ಣಾಙ್ಕ	success = TRUE;

		ಯಾವತ್ (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    ಯದಿ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ಯದಿ (list_append_ಸಂಖ್ಯೆ(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    ಭಙ್ಗ;
			}
			j++;
		    }
		    ಯದಿ (has_mbyte)
			MB_PTR_ADV(p);
		    ಅನ್ಯಥಾ
			++p;
		}

		ಯದಿ (!success)
		{
		    list_free(match_positions);
		    ಯದಿ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ಅನುವರ್ತ;
		}
	    }
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].idx = match_ಎಣಿಕೆ;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].item = li;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].score = score;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].pat = str;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].startpos = matches[0];
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].itemstr = itemstr_copy;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].itemstr_allocated = itemstr_allocate;
	    ವಸ್ತುಗಳು[match_ಎಣಿಕೆ].lmatchpos = match_positions;

	    ++match_ಎಣಿಕೆ;
	}
	clear_tv(&rettv);
    }

    ಯದಿ (match_ಎಣಿಕೆ > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((ಶೂನ್ಯ *)ವಸ್ತುಗಳು, (size_t)match_ಎಣಿಕೆ, ಪರಿಮಾಣ(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ಯದಿ (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    ಯದಿ (li == ರಿಕ್ತ || li->li_tv.vval.v_list == ರಿಕ್ತ)
		ಗಚ್ಛ done;
	    retlist = li->li_tv.vval.v_list;
	}
	ಅನ್ಯಥಾ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < match_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (list_append_tv(retlist, &ವಸ್ತುಗಳು[i].item->li_tv) == FAIL)
		ಗಚ್ಛ done;
	}

	// next copy the list of matching positions
	ಯದಿ (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    ಯದಿ (li == ರಿಕ್ತ || li->li_tv.vval.v_list == ರಿಕ್ತ)
		ಗಚ್ಛ done;
	    retlist = li->li_tv.vval.v_list;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < match_ಎಣಿಕೆ; i++)
	    {
		ಯದಿ (ವಸ್ತುಗಳು[i].lmatchpos != ರಿಕ್ತ)
		{
		    ಯದಿ (list_append_list(retlist, ವಸ್ತುಗಳು[i].lmatchpos) == OK)
			ವಸ್ತುಗಳು[i].lmatchpos = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
			ಗಚ್ಛ done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    ಯದಿ (li == ರಿಕ್ತ || li->li_tv.vval.v_list == ರಿಕ್ತ)
		ಗಚ್ಛ done;
	    retlist = li->li_tv.vval.v_list;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < match_ಎಣಿಕೆ; i++)
	    {
		ಯದಿ (list_append_ಸಂಖ್ಯೆ(retlist, ವಸ್ತುಗಳು[i].score) == FAIL)
		    ಗಚ್ಛ done;
	    }
	}
    }

done:
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < match_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (ವಸ್ತುಗಳು[i].itemstr_allocated)
	    vim_free(ವಸ್ತುಗಳು[i].itemstr);

	ಯದಿ (ವಸ್ತುಗಳು[i].lmatchpos)
	    list_free(ವಸ್ತುಗಳು[i].lmatchpos);
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ retmatchpos)
{
    callback_T	cb;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		matchseq = FALSE;
    long	max_matches = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    ಯದಿ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ರಿಕ್ತ)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ಯದಿ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ಯದಿ ((di = dict_find(d, (char_u *)"key", -1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ರಿಕ್ತ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಕೀಲಿ = tv_get_string(&di->di_tv);
	}
	ಅನ್ಯಥಾ ಯದಿ ((di = dict_find(d, (char_u *)"text_cb", -1)) != ರಿಕ್ತ)
	{
	    cb = get_callback(&di->di_tv);
	    ಯದಿ (cb.cb_name == ರಿಕ್ತ)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "text_cb");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	ಯದಿ ((di = dict_find(d, (char_u *)"limit", -1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "limit");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    max_matches = (long)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, ರಿಕ್ತ);
	}

	ಯದಿ (dict_has_ಕೀಲಿ(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ಯದಿ (ret == FAIL)
	ಗಚ್ಛ done;
    ಯದಿ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ಯದಿ (l == ರಿಕ್ತ)
	    ಗಚ್ಛ done;
	ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಗಚ್ಛ done;
	}
	l = list_alloc();
	ಯದಿ (l == ರಿಕ್ತ)
	    ಗಚ್ಛ done;
	ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಗಚ್ಛ done;
	}
	l = list_alloc();
	ಯದಿ (l == ರಿಕ್ತ)
	    ಗಚ್ಛ done;
	ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಗಚ್ಛ done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, ಕೀಲಿ, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    ಶೂನ್ಯ
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    ಶೂನ್ಯ
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fuzzy_match_str_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ		v1 = ((fuzmatch_str_T *)s1)->score;
    ಪೂರ್ಣಾಙ್ಕ		v2 = ((fuzmatch_str_T *)s2)->score;
    ಪೂರ್ಣಾಙ್ಕ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ಪೂರ್ಣಾಙ್ಕ		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ಯದಿ (v1 == v2)
	ಪ್ರತ್ಯಾಗಮ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_match_str_sort(fuzmatch_str_T *fm, ಪೂರ್ಣಾಙ್ಕ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ಶೂನ್ಯ *)fm, (size_t)sz, ಪರಿಮಾಣ(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fuzzy_match_func_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ		v1 = ((fuzmatch_str_T *)s1)->score;
    ಪೂರ್ಣಾಙ್ಕ		v2 = ((fuzmatch_str_T *)s2)->score;
    ಪೂರ್ಣಾಙ್ಕ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ಪೂರ್ಣಾಙ್ಕ		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ಯದಿ (*str1 != '<' && *str2 == '<')
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (*str1 == '<' && *str2 != '<')
	ಪ್ರತ್ಯಾಗಮ 1;
    ಯದಿ (v1 == v2)
	ಪ್ರತ್ಯಾಗಮ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_match_func_sort(fuzmatch_str_T *fm, ಪೂರ್ಣಾಙ್ಕ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ಶೂನ್ಯ *)fm, (size_t)sz, ಪರಿಮಾಣ(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    ಪೂರ್ಣಾಙ್ಕ
fuzzy_match_str(char_u *str, char_u *pat)
{
    ಪೂರ್ಣಾಙ್ಕ		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    ಯದಿ (str == ರಿಕ್ತ || pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				ಪರಿಮಾಣ(matchpos) / ಪರಿಮಾಣ(matchpos[0]));

    ಪ್ರತ್ಯಾಗಮ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = ರಿಕ್ತ;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    ಪೂರ್ಣಾಙ್ಕ		    j = 0;

    ಯದಿ (str == ರಿಕ್ತ || pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    match_positions = ALLOC_ONE(garray_T);
    ಯದಿ (match_positions == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_init2(match_positions, ಪರಿಮಾಣ(int_u), 10);

    ಯದಿ (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ಯದಿ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ಯದಿ (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    ಪ್ರತ್ಯಾಗಮ match_positions;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ
fuzzy_match_str_in_ಸಾಲು(
    char_u	**ptr,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		*len,
    pos_T	*current_pos,
    ಪೂರ್ಣಾಙ್ಕ		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = ರಿಕ್ತ;
    char_u	*start = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;
    ವರ್ಣ	save_end;
    char_u	*ಸಾಲು_end = ರಿಕ್ತ;

    ಯದಿ (str == ರಿಕ್ತ || pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ found;
    ಸಾಲು_end = find_ಸಾಲು_end(str);

    ಯಾವತ್ (str < ಸಾಲು_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	ಯದಿ (*start == NUL)
	    ಭಙ್ಗ;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	ಯದಿ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (ಪೂರ್ಣಾಙ್ಕ)(end - start);
	    found = TRUE;
	    *ptr = start;
	    ಯದಿ (current_pos)
		current_pos->col += (ಪೂರ್ಣಾಙ್ಕ)(end - strBegin);
	    ಭಙ್ಗ;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	ಯಾವತ್ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ಯದಿ (!found)
	*ptr = ಸಾಲು_end;

    ಪ್ರತ್ಯಾಗಮ found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಹುಡುಕು_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    pos_T	*start_pos,
    ಪೂರ್ಣಾಙ್ಕ		*len,
    char_u	**ptr,
    ಪೂರ್ಣಾಙ್ಕ		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    ಪೂರ್ಣಾಙ್ಕ		found_new_match = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		looped_around = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		whole_ಸಾಲು = ctrl_x_mode_whole_ಸಾಲು();

    ಯದಿ (buf == curbuf)
	circly_end = *start_pos;
    ಅನ್ಯಥಾ
    {
	circly_end.lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    ಯದಿ (whole_ಸಾಲು && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    ಕುರು
    {

	// Check if looped around and back to start position
	ಯದಿ (looped_around && (whole_ಸಾಲು ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    ಭಙ್ಗ;

	// Ensure current_pos is valid
	ಯದಿ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ಯದಿ (!whole_ಸಾಲು)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ಯದಿ (*ptr != ರಿಕ್ತ && **ptr != NUL)
	    {
		ಯದಿ (!whole_ಸಾಲು)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_ಸಾಲು(ptr, pattern,
						    len, &current_pos, score);
		    ಯದಿ (found_new_match)
		    {
			*pos = current_pos;
			ಭಙ್ಗ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (looped_around && current_pos.lnum == circly_end.lnum)
			ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		{
		    char_u *ಸಾಲು = *ptr;
		    char_u *p = skipwhite(ಸಾಲು);
		    ಯದಿ (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (ಪೂರ್ಣಾಙ್ಕ)ml_get_buf_len(buf, current_pos.lnum) -
								(ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
			ಭಙ್ಗ;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ಯದಿ (dir == FORWARD)
	{
	    ಯದಿ (++current_pos.lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಯದಿ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (--current_pos.lnum < 1)
	    {
		ಯದಿ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    looped_around = TRUE;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;

	    }
	}
	current_pos.col = 0;
    } ಯಾವತ್ (TRUE);

    ಪ್ರತ್ಯಾಗಮ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    ಶೂನ್ಯ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಯದಿ (fuzmatch == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
	ಪೂರ್ಣಾಙ್ಕ		funcsort)
{
    ಯದಿ (ಎಣಿಕೆ <= 0)
	ಗಚ್ಛ theend;

    *matches = ALLOC_MULT(char_u *, ಎಣಿಕೆ);
    ಯದಿ (*matches == ರಿಕ್ತ)
    {
	fuzmatch_str_free(fuzmatch, ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Sort the list by the descending order of the match score
    ಯದಿ (funcsort)
	fuzzy_match_func_sort((ಶೂನ್ಯ *)fuzmatch, (size_t)ಎಣಿಕೆ);
    ಅನ್ಯಥಾ
	fuzzy_match_str_sort((ಶೂನ್ಯ *)fuzmatch, (size_t)ಎಣಿಕೆ);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ಪ್ರತ್ಯಾಗಮ OK;
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_match(char_u *needle, char_u *haystack)
{
    ಯದಿ (!needle || !haystack || !*needle)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ಯಾವತ್ (*n_ptr)
    {
	ಪೂರ್ಣಾಙ್ಕ n_char = mb_ptr2char(n_ptr);
	ಪೂರ್ಣಾಙ್ಕ n_upper = MB_TOUPPER(n_char);
	ಪೂರ್ಣಾಙ್ಕ found = FALSE;

	ಯಾವತ್ (*h_ptr)
	{
	    ಪೂರ್ಣಾಙ್ಕ h_char = mb_ptr2char(h_ptr);
	    ಯದಿ (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		ಭಙ್ಗ;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ಯದಿ (!found)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ match_struct
{
    ಪೂರ್ಣಾಙ್ಕ needle_len;
    ಪೂರ್ಣಾಙ್ಕ haystack_len;
    ಪೂರ್ಣಾಙ್ಕ lower_needle[MATCH_MAX_LEN];     // stores codepoints
    ಪೂರ್ಣಾಙ್ಕ lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    ಸ್ಥಿರ score_t
compute_bonus_codepoint(ಪೂರ್ಣಾಙ್ಕ last_c, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ಯದಿ (IS_PATH_SEP(last_c))
	    ಪ್ರತ್ಯಾಗಮ SCORE_MATCH_SLASH;
	ಯದಿ (IS_WORD_SEP(last_c))
	    ಪ್ರತ್ಯಾಗಮ SCORE_MATCH_WORD;
	ಯದಿ (IS_DOT(last_c))
	    ಪ್ರತ್ಯಾಗಮ SCORE_MATCH_DOT;
	ಯದಿ (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    ಪ್ರತ್ಯಾಗಮ SCORE_MATCH_CAPITAL;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    ಪೂರ್ಣಾಙ್ಕ i = 0;
    char_u *p = needle;
    ಯಾವತ್ (*p != NUL && i < MATCH_MAX_LEN)
    {
	ಪೂರ್ಣಾಙ್ಕ c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    ಪೂರ್ಣಾಙ್ಕ prev_c = '/';
    ಯಾವತ್ (*p != NUL && i < MATCH_MAX_LEN)
    {
	ಪೂರ್ಣಾಙ್ಕ c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
match_row(ಧ್ರುವ match_struct *match, ಪೂರ್ಣಾಙ್ಕ row, score_t *curr_D,
	score_t *curr_M, ಧ್ರುವ score_t *last_D, ಧ್ರುವ score_t *last_M)
{
    ಪೂರ್ಣಾಙ್ಕ n = match->needle_len;
    ಪೂರ್ಣಾಙ್ಕ m = match->haystack_len;
    ಪೂರ್ಣಾಙ್ಕ i = row;

    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *lower_needle = match->lower_needle;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *lower_haystack = match->lower_haystack;
    ಧ್ರುವ score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < m; j++)
    {
	ಯದಿ (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ಯದಿ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    ಅನ್ಯಥಾ ಯದಿ (j)
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
	ಅನ್ಯಥಾ
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ಸ್ಥಿರ score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ಯದಿ (!needle || !haystack || !*needle)
	ಪ್ರತ್ಯಾಗಮ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    ಪೂರ್ಣಾಙ್ಕ n = match.needle_len;
    ಪೂರ್ಣಾಙ್ಕ m = match.haystack_len;

    ಯದಿ (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ಪ್ರತ್ಯಾಗಮ SCORE_MIN;
    }
    ಅನ್ಯಥಾ ಯದಿ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n; i++)
	{
	    ಯದಿ (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (equal)
	{
	    ಯದಿ (positions)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ಪ್ರತ್ಯಾಗಮ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ಯದಿ ((size_t)n > (SIZE_MAX / ಪರಿಮಾಣ(score_t)) / MATCH_MAX_LEN / 2)
	ಪ್ರತ್ಯಾಗಮ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(ಪರಿಮಾಣ(score_t) * MATCH_MAX_LEN * n * 2);
    ಯದಿ (!block)
	ಪ್ರತ್ಯಾಗಮ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ಯದಿ (positions)
    {
	ಪೂರ್ಣಾಙ್ಕ match_required = 0;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = n - 1, j = m - 1; i >= 0; i--)
	{
	    ಪ್ರತಿ (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ಯದಿ (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    ಭಙ್ಗ;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    ಪ್ರತ್ಯಾಗಮ result;
}
