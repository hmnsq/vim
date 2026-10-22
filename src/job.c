/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Implements starting jobs and controlling them.
 */

#ضمن "vim.h"

#إذا defined(FEAT_ڪم_CHANNEL)

#وصف_ڪريو FOR_ALL_JOBS(ڪم) \
    لكل ((ڪم) = پهريون_ڪم; (ڪم) != عدم; (ڪم) = (ڪم)->jv_ايندڙ)

    ساكن صحيح
handle_گهڻائي(typval_T *شي, jobopt_T *opt, ch_گهڻائي_T *modep, صحيح jo)
{
    char_u	*val = tv_get_string(شي);

    opt->jo_سيٽ |= jo;
    إذا (STRCMP(val, "nl") == 0)
	*modep = CH_گهڻائي_NL;
    وإلا إذا (STRCMP(val, "raw") == 0)
	*modep = CH_گهڻائي_RAW;
    وإلا إذا (STRCMP(val, "blob") == 0)
	*modep = CH_گهڻائي_BLOB;
    وإلا إذا (STRCMP(val, "js") == 0)
	*modep = CH_گهڻائي_JS;
    وإلا إذا (STRCMP(val, "json") == 0)
	*modep = CH_گهڻائي_JSON;
    وإلا إذا (STRCMP(val, "lsp") == 0)
	*modep = CH_گهڻائي_LSP;
    وإلا إذا (STRCMP(val, "dap") == 0)
	*modep = CH_گهڻائي_DAP;
    وإلا
    {
	semsg(_(e_invalid_دليل_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
handle_io(typval_T *شي, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(شي);

    opt->jo_سيٽ |= JO_ٻاهريون_IO << (part - PART_ٻاهريون);
    إذا (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    وإلا إذا (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    وإلا إذا (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    وإلا إذا (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_بفر;
    وإلا إذا (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_ٻاهريون;
    وإلا
    {
	semsg(_(e_invalid_دليل_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    فراغ
clear_ڪم_options(jobopt_T *opt)
{
    CLEAR_اشارو(opt);
}

    ساكن فراغ
unref_ڪم_callback(callback_T *cb)
{
    إذا (cb->cb_partial != عدم)
	partial_unref(cb->cb_partial);
    وإلا إذا (cb->cb_name != عدم)
    {
	func_unref(cb->cb_name);
	إذا (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    فراغ
free_ڪم_options(jobopt_T *opt)
{
    unref_ڪم_callback(&opt->jo_callback);
    unref_ڪم_callback(&opt->jo_ٻاهريون_cb);
    unref_ڪم_callback(&opt->jo_err_cb);
    unref_ڪم_callback(&opt->jo_close_cb);
    unref_ڪم_callback(&opt->jo_exit_cb);

    إذا (opt->jo_env != عدم)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ساكن صحيح
part_from_char(صحيح c)
{
    أرجع c == 'i' ? PART_اندريون : c == 'o' ? PART_ٻاهريون: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحيح
get_ڪم_options(typval_T *tv, jobopt_T *opt, صحيح supported, صحيح supported2)
{
    typval_T	*شي;
    char_u	*val;
    dict_T	*dict;
    صحيح		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    إذا (tv->v_type == VAR_UNKNOWN)
	أرجع OK;
    إذا (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	أرجع FAIL;
    }
    dict = tv->vval.v_dict;
    إذا (dict == عدم)
	أرجع OK;

    todo = (صحيح)dict->dv_hashtab.ht_استعمال_ٿيل;
    FOR_ALL_HASHTAB_شيون(&dict->dv_hashtab, hi, todo)
	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    شي = &dict_lookup(hi)->di_tv;

	    إذا (STRCMP(hi->hi_ڪنجي, "mode") == 0)
	    {
		إذا (!(supported & JO_گهڻائي))
		    اكسر;
		إذا (handle_گهڻائي(شي, opt, &opt->jo_گهڻائي, JO_گهڻائي) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "in_mode") == 0)
	    {
		إذا (!(supported & JO_اندريون_گهڻائي))
		    اكسر;
		إذا (handle_گهڻائي(شي, opt, &opt->jo_اندريون_گهڻائي, JO_اندريون_گهڻائي)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "out_mode") == 0)
	    {
		إذا (!(supported & JO_ٻاهريون_گهڻائي))
		    اكسر;
		إذا (handle_گهڻائي(شي, opt, &opt->jo_ٻاهريون_گهڻائي, JO_ٻاهريون_گهڻائي)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "err_mode") == 0)
	    {
		إذا (!(supported & JO_ERR_گهڻائي))
		    اكسر;
		إذا (handle_گهڻائي(شي, opt, &opt->jo_err_گهڻائي, JO_ERR_گهڻائي)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "noblock") == 0)
	    {
		إذا (!(supported & JO_گهڻائي))
		    اكسر;
		opt->jo_noblock = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "in_io") == 0
		    || STRCMP(hi->hi_ڪنجي, "out_io") == 0
		    || STRCMP(hi->hi_ڪنجي, "err_io") == 0)
	    {
		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		إذا (handle_io(شي, part_from_char(*hi->hi_ڪنجي), opt) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "in_name") == 0
		    || STRCMP(hi->hi_ڪنجي, "out_name") == 0
		    || STRCMP(hi->hi_ڪنجي, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ڪنجي);

		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		opt->jo_سيٽ |= JO_ٻاهريون_NAME << (part - PART_ٻاهريون);
		opt->jo_io_name[part] = tv_get_string_buf_chk(شي,
						   opt->jo_io_name_buf[part]);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "pty") == 0)
	    {
		إذا (!(supported & JO_گهڻائي))
		    اكسر;
		opt->jo_pty = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "in_buf") == 0
		    || STRCMP(hi->hi_ڪنجي, "out_buf") == 0
		    || STRCMP(hi->hi_ڪنجي, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ڪنجي);

		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		opt->jo_سيٽ |= JO_ٻاهريون_BUF << (part - PART_ٻاهريون);
		opt->jo_io_buf[part] = tv_get_عدد(شي);
		إذا (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str),
					      hi->hi_ڪنجي, tv_get_string(شي));
		    أرجع FAIL;
		}
		إذا (buflist_findnr(opt->jo_io_buf[part]) == عدم)
		{
		    semsg(_(e_بفر_nr_does_not_exist),
						   (ڊگهو)opt->jo_io_buf[part]);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "out_modifiable") == 0
		    || STRCMP(hi->hi_ڪنجي, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ڪنجي);

		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		opt->jo_سيٽ |= JO_ٻاهريون_MODIFIABLE << (part - PART_ٻاهريون);
		opt->jo_modifiable[part] = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "out_msg") == 0
		    || STRCMP(hi->hi_ڪنجي, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ڪنجي);

		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		opt->jo_set2 |= JO2_ٻاهريون_نياپو << (part - PART_ٻاهريون);
		opt->jo_پيغام[part] = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "in_top") == 0
		    || STRCMP(hi->hi_ڪنجي, "in_bot") == 0)
	    {
		linenr_T *lp;

		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		إذا (hi->hi_ڪنجي[3] == 't')
		{
		    lp = &opt->jo_اندريون_چوٽي;
		    opt->jo_سيٽ |= JO_اندريون_چوٽي;
		}
		وإلا
		{
		    lp = &opt->jo_اندريون_bot;
		    opt->jo_سيٽ |= JO_اندريون_BOT;
		}
		*lp = tv_get_عدد(شي);
		إذا (*lp < 0)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str),
					      hi->hi_ڪنجي, tv_get_string(شي));
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "channel") == 0)
	    {
		إذا (!(supported & JO_ٻاهريون_IO))
		    اكسر;
		opt->jo_سيٽ |= JO_CHANNEL;
		إذا (شي->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "channel");
		    أرجع FAIL;
		}
		opt->jo_channel = شي->vval.v_channel;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "callback") == 0)
	    {
		إذا (!(supported & JO_CALLBACK))
		    اكسر;
		opt->jo_سيٽ |= JO_CALLBACK;
		opt->jo_callback = get_callback(شي);
		إذا (opt->jo_callback.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "callback");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "out_cb") == 0)
	    {
		إذا (!(supported & JO_ٻاهريون_CALLBACK))
		    اكسر;
		opt->jo_سيٽ |= JO_ٻاهريون_CALLBACK;
		opt->jo_ٻاهريون_cb = get_callback(شي);
		إذا (opt->jo_ٻاهريون_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "out_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "err_cb") == 0)
	    {
		إذا (!(supported & JO_ERR_CALLBACK))
		    اكسر;
		opt->jo_سيٽ |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(شي);
		إذا (opt->jo_err_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "err_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "close_cb") == 0)
	    {
		إذا (!(supported & JO_CLOSE_CALLBACK))
		    اكسر;
		opt->jo_سيٽ |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(شي);
		إذا (opt->jo_close_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "close_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "drop") == 0)
	    {
		صحيح never = FALSE;
		val = tv_get_string(شي);

		إذا (STRCMP(val, "never") == 0)
		    never = TRUE;
		وإلا إذا (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str), "drop", val);
		    أرجع FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "exit_cb") == 0)
	    {
		إذا (!(supported & JO_EXIT_CB))
		    اكسر;
		opt->jo_سيٽ |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(شي);
		إذا (opt->jo_exit_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "exit_cb");
		    أرجع FAIL;
		}
	    }
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_name") == 0)
	    {
		إذا (!(supported2 & JO2_رڪن_NAME))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_NAME;
		opt->jo_رڪن_name = tv_get_string_buf_chk(شي,
						       opt->jo_رڪن_name_buf);
		إذا (opt->jo_رڪن_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_name");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_finish") == 0)
	    {
		إذا (!(supported2 & JO2_رڪن_پڄاڻي))
		    اكسر;
		val = tv_get_string(شي);
		إذا (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str),
							   "term_finish", val);
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_رڪن_پڄاڻي;
		opt->jo_رڪن_پڄاڻي = *val;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_opencmd") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_رڪن_OPENCMD))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_OPENCMD;
		p = opt->jo_رڪن_opencmd = tv_get_string_buf_chk(شي,
						    opt->jo_رڪن_opencmd_buf);
		إذا (p != عدم)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    إذا (p != عدم && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != عدم))
			p = عدم;
		}
		إذا (p == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_opencmd");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "eof_chars") == 0)
	    {
		إذا (!(supported2 & JO2_EOF_CHARS))
		    اكسر;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(شي,
						       opt->jo_eof_chars_buf);
		إذا (opt->jo_eof_chars == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "eof_chars");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_rows") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_رڪن_صفون))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_صفون;
		opt->jo_رڪن_صفون = tv_get_عدد_chk(شي, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_رڪن_صفون < 0 || opt->jo_رڪن_صفون > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_rows");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_cols") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_رڪن_کانن_ڳڻپ))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_کانن_ڳڻپ;
		opt->jo_رڪن_کانن_ڳڻپ = tv_get_عدد_chk(شي, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_رڪن_کانن_ڳڻپ < 0 || opt->jo_رڪن_کانن_ڳڻپ > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_cols");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "vertical") == 0)
	    {
		إذا (!(supported2 & JO2_VERTICAL))
		    اكسر;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "curwin") == 0)
	    {
		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "bufnr") == 0)
	    {
		صحيح nr;

		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_عدد(شي);
		إذا (nr <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str), hi->hi_ڪنجي, tv_get_string(شي));
		    أرجع FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		إذا (opt->jo_bufnr_buf == عدم)
		{
		    semsg(_(e_بفر_nr_does_not_exist), (ڊگهو)nr);
		    أرجع FAIL;
		}
		إذا (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_رڪن == عدم)
		{
		    semsg(_(e_invalid_دليل_str), "bufnr");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "hidden") == 0)
	    {
		إذا (!(supported2 & JO2_HIDDEN))
		    اكسر;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "norestore") == 0)
	    {
		إذا (!(supported2 & JO2_NORESTORE))
		    اكسر;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_رڪن_norestore = tv_get_bool(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_kill") == 0)
	    {
		إذا (!(supported2 & JO2_رڪن_KILL))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_KILL;
		opt->jo_رڪن_kill = tv_get_string_buf_chk(شي,
						       opt->jo_رڪن_kill_buf);
		إذا (opt->jo_رڪن_kill == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_kill");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "tty_type") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TTY_TYPE))
		    اكسر;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(شي);
		إذا (p == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "tty_type");
		    أرجع FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		إذا (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "tty_type");
		    أرجع FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "ansi_colors") == 0)
	    {
		صحيح		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		إذا (!(supported2 & JO2_ANSI_COLORS))
		    اكسر;

		إذا (شي == عدم || شي->v_type != VAR_LIST
			|| شي->vval.v_list == عدم
			|| شي->vval.v_list->lv_پهريون == &range_list_شي)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "ansi_colors");
		    أرجع FAIL;
		}

		li = شي->vval.v_list->lv_پهريون;
		لكل (; li != عدم && n < 16; li = li->li_ايندڙ, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    صحيح		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    إذا (color_name == عدم)
			أرجع FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    إذا (guicolor == INVALCOLOR)
		    {
			إذا (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			أرجع FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		إذا (n != 16 || li != عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "ansi_colors");
		    أرجع FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		يادگيري_نقل(opt->jo_ansi_colors, rgb, حجم(rgb));
	    }
# endif
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_highlight") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_رڪن_HIGHLIGHT))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_HIGHLIGHT;
		p = tv_get_string_buf_chk(شي, opt->jo_رڪن_highlight_buf);
		إذا (p == عدم || *p == NUL)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_highlight");
		    أرجع FAIL;
		}
		opt->jo_رڪن_highlight = p;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "term_api") == 0)
	    {
		إذا (!(supported2 & JO2_رڪن_API))
		    اكسر;
		opt->jo_set2 |= JO2_رڪن_API;
		opt->jo_رڪن_api = tv_get_string_buf_chk(شي,
							opt->jo_رڪن_api_buf);
		إذا (opt->jo_رڪن_api == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "term_api");
		    أرجع FAIL;
		}
	    }
#جيڪڏهن_پورو
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "env") == 0)
	    {
		إذا (!(supported2 & JO2_ENV))
		    اكسر;
		إذا (شي->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "env");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = شي->vval.v_dict;
		إذا (opt->jo_env != عدم)
		    ++opt->jo_env->dv_refcount;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "cwd") == 0)
	    {
		إذا (!(supported2 & JO2_CWD))
		    اكسر;
		opt->jo_cwd = tv_get_string_buf_chk(شي, opt->jo_cwd_buf);
		إذا (opt->jo_cwd == عدم || !mch_isdir(opt->jo_cwd)
#جيڪڏهن_وصف_نه_ٿيل MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((حرف *)opt->jo_cwd, X_OK) != 0
#جيڪڏهن_پورو
				)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "cwd");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "waittime") == 0)
	    {
		إذا (!(supported & JO_WAITTIME))
		    اكسر;
		opt->jo_سيٽ |= JO_WAITTIME;
		opt->jo_waittime = tv_get_عدد(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "timeout") == 0)
	    {
		إذا (!(supported & JO_TIMEOUT))
		    اكسر;
		opt->jo_سيٽ |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_عدد(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "out_timeout") == 0)
	    {
		إذا (!(supported & JO_ٻاهريون_TIMEOUT))
		    اكسر;
		opt->jo_سيٽ |= JO_ٻاهريون_TIMEOUT;
		opt->jo_ٻاهريون_timeout = tv_get_عدد(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "err_timeout") == 0)
	    {
		إذا (!(supported & JO_ERR_TIMEOUT))
		    اكسر;
		opt->jo_سيٽ |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_عدد(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "part") == 0)
	    {
		إذا (!(supported & JO_PART))
		    اكسر;
		opt->jo_سيٽ |= JO_PART;
		val = tv_get_string(شي);
		إذا (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		وإلا إذا (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_ٻاهريون;
		وإلا
		{
		    semsg(_(e_invalid_قدر_for_دليل_str_str), "part", val);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "id") == 0)
	    {
		إذا (!(supported & JO_ID))
		    اكسر;
		opt->jo_سيٽ |= JO_ID;
		opt->jo_id = tv_get_عدد(شي);
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "stoponexit") == 0)
	    {
		إذا (!(supported & JO_STOPONEXIT))
		    اكسر;
		opt->jo_سيٽ |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(شي,
						      opt->jo_stoponexit_buf);
		إذا (opt->jo_stoponexit == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دليل_str), "stoponexit");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_ڪنجي, "block_write") == 0)
	    {
		إذا (!(supported & JO_بلاڪ_WRITE))
		    اكسر;
		opt->jo_سيٽ |= JO_بلاڪ_WRITE;
		opt->jo_بلاڪ_write = tv_get_عدد(شي);
	    }
	    وإلا
		اكسر;
	    --todo;
	}
    إذا (todo > 0)
    {
	semsg(_(e_invalid_دليل_str), hi->hi_ڪنجي);
	أرجع FAIL;
    }

    أرجع OK;
}

