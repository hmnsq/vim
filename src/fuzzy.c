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
#شامل "vim.h"

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
ساکن صحیح fuzzy_مطابقت_شے_compare(ثابت خالی *s1, ثابت خالی *s2);
ساکن خالی fuzzy_مطابقت_اندر_list(list_T *l, char_u *str, صحیح matchseq, char_u *کلید, callback_T *شے_cb, صحیح retmatchpos, list_T *fmatchlist, طویل max_matches);
ساکن خالی do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحیح retmatchpos);
#اگر_ختم
ساکن صحیح fuzzy_مطابقت_str_compare(ثابت خالی *s1, ثابت خالی *s2);
ساکن خالی fuzzy_مطابقت_str_ترتیب_دو(fuzmatch_str_T *fm, صحیح sz);
ساکن صحیح fuzzy_مطابقت_func_compare(ثابت خالی *s1, ثابت خالی *s2);
ساکن خالی fuzzy_مطابقت_func_ترتیب_دو(fuzmatch_str_T *fm, صحیح sz);

قسمتعریف دوہرا score_t;
ساکن score_t مطابقت_positions(char_u *needle, char_u *haystack, int_u *positions);
ساکن صحیح has_مطابقت(char_u *needle, char_u *haystack);

#تعریف_کرو SCORE_MAX INFINITY
#تعریف_کرو SCORE_MIN (-INFINITY)
#تعریف_کرو SCORE_SCALE 1000

قسمتعریف ساخت
{
    صحیح		idx;		// used for stable sort
    listitem_T	*شے;
    صحیح		score;
    list_T	*lmatchpos;
    char_u	*pat;
    char_u	*itemstr;
    صحیح		itemstr_allocated;
    صحیح		startpos;
} fuzzy_شے_T;

/*
 * fuzzy_match()
 *
 * Returns TRUE if "pat_arg" matches "str". Also returns the match score in
 * "outScore" and the matching character positions in "matches".
 */
    صحیح
fuzzy_مطابقت(
    char_u	*str,
    char_u	*pat_arg,
    صحیح		matchseq,
    صحیح		*برآمد_Score,
    int_u	*matches,
    صحیح		maxMatches)
{
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    صحیح		complete = FALSE;
    صحیح		score = 0;
    صحیح		عدد_Matches = 0;
    صحیح		pat_chars = 0;
    score_t	fzy_score;

    *برآمد_Score = 0;

    save_pat = vim_strsave(pat_arg);
    اگر (save_pat == تہی)
	واپس FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    جب (TRUE)
    {
	اگر (matchseq)
	    complete = TRUE;
	ورنہ
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    اگر (*p == NUL)
		توڑو;
	    pat = p;
	    جب (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		اگر (has_mbyte)
		    MB_PTR_ADV(p);
		ورنہ
		    ++p;
	    }
	    اگر (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}
	// match_positions() always writes pat_chars entries,
	// so bail if they won't fit.
	pat_chars = MB_CHARLEN(pat);
	اگر (pat_chars > maxMatches)
	    pat_chars = maxMatches;
	اگر (عدد_Matches > maxMatches - pat_chars)
	{
	    عدد_Matches = 0;
	    *برآمد_Score = FUZZY_SCORE_NONE;
	    توڑو;
	}

	score = FUZZY_SCORE_NONE;
	اگر (has_مطابقت(pat, str))
	{
	    fzy_score = مطابقت_positions(pat, str, matches + عدد_Matches);
	    اگر (fzy_score != SCORE_MIN)
		score = (fzy_score == SCORE_MAX) ? زیادہ_سے_زیادہ_عدد
		    : (fzy_score < 0) ? (صحیح)ceil(fzy_score * SCORE_SCALE - 0.5)
		    : (صحیح)floor(fzy_score * SCORE_SCALE + 0.5);
	}

	اگر (score == FUZZY_SCORE_NONE)
	{
	    عدد_Matches = 0;
	    *برآمد_Score = FUZZY_SCORE_NONE;
	    توڑو;
	}

	اگر (score > 0 && *برآمد_Score > زیادہ_سے_زیادہ_عدد - score)
	    *برآمد_Score = زیادہ_سے_زیادہ_عدد;
	ورنہ اگر (score < 0 && *برآمد_Score < INT_MIN + 1 - score)
	    *برآمد_Score = INT_MIN + 1;
	ورنہ
	    *برآمد_Score += score;

	عدد_Matches += pat_chars;

	اگر (complete || عدد_Matches >= maxMatches)
	    توڑو;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    واپس عدد_Matches != 0;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_PROTO)
/*
 * Sort the fuzzy matches in the descending order of the match score.
 * For items with same score, retain the order using the index (stable sort)
 */
    ساکن صحیح
fuzzy_مطابقت_شے_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    صحیح		v1 = ((fuzzy_شے_T *)s1)->score;
    صحیح		v2 = ((fuzzy_شے_T *)s2)->score;

    اگر (v1 == v2)
    {
	صحیح exact_match1 = FALSE, exact_match2 = FALSE;
	char_u *pat = ((fuzzy_شے_T *)s1)->pat;
	صحیح patlen = (صحیح)STRLEN(pat);
	صحیح startpos = ((fuzzy_شے_T *)s1)->startpos;
	exact_match1 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_شے_T *)s1)->itemstr + startpos, patlen) == 0;
	startpos = ((fuzzy_شے_T *)s2)->startpos;
	exact_match2 = (startpos >= 0) && STRNCMP(pat,
		((fuzzy_شے_T *)s2)->itemstr + startpos, patlen) == 0;

	اگر (exact_match1 == exact_match2)
	{
	    صحیح idx1 = ((fuzzy_شے_T *)s1)->idx;
	    صحیح idx2 = ((fuzzy_شے_T *)s2)->idx;
	    واپس idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
	}
	ورنہ اگر (exact_match2)
	    واپس 1;
	واپس -1;
    }
    ورنہ
	واپس v1 > v2 ? -1 : 1;
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
    ساکن خالی
