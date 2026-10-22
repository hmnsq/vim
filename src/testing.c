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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ਜੇ (sname != ਨਿਰਾ)
    {
	ga_concat(gap, sname);
	ਜੇ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ਜੇ (SOURCING_LNUM > 0)
    {
	ਅੱਖਰ    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, ਆਕਾਰ(buf), "line %ld", (ਲੰਮਾ)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ਜੇ (sname != ਨਿਰਾ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ਸਥਿਰ ਖਾਲੀ
ga_concat_esc(garray_T *gap, char_u *p, ਪੂਰਨ clen)
{
    char_u  buf[NUMBUFLEN];

    ਜੇ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ਵਾਪਸ;
    }

    ਚੋਣ (*p)
    {
	ਹਾਲਤ BS: GA_CONCAT_LITERAL(gap, "\\b"); ਰੋਕੋ;
	ਹਾਲਤ ESC: GA_CONCAT_LITERAL(gap, "\\e"); ਰੋਕੋ;
	ਹਾਲਤ FF: GA_CONCAT_LITERAL(gap, "\\f"); ਰੋਕੋ;
	ਹਾਲਤ NL: GA_CONCAT_LITERAL(gap, "\\n"); ਰੋਕੋ;
	ਹਾਲਤ TAB: GA_CONCAT_LITERAL(gap, "\\t"); ਰੋਕੋ;
	ਹਾਲਤ CAR: GA_CONCAT_LITERAL(gap, "\\r"); ਰੋਕੋ;
	ਹਾਲਤ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ਰੋਕੋ;
	ਮੂਲ:
	   ਜੇ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, ਆਕਾਰ(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ਨਹੀਂਤਾਂ
	       ga_append(gap, *p);
	   ਰੋਕੋ;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ਸਥਿਰ ਖਾਲੀ
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    ਪੂਰਨ	    c;
    ਪੂਰਨ	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    ਪੂਰਨ	    same_len;

    ਜੇ (str == ਨਿਰਾ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ਵਾਪਸ;
    }

    ਲਈ (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ਜਦੋਂ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ਜੇ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, ਆਕਾਰ(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	ਨਹੀਂਤਾਂ
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ਸਥਿਰ ਖਾਲੀ
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_ਸੁਨੇਹਾ_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    ਪੂਰਨ		did_copy = FALSE;
    ਪੂਰਨ		omitted = 0;

    ਜੇ (opt_ਸੁਨੇਹਾ_tv->v_type != VAR_UNKNOWN
	    && !(opt_ਸੁਨੇਹਾ_tv->v_type == VAR_STRING
		&& (opt_ਸੁਨੇਹਾ_tv->vval.v_string == ਨਿਰਾ
		    || *opt_ਸੁਨੇਹਾ_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_ਸੁਨੇਹਾ_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ਜੇ (atype == ASSERT_ਮੇਲ || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ਨਹੀਂਤਾਂ ਜੇ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ਨਹੀਂਤਾਂ
	GA_CONCAT_LITERAL(gap, "Expected ");
    ਜੇ (exp_str == ਨਿਰਾ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ਜੇ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ਨਿਰਾ && got_tv->vval.v_dict != ਨਿਰਾ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    ਪੂਰਨ		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ਜੇ (exp_tv->vval.v_dict == ਨਿਰਾ || got_tv->vval.v_dict == ਨਿਰਾ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ਨਿਰਾ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ਨਿਰਾ;
		ਵਾਪਸ;
	    }

	    todo = (ਪੂਰਨ)exp_d->dv_hashtab.ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&exp_d->dv_hashtab, hi, todo)
	    {
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    item2 = dict_ਲੱਭੋ(got_d, hi->hi_ਕੁੰਜੀ, -1);
		    ਜੇ (item2 == ਨਿਰਾ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_ਜੋੜੋ_tv(exp_tv->vval.v_dict,
					(ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ, &HI2DI(hi)->di_tv);
			ਜੇ (item2 != ਨਿਰਾ)
			    dict_ਜੋੜੋ_tv(got_tv->vval.v_dict,
					    (ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ, &item2->di_tv);
		    }
		    ਨਹੀਂਤਾਂ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (ਪੂਰਨ)got_d->dv_hashtab.ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&got_d->dv_hashtab, hi, todo)
	    {
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    item2 = dict_ਲੱਭੋ(exp_d, hi->hi_ਕੁੰਜੀ, -1);
		    ਜੇ (item2 == ਨਿਰਾ)
			// item of got_d not present in exp_d
			dict_ਜੋੜੋ_tv(got_tv->vval.v_dict,
					(ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ਜੇ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ਜੇ (atype != ASSERT_NOTEQUAL)
    {
	ਜੇ (atype == ASSERT_ਮੇਲ)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ਨਹੀਂਤਾਂ ਜੇ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ਨਹੀਂਤਾਂ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ਜੇ (omitted != 0)
	{
	    ਅੱਖਰ    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, ਆਕਾਰ(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ਜੇ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ਸਥਿਰ ਪੂਰਨ
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ਜੇ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ਨਿਰਾ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
assert_ਮੇਲ_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *ਪਾਠ = tv_get_string_buf_chk(&argvars[1], buf2);
    ਜੇ (pat != ਨਿਰਾ && ਪਾਠ != ਨਿਰਾ
		 && ਨਮੂਨਾ_ਮੇਲ(pat, ਪਾਠ, FALSE) != (atype == ASSERT_ਮੇਲ))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ਨਿਰਾ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
assert_bool(typval_T *argvars, ਪੂਰਨ isTrue)
{
    ਪੂਰਨ		error = FALSE;
    garray_T	ga;

    ਜੇ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_ਸੰਖਿਆ == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ਵਾਪਸ 0;
    ਜੇ (argvars[0].v_type != VAR_ਸੰਖਿਆ
	    || (tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ਨਿਰਾ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਖਾਲੀ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ਨਹੀਂਤਾਂ
	ga_concat(gap, cmd);
}

    ਸਥਿਰ ਪੂਰਨ
assert_beeps(typval_T *argvars, ਪੂਰਨ no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    ਪੂਰਨ		ret = 0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ਜੇ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ਜੇ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	ਨਹੀਂਤਾਂ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ਵਾਪਸ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    ਖਾਲੀ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    ਖਾਲੀ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ਸਥਿਰ ਪੂਰਨ
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    ਅੱਖਰ	line1[200];
    ਅੱਖਰ	line2[200];
    ਪੂਰਨ		lineidx = 0;
    size_t	IObufflen;

    ਜੇ (fname1 == ਨਿਰਾ || fname2 == ਨਿਰਾ)
	ਵਾਪਸ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((ਅੱਖਰ *)fname1, READBIN);
    ਜੇ (fd1 == ਨਿਰਾ)
    {
	IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
	    (ਅੱਖਰ *)e_cant_read_file_str, fname1);
    }
    ਨਹੀਂਤਾਂ
    {
	fd2 = mch_fopen((ਅੱਖਰ *)fname2, READBIN);
	ਜੇ (fd2 == ਨਿਰਾ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
		(ਅੱਖਰ *)e_cant_read_file_str, fname2);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ	    c1, c2;
	    ਲੰਮਾ    ਗਿਣਤੀ_2 = 0;
	    ਲੰਮਾ    linecount = 1;

	    ਲਈ (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ਜੇ (c1 == EOF)
		{
		    ਜੇ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ਜੇ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ਗਿਣਤੀ_2, linecount);
			ਰੋਕੋ;
		    }
		}
		++ਗਿਣਤੀ_2;
		ਜੇ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ਨਹੀਂਤਾਂ ਜੇ (lineidx + 2 == (ਪੂਰਨ)ਆਕਾਰ(line1))
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

    ਜੇ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ਜੇ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ਜੇ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ਵਾਪਸ 1;
    }

    ਵਾਪਸ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    ਖਾਲੀ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    ਖਾਲੀ
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    error = tv_get_string_chk(&argvars[0]);
    ਜੇ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ਸੰਖਿਆ = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (error != ਨਿਰਾ
	&& ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)get_vim_var_str(VV_EXCEPTION), (ਅੱਖਰ *)error) == ਨਿਰਾ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ਨਿਰਾ, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ਸੰਖਿਆ = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    ਖਾਲੀ
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    ਪੂਰਨ		save_trylevel = trylevel;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    ਅੱਖਰ	*wrong_arg_ਸੁਨੇਹਾ = ਨਿਰਾ;
    char_u	*tofree = ਨਿਰਾ;

    ਜੇ (check_for_string_or_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ਵਾਪਸ;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    ਵਿੱਚ_assert_fails = TRUE;
    ++no_wait_return;

    char_u *cmd = tv_get_string_chk(&argvars[0]);
    do_cmdline_cmd(cmd);

    // reset here for any errors reported below
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;

    ਜੇ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ਸੰਖਿਆ = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ਨਿਰਾ;
	ਪੂਰਨ	error_ਮਿਲਿਆ = FALSE;
	ਪੂਰਨ	error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ = 1;
	char_u	*actual = emsg_assert_fails_ਸੁਨੇਹਾ == ਨਿਰਾ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_ਸੁਨੇਹਾ;

	ਜੇ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_ਮਿਲਿਆ = expected == ਨਿਰਾ
			   || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)actual, (ਅੱਖਰ *)expected) == ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ਜੇ (list == ਨਿਰਾ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_ਸੁਨੇਹਾ = e_assert_fails_ਦੂਜਾ_arg;
		ਜਾਓ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_ਪਹਿਲਾ->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ਜੇ (expected == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਜੇ (!ਨਮੂਨਾ_ਮੇਲ(expected, actual, FALSE))
	    {
		error_ਮਿਲਿਆ = TRUE;
		expected_str = expected;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ਜੇ (actual != ਨਿਰਾ)
		{
		    tv = &list->lv_u.mat.lv_ਆਖਰੀ->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ਜੇ (expected == ਨਿਰਾ)
			ਜਾਓ theend;
		    ਜੇ (!ਨਮੂਨਾ_ਮੇਲ(expected, actual, FALSE))
		    {
			error_ਮਿਲਿਆ = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    wrong_arg_ਸੁਨੇਹਾ = e_assert_fails_ਦੂਜਾ_arg;
	    ਜਾਓ theend;
	}

	ਜੇ (!error_ਮਿਲਿਆ && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ਜੇ (argvars[3].v_type != VAR_ਸੰਖਿਆ)
	    {
		wrong_arg_ਸੁਨੇਹਾ = e_assert_fails_fourth_ਦਲੀਲ;
		ਜਾਓ theend;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (argvars[3].vval.v_ਸੰਖਿਆ >= 0
			 && argvars[3].vval.v_ਸੰਖਿਆ != emsg_assert_fails_lnum)
	    {
		error_ਮਿਲਿਆ = TRUE;
		error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ = 3;
	    }
	    ਜੇ (!error_ਮਿਲਿਆ && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ਜੇ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_ਸੁਨੇਹਾ = e_assert_fails_fifth_ਦਲੀਲ;
		    ਜਾਓ theend;
		}
		ਨਹੀਂਤਾਂ ਜੇ (argvars[4].vval.v_string != ਨਿਰਾ
		    && !ਨਮੂਨਾ_ਮੇਲ(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_ਮਿਲਿਆ = TRUE;
		    error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ = 4;
		}
	    }
	}

	ਜੇ (error_ਮਿਲਿਆ)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ਜੇ (error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ == 3)
	    {
		actual_tv.v_type = VAR_ਸੰਖਿਆ;
		actual_tv.vval.v_ਸੰਖਿਆ = emsg_assert_fails_lnum;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_ਮਿਲਿਆ_ਸੂਚਕਾਂਕ], &actual_tv, ASSERT_FAILS);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    assert_append_cmd_or_arg(&ga, argvars, cmd);
	    assert_error(&ga);
	    ga_clear(&ga);
	    rettv->vval.v_ਸੰਖਿਆ = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    ਵਿੱਚ_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    ਸੁਨੇਹਾ_scrolled = 0;
    ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ;
    VIM_CLEAR(emsg_assert_fails_ਸੁਨੇਹਾ);
    vim_free(tofree);
    ਸੈੱਟ_vim_var_string(VV_ERRMSG, ਨਿਰਾ, 0);
    ਜੇ (wrong_arg_ਸੁਨੇਹਾ != ਨਿਰਾ)
	emsg(_(wrong_arg_ਸੁਨੇਹਾ));
}

/*
 * "assert_false(actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_bool(argvars, FALSE);
}

    ਸਥਿਰ ਪੂਰਨ
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    ਪੂਰਨ		error = FALSE;
    char_u	expected_str[200];

    ਜੇ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ਜੇ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ਅੱਖਰ *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ਨਿਰਾ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ਵਾਪਸ 1;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	varnumber_T	ਹੇਠਲਾ = tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error);
	varnumber_T	ਉੱਤਲਾ_2 = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);

	ਜੇ (error)
	    ਵਾਪਸ 0;
	ਜੇ (actual < ਹੇਠਲਾ || actual > ਉੱਤਲਾ_2)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ਅੱਖਰ *)expected_str, 200, "range %ld - %ld,",
						     (ਲੰਮਾ)ਹੇਠਲਾ, (ਲੰਮਾ)ਉੱਤਲਾ_2);
	    fill_assert_error(&ga, &argvars[3], expected_str, ਨਿਰਾ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ਵਾਪਸ 1;
	}
    }
    ਵਾਪਸ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    ਖਾਲੀ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_ਮੇਲ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_ਮੇਲ_common(argvars, ASSERT_ਮੇਲ);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_ਮੇਲ_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    ਖਾਲੀ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_ਸੰਖਿਆ = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    ਖਾਲੀ
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    ਖਾਲੀ
f_test_alloc_fail(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_ਸੰਖਿਆ
	    || argvars[0].vval.v_ਸੰਖਿਆ <= 0
	    || argvars[1].v_type != VAR_ਸੰਖਿਆ
	    || argvars[1].vval.v_ਸੰਖਿਆ < 0
	    || argvars[2].v_type != VAR_ਸੰਖਿਆ)
	emsg(_(e_invalid_ਦਲੀਲ));
    ਨਹੀਂਤਾਂ
    {
	alloc_fail_id = argvars[0].vval.v_ਸੰਖਿਆ;
	ਜੇ (alloc_fail_id >= aid_ਆਖਰੀ)
	    emsg(_(e_invalid_ਦਲੀਲ));
	alloc_fail_countdown = argvars[1].vval.v_ਸੰਖਿਆ;
	alloc_fail_repeat = argvars[2].vval.v_ਸੰਖਿਆ;
	did_outofmem_ਸੁਨੇਹਾ = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    ਖਾਲੀ
f_test_autochdir(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#ਜੇ_ਅੰਤ
}

/*
 * "test_feedinput()"
 */
    ਖਾਲੀ
f_test_feedinput(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਨਿਵੇਸ਼_BUF
    char_u	*val;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif

    ਜੇ (val != ਨਿਰਾ)
    {
	trash_ਨਿਵੇਸ਼_buf();
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(val, (ਪੂਰਨ)STRLEN(val));
    }
#ਜੇ_ਅੰਤ
}

/*
 * "test_getvalue({name})" function
 */
    ਖਾਲੀ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);

    ਜੇ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_ਸੰਖਿਆ = need_fileinfo;
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    ਖਾਲੀ
f_test_option_not_ਸੈੱਟ(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u *name = (char_u *)"";

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (reset_option_was_ਸੈੱਟ(name) == FAIL)
	semsg(_(e_invalid_ਦਲੀਲ_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    ਖਾਲੀ
f_test_override(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u *name = (char_u *)"";
    ਪੂਰਨ     val;
    ਸਥਿਰ ਪੂਰਨ save_starting = -1;

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    val = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]);

    ਜੇ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"starting") == 0)
    {
	ਜੇ (val)
	{
	    ਜੇ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_ਕੋਡ_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_ਪਦ_props_on_termresponse = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_ਸਤਰਾਂ = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_ਪਦ_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	ਜੇ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    ਖਾਲੀ
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ retval = -1;

    ਚੋਣ (argvars[0].v_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_INSTR:
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (argvars[0].vval.v_ਕੰਮ != ਨਿਰਾ)
		retval = argvars[0].vval.v_ਕੰਮ->jv_refcount - 1;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (argvars[0].vval.v_channel != ਨਿਰਾ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FUNC:
	    ਜੇ (argvars[0].vval.v_string != ਨਿਰਾ)
	    {
		ufunc_T *fp;

		fp = ਲੱਭੋ_func(argvars[0].vval.v_string, FALSE);
		ਜੇ (fp != ਨਿਰਾ)
		    retval = fp->uf_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (argvars[0].vval.v_partial != ਨਿਰਾ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    ਜੇ (argvars[0].vval.v_blob != ਨਿਰਾ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਜੇ (argvars[0].vval.v_list != ਨਿਰਾ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (argvars[0].vval.v_tuple != ਨਿਰਾ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ (argvars[0].vval.v_dict != ਨਿਰਾ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	    ਜੇ (argvars[0].vval.v_class != ਨਿਰਾ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    ਜੇ (argvars[0].vval.v_object != ਨਿਰਾ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਜੇ (argvars[0].vval.v_typealias != ਨਿਰਾ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ਰੋਕੋ;
    }

    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    ਖਾਲੀ
f_test_garbagecollect_now(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ਜੇ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_ਸੈੱਟ));
    ਨਹੀਂਤਾਂ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    ਖਾਲੀ
f_test_garbagecollect_soon(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    ਖਾਲੀ
f_test_ignore_error(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    ਖਾਲੀ
f_test_null_blob(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ਨਿਰਾ;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
f_test_null_channel(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
f_test_null_dict(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv_dict_ਸੈੱਟ(rettv, ਨਿਰਾ);
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
f_test_null_ਕੰਮ(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_ਕੰਮ;
    rettv->vval.v_ਕੰਮ = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
f_test_null_list(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);
}

    ਖਾਲੀ
f_test_null_function(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ਨਿਰਾ;
}

    ਖਾਲੀ
f_test_null_partial(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ਨਿਰਾ;
}

    ਖਾਲੀ
f_test_null_string(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
}

    ਖਾਲੀ
f_test_null_tuple(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv_tuple_ਸੈੱਟ(rettv, ਨਿਰਾ);
}

    ਖਾਲੀ
f_test_unknown(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    ਖਾਲੀ
f_test_void(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    ਖਾਲੀ
f_test_setmouse(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_ਸੰਖਿਆ || argvars[1].v_type != VAR_ਸੰਖਿਆ)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    mouse_ਪੰਕਤੀ = (time_t)tv_get_ਸੰਖਿਆ(&argvars[0]) - 1;
    mouse_ਖਾਨਾ = (time_t)tv_get_ਸੰਖਿਆ(&argvars[1]) - 1;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਸਥਿਰ ਪੂਰਨ
test_gui_drop_files(dict_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
# ਜੇ defined(HAVE_DROP_FILE)
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    int_u	mods;
    char_u	**fnames;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "files")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "row")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "col")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "modifiers"))
	ਵਾਪਸ FALSE;

    (ਖਾਲੀ)dict_get_tv(ਦਲੀਲਾਂ, "files", &t);
    ਪੰਕਤੀ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "row");
    ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "col");
    mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");

    ਜੇ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ਵਾਪਸ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ਜੇ (fnames == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	// ignore non-string items
	ਜੇ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ਨਿਰਾ)
	    ਜਾਰੀ;

	fnames[ਗਿਣਤੀ_2] = vim_strsave(li->li_tv.vval.v_string);
	ਜੇ (fnames[ਗਿਣਤੀ_2] == ਨਿਰਾ)
	{
	    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
		vim_free(fnames[ਗਿਣਤੀ_2]);
	    vim_free(fnames);
	    ਵਾਪਸ FALSE;
	}
	ਗਿਣਤੀ_2++;
    }

    ਜੇ (ਗਿਣਤੀ_2 > 0)
	gui_handle_drop(ਪਾਠ_X(ਖਾਨਾ - 1), ਪਾਠ_Y(ਪੰਕਤੀ - 1), mods, fnames, ਗਿਣਤੀ_2);
    ਨਹੀਂਤਾਂ
	vim_free(fnames);
# endif

    ਵਾਪਸ TRUE;
}

# ਜੇ defined(ਲੱਭੋ_REPLACE_DIALOG)
    ਸਥਿਰ ਪੂਰਨ
test_gui_ਲੱਭੋ_repl(dict_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ		flags;
    char_u	*ਲੱਭੋ_ਪਾਠ;
    char_u	*repl_ਪਾਠ;
    ਪੂਰਨ		forward;
    ਪੂਰਨ		retval;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "find_text")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "repl_text")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "flags")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "forward"))
	ਵਾਪਸ FALSE;

    ਲੱਭੋ_ਪਾਠ = dict_get_string(ਦਲੀਲਾਂ, "find_text", TRUE);
    repl_ਪਾਠ = dict_get_string(ਦਲੀਲਾਂ, "repl_text", TRUE);
    flags = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "flags");
    forward = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "forward");

    retval = gui_do_findrepl(flags, ਲੱਭੋ_ਪਾਠ, repl_ਪਾਠ, forward);
    vim_free(ਲੱਭੋ_ਪਾਠ);
    vim_free(repl_ਪਾਠ);

    ਵਾਪਸ retval;
}
# endif

    ਸਥਿਰ ਪੂਰਨ
test_gui_mouse_event(dict_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ		button;
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		repeated_click;
    int_u	mods;
    ਪੂਰਨ		move;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "row")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "col"))
	ਵਾਪਸ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ਪੂਰਨ)dict_get_bool(ਦਲੀਲਾਂ, "move", FALSE);

    ਜੇ (!move && (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "button")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "multiclick")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "modifiers")))
	ਵਾਪਸ FALSE;

    ਪੰਕਤੀ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "row");
    ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "col");

    ਜੇ (move)
    {
	ਪੂਰਨ pY = ਪੰਕਤੀ;
	ਪੂਰਨ pX = ਖਾਨਾ;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ਜੇ (dict_get_bool(ਦਲੀਲਾਂ, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (ਪੰਕਤੀ - 1) * gui.char_ਉਚਾਈ + gui.char_ਉਚਾਈ / 2;
	    pX = (ਖਾਨਾ - 1) * gui.char_ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ਨਹੀਂਤਾਂ
    {
	button = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "button");
	repeated_click = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "multiclick");
	mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(6);
	mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(3);

	gui_send_mouse_event(button, ਪਾਠ_X(ਖਾਨਾ - 1), ਪਾਠ_Y(ਪੰਕਤੀ - 1),
							repeated_click, mods);
    }

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
test_gui_scrollbar(dict_T *ਦਲੀਲਾਂ)
{
    char_u	*which;
    ਲੰਮਾ	ਮੁੱਲ;
    ਪੂਰਨ		dragging;
    scrollbar_T *sb = ਨਿਰਾ;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "which")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "value")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "dragging"))
	ਵਾਪਸ FALSE;

    which = dict_get_string(ਦਲੀਲਾਂ, "which", FALSE);
    ਮੁੱਲ = (ਲੰਮਾ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "value");
    dragging = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "dragging");

    ਜੇ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ਖੱਬਾ];
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ਸੱਜਾ];
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ਜੇ (sb == ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), which);
	ਵਾਪਸ FALSE;
    }
    gui_drag_scrollbar(sb, ਮੁੱਲ, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_ਸਾਧਾਰਨ(FALSE, TRUE, FALSE);
# endif

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
test_gui_tabline_event(dict_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_GUI_TABLINE
    ਪੂਰਨ		tabnr;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "tabnr"))
	ਵਾਪਸ FALSE;

    tabnr = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "tabnr");

    ਵਾਪਸ send_tabline_event(tabnr);
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
# endif
}

    ਸਥਿਰ ਪੂਰਨ
test_gui_tabmenu_event(dict_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_GUI_TABLINE
    ਪੂਰਨ	tabnr;
    ਪੂਰਨ	ਵਸਤੂ;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "tabnr")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "item"))
	ਵਾਪਸ FALSE;

    tabnr = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "tabnr");
    ਵਸਤੂ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "item");

    send_tabline_menu_event(tabnr, ਵਸਤੂ);
# endif
    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
f_test_mswin_event(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = FALSE;

    ਜੇ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	ਵਾਪਸ;
    }

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_ਸੰਖਿਆ = test_mswin_event(event, argvars[1].vval.v_dict);

#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
f_test_gui_event(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = FALSE;

    ਜੇ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	ਵਾਪਸ;
    }

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ਨਿਰਾ)
	ਵਾਪਸ;

    event = tv_get_string(&argvars[0]);
    ਜੇ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_drop_files(argvars[1].vval.v_dict);
# ਜੇ defined(ਲੱਭੋ_REPLACE_DIALOG)
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_ਲੱਭੋ_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_mouse_event(argvars[1].vval.v_dict);
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_scrollbar(argvars[1].vval.v_dict);
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_tabline_event(argvars[1].vval.v_dict);
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_ਸੰਖਿਆ = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), event);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
f_test_settime(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਸਮਾਂ_for_testing = (time_t)tv_get_ਸੰਖਿਆ(&argvars[0]);
}

#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
