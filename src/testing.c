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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ایستا تهی
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    اگر (sname != پوچ)
    {
	ga_concat(gap, sname);
	اگر (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    اگر (SOURCING_LNUM > 0)
    {
	نویسه    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, اندازه(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    اگر (sname != پوچ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ایستا تهی
ga_concat_esc(garray_T *gap, char_u *p, صحیح clen)
{
    char_u  buf[NUMBUFLEN];

    اگر (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	بازگشت;
    }

    انتخاب (*p)
    {
	حالت BS: GA_CONCAT_LITERAL(gap, "\\b"); بشکن;
	حالت ESC: GA_CONCAT_LITERAL(gap, "\\e"); بشکن;
	حالت FF: GA_CONCAT_LITERAL(gap, "\\f"); بشکن;
	حالت NL: GA_CONCAT_LITERAL(gap, "\\n"); بشکن;
	حالت TAB: GA_CONCAT_LITERAL(gap, "\\t"); بشکن;
	حالت CAR: GA_CONCAT_LITERAL(gap, "\\r"); بشکن;
	حالت '\\': GA_CONCAT_LITERAL(gap, "\\\\"); بشکن;
	پیشفرض:
	   اگر (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((نویسه *)buf, اندازه(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   وگرنه
	       ga_append(gap, *p);
	   بشکن;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ایستا تهی
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    صحیح	    c;
    صحیح	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    صحیح	    same_len;

    اگر (str == پوچ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	بازگشت;
    }

    برای (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	وقتی (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	اگر (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((نویسه *)buf, اندازه(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	وگرنه
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ایستا تهی
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_پیام_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    صحیح		did_copy = FALSE;
    صحیح		omitted = 0;

    اگر (opt_پیام_tv->v_type != VAR_UNKNOWN
	    && !(opt_پیام_tv->v_type == VAR_STRING
		&& (opt_پیام_tv->vval.v_string == پوچ
		    || *opt_پیام_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_پیام_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    اگر (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    وگرنه اگر (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    وگرنه
	GA_CONCAT_LITERAL(gap, "Expected ");
    اگر (exp_str == پوچ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	اگر (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != پوچ && got_tv->vval.v_dict != پوچ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    صحیح		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    اگر (exp_tv->vval.v_dict == پوچ || got_tv->vval.v_dict == پوچ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = پوچ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = پوچ;
		بازگشت;
	    }

	    todo = (صحیح)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_اقلام(&exp_d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_کلید, -1);
		    اگر (item2 == پوچ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(نویسه *)hi->hi_کلید, &HI2DI(hi)->di_tv);
			اگر (item2 != پوچ)
			    dict_add_tv(got_tv->vval.v_dict,
					    (نویسه *)hi->hi_کلید, &item2->di_tv);
		    }
		    وگرنه
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (صحیح)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_اقلام(&got_d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_کلید, -1);
		    اگر (item2 == پوچ)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(نویسه *)hi->hi_کلید, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    وگرنه
    {
	اگر (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	اگر (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    اگر (atype != ASSERT_NOTEQUAL)
    {
	اگر (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	وگرنه اگر (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	وگرنه
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	اگر (omitted != 0)
	{
	    نویسه    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, اندازه(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    اگر (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ایستا صحیح
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    اگر (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], پوچ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	بازگشت 1;
    }
    بازگشت 0;
}

    ایستا صحیح
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    اگر (pat != پوچ && text != پوچ
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], پوچ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	بازگشت 1;
    }
    بازگشت 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ایستا صحیح
assert_bool(typval_T *argvars, صحیح isTrue)
{
    صحیح		error = FALSE;
    garray_T	ga;

    اگر (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_عدد == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	بازگشت 0;
    اگر (argvars[0].v_type != VAR_عدد
	    || (tv_get_عدد_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		پوچ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	بازگشت 1;
    }
    بازگشت 0;
}

    ایستا تهی
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    وگرنه
	ga_concat(gap, cmd);
}

    ایستا صحیح
assert_beeps(typval_T *argvars, صحیح no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    صحیح		ret = 0;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    اگر (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	اگر (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	وگرنه
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    بازگشت ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    تهی
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    تهی
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    تهی
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ایستا صحیح
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    نویسه	line1[200];
    نویسه	line2[200];
    صحیح		lineidx = 0;
    size_t	IObufflen;

    اگر (fname1 == پوچ || fname2 == پوچ)
	بازگشت 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((نویسه *)fname1, READBIN);
    اگر (fd1 == پوچ)
    {
	IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
	    (نویسه *)e_cant_read_file_str, fname1);
    }
    وگرنه
    {
	fd2 = mch_fopen((نویسه *)fname2, READBIN);
	اگر (fd2 == پوچ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
		(نویسه *)e_cant_read_file_str, fname2);
	}
	وگرنه
	{
	    صحیح	    c1, c2;
	    long    شمار = 0;
	    long    linecount = 1;

	    برای (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		اگر (c1 == EOF)
		{
		    اگر (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    بشکن;
		}
		وگرنه اگر (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    بشکن;
		}
		وگرنه
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    اگر (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", شمار, linecount);
			بشکن;
		    }
		}
		++شمار;
		اگر (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		وگرنه اگر (lineidx + 2 == (صحیح)اندازه(line1))
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

    اگر (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	اگر (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    اگر (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	بازگشت 1;
    }

    بازگشت 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    تهی
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    rettv->vval.v_عدد = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    تهی
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    تهی
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    error = tv_get_string_chk(&argvars[0]);
    اگر (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
    }
    وگرنه اگر (error != پوچ
	&& strstr((نویسه *)get_vim_var_str(VV_EXCEPTION), (نویسه *)error) == پوچ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], پوچ, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    تهی
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    صحیح		save_trylevel = trylevel;
    صحیح		called_emsg_before = called_emsg;
    نویسه	*wrong_arg_پیام = پوچ;
    char_u	*tofree = پوچ;

    اگر (check_for_string_or_عدد_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	بازگشت;

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

    اگر (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
    }
    وگرنه اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = پوچ;
	صحیح	error_found = FALSE;
	صحیح	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_پیام == پوچ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_پیام;

	اگر (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == پوچ
			   || strstr((نویسه *)actual, (نویسه *)expected) == پوچ;
	}
	وگرنه اگر (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    اگر (list == پوچ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_پیام = e_assert_fails_second_arg;
		برو theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    اگر (expected == پوچ)
		برو theend;
	    اگر (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    وگرنه اگر (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		اگر (actual != پوچ)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    اگر (expected == پوچ)
			برو theend;
		    اگر (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	وگرنه
	{
	    wrong_arg_پیام = e_assert_fails_second_arg;
	    برو theend;
	}

	اگر (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    اگر (argvars[3].v_type != VAR_عدد)
	    {
		wrong_arg_پیام = e_assert_fails_fourth_argument;
		برو theend;
	    }
	    وگرنه اگر (argvars[3].vval.v_عدد >= 0
			 && argvars[3].vval.v_عدد != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    اگر (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		اگر (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_پیام = e_assert_fails_fifth_argument;
		    برو theend;
		}
		وگرنه اگر (argvars[4].vval.v_string != پوچ
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	اگر (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    اگر (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_عدد;
		actual_tv.vval.v_عدد = emsg_assert_fails_lnum;
	    }
	    وگرنه اگر (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    وگرنه
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
	    rettv->vval.v_عدد = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    پیام_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    پیام_scrolled = 0;
    سطرها_چپ = Rows;
    VIM_CLEAR(emsg_assert_fails_پیام);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, پوچ, 0);
    اگر (wrong_arg_پیام != پوچ)
	emsg(_(wrong_arg_پیام));
}

/*
 * "assert_false(actual[, msg])" function
 */
    تهی
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_bool(argvars, FALSE);
}

    ایستا صحیح
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    صحیح		error = FALSE;
    char_u	expected_str[200];

    اگر (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	اگر (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((نویسه *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, پوچ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    بازگشت 1;
	}
    }
    وگرنه
    {
	varnumber_T	lower = tv_get_عدد_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_عدد_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_عدد_chk(&argvars[2], &error);

	اگر (error)
	    بازگشت 0;
	اگر (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((نویسه *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, پوچ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    بازگشت 1;
	}
    }
    بازگشت 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    تهی
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    تهی
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    تهی
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    تهی
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_عدد = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    تهی
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    تهی
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_عدد
	    || argvars[0].vval.v_عدد <= 0
	    || argvars[1].v_type != VAR_عدد
	    || argvars[1].vval.v_عدد < 0
	    || argvars[2].v_type != VAR_عدد)
	emsg(_(e_invalid_argument));
    وگرنه
    {
	alloc_fail_id = argvars[0].vval.v_عدد;
	اگر (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_عدد;
	alloc_fail_repeat = argvars[2].vval.v_عدد;
	did_outofmem_پیام = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    تهی
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#اگر defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    تهی
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_درون_داد_BUF
    char_u	*val;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    اگر (!gui.in_use)
	بازگشت;
# endif

    اگر (val != پوچ)
    {
	trash_درون_داد_buf();
	add_to_درون_داد_buf_csi(val, (صحیح)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    تهی
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    name = tv_get_string(&argvars[0]);

    اگر (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_عدد = need_fileinfo;
    وگرنه
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    تهی
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    تهی
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    صحیح     val;
    ایستا صحیح save_starting = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL)
	بازگشت;

    name = tv_get_string(&argvars[0]);
    val = (صحیح)tv_get_عدد(&argvars[1]);

    اگر (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"starting") == 0)
    {
	اگر (val)
	{
	    اگر (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	وگرنه
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    وگرنه اگر (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    وگرنه اگر (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    وگرنه اگر (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    وگرنه اگر (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    وگرنه اگر (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_سطرها = val;
    وگرنه اگر (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    وگرنه اگر (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    وگرنه اگر (STRCMP(name, (char_u *)"ALL") == 0)
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
	اگر (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    وگرنه
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    تهی
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    صحیح retval = -1;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_عدد:
	حالت VAR_BOOL:
	حالت VAR_FLOAT:
	حالت VAR_SPECIAL:
	حالت VAR_STRING:
	حالت VAR_INSTR:
	    بشکن;

	حالت VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    اگر (argvars[0].vval.v_job != پوچ)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    بشکن;
	حالت VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    اگر (argvars[0].vval.v_channel != پوچ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    بشکن;
	حالت VAR_FUNC:
	    اگر (argvars[0].vval.v_string != پوچ)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		اگر (fp != پوچ)
		    retval = fp->uf_refcount;
	    }
	    بشکن;
	حالت VAR_PARTIAL:
	    اگر (argvars[0].vval.v_partial != پوچ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    بشکن;
	حالت VAR_BLOB:
	    اگر (argvars[0].vval.v_blob != پوچ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    بشکن;
	حالت VAR_LIST:
	    اگر (argvars[0].vval.v_list != پوچ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    بشکن;
	حالت VAR_TUPLE:
	    اگر (argvars[0].vval.v_tuple != پوچ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    بشکن;
	حالت VAR_DICT:
	    اگر (argvars[0].vval.v_dict != پوچ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    بشکن;
	حالت VAR_CLASS:
	    اگر (argvars[0].vval.v_class != پوچ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    بشکن;
	حالت VAR_OBJECT:
	    اگر (argvars[0].vval.v_object != پوچ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    بشکن;
	حالت VAR_TYPEALIAS:
	    اگر (argvars[0].vval.v_typealias != پوچ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    بشکن;
    }

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    تهی
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    اگر (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    وگرنه
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    تهی
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    تهی
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    تهی
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = پوچ;
}

#اگر defined(FEAT_JOB_CHANNEL)
    تهی
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = پوچ;
}
#endif

    تهی
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, پوچ);
}

#اگر defined(FEAT_JOB_CHANNEL)
    تهی
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = پوچ;
}
#endif

    تهی
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, پوچ);
}

    تهی
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = پوچ;
}

    تهی
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = پوچ;
}

    تهی
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
}

    تهی
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, پوچ);
}

    تهی
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    تهی
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    تهی
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_عدد || argvars[1].v_type != VAR_عدد)
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }

    mouse_row = (time_t)tv_get_عدد(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_عدد(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    ایستا صحیح
test_gui_drop_files(dict_T *args UNUSED)
{
# اگر defined(HAVE_DROP_FILE)
    صحیح		row;
    صحیح		col;
    int_u	mods;
    char_u	**fnames;
    صحیح		شمار = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    اگر (!dict_has_کلید(args, "files")
	    || !dict_has_کلید(args, "row")
	    || !dict_has_کلید(args, "col")
	    || !dict_has_کلید(args, "modifiers"))
	بازگشت FALSE;

    (تهی)dict_get_tv(args, "files", &t);
    row = (صحیح)dict_get_عدد(args, "row");
    col = (صحیح)dict_get_عدد(args, "col");
    mods = (صحیح)dict_get_عدد(args, "modifiers");

    اگر (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	بازگشت FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    اگر (fnames == پوچ)
	بازگشت FALSE;

    FOR_ALL_LIST_اقلام(l, li)
    {
	// ignore non-string items
	اگر (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == پوچ)
	    ادامه;

	fnames[شمار] = vim_strsave(li->li_tv.vval.v_string);
	اگر (fnames[شمار] == پوچ)
	{
	    وقتی (--شمار >= 0)
		vim_free(fnames[شمار]);
	    vim_free(fnames);
	    بازگشت FALSE;
	}
	شمار++;
    }

    اگر (شمار > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, شمار);
    وگرنه
	vim_free(fnames);
# endif

    بازگشت TRUE;
}

# اگر defined(FIND_REPLACE_DIALOG)
    ایستا صحیح
test_gui_find_repl(dict_T *args)
{
    صحیح		flags;
    char_u	*find_text;
    char_u	*repl_text;
    صحیح		forward;
    صحیح		retval;

    اگر (!dict_has_کلید(args, "find_text")
	    || !dict_has_کلید(args, "repl_text")
	    || !dict_has_کلید(args, "flags")
	    || !dict_has_کلید(args, "forward"))
	بازگشت FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (صحیح)dict_get_عدد(args, "flags");
    forward = (صحیح)dict_get_عدد(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    بازگشت retval;
}
# endif

    ایستا صحیح
test_gui_mouse_event(dict_T *args)
{
    صحیح		button;
    صحیح		row;
    صحیح		col;
    صحیح		repeated_click;
    int_u	mods;
    صحیح		move;

    اگر (!dict_has_کلید(args, "row")
	    || !dict_has_کلید(args, "col"))
	بازگشت FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحیح)dict_get_bool(args, "move", FALSE);

    اگر (!move && (!dict_has_کلید(args, "button")
	    || !dict_has_کلید(args, "multiclick")
	    || !dict_has_کلید(args, "modifiers")))
	بازگشت FALSE;

    row = (صحیح)dict_get_عدد(args, "row");
    col = (صحیح)dict_get_عدد(args, "col");

    اگر (move)
    {
	صحیح pY = row;
	صحیح pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	اگر (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    وگرنه
    {
	button = (صحیح)dict_get_عدد(args, "button");
	repeated_click = (صحیح)dict_get_عدد(args, "multiclick");
	mods = (صحیح)dict_get_عدد(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    بازگشت TRUE;
}

    ایستا صحیح
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	مقدار;
    صحیح		dragging;
    scrollbar_T *sb = پوچ;

    اگر (!dict_has_کلید(args, "which")
	    || !dict_has_کلید(args, "value")
	    || !dict_has_کلید(args, "dragging"))
	بازگشت FALSE;

    which = dict_get_string(args, "which", FALSE);
    مقدار = (long)dict_get_عدد(args, "value");
    dragging = (صحیح)dict_get_عدد(args, "dragging");

    اگر (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_چپ];
    وگرنه اگر (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_راست];
    وگرنه اگر (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    اگر (sb == پوچ)
    {
	semsg(_(e_invalid_argument_str), which);
	بازگشت FALSE;
    }
    gui_drag_scrollbar(sb, مقدار, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    بازگشت TRUE;
}

    ایستا صحیح
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    صحیح		tabnr;

    اگر (!dict_has_کلید(args, "tabnr"))
	بازگشت FALSE;

    tabnr = (صحیح)dict_get_عدد(args, "tabnr");

    بازگشت send_tabline_event(tabnr);
# وگرنه
    بازگشت FALSE;
# endif
}

    ایستا صحیح
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    صحیح	tabnr;
    صحیح	item;

    اگر (!dict_has_کلید(args, "tabnr")
	    || !dict_has_کلید(args, "item"))
	بازگشت FALSE;

    tabnr = (صحیح)dict_get_عدد(args, "tabnr");
    item = (صحیح)dict_get_عدد(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    بازگشت TRUE;
}
#endif

    تهی
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = FALSE;

    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	بازگشت;
    }

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == پوچ)
	بازگشت;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_عدد = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    تهی
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = FALSE;

    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	بازگشت;
    }

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == پوچ)
	بازگشت;

    event = tv_get_string(&argvars[0]);
    اگر (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_عدد = test_gui_drop_files(argvars[1].vval.v_dict);
# اگر defined(FIND_REPLACE_DIALOG)
    وگرنه اگر (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_عدد = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    وگرنه اگر (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_عدد = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    وگرنه اگر (STRCMP(event, "mouse") == 0)
	rettv->vval.v_عدد = test_gui_mouse_event(argvars[1].vval.v_dict);
    وگرنه اگر (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_عدد = test_gui_scrollbar(argvars[1].vval.v_dict);
    وگرنه اگر (STRCMP(event, "tabline") == 0)
	rettv->vval.v_عدد = test_gui_tabline_event(argvars[1].vval.v_dict);
    وگرنه اگر (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_عدد = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    وگرنه
    {
	semsg(_(e_invalid_argument_str), event);
	بازگشت;
    }
#endif
}

    تهی
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    time_for_testing = (time_t)tv_get_عدد(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