ساكن ڪم_T *پهريون_ڪم = عدم;

    ساكن فراغ
ڪم_free_contents(ڪم_T *ڪم)
{
    صحيح		i;

    ch_log(ڪم->jv_channel, "Freeing job");
    إذا (ڪم->jv_channel != عدم)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	ڪم->jv_channel->ch_ڪم = عدم;
	channel_unref(ڪم->jv_channel);
    }
    mch_clear_ڪم(ڪم);

    vim_free(ڪم->jv_tty_اندريون);
    vim_free(ڪم->jv_tty_ٻاهريون);
    vim_free(ڪم->jv_stoponexit);
#جيڪڏهن_وصف_ٿيل UNIX
    vim_free(ڪم->jv_termsig);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    vim_free(ڪم->jv_tty_type);
#جيڪڏهن_پورو
    free_callback(&ڪم->jv_exit_cb);
    إذا (ڪم->jv_argv != عدم)
    {
	لكل (i = 0; ڪم->jv_argv[i] != عدم; i++)
	    vim_free(ڪم->jv_argv[i]);
	vim_free(ڪم->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ساكن فراغ
ڪم_unlink(ڪم_T *ڪم)
{
    إذا (ڪم->jv_ايندڙ != عدم)
	ڪم->jv_ايندڙ->jv_prev = ڪم->jv_prev;
    إذا (ڪم->jv_prev == عدم)
	پهريون_ڪم = ڪم->jv_ايندڙ;
    وإلا
	ڪم->jv_prev->jv_ايندڙ = ڪم->jv_ايندڙ;
}

    ساكن فراغ
ڪم_free_ڪم(ڪم_T *ڪم)
{
    ڪم_unlink(ڪم);
    vim_free(ڪم);
}

    ساكن فراغ
ڪم_free(ڪم_T *ڪم)
{
    إذا (اندريون_free_unref_شيون)
	أرجع;

    ڪم_free_contents(ڪم);
    ڪم_free_ڪم(ڪم);
}

ساكن ڪم_T *jobs_منزل_free = عدم;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ساكن فراغ
ڪم_free_later(ڪم_T *ڪم)
{
    ڪم_unlink(ڪم);
    ڪم->jv_ايندڙ = jobs_منزل_free;
    jobs_منزل_free = ڪم;
}

    ساكن فراغ
free_jobs_منزل_free_later(فراغ)
{
    ڪم_T *ڪم;

    طالما (jobs_منزل_free != عدم)
    {
	ڪم = jobs_منزل_free;
	jobs_منزل_free = ڪم->jv_ايندڙ;
	ڪم_free_contents(ڪم);
	vim_free(ڪم);
    }
}

#إذا defined(EXITFREE)
    فراغ
ڪم_free_all(فراغ)
{
    طالما (پهريون_ڪم != عدم)
	ڪم_free(پهريون_ڪم);
    free_jobs_منزل_free_later();

# ifdef FEAT_TERMINAL
    free_اڻ_استعمال_ٿيل_terminals();
# endif
}
#جيڪڏهن_پورو

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ساكن صحيح
ڪم_need_پڇاڙي_check(ڪم_T *ڪم)
{
    أرجع ڪم->jv_حالت == ڪم_STARTED
	    && (ڪم->jv_stoponexit != عدم || ڪم->jv_exit_cb.cb_name != عدم);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ساكن صحيح
ڪم_channel_still_useful(ڪم_T *ڪم)
{
    أرجع ڪم->jv_channel != عدم && channel_still_useful(ڪم->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ساكن صحيح
ڪم_channel_can_close(ڪم_T *ڪم)
{
    أرجع ڪم->jv_channel != عدم && channel_can_close(ڪم->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ساكن صحيح
ڪم_still_useful(ڪم_T *ڪم)
{
    أرجع ڪم_need_پڇاڙي_check(ڪم) || ڪم_channel_still_useful(ڪم);
}

#إذا defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحيح
ڪم_any_running(فراغ)
{
    ڪم_T	*ڪم;

    FOR_ALL_JOBS(ڪم)
	إذا (ڪم_still_useful(ڪم))
	{
	    ch_log(عدم, "GUI not forking because a job is running");
	    أرجع TRUE;
	}
    أرجع FALSE;
}
#جيڪڏهن_پورو

// Unix uses argv[] for the command, other systems use a string.
#إذا defined(UNIX)
# define USE_ARGV
#جيڪڏهن_پورو

#إذا !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ساكن char_u *
win32_escape_arg(char_u *arg)
{
    صحيح		slen, dlen;
    صحيح		escaping = 0;
    صحيح		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    صحيح		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (صحيح)STRLEN(arg);
    dlen = slen;
    لكل (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	إذا (*s == '"' || *s == '\\')
	    ++dlen;
	إذا (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    إذا (has_spaces)
	dlen += 2;

    إذا (dlen == slen)
	أرجع vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    إذا (escaped_arg == عدم)
	أرجع عدم;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    إذا (has_spaces)
	*d++ = '"';

    لكل (s = arg; *s != NUL;)
    {
	بدل (*s)
	{
	    حالة '"':
		لكل (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		اكسر;
	    حالة '\\':
		escaping++;
		*d++ = *s++;
		اكسر;
	    افتراضي:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		اكسر;
	}
    }

    // add terminating quote and finish with a NUL
    إذا (has_spaces)
    {
	لكل (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    أرجع escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    صحيح
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_شيون(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	إذا (s == عدم)
	    أرجع FAIL;
	s = win32_escape_arg(s);
	إذا (s == عدم)
	    أرجع FAIL;
	ga_concat(gap, s);
	vim_free(s);
	إذا (li->li_ايندڙ != عدم)
	    ga_append(gap, ' ');
    }
    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    فراغ
ڪم_cleanup(ڪم_T *ڪم)
{
    إذا (ڪم->jv_حالت != ڪم_ENDED)
	أرجع;

    // Ready to cleanup the job.
    ڪم->jv_حالت = ڪم_ختم;

    // When only channel-in is kept open, close explicitly.
    إذا (ڪم->jv_channel != عدم)
	ch_close_part(ڪم->jv_channel, PART_اندريون);

    إذا (ڪم->jv_exit_cb.cb_name != عدم)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(ڪم->jv_channel, "Invoking exit callback %s",
						      ڪم->jv_exit_cb.cb_name);
	++ڪم->jv_refcount;
	argv[0].v_type = VAR_ڪم;
	argv[0].vval.v_ڪم = ڪم;
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = ڪم->jv_exitval;
	call_callback(&ڪم->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--ڪم->jv_refcount;
	channel_need_redraw = TRUE;
    }

    إذا (ڪم->jv_channel != عدم && ڪم->jv_channel->ch_anonymous_pipe)
	ڪم->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    إذا (ڪم->jv_refcount == 0 && !ڪم_channel_still_useful(ڪم))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	ڪم_free_later(ڪم);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحيح
سيٽ_ref_اندريون_ڪم(صحيح copyID)
{
    صحيح		abort = FALSE;
    ڪم_T	*ڪم;
    typval_T	tv;

    لكل (ڪم = پهريون_ڪم; !abort && ڪم != عدم; ڪم = ڪم->jv_ايندڙ)
	إذا (ڪم_still_useful(ڪم))
	{
	    tv.v_type = VAR_ڪم;
	    tv.vval.v_ڪم = ڪم;
	    abort = abort || سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    فراغ
ڪم_unref(ڪم_T *ڪم)
{
    إذا (ڪم == عدم || --ڪم->jv_refcount > 0)
	أرجع;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    إذا (ڪم_channel_still_useful(ڪم))
	أرجع;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    إذا (!ڪم_need_پڇاڙي_check(ڪم))
    {
	ڪم_free(ڪم);
    }
    وإلا إذا (ڪم->jv_channel != عدم)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(ڪم->jv_channel, "detaching channel from job");
	ڪم->jv_channel->ch_ڪم = عدم;
	channel_unref(ڪم->jv_channel);
	ڪم->jv_channel = عدم;
    }
}

    صحيح
free_اڻ_استعمال_ٿيل_jobs_contents(صحيح copyID, صحيح نقاب)
{
    صحيح		did_free = FALSE;
    ڪم_T	*ڪم;

    FOR_ALL_JOBS(ڪم)
	إذا ((ڪم->jv_copyID & نقاب) != (copyID & نقاب)
						    && !ڪم_still_useful(ڪم))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    ڪم_free_contents(ڪم);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    فراغ
free_اڻ_استعمال_ٿيل_jobs(صحيح copyID, صحيح نقاب)
{
    ڪم_T	*ڪم;
    ڪم_T	*ڪم_ايندڙ;

    لكل (ڪم = پهريون_ڪم; ڪم != عدم; ڪم = ڪم_ايندڙ)
    {
	ڪم_ايندڙ = ڪم->jv_ايندڙ;
	إذا ((ڪم->jv_copyID & نقاب) != (copyID & نقاب)
						    && !ڪم_still_useful(ڪم))
	{
	    // Free the job struct itself.
	    ڪم_free_ڪم(ڪم);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    ڪم_T *
ڪم_alloc(فراغ)
{
    ڪم_T *ڪم;

    ڪم = ALLOC_CLEAR_ONE(ڪم_T);
    إذا (ڪم == عدم)
	أرجع عدم;

    ڪم->jv_refcount = 1;
    ڪم->jv_stoponexit = vim_strsave((char_u *)"term");

    إذا (پهريون_ڪم != عدم)
    {
	پهريون_ڪم->jv_prev = ڪم;
	ڪم->jv_ايندڙ = پهريون_ڪم;
    }
    پهريون_ڪم = ڪم;
    أرجع ڪم;
}

    فراغ
ڪم_سيٽ_options(ڪم_T *ڪم, jobopt_T *opt)
{
    إذا (opt->jo_سيٽ & JO_STOPONEXIT)
    {
	vim_free(ڪم->jv_stoponexit);
	إذا (opt->jo_stoponexit == عدم || *opt->jo_stoponexit == NUL)
	    ڪم->jv_stoponexit = عدم;
	وإلا
	    ڪم->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    إذا (opt->jo_سيٽ & JO_EXIT_CB)
    {
	free_callback(&ڪم->jv_exit_cb);
	إذا (opt->jo_exit_cb.cb_name == عدم || *opt->jo_exit_cb.cb_name == NUL)
	{
	    ڪم->jv_exit_cb.cb_name = عدم;
	    ڪم->jv_exit_cb.cb_partial = عدم;
	}
	وإلا
	    copy_callback(&ڪم->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    فراغ
ڪم_stop_on_exit(فراغ)
{
    ڪم_T	*ڪم;

    FOR_ALL_JOBS(ڪم)
	إذا (ڪم->jv_حالت == ڪم_STARTED && ڪم->jv_stoponexit != عدم)
	    mch_signal_ڪم(ڪم, ڪم->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحيح
has_pending_ڪم(فراغ)
{
    ڪم_T	    *ڪم;

    FOR_ALL_JOBS(ڪم)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	إذا ((ڪم->jv_حالت == ڪم_STARTED && !ڪم_channel_still_useful(ڪم))
		    || (ڪم->jv_حالت == ڪم_ختم
					      && ڪم_channel_can_close(ڪم)))
	    أرجع TRUE;
    أرجع FALSE;
}

#وصف_ڪريو MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحيح
ڪم_check_ended(فراغ)
{
    صحيح		i;
    صحيح		did_پڇاڙي = FALSE;

    // be quick if there are no jobs to check
    إذا (پهريون_ڪم == عدم)
	أرجع did_پڇاڙي;

    لكل (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	ڪم_T	*ڪم = mch_detect_ended_ڪم(پهريون_ڪم);

	إذا (ڪم == عدم)
	    اكسر;
	did_پڇاڙي = TRUE;
	ڪم_cleanup(ڪم); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_منزل_free_later();

    إذا (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    أرجع did_پڇاڙي;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    ڪم_T *
ڪم_شروعات(
	typval_T    *argvars,
	حرف	    **argv_arg اڻ_استعمال_ٿيل,
	jobopt_T    *opt_arg,
	ڪم_T	    **رڪن_ڪم)
{
    ڪم_T	*ڪم;
    char_u	*cmd = عدم;
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;
#جيڪڏهن_وصف_نه_ٿيل USE_ARGV
    garray_T	ga;
#جيڪڏهن_پورو
    jobopt_T	opt;
    ch_part_T	part;

    ڪم = ڪم_alloc();
    إذا (ڪم == عدم)
	أرجع عدم;

    ڪم->jv_حالت = ڪم_FAILED;
#جيڪڏهن_وصف_نه_ٿيل USE_ARGV
    ga_init2(&ga, حجم(حرف*), 20);
#جيڪڏهن_پورو

    إذا (opt_arg != عدم)
	opt = *opt_arg;
    وإلا
    {
	// Default mode is NL.
	clear_ڪم_options(&opt);
	opt.jo_گهڻائي = CH_گهڻائي_NL;
	إذا (get_ڪم_options(&argvars[1], &opt,
		    JO_گهڻائي_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_ٻاهريون_IO + JO_بلاڪ_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    اذهب theend;
    }

    // Check that when io is "file" that there is a file name.
    لكل (part = PART_ٻاهريون; part < PART_ڳڻپ; ++part)
	إذا ((opt.jo_سيٽ & (JO_ٻاهريون_IO << (part - PART_ٻاهريون)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_سيٽ & (JO_ٻاهريون_NAME << (part - PART_ٻاهريون)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_منزل_be_سيٽ));
	    اذهب theend;
	}

    إذا ((opt.jo_سيٽ & JO_اندريون_IO) && opt.jo_io[PART_اندريون] == JIO_بفر)
    {
	buf_T *buf = عدم;

	// check that we can find the buffer before starting the job
	إذا (opt.jo_سيٽ & JO_اندريون_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_اندريون]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
						 (ڊگهو)opt.jo_io_buf[PART_اندريون]);
	}
	وإلا إذا (!(opt.jo_سيٽ & JO_اندريون_NAME))
	{
	    emsg(_(e_اندريون_io_بفر_requires_اندريون_buf_or_اندريون_name_منزل_be_سيٽ));
	}
	وإلا
	    buf = buflist_ڳوليو_by_name(opt.jo_io_name[PART_اندريون], FALSE);
	إذا (buf == عدم)
	    اذهب theend;
	إذا (buf->b_ml.ml_mfp == عدم)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    إذا (opt.jo_سيٽ & JO_اندريون_BUF)
	    {
		sprintf((حرف *)numbuf, "%d", opt.jo_io_buf[PART_اندريون]);
		s = numbuf;
	    }
	    وإلا
		s = opt.jo_io_name[PART_اندريون];
	    semsg(_(e_بفر_must_be_loaded_str), s);
	    اذهب theend;
	}
	ڪم->jv_اندريون_buf = buf;
    }

    ڪم_سيٽ_options(ڪم, &opt);

#جيڪڏهن_وصف_ٿيل USE_ARGV
    إذا (argv_arg != عدم)
    {
	// Make a copy of argv_arg for job->jv_argv.
	لكل (i = 0; argv_arg[i] != عدم; i++)
	    argc++;
	argv = ALLOC_MULT(حرف *, argc + 1);
	إذا (argv == عدم)
	    اذهب theend;
	لكل (i = 0; i < argc; i++)
	    argv[i] = (حرف *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = عدم;
    }
    وإلا
#جيڪڏهن_پورو
    إذا (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دليل));
	    اذهب theend;
	}

	إذا (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    اذهب theend;
    }
    وإلا إذا (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == عدم
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_دليل));
	اذهب theend;
    }
    وإلا
    {
	list_T *l = argvars[0].vval.v_list;

	إذا (build_argv_from_list(l, &argv, &argc) == FAIL)
	    اذهب theend;

	// Empty command is invalid.
	إذا (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_دليل));
	    اذهب theend;
	}
#جيڪڏهن_وصف_نه_ٿيل USE_ARGV
	إذا (win32_build_cmd(l, &ga) == FAIL)
	    اذهب theend;
	cmd = ga.ga_معلومات;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دليل));
	    اذهب theend;
	}
#جيڪڏهن_پورو
    }

    // Save the command used to start the job.
    ڪم->jv_argv = argv;

    إذا (رڪن_ڪم != عدم)
	*رڪن_ڪم = ڪم;

#جيڪڏهن_وصف_ٿيل USE_ARGV
    إذا (ch_log_سرگرم())
    {
	garray_T    ga;

	ga_init2(&ga, حجم(حرف), 200);
	لكل (i = 0; i < argc; ++i)
	{
	    إذا (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(عدم, "Starting job: %s", (حرف *)ga.ga_معلومات);
	ga_clear(&ga);
    }
    mch_ڪم_شروعات(argv, ڪم, &opt, رڪن_ڪم != عدم);
#وإلا
    ch_log(عدم, "Starting job: %s", (حرف *)cmd);
    mch_ڪم_شروعات((حرف *)cmd, ڪم, &opt);
#جيڪڏهن_پورو

    // If the channel is reading from a buffer, write lines now.
    إذا (ڪم->jv_channel != عدم)
	channel_write_اندريون(ڪم->jv_channel);

theend:
#جيڪڏهن_وصف_نه_ٿيل USE_ARGV
    vim_free(ga.ga_معلومات);
#جيڪڏهن_پورو
    إذا (argv != عدم && argv != ڪم->jv_argv)
    {
	لكل (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_ڪم_options(&opt);
    أرجع ڪم;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    حرف *
ڪم_حالت(ڪم_T *ڪم)
{
    حرف	*نتيجو;

    إذا (ڪم->jv_حالت >= ڪم_ENDED)
	// No need to check, dead is dead.
	نتيجو = "dead";
    وإلا إذا (ڪم->jv_حالت == ڪم_FAILED)
	نتيجو = "fail";
    وإلا
    {
	نتيجو = mch_ڪم_حالت(ڪم);
	إذا (ڪم->jv_حالت == ڪم_ENDED)
	    ڪم_cleanup(ڪم);
    }
    أرجع نتيجو;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحيح
ڪم_stop(ڪم_T *ڪم, typval_T *argvars, حرف *type)
{
    char_u *arg;

    إذا (type != عدم)
	arg = (char_u *)type;
    وإلا إذا (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    وإلا
    {
	arg = tv_get_string_chk(&argvars[1]);
	إذا (arg == عدم)
	{
	    emsg(_(e_invalid_دليل));
	    أرجع 0;
	}
    }
    إذا (ڪم->jv_حالت == ڪم_FAILED)
    {
	ch_log(ڪم->jv_channel, "Job failed to start, job_stop() skipped");
	أرجع 0;
    }
    إذا (ڪم->jv_حالت == ڪم_ENDED)
    {
	ch_log(ڪم->jv_channel, "Job has already ended, job_stop() skipped");
	أرجع 0;
    }
    ch_log(ڪم->jv_channel, "Stopping job with '%s'", (حرف *)arg);
    إذا (mch_signal_ڪم(ڪم, arg) == FAIL)
	أرجع 0;

    // Assume that only "kill" will kill the job.
    إذا (ڪم->jv_channel != عدم && STRCMP(arg, "kill") == 0)
	ڪم->jv_channel->ch_ڪم_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    أرجع 1;
}

    فراغ
invoke_prompt_callback(فراغ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*متن;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.کانو = 0;

    إذا (curbuf->b_prompt_callback.cb_name == عدم
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	أرجع;
    متن = ml_get(lnum);
    prompt = prompt_متن();
    إذا (STRLEN(متن) >= STRLEN(prompt))
	متن += STRLEN(prompt);
    argv[0].v_type = VAR_STRING;
    argv[0].vval.v_string = vim_strsave(متن);
    argv[1].v_type = VAR_UNKNOWN;

    call_callback(&curbuf->b_prompt_callback, -1, &rettv, 1, argv);
    clear_tv(&argv[0]);
    clear_tv(&rettv);
}

/*
 * Return TRUE when the interrupt callback was invoked.
 */
    صحيح
invoke_prompt_مداخلت(فراغ)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحيح		ret;

    إذا (curbuf->b_prompt_مداخلت.cb_name == عدم
	    || *curbuf->b_prompt_مداخلت.cb_name == NUL)
	أرجع FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_مداخلت, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    أرجع ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ساكن char_u *
buf_prompt_متن(buf_T* buf)
{
    إذا (buf->b_prompt_متن == عدم)
	أرجع (char_u *)"% ";
    أرجع buf->b_prompt_متن;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_متن(فراغ)
{
    أرجع buf_prompt_متن(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    فراغ
init_prompt(صحيح cmdchar_todo)
{
    char_u *prompt = prompt_متن();
    char_u *متن;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
    متن = ml_get_curline();
    إذا (STRNCMP(متن, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	إذا (*متن == NUL)
	    ml_replace(curbuf->b_ml.ml_سٽ_ڳڻپ, prompt, TRUE);
	وإلا
	    ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	coladvance((colnr_T)MAXCOL);
	بدليو_بائيٽون(curbuf->b_ml.ml_سٽ_ڳڻپ, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    إذا (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.کانو != (صحيح)STRLEN(prompt))
	سيٽ_insstart(curwin->w_cursor.lnum, (صحيح)STRLEN(prompt));

    إذا (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    إذا (curwin->w_cursor.کانو < (صحيح)STRLEN(prompt))
	curwin->w_cursor.کانو = (صحيح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحيح
prompt_curpos_editable(فراغ)
{
    أرجع curwin->w_cursor.lnum == curbuf->b_ml.ml_سٽ_ڳڻپ
	&& curwin->w_cursor.کانو >= (صحيح)STRLEN(prompt_متن());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    فراغ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_callback);
    سيٽ_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    فراغ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_مداخلت);
    سيٽ_callback(&buf->b_prompt_مداخلت, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    فراغ
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[0]);
    إذا (buf == عدم)
	أرجع;

    إذا (!bt_prompt(buf))
	أرجع;

    rettv->vval.v_string = vim_strsave(buf_prompt_متن(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    فراغ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    char_u	*متن;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_secure())
	أرجع;
    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    متن = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_متن);
    buf->b_prompt_متن = vim_strsave(متن);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ساكن ڪم_T *
get_ڪم_arg(typval_T *tv)
{
    ڪم_T *ڪم;

    إذا (tv->v_type != VAR_ڪم)
    {
	semsg(_(e_invalid_دليل_str), tv_get_string(tv));
	أرجع عدم;
    }
    ڪم = tv->vval.v_ڪم;

    إذا (ڪم == عدم)
	emsg(_(e_not_valid_ڪم));
    أرجع ڪم;
}

/*
 * "job_getchannel()" function
 */
    فراغ
f_ڪم_getchannel(typval_T *argvars, typval_T *rettv)
{
    ڪم_T	*ڪم;

    إذا (اندريون_vim9script() && check_for_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    ڪم = get_ڪم_arg(&argvars[0]);
    إذا (ڪم == عدم)
	أرجع;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ڪم->jv_channel;
    إذا (ڪم->jv_channel != عدم)
	++ڪم->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ساكن فراغ
ڪم_info(ڪم_T *ڪم, dict_T *dict)
{
    dictitem_T	*شي;
    varnumber_T	nr;
    list_T	*l;
    صحيح		i;

    dict_جوڙيو_string(dict, "status", (char_u *)ڪم_حالت(ڪم));

    شي = dictitem_alloc((char_u *)"channel");
    إذا (شي == عدم)
	أرجع;
    شي->di_tv.v_type = VAR_CHANNEL;
    شي->di_tv.vval.v_channel = ڪم->jv_channel;
    إذا (ڪم->jv_channel != عدم)
	++ڪم->jv_channel->ch_refcount;
    إذا (dict_جوڙيو(dict, شي) == FAIL)
	dictitem_free(شي);

#جيڪڏهن_وصف_ٿيل UNIX
    nr = ڪم->jv_pid;
#وإلا
    nr = ڪم->jv_proc_info.dwProcessId;
#جيڪڏهن_پورو
    dict_جوڙيو_عدد(dict, "process", nr);
    dict_جوڙيو_string(dict, "tty_in", ڪم->jv_tty_اندريون);
    dict_جوڙيو_string(dict, "tty_out", ڪم->jv_tty_ٻاهريون);

    dict_جوڙيو_عدد(dict, "exitval", ڪم->jv_exitval);
    dict_جوڙيو_string(dict, "exit_cb", ڪم->jv_exit_cb.cb_name);
    dict_جوڙيو_string(dict, "stoponexit", ڪم->jv_stoponexit);
#جيڪڏهن_وصف_ٿيل UNIX
    dict_جوڙيو_string(dict, "termsig", ڪم->jv_termsig);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    dict_جوڙيو_string(dict, "tty_type", ڪم->jv_tty_type);
#جيڪڏهن_پورو

    l = list_alloc();
    إذا (l == عدم)
	أرجع;

    إذا (dict_جوڙيو_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	أرجع;
    }
    إذا (ڪم->jv_argv != عدم)
	لكل (i = 0; ڪم->jv_argv[i] != عدم; i++)
	    list_append_string(l, (char_u *)ڪم->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ساكن فراغ
ڪم_info_all(list_T *l)
{
    ڪم_T	*ڪم;
    typval_T	tv;

    FOR_ALL_JOBS(ڪم)
    {
	tv.v_type = VAR_ڪم;
	tv.vval.v_ڪم = ڪم;

	إذا (list_append_tv(l, &tv) != OK)
	    أرجع;
    }
}

/*
 * "job_info()" function
 */
    فراغ
f_ڪم_info(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_opt_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	ڪم_T	*ڪم;

	ڪم = get_ڪم_arg(&argvars[0]);
	إذا (ڪم != عدم && rettv_dict_alloc(rettv) == OK)
	    ڪم_info(ڪم, rettv->vval.v_dict);
    }
    وإلا إذا (rettv_list_alloc(rettv) == OK)
	ڪم_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    فراغ
f_ڪم_setoptions(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    ڪم_T	*ڪم;
    jobopt_T	opt;

    إذا (اندريون_vim9script()
	    && (check_for_ڪم_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    ڪم = get_ڪم_arg(&argvars[0]);
    إذا (ڪم == عدم)
	أرجع;
    clear_ڪم_options(&opt);
    إذا (get_ڪم_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	ڪم_سيٽ_options(ڪم, &opt);
    free_ڪم_options(&opt);
}

/*
 * "job_start()" function
 */
    فراغ
f_ڪم_شروعات(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_ڪم;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_ڪم = ڪم_شروعات(argvars, عدم, عدم, عدم);
}

/*
 * "job_status()" function
 */
    فراغ
f_ڪم_حالت(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_ڪم && argvars[0].vval.v_ڪم == عدم)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    وإلا
    {
	ڪم_T	*ڪم = get_ڪم_arg(&argvars[0]);

	إذا (ڪم != عدم)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)ڪم_حالت(ڪم));
	}
    }
}

/*
 * "job_stop()" function
 */
    فراغ
f_ڪم_stop(typval_T *argvars, typval_T *rettv)
{
    ڪم_T	*ڪم;

    إذا (اندريون_vim9script()
	    && (check_for_ڪم_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    ڪم = get_ڪم_arg(&argvars[0]);
    إذا (ڪم != عدم)
	rettv->vval.v_عدد = ڪم_stop(ڪم, argvars, عدم);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
ڪم_منزل_string_buf(typval_T *varp, char_u *buf)
{
    ڪم_T *ڪم = varp->vval.v_ڪم;
    حرف  *حالت;

    إذا (ڪم == عدم)
    {
	vim_snprintf((حرف *)buf, NUMBUFLEN, "no process");
	أرجع buf;
    }
    حالت = ڪم->jv_حالت == ڪم_FAILED ? "fail"
		    : ڪم->jv_حالت >= ڪم_ENDED ? "dead"
		    : "run";
#جيڪڏهن_وصف_ٿيل UNIX
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s", (ڊگهو)ڪم->jv_pid, حالت);
#وإلاإذا defined(MSWIN)
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s",
		(ڊگهو)ڪم->jv_proc_info.dwProcessId,
		حالت);
#وإلا
    // fall-back
    vim_snprintf((حرف *)buf, NUMBUFLEN, "process ? %s", حالت);
#جيڪڏهن_پورو
    أرجع buf;
}

#جيڪڏهن_پورو // FEAT_JOB_CHANNEL
