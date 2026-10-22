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
#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PROTO)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fuzzy_ಹೊಂದಿಕೆ_ಅಂಶ_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_ಹೊಂದಿಕೆ_ಒಳ_list(list_T *l, char_u *str, ಪೂರ್ಣಾಂಕ matchseq, char_u *ಕೀಲಿ, callback_T *ಅಂಶ_cb, ಪೂರ್ಣಾಂಕ retmatchpos, list_T *fmatchlist, ದೀರ್ಘ max_matches);
ಸ್ಥಿರ ಶೂನ್ಯ do_fuzzymatch(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ retmatchpos);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fuzzy_ಹೊಂದಿಕೆ_str_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_ಹೊಂದಿಕೆ_str_ವಿಂಗಡಿಸು(fuzmatch_str_T *fm, ಪೂರ್ಣಾಂಕ sz);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fuzzy_ಹೊಂದಿಕೆ_func_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಶೂನ್ಯ fuzzy_ಹೊಂದಿಕೆ_func_ವಿಂಗಡಿಸು(fuzmatch_str_T *fm, ಪೂರ್ಣಾಂಕ sz);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದ್ವಿಗುಣ score_t;
ಸ್ಥಿರ score_t ಹೊಂದಿಕೆ_positions(char_u *needle, char_u *haystack, int_u *positions);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_ಹೊಂದಿಕೆ(char_u *needle, char_u *haystack);

