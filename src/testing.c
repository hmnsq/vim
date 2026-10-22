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

#जर defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    स्थिर शून्य
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    जर (sname != रिक्त)
    {
	ga_concat(gap, sname);
	जर (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    जर (SOURCING_LNUM > 0)
    {
	अक्षर    buf[NUMBUFLEN];
	माप_प्रकार	buflen;

	buflen = vim_snprintf_safelen(buf, आकार(buf), "line %ld", (दीर्घ)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    जर (sname != रिक्त || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    स्थिर शून्य
ga_concat_esc(garray_T *gap, char_u *p, पूर्णांक clen)
{
    char_u  buf[NUMBUFLEN];

    जर (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	परत;
    }

    निवड (*p)
    {
	स्थिती BS: GA_CONCAT_LITERAL(gap, "\\b"); थांबा;
	स्थिती ESC: GA_CONCAT_LITERAL(gap, "\\e"); थांबा;
	स्थिती FF: GA_CONCAT_LITERAL(gap, "\\f"); थांबा;
	स्थिती NL: GA_CONCAT_LITERAL(gap, "\\n"); थांबा;
	स्थिती TAB: GA_CONCAT_LITERAL(gap, "\\t"); थांबा;
	स्थिती CAR: GA_CONCAT_LITERAL(gap, "\\r"); थांबा;
	स्थिती '\\': GA_CONCAT_LITERAL(gap, "\\\\"); थांबा;
	पूर्वनिर्धारित:
	   जर (*p < ' ' || *p == 0x7f)
	   {
		माप_प्रकार	buflen;

		buflen = vim_snprintf_safelen((अक्षर *)buf, आकार(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   अन्यथा
	       ga_append(gap, *p);
	   थांबा;
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
    पूर्णांक	    c;
    पूर्णांक	    clen;
    char_u  buf[NUMBUFLEN];
    माप_प्रकार  buflen;
    पूर्णांक	    same_len;

    जर (str == रिक्त)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	परत;
    }

    साठी (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	पर्यंत (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	जर (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((अक्षर *)buf, आकार(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	अन्यथा
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
    typval_T	*opt_संदेश_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    पूर्णांक		did_copy = FALSE;
    पूर्णांक		omitted = 0;

    जर (opt_संदेश_tv->v_type != VAR_UNKNOWN
	    && !(opt_संदेश_tv->v_type == VAR_STRING
		&& (opt_संदेश_tv->vval.v_string == रिक्त
		    || *opt_संदेश_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_संदेश_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    जर (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    अन्यथा जर (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    अन्यथा
	GA_CONCAT_LITERAL(gap, "Expected ");
    जर (exp_str == रिक्त)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	जर (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != रिक्त && got_tv->vval.v_dict != रिक्त)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    पूर्णांक		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    जर (exp_tv->vval.v_dict == रिक्त || got_tv->vval.v_dict == रिक्त)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = रिक्त;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = रिक्त;
		परत;
	    }

	    todo = (पूर्णांक)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_वस्तुसंच(&exp_d->dv_hashtab, hi, todo)
	    {
		जर (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_किल्ली, -1);
		    जर (item2 == रिक्त || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(अक्षर *)hi->hi_किल्ली, &HI2DI(hi)->di_tv);
			जर (item2 != रिक्त)
			    dict_add_tv(got_tv->vval.v_dict,
					    (अक्षर *)hi->hi_किल्ली, &item2->di_tv);
		    }
		    अन्यथा
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (पूर्णांक)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_वस्तुसंच(&got_d->dv_hashtab, hi, todo)
	    {
		जर (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_किल्ली, -1);
		    जर (item2 == रिक्त)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(अक्षर *)hi->hi_किल्ली, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    अन्यथा
    {
	जर (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	जर (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    जर (atype != ASSERT_NOTEQUAL)
    {
	जर (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	अन्यथा जर (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	अन्यथा
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	जर (omitted != 0)
	{
	    अक्षर    buf[100];
	    माप_प्रकार  buflen;

	    buflen = vim_snprintf_safelen(buf, आकार(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    जर (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    स्थिर पूर्णांक
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    जर (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], रिक्त, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	परत 1;
    }
    परत 0;
}

    स्थिर पूर्णांक
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	परत 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    जर (pat != रिक्त && text != रिक्त
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], रिक्त, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	परत 1;
    }
    परत 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    स्थिर पूर्णांक
assert_bool(typval_T *argvars, पूर्णांक isTrue)
{
    पूर्णांक		error = FALSE;
    garray_T	ga;

    जर (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_संख्या == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	परत 0;
    जर (argvars[0].v_type != VAR_संख्या
	    || (tv_get_संख्या_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		रिक्त, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	परत 1;
    }
    परत 0;
}

    स्थिर शून्य
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    जर (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    अन्यथा
	ga_concat(gap, cmd);
}

    स्थिर पूर्णांक
assert_beeps(typval_T *argvars, पूर्णांक no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    पूर्णांक		ret = 0;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    जर (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	जर (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	अन्यथा
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    परत ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    शून्य
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    शून्य
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    शून्य
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_equal_common(argvars, ASSERT_EQUAL);
}

    स्थिर पूर्णांक
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    खाता	*fd1;
    खाता	*fd2;
    अक्षर	line1[200];
    अक्षर	line2[200];
    पूर्णांक		lineidx = 0;
    माप_प्रकार	IObufflen;

    जर (fname1 == रिक्त || fname2 == रिक्त)
	परत 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((अक्षर *)fname1, READBIN);
    जर (fd1 == रिक्त)
    {
	IObufflen = vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE,
	    (अक्षर *)e_cant_read_file_str, fname1);
    }
    अन्यथा
    {
	fd2 = mch_fopen((अक्षर *)fname2, READBIN);
	जर (fd2 == रिक्त)
	{
	    ख_बंद(fd1);
	    IObufflen = vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE,
		(अक्षर *)e_cant_read_file_str, fname2);
	}
	अन्यथा
	{
	    पूर्णांक	    c1, c2;
	    दीर्घ    मोजणी = 0;
	    दीर्घ    linecount = 1;

	    साठी (;;)
	    {
		c1 = ख_अक्षर_लो(fd1);
		c2 = ख_अक्षर_लो(fd2);
		जर (c1 == खातापूर्ण)
		{
		    जर (c2 != खातापूर्ण)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    थांबा;
		}
		अन्यथा जर (c2 == खातापूर्ण)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    थांबा;
		}
		अन्यथा
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    जर (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", मोजणी, linecount);
			थांबा;
		    }
		}
		++मोजणी;
		जर (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		अन्यथा जर (lineidx + 2 == (पूर्णांक)आकार(line1))
		{
		    mch_memmove(line1, line1 + 100, lineidx - 100);
		    mch_memmove(line2, line2 + 100, lineidx - 100);
		    lineidx -= 100;
		}
	    }
	    ख_बंद(fd1);
	    ख_बंद(fd2);
	}
    }

    जर (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	जर (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    जर (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	परत 1;
    }

    परत 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    शून्य
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	परत;

    rettv->vval.v_संख्या = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    शून्य
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    शून्य
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    error = tv_get_string_chk(&argvars[0]);
    जर (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_संख्या = 1;
    }
    अन्यथा जर (error != रिक्त
	&& म_माला((अक्षर *)get_vim_var_str(VV_EXCEPTION), (अक्षर *)error) == रिक्त)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], रिक्त, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_संख्या = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    शून्य
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    पूर्णांक		save_trylevel = trylevel;
    पूर्णांक		called_emsg_before = called_emsg;
    अक्षर	*wrong_arg_संदेश = रिक्त;
    char_u	*tofree = रिक्त;

    जर (check_for_string_or_संख्या_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_संख्या_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	परत;

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

    जर (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_संख्या = 1;
    }
    अन्यथा जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = रिक्त;
	पूर्णांक	error_found = FALSE;
	पूर्णांक	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_संदेश == रिक्त ? (char_u *)"[unknown]"
						       : emsg_assert_fails_संदेश;

	जर (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == रिक्त
			   || म_माला((अक्षर *)actual, (अक्षर *)expected) == रिक्त;
	}
	अन्यथा जर (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    जर (list == रिक्त || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_संदेश = e_assert_fails_second_arg;
		जा theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    जर (expected == रिक्त)
		जा theend;
	    जर (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    अन्यथा जर (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		जर (actual != रिक्त)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    जर (expected == रिक्त)
			जा theend;
		    जर (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	अन्यथा
	{
	    wrong_arg_संदेश = e_assert_fails_second_arg;
	    जा theend;
	}

	जर (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    जर (argvars[3].v_type != VAR_संख्या)
	    {
		wrong_arg_संदेश = e_assert_fails_fourth_argument;
		जा theend;
	    }
	    अन्यथा जर (argvars[3].vval.v_संख्या >= 0
			 && argvars[3].vval.v_संख्या != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    जर (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		जर (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_संदेश = e_assert_fails_fifth_argument;
		    जा theend;
		}
		अन्यथा जर (argvars[4].vval.v_string != रिक्त
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	जर (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    जर (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_संख्या;
		actual_tv.vval.v_संख्या = emsg_assert_fails_lnum;
	    }
	    अन्यथा जर (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    अन्यथा
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
	    rettv->vval.v_संख्या = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    संदेश_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    संदेश_scrolled = 0;
    ओळी_डावा = Rows;
    VIM_CLEAR(emsg_assert_fails_संदेश);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, रिक्त, 0);
    जर (wrong_arg_संदेश != रिक्त)
	emsg(_(wrong_arg_संदेश));
}

/*
 * "assert_false(actual[, msg])" function
 */
    शून्य
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_bool(argvars, FALSE);
}

    स्थिर पूर्णांक
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    पूर्णांक		error = FALSE;
    char_u	expected_str[200];

    जर (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	जर (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((अक्षर *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, रिक्त,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    परत 1;
	}
    }
    अन्यथा
    {
	varnumber_T	lower = tv_get_संख्या_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_संख्या_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_संख्या_chk(&argvars[2], &error);

	जर (error)
	    परत 0;
	जर (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((अक्षर *)expected_str, 200, "range %ld - %ld,",
						     (दीर्घ)lower, (दीर्घ)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, रिक्त,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    परत 1;
	}
    }
    परत 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    शून्य
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    जर (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	परत;

    rettv->vval.v_संख्या = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    शून्य
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    शून्य
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    शून्य
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_संख्या = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    शून्य
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    शून्य
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_संख्या_arg(argvars, 2) == FAIL))
	परत;

    जर (argvars[0].v_type != VAR_संख्या
	    || argvars[0].vval.v_संख्या <= 0
	    || argvars[1].v_type != VAR_संख्या
	    || argvars[1].vval.v_संख्या < 0
	    || argvars[2].v_type != VAR_संख्या)
	emsg(_(e_invalid_argument));
    अन्यथा
    {
	alloc_fail_id = argvars[0].vval.v_संख्या;
	जर (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_संख्या;
	alloc_fail_repeat = argvars[2].vval.v_संख्या;
	did_outofmem_संदेश = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    शून्य
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#जर defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    शून्य
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_आदान_BUF
    char_u	*val;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    जर (!gui.in_use)
	परत;
# endif

    जर (val != रिक्त)
    {
	trash_आदान_buf();
	add_to_आदान_buf_csi(val, (पूर्णांक)STRLEN(val));
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

    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

    name = tv_get_string(&argvars[0]);

    जर (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_संख्या = need_fileinfo;
    अन्यथा
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    शून्य
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

    name = tv_get_string(&argvars[0]);
    जर (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    शून्य
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    पूर्णांक     val;
    स्थिर पूर्णांक save_starting = -1;

    जर (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL)
	परत;

    name = tv_get_string(&argvars[0]);
    val = (पूर्णांक)tv_get_संख्या(&argvars[1]);

    जर (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"starting") == 0)
    {
	जर (val)
	{
	    जर (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	अन्यथा
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    अन्यथा जर (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    अन्यथा जर (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    अन्यथा जर (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    अन्यथा जर (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    अन्यथा जर (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_ओळी = val;
    अन्यथा जर (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    अन्यथा जर (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    अन्यथा जर (STRCMP(name, (char_u *)"ALL") == 0)
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
	जर (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    अन्यथा
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    शून्य
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक retval = -1;

    निवड (argvars[0].v_type)
    {
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_VOID:
	स्थिती VAR_संख्या:
	स्थिती VAR_BOOL:
	स्थिती VAR_FLOAT:
	स्थिती VAR_SPECIAL:
	स्थिती VAR_STRING:
	स्थिती VAR_INSTR:
	    थांबा;

	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    जर (argvars[0].vval.v_job != रिक्त)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    थांबा;
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    जर (argvars[0].vval.v_channel != रिक्त)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    थांबा;
	स्थिती VAR_FUNC:
	    जर (argvars[0].vval.v_string != रिक्त)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		जर (fp != रिक्त)
		    retval = fp->uf_refcount;
	    }
	    थांबा;
	स्थिती VAR_PARTIAL:
	    जर (argvars[0].vval.v_partial != रिक्त)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    थांबा;
	स्थिती VAR_BLOB:
	    जर (argvars[0].vval.v_blob != रिक्त)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    थांबा;
	स्थिती VAR_LIST:
	    जर (argvars[0].vval.v_list != रिक्त)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    थांबा;
	स्थिती VAR_TUPLE:
	    जर (argvars[0].vval.v_tuple != रिक्त)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    थांबा;
	स्थिती VAR_DICT:
	    जर (argvars[0].vval.v_dict != रिक्त)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    थांबा;
	स्थिती VAR_CLASS:
	    जर (argvars[0].vval.v_class != रिक्त)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    थांबा;
	स्थिती VAR_OBJECT:
	    जर (argvars[0].vval.v_object != रिक्त)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    थांबा;
	स्थिती VAR_TYPEALIAS:
	    जर (argvars[0].vval.v_typealias != रिक्त)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    थांबा;
    }

    rettv->v_type = VAR_संख्या;
    rettv->vval.v_संख्या = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    शून्य
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    जर (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    अन्यथा
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
    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    शून्य
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = रिक्त;
}

#जर defined(FEAT_JOB_CHANNEL)
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

#जर defined(FEAT_JOB_CHANNEL)
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
    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[0].v_type != VAR_संख्या || argvars[1].v_type != VAR_संख्या)
    {
	emsg(_(e_invalid_argument));
	परत;
    }

    mouse_row = (समय_प्रकार)tv_get_संख्या(&argvars[0]) - 1;
    mouse_col = (समय_प्रकार)tv_get_संख्या(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    स्थिर पूर्णांक
test_gui_drop_files(dict_T *args UNUSED)
{
# जर defined(HAVE_DROP_FILE)
    पूर्णांक		row;
    पूर्णांक		col;
    int_u	mods;
    char_u	**fnames;
    पूर्णांक		मोजणी = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    जर (!dict_has_किल्ली(args, "files")
	    || !dict_has_किल्ली(args, "row")
	    || !dict_has_किल्ली(args, "col")
	    || !dict_has_किल्ली(args, "modifiers"))
	परत FALSE;

    (शून्य)dict_get_tv(args, "files", &t);
    row = (पूर्णांक)dict_get_संख्या(args, "row");
    col = (पूर्णांक)dict_get_संख्या(args, "col");
    mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");

    जर (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	परत FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    जर (fnames == रिक्त)
	परत FALSE;

    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	// ignore non-string items
	जर (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == रिक्त)
	    सुरूठेवा;

	fnames[मोजणी] = vim_strsave(li->li_tv.vval.v_string);
	जर (fnames[मोजणी] == रिक्त)
	{
	    पर्यंत (--मोजणी >= 0)
		vim_free(fnames[मोजणी]);
	    vim_free(fnames);
	    परत FALSE;
	}
	मोजणी++;
    }

    जर (मोजणी > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, मोजणी);
    अन्यथा
	vim_free(fnames);
# endif

    परत TRUE;
}

# जर defined(FIND_REPLACE_DIALOG)
    स्थिर पूर्णांक
test_gui_find_repl(dict_T *args)
{
    पूर्णांक		flags;
    char_u	*find_text;
    char_u	*repl_text;
    पूर्णांक		forward;
    पूर्णांक		retval;

    जर (!dict_has_किल्ली(args, "find_text")
	    || !dict_has_किल्ली(args, "repl_text")
	    || !dict_has_किल्ली(args, "flags")
	    || !dict_has_किल्ली(args, "forward"))
	परत FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (पूर्णांक)dict_get_संख्या(args, "flags");
    forward = (पूर्णांक)dict_get_संख्या(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    परत retval;
}
# endif

    स्थिर पूर्णांक
test_gui_mouse_event(dict_T *args)
{
    पूर्णांक		button;
    पूर्णांक		row;
    पूर्णांक		col;
    पूर्णांक		repeated_click;
    int_u	mods;
    पूर्णांक		move;

    जर (!dict_has_किल्ली(args, "row")
	    || !dict_has_किल्ली(args, "col"))
	परत FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (पूर्णांक)dict_get_bool(args, "move", FALSE);

    जर (!move && (!dict_has_किल्ली(args, "button")
	    || !dict_has_किल्ली(args, "multiclick")
	    || !dict_has_किल्ली(args, "modifiers")))
	परत FALSE;

    row = (पूर्णांक)dict_get_संख्या(args, "row");
    col = (पूर्णांक)dict_get_संख्या(args, "col");

    जर (move)
    {
	पूर्णांक pY = row;
	पूर्णांक pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	जर (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    अन्यथा
    {
	button = (पूर्णांक)dict_get_संख्या(args, "button");
	repeated_click = (पूर्णांक)dict_get_संख्या(args, "multiclick");
	mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    परत TRUE;
}

    स्थिर पूर्णांक
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    दीर्घ	मूल्य;
    पूर्णांक		dragging;
    scrollbar_T *sb = रिक्त;

    जर (!dict_has_किल्ली(args, "which")
	    || !dict_has_किल्ली(args, "value")
	    || !dict_has_किल्ली(args, "dragging"))
	परत FALSE;

    which = dict_get_string(args, "which", FALSE);
    मूल्य = (दीर्घ)dict_get_संख्या(args, "value");
    dragging = (पूर्णांक)dict_get_संख्या(args, "dragging");

    जर (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_डावा];
    अन्यथा जर (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_उजवा];
    अन्यथा जर (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    जर (sb == रिक्त)
    {
	semsg(_(e_invalid_argument_str), which);
	परत FALSE;
    }
    gui_drag_scrollbar(sb, मूल्य, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    परत TRUE;
}

    स्थिर पूर्णांक
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    पूर्णांक		tabnr;

    जर (!dict_has_किल्ली(args, "tabnr"))
	परत FALSE;

    tabnr = (पूर्णांक)dict_get_संख्या(args, "tabnr");

    परत send_tabline_event(tabnr);
# अन्यथा
    परत FALSE;
# endif
}

    स्थिर पूर्णांक
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    पूर्णांक	tabnr;
    पूर्णांक	item;

    जर (!dict_has_किल्ली(args, "tabnr")
	    || !dict_has_किल्ली(args, "item"))
	परत FALSE;

    tabnr = (पूर्णांक)dict_get_संख्या(args, "tabnr");
    item = (पूर्णांक)dict_get_संख्या(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    परत TRUE;
}
#endif

    शून्य
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = FALSE;

    जर (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	परत;
    }

    जर (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == रिक्त)
	परत;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_संख्या = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    शून्य
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = FALSE;

    जर (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	परत;
    }

    जर (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == रिक्त)
	परत;

    event = tv_get_string(&argvars[0]);
    जर (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_संख्या = test_gui_drop_files(argvars[1].vval.v_dict);
# जर defined(FIND_REPLACE_DIALOG)
    अन्यथा जर (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_संख्या = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    अन्यथा जर (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_संख्या = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    अन्यथा जर (STRCMP(event, "mouse") == 0)
	rettv->vval.v_संख्या = test_gui_mouse_event(argvars[1].vval.v_dict);
    अन्यथा जर (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_संख्या = test_gui_scrollbar(argvars[1].vval.v_dict);
    अन्यथा जर (STRCMP(event, "tabline") == 0)
	rettv->vval.v_संख्या = test_gui_tabline_event(argvars[1].vval.v_dict);
    अन्यथा जर (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_संख्या = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    अन्यथा
    {
	semsg(_(e_invalid_argument_str), event);
	परत;
    }
#endif
}

    शून्य
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    जर (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	परत;

    time_for_testing = (समय_प्रकार)tv_get_संख्या(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
