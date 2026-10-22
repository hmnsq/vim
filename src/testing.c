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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    സ്ഥിതം ശൂന്യം
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    എങ്കിൽ (sname != ശൂന്യ)
    {
	ga_concat(gap, sname);
	എങ്കിൽ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    എങ്കിൽ (SOURCING_LNUM > 0)
    {
	അക്ഷരം    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, വലുപ്പം(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    എങ്കിൽ (sname != ശൂന്യ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    സ്ഥിതം ശൂന്യം
ga_concat_esc(garray_T *gap, char_u *p, പൂർണ്ണസംഖ്യ clen)
{
    char_u  buf[NUMBUFLEN];

    എങ്കിൽ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	തിരികെ;
    }

    തിരഞ്ഞെടുപ്പ് (*p)
    {
	അവസ്ഥ BS: GA_CONCAT_LITERAL(gap, "\\b"); നിർത്തുക;
	അവസ്ഥ ESC: GA_CONCAT_LITERAL(gap, "\\e"); നിർത്തുക;
	അവസ്ഥ FF: GA_CONCAT_LITERAL(gap, "\\f"); നിർത്തുക;
	അവസ്ഥ NL: GA_CONCAT_LITERAL(gap, "\\n"); നിർത്തുക;
	അവസ്ഥ TAB: GA_CONCAT_LITERAL(gap, "\\t"); നിർത്തുക;
	അവസ്ഥ CAR: GA_CONCAT_LITERAL(gap, "\\r"); നിർത്തുക;
	അവസ്ഥ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	   എങ്കിൽ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((അക്ഷരം *)buf, വലുപ്പം(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   അല്ലെങ്കിൽ
	       ga_append(gap, *p);
	   നിർത്തുക;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    സ്ഥിതം ശൂന്യം
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    പൂർണ്ണസംഖ്യ	    c;
    പൂർണ്ണസംഖ്യ	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    പൂർണ്ണസംഖ്യ	    same_len;

    എങ്കിൽ (str == ശൂന്യ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	തിരികെ;
    }

    വേണ്ടി (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ആകുമ്പോൾ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	എങ്കിൽ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((അക്ഷരം *)buf, വലുപ്പം(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	അല്ലെങ്കിൽ
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    സ്ഥിതം ശൂന്യം
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_സന്ദേശം_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    പൂർണ്ണസംഖ്യ		did_copy = FALSE;
    പൂർണ്ണസംഖ്യ		omitted = 0;

    എങ്കിൽ (opt_സന്ദേശം_tv->v_type != VAR_UNKNOWN
	    && !(opt_സന്ദേശം_tv->v_type == VAR_STRING
		&& (opt_സന്ദേശം_tv->vval.v_string == ശൂന്യ
		    || *opt_സന്ദേശം_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_സന്ദേശം_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    എങ്കിൽ (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    അല്ലെങ്കിൽ എങ്കിൽ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    അല്ലെങ്കിൽ
	GA_CONCAT_LITERAL(gap, "Expected ");
    എങ്കിൽ (exp_str == ശൂന്യ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	എങ്കിൽ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ശൂന്യ && got_tv->vval.v_dict != ശൂന്യ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    പൂർണ്ണസംഖ്യ		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    എങ്കിൽ (exp_tv->vval.v_dict == ശൂന്യ || got_tv->vval.v_dict == ശൂന്യ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ശൂന്യ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ശൂന്യ;
		തിരികെ;
	    }

	    todo = (പൂർണ്ണസംഖ്യ)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&exp_d->dv_hashtab, hi, todo)
	    {
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_താക്കോൽ, -1);
		    എങ്കിൽ (item2 == ശൂന്യ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(അക്ഷരം *)hi->hi_താക്കോൽ, &HI2DI(hi)->di_tv);
			എങ്കിൽ (item2 != ശൂന്യ)
			    dict_add_tv(got_tv->vval.v_dict,
					    (അക്ഷരം *)hi->hi_താക്കോൽ, &item2->di_tv);
		    }
		    അല്ലെങ്കിൽ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (പൂർണ്ണസംഖ്യ)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&got_d->dv_hashtab, hi, todo)
	    {
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_താക്കോൽ, -1);
		    എങ്കിൽ (item2 == ശൂന്യ)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(അക്ഷരം *)hi->hi_താക്കോൽ, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	എങ്കിൽ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    എങ്കിൽ (atype != ASSERT_NOTEQUAL)
    {
	എങ്കിൽ (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	അല്ലെങ്കിൽ എങ്കിൽ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	അല്ലെങ്കിൽ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	എങ്കിൽ (omitted != 0)
	{
	    അക്ഷരം    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, വലുപ്പം(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    എങ്കിൽ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    എങ്കിൽ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ശൂന്യ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	തിരികെ 1;
    }
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    എങ്കിൽ (pat != ശൂന്യ && text != ശൂന്യ
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ശൂന്യ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	തിരികെ 1;
    }
    തിരികെ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_bool(typval_T *argvars, പൂർണ്ണസംഖ്യ isTrue)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;
    garray_T	ga;

    എങ്കിൽ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_സംഖ്യ == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	തിരികെ 0;
    എങ്കിൽ (argvars[0].v_type != VAR_സംഖ്യ
	    || (tv_get_സംഖ്യ_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ശൂന്യ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	തിരികെ 1;
    }
    തിരികെ 0;
}

    സ്ഥിതം ശൂന്യം
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    അല്ലെങ്കിൽ
	ga_concat(gap, cmd);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_beeps(typval_T *argvars, പൂർണ്ണസംഖ്യ no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		ret = 0;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    എങ്കിൽ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	എങ്കിൽ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	അല്ലെങ്കിൽ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    തിരികെ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    ശൂന്യം
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    ശൂന്യം
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    ശൂന്യം
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_equal_common(argvars, ASSERT_EQUAL);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    അക്ഷരം	line1[200];
    അക്ഷരം	line2[200];
    പൂർണ്ണസംഖ്യ		lineidx = 0;
    size_t	IObufflen;

    എങ്കിൽ (fname1 == ശൂന്യ || fname2 == ശൂന്യ)
	തിരികെ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((അക്ഷരം *)fname1, READBIN);
    എങ്കിൽ (fd1 == ശൂന്യ)
    {
	IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE,
	    (അക്ഷരം *)e_cant_read_file_str, fname1);
    }
    അല്ലെങ്കിൽ
    {
	fd2 = mch_fopen((അക്ഷരം *)fname2, READBIN);
	എങ്കിൽ (fd2 == ശൂന്യ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE,
		(അക്ഷരം *)e_cant_read_file_str, fname2);
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ	    c1, c2;
	    long    എണ്ണം = 0;
	    long    linecount = 1;

	    വേണ്ടി (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		എങ്കിൽ (c1 == EOF)
		{
		    എങ്കിൽ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    എങ്കിൽ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", എണ്ണം, linecount);
			നിർത്തുക;
		    }
		}
		++എണ്ണം;
		എങ്കിൽ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (lineidx + 2 == (പൂർണ്ണസംഖ്യ)വലുപ്പം(line1))
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

    എങ്കിൽ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	എങ്കിൽ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    എങ്കിൽ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	തിരികെ 1;
    }

    തിരികെ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    ശൂന്യം
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    ശൂന്യം
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    ശൂന്യം
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    error = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_സംഖ്യ = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (error != ശൂന്യ
	&& strstr((അക്ഷരം *)get_vim_var_str(VV_EXCEPTION), (അക്ഷരം *)error) == ശൂന്യ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ശൂന്യ, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_സംഖ്യ = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    ശൂന്യം
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		save_trylevel = trylevel;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    അക്ഷരം	*wrong_arg_സന്ദേശം = ശൂന്യ;
    char_u	*tofree = ശൂന്യ;

    എങ്കിൽ (check_for_string_or_സംഖ്യ_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	തിരികെ;

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

    എങ്കിൽ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_സംഖ്യ = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	error_found = FALSE;
	പൂർണ്ണസംഖ്യ	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_സന്ദേശം == ശൂന്യ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_സന്ദേശം;

	എങ്കിൽ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == ശൂന്യ
			   || strstr((അക്ഷരം *)actual, (അക്ഷരം *)expected) == ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    എങ്കിൽ (list == ശൂന്യ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_സന്ദേശം = e_assert_fails_second_arg;
		പോകുക theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    എങ്കിൽ (expected == ശൂന്യ)
		പോകുക theend;
	    എങ്കിൽ (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		എങ്കിൽ (actual != ശൂന്യ)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    എങ്കിൽ (expected == ശൂന്യ)
			പോകുക theend;
		    എങ്കിൽ (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    wrong_arg_സന്ദേശം = e_assert_fails_second_arg;
	    പോകുക theend;
	}

	എങ്കിൽ (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (argvars[3].v_type != VAR_സംഖ്യ)
	    {
		wrong_arg_സന്ദേശം = e_assert_fails_fourth_argument;
		പോകുക theend;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (argvars[3].vval.v_സംഖ്യ >= 0
			 && argvars[3].vval.v_സംഖ്യ != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    എങ്കിൽ (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		എങ്കിൽ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_സന്ദേശം = e_assert_fails_fifth_argument;
		    പോകുക theend;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argvars[4].vval.v_string != ശൂന്യ
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	എങ്കിൽ (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    എങ്കിൽ (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_സംഖ്യ;
		actual_tv.vval.v_സംഖ്യ = emsg_assert_fails_lnum;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    അല്ലെങ്കിൽ
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
	    rettv->vval.v_സംഖ്യ = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    സന്ദേശം_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    സന്ദേശം_scrolled = 0;
    വരികൾ_ഇടത് = Rows;
    VIM_CLEAR(emsg_assert_fails_സന്ദേശം);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, ശൂന്യ, 0);
    എങ്കിൽ (wrong_arg_സന്ദേശം != ശൂന്യ)
	emsg(_(wrong_arg_സന്ദേശം));
}

/*
 * "assert_false(actual[, msg])" function
 */
    ശൂന്യം
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_bool(argvars, FALSE);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    char_u	expected_str[200];

    എങ്കിൽ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	എങ്കിൽ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((അക്ഷരം *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ശൂന്യ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    തിരികെ 1;
	}
    }
    അല്ലെങ്കിൽ
    {
	varnumber_T	lower = tv_get_സംഖ്യ_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_സംഖ്യ_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_സംഖ്യ_chk(&argvars[2], &error);

	എങ്കിൽ (error)
	    തിരികെ 0;
	എങ്കിൽ (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((അക്ഷരം *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ശൂന്യ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    തിരികെ 1;
	}
    }
    തിരികെ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    ശൂന്യം
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    ശൂന്യം
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    ശൂന്യം
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    ശൂന്യം
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_സംഖ്യ = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    ശൂന്യം
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    ശൂന്യം
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_സംഖ്യ
	    || argvars[0].vval.v_സംഖ്യ <= 0
	    || argvars[1].v_type != VAR_സംഖ്യ
	    || argvars[1].vval.v_സംഖ്യ < 0
	    || argvars[2].v_type != VAR_സംഖ്യ)
	emsg(_(e_invalid_argument));
    അല്ലെങ്കിൽ
    {
	alloc_fail_id = argvars[0].vval.v_സംഖ്യ;
	എങ്കിൽ (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_സംഖ്യ;
	alloc_fail_repeat = argvars[2].vval.v_സംഖ്യ;
	did_outofmem_സന്ദേശം = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    ശൂന്യം
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#എങ്കിൽ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    ശൂന്യം
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_നിവേശം_BUF
    char_u	*val;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    എങ്കിൽ (!gui.in_use)
	തിരികെ;
# endif

    എങ്കിൽ (val != ശൂന്യ)
    {
	trash_നിവേശം_buf();
	add_to_നിവേശം_buf_csi(val, (പൂർണ്ണസംഖ്യ)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    ശൂന്യം
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    name = tv_get_string(&argvars[0]);

    എങ്കിൽ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_സംഖ്യ = need_fileinfo;
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    ശൂന്യം
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    ശൂന്യം
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    പൂർണ്ണസംഖ്യ     val;
    സ്ഥിതം പൂർണ്ണസംഖ്യ save_starting = -1;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL)
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    val = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);

    എങ്കിൽ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"starting") == 0)
    {
	എങ്കിൽ (val)
	{
	    എങ്കിൽ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	അല്ലെങ്കിൽ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_വരികൾ = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, (char_u *)"ALL") == 0)
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
	എങ്കിൽ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    ശൂന്യം
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ retval = -1;

    തിരഞ്ഞെടുപ്പ് (argvars[0].v_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_INSTR:
	    നിർത്തുക;

	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (argvars[0].vval.v_job != ശൂന്യ)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (argvars[0].vval.v_channel != ശൂന്യ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_FUNC:
	    എങ്കിൽ (argvars[0].vval.v_string != ശൂന്യ)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		എങ്കിൽ (fp != ശൂന്യ)
		    retval = fp->uf_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (argvars[0].vval.v_partial != ശൂന്യ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ (argvars[0].vval.v_blob != ശൂന്യ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (argvars[0].vval.v_list != ശൂന്യ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (argvars[0].vval.v_tuple != ശൂന്യ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ (argvars[0].vval.v_dict != ശൂന്യ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	    എങ്കിൽ (argvars[0].vval.v_class != ശൂന്യ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    എങ്കിൽ (argvars[0].vval.v_object != ശൂന്യ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    നിർത്തുക;
	അവസ്ഥ VAR_TYPEALIAS:
	    എങ്കിൽ (argvars[0].vval.v_typealias != ശൂന്യ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    നിർത്തുക;
    }

    rettv->v_type = VAR_സംഖ്യ;
    rettv->vval.v_സംഖ്യ = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    ശൂന്യം
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    എങ്കിൽ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    അല്ലെങ്കിൽ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    ശൂന്യം
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    ശൂന്യം
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    ശൂന്യം
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    ശൂന്യം
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ശൂന്യ;
}
#endif

    ശൂന്യം
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, ശൂന്യ);
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    ശൂന്യം
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = ശൂന്യ;
}
#endif

    ശൂന്യം
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, ശൂന്യ);
}

    ശൂന്യം
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ശൂന്യ;
}

    ശൂന്യം
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ശൂന്യ;
}

    ശൂന്യം
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
}

    ശൂന്യം
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, ശൂന്യ);
}

    ശൂന്യം
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    ശൂന്യം
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    ശൂന്യം
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_സംഖ്യ || argvars[1].v_type != VAR_സംഖ്യ)
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    mouse_row = (time_t)tv_get_സംഖ്യ(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_സംഖ്യ(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_drop_files(dict_T *args UNUSED)
{
# എങ്കിൽ defined(HAVE_DROP_FILE)
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    int_u	mods;
    char_u	**fnames;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "files")
	    || !dict_has_താക്കോൽ(args, "row")
	    || !dict_has_താക്കോൽ(args, "col")
	    || !dict_has_താക്കോൽ(args, "modifiers"))
	തിരികെ FALSE;

    (ശൂന്യം)dict_get_tv(args, "files", &t);
    row = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "row");
    col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "col");
    mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");

    എങ്കിൽ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	തിരികെ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    എങ്കിൽ (fnames == ശൂന്യ)
	തിരികെ FALSE;

    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	// ignore non-string items
	എങ്കിൽ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ശൂന്യ)
	    തുടരുക;

	fnames[എണ്ണം] = vim_strsave(li->li_tv.vval.v_string);
	എങ്കിൽ (fnames[എണ്ണം] == ശൂന്യ)
	{
	    ആകുമ്പോൾ (--എണ്ണം >= 0)
		vim_free(fnames[എണ്ണം]);
	    vim_free(fnames);
	    തിരികെ FALSE;
	}
	എണ്ണം++;
    }

    എങ്കിൽ (എണ്ണം > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, എണ്ണം);
    അല്ലെങ്കിൽ
	vim_free(fnames);
# endif

    തിരികെ TRUE;
}

# എങ്കിൽ defined(FIND_REPLACE_DIALOG)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_find_repl(dict_T *args)
{
    പൂർണ്ണസംഖ്യ		flags;
    char_u	*find_text;
    char_u	*repl_text;
    പൂർണ്ണസംഖ്യ		forward;
    പൂർണ്ണസംഖ്യ		retval;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "find_text")
	    || !dict_has_താക്കോൽ(args, "repl_text")
	    || !dict_has_താക്കോൽ(args, "flags")
	    || !dict_has_താക്കോൽ(args, "forward"))
	തിരികെ FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "flags");
    forward = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    തിരികെ retval;
}
# endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_mouse_event(dict_T *args)
{
    പൂർണ്ണസംഖ്യ		button;
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		repeated_click;
    int_u	mods;
    പൂർണ്ണസംഖ്യ		move;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "row")
	    || !dict_has_താക്കോൽ(args, "col"))
	തിരികെ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (പൂർണ്ണസംഖ്യ)dict_get_bool(args, "move", FALSE);

    എങ്കിൽ (!move && (!dict_has_താക്കോൽ(args, "button")
	    || !dict_has_താക്കോൽ(args, "multiclick")
	    || !dict_has_താക്കോൽ(args, "modifiers")))
	തിരികെ FALSE;

    row = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "row");
    col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "col");

    എങ്കിൽ (move)
    {
	പൂർണ്ണസംഖ്യ pY = row;
	പൂർണ്ണസംഖ്യ pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	എങ്കിൽ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    അല്ലെങ്കിൽ
    {
	button = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "button");
	repeated_click = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "multiclick");
	mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    തിരികെ TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	മൂല്യം;
    പൂർണ്ണസംഖ്യ		dragging;
    scrollbar_T *sb = ശൂന്യ;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "which")
	    || !dict_has_താക്കോൽ(args, "value")
	    || !dict_has_താക്കോൽ(args, "dragging"))
	തിരികെ FALSE;

    which = dict_get_string(args, "which", FALSE);
    മൂല്യം = (long)dict_get_സംഖ്യ(args, "value");
    dragging = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "dragging");

    എങ്കിൽ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ഇടത്];
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_വലത്];
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    എങ്കിൽ (sb == ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str), which);
	തിരികെ FALSE;
    }
    gui_drag_scrollbar(sb, മൂല്യം, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    തിരികെ TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    പൂർണ്ണസംഖ്യ		tabnr;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "tabnr"))
	തിരികെ FALSE;

    tabnr = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "tabnr");

    തിരികെ send_tabline_event(tabnr);
# അല്ലെങ്കിൽ
    തിരികെ FALSE;
# endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    പൂർണ്ണസംഖ്യ	tabnr;
    പൂർണ്ണസംഖ്യ	item;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "tabnr")
	    || !dict_has_താക്കോൽ(args, "item"))
	തിരികെ FALSE;

    tabnr = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "tabnr");
    item = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    തിരികെ TRUE;
}
#endif

    ശൂന്യം
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = FALSE;

    എങ്കിൽ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	തിരികെ;
    }

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ശൂന്യ)
	തിരികെ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_സംഖ്യ = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    ശൂന്യം
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = FALSE;

    എങ്കിൽ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	തിരികെ;
    }

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ശൂന്യ)
	തിരികെ;

    event = tv_get_string(&argvars[0]);
    എങ്കിൽ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_drop_files(argvars[1].vval.v_dict);
# എങ്കിൽ defined(FIND_REPLACE_DIALOG)
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_സംഖ്യ = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_mouse_event(argvars[1].vval.v_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_scrollbar(argvars[1].vval.v_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_tabline_event(argvars[1].vval.v_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_സംഖ്യ = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str), event);
	തിരികെ;
    }
#endif
}

    ശൂന്യം
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    time_for_testing = (time_t)tv_get_സംഖ്യ(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