fuzzy_مطابقت_اندر_list(
    list_T	*l,
    char_u	*str,
    صحیح		matchseq,
    char_u	*کلید,
    callback_T	*شے_cb,
    صحیح		retmatchpos,
    list_T	*fmatchlist,
    طویل	max_matches)
{
    طویل	    len;
    fuzzy_شے_T	    *اشیاء = تہی;
    listitem_T	    *li;
    طویل	    مطابقت_گنتی = 0;
    int_u	    matches[FUZZY_مطابقت_MAX_LEN];

    len = list_len(l);
    اگر (len == 0)
	واپس;
    اگر (max_matches > 0 && len > max_matches)
	len = max_matches;

    اشیاء = ALLOC_CLEAR_MULT(fuzzy_شے_T, len);
    اگر (اشیاء == تہی)
	واپس;

    // For all the string items in items, get the fuzzy matching score
    FOR_ALL_LIST_اشیاء(l, li)
    {
	صحیح		score;
	char_u		*itemstr = تہی;
	char_u		*itemstr_copy = تہی;
	typval_T	rettv;
	صحیح		itemstr_allocate = FALSE;
	list_T		*مطابقت_positions = تہی;

	اگر (max_matches > 0 && مطابقت_گنتی >= max_matches)
	    توڑو;

	rettv.v_type = VAR_UNKNOWN;
	اگر (li->li_tv.v_type == VAR_STRING)	// list of strings
	    itemstr = li->li_tv.vval.v_string;
	ورنہ اگر (li->li_tv.v_type == VAR_DICT
				&& (کلید != تہی || شے_cb->cb_name != تہی))
	{
	    // For a dict, either use the specified key to lookup the string or
	    // use the specified callback function to get the string.
	    اگر (کلید != تہی)
		itemstr = dict_get_string(li->li_tv.vval.v_dict,
							   (حرف *)کلید, FALSE);
	    ورنہ
	    {
		typval_T	argv[2];

		// Invoke the supplied callback (if any) to get the dict item
		li->li_tv.vval.v_dict->dv_refcount++;
		argv[0].v_type = VAR_DICT;
		argv[0].vval.v_dict = li->li_tv.vval.v_dict;
		argv[1].v_type = VAR_UNKNOWN;
		اگر (call_callback(شے_cb, -1, &rettv, 1, argv) != FAIL)
		{
		    اگر (rettv.v_type == VAR_STRING)
		    {
			itemstr = rettv.vval.v_string;
			itemstr_allocate = TRUE;
		    }
		}
		dict_unref(li->li_tv.vval.v_dict);
	    }
	}

	اگر (itemstr != تہی
		&& fuzzy_مطابقت(itemstr, str, matchseq, &score, matches,
						FUZZY_مطابقت_MAX_LEN))
	{
	    اگر (itemstr_allocate)
	    {
		itemstr_copy = vim_strsave(itemstr);
		اگر (itemstr_copy == تہی)
		{
		    clear_tv(&rettv);
		    جاری;
		}
	    }
	    ورنہ
		itemstr_copy = itemstr;

	    // Copy the list of matching positions in itemstr to a list, if
	    // "retmatchpos" is set.
	    اگر (retmatchpos)
	    {
		مطابقت_positions = list_alloc();
		اگر (مطابقت_positions == تہی)
		{
		    اگر (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    جاری;
		}

		// Fill position information
		صحیح	j = 0;
		char_u	*p = str;
		صحیح	success = TRUE;

		جب (*p != NUL && j < FUZZY_مطابقت_MAX_LEN && success)
		{
		    اگر (!VIM_ISWHITE(PTR2CHAR(p)) || matchseq)
		    {
			اگر (list_append_نمبر(مطابقت_positions, matches[j]) == FAIL)
			{
			    success = FALSE;
			    توڑو;
			}
			j++;
		    }
		    اگر (has_mbyte)
			MB_PTR_ADV(p);
		    ورنہ
			++p;
		}

		اگر (!success)
		{
		    list_free(مطابقت_positions);
		    اگر (itemstr_allocate && itemstr_copy)
			vim_free(itemstr_copy);
		    clear_tv(&rettv);
		    جاری;
		}
	    }
	    اشیاء[مطابقت_گنتی].idx = مطابقت_گنتی;
	    اشیاء[مطابقت_گنتی].شے = li;
	    اشیاء[مطابقت_گنتی].score = score;
	    اشیاء[مطابقت_گنتی].pat = str;
	    اشیاء[مطابقت_گنتی].startpos = matches[0];
	    اشیاء[مطابقت_گنتی].itemstr = itemstr_copy;
	    اشیاء[مطابقت_گنتی].itemstr_allocated = itemstr_allocate;
	    اشیاء[مطابقت_گنتی].lmatchpos = مطابقت_positions;

	    ++مطابقت_گنتی;
	}
	clear_tv(&rettv);
    }

    اگر (مطابقت_گنتی > 0)
    {
	list_T		*retlist;

	// Sort the list by the descending order of the match score
	qsort((خالی *)اشیاء, (size_t)مطابقت_گنتی, حجم(fuzzy_شے_T),
		fuzzy_مطابقت_شے_compare);

	// For matchfuzzy(), return a list of matched strings.
	//	    ['str1', 'str2', 'str3']
	// For matchfuzzypos(), return a list with three items.
	// The first item is a list of matched strings. The second item
	// is a list of lists where each list item is a list of matched
	// character positions. The third item is a list of matching scores.
	//	[['str1', 'str2', 'str3'], [[1, 3], [1, 3], [1, 3]]]
	اگر (retmatchpos)
	{
	    li = list_ڈھونڈو(fmatchlist, 0);
	    اگر (li == تہی || li->li_tv.vval.v_list == تہی)
		جاؤ مکمل;
	    retlist = li->li_tv.vval.v_list;
	}
	ورنہ
	    retlist = fmatchlist;

	// Copy the matching strings to the return list
	برائے (صحیح i = 0; i < مطابقت_گنتی; i++)
	{
	    اگر (list_append_tv(retlist, &اشیاء[i].شے->li_tv) == FAIL)
		جاؤ مکمل;
	}

	// next copy the list of matching positions
	اگر (retmatchpos)
	{
	    li = list_ڈھونڈو(fmatchlist, -2);
	    اگر (li == تہی || li->li_tv.vval.v_list == تہی)
		جاؤ مکمل;
	    retlist = li->li_tv.vval.v_list;

	    برائے (صحیح i = 0; i < مطابقت_گنتی; i++)
	    {
		اگر (اشیاء[i].lmatchpos != تہی)
		{
		    اگر (list_append_list(retlist, اشیاء[i].lmatchpos) == OK)
			اشیاء[i].lmatchpos = تہی;
		    ورنہ
			جاؤ مکمل;

		}
	    }

	    // copy the matching scores
	    li = list_ڈھونڈو(fmatchlist, -1);
	    اگر (li == تہی || li->li_tv.vval.v_list == تہی)
		جاؤ مکمل;
	    retlist = li->li_tv.vval.v_list;
	    برائے (صحیح i = 0; i < مطابقت_گنتی; i++)
	    {
		اگر (list_append_نمبر(retlist, اشیاء[i].score) == FAIL)
		    جاؤ مکمل;
	    }
	}
    }

مکمل:
    برائے (صحیح i = 0; i < مطابقت_گنتی; i++)
    {
	اگر (اشیاء[i].itemstr_allocated)
	    vim_free(اشیاء[i].itemstr);

	اگر (اشیاء[i].lmatchpos)
	    list_free(اشیاء[i].lmatchpos);
    }
    vim_free(اشیاء);
}

/*
 * Do fuzzy matching. Returns the list of matched strings in 'rettv'.
 * If 'retmatchpos' is TRUE, also returns the matching character positions.
 */
    ساکن خالی
do_fuzzymatch(typval_T *argvars, typval_T *rettv, صحیح retmatchpos)
{
    callback_T	cb;
    char_u	*کلید = تہی;
    صحیح		ret;
    صحیح		matchseq = FALSE;
    طویل	max_matches = 0;

    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    CLEAR_اشارہ_گر(&cb);

    // validate and get the arguments
    اگر (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == تہی)
    {
	semsg(_(e_دلیل_کا_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	واپس;
    }
    اگر (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == تہی)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	واپس;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    واپس;

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	اگر ((di = dict_ڈھونڈو(d, (char_u *)"key", -1)) != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == تہی
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "key",
						    tv_get_string(&di->di_tv));
		واپس;
	    }
	    کلید = tv_get_string(&di->di_tv);
	}
	ورنہ اگر ((di = dict_ڈھونڈو(d, (char_u *)"text_cb", -1)) != تہی)
	{
	    cb = get_callback(&di->di_tv);
	    اگر (cb.cb_name == تہی)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "text_cb");
		واپس;
	    }
	}

	اگر ((di = dict_ڈھونڈو(d, (char_u *)"limit", -1)) != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_نمبر)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "limit");
		واپس;
	    }
	    max_matches = (طویل)tv_get_نمبر_chk(&di->di_tv, تہی);
	}

	اگر (dict_has_کلید(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    اگر (ret == FAIL)
	جاؤ مکمل;
    اگر (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	اگر (l == تہی)
	    جاؤ مکمل;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    جاؤ مکمل;
	}
	l = list_alloc();
	اگر (l == تہی)
	    جاؤ مکمل;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    جاؤ مکمل;
	}
	l = list_alloc();
	اگر (l == تہی)
	    جاؤ مکمل;
	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    جاؤ مکمل;
	}
    }

    fuzzy_مطابقت_اندر_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, کلید, &cb, retmatchpos, rettv->vval.v_list, max_matches);

