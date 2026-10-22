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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    நிலையான வெறுமை
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    எனில் (sname != வெற்று)
    {
	ga_concat(gap, sname);
	எனில் (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    எனில் (SOURCING_LNUM > 0)
    {
	எழுத்து    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, அளவு(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    எனில் (sname != வெற்று || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    நிலையான வெறுமை
ga_concat_esc(garray_T *gap, char_u *p, முழு clen)
{
    char_u  buf[NUMBUFLEN];

    எனில் (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	திரும்பு;
    }

    தேர்வு (*p)
    {
	நிலை BS: GA_CONCAT_LITERAL(gap, "\\b"); நிறுத்து;
	நிலை ESC: GA_CONCAT_LITERAL(gap, "\\e"); நிறுத்து;
	நிலை FF: GA_CONCAT_LITERAL(gap, "\\f"); நிறுத்து;
	நிலை NL: GA_CONCAT_LITERAL(gap, "\\n"); நிறுத்து;
	நிலை TAB: GA_CONCAT_LITERAL(gap, "\\t"); நிறுத்து;
	நிலை CAR: GA_CONCAT_LITERAL(gap, "\\r"); நிறுத்து;
	நிலை '\\': GA_CONCAT_LITERAL(gap, "\\\\"); நிறுத்து;
	இயல்பு:
	   எனில் (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((எழுத்து *)buf, அளவு(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   இல்லையெனில்
	       ga_append(gap, *p);
	   நிறுத்து;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    நிலையான வெறுமை
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    முழு	    c;
    முழு	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    முழு	    same_len;

    எனில் (str == வெற்று)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	திரும்பு;
    }

    க்கு (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	போது (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	எனில் (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((எழுத்து *)buf, அளவு(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	இல்லையெனில்
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    நிலையான வெறுமை
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_செய்தி_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    முழு		did_copy = FALSE;
    முழு		omitted = 0;

    எனில் (opt_செய்தி_tv->v_type != VAR_UNKNOWN
	    && !(opt_செய்தி_tv->v_type == VAR_STRING
		&& (opt_செய்தி_tv->vval.v_string == வெற்று
		    || *opt_செய்தி_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_செய்தி_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    எனில் (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    இல்லையெனில் எனில் (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    இல்லையெனில்
	GA_CONCAT_LITERAL(gap, "Expected ");
    எனில் (exp_str == வெற்று)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	எனில் (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != வெற்று && got_tv->vval.v_dict != வெற்று)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    முழு		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    எனில் (exp_tv->vval.v_dict == வெற்று || got_tv->vval.v_dict == வெற்று)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = வெற்று;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = வெற்று;
		திரும்பு;
	    }

	    todo = (முழு)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_பொருட்கள்(&exp_d->dv_hashtab, hi, todo)
	    {
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_சாவி, -1);
		    எனில் (item2 == வெற்று || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(எழுத்து *)hi->hi_சாவி, &HI2DI(hi)->di_tv);
			எனில் (item2 != வெற்று)
			    dict_add_tv(got_tv->vval.v_dict,
					    (எழுத்து *)hi->hi_சாவி, &item2->di_tv);
		    }
		    இல்லையெனில்
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (முழு)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_பொருட்கள்(&got_d->dv_hashtab, hi, todo)
	    {
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_சாவி, -1);
		    எனில் (item2 == வெற்று)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(எழுத்து *)hi->hi_சாவி, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    இல்லையெனில்
    {
	எனில் (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	எனில் (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    எனில் (atype != ASSERT_NOTEQUAL)
    {
	எனில் (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	இல்லையெனில் எனில் (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	இல்லையெனில்
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	எனில் (omitted != 0)
	{
	    எழுத்து    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, அளவு(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    எனில் (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    நிலையான முழு
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    எனில் (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], வெற்று, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	திரும்பு 1;
    }
    திரும்பு 0;
}

    நிலையான முழு
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    எனில் (pat != வெற்று && text != வெற்று
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], வெற்று, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	திரும்பு 1;
    }
    திரும்பு 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    நிலையான முழு
assert_bool(typval_T *argvars, முழு isTrue)
{
    முழு		error = FALSE;
    garray_T	ga;

    எனில் (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_எண் == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	திரும்பு 0;
    எனில் (argvars[0].v_type != VAR_எண்
	    || (tv_get_எண்_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		வெற்று, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	திரும்பு 1;
    }
    திரும்பு 0;
}

    நிலையான வெறுமை
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    எனில் (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    இல்லையெனில்
	ga_concat(gap, cmd);
}

    நிலையான முழு
assert_beeps(typval_T *argvars, முழு no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    முழு		ret = 0;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    எனில் (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	எனில் (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	இல்லையெனில்
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    திரும்பு ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    வெறுமை
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    வெறுமை
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    வெறுமை
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_equal_common(argvars, ASSERT_EQUAL);
}

    நிலையான முழு
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    எழுத்து	line1[200];
    எழுத்து	line2[200];
    முழு		lineidx = 0;
    size_t	IObufflen;

    எனில் (fname1 == வெற்று || fname2 == வெற்று)
	திரும்பு 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((எழுத்து *)fname1, READBIN);
    எனில் (fd1 == வெற்று)
    {
	IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE,
	    (எழுத்து *)e_cant_read_file_str, fname1);
    }
    இல்லையெனில்
    {
	fd2 = mch_fopen((எழுத்து *)fname2, READBIN);
	எனில் (fd2 == வெற்று)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE,
		(எழுத்து *)e_cant_read_file_str, fname2);
	}
	இல்லையெனில்
	{
	    முழு	    c1, c2;
	    long    மொத்தம் = 0;
	    long    linecount = 1;

	    க்கு (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		எனில் (c1 == EOF)
		{
		    எனில் (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    நிறுத்து;
		}
		இல்லையெனில் எனில் (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    நிறுத்து;
		}
		இல்லையெனில்
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    எனில் (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", மொத்தம், linecount);
			நிறுத்து;
		    }
		}
		++மொத்தம்;
		எனில் (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		இல்லையெனில் எனில் (lineidx + 2 == (முழு)அளவு(line1))
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

    எனில் (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	எனில் (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    எனில் (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	திரும்பு 1;
    }

    திரும்பு 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    வெறுமை
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    rettv->vval.v_எண் = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    வெறுமை
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    வெறுமை
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    error = tv_get_string_chk(&argvars[0]);
    எனில் (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_எண் = 1;
    }
    இல்லையெனில் எனில் (error != வெற்று
	&& strstr((எழுத்து *)get_vim_var_str(VV_EXCEPTION), (எழுத்து *)error) == வெற்று)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], வெற்று, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_எண் = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    வெறுமை
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    முழு		save_trylevel = trylevel;
    முழு		called_emsg_before = called_emsg;
    எழுத்து	*wrong_arg_செய்தி = வெற்று;
    char_u	*tofree = வெற்று;

    எனில் (check_for_string_or_எண்_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_எண்_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	திரும்பு;

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

    எனில் (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_எண் = 1;
    }
    இல்லையெனில் எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = வெற்று;
	முழு	error_found = FALSE;
	முழு	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_செய்தி == வெற்று ? (char_u *)"[unknown]"
						       : emsg_assert_fails_செய்தி;

	எனில் (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == வெற்று
			   || strstr((எழுத்து *)actual, (எழுத்து *)expected) == வெற்று;
	}
	இல்லையெனில் எனில் (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    எனில் (list == வெற்று || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_செய்தி = e_assert_fails_second_arg;
		செல் theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    எனில் (expected == வெற்று)
		செல் theend;
	    எனில் (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    இல்லையெனில் எனில் (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		எனில் (actual != வெற்று)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    எனில் (expected == வெற்று)
			செல் theend;
		    எனில் (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	இல்லையெனில்
	{
	    wrong_arg_செய்தி = e_assert_fails_second_arg;
	    செல் theend;
	}

	எனில் (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    எனில் (argvars[3].v_type != VAR_எண்)
	    {
		wrong_arg_செய்தி = e_assert_fails_fourth_argument;
		செல் theend;
	    }
	    இல்லையெனில் எனில் (argvars[3].vval.v_எண் >= 0
			 && argvars[3].vval.v_எண் != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    எனில் (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		எனில் (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_செய்தி = e_assert_fails_fifth_argument;
		    செல் theend;
		}
		இல்லையெனில் எனில் (argvars[4].vval.v_string != வெற்று
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	எனில் (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    எனில் (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_எண்;
		actual_tv.vval.v_எண் = emsg_assert_fails_lnum;
	    }
	    இல்லையெனில் எனில் (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    இல்லையெனில்
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
	    rettv->vval.v_எண் = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    செய்தி_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    செய்தி_scrolled = 0;
    வரிகள்_இடது = Rows;
    VIM_CLEAR(emsg_assert_fails_செய்தி);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, வெற்று, 0);
    எனில் (wrong_arg_செய்தி != வெற்று)
	emsg(_(wrong_arg_செய்தி));
}

/*
 * "assert_false(actual[, msg])" function
 */
    வெறுமை
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_bool(argvars, FALSE);
}

    நிலையான முழு
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    முழு		error = FALSE;
    char_u	expected_str[200];

    எனில் (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	எனில் (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((எழுத்து *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, வெற்று,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    திரும்பு 1;
	}
    }
    இல்லையெனில்
    {
	varnumber_T	lower = tv_get_எண்_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_எண்_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_எண்_chk(&argvars[2], &error);

	எனில் (error)
	    திரும்பு 0;
	எனில் (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((எழுத்து *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, வெற்று,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    திரும்பு 1;
	}
    }
    திரும்பு 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    வெறுமை
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    வெறுமை
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    வெறுமை
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    வெறுமை
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_எண் = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    வெறுமை
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    வெறுமை
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_எண்
	    || argvars[0].vval.v_எண் <= 0
	    || argvars[1].v_type != VAR_எண்
	    || argvars[1].vval.v_எண் < 0
	    || argvars[2].v_type != VAR_எண்)
	emsg(_(e_invalid_argument));
    இல்லையெனில்
    {
	alloc_fail_id = argvars[0].vval.v_எண்;
	எனில் (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_எண்;
	alloc_fail_repeat = argvars[2].vval.v_எண்;
	did_outofmem_செய்தி = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    வெறுமை
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#எனில் defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    வெறுமை
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_உள்ளீடு_BUF
    char_u	*val;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    எனில் (!gui.in_use)
	திரும்பு;
# endif

    எனில் (val != வெற்று)
    {
	trash_உள்ளீடு_buf();
	add_to_உள்ளீடு_buf_csi(val, (முழு)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    வெறுமை
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    name = tv_get_string(&argvars[0]);

    எனில் (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_எண் = need_fileinfo;
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    வெறுமை
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    வெறுமை
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    முழு     val;
    நிலையான முழு save_starting = -1;

    எனில் (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL)
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    val = (முழு)tv_get_எண்(&argvars[1]);

    எனில் (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"starting") == 0)
    {
	எனில் (val)
	{
	    எனில் (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	இல்லையெனில்
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_வரிகள் = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    இல்லையெனில் எனில் (STRCMP(name, (char_u *)"ALL") == 0)
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
	எனில் (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    வெறுமை
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    முழு retval = -1;

    தேர்வு (argvars[0].v_type)
    {
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_எண்:
	நிலை VAR_BOOL:
	நிலை VAR_FLOAT:
	நிலை VAR_SPECIAL:
	நிலை VAR_STRING:
	நிலை VAR_INSTR:
	    நிறுத்து;

	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    எனில் (argvars[0].vval.v_job != வெற்று)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    நிறுத்து;
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    எனில் (argvars[0].vval.v_channel != வெற்று)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    நிறுத்து;
	நிலை VAR_FUNC:
	    எனில் (argvars[0].vval.v_string != வெற்று)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		எனில் (fp != வெற்று)
		    retval = fp->uf_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    எனில் (argvars[0].vval.v_partial != வெற்று)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    நிறுத்து;
	நிலை VAR_BLOB:
	    எனில் (argvars[0].vval.v_blob != வெற்று)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    நிறுத்து;
	நிலை VAR_LIST:
	    எனில் (argvars[0].vval.v_list != வெற்று)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    எனில் (argvars[0].vval.v_tuple != வெற்று)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் (argvars[0].vval.v_dict != வெற்று)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    நிறுத்து;
	நிலை VAR_CLASS:
	    எனில் (argvars[0].vval.v_class != வெற்று)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    எனில் (argvars[0].vval.v_object != வெற்று)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    நிறுத்து;
	நிலை VAR_TYPEALIAS:
	    எனில் (argvars[0].vval.v_typealias != வெற்று)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    நிறுத்து;
    }

    rettv->v_type = VAR_எண்;
    rettv->vval.v_எண் = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    வெறுமை
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    எனில் (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    இல்லையெனில்
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    வெறுமை
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    வெறுமை
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    வெறுமை
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = வெற்று;
}

#எனில் defined(FEAT_JOB_CHANNEL)
    வெறுமை
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = வெற்று;
}
#endif

    வெறுமை
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, வெற்று);
}

#எனில் defined(FEAT_JOB_CHANNEL)
    வெறுமை
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = வெற்று;
}
#endif

    வெறுமை
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, வெற்று);
}

    வெறுமை
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = வெற்று;
}

    வெறுமை
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = வெற்று;
}

    வெறுமை
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
}

    வெறுமை
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, வெற்று);
}

    வெறுமை
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    வெறுமை
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    வெறுமை
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_எண் || argvars[1].v_type != VAR_எண்)
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    mouse_row = (time_t)tv_get_எண்(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_எண்(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    நிலையான முழு
test_gui_drop_files(dict_T *args UNUSED)
{
# எனில் defined(HAVE_DROP_FILE)
    முழு		row;
    முழு		col;
    int_u	mods;
    char_u	**fnames;
    முழு		மொத்தம் = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    எனில் (!dict_has_சாவி(args, "files")
	    || !dict_has_சாவி(args, "row")
	    || !dict_has_சாவி(args, "col")
	    || !dict_has_சாவி(args, "modifiers"))
	திரும்பு FALSE;

    (வெறுமை)dict_get_tv(args, "files", &t);
    row = (முழு)dict_get_எண்(args, "row");
    col = (முழு)dict_get_எண்(args, "col");
    mods = (முழு)dict_get_எண்(args, "modifiers");

    எனில் (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	திரும்பு FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    எனில் (fnames == வெற்று)
	திரும்பு FALSE;

    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	// ignore non-string items
	எனில் (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == வெற்று)
	    தொடர்;

	fnames[மொத்தம்] = vim_strsave(li->li_tv.vval.v_string);
	எனில் (fnames[மொத்தம்] == வெற்று)
	{
	    போது (--மொத்தம் >= 0)
		vim_free(fnames[மொத்தம்]);
	    vim_free(fnames);
	    திரும்பு FALSE;
	}
	மொத்தம்++;
    }

    எனில் (மொத்தம் > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, மொத்தம்);
    இல்லையெனில்
	vim_free(fnames);
# endif

    திரும்பு TRUE;
}

# எனில் defined(FIND_REPLACE_DIALOG)
    நிலையான முழு
test_gui_find_repl(dict_T *args)
{
    முழு		flags;
    char_u	*find_text;
    char_u	*repl_text;
    முழு		forward;
    முழு		retval;

    எனில் (!dict_has_சாவி(args, "find_text")
	    || !dict_has_சாவி(args, "repl_text")
	    || !dict_has_சாவி(args, "flags")
	    || !dict_has_சாவி(args, "forward"))
	திரும்பு FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (முழு)dict_get_எண்(args, "flags");
    forward = (முழு)dict_get_எண்(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    திரும்பு retval;
}
# endif

    நிலையான முழு
test_gui_mouse_event(dict_T *args)
{
    முழு		button;
    முழு		row;
    முழு		col;
    முழு		repeated_click;
    int_u	mods;
    முழு		move;

    எனில் (!dict_has_சாவி(args, "row")
	    || !dict_has_சாவி(args, "col"))
	திரும்பு FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (முழு)dict_get_bool(args, "move", FALSE);

    எனில் (!move && (!dict_has_சாவி(args, "button")
	    || !dict_has_சாவி(args, "multiclick")
	    || !dict_has_சாவி(args, "modifiers")))
	திரும்பு FALSE;

    row = (முழு)dict_get_எண்(args, "row");
    col = (முழு)dict_get_எண்(args, "col");

    எனில் (move)
    {
	முழு pY = row;
	முழு pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	எனில் (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    இல்லையெனில்
    {
	button = (முழு)dict_get_எண்(args, "button");
	repeated_click = (முழு)dict_get_எண்(args, "multiclick");
	mods = (முழு)dict_get_எண்(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    திரும்பு TRUE;
}

    நிலையான முழு
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	மதிப்பு;
    முழு		dragging;
    scrollbar_T *sb = வெற்று;

    எனில் (!dict_has_சாவி(args, "which")
	    || !dict_has_சாவி(args, "value")
	    || !dict_has_சாவி(args, "dragging"))
	திரும்பு FALSE;

    which = dict_get_string(args, "which", FALSE);
    மதிப்பு = (long)dict_get_எண்(args, "value");
    dragging = (முழு)dict_get_எண்(args, "dragging");

    எனில் (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_இடது];
    இல்லையெனில் எனில் (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_வலது];
    இல்லையெனில் எனில் (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    எனில் (sb == வெற்று)
    {
	semsg(_(e_invalid_argument_str), which);
	திரும்பு FALSE;
    }
    gui_drag_scrollbar(sb, மதிப்பு, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    திரும்பு TRUE;
}

    நிலையான முழு
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    முழு		tabnr;

    எனில் (!dict_has_சாவி(args, "tabnr"))
	திரும்பு FALSE;

    tabnr = (முழு)dict_get_எண்(args, "tabnr");

    திரும்பு send_tabline_event(tabnr);
# இல்லையெனில்
    திரும்பு FALSE;
# endif
}

    நிலையான முழு
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    முழு	tabnr;
    முழு	item;

    எனில் (!dict_has_சாவி(args, "tabnr")
	    || !dict_has_சாவி(args, "item"))
	திரும்பு FALSE;

    tabnr = (முழு)dict_get_எண்(args, "tabnr");
    item = (முழு)dict_get_எண்(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    திரும்பு TRUE;
}
#endif

    வெறுமை
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = FALSE;

    எனில் (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	திரும்பு;
    }

    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == வெற்று)
	திரும்பு;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_எண் = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    வெறுமை
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = FALSE;

    எனில் (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	திரும்பு;
    }

    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == வெற்று)
	திரும்பு;

    event = tv_get_string(&argvars[0]);
    எனில் (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_எண் = test_gui_drop_files(argvars[1].vval.v_dict);
# எனில் defined(FIND_REPLACE_DIALOG)
    இல்லையெனில் எனில் (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_எண் = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    இல்லையெனில் எனில் (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_எண் = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    இல்லையெனில் எனில் (STRCMP(event, "mouse") == 0)
	rettv->vval.v_எண் = test_gui_mouse_event(argvars[1].vval.v_dict);
    இல்லையெனில் எனில் (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_எண் = test_gui_scrollbar(argvars[1].vval.v_dict);
    இல்லையெனில் எனில் (STRCMP(event, "tabline") == 0)
	rettv->vval.v_எண் = test_gui_tabline_event(argvars[1].vval.v_dict);
    இல்லையெனில் எனில் (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_எண் = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str), event);
	திரும்பு;
    }
#endif
}

    வெறுமை
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    time_for_testing = (time_t)tv_get_எண்(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
