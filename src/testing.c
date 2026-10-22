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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    স্থির শূন্য
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    যদি (sname != রিক্ত)
    {
	ga_concat(gap, sname);
	যদি (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    যদি (SOURCING_LNUM > 0)
    {
	ৱর্ণ    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, পরিমাণ(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    যদি (sname != রিক্ত || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    স্থির শূন্য
ga_concat_esc(garray_T *gap, char_u *p, পূর্ণাঙ্ক clen)
{
    char_u  buf[NUMBUFLEN];

    যদি (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ঘূৰাই;
    }

    ৱিকল্প (*p)
    {
	পক্ষ BS: GA_CONCAT_LITERAL(gap, "\\b"); ভঙ্গ;
	পক্ষ ESC: GA_CONCAT_LITERAL(gap, "\\e"); ভঙ্গ;
	পক্ষ FF: GA_CONCAT_LITERAL(gap, "\\f"); ভঙ্গ;
	পক্ষ NL: GA_CONCAT_LITERAL(gap, "\\n"); ভঙ্গ;
	পক্ষ TAB: GA_CONCAT_LITERAL(gap, "\\t"); ভঙ্গ;
	পক্ষ CAR: GA_CONCAT_LITERAL(gap, "\\r"); ভঙ্গ;
	পক্ষ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ভঙ্গ;
	স্ৱভাৱ:
	   যদি (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((ৱর্ণ *)buf, পরিমাণ(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   অন্যথা
	       ga_append(gap, *p);
	   ভঙ্গ;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    স্থির শূন্য
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    পূর্ণাঙ্ক	    c;
    পূর্ণাঙ্ক	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    পূর্ণাঙ্ক	    same_len;

    যদি (str == রিক্ত)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ঘূৰাই;
    }

    বাবে (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	যেতিয়ালৈ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	যদি (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((ৱর্ণ *)buf, পরিমাণ(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	অন্যথা
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    স্থির শূন্য
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_বাৰ্তা_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    পূর্ণাঙ্ক		did_copy = FALSE;
    পূর্ণাঙ্ক		omitted = 0;

    যদি (opt_বাৰ্তা_tv->v_type != VAR_UNKNOWN
	    && !(opt_বাৰ্তা_tv->v_type == VAR_STRING
		&& (opt_বাৰ্তা_tv->vval.v_string == রিক্ত
		    || *opt_বাৰ্তা_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_বাৰ্তা_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    যদি (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    অন্যথা যদি (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    অন্যথা
	GA_CONCAT_LITERAL(gap, "Expected ");
    যদি (exp_str == রিক্ত)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	যদি (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != রিক্ত && got_tv->vval.v_dict != রিক্ত)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    পূর্ণাঙ্ক		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    যদি (exp_tv->vval.v_dict == রিক্ত || got_tv->vval.v_dict == রিক্ত)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = রিক্ত;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = রিক্ত;
		ঘূৰাই;
	    }

	    todo = (পূর্ণাঙ্ক)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(&exp_d->dv_hashtab, hi, todo)
	    {
		যদি (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_চাবি, -1);
		    যদি (item2 == রিক্ত || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(ৱর্ণ *)hi->hi_চাবি, &HI2DI(hi)->di_tv);
			যদি (item2 != রিক্ত)
			    dict_add_tv(got_tv->vval.v_dict,
					    (ৱর্ণ *)hi->hi_চাবি, &item2->di_tv);
		    }
		    অন্যথা
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (পূর্ণাঙ্ক)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(&got_d->dv_hashtab, hi, todo)
	    {
		যদি (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_চাবি, -1);
		    যদি (item2 == রিক্ত)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(ৱর্ণ *)hi->hi_চাবি, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    অন্যথা
    {
	যদি (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	যদি (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    যদি (atype != ASSERT_NOTEQUAL)
    {
	যদি (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	অন্যথা যদি (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	অন্যথা
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	যদি (omitted != 0)
	{
	    ৱর্ণ    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, পরিমাণ(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    যদি (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    স্থির পূর্ণাঙ্ক
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    যদি (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], রিক্ত, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

    স্থির পূর্ণাঙ্ক
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ঘূৰাই 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    যদি (pat != রিক্ত && text != রিক্ত
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], রিক্ত, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    স্থির পূর্ণাঙ্ক
assert_bool(typval_T *argvars, পূর্ণাঙ্ক isTrue)
{
    পূর্ণাঙ্ক		error = FALSE;
    garray_T	ga;

    যদি (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_সংখ্যা == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ঘূৰাই 0;
    যদি (argvars[0].v_type != VAR_সংখ্যা
	    || (tv_get_সংখ্যা_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		রিক্ত, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

    স্থির শূন্য
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    যদি (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    অন্যথা
	ga_concat(gap, cmd);
}

    স্থির পূর্ণাঙ্ক
assert_beeps(typval_T *argvars, পূর্ণাঙ্ক no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    পূর্ণাঙ্ক		ret = 0;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    যদি (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	যদি (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	অন্যথা
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ঘূৰাই ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    শূন্য
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    শূন্য
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    শূন্য
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_equal_common(argvars, ASSERT_EQUAL);
}

    স্থির পূর্ণাঙ্ক
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    ৱর্ণ	line1[200];
    ৱর্ণ	line2[200];
    পূর্ণাঙ্ক		lineidx = 0;
    size_t	IObufflen;

    যদি (fname1 == রিক্ত || fname2 == রিক্ত)
	ঘূৰাই 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((ৱর্ণ *)fname1, READBIN);
    যদি (fd1 == রিক্ত)
    {
	IObufflen = vim_snprintf_safelen((ৱর্ণ *)IObuff, IOSIZE,
	    (ৱর্ণ *)e_cant_read_file_str, fname1);
    }
    অন্যথা
    {
	fd2 = mch_fopen((ৱর্ণ *)fname2, READBIN);
	যদি (fd2 == রিক্ত)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((ৱর্ণ *)IObuff, IOSIZE,
		(ৱর্ণ *)e_cant_read_file_str, fname2);
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক	    c1, c2;
	    long    গুনতি = 0;
	    long    linecount = 1;

	    বাবে (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		যদি (c1 == EOF)
		{
		    যদি (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ভঙ্গ;
		}
		অন্যথা যদি (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ভঙ্গ;
		}
		অন্যথা
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    যদি (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((ৱর্ণ *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", গুনতি, linecount);
			ভঙ্গ;
		    }
		}
		++গুনতি;
		যদি (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		অন্যথা যদি (lineidx + 2 == (পূর্ণাঙ্ক)পরিমাণ(line1))
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

    যদি (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	যদি (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    যদি (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ঘূৰাই 1;
    }

    ঘূৰাই 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    শূন্য
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    শূন্য
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    শূন্য
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    error = tv_get_string_chk(&argvars[0]);
    যদি (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_সংখ্যা = 1;
    }
    অন্যথা যদি (error != রিক্ত
	&& strstr((ৱর্ণ *)get_vim_var_str(VV_EXCEPTION), (ৱর্ণ *)error) == রিক্ত)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], রিক্ত, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_সংখ্যা = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    শূন্য
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    পূর্ণাঙ্ক		save_trylevel = trylevel;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    ৱর্ণ	*wrong_arg_বাৰ্তা = রিক্ত;
    char_u	*tofree = রিক্ত;

    যদি (check_for_string_or_সংখ্যা_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ঘূৰাই;

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

    যদি (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_সংখ্যা = 1;
    }
    অন্যথা যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = রিক্ত;
	পূর্ণাঙ্ক	error_found = FALSE;
	পূর্ণাঙ্ক	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_বাৰ্তা == রিক্ত ? (char_u *)"[unknown]"
						       : emsg_assert_fails_বাৰ্তা;

	যদি (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == রিক্ত
			   || strstr((ৱর্ণ *)actual, (ৱর্ণ *)expected) == রিক্ত;
	}
	অন্যথা যদি (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    যদি (list == রিক্ত || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_বাৰ্তা = e_assert_fails_second_arg;
		গচ্ছ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    যদি (expected == রিক্ত)
		গচ্ছ theend;
	    যদি (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    অন্যথা যদি (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		যদি (actual != রিক্ত)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    যদি (expected == রিক্ত)
			গচ্ছ theend;
		    যদি (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	অন্যথা
	{
	    wrong_arg_বাৰ্তা = e_assert_fails_second_arg;
	    গচ্ছ theend;
	}

	যদি (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    যদি (argvars[3].v_type != VAR_সংখ্যা)
	    {
		wrong_arg_বাৰ্তা = e_assert_fails_fourth_argument;
		গচ্ছ theend;
	    }
	    অন্যথা যদি (argvars[3].vval.v_সংখ্যা >= 0
			 && argvars[3].vval.v_সংখ্যা != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    যদি (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		যদি (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_বাৰ্তা = e_assert_fails_fifth_argument;
		    গচ্ছ theend;
		}
		অন্যথা যদি (argvars[4].vval.v_string != রিক্ত
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	যদি (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    যদি (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_সংখ্যা;
		actual_tv.vval.v_সংখ্যা = emsg_assert_fails_lnum;
	    }
	    অন্যথা যদি (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    অন্যথা
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
	    rettv->vval.v_সংখ্যা = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    বাৰ্তা_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    বাৰ্তা_scrolled = 0;
    সাৰিগুলি_বাম = Rows;
    VIM_CLEAR(emsg_assert_fails_বাৰ্তা);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, রিক্ত, 0);
    যদি (wrong_arg_বাৰ্তা != রিক্ত)
	emsg(_(wrong_arg_বাৰ্তা));
}

/*
 * "assert_false(actual[, msg])" function
 */
    শূন্য
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_bool(argvars, FALSE);
}

    স্থির পূর্ণাঙ্ক
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    পূর্ণাঙ্ক		error = FALSE;
    char_u	expected_str[200];

    যদি (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	যদি (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ৱর্ণ *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, রিক্ত,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ঘূৰাই 1;
	}
    }
    অন্যথা
    {
	varnumber_T	lower = tv_get_সংখ্যা_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_সংখ্যা_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_সংখ্যা_chk(&argvars[2], &error);

	যদি (error)
	    ঘূৰাই 0;
	যদি (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ৱর্ণ *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, রিক্ত,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ঘূৰাই 1;
	}
    }
    ঘূৰাই 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    শূন্য
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    যদি (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    শূন্য
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    শূন্য
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    শূন্য
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_সংখ্যা = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    শূন্য
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    শূন্য
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_সংখ্যা
	    || argvars[0].vval.v_সংখ্যা <= 0
	    || argvars[1].v_type != VAR_সংখ্যা
	    || argvars[1].vval.v_সংখ্যা < 0
	    || argvars[2].v_type != VAR_সংখ্যা)
	emsg(_(e_invalid_argument));
    অন্যথা
    {
	alloc_fail_id = argvars[0].vval.v_সংখ্যা;
	যদি (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_সংখ্যা;
	alloc_fail_repeat = argvars[2].vval.v_সংখ্যা;
	did_outofmem_বাৰ্তা = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    শূন্য
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#যদি defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    শূন্য
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_প্ৰবেশ_BUF
    char_u	*val;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    যদি (!gui.in_use)
	ঘূৰাই;
# endif

    যদি (val != রিক্ত)
    {
	trash_প্ৰবেশ_buf();
	add_to_প্ৰবেশ_buf_csi(val, (পূর্ণাঙ্ক)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    শূন্য
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);

    যদি (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_সংখ্যা = need_fileinfo;
    অন্যথা
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    শূন্য
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    যদি (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    শূন্য
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    পূর্ণাঙ্ক     val;
    স্থির পূর্ণাঙ্ক save_starting = -1;

    যদি (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    val = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]);

    যদি (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"starting") == 0)
    {
	যদি (val)
	{
	    যদি (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	অন্যথা
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    অন্যথা যদি (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_সাৰিগুলি = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    অন্যথা যদি (STRCMP(name, (char_u *)"ALL") == 0)
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
	যদি (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    অন্যথা
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    শূন্য
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক retval = -1;

    ৱিকল্প (argvars[0].v_type)
    {
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_STRING:
	পক্ষ VAR_INSTR:
	    ভঙ্গ;

	পক্ষ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    যদি (argvars[0].vval.v_job != রিক্ত)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    ভঙ্গ;
	পক্ষ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    যদি (argvars[0].vval.v_channel != রিক্ত)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    ভঙ্গ;
	পক্ষ VAR_FUNC:
	    যদি (argvars[0].vval.v_string != রিক্ত)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		যদি (fp != রিক্ত)
		    retval = fp->uf_refcount;
	    }
	    ভঙ্গ;
	পক্ষ VAR_PARTIAL:
	    যদি (argvars[0].vval.v_partial != রিক্ত)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_BLOB:
	    যদি (argvars[0].vval.v_blob != রিক্ত)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_LIST:
	    যদি (argvars[0].vval.v_list != রিক্ত)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    যদি (argvars[0].vval.v_tuple != রিক্ত)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_DICT:
	    যদি (argvars[0].vval.v_dict != রিক্ত)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_CLASS:
	    যদি (argvars[0].vval.v_class != রিক্ত)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_OBJECT:
	    যদি (argvars[0].vval.v_object != রিক্ত)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ভঙ্গ;
	পক্ষ VAR_TYPEALIAS:
	    যদি (argvars[0].vval.v_typealias != রিক্ত)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ভঙ্গ;
    }

    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    শূন্য
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    যদি (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    অন্যথা
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    শূন্য
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    শূন্য
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    শূন্য
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = রিক্ত;
}

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = রিক্ত;
}
#endif

    শূন্য
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, রিক্ত);
}

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = রিক্ত;
}
#endif

    শূন্য
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, রিক্ত);
}

    শূন্য
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = রিক্ত;
}

    শূন্য
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = রিক্ত;
}

    শূন্য
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
}

    শূন্য
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, রিক্ত);
}

    শূন্য
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    শূন্য
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    শূন্য
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_সংখ্যা || argvars[1].v_type != VAR_সংখ্যা)
    {
	emsg(_(e_invalid_argument));
	ঘূৰাই;
    }

    mouse_row = (time_t)tv_get_সংখ্যা(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_সংখ্যা(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    স্থির পূর্ণাঙ্ক
test_gui_drop_files(dict_T *args UNUSED)
{
# যদি defined(HAVE_DROP_FILE)
    পূর্ণাঙ্ক		row;
    পূর্ণাঙ্ক		col;
    int_u	mods;
    char_u	**fnames;
    পূর্ণাঙ্ক		গুনতি = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    যদি (!dict_has_চাবি(args, "files")
	    || !dict_has_চাবি(args, "row")
	    || !dict_has_চাবি(args, "col")
	    || !dict_has_চাবি(args, "modifiers"))
	ঘূৰাই FALSE;

    (শূন্য)dict_get_tv(args, "files", &t);
    row = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "row");
    col = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "col");
    mods = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "modifiers");

    যদি (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ঘূৰাই FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    যদি (fnames == রিক্ত)
	ঘূৰাই FALSE;

    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	// ignore non-string items
	যদি (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == রিক্ত)
	    অনুৱর্ত;

	fnames[গুনতি] = vim_strsave(li->li_tv.vval.v_string);
	যদি (fnames[গুনতি] == রিক্ত)
	{
	    যেতিয়ালৈ (--গুনতি >= 0)
		vim_free(fnames[গুনতি]);
	    vim_free(fnames);
	    ঘূৰাই FALSE;
	}
	গুনতি++;
    }

    যদি (গুনতি > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, গুনতি);
    অন্যথা
	vim_free(fnames);
# endif

    ঘূৰাই TRUE;
}

# যদি defined(FIND_REPLACE_DIALOG)
    স্থির পূর্ণাঙ্ক
test_gui_find_repl(dict_T *args)
{
    পূর্ণাঙ্ক		flags;
    char_u	*find_text;
    char_u	*repl_text;
    পূর্ণাঙ্ক		forward;
    পূর্ণাঙ্ক		retval;

    যদি (!dict_has_চাবি(args, "find_text")
	    || !dict_has_চাবি(args, "repl_text")
	    || !dict_has_চাবি(args, "flags")
	    || !dict_has_চাবি(args, "forward"))
	ঘূৰাই FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "flags");
    forward = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    ঘূৰাই retval;
}
# endif

    স্থির পূর্ণাঙ্ক
test_gui_mouse_event(dict_T *args)
{
    পূর্ণাঙ্ক		button;
    পূর্ণাঙ্ক		row;
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		repeated_click;
    int_u	mods;
    পূর্ণাঙ্ক		move;

    যদি (!dict_has_চাবি(args, "row")
	    || !dict_has_চাবি(args, "col"))
	ঘূৰাই FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (পূর্ণাঙ্ক)dict_get_bool(args, "move", FALSE);

    যদি (!move && (!dict_has_চাবি(args, "button")
	    || !dict_has_চাবি(args, "multiclick")
	    || !dict_has_চাবি(args, "modifiers")))
	ঘূৰাই FALSE;

    row = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "row");
    col = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "col");

    যদি (move)
    {
	পূর্ণাঙ্ক pY = row;
	পূর্ণাঙ্ক pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	যদি (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    অন্যথা
    {
	button = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "button");
	repeated_click = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "multiclick");
	mods = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    ঘূৰাই TRUE;
}

    স্থির পূর্ণাঙ্ক
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	মান;
    পূর্ণাঙ্ক		dragging;
    scrollbar_T *sb = রিক্ত;

    যদি (!dict_has_চাবি(args, "which")
	    || !dict_has_চাবি(args, "value")
	    || !dict_has_চাবি(args, "dragging"))
	ঘূৰাই FALSE;

    which = dict_get_string(args, "which", FALSE);
    মান = (long)dict_get_সংখ্যা(args, "value");
    dragging = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "dragging");

    যদি (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_বাম];
    অন্যথা যদি (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ডান];
    অন্যথা যদি (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    যদি (sb == রিক্ত)
    {
	semsg(_(e_invalid_argument_str), which);
	ঘূৰাই FALSE;
    }
    gui_drag_scrollbar(sb, মান, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    ঘূৰাই TRUE;
}

    স্থির পূর্ণাঙ্ক
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    পূর্ণাঙ্ক		tabnr;

    যদি (!dict_has_চাবি(args, "tabnr"))
	ঘূৰাই FALSE;

    tabnr = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "tabnr");

    ঘূৰাই send_tabline_event(tabnr);
# অন্যথা
    ঘূৰাই FALSE;
# endif
}

    স্থির পূর্ণাঙ্ক
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    পূর্ণাঙ্ক	tabnr;
    পূর্ণাঙ্ক	item;

    যদি (!dict_has_চাবি(args, "tabnr")
	    || !dict_has_চাবি(args, "item"))
	ঘূৰাই FALSE;

    tabnr = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "tabnr");
    item = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    ঘূৰাই TRUE;
}
#endif

    শূন্য
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = FALSE;

    যদি (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ঘূৰাই;
    }

    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == রিক্ত)
	ঘূৰাই;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_সংখ্যা = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    শূন্য
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = FALSE;

    যদি (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ঘূৰাই;
    }

    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == রিক্ত)
	ঘূৰাই;

    event = tv_get_string(&argvars[0]);
    যদি (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_সংখ্যা = test_gui_drop_files(argvars[1].vval.v_dict);
# যদি defined(FIND_REPLACE_DIALOG)
    অন্যথা যদি (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_সংখ্যা = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    অন্যথা যদি (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_সংখ্যা = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    অন্যথা যদি (STRCMP(event, "mouse") == 0)
	rettv->vval.v_সংখ্যা = test_gui_mouse_event(argvars[1].vval.v_dict);
    অন্যথা যদি (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_সংখ্যা = test_gui_scrollbar(argvars[1].vval.v_dict);
    অন্যথা যদি (STRCMP(event, "tabline") == 0)
	rettv->vval.v_সংখ্যা = test_gui_tabline_event(argvars[1].vval.v_dict);
    অন্যথা যদি (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_সংখ্যা = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), event);
	ঘূৰাই;
    }
#endif
}

    শূন্য
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    time_for_testing = (time_t)tv_get_সংখ্যা(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
