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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PROTO)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fuzzy_match_item_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);
ସ୍ଥିର ଶୂନ୍ୟ fuzzy_match_in_list(list_T *l, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା matchseq, char_u *ଚାବି, callback_T *item_cb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retmatchpos, list_T *fmatchlist, long max_matches);
ସ୍ଥିର ଶୂନ୍ୟ do_fuzzymatch(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retmatchpos);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fuzzy_match_str_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);
ସ୍ଥିର ଶୂନ୍ୟ fuzzy_match_str_sort(fuzmatch_str_T *fm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fuzzy_match_func_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);
ସ୍ଥିର ଶୂନ୍ୟ fuzzy_match_func_sort(fuzmatch_str_T *fm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଦ୍ୱିଗୁଣ score_t;
ସ୍ଥିର score_t match_positions(char_u *needle, char_u *haystack, int_u *positions);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_match(char_u *needle, char_u *haystack);

#define SCORE_MAX INFINITY
#define SCORE_MIN (-INFINITY)
#define SCORE_SCALE 1000

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;		// used for stable sort
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemstr_allocated;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startpos;
} fuzzyItem_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match(
    char_u	*str,
    char_u	*pat_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matchseq,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*outScore,
    int_u	*matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		complete = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Matches = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_chars = 0;
    score_t	fzy_score;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    ଯଦି (save_pat == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    ଯେତେବେଳେ (TRUE)
    {
	ଯଦି (matchseq)
	    complete = TRUE;
	ଅନ୍ୟଥା
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    pat = p;
	    ଯେତେବେଳେ (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ଯଦି (has_mbyte)
		    MB_PTR_ADV(p);
		ଅନ୍ୟଥା
		    ++p;
	    }
	    ଯଦି (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ଯଦି (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ଯଦି (ଅଙ୍କ_Matches > maxMatches - pat_chars)
	{
	    ଅଙ୍କ_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    ଭାଙ୍ଗ;
	}

	score = FUZZY_SCORE_NONE;
	ଯଦି (has_match(pat, str))
	{
	    fzy_score = match_positions(pat, str, matches + ଅଙ୍କ_Matches);
	    ଯଦି (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? INT_MAX
		    : (fzy_score < 0) ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ଯଦି (score == FUZZY_SCORE_NONE)
	{
	    ଅଙ୍କ_Matches = 0;
	    *outScore = FUZZY_SCORE_NONE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (score > 0 && *outScore > INT_MAX - score)
	    *outScore = INT_MAX;
	ଅନ୍ୟଥା ଯଦି (score < 0 && *outScore < INT_MIN + 1 - score)
	    *outScore = INT_MIN + 1;
	ଅନ୍ୟଥା
	    *outScore += score;

	ଅଙ୍କ_Matches += pat_chars;

	ଯଦି (complete || ଅଙ୍କ_Matches >= maxMatches)
	    ଭାଙ୍ଗ;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    ଫେରନ୍ତୁ ଅଙ୍କ_Matches != 0;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match_item_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v1 = ((fuzzyItem_T *)s1)->score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v2 = ((fuzzyItem_T *)s2)->score;

    ଯଦି (v1 == v2)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzyItem_T *)s1)->pat;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା startpos = ((fuzzyItem_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzyItem_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzyItem_T *)s2)->itemstr + startpos, patlen) == 0;

	ଯଦି (exact_match1 == exact_match2)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1 = ((fuzzyItem_T *)s1)->idx;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx2 = ((fuzzyItem_T *)s2)->idx;
	    ଫେରନ୍ତୁ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ଅନ୍ୟଥା ଯଦି (exact_match2)
	    ଫେରନ୍ତୁ 1;
	ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ v1 > v2 ? -1 : 1;
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
    ସ୍ଥିର ଶୂନ୍ୟ
fuzzy_match_in_list(
    list_T	*l,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matchseq,
    char_u	*ଚାବି,
    callback_T	*item_cb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retmatchpos,
    list_T	*fmatchlist,
    long	max_matches)
{
    long	    len;
    fuzzyItem_T	    *ବସ୍ତୁଗୁଡ଼ିକ = ଖାଲି;
    listitem_T	    *li;
    long	    match_ଗଣନା = 0;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];

    len = list_len(l);
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ;
    ଯଦି (max_matches > 0 && len > max_matches)
	len = max_matches;

    ବସ୍ତୁଗୁଡ଼ିକ = ALLOC_CLEAR_MULT(fuzzyItem_T, len);
    ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score;
	char_u		*itemstr = ଖାଲି;
	char_u		*itemstr_copy = ଖାଲି;
	typval_T	rettv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemstr_allocate = FALSE;
	list_T		*match_positions = ଖାଲି;

	ଯଦି (max_matches > 0 && match_ଗଣନା >= max_matches)
	    ଭାଙ୍ଗ;

	rettv.v_type = VAR_UNKNOWN;
	ଯଦି (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ଅନ୍ୟଥା ଯଦି (li->li_tv.v_type == VAR_DICT
				&& (ଚାବି != ଖାଲି || item_cb->cb_name != ଖାଲି))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ଯଦି (ଚାବି != ଖାଲି)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (ଅକ୍ଷର *)ଚାବି, FALSE);
	    ଅନ୍ୟଥା
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ଯଦି (call_callback(item_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ଯଦି (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ଯଦି (itemstr != ଖାଲି
		&& fuzzy_match(itemstr, str, matchseq, &score, matches,
						FUZZY_MATCH_MAX_LEN))
	{
	    ଯଦି (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ଯଦି (itemstr_copy == ଖାଲି)
		{
		    clear_tv(&rettv);
		    ଜାରି;
		}
	    }
	    ଅନ୍ୟଥା
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ଯଦି (retmatchpos)
	    {
		match_positions = list_alloc();
		ଯଦି (match_positions == ଖାଲି)
		{
		    ଯଦି (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ଜାରି;
		}

		// Fill position information
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j = 0;
		char_u	*p = str;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	success = TRUE;

		ଯେତେବେଳେ (*p != NUL && j < FUZZY_MATCH_MAX_LEN && success)
		{
		    ଯଦି (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ଯଦି (list_append_ସଂଖ୍ୟା(match_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    ଭାଙ୍ଗ;
			}
			j++;
		    }
		    ଯଦି (has_mbyte)
			MB_PTR_ADV(p);
		    ଅନ୍ୟଥା
			++p;
		}

		ଯଦି (!success)
		{
		    list_free(match_positions);
		    ଯଦି (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    ଜାରି;
		}
	    }
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].idx = match_ଗଣନା;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].item = li;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].score = score;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].pat = str;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].startpos = matches[0];
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].itemstr = itemstr_copy;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].itemstr_allocated = itemstr_allocate;
	    ବସ୍ତୁଗୁଡ଼ିକ[match_ଗଣନା].lmatchpos = match_positions;

	    ++match_ଗଣନା;
	}
	clear_tv(&rettv);
    }

    ଯଦି (match_ଗଣନା > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((ଶୂନ୍ୟ *)ବସ୍ତୁଗୁଡ଼ିକ, (size_t)match_ଗଣନା, ଆକାର(fuzzyItem_T),
		fuzzy_match_item_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ଯଦି (retmatchpos)
	{
	    li = list_find(fmatchlist, 0);
	    ଯଦି (li == ଖାଲି || li->li_tv.vval.v_list == ଖାଲି)
		ଯାଅ done;
	    retlist = li->li_tv.vval.v_list;
	}
	ଅନ୍ୟଥା
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < match_ଗଣନା; i++)
	{
	    ଯଦି (list_append_tv(retlist, &ବସ୍ତୁଗୁଡ଼ିକ[i].item->li_tv) == FAIL)
		ଯାଅ done;
	}

	// next copy the list of matching positions
	ଯଦି (retmatchpos)
	{
	    li = list_find(fmatchlist, -2);
	    ଯଦି (li == ଖାଲି || li->li_tv.vval.v_list == ଖାଲି)
		ଯାଅ done;
	    retlist = li->li_tv.vval.v_list;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < match_ଗଣନା; i++)
	    {
		ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[i].lmatchpos != ଖାଲି)
		{
		    ଯଦି (list_append_list(retlist, ବସ୍ତୁଗୁଡ଼ିକ[i].lmatchpos) == OK)
			ବସ୍ତୁଗୁଡ଼ିକ[i].lmatchpos = ଖାଲି;
		    ଅନ୍ୟଥା
			ଯାଅ done;

		}
	    }

	    // copy the matching scores
	    li = list_find(fmatchlist, -1);
	    ଯଦି (li == ଖାଲି || li->li_tv.vval.v_list == ଖାଲି)
		ଯାଅ done;
	    retlist = li->li_tv.vval.v_list;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < match_ଗଣନା; i++)
	    {
		ଯଦି (list_append_ସଂଖ୍ୟା(retlist, ବସ୍ତୁଗୁଡ଼ିକ[i].score) == FAIL)
		    ଯାଅ done;
	    }
	}
    }

