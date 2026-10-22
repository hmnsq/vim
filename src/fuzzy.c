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
#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PROTO)
స్థిర పూర్ణాంకం fuzzy_సరిపోలిక_అంశం_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);
స్థిర శూన్య fuzzy_సరిపోలిక_లోపల_list(list_T *l, char_u *str, పూర్ణాంకం matchseq, char_u *కీలకం, callback_T *అంశం_cb, పూర్ణాంకం retmatchpos, list_T *fmatchlist, దీర్ఘ max_matches);
స్థిర శూన్య do_fuzzymatch(typval_T *argvars, typval_T *rettv, పూర్ణాంకం retmatchpos);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం fuzzy_సరిపోలిక_str_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);
స్థిర శూన్య fuzzy_సరిపోలిక_str_క్రమీకరించు(fuzmatch_str_T *fm, పూర్ణాంకం sz);
స్థిర పూర్ణాంకం fuzzy_సరిపోలిక_func_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);
స్థిర శూన్య fuzzy_సరిపోలిక_func_క్రమీకరించు(fuzmatch_str_T *fm, పూర్ణాంకం sz);

రకనిర్వచనం ద్విగుణ score_t;
స్థిర score_t సరిపోలిక_positions(char_u *needle, char_u *haystack, int_u *positions);
స్థిర పూర్ణాంకం has_సరిపోలిక(char_u *needle, char_u *haystack);

