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
#تعریف_کن USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساكن صحيح fuzzy_تطابق_قلم_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_تطابق_درون_list(list_T *l, char_u *str, صحيح matchseq, char_u *کلید, callback_T *قلم_cb, صحيح retmatchpos, list_T *fmatchlist, دراز max_matches);
ساكن فراغ do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحيح retmatchpos);
#پایان_اگر
ساكن صحيح fuzzy_تطابق_str_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_تطابق_str_مرتب_سازی(fuzmatch_str_T *fm, صحيح sz);
ساكن صحيح fuzzy_تطابق_func_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن فراغ fuzzy_تطابق_func_مرتب_سازی(fuzmatch_str_T *fm, صحيح sz);

تعريفنوع مزدوج score_t;
ساكن score_t تطابق_positions(char_u *needle, char_u *haystack, int_u *positions);
ساكن صحيح has_تطابق(char_u *needle, char_u *haystack);

#تعریف_کن SCORE_MAX INFINITY
#تعریف_کن SCORE_MIN (-INFINITY)
#تعریف_کن SCORE_SCALE 1000

تعريفنوع بنية
{
    صحيح		idx;		// used for stable sort
    listitem_T	*قلم;
    صحيح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحيح		itemstr_allocated;
    صحيح		startpos;
} fuzzy_قلم_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحيح
fuzzy_تطابق(
    char_u	*str,
    char_u	*pat_arg,
    صحيح		matchseq,
    صحيح		*بیرون_Score,
    int_u	*matches,
    صحيح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحيح		complete = FALSE;
    صحيح		score = 0;
    صحيح		رقم_Matches = 0;
    صحيح		pat_chars = 0;
    score_t	fzy_score;

    *بیرون_Score = 0;

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
	إذا (رقم_Matches > maxMatches - pat_chars)
	{
	    رقم_Matches = 0;
	    *بیرون_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	score = FUZZY_SCORE_NONE;
	إذا (has_تطابق(pat, str))
	{
	    fzy_score = تطابق_positions(pat, str, matches + رقم_Matches);
	    إذا (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? بیشترین_صحیح
		    : (fzy_score < 0) ? (صحيح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحيح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	إذا (score == FUZZY_SCORE_NONE)
	{
	    رقم_Matches = 0;
	    *بیرون_Score = FUZZY_SCORE_NONE;
	    اكسر;
	}

	إذا (score > 0 && *بیرون_Score > بیشترین_صحیح - score)
	    *بیرون_Score = بیشترین_صحیح;
	وإلا إذا (score < 0 && *بیرون_Score < INT_MIN + 1 - score)
	    *بیرون_Score = INT_MIN + 1;
	وإلا
	    *بیرون_Score += score;

	رقم_Matches += pat_chars;

	إذا (complete || رقم_Matches >= maxMatches)
	    اكسر;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    أرجع رقم_Matches != 0;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساكن صحيح
fuzzy_تطابق_قلم_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح		v1 = ((fuzzy_قلم_T *)s1)->score;
    صحيح		v2 = ((fuzzy_قلم_T *)s2)->score;

    إذا (v1 == v2)
    {
	صحيح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_قلم_T *)s1)->pat;
	صحيح patlen = (صحيح)STRLEN(pat);
	صحيح startpos = ((fuzzy_قلم_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_قلم_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_قلم_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_قلم_T *)s2)->itemstr + startpos, patlen) == 0;

	إذا (exact_match1 == exact_match2)
	{
	    صحيح idx1 = ((fuzzy_قلم_T *)s1)->idx;
	    صحيح idx2 = ((fuzzy_قلم_T *)s2)->idx;
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
fuzzy_تطابق_درون_list(
    list_T	*l,
    char_u	*str,
    صحيح		matchseq,
    char_u	*کلید,
    callback_T	*قلم_cb,
    صحيح		retmatchpos,
    list_T	*fmatchlist,
    دراز	max_matches)
{
    دراز	    len;
    fuzzy_قلم_T	    *اقلام = عدم;
    listitem_T	    *li;
    دراز	    تطابق_شمار = 0;
    int_u	    matches[FUZZY_تطابق_MAX_LEN];

    len = list_len(l);
    إذا (len == 0)
	أرجع;
    إذا (max_matches > 0 && len > max_matches)
	len = max_matches;

    اقلام = ALLOC_CLEAR_MULT(fuzzy_قلم_T, len);
    إذا (اقلام == عدم)
	أرجع;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_اقلام(l, li)
    {
	صحيح		score;
	char_u		*itemstr = عدم;
	char_u		*itemstr_copy = عدم;
	typval_T	rettv;
	صحيح		itemstr_allocate = FALSE;
	list_T		*تطابق_positions = عدم;

	إذا (max_matches > 0 && تطابق_شمار >= max_matches)
	    اكسر;

	rettv.v_type = VAR_UNKNOWN;
	إذا (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	وإلا إذا (li->li_tv.v_type == VAR_DICT
				&& (کلید != عدم || قلم_cb->cb_name != عدم))
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
		إذا (call_callback(قلم_cb, -1, &rettv, 1, argv) != FAIL)
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
		&& fuzzy_تطابق(itemstr, str, matchseq, &score, matches,
						FUZZY_تطابق_MAX_LEN))
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
		تطابق_positions = list_alloc();
		إذا (تطابق_positions == عدم)
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

		طالما (*p != NUL && j < FUZZY_تطابق_MAX_LEN && success)
		{
		    إذا (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			إذا (list_append_عدد(تطابق_positions, matches[j]) == FAIL)
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
		    list_free(تطابق_positions);
		    إذا (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    استمر;
		}
	    }
	    اقلام[تطابق_شمار].idx = تطابق_شمار;
	    اقلام[تطابق_شمار].قلم = li;
	    اقلام[تطابق_شمار].score = score;
	    اقلام[تطابق_شمار].pat = str;
	    اقلام[تطابق_شمار].startpos = matches[0];
	    اقلام[تطابق_شمار].itemstr = itemstr_copy;
	    اقلام[تطابق_شمار].itemstr_allocated = itemstr_allocate;
	    اقلام[تطابق_شمار].lmatchpos = تطابق_positions;

	    ++تطابق_شمار;
	}
	clear_tv(&rettv);
    }

    إذا (تطابق_شمار > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((فراغ *)اقلام, (size_t)تطابق_شمار, حجم(fuzzy_قلم_T),
		fuzzy_تطابق_قلم_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	إذا (retmatchpos)
	{
	    li = list_پیدا_کن(fmatchlist, 0);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب انجامشده;
	    retlist = li->li_tv.vval.v_list;
	}
	وإلا
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	لكل (صحيح i = 0; i < تطابق_شمار; i++)
	{
	    إذا (list_append_tv(retlist, &اقلام[i].قلم->li_tv) == FAIL)
		اذهب انجامشده;
	}

	// next copy the list of matching positions
	إذا (retmatchpos)
	{
	    li = list_پیدا_کن(fmatchlist, -2);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب انجامشده;
	    retlist = li->li_tv.vval.v_list;

	    لكل (صحيح i = 0; i < تطابق_شمار; i++)
	    {
		إذا (اقلام[i].lmatchpos != عدم)
		{
		    إذا (list_append_list(retlist, اقلام[i].lmatchpos) == OK)
			اقلام[i].lmatchpos = عدم;
		    وإلا
			اذهب انجامشده;

		}
	    }

	    // copy the matching scores
	    li = list_پیدا_کن(fmatchlist, -1);
	    إذا (li == عدم || li->li_tv.vval.v_list == عدم)
		اذهب انجامشده;
	    retlist = li->li_tv.vval.v_list;
	    لكل (صحيح i = 0; i < تطابق_شمار; i++)
	    {
		إذا (list_append_عدد(retlist, اقلام[i].score) == FAIL)
		    اذهب انجامشده;
	    }
	}
    }

انجامشده:
    لكل (صحيح i = 0; i < تطابق_شمار; i++)
    {
	إذا (اقلام[i].itemstr_allocated)
	    vim_free(اقلام[i].itemstr);

	إذا (اقلام[i].lmatchpos)
	    list_free(اقلام[i].lmatchpos);
    }
    vim_free(اقلام);
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
    دراز	max_matches = 0;

    إذا (درون_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    CLEAR_اشاره_گر(&cb);

    // validate and get the arguments
    إذا (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_آرگومان_از_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	أرجع;
    }
    إذا (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == عدم)
    {
	semsg(_(e_invalid_آرگومان_str), tv_get_string(&argvars[1]));
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
	إذا ((di = dict_پیدا_کن(d, (char_u *)"key", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == عدم
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "key",
						    tv_get_string(&di->di_tv));
		أرجع;
	    }
	    کلید = tv_get_string(&di->di_tv);
	}
	وإلا إذا ((di = dict_پیدا_کن(d, (char_u *)"text_cb", -1)) != عدم)
	{
	    cb = get_callback(&di->di_tv);
	    إذا (cb.cb_name == عدم)
	    {
		semsg(_(e_invalid_مقدار_for_آرگومان_str), "text_cb");
		أرجع;
	    }
	}

	إذا ((di = dict_پیدا_کن(d, (char_u *)"limit", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_عدد)
	    {
		semsg(_(e_invalid_مقدار_for_آرگومان_str), "limit");
		أرجع;
	    }
	    max_matches = (دراز)tv_get_عدد_chk(&di->di_tv, عدم);
	}

	إذا (dict_has_کلید(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    إذا (ret == FAIL)
	اذهب انجامشده;
    إذا (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	إذا (l == عدم)
	    اذهب انجامشده;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب انجامشده;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب انجامشده;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب انجامشده;
	}
	l = list_alloc();
	إذا (l == عدم)
	    اذهب انجامشده;
	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اذهب انجامشده;
	}
    }

    fuzzy_تطابق_درون_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلید, &cb, retmatchpos, rettv->vval.v_list, max_matches);

انجامشده:
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
#پایان_اگر

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ساكن صحيح
fuzzy_تطابق_str_compare(ثابت فراغ *s1, ثابت فراغ *s2)
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
fuzzy_تطابق_str_مرتب_سازی(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
	    fuzzy_تطابق_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ساكن صحيح
fuzzy_تطابق_func_compare(ثابت فراغ *s1, ثابت فراغ *s2)
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
fuzzy_تطابق_func_مرتب_سازی(fuzmatch_str_T *fm, صحيح sz)
{
    // Sort the list by the descending order of the match score
    qsort((فراغ *)fm, (size_t)sz, حجم(fuzmatch_str_T),
		fuzzy_تطابق_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحيح
fuzzy_تطابق_str(char_u *str, char_u *pat)
{
    صحيح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_تطابق_MAX_LEN];

    إذا (str == عدم || pat == عدم)
	أرجع score;

    fuzzy_تطابق(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    أرجع score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_تطابق_str_with_pos(char_u *str استفادهنشده, char_u *pat استفادهنشده)
{
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    صحيح		    score = FUZZY_SCORE_NONE;
    garray_T	    *تطابق_positions = عدم;
    int_u	    matches[FUZZY_تطابق_MAX_LEN];
    صحيح		    j = 0;

    إذا (str == عدم || pat == عدم)
	أرجع عدم;

    تطابق_positions = ALLOC_ONE(garray_T);
    إذا (تطابق_positions == عدم)
	أرجع عدم;
    ga_init2(تطابق_positions, حجم(int_u), 10);

    إذا (!fuzzy_تطابق(str, pat, FALSE, &score, matches, FUZZY_تطابق_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(تطابق_positions);
	vim_free(تطابق_positions);
	أرجع عدم;
    }

    لكل (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	إذا (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    إذا (ga_grow(تطابق_positions, 1) == FAIL)
	    {
		ga_clear(تطابق_positions);
		vim_free(تطابق_positions);
		أرجع عدم;
	    }
	    ((int_u *)تطابق_positions->ga_داده)[تطابق_positions->ga_len] =
								    matches[j];
	    تطابق_positions->ga_len++;
	    j++;
	}
    }

    أرجع تطابق_positions;
#وإلا
    أرجع عدم;
#پایان_اگر
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
fuzzy_تطابق_str_درون_سطر(
    char_u	**ptr,
    char_u	*pat,
    صحيح		*len,
    pos_T	*current_pos,
    صحيح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*انتها = عدم;
    char_u	*شروع = عدم;
    صحيح		یافت = FALSE;
    حرف	save_انتها;
    char_u	*سطر_انتها = عدم;

    إذا (str == عدم || pat == عدم)
	أرجع یافت;
    سطر_انتها = پیدا_کن_سطر_انتها(str);

    طالما (str < سطر_انتها)
    {
	// Skip non-word characters
	شروع = پیدا_کن_کلمه_شروع(str);
	إذا (*شروع == NUL)
	    اكسر;
	انتها = پیدا_کن_کلمه_انتها(شروع);

	// Extract the word from start to end
	save_انتها = *انتها;
	*انتها = NUL;

	// Perform fuzzy match
	*score = fuzzy_تطابق_str(شروع, pat);
	*انتها = save_انتها;

	إذا (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحيح)(انتها - شروع);
	    یافت = TRUE;
	    *ptr = شروع;
	    إذا (current_pos)
		current_pos->ستون += (صحيح)(انتها - strBegin);
	    اكسر;
	}

	// Move to the end of the current word for the next iteration
	str = انتها;
	// Ensure we continue searching after the current word
	طالما (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    إذا (!یافت)
	*ptr = سطر_انتها;

    أرجع یافت;
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
جستجو_for_fuzzy_تطابق(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*الگو,
    صحيح		dir,
    pos_T	*شروع_pos,
    صحيح		*len,
    char_u	**ptr,
    صحيح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_انتها;
    صحيح		یافت_new_تطابق = FALSE;
    صحيح		looped_around = FALSE;
    صحيح		کامل_سطر = ctrl_x_نما_کامل_سطر();

    إذا (buf == curbuf)
	circly_انتها = *شروع_pos;
    وإلا
    {
	circly_انتها.lnum = buf->b_ml.ml_سطر_شمار;
	circly_انتها.ستون = 0;
	circly_انتها.coladd = 0;
    }

    إذا (کامل_سطر && شروع_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    افعل
    {

	// Check if looped around and back to start position
	إذا (looped_around && (کامل_سطر ? current_pos.lnum == circly_انتها.lnum
					: EQUAL_POS(current_pos, circly_انتها)))
	    اكسر;

	// Ensure current_pos is valid
	إذا (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_سطر_شمار)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    إذا (!کامل_سطر)
		*ptr += current_pos.ستون;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    إذا (*ptr != عدم && **ptr != NUL)
	    {
		إذا (!کامل_سطر)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    یافت_new_تطابق = fuzzy_تطابق_str_درون_سطر(ptr, الگو,
						    len, &current_pos, score);
		    إذا (یافت_new_تطابق)
		    {
			*pos = current_pos;
			اكسر;
		    }
		    وإلا إذا (looped_around && current_pos.lnum == circly_انتها.lnum)
			اكسر;
		}
		وإلا
		{
		    char_u *سطر = *ptr;
		    char_u *p = skipwhite(سطر);
		    إذا (fuzzy_تطابق_str(p, الگو) != FUZZY_SCORE_NONE)
		    {
			یافت_new_تطابق = TRUE;
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
	    إذا (++current_pos.lnum > buf->b_ml.ml_سطر_شمار)
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
		    current_pos.lnum = buf->b_ml.ml_سطر_شمار;
		    looped_around = TRUE;
		}
		وإلا
		    اكسر;

	    }
	}
	current_pos.ستون = 0;
    } طالما (TRUE);

    أرجع یافت_new_تطابق;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    فراغ
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحيح شمار)
{
    إذا (fuzmatch == عدم)
	أرجع;

    لكل (صحيح i = 0; i < شمار; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحيح
fuzzymatches_مقصد_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحيح		شمار,
	صحيح		funcsort)
{
    إذا (شمار <= 0)
	اذهب theend;

    *matches = ALLOC_MULT(char_u *, شمار);
    إذا (*matches == عدم)
    {
	fuzmatch_str_free(fuzmatch, شمار);
	أرجع FAIL;
    }

    // Sort the list by the descending order of the match score
    إذا (funcsort)
	fuzzy_تطابق_func_مرتب_سازی((فراغ *)fuzmatch, (size_t)شمار);
    وإلا
	fuzzy_تطابق_str_مرتب_سازی((فراغ *)fuzmatch, (size_t)شمار);

    لكل (صحيح i = 0; i < شمار; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    أرجع OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#تعریف_کن تطابق_MAX_LEN FUZZY_تطابق_MAX_LEN

#تعریف_کن SCORE_GAP_LEADING -0.005
#تعریف_کن SCORE_GAP_TRAILING -0.005
#تعریف_کن SCORE_GAP_INNER -0.01
#تعریف_کن SCORE_تطابق_CONSECUTIVE 1.0
#تعریف_کن SCORE_تطابق_SLASH 0.9
#تعریف_کن SCORE_تطابق_کلمه 0.8
#تعریف_کن SCORE_تطابق_CAPITAL 0.7
#تعریف_کن SCORE_تطابق_DOT 0.6

    ساكن صحيح
has_تطابق(char_u *needle, char_u *haystack)
{
    إذا (!needle || !haystack || !*needle)
	أرجع FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    طالما (*n_ptr)
    {
	صحيح n_char = mb_ptr2char(n_ptr);
	صحيح n_فوقانی = MB_TOUPPER(n_char);
	صحيح یافت = FALSE;

	طالما (*h_ptr)
	{
	    صحيح h_char = mb_ptr2char(h_ptr);
	    إذا (h_char == n_char || h_char == n_فوقانی)
	    {
		یافت = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		اكسر;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	إذا (!یافت)
	    أرجع FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    أرجع OK;
}

تعريفنوع بنية تطابق_struct
{
    صحيح needle_len;
    صحيح haystack_len;
    صحيح پایینی_needle[تطابق_MAX_LEN];     // stores codepoints
    صحيح پایینی_haystack[تطابق_MAX_LEN];   // stores codepoints
    score_t تطابق_bonus[تطابق_MAX_LEN];
} تطابق_struct;

#تعریف_کن IS_کلمه_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#تعریف_کن IS_مسیر_SEP(c) ((c) == '/')
#تعریف_کن IS_DOT(c)      ((c) == '.')

    ساكن score_t
compute_bonus_codepoint(صحيح آخرین_c, صحيح c)
{
    إذا (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	إذا (IS_مسیر_SEP(آخرین_c))
	    أرجع SCORE_تطابق_SLASH;
	إذا (IS_کلمه_SEP(آخرین_c))
	    أرجع SCORE_تطابق_کلمه;
	إذا (IS_DOT(آخرین_c))
	    أرجع SCORE_تطابق_DOT;
	إذا (MB_ISUPPER(c) && MB_ISLOWER(آخرین_c))
	    أرجع SCORE_تطابق_CAPITAL;
    }
    أرجع 0;
}

    ساكن فراغ
راهاندازی_تطابق_struct(تطابق_struct *تطابق, char_u *needle, char_u *haystack)
{
    صحيح i = 0;
    char_u *p = needle;
    طالما (*p != NUL && i < تطابق_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	تطابق->پایینی_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    تطابق->needle_len = i;

    i = 0;
    p = haystack;
    صحيح prev_c = '/';
    طالما (*p != NUL && i < تطابق_MAX_LEN)
    {
	صحيح c = mb_ptr2char(p);
	تطابق->پایینی_haystack[i] = MB_TOLOWER(c);
	تطابق->تطابق_bonus[i] = compute_bonus_codepoint(prev_c, c);
	prev_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    تطابق->haystack_len = i;
}

    ساكن مضمن فراغ
تطابق_ردیف(ثابت تطابق_struct *تطابق, صحيح ردیف, score_t *curr_D,
	score_t *curr_M, ثابت score_t *آخرین_D, ثابت score_t *آخرین_M)
{
    صحيح n = تطابق->needle_len;
    صحيح m = تطابق->haystack_len;
    صحيح i = ردیف;

    ثابت صحيح *پایینی_needle = تطابق->پایینی_needle;
    ثابت صحيح *پایینی_haystack = تطابق->پایینی_haystack;
    ثابت score_t *تطابق_bonus = تطابق->تطابق_bonus;

    score_t prev_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t prev_M = SCORE_MIN, prev_D = SCORE_MIN;

    لكل (صحيح j = 0; j < m; j++)
    {
	إذا (پایینی_needle[i] == پایینی_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    إذا (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + تطابق_bonus[j];
	    }
	    وإلا إذا (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			prev_M + تطابق_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			prev_D + SCORE_تطابق_CONSECUTIVE);
	    }
	    prev_D = آخرین_D[j];
	    prev_M = آخرین_M[j];
	    curr_D[j] = score;
	    curr_M[j] = prev_score = MAX(score, prev_score + gap_score);
	}
	وإلا
	{
	    prev_D = آخرین_D[j];
	    prev_M = آخرین_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = prev_score = prev_score + gap_score;
	}
    }
}

    ساكن score_t
تطابق_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    إذا (!needle || !haystack || !*needle)
	أرجع SCORE_MIN;

    تطابق_struct تطابق;
    راهاندازی_تطابق_struct(&تطابق, needle, haystack);

    صحيح n = تطابق.needle_len;
    صحيح m = تطابق.haystack_len;

    إذا (m > تطابق_MAX_LEN || n > m)
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
	    إذا (تطابق.پایینی_needle[i] != تطابق.پایینی_haystack[i])
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
    إذا ((size_t)n > (اندازه_MAX / حجم(score_t)) / تطابق_MAX_LEN / 2)
	أرجع SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *بلوک = (score_t*)alloc(حجم(score_t) * تطابق_MAX_LEN * n * 2);
    إذا (!بلوک)
	أرجع SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[تطابق_MAX_LEN] = (score_t(*)[تطابق_MAX_LEN])بلوک;
    score_t (*M)[تطابق_MAX_LEN] = (score_t(*)[تطابق_MAX_LEN])(بلوک
							+ تطابق_MAX_LEN * n);

    تطابق_ردیف(&تطابق, 0, D[0], M[0], D[0], M[0]);
    لكل (صحيح i = 1; i < n; i++)
	تطابق_ردیف(&تطابق, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    إذا (positions)
    {
	صحيح تطابق_required = 0;
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
			(تطابق_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    تطابق_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_تطابق_CONSECUTIVE;
		    positions[i] = j--;
		    اكسر;
		}
	    }
	}
    }

    score_t نتیجه = M[n - 1][m - 1];
    vim_free(بلوک);
    أرجع نتیجه;
}
