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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PROTO)
ਸਥਿਰ ਪੂਰਨ fuzzy_ਮੇਲ_ਵਸਤੂ_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);
ਸਥਿਰ ਖਾਲੀ fuzzy_ਮੇਲ_ਵਿੱਚ_list(list_T *l, char_u *str, ਪੂਰਨ matchseq, char_u *ਕੁੰਜੀ, callback_T *ਵਸਤੂ_cb, ਪੂਰਨ retmatchpos, list_T *fmatchlist, ਲੰਮਾ max_matches);
ਸਥਿਰ ਖਾਲੀ do_fuzzymatch(typval_T *argvars, typval_T *rettv, ਪੂਰਨ retmatchpos);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ fuzzy_ਮੇਲ_str_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);
ਸਥਿਰ ਖਾਲੀ fuzzy_ਮੇਲ_str_ਛਾਂਟੋ(fuzmatch_str_T *fm, ਪੂਰਨ sz);
ਸਥਿਰ ਪੂਰਨ fuzzy_ਮੇਲ_func_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);
ਸਥਿਰ ਖਾਲੀ fuzzy_ਮੇਲ_func_ਛਾਂਟੋ(fuzmatch_str_T *fm, ਪੂਰਨ sz);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਦੁੱਗਣਾ score_t;
ਸਥਿਰ score_t ਮੇਲ_positions(char_u *needle, char_u *haystack, int_u *positions);
ਸਥਿਰ ਪੂਰਨ has_ਮੇਲ(char_u *needle, char_u *haystack);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_MAX INFINITY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_MIN (-INFINITY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SCALE 1000

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		idx;		// used for stable sort
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    ਪੂਰਨ		itemstr_allocated;
    ਪੂਰਨ		startpos;
} fuzzy_ਵਸਤੂ_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    ਪੂਰਨ
fuzzy_ਮੇਲ(
    char_u	*str,
    char_u	*pat_arg,
    ਪੂਰਨ		matchseq,
    ਪੂਰਨ		*ਬਾਹਰਲਾ_Score,
    int_u	*matches,
    ਪੂਰਨ		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    ਪੂਰਨ		complete = FALSE;
    ਪੂਰਨ		score = 0;
    ਪੂਰਨ		ਅੰਕ_Matches = 0;
    ਪੂਰਨ		pat_chars = 0;
    score_t	fzy_score;

    *ਬਾਹਰਲਾ_Score = 0;

    save_pat = vim_strsave(pat_arg);
    ਜੇ (save_pat == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ਜਦੋਂ (TRUE)
    {
	ਜੇ (matchseq)
	    complete = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    pat = p;
	    ਜਦੋਂ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ਜੇ (has_mbyte)
		    MB_PTR_ADV(p);
		ਨਹੀਂਤਾਂ
		    ++p;
	    }
	    ਜੇ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ਜੇ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ਜੇ (ਅੰਕ_Matches > maxMatches - pat_chars)
	{
	    ਅੰਕ_Matches = 0;
	    *ਬਾਹਰਲਾ_Score = FUZZY_SCORE_NONE;
	    ਰੋਕੋ;
	}

	score = FUZZY_SCORE_NONE;
	ਜੇ (has_ਮੇਲ(pat, str))
	{
	    fzy_score = ਮੇਲ_positions(pat, str, matches + ਅੰਕ_Matches);
	    ਜੇ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
		    : (fzy_score < 0) ? (ਪੂਰਨ)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (ਪੂਰਨ)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ਜੇ (score == FUZZY_SCORE_NONE)
	{
	    ਅੰਕ_Matches = 0;
	    *ਬਾਹਰਲਾ_Score = FUZZY_SCORE_NONE;
	    ਰੋਕੋ;
	}

	ਜੇ (score > 0 && *ਬਾਹਰਲਾ_Score > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - score)
	    *ਬਾਹਰਲਾ_Score = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
	ਨਹੀਂਤਾਂ ਜੇ (score < 0 && *ਬਾਹਰਲਾ_Score < INT_MIN + 1 - score)
	    *ਬਾਹਰਲਾ_Score = INT_MIN + 1;
	ਨਹੀਂਤਾਂ
	    *ਬਾਹਰਲਾ_Score += score;

	ਅੰਕ_Matches += pat_chars;

	ਜੇ (complete || ਅੰਕ_Matches >= maxMatches)
	    ਰੋਕੋ;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ਵਾਪਸ ਅੰਕ_Matches != 0;
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ਸਥਿਰ ਪੂਰਨ
fuzzy_ਮੇਲ_ਵਸਤੂ_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਪੂਰਨ		v1 = ((fuzzy_ਵਸਤੂ_T *)s1)->score;
    ਪੂਰਨ		v2 = ((fuzzy_ਵਸਤੂ_T *)s2)->score;

    ਜੇ (v1 == v2)
    {
	ਪੂਰਨ exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_ਵਸਤੂ_T *)s1)->pat;
	ਪੂਰਨ patlen = (ਪੂਰਨ)STRLEN(pat);
	ਪੂਰਨ startpos = ((fuzzy_ਵਸਤੂ_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_ਵਸਤੂ_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_ਵਸਤੂ_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_ਵਸਤੂ_T *)s2)->itemstr + startpos, patlen) == 0;

	ਜੇ (exact_match1 == exact_match2)
	{
	    ਪੂਰਨ idx1 = ((fuzzy_ਵਸਤੂ_T *)s1)->idx;
	    ਪੂਰਨ idx2 = ((fuzzy_ਵਸਤੂ_T *)s2)->idx;
	    ਵਾਪਸ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (exact_match2)
	    ਵਾਪਸ 1;
	ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ v1 > v2 ? -1 : 1;
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
    ਸਥਿਰ ਖਾਲੀ
fuzzy_ਮੇਲ_ਵਿੱਚ_list(
    list_T	*l,
    char_u	*str,
    ਪੂਰਨ		matchseq,
    char_u	*ਕੁੰਜੀ,
    callback_T	*ਵਸਤੂ_cb,
    ਪੂਰਨ		retmatchpos,
    list_T	*fmatchlist,
    ਲੰਮਾ	max_matches)
{
    ਲੰਮਾ	    len;
    fuzzy_ਵਸਤੂ_T	    *ਚੀਜ਼ਾਂ = ਨਿਰਾ;
    listitem_T	    *li;
    ਲੰਮਾ	    ਮੇਲ_ਗਿਣਤੀ = 0;
    int_u	    matches[FUZZY_ਮੇਲ_MAX_LEN];

    len = list_len(l);
    ਜੇ (len == 0)
	ਵਾਪਸ;
    ਜੇ (max_matches > 0 && len > max_matches)
	len = max_matches;

    ਚੀਜ਼ਾਂ = ALLOC_CLEAR_MULT(fuzzy_ਵਸਤੂ_T, len);
    ਜੇ (ਚੀਜ਼ਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਪੂਰਨ		score;
	char_u		*itemstr = ਨਿਰਾ;
	char_u		*itemstr_copy = ਨਿਰਾ;
	typval_T	rettv;
	ਪੂਰਨ		itemstr_allocate = FALSE;
	list_T		*ਮੇਲ_positions = ਨਿਰਾ;

	ਜੇ (max_matches > 0 && ਮੇਲ_ਗਿਣਤੀ >= max_matches)
	    ਰੋਕੋ;

	rettv.v_type = VAR_UNKNOWN;
	ਜੇ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ਨਹੀਂਤਾਂ ਜੇ (li->li_tv.v_type == VAR_DICT
				&& (ਕੁੰਜੀ != ਨਿਰਾ || ਵਸਤੂ_cb->cb_name != ਨਿਰਾ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ਜੇ (ਕੁੰਜੀ != ਨਿਰਾ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (ਅੱਖਰ *)ਕੁੰਜੀ, FALSE);
	    ਨਹੀਂਤਾਂ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ਜੇ (call_callback(ਵਸਤੂ_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ਜੇ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ਜੇ (itemstr != ਨਿਰਾ
		&& fuzzy_ਮੇਲ(itemstr, str, matchseq, &score, matches,
						FUZZY_ਮੇਲ_MAX_LEN))
	{
	    ਜੇ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ਜੇ (itemstr_copy == ਨਿਰਾ)
		{
		    clear_tv(&rettv);
		    ਜਾਰੀ;
		}
	    }
	    ਨਹੀਂਤਾਂ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ਜੇ (retmatchpos)
	    {
		ਮੇਲ_positions = list_alloc();
		ਜੇ (ਮੇਲ_positions == ਨਿਰਾ)
		{
		    ਜੇ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ਜਾਰੀ;
		}

		// Fill position information
		ਪੂਰਨ	j = 0;
		char_u	*p = str;
		ਪੂਰਨ	success = TRUE;

		ਜਦੋਂ (*p != NUL && j < FUZZY_ਮੇਲ_MAX_LEN && success)
		{
		    ਜੇ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ਜੇ (list_append_ਸੰਖਿਆ(ਮੇਲ_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    ਰੋਕੋ;
			}
			j++;
		    }
		    ਜੇ (has_mbyte)
			MB_PTR_ADV(p);
		    ਨਹੀਂਤਾਂ
			++p;
		}

		ਜੇ (!success)
		{
		    list_free(ਮੇਲ_positions);
		    ਜੇ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ਜਾਰੀ;
		}
	    }
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].idx = ਮੇਲ_ਗਿਣਤੀ;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].ਵਸਤੂ = li;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].score = score;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].pat = str;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].startpos = matches[0];
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].itemstr = itemstr_copy;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].itemstr_allocated = itemstr_allocate;
	    ਚੀਜ਼ਾਂ[ਮੇਲ_ਗਿਣਤੀ].lmatchpos = ਮੇਲ_positions;

	    ++ਮੇਲ_ਗਿਣਤੀ;
	}
	clear_tv(&rettv);
    }

    ਜੇ (ਮੇਲ_ਗਿਣਤੀ > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((ਖਾਲੀ *)ਚੀਜ਼ਾਂ, (size_t)ਮੇਲ_ਗਿਣਤੀ, ਆਕਾਰ(fuzzy_ਵਸਤੂ_T),
		fuzzy_ਮੇਲ_ਵਸਤੂ_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ਜੇ (retmatchpos)
	{
	    li = list_ਲੱਭੋ(fmatchlist, 0);
	    ਜੇ (li == ਨਿਰਾ || li->li_tv.vval.v_list == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;
	    retlist = li->li_tv.vval.v_list;
	}
	ਨਹੀਂਤਾਂ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	ਲਈ (ਪੂਰਨ i = 0; i < ਮੇਲ_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (list_append_tv(retlist, &ਚੀਜ਼ਾਂ[i].ਵਸਤੂ->li_tv) == FAIL)
		ਜਾਓ ਪੂਰਾ;
	}

	// next copy the list of matching positions
	ਜੇ (retmatchpos)
	{
	    li = list_ਲੱਭੋ(fmatchlist, -2);
	    ਜੇ (li == ਨਿਰਾ || li->li_tv.vval.v_list == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;
	    retlist = li->li_tv.vval.v_list;

	    ਲਈ (ਪੂਰਨ i = 0; i < ਮੇਲ_ਗਿਣਤੀ; i++)
	    {
		ਜੇ (ਚੀਜ਼ਾਂ[i].lmatchpos != ਨਿਰਾ)
		{
		    ਜੇ (list_append_list(retlist, ਚੀਜ਼ਾਂ[i].lmatchpos) == OK)
			ਚੀਜ਼ਾਂ[i].lmatchpos = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
			ਜਾਓ ਪੂਰਾ;

		}
	    }

	    // copy the matching scores
	    li = list_ਲੱਭੋ(fmatchlist, -1);
	    ਜੇ (li == ਨਿਰਾ || li->li_tv.vval.v_list == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;
	    retlist = li->li_tv.vval.v_list;
	    ਲਈ (ਪੂਰਨ i = 0; i < ਮੇਲ_ਗਿਣਤੀ; i++)
	    {
		ਜੇ (list_append_ਸੰਖਿਆ(retlist, ਚੀਜ਼ਾਂ[i].score) == FAIL)
		    ਜਾਓ ਪੂਰਾ;
	    }
	}
    }

ਪੂਰਾ:
    ਲਈ (ਪੂਰਨ i = 0; i < ਮੇਲ_ਗਿਣਤੀ; i++)
    {
	ਜੇ (ਚੀਜ਼ਾਂ[i].itemstr_allocated)
	    vim_free(ਚੀਜ਼ਾਂ[i].itemstr);

	ਜੇ (ਚੀਜ਼ਾਂ[i].lmatchpos)
	    list_free(ਚੀਜ਼ਾਂ[i].lmatchpos);
    }
    vim_free(ਚੀਜ਼ਾਂ);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ਸਥਿਰ ਖਾਲੀ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, ਪੂਰਨ retmatchpos)
{
    callback_T	cb;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ;
    ਪੂਰਨ		ret;
    ਪੂਰਨ		matchseq = FALSE;
    ਲੰਮਾ	max_matches = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    CLEAR_ਸੂਚਕ(&cb);

    // validate and get the arguments
    ਜੇ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ਨਿਰਾ)
    {
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ਵਾਪਸ;
    }
    ਜੇ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[1]));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ਜੇ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"key", -1)) != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ਨਿਰਾ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "key",
						    tv_get_string(&di->di_tv));
		ਵਾਪਸ;
	    }
	    ਕੁੰਜੀ = tv_get_string(&di->di_tv);
	}
	ਨਹੀਂਤਾਂ ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"text_cb", -1)) != ਨਿਰਾ)
	{
	    cb = get_callback(&di->di_tv);
	    ਜੇ (cb.cb_name == ਨਿਰਾ)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "text_cb");
		ਵਾਪਸ;
	    }
	}

	ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"limit", -1)) != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_ਸੰਖਿਆ)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "limit");
		ਵਾਪਸ;
	    }
	    max_matches = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&di->di_tv, ਨਿਰਾ);
	}

	ਜੇ (dict_has_ਕੁੰਜੀ(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ਜੇ (ret == FAIL)
	ਜਾਓ ਪੂਰਾ;
    ਜੇ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;
	ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ਜਾਓ ਪੂਰਾ;
	}
	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;
	ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ਜਾਓ ਪੂਰਾ;
	}
	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;
	ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ਜਾਓ ਪੂਰਾ;
	}
    }

    fuzzy_ਮੇਲ_ਵਿੱਚ_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, ਕੁੰਜੀ, &cb, retmatchpos, rettv->vval.v_list, max_matches);

