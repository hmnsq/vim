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
    ساکن خالی
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    اگر (sname != تہی)
    {
	ga_concat(gap, sname);
	اگر (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    اگر (SOURCING_LNUM > 0)
    {
	حرف    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, حجم(buf), "line %ld", (طویل)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    اگر (sname != تہی || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ساکن خالی
ga_concat_esc(garray_T *gap, char_u *p, صحیح clen)
{
    char_u  buf[NUMBUFLEN];

    اگر (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	واپس;
    }

    انتخاب (*p)
    {
	حالت BS: GA_CONCAT_LITERAL(gap, "\\b"); توڑو;
	حالت ESC: GA_CONCAT_LITERAL(gap, "\\e"); توڑو;
	حالت FF: GA_CONCAT_LITERAL(gap, "\\f"); توڑو;
	حالت NL: GA_CONCAT_LITERAL(gap, "\\n"); توڑو;
	حالت TAB: GA_CONCAT_LITERAL(gap, "\\t"); توڑو;
	حالت CAR: GA_CONCAT_LITERAL(gap, "\\r"); توڑو;
	حالت '\\': GA_CONCAT_LITERAL(gap, "\\\\"); توڑو;
	طےشدہ:
	   اگر (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ورنہ
	       ga_append(gap, *p);
	   توڑو;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ساکن خالی
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    صحیح	    c;
    صحیح	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    صحیح	    same_len;

    اگر (str == تہی)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	واپس;
    }

    برائے (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	جب (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	اگر (same_len > 20)
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
	ورنہ
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ساکن خالی
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_پیغام_tv,
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

    اگر (opt_پیغام_tv->v_type != VAR_UNKNOWN
	    && !(opt_پیغام_tv->v_type == VAR_STRING
		&& (opt_پیغام_tv->vval.v_string == تہی
		    || *opt_پیغام_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_پیغام_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    اگر (atype == ASSERT_مطابقت || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ورنہ اگر (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ورنہ
	GA_CONCAT_LITERAL(gap, "Expected ");
    اگر (exp_str == تہی)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	اگر (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != تہی && got_tv->vval.v_dict != تہی)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    صحیح		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    اگر (exp_tv->vval.v_dict == تہی || got_tv->vval.v_dict == تہی)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = تہی;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = تہی;
		واپس;
	    }

	    todo = (صحیح)exp_d->dv_hashtab.ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(&exp_d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    item2 = dict_ڈھونڈو(got_d, hi->hi_کلید, -1);
		    اگر (item2 == تہی || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_جوڑو_tv(exp_tv->vval.v_dict,
					(حرف *)hi->hi_کلید, &HI2DI(hi)->di_tv);
			اگر (item2 != تہی)
			    dict_جوڑو_tv(got_tv->vval.v_dict,
					    (حرف *)hi->hi_کلید, &item2->di_tv);
		    }
		    ورنہ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (صحیح)got_d->dv_hashtab.ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(&got_d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    item2 = dict_ڈھونڈو(exp_d, hi->hi_کلید, -1);
		    اگر (item2 == تہی)
			// item of got_d not present in exp_d
			dict_جوڑو_tv(got_tv->vval.v_dict,
					(حرف *)hi->hi_کلید, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ورنہ
    {
	اگر (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	اگر (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    اگر (atype != ASSERT_NOTEQUAL)
    {
	اگر (atype == ASSERT_مطابقت)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ورنہ اگر (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ورنہ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	اگر (omitted != 0)
	{
	    حرف    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, حجم(buf),
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

    ساکن صحیح
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    اگر (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], تہی, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	واپس 1;
    }
    واپس 0;
}

    ساکن صحیح
assert_مطابقت_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *متن = tv_get_string_buf_chk(&argvars[1], buf2);
    اگر (pat != تہی && متن != تہی
		 && نمونہ_مطابقت(pat, متن, FALSE) != (atype == ASSERT_مطابقت))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], تہی, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	واپس 1;
    }
    واپس 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ساکن صحیح
assert_bool(typval_T *argvars, صحیح isTrue)
{
    صحیح		error = FALSE;
    garray_T	ga;

    اگر (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_نمبر == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	واپس 0;
    اگر (argvars[0].v_type != VAR_نمبر
	    || (tv_get_نمبر_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		تہی, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	واپس 1;
    }
    واپس 0;
}

    ساکن خالی
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ورنہ
	ga_concat(gap, cmd);
}

    ساکن صحیح
assert_beeps(typval_T *argvars, صحیح no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    صحیح		ret = 0;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس 0;

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
	ورنہ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    واپس ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    خالی
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    خالی
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    خالی
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ساکن صحیح
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
    صحیح		lineidx = 0;
    size_t	IObufflen;

    اگر (fname1 == تہی || fname2 == تہی)
	واپس 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    اگر (fd1 == تہی)
    {
	IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    (حرف *)e_cant_read_file_str, fname1);
    }
    ورنہ
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	اگر (fd2 == تہی)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
		(حرف *)e_cant_read_file_str, fname2);
	}
	ورنہ
	{
	    صحیح	    c1, c2;
	    طویل    گنتی = 0;
	    طویل    linecount = 1;

	    برائے (;;)
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
		    توڑو;
		}
		ورنہ اگر (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    توڑو;
		}
		ورنہ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    اگر (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", گنتی, linecount);
			توڑو;
		    }
		}
		++گنتی;
		اگر (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ورنہ اگر (lineidx + 2 == (صحیح)حجم(line1))
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
	واپس 1;
    }

    واپس 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    خالی
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    rettv->vval.v_نمبر = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    خالی
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    خالی
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    error = tv_get_string_chk(&argvars[0]);
    اگر (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_نمبر = 1;
    }
    ورنہ اگر (error != تہی
	&& لڑی_تلاش((حرف *)get_vim_var_str(VV_EXCEPTION), (حرف *)error) == تہی)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], تہی, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_نمبر = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    خالی
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    صحیح		save_trylevel = trylevel;
    صحیح		called_emsg_before = called_emsg;
    حرف	*wrong_arg_پیغام = تہی;
    char_u	*tofree = تہی;

    اگر (check_for_string_or_نمبر_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_نمبر_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	واپس;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    اندر_assert_fails = TRUE;
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
	rettv->vval.v_نمبر = 1;
    }
    ورنہ اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = تہی;
	صحیح	error_ملا = FALSE;
	صحیح	error_ملا_اشاریہ = 1;
	char_u	*actual = emsg_assert_fails_پیغام == تہی ? (char_u *)"[unknown]"
						       : emsg_assert_fails_پیغام;

	اگر (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_ملا = expected == تہی
			   || لڑی_تلاش((حرف *)actual, (حرف *)expected) == تہی;
	}
	ورنہ اگر (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    اگر (list == تہی || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_پیغام = e_assert_fails_دوسرا_arg;
		جاؤ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_پہلا->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    اگر (expected == تہی)
		جاؤ theend;
	    اگر (!نمونہ_مطابقت(expected, actual, FALSE))
	    {
		error_ملا = TRUE;
		expected_str = expected;
	    }
	    ورنہ اگر (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		اگر (actual != تہی)
		{
		    tv = &list->lv_u.mat.lv_آخری->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    اگر (expected == تہی)
			جاؤ theend;
		    اگر (!نمونہ_مطابقت(expected, actual, FALSE))
		    {
			error_ملا = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ورنہ
	{
	    wrong_arg_پیغام = e_assert_fails_دوسرا_arg;
	    جاؤ theend;
	}

	اگر (!error_ملا && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    اگر (argvars[3].v_type != VAR_نمبر)
	    {
		wrong_arg_پیغام = e_assert_fails_fourth_دلیل;
		جاؤ theend;
	    }
	    ورنہ اگر (argvars[3].vval.v_نمبر >= 0
			 && argvars[3].vval.v_نمبر != emsg_assert_fails_lnum)
	    {
		error_ملا = TRUE;
		error_ملا_اشاریہ = 3;
	    }
	    اگر (!error_ملا && argvars[4].v_type != VAR_UNKNOWN)
	    {
		اگر (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_پیغام = e_assert_fails_fifth_دلیل;
		    جاؤ theend;
		}
		ورنہ اگر (argvars[4].vval.v_string != تہی
		    && !نمونہ_مطابقت(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_ملا = TRUE;
		    error_ملا_اشاریہ = 4;
		}
	    }
	}

	اگر (error_ملا)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    اگر (error_ملا_اشاریہ == 3)
	    {
		actual_tv.v_type = VAR_نمبر;
		actual_tv.vval.v_نمبر = emsg_assert_fails_lnum;
	    }
	    ورنہ اگر (error_ملا_اشاریہ == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ورنہ
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_ملا_اشاریہ], &actual_tv, ASSERT_FAILS);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    assert_append_cmd_or_arg(&ga, argvars, cmd);
	    assert_error(&ga);
	    ga_clear(&ga);
	    rettv->vval.v_نمبر = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    اندر_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    پیغام_ستون = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    پیغام_scrolled = 0;
    سطریں_بایاں = صفیں;
    VIM_CLEAR(emsg_assert_fails_پیغام);
    vim_free(tofree);
    سیٹ_vim_var_string(VV_ERRMSG, تہی, 0);
    اگر (wrong_arg_پیغام != تہی)
	emsg(_(wrong_arg_پیغام));
}

/*
 * "assert_false(actual[, msg])" function
 */
    خالی
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_bool(argvars, FALSE);
}

    ساکن صحیح
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
	    vim_snprintf((حرف *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, تہی,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    واپس 1;
	}
    }
    ورنہ
    {
	varnumber_T	زیریں = tv_get_نمبر_chk(&argvars[0], &error);
	varnumber_T	بالائی_2 = tv_get_نمبر_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_نمبر_chk(&argvars[2], &error);

	اگر (error)
	    واپس 0;
	اگر (actual < زیریں || actual > بالائی_2)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((حرف *)expected_str, 200, "range %ld - %ld,",
						     (طویل)زیریں, (طویل)بالائی_2);
	    fill_assert_error(&ga, &argvars[3], expected_str, تہی,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    واپس 1;
	}
    }
    واپس 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    خالی
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	واپس;

    rettv->vval.v_نمبر = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    خالی
f_assert_مطابقت(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_مطابقت_common(argvars, ASSERT_مطابقت);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    خالی
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_مطابقت_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    خالی
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_نمبر = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    خالی
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    خالی
f_test_alloc_fail(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_نمبر
	    || argvars[0].vval.v_نمبر <= 0
	    || argvars[1].v_type != VAR_نمبر
	    || argvars[1].vval.v_نمبر < 0
	    || argvars[2].v_type != VAR_نمبر)
	emsg(_(e_invalid_دلیل));
    ورنہ
    {
	alloc_fail_id = argvars[0].vval.v_نمبر;
	اگر (alloc_fail_id >= aid_آخری)
	    emsg(_(e_invalid_دلیل));
	alloc_fail_countdown = argvars[1].vval.v_نمبر;
	alloc_fail_repeat = argvars[2].vval.v_نمبر;
	did_outofmem_پیغام = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    خالی
f_test_autochdir(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#اگر_ختم
}

/*
 * "test_feedinput()"
 */
    خالی
f_test_feedinput(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
#اگر_تعریف_شدہ USE_داخلہ_BUF
    char_u	*val;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    اگر (!gui.اندر_use)
	واپس;
# endif

    اگر (val != تہی)
    {
	trash_داخلہ_buf();
	جوڑو_مقصد_داخلہ_buf_csi(val, (صحیح)STRLEN(val));
    }
#اگر_ختم
}

/*
 * "test_getvalue({name})" function
 */
    خالی
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    name = tv_get_string(&argvars[0]);

    اگر (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_نمبر = need_fileinfo;
    ورنہ
	semsg(_(e_invalid_دلیل_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    خالی
f_test_option_not_سیٹ(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u *name = (char_u *)"";

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (reset_option_was_سیٹ(name) == FAIL)
	semsg(_(e_invalid_دلیل_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    خالی
f_test_override(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u *name = (char_u *)"";
    صحیح     val;
    ساکن صحیح save_starting = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL)
	واپس;

    name = tv_get_string(&argvars[0]);
    val = (صحیح)tv_get_نمبر(&argvars[1]);

    اگر (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"starting") == 0)
    {
	اگر (val)
	{
	    اگر (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ورنہ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ورنہ اگر (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ورنہ اگر (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_رمز_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"term_props") == 0)
	reset_رکن_props_on_termresponse = val;
    ورنہ اگر (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ورنہ اگر (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ورنہ اگر (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_سطریں = val;
    ورنہ اگر (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ورنہ اگر (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ورنہ اگر (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_رکن_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	اگر (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ورنہ
	semsg(_(e_invalid_دلیل_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    خالی
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    صحیح retval = -1;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_نمبر:
	حالت VAR_BOOL:
	حالت VAR_FLOAT:
	حالت VAR_SPECIAL:
	حالت VAR_STRING:
	حالت VAR_INSTR:
	    توڑو;

	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (argvars[0].vval.v_کام != تہی)
		retval = argvars[0].vval.v_کام->jv_refcount - 1;
#اگر_ختم
	    توڑو;
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (argvars[0].vval.v_channel != تہی)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#اگر_ختم
	    توڑو;
	حالت VAR_FUNC:
	    اگر (argvars[0].vval.v_string != تہی)
	    {
		ufunc_T *fp;

		fp = ڈھونڈو_func(argvars[0].vval.v_string, FALSE);
		اگر (fp != تہی)
		    retval = fp->uf_refcount;
	    }
	    توڑو;
	حالت VAR_PARTIAL:
	    اگر (argvars[0].vval.v_partial != تہی)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    توڑو;
	حالت VAR_BLOB:
	    اگر (argvars[0].vval.v_blob != تہی)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    توڑو;
	حالت VAR_LIST:
	    اگر (argvars[0].vval.v_list != تہی)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    توڑو;
	حالت VAR_TUPLE:
	    اگر (argvars[0].vval.v_tuple != تہی)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    توڑو;
	حالت VAR_DICT:
	    اگر (argvars[0].vval.v_dict != تہی)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    توڑو;
	حالت VAR_CLASS:
	    اگر (argvars[0].vval.v_class != تہی)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    توڑو;
	حالت VAR_OBJECT:
	    اگر (argvars[0].vval.v_object != تہی)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    توڑو;
	حالت VAR_TYPEALIAS:
	    اگر (argvars[0].vval.v_typealias != تہی)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    توڑو;
    }

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    خالی
f_test_garbagecollect_now(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    اگر (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_سیٹ));
    ورنہ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    خالی
f_test_garbagecollect_soon(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    خالی
f_test_ignore_error(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    خالی
f_test_null_blob(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = تہی;
}

#اگر defined(FEAT_کام_CHANNEL)
    خالی
f_test_null_channel(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = تہی;
}
#اگر_ختم

    خالی
f_test_null_dict(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv_dict_سیٹ(rettv, تہی);
}

#اگر defined(FEAT_کام_CHANNEL)
    خالی
f_test_null_کام(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_کام;
    rettv->vval.v_کام = تہی;
}
#اگر_ختم

    خالی
f_test_null_list(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv_list_سیٹ(rettv, تہی);
}

    خالی
f_test_null_function(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = تہی;
}

    خالی
f_test_null_partial(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = تہی;
}

    خالی
f_test_null_string(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
}

    خالی
f_test_null_tuple(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv_tuple_سیٹ(rettv, تہی);
}

    خالی
f_test_unknown(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    خالی
f_test_void(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    خالی
f_test_setmouse(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_نمبر || argvars[1].v_type != VAR_نمبر)
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    mouse_صف = (time_t)tv_get_نمبر(&argvars[0]) - 1;
    mouse_ستون = (time_t)tv_get_نمبر(&argvars[1]) - 1;
}

#اگر_تعریف_شدہ FEAT_GUI
    ساکن صحیح
test_gui_drop_files(dict_T *دلائل غیر_مستعمل)
{
# اگر defined(HAVE_DROP_FILE)
    صحیح		صف;
    صحیح		ستون;
    int_u	mods;
    char_u	**fnames;
    صحیح		گنتی = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    اگر (!dict_has_کلید(دلائل, "files")
	    || !dict_has_کلید(دلائل, "row")
	    || !dict_has_کلید(دلائل, "col")
	    || !dict_has_کلید(دلائل, "modifiers"))
	واپس FALSE;

    (خالی)dict_get_tv(دلائل, "files", &t);
    صف = (صحیح)dict_get_نمبر(دلائل, "row");
    ستون = (صحیح)dict_get_نمبر(دلائل, "col");
    mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");

    اگر (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	واپس FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    اگر (fnames == تہی)
	واپس FALSE;

    FOR_ALL_LIST_اشیاء(l, li)
    {
	// ignore non-string items
	اگر (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == تہی)
	    جاری;

	fnames[گنتی] = vim_strsave(li->li_tv.vval.v_string);
	اگر (fnames[گنتی] == تہی)
	{
	    جب (--گنتی >= 0)
		vim_free(fnames[گنتی]);
	    vim_free(fnames);
	    واپس FALSE;
	}
	گنتی++;
    }

    اگر (گنتی > 0)
	gui_handle_drop(متن_X(ستون - 1), متن_Y(صف - 1), mods, fnames, گنتی);
    ورنہ
	vim_free(fnames);
# endif

    واپس TRUE;
}

# اگر defined(ڈھونڈو_REPLACE_DIALOG)
    ساکن صحیح
test_gui_ڈھونڈو_repl(dict_T *دلائل)
{
    صحیح		flags;
    char_u	*ڈھونڈو_متن;
    char_u	*repl_متن;
    صحیح		forward;
    صحیح		retval;

    اگر (!dict_has_کلید(دلائل, "find_text")
	    || !dict_has_کلید(دلائل, "repl_text")
	    || !dict_has_کلید(دلائل, "flags")
	    || !dict_has_کلید(دلائل, "forward"))
	واپس FALSE;

    ڈھونڈو_متن = dict_get_string(دلائل, "find_text", TRUE);
    repl_متن = dict_get_string(دلائل, "repl_text", TRUE);
    flags = (صحیح)dict_get_نمبر(دلائل, "flags");
    forward = (صحیح)dict_get_نمبر(دلائل, "forward");

    retval = gui_do_findrepl(flags, ڈھونڈو_متن, repl_متن, forward);
    vim_free(ڈھونڈو_متن);
    vim_free(repl_متن);

    واپس retval;
}
# endif

    ساکن صحیح
test_gui_mouse_event(dict_T *دلائل)
{
    صحیح		button;
    صحیح		صف;
    صحیح		ستون;
    صحیح		repeated_click;
    int_u	mods;
    صحیح		move;

    اگر (!dict_has_کلید(دلائل, "row")
	    || !dict_has_کلید(دلائل, "col"))
	واپس FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحیح)dict_get_bool(دلائل, "move", FALSE);

    اگر (!move && (!dict_has_کلید(دلائل, "button")
	    || !dict_has_کلید(دلائل, "multiclick")
	    || !dict_has_کلید(دلائل, "modifiers")))
	واپس FALSE;

    صف = (صحیح)dict_get_نمبر(دلائل, "row");
    ستون = (صحیح)dict_get_نمبر(دلائل, "col");

    اگر (move)
    {
	صحیح pY = صف;
	صحیح pX = ستون;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	اگر (dict_get_bool(دلائل, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (صف - 1) * gui.char_اونچائی + gui.char_اونچائی / 2;
	    pX = (ستون - 1) * gui.char_چوڑائی + gui.char_چوڑائی / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ورنہ
    {
	button = (صحیح)dict_get_نمبر(دلائل, "button");
	repeated_click = (صحیح)dict_get_نمبر(دلائل, "multiclick");
	mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سیٹ_hor_scroll_قدم(6);
	mouse_سیٹ_vert_scroll_قدم(3);

	gui_send_mouse_event(button, متن_X(ستون - 1), متن_Y(صف - 1),
							repeated_click, mods);
    }

    واپس TRUE;
}

    ساکن صحیح
test_gui_scrollbar(dict_T *دلائل)
{
    char_u	*which;
    طویل	قدر;
    صحیح		dragging;
    scrollbar_T *sb = تہی;

    اگر (!dict_has_کلید(دلائل, "which")
	    || !dict_has_کلید(دلائل, "value")
	    || !dict_has_کلید(دلائل, "dragging"))
	واپس FALSE;

    which = dict_get_string(دلائل, "which", FALSE);
    قدر = (طویل)dict_get_نمبر(دلائل, "value");
    dragging = (صحیح)dict_get_نمبر(دلائل, "dragging");

    اگر (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_بایاں];
    ورنہ اگر (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_دایاں];
    ورنہ اگر (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    اگر (sb == تہی)
    {
	semsg(_(e_invalid_دلیل_str), which);
	واپس FALSE;
    }
    gui_drag_scrollbar(sb, قدر, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_معمول(FALSE, TRUE, FALSE);
# endif

    واپس TRUE;
}

    ساکن صحیح
test_gui_tabline_event(dict_T *دلائل غیر_مستعمل)
{
# ifdef FEAT_GUI_TABLINE
    صحیح		tabnr;

    اگر (!dict_has_کلید(دلائل, "tabnr"))
	واپس FALSE;

    tabnr = (صحیح)dict_get_نمبر(دلائل, "tabnr");

    واپس send_tabline_event(tabnr);
# ورنہ
    واپس FALSE;
# endif
}

    ساکن صحیح
test_gui_tabmenu_event(dict_T *دلائل غیر_مستعمل)
{
# ifdef FEAT_GUI_TABLINE
    صحیح	tabnr;
    صحیح	شے;

    اگر (!dict_has_کلید(دلائل, "tabnr")
	    || !dict_has_کلید(دلائل, "item"))
	واپس FALSE;

    tabnr = (صحیح)dict_get_نمبر(دلائل, "tabnr");
    شے = (صحیح)dict_get_نمبر(دلائل, "item");

    send_tabline_menu_event(tabnr, شے);
# endif
    واپس TRUE;
}
#اگر_ختم

    خالی
f_test_mswin_event(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر_تعریف_شدہ MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = FALSE;

    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندر_sandbox));
	واپس;
    }

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == تہی)
	واپس;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_نمبر = test_mswin_event(event, argvars[1].vval.v_dict);

#اگر_ختم
}

    خالی
f_test_gui_event(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = FALSE;

    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندر_sandbox));
	واپس;
    }

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == تہی)
	واپس;

    event = tv_get_string(&argvars[0]);
    اگر (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_نمبر = test_gui_drop_files(argvars[1].vval.v_dict);
# اگر defined(ڈھونڈو_REPLACE_DIALOG)
    ورنہ اگر (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_نمبر = test_gui_ڈھونڈو_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ورنہ اگر (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_نمبر = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ورنہ اگر (STRCMP(event, "mouse") == 0)
	rettv->vval.v_نمبر = test_gui_mouse_event(argvars[1].vval.v_dict);
    ورنہ اگر (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_نمبر = test_gui_scrollbar(argvars[1].vval.v_dict);
    ورنہ اگر (STRCMP(event, "tabline") == 0)
	rettv->vval.v_نمبر = test_gui_tabline_event(argvars[1].vval.v_dict);
    ورنہ اگر (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_نمبر = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str), event);
	واپس;
    }
#اگر_ختم
}

    خالی
f_test_settime(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    وقت_for_testing = (time_t)tv_get_نمبر(&argvars[0]);
}

#اگر_ختم // defined(FEAT_EVAL)
