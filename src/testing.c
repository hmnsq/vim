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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    સ્થિર શૂન્ય
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    જો (sname != ખાલી)
    {
	ga_concat(gap, sname);
	જો (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    જો (SOURCING_LNUM > 0)
    {
	અક્ષર    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, કદ(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    જો (sname != ખાલી || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    સ્થિર શૂન્ય
ga_concat_esc(garray_T *gap, char_u *p, પૂર્ણાંક clen)
{
    char_u  buf[NUMBUFLEN];

    જો (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	પાછા;
    }

    પસંદગી (*p)
    {
	સ્થિતિ BS: GA_CONCAT_LITERAL(gap, "\\b"); તોડો;
	સ્થિતિ ESC: GA_CONCAT_LITERAL(gap, "\\e"); તોડો;
	સ્થિતિ FF: GA_CONCAT_LITERAL(gap, "\\f"); તોડો;
	સ્થિતિ NL: GA_CONCAT_LITERAL(gap, "\\n"); તોડો;
	સ્થિતિ TAB: GA_CONCAT_LITERAL(gap, "\\t"); તોડો;
	સ્થિતિ CAR: GA_CONCAT_LITERAL(gap, "\\r"); તોડો;
	સ્થિતિ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); તોડો;
	મૂળભૂત:
	   જો (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((અક્ષર *)buf, કદ(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   નહીંતર
	       ga_append(gap, *p);
	   તોડો;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    સ્થિર શૂન્ય
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    પૂર્ણાંક	    c;
    પૂર્ણાંક	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    પૂર્ણાંક	    same_len;

    જો (str == ખાલી)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	પાછા;
    }

    માટે (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	જ્યારે (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	જો (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((અક્ષર *)buf, કદ(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	નહીંતર
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    સ્થિર શૂન્ય
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_સંદેશ_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    પૂર્ણાંક		did_copy = FALSE;
    પૂર્ણાંક		omitted = 0;

    જો (opt_સંદેશ_tv->v_type != VAR_UNKNOWN
	    && !(opt_સંદેશ_tv->v_type == VAR_STRING
		&& (opt_સંદેશ_tv->vval.v_string == ખાલી
		    || *opt_સંદેશ_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_સંદેશ_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    જો (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    નહીંતર જો (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    નહીંતર
	GA_CONCAT_LITERAL(gap, "Expected ");
    જો (exp_str == ખાલી)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	જો (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ખાલી && got_tv->vval.v_dict != ખાલી)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    પૂર્ણાંક		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    જો (exp_tv->vval.v_dict == ખાલી || got_tv->vval.v_dict == ખાલી)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ખાલી;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ખાલી;
		પાછા;
	    }

	    todo = (પૂર્ણાંક)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(&exp_d->dv_hashtab, hi, todo)
	    {
		જો (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_ચાવી, -1);
		    જો (item2 == ખાલી || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(અક્ષર *)hi->hi_ચાવી, &HI2DI(hi)->di_tv);
			જો (item2 != ખાલી)
			    dict_add_tv(got_tv->vval.v_dict,
					    (અક્ષર *)hi->hi_ચાવી, &item2->di_tv);
		    }
		    નહીંતર
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (પૂર્ણાંક)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(&got_d->dv_hashtab, hi, todo)
	    {
		જો (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_ચાવી, -1);
		    જો (item2 == ખાલી)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(અક્ષર *)hi->hi_ચાવી, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    નહીંતર
    {
	જો (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	જો (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    જો (atype != ASSERT_NOTEQUAL)
    {
	જો (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	નહીંતર જો (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	નહીંતર
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	જો (omitted != 0)
	{
	    અક્ષર    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, કદ(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    જો (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    સ્થિર પૂર્ણાંક
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    જો (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ખાલી, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	પાછા 1;
    }
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    જો (pat != ખાલી && text != ખાલી
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ખાલી, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	પાછા 1;
    }
    પાછા 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    સ્થિર પૂર્ણાંક
assert_bool(typval_T *argvars, પૂર્ણાંક isTrue)
{
    પૂર્ણાંક		error = FALSE;
    garray_T	ga;

    જો (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_સંખ્યા == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	પાછા 0;
    જો (argvars[0].v_type != VAR_સંખ્યા
	    || (tv_get_સંખ્યા_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ખાલી, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	પાછા 1;
    }
    પાછા 0;
}

    સ્થિર શૂન્ય
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    જો (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    નહીંતર
	ga_concat(gap, cmd);
}

    સ્થિર પૂર્ણાંક
assert_beeps(typval_T *argvars, પૂર્ણાંક no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    પૂર્ણાંક		ret = 0;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    જો (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	જો (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	નહીંતર
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    પાછા ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    શૂન્ય
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    શૂન્ય
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    શૂન્ય
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_equal_common(argvars, ASSERT_EQUAL);
}

    સ્થિર પૂર્ણાંક
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    અક્ષર	line1[200];
    અક્ષર	line2[200];
    પૂર્ણાંક		lineidx = 0;
    size_t	IObufflen;

    જો (fname1 == ખાલી || fname2 == ખાલી)
	પાછા 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((અક્ષર *)fname1, READBIN);
    જો (fd1 == ખાલી)
    {
	IObufflen = vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE,
	    (અક્ષર *)e_cant_read_file_str, fname1);
    }
    નહીંતર
    {
	fd2 = mch_fopen((અક્ષર *)fname2, READBIN);
	જો (fd2 == ખાલી)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE,
		(અક્ષર *)e_cant_read_file_str, fname2);
	}
	નહીંતર
	{
	    પૂર્ણાંક	    c1, c2;
	    long    ગણતરી = 0;
	    long    linecount = 1;

	    માટે (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		જો (c1 == EOF)
		{
		    જો (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    તોડો;
		}
		નહીંતર જો (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    તોડો;
		}
		નહીંતર
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    જો (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ગણતરી, linecount);
			તોડો;
		    }
		}
		++ગણતરી;
		જો (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		નહીંતર જો (lineidx + 2 == (પૂર્ણાંક)કદ(line1))
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

    જો (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	જો (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    જો (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	પાછા 1;
    }

    પાછા 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    શૂન્ય
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    rettv->vval.v_સંખ્યા = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    શૂન્ય
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    શૂન્ય
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    error = tv_get_string_chk(&argvars[0]);
    જો (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_સંખ્યા = 1;
    }
    નહીંતર જો (error != ખાલી
	&& strstr((અક્ષર *)get_vim_var_str(VV_EXCEPTION), (અક્ષર *)error) == ખાલી)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ખાલી, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_સંખ્યા = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    શૂન્ય
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    પૂર્ણાંક		save_trylevel = trylevel;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    અક્ષર	*wrong_arg_સંદેશ = ખાલી;
    char_u	*tofree = ખાલી;

    જો (check_for_string_or_સંખ્યા_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	પાછા;

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

    જો (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_સંખ્યા = 1;
    }
    નહીંતર જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ખાલી;
	પૂર્ણાંક	error_found = FALSE;
	પૂર્ણાંક	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_સંદેશ == ખાલી ? (char_u *)"[unknown]"
						       : emsg_assert_fails_સંદેશ;

	જો (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == ખાલી
			   || strstr((અક્ષર *)actual, (અક્ષર *)expected) == ખાલી;
	}
	નહીંતર જો (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    જો (list == ખાલી || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_સંદેશ = e_assert_fails_second_arg;
		જાઓ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    જો (expected == ખાલી)
		જાઓ theend;
	    જો (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    નહીંતર જો (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		જો (actual != ખાલી)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    જો (expected == ખાલી)
			જાઓ theend;
		    જો (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	નહીંતર
	{
	    wrong_arg_સંદેશ = e_assert_fails_second_arg;
	    જાઓ theend;
	}

	જો (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    જો (argvars[3].v_type != VAR_સંખ્યા)
	    {
		wrong_arg_સંદેશ = e_assert_fails_fourth_argument;
		જાઓ theend;
	    }
	    નહીંતર જો (argvars[3].vval.v_સંખ્યા >= 0
			 && argvars[3].vval.v_સંખ્યા != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    જો (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		જો (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_સંદેશ = e_assert_fails_fifth_argument;
		    જાઓ theend;
		}
		નહીંતર જો (argvars[4].vval.v_string != ખાલી
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	જો (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    જો (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_સંખ્યા;
		actual_tv.vval.v_સંખ્યા = emsg_assert_fails_lnum;
	    }
	    નહીંતર જો (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    નહીંતર
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
	    rettv->vval.v_સંખ્યા = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    સંદેશ_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    સંદેશ_scrolled = 0;
    લીટીઓ_ડાબું = Rows;
    VIM_CLEAR(emsg_assert_fails_સંદેશ);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, ખાલી, 0);
    જો (wrong_arg_સંદેશ != ખાલી)
	emsg(_(wrong_arg_સંદેશ));
}

/*
 * "assert_false(actual[, msg])" function
 */
    શૂન્ય
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_bool(argvars, FALSE);
}

    સ્થિર પૂર્ણાંક
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    પૂર્ણાંક		error = FALSE;
    char_u	expected_str[200];

    જો (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	જો (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((અક્ષર *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ખાલી,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    પાછા 1;
	}
    }
    નહીંતર
    {
	varnumber_T	lower = tv_get_સંખ્યા_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_સંખ્યા_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_સંખ્યા_chk(&argvars[2], &error);

	જો (error)
	    પાછા 0;
	જો (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((અક્ષર *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ખાલી,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    પાછા 1;
	}
    }
    પાછા 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    શૂન્ય
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    જો (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    શૂન્ય
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    શૂન્ય
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    શૂન્ય
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_સંખ્યા = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    શૂન્ય
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    શૂન્ય
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    જો (argvars[0].v_type != VAR_સંખ્યા
	    || argvars[0].vval.v_સંખ્યા <= 0
	    || argvars[1].v_type != VAR_સંખ્યા
	    || argvars[1].vval.v_સંખ્યા < 0
	    || argvars[2].v_type != VAR_સંખ્યા)
	emsg(_(e_invalid_argument));
    નહીંતર
    {
	alloc_fail_id = argvars[0].vval.v_સંખ્યા;
	જો (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_સંખ્યા;
	alloc_fail_repeat = argvars[2].vval.v_સંખ્યા;
	did_outofmem_સંદેશ = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    શૂન્ય
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#જો defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    શૂન્ય
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_આગત_BUF
    char_u	*val;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    જો (!gui.in_use)
	પાછા;
# endif

    જો (val != ખાલી)
    {
	trash_આગત_buf();
	add_to_આગત_buf_csi(val, (પૂર્ણાંક)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    શૂન્ય
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    name = tv_get_string(&argvars[0]);

    જો (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_સંખ્યા = need_fileinfo;
    નહીંતર
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    શૂન્ય
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    શૂન્ય
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    પૂર્ણાંક     val;
    સ્થિર પૂર્ણાંક save_starting = -1;

    જો (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL)
	પાછા;

    name = tv_get_string(&argvars[0]);
    val = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);

    જો (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"starting") == 0)
    {
	જો (val)
	{
	    જો (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	નહીંતર
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    નહીંતર જો (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    નહીંતર જો (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    નહીંતર જો (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    નહીંતર જો (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    નહીંતર જો (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_લીટીઓ = val;
    નહીંતર જો (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    નહીંતર જો (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    નહીંતર જો (STRCMP(name, (char_u *)"ALL") == 0)
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
	જો (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    નહીંતર
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    શૂન્ય
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક retval = -1;

    પસંદગી (argvars[0].v_type)
    {
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_INSTR:
	    તોડો;

	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    જો (argvars[0].vval.v_job != ખાલી)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    તોડો;
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    જો (argvars[0].vval.v_channel != ખાલી)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    તોડો;
	સ્થિતિ VAR_FUNC:
	    જો (argvars[0].vval.v_string != ખાલી)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		જો (fp != ખાલી)
		    retval = fp->uf_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    જો (argvars[0].vval.v_partial != ખાલી)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    જો (argvars[0].vval.v_blob != ખાલી)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_LIST:
	    જો (argvars[0].vval.v_list != ખાલી)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    જો (argvars[0].vval.v_tuple != ખાલી)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_DICT:
	    જો (argvars[0].vval.v_dict != ખાલી)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_CLASS:
	    જો (argvars[0].vval.v_class != ખાલી)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    જો (argvars[0].vval.v_object != ખાલી)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    તોડો;
	સ્થિતિ VAR_TYPEALIAS:
	    જો (argvars[0].vval.v_typealias != ખાલી)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    તોડો;
    }

    rettv->v_type = VAR_સંખ્યા;
    rettv->vval.v_સંખ્યા = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    શૂન્ય
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    જો (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    નહીંતર
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    શૂન્ય
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    શૂન્ય
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    શૂન્ય
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ખાલી;
}

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ખાલી;
}
#endif

    શૂન્ય
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, ખાલી);
}

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = ખાલી;
}
#endif

    શૂન્ય
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, ખાલી);
}

    શૂન્ય
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ખાલી;
}

    શૂન્ય
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ખાલી;
}

    શૂન્ય
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
}

    શૂન્ય
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, ખાલી);
}

    શૂન્ય
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    શૂન્ય
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    શૂન્ય
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[0].v_type != VAR_સંખ્યા || argvars[1].v_type != VAR_સંખ્યા)
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    mouse_row = (time_t)tv_get_સંખ્યા(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_સંખ્યા(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    સ્થિર પૂર્ણાંક
test_gui_drop_files(dict_T *args UNUSED)
{
# જો defined(HAVE_DROP_FILE)
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    int_u	mods;
    char_u	**fnames;
    પૂર્ણાંક		ગણતરી = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    જો (!dict_has_ચાવી(args, "files")
	    || !dict_has_ચાવી(args, "row")
	    || !dict_has_ચાવી(args, "col")
	    || !dict_has_ચાવી(args, "modifiers"))
	પાછા FALSE;

    (શૂન્ય)dict_get_tv(args, "files", &t);
    row = (પૂર્ણાંક)dict_get_સંખ્યા(args, "row");
    col = (પૂર્ણાંક)dict_get_સંખ્યા(args, "col");
    mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");

    જો (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	પાછા FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    જો (fnames == ખાલી)
	પાછા FALSE;

    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	// ignore non-string items
	જો (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ખાલી)
	    ચાલુ;

	fnames[ગણતરી] = vim_strsave(li->li_tv.vval.v_string);
	જો (fnames[ગણતરી] == ખાલી)
	{
	    જ્યારે (--ગણતરી >= 0)
		vim_free(fnames[ગણતરી]);
	    vim_free(fnames);
	    પાછા FALSE;
	}
	ગણતરી++;
    }

    જો (ગણતરી > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, ગણતરી);
    નહીંતર
	vim_free(fnames);
# endif

    પાછા TRUE;
}

# જો defined(FIND_REPLACE_DIALOG)
    સ્થિર પૂર્ણાંક
test_gui_find_repl(dict_T *args)
{
    પૂર્ણાંક		flags;
    char_u	*find_text;
    char_u	*repl_text;
    પૂર્ણાંક		forward;
    પૂર્ણાંક		retval;

    જો (!dict_has_ચાવી(args, "find_text")
	    || !dict_has_ચાવી(args, "repl_text")
	    || !dict_has_ચાવી(args, "flags")
	    || !dict_has_ચાવી(args, "forward"))
	પાછા FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (પૂર્ણાંક)dict_get_સંખ્યા(args, "flags");
    forward = (પૂર્ણાંક)dict_get_સંખ્યા(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    પાછા retval;
}
# endif

    સ્થિર પૂર્ણાંક
test_gui_mouse_event(dict_T *args)
{
    પૂર્ણાંક		button;
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    પૂર્ણાંક		repeated_click;
    int_u	mods;
    પૂર્ણાંક		move;

    જો (!dict_has_ચાવી(args, "row")
	    || !dict_has_ચાવી(args, "col"))
	પાછા FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (પૂર્ણાંક)dict_get_bool(args, "move", FALSE);

    જો (!move && (!dict_has_ચાવી(args, "button")
	    || !dict_has_ચાવી(args, "multiclick")
	    || !dict_has_ચાવી(args, "modifiers")))
	પાછા FALSE;

    row = (પૂર્ણાંક)dict_get_સંખ્યા(args, "row");
    col = (પૂર્ણાંક)dict_get_સંખ્યા(args, "col");

    જો (move)
    {
	પૂર્ણાંક pY = row;
	પૂર્ણાંક pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	જો (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    નહીંતર
    {
	button = (પૂર્ણાંક)dict_get_સંખ્યા(args, "button");
	repeated_click = (પૂર્ણાંક)dict_get_સંખ્યા(args, "multiclick");
	mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    પાછા TRUE;
}

    સ્થિર પૂર્ણાંક
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	કિંમત;
    પૂર્ણાંક		dragging;
    scrollbar_T *sb = ખાલી;

    જો (!dict_has_ચાવી(args, "which")
	    || !dict_has_ચાવી(args, "value")
	    || !dict_has_ચાવી(args, "dragging"))
	પાછા FALSE;

    which = dict_get_string(args, "which", FALSE);
    કિંમત = (long)dict_get_સંખ્યા(args, "value");
    dragging = (પૂર્ણાંક)dict_get_સંખ્યા(args, "dragging");

    જો (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ડાબું];
    નહીંતર જો (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_જમણું];
    નહીંતર જો (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    જો (sb == ખાલી)
    {
	semsg(_(e_invalid_argument_str), which);
	પાછા FALSE;
    }
    gui_drag_scrollbar(sb, કિંમત, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    પાછા TRUE;
}

    સ્થિર પૂર્ણાંક
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    પૂર્ણાંક		tabnr;

    જો (!dict_has_ચાવી(args, "tabnr"))
	પાછા FALSE;

    tabnr = (પૂર્ણાંક)dict_get_સંખ્યા(args, "tabnr");

    પાછા send_tabline_event(tabnr);
# નહીંતર
    પાછા FALSE;
# endif
}

    સ્થિર પૂર્ણાંક
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    પૂર્ણાંક	tabnr;
    પૂર્ણાંક	item;

    જો (!dict_has_ચાવી(args, "tabnr")
	    || !dict_has_ચાવી(args, "item"))
	પાછા FALSE;

    tabnr = (પૂર્ણાંક)dict_get_સંખ્યા(args, "tabnr");
    item = (પૂર્ણાંક)dict_get_સંખ્યા(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    પાછા TRUE;
}
#endif

    શૂન્ય
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = FALSE;

    જો (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	પાછા;
    }

    જો (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ખાલી)
	પાછા;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_સંખ્યા = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    શૂન્ય
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = FALSE;

    જો (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	પાછા;
    }

    જો (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ખાલી)
	પાછા;

    event = tv_get_string(&argvars[0]);
    જો (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_સંખ્યા = test_gui_drop_files(argvars[1].vval.v_dict);
# જો defined(FIND_REPLACE_DIALOG)
    નહીંતર જો (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_સંખ્યા = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    નહીંતર જો (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_સંખ્યા = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    નહીંતર જો (STRCMP(event, "mouse") == 0)
	rettv->vval.v_સંખ્યા = test_gui_mouse_event(argvars[1].vval.v_dict);
    નહીંતર જો (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_સંખ્યા = test_gui_scrollbar(argvars[1].vval.v_dict);
    નહીંતર જો (STRCMP(event, "tabline") == 0)
	rettv->vval.v_સંખ્યા = test_gui_tabline_event(argvars[1].vval.v_dict);
    નહીંતર જો (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_સંખ્યા = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    નહીંતર
    {
	semsg(_(e_invalid_argument_str), event);
	પાછા;
    }
#endif
}

    શૂન્ય
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    time_for_testing = (time_t)tv_get_સંખ્યા(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