done:
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < match_ଗଣନା; i++)
    {
	ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[i].itemstr_allocated)
	    vim_free(ବସ୍ତୁଗୁଡ଼ିକ[i].itemstr);

	ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[i].lmatchpos)
	    list_free(ବସ୍ତୁଗୁଡ଼ିକ[i].lmatchpos);
    }
    vim_free(ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retmatchpos)
{
    callback_T	cb;
    char_u	*ଚାବି = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matchseq = FALSE;
    long	max_matches = 0;

    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    ଯଦି (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ଖାଲି)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	ଫେରନ୍ତୁ;
    }
    ଯଦି (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ଖାଲି)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ଯଦି (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ଯଦି ((di = dict_find(d, (char_u *)"key", -1)) != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ଖାଲି
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "key",
						    tv_get_string(&di->di_tv));
		ଫେରନ୍ତୁ;
	    }
	    ଚାବି = tv_get_string(&di->di_tv);
	}
	ଅନ୍ୟଥା ଯଦି ((di = dict_find(d, (char_u *)"text_cb", -1)) != ଖାଲି)
	{
	    cb = get_callback(&di->di_tv);
	    ଯଦି (cb.cb_name == ଖାଲି)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "text_cb");
		ଫେରନ୍ତୁ;
	    }
	}

	ଯଦି ((di = dict_find(d, (char_u *)"limit", -1)) != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_ସଂଖ୍ୟା)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "limit");
		ଫେରନ୍ତୁ;
	    }
	    max_matches = (long)tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, ଖାଲି);
	}

	ଯଦି (dict_has_ଚାବି(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ଯଦି (ret == FAIL)
	ଯାଅ done;
    ଯଦି (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଯାଅ done;
	ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ଯାଅ done;
	}
	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଯାଅ done;
	ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ଯାଅ done;
	}
	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଯାଅ done;
	ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ଯାଅ done;
	}
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, ଚାବି, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    ଶୂନ୍ୟ
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    ଶୂନ୍ୟ
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#endif

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match_str_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v1 = ((fuzmatch_str_T *)s1)->score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v2 = ((fuzmatch_str_T *)s2)->score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ଯଦି (v1 == v2)
	ଫେରନ୍ତୁ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fuzzy_match_str_sort(fuzmatch_str_T *fm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz)
{
    // Sort the list by the descending order of the match score
    qsort((ଶୂନ୍ୟ *)fm, (size_t)sz, ଆକାର(fuzmatch_str_T),
	    fuzzy_match_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match_func_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v1 = ((fuzmatch_str_T *)s1)->score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		v2 = ((fuzmatch_str_T *)s2)->score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx1 = ((fuzmatch_str_T *)s1)->idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ଯଦି (*str1 != '<' && *str2 == '<')
	ଫେରନ୍ତୁ -1;
    ଯଦି (*str1 == '<' && *str2 != '<')
	ଫେରନ୍ତୁ 1;
    ଯଦି (v1 == v2)
	ଫେରନ୍ତୁ idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fuzzy_match_func_sort(fuzmatch_str_T *fm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz)
{
    // Sort the list by the descending order of the match score
    qsort((ଶୂନ୍ୟ *)fm, (size_t)sz, ଆକାର(fuzmatch_str_T),
		fuzzy_match_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match_str(char_u *str, char_u *pat)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_MATCH_MAX_LEN];

    ଯଦି (str == ଖାଲି || pat == ଖାଲି)
	ଫେରନ୍ତୁ score;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				ଆକାର(matchpos) / ଆକାର(matchpos[0]));

    ଫେରନ୍ତୁ score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_match_str_with_pos(char_u *str UNUSED, char_u *pat UNUSED)
{
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    score = FUZZY_SCORE_NONE;
    garray_T	    *match_positions = ଖାଲି;
    int_u	    matches[FUZZY_MATCH_MAX_LEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    j = 0;

    ଯଦି (str == ଖାଲି || pat == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    match_positions = ALLOC_ONE(garray_T);
    ଯଦି (match_positions == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_init2(match_positions, ଆକାର(int_u), 10);

    ଯଦି (!fuzzy_match(str, pat, FALSE, &score, matches, FUZZY_MATCH_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(match_positions);
	vim_free(match_positions);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ଯଦି (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ଯଦି (ga_grow(match_positions, 1) == FAIL)
	    {
		ga_clear(match_positions);
		vim_free(match_positions);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ((int_u *)match_positions->ga_data)[match_positions->ga_len] =
								    matches[j];
	    match_positions->ga_len++;
	    j++;
	}
    }

    ଫେରନ୍ତୁ match_positions;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzy_match_str_in_ଧାଡ଼ି(
    char_u	**ptr,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*len,
    pos_T	*current_pos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*end = ଖାଲି;
    char_u	*start = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;
    ଅକ୍ଷର	save_end;
    char_u	*ଧାଡ଼ି_end = ଖାଲି;

    ଯଦି (str == ଖାଲି || pat == ଖାଲି)
	ଫେରନ୍ତୁ found;
    ଧାଡ଼ି_end = find_ଧାଡ଼ି_end(str);

    ଯେତେବେଳେ (str < ଧାଡ଼ି_end)
    {
	// Skip non-word characters
	start = find_word_start(str);
	ଯଦି (*start == NUL)
	    ଭାଙ୍ଗ;
	end = find_word_end(start);

	// Extract the word from start to end
	save_end = *end;
	*end = NUL;

	// Perform fuzzy match
	*score = fuzzy_match_str(start, pat);
	*end = save_end;

	ଯଦି (*score != FUZZY_SCORE_NONE)
	{
	    *len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - start);
	    found = TRUE;
	    *ptr = start;
	    ଯଦି (current_pos)
		current_pos->col += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - strBegin);
	    ଭାଙ୍ଗ;
	}

	// Move to the end of the current word for the next iteration
	str = end;
	// Ensure we continue searching after the current word
	ଯେତେବେଳେ (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ଯଦି (!found)
	*ptr = ଧାଡ଼ି_end;

    ଫେରନ୍ତୁ found;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଧାନ_for_fuzzy_match(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*pattern,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    pos_T	*start_pos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*len,
    char_u	**ptr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_new_match = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		looped_around = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whole_ଧାଡ଼ି = ctrl_x_mode_whole_ଧାଡ଼ି();

    ଯଦି (buf == curbuf)
	circly_end = *start_pos;
    ଅନ୍ୟଥା
    {
	circly_end.lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	circly_end.col = 0;
	circly_end.coladd = 0;
    }

    ଯଦି (whole_ଧାଡ଼ି && start_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    କର
    {

	// Check if looped around and back to start position
	ଯଦି (looped_around && (whole_ଧାଡ଼ି ? current_pos.lnum == circly_end.lnum
					: EQUAL_POS(current_pos, circly_end)))
	    ଭାଙ୍ଗ;

	// Ensure current_pos is valid
	ଯଦି (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ଯଦି (!whole_ଧାଡ଼ି)
		*ptr += current_pos.col;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ଯଦି (*ptr != ଖାଲି && **ptr != NUL)
	    {
		ଯଦି (!whole_ଧାଡ଼ି)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    found_new_match = fuzzy_match_str_in_ଧାଡ଼ି(ptr, pattern,
						    len, &current_pos, score);
		    ଯଦି (found_new_match)
		    {
			*pos = current_pos;
			ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା ଯଦି (looped_around && current_pos.lnum == circly_end.lnum)
			ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		{
		    char_u *ଧାଡ଼ି = *ptr;
		    char_u *p = skipwhite(ଧାଡ଼ି);
		    ଯଦି (fuzzy_match_str(p, pattern) != FUZZY_SCORE_NONE)
		    {
			found_new_match = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ml_get_buf_len(buf, current_pos.lnum) -
								(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
			ଭାଙ୍ଗ;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ଯଦି (dir == FORWARD)
	{
	    ଯଦି (++current_pos.lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		ଯଦି (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (--current_pos.lnum < 1)
	    {
		ଯଦି (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		    looped_around = TRUE;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;

	    }
	}
	current_pos.col = 0;
    } ଯେତେବେଳେ (TRUE);

    ଫେରନ୍ତୁ found_new_match;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    ଶୂନ୍ୟ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ଯଦି (fuzmatch == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		funcsort)
{
    ଯଦି (ଗଣନା_2 <= 0)
	ଯାଅ theend;

    *matches = ALLOC_MULT(char_u *, ଗଣନା_2);
    ଯଦି (*matches == ଖାଲି)
    {
	fuzmatch_str_free(fuzmatch, ଗଣନା_2);
	ଫେରନ୍ତୁ FAIL;
    }

    // Sort the list by the descending order of the match score
    ଯଦି (funcsort)
	fuzzy_match_func_sort((ଶୂନ୍ୟ *)fuzmatch, (size_t)ଗଣନା_2);
    ଅନ୍ୟଥା
	fuzzy_match_str_sort((ଶୂନ୍ୟ *)fuzmatch, (size_t)ଗଣନା_2);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    ଫେରନ୍ତୁ OK;
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_match(char_u *needle, char_u *haystack)
{
    ଯଦି (!needle || !haystack || !*needle)
	ଫେରନ୍ତୁ FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    ଯେତେବେଳେ (*n_ptr)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_char = mb_ptr2char(n_ptr);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_upper = MB_TOUPPER(n_char);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = FALSE;

	ଯେତେବେଳେ (*h_ptr)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h_char = mb_ptr2char(h_ptr);
	    ଯଦି (h_char == n_char || h_char == n_upper)
	    {
		found = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		ଭାଙ୍ଗ;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ଯଦି (!found)
	    ଫେରନ୍ତୁ FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    ଫେରନ୍ତୁ OK;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା match_struct
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା needle_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା haystack_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lower_needle[MATCH_MAX_LEN];     // stores codepoints
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lower_haystack[MATCH_MAX_LEN];   // stores codepoints
    score_t match_bonus[MATCH_MAX_LEN];
} match_struct;

#define IS_WORD_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#define IS_PATH_SEP(c) ((c) == '/')
#define IS_DOT(c)      ((c) == '.')

    ସ୍ଥିର score_t
compute_bonus_codepoint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ଯଦି (IS_PATH_SEP(last_c))
	    ଫେରନ୍ତୁ SCORE_MATCH_SLASH;
	ଯଦି (IS_WORD_SEP(last_c))
	    ଫେରନ୍ତୁ SCORE_MATCH_WORD;
	ଯଦି (IS_DOT(last_c))
	    ଫେରନ୍ତୁ SCORE_MATCH_DOT;
	ଯଦି (MB_ISUPPER(c) && MB_ISLOWER(last_c))
	    ଫେରନ୍ତୁ SCORE_MATCH_CAPITAL;
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
setup_match_struct(match_struct *match, char_u *needle, char_u *haystack)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
    char_u *p = needle;
    ଯେତେବେଳେ (*p != NUL && i < MATCH_MAX_LEN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = mb_ptr2char(p);
	match->lower_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    match->needle_len = i;

    i = 0;
    p = haystack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_c = '/';
    ଯେତେବେଳେ (*p != NUL && i < MATCH_MAX_LEN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = mb_ptr2char(p);
	match->lower_haystack[i] = MB_TOLOWER(c);
	match->match_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    match->haystack_len = i;
}

    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
match_row(ଅଚଳ match_struct *match, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, score_t *curr_D,
	score_t *curr_M, ଅଚଳ score_t *last_D, ଅଚଳ score_t *last_M)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = match->needle_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = match->haystack_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = row;

    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lower_needle = match->lower_needle;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lower_haystack = match->lower_haystack;
    ଅଚଳ score_t *match_bonus = match->match_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < m; j++)
    {
	ଯଦି (lower_needle[i] == lower_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ଯଦି (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + match_bonus[j];
	    }
	    ଅନ୍ୟଥା ଯଦି (j)
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
	ଅନ୍ୟଥା
	{
	    prev_D = last_D[j];
	    prev_M = last_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ସ୍ଥିର score_t
match_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ଯଦି (!needle || !haystack || !*needle)
	ଫେରନ୍ତୁ SCORE_MIN;

    match_struct match;
    setup_match_struct(&match, needle, haystack);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = match.needle_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = match.haystack_len;

    ଯଦି (m > MATCH_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	ଫେରନ୍ତୁ SCORE_MIN;
    }
    ଅନ୍ୟଥା ଯଦି (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n; i++)
	{
	    ଯଦି (match.lower_needle[i] != match.lower_haystack[i])
	    {
		equal = false;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (equal)
	{
	    ଯଦି (positions)
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    ଫେରନ୍ତୁ SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ଯଦି ((size_t)n > (SIZE_MAX / ଆକାର(score_t)) / MATCH_MAX_LEN / 2)
	ଫେରନ୍ତୁ SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *block = (score_t*)alloc(ଆକାର(score_t) * MATCH_MAX_LEN * n * 2);
    ଯଦି (!block)
	ଫେରନ୍ତୁ SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])block;
    score_t (*M)[MATCH_MAX_LEN] = (score_t(*)[MATCH_MAX_LEN])(block
							+ MATCH_MAX_LEN * n);

    match_row(&match, 0, D[0], M[0], D[0], M[0]);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < n; i++)
	match_row(&match, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ଯଦି (positions)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_required = 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = n - 1, j = m - 1; i >= 0; i--)
	{
	    ପାଇଁ (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ଯଦି (D[i][j] != SCORE_MIN &&
			(match_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    match_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_MATCH_CONSECUTIVE;
		    positions[i] = j--;
		    ଭାଙ୍ଗ;
		}
	    }
	}
    }

    score_t result = M[n - 1][m - 1];
    vim_free(block);
    ଫେରନ୍ତୁ result;
}