#నిర్వచించు SCORE_MAX INFINITY
#నిర్వచించు SCORE_MIN (-INFINITY)
#నిర్వచించు SCORE_SCALE 1000

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		idx;		// used for stable sort
    listitem_T	*అంశం;
    పూర్ణాంకం		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    పూర్ణాంకం		itemstr_allocated;
    పూర్ణాంకం		startpos;
} fuzzy_అంశం_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    పూర్ణాంకం
fuzzy_సరిపోలిక(
    char_u	*str,
    char_u	*pat_arg,
    పూర్ణాంకం		matchseq,
    పూర్ణాంకం		*బహిర్గతం_Score,
    int_u	*matches,
    పూర్ణాంకం		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    పూర్ణాంకం		complete = FALSE;
    పూర్ణాంకం		score = 0;
    పూర్ణాంకం		అంకె_Matches = 0;
    పూర్ణాంకం		pat_chars = 0;
    score_t	fzy_score;

    *బహిర్గతం_Score = 0;

    save_pat = vim_strsave(pat_arg);
    ఒకవేళ (save_pat == ఖాళీ)
	తిరిగి FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    అయితే (TRUE)
    {
	ఒకవేళ (matchseq)
	    complete = TRUE;
	లేకపోతే
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    pat = p;
	    అయితే (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		ఒకవేళ (has_mbyte)
		    MB_PTR_ADV(p);
		లేకపోతే
		    ++p;
	    }
	    ఒకవేళ (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	ఒకవేళ (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	ఒకవేళ (అంకె_Matches > maxMatches - pat_chars)
	{
	    అంకె_Matches = 0;
	    *బహిర్గతం_Score = FUZZY_SCORE_NONE;
	    ఆపు;
	}

	score = FUZZY_SCORE_NONE;
	ఒకవేళ (has_సరిపోలిక(pat, str))
	{
	    fzy_score = సరిపోలిక_positions(pat, str, matches + అంకె_Matches);
	    ఒకవేళ (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? గరిష్ఠ_పూర్ణాంకం
		    : (fzy_score < 0) ? (పూర్ణాంకం)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (పూర్ణాంకం)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	ఒకవేళ (score == FUZZY_SCORE_NONE)
	{
	    అంకె_Matches = 0;
	    *బహిర్గతం_Score = FUZZY_SCORE_NONE;
	    ఆపు;
	}

	ఒకవేళ (score > 0 && *బహిర్గతం_Score > గరిష్ఠ_పూర్ణాంకం - score)
	    *బహిర్గతం_Score = గరిష్ఠ_పూర్ణాంకం;
	లేకపోతే ఒకవేళ (score < 0 && *బహిర్గతం_Score < INT_MIN + 1 - score)
	    *బహిర్గతం_Score = INT_MIN + 1;
	లేకపోతే
	    *బహిర్గతం_Score += score;

	అంకె_Matches += pat_chars;

	ఒకవేళ (complete || అంకె_Matches >= maxMatches)
	    ఆపు;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    తిరిగి అంకె_Matches != 0;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    స్థిర పూర్ణాంకం
fuzzy_సరిపోలిక_అంశం_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    పూర్ణాంకం		v1 = ((fuzzy_అంశం_T *)s1)->score;
    పూర్ణాంకం		v2 = ((fuzzy_అంశం_T *)s2)->score;

    ఒకవేళ (v1 == v2)
    {
	పూర్ణాంకం exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_అంశం_T *)s1)->pat;
	పూర్ణాంకం patlen = (పూర్ణాంకం)STRLEN(pat);
	పూర్ణాంకం startpos = ((fuzzy_అంశం_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_అంశం_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_అంశం_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_అంశం_T *)s2)->itemstr + startpos, patlen) == 0;

	ఒకవేళ (exact_match1 == exact_match2)
	{
	    పూర్ణాంకం idx1 = ((fuzzy_అంశం_T *)s1)->idx;
	    పూర్ణాంకం idx2 = ((fuzzy_అంశం_T *)s2)->idx;
	    తిరిగి idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	లేకపోతే ఒకవేళ (exact_match2)
	    తిరిగి 1;
	తిరిగి -1;
    }
    లేకపోతే
	తిరిగి v1 > v2 ? -1 : 1;
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
    స్థిర శూన్య
fuzzy_సరిపోలిక_లోపల_list(
    list_T	*l,
    char_u	*str,
    పూర్ణాంకం		matchseq,
    char_u	*కీలకం,
    callback_T	*అంశం_cb,
    పూర్ణాంకం		retmatchpos,
    list_T	*fmatchlist,
    దీర్ఘ	max_matches)
{
    దీర్ఘ	    len;
    fuzzy_అంశం_T	    *వస్తువులు = ఖాళీ;
    listitem_T	    *li;
    దీర్ఘ	    సరిపోలిక_లెక్క = 0;
    int_u	    matches[FUZZY_సరిపోలిక_MAX_LEN];

    len = list_len(l);
    ఒకవేళ (len == 0)
	తిరిగి;
    ఒకవేళ (max_matches > 0 && len > max_matches)
	len = max_matches;

    వస్తువులు = ALLOC_CLEAR_MULT(fuzzy_అంశం_T, len);
    ఒకవేళ (వస్తువులు == ఖాళీ)
	తిరిగి;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	పూర్ణాంకం		score;
	char_u		*itemstr = ఖాళీ;
	char_u		*itemstr_copy = ఖాళీ;
	typval_T	rettv;
	పూర్ణాంకం		itemstr_allocate = FALSE;
	list_T		*సరిపోలిక_positions = ఖాళీ;

	ఒకవేళ (max_matches > 0 && సరిపోలిక_లెక్క >= max_matches)
	    ఆపు;

	rettv.v_type = VAR_UNKNOWN;
	ఒకవేళ (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	లేకపోతే ఒకవేళ (li->li_tv.v_type == VAR_DICT
				&& (కీలకం != ఖాళీ || అంశం_cb->cb_name != ఖాళీ))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    ఒకవేళ (కీలకం != ఖాళీ)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (అక్షరం *)కీలకం, FALSE);
	    లేకపోతే
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		ఒకవేళ (call_callback(అంశం_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    ఒకవేళ (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	ఒకవేళ (itemstr != ఖాళీ
		&& fuzzy_సరిపోలిక(itemstr, str, matchseq, &score, matches,
						FUZZY_సరిపోలిక_MAX_LEN))
	{
	    ఒకవేళ (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		ఒకవేళ (itemstr_copy == ఖాళీ)
		{
		    clear_tv(&rettv);
		    కొనసాగు;
		}
	    }
	    లేకపోతే
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    ఒకవేళ (retmatchpos)
	    {
		సరిపోలిక_positions = list_alloc();
		ఒకవేళ (సరిపోలిక_positions == ఖాళీ)
		{
		    ఒకవేళ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    కొనసాగు;
		}

		// Fill position information
		పూర్ణాంకం	j = 0;
		char_u	*p = str;
		పూర్ణాంకం	success = TRUE;

		అయితే (*p != NUL && j < FUZZY_సరిపోలిక_MAX_LEN && success)
		{
		    ఒకవేళ (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			ఒకవేళ (list_append_సంఖ్య(సరిపోలిక_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    ఆపు;
			}
			j++;
		    }
		    ఒకవేళ (has_mbyte)
			MB_PTR_ADV(p);
		    లేకపోతే
			++p;
		}

		ఒకవేళ (!success)
		{
		    list_free(సరిపోలిక_positions);
		    ఒకవేళ (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    కొనసాగు;
		}
	    }
	    వస్తువులు[సరిపోలిక_లెక్క].idx = సరిపోలిక_లెక్క;
	    వస్తువులు[సరిపోలిక_లెక్క].అంశం = li;
	    వస్తువులు[సరిపోలిక_లెక్క].score = score;
	    వస్తువులు[సరిపోలిక_లెక్క].pat = str;
	    వస్తువులు[సరిపోలిక_లెక్క].startpos = matches[0];
	    వస్తువులు[సరిపోలిక_లెక్క].itemstr = itemstr_copy;
	    వస్తువులు[సరిపోలిక_లెక్క].itemstr_allocated = itemstr_allocate;
	    వస్తువులు[సరిపోలిక_లెక్క].lmatchpos = సరిపోలిక_positions;

	    ++సరిపోలిక_లెక్క;
	}
	clear_tv(&rettv);
    }

    ఒకవేళ (సరిపోలిక_లెక్క > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((శూన్య *)వస్తువులు, (size_t)సరిపోలిక_లెక్క, పరిమాణం(fuzzy_అంశం_T),
		fuzzy_సరిపోలిక_అంశం_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	ఒకవేళ (retmatchpos)
	{
	    li = list_కనుగొను(fmatchlist, 0);
	    ఒకవేళ (li == ఖాళీ || li->li_tv.vval.v_list == ఖాళీ)
		వెళ్ళు పూర్తి;
	    retlist = li->li_tv.vval.v_list;
	}
	లేకపోతే
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	కొరకు (పూర్ణాంకం i = 0; i < సరిపోలిక_లెక్క; i++)
	{
	    ఒకవేళ (list_append_tv(retlist, &వస్తువులు[i].అంశం->li_tv) == FAIL)
		వెళ్ళు పూర్తి;
	}

	// next copy the list of matching positions
	ఒకవేళ (retmatchpos)
	{
	    li = list_కనుగొను(fmatchlist, -2);
	    ఒకవేళ (li == ఖాళీ || li->li_tv.vval.v_list == ఖాళీ)
		వెళ్ళు పూర్తి;
	    retlist = li->li_tv.vval.v_list;

	    కొరకు (పూర్ణాంకం i = 0; i < సరిపోలిక_లెక్క; i++)
	    {
		ఒకవేళ (వస్తువులు[i].lmatchpos != ఖాళీ)
		{
		    ఒకవేళ (list_append_list(retlist, వస్తువులు[i].lmatchpos) == OK)
			వస్తువులు[i].lmatchpos = ఖాళీ;
		    లేకపోతే
			వెళ్ళు పూర్తి;

		}
	    }

	    // copy the matching scores
	    li = list_కనుగొను(fmatchlist, -1);
	    ఒకవేళ (li == ఖాళీ || li->li_tv.vval.v_list == ఖాళీ)
		వెళ్ళు పూర్తి;
	    retlist = li->li_tv.vval.v_list;
	    కొరకు (పూర్ణాంకం i = 0; i < సరిపోలిక_లెక్క; i++)
	    {
		ఒకవేళ (list_append_సంఖ్య(retlist, వస్తువులు[i].score) == FAIL)
		    వెళ్ళు పూర్తి;
	    }
	}
    }

పూర్తి:
    కొరకు (పూర్ణాంకం i = 0; i < సరిపోలిక_లెక్క; i++)
    {
	ఒకవేళ (వస్తువులు[i].itemstr_allocated)
	    vim_free(వస్తువులు[i].itemstr);

	ఒకవేళ (వస్తువులు[i].lmatchpos)
	    list_free(వస్తువులు[i].lmatchpos);
    }
    vim_free(వస్తువులు);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    స్థిర శూన్య
do_fuzzymatch(typval_T *argvars, typval_T *rettv, పూర్ణాంకం retmatchpos)
{
    callback_T	cb;
    char_u	*కీలకం = ఖాళీ;
    పూర్ణాంకం		ret;
    పూర్ణాంకం		matchseq = FALSE;
    దీర్ఘ	max_matches = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    CLEAR_సూచిక(&cb);

    // validate and get the arguments
    ఒకవేళ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ఖాళీ)
    {
	semsg(_(e_వాదం_యొక్క_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	తిరిగి;
    }
    ఒకవేళ (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[1]));
	తిరిగి;
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	ఒకవేళ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    తిరిగి;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"key", -1)) != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == ఖాళీ
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str_str), "key",
						    tv_get_string(&di->di_tv));
		తిరిగి;
	    }
	    కీలకం = tv_get_string(&di->di_tv);
	}
	లేకపోతే ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"text_cb", -1)) != ఖాళీ)
	{
	    cb = get_callback(&di->di_tv);
	    ఒకవేళ (cb.cb_name == ఖాళీ)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str), "text_cb");
		తిరిగి;
	    }
	}

	ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"limit", -1)) != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_సంఖ్య)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str), "limit");
		తిరిగి;
	    }
	    max_matches = (దీర్ఘ)tv_get_సంఖ్య_chk(&di->di_tv, ఖాళీ);
	}

	ఒకవేళ (dict_has_కీలకం(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    ఒకవేళ (ret == FAIL)
	వెళ్ళు పూర్తి;
    ఒకవేళ (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    వెళ్ళు పూర్తి;
	ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    వెళ్ళు పూర్తి;
	}
	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    వెళ్ళు పూర్తి;
	ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    వెళ్ళు పూర్తి;
	}
	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    వెళ్ళు పూర్తి;
	ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    వెళ్ళు పూర్తి;
	}
    }

    fuzzy_సరిపోలిక_లోపల_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, కీలకం, &cb, retmatchpos, rettv->vval.v_list, max_matches);