مکمل:
    free_callback(&cb);
}

/*
 * "matchfuzzy()" function
 */
    خالی
f_matchfuzzy(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, FALSE);
}

/*
 * "matchfuzzypos()" function
 */
    خالی
f_matchfuzzypos(typval_T *argvars, typval_T *rettv)
{
    do_fuzzymatch(argvars, rettv, TRUE);
}
#اگر_ختم

/*
 * Same as fuzzy_match_item_compare() except for use with a string match
 */
    ساکن صحیح
fuzzy_مطابقت_str_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    صحیح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحیح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحیح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحیح		idx2 = ((fuzmatch_str_T *)s2)->idx;

    اگر (v1 == v2)
	واپس idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ورنہ
	واپس v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches by score
 */
    ساکن خالی
fuzzy_مطابقت_str_ترتیب_دو(fuzmatch_str_T *fm, صحیح sz)
{
    // Sort the list by the descending order of the match score
    qsort((خالی *)fm, (size_t)sz, حجم(fuzmatch_str_T),
	    fuzzy_مطابقت_str_compare);
}

/*
 * Same as fuzzy_match_item_compare() except for use with a function name
 * string match. <SNR> functions should be sorted to the end.
 */
    ساکن صحیح
fuzzy_مطابقت_func_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    صحیح		v1 = ((fuzmatch_str_T *)s1)->score;
    صحیح		v2 = ((fuzmatch_str_T *)s2)->score;
    صحیح		idx1 = ((fuzmatch_str_T *)s1)->idx;
    صحیح		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    اگر (*str1 != '<' && *str2 == '<')
	واپس -1;
    اگر (*str1 == '<' && *str2 != '<')
	واپس 1;
    اگر (v1 == v2)
	واپس idx1 == idx2 ? 0 : idx1 > idx2 ? 1 : -1;
    ورنہ
	واپس v1 > v2 ? -1 : 1;
}