#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_MAX INFINITY
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_MIN (-INFINITY)
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SCALE 1000

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		idx;		// used for stable sort
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    ಪೂರ್ಣಾಂಕ		itemstr_allocated;
    ಪೂರ್ಣಾಂಕ		startpos;
} fuzzy_ಅಂಶ_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ(
    char_u	*str,
    char_u	*pat_arg,
    ಪೂರ್ಣಾಂಕ		matchseq,
    ಪೂರ್ಣಾಂಕ		*ಹೊರಹೋಗು_Score,
    int_u	*matches,
    ಪೂರ್ಣಾಂಕ		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		complete = FALSE;
    ಪೂರ್ಣಾಂಕ		score = 0;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Matches = 0;
    ಪೂರ್ಣಾಂಕ		pat_chars = 0;
    score_t	fzy_score;

    *ಹೊರಹೋಗು_Score = 0;

    save_pat = vim_strsave(pat_arg);
    ಆದರೆ (save_pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ಆಗ (TRUE)
    {
	ಆದರೆ (matchseq)
	    complete = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	    pat = p;
	    ಆಗ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ಆದರೆ (has_mbyte)
		    MB_PTR_ADV(p);
		ಇಲ್ಲದಿದ್ದರೆ
		    ++p;
	    }
	    ಆದರೆ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ಆದರೆ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ಆದರೆ (ಅಂಕೆ_Matches > maxMatches - pat_chars)
	{
	    ಅಂಕೆ_Matches = 0;
	    *ಹೊರಹೋಗು_Score = FUZZY_SCORE_NONE;
	    ನಿಲ್ಲಿಸು;
	}

	score = FUZZY_SCORE_NONE;
	ಆದರೆ (has_ಹೊಂದಿಕೆ(pat, str))
	{
	    fzy_score = ಹೊಂದಿಕೆ_positions(pat, str, matches + ಅಂಕೆ_Matches);
	    ಆದರೆ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
		    : (fzy_score < 0) ? (ಪೂರ್ಣಾಂಕ)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (ಪೂರ್ಣಾಂಕ)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ಆದರೆ (score == FUZZY_SCORE_NONE)
	{
	    ಅಂಕೆ_Matches = 0;
	    *ಹೊರಹೋಗು_Score = FUZZY_SCORE_NONE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (score > 0 && *ಹೊರಹೋಗು_Score > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - score)
	    *ಹೊರಹೋಗು_Score = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (score < 0 && *ಹೊರಹೋಗು_Score < INT_MIN + 1 - score)
	    *ಹೊರಹೋಗು_Score = INT_MIN + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಹೊರಹೋಗು_Score += score;

	ಅಂಕೆ_Matches += pat_chars;

	ಆದರೆ (complete || ಅಂಕೆ_Matches >= maxMatches)
	    ನಿಲ್ಲಿಸು;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ಹಿಂತಿರುಗಿ ಅಂಕೆ_Matches != 0;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ_ಅಂಶ_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಂಕ		v1 = ((fuzzy_ಅಂಶ_T *)s1)->score;
    ಪೂರ್ಣಾಂಕ		v2 = ((fuzzy_ಅಂಶ_T *)s2)->score;

    ಆದರೆ (v1 == v2)
    {
	ಪೂರ್ಣಾಂಕ exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_ಅಂಶ_T *)s1)->pat;
	ಪೂರ್ಣಾಂಕ patlen = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
	ಪೂರ್ಣಾಂಕ startpos = ((fuzzy_ಅಂಶ_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_ಅಂಶ_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_ಅಂಶ_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_ಅಂಶ_T *)s2)->itemstr + startpos, patlen) == 0;

	ಆದರೆ (exact_match1 == exact_match2)
	{
	    ಪೂರ್ಣಾಂಕ idx1 = ((fuzzy_ಅಂಶ_T *)s1)->idx;
	    ಪೂರ್ಣಾಂಕ idx2 = ((fuzzy_ಅಂಶ_T *)s2)->idx;
	    ಹಿಂತಿರುಗಿ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (exact_match2)
	    ಹಿಂತಿರುಗಿ 1;
	ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ v1 > v2 ? -1 : 1;
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
fuzzy_ಹೊಂದಿಕೆ_ಒಳ_list(
    list_T	*l,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		matchseq,
    char_u	*ಕೀಲಿ,
    callback_T	*ಅಂಶ_cb,
    ಪೂರ್ಣಾಂಕ		retmatchpos,
    list_T	*fmatchlist,
    ದೀರ್ಘ	max_matches)
{
    ದೀರ್ಘ	    len;
    fuzzy_ಅಂಶ_T	    *ವಸ್ತುಗಳು = ಖಾಲಿ;
    listitem_T	    *li;
    ದೀರ್ಘ	    ಹೊಂದಿಕೆ_ಎಣಿಕೆ = 0;
    int_u	    matches[FUZZY_ಹೊಂದಿಕೆ_MAX_LEN];

    len = list_len(l);
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (max_matches > 0 && len > max_matches)
	len = max_matches;

    ವಸ್ತುಗಳು = ALLOC_CLEAR_MULT(fuzzy_ಅಂಶ_T, len);
    ಆದರೆ (ವಸ್ತುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಪೂರ್ಣಾಂಕ		score;
	char_u		*itemstr = ಖಾಲಿ;
	char_u		*itemstr_copy = ಖಾಲಿ;
	typval_T	rettv;
	ಪೂರ್ಣಾಂಕ		itemstr_allocate = FALSE;
	list_T		*ಹೊಂದಿಕೆ_positions = ಖಾಲಿ;

	ಆದರೆ (max_matches > 0 && ಹೊಂದಿಕೆ_ಎಣಿಕೆ >= max_matches)
	    ನಿಲ್ಲಿಸು;

	rettv.v_type = VAR_UNKNOWN;
	ಆದರೆ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (li->li_tv.v_type == VAR_DICT
				&& (ಕೀಲಿ != ಖಾಲಿ || ಅಂಶ_cb->cb_name != ಖಾಲಿ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ಆದರೆ (ಕೀಲಿ != ಖಾಲಿ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (ಅಕ್ಷರ *)ಕೀಲಿ, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ಆದರೆ (call_callback(ಅಂಶ_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ಆದರೆ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ಆದರೆ (itemstr != ಖಾಲಿ
		&& fuzzy_ಹೊಂದಿಕೆ(itemstr, str, matchseq, &score, matches,
						FUZZY_ಹೊಂದಿಕೆ_MAX_LEN))
	{
	    ಆದರೆ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ಆದರೆ (itemstr_copy == ಖಾಲಿ)
		{
		    clear_tv(&rettv);
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ಆದರೆ (retmatchpos)
	    {
		ಹೊಂದಿಕೆ_positions = list_alloc();
		ಆದರೆ (ಹೊಂದಿಕೆ_positions == ಖಾಲಿ)
		{
		    ಆದರೆ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ಮುಂದುವರಿ;
		}

		// Fill position information
		ಪೂರ್ಣಾಂಕ	j = 0;
		char_u	*p = str;
		ಪೂರ್ಣಾಂಕ	success = TRUE;

		ಆಗ (*p != NUL && j < FUZZY_ಹೊಂದಿಕೆ_MAX_LEN && success)
		{
		    ಆದರೆ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ಆದರೆ (list_append_ಸಂಖ್ಯೆ(ಹೊಂದಿಕೆ_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    ನಿಲ್ಲಿಸು;
			}
			j++;
		    }
		    ಆದರೆ (has_mbyte)
			MB_PTR_ADV(p);
		    ಇಲ್ಲದಿದ್ದರೆ
			++p;
		}

		ಆದರೆ (!success)
		{
		    list_free(ಹೊಂದಿಕೆ_positions);
		    ಆದರೆ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ಮುಂದುವರಿ;
		}
	    }
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].idx = ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].ಅಂಶ = li;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].score = score;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].pat = str;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].startpos = matches[0];
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].itemstr = itemstr_copy;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].itemstr_allocated = itemstr_allocate;
	    ವಸ್ತುಗಳು[ಹೊಂದಿಕೆ_ಎಣಿಕೆ].lmatchpos = ಹೊಂದಿಕೆ_positions;

	    ++ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
	}
	clear_tv(&rettv);
    }

    ಆದರೆ (ಹೊಂದಿಕೆ_ಎಣಿಕೆ > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((ಶೂನ್ಯ *)ವಸ್ತುಗಳು, (size_t)ಹೊಂದಿಕೆ_ಎಣಿಕೆ, ಗಾತ್ರ(fuzzy_ಅಂಶ_T),
		fuzzy_ಹೊಂದಿಕೆ_ಅಂಶ_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ಆದರೆ (retmatchpos)
	{
	    li = list_ಕಂಡುಹಿಡಿ(fmatchlist, 0);
	    ಆದರೆ (li == ಖಾಲಿ || li->li_tv.vval.v_list == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;
	    retlist = li->li_tv.vval.v_list;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಹೊಂದಿಕೆ_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (list_append_tv(retlist, &ವಸ್ತುಗಳು[i].ಅಂಶ->li_tv) == FAIL)
		ಹೋಗು ಮುಗಿದ;
	}

	// next copy the list of matching positions
	ಆದರೆ (retmatchpos)
	{
	    li = list_ಕಂಡುಹಿಡಿ(fmatchlist, -2);
	    ಆದರೆ (li == ಖಾಲಿ || li->li_tv.vval.v_list == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;
	    retlist = li->li_tv.vval.v_list;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಹೊಂದಿಕೆ_ಎಣಿಕೆ; i++)
	    {
		ಆದರೆ (ವಸ್ತುಗಳು[i].lmatchpos != ಖಾಲಿ)
		{
		    ಆದರೆ (list_append_list(retlist, ವಸ್ತುಗಳು[i].lmatchpos) == OK)
			ವಸ್ತುಗಳು[i].lmatchpos = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಹೋಗು ಮುಗಿದ;

		}
	    }

	    // copy the matching scores
	    li = list_ಕಂಡುಹಿಡಿ(fmatchlist, -1);
	    ಆದರೆ (li == ಖಾಲಿ || li->li_tv.vval.v_list == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;
	    retlist = li->li_tv.vval.v_list;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಹೊಂದಿಕೆ_ಎಣಿಕೆ; i++)
	    {
		ಆದರೆ (list_append_ಸಂಖ್ಯೆ(retlist, ವಸ್ತುಗಳು[i].score) == FAIL)
		    ಹೋಗು ಮುಗಿದ;
	    }
	}
    }

ಮುಗಿದ:
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಹೊಂದಿಕೆ_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (ವಸ್ತುಗಳು[i].itemstr_allocated)
	    vim_free(ವಸ್ತುಗಳು[i].itemstr);

	ಆದರೆ (ವಸ್ತುಗಳು[i].lmatchpos)
	    list_free(ವಸ್ತುಗಳು[i].lmatchpos);
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ retmatchpos)
{
    callback_T	cb;
    char_u	*ಕೀಲಿ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		matchseq = FALSE;
    ದೀರ್ಘ	max_matches = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    CLEAR_ಸೂಚಕ(&cb);

    // validate and get the arguments
    ಆದರೆ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ಖಾಲಿ)
    {
	semsg(_(e_ವಾದ_ದ_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[1]));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ಆದರೆ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"key", -1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ಖಾಲಿ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "key",
						    tv_get_string(&di->di_tv));
		ಹಿಂತಿರುಗಿ;
	    }
	    ಕೀಲಿ = tv_get_string(&di->di_tv);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"text_cb", -1)) != ಖಾಲಿ)
	{
	    cb = get_callback(&di->di_tv);
	    ಆದರೆ (cb.cb_name == ಖಾಲಿ)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "text_cb");
		ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"limit", -1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "limit");
		ಹಿಂತಿರುಗಿ;
	    }
	    max_matches = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, ಖಾಲಿ);
	}

	ಆದರೆ (dict_has_ಕೀಲಿ(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ಆದರೆ (ret == FAIL)
	ಹೋಗು ಮುಗಿದ;
    ಆದರೆ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;
	ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಹೋಗು ಮುಗಿದ;
	}
	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;
	ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಹೋಗು ಮುಗಿದ;
	}
	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;
	ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಹೋಗು ಮುಗಿದ;
	}
    }

    fuzzy_ಹೊಂದಿಕೆ_ಒಳ_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, ಕೀಲಿ, &cb, retmatchpos, rettv->vval.v_list, max_matches);