ਪੂਰਾ:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    ਖਾਲੀ
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    ਖਾਲੀ
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#ਜੇ_ਅੰਤ

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ਸਥਿਰ ਪੂਰਨ
fuzzy_ਮੇਲ_str_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਪੂਰਨ		v1 = ((fuzmatch_str_T *)s1)->score;
    ਪੂਰਨ		v2 = ((fuzmatch_str_T *)s2)->score;
    ਪੂਰਨ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ਪੂਰਨ		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ਜੇ (v1 == v2)
	ਵਾਪਸ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ਸਥਿਰ ਖਾਲੀ
fuzzy_ਮੇਲ_str_ਛਾਂਟੋ(fuzmatch_str_T *fm, ਪੂਰਨ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ਖਾਲੀ *)fm, (size_t)sz, ਆਕਾਰ(fuzmatch_str_T),
	    fuzzy_ਮੇਲ_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ਸਥਿਰ ਪੂਰਨ
fuzzy_ਮੇਲ_func_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਪੂਰਨ		v1 = ((fuzmatch_str_T *)s1)->score;
    ਪੂਰਨ		v2 = ((fuzmatch_str_T *)s2)->score;
    ਪੂਰਨ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ਪੂਰਨ		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ਜੇ (*str1 != '<' && *str2 == '<')
	ਵਾਪਸ -1;
    ਜੇ (*str1 == '<' && *str2 != '<')
	ਵਾਪਸ 1;
    ਜੇ (v1 == v2)
	ਵਾਪਸ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ਸਥਿਰ ਖਾਲੀ
