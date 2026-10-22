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
#تعریف_کرو USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساكن صحيح fuzzy_میل_شے_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_میل_وچ_list(list_T *l, char_u *str, صحيح matchseq, char_u *کلید, callback_T *شے_cb, صحيح retmatchpos, list_T *fmatchlist, لما max_matches);
ساكن فراغ do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos);
#جے_انت
ساكن صحيح fuzzy_میل_str_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_میل_str_ترتیب_دیو(fuzmatch_str_T *fm, صحيح sz);
ساكن صحيح fuzzy_میل_func_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_میل_func_ترتیب_دیو(fuzmatch_str_T *fm, صحيح sz);

تعريفنوع مزدوج score_t;
ساكن score_t میل_positions(char_u *needle, char_u *haystack, int_u *positions);
ساكن صحيح has_میل(char_u *needle, char_u *haystack);

#تعریف_کرو SCORE_MAX INFINITY
#تعریف_کرو SCORE_MIN (-INFINITY)
#تعریف_کرو SCORE_SCALE 1000

تعريفنوع بنية
{
    صحيح		idx;		// used for stable sort
    listitem_T	*شے;
    صحيح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحيح		itemstr_allocated;
    صحيح		startpos;
} fuzzy_شے_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحيح
fuzzy_میل(
    char_u	*str,
    char_u	*pat_arg,
    صحيح		matchseq,
    صحيح		*باہرلا_Score,
    int_u	*matches,
    صحيح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحيح		complete = FALSE;
    صحيح		score = 0;
    صحيح		عدد_Matches = 0;
    صحيح		pat_chars = 0;
    score_t	fzy_score;

    *باہرلا_Score = 0;

    save_pat = vim_strsave(pat_arg);
    إذا (save_pat == عدم)
	أرجع FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    طالما (TRUE)
    {
	إذا (matchseq)
	    complete = TRUE;
	وإلا
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    إذا (*p == NUL)
		اكسر;
	    pat = p;
	    طالما (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		إذا (has_mbyte)
		    MB_PTR_ADV(p);
		وإلا
		    ++p;
	    }
	    إذا (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	إذا (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	إذا (عدد_Matches > maxMatches - pat_chars)
	{
	    عدد_Matches = 0;
	    *باہرلا_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	score = FUZZY_SCORE_NONE;
	إذا (has_میل(pat, str))
	{
	    fzy_score = میل_positions(pat, str, matches + عدد_Matches);
	    إذا (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? ودھ_توں_ودھ_عدد
		    : (fzy_score < 0) ? (صحيح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحيح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	إذا (score == FUZZY_SCORE_NONE)
	{
	    عدد_Matches = 0;
	    *باہرلا_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	إذا (score > 0 && *باہرلا_Score > ودھ_توں_ودھ_عدد - score)
	    *باہرلا_Score = ودھ_توں_ودھ_عدد;
	وإلا إذا (score < 0 && *باہرلا_Score < INT_MIN + 1 - score)
	    *باہرلا_Score = INT_MIN + 1;
	وإلا
	    *باہرلا_Score += score;

	عدد_Matches += pat_chars;

	إذا (complete || عدد_Matches >= maxMatches)
	    اكسر;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    أرجع عدد_Matches != 0;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساكن صحيح
fuzzy_میل_شے_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzzy_شے_T *)s1)->score;
    صحيح		v2 = ((fuzzy_شے_T *)s2)->score;

    إذا (v1 == v2)
    {
	صحيح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_شے_T *)s1)->pat;
	صحيح patlen = (صحيح)STRLEN(pat);
	صحيح startpos = ((fuzzy_شے_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_شے_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_شے_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_شے_T *)s2)->itemstr + startpos, patlen) == 0;

	إذا (exact_match1 == exact_match2)
	{
	    صحيح idx1 = ((fuzzy_شے_T *)s1)->idx;
	    صحيح idx2 = ((fuzzy_شے_T *)s2)->idx;
	    أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	وإلا إذا (exact_match2)
	    أرجع 1;
	أرجع -1;
    }
    وإلا
	أرجع v1 > v2 ? -1 : 1;
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
    ساكن فراغ
fuzzy_میل_وچ_list(
    list_T	*l,
    char_u	*str,
    صحيح		matchseq,
    char_u	*کلید,
    callback_T	*شے_cb,
    صحيح		retmatchpos,
    list_T	*fmatchlist,
    لما	max_matches)
{
    لما	    len;
    fuzzy_شے_T	    *اشیاء = عدم;
    listitem_T	    *li;
    لما	    میل_گنتی = 0;
    int_u	    matches[FUZZY_میل_MAX_LEN];

    len = list_len(l);
    إذا (len == 0)
	أرجع;
    إذا (max_matches > 0 && len > max_matches)
	len = max_matches;

    اشیاء = ALLOC_CLEAR_MULT(fuzzy_شے_T, len);
    إذا (اشیاء == عدم)
	أرجع;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_اشیاء(l, li)
    {
	صحيح		score;
	char_u		*itemstr = عدم;
	char_u		*itemstr_copy = عدم;
	typval_T	rettv;
	صحيح		itemstr_allocate = FALSE;
	list_T		*میل_positions = عدم;

	إذا (max_matches > 0 && میل_گنتی >= max_matches)
	    اكسر;

	rettv.v_type = VAR_UNKNOWN;
	إذا (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	وإلا إذا (li->li_tv.v_type == VAR_DICT
				&& (کلید != عدم || شے_cb->cb_name != عدم))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    إذا (کلید != عدم)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (حرف *)کلید, FALSE);
	    وإلا
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		إذا (call_callback(شے_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    إذا (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	إذا (itemstr != عدم
		&& fuzzy_میل(itemstr, str, matchseq, &score, matches,
						FUZZY_میل_MAX_LEN))
	{
	    إذا (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		إذا (itemstr_copy == عدم)
		{
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    وإلا
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    إذا (retmatchpos)
	    {
		میل_positions = list_alloc();
		إذا (میل_positions == عدم)
		{
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}

		// Fill position information
		صحيح	j = 0;
		char_u	*p = str;
		صحيح	success = TRUE;

		طالما (*p != NUL && j < FUZZY_میل_MAX_LEN && success)
		{
		    إذا (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			إذا (list_append_نمبر(میل_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    اكسر;
			}
			j++;
		    }
		    إذا (has_mbyte)
			MB_PTR_ADV(p);
		    وإلا
			++p;
		}

		إذا (!success)
		{
		    list_free(میل_positions);
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    اشیاء[میل_گنتی].idx = میل_گنتی;
	    اشیاء[میل_گنتی].شے = li;
	    اشیاء[میل_گنتی].score = score;
	    اشیاء[میل_گنتی].pat = str;
	    اشیاء[میل_گنتی].startpos = matches[0];
	    اشیاء[میل_گنتی].itemstr = itemstr_copy;
	    اشیاء[میل_گنتی].itemstr_allocated = itemstr_allocate;
	    اشیاء[میل_گنتی].lmatchpos = میل_positions;

	    ++میل_گنتی;
	}
	clear_tv(&rettv);
    }

    إذا (میل_گنتی > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((فراغ *)اشیاء, (size_t)میل_گنتی, حجم(fuzzy_شے_T),
		fuzzy_میل_شے_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	إذا (retmatchpos)
	{
	    li = list_لبھو(fmatchlist, 0);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب پورا;
	    retlist = li->li_tv.vval.v_list;
	}
	وإلا
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	لكل (صحيح i = 0; i < میل_گنتی; i++)
	{
	    إذا (list_append_tv(retlist, &اشیاء[i].شے->li_tv) == FAIL)
		اذهب پورا;
	}

	// next copy the list of matching positions
	إذا (retmatchpos)
	{
	    li = list_لبھو(fmatchlist, -2);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب پورا;
	    retlist = li->li_tv.vval.v_list;

	    لكل (صحيح i = 0; i < میل_گنتی; i++)
	    {
		إذا (اشیاء[i].lmatchpos != عدم)
		{
		    إذا (list_append_list(retlist, اشیاء[i].lmatchpos) == OK)
			اشیاء[i].lmatchpos = عدم;
		    وإلا
			اذهب پورا;

		}
	    }

	    // copy the matching scores
	    li = list_لبھو(fmatchlist, -1);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب پورا;
	    retlist = li->li_tv.vval.v_list;
	    لكل (صحيح i = 0; i < میل_گنتی; i++)
	    {
		إذا (list_append_نمبر(retlist, اشیاء[i].score) == FAIL)
		    اذهب پورا;
	    }
	}
    }

پورا:
    لكل (صحيح i = 0; i < میل_گنتی; i++)
    {
	إذا (اشیاء[i].itemstr_allocated)
	    vim_free(اشیاء[i].itemstr);

	إذا (اشیاء[i].lmatchpos)
	    list_free(اشیاء[i].lmatchpos);
    }
    vim_free(اشیاء);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ساكن فراغ
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos)
{
    callback_T	cb;
    char_u	*کلید = عدم;
    صحيح		ret;
    صحيح		matchseq = FALSE;
    لما	max_matches = 0;

    إذا (وچ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    CLEAR_اشارہ_گر(&cb);

    // validate and get the arguments
    إذا (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_دلیل_دا_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	أرجع;
    }
    إذا (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == عدم)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	أرجع;
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	إذا (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    أرجع;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	إذا ((di = dict_لبھو(d, (char_u *)"key", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == عدم
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "key",
						    tv_get_string(&di->di_tv));
		أرجع;
	    }
	    کلید = tv_get_string(&di->di_tv);
	}
	وإلا إذا ((di = dict_لبھو(d, (char_u *)"text_cb", -1)) != عدم)
	{
	    cb = get_callback(&di->di_tv);
	    إذا (cb.cb_name == عدم)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "text_cb");
		أرجع;
	    }
	}

	إذا ((di = dict_لبھو(d, (char_u *)"limit", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_نمبر)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "limit");
		أرجع;
	    }
	    max_matches = (لما)tv_get_نمبر_chk(&di->di_tv, عدم);
	}

	إذا (dict_has_کلید(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    إذا (ret == FAIL)
	اذهب پورا;
    إذا (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	إذا (l == عدم)
	    اذهب پورا;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب پورا;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب پورا;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب پورا;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب پورا;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب پورا;
	}
    }

    fuzzy_میل_وچ_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلید, &cb, retmatchpos, rettv->vval.v_list, max_matches);

پورا:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    فراغ
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    فراغ
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#جے_انت

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ساكن صحيح
fuzzy_میل_str_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحيح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحيح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحيح		idx2 = ((fuzmatch_str_T *)s2)->idx;

    إذا (v1 == v2)
	أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وإلا
	أرجع v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ساكن فراغ
fuzzy_میل_str_ترتیب_دیو(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
	    fuzzy_میل_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ساكن صحيح
fuzzy_میل_func_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحيح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحيح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحيح		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    إذا (*str1 != '<' && *str2 == '<')
	أرجع -1;
    إذا (*str1 == '<' && *str2 != '<')
	أرجع 1;
    إذا (v1 == v2)
	أرجع idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    وإلا
	أرجع v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ساكن فراغ
fuzzy_میل_func_ترتیب_دیو(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
		fuzzy_میل_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحيح
fuzzy_میل_str(char_u *str, char_u *pat)
{
    صحيح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_میل_MAX_LEN];

    إذا (str == عدم || pat == عدم)
	أرجع score;

    fuzzy_میل(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    أرجع score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_میل_str_with_pos(char_u *str بن_ورتیا, char_u *pat بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    صحيح		    score = FUZZY_SCORE_NONE;
    garray_T	    *میل_positions = عدم;
    int_u	    matches[FUZZY_میل_MAX_LEN];
    صحيح		    j = 0;

    إذا (str == عدم || pat == عدم)
	أرجع عدم;

    میل_positions = ALLOC_ONE(garray_T);
    إذا (میل_positions == عدم)
	أرجع عدم;
    ga_init2(میل_positions, حجم(int_u), 10);

    إذا (!fuzzy_میل(str, pat, FALSE, &score, matches, FUZZY_میل_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(میل_positions);
	vim_free(میل_positions);
	أرجع عدم;
    }

    لكل (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	إذا (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    إذا (ga_grow(میل_positions, 1) == FAIL)
	    {
		ga_clear(میل_positions);
		vim_free(میل_positions);
		أرجع عدم;
	    }
	    ((int_u *)میل_positions->ga_انکڑے)[میل_positions->ga_len] =
								    matches[j];
	    میل_positions->ga_len++;
	    j++;
	}
    }

    أرجع میل_positions;
#وإلا
    أرجع عدم;
#جے_انت
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
    صحيح
fuzzy_میل_str_وچ_سطر(
    char_u	**ptr,
    char_u	*pat,
    صحيح		*len,
    pos_T	*current_pos,
    صحيح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*سرا = عدم;
    char_u	*شروع = عدم;
    صحيح		لبھیا = FALSE;
    حرف	save_سرا;
    char_u	*سطر_سرا = عدم;

    إذا (str == عدم || pat == عدم)
	أرجع لبھیا;
    سطر_سرا = لبھو_سطر_سرا(str);

    طالما (str < سطر_سرا)
    {
	// Skip non-word characters
	شروع = لبھو_لفظ_شروع(str);
	إذا (*شروع == NUL)
	    اكسر;
	سرا = لبھو_لفظ_سرا(شروع);

	// Extract the word from start to end
	save_سرا = *سرا;
	*سرا = NUL;

	// Perform fuzzy match
	*score = fuzzy_میل_str(شروع, pat);
	*سرا = save_سرا;

	إذا (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحيح)(سرا - شروع);
	    لبھیا = TRUE;
	    *ptr = شروع;
	    إذا (current_pos)
		current_pos->خانہ += (صحيح)(سرا - strBegin);
	    اكسر;
	}

	// Move to the end of the current word for the next iteration
	str = سرا;
	// Ensure we continue searching after the current word
	طالما (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    إذا (!لبھیا)
	*ptr = سطر_سرا;

    أرجع لبھیا;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    صحيح
تلاش_for_fuzzy_میل(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*نمونہ,
    صحيح		dir,
    pos_T	*شروع_pos,
    صحيح		*len,
    char_u	**ptr,
    صحيح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_سرا;
    صحيح		لبھیا_new_میل = FALSE;
    صحيح		looped_around = FALSE;
    صحيح		سارا_سطر = ctrl_x_منوال_سارا_سطر();

    إذا (buf == curbuf)
	circly_سرا = *شروع_pos;
    وإلا
    {
	circly_سرا.lnum = buf->b_ml.ml_سطر_گنتی;
	circly_سرا.خانہ = 0;
	circly_سرا.coladd = 0;
    }

    إذا (سارا_سطر && شروع_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    افعل
    {

	// Check if looped around and back to start position
	إذا (looped_around && (سارا_سطر ? current_pos.lnum == circly_سرا.lnum
					: EQUAL_POS(current_pos, circly_سرا)))
	    اكسر;

	// Ensure current_pos is valid
	إذا (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    إذا (!سارا_سطر)
		*ptr += current_pos.خانہ;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    إذا (*ptr != عدم && **ptr != NUL)
	    {
		إذا (!سارا_سطر)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    لبھیا_new_میل = fuzzy_میل_str_وچ_سطر(ptr, نمونہ,
						    len, &current_pos, score);
		    إذا (لبھیا_new_میل)
		    {
			*pos = current_pos;
			اكسر;
		    }
		    وإلا إذا (looped_around && current_pos.lnum == circly_سرا.lnum)
			اكسر;
		}
		وإلا
		{
		    char_u *سطر = *ptr;
		    char_u *p = skipwhite(سطر);
		    إذا (fuzzy_میل_str(p, نمونہ) != FUZZY_SCORE_NONE)
		    {
			لبھیا_new_میل = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحيح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحيح)(p - سطر);
			اكسر;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	إذا (dir == FORWARD)
	{
	    إذا (++current_pos.lnum > buf->b_ml.ml_سطر_گنتی)
	    {
		إذا (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;
	    }
	}
	وإلا
	{
	    إذا (--current_pos.lnum < 1)
	    {
		إذا (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_سطر_گنتی;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;

	    }
	}
	current_pos.خانہ = 0;
    } طالما (TRUE);

    أرجع لبھیا_new_میل;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    فراغ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحيح گنتی)
{
    إذا (fuzmatch == عدم)
	أرجع;

    لكل (صحيح i = 0; i < گنتی; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحيح
fuzzymatches_منزل_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحيح		گنتی,
	صحيح		funcsort)
{
    إذا (گنتی <= 0)
	اذهب theend;

    *matches = ALLOC_MULT(char_u *, گنتی);
    إذا (*matches == عدم)
    {
	fuzmatch_str_free(fuzmatch, گنتی);
	أرجع FAIL;
    }

    // Sort the list by the descending order of the match score
    إذا (funcsort)
	fuzzy_میل_func_ترتیب_دیو((فراغ *)fuzmatch, (size_t)گنتی);
    وإلا
	fuzzy_میل_str_ترتیب_دیو((فراغ *)fuzmatch, (size_t)گنتی);

    لكل (صحيح i = 0; i < گنتی; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    أرجع OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#تعریف_کرو میل_MAX_LEN FUZZY_میل_MAX_LEN

#تعریف_کرو SCORE_GAP_LEADING -0.005
#تعریف_کرو SCORE_GAP_TRAILING -0.005
#تعریف_کرو SCORE_GAP_INNER -0.01
#تعریف_کرو SCORE_میل_CONSECUTIVE 1.0
#تعریف_کرو SCORE_میل_SLASH 0.9
#تعریف_کرو SCORE_میل_لفظ 0.8
#تعریف_کرو SCORE_میل_CAPITAL 0.7
#تعریف_کرو SCORE_میل_DOT 0.6

    ساكن صحيح
has_میل(char_u *needle, char_u *haystack)
{
    إذا (!needle || !haystack || !*needle)
	أرجع FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    طالما (*n_ptr)
    {
	صحيح n_char = mb_ptr2char(n_ptr);
	صحيح n_اتلا = MB_TOUPPER(n_char);
	صحيح لبھیا = FALSE;

	طالما (*h_ptr)
	{
	    صحيح h_char = mb_ptr2char(h_ptr);
	    إذا (h_char == n_char || h_char == n_اتلا)
	    {
		لبھیا = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		اكسر;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	إذا (!لبھیا)
	    أرجع FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    أرجع OK;
}

تعريفنوع بنية میل_struct
{
    صحيح needle_len;
    صحيح haystack_len;
    صحيح ہیٹھلا_needle[میل_MAX_LEN];     // stores codepoints
    صحيح ہیٹھلا_haystack[میل_MAX_LEN];   // stores codepoints
    score_t میل_bonus[میل_MAX_LEN];
} میل_struct;

#تعریف_کرو IS_لفظ_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#تعریف_کرو IS_رستہ_SEP(c) ((c) == '/')
#تعریف_کرو IS_DOT(c)      ((c) == '.')

    ساكن score_t
compute_bonus_codepoint(صحيح آخری_c, صحيح c)
{
    إذا (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	إذا (IS_رستہ_SEP(آخری_c))
	    أرجع SCORE_میل_SLASH;
	إذا (IS_لفظ_SEP(آخری_c))
	    أرجع SCORE_میل_لفظ;
	إذا (IS_DOT(آخری_c))
	    أرجع SCORE_میل_DOT;
	إذا (MB_ISUPPER(c) && MB_ISLOWER(آخری_c))
	    أرجع SCORE_میل_CAPITAL;
    }
    أرجع 0;
}

    ساكن فراغ
تیاری_میل_struct(میل_struct *میل, char_u *needle, char_u *haystack)
{
    صحيح i = 0;
    char_u *p = needle;
    طالما (*p != NUL && i < میل_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	میل->ہیٹھلا_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    میل->needle_len = i;

    i = 0;
    p = haystack;
    صحيح prev_c = '/';
    طالما (*p != NUL && i < میل_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	میل->ہیٹھلا_haystack[i] = MB_TOLOWER(c);
	میل->میل_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    میل->haystack_len = i;
}

    ساكن مضمن فراغ
میل_صف(ثابت میل_struct *میل, صحيح صف, score_t *curr_D,
	score_t *curr_M, ثابت score_t *آخری_D, ثابت score_t *آخری_M)
{
    صحيح n = میل->needle_len;
    صحيح m = میل->haystack_len;
    صحيح i = صف;

    ثابت صحيح *ہیٹھلا_needle = میل->ہیٹھلا_needle;
    ثابت صحيح *ہیٹھلا_haystack = میل->ہیٹھلا_haystack;
    ثابت score_t *میل_bonus = میل->میل_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    لكل (صحيح j = 0; j < m; j++)
    {
	إذا (ہیٹھلا_needle[i] == ہیٹھلا_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    إذا (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + میل_bonus[j];
	    }
	    وإلا إذا (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + میل_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_میل_CONSECUTIVE);
	    }
	    prev_D = آخری_D[j];
	    prev_M = آخری_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	وإلا
	{
	    prev_D = آخری_D[j];
	    prev_M = آخری_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ساكن score_t
میل_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    إذا (!needle || !haystack || !*needle)
	أرجع SCORE_MIN;

    میل_struct میل;
    تیاری_میل_struct(&میل, needle, haystack);

    صحيح n = میل.needle_len;
    صحيح m = میل.haystack_len;

    إذا (m > میل_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	أرجع SCORE_MIN;
    }
    وإلا إذا (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	لكل (صحيح i = 0; i < n; i++)
	{
	    إذا (میل.ہیٹھلا_needle[i] != میل.ہیٹھلا_haystack[i])
	    {
		equal = false;
		اكسر;
	    }
	}
	إذا (equal)
	{
	    إذا (positions)
	    {
		لكل (صحيح i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    أرجع SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    إذا ((size_t)n > (ناپ_MAX / حجم(score_t)) / میل_MAX_LEN / 2)
	أرجع SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *بلاک = (score_t*)alloc(حجم(score_t) * میل_MAX_LEN * n * 2);
    إذا (!بلاک)
	أرجع SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[میل_MAX_LEN] = (score_t(*)[میل_MAX_LEN])بلاک;
    score_t (*M)[میل_MAX_LEN] = (score_t(*)[میل_MAX_LEN])(بلاک
							+ میل_MAX_LEN * n);

    میل_صف(&میل, 0, D[0], M[0], D[0], M[0]);
    لكل (صحيح i = 1; i < n; i++)
	میل_صف(&میل, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    إذا (positions)
    {
	صحيح میل_required = 0;
	لكل (صحيح i = n - 1, j = m - 1; i >= 0; i--)
	{
	    لكل (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		إذا (D[i][j] != SCORE_MIN &&
			(میل_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    میل_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_میل_CONSECUTIVE;
		    positions[i] = j--;
		    اكسر;
		}
	    }
	}
    }

    score_t نتیجہ = M[n - 1][m - 1];
    vim_free(بلاک);
    أرجع نتیجہ;
}
