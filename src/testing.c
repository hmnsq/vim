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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    स्थिर शून्य
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    जइ (sname != रिक्त)
    {
	ga_concat(gap, sname);
	जइ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    जइ (SOURCING_LNUM > 0)
    {
	वर्ण    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, परिमाण(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    जइ (sname != रिक्त || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    स्थिर शून्य
ga_concat_esc(garray_T *gap, char_u *p, पूर्णाङ्क clen)
{
    char_u  buf[NUMBUFLEN];

    जइ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	प्रत्यागम;
    }

    विकल्प (*p)
    {
	पक्ष BS: GA_CONCAT_LITERAL(gap, "\\b"); भङ्ग;
	पक्ष ESC: GA_CONCAT_LITERAL(gap, "\\e"); भङ्ग;
	पक्ष FF: GA_CONCAT_LITERAL(gap, "\\f"); भङ्ग;
	पक्ष NL: GA_CONCAT_LITERAL(gap, "\\n"); भङ्ग;
	पक्ष TAB: GA_CONCAT_LITERAL(gap, "\\t"); भङ्ग;
	पक्ष CAR: GA_CONCAT_LITERAL(gap, "\\r"); भङ्ग;
	पक्ष '\\': GA_CONCAT_LITERAL(gap, "\\\\"); भङ्ग;
	स्वभाव:
	   जइ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((वर्ण *)buf, परिमाण(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   अण्णहा
	       ga_append(gap, *p);
	   भङ्ग;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    स्थिर शून्य
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    पूर्णाङ्क	    c;
    पूर्णाङ्क	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    पूर्णाङ्क	    same_len;

    जइ (str == रिक्त)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	प्रत्यागम;
    }

    प्रति (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	जाव (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	जइ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((वर्ण *)buf, परिमाण(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	अण्णहा
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    स्थिर शून्य
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_सन्देशः_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    पूर्णाङ्क		did_copy = FALSE;
    पूर्णाङ्क		omitted = 0;

    जइ (opt_सन्देशः_tv->v_type != VAR_UNKNOWN
	    && !(opt_सन्देशः_tv->v_type == VAR_STRING
		&& (opt_सन्देशः_tv->vval.v_string == रिक्त
		    || *opt_सन्देशः_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_सन्देशः_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    जइ (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    अण्णहा जइ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    अण्णहा
	GA_CONCAT_LITERAL(gap, "Expected ");
    जइ (exp_str == रिक्त)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	जइ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != रिक्त && got_tv->vval.v_dict != रिक्त)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    पूर्णाङ्क		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    जइ (exp_tv->vval.v_dict == रिक्त || got_tv->vval.v_dict == रिक्त)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = रिक्त;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = रिक्त;
		प्रत्यागम;
	    }

	    todo = (पूर्णाङ्क)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_वस्तूनि(&exp_d->dv_hashtab, hi, todo)
	    {
		जइ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_कुञ्चिका, -1);
		    जइ (item2 == रिक्त || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(वर्ण *)hi->hi_कुञ्चिका, &HI2DI(hi)->di_tv);
			जइ (item2 != रिक्त)
			    dict_add_tv(got_tv->vval.v_dict,
					    (वर्ण *)hi->hi_कुञ्चिका, &item2->di_tv);
		    }
		    अण्णहा
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (पूर्णाङ्क)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_वस्तूनि(&got_d->dv_hashtab, hi, todo)
	    {
		जइ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_कुञ्चिका, -1);
		    जइ (item2 == रिक्त)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(वर्ण *)hi->hi_कुञ्चिका, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    अण्णहा
    {
	जइ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	जइ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    जइ (atype != ASSERT_NOTEQUAL)
    {
	जइ (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	अण्णहा जइ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	अण्णहा
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	जइ (omitted != 0)
	{
	    वर्ण    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, परिमाण(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    जइ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    स्थिर पूर्णाङ्क
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    जइ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], रिक्त, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    जइ (pat != रिक्त && text != रिक्त
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], रिक्त, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    स्थिर पूर्णाङ्क
assert_bool(typval_T *argvars, पूर्णाङ्क isTrue)
{
    पूर्णाङ्क		error = FALSE;
    garray_T	ga;

    जइ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_सङ्ख्या == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	प्रत्यागम 0;
    जइ (argvars[0].v_type != VAR_सङ्ख्या
	    || (tv_get_सङ्ख्या_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		रिक्त, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

    स्थिर शून्य
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    जइ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    अण्णहा
	ga_concat(gap, cmd);
}

    स्थिर पूर्णाङ्क
assert_beeps(typval_T *argvars, पूर्णाङ्क no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    पूर्णाङ्क		ret = 0;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    जइ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	जइ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	अण्णहा
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    प्रत्यागम ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    शून्य
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    शून्य
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    शून्य
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_equal_common(argvars, ASSERT_EQUAL);
}

    स्थिर पूर्णाङ्क
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    वर्ण	line1[200];
    वर्ण	line2[200];
    पूर्णाङ्क		lineidx = 0;
    size_t	IObufflen;

    जइ (fname1 == रिक्त || fname2 == रिक्त)
	प्रत्यागम 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((वर्ण *)fname1, READBIN);
    जइ (fd1 == रिक्त)
    {
	IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
	    (वर्ण *)e_cant_read_file_str, fname1);
    }
    अण्णहा
    {
	fd2 = mch_fopen((वर्ण *)fname2, READBIN);
	जइ (fd2 == रिक्त)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
		(वर्ण *)e_cant_read_file_str, fname2);
	}
	अण्णहा
	{
	    पूर्णाङ्क	    c1, c2;
	    long    गणनम् = 0;
	    long    linecount = 1;

	    प्रति (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		जइ (c1 == EOF)
		{
		    जइ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    भङ्ग;
		}
		अण्णहा जइ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    भङ्ग;
		}
		अण्णहा
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    जइ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", गणनम्, linecount);
			भङ्ग;
		    }
		}
		++गणनम्;
		जइ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		अण्णहा जइ (lineidx + 2 == (पूर्णाङ्क)परिमाण(line1))
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

    जइ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	जइ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	जइ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    जइ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	प्रत्यागम 1;
    }

    प्रत्यागम 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    शून्य
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    शून्य
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    शून्य
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    error = tv_get_string_chk(&argvars[0]);
    जइ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_सङ्ख्या = 1;
    }
    अण्णहा जइ (error != रिक्त
	&& strstr((वर्ण *)get_vim_var_str(VV_EXCEPTION), (वर्ण *)error) == रिक्त)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], रिक्त, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_सङ्ख्या = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    शून्य
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    पूर्णाङ्क		save_trylevel = trylevel;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    वर्ण	*wrong_arg_सन्देशः = रिक्त;
    char_u	*tofree = रिक्त;

    जइ (check_for_string_or_सङ्ख्या_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	प्रत्यागम;

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

    जइ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_सङ्ख्या = 1;
    }
    अण्णहा जइ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = रिक्त;
	पूर्णाङ्क	error_found = FALSE;
	पूर्णाङ्क	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_सन्देशः == रिक्त ? (char_u *)"[unknown]"
						       : emsg_assert_fails_सन्देशः;

	जइ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == रिक्त
			   || strstr((वर्ण *)actual, (वर्ण *)expected) == रिक्त;
	}
	अण्णहा जइ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    जइ (list == रिक्त || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_सन्देशः = e_assert_fails_second_arg;
		गच्छ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    जइ (expected == रिक्त)
		गच्छ theend;
	    जइ (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    अण्णहा जइ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		जइ (actual != रिक्त)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    जइ (expected == रिक्त)
			गच्छ theend;
		    जइ (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	अण्णहा
	{
	    wrong_arg_सन्देशः = e_assert_fails_second_arg;
	    गच्छ theend;
	}

	जइ (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    जइ (argvars[3].v_type != VAR_सङ्ख्या)
	    {
		wrong_arg_सन्देशः = e_assert_fails_fourth_argument;
		गच्छ theend;
	    }
	    अण्णहा जइ (argvars[3].vval.v_सङ्ख्या >= 0
			 && argvars[3].vval.v_सङ्ख्या != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    जइ (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		जइ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_सन्देशः = e_assert_fails_fifth_argument;
		    गच्छ theend;
		}
		अण्णहा जइ (argvars[4].vval.v_string != रिक्त
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	जइ (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    जइ (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_सङ्ख्या;
		actual_tv.vval.v_सङ्ख्या = emsg_assert_fails_lnum;
	    }
	    अण्णहा जइ (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    अण्णहा
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
	    rettv->vval.v_सङ्ख्या = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    सन्देशः_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    सन्देशः_scrolled = 0;
    पङ्क्तयः_वामः = Rows;
    VIM_CLEAR(emsg_assert_fails_सन्देशः);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, रिक्त, 0);
    जइ (wrong_arg_सन्देशः != रिक्त)
	emsg(_(wrong_arg_सन्देशः));
}

/*
 * "assert_false(actual[, msg])" function
 */
    शून्य
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_bool(argvars, FALSE);
}

    स्थिर पूर्णाङ्क
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    पूर्णाङ्क		error = FALSE;
    char_u	expected_str[200];

    जइ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	जइ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((वर्ण *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, रिक्त,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    प्रत्यागम 1;
	}
    }
    अण्णहा
    {
	varnumber_T	lower = tv_get_सङ्ख्या_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_सङ्ख्या_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_सङ्ख्या_chk(&argvars[2], &error);

	जइ (error)
	    प्रत्यागम 0;
	जइ (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((वर्ण *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, रिक्त,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    प्रत्यागम 1;
	}
    }
    प्रत्यागम 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    शून्य
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    जइ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    शून्य
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    शून्य
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    शून्य
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_सङ्ख्या = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    शून्य
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    शून्य
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    जइ (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    जइ (argvars[0].v_type != VAR_सङ्ख्या
	    || argvars[0].vval.v_सङ्ख्या <= 0
	    || argvars[1].v_type != VAR_सङ्ख्या
	    || argvars[1].vval.v_सङ्ख्या < 0
	    || argvars[2].v_type != VAR_सङ्ख्या)
	emsg(_(e_invalid_argument));
    अण्णहा
    {
	alloc_fail_id = argvars[0].vval.v_सङ्ख्या;
	जइ (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_सङ्ख्या;
	alloc_fail_repeat = argvars[2].vval.v_सङ्ख्या;
	did_outofmem_सन्देशः = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    शून्य
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#जइ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    शून्य
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_निवेशः_BUF
    char_u	*val;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    जइ (!gui.in_use)
	प्रत्यागम;
# endif

    जइ (val != रिक्त)
    {
	trash_निवेशः_buf();
	add_to_निवेशः_buf_csi(val, (पूर्णाङ्क)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    शून्य
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    जइ (check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);

    जइ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_सङ्ख्या = need_fileinfo;
    अण्णहा
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    शून्य
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    जइ (check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    जइ (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    शून्य
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    पूर्णाङ्क     val;
    स्थिर पूर्णाङ्क save_starting = -1;

    जइ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    val = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]);

    जइ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"starting") == 0)
    {
	जइ (val)
	{
	    जइ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	अण्णहा
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    अण्णहा जइ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_पङ्क्तयः = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    अण्णहा जइ (STRCMP(name, (char_u *)"ALL") == 0)
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
	जइ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    अण्णहा
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    शून्य
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क retval = -1;

    विकल्प (argvars[0].v_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_BOOL:
	पक्ष VAR_FLOAT:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_STRING:
	पक्ष VAR_INSTR:
	    भङ्ग;

	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    जइ (argvars[0].vval.v_job != रिक्त)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    भङ्ग;
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    जइ (argvars[0].vval.v_channel != रिक्त)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    भङ्ग;
	पक्ष VAR_FUNC:
	    जइ (argvars[0].vval.v_string != रिक्त)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		जइ (fp != रिक्त)
		    retval = fp->uf_refcount;
	    }
	    भङ्ग;
	पक्ष VAR_PARTIAL:
	    जइ (argvars[0].vval.v_partial != रिक्त)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    भङ्ग;
	पक्ष VAR_BLOB:
	    जइ (argvars[0].vval.v_blob != रिक्त)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    भङ्ग;
	पक्ष VAR_LIST:
	    जइ (argvars[0].vval.v_list != रिक्त)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    जइ (argvars[0].vval.v_tuple != रिक्त)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    भङ्ग;
	पक्ष VAR_DICT:
	    जइ (argvars[0].vval.v_dict != रिक्त)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    भङ्ग;
	पक्ष VAR_CLASS:
	    जइ (argvars[0].vval.v_class != रिक्त)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    जइ (argvars[0].vval.v_object != रिक्त)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    भङ्ग;
	पक्ष VAR_TYPEALIAS:
	    जइ (argvars[0].vval.v_typealias != रिक्त)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    भङ्ग;
    }

    rettv->v_type = VAR_सङ्ख्या;
    rettv->vval.v_सङ्ख्या = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    शून्य
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    जइ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    अण्णहा
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    शून्य
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    शून्य
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    जइ (check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    शून्य
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = रिक्त;
}

#जइ defined(FEAT_JOB_CHANNEL)
    शून्य
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = रिक्त;
}
#endif

    शून्य
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, रिक्त);
}

#जइ defined(FEAT_JOB_CHANNEL)
    शून्य
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = रिक्त;
}
#endif

    शून्य
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, रिक्त);
}

    शून्य
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = रिक्त;
}

    शून्य
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = रिक्त;
}

    शून्य
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
}

    शून्य
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, रिक्त);
}

    शून्य
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    शून्य
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    शून्य
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    जइ (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    जइ (argvars[0].v_type != VAR_सङ्ख्या || argvars[1].v_type != VAR_सङ्ख्या)
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    mouse_row = (time_t)tv_get_सङ्ख्या(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_सङ्ख्या(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    स्थिर पूर्णाङ्क
test_gui_drop_files(dict_T *args UNUSED)
{
# जइ defined(HAVE_DROP_FILE)
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    int_u	mods;
    char_u	**fnames;
    पूर्णाङ्क		गणनम् = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    जइ (!dict_has_कुञ्चिका(args, "files")
	    || !dict_has_कुञ्चिका(args, "row")
	    || !dict_has_कुञ्चिका(args, "col")
	    || !dict_has_कुञ्चिका(args, "modifiers"))
	प्रत्यागम FALSE;

    (शून्य)dict_get_tv(args, "files", &t);
    row = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "row");
    col = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "col");
    mods = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "modifiers");

    जइ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	प्रत्यागम FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    जइ (fnames == रिक्त)
	प्रत्यागम FALSE;

    FOR_ALL_LIST_वस्तूनि(l, li)
    {
	// ignore non-string items
	जइ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == रिक्त)
	    अनुवर्त;

	fnames[गणनम्] = vim_strsave(li->li_tv.vval.v_string);
	जइ (fnames[गणनम्] == रिक्त)
	{
	    जाव (--गणनम् >= 0)
		vim_free(fnames[गणनम्]);
	    vim_free(fnames);
	    प्रत्यागम FALSE;
	}
	गणनम्++;
    }

    जइ (गणनम् > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, गणनम्);
    अण्णहा
	vim_free(fnames);
# endif

    प्रत्यागम TRUE;
}

# जइ defined(FIND_REPLACE_DIALOG)
    स्थिर पूर्णाङ्क
test_gui_find_repl(dict_T *args)
{
    पूर्णाङ्क		flags;
    char_u	*find_text;
    char_u	*repl_text;
    पूर्णाङ्क		forward;
    पूर्णाङ्क		retval;

    जइ (!dict_has_कुञ्चिका(args, "find_text")
	    || !dict_has_कुञ्चिका(args, "repl_text")
	    || !dict_has_कुञ्चिका(args, "flags")
	    || !dict_has_कुञ्चिका(args, "forward"))
	प्रत्यागम FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "flags");
    forward = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    प्रत्यागम retval;
}
# endif

    स्थिर पूर्णाङ्क
test_gui_mouse_event(dict_T *args)
{
    पूर्णाङ्क		button;
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    पूर्णाङ्क		repeated_click;
    int_u	mods;
    पूर्णाङ्क		move;

    जइ (!dict_has_कुञ्चिका(args, "row")
	    || !dict_has_कुञ्चिका(args, "col"))
	प्रत्यागम FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (पूर्णाङ्क)dict_get_bool(args, "move", FALSE);

    जइ (!move && (!dict_has_कुञ्चिका(args, "button")
	    || !dict_has_कुञ्चिका(args, "multiclick")
	    || !dict_has_कुञ्चिका(args, "modifiers")))
	प्रत्यागम FALSE;

    row = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "row");
    col = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "col");

    जइ (move)
    {
	पूर्णाङ्क pY = row;
	पूर्णाङ्क pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	जइ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    अण्णहा
    {
	button = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "button");
	repeated_click = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "multiclick");
	mods = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    प्रत्यागम TRUE;
}

    स्थिर पूर्णाङ्क
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	मानम्;
    पूर्णाङ्क		dragging;
    scrollbar_T *sb = रिक्त;

    जइ (!dict_has_कुञ्चिका(args, "which")
	    || !dict_has_कुञ्चिका(args, "value")
	    || !dict_has_कुञ्चिका(args, "dragging"))
	प्रत्यागम FALSE;

    which = dict_get_string(args, "which", FALSE);
    मानम् = (long)dict_get_सङ्ख्या(args, "value");
    dragging = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "dragging");

    जइ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_वामः];
    अण्णहा जइ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_दक्षिणः];
    अण्णहा जइ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    जइ (sb == रिक्त)
    {
	semsg(_(e_invalid_argument_str), which);
	प्रत्यागम FALSE;
    }
    gui_drag_scrollbar(sb, मानम्, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    प्रत्यागम TRUE;
}

    स्थिर पूर्णाङ्क
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    पूर्णाङ्क		tabnr;

    जइ (!dict_has_कुञ्चिका(args, "tabnr"))
	प्रत्यागम FALSE;

    tabnr = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "tabnr");

    प्रत्यागम send_tabline_event(tabnr);
# अण्णहा
    प्रत्यागम FALSE;
# endif
}

    स्थिर पूर्णाङ्क
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    पूर्णाङ्क	tabnr;
    पूर्णाङ्क	item;

    जइ (!dict_has_कुञ्चिका(args, "tabnr")
	    || !dict_has_कुञ्चिका(args, "item"))
	प्रत्यागम FALSE;

    tabnr = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "tabnr");
    item = (पूर्णाङ्क)dict_get_सङ्ख्या(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    प्रत्यागम TRUE;
}
#endif

    शून्य
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_सङ्ख्या = FALSE;

    जइ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	प्रत्यागम;
    }

    जइ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == रिक्त)
	प्रत्यागम;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_सङ्ख्या = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    शून्य
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_सङ्ख्या = FALSE;

    जइ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	प्रत्यागम;
    }

    जइ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == रिक्त)
	प्रत्यागम;

    event = tv_get_string(&argvars[0]);
    जइ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_drop_files(argvars[1].vval.v_dict);
# जइ defined(FIND_REPLACE_DIALOG)
    अण्णहा जइ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    अण्णहा जइ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_सङ्ख्या = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    अण्णहा जइ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_mouse_event(argvars[1].vval.v_dict);
    अण्णहा जइ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_scrollbar(argvars[1].vval.v_dict);
    अण्णहा जइ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_tabline_event(argvars[1].vval.v_dict);
    अण्णहा जइ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_सङ्ख्या = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    अण्णहा
    {
	semsg(_(e_invalid_argument_str), event);
	प्रत्यागम;
    }
#endif
}

    शून्य
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    जइ (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    time_for_testing = (time_t)tv_get_सङ्ख्या(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
