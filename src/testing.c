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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ଯଦି (sname != ଖାଲି)
    {
	ga_concat(gap, sname);
	ଯଦି (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ଯଦି (SOURCING_LNUM > 0)
    {
	ଅକ୍ଷର    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, ଆକାର(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ଯଦି (sname != ଖାଲି || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ga_concat_esc(garray_T *gap, char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen)
{
    char_u  buf[NUMBUFLEN];

    ଯଦି (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ଫେରନ୍ତୁ;
    }

    ବାଛ (*p)
    {
	ସ୍ଥିତି BS: GA_CONCAT_LITERAL(gap, "\\b"); ଭାଙ୍ଗ;
	ସ୍ଥିତି ESC: GA_CONCAT_LITERAL(gap, "\\e"); ଭାଙ୍ଗ;
	ସ୍ଥିତି FF: GA_CONCAT_LITERAL(gap, "\\f"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NL: GA_CONCAT_LITERAL(gap, "\\n"); ଭାଙ୍ଗ;
	ସ୍ଥିତି TAB: GA_CONCAT_LITERAL(gap, "\\t"); ଭାଙ୍ଗ;
	ସ୍ଥିତି CAR: GA_CONCAT_LITERAL(gap, "\\r"); ଭାଙ୍ଗ;
	ସ୍ଥିତି '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	   ଯଦି (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, ଆକାର(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ଅନ୍ୟଥା
	       ga_append(gap, *p);
	   ଭାଙ୍ଗ;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    same_len;

    ଯଦି (str == ଖାଲି)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ଯେତେବେଳେ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ଯଦି (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, ଆକାର(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	ଅନ୍ୟଥା
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_ସନ୍ଦେଶ_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_copy = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		omitted = 0;

    ଯଦି (opt_ସନ୍ଦେଶ_tv->v_type != VAR_UNKNOWN
	    && !(opt_ସନ୍ଦେଶ_tv->v_type == VAR_STRING
		&& (opt_ସନ୍ଦେଶ_tv->vval.v_string == ଖାଲି
		    || *opt_ସନ୍ଦେଶ_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_ସନ୍ଦେଶ_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ଯଦି (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ଅନ୍ୟଥା ଯଦି (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ଅନ୍ୟଥା
	GA_CONCAT_LITERAL(gap, "Expected ");
    ଯଦି (exp_str == ଖାଲି)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ଯଦି (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ଖାଲି && got_tv->vval.v_dict != ଖାଲି)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ଯଦି (exp_tv->vval.v_dict == ଖାଲି || got_tv->vval.v_dict == ଖାଲି)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ଖାଲି;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ଖାଲି;
		ଫେରନ୍ତୁ;
	    }

	    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&exp_d->dv_hashtab, hi, todo)
	    {
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_ଚାବି, -1);
		    ଯଦି (item2 == ଖାଲି || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(ଅକ୍ଷର *)hi->hi_ଚାବି, &HI2DI(hi)->di_tv);
			ଯଦି (item2 != ଖାଲି)
			    dict_add_tv(got_tv->vval.v_dict,
					    (ଅକ୍ଷର *)hi->hi_ଚାବି, &item2->di_tv);
		    }
		    ଅନ୍ୟଥା
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&got_d->dv_hashtab, hi, todo)
	    {
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_ଚାବି, -1);
		    ଯଦି (item2 == ଖାଲି)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(ଅକ୍ଷର *)hi->hi_ଚାବି, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ଯଦି (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ଯଦି (atype != ASSERT_NOTEQUAL)
    {
	ଯଦି (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ଅନ୍ୟଥା ଯଦି (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ଅନ୍ୟଥା
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ଯଦି (omitted != 0)
	{
	    ଅକ୍ଷର    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, ଆକାର(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ଯଦି (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ଯଦି (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ଖାଲି, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    ଯଦି (pat != ଖାଲି && text != ଖାଲି
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ଖାଲି, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_bool(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା isTrue)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    garray_T	ga;

    ଯଦି (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_ସଂଖ୍ୟା == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ଫେରନ୍ତୁ 0;
    ଯଦି (argvars[0].v_type != VAR_ସଂଖ୍ୟା
	    || (tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ଖାଲି, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ଅନ୍ୟଥା
	ga_concat(gap, cmd);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_beeps(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = 0;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ଯଦି (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ଯଦି (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	ଅନ୍ୟଥା
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ଫେରନ୍ତୁ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    ଶୂନ୍ୟ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    ଶୂନ୍ୟ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    ଅକ୍ଷର	line1[200];
    ଅକ୍ଷର	line2[200];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lineidx = 0;
    size_t	IObufflen;

    ଯଦି (fname1 == ଖାଲି || fname2 == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((ଅକ୍ଷର *)fname1, READBIN);
    ଯଦି (fd1 == ଖାଲି)
    {
	IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE,
	    (ଅକ୍ଷର *)e_cant_read_file_str, fname1);
    }
    ଅନ୍ୟଥା
    {
	fd2 = mch_fopen((ଅକ୍ଷର *)fname2, READBIN);
	ଯଦି (fd2 == ଖାଲି)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE,
		(ଅକ୍ଷର *)e_cant_read_file_str, fname2);
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c1, c2;
	    long    ଗଣନା_2 = 0;
	    long    linecount = 1;

	    ପାଇଁ (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ଯଦି (c1 == EOF)
		{
		    ଯଦି (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ଯଦି (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ଗଣନା_2, linecount);
			ଭାଙ୍ଗ;
		    }
		}
		++ଗଣନା_2;
		ଯଦି (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ଅନ୍ୟଥା ଯଦି (lineidx + 2 == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(line1))
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

    ଯଦି (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ଯଦି (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ଯଦି (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ଫେରନ୍ତୁ 1;
    }

    ଫେରନ୍ତୁ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    error = tv_get_string_chk(&argvars[0]);
    ଯଦି (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ସଂଖ୍ୟା = 1;
    }
    ଅନ୍ୟଥା ଯଦି (error != ଖାଲି
	&& strstr((ଅକ୍ଷର *)get_vim_var_str(VV_EXCEPTION), (ଅକ୍ଷର *)error) == ଖାଲି)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ଖାଲି, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ସଂଖ୍ୟା = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    ଶୂନ୍ୟ
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_trylevel = trylevel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    ଅକ୍ଷର	*wrong_arg_ସନ୍ଦେଶ = ଖାଲି;
    char_u	*tofree = ଖାଲି;

    ଯଦି (check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ଫେରନ୍ତୁ;

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

    ଯଦି (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ସଂଖ୍ୟା = 1;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error_found = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_ସନ୍ଦେଶ == ଖାଲି ? (char_u *)"[unknown]"
						       : emsg_assert_fails_ସନ୍ଦେଶ;

	ଯଦି (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == ଖାଲି
			   || strstr((ଅକ୍ଷର *)actual, (ଅକ୍ଷର *)expected) == ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ଯଦି (list == ଖାଲି || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_ସନ୍ଦେଶ = e_assert_fails_second_arg;
		ଯାଅ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ଯଦି (expected == ଖାଲି)
		ଯାଅ theend;
	    ଯଦି (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    ଅନ୍ୟଥା ଯଦି (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ଯଦି (actual != ଖାଲି)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ଯଦି (expected == ଖାଲି)
			ଯାଅ theend;
		    ଯଦି (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    wrong_arg_ସନ୍ଦେଶ = e_assert_fails_second_arg;
	    ଯାଅ theend;
	}

	ଯଦି (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ଯଦି (argvars[3].v_type != VAR_ସଂଖ୍ୟା)
	    {
		wrong_arg_ସନ୍ଦେଶ = e_assert_fails_fourth_argument;
		ଯାଅ theend;
	    }
	    ଅନ୍ୟଥା ଯଦି (argvars[3].vval.v_ସଂଖ୍ୟା >= 0
			 && argvars[3].vval.v_ସଂଖ୍ୟା != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    ଯଦି (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ଯଦି (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_ସନ୍ଦେଶ = e_assert_fails_fifth_argument;
		    ଯାଅ theend;
		}
		ଅନ୍ୟଥା ଯଦି (argvars[4].vval.v_string != ଖାଲି
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	ଯଦି (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ଯଦି (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_ସଂଖ୍ୟା;
		actual_tv.vval.v_ସଂଖ୍ୟା = emsg_assert_fails_lnum;
	    }
	    ଅନ୍ୟଥା ଯଦି (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ଅନ୍ୟଥା
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
	    rettv->vval.v_ସଂଖ୍ୟା = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    ସନ୍ଦେଶ_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    ସନ୍ଦେଶ_scrolled = 0;
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows;
    VIM_CLEAR(emsg_assert_fails_ସନ୍ଦେଶ);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, ଖାଲି, 0);
    ଯଦି (wrong_arg_ସନ୍ଦେଶ != ଖାଲି)
	emsg(_(wrong_arg_ସନ୍ଦେଶ));
}

/*
 * "assert_false(actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_bool(argvars, FALSE);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    char_u	expected_str[200];

    ଯଦି (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ଯଦି (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ଅକ୍ଷର *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ଖାଲି,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ଫେରନ୍ତୁ 1;
	}
    }
    ଅନ୍ୟଥା
    {
	varnumber_T	lower = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);

	ଯଦି (error)
	    ଫେରନ୍ତୁ 0;
	ଯଦି (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ଅକ୍ଷର *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ଖାଲି,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ଫେରନ୍ତୁ 1;
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    ଶୂନ୍ୟ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_ସଂଖ୍ୟା = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    ଶୂନ୍ୟ
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    ଶୂନ୍ୟ
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_ସଂଖ୍ୟା
	    || argvars[0].vval.v_ସଂଖ୍ୟା <= 0
	    || argvars[1].v_type != VAR_ସଂଖ୍ୟା
	    || argvars[1].vval.v_ସଂଖ୍ୟା < 0
	    || argvars[2].v_type != VAR_ସଂଖ୍ୟା)
	emsg(_(e_invalid_argument));
    ଅନ୍ୟଥା
    {
	alloc_fail_id = argvars[0].vval.v_ସଂଖ୍ୟା;
	ଯଦି (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_ସଂଖ୍ୟା;
	alloc_fail_repeat = argvars[2].vval.v_ସଂଖ୍ୟା;
	did_outofmem_ସନ୍ଦେଶ = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    ଶୂନ୍ୟ
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ଯଦି defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    ଶୂନ୍ୟ
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_ନିବେଶ_BUF
    char_u	*val;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;
# endif

    ଯଦି (val != ଖାଲି)
    {
	trash_ନିବେଶ_buf();
	add_to_ନିବେଶ_buf_csi(val, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    ଶୂନ୍ୟ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);

    ଯଦି (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = need_fileinfo;
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    ଶୂନ୍ୟ
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    ଶୂନ୍ୟ
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     val;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_starting = -1;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    val = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);

    ଯଦି (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"starting") == 0)
    {
	ଯଦି (val)
	{
	    ଯଦି (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ଅନ୍ୟଥା
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_ଧାଡ଼ିଗୁଡ଼ିକ = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, (char_u *)"ALL") == 0)
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
	ଯଦି (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    ଶୂନ୍ୟ
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;

    ବାଛ (argvars[0].v_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_INSTR:
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (argvars[0].vval.v_job != ଖାଲି)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (argvars[0].vval.v_channel != ଖାଲି)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FUNC:
	    ଯଦି (argvars[0].vval.v_string != ଖାଲି)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		ଯଦି (fp != ଖାଲି)
		    retval = fp->uf_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (argvars[0].vval.v_partial != ଖାଲି)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି (argvars[0].vval.v_blob != ଖାଲି)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (argvars[0].vval.v_list != ଖାଲି)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (argvars[0].vval.v_tuple != ଖାଲି)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି (argvars[0].vval.v_dict != ଖାଲି)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	    ଯଦି (argvars[0].vval.v_class != ଖାଲି)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    ଯଦି (argvars[0].vval.v_object != ଖାଲି)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଯଦି (argvars[0].vval.v_typealias != ଖାଲି)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ଭାଙ୍ଗ;
    }

    rettv->v_type = VAR_ସଂଖ୍ୟା;
    rettv->vval.v_ସଂଖ୍ୟା = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    ଶୂନ୍ୟ
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ଯଦି (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    ଅନ୍ୟଥା
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    ଶୂନ୍ୟ
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    ଶୂନ୍ୟ
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    ଶୂନ୍ୟ
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ଖାଲି;
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଶୂନ୍ୟ
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ଖାଲି;
}
#endif

    ଶୂନ୍ୟ
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, ଖାଲି);
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଶୂନ୍ୟ
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = ଖାଲି;
}
#endif

    ଶୂନ୍ୟ
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, ଖାଲି);
}

    ଶୂନ୍ୟ
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ଖାଲି;
}

    ଶୂନ୍ୟ
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ଖାଲି;
}

    ଶୂନ୍ୟ
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
}

    ଶୂନ୍ୟ
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, ଖାଲି);
}

    ଶୂନ୍ୟ
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    ଶୂନ୍ୟ
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    ଶୂନ୍ୟ
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_ସଂଖ୍ୟା || argvars[1].v_type != VAR_ସଂଖ୍ୟା)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    mouse_row = (time_t)tv_get_ସଂଖ୍ୟା(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_drop_files(dict_T *args UNUSED)
{
# ଯଦି defined(HAVE_DROP_FILE)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    int_u	mods;
    char_u	**fnames;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ଯଦି (!dict_has_ଚାବି(args, "files")
	    || !dict_has_ଚାବି(args, "row")
	    || !dict_has_ଚାବି(args, "col")
	    || !dict_has_ଚାବି(args, "modifiers"))
	ଫେରନ୍ତୁ FALSE;

    (ଶୂନ୍ୟ)dict_get_tv(args, "files", &t);
    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "row");
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "col");
    mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");

    ଯଦି (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ଫେରନ୍ତୁ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ଯଦି (fnames == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	// ignore non-string items
	ଯଦି (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ଖାଲି)
	    ଜାରି;

	fnames[ଗଣନା_2] = vim_strsave(li->li_tv.vval.v_string);
	ଯଦି (fnames[ଗଣନା_2] == ଖାଲି)
	{
	    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
		vim_free(fnames[ଗଣନା_2]);
	    vim_free(fnames);
	    ଫେରନ୍ତୁ FALSE;
	}
	ଗଣନା_2++;
    }

    ଯଦି (ଗଣନା_2 > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, ଗଣନା_2);
    ଅନ୍ୟଥା
	vim_free(fnames);
# endif

    ଫେରନ୍ତୁ TRUE;
}

# ଯଦି defined(FIND_REPLACE_DIALOG)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_find_repl(dict_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    char_u	*find_text;
    char_u	*repl_text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forward;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    ଯଦି (!dict_has_ଚାବି(args, "find_text")
	    || !dict_has_ଚାବି(args, "repl_text")
	    || !dict_has_ଚାବି(args, "flags")
	    || !dict_has_ଚାବି(args, "forward"))
	ଫେରନ୍ତୁ FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "flags");
    forward = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    ଫେରନ୍ତୁ retval;
}
# endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_mouse_event(dict_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeated_click;
    int_u	mods;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		move;

    ଯଦି (!dict_has_ଚାବି(args, "row")
	    || !dict_has_ଚାବି(args, "col"))
	ଫେରନ୍ତୁ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_bool(args, "move", FALSE);

    ଯଦି (!move && (!dict_has_ଚାବି(args, "button")
	    || !dict_has_ଚାବି(args, "multiclick")
	    || !dict_has_ଚାବି(args, "modifiers")))
	ଫେରନ୍ତୁ FALSE;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "row");
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "col");

    ଯଦି (move)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pY = row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ଯଦି (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ଅନ୍ୟଥା
    {
	button = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "button");
	repeated_click = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "multiclick");
	mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dragging;
    scrollbar_T *sb = ଖାଲି;

    ଯଦି (!dict_has_ଚାବି(args, "which")
	    || !dict_has_ଚାବି(args, "value")
	    || !dict_has_ଚାବି(args, "dragging"))
	ଫେରନ୍ତୁ FALSE;

    which = dict_get_string(args, "which", FALSE);
    ମୂଲ୍ୟ = (long)dict_get_ସଂଖ୍ୟା(args, "value");
    dragging = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "dragging");

    ଯଦି (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ବାମ];
    ଅନ୍ୟଥା ଯଦି (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ଡାହାଣ];
    ଅନ୍ୟଥା ଯଦି (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ଯଦି (sb == ଖାଲି)
    {
	semsg(_(e_invalid_argument_str), which);
	ଫେରନ୍ତୁ FALSE;
    }
    gui_drag_scrollbar(sb, ମୂଲ୍ୟ, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabnr;

    ଯଦି (!dict_has_ଚାବି(args, "tabnr"))
	ଫେରନ୍ତୁ FALSE;

    tabnr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "tabnr");

    ଫେରନ୍ତୁ send_tabline_event(tabnr);
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
# endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tabnr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	item;

    ଯଦି (!dict_has_ଚାବି(args, "tabnr")
	    || !dict_has_ଚାବି(args, "item"))
	ଫେରନ୍ତୁ FALSE;

    tabnr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "tabnr");
    item = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    ଫେରନ୍ତୁ TRUE;
}
#endif

    ଶୂନ୍ୟ
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = FALSE;

    ଯଦି (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    ଶୂନ୍ୟ
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = FALSE;

    ଯଦି (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ଖାଲି)
	ଫେରନ୍ତୁ;

    event = tv_get_string(&argvars[0]);
    ଯଦି (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_drop_files(argvars[1].vval.v_dict);
# ଯଦି defined(FIND_REPLACE_DIALOG)
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "mouse") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_mouse_event(argvars[1].vval.v_dict);
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_scrollbar(argvars[1].vval.v_dict);
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "tabline") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_tabline_event(argvars[1].vval.v_dict);
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_ସଂଖ୍ୟା = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str), event);
	ଫେରନ୍ତୁ;
    }
#endif
}

    ଶୂନ୍ୟ
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    time_for_testing = (time_t)tv_get_ସଂଖ୍ୟା(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
