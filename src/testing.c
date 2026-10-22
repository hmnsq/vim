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

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Prepare "gap" for an assert error and add the sourcing position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_assert_error(garray_T *gap)
{
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    ಆದರೆ (sname != ಖಾಲಿ)
    {
	ga_concat(gap, sname);
	ಆದರೆ (SOURCING_LNUM > 0)
	    GA_CONCAT_LITERAL(gap, " ");
    }
    ಆದರೆ (SOURCING_LNUM > 0)
    {
	ಅಕ್ಷರ    buf[NUMBUFLEN];
	size_t	buflen;

	buflen = vim_snprintf_safelen(buf, ಗಾತ್ರ(buf), "line %ld", (ದೀರ್ಘ)SOURCING_LNUM);
	ga_concat_len(gap, (char_u *)buf, buflen);
    }
    ಆದರೆ (sname != ಖಾಲಿ || SOURCING_LNUM > 0)
	GA_CONCAT_LITERAL(gap, ": ");
    vim_free(sname);
}

/*
 * Append "p[clen]" to "gap", escaping unprintable characters.
 * Changes NL to \n, CR to \r, etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ga_concat_esc(garray_T *gap, char_u *p, ಪೂರ್ಣಾಂಕ clen)
{
    char_u  buf[NUMBUFLEN];

    ಆದರೆ (clen > 1)
    {
	mch_memmove(buf, p, clen);
	buf[clen] = NUL;
	ga_concat_len(gap, buf, clen);
	ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (*p)
    {
	ಸ್ಥಿತಿ BS: GA_CONCAT_LITERAL(gap, "\\b"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ESC: GA_CONCAT_LITERAL(gap, "\\e"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FF: GA_CONCAT_LITERAL(gap, "\\f"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NL: GA_CONCAT_LITERAL(gap, "\\n"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TAB: GA_CONCAT_LITERAL(gap, "\\t"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CAR: GA_CONCAT_LITERAL(gap, "\\r"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '\\': GA_CONCAT_LITERAL(gap, "\\\\"); ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	   ಆದರೆ (*p < ' ' || *p == 0x7f)
	   {
		size_t	buflen;

		buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf),
		    "\\x%02x", *p);
		ga_concat_len(gap, buf, buflen);
	   }
	   ಇಲ್ಲದಿದ್ದರೆ
	       ga_append(gap, *p);
	   ನಿಲ್ಲಿಸು;
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
    ಪೂರ್ಣಾಂಕ	    c;
    ಪೂರ್ಣಾಂಕ	    clen;
    char_u  buf[NUMBUFLEN];
    size_t  buflen;
    ಪೂರ್ಣಾಂಕ	    same_len;

    ಆದರೆ (str == ಖಾಲಿ)
    {
	GA_CONCAT_LITERAL(gap, "NULL");
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (p = str; *p != NUL; )
    {
	same_len = 1;
	s = p;
	c = mb_cptr2char_adv(&s);
	clen = s - p;
	ಆಗ (*s != NUL && c == mb_ptr2char(s))
	{
	    ++same_len;
	    s += clen;
	}
	ಆದರೆ (same_len > 20)
	{
	    GA_CONCAT_LITERAL(gap, "\\[");
	    ga_concat_esc(gap, p, clen);
	    GA_CONCAT_LITERAL(gap, " occurs ");
	    buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf),
		"%d", same_len);
	    ga_concat_len(gap, buf, buflen);
	    GA_CONCAT_LITERAL(gap, " times]");
	    p = s;
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ		did_copy = FALSE;
    ಪೂರ್ಣಾಂಕ		omitted = 0;

    ಆದರೆ (opt_ಸಂದೇಶ_tv->v_type != VAR_UNKNOWN
	    && !(opt_ಸಂದೇಶ_tv->v_type == VAR_STRING
		&& (opt_ಸಂದೇಶ_tv->vval.v_string == ಖಾಲಿ
		    || *opt_ಸಂದೇಶ_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_ಸಂದೇಶ_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	GA_CONCAT_LITERAL(gap, ": ");
    }

    ಆದರೆ (atype == ASSERT_ಹೊಂದಿಕೆ || atype == ASSERT_NOTMATCH)
	GA_CONCAT_LITERAL(gap, "Pattern ");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (atype == ASSERT_NOTEQUAL)
	GA_CONCAT_LITERAL(gap, "Expected not equal to ");
    ಇಲ್ಲದಿದ್ದರೆ
	GA_CONCAT_LITERAL(gap, "Expected ");
    ಆದರೆ (exp_str == ಖಾಲಿ)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	ಆದರೆ (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != ಖಾಲಿ && got_tv->vval.v_dict != ಖಾಲಿ)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    ಪೂರ್ಣಾಂಕ		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    ಆದರೆ (exp_tv->vval.v_dict == ಖಾಲಿ || got_tv->vval.v_dict == ಖಾಲಿ)
	    {
		dict_unref(exp_tv->vval.v_dict);
		exp_tv->vval.v_dict = ಖಾಲಿ;
		dict_unref(got_tv->vval.v_dict);
		got_tv->vval.v_dict = ಖಾಲಿ;
		ಹಿಂತಿರುಗಿ;
	    }

	    todo = (ಪೂರ್ಣಾಂಕ)exp_d->dv_hashtab.ht_ಬಳಸಿದ;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&exp_d->dv_hashtab, hi, todo)
	    {
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    item2 = dict_ಕಂಡುಹಿಡಿ(got_d, hi->hi_ಕೀಲಿ, -1);
		    ಆದರೆ (item2 == ಖಾಲಿ || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_ಕೂಡಿಸು_tv(exp_tv->vval.v_dict,
					(ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ, &HI2DI(hi)->di_tv);
			ಆದರೆ (item2 != ಖಾಲಿ)
			    dict_ಕೂಡಿಸು_tv(got_tv->vval.v_dict,
					    (ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ, &item2->di_tv);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (ಪೂರ್ಣಾಂಕ)got_d->dv_hashtab.ht_ಬಳಸಿದ;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&got_d->dv_hashtab, hi, todo)
	    {
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    item2 = dict_ಕಂಡುಹಿಡಿ(exp_d, hi->hi_ಕೀಲಿ, -1);
		    ಆದರೆ (item2 == ಖಾಲಿ)
			// item of got_d not present in exp_d
			dict_ಕೂಡಿಸು_tv(got_tv->vval.v_dict,
					(ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
	ga_concat_shorten_esc(gap, exp_str);
	ಆದರೆ (atype == ASSERT_FAILS)
	    GA_CONCAT_LITERAL(gap, "'");
    }
    ಆದರೆ (atype != ASSERT_NOTEQUAL)
    {
	ಆದರೆ (atype == ASSERT_ಹೊಂದಿಕೆ)
	    GA_CONCAT_LITERAL(gap, " does not match ");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (atype == ASSERT_NOTMATCH)
	    GA_CONCAT_LITERAL(gap, " does match ");
	ಇಲ್ಲದಿದ್ದರೆ
	    GA_CONCAT_LITERAL(gap, " but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	ಆದರೆ (omitted != 0)
	{
	    ಅಕ್ಷರ    buf[100];
	    size_t  buflen;

	    buflen = vim_snprintf_safelen(buf, ಗಾತ್ರ(buf),
		" - %d equal item%s omitted", omitted, omitted == 1 ? "" : "s");
	    ga_concat_len(gap, (char_u *)buf, buflen);
	}
    }

    ಆದರೆ (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    ಆದರೆ (tv_equal(&argvars[0], &argvars[1], FALSE)
						   != (atype == ASSERT_EQUAL))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ಖಾಲಿ, &argvars[0], &argvars[1],
								       atype);
	assert_error(&ga);
	ga_clear(&ga);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_ಹೊಂದಿಕೆ_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ 1;

    char_u *pat = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u *ಪಠ್ಯ = tv_get_string_buf_chk(&argvars[1], buf2);
    ಆದರೆ (pat != ಖಾಲಿ && ಪಠ್ಯ != ಖಾಲಿ
		 && ನಮೂನೆ_ಹೊಂದಿಕೆ(pat, ಪಠ್ಯ, FALSE) != (atype == ASSERT_ಹೊಂದಿಕೆ))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], ಖಾಲಿ, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Common for assert_true() and assert_false().
 * Return non-zero for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_bool(typval_T *argvars, ಪೂರ್ಣಾಂಕ isTrue)
{
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    garray_T	ga;

    ಆದರೆ (argvars[0].v_type == VAR_BOOL
	    && argvars[0].vval.v_ಸಂಖ್ಯೆ == (isTrue ? VVAL_TRUE : VVAL_FALSE))
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ
	    || (tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error) == 0) == isTrue
	    || error)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1],
		(char_u *)(isTrue ? "True" : "False"),
		ಖಾಲಿ, &argvars[0], ASSERT_OTHER);
	assert_error(&ga);
	ga_clear(&ga);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
assert_append_cmd_or_arg(garray_T *gap, typval_T *argvars, char_u *cmd)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	ga_concat(gap, echo_string(&argvars[2], &tofree, numbuf, 0));
	vim_free(tofree);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ga_concat(gap, cmd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_beeps(typval_T *argvars, ಪೂರ್ಣಾಂಕ no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		ret = 0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    ಆದರೆ (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	ಆದರೆ (no_beep)
	    GA_CONCAT_LITERAL(&ga, "command did beep: ");
	ಇಲ್ಲದಿದ್ದರೆ
	    GA_CONCAT_LITERAL(&ga, "command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * "assert_beeps(cmd)" function
 */
    ಶೂನ್ಯ