/*
 * Sort fuzzy matches of function names by score.
 * <SNR> functions should be sorted to the end.
 */
    ساکن خالی
fuzzy_مطابقت_func_ترتیب_دو(fuzmatch_str_T *fm, صحیح sz)
{
    // Sort the list by the descending order of the match score
    qsort((خالی *)fm, (size_t)sz, حجم(fuzmatch_str_T),
		fuzzy_مطابقت_func_compare);
}

/*
 * Fuzzy match 'pat' in 'str'. Returns 0 if there is no match. Otherwise,
 * returns the match score.
 */
    صحیح
fuzzy_مطابقت_str(char_u *str, char_u *pat)
{
    صحیح		score = FUZZY_SCORE_NONE;
    int_u	matchpos[FUZZY_مطابقت_MAX_LEN];

    اگر (str == تہی || pat == تہی)
	واپس score;

    fuzzy_مطابقت(str, pat, TRUE, &score, matchpos,
				حجم(matchpos) / حجم(matchpos[0]));

    واپس score;
}

/*
 * Fuzzy match the position of string 'pat' in string 'str'.
 * Returns a dynamic array of matching positions. If there is no match,
 * returns NULL.
 */
    garray_T *
fuzzy_مطابقت_str_with_pos(char_u *str غیر_مستعمل, char_u *pat غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    صحیح		    score = FUZZY_SCORE_NONE;
    garray_T	    *مطابقت_positions = تہی;
    int_u	    matches[FUZZY_مطابقت_MAX_LEN];
    صحیح		    j = 0;

    اگر (str == تہی || pat == تہی)
	واپس تہی;

    مطابقت_positions = ALLOC_ONE(garray_T);
    اگر (مطابقت_positions == تہی)
	واپس تہی;
    ga_init2(مطابقت_positions, حجم(int_u), 10);

    اگر (!fuzzy_مطابقت(str, pat, FALSE, &score, matches, FUZZY_مطابقت_MAX_LEN)
	    || score == FUZZY_SCORE_NONE)
    {
	ga_clear(مطابقت_positions);
	vim_free(مطابقت_positions);
	واپس تہی;
    }

    برائے (char_u *p = pat; *p != NUL; MB_PTR_ADV(p))
    {
	اگر (!VIM_ISWHITE(PTR2CHAR(p)))
	{
	    اگر (ga_grow(مطابقت_positions, 1) == FAIL)
	    {
		ga_clear(مطابقت_positions);
		vim_free(مطابقت_positions);
		واپس تہی;
	    }
	    ((int_u *)مطابقت_positions->ga_ڈیٹا)[مطابقت_positions->ga_len] =
								    matches[j];
	    مطابقت_positions->ga_len++;
	    j++;
	}
    }

    واپس مطابقت_positions;
#ورنہ
    واپس تہی;
#اگر_ختم
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
    صحیح
fuzzy_مطابقت_str_اندر_سطر(
    char_u	**ptr,
    char_u	*pat,
    صحیح		*len,
    pos_T	*current_pos,
    صحیح		*score)
{
    char_u	*str = *ptr;
    char_u	*strBegin = str;
    char_u	*انتہا = تہی;
    char_u	*آغاز = تہی;
    صحیح		ملا = FALSE;
    حرف	save_انتہا;
    char_u	*سطر_انتہا = تہی;

    اگر (str == تہی || pat == تہی)
	واپس ملا;
    سطر_انتہا = ڈھونڈو_سطر_انتہا(str);

    جب (str < سطر_انتہا)
    {
	// Skip non-word characters
	آغاز = ڈھونڈو_لفظ_آغاز(str);
	اگر (*آغاز == NUL)
	    توڑو;
	انتہا = ڈھونڈو_لفظ_انتہا(آغاز);

	// Extract the word from start to end
	save_انتہا = *انتہا;
	*انتہا = NUL;

	// Perform fuzzy match
	*score = fuzzy_مطابقت_str(آغاز, pat);
	*انتہا = save_انتہا;

	اگر (*score != FUZZY_SCORE_NONE)
	{
	    *len = (صحیح)(انتہا - آغاز);
	    ملا = TRUE;
	    *ptr = آغاز;
	    اگر (current_pos)
		current_pos->ستون += (صحیح)(انتہا - strBegin);
	    توڑو;
	}

	// Move to the end of the current word for the next iteration
	str = انتہا;
	// Ensure we continue searching after the current word
	جب (*str != NUL && !vim_iswordp(str))
	    MB_PTR_ADV(str);
    }

    اگر (!ملا)
	*ptr = سطر_انتہا;

    واپس ملا;
}

/*
 * Search for the next fuzzy match in the specified buffer.
 * This function attempts to find the next occurrence of the given pattern
 * in the buffer, starting from the current position. It handles line wrapping
 * and direction of search.
 *
 * Return TRUE if a match is found, otherwise FALSE.
 */
    صحیح
تلاش_for_fuzzy_مطابقت(
    buf_T	*buf,
    pos_T	*pos,
    char_u	*نمونہ,
    صحیح		dir,
    pos_T	*آغاز_pos,
    صحیح		*len,
    char_u	**ptr,
    صحیح		*score)
{
    pos_T	current_pos = *pos;
    pos_T	circly_انتہا;
    صحیح		ملا_new_مطابقت = FALSE;
    صحیح		looped_around = FALSE;
    صحیح		پورا_سطر = ctrl_x_منوال_پورا_سطر();

    اگر (buf == curbuf)
	circly_انتہا = *آغاز_pos;
    ورنہ
    {
	circly_انتہا.lnum = buf->b_ml.ml_سطر_گنتی;
	circly_انتہا.ستون = 0;
	circly_انتہا.coladd = 0;
    }

    اگر (پورا_سطر && آغاز_pos->lnum != pos->lnum)
	current_pos.lnum += dir;

    کرو
    {

	// Check if looped around and back to start position
	اگر (looped_around && (پورا_سطر ? current_pos.lnum == circly_انتہا.lnum
					: EQUAL_POS(current_pos, circly_انتہا)))
	    توڑو;

	// Ensure current_pos is valid
	اگر (current_pos.lnum >= 1 && current_pos.lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    // Get the current line buffer
	    *ptr = ml_get_buf(buf, current_pos.lnum, FALSE);
	    اگر (!پورا_سطر)
		*ptr += current_pos.ستون;

	    // If ptr is end of line is reached, move to next line
	    // or previous line based on direction
	    اگر (*ptr != تہی && **ptr != NUL)
	    {
		اگر (!پورا_سطر)
		{
		    // Try to find a fuzzy match in the current line starting
		    // from current position
		    ملا_new_مطابقت = fuzzy_مطابقت_str_اندر_سطر(ptr, نمونہ,
						    len, &current_pos, score);
		    اگر (ملا_new_مطابقت)
		    {
			*pos = current_pos;
			توڑو;
		    }
		    ورنہ اگر (looped_around && current_pos.lnum == circly_انتہا.lnum)
			توڑو;
		}
		ورنہ
		{
		    char_u *سطر = *ptr;
		    char_u *p = skipwhite(سطر);
		    اگر (fuzzy_مطابقت_str(p, نمونہ) != FUZZY_SCORE_NONE)
		    {
			ملا_new_مطابقت = TRUE;
			*pos = current_pos;
			*ptr = p;
			*len = (صحیح)ml_get_buf_len(buf, current_pos.lnum) -
								(صحیح)(p - سطر);
			توڑو;
		    }
		}
	    }
	}

	// Move to the next line or previous line based on direction
	اگر (dir == FORWARD)
	{
	    اگر (++current_pos.lnum > buf->b_ml.ml_سطر_گنتی)
	    {
		اگر (p_ws)
		{
		    current_pos.lnum = 1;
		    looped_around = TRUE;
		}
		ورنہ
		    توڑو;
	    }
	}
	ورنہ
	{
	    اگر (--current_pos.lnum < 1)
	    {
		اگر (p_ws)
		{
		    current_pos.lnum = buf->b_ml.ml_سطر_گنتی;
		    looped_around = TRUE;
		}
		ورنہ
		    توڑو;

	    }
	}
	current_pos.ستون = 0;
    } جب (TRUE);

    واپس ملا_new_مطابقت;
}

/*
 * Free an array of fuzzy string matches "fuzmatch[count]".
 */
    خالی
fuzmatch_str_free(fuzmatch_str_T *fuzmatch, صحیح گنتی)
{
    اگر (fuzmatch == تہی)
	واپس;

    برائے (صحیح i = 0; i < گنتی; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}

/*
 * Copy a list of fuzzy matches into a string list after sorting the matches by
 * the fuzzy score. Frees the memory allocated for 'fuzmatch'.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحیح
fuzzymatches_مقصد_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	صحیح		گنتی,
	صحیح		funcsort)
{
    اگر (گنتی <= 0)
	جاؤ theend;

    *matches = ALLOC_MULT(char_u *, گنتی);
    اگر (*matches == تہی)
    {
	fuzmatch_str_free(fuzmatch, گنتی);
	واپس FAIL;
    }

    // Sort the list by the descending order of the match score
    اگر (funcsort)
	fuzzy_مطابقت_func_ترتیب_دو((خالی *)fuzmatch, (size_t)گنتی);
    ورنہ
	fuzzy_مطابقت_str_ترتیب_دو((خالی *)fuzmatch, (size_t)گنتی);

    برائے (صحیح i = 0; i < گنتی; i++)
	(*matches)[i] = fuzmatch[i].str;

theend:
    vim_free(fuzmatch);
    واپس OK;
}

/*
 * Fuzzy match algorithm ported from https://github.com/jhawthorn/fzy.
 * This implementation extends the original by supporting multibyte characters.
 */

#تعریف_کرو مطابقت_MAX_LEN FUZZY_مطابقت_MAX_LEN

#تعریف_کرو SCORE_GAP_LEADING -0.005
#تعریف_کرو SCORE_GAP_TRAILING -0.005
#تعریف_کرو SCORE_GAP_INNER -0.01
#تعریف_کرو SCORE_مطابقت_CONSECUTIVE 1.0
#تعریف_کرو SCORE_مطابقت_SLASH 0.9
#تعریف_کرو SCORE_مطابقت_لفظ 0.8
#تعریف_کرو SCORE_مطابقت_CAPITAL 0.7
#تعریف_کرو SCORE_مطابقت_DOT 0.6

    ساکن صحیح
has_مطابقت(char_u *needle, char_u *haystack)
{
    اگر (!needle || !haystack || !*needle)
	واپس FAIL;

    char_u *n_ptr = needle;
    char_u *h_ptr = haystack;

    جب (*n_ptr)
    {
	صحیح n_char = mb_ptr2char(n_ptr);
	صحیح n_بالائی = MB_TOUPPER(n_char);
	صحیح ملا = FALSE;

	جب (*h_ptr)
	{
	    صحیح h_char = mb_ptr2char(h_ptr);
	    اگر (h_char == n_char || h_char == n_بالائی)
	    {
		ملا = TRUE;
		h_ptr += mb_ptr2len(h_ptr);
		توڑو;
	    }
	    h_ptr += mb_ptr2len(h_ptr);
	}

	اگر (!ملا)
	    واپس FAIL;

	n_ptr += mb_ptr2len(n_ptr);
    }

    واپس OK;
}

قسمتعریف ساخت مطابقت_struct
{
    صحیح needle_len;
    صحیح haystack_len;
    صحیح زیریں_needle[مطابقت_MAX_LEN];     // stores codepoints
    صحیح زیریں_haystack[مطابقت_MAX_LEN];   // stores codepoints
    score_t مطابقت_bonus[مطابقت_MAX_LEN];
} مطابقت_struct;

#تعریف_کرو IS_لفظ_SEP(c) ((c) == '-' || (c) == '_' || (c) == ' ')
#تعریف_کرو IS_راستہ_SEP(c) ((c) == '/')
#تعریف_کرو IS_DOT(c)      ((c) == '.')

    ساکن score_t
compute_bonus_codepoint(صحیح آخری_c, صحیح c)
{
    اگر (ASCII_ISALNUM(c) || vim_iswordc(c))
    {
	اگر (IS_راستہ_SEP(آخری_c))
	    واپس SCORE_مطابقت_SLASH;
	اگر (IS_لفظ_SEP(آخری_c))
	    واپس SCORE_مطابقت_لفظ;
	اگر (IS_DOT(آخری_c))
	    واپس SCORE_مطابقت_DOT;
	اگر (MB_ISUPPER(c) && MB_ISLOWER(آخری_c))
	    واپس SCORE_مطابقت_CAPITAL;
    }
    واپس 0;
}

    ساکن خالی
تیاری_مطابقت_struct(مطابقت_struct *مطابقت, char_u *needle, char_u *haystack)
{
    صحیح i = 0;
    char_u *p = needle;
    جب (*p != NUL && i < مطابقت_MAX_LEN)
    {
	صحیح c = mb_ptr2char(p);
	مطابقت->زیریں_needle[i++] = MB_TOLOWER(c);
	MB_PTR_ADV(p);
    }
    مطابقت->needle_len = i;

    i = 0;
    p = haystack;
    صحیح پچھلا_c = '/';
    جب (*p != NUL && i < مطابقت_MAX_LEN)
    {
	صحیح c = mb_ptr2char(p);
	مطابقت->زیریں_haystack[i] = MB_TOLOWER(c);
	مطابقت->مطابقت_bonus[i] = compute_bonus_codepoint(پچھلا_c, c);
	پچھلا_c = c;
	MB_PTR_ADV(p);
	i++;
    }
    مطابقت->haystack_len = i;
}

    ساکن اندرونی خالی
مطابقت_صف(ثابت مطابقت_struct *مطابقت, صحیح صف, score_t *curr_D,
	score_t *curr_M, ثابت score_t *آخری_D, ثابت score_t *آخری_M)
{
    صحیح n = مطابقت->needle_len;
    صحیح m = مطابقت->haystack_len;
    صحیح i = صف;

    ثابت صحیح *زیریں_needle = مطابقت->زیریں_needle;
    ثابت صحیح *زیریں_haystack = مطابقت->زیریں_haystack;
    ثابت score_t *مطابقت_bonus = مطابقت->مطابقت_bonus;

    score_t پچھلا_score = SCORE_MIN;
    score_t gap_score = i == n - 1 ? SCORE_GAP_TRAILING : SCORE_GAP_INNER;

    // These will not be used with this value, but not all compilers see it
    score_t پچھلا_M = SCORE_MIN, پچھلا_D = SCORE_MIN;

    برائے (صحیح j = 0; j < m; j++)
    {
	اگر (زیریں_needle[i] == زیریں_haystack[j])
	{
	    score_t score = SCORE_MIN;
	    اگر (!i)
	    {
		score = (j * SCORE_GAP_LEADING) + مطابقت_bonus[j];
	    }
	    ورنہ اگر (j)
	    { /* i > 0 && j > 0*/
		score = MAX(
			پچھلا_M + مطابقت_bonus[j],
			// consecutive match, doesn't stack with match_bonus
			پچھلا_D + SCORE_مطابقت_CONSECUTIVE);
	    }
	    پچھلا_D = آخری_D[j];
	    پچھلا_M = آخری_M[j];
	    curr_D[j] = score;
	    curr_M[j] = پچھلا_score = MAX(score, پچھلا_score + gap_score);
	}
	ورنہ
	{
	    پچھلا_D = آخری_D[j];
	    پچھلا_M = آخری_M[j];
	    curr_D[j] = SCORE_MIN;
	    curr_M[j] = پچھلا_score = پچھلا_score + gap_score;
	}
    }
}

    ساکن score_t
مطابقت_positions(char_u *needle, char_u *haystack, int_u *positions)
{
    اگر (!needle || !haystack || !*needle)
	واپس SCORE_MIN;

    مطابقت_struct مطابقت;
    تیاری_مطابقت_struct(&مطابقت, needle, haystack);

    صحیح n = مطابقت.needle_len;
    صحیح m = مطابقت.haystack_len;

    اگر (m > مطابقت_MAX_LEN || n > m)
    {
	// Unreasonably large candidate: return no score
	واپس SCORE_MIN;
    }
    ورنہ اگر (n == m)
    {
	// Since this method can only be called with a haystack which
	// matches needle. If the lengths of the strings are equal the
	// strings themselves must also be equal (ignoring case).
	// After truncation to MATCH_MAX_LEN n == m can also happen for
	// unequal strings, so check before taking the shortcut.
	bool equal = true;
	برائے (صحیح i = 0; i < n; i++)
	{
	    اگر (مطابقت.زیریں_needle[i] != مطابقت.زیریں_haystack[i])
	    {
		equal = false;
		توڑو;
	    }
	}
	اگر (equal)
	{
	    اگر (positions)
	    {
		برائے (صحیح i = 0; i < n; i++)
		    positions[i] = i;
	    }
	    واپس SCORE_MAX;
	}
    }

    // ensure n * MATCH_MAX_LEN * 2 won't overflow
    اگر ((size_t)n > (سائز_MAX / حجم(score_t)) / مطابقت_MAX_LEN / 2)
	واپس SCORE_MIN;

    // Allocate for both D and M matrices in one contiguous block
    score_t *بلاک = (score_t*)alloc(حجم(score_t) * مطابقت_MAX_LEN * n * 2);
    اگر (!بلاک)
	واپس SCORE_MIN;

    // D[][] Stores the best score for this position ending with a match.
    // M[][] Stores the best possible score at this position.
    score_t (*D)[مطابقت_MAX_LEN] = (score_t(*)[مطابقت_MAX_LEN])بلاک;
    score_t (*M)[مطابقت_MAX_LEN] = (score_t(*)[مطابقت_MAX_LEN])(بلاک
							+ مطابقت_MAX_LEN * n);

    مطابقت_صف(&مطابقت, 0, D[0], M[0], D[0], M[0]);
    برائے (صحیح i = 1; i < n; i++)
	مطابقت_صف(&مطابقت, i, D[i], M[i], D[i - 1], M[i - 1]);

    // backtrace to find the positions of optimal matching
    اگر (positions)
    {
	صحیح مطابقت_required = 0;
	برائے (صحیح i = n - 1, j = m - 1; i >= 0; i--)
	{
	    برائے (; j >= 0; j--)
	    {
		// There may be multiple paths which result in
		// the optimal weight.
		//
		// For simplicity, we will pick the first one
		// we encounter, the latest in the candidate
		// string.
		اگر (D[i][j] != SCORE_MIN &&
			(مطابقت_required || D[i][j] == M[i][j]))
		{
		    // If this score was determined using
		    // SCORE_MATCH_CONSECUTIVE, the
		    // previous character MUST be a match
		    مطابقت_required = i && j &&
			M[i][j] == D[i - 1][j - 1] + SCORE_مطابقت_CONSECUTIVE;
		    positions[i] = j--;
		    توڑو;
		}
	    }
	}
    }

    score_t نتیجہ = M[n - 1][m - 1];
    vim_free(بلاک);
    واپس نتیجہ;
}
