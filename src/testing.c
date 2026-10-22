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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    స్థిర శూన్య
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ఒకవేళ (sname != ఖాళీ)
    {
	ga_concat(gap, sname);
	ఒకవేళ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ఒకవేళ (SOURCING_LNUM > 0)
    {
	అక్షరం    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, పరిమాణం(buf), "line %ld", (దీర్ఘ)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ఒకవేళ (sname != ఖాళీ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    స్థిర శూన్య
ga_concat_esc(garray_T *gap, char_u *p, పూర్ణాంకం clen)
{
    char_u  buf[NUMBUFLEN];

    ఒకవేళ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	తిరిగి;
    }

    ఎంపిక (*p)
    {
	స్థితి BS: GA_CONCAT_LITERAL(gap, "\\b"); ఆపు;
	స్థితి ESC: GA_CONCAT_LITERAL(gap, "\\e"); ఆపు;
	స్థితి FF: GA_CONCAT_LITERAL(gap, "\\f"); ఆపు;
	స్థితి NL: GA_CONCAT_LITERAL(gap, "\\n"); ఆపు;
	స్థితి TAB: GA_CONCAT_LITERAL(gap, "\\t"); ఆపు;
	స్థితి CAR: GA_CONCAT_LITERAL(gap, "\\r"); ఆపు;
	స్థితి '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ఆపు;
	అప్రమేయ:
	   ఒకవేళ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((అక్షరం *)buf, పరిమాణం(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   లేకపోతే
	       ga_append(gap, *p);
	   ఆపు;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    స్థిర శూన్య
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    పూర్ణాంకం	    c;
    పూర్ణాంకం	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    పూర్ణాంకం	    same_len;

    ఒకవేళ (str == ఖాళీ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	తిరిగి;
    }

    కొరకు (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	అయితే (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ఒకవేళ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((అక్షరం *)buf, పరిమాణం(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	లేకపోతే
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    స్థిర శూన్య
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_సందేశం_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    పూర్ణాంకం		did_copy = FALSE;
    పూర్ణాంకం		omitted = 0;

    ఒకవేళ (opt_సందేశం_tv->v_type != VAR_UNKNOWN
	    && !(opt_సందేశం_tv->v_type == VAR_STRING
		&& (opt_సందేశం_tv->vval.v_string == ఖాళీ
		    || *opt_సందేశం_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_సందేశం_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ఒకవేళ (atype == ASSERT_సరిపోలిక || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    లేకపోతే ఒకవేళ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    లేకపోతే
	GA_CONCAT_LITERAL(gap, "Expected ");
    ఒకవేళ (exp_str == ఖాళీ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ఒకవేళ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ఖాళీ && got_tv->vval.v_dict != ఖాళీ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    పూర్ణాంకం		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ఒకవేళ (exp_tv->vval.v_dict == ఖాళీ || got_tv->vval.v_dict == ఖాళీ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ఖాళీ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ఖాళీ;
		తిరిగి;
	    }

	    todo = (పూర్ణాంకం)exp_d->dv_hashtab.ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(&exp_d->dv_hashtab, hi, todo)
	    {
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    item2 = dict_కనుగొను(got_d, hi->hi_కీలకం, -1);
		    ఒకవేళ (item2 == ఖాళీ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_కూడు_tv(exp_tv->vval.v_dict,
					(అక్షరం *)hi->hi_కీలకం, &HI2DI(hi)->di_tv);
			ఒకవేళ (item2 != ఖాళీ)
			    dict_కూడు_tv(got_tv->vval.v_dict,
					    (అక్షరం *)hi->hi_కీలకం, &item2->di_tv);
		    }
		    లేకపోతే
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (పూర్ణాంకం)got_d->dv_hashtab.ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(&got_d->dv_hashtab, hi, todo)
	    {
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    item2 = dict_కనుగొను(exp_d, hi->hi_కీలకం, -1);
		    ఒకవేళ (item2 == ఖాళీ)
			// item of got_d not present in exp_d
			dict_కూడు_tv(got_tv->vval.v_dict,
					(అక్షరం *)hi->hi_కీలకం, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    లేకపోతే
    {
	ఒకవేళ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ఒకవేళ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ఒకవేళ (atype != ASSERT_NOTEQUAL)
    {
	ఒకవేళ (atype == ASSERT_సరిపోలిక)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	లేకపోతే ఒకవేళ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	లేకపోతే
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ఒకవేళ (omitted != 0)
	{
	    అక్షరం    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, పరిమాణం(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ఒకవేళ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    స్థిర పూర్ణాంకం
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ఒకవేళ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ఖాళీ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	తిరిగి 1;
    }
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
assert_సరిపోలిక_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *పాఠ్యం = tv_get_string_buf_chk(&argvars[1], buf2);
    ఒకవేళ (pat != ఖాళీ && పాఠ్యం != ఖాళీ
		 && నమూనా_సరిపోలిక(pat, పాఠ్యం, FALSE) != (atype == ASSERT_సరిపోలిక))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ఖాళీ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	తిరిగి 1;
    }
    తిరిగి 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    స్థిర పూర్ణాంకం
assert_bool(typval_T *argvars, పూర్ణాంకం isTrue)
{
    పూర్ణాంకం		error = FALSE;
    garray_T	ga;

    ఒకవేళ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_సంఖ్య == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	తిరిగి 0;
    ఒకవేళ (argvars[0].v_type != VAR_సంఖ్య
	    || (tv_get_సంఖ్య_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ఖాళీ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	తిరిగి 1;
    }
    తిరిగి 0;
}

    స్థిర శూన్య
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    లేకపోతే
	ga_concat(gap, cmd);
}

    స్థిర పూర్ణాంకం
assert_beeps(typval_T *argvars, పూర్ణాంకం no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    పూర్ణాంకం		ret = 0;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ఒకవేళ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ఒకవేళ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	లేకపోతే
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    తిరిగి ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    శూన్య
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    శూన్య
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    శూన్య
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_equal_common(argvars, ASSERT_EQUAL);
}

    స్థిర పూర్ణాంకం
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    అక్షరం	line1[200];
    అక్షరం	line2[200];
    పూర్ణాంకం		lineidx = 0;
    size_t	IObufflen;

    ఒకవేళ (fname1 == ఖాళీ || fname2 == ఖాళీ)
	తిరిగి 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((అక్షరం *)fname1, READBIN);
    ఒకవేళ (fd1 == ఖాళీ)
    {
	IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE,
	    (అక్షరం *)e_cant_read_file_str, fname1);
    }
    లేకపోతే
    {
	fd2 = mch_fopen((అక్షరం *)fname2, READBIN);
	ఒకవేళ (fd2 == ఖాళీ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE,
		(అక్షరం *)e_cant_read_file_str, fname2);
	}
	లేకపోతే
	{
	    పూర్ణాంకం	    c1, c2;
	    దీర్ఘ    లెక్క = 0;
	    దీర్ఘ    linecount = 1;

	    కొరకు (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ఒకవేళ (c1 == EOF)
		{
		    ఒకవేళ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ఆపు;
		}
		లేకపోతే ఒకవేళ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ఆపు;
		}
		లేకపోతే
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ఒకవేళ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", లెక్క, linecount);
			ఆపు;
		    }
		}
		++లెక్క;
		ఒకవేళ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		లేకపోతే ఒకవేళ (lineidx + 2 == (పూర్ణాంకం)పరిమాణం(line1))
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

    ఒకవేళ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ఒకవేళ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ఒకవేళ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	తిరిగి 1;
    }

    తిరిగి 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    శూన్య
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    rettv->vval.v_సంఖ్య = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    శూన్య
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    శూన్య
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    error = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_సంఖ్య = 1;
    }
    లేకపోతే ఒకవేళ (error != ఖాళీ
	&& అక్షరశ్రేణి_శోధన((అక్షరం *)get_vim_var_str(VV_EXCEPTION), (అక్షరం *)error) == ఖాళీ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ఖాళీ, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_సంఖ్య = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    శూన్య
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    పూర్ణాంకం		save_trylevel = trylevel;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    అక్షరం	*wrong_arg_సందేశం = ఖాళీ;
    char_u	*tofree = ఖాళీ;

    ఒకవేళ (check_for_string_or_సంఖ్య_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	తిరిగి;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    లోపల_assert_fails = TRUE;
    ++no_wait_return;

    char_u *cmd = tv_get_string_chk(&argvars[0]);
    do_cmdline_cmd(cmd);

    // reset here for any errors reported below
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;

    ఒకవేళ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_సంఖ్య = 1;
    }
    లేకపోతే ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ఖాళీ;
	పూర్ణాంకం	error_దొరికింది = FALSE;
	పూర్ణాంకం	error_దొరికింది_సూచీ = 1;
	char_u	*actual = emsg_assert_fails_సందేశం == ఖాళీ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_సందేశం;

	ఒకవేళ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_దొరికింది = expected == ఖాళీ
			   || అక్షరశ్రేణి_శోధన((అక్షరం *)actual, (అక్షరం *)expected) == ఖాళీ;
	}
	లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ఒకవేళ (list == ఖాళీ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_సందేశం = e_assert_fails_రెండవ_arg;
		వెళ్ళు theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_మొదటి->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ఒకవేళ (expected == ఖాళీ)
		వెళ్ళు theend;
	    ఒకవేళ (!నమూనా_సరిపోలిక(expected, actual, FALSE))
	    {
		error_దొరికింది = TRUE;
		expected_str = expected;
	    }
	    లేకపోతే ఒకవేళ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ఒకవేళ (actual != ఖాళీ)
		{
		    tv = &list->lv_u.mat.lv_చివరి->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ఒకవేళ (expected == ఖాళీ)
			వెళ్ళు theend;
		    ఒకవేళ (!నమూనా_సరిపోలిక(expected, actual, FALSE))
		    {
			error_దొరికింది = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	లేకపోతే
	{
	    wrong_arg_సందేశం = e_assert_fails_రెండవ_arg;
	    వెళ్ళు theend;
	}

	ఒకవేళ (!error_దొరికింది && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (argvars[3].v_type != VAR_సంఖ్య)
	    {
		wrong_arg_సందేశం = e_assert_fails_fourth_వాదం;
		వెళ్ళు theend;
	    }
	    లేకపోతే ఒకవేళ (argvars[3].vval.v_సంఖ్య >= 0
			 && argvars[3].vval.v_సంఖ్య != emsg_assert_fails_lnum)
	    {
		error_దొరికింది = TRUE;
		error_దొరికింది_సూచీ = 3;
	    }
	    ఒకవేళ (!error_దొరికింది && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ఒకవేళ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_సందేశం = e_assert_fails_fifth_వాదం;
		    వెళ్ళు theend;
		}
		లేకపోతే ఒకవేళ (argvars[4].vval.v_string != ఖాళీ
		    && !నమూనా_సరిపోలిక(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_దొరికింది = TRUE;
		    error_దొరికింది_సూచీ = 4;
		}
	    }
	}

	ఒకవేళ (error_దొరికింది)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ఒకవేళ (error_దొరికింది_సూచీ == 3)
	    {
		actual_tv.v_type = VAR_సంఖ్య;
		actual_tv.vval.v_సంఖ్య = emsg_assert_fails_lnum;
	    }
	    లేకపోతే ఒకవేళ (error_దొరికింది_సూచీ == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    లేకపోతే
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_దొరికింది_సూచీ], &actual_tv, ASSERT_FAILS);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    assert_append_cmd_or_arg(&ga, argvars, cmd);
	    assert_error(&ga);
	    ga_clear(&ga);
	    rettv->vval.v_సంఖ్య = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    లోపల_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    సందేశం_నిలువు = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    సందేశం_scrolled = 0;
    పంక్తులు_ఎడమ = అడ్డువరుసలు;
    VIM_CLEAR(emsg_assert_fails_సందేశం);
    vim_free(tofree);
    సమితి_vim_var_string(VV_ERRMSG, ఖాళీ, 0);
    ఒకవేళ (wrong_arg_సందేశం != ఖాళీ)
	emsg(_(wrong_arg_సందేశం));
}

/*
 * "assert_false(actual[, msg])" function
 */
    శూన్య
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_bool(argvars, FALSE);
}

    స్థిర పూర్ణాంకం
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    పూర్ణాంకం		error = FALSE;
    char_u	expected_str[200];

    ఒకవేళ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ఒకవేళ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((అక్షరం *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ఖాళీ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    తిరిగి 1;
	}
    }
    లేకపోతే
    {
	varnumber_T	క్రింది = tv_get_సంఖ్య_chk(&argvars[0], &error);
	varnumber_T	పై_2 = tv_get_సంఖ్య_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_సంఖ్య_chk(&argvars[2], &error);

	ఒకవేళ (error)
	    తిరిగి 0;
	ఒకవేళ (actual < క్రింది || actual > పై_2)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((అక్షరం *)expected_str, 200, "range %ld - %ld,",
						     (దీర్ఘ)క్రింది, (దీర్ఘ)పై_2);
	    fill_assert_error(&ga, &argvars[3], expected_str, ఖాళీ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    తిరిగి 1;
	}
    }
    తిరిగి 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    శూన్య
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    శూన్య
f_assert_సరిపోలిక(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_సరిపోలిక_common(argvars, ASSERT_సరిపోలిక);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    శూన్య
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_సరిపోలిక_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    శూన్య
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_సంఖ్య = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    శూన్య
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    శూన్య
f_test_alloc_fail(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_సంఖ్య
	    || argvars[0].vval.v_సంఖ్య <= 0
	    || argvars[1].v_type != VAR_సంఖ్య
	    || argvars[1].vval.v_సంఖ్య < 0
	    || argvars[2].v_type != VAR_సంఖ్య)
	emsg(_(e_invalid_వాదం));
    లేకపోతే
    {
	alloc_fail_id = argvars[0].vval.v_సంఖ్య;
	ఒకవేళ (alloc_fail_id >= aid_చివరి)
	    emsg(_(e_invalid_వాదం));
	alloc_fail_countdown = argvars[1].vval.v_సంఖ్య;
	alloc_fail_repeat = argvars[2].vval.v_సంఖ్య;
	did_outofmem_సందేశం = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    శూన్య
f_test_autochdir(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#ఒకవేళ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#అయితే_ముగింపు
}

/*
 * "test_feedinput()"
 */
    శూన్య
f_test_feedinput(typval_T *argvars, typval_T *rettv వాడని)
{
#నిర్వచితమైతే USE_ఆదానం_BUF
    char_u	*val;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;
# endif

    ఒకవేళ (val != ఖాళీ)
    {
	trash_ఆదానం_buf();
	కూడు_గమ్యం_ఆదానం_buf_csi(val, (పూర్ణాంకం)STRLEN(val));
    }
#అయితే_ముగింపు
}

/*
 * "test_getvalue({name})" function
 */
    శూన్య
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    name = tv_get_string(&argvars[0]);

    ఒకవేళ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_సంఖ్య = need_fileinfo;
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    శూన్య
f_test_option_not_సమితి(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u *name = (char_u *)"";

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (reset_option_was_సమితి(name) == FAIL)
	semsg(_(e_invalid_వాదం_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    శూన్య
f_test_override(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u *name = (char_u *)"";
    పూర్ణాంకం     val;
    స్థిర పూర్ణాంకం save_starting = -1;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL)
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    val = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);

    ఒకవేళ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"starting") == 0)
    {
	ఒకవేళ (val)
	{
	    ఒకవేళ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	లేకపోతే
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_సంకేతం_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_పదం_props_on_termresponse = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_పంక్తులు = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    లేకపోతే ఒకవేళ (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_పదం_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	ఒకవేళ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    శూన్య
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం retval = -1;

    ఎంపిక (argvars[0].v_type)
    {
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_BOOL:
	స్థితి VAR_FLOAT:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_STRING:
	స్థితి VAR_INSTR:
	    ఆపు;

	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (argvars[0].vval.v_పని != ఖాళీ)
		retval = argvars[0].vval.v_పని->jv_refcount - 1;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (argvars[0].vval.v_channel != ఖాళీ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_FUNC:
	    ఒకవేళ (argvars[0].vval.v_string != ఖాళీ)
	    {
		ufunc_T *fp;

		fp = కనుగొను_func(argvars[0].vval.v_string, FALSE);
		ఒకవేళ (fp != ఖాళీ)
		    retval = fp->uf_refcount;
	    }
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (argvars[0].vval.v_partial != ఖాళీ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ఆపు;
	స్థితి VAR_BLOB:
	    ఒకవేళ (argvars[0].vval.v_blob != ఖాళీ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ఆపు;
	స్థితి VAR_LIST:
	    ఒకవేళ (argvars[0].vval.v_list != ఖాళీ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ఆపు;
	స్థితి VAR_TUPLE:
	    ఒకవేళ (argvars[0].vval.v_tuple != ఖాళీ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ (argvars[0].vval.v_dict != ఖాళీ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ఆపు;
	స్థితి VAR_CLASS:
	    ఒకవేళ (argvars[0].vval.v_class != ఖాళీ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ఆపు;
	స్థితి VAR_OBJECT:
	    ఒకవేళ (argvars[0].vval.v_object != ఖాళీ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ఆపు;
	స్థితి VAR_TYPEALIAS:
	    ఒకవేళ (argvars[0].vval.v_typealias != ఖాళీ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ఆపు;
    }

    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    శూన్య
f_test_garbagecollect_now(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ఒకవేళ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_సమితి));
    లేకపోతే
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    శూన్య
f_test_garbagecollect_soon(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    శూన్య
f_test_ignore_error(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    శూన్య
f_test_null_blob(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ఖాళీ;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
f_test_null_channel(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ఖాళీ;
}
#అయితే_ముగింపు

    శూన్య
f_test_null_dict(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv_dict_సమితి(rettv, ఖాళీ);
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
f_test_null_పని(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_పని;
    rettv->vval.v_పని = ఖాళీ;
}
#అయితే_ముగింపు

    శూన్య
f_test_null_list(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv_list_సమితి(rettv, ఖాళీ);
}

    శూన్య
f_test_null_function(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ఖాళీ;
}

    శూన్య
f_test_null_partial(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ఖాళీ;
}

    శూన్య
f_test_null_string(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
}

    శూన్య
f_test_null_tuple(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv_tuple_సమితి(rettv, ఖాళీ);
}

    శూన్య
f_test_unknown(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    శూన్య
f_test_void(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    శూన్య
f_test_setmouse(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_సంఖ్య || argvars[1].v_type != VAR_సంఖ్య)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    mouse_అడ్డువరుస = (time_t)tv_get_సంఖ్య(&argvars[0]) - 1;
    mouse_నిలువు = (time_t)tv_get_సంఖ్య(&argvars[1]) - 1;
}

#నిర్వచితమైతే FEAT_GUI
    స్థిర పూర్ణాంకం
test_gui_drop_files(dict_T *వాదనలు వాడని)
{
# ఒకవేళ defined(HAVE_DROP_FILE)
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    int_u	mods;
    char_u	**fnames;
    పూర్ణాంకం		లెక్క = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "files")
	    || !dict_has_కీలకం(వాదనలు, "row")
	    || !dict_has_కీలకం(వాదనలు, "col")
	    || !dict_has_కీలకం(వాదనలు, "modifiers"))
	తిరిగి FALSE;

    (శూన్య)dict_get_tv(వాదనలు, "files", &t);
    అడ్డువరుస = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "row");
    నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "col");
    mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");

    ఒకవేళ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	తిరిగి FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ఒకవేళ (fnames == ఖాళీ)
	తిరిగి FALSE;

    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	// ignore non-string items
	ఒకవేళ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ఖాళీ)
	    కొనసాగు;

	fnames[లెక్క] = vim_strsave(li->li_tv.vval.v_string);
	ఒకవేళ (fnames[లెక్క] == ఖాళీ)
	{
	    అయితే (--లెక్క >= 0)
		vim_free(fnames[లెక్క]);
	    vim_free(fnames);
	    తిరిగి FALSE;
	}
	లెక్క++;
    }

    ఒకవేళ (లెక్క > 0)
	gui_handle_drop(పాఠ్యం_X(నిలువు - 1), పాఠ్యం_Y(అడ్డువరుస - 1), mods, fnames, లెక్క);
    లేకపోతే
	vim_free(fnames);
# endif

    తిరిగి TRUE;
}

# ఒకవేళ defined(కనుగొను_REPLACE_DIALOG)
    స్థిర పూర్ణాంకం
test_gui_కనుగొను_repl(dict_T *వాదనలు)
{
    పూర్ణాంకం		flags;
    char_u	*కనుగొను_పాఠ్యం;
    char_u	*repl_పాఠ్యం;
    పూర్ణాంకం		forward;
    పూర్ణాంకం		retval;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "find_text")
	    || !dict_has_కీలకం(వాదనలు, "repl_text")
	    || !dict_has_కీలకం(వాదనలు, "flags")
	    || !dict_has_కీలకం(వాదనలు, "forward"))
	తిరిగి FALSE;

    కనుగొను_పాఠ్యం = dict_get_string(వాదనలు, "find_text", TRUE);
    repl_పాఠ్యం = dict_get_string(వాదనలు, "repl_text", TRUE);
    flags = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "flags");
    forward = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "forward");

    retval = gui_do_findrepl(flags, కనుగొను_పాఠ్యం, repl_పాఠ్యం, forward);
    vim_free(కనుగొను_పాఠ్యం);
    vim_free(repl_పాఠ్యం);

    తిరిగి retval;
}
# endif

    స్థిర పూర్ణాంకం
test_gui_mouse_event(dict_T *వాదనలు)
{
    పూర్ణాంకం		button;
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		repeated_click;
    int_u	mods;
    పూర్ణాంకం		move;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "row")
	    || !dict_has_కీలకం(వాదనలు, "col"))
	తిరిగి FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (పూర్ణాంకం)dict_get_bool(వాదనలు, "move", FALSE);

    ఒకవేళ (!move && (!dict_has_కీలకం(వాదనలు, "button")
	    || !dict_has_కీలకం(వాదనలు, "multiclick")
	    || !dict_has_కీలకం(వాదనలు, "modifiers")))
	తిరిగి FALSE;

    అడ్డువరుస = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "row");
    నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "col");

    ఒకవేళ (move)
    {
	పూర్ణాంకం pY = అడ్డువరుస;
	పూర్ణాంకం pX = నిలువు;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ఒకవేళ (dict_get_bool(వాదనలు, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (అడ్డువరుస - 1) * gui.char_ఎత్తు + gui.char_ఎత్తు / 2;
	    pX = (నిలువు - 1) * gui.char_వెడల్పు + gui.char_వెడల్పు / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    లేకపోతే
    {
	button = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "button");
	repeated_click = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "multiclick");
	mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_సమితి_hor_scroll_అడుగు(6);
	mouse_సమితి_vert_scroll_అడుగు(3);

	gui_send_mouse_event(button, పాఠ్యం_X(నిలువు - 1), పాఠ్యం_Y(అడ్డువరుస - 1),
							repeated_click, mods);
    }

    తిరిగి TRUE;
}

    స్థిర పూర్ణాంకం
test_gui_scrollbar(dict_T *వాదనలు)
{
    char_u	*which;
    దీర్ఘ	విలువ;
    పూర్ణాంకం		dragging;
    scrollbar_T *sb = ఖాళీ;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "which")
	    || !dict_has_కీలకం(వాదనలు, "value")
	    || !dict_has_కీలకం(వాదనలు, "dragging"))
	తిరిగి FALSE;

    which = dict_get_string(వాదనలు, "which", FALSE);
    విలువ = (దీర్ఘ)dict_get_సంఖ్య(వాదనలు, "value");
    dragging = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "dragging");

    ఒకవేళ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ఎడమ];
    లేకపోతే ఒకవేళ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_కుడి];
    లేకపోతే ఒకవేళ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ఒకవేళ (sb == ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str), which);
	తిరిగి FALSE;
    }
    gui_drag_scrollbar(sb, విలువ, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_సాధారణ(FALSE, TRUE, FALSE);
# endif

    తిరిగి TRUE;
}

    స్థిర పూర్ణాంకం
test_gui_tabline_event(dict_T *వాదనలు వాడని)
{
# ifdef FEAT_GUI_TABLINE
    పూర్ణాంకం		tabnr;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "tabnr"))
	తిరిగి FALSE;

    tabnr = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "tabnr");

    తిరిగి send_tabline_event(tabnr);
# లేకపోతే
    తిరిగి FALSE;
# endif
}

    స్థిర పూర్ణాంకం
test_gui_tabmenu_event(dict_T *వాదనలు వాడని)
{
# ifdef FEAT_GUI_TABLINE
    పూర్ణాంకం	tabnr;
    పూర్ణాంకం	అంశం;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "tabnr")
	    || !dict_has_కీలకం(వాదనలు, "item"))
	తిరిగి FALSE;

    tabnr = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "tabnr");
    అంశం = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "item");

    send_tabline_menu_event(tabnr, అంశం);
# endif
    తిరిగి TRUE;
}
#అయితే_ముగింపు

    శూన్య
f_test_mswin_event(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#నిర్వచితమైతే MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = FALSE;

    ఒకవేళ (sandbox != 0)
    {
	emsg(_(e_not_allowed_లోపల_sandbox));
	తిరిగి;
    }

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ఖాళీ)
	తిరిగి;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_సంఖ్య = test_mswin_event(event, argvars[1].vval.v_dict);

#అయితే_ముగింపు
}

    శూన్య
f_test_gui_event(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#నిర్వచితమైతే FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = FALSE;

    ఒకవేళ (sandbox != 0)
    {
	emsg(_(e_not_allowed_లోపల_sandbox));
	తిరిగి;
    }

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ఖాళీ)
	తిరిగి;

    event = tv_get_string(&argvars[0]);
    ఒకవేళ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_సంఖ్య = test_gui_drop_files(argvars[1].vval.v_dict);
# ఒకవేళ defined(కనుగొను_REPLACE_DIALOG)
    లేకపోతే ఒకవేళ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_సంఖ్య = test_gui_కనుగొను_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    లేకపోతే ఒకవేళ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_సంఖ్య = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    లేకపోతే ఒకవేళ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_సంఖ్య = test_gui_mouse_event(argvars[1].vval.v_dict);
    లేకపోతే ఒకవేళ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_సంఖ్య = test_gui_scrollbar(argvars[1].vval.v_dict);
    లేకపోతే ఒకవేళ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_సంఖ్య = test_gui_tabline_event(argvars[1].vval.v_dict);
    లేకపోతే ఒకవేళ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_సంఖ్య = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str), event);
	తిరిగి;
    }
#అయితే_ముగింపు
}

    శూన్య
f_test_settime(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    సమయం_for_testing = (time_t)tv_get_సంఖ్య(&argvars[0]);
}

#అయితే_ముగింపు // defined(FEAT_EVAL)
