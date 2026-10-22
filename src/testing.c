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

#ضمن "vim.h"

#ܐܢ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ساكن فراغ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ܐܢ (sname != عدم)
    {
	ga_concat(gap, sname);
	ܐܢ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ܐܢ (SOURCING_LNUM > 0)
    {
	حرف    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, حجم(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ܐܢ (sname != عدم || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ساكن فراغ
ga_concat_esc(garray_T *gap, char_u *p, صحيح clen)
{
    char_u  buf[NUMBUFLEN];

    ܐܢ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ܦܢܐ;
    }

    بدل (*p)
    {
	حالة BS: GA_CONCAT_LITERAL(gap, "\\b"); اكسر;
	حالة ESC: GA_CONCAT_LITERAL(gap, "\\e"); اكسر;
	حالة FF: GA_CONCAT_LITERAL(gap, "\\f"); اكسر;
	حالة NL: GA_CONCAT_LITERAL(gap, "\\n"); اكسر;
	حالة TAB: GA_CONCAT_LITERAL(gap, "\\t"); اكسر;
	حالة CAR: GA_CONCAT_LITERAL(gap, "\\r"); اكسر;
	حالة '\\': GA_CONCAT_LITERAL(gap, "\\\\"); اكسر;
	افتراضي:
	   ܐܢ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ܐܚܪܢܐ
	       ga_append(gap, *p);
	   اكسر;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ساكن فراغ
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    صحيح	    c;
    صحيح	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    صحيح	    same_len;

    ܐܢ (str == عدم)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ܦܢܐ;
    }

    لكل (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ܟܡܐ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ܐܢ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	ܐܚܪܢܐ
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ساكن فراغ
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
    صحيح		did_copy = FALSE;
    صحيح		omitted = 0;

    ܐܢ (opt_msg_tv->v_type != VAR_UNKNOWN
	    && !(opt_msg_tv->v_type == VAR_STRING
		&& (opt_msg_tv->vval.v_string == عدم
		    || *opt_msg_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_msg_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ܐܢ (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ܐܚܪܢܐ ܐܢ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ܐܚܪܢܐ
	GA_CONCAT_LITERAL(gap, "Expected ");
    ܐܢ (exp_str == عدم)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ܐܢ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != عدم && got_tv->vval.v_dict != عدم)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    صحيح		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ܐܢ (exp_tv->vval.v_dict == عدم || got_tv->vval.v_dict == عدم)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = عدم;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = عدم;
		ܦܢܐ;
	    }

	    todo = (صحيح)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&exp_d->dv_hashtab, hi, todo)
	    {
		ܐܢ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_key, -1);
		    ܐܢ (item2 == عدم || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(حرف *)hi->hi_key, &HI2DI(hi)->di_tv);
			ܐܢ (item2 != عدم)
			    dict_add_tv(got_tv->vval.v_dict,
					    (حرف *)hi->hi_key, &item2->di_tv);
		    }
		    ܐܚܪܢܐ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (صحيح)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&got_d->dv_hashtab, hi, todo)
	    {
		ܐܢ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_key, -1);
		    ܐܢ (item2 == عدم)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(حرف *)hi->hi_key, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ܐܚܪܢܐ
    {
	ܐܢ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ܐܢ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ܐܢ (atype != ASSERT_NOTEQUAL)
    {
	ܐܢ (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ܐܚܪܢܐ ܐܢ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ܐܚܪܢܐ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ܐܢ (omitted != 0)
	{
	    حرف    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, حجم(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ܐܢ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ساكن صحيح
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ܐܢ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], عدم, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ܦܢܐ 1;
    }
    ܦܢܐ 0;
}

    ساكن صحيح
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ܦܢܐ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    ܐܢ (pat != عدم && text != عدم
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], عدم, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ܦܢܐ 1;
    }
    ܦܢܐ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ساكن صحيح
assert_bool(typval_T *argvars, صحيح isTrue)
{
    صحيح		error = FALSE;
    garray_T	ga;

    ܐܢ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_number == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ܦܢܐ 0;
    ܐܢ (argvars[0].v_type != VAR_NUMBER
	    || (tv_get_number_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		عدم, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ܦܢܐ 1;
    }
    ܦܢܐ 0;
}

    ساكن فراغ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ܐܢ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ܐܚܪܢܐ
	ga_concat(gap, cmd);
}

    ساكن صحيح
assert_beeps(typval_T *argvars, صحيح no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    صحيح		ret = 0;

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ܐܢ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ܐܢ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	ܐܚܪܢܐ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ܦܢܐ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    فراغ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    rettv->vval.v_number = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    فراغ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    rettv->vval.v_number = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    فراغ
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ساكن صحيح
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    حرف	line1[200];
    حرف	line2[200];
    صحيح		lineidx = 0;
    size_t	IObufflen;

    ܐܢ (fname1 == عدم || fname2 == عدم)
	ܦܢܐ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    ܐܢ (fd1 == عدم)
    {
	IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    (حرف *)e_cant_read_file_str, fname1);
    }
    ܐܚܪܢܐ
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	ܐܢ (fd2 == عدم)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
		(حرف *)e_cant_read_file_str, fname2);
	}
	ܐܚܪܢܐ
	{
	    صحيح	    c1, c2;
	    long    count = 0;
	    long    linecount = 1;

	    لكل (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ܐܢ (c1 == EOF)
		{
		    ܐܢ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    اكسر;
		}
		ܐܚܪܢܐ ܐܢ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    اكسر;
		}
		ܐܚܪܢܐ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ܐܢ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", count, linecount);
			اكسر;
		    }
		}
		++count;
		ܐܢ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ܐܚܪܢܐ ܐܢ (lineidx + 2 == (صحيح)حجم(line1))
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

    ܐܢ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ܐܢ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ܐܢ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ܐܢ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ܦܢܐ 1;
    }

    ܦܢܐ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    فراغ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    rettv->vval.v_number = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    فراغ
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    فراغ
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    error = tv_get_string_chk(&argvars[0]);
    ܐܢ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    ܐܚܪܢܐ ܐܢ (error != عدم
	&& strstr((حرف *)get_vim_var_str(VV_EXCEPTION), (حرف *)error) == عدم)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], عدم, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    فراغ
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    صحيح		save_trylevel = trylevel;
    صحيح		called_emsg_before = called_emsg;
    حرف	*wrong_arg_msg = عدم;
    char_u	*tofree = عدم;

    ܐܢ (check_for_string_or_number_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ܦܢܐ;

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

    ܐܢ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_number = 1;
    }
    ܐܚܪܢܐ ܐܢ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = عدم;
	صحيح	error_found = FALSE;
	صحيح	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_msg == عدم ? (char_u *)"[unknown]"
						       : emsg_assert_fails_msg;

	ܐܢ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == عدم
			   || strstr((حرف *)actual, (حرف *)expected) == عدم;
	}
	ܐܚܪܢܐ ܐܢ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ܐܢ (list == عدم || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_msg = e_assert_fails_second_arg;
		اذهب theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ܐܢ (expected == عدم)
		اذهب theend;
	    ܐܢ (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    ܐܚܪܢܐ ܐܢ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ܐܢ (actual != عدم)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ܐܢ (expected == عدم)
			اذهب theend;
		    ܐܢ (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ܐܚܪܢܐ
	{
	    wrong_arg_msg = e_assert_fails_second_arg;
	    اذهب theend;
	}

	ܐܢ (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ܐܢ (argvars[3].v_type != VAR_NUMBER)
	    {
		wrong_arg_msg = e_assert_fails_fourth_argument;
		اذهب theend;
	    }
	    ܐܚܪܢܐ ܐܢ (argvars[3].vval.v_number >= 0
			 && argvars[3].vval.v_number != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    ܐܢ (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ܐܢ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_msg = e_assert_fails_fifth_argument;
		    اذهب theend;
		}
		ܐܚܪܢܐ ܐܢ (argvars[4].vval.v_string != عدم
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	ܐܢ (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ܐܢ (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_NUMBER;
		actual_tv.vval.v_number = emsg_assert_fails_lnum;
	    }
	    ܐܚܪܢܐ ܐܢ (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ܐܚܪܢܐ
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
    set_vim_var_string(VV_ERRMSG, عدم, 0);
    ܐܢ (wrong_arg_msg != عدم)
	emsg(_(wrong_arg_msg));
}

/*
 * "assert_false(actual[, msg])" function
 */
    فراغ
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_bool(argvars, FALSE);
}

    ساكن صحيح
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    صحيح		error = FALSE;
    char_u	expected_str[200];

    ܐܢ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ܐܢ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((حرف *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, عدم,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ܦܢܐ 1;
	}
    }
    ܐܚܪܢܐ
    {
	varnumber_T	lower = tv_get_number_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_number_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_number_chk(&argvars[2], &error);

	ܐܢ (error)
	    ܦܢܐ 0;
	ܐܢ (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((حرف *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, عدم,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ܦܢܐ 1;
	}
    }
    ܦܢܐ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    فراغ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ܐܢ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ܦܢܐ;

    rettv->vval.v_number = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    فراغ
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    فراغ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    فراغ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_number = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    فراغ
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    فراغ
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    ܐܢ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    ܐܢ (argvars[0].v_type != VAR_NUMBER
	    || argvars[0].vval.v_number <= 0
	    || argvars[1].v_type != VAR_NUMBER
	    || argvars[1].vval.v_number < 0
	    || argvars[2].v_type != VAR_NUMBER)
	emsg(_(e_invalid_argument));
    ܐܚܪܢܐ
    {
	alloc_fail_id = argvars[0].vval.v_number;
	ܐܢ (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_number;
	alloc_fail_repeat = argvars[2].vval.v_number;
	did_outofmem_msg = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    فراغ
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ܐܢ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    فراغ
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_INPUT_BUF
    char_u	*val;

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ܐܢ (!gui.in_use)
	ܦܢܐ;
# endif

    ܐܢ (val != عدم)
    {
	trash_input_buf();
	add_to_input_buf_csi(val, (صحيح)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    فراغ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    name = tv_get_string(&argvars[0]);

    ܐܢ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_number = need_fileinfo;
    ܐܚܪܢܐ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    فراغ
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    name = tv_get_string(&argvars[0]);
    ܐܢ (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    فراغ
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    صحيح     val;
    ساكن صحيح save_starting = -1;

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL)
	ܦܢܐ;

    name = tv_get_string(&argvars[0]);
    val = (صحيح)tv_get_number(&argvars[1]);

    ܐܢ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"starting") == 0)
    {
	ܐܢ (val)
	{
	    ܐܢ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ܐܚܪܢܐ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_lines = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ܐܚܪܢܐ ܐܢ (STRCMP(name, (char_u *)"ALL") == 0)
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
	ܐܢ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ܐܚܪܢܐ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    فراغ
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    صحيح retval = -1;

    بدل (argvars[0].v_type)
    {
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_NUMBER:
	حالة VAR_BOOL:
	حالة VAR_FLOAT:
	حالة VAR_SPECIAL:
	حالة VAR_STRING:
	حالة VAR_INSTR:
	    اكسر;

	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ܐܢ (argvars[0].vval.v_job != عدم)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    اكسر;
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ܐܢ (argvars[0].vval.v_channel != عدم)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    اكسر;
	حالة VAR_FUNC:
	    ܐܢ (argvars[0].vval.v_string != عدم)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		ܐܢ (fp != عدم)
		    retval = fp->uf_refcount;
	    }
	    اكسر;
	حالة VAR_PARTIAL:
	    ܐܢ (argvars[0].vval.v_partial != عدم)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    اكسر;
	حالة VAR_BLOB:
	    ܐܢ (argvars[0].vval.v_blob != عدم)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    اكسر;
	حالة VAR_LIST:
	    ܐܢ (argvars[0].vval.v_list != عدم)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    اكسر;
	حالة VAR_TUPLE:
	    ܐܢ (argvars[0].vval.v_tuple != عدم)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    اكسر;
	حالة VAR_DICT:
	    ܐܢ (argvars[0].vval.v_dict != عدم)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    اكسر;
	حالة VAR_CLASS:
	    ܐܢ (argvars[0].vval.v_class != عدم)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    اكسر;
	حالة VAR_OBJECT:
	    ܐܢ (argvars[0].vval.v_object != عدم)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    اكسر;
	حالة VAR_TYPEALIAS:
	    ܐܢ (argvars[0].vval.v_typealias != عدم)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    اكسر;
    }

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    فراغ
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ܐܢ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    ܐܚܪܢܐ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    فراغ
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    فراغ
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    ܐܢ (check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    فراغ
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = عدم;
}

#ܐܢ defined(FEAT_JOB_CHANNEL)
    فراغ
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = عدم;
}
#endif

    فراغ
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, عدم);
}

#ܐܢ defined(FEAT_JOB_CHANNEL)
    فراغ
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = عدم;
}
#endif

    فراغ
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, عدم);
}

    فراغ
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = عدم;
}

    فراغ
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = عدم;
}

    فراغ
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
}

    فراغ
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, عدم);
}

    فراغ
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    فراغ
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    فراغ
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    ܐܢ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    ܐܢ (argvars[0].v_type != VAR_NUMBER || argvars[1].v_type != VAR_NUMBER)
    {
	emsg(_(e_invalid_argument));
	ܦܢܐ;
    }

    mouse_row = (time_t)tv_get_number(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_number(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    ساكن صحيح
test_gui_drop_files(dict_T *args UNUSED)
{
# ܐܢ defined(HAVE_DROP_FILE)
    صحيح		row;
    صحيح		col;
    int_u	mods;
    char_u	**fnames;
    صحيح		count = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ܐܢ (!dict_has_key(args, "files")
	    || !dict_has_key(args, "row")
	    || !dict_has_key(args, "col")
	    || !dict_has_key(args, "modifiers"))
	ܦܢܐ FALSE;

    (فراغ)dict_get_tv(args, "files", &t);
    row = (صحيح)dict_get_number(args, "row");
    col = (صحيح)dict_get_number(args, "col");
    mods = (صحيح)dict_get_number(args, "modifiers");

    ܐܢ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ܦܢܐ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ܐܢ (fnames == عدم)
	ܦܢܐ FALSE;

    FOR_ALL_LIST_ITEMS(l, li)
    {
	// ignore non-string items
	ܐܢ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == عدم)
	    استمر;

	fnames[count] = vim_strsave(li->li_tv.vval.v_string);
	ܐܢ (fnames[count] == عدم)
	{
	    ܟܡܐ (--count >= 0)
		vim_free(fnames[count]);
	    vim_free(fnames);
	    ܦܢܐ FALSE;
	}
	count++;
    }

    ܐܢ (count > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, count);
    ܐܚܪܢܐ
	vim_free(fnames);
# endif

    ܦܢܐ TRUE;
}

# ܐܢ defined(FIND_REPLACE_DIALOG)
    ساكن صحيح
test_gui_find_repl(dict_T *args)
{
    صحيح		flags;
    char_u	*find_text;
    char_u	*repl_text;
    صحيح		forward;
    صحيح		retval;

    ܐܢ (!dict_has_key(args, "find_text")
	    || !dict_has_key(args, "repl_text")
	    || !dict_has_key(args, "flags")
	    || !dict_has_key(args, "forward"))
	ܦܢܐ FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (صحيح)dict_get_number(args, "flags");
    forward = (صحيح)dict_get_number(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    ܦܢܐ retval;
}
# endif

    ساكن صحيح
test_gui_mouse_event(dict_T *args)
{
    صحيح		button;
    صحيح		row;
    صحيح		col;
    صحيح		repeated_click;
    int_u	mods;
    صحيح		move;

    ܐܢ (!dict_has_key(args, "row")
	    || !dict_has_key(args, "col"))
	ܦܢܐ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحيح)dict_get_bool(args, "move", FALSE);

    ܐܢ (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	ܦܢܐ FALSE;

    row = (صحيح)dict_get_number(args, "row");
    col = (صحيح)dict_get_number(args, "col");

    ܐܢ (move)
    {
	صحيح pY = row;
	صحيح pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ܐܢ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ܐܚܪܢܐ
    {
	button = (صحيح)dict_get_number(args, "button");
	repeated_click = (صحيح)dict_get_number(args, "multiclick");
	mods = (صحيح)dict_get_number(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    ܦܢܐ TRUE;
}

    ساكن صحيح
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	value;
    صحيح		dragging;
    scrollbar_T *sb = عدم;

    ܐܢ (!dict_has_key(args, "which")
	    || !dict_has_key(args, "value")
	    || !dict_has_key(args, "dragging"))
	ܦܢܐ FALSE;

    which = dict_get_string(args, "which", FALSE);
    value = (long)dict_get_number(args, "value");
    dragging = (صحيح)dict_get_number(args, "dragging");

    ܐܢ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_LEFT];
    ܐܚܪܢܐ ܐܢ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_RIGHT];
    ܐܚܪܢܐ ܐܢ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ܐܢ (sb == عدم)
    {
	semsg(_(e_invalid_argument_str), which);
	ܦܢܐ FALSE;
    }
    gui_drag_scrollbar(sb, value, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    ܦܢܐ TRUE;
}

    ساكن صحيح
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    صحيح		tabnr;

    ܐܢ (!dict_has_key(args, "tabnr"))
	ܦܢܐ FALSE;

    tabnr = (صحيح)dict_get_number(args, "tabnr");

    ܦܢܐ send_tabline_event(tabnr);
# ܐܚܪܢܐ
    ܦܢܐ FALSE;
# endif
}

    ساكن صحيح
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    صحيح	tabnr;
    صحيح	item;

    ܐܢ (!dict_has_key(args, "tabnr")
	    || !dict_has_key(args, "item"))
	ܦܢܐ FALSE;

    tabnr = (صحيح)dict_get_number(args, "tabnr");
    item = (صحيح)dict_get_number(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    ܦܢܐ TRUE;
}
#endif

    فراغ
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = FALSE;

    ܐܢ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ܦܢܐ;
    }

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == عدم)
	ܦܢܐ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_number = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    فراغ
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = FALSE;

    ܐܢ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ܦܢܐ;
    }

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == عدم)
	ܦܢܐ;

    event = tv_get_string(&argvars[0]);
    ܐܢ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_number = test_gui_drop_files(argvars[1].vval.v_dict);
# ܐܢ defined(FIND_REPLACE_DIALOG)
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_number = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_number = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_number = test_gui_mouse_event(argvars[1].vval.v_dict);
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_number = test_gui_scrollbar(argvars[1].vval.v_dict);
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_number = test_gui_tabline_event(argvars[1].vval.v_dict);
    ܐܚܪܢܐ ܐܢ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_number = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ܐܚܪܢܐ
    {
	semsg(_(e_invalid_argument_str), event);
	ܦܢܐ;
    }
#endif
}

    فراغ
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    ܐܢ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    time_for_testing = (time_t)tv_get_number(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