f_assert_beeps(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_beeps(argvars, FALSE);
}

/*
 * "assert_nobeep(cmd)" function
 */
    ಶೂನ್ಯ
f_assert_nobeep(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

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

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    FILE	*fd1;
    FILE	*fd2;
    ಅಕ್ಷರ	line1[200];
    ಅಕ್ಷರ	line2[200];
    ಪೂರ್ಣಾಂಕ		lineidx = 0;
    size_t	IObufflen;

    ಆದರೆ (fname1 == ಖಾಲಿ || fname2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    IObuff[0] = NUL;
    IObufflen = 0;
    fd1 = mch_fopen((ಅಕ್ಷರ *)fname1, READBIN);
    ಆದರೆ (fd1 == ಖಾಲಿ)
    {
	IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE,
	    (ಅಕ್ಷರ *)e_cant_read_file_str, fname1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fd2 = mch_fopen((ಅಕ್ಷರ *)fname2, READBIN);
	ಆದರೆ (fd2 == ಖಾಲಿ)
	{
	    fclose(fd1);
	    IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE,
		(ಅಕ್ಷರ *)e_cant_read_file_str, fname2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ	    c1, c2;
	    ದೀರ್ಘ    ಎಣಿಕೆ_2 = 0;
	    ದೀರ್ಘ    linecount = 1;

	    ಗಾಗಿ (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		ಆದರೆ (c1 == EOF)
		{
		    ಆದರೆ (c2 != EOF)
		    {
			STRCPY(IObuff, "first file is shorter");
			IObufflen = 21;
		    }
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    IObufflen = 22;
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    ಆದರೆ (c1 != c2)
		    {
			IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE,
			    "difference at byte %ld, line %ld", ಎಣಿಕೆ_2, linecount);
			ನಿಲ್ಲಿಸು;
		    }
		}
		++ಎಣಿಕೆ_2;
		ಆದರೆ (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lineidx + 2 == (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(line1))
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

    ಆದರೆ (IObufflen > 0)
    {
	garray_T	ga;
	prepare_assert_error(&ga);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    GA_CONCAT_LITERAL(&ga, ": ");
	}
	ga_concat_len(&ga, IObuff, IObufflen);
	ಆದರೆ (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    GA_CONCAT_LITERAL(&ga, " after \"");
	    ga_concat_len(&ga, (char_u *)line1, lineidx);
	    ಆದರೆ (STRCMP(line1, line2) != 0)
	    {
		GA_CONCAT_LITERAL(&ga, "\" vs \"");
		ga_concat_len(&ga, (char_u *)line2, lineidx);
	    }
	    GA_CONCAT_LITERAL(&ga, "\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	ಹಿಂತಿರುಗಿ 1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * "assert_equalfile(fname-one, fname-two[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_equalfile(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

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

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    error = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (*get_vim_var_str(VV_EXCEPTION) == NUL)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "v:exception is not set");
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error != ಖಾಲಿ
	&& ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)get_vim_var_str(VV_EXCEPTION), (ಅಕ್ಷರ *)error) == ಖಾಲಿ)
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[1], ಖಾಲಿ, &argvars[0],
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
    ಪೂರ್ಣಾಂಕ		save_trylevel = trylevel;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    ಅಕ್ಷರ	*wrong_arg_ಸಂದೇಶ = ಖಾಲಿ;
    char_u	*tofree = ಖಾಲಿ;

    ಆದರೆ (check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
	    || check_for_opt_string_or_list_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 4) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    // trylevel must be zero for a ":throw" command to be considered failed
    trylevel = 0;
    suppress_errthrow = TRUE;
    ಒಳ_assert_fails = TRUE;
    ++no_wait_return;

    char_u *cmd = tv_get_string_chk(&argvars[0]);
    do_cmdline_cmd(cmd);

    // reset here for any errors reported below
    trylevel = save_trylevel;
    suppress_errthrow = FALSE;

    ಆದರೆ (called_emsg == called_emsg_before)
    {
	prepare_assert_error(&ga);
	GA_CONCAT_LITERAL(&ga, "command did not fail: ");
	assert_append_cmd_or_arg(&ga, argvars, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u	*expected;
	char_u	*expected_str = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	error_ಸಿಕ್ಕಿತು = FALSE;
	ಪೂರ್ಣಾಂಕ	error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ = 1;
	char_u	*actual = emsg_assert_fails_ಸಂದೇಶ == ಖಾಲಿ ? (char_u *)"[unknown]"
						       : emsg_assert_fails_ಸಂದೇಶ;

	ಆದರೆ (argvars[1].v_type == VAR_STRING)
	{
	    expected = tv_get_string_buf_chk(&argvars[1], buf);
	    error_ಸಿಕ್ಕಿತು = expected == ಖಾಲಿ
			   || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)actual, (ಅಕ್ಷರ *)expected) == ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_LIST)
	{
	    list_T	*list = argvars[1].vval.v_list;
	    typval_T	*tv;

	    ಆದರೆ (list == ಖಾಲಿ || list->lv_len < 1 || list->lv_len > 2)
	    {
		wrong_arg_ಸಂದೇಶ = e_assert_fails_ಎರಡನೇ_arg;
		ಹೋಗು theend;
	    }
	    CHECK_LIST_MATERIALIZE(list);
	    tv = &list->lv_ಮೊದಲ->li_tv;
	    expected = tv_get_string_buf_chk(tv, buf);
	    ಆದರೆ (expected == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಆದರೆ (!ನಮೂನೆ_ಹೊಂದಿಕೆ(expected, actual, FALSE))
	    {
		error_ಸಿಕ್ಕಿತು = TRUE;
		expected_str = expected;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list->lv_len == 2)
	    {
		// make a copy, an error in pattern_match() may free it
		tofree = actual = vim_strsave(get_vim_var_str(VV_ERRMSG));
		ಆದರೆ (actual != ಖಾಲಿ)
		{
		    tv = &list->lv_u.mat.lv_ಕೊನೆಯ->li_tv;
		    expected = tv_get_string_buf_chk(tv, buf);
		    ಆದರೆ (expected == ಖಾಲಿ)
			ಹೋಗು theend;
		    ಆದರೆ (!ನಮೂನೆ_ಹೊಂದಿಕೆ(expected, actual, FALSE))
		    {
			error_ಸಿಕ್ಕಿತು = TRUE;
			expected_str = expected;
		    }
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wrong_arg_ಸಂದೇಶ = e_assert_fails_ಎರಡನೇ_arg;
	    ಹೋಗು theend;
	}

	ಆದರೆ (!error_ಸಿಕ್ಕಿತು && argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (argvars[3].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		wrong_arg_ಸಂದೇಶ = e_assert_fails_fourth_ವಾದ;
		ಹೋಗು theend;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[3].vval.v_ಸಂಖ್ಯೆ >= 0
			 && argvars[3].vval.v_ಸಂಖ್ಯೆ != emsg_assert_fails_lnum)
	    {
		error_ಸಿಕ್ಕಿತು = TRUE;
		error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ = 3;
	    }
	    ಆದರೆ (!error_ಸಿಕ್ಕಿತು && argvars[4].v_type != VAR_UNKNOWN)
	    {
		ಆದರೆ (argvars[4].v_type != VAR_STRING)
		{
		    wrong_arg_ಸಂದೇಶ = e_assert_fails_fifth_ವಾದ;
		    ಹೋಗು theend;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[4].vval.v_string != ಖಾಲಿ
		    && !ನಮೂನೆ_ಹೊಂದಿಕೆ(argvars[4].vval.v_string,
					     emsg_assert_fails_context, FALSE))
		{
		    error_ಸಿಕ್ಕಿತು = TRUE;
		    error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ = 4;
		}
	    }
	}

	ಆದರೆ (error_ಸಿಕ್ಕಿತು)
	{
	    typval_T actual_tv;

	    prepare_assert_error(&ga);
	    ಆದರೆ (error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ == 3)
	    {
		actual_tv.v_type = VAR_ಸಂಖ್ಯೆ;
		actual_tv.vval.v_ಸಂಖ್ಯೆ = emsg_assert_fails_lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ == 4)
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = emsg_assert_fails_context;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		actual_tv.v_type = VAR_STRING;
		actual_tv.vval.v_string = actual;
	    }
	    fill_assert_error(&ga, &argvars[2], expected_str,
			&argvars[error_ಸಿಕ್ಕಿತು_ಸೂಚ್ಯಂಕ], &actual_tv, ASSERT_FAILS);
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
    ಒಳ_assert_fails = FALSE;
    did_emsg = FALSE;
    got_int = FALSE;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    --no_wait_return;
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    ಸಂದೇಶ_scrolled = 0;
    ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು;
    VIM_CLEAR(emsg_assert_fails_ಸಂದೇಶ);
    vim_free(tofree);
    ಗಣ_vim_var_string(VV_ERRMSG, ಖಾಲಿ, 0);
    ಆದರೆ (wrong_arg_ಸಂದೇಶ != ಖಾಲಿ)
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    char_u	expected_str[200];

    ಆದರೆ (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	ಆದರೆ (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ಅಕ್ಷರ *)expected_str, 200, "range %g - %g,",
							       flower, fupper);
	    fill_assert_error(&ga, &argvars[3], expected_str, ಖಾಲಿ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ಹಿಂತಿರುಗಿ 1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	varnumber_T	ಅಧೋ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	varnumber_T	ಊರ್ಧ್ವ_2 = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);

	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (actual < ಅಧೋ || actual > ಊರ್ಧ್ವ_2)
	{
	    prepare_assert_error(&ga);
	    vim_snprintf((ಅಕ್ಷರ *)expected_str, 200, "range %ld - %ld,",
						     (ದೀರ್ಘ)ಅಧೋ, (ದೀರ್ಘ)ಊರ್ಧ್ವ_2);
	    fill_assert_error(&ga, &argvars[3], expected_str, ಖಾಲಿ,
						    &argvars[2], ASSERT_OTHER);
	    assert_error(&ga);
	    ga_clear(&ga);
	    ಹಿಂತಿರುಗಿ 1;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * "assert_inrange(lower, upper[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = assert_inrange(argvars);
}

/*
 * "assert_match(pattern, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_ಹೊಂದಿಕೆ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_ಹೊಂದಿಕೆ_common(argvars, ASSERT_ಹೊಂದಿಕೆ);
}

/*
 * "assert_notmatch(pattern, actual[, msg])" function
 */
    ಶೂನ್ಯ
f_assert_notmatch(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = assert_ಹೊಂದಿಕೆ_common(argvars, ASSERT_NOTMATCH);
}

/*
 * "assert_report(msg)" function
 */
    ಶೂನ್ಯ
f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

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
f_test_alloc_fail(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ
	    || argvars[0].vval.v_ಸಂಖ್ಯೆ <= 0
	    || argvars[1].v_type != VAR_ಸಂಖ್ಯೆ
	    || argvars[1].vval.v_ಸಂಖ್ಯೆ < 0
	    || argvars[2].v_type != VAR_ಸಂಖ್ಯೆ)
	emsg(_(e_invalid_ವಾದ));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	alloc_fail_id = argvars[0].vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (alloc_fail_id >= aid_ಕೊನೆಯ)
	    emsg(_(e_invalid_ವಾದ));
	alloc_fail_countdown = argvars[1].vval.v_ಸಂಖ್ಯೆ;
	alloc_fail_repeat = argvars[2].vval.v_ಸಂಖ್ಯೆ;
	did_outofmem_ಸಂದೇಶ = FALSE;
    }
}

/*
 * "test_autochdir()"
 */
    ಶೂನ್ಯ
f_test_autochdir(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "test_feedinput()"
 */
    ಶೂನ್ಯ
f_test_feedinput(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಒಳಹರಿವು_BUF
    char_u	*val;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif

    ಆದರೆ (val != ಖಾಲಿ)
    {
	trash_ಒಳಹರಿವು_buf();
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(val, (ಪೂರ್ಣಾಂಕ)STRLEN(val));
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "test_getvalue({name})" function
 */
    ಶೂನ್ಯ
f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);

    ಆದರೆ (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = need_fileinfo;
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), name);
}

/*
 * "test_option_not_set({name})" function
 */
    ಶೂನ್ಯ
f_test_option_not_ಗಣ(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u *name = (char_u *)"";

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (reset_option_was_ಗಣ(name) == FAIL)
	semsg(_(e_invalid_ವಾದ_str), name);
}

/*
 * "test_override({name}, {val})" function
 */
    ಶೂನ್ಯ
f_test_override(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u *name = (char_u *)"";
    ಪೂರ್ಣಾಂಕ     val;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_starting = -1;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    val = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

    ಆದರೆ (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"starting") == 0)
    {
	ಆದರೆ (val)
	{
	    ಆದರೆ (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"syn_idlist_cache") == 0)
	disable_syn_idlist_cache_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"unreachable") == 0)
	ignore_unreachable_ಸಂಕೇತ_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"term_props") == 0)
	reset_ಪದ_props_on_termresponse = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_ಸಾಲುಗಳು = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"defcompile") == 0)
	override_defcompile = val;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	disable_syn_idlist_cache_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_ಪದ_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	ಆದರೆ (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), name);
}