fuzzy_ਮੇਲ_func_ਛਾਂਟੋ(fuzmatch_str_T *fm, ਪੂਰਨ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ਖਾਲੀ *)fm, (size_t)sz, ਆਕਾਰ(fuzmatch_str_T),
		fuzzy_ਮੇਲ_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    ਪੂਰਨ
fuzzy_ਮੇਲ_str(char_u *str, char_u *pat)
{
    ਪੂਰਨ		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_ਮੇਲ_MAX_LEN];

    ਜੇ (str == ਨਿਰਾ || pat == ਨਿਰਾ)
	ਵਾਪਸ score;

    fuzzy_ਮੇਲ(str, pat, TRUE, &score, matchpos,
				ਆਕਾਰ(matchpos) / ਆਕਾਰ(matchpos[0]));

    ਵਾਪਸ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_ਮੇਲ_str_with_pos(char_u *str ਅਣਵਰਤਿਆ, char_u *pat ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ		    score = FUZZY_SCORE_NONE;
    garray_T	    *ਮੇਲ_positions = ਨਿਰਾ;
    int_u	    matches[FUZZY_ਮੇਲ_MAX_LEN];
    ਪੂਰਨ		    j = 0;

    ਜੇ (str == ਨਿਰਾ || pat == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਮੇਲ_positions = ALLOC_ONE(garray_T);
    ਜੇ (ਮੇਲ_positions == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ga_init2(ਮੇਲ_positions, ਆਕਾਰ(int_u), 10);

    ਜੇ (!fuzzy_ਮੇਲ(str, pat, FALSE, &score, matches, FUZZY_ਮੇਲ_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(ਮੇਲ_positions);
	vim_free(ਮੇਲ_positions);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ਜੇ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ਜੇ (ga_grow(ਮੇਲ_positions, 1) == FAIL)
	    {
		ga_clear(ਮੇਲ_positions);
		vim_free(ਮੇਲ_positions);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ((int_u *)ਮੇਲ_positions->ga_ਅੰਕੜੇ)[ਮੇਲ_positions->ga_len] =
								    matches[j];
	    ਮੇਲ_positions->ga_len++;
	    j++;
	}
    }

    ਵਾਪਸ ਮੇਲ_positions;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
fuzzy_ਮੇਲ_str_ਵਿੱਚ_ਸਤਰ(
    char_u	**ptr,
    char_u	*pat,
    ਪੂਰਨ		*len,
    pos_T	*current_pos,
    ਪੂਰਨ		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*ਸਿਰਾ = ਨਿਰਾ;
    char_u	*ਸ਼ੁਰੂ = ਨਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;
    ਅੱਖਰ	save_ਸਿਰਾ;
    char_u	*ਸਤਰ_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ (str == ਨਿਰਾ || pat == ਨਿਰਾ)
	ਵਾਪਸ ਮਿਲਿਆ;
    ਸਤਰ_ਸਿਰਾ = ਲੱਭੋ_ਸਤਰ_ਸਿਰਾ(str);

    ਜਦੋਂ (str < ਸਤਰ_ਸਿਰਾ)
    {
	// Skip non-word characters
	ਸ਼ੁਰੂ = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(str);
	ਜੇ (*ਸ਼ੁਰੂ == NUL)
	    ਰੋਕੋ;
	ਸਿਰਾ = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ਸ਼ੁਰੂ);

	// Extract the word from start to end
	save_ਸਿਰਾ = *ਸਿਰਾ;
	*ਸਿਰਾ = NUL;

	// Perform fuzzy match
	*score = fuzzy_ਮੇਲ_str(ਸ਼ੁਰੂ, pat);
	*ਸਿਰਾ = save_ਸਿਰਾ;

	ਜੇ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (ਪੂਰਨ)(ਸਿਰਾ - ਸ਼ੁਰੂ);
	    ਮਿਲਿਆ = TRUE;
	    *ptr = ਸ਼ੁਰੂ;
	    ਜੇ (current_pos)
		current_pos->ਖਾਨਾ += (ਪੂਰਨ)(ਸਿਰਾ - strBegin);
	    ਰੋਕੋ;
	}

	// Move to the end of the current word for the next iteration
	str = ਸਿਰਾ;
	// Ensure we continue searching after the current word
	ਜਦੋਂ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ਜੇ (!ਮਿਲਿਆ)
	*ptr = ਸਤਰ_ਸਿਰਾ;

    ਵਾਪਸ ਮਿਲਿਆ;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    ਪੂਰਨ
ਖੋਜ_for_fuzzy_ਮੇਲ(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*ਨਮੂਨਾ,
    ਪੂਰਨ		dir,
    pos_T	*ਸ਼ੁਰੂ_pos,
    ਪੂਰਨ		*len,
    char_u	**ptr,
    ਪੂਰਨ		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_ਸਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ_new_ਮੇਲ = FALSE;
    ਪੂਰਨ		looped_around = FALSE;
    ਪੂਰਨ		ਸਾਰਾ_ਸਤਰ = ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ();

    ਜੇ (buf == curbuf)
	circly_ਸਿਰਾ = *ਸ਼ੁਰੂ_pos;
    ਨਹੀਂਤਾਂ
    {
	circly_ਸਿਰਾ.lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	circly_ਸਿਰਾ.ਖਾਨਾ = 0;
	circly_ਸਿਰਾ.coladd = 0;
    }

    ਜੇ (ਸਾਰਾ_ਸਤਰ && ਸ਼ੁਰੂ_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    ਕਰੋ
    {

	// Check if looped around and back to start position
	ਜੇ (looped_around && (ਸਾਰਾ_ਸਤਰ ? current_pos.lnum == circly_ਸਿਰਾ.lnum
					: EQUAL_POS(current_pos, circly_ਸਿਰਾ)))
	    ਰੋਕੋ;

	// Ensure current_pos is valid
	ਜੇ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ਜੇ (!ਸਾਰਾ_ਸਤਰ)
		*ptr += current_pos.ਖਾਨਾ;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ਜੇ (*ptr != ਨਿਰਾ && **ptr != NUL)
	    {
		ਜੇ (!ਸਾਰਾ_ਸਤਰ)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    ਮਿਲਿਆ_new_ਮੇਲ = fuzzy_ਮੇਲ_str_ਵਿੱਚ_ਸਤਰ(ptr, ਨਮੂਨਾ,
						    len, &current_pos, score);
		    ਜੇ (ਮਿਲਿਆ_new_ਮੇਲ)
		    {
			*pos = current_pos;
			ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (looped_around && current_pos.lnum == circly_ਸਿਰਾ.lnum)
			ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		{
		    char_u *ਸਤਰ = *ptr;
		    char_u *p = skipwhite(ਸਤਰ);
		    ਜੇ (fuzzy_ਮੇਲ_str(p, ਨਮੂਨਾ) != FUZZY_SCORE_NONE)
		    {
			ਮਿਲਿਆ_new_ਮੇਲ = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (ਪੂਰਨ)ml_get_buf_len(buf, current_pos.lnum) -
								(ਪੂਰਨ)(p - ਸਤਰ);
			ਰੋਕੋ;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ਜੇ (dir == FORWARD)
	{
	    ਜੇ (++current_pos.lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਜੇ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (--current_pos.lnum < 1)
	    {
		ਜੇ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		    looped_around = TRUE;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;

	    }
	}
	current_pos.ਖਾਨਾ = 0;
    } ਜਦੋਂ (TRUE);

    ਵਾਪਸ ਮਿਲਿਆ_new_ਮੇਲ;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    ਖਾਲੀ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (fuzmatch == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ਪੂਰਨ
fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	ਪੂਰਨ		ਗਿਣਤੀ_2,
	ਪੂਰਨ		funcsort)
{
    ਜੇ (ਗਿਣਤੀ_2 <= 0)
	ਜਾਓ theend;

    *matches = ALLOC_MULT(char_u *, ਗਿਣਤੀ_2);
    ਜੇ (*matches == ਨਿਰਾ)
    {
	fuzmatch_str_free(fuzmatch, ਗਿਣਤੀ_2);
	ਵਾਪਸ FAIL;
    }

    // Sort the list by the descending order of the match score
    ਜੇ (funcsort)
	fuzzy_ਮੇਲ_func_ਛਾਂਟੋ((ਖਾਲੀ *)fuzmatch, (size_t)ਗਿਣਤੀ_2);
    ਨਹੀਂਤਾਂ
	fuzzy_ਮੇਲ_str_ਛਾਂਟੋ((ਖਾਲੀ *)fuzmatch, (size_t)ਗਿਣਤੀ_2);

    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ਵਾਪਸ OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੇਲ_MAX_LEN FUZZY_ਮੇਲ_MAX_LEN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_GAP_LEADING -0.005
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_GAP_TRAILING -0.005
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_GAP_INNER -0.01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਮੇਲ_CONSECUTIVE 1.0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਮੇਲ_SLASH 0.9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਮੇਲ_ਸ਼ਬਦ 0.8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਮੇਲ_CAPITAL 0.7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਮੇਲ_DOT 0.6

    ਸਥਿਰ ਪੂਰਨ
has_ਮੇਲ(char_u *needle, char_u *haystack)
{
    ਜੇ (!needle || !haystack || !*needle)
	ਵਾਪਸ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ਜਦੋਂ (*n_ptr)
    {
	ਪੂਰਨ n_char = mb_ptr2char(n_ptr);
	ਪੂਰਨ n_ਉੱਤਲਾ = MB_TOUPPER(n_char);
	ਪੂਰਨ ਮਿਲਿਆ = FALSE;

	ਜਦੋਂ (*h_ptr)
	{
	    ਪੂਰਨ h_char = mb_ptr2char(h_ptr);
	    ਜੇ (h_char == n_char || h_char == n_ਉੱਤਲਾ)
	    {
		ਮਿਲਿਆ = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		ਰੋਕੋ;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ਜੇ (!ਮਿਲਿਆ)
	    ਵਾਪਸ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ਵਾਪਸ OK;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਮੇਲ_struct
{
    ਪੂਰਨ needle_len;
    ਪੂਰਨ haystack_len;
    ਪੂਰਨ ਹੇਠਲਾ_needle[ਮੇਲ_MAX_LEN];     // stores codepoints
    ਪੂਰਨ ਹੇਠਲਾ_haystack[ਮੇਲ_MAX_LEN];   // stores codepoints
    score_t ਮੇਲ_bonus[ਮੇਲ_MAX_LEN];
} ਮੇਲ_struct;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਸ਼ਬਦ_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਰਸਤਾ_SEP(c) ((c) == '/')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_DOT(c)      ((c) == '.')

    ਸਥਿਰ score_t
compute_bonus_codepoint(ਪੂਰਨ ਆਖਰੀ_c, ਪੂਰਨ c)
{
    ਜੇ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ਜੇ (IS_ਰਸਤਾ_SEP(ਆਖਰੀ_c))
	    ਵਾਪਸ SCORE_ਮੇਲ_SLASH;
	ਜੇ (IS_ਸ਼ਬਦ_SEP(ਆਖਰੀ_c))
	    ਵਾਪਸ SCORE_ਮੇਲ_ਸ਼ਬਦ;
	ਜੇ (IS_DOT(ਆਖਰੀ_c))
	    ਵਾਪਸ SCORE_ਮੇਲ_DOT;
	ਜੇ (MB_ISUPPER(c) && MB_ISLOWER(ਆਖਰੀ_c))
	    ਵਾਪਸ SCORE_ਮੇਲ_CAPITAL;
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_ਮੇਲ_struct(ਮੇਲ_struct *ਮੇਲ, char_u *needle, char_u *haystack)
{
    ਪੂਰਨ i = 0;
    char_u *p = needle;
    ਜਦੋਂ (*p != NUL && i < ਮੇਲ_MAX_LEN)
    {
	ਪੂਰਨ c = mb_ptr2char(p);
	ਮੇਲ->ਹੇਠਲਾ_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    ਮੇਲ->needle_len = i;

    i = 0;
    p = haystack;
    ਪੂਰਨ prev_c = '/';
    ਜਦੋਂ (*p != NUL && i < ਮੇਲ_MAX_LEN)
    {
	ਪੂਰਨ c = mb_ptr2char(p);
	ਮੇਲ->ਹੇਠਲਾ_haystack[i] = MB_TOLOWER(c);
	ਮੇਲ->ਮੇਲ_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    ਮੇਲ->haystack_len = i;
}

    ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਮੇਲ_ਪੰਕਤੀ(ਅਚਲ ਮੇਲ_struct *ਮੇਲ, ਪੂਰਨ ਪੰਕਤੀ, score_t *curr_D,
	score_t *curr_M, ਅਚਲ score_t *ਆਖਰੀ_D, ਅਚਲ score_t *ਆਖਰੀ_M)
{
    ਪੂਰਨ n = ਮੇਲ->needle_len;
    ਪੂਰਨ m = ਮੇਲ->haystack_len;
    ਪੂਰਨ i = ਪੰਕਤੀ;

    ਅਚਲ ਪੂਰਨ *ਹੇਠਲਾ_needle = ਮੇਲ->ਹੇਠਲਾ_needle;
    ਅਚਲ ਪੂਰਨ *ਹੇਠਲਾ_haystack = ਮੇਲ->ਹੇਠਲਾ_haystack;
    ਅਚਲ score_t *ਮੇਲ_bonus = ਮੇਲ->ਮੇਲ_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    ਲਈ (ਪੂਰਨ j = 0; j < m; j++)
    {
	ਜੇ (ਹੇਠਲਾ_needle[i] == ਹੇਠਲਾ_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ਜੇ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + ਮੇਲ_bonus[j];
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + ਮੇਲ_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_ਮੇਲ_CONSECUTIVE);
	    }
	    prev_D = ਆਖਰੀ_D[j];
	    prev_M = ਆਖਰੀ_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	ਨਹੀਂਤਾਂ
	{
	    prev_D = ਆਖਰੀ_D[j];
	    prev_M = ਆਖਰੀ_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ਸਥਿਰ score_t
ਮੇਲ_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ਜੇ (!needle || !haystack || !*needle)
	ਵਾਪਸ SCORE_MIN;

    ਮੇਲ_struct ਮੇਲ;
    ਤਿਆਰੀ_ਮੇਲ_struct(&ਮੇਲ, needle, haystack);

    ਪੂਰਨ n = ਮੇਲ.needle_len;
    ਪੂਰਨ m = ਮੇਲ.haystack_len;

    ਜੇ (m > ਮੇਲ_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ਵਾਪਸ SCORE_MIN;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	ਲਈ (ਪੂਰਨ i = 0; i < n; i++)
	{
	    ਜੇ (ਮੇਲ.ਹੇਠਲਾ_needle[i] != ਮੇਲ.ਹੇਠਲਾ_haystack[i])
	    {
		equal = false;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (equal)
	{
	    ਜੇ (positions)
	    {
		ਲਈ (ਪੂਰਨ i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ਵਾਪਸ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ਜੇ ((size_t)n > (ਮਾਪ_MAX / ਆਕਾਰ(score_t)) / ਮੇਲ_MAX_LEN / 2)
	ਵਾਪਸ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *ਖੰਡ = (score_t*)alloc(ਆਕਾਰ(score_t) * ਮੇਲ_MAX_LEN * n * 2);
    ਜੇ (!ਖੰਡ)
	ਵਾਪਸ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[ਮੇਲ_MAX_LEN] = (score_t(*)[ਮੇਲ_MAX_LEN])ਖੰਡ;
    score_t (*M)[ਮੇਲ_MAX_LEN] = (score_t(*)[ਮੇਲ_MAX_LEN])(ਖੰਡ
							+ ਮੇਲ_MAX_LEN * n);

    ਮੇਲ_ਪੰਕਤੀ(&ਮੇਲ, 0, D[0], M[0], D[0], M[0]);
    ਲਈ (ਪੂਰਨ i = 1; i < n; i++)
	ਮੇਲ_ਪੰਕਤੀ(&ਮੇਲ, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ਜੇ (positions)
    {
	ਪੂਰਨ ਮੇਲ_required = 0;
	ਲਈ (ਪੂਰਨ i = n - 1, j = m - 1; i >= 0; i--)
	{
	    ਲਈ (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ਜੇ (D[i][j] != SCORE_MIN &&
			(ਮੇਲ_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    ਮੇਲ_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_ਮੇਲ_CONSECUTIVE;
		    positions[i] = j--;
		    ਰੋਕੋ;
		}
	    }
	}
    }

    score_t ਨਤੀਜਾ = M[n - 1][m - 1];
    vim_free(ਖੰਡ);
    ਵਾਪਸ ਨਤੀਜਾ;
}