ಮುಗಿದ:
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
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ_str_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಂಕ		v1 = ((fuzmatch_str_T *)s1)->score;
    ಪೂರ್ಣಾಂಕ		v2 = ((fuzmatch_str_T *)s2)->score;
    ಪೂರ್ಣಾಂಕ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ಪೂರ್ಣಾಂಕ		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ಆದರೆ (v1 == v2)
	ಹಿಂತಿರುಗಿ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_ಹೊಂದಿಕೆ_str_ವಿಂಗಡಿಸು(fuzmatch_str_T *fm, ಪೂರ್ಣಾಂಕ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ಶೂನ್ಯ *)fm, (size_t)sz, ಗಾತ್ರ(fuzmatch_str_T),
	    fuzzy_ಹೊಂದಿಕೆ_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ_func_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಂಕ		v1 = ((fuzmatch_str_T *)s1)->score;
    ಪೂರ್ಣಾಂಕ		v2 = ((fuzmatch_str_T *)s2)->score;
    ಪೂರ್ಣಾಂಕ		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ಪೂರ್ಣಾಂಕ		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ಆದರೆ (*str1 != '<' && *str2 == '<')
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (*str1 == '<' && *str2 != '<')
	ಹಿಂತಿರುಗಿ 1;
    ಆದರೆ (v1 == v2)
	ಹಿಂತಿರುಗಿ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_ಹೊಂದಿಕೆ_func_ವಿಂಗಡಿಸು(fuzmatch_str_T *fm, ಪೂರ್ಣಾಂಕ sz)
{
    // Sort the list by the descending order of the match score
    qsort((ಶೂನ್ಯ *)fm, (size_t)sz, ಗಾತ್ರ(fuzmatch_str_T),
		fuzzy_ಹೊಂದಿಕೆ_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ_str(char_u *str, char_u *pat)
{
    ಪೂರ್ಣಾಂಕ		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_ಹೊಂದಿಕೆ_MAX_LEN];

    ಆದರೆ (str == ಖಾಲಿ || pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ score;

    fuzzy_ಹೊಂದಿಕೆ(str, pat, TRUE, &score, matchpos,
				ಗಾತ್ರ(matchpos) / ಗಾತ್ರ(matchpos[0]));

    ಹಿಂತಿರುಗಿ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_ಹೊಂದಿಕೆ_str_with_pos(char_u *str ಬಳಸದ, char_u *pat ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ		    score = FUZZY_SCORE_NONE;
    garray_T	    *ಹೊಂದಿಕೆ_positions = ಖಾಲಿ;
    int_u	    matches[FUZZY_ಹೊಂದಿಕೆ_MAX_LEN];
    ಪೂರ್ಣಾಂಕ		    j = 0;

    ಆದರೆ (str == ಖಾಲಿ || pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹೊಂದಿಕೆ_positions = ALLOC_ONE(garray_T);
    ಆದರೆ (ಹೊಂದಿಕೆ_positions == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_init2(ಹೊಂದಿಕೆ_positions, ಗಾತ್ರ(int_u), 10);

    ಆದರೆ (!fuzzy_ಹೊಂದಿಕೆ(str, pat, FALSE, &score, matches, FUZZY_ಹೊಂದಿಕೆ_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(ಹೊಂದಿಕೆ_positions);
	vim_free(ಹೊಂದಿಕೆ_positions);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ಆದರೆ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ಆದರೆ (ga_grow(ಹೊಂದಿಕೆ_positions, 1) == FAIL)
	    {
		ga_clear(ಹೊಂದಿಕೆ_positions);
		vim_free(ಹೊಂದಿಕೆ_positions);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ((int_u *)ಹೊಂದಿಕೆ_positions->ga_ದತ್ತಾಂಶ)[ಹೊಂದಿಕೆ_positions->ga_len] =
								    matches[j];
	    ಹೊಂದಿಕೆ_positions->ga_len++;
	    j++;
	}
    }

    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_positions;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
fuzzy_ಹೊಂದಿಕೆ_str_ಒಳ_ಸಾಲು(
    char_u	**ptr,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*len,
    pos_T	*current_pos,
    ಪೂರ್ಣಾಂಕ		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*ಕೊನೆ = ಖಾಲಿ;
    char_u	*ಆರಂಭ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;
    ಅಕ್ಷರ	save_ಕೊನೆ;
    char_u	*ಸಾಲು_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ (str == ಖಾಲಿ || pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
    ಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಸಾಲು_ಕೊನೆ(str);

    ಆಗ (str < ಸಾಲು_ಕೊನೆ)
    {
	// Skip non-word characters
	ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(str);
	ಆದರೆ (*ಆರಂಭ == NUL)
	    ನಿಲ್ಲಿಸು;
	ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ಆರಂಭ);

	// Extract the word from start to end
	save_ಕೊನೆ = *ಕೊನೆ;
	*ಕೊನೆ = NUL;

	// Perform fuzzy match
	*score = fuzzy_ಹೊಂದಿಕೆ_str(ಆರಂಭ, pat);
	*ಕೊನೆ = save_ಕೊನೆ;

	ಆದರೆ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ಆರಂಭ);
	    ಸಿಕ್ಕಿತು = TRUE;
	    *ptr = ಆರಂಭ;
	    ಆದರೆ (current_pos)
		current_pos->ಲಂಬಸಾಲು += (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - strBegin);
	    ನಿಲ್ಲಿಸು;
	}

	// Move to the end of the current word for the next iteration
	str = ಕೊನೆ;
	// Ensure we continue searching after the current word
	ಆಗ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ಆದರೆ (!ಸಿಕ್ಕಿತು)
	*ptr = ಸಾಲು_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    ಪೂರ್ಣಾಂಕ
ಹುಡುಕು_for_fuzzy_ಹೊಂದಿಕೆ(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*ನಮೂನೆ,
    ಪೂರ್ಣಾಂಕ		dir,
    pos_T	*ಆರಂಭ_pos,
    ಪೂರ್ಣಾಂಕ		*len,
    char_u	**ptr,
    ಪೂರ್ಣಾಂಕ		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		looped_around = FALSE;
    ಪೂರ್ಣಾಂಕ		ಸಂಪೂರ್ಣ_ಸಾಲು = ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು();

    ಆದರೆ (buf == curbuf)
	circly_ಕೊನೆ = *ಆರಂಭ_pos;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	circly_ಕೊನೆ.lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	circly_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
	circly_ಕೊನೆ.coladd = 0;
    }

    ಆದರೆ (ಸಂಪೂರ್ಣ_ಸಾಲು && ಆರಂಭ_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    ಮಾಡು
    {

	// Check if looped around and back to start position
	ಆದರೆ (looped_around && (ಸಂಪೂರ್ಣ_ಸಾಲು ? current_pos.lnum == circly_ಕೊನೆ.lnum
					: EQUAL_POS(current_pos, circly_ಕೊನೆ)))
	    ನಿಲ್ಲಿಸು;

	// Ensure current_pos is valid
	ಆದರೆ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ಆದರೆ (!ಸಂಪೂರ್ಣ_ಸಾಲು)
		*ptr += current_pos.ಲಂಬಸಾಲು;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ಆದರೆ (*ptr != ಖಾಲಿ && **ptr != NUL)
	    {
		ಆದರೆ (!ಸಂಪೂರ್ಣ_ಸಾಲು)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = fuzzy_ಹೊಂದಿಕೆ_str_ಒಳ_ಸಾಲು(ptr, ನಮೂನೆ,
						    len, &current_pos, score);
		    ಆದರೆ (ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ)
		    {
			*pos = current_pos;
			ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (looped_around && current_pos.lnum == circly_ಕೊನೆ.lnum)
			ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u *ಸಾಲು = *ptr;
		    char_u *p = skipwhite(ಸಾಲು);
		    ಆದರೆ (fuzzy_ಹೊಂದಿಕೆ_str(p, ನಮೂನೆ) != FUZZY_SCORE_NONE)
		    {
			ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (ಪೂರ್ಣಾಂಕ)ml_get_buf_len(buf, current_pos.lnum) -
								(ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ಆದರೆ (dir == FORWARD)
	{
	    ಆದರೆ (++current_pos.lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಆದರೆ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (--current_pos.lnum < 1)
	    {
		ಆದರೆ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    looped_around = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;

	    }
	}
	current_pos.ಲಂಬಸಾಲು = 0;
    } ಆಗ (TRUE);

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    ಶೂನ್ಯ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (fuzmatch == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಪೂರ್ಣಾಂಕ
fuzzymatches_ಗಮ್ಯ_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
	ಪೂರ್ಣಾಂಕ		funcsort)
{
    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
	ಹೋಗು theend;

    *matches = ALLOC_MULT(char_u *, ಎಣಿಕೆ_2);
    ಆದರೆ (*matches == ಖಾಲಿ)
    {
	fuzmatch_str_free(fuzmatch, ಎಣಿಕೆ_2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Sort the list by the descending order of the match score
    ಆದರೆ (funcsort)
	fuzzy_ಹೊಂದಿಕೆ_func_ವಿಂಗಡಿಸು((ಶೂನ್ಯ *)fuzmatch, (size_t)ಎಣಿಕೆ_2);
    ಇಲ್ಲದಿದ್ದರೆ
	fuzzy_ಹೊಂದಿಕೆ_str_ವಿಂಗಡಿಸು((ಶೂನ್ಯ *)fuzmatch, (size_t)ಎಣಿಕೆ_2);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊಂದಿಕೆ_MAX_LEN FUZZY_ಹೊಂದಿಕೆ_MAX_LEN

#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_GAP_LEADING -0.005
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_GAP_TRAILING -0.005
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_GAP_INNER -0.01
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಹೊಂದಿಕೆ_CONSECUTIVE 1.0
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಹೊಂದಿಕೆ_SLASH 0.9
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಹೊಂದಿಕೆ_ಶಬ್ದ 0.8
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಹೊಂದಿಕೆ_CAPITAL 0.7
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಹೊಂದಿಕೆ_DOT 0.6

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_ಹೊಂದಿಕೆ(char_u *needle, char_u *haystack)
{
    ಆದರೆ (!needle || !haystack || !*needle)
	ಹಿಂತಿರುಗಿ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ಆಗ (*n_ptr)
    {
	ಪೂರ್ಣಾಂಕ n_char = mb_ptr2char(n_ptr);
	ಪೂರ್ಣಾಂಕ n_ಊರ್ಧ್ವ = MB_TOUPPER(n_char);
	ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = FALSE;

	ಆಗ (*h_ptr)
	{
	    ಪೂರ್ಣಾಂಕ h_char = mb_ptr2char(h_ptr);
	    ಆದರೆ (h_char == n_char || h_char == n_ಊರ್ಧ್ವ)
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		ನಿಲ್ಲಿಸು;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ಆದರೆ (!ಸಿಕ್ಕಿತು)
	    ಹಿಂತಿರುಗಿ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ಹಿಂತಿರುಗಿ OK;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಹೊಂದಿಕೆ_struct
{
    ಪೂರ್ಣಾಂಕ needle_len;
    ಪೂರ್ಣಾಂಕ haystack_len;
    ಪೂರ್ಣಾಂಕ ಅಧೋ_needle[ಹೊಂದಿಕೆ_MAX_LEN];     // stores codepoints
    ಪೂರ್ಣಾಂಕ ಅಧೋ_haystack[ಹೊಂದಿಕೆ_MAX_LEN];   // stores codepoints
    score_t ಹೊಂದಿಕೆ_bonus[ಹೊಂದಿಕೆ_MAX_LEN];
} ಹೊಂದಿಕೆ_struct;

#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಶಬ್ದ_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಮಾರ್ಗ_SEP(c) ((c) == '/')
#ವ್ಯಾಖ್ಯಾನಿಸು IS_DOT(c)      ((c) == '.')

    ಸ್ಥಿರ score_t
compute_bonus_codepoint(ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_c, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ಆದರೆ (IS_ಮಾರ್ಗ_SEP(ಕೊನೆಯ_c))
	    ಹಿಂತಿರುಗಿ SCORE_ಹೊಂದಿಕೆ_SLASH;
	ಆದರೆ (IS_ಶಬ್ದ_SEP(ಕೊನೆಯ_c))
	    ಹಿಂತಿರುಗಿ SCORE_ಹೊಂದಿಕೆ_ಶಬ್ದ;
	ಆದರೆ (IS_DOT(ಕೊನೆಯ_c))
	    ಹಿಂತಿರುಗಿ SCORE_ಹೊಂದಿಕೆ_DOT;
	ಆದರೆ (MB_ISUPPER(c) && MB_ISLOWER(ಕೊನೆಯ_c))
	    ಹಿಂತಿರುಗಿ SCORE_ಹೊಂದಿಕೆ_CAPITAL;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_ಹೊಂದಿಕೆ_struct(ಹೊಂದಿಕೆ_struct *ಹೊಂದಿಕೆ, char_u *needle, char_u *haystack)
{
    ಪೂರ್ಣಾಂಕ i = 0;
    char_u *p = needle;
    ಆಗ (*p != NUL && i < ಹೊಂದಿಕೆ_MAX_LEN)
    {
	ಪೂರ್ಣಾಂಕ c = mb_ptr2char(p);
	ಹೊಂದಿಕೆ->ಅಧೋ_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    ಹೊಂದಿಕೆ->needle_len = i;

    i = 0;
    p = haystack;
    ಪೂರ್ಣಾಂಕ prev_c = '/';
    ಆಗ (*p != NUL && i < ಹೊಂದಿಕೆ_MAX_LEN)
    {
	ಪೂರ್ಣಾಂಕ c = mb_ptr2char(p);
	ಹೊಂದಿಕೆ->ಅಧೋ_haystack[i] = MB_TOLOWER(c);
	ಹೊಂದಿಕೆ->ಹೊಂದಿಕೆ_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    ಹೊಂದಿಕೆ->haystack_len = i;
}

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಹೊಂದಿಕೆ_ಅಡ್ಡಸಾಲು(ಸ್ಥಿರಾಂಕ ಹೊಂದಿಕೆ_struct *ಹೊಂದಿಕೆ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, score_t *curr_D,
	score_t *curr_M, ಸ್ಥಿರಾಂಕ score_t *ಕೊನೆಯ_D, ಸ್ಥಿರಾಂಕ score_t *ಕೊನೆಯ_M)
{
    ಪೂರ್ಣಾಂಕ n = ಹೊಂದಿಕೆ->needle_len;
    ಪೂರ್ಣಾಂಕ m = ಹೊಂದಿಕೆ->haystack_len;
    ಪೂರ್ಣಾಂಕ i = ಅಡ್ಡಸಾಲು;

    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *ಅಧೋ_needle = ಹೊಂದಿಕೆ->ಅಧೋ_needle;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *ಅಧೋ_haystack = ಹೊಂದಿಕೆ->ಅಧೋ_haystack;
    ಸ್ಥಿರಾಂಕ score_t *ಹೊಂದಿಕೆ_bonus = ಹೊಂದಿಕೆ->ಹೊಂದಿಕೆ_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < m; j++)
    {
	ಆದರೆ (ಅಧೋ_needle[i] == ಅಧೋ_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ಆದರೆ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + ಹೊಂದಿಕೆ_bonus[j];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + ಹೊಂದಿಕೆ_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_ಹೊಂದಿಕೆ_CONSECUTIVE);
	    }
	    prev_D = ಕೊನೆಯ_D[j];
	    prev_M = ಕೊನೆಯ_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    prev_D = ಕೊನೆಯ_D[j];
	    prev_M = ಕೊನೆಯ_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ಸ್ಥಿರ score_t
ಹೊಂದಿಕೆ_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ಆದರೆ (!needle || !haystack || !*needle)
	ಹಿಂತಿರುಗಿ SCORE_MIN;

    ಹೊಂದಿಕೆ_struct ಹೊಂದಿಕೆ;
    ಸಿದ್ಧತೆ_ಹೊಂದಿಕೆ_struct(&ಹೊಂದಿಕೆ, needle, haystack);

    ಪೂರ್ಣಾಂಕ n = ಹೊಂದಿಕೆ.needle_len;
    ಪೂರ್ಣಾಂಕ m = ಹೊಂದಿಕೆ.haystack_len;

    ಆದರೆ (m > ಹೊಂದಿಕೆ_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ಹಿಂತಿರುಗಿ SCORE_MIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n; i++)
	{
	    ಆದರೆ (ಹೊಂದಿಕೆ.ಅಧೋ_needle[i] != ಹೊಂದಿಕೆ.ಅಧೋ_haystack[i])
	    {
		equal = false;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (equal)
	{
	    ಆದರೆ (positions)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ಹಿಂತಿರುಗಿ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ಆದರೆ ((size_t)n > (ಗಾತ್ರ_MAX / ಗಾತ್ರ(score_t)) / ಹೊಂದಿಕೆ_MAX_LEN / 2)
	ಹಿಂತಿರುಗಿ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *ಖಂಡ = (score_t*)alloc(ಗಾತ್ರ(score_t) * ಹೊಂದಿಕೆ_MAX_LEN * n * 2);
    ಆದರೆ (!ಖಂಡ)
	ಹಿಂತಿರುಗಿ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[ಹೊಂದಿಕೆ_MAX_LEN] = (score_t(*)[ಹೊಂದಿಕೆ_MAX_LEN])ಖಂಡ;
    score_t (*M)[ಹೊಂದಿಕೆ_MAX_LEN] = (score_t(*)[ಹೊಂದಿಕೆ_MAX_LEN])(ಖಂಡ
							+ ಹೊಂದಿಕೆ_MAX_LEN * n);

    ಹೊಂದಿಕೆ_ಅಡ್ಡಸಾಲು(&ಹೊಂದಿಕೆ, 0, D[0], M[0], D[0], M[0]);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < n; i++)
	ಹೊಂದಿಕೆ_ಅಡ್ಡಸಾಲು(&ಹೊಂದಿಕೆ, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ಆದರೆ (positions)
    {
	ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_required = 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = n - 1, j = m - 1; i >= 0; i--)
	{
	    ಗಾಗಿ (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ಆದರೆ (D[i][j] != SCORE_MIN &&
			(ಹೊಂದಿಕೆ_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    ಹೊಂದಿಕೆ_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_ಹೊಂದಿಕೆ_CONSECUTIVE;
		    positions[i] = j--;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
    }

    score_t ಫಲಿತಾಂಶ = M[n - 1][m - 1];
    vim_free(ಖಂಡ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