/*
 * "test_refcount({expr})" function
 */
    ಶೂನ್ಯ
f_test_refcount(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ retval = -1;

    ಆಯ್ಕೆ (argvars[0].v_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (argvars[0].vval.v_ಕೆಲಸ != ಖಾಲಿ)
		retval = argvars[0].vval.v_ಕೆಲಸ->jv_refcount - 1;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (argvars[0].vval.v_channel != ಖಾಲಿ)
		retval = argvars[0].vval.v_channel->ch_refcount - 1;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FUNC:
	    ಆದರೆ (argvars[0].vval.v_string != ಖಾಲಿ)
	    {
		ufunc_T *fp;

		fp = ಕಂಡುಹಿಡಿ_func(argvars[0].vval.v_string, FALSE);
		ಆದರೆ (fp != ಖಾಲಿ)
		    retval = fp->uf_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (argvars[0].vval.v_partial != ಖಾಲಿ)
		retval = argvars[0].vval.v_partial->pt_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ (argvars[0].vval.v_blob != ಖಾಲಿ)
		retval = argvars[0].vval.v_blob->bv_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (argvars[0].vval.v_list != ಖಾಲಿ)
		retval = argvars[0].vval.v_list->lv_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (argvars[0].vval.v_tuple != ಖಾಲಿ)
		retval = argvars[0].vval.v_tuple->tv_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ (argvars[0].vval.v_dict != ಖಾಲಿ)
		retval = argvars[0].vval.v_dict->dv_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	    ಆದರೆ (argvars[0].vval.v_class != ಖಾಲಿ)
		retval = argvars[0].vval.v_class->class_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಆದರೆ (argvars[0].vval.v_object != ಖಾಲಿ)
		retval = argvars[0].vval.v_object->obj_refcount - 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಆದರೆ (argvars[0].vval.v_typealias != ಖಾಲಿ)
		retval = argvars[0].vval.v_typealias->ta_refcount - 1;
	    ನಿಲ್ಲಿಸು;
    }

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = retval;

}

/*
 * "test_garbagecollect_now()" function
 */
    ಶೂನ್ಯ
f_test_garbagecollect_now(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    // This is dangerous, any Lists and Dicts used internally may be freed
    // while still in use.
    ಆದರೆ (!get_vim_var_nr(VV_TESTING))
	emsg(_(e_calling_test_garbagecollect_now_while_v_testing_is_not_ಗಣ));
    ಇಲ್ಲದಿದ್ದರೆ
	garbage_collect(TRUE);
}

/*
 * "test_garbagecollect_soon()" function
 */
    ಶೂನ್ಯ
f_test_garbagecollect_soon(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    may_garbage_collect = TRUE;
}

/*
 * "test_ignore_error()" function
 */
    ಶೂನ್ಯ
f_test_ignore_error(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ignore_error_for_testing(tv_get_string(&argvars[0]));
}

    ಶೂನ್ಯ
f_test_null_blob(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
f_test_null_channel(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
f_test_null_dict(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv_dict_ಗಣ(rettv, ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
f_test_null_ಕೆಲಸ(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_ಕೆಲಸ;
    rettv->vval.v_ಕೆಲಸ = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
f_test_null_list(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv_list_ಗಣ(rettv, ಖಾಲಿ);
}

    ಶೂನ್ಯ
f_test_null_function(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_FUNC;
    rettv->vval.v_string = ಖಾಲಿ;
}

    ಶೂನ್ಯ
f_test_null_partial(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = ಖಾಲಿ;
}

    ಶೂನ್ಯ
f_test_null_string(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
}

    ಶೂನ್ಯ
f_test_null_tuple(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv_tuple_ಗಣ(rettv, ಖಾಲಿ);
}

    ಶೂನ್ಯ
f_test_unknown(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;
}

    ಶೂನ್ಯ
f_test_void(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_VOID;
}

    ಶೂನ್ಯ
f_test_setmouse(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ || argvars[1].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    mouse_ಅಡ್ಡಸಾಲು = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;
    mouse_ಲಂಬಸಾಲು = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_drop_files(dict_T *ವಾದಗಳು ಬಳಸದ)
{
# ಆದರೆ defined(HAVE_DROP_FILE)
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    int_u	mods;
    char_u	**fnames;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "files")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "row")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "col")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "modifiers"))
	ಹಿಂತಿರುಗಿ FALSE;

    (ಶೂನ್ಯ)dict_get_tv(ವಾದಗಳು, "files", &t);
    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "row");
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "col");
    mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");

    ಆದರೆ (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	ಹಿಂತಿರುಗಿ FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    ಆದರೆ (fnames == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	// ignore non-string items
	ಆದರೆ (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	fnames[ಎಣಿಕೆ_2] = vim_strsave(li->li_tv.vval.v_string);
	ಆದರೆ (fnames[ಎಣಿಕೆ_2] == ಖಾಲಿ)
	{
	    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
		vim_free(fnames[ಎಣಿಕೆ_2]);
	    vim_free(fnames);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಎಣಿಕೆ_2++;
    }

    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	gui_handle_drop(ಪಠ್ಯ_X(ಲಂಬಸಾಲು - 1), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು - 1), mods, fnames, ಎಣಿಕೆ_2);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(fnames);
# endif

    ಹಿಂತಿರುಗಿ TRUE;
}

# ಆದರೆ defined(ಕಂಡುಹಿಡಿ_REPLACE_DIALOG)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_ಕಂಡುಹಿಡಿ_repl(dict_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ		flags;
    char_u	*ಕಂಡುಹಿಡಿ_ಪಠ್ಯ;
    char_u	*repl_ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		forward;
    ಪೂರ್ಣಾಂಕ		retval;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "find_text")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "repl_text")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "flags")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "forward"))
	ಹಿಂತಿರುಗಿ FALSE;

    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = dict_get_string(ವಾದಗಳು, "find_text", TRUE);
    repl_ಪಠ್ಯ = dict_get_string(ವಾದಗಳು, "repl_text", TRUE);
    flags = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "flags");
    forward = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "forward");

    retval = gui_do_findrepl(flags, ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, repl_ಪಠ್ಯ, forward);
    vim_free(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
    vim_free(repl_ಪಠ್ಯ);

    ಹಿಂತಿರುಗಿ retval;
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_mouse_event(dict_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ		button;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		repeated_click;
    int_u	mods;
    ಪೂರ್ಣಾಂಕ		move;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "row")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "col"))
	ಹಿಂತಿರುಗಿ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ಪೂರ್ಣಾಂಕ)dict_get_bool(ವಾದಗಳು, "move", FALSE);

    ಆದರೆ (!move && (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "button")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "multiclick")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "modifiers")))
	ಹಿಂತಿರುಗಿ FALSE;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "row");
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "col");

    ಆದರೆ (move)
    {
	ಪೂರ್ಣಾಂಕ pY = ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ pX = ಲಂಬಸಾಲು;
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ಆದರೆ (dict_get_bool(ವಾದಗಳು, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    pY = (ಅಡ್ಡಸಾಲು - 1) * gui.char_ಎತ್ತರ + gui.char_ಎತ್ತರ / 2;
	    pX = (ಲಂಬಸಾಲು - 1) * gui.char_ಅಗಲ + gui.char_ಅಗಲ / 2;
	}
	gui_mouse_moved(pX, pY);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	button = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "button");
	repeated_click = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "multiclick");
	mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(6);
	mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(3);

	gui_send_mouse_event(button, ಪಠ್ಯ_X(ಲಂಬಸಾಲು - 1), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು - 1),
							repeated_click, mods);
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_scrollbar(dict_T *ವಾದಗಳು)
{
    char_u	*which;
    ದೀರ್ಘ	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ		dragging;
    scrollbar_T *sb = ಖಾಲಿ;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "which")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "value")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "dragging"))
	ಹಿಂತಿರುಗಿ FALSE;

    which = dict_get_string(ವಾದಗಳು, "which", FALSE);
    ಮೌಲ್ಯ = (ದೀರ್ಘ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "value");
    dragging = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "dragging");

    ಆದರೆ (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_ಎಡ];
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_ಬಲ];
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    ಆದರೆ (sb == ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str), which);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
# ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_ಸಾಮಾನ್ಯ(FALSE, TRUE, FALSE);
# endif

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_tabline_event(dict_T *ವಾದಗಳು ಬಳಸದ)
{
# ifdef FEAT_GUI_TABLINE
    ಪೂರ್ಣಾಂಕ		tabnr;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "tabnr"))
	ಹಿಂತಿರುಗಿ FALSE;

    tabnr = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "tabnr");

    ಹಿಂತಿರುಗಿ send_tabline_event(tabnr);
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
# endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_tabmenu_event(dict_T *ವಾದಗಳು ಬಳಸದ)
{
# ifdef FEAT_GUI_TABLINE
    ಪೂರ್ಣಾಂಕ	tabnr;
    ಪೂರ್ಣಾಂಕ	ಅಂಶ;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "tabnr")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "item"))
	ಹಿಂತಿರುಗಿ FALSE;

    tabnr = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "tabnr");
    ಅಂಶ = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "item");

    send_tabline_menu_event(tabnr, ಅಂಶ);
