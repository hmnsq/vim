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

#නම් defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    static void
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    නම් (sname != NULL)
    {
	ga_concat(gap, sname);
	නම් (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    නම් (SOURCING_LNUM > 0)
    {
	var_na    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, parimaa_na(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    නම් (sname != NULL || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    static void
ga_concat_esc(garray_T *gap, char_u *p, පූර්ණ_සංඛ්‍යා clen)
{
    char_u  buf[NUMBUFLEN];

    නම් (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ආපසු;
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
	   නම් (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((var_na *)buf, parimaa_na(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   නැතහොත්
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
    පූර්ණ_සංඛ්‍යා	    c;
    පූර්ණ_සංඛ්‍යා	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    පූර්ණ_සංඛ්‍යා	    same_len;

    නම් (str == NULL)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ආපසු;
    }

    සඳහා (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	අතරතුර (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	නම් (same_len > 20)
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
	නැතහොත්
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
    පූර්ණ_සංඛ්‍යා		did_copy = FALSE;
    පූර්ණ_සංඛ්‍යා		omitted = 0;

    නම් (opt_msg_tv->v_type != VAR_UNKNOWN
	    && !(opt_msg_tv->v_type == VAR_STRING
		&& (opt_msg_tv->vval.v_string == NULL
		    || *opt_msg_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_msg_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    නම් (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    නැතහොත් නම් (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    නැතහොත්
	GA_CONCAT_LITERAL(gap, "Expected ");
    නම් (exp_str == NULL)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	නම් (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != NULL && got_tv->vval.v_dict != NULL)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    පූර්ණ_සංඛ්‍යා		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    නම් (exp_tv->vval.v_dict == NULL || got_tv->vval.v_dict == NULL)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = NULL;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = NULL;
		ආපසු;
	    }

	    todo = (පූර්ණ_සංඛ්‍යා)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&exp_d->dv_hashtab, hi, todo)
	    {
		නම් (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_key, -1);
		    නම් (item2 == NULL || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(var_na *)hi->hi_key, &HI2DI(hi)->di_tv);
			නම් (item2 != NULL)
			    dict_add_tv(got_tv->vval.v_dict,
					    (var_na *)hi->hi_key, &item2->di_tv);
		    }
		    නැතහොත්
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (පූර්ණ_සංඛ්‍යා)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&got_d->dv_hashtab, hi, todo)
	    {
		නම් (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_key, -1);
		    නම් (item2 == NULL)
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
    නැතහොත්
    {
	නම් (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	නම් (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    නම් (atype != ASSERT_NOTEQUAL)
    {
	නම් (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	නැතහොත් නම් (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	නැතහොත්
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	නම් (omitted != 0)
	{
	    var_na    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, parimaa_na(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    නම් (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    static පූර්ණ_සංඛ්‍යා
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    නම් (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], NULL, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ආපසු 1;
    }
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ආපසු 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    නම් (pat != NULL && text != NULL
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], NULL, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ආපසු 1;
    }
    ආපසු 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    static පූර්ණ_සංඛ්‍යා
assert_bool(typval_T *argvars, පූර්ණ_සංඛ්‍යා isTrue)
{
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    garray_T	ga;

    නම් (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_number == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ආපසු 0;
    නම් (argvars[0].v_type != VAR_NUMBER
	    || (tv_get_number_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		NULL, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ආපසු 1;
    }
    ආපසු 0;
}

    static void
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    නම් (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    නැතහොත්
	ga_concat(gap, cmd);
}

    static පූර්ණ_සංඛ්‍යා
assert_beeps(typval_T *argvars, පූර්ණ_සංඛ්‍යා no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		ret = 0;

    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    නම් (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	නම් (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	නැතහොත්
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ආපසු ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    void
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    rettv->vval.v_number = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    void
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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

    static පූර්ණ_සංඛ්‍යා
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
    පූර්ණ_සංඛ්‍යා		lineidx = 0;
    size_t	IObufflen;

    නම් (fname1 == NULL || fname2 == NULL)
	ආපසු 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((var_na *)fname1, READBIN);
    නම් (fd1 == NULL)
    {
	IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
	    (var_na *)e_cant_read_file_str, fname1);
    }
    නැතහොත්
    {
	fd2 = mch_fopen((var_na *)fname2, READBIN);
	නම් (fd2 == NULL)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
		(var_na *)e_cant_read_file_str, fname2);
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා	    c1, c2;
	    long    count = 0;
	    long    linecount = 1;

	    සඳහා (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		නම් (c1 == EOF)
		{
		    නම් (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    bha_nga;
		}
		නැතහොත් නම් (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    bha_nga;
		}
		නැතහොත්
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    නම් (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", count, linecount);
			bha_nga;
		    }
		}
		++count;
		නම් (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		නැතහොත් නම් (lineidx + 2 == (පූර්ණ_සංඛ්‍යා)parimaa_na(line1))
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

    නම් (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	නම් (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	නම් (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    නම් (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ආපසු 1;
    }

    ආපසු 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    void
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ආපසු;

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

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ආපසු;

    error = tv_get_string_chk(&argvars[0]);
    නම් (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    නැතහොත් නම් (error != NULL
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
    පූර්ණ_සංඛ්‍යා		save_trylevel = trylevel;
    පූර්ණ_සංඛ්‍යා		called_emsg_before = called_emsg;
    var_na	*wrong_arg_msg = NULL;
    char_u	*tofree = NULL;

    නම් (check_for_string_or_number_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ආපසු;

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

    නම් (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    නැතහොත් නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = NULL;
	පූර්ණ_සංඛ්‍යා	error_found = FALSE;
	පූර්ණ_සංඛ්‍යා	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_msg == NULL ? (char_u *)"[unknown]"
						       : emsg_assert_fails_msg;

	නම් (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == NULL
			   || strstr((var_na *)actual, (var_na *)expected) == NULL;
	}
	නැතහොත් නම් (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    නම් (list == NULL || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_msg = e_assert_fails_second_arg;
		goto theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    නම් (expected == NULL)
		goto theend;
	    නම් (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    නැතහොත් නම් (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		නම් (actual != NULL)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    නම් (expected == NULL)
			goto theend;
		    නම් (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	නැතහොත්
	{
	    wrong_arg_msg = e_assert_fails_second_arg;
	    goto theend;
	}

	නම් (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    නම් (argvars[3].v_type != VAR_NUMBER)
	    {
		wrong_arg_msg = e_assert_fails_fourth_argument;
		goto theend;
	    }
	    නැතහොත් නම් (argvars[3].vval.v_number >= 0
			 && argvars[3].vval.v_number != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    නම් (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		නම් (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_msg = e_assert_fails_fifth_argument;
		    goto theend;
		}
		නැතහොත් නම් (argvars[4].vval.v_string != NULL
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	නම් (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    නම් (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_NUMBER;
		actual_tv.vval.v_number = emsg_assert_fails_lnum;
	    }
	    නැතහොත් නම් (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    නැතහොත්
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
    නම් (wrong_arg_msg != NULL)
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

    static පූර්ණ_සංඛ්‍යා
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    char_u	expected_str[200];

    නම් (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	නම් (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((var_na *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, NULL,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ආපසු 1;
	}
    }
    නැතහොත්
    {
	varnumber_T	lower = tv_get_number_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_number_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_number_chk(&argvars[2], &error);

	නම් (error)
	    ආපසු 0;
	නම් (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((var_na *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, NULL,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ආපසු 1;
	}
    }
    ආපසු 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    void
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    නම් (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ආපසු;

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

    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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
    නම් (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	ආපසු;

    නම් (argvars[0].v_type != VAR_NUMBER
	    || argvars[0].vval.v_number <= 0
	    || argvars[1].v_type != VAR_NUMBER
	    || argvars[1].vval.v_number < 0
	    || argvars[2].v_type != VAR_NUMBER)
	emsg(_(e_invalid_argument));
    නැතහොත්
    {
	alloc_fail_id = argvars[0].vval.v_number;
	නම් (alloc_fail_id >= aid_last)
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
#නම් defined(FEAT_AUTOCHDIR)
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

    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    නම් (!gui.in_use)
	ආපසු;
# endif

    නම් (val != NULL)
    {
	trash_input_buf();
	add_to_input_buf_csi(val, (පූර්ණ_සංඛ්‍යා)STRLEN(val));
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

    නම් (check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    name = tv_get_string(&argvars[0]);

    නම් (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_number = need_fileinfo;
    නැතහොත්
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    void
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    නම් (check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    name = tv_get_string(&argvars[0]);
    නම් (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    void
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    පූර්ණ_සංඛ්‍යා     val;
    static පූර්ණ_සංඛ්‍යා save_starting = -1;

    නම් (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL)
	ආපසු;

    name = tv_get_string(&argvars[0]);
    val = (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[1]);

    නම් (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"starting") == 0)
    {
	නම් (val)
	{
	    නම් (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	නැතහොත්
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    නැතහොත් නම් (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_lines = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    නැතහොත් නම් (STRCMP(name, (char_u *)"ALL") == 0)
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
	නම් (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    නැතහොත්
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    void
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    පූර්ණ_සංඛ්‍යා retval = -1;

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
	    නම් (argvars[0].vval.v_job != NULL)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    bha_nga;
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    නම් (argvars[0].vval.v_channel != NULL)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    bha_nga;
	pak_sa VAR_FUNC:
	    නම් (argvars[0].vval.v_string != NULL)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		නම් (fp != NULL)
		    retval = fp->uf_refcount;
	    }
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    නම් (argvars[0].vval.v_partial != NULL)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    bha_nga;
	pak_sa VAR_BLOB:
	    නම් (argvars[0].vval.v_blob != NULL)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    bha_nga;
	pak_sa VAR_LIST:
	    නම් (argvars[0].vval.v_list != NULL)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    bha_nga;
	pak_sa VAR_TUPLE:
	    නම් (argvars[0].vval.v_tuple != NULL)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    bha_nga;
	pak_sa VAR_DICT:
	    නම් (argvars[0].vval.v_dict != NULL)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    bha_nga;
	pak_sa VAR_CLASS:
	    නම් (argvars[0].vval.v_class != NULL)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    bha_nga;
	pak_sa VAR_OBJECT:
	    නම් (argvars[0].vval.v_object != NULL)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    bha_nga;
	pak_sa VAR_TYPEALIAS:
	    නම් (argvars[0].vval.v_typealias != NULL)
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
    නම් (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    නැතහොත්
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
    නම් (check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    void
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = NULL;
}

#නම් defined(FEAT_JOB_CHANNEL)
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

#නම් defined(FEAT_JOB_CHANNEL)
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
    නම් (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ආපසු;

    නම් (argvars[0].v_type != VAR_NUMBER || argvars[1].v_type != VAR_NUMBER)
    {
	emsg(_(e_invalid_argument));
	ආපසු;
    }

    mouse_row = (time_t)tv_get_number(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_number(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    static පූර්ණ_සංඛ්‍යා
test_gui_drop_files(dict_T *args UNUSED)
{
# නම් defined(HAVE_DROP_FILE)
    පූර්ණ_සංඛ්‍යා		row;
    පූර්ණ_සංඛ්‍යා		col;
    int_u	mods;
    char_u	**fnames;
    පූර්ණ_සංඛ්‍යා		count = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    නම් (!dict_has_key(args, "files")
	    || !dict_has_key(args, "row")
	    || !dict_has_key(args, "col")
	    || !dict_has_key(args, "modifiers"))
	ආපසු FALSE;

    (void)dict_get_tv(args, "files", &t);
    row = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "row");
    col = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "col");
    mods = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "modifiers");

    නම් (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ආපසු FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    නම් (fnames == NULL)
	ආපසු FALSE;

    FOR_ALL_LIST_ITEMS(l, li)
    {
	// ignore non-string items
	නම් (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == NULL)
	    continue;

	fnames[count] = vim_strsave(li->li_tv.vval.v_string);
	නම් (fnames[count] == NULL)
	{
	    අතරතුර (--count >= 0)
		vim_free(fnames[count]);
	    vim_free(fnames);
	    ආපසු FALSE;
	}
	count++;
    }

    නම් (count > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, count);
    නැතහොත්
	vim_free(fnames);
# endif

    ආපසු TRUE;
}

# නම් defined(FIND_REPLACE_DIALOG)
    static පූර්ණ_සංඛ්‍යා
test_gui_find_repl(dict_T *args)
{
    පූර්ණ_සංඛ්‍යා		flags;
    char_u	*find_text;
    char_u	*repl_text;
    පූර්ණ_සංඛ්‍යා		forward;
    පූර්ණ_සංඛ්‍යා		retval;

    නම් (!dict_has_key(args, "find_text")
	    || !dict_has_key(args, "repl_text")
	    || !dict_has_key(args, "flags")
	    || !dict_has_key(args, "forward"))
	ආපසු FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "flags");
    forward = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    ආපසු retval;
}
# endif

    static පූර්ණ_සංඛ්‍යා
test_gui_mouse_event(dict_T *args)
{
    පූර්ණ_සංඛ්‍යා		button;
    පූර්ණ_සංඛ්‍යා		row;
    පූර්ණ_සංඛ්‍යා		col;
    පූර්ණ_සංඛ්‍යා		repeated_click;
    int_u	mods;
    පූර්ණ_සංඛ්‍යා		move;

    නම් (!dict_has_key(args, "row")
	    || !dict_has_key(args, "col"))
	ආපසු FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (පූර්ණ_සංඛ්‍යා)dict_get_bool(args, "move", FALSE);

    නම් (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	ආපසු FALSE;

    row = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "row");
    col = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "col");

    නම් (move)
    {
	පූර්ණ_සංඛ්‍යා pY = row;
	පූර්ණ_සංඛ්‍යා pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	නම් (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    නැතහොත්
    {
	button = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "button");
	repeated_click = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "multiclick");
	mods = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    ආපසු TRUE;
}

    static පූර්ණ_සංඛ්‍යා
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	value;
    පූර්ණ_සංඛ්‍යා		dragging;
    scrollbar_T *sb = NULL;

    නම් (!dict_has_key(args, "which")
	    || !dict_has_key(args, "value")
	    || !dict_has_key(args, "dragging"))
	ආපසු FALSE;

    which = dict_get_string(args, "which", FALSE);
    value = (long)dict_get_number(args, "value");
    dragging = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "dragging");

    නම් (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_LEFT];
    නැතහොත් නම් (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_RIGHT];
    නැතහොත් නම් (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    නම් (sb == NULL)
    {
	semsg(_(e_invalid_argument_str), which);
	ආපසු FALSE;
    }
    gui_drag_scrollbar(sb, value, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    ආපසු TRUE;
}

    static පූර්ණ_සංඛ්‍යා
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    පූර්ණ_සංඛ්‍යා		tabnr;

    නම් (!dict_has_key(args, "tabnr"))
	ආපසු FALSE;

    tabnr = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "tabnr");

    ආපසු send_tabline_event(tabnr);
# නැතහොත්
    ආපසු FALSE;
# endif
}

    static පූර්ණ_සංඛ්‍යා
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    පූර්ණ_සංඛ්‍යා	tabnr;
    පූර්ණ_සංඛ්‍යා	item;

    නම් (!dict_has_key(args, "tabnr")
	    || !dict_has_key(args, "item"))
	ආපසු FALSE;

    tabnr = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "tabnr");
    item = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    ආපසු TRUE;
}
#endif

    void
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = FALSE;

    නම් (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ආපසු;
    }

    නම් (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == NULL)
	ආපසු;

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

    නම් (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ආපසු;
    }

    නම් (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == NULL)
	ආපසු;

    event = tv_get_string(&argvars[0]);
    නම් (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_number = test_gui_drop_files(argvars[1].vval.v_dict);
# නම් defined(FIND_REPLACE_DIALOG)
    නැතහොත් නම් (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_number = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    නැතහොත් නම් (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_number = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    නැතහොත් නම් (STRCMP(event, "mouse") == 0)
	rettv->vval.v_number = test_gui_mouse_event(argvars[1].vval.v_dict);
    නැතහොත් නම් (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_number = test_gui_scrollbar(argvars[1].vval.v_dict);
    නැතහොත් නම් (STRCMP(event, "tabline") == 0)
	rettv->vval.v_number = test_gui_tabline_event(argvars[1].vval.v_dict);
    නැතහොත් නම් (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_number = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    නැතහොත්
    {
	semsg(_(e_invalid_argument_str), event);
	ආපසු;
    }
#endif
}

    void
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    නම් (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ආපසු;

    time_for_testing = (time_t)tv_get_number(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
