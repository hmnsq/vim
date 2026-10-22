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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ಯದಿ (sname != ರಿಕ್ತ)
    {
	ga_concat(gap, sname);
	ಯದಿ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ಯದಿ (SOURCING_LNUM > 0)
    {
	ವರ್ಣ    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, ಪರಿಮಾಣ(buf), "line %ld", (long)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ಯದಿ (sname != ರಿಕ್ತ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ga_concat_esc(garray_T *gap, char_u *p, ಪೂರ್ಣಾಙ್ಕ clen)
{
    char_u  buf[NUMBUFLEN];

    ಯದಿ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ವಿಕಲ್ಪ (*p)
    {
	ಪಕ್ಷ BS: GA_CONCAT_LITERAL(gap, "\\b"); ಭಙ್ಗ;
	ಪಕ್ಷ ESC: GA_CONCAT_LITERAL(gap, "\\e"); ಭಙ್ಗ;
	ಪಕ್ಷ FF: GA_CONCAT_LITERAL(gap, "\\f"); ಭಙ್ಗ;
	ಪಕ್ಷ NL: GA_CONCAT_LITERAL(gap, "\\n"); ಭಙ್ಗ;
	ಪಕ್ಷ TAB: GA_CONCAT_LITERAL(gap, "\\t"); ಭಙ್ಗ;
	ಪಕ್ಷ CAR: GA_CONCAT_LITERAL(gap, "\\r"); ಭಙ್ಗ;
	ಪಕ್ಷ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	   ಯದಿ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ಅನ್ಯಥಾ
	       ga_append(gap, *p);
	   ಭಙ್ಗ;
    }
}

/*
 * Append "str" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ga_concat_shorten_esc(garray_T *gap, char_u *str)
{
    char_u  *p;
    char_u  *s;
    ಪೂರ್ಣಾಙ್ಕ	    c;
    ಪೂರ್ಣಾಙ್ಕ	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    ಪೂರ್ಣಾಙ್ಕ	    same_len;

    ಯದಿ (str == ರಿಕ್ತ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ಯಾವತ್ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ಯದಿ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	ಅನ್ಯಥಾ
	{
	    ga_concat_esc(gap, p, clen);
	    p += clen;
	}
    }
}

/*
 * Fill "gap" with information about an assert error.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_ಸಂದೇಶ_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    ಪೂರ್ಣಾಙ್ಕ		did_copy = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		omitted = 0;

    ಯದಿ (opt_ಸಂದೇಶ_tv->v_type != VAR_UNKNOWN
	    && !(opt_ಸಂದೇಶ_tv->v_type == VAR_STRING
		&& (opt_ಸಂದೇಶ_tv->vval.v_string == ರಿಕ್ತ
		    || *opt_ಸಂದೇಶ_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_ಸಂದೇಶ_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ಯದಿ (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ಅನ್ಯಥಾ ಯದಿ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ಅನ್ಯಥಾ
	GA_CONCAT_LITERAL(gap, "Expected ");
    ಯದಿ (exp_str == ರಿಕ್ತ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ಯದಿ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ರಿಕ್ತ && got_tv->vval.v_dict != ರಿಕ್ತ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    ಪೂರ್ಣಾಙ್ಕ		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ಯದಿ (exp_tv->vval.v_dict == ರಿಕ್ತ || got_tv->vval.v_dict == ರಿಕ್ತ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ರಿಕ್ತ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ರಿಕ್ತ;
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    todo = (ಪೂರ್ಣಾಙ್ಕ)exp_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&exp_d->dv_hashtab, hi, todo)
	    {
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_ಕೀಲಿ, -1);
		    ಯದಿ (item2 == ರಿಕ್ತ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(ವರ್ಣ *)hi->hi_ಕೀಲಿ, &HI2DI(hi)->di_tv);
			ಯದಿ (item2 != ರಿಕ್ತ)
			    dict_add_tv(got_tv->vval.v_dict,
					    (ವರ್ಣ *)hi->hi_ಕೀಲಿ, &item2->di_tv);
		    }
		    ಅನ್ಯಥಾ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (ಪೂರ್ಣಾಙ್ಕ)got_d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&got_d->dv_hashtab, hi, todo)
	    {
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_ಕೀಲಿ, -1);
		    ಯದಿ (item2 == ರಿಕ್ತ)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(ವರ್ಣ *)hi->hi_ಕೀಲಿ, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ಯದಿ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ಯದಿ (atype != ASSERT_NOTEQUAL)
    {
	ಯದಿ (atype == ASSERT_MATCH)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ಅನ್ಯಥಾ ಯದಿ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ಅನ್ಯಥಾ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ಯದಿ (omitted != 0)
	{
	    ವರ್ಣ    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, ಪರಿಮಾಣ(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ಯದಿ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ಯದಿ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ರಿಕ್ತ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *text = tv_get_string_buf_chk(&argvars[1], buf2);
    ಯದಿ (pat != ರಿಕ್ತ && text != ರಿಕ್ತ
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ರಿಕ್ತ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_bool(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ isTrue)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    garray_T	ga;

    ಯದಿ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_ಸಂಖ್ಯೆ == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ
	    || (tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ರಿಕ್ತ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ಅನ್ಯಥಾ
	ga_concat(gap, cmd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_beeps(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		ret = 0;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ಯದಿ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ಯದಿ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	ಅನ್ಯಥಾ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    ಶೂನ್ಯ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    ಶೂನ್ಯ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_beeps(argvars, TRUE);
}

/*
 * "assert_equal(expected, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_equal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_equal_common(argvars, ASSERT_EQUAL);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    ವರ್ಣ	line1[200];
    ವರ್ಣ	line2[200];
    ಪೂರ್ಣಾಙ್ಕ		lineidx = 0;
    size_t	IObufflen;

    ಯದಿ (fname1 == ರಿಕ್ತ || fname2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((ವರ್ಣ *)fname1, READBIN);
    ಯದಿ (fd1 == ರಿಕ್ತ)
    {
	IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE,
	    (ವರ್ಣ *)e_cant_read_file_str, fname1);
    }
    ಅನ್ಯಥಾ
    {
	fd2 = mch_fopen((ವರ್ಣ *)fname2, READBIN);
	ಯದಿ (fd2 == ರಿಕ್ತ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE,
		(ವರ್ಣ *)e_cant_read_file_str, fname2);
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ	    c1, c2;
	    long    ಎಣಿಕೆ = 0;
	    long    linecount = 1;

	    ಪ್ರತಿ (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ಯದಿ (c1 == EOF)
		{
		    ಯದಿ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ಯದಿ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ಎಣಿಕೆ, linecount);
			ಭಙ್ಗ;
		    }
		}
		++ಎಣಿಕೆ;
		ಯದಿ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ಅನ್ಯಥಾ ಯದಿ (lineidx + 2 == (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(line1))
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

    ಯದಿ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ಯದಿ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ಯದಿ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_equalfile(argvars);
}

/*
 * "assert_notequal(expected, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_notequal(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_equal_common(argvars, ASSERT_NOTEQUAL);
}

/*
 * "assert_exception(string[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_exception(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*error;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    error = tv_get_string_chk(&argvars[0]);
    ಯದಿ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (error != ರಿಕ್ತ
	&& strstr((ವರ್ಣ *)get_vim_var_str(VV_EXCEPTION), (ವರ್ಣ *)error) == ರಿಕ್ತ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ರಿಕ್ತ, &argvars[0],
				  get_vim_var_tv(VV_EXCEPTION), ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    }
}

/*
 * "assert_fails(cmd [, error[, msg]])" function
 */
    ಶೂನ್ಯ
f_assert_fails(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		save_trylevel = trylevel;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    ವರ್ಣ	*wrong_arg_ಸಂದೇಶ = ರಿಕ್ತ;
    char_u	*tofree = ರಿಕ್ತ;

    ಯದಿ (check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯದಿ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	error_found = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	error_found_index = 1;
	char_u	*actual = emsg_assert_fails_ಸಂದೇಶ == ರಿಕ್ತ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_ಸಂದೇಶ;

	ಯದಿ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_found = expected == ರಿಕ್ತ
			   || strstr((ವರ್ಣ *)actual, (ವರ್ಣ *)expected) == ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ಯದಿ (list == ರಿಕ್ತ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_ಸಂದೇಶ = e_assert_fails_second_arg;
		ಗಚ್ಛ theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_first->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ಯದಿ (expected == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ಯದಿ (!pattern_match(expected, actual, FALSE))
	    {
		error_found = TRUE;
		expected_str = expected;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ಯದಿ (actual != ರಿಕ್ತ)
		{
		    tv = &list->lv_u.mat.lv_last->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ಯದಿ (expected == ರಿಕ್ತ)
			ಗಚ್ಛ theend;
		    ಯದಿ (!pattern_match(expected, actual, FALSE))
		    {
			error_found = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    wrong_arg_ಸಂದೇಶ = e_assert_fails_second_arg;
	    ಗಚ್ಛ theend;
	}

	ಯದಿ (!error_found && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (argvars[3].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		wrong_arg_ಸಂದೇಶ = e_assert_fails_fourth_argument;
		ಗಚ್ಛ theend;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (argvars[3].vval.v_ಸಂಖ್ಯೆ >= 0
			 && argvars[3].vval.v_ಸಂಖ್ಯೆ != emsg_assert_fails_lnum)
	    {
		error_found = TRUE;
		error_found_index = 3;
	    }
	    ಯದಿ (!error_found && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ಯದಿ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_ಸಂದೇಶ = e_assert_fails_fifth_argument;
		    ಗಚ್ಛ theend;
		}
		ಅನ್ಯಥಾ ಯದಿ (argvars[4].vval.v_string != ರಿಕ್ತ
		    && !pattern_match(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_found = TRUE;
		    error_found_index = 4;
		}
	    }
	}

	ಯದಿ (error_found)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ಯದಿ (error_found_index == 3)
	    {
		actual_tv.v_type = VAR_ಸಂಖ್ಯೆ;
		actual_tv.vval.v_ಸಂಖ್ಯೆ = emsg_assert_fails_lnum;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (error_found_index == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ಅನ್ಯಥಾ
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
	    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	}
    }

theend:
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;
    in_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    ಸಂದೇಶ_col = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    ಸಂದೇಶ_scrolled = 0;
    ಸಾಲುಗಳು_ಎಡ = Rows;
    VIM_CLEAR(emsg_assert_fails_ಸಂದೇಶ);
    vim_free(tofree);
    set_vim_var_string(VV_ERRMSG, ರಿಕ್ತ, 0);
    ಯದಿ (wrong_arg_ಸಂದೇಶ != ರಿಕ್ತ)
	emsg(_(wrong_arg_ಸಂದೇಶ));
}

/*
 * "assert_false(actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_false(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_bool(argvars, FALSE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    char_u	expected_str[200];

    ಯದಿ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ಯದಿ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ವರ್ಣ *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ರಿಕ್ತ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
    }
    ಅನ್ಯಥಾ
    {
	varnumber_T	lower = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);

	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ವರ್ಣ *)expected_str, 200, "range %ld - %ld,",
						     (long)lower, (long)upper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ರಿಕ್ತ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_match(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_match_common(argvars, ASSERT_MATCH);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_match_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    ಶೂನ್ಯ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

/*
 * "assert_true(actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_true(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_bool(argvars, TRUE);
}

/*
 * "test_alloc_fail(id, countdown, repeat)" function
 */
    ಶೂನ್ಯ
f_test_alloc_fail(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ
	    || argvars[0].vval.v_ಸಂಖ್ಯೆ <= 0
	    || argvars[1].v_type != VAR_ಸಂಖ್ಯೆ
	    || argvars[1].vval.v_ಸಂಖ್ಯೆ < 0
	    || argvars[2].v_type != VAR_ಸಂಖ್ಯೆ)
	emsg(_(e_invalid_argument));
    ಅನ್ಯಥಾ
    {
	alloc_fail_id = argvars[0].vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (alloc_fail_id >= aid_last)
	    emsg(_(e_invalid_argument));
	alloc_fail_countdown = argvars[1].vval.v_ಸಂಖ್ಯೆ;
	alloc_fail_repeat = argvars[2].vval.v_ಸಂಖ್ಯೆ;
	did_outofmem_ಸಂದೇಶ = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    ಶೂನ್ಯ
f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ಯದಿ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}

/*
 * "test_feedinput()"
 */
    ಶೂನ್ಯ
f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_ಒಳಹರಿವು_BUF
    char_u	*val;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಯದಿ (val != ರಿಕ್ತ)
    {
	trash_ಒಳಹರಿವು_buf();
	add_to_ಒಳಹರಿವು_buf_csi(val, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(val));
    }
#endif
}

/*
 * "test_getvalue({name})" function
 */
    ಶೂನ್ಯ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);

    ಯದಿ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = need_fileinfo;
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    ಶೂನ್ಯ
f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (reset_option_was_set(name) == FAIL)
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    ಶೂನ್ಯ
f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    ಪೂರ್ಣಾಙ್ಕ     val;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_starting = -1;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    val = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

    ಯದಿ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"starting") == 0)
    {
	ಯದಿ (val)
	{
	    ಯದಿ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ಅನ್ಯಥಾ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_code_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_ಸಾಲುಗಳು = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, (char_u *)"ALL") == 0)
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
	ಯದಿ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    ಶೂನ್ಯ
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ retval = -1;

    ವಿಕಲ್ಪ (argvars[0].v_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_INSTR:
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (argvars[0].vval.v_job != ರಿಕ್ತ)
		retval = argvars[0].vval.v_job->jv_refcount - 1;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (argvars[0].vval.v_channel != ರಿಕ್ತ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FUNC:
	    ಯದಿ (argvars[0].vval.v_string != ರಿಕ್ತ)
	    {
		ufunc_T *fp;

		fp = find_func(argvars[0].vval.v_string, FALSE);
		ಯದಿ (fp != ರಿಕ್ತ)
		    retval = fp->uf_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (argvars[0].vval.v_partial != ರಿಕ್ತ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ (argvars[0].vval.v_blob != ರಿಕ್ತ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (argvars[0].vval.v_list != ರಿಕ್ತ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (argvars[0].vval.v_tuple != ರಿಕ್ತ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ (argvars[0].vval.v_dict != ರಿಕ್ತ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	    ಯದಿ (argvars[0].vval.v_class != ರಿಕ್ತ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    ಯದಿ (argvars[0].vval.v_object != ರಿಕ್ತ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಯದಿ (argvars[0].vval.v_typealias != ರಿಕ್ತ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ಭಙ್ಗ;
    }

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    ಶೂನ್ಯ
f_test_garbagecollect_now(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ಯದಿ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_set));
    ಅನ್ಯಥಾ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    ಶೂನ್ಯ
f_test_garbagecollect_soon(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    ಶೂನ್ಯ
f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    ಶೂನ್ಯ
f_test_null_blob(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
f_test_null_channel(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ರಿಕ್ತ;
}
#endif

    ಶೂನ್ಯ
f_test_null_dict(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_dict_set(rettv, ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
f_test_null_job(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = ರಿಕ್ತ;
}
#endif

    ಶೂನ್ಯ
f_test_null_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, ರಿಕ್ತ);
}

    ಶೂನ್ಯ
f_test_null_function(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
f_test_null_partial(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
f_test_null_string(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
f_test_null_tuple(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_tuple_set(rettv, ರಿಕ್ತ);
}

    ಶೂನ್ಯ
f_test_unknown(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    ಶೂನ್ಯ
f_test_void(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    ಶೂನ್ಯ
f_test_setmouse(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ || argvars[1].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    mouse_row = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;
    mouse_col = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;
}

#ifdef FEAT_GUI
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_drop_files(dict_T *args UNUSED)
{
# ಯದಿ defined(HAVE_DROP_FILE)
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    int_u	mods;
    char_u	**fnames;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "files")
	    || !dict_has_ಕೀಲಿ(args, "row")
	    || !dict_has_ಕೀಲಿ(args, "col")
	    || !dict_has_ಕೀಲಿ(args, "modifiers"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    (ಶೂನ್ಯ)dict_get_tv(args, "files", &t);
    row = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "row");
    col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "col");
    mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");

    ಯದಿ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ಯದಿ (fnames == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	// ignore non-string items
	ಯದಿ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	fnames[ಎಣಿಕೆ] = vim_strsave(li->li_tv.vval.v_string);
	ಯದಿ (fnames[ಎಣಿಕೆ] == ರಿಕ್ತ)
	{
	    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
		vim_free(fnames[ಎಣಿಕೆ]);
	    vim_free(fnames);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಎಣಿಕೆ++;
    }

    ಯದಿ (ಎಣಿಕೆ > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, ಎಣಿಕೆ);
    ಅನ್ಯಥಾ
	vim_free(fnames);
# endif

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ಯದಿ defined(FIND_REPLACE_DIALOG)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_find_repl(dict_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ		flags;
    char_u	*find_text;
    char_u	*repl_text;
    ಪೂರ್ಣಾಙ್ಕ		forward;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "find_text")
	    || !dict_has_ಕೀಲಿ(args, "repl_text")
	    || !dict_has_ಕೀಲಿ(args, "flags")
	    || !dict_has_ಕೀಲಿ(args, "forward"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "flags");
    forward = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    ಪ್ರತ್ಯಾಗಮ retval;
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_mouse_event(dict_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ		button;
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		repeated_click;
    int_u	mods;
    ಪೂರ್ಣಾಙ್ಕ		move;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "row")
	    || !dict_has_ಕೀಲಿ(args, "col"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ಪೂರ್ಣಾಙ್ಕ)dict_get_bool(args, "move", FALSE);

    ಯದಿ (!move && (!dict_has_ಕೀಲಿ(args, "button")
	    || !dict_has_ಕೀಲಿ(args, "multiclick")
	    || !dict_has_ಕೀಲಿ(args, "modifiers")))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    row = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "row");
    col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "col");

    ಯದಿ (move)
    {
	ಪೂರ್ಣಾಙ್ಕ pY = row;
	ಪೂರ್ಣಾಙ್ಕ pX = col;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ಯದಿ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    pX = (col - 1) * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ಅನ್ಯಥಾ
    {
	button = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "button");
	repeated_click = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "multiclick");
	mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ		dragging;
    scrollbar_T *sb = ರಿಕ್ತ;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "which")
	    || !dict_has_ಕೀಲಿ(args, "value")
	    || !dict_has_ಕೀಲಿ(args, "dragging"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    which = dict_get_string(args, "which", FALSE);
    ಮೌಲ್ಯ = (long)dict_get_ಸಂಖ್ಯೆ(args, "value");
    dragging = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "dragging");

    ಯದಿ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ಎಡ];
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ಬಲ];
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ಯದಿ (sb == ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str), which);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
# endif

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_tabline_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    ಪೂರ್ಣಾಙ್ಕ		tabnr;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "tabnr"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    tabnr = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "tabnr");

    ಪ್ರತ್ಯಾಗಮ send_tabline_event(tabnr);
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
# endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_tabmenu_event(dict_T *args UNUSED)
{
# ifdef FEAT_GUI_TABLINE
    ಪೂರ್ಣಾಙ್ಕ	tabnr;
    ಪೂರ್ಣಾಙ್ಕ	item;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "tabnr")
	    || !dict_has_ಕೀಲಿ(args, "item"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    tabnr = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "tabnr");
    item = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "item");

    send_tabline_menu_event(tabnr, item);
# endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

    ಶೂನ್ಯ
f_test_mswin_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಯದಿ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = test_mswin_event(event, argvars[1].vval.v_dict);

#endif
}

    ಶೂನ್ಯ
f_test_gui_event(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಯದಿ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    event = tv_get_string(&argvars[0]);
    ಯದಿ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_drop_files(argvars[1].vval.v_dict);
# ಯದಿ defined(FIND_REPLACE_DIALOG)
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_find_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_mouse_event(argvars[1].vval.v_dict);
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_scrollbar(argvars[1].vval.v_dict);
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_tabline_event(argvars[1].vval.v_dict);
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str), event);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
}

    ಶೂನ್ಯ
f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    time_for_testing = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
}

#endif // defined(FEAT_EVAL)