# endif
    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
f_test_mswin_event(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಆದರೆ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ಒಳ_sandbox));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *event = tv_get_string(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = test_mswin_event(event, argvars[1].vval.v_dict);

#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
f_test_gui_event(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    char_u	*event;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಆದರೆ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ಒಳ_sandbox));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_dict_arg(argvars, 1) == FAIL
	    || argvars[1].vval.v_dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    event = tv_get_string(&argvars[0]);
    ಆದರೆ (STRCMP(event, "dropfiles") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_drop_files(argvars[1].vval.v_dict);
# ಆದರೆ defined(ಕಂಡುಹಿಡಿ_REPLACE_DIALOG)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "findrepl") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_ಕಂಡುಹಿಡಿ_repl(argvars[1].vval.v_dict);
# endif
# ifdef MSWIN
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "key") == 0 || STRCMP(event, "mouse") == 0 || STRCMP(event, "set_keycode_trans_strategy") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_mswin_event(event, argvars[1].vval.v_dict);
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "mouse") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_mouse_event(argvars[1].vval.v_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "scrollbar") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_scrollbar(argvars[1].vval.v_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "tabline") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_tabline_event(argvars[1].vval.v_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "tabmenu") == 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = test_gui_tabmenu_event(argvars[1].vval.v_dict);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str), event);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
f_test_settime(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಸಮಯ_for_testing = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
