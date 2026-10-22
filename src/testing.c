/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * testing.c: Support for tests.
 */

#include "vim.h"

#ถ้า defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    static void
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ถ้า (sname != NULL)
    {
	ga_concat(gap, sname);
	ถ้า (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ถ้า (SOURCING_LNUM > 0)
    {
	var_na    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, parimaa_na(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ถ้า (sname != NULL || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    static void
ga_concat_esc(garray_T *gap, char_u *p, จำนวนเต็ม clen)
{
    char_u  buf[NUMBUFLEN];

    ถ้า (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	คืน;
    }

    switch (*p)
    {
	pak_sa BS: GA_CONCAT_LITERAL(gap, "\\b"); bha_nga;
	pak_sa ESC: GA_CONCAT_LITERAL(gap, "\\e"); bha_nga;
	pak_sa FF: GA_CONCAT_LITERAL(gap, "\\f"); bha_nga;
	pak_sa NL: GA_CONCAT_LITERAL(gap, "\\n"); bha_nga;
	pak_sa TAB: GA_CONCAT_LITERAL(gap, "\\t"); bha_nga;
	pak_sa CAR: GA_CONCAT_LITERAL(gap, "\\r"); bha_nga;
	pak_sa '\\': GA_CONCAT_LITERAL(gap, "\\\\"); bha_nga;
	default:
	   ถ้า (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((var_na *)buf, parimaa_na(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   มิฉะนั้น
	       ga_append(gap, *p);
	   bha_nga;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    static void
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    จำนวนเต็ม	    c;
    จำนวนเต็ม	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    จำนวนเต็ม	    same_len;

    ถ้า (str == NULL)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	คืน;
    }

    สำหรับ (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ขณะ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ถ้า (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((var_na *)buf, parimaa_na(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	มิฉะนั้น
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    static void
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_msg_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    จำนวนเต็ม		did_copy = FALSE;
    จำนวนเต็ม		omitted = 0;

    ถ้า (opt_msg_tv->v_type != VAR_UNKNOWN
	    && !(opt_msg_tv->v_type == VAR_STRING
		&& (opt_msg_tv->vval.v_string == NULL
		    || *opt_msg_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_msg_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ถ้า (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    มิฉะนั้น ถ้า (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    มิฉะนั้น
	GA_CONCAT_LITERAL(gap, "Expected ");
    ถ้า (exp_str == NULL)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ถ้า (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != NULL && got_tv->vval.v_dict != NULL)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    จำนวนเต็ม		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ถ้า (exp_tv->vval.v_dict == NULL || got_tv->vval.v_dict == NULL)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = NULL;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = NULL;
		คืน;
	    }

	    todo = (จำนวนเต็ม)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&exp_d->dv_hashtab, hi, todo)
	    {
		ถ้า (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_key, -1);
		    ถ้า (item2 == NULL || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(var_na *)hi->hi_key, &HI2DI(hi)->di_tv);
			ถ้า (item2 != NULL)
			    dict_add_tv(got_tv->vval.v_dict,
					    (var_na *)hi->hi_key, &item2->di_tv);
		    }
		    มิฉะนั้น
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (จำนวนเต็ม)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&got_d->dv_hashtab, hi, todo)
	    {
		ถ้า (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_key, -1);
		    ถ้า (item2 == NULL)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(var_na *)hi->hi_key, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    มิฉะนั้น
    {
	ถ้า (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ถ้า (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ถ้า (atype != ASSERT_NOTEQUAL)
    {
	ถ้า (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	มิฉะนั้น ถ้า (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	มิฉะนั้น
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ถ้า (omitted != 0)
	{
	    var_na    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, parimaa_na(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ถ้า (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    static จำนวนเต็ม
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ถ้า (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], NULL, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	คืน 1;
    }
    คืน 0;
}

    static จำนวนเต็ม
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	คืน 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    ถ้า (pat != NULL && text != NULL
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], NULL, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	คืน 1;
    }
    คืน 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    static จำนวนเต็ม
assert_bool(typval_T *argvars, จำนวนเต็ม isTrue)
{
    จำนวนเต็ม		error = FALSE;
    garray_T	ga;

    ถ้า (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_number == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	คืน 0;
    ถ้า (argvars[0].v_type != VAR_NUMBER
	    || (tv_get_number_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		NULL, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	คืน 1;
    }
    คืน 0;
}

    static void
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ถ้า (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    มิฉะนั้น
	ga_concat(gap, cmd);
}

    static จำนวนเต็ม
assert_beeps(typval_T *argvars, จำนวนเต็ม no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    จำนวนเต็ม		ret = 0;

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ถ้า (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ถ้า (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	มิฉะนั้น
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    คืน ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    void
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    rettv->vval.v_number = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    void
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    rettv->vval.v_number = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    void
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_equal_common(argvars, ASSERT_EQUAL);
}

    static จำนวนเต็ม
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    var_na	line1[200];
    var_na	line2[200];
    จำนวนเต็ม		lineidx = 0;
    size_t	IObufflen;

    ถ้า (fname1 == NULL || fname2 == NULL)
	คืน 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((var_na *)fname1, READBIN);
    ถ้า (fd1 == NULL)
    {
	IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
	    (var_na *)e_cant_read_file_str, fname1);
    }
    มิฉะนั้น
    {
	fd2 = mch_fopen((var_na *)fname2, READBIN);
	ถ้า (fd2 == NULL)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
		(var_na *)e_cant_read_file_str, fname2);
	}
	มิฉะนั้น
	{
	    จำนวนเต็ม	    c1, c2;
	    long    count = 0;
	    long    linecount = 1;

	    สำหรับ (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ถ้า (c1 == EOF)
		{
		    ถ้า (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    bha_nga;
		}
		มิฉะนั้น ถ้า (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    bha_nga;
		}
		มิฉะนั้น
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ถ้า (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", count, linecount);
			bha_nga;
		    }
		}
		++count;
		ถ้า (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		มิฉะนั้น ถ้า (lineidx + 2 == (จำนวนเต็ม)parimaa_na(line1))
		{
		    mch_memmove(line1, line1 + 100, lineidx - 100);
		    mch_memmove(line2, line2 + 100, lineidx - 100);
		    lineidx -= 100;
		}
	    }
	    fclose(fd1);
	    fclose(fd2);
	}
    }

    ถ้า (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ถ้า (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ถ้า (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	คืน 1;
    }

    คืน 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    void
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	คืน;

    rettv->vval.v_number = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    void
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    void
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	คืน;

    error = tv_get_string_chk(&argvars[0]);
    ถ้า (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    มิฉะนั้น ถ้า (error != NULL
	&& strstr((var_na *)get_vim_var_str(VV_EXCEPTION), (var_na *)error) == NULL)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], NULL, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    void
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    จำนวนเต็ม		save_trylevel = trylevel;
    จำนวนเต็ม		called_emsg_before = called_emsg;
    var_na	*wrong_arg_msg = NULL;
    char_u	*tofree = NULL;

    ถ้า (check_for_string_or_number_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	คืน;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    in_assert_fails = TRUE;
    ++no_wait_return;

    char_u *cmd = tv_get_string_chk(&argvars[0]);
    do_cmdline_cmd(cmd);

    // reset here for any errors reported below
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;

    ถ้า (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    มิฉะนั้น ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = NULL;
	จำนวนเต็ม	error_found = FALSE;
	จำนวนเต็ม	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_msg == NULL ? (char_u *)"[unknown]"
						       : emsg_assert_fails_msg;

	ถ้า (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == NULL
			   || strstr((var_na *)actual, (var_na *)expected) == NULL;
	}
	มิฉะนั้น ถ้า (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ถ้า (list == NULL || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_msg = e_assert_fails_second_arg;
		goto theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ถ้า (expected == NULL)
		goto theend;
	    ถ้า (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    มิฉะนั้น ถ้า (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ถ้า (actual != NULL)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ถ้า (expected == NULL)
			goto theend;
		    ถ้า (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	มิฉะนั้น
	{
	    wrong_arg_msg = e_assert_fails_second_arg;
	    goto theend;
	}

	ถ้า (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ถ้า (argvars[3].v_type != VAR_NUMBER)
	    {
		wrong_arg_msg = e_assert_fails_fourth_argument;
		goto theend;
	    }
	    มิฉะนั้น ถ้า (argvars[3].vval.v_number >= 0
			 && argvars[3].vval.v_number != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    ถ้า (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ถ้า (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_msg = e_assert_fails_fifth_argument;
		    goto theend;
		}
		มิฉะนั้น ถ้า (argvars[4].vval.v_string != NULL
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	ถ้า (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ถ้า (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_NUMBER;
		actual_tv.vval.v_number = emsg_assert_fails_lnum;
	    }
	    มิฉะนั้น ถ้า (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    มิฉะนั้น
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_found_index], &actual_tv, ASSERT_FAILS);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    assert_append_cmd_or_arg(&ga, argvars, cmd);
	    assert_error(&ga);
	    ga_clear(&ga);
	    rettv->vval.v_number = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    msg_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    msg_scrolled = 0;
    lines_left = Rows;
    VIM_CLEAR(emsg_assert_fails_msg);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, NULL, 0);
    ถ้า (wrong_arg_msg != NULL)
	emsg(_(wrong_arg_msg));
}

/*
 * "assert_false(actual[, msg])" function
 */
    void
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_bool(argvars, FALSE);
}

    static จำนวนเต็ม
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    จำนวนเต็ม		error = FALSE;
    char_u	expected_str[200];

    ถ้า (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ถ้า (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((var_na *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, NULL,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    คืน 1;
	}
    }
    มิฉะนั้น
    {
	varnumber_T	lower = tv_get_number_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_number_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_number_chk(&argvars[2], &error);

	ถ้า (error)
	    คืน 0;
	ถ้า (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((var_na *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, NULL,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    คืน 1;
	}
    }
    คืน 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    void
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ถ้า (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	คืน;

    rettv->vval.v_number = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    void
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    void
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    void
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_number = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    void
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    void
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	คืน;

    ถ้า (argvars[0].v_type != VAR_NUMBER
	    || argvars[0].vval.v_number <= 0
	    || argvars[1].v_type != VAR_NUMBER
	    || argvars[1].vval.v_number < 0
	    || argvars[2].v_type != VAR_NUMBER)
	emsg(_(e_invalid_argument));
    มิฉะนั้น
    {
	alloc_fail_id = argvars[0].vval.v_number;
	ถ้า (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_number;
	alloc_fail_repeat = argvars[2].vval.v_number;
	did_outofmem_msg = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    void
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ถ้า defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    void
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_INPUT_BUF
    char_u	*val;

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ถ้า (!gui.in_use)
	คืน;
# endif

    ถ้า (val != NULL)
    {
	trash_input_buf();
	add_to_input_buf_csi(val, (จำนวนเต็ม)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    void
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ถ้า (check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    name = tv_get_string(&argvars[0]);

    ถ้า (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_number = need_fileinfo;
    มิฉะนั้น
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    void
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    ถ้า (check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    name = tv_get_string(&argvars[0]);
    ถ้า (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    void
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    จำนวนเต็ม     val;
    static จำนวนเต็ม save_starting = -1;

    ถ้า (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL)
	คืน;

    name = tv_get_string(&argvars[0]);
    val = (จำนวนเต็ม)tv_get_number(&argvars[1]);

    ถ้า (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"starting") == 0)
    {
	ถ้า (val)
	{
	    ถ้า (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	มิฉะนั้น
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_lines = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    มิฉะนั้น ถ้า (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_term_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	ถ้า (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    มิฉะนั้น
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    void
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    จำนวนเต็ม retval = -1;

    switch (argvars[0].v_type)
    {
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_FLOAT:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_STRING:
	pak_sa VAR_INSTR:
	    bha_nga;

	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ถ้า (argvars[0].vval.v_job != NULL)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    bha_nga;
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ถ้า (argvars[0].vval.v_channel != NULL)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    bha_nga;
	pak_sa VAR_FUNC:
	    ถ้า (argvars[0].vval.v_string != NULL)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		ถ้า (fp != NULL)
		    retval = fp->uf_refcount;
	    }
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    ถ้า (argvars[0].vval.v_partial != NULL)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    bha_nga;
	pak_sa VAR_BLOB:
	    ถ้า (argvars[0].vval.v_blob != NULL)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    bha_nga;
	pak_sa VAR_LIST:
	    ถ้า (argvars[0].vval.v_list != NULL)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ถ้า (argvars[0].vval.v_tuple != NULL)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    bha_nga;
	pak_sa VAR_DICT:
	    ถ้า (argvars[0].vval.v_dict != NULL)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    bha_nga;
	pak_sa VAR_CLASS:
	    ถ้า (argvars[0].vval.v_class != NULL)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    bha_nga;
	pak_sa VAR_OBJECT:
	    ถ้า (argvars[0].vval.v_object != NULL)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    bha_nga;
	pak_sa VAR_TYPEALIAS:
	    ถ้า (argvars[0].vval.v_typealias != NULL)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    bha_nga;
    }

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    void
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ถ้า (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    มิฉะนั้น
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    void
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    void
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    void
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = NULL;
}

#ถ้า defined(FEAT_JOB_CHANNEL)
    void
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = NULL;
}
#endif

    void
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, NULL);
}

#ถ้า defined(FEAT_JOB_CHANNEL)
    void
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = NULL;
}
#endif

    void
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, NULL);
}

    void
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = NULL;
}

    void
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = NULL;
}

    void
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
}

    void
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, NULL);
}

    void
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    void
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    void
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	คืน;

    ถ้า (argvars[0].v_type != VAR_NUMBER || argvars[1].v_type != VAR_NUMBER)
    {
	emsg(_(e_invalid_argument));
	คืน;
    }

    mouse_row = (time_t)tv_get_number(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_number(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    static จำนวนเต็ม
test_gui_drop_files(dict_T *args UNUSED)
{
# ถ้า defined(HAVE_DROP_FILE)
    จำนวนเต็ม		row;
    จำนวนเต็ม		col;
    int_u	mods;
    char_u	**fnames;
    จำนวนเต็ม		count = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ถ้า (!dict_has_key(args, "files")
	    || !dict_has_key(args, "row")
	    || !dict_has_key(args, "col")
	    || !dict_has_key(args, "modifiers"))
	คืน FALSE;

    (void)dict_get_tv(args, "files", &t);
    row = (จำนวนเต็ม)dict_get_number(args, "row");
    col = (จำนวนเต็ม)dict_get_number(args, "col");
    mods = (จำนวนเต็ม)dict_get_number(args, "modifiers");

    ถ้า (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	คืน FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ถ้า (fnames == NULL)
	คืน FALSE;

    FOR_ALL_LIST_ITEMS(l, li)
    {
	// ignore non-string items
	ถ้า (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == NULL)
	    continue;

	fnames[count] = vim_strsave(li->li_tv.vval.v_string);
	ถ้า (fnames[count] == NULL)
	{
	    ขณะ (--count >= 0)
		vim_free(fnames[count]);
	    vim_free(fnames);
	    คืน FALSE;
	}
	count++;
    }

    ถ้า (count > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, count);
    มิฉะนั้น
	vim_free(fnames);
# endif

    คืน TRUE;
}

# ถ้า defined(FIND_REPLACE_DIALOG)
    static จำนวนเต็ม
test_gui_find_repl(dict_T *args)
{
    จำนวนเต็ม		flags;
    char_u	*find_text;
    char_u	*repl_text;
    จำนวนเต็ม		forward;
    จำนวนเต็ม		retval;

    ถ้า (!dict_has_key(args, "find_text")
	    || !dict_has_key(args, "repl_text")
	    || !dict_has_key(args, "flags")
	    || !dict_has_key(args, "forward"))
	คืน FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (จำนวนเต็ม)dict_get_number(args, "flags");
    forward = (จำนวนเต็ม)dict_get_number(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    คืน retval;
}
# endif

    static จำนวนเต็ม
test_gui_mouse_event(dict_T *args)
{
    จำนวนเต็ม		button;
    จำนวนเต็ม		row;
    จำนวนเต็ม		col;
    จำนวนเต็ม		repeated_click;
    int_u	mods;
    จำนวนเต็ม		move;

    ถ้า (!dict_has_key(args, "row")
	    || !dict_has_key(args, "col"))
	คืน FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (จำนวนเต็ม)dict_get_bool(args, "move", FALSE);

    ถ้า (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	คืน FALSE;

    row = (จำนวนเต็ม)dict_get_number(args, "row");
    col = (จำนวนเต็ม)dict_get_number(args, "col");

    ถ้า (move)
    {
	จำนวนเต็ม pY = row;
	จำนวนเต็ม pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ถ้า (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    มิฉะนั้น
    {
	button = (จำนวนเต็ม)dict_get_number(args, "button");
	repeated_click = (จำนวนเต็ม)dict_get_number(args, "multiclick");
	mods = (จำนวนเต็ม)dict_get_number(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    คืน TRUE;
}

    static จำนวนเต็ม
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	value;
    จำนวนเต็ม		dragging;
    scrollbar_T *sb = NULL;

    ถ้า (!dict_has_key(args, "which")
	    || !dict_has_key(args, "value")
	    || !dict_has_key(args, "dragging"))
	คืน FALSE;

    which = dict_get_string(args, "which", FALSE);
    value = (long)dict_get_number(args, "value");
    dragging = (จำนวนเต็ม)dict_get_number(args, "dragging");

    ถ้า (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_LEFT];
    มิฉะนั้น ถ้า (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_RIGHT];
    มิฉะนั้น ถ้า (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ถ้า (sb == NULL)
    {
	semsg(_(e_invalid_argument_str), which);
	คืน FALSE;
    }
    gui_drag_scrollbar(sb, value, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    คืน TRUE;
}

    static จำนวนเต็ม
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    จำนวนเต็ม		tabnr;

    ถ้า (!dict_has_key(args, "tabnr"))
	คืน FALSE;

    tabnr = (จำนวนเต็ม)dict_get_number(args, "tabnr");

    คืน send_tabline_event(tabnr);
# มิฉะนั้น
    คืน FALSE;
# endif
}

    static จำนวนเต็ม
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    จำนวนเต็ม	tabnr;
    จำนวนเต็ม	item;

    ถ้า (!dict_has_key(args, "tabnr")
	    || !dict_has_key(args, "item"))
	คืน FALSE;

    tabnr = (จำนวนเต็ม)dict_get_number(args, "tabnr");
    item = (จำนวนเต็ม)dict_get_number(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    คืน TRUE;
}
#endif

    void
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = FALSE;

    ถ้า (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	คืน;
    }

    ถ้า (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == NULL)
	คืน;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_number = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    void
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = FALSE;

    ถ้า (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	คืน;
    }

    ถ้า (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == NULL)
	คืน;

    event = tv_get_string(&argvars[0]);
    ถ้า (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_number = test_gui_drop_files(argvars[1].vval.v_dict);
# ถ้า defined(FIND_REPLACE_DIALOG)
    มิฉะนั้น ถ้า (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_number = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    มิฉะนั้น ถ้า (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_number = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    มิฉะนั้น ถ้า (STRCMP(event, "mouse") == 0)
	rettv->vval.v_number = test_gui_mouse_event(argvars[1].vval.v_dict);
    มิฉะนั้น ถ้า (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_number = test_gui_scrollbar(argvars[1].vval.v_dict);
    มิฉะนั้น ถ้า (STRCMP(event, "tabline") == 0)
	rettv->vval.v_number = test_gui_tabline_event(argvars[1].vval.v_dict);
    มิฉะนั้น ถ้า (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_number = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    มิฉะนั้น
    {
	semsg(_(e_invalid_argument_str), event);
	คืน;
    }
#endif
}

    void
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	คืน;

    time_for_testing = (time_t)tv_get_number(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
