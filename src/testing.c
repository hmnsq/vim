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

#إذا defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ساكن فراغ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    إذا (sname != عدم)
    {
	ga_concat(gap, sname);
	إذا (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    إذا (SOURCING_LNUM > 0)
    {
	حرف    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, حجم(buf), "line %ld", (ڊگهو)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    إذا (sname != عدم || SOURCING_LNUM > 0)
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

    إذا (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	أرجع;
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
	   إذا (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   وإلا
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

    إذا (str == عدم)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	أرجع;
    }

    لكل (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	طالما (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	إذا (same_len > 20)
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
	وإلا
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
    typval_T	*opt_نياپو_tv,
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

    إذا (opt_نياپو_tv->v_type != VAR_UNKNOWN
	    && !(opt_نياپو_tv->v_type == VAR_STRING
		&& (opt_نياپو_tv->vval.v_string == عدم
		    || *opt_نياپو_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_نياپو_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    إذا (atype == ASSERT_ميلاپ || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    وإلا إذا (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    وإلا
	GA_CONCAT_LITERAL(gap, "Expected ");
    إذا (exp_str == عدم)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	إذا (atype != ASSERT_NOTEQUAL
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
	    إذا (exp_tv->vval.v_dict == عدم || got_tv->vval.v_dict == عدم)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = عدم;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = عدم;
		أرجع;
	    }

	    todo = (صحيح)exp_d->dv_hashtab.ht_استعمال_ٿيل;
	    FOR_ALL_HASHTAB_شيون(&exp_d->dv_hashtab, hi, todo)
	    {
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    item2 = dict_ڳوليو(got_d, hi->hi_ڪنجي, -1);
		    إذا (item2 == عدم || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_جوڙيو_tv(exp_tv->vval.v_dict,
					(حرف *)hi->hi_ڪنجي, &HI2DI(hi)->di_tv);
			إذا (item2 != عدم)
			    dict_جوڙيو_tv(got_tv->vval.v_dict,
					    (حرف *)hi->hi_ڪنجي, &item2->di_tv);
		    }
		    وإلا
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (صحيح)got_d->dv_hashtab.ht_استعمال_ٿيل;
	    FOR_ALL_HASHTAB_شيون(&got_d->dv_hashtab, hi, todo)
	    {
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    item2 = dict_ڳوليو(exp_d, hi->hi_ڪنجي, -1);
		    إذا (item2 == عدم)
			// item of got_d not present in exp_d
			dict_جوڙيو_tv(got_tv->vval.v_dict,
					(حرف *)hi->hi_ڪنجي, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    وإلا
    {
	إذا (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	إذا (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    إذا (atype != ASSERT_NOTEQUAL)
    {
	إذا (atype == ASSERT_ميلاپ)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	وإلا إذا (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	وإلا
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	إذا (omitted != 0)
	{
	    حرف    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, حجم(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    إذا (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ساكن صحيح
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    إذا (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], عدم, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	أرجع 1;
    }
    أرجع 0;
}

    ساكن صحيح
assert_ميلاپ_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *متن = tv_get_string_buf_chk(&argvars[1], buf2);
    إذا (pat != عدم && متن != عدم
		 && نمونو_ميلاپ(pat, متن, FALSE) != (atype == ASSERT_ميلاپ))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], عدم, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	أرجع 1;
    }
    أرجع 0;
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

    إذا (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_عدد == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	أرجع 0;
    إذا (argvars[0].v_type != VAR_عدد
	    || (tv_get_عدد_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		عدم, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	أرجع 1;
    }
    أرجع 0;
}

    ساكن فراغ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    إذا (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    وإلا
	ga_concat(gap, cmd);
}

    ساكن صحيح
assert_beeps(typval_T *argvars, صحيح no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    صحيح		ret = 0;

    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    إذا (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	إذا (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	وإلا
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    أرجع ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    فراغ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    فراغ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    فراغ
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_equal_common(argvars, ASSERT_EQUAL);
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

    إذا (fname1 == عدم || fname2 == عدم)
	أرجع 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    إذا (fd1 == عدم)
    {
	IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    (حرف *)e_cant_read_file_str, fname1);
    }
    وإلا
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	إذا (fd2 == عدم)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
		(حرف *)e_cant_read_file_str, fname2);
	}
	وإلا
	{
	    صحيح	    c1, c2;
	    ڊگهو    ڳڻپ = 0;
	    ڊگهو    linecount = 1;

	    لكل (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		إذا (c1 == EOF)
		{
		    إذا (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    اكسر;
		}
		وإلا إذا (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    اكسر;
		}
		وإلا
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    إذا (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ڳڻپ, linecount);
			اكسر;
		    }
		}
		++ڳڻپ;
		إذا (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		وإلا إذا (lineidx + 2 == (صحيح)حجم(line1))
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

    إذا (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	إذا (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    إذا (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	أرجع 1;
    }

    أرجع 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    فراغ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    rettv->vval.v_عدد = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    فراغ
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    فراغ
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    error = tv_get_string_chk(&argvars[0]);
    إذا (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
    }
    وإلا إذا (error != عدم
	&& لڙي_ڳولا((حرف *)get_vim_var_str(VV_EXCEPTION), (حرف *)error) == عدم)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], عدم, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
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
    حرف	*wrong_arg_نياپو = عدم;
    char_u	*tofree = عدم;

    إذا (check_for_string_or_عدد_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	أرجع;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    اندريون_assert_fails = TRUE;
    ++no_wait_return;

    char_u *cmd = tv_get_string_chk(&argvars[0]);
    do_cmdline_cmd(cmd);

    // reset here for any errors reported below
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;

    إذا (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_عدد = 1;
    }
    وإلا إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = عدم;
	صحيح	error_مليو = FALSE;
	صحيح	error_مليو_اشاريو = 1;
	char_u	*actual = emsg_assert_fails_نياپو == عدم ? (char_u *)"[unknown]"
						       : emsg_assert_fails_نياپو;

	إذا (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_مليو = expected == عدم
			   || لڙي_ڳولا((حرف *)actual, (حرف *)expected) == عدم;
	}
	وإلا إذا (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    إذا (list == عدم || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_نياپو = e_assert_fails_ٻيو_arg;
		اذهب theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_پهريون->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    إذا (expected == عدم)
		اذهب theend;
	    إذا (!نمونو_ميلاپ(expected, actual, FALSE))
	    {
		error_مليو = TRUE;
		expected_str = expected;
	    }
	    وإلا إذا (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		إذا (actual != عدم)
		{
		    tv = &list->lv_u.mat.lv_آخري->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    إذا (expected == عدم)
			اذهب theend;
		    إذا (!نمونو_ميلاپ(expected, actual, FALSE))
		    {
			error_مليو = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	وإلا
	{
	    wrong_arg_نياپو = e_assert_fails_ٻيو_arg;
	    اذهب theend;
	}

	إذا (!error_مليو && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    إذا (argvars[3].v_type != VAR_عدد)
	    {
		wrong_arg_نياپو = e_assert_fails_fourth_دليل;
		اذهب theend;
	    }
	    وإلا إذا (argvars[3].vval.v_عدد >= 0
			 && argvars[3].vval.v_عدد != emsg_assert_fails_lnum)
	    {
		error_مليو = TRUE;
		error_مليو_اشاريو = 3;
	    }
	    إذا (!error_مليو && argvars[4].v_type != VAR_UNKNOWN)
	    {
		إذا (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_نياپو = e_assert_fails_fifth_دليل;
		    اذهب theend;
		}
		وإلا إذا (argvars[4].vval.v_string != عدم
		    && !نمونو_ميلاپ(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_مليو = TRUE;
		    error_مليو_اشاريو = 4;
		}
	    }
	}

	إذا (error_مليو)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    إذا (error_مليو_اشاريو == 3)
	    {
		actual_tv.v_type = VAR_عدد;
		actual_tv.vval.v_عدد = emsg_assert_fails_lnum;
	    }
	    وإلا إذا (error_مليو_اشاريو == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    وإلا
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_مليو_اشاريو], &actual_tv, ASSERT_FAILS);
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
    اندريون_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    نياپو_کانو = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    نياپو_scrolled = 0;
    سٽون_کاٻو = صفون;
    VIM_CLEAR(emsg_assert_fails_نياپو);
    vim_free(tofree);
    سيٽ_vim_var_string(VV_ERRMSG, عدم, 0);
    إذا (wrong_arg_نياپو != عدم)
	emsg(_(wrong_arg_نياپو));
}

/*
 * "assert_false(actual[, msg])" function
 */
    فراغ
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_bool(argvars, FALSE);
}

    ساكن صحيح
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    صحيح		error = FALSE;
    char_u	expected_str[200];

    إذا (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	إذا (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((حرف *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, عدم,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    أرجع 1;
	}
    }
    وإلا
    {
	varnumber_T	هيٺاهون = tv_get_عدد_chk(&argvars[0], &error);
	varnumber_T	مٿانهون_2 = tv_get_عدد_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_عدد_chk(&argvars[2], &error);

	إذا (error)
	    أرجع 0;
	إذا (actual < هيٺاهون || actual > مٿانهون_2)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((حرف *)expected_str, 200, "range %ld - %ld,",
						     (ڊگهو)هيٺاهون, (ڊگهو)مٿانهون_2);
	    fill_assert_error(&ga, &argvars[3], expected_str, عدم,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    أرجع 1;
	}
    }
    أرجع 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    فراغ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    إذا (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	أرجع;

    rettv->vval.v_عدد = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    فراغ
f_assert_ميلاپ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_ميلاپ_common(argvars, ASSERT_ميلاپ);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    فراغ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_ميلاپ_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    فراغ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_عدد = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    فراغ
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    فراغ
f_test_alloc_fail(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_عدد
	    || argvars[0].vval.v_عدد <= 0
	    || argvars[1].v_type != VAR_عدد
	    || argvars[1].vval.v_عدد < 0
	    || argvars[2].v_type != VAR_عدد)
	emsg(_(e_invalid_دليل));
    وإلا
    {
	alloc_fail_id = argvars[0].vval.v_عدد;
	إذا (alloc_fail_id >= aid_آخري)
	    emsg(_(e_invalid_دليل));
	alloc_fail_countdown = argvars[1].vval.v_عدد;
	alloc_fail_repeat = argvars[2].vval.v_عدد;
	did_outofmem_نياپو = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    فراغ
f_test_autochdir(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
#إذا defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#جيڪڏهن_پورو
}

/*
 * "test_feedinput()"
 */
    فراغ
f_test_feedinput(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل USE_داخلا_BUF
    char_u	*val;

    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    إذا (!gui.اندريون_use)
	أرجع;
# endif

    إذا (val != عدم)
    {
	trash_داخلا_buf();
	جوڙيو_منزل_داخلا_buf_csi(val, (صحيح)STRLEN(val));
    }
#جيڪڏهن_پورو
}

/*
 * "test_getvalue({name})" function
 */
    فراغ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    name = tv_get_string(&argvars[0]);

    إذا (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_عدد = need_fileinfo;
    وإلا
	semsg(_(e_invalid_دليل_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    فراغ
f_test_option_not_سيٽ(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    char_u *name = (char_u *)"";

    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (reset_option_was_سيٽ(name) == FAIL)
	semsg(_(e_invalid_دليل_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    فراغ
f_test_override(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    char_u *name = (char_u *)"";
    صحيح     val;
    ساكن صحيح save_starting = -1;

    إذا (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL)
	أرجع;

    name = tv_get_string(&argvars[0]);
    val = (صحيح)tv_get_عدد(&argvars[1]);

    إذا (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"starting") == 0)
    {
	إذا (val)
	{
	    إذا (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	وإلا
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    وإلا إذا (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    وإلا إذا (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_ڪوڊ_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"term_props") == 0)
	reset_رڪن_props_on_termresponse = val;
    وإلا إذا (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    وإلا إذا (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    وإلا إذا (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_سٽون = val;
    وإلا إذا (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    وإلا إذا (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    وإلا إذا (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_رڪن_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	إذا (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    وإلا
	semsg(_(e_invalid_دليل_str), name);
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
	حالة VAR_عدد:
	حالة VAR_BOOL:
	حالة VAR_FLOAT:
	حالة VAR_SPECIAL:
	حالة VAR_STRING:
	حالة VAR_INSTR:
	    اكسر;

	حالة VAR_ڪم:
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	    إذا (argvars[0].vval.v_ڪم != عدم)
		retval = argvars[0].vval.v_ڪم->jv_refcount - 1;
#جيڪڏهن_پورو
	    اكسر;
	حالة VAR_CHANNEL:
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	    إذا (argvars[0].vval.v_channel != عدم)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#جيڪڏهن_پورو
	    اكسر;
	حالة VAR_FUNC:
	    إذا (argvars[0].vval.v_string != عدم)
	    {
		ufunc_T *fp;

		fp = ڳوليو_func(argvars[0].vval.v_string, FALSE);
		إذا (fp != عدم)
		    retval = fp->uf_refcount;
	    }
	    اكسر;
	حالة VAR_PARTIAL:
	    إذا (argvars[0].vval.v_partial != عدم)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    اكسر;
	حالة VAR_BLOB:
	    إذا (argvars[0].vval.v_blob != عدم)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    اكسر;
	حالة VAR_LIST:
	    إذا (argvars[0].vval.v_list != عدم)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    اكسر;
	حالة VAR_TUPLE:
	    إذا (argvars[0].vval.v_tuple != عدم)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    اكسر;
	حالة VAR_DICT:
	    إذا (argvars[0].vval.v_dict != عدم)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    اكسر;
	حالة VAR_CLASS:
	    إذا (argvars[0].vval.v_class != عدم)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    اكسر;
	حالة VAR_OBJECT:
	    إذا (argvars[0].vval.v_object != عدم)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    اكسر;
	حالة VAR_TYPEALIAS:
	    إذا (argvars[0].vval.v_typealias != عدم)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    اكسر;
    }

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    فراغ
f_test_garbagecollect_now(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    إذا (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_سيٽ));
    وإلا
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    فراغ
f_test_garbagecollect_soon(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    فراغ
f_test_ignore_error(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    فراغ
f_test_null_blob(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = عدم;
}

#إذا defined(FEAT_ڪم_CHANNEL)
    فراغ
f_test_null_channel(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = عدم;
}
#جيڪڏهن_پورو

    فراغ
f_test_null_dict(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv_dict_سيٽ(rettv, عدم);
}

#إذا defined(FEAT_ڪم_CHANNEL)
    فراغ
f_test_null_ڪم(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_ڪم;
    rettv->vval.v_ڪم = عدم;
}
#جيڪڏهن_پورو

    فراغ
f_test_null_list(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv_list_سيٽ(rettv, عدم);
}

    فراغ
f_test_null_function(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = عدم;
}

    فراغ
f_test_null_partial(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = عدم;
}

    فراغ
f_test_null_string(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
}

    فراغ
f_test_null_tuple(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv_tuple_سيٽ(rettv, عدم);
}

    فراغ
f_test_unknown(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    فراغ
f_test_void(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    فراغ
f_test_setmouse(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_عدد || argvars[1].v_type != VAR_عدد)
    {
	emsg(_(e_invalid_دليل));
	أرجع;
    }

    mouse_صف = (time_t)tv_get_عدد(&argvars[0]) - 1;
    mouse_کانو = (time_t)tv_get_عدد(&argvars[1]) - 1;
}

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    ساكن صحيح
test_gui_drop_files(dict_T *دليلون اڻ_استعمال_ٿيل)
{
# إذا defined(HAVE_DROP_FILE)
    صحيح		صف;
    صحيح		کانو;
    int_u	mods;
    char_u	**fnames;
    صحيح		ڳڻپ = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    إذا (!dict_has_ڪنجي(دليلون, "files")
	    || !dict_has_ڪنجي(دليلون, "row")
	    || !dict_has_ڪنجي(دليلون, "col")
	    || !dict_has_ڪنجي(دليلون, "modifiers"))
	أرجع FALSE;

    (فراغ)dict_get_tv(دليلون, "files", &t);
    صف = (صحيح)dict_get_عدد(دليلون, "row");
    کانو = (صحيح)dict_get_عدد(دليلون, "col");
    mods = (صحيح)dict_get_عدد(دليلون, "modifiers");

    إذا (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	أرجع FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    إذا (fnames == عدم)
	أرجع FALSE;

    FOR_ALL_LIST_شيون(l, li)
    {
	// ignore non-string items
	إذا (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == عدم)
	    استمر;

	fnames[ڳڻپ] = vim_strsave(li->li_tv.vval.v_string);
	إذا (fnames[ڳڻپ] == عدم)
	{
	    طالما (--ڳڻپ >= 0)
		vim_free(fnames[ڳڻپ]);
	    vim_free(fnames);
	    أرجع FALSE;
	}
	ڳڻپ++;
    }

    إذا (ڳڻپ > 0)
	gui_handle_drop(متن_X(کانو - 1), متن_Y(صف - 1), mods, fnames, ڳڻپ);
    وإلا
	vim_free(fnames);
# endif

    أرجع TRUE;
}

# إذا defined(ڳوليو_REPLACE_DIALOG)
    ساكن صحيح
test_gui_ڳوليو_repl(dict_T *دليلون)
{
    صحيح		flags;
    char_u	*ڳوليو_متن;
    char_u	*repl_متن;
    صحيح		forward;
    صحيح		retval;

    إذا (!dict_has_ڪنجي(دليلون, "find_text")
	    || !dict_has_ڪنجي(دليلون, "repl_text")
	    || !dict_has_ڪنجي(دليلون, "flags")
	    || !dict_has_ڪنجي(دليلون, "forward"))
	أرجع FALSE;

    ڳوليو_متن = dict_get_string(دليلون, "find_text", TRUE);
    repl_متن = dict_get_string(دليلون, "repl_text", TRUE);
    flags = (صحيح)dict_get_عدد(دليلون, "flags");
    forward = (صحيح)dict_get_عدد(دليلون, "forward");

    retval = gui_do_findrepl(flags, ڳوليو_متن, repl_متن, forward);
    vim_free(ڳوليو_متن);
    vim_free(repl_متن);

    أرجع retval;
}
# endif

    ساكن صحيح
test_gui_mouse_event(dict_T *دليلون)
{
    صحيح		button;
    صحيح		صف;
    صحيح		کانو;
    صحيح		repeated_click;
    int_u	mods;
    صحيح		move;

    إذا (!dict_has_ڪنجي(دليلون, "row")
	    || !dict_has_ڪنجي(دليلون, "col"))
	أرجع FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحيح)dict_get_bool(دليلون, "move", FALSE);

    إذا (!move && (!dict_has_ڪنجي(دليلون, "button")
	    || !dict_has_ڪنجي(دليلون, "multiclick")
	    || !dict_has_ڪنجي(دليلون, "modifiers")))
	أرجع FALSE;

    صف = (صحيح)dict_get_عدد(دليلون, "row");
    کانو = (صحيح)dict_get_عدد(دليلون, "col");

    إذا (move)
    {
	صحيح pY = صف;
	صحيح pX = کانو;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	إذا (dict_get_bool(دليلون, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (صف - 1) * gui.char_اوچائي + gui.char_اوچائي / 2;
	    pX = (کانو - 1) * gui.char_ويڪر + gui.char_ويڪر / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    وإلا
    {
	button = (صحيح)dict_get_عدد(دليلون, "button");
	repeated_click = (صحيح)dict_get_عدد(دليلون, "multiclick");
	mods = (صحيح)dict_get_عدد(دليلون, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سيٽ_hor_scroll_قدم(6);
	mouse_سيٽ_vert_scroll_قدم(3);

	gui_send_mouse_event(button, متن_X(کانو - 1), متن_Y(صف - 1),
							repeated_click, mods);
    }

    أرجع TRUE;
}

    ساكن صحيح
test_gui_scrollbar(dict_T *دليلون)
{
    char_u	*which;
    ڊگهو	قدر;
    صحيح		dragging;
    scrollbar_T *sb = عدم;

    إذا (!dict_has_ڪنجي(دليلون, "which")
	    || !dict_has_ڪنجي(دليلون, "value")
	    || !dict_has_ڪنجي(دليلون, "dragging"))
	أرجع FALSE;

    which = dict_get_string(دليلون, "which", FALSE);
    قدر = (ڊگهو)dict_get_عدد(دليلون, "value");
    dragging = (صحيح)dict_get_عدد(دليلون, "dragging");

    إذا (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_کاٻو];
    وإلا إذا (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ساڄو];
    وإلا إذا (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    إذا (sb == عدم)
    {
	semsg(_(e_invalid_دليل_str), which);
	أرجع FALSE;
    }
    gui_drag_scrollbar(sb, قدر, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_عام(FALSE, TRUE, FALSE);
# endif

    أرجع TRUE;
}

    ساكن صحيح
test_gui_tabline_event(dict_T *دليلون اڻ_استعمال_ٿيل)
{
# ifdef FEAT_GUI_TABLINE
    صحيح		tabnr;

    إذا (!dict_has_ڪنجي(دليلون, "tabnr"))
	أرجع FALSE;

    tabnr = (صحيح)dict_get_عدد(دليلون, "tabnr");

    أرجع send_tabline_event(tabnr);
# وإلا
    أرجع FALSE;
# endif
}

    ساكن صحيح
test_gui_tabmenu_event(dict_T *دليلون اڻ_استعمال_ٿيل)
{
# ifdef FEAT_GUI_TABLINE
    صحيح	tabnr;
    صحيح	شي;

    إذا (!dict_has_ڪنجي(دليلون, "tabnr")
	    || !dict_has_ڪنجي(دليلون, "item"))
	أرجع FALSE;

    tabnr = (صحيح)dict_get_عدد(دليلون, "tabnr");
    شي = (صحيح)dict_get_عدد(دليلون, "item");

    send_tabline_menu_event(tabnr, شي);
# endif
    أرجع TRUE;
}
#جيڪڏهن_پورو

    فراغ
f_test_mswin_event(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = FALSE;

    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندريون_sandbox));
	أرجع;
    }

    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == عدم)
	أرجع;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_عدد = test_mswin_event(event, argvars[1].vval.v_dict);

#جيڪڏهن_پورو
}

    فراغ
f_test_gui_event(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = FALSE;

    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندريون_sandbox));
	أرجع;
    }

    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == عدم)
	أرجع;

    event = tv_get_string(&argvars[0]);
    إذا (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_عدد = test_gui_drop_files(argvars[1].vval.v_dict);
# إذا defined(ڳوليو_REPLACE_DIALOG)
    وإلا إذا (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_عدد = test_gui_ڳوليو_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    وإلا إذا (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_عدد = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    وإلا إذا (STRCMP(event, "mouse") == 0)
	rettv->vval.v_عدد = test_gui_mouse_event(argvars[1].vval.v_dict);
    وإلا إذا (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_عدد = test_gui_scrollbar(argvars[1].vval.v_dict);
    وإلا إذا (STRCMP(event, "tabline") == 0)
	rettv->vval.v_عدد = test_gui_tabline_event(argvars[1].vval.v_dict);
    وإلا إذا (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_عدد = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    وإلا
    {
	semsg(_(e_invalid_دليل_str), event);
	أرجع;
    }
#جيڪڏهن_پورو
}

    فراغ
f_test_settime(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    وقت_for_testing = (time_t)tv_get_عدد(&argvars[0]);
}

#جيڪڏهن_پورو // defined(FEAT_EVAL)