పూర్తి:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    శూన్య
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    శూన్య
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#అయితే_ముగింపు

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    స్థిర పూర్ణాంకం
fuzzy_సరిపోలిక_str_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    పూర్ణాంకం		v1 = ((fuzmatch_str_T *)s1)->score;
    పూర్ణాంకం		v2 = ((fuzmatch_str_T *)s2)->score;
    పూర్ణాంకం		idx1 = ((fuzmatch_str_T *)s1)->idx;
    పూర్ణాంకం		idx2 = ((fuzmatch_str_T *)s2)->idx;

    ఒకవేళ (v1 == v2)
	తిరిగి idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    లేకపోతే
	తిరిగి v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    స్థిర శూన్య
fuzzy_సరిపోలిక_str_క్రమీకరించు(fuzmatch_str_T *fm, పూర్ణాంకం sz)
{
    // Sort the list by the descending order of the match score
    qsort((శూన్య *)fm, (size_t)sz, పరిమాణం(fuzmatch_str_T),
	    fuzzy_సరిపోలిక_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    స్థిర పూర్ణాంకం
fuzzy_సరిపోలిక_func_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    పూర్ణాంకం		v1 = ((fuzmatch_str_T *)s1)->score;
    పూర్ణాంకం		v2 = ((fuzmatch_str_T *)s2)->score;
    పూర్ణాంకం		idx1 = ((fuzmatch_str_T *)s1)->idx;
    పూర్ణాంకం		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    ఒకవేళ (*str1 != '<' && *str2 == '<')
	తిరిగి -1;
    ఒకవేళ (*str1 == '<' && *str2 != '<')
	తిరిగి 1;
    ఒకవేళ (v1 == v2)
	తిరిగి idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    లేకపోతే
	తిరిగి v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    స్థిర శూన్య
fuzzy_సరిపోలిక_func_క్రమీకరించు(fuzmatch_str_T *fm, పూర్ణాంకం sz)
{
    // Sort the list by the descending order of the match score
    qsort((శూన్య *)fm, (size_t)sz, పరిమాణం(fuzmatch_str_T),
		fuzzy_సరిపోలిక_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    పూర్ణాంకం
fuzzy_సరిపోలిక_str(char_u *str, char_u *pat)
{
    పూర్ణాంకం		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_సరిపోలిక_MAX_LEN];

    ఒకవేళ (str == ఖాళీ || pat == ఖాళీ)
	తిరిగి score;

    fuzzy_సరిపోలిక(str, pat, TRUE, &score, matchpos,
				పరిమాణం(matchpos) / పరిమాణం(matchpos[0]));

    తిరిగి score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_సరిపోలిక_str_with_pos(char_u *str వాడని, char_u *pat వాడని)
{
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    పూర్ణాంకం		    score = FUZZY_SCORE_NONE;
    garray_T	    *సరిపోలిక_positions = ఖాళీ;
    int_u	    matches[FUZZY_సరిపోలిక_MAX_LEN];
    పూర్ణాంకం		    j = 0;

    ఒకవేళ (str == ఖాళీ || pat == ఖాళీ)
	తిరిగి ఖాళీ;

    సరిపోలిక_positions = ALLOC_ONE(garray_T);
    ఒకవేళ (సరిపోలిక_positions == ఖాళీ)
	తిరిగి ఖాళీ;
    ga_init2(సరిపోలిక_positions, పరిమాణం(int_u), 10);

    ఒకవేళ (!fuzzy_సరిపోలిక(str, pat, FALSE, &score, matches, FUZZY_సరిపోలిక_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(సరిపోలిక_positions);
	vim_free(సరిపోలిక_positions);
	తిరిగి ఖాళీ;
    }

    కొరకు (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	ఒకవేళ (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    ఒకవేళ (ga_grow(సరిపోలిక_positions, 1) == FAIL)
	    {
		ga_clear(సరిపోలిక_positions);
		vim_free(సరిపోలిక_positions);
		తిరిగి ఖాళీ;
	    }
	    ((int_u *)సరిపోలిక_positions->ga_దత్తాంశం)[సరిపోలిక_positions->ga_len] =
								    matches[j];
	    సరిపోలిక_positions->ga_len++;
	    j++;
	}
    }

    తిరిగి సరిపోలిక_positions;
#లేకపోతే
    తిరిగి ఖాళీ;
#అయితే_ముగింపు
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
    పూర్ణాంకం
fuzzy_సరిపోలిక_str_లోపల_పంక్తి(
    char_u	**ptr,
    char_u	*pat,
    పూర్ణాంకం		*len,
    pos_T	*current_pos,
    పూర్ణాంకం		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*చివర = ఖాళీ;
    char_u	*ప్రారంభం = ఖాళీ;
    పూర్ణాంకం		దొరికింది = FALSE;
    అక్షరం	save_చివర;
    char_u	*పంక్తి_చివర = ఖాళీ;

    ఒకవేళ (str == ఖాళీ || pat == ఖాళీ)
	తిరిగి దొరికింది;
    పంక్తి_చివర = కనుగొను_పంక్తి_చివర(str);

    అయితే (str < పంక్తి_చివర)
    {
	// Skip non-word characters
	ప్రారంభం = కనుగొను_మాట_ప్రారంభం(str);
	ఒకవేళ (*ప్రారంభం == NUL)
	    ఆపు;
	చివర = కనుగొను_మాట_చివర(ప్రారంభం);

	// Extract the word from start to end
	save_చివర = *చివర;
	*చివర = NUL;

	// Perform fuzzy match
	*score = fuzzy_సరిపోలిక_str(ప్రారంభం, pat);
	*చివర = save_చివర;

	ఒకవేళ (*score != FUZZY_SCORE_NONE)
	{
	    *len = (పూర్ణాంకం)(చివర - ప్రారంభం);
	    దొరికింది = TRUE;
	    *ptr = ప్రారంభం;
	    ఒకవేళ (current_pos)
		current_pos->నిలువు += (పూర్ణాంకం)(చివర - strBegin);
	    ఆపు;
	}

	// Move to the end of the current word for the next iteration
	str = చివర;
	// Ensure we continue searching after the current word
	అయితే (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    ఒకవేళ (!దొరికింది)
	*ptr = పంక్తి_చివర;

    తిరిగి దొరికింది;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    పూర్ణాంకం
శోధన_for_fuzzy_సరిపోలిక(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*నమూనా,
    పూర్ణాంకం		dir,
    pos_T	*ప్రారంభం_pos,
    పూర్ణాంకం		*len,
    char_u	**ptr,
    పూర్ణాంకం		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_చివర;
    పూర్ణాంకం		దొరికింది_new_సరిపోలిక = FALSE;
    పూర్ణాంకం		looped_around = FALSE;
    పూర్ణాంకం		సంపూర్ణ_పంక్తి = ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి();

    ఒకవేళ (buf == curbuf)
	circly_చివర = *ప్రారంభం_pos;
    లేకపోతే
    {
	circly_చివర.lnum = buf->b_ml.ml_పంక్తి_లెక్క;
	circly_చివర.నిలువు = 0;
	circly_చివర.coladd = 0;
    }

    ఒకవేళ (సంపూర్ణ_పంక్తి && ప్రారంభం_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    చేయి
    {

	// Check if looped around and back to start position
	ఒకవేళ (looped_around && (సంపూర్ణ_పంక్తి ? current_pos.lnum == circly_చివర.lnum
					: EQUAL_POS(current_pos, circly_చివర)))
	    ఆపు;

	// Ensure current_pos is valid
	ఒకవేళ (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_పంక్తి_లెక్క)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    ఒకవేళ (!సంపూర్ణ_పంక్తి)
		*ptr += current_pos.నిలువు;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    ఒకవేళ (*ptr != ఖాళీ && **ptr != NUL)
	    {
		ఒకవేళ (!సంపూర్ణ_పంక్తి)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    దొరికింది_new_సరిపోలిక = fuzzy_సరిపోలిక_str_లోపల_పంక్తి(ptr, నమూనా,
						    len, &current_pos, score);
		    ఒకవేళ (దొరికింది_new_సరిపోలిక)
		    {
			*pos = current_pos;
			ఆపు;
		    }
		    లేకపోతే ఒకవేళ (looped_around && current_pos.lnum == circly_చివర.lnum)
			ఆపు;
		}
		లేకపోతే
		{
		    char_u *పంక్తి = *ptr;
		    char_u *p = skipwhite(పంక్తి);
		    ఒకవేళ (fuzzy_సరిపోలిక_str(p, నమూనా) != FUZZY_SCORE_NONE)
		    {
			దొరికింది_new_సరిపోలిక = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (పూర్ణాంకం)ml_get_buf_len(buf, current_pos.lnum) -
								(పూర్ణాంకం)(p - పంక్తి);
			ఆపు;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	ఒకవేళ (dir == FORWARD)
	{
	    ఒకవేళ (++current_pos.lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	    {
		ఒకవేళ (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		లేకపోతే
		    ఆపు;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (--current_pos.lnum < 1)
	    {
		ఒకవేళ (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_పంక్తి_లెక్క;
		    looped_around = TRUE;
		}
		లేకపోతే
		    ఆపు;

	    }
	}
	current_pos.నిలువు = 0;
    } అయితే (TRUE);

    తిరిగి దొరికింది_new_సరిపోలిక;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    శూన్య
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, పూర్ణాంకం లెక్క)
{
    ఒకవేళ (fuzmatch == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    పూర్ణాంకం
fuzzymatches_గమ్యం_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	పూర్ణాంకం		లెక్క,
	పూర్ణాంకం		funcsort)
{
    ఒకవేళ (లెక్క <= 0)
	వెళ్ళు theend;

    *matches = ALLOC_MULT(char_u *, లెక్క);
    ఒకవేళ (*matches == ఖాళీ)
    {
	fuzmatch_str_free(fuzmatch, లెక్క);
	తిరిగి FAIL;
    }

    // Sort the list by the descending order of the match score
    ఒకవేళ (funcsort)
	fuzzy_సరిపోలిక_func_క్రమీకరించు((శూన్య *)fuzmatch, (size_t)లెక్క);
    లేకపోతే
	fuzzy_సరిపోలిక_str_క్రమీకరించు((శూన్య *)fuzmatch, (size_t)లెక్క);

    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    తిరిగి OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#నిర్వచించు సరిపోలిక_MAX_LEN FUZZY_సరిపోలిక_MAX_LEN

#నిర్వచించు SCORE_GAP_LEADING -0.005
#నిర్వచించు SCORE_GAP_TRAILING -0.005
#నిర్వచించు SCORE_GAP_INNER -0.01
#నిర్వచించు SCORE_సరిపోలిక_CONSECUTIVE 1.0
#నిర్వచించు SCORE_సరిపోలిక_SLASH 0.9
#నిర్వచించు SCORE_సరిపోలిక_మాట 0.8
#నిర్వచించు SCORE_సరిపోలిక_CAPITAL 0.7
#నిర్వచించు SCORE_సరిపోలిక_DOT 0.6

    స్థిర పూర్ణాంకం
has_సరిపోలిక(char_u *needle, char_u *haystack)
{
    ఒకవేళ (!needle || !haystack || !*needle)
	తిరిగి FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    అయితే (*n_ptr)
    {
	పూర్ణాంకం n_char = mb_ptr2char(n_ptr);
	పూర్ణాంకం n_పై = MB_TOUPPER(n_char);
	పూర్ణాంకం దొరికింది = FALSE;

	అయితే (*h_ptr)
	{
	    పూర్ణాంకం h_char = mb_ptr2char(h_ptr);
	    ఒకవేళ (h_char == n_char || h_char == n_పై)
	    {
		దొరికింది = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		ఆపు;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	ఒకవేళ (!దొరికింది)
	    తిరిగి FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    తిరిగి OK;
}

రకనిర్వచనం నిర్మాణం సరిపోలిక_struct
{
    పూర్ణాంకం needle_len;
    పూర్ణాంకం haystack_len;
    పూర్ణాంకం క్రింది_needle[సరిపోలిక_MAX_LEN];     // stores codepoints
    పూర్ణాంకం క్రింది_haystack[సరిపోలిక_MAX_LEN];   // stores codepoints
    score_t సరిపోలిక_bonus[సరిపోలిక_MAX_LEN];
} సరిపోలిక_struct;

#నిర్వచించు IS_మాట_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#నిర్వచించు IS_మార్గం_SEP(c) ((c) == '/')
#నిర్వచించు IS_DOT(c)      ((c) == '.')

    స్థిర score_t
compute_bonus_codepoint(పూర్ణాంకం చివరి_c, పూర్ణాంకం c)
{
    ఒకవేళ (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	ఒకవేళ (IS_మార్గం_SEP(చివరి_c))
	    తిరిగి SCORE_సరిపోలిక_SLASH;
	ఒకవేళ (IS_మాట_SEP(చివరి_c))
	    తిరిగి SCORE_సరిపోలిక_మాట;
	ఒకవేళ (IS_DOT(చివరి_c))
	    తిరిగి SCORE_సరిపోలిక_DOT;
	ఒకవేళ (MB_ISUPPER(c) && MB_ISLOWER(చివరి_c))
	    తిరిగి SCORE_సరిపోలిక_CAPITAL;
    }
    తిరిగి 0;
}

    స్థిర శూన్య
అమరిక_సరిపోలిక_struct(సరిపోలిక_struct *సరిపోలిక, char_u *needle, char_u *haystack)
{
    పూర్ణాంకం i = 0;
    char_u *p = needle;
    అయితే (*p != NUL && i < సరిపోలిక_MAX_LEN)
    {
	పూర్ణాంకం c = mb_ptr2char(p);
	సరిపోలిక->క్రింది_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    సరిపోలిక->needle_len = i;

    i = 0;
    p = haystack;
    పూర్ణాంకం prev_c = '/';
    అయితే (*p != NUL && i < సరిపోలిక_MAX_LEN)
    {
	పూర్ణాంకం c = mb_ptr2char(p);
	సరిపోలిక->క్రింది_haystack[i] = MB_TOLOWER(c);
	సరిపోలిక->సరిపోలిక_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    సరిపోలిక->haystack_len = i;
}

    స్థిర అంతఃవరుస శూన్య
సరిపోలిక_అడ్డువరుస(స్థిరరాశి సరిపోలిక_struct *సరిపోలిక, పూర్ణాంకం అడ్డువరుస, score_t *curr_D,
	score_t *curr_M, స్థిరరాశి score_t *చివరి_D, స్థిరరాశి score_t *చివరి_M)
{
    పూర్ణాంకం n = సరిపోలిక->needle_len;
    పూర్ణాంకం m = సరిపోలిక->haystack_len;
    పూర్ణాంకం i = అడ్డువరుస;

    స్థిరరాశి పూర్ణాంకం *క్రింది_needle = సరిపోలిక->క్రింది_needle;
    స్థిరరాశి పూర్ణాంకం *క్రింది_haystack = సరిపోలిక->క్రింది_haystack;
    స్థిరరాశి score_t *సరిపోలిక_bonus = సరిపోలిక->సరిపోలిక_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    కొరకు (పూర్ణాంకం j = 0; j < m; j++)
    {
	ఒకవేళ (క్రింది_needle[i] == క్రింది_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    ఒకవేళ (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + సరిపోలిక_bonus[j];
	    }
	    లేకపోతే ఒకవేళ (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + సరిపోలిక_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_సరిపోలిక_CONSECUTIVE);
	    }
	    prev_D = చివరి_D[j];
	    prev_M = చివరి_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	లేకపోతే
	{
	    prev_D = చివరి_D[j];
	    prev_M = చివరి_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    స్థిర score_t
సరిపోలిక_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    ఒకవేళ (!needle || !haystack || !*needle)
	తిరిగి SCORE_MIN;

    సరిపోలిక_struct సరిపోలిక;
    అమరిక_సరిపోలిక_struct(&సరిపోలిక, needle, haystack);

    పూర్ణాంకం n = సరిపోలిక.needle_len;
    పూర్ణాంకం m = సరిపోలిక.haystack_len;

    ఒకవేళ (m > సరిపోలిక_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	తిరిగి SCORE_MIN;
    }
    లేకపోతే ఒకవేళ (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	కొరకు (పూర్ణాంకం i = 0; i < n; i++)
	{
	    ఒకవేళ (సరిపోలిక.క్రింది_needle[i] != సరిపోలిక.క్రింది_haystack[i])
	    {
		equal = false;
		ఆపు;
	    }
	}
	ఒకవేళ (equal)
	{
	    ఒకవేళ (positions)
	    {
		కొరకు (పూర్ణాంకం i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    తిరిగి SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    ఒకవేళ ((size_t)n > (కొలత_MAX / పరిమాణం(score_t)) / సరిపోలిక_MAX_LEN / 2)
	తిరిగి SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *ఖండం = (score_t*)alloc(పరిమాణం(score_t) * సరిపోలిక_MAX_LEN * n * 2);
    ఒకవేళ (!ఖండం)
	తిరిగి SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[సరిపోలిక_MAX_LEN] = (score_t(*)[సరిపోలిక_MAX_LEN])ఖండం;
    score_t (*M)[సరిపోలిక_MAX_LEN] = (score_t(*)[సరిపోలిక_MAX_LEN])(ఖండం
							+ సరిపోలిక_MAX_LEN * n);

    సరిపోలిక_అడ్డువరుస(&సరిపోలిక, 0, D[0], M[0], D[0], M[0]);
    కొరకు (పూర్ణాంకం i = 1; i < n; i++)
	సరిపోలిక_అడ్డువరుస(&సరిపోలిక, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    ఒకవేళ (positions)
    {
	పూర్ణాంకం సరిపోలిక_required = 0;
	కొరకు (పూర్ణాంకం i = n - 1, j = m - 1; i >= 0; i--)
	{
	    కొరకు (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		ఒకవేళ (D[i][j] != SCORE_MIN &&
			(సరిపోలిక_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    సరిపోలిక_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_సరిపోలిక_CONSECUTIVE;
		    positions[i] = j--;
		    ఆపు;
		}
	    }
	}
    }

    score_t ఫలితం = M[n - 1][m - 1];
    vim_free(ఖండం);
    తిరిగి ఫలితం;
}
