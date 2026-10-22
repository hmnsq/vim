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

#إذا defined(FEAT_کار_CHANNEL)

#تعریف_کن FOR_ALL_JOBS(کار) \
    لكل ((کار) = اولی_کار; (کار) != عدم; (کار) = (کار)->jv_بعدی)

    ساكن صحيح
handle_نما(typval_T *قلم, jobopt_T *opt, ch_نما_T *modep, صحيح jo)
{
    char_u	*val = tv_get_string(قلم);

    opt->jo_مجموعه |= jo;
    إذا (STRCMP(val, "nl") == 0)
	*modep = CH_نما_NL;
    وإلا إذا (STRCMP(val, "raw") == 0)
	*modep = CH_نما_RAW;
    وإلا إذا (STRCMP(val, "blob") == 0)
	*modep = CH_نما_BLOB;
    وإلا إذا (STRCMP(val, "js") == 0)
	*modep = CH_نما_JS;
    وإلا إذا (STRCMP(val, "json") == 0)
	*modep = CH_نما_JSON;
    وإلا إذا (STRCMP(val, "lsp") == 0)
	*modep = CH_نما_LSP;
    وإلا إذا (STRCMP(val, "dap") == 0)
	*modep = CH_نما_DAP;
    وإلا
    {
	semsg(_(e_invalid_آرگومان_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
handle_io(typval_T *قلم, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(قلم);

    opt->jo_مجموعه |= JO_بیرون_IO << (part - PART_بیرون);
    إذا (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    وإلا إذا (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    وإلا إذا (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    وإلا إذا (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_میانگیر;
    وإلا إذا (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_بیرون;
    وإلا
    {
	semsg(_(e_invalid_آرگومان_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    فراغ
clear_کار_options(jobopt_T *opt)
{
    CLEAR_اشاره_گر(opt);
}

    ساكن فراغ
unref_کار_callback(callback_T *cb)
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
free_کار_options(jobopt_T *opt)
{
    unref_کار_callback(&opt->jo_callback);
    unref_کار_callback(&opt->jo_بیرون_cb);
    unref_کار_callback(&opt->jo_err_cb);
    unref_کار_callback(&opt->jo_close_cb);
    unref_کار_callback(&opt->jo_exit_cb);

    إذا (opt->jo_env != عدم)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ساكن صحيح
part_from_char(صحيح c)
{
    أرجع c == 'i' ? PART_درون : c == 'o' ? PART_بیرون: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحيح
get_کار_options(typval_T *tv, jobopt_T *opt, صحيح supported, صحيح supported2)
{
    typval_T	*قلم;
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

    todo = (صحيح)dict->dv_hashtab.ht_استفاده_شده;
    FOR_ALL_HASHTAB_اقلام(&dict->dv_hashtab, hi, todo)
	إذا (!HASHITEM_خالی(hi))
	{
	    قلم = &dict_lookup(hi)->di_tv;

	    إذا (STRCMP(hi->hi_کلید, "mode") == 0)
	    {
		إذا (!(supported & JO_نما))
		    اكسر;
		إذا (handle_نما(قلم, opt, &opt->jo_نما, JO_نما) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_mode") == 0)
	    {
		إذا (!(supported & JO_درون_نما))
		    اكسر;
		إذا (handle_نما(قلم, opt, &opt->jo_درون_نما, JO_درون_نما)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_mode") == 0)
	    {
		إذا (!(supported & JO_بیرون_نما))
		    اكسر;
		إذا (handle_نما(قلم, opt, &opt->jo_بیرون_نما, JO_بیرون_نما)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_mode") == 0)
	    {
		إذا (!(supported & JO_ERR_نما))
		    اكسر;
		إذا (handle_نما(قلم, opt, &opt->jo_err_نما, JO_ERR_نما)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "noblock") == 0)
	    {
		إذا (!(supported & JO_نما))
		    اكسر;
		opt->jo_noblock = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_io") == 0
		    || STRCMP(hi->hi_کلید, "out_io") == 0
		    || STRCMP(hi->hi_کلید, "err_io") == 0)
	    {
		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		إذا (handle_io(قلم, part_from_char(*hi->hi_کلید), opt) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_name") == 0
		    || STRCMP(hi->hi_کلید, "out_name") == 0
		    || STRCMP(hi->hi_کلید, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		opt->jo_مجموعه |= JO_بیرون_NAME << (part - PART_بیرون);
		opt->jo_io_name[part] = tv_get_string_buf_chk(قلم,
						   opt->jo_io_name_buf[part]);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "pty") == 0)
	    {
		إذا (!(supported & JO_نما))
		    اكسر;
		opt->jo_pty = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_buf") == 0
		    || STRCMP(hi->hi_کلید, "out_buf") == 0
		    || STRCMP(hi->hi_کلید, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		opt->jo_مجموعه |= JO_بیرون_BUF << (part - PART_بیرون);
		opt->jo_io_buf[part] = tv_get_عدد(قلم);
		إذا (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str),
					      hi->hi_کلید, tv_get_string(قلم));
		    أرجع FAIL;
		}
		إذا (buflist_findnr(opt->jo_io_buf[part]) == عدم)
		{
		    semsg(_(e_میانگیر_nr_does_not_exist),
						   (دراز)opt->jo_io_buf[part]);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_modifiable") == 0
		    || STRCMP(hi->hi_کلید, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		opt->jo_مجموعه |= JO_بیرون_MODIFIABLE << (part - PART_بیرون);
		opt->jo_modifiable[part] = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_msg") == 0
		    || STRCMP(hi->hi_کلید, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		opt->jo_set2 |= JO2_بیرون_پیام << (part - PART_بیرون);
		opt->jo_پیام[part] = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_top") == 0
		    || STRCMP(hi->hi_کلید, "in_bot") == 0)
	    {
		linenr_T *lp;

		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		إذا (hi->hi_کلید[3] == 't')
		{
		    lp = &opt->jo_درون_بالاترین;
		    opt->jo_مجموعه |= JO_درون_بالاترین;
		}
		وإلا
		{
		    lp = &opt->jo_درون_bot;
		    opt->jo_مجموعه |= JO_درون_BOT;
		}
		*lp = tv_get_عدد(قلم);
		إذا (*lp < 0)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str),
					      hi->hi_کلید, tv_get_string(قلم));
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "channel") == 0)
	    {
		إذا (!(supported & JO_بیرون_IO))
		    اكسر;
		opt->jo_مجموعه |= JO_CHANNEL;
		إذا (قلم->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "channel");
		    أرجع FAIL;
		}
		opt->jo_channel = قلم->vval.v_channel;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "callback") == 0)
	    {
		إذا (!(supported & JO_CALLBACK))
		    اكسر;
		opt->jo_مجموعه |= JO_CALLBACK;
		opt->jo_callback = get_callback(قلم);
		إذا (opt->jo_callback.cb_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "callback");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_cb") == 0)
	    {
		إذا (!(supported & JO_بیرون_CALLBACK))
		    اكسر;
		opt->jo_مجموعه |= JO_بیرون_CALLBACK;
		opt->jo_بیرون_cb = get_callback(قلم);
		إذا (opt->jo_بیرون_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "out_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_cb") == 0)
	    {
		إذا (!(supported & JO_ERR_CALLBACK))
		    اكسر;
		opt->jo_مجموعه |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(قلم);
		إذا (opt->jo_err_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "err_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "close_cb") == 0)
	    {
		إذا (!(supported & JO_CLOSE_CALLBACK))
		    اكسر;
		opt->jo_مجموعه |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(قلم);
		إذا (opt->jo_close_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "close_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "drop") == 0)
	    {
		صحيح never = FALSE;
		val = tv_get_string(قلم);

		إذا (STRCMP(val, "never") == 0)
		    never = TRUE;
		وإلا إذا (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "drop", val);
		    أرجع FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "exit_cb") == 0)
	    {
		إذا (!(supported & JO_EXIT_CB))
		    اكسر;
		opt->jo_مجموعه |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(قلم);
		إذا (opt->jo_exit_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "exit_cb");
		    أرجع FAIL;
		}
	    }
#اگر_تعریف_شده FEAT_TERMINAL
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_name") == 0)
	    {
		إذا (!(supported2 & JO2_جمله_NAME))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_NAME;
		opt->jo_جمله_name = tv_get_string_buf_chk(قلم,
						       opt->jo_جمله_name_buf);
		إذا (opt->jo_جمله_name == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_name");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_finish") == 0)
	    {
		إذا (!(supported2 & JO2_جمله_پایان))
		    اكسر;
		val = tv_get_string(قلم);
		إذا (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str),
							   "term_finish", val);
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_جمله_پایان;
		opt->jo_جمله_پایان = *val;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_opencmd") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_جمله_OPENCMD))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_OPENCMD;
		p = opt->jo_جمله_opencmd = tv_get_string_buf_chk(قلم,
						    opt->jo_جمله_opencmd_buf);
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
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_opencmd");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "eof_chars") == 0)
	    {
		إذا (!(supported2 & JO2_EOF_CHARS))
		    اكسر;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(قلم,
						       opt->jo_eof_chars_buf);
		إذا (opt->jo_eof_chars == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "eof_chars");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_rows") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_جمله_ردیفها))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_ردیفها;
		opt->jo_جمله_ردیفها = tv_get_عدد_chk(قلم, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_جمله_ردیفها < 0 || opt->jo_جمله_ردیفها > 1000)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_rows");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_cols") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_جمله_تعداد_ستون))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_تعداد_ستون;
		opt->jo_جمله_تعداد_ستون = tv_get_عدد_chk(قلم, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_جمله_تعداد_ستون < 0 || opt->jo_جمله_تعداد_ستون > 1000)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_cols");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "vertical") == 0)
	    {
		إذا (!(supported2 & JO2_VERTICAL))
		    اكسر;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "curwin") == 0)
	    {
		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "bufnr") == 0)
	    {
		صحيح nr;

		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_عدد(قلم);
		إذا (nr <= 0)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str), hi->hi_کلید, tv_get_string(قلم));
		    أرجع FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		إذا (opt->jo_bufnr_buf == عدم)
		{
		    semsg(_(e_میانگیر_nr_does_not_exist), (دراز)nr);
		    أرجع FAIL;
		}
		إذا (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_جمله == عدم)
		{
		    semsg(_(e_invalid_آرگومان_str), "bufnr");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "hidden") == 0)
	    {
		إذا (!(supported2 & JO2_HIDDEN))
		    اكسر;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "norestore") == 0)
	    {
		إذا (!(supported2 & JO2_NORESTORE))
		    اكسر;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_جمله_norestore = tv_get_bool(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_kill") == 0)
	    {
		إذا (!(supported2 & JO2_جمله_KILL))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_KILL;
		opt->jo_جمله_kill = tv_get_string_buf_chk(قلم,
						       opt->jo_جمله_kill_buf);
		إذا (opt->jo_جمله_kill == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_kill");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "tty_type") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TTY_TYPE))
		    اكسر;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(قلم);
		إذا (p == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "tty_type");
		    أرجع FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		إذا (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "tty_type");
		    أرجع FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    وإلا إذا (STRCMP(hi->hi_کلید, "ansi_colors") == 0)
	    {
		صحيح		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		إذا (!(supported2 & JO2_ANSI_COLORS))
		    اكسر;

		إذا (قلم == عدم || قلم->v_type != VAR_LIST
			|| قلم->vval.v_list == عدم
			|| قلم->vval.v_list->lv_اولی == &range_list_قلم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "ansi_colors");
		    أرجع FAIL;
		}

		li = قلم->vval.v_list->lv_اولی;
		لكل (; li != عدم && n < 16; li = li->li_بعدی, n++)
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
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "ansi_colors");
		    أرجع FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		کپی_حافظه(opt->jo_ansi_colors, rgb, حجم(rgb));
	    }
# endif
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_highlight") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_جمله_HIGHLIGHT))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_HIGHLIGHT;
		p = tv_get_string_buf_chk(قلم, opt->jo_جمله_highlight_buf);
		إذا (p == عدم || *p == NUL)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_highlight");
		    أرجع FAIL;
		}
		opt->jo_جمله_highlight = p;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_api") == 0)
	    {
		إذا (!(supported2 & JO2_جمله_API))
		    اكسر;
		opt->jo_set2 |= JO2_جمله_API;
		opt->jo_جمله_api = tv_get_string_buf_chk(قلم,
							opt->jo_جمله_api_buf);
		إذا (opt->jo_جمله_api == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "term_api");
		    أرجع FAIL;
		}
	    }
#پایان_اگر
	    وإلا إذا (STRCMP(hi->hi_کلید, "env") == 0)
	    {
		إذا (!(supported2 & JO2_ENV))
		    اكسر;
		إذا (قلم->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "env");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = قلم->vval.v_dict;
		إذا (opt->jo_env != عدم)
		    ++opt->jo_env->dv_refcount;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "cwd") == 0)
	    {
		إذا (!(supported2 & JO2_CWD))
		    اكسر;
		opt->jo_cwd = tv_get_string_buf_chk(قلم, opt->jo_cwd_buf);
		إذا (opt->jo_cwd == عدم || !mch_isdir(opt->jo_cwd)
#اگر_تعریف_نشده MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((حرف *)opt->jo_cwd, X_OK) != 0
#پایان_اگر
				)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "cwd");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "waittime") == 0)
	    {
		إذا (!(supported & JO_WAITTIME))
		    اكسر;
		opt->jo_مجموعه |= JO_WAITTIME;
		opt->jo_waittime = tv_get_عدد(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "timeout") == 0)
	    {
		إذا (!(supported & JO_TIMEOUT))
		    اكسر;
		opt->jo_مجموعه |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_عدد(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_timeout") == 0)
	    {
		إذا (!(supported & JO_بیرون_TIMEOUT))
		    اكسر;
		opt->jo_مجموعه |= JO_بیرون_TIMEOUT;
		opt->jo_بیرون_timeout = tv_get_عدد(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_timeout") == 0)
	    {
		إذا (!(supported & JO_ERR_TIMEOUT))
		    اكسر;
		opt->jo_مجموعه |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_عدد(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "part") == 0)
	    {
		إذا (!(supported & JO_PART))
		    اكسر;
		opt->jo_مجموعه |= JO_PART;
		val = tv_get_string(قلم);
		إذا (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		وإلا إذا (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_بیرون;
		وإلا
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "part", val);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "id") == 0)
	    {
		إذا (!(supported & JO_ID))
		    اكسر;
		opt->jo_مجموعه |= JO_ID;
		opt->jo_id = tv_get_عدد(قلم);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "stoponexit") == 0)
	    {
		إذا (!(supported & JO_STOPONEXIT))
		    اكسر;
		opt->jo_مجموعه |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(قلم,
						      opt->jo_stoponexit_buf);
		إذا (opt->jo_stoponexit == عدم)
		{
		    semsg(_(e_invalid_مقدار_for_آرگومان_str), "stoponexit");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "block_write") == 0)
	    {
		إذا (!(supported & JO_بلوک_WRITE))
		    اكسر;
		opt->jo_مجموعه |= JO_بلوک_WRITE;
		opt->jo_بلوک_write = tv_get_عدد(قلم);
	    }
	    وإلا
		اكسر;
	    --todo;
	}
    إذا (todo > 0)
    {
	semsg(_(e_invalid_آرگومان_str), hi->hi_کلید);
	أرجع FAIL;
    }

    أرجع OK;
}

ساكن کار_T *اولی_کار = عدم;

    ساكن فراغ
کار_free_contents(کار_T *کار)
{
    صحيح		i;

    ch_log(کار->jv_channel, "Freeing job");
    إذا (کار->jv_channel != عدم)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	کار->jv_channel->ch_کار = عدم;
	channel_unref(کار->jv_channel);
    }
    mch_clear_کار(کار);

    vim_free(کار->jv_tty_درون);
    vim_free(کار->jv_tty_بیرون);
    vim_free(کار->jv_stoponexit);
#اگر_تعریف_شده UNIX
    vim_free(کار->jv_termsig);
#پایان_اگر
#اگر_تعریف_شده MSWIN
    vim_free(کار->jv_tty_type);
#پایان_اگر
    free_callback(&کار->jv_exit_cb);
    إذا (کار->jv_argv != عدم)
    {
	لكل (i = 0; کار->jv_argv[i] != عدم; i++)
	    vim_free(کار->jv_argv[i]);
	vim_free(کار->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ساكن فراغ
کار_unlink(کار_T *کار)
{
    إذا (کار->jv_بعدی != عدم)
	کار->jv_بعدی->jv_prev = کار->jv_prev;
    إذا (کار->jv_prev == عدم)
	اولی_کار = کار->jv_بعدی;
    وإلا
	کار->jv_prev->jv_بعدی = کار->jv_بعدی;
}

    ساكن فراغ
کار_free_کار(کار_T *کار)
{
    کار_unlink(کار);
    vim_free(کار);
}

    ساكن فراغ
کار_free(کار_T *کار)
{
    إذا (درون_free_unref_اقلام)
	أرجع;

    کار_free_contents(کار);
    کار_free_کار(کار);
}

ساكن کار_T *jobs_مقصد_free = عدم;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ساكن فراغ
کار_free_later(کار_T *کار)
{
    کار_unlink(کار);
    کار->jv_بعدی = jobs_مقصد_free;
    jobs_مقصد_free = کار;
}

    ساكن فراغ
free_jobs_مقصد_free_later(فراغ)
{
    کار_T *کار;

    طالما (jobs_مقصد_free != عدم)
    {
	کار = jobs_مقصد_free;
	jobs_مقصد_free = کار->jv_بعدی;
	کار_free_contents(کار);
	vim_free(کار);
    }
}

#إذا defined(EXITFREE)
    فراغ
کار_free_all(فراغ)
{
    طالما (اولی_کار != عدم)
	کار_free(اولی_کار);
    free_jobs_مقصد_free_later();

# ifdef FEAT_TERMINAL
    free_استفادهنشده_terminals();
# endif
}
#پایان_اگر

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ساكن صحيح
کار_need_انتها_check(کار_T *کار)
{
    أرجع کار->jv_وضعیت == کار_STARTED
	    && (کار->jv_stoponexit != عدم || کار->jv_exit_cb.cb_name != عدم);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ساكن صحيح
کار_channel_still_useful(کار_T *کار)
{
    أرجع کار->jv_channel != عدم && channel_still_useful(کار->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ساكن صحيح
کار_channel_can_close(کار_T *کار)
{
    أرجع کار->jv_channel != عدم && channel_can_close(کار->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ساكن صحيح
کار_still_useful(کار_T *کار)
{
    أرجع کار_need_انتها_check(کار) || کار_channel_still_useful(کار);
}

#إذا defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحيح
کار_any_running(فراغ)
{
    کار_T	*کار;

    FOR_ALL_JOBS(کار)
	إذا (کار_still_useful(کار))
	{
	    ch_log(عدم, "GUI not forking because a job is running");
	    أرجع TRUE;
	}
    أرجع FALSE;
}
#پایان_اگر

// Unix uses argv[] for the command, other systems use a string.
#إذا defined(UNIX)
# define USE_ARGV
#پایان_اگر

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
    FOR_ALL_LIST_اقلام(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	إذا (s == عدم)
	    أرجع FAIL;
	s = win32_escape_arg(s);
	إذا (s == عدم)
	    أرجع FAIL;
	ga_concat(gap, s);
	vim_free(s);
	إذا (li->li_بعدی != عدم)
	    ga_append(gap, ' ');
    }
    أرجع OK;
}
#پایان_اگر

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    فراغ
کار_cleanup(کار_T *کار)
{
    إذا (کار->jv_وضعیت != کار_ENDED)
	أرجع;

    // Ready to cleanup the job.
    کار->jv_وضعیت = کار_پایانیافته;

    // When only channel-in is kept open, close explicitly.
    إذا (کار->jv_channel != عدم)
	ch_close_part(کار->jv_channel, PART_درون);

    إذا (کار->jv_exit_cb.cb_name != عدم)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(کار->jv_channel, "Invoking exit callback %s",
						      کار->jv_exit_cb.cb_name);
	++کار->jv_refcount;
	argv[0].v_type = VAR_کار;
	argv[0].vval.v_کار = کار;
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = کار->jv_exitval;
	call_callback(&کار->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--کار->jv_refcount;
	channel_need_redraw = TRUE;
    }

    إذا (کار->jv_channel != عدم && کار->jv_channel->ch_anonymous_pipe)
	کار->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    إذا (کار->jv_refcount == 0 && !کار_channel_still_useful(کار))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	کار_free_later(کار);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحيح
مجموعه_ref_درون_کار(صحيح copyID)
{
    صحيح		abort = FALSE;
    کار_T	*کار;
    typval_T	tv;

    لكل (کار = اولی_کار; !abort && کار != عدم; کار = کار->jv_بعدی)
	إذا (کار_still_useful(کار))
	{
	    tv.v_type = VAR_کار;
	    tv.vval.v_کار = کار;
	    abort = abort || مجموعه_ref_درون_قلم(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    فراغ
کار_unref(کار_T *کار)
{
    إذا (کار == عدم || --کار->jv_refcount > 0)
	أرجع;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    إذا (کار_channel_still_useful(کار))
	أرجع;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    إذا (!کار_need_انتها_check(کار))
    {
	کار_free(کار);
    }
    وإلا إذا (کار->jv_channel != عدم)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(کار->jv_channel, "detaching channel from job");
	کار->jv_channel->ch_کار = عدم;
	channel_unref(کار->jv_channel);
	کار->jv_channel = عدم;
    }
}

    صحيح
free_استفادهنشده_jobs_contents(صحيح copyID, صحيح ماسک)
{
    صحيح		did_free = FALSE;
    کار_T	*کار;

    FOR_ALL_JOBS(کار)
	إذا ((کار->jv_copyID & ماسک) != (copyID & ماسک)
						    && !کار_still_useful(کار))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    کار_free_contents(کار);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    فراغ
free_استفادهنشده_jobs(صحيح copyID, صحيح ماسک)
{
    کار_T	*کار;
    کار_T	*کار_بعدی;

    لكل (کار = اولی_کار; کار != عدم; کار = کار_بعدی)
    {
	کار_بعدی = کار->jv_بعدی;
	إذا ((کار->jv_copyID & ماسک) != (copyID & ماسک)
						    && !کار_still_useful(کار))
	{
	    // Free the job struct itself.
	    کار_free_کار(کار);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    کار_T *
کار_alloc(فراغ)
{
    کار_T *کار;

    کار = ALLOC_CLEAR_ONE(کار_T);
    إذا (کار == عدم)
	أرجع عدم;

    کار->jv_refcount = 1;
    کار->jv_stoponexit = vim_strsave((char_u *)"term");

    إذا (اولی_کار != عدم)
    {
	اولی_کار->jv_prev = کار;
	کار->jv_بعدی = اولی_کار;
    }
    اولی_کار = کار;
    أرجع کار;
}

    فراغ
کار_مجموعه_options(کار_T *کار, jobopt_T *opt)
{
    إذا (opt->jo_مجموعه & JO_STOPONEXIT)
    {
	vim_free(کار->jv_stoponexit);
	إذا (opt->jo_stoponexit == عدم || *opt->jo_stoponexit == NUL)
	    کار->jv_stoponexit = عدم;
	وإلا
	    کار->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    إذا (opt->jo_مجموعه & JO_EXIT_CB)
    {
	free_callback(&کار->jv_exit_cb);
	إذا (opt->jo_exit_cb.cb_name == عدم || *opt->jo_exit_cb.cb_name == NUL)
	{
	    کار->jv_exit_cb.cb_name = عدم;
	    کار->jv_exit_cb.cb_partial = عدم;
	}
	وإلا
	    copy_callback(&کار->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    فراغ
کار_stop_on_exit(فراغ)
{
    کار_T	*کار;

    FOR_ALL_JOBS(کار)
	إذا (کار->jv_وضعیت == کار_STARTED && کار->jv_stoponexit != عدم)
	    mch_signal_کار(کار, کار->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحيح
has_pending_کار(فراغ)
{
    کار_T	    *کار;

    FOR_ALL_JOBS(کار)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	إذا ((کار->jv_وضعیت == کار_STARTED && !کار_channel_still_useful(کار))
		    || (کار->jv_وضعیت == کار_پایانیافته
					      && کار_channel_can_close(کار)))
	    أرجع TRUE;
    أرجع FALSE;
}

#تعریف_کن MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحيح
کار_check_ended(فراغ)
{
    صحيح		i;
    صحيح		did_انتها = FALSE;

    // be quick if there are no jobs to check
    إذا (اولی_کار == عدم)
	أرجع did_انتها;

    لكل (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	کار_T	*کار = mch_detect_ended_کار(اولی_کار);

	إذا (کار == عدم)
	    اكسر;
	did_انتها = TRUE;
	کار_cleanup(کار); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_مقصد_free_later();

    إذا (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    أرجع did_انتها;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    کار_T *
کار_شروع(
	typval_T    *argvars,
	حرف	    **argv_arg استفادهنشده,
	jobopt_T    *opt_arg,
	کار_T	    **جمله_کار)
{
    کار_T	*کار;
    char_u	*cmd = عدم;
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;
#اگر_تعریف_نشده USE_ARGV
    garray_T	ga;
#پایان_اگر
    jobopt_T	opt;
    ch_part_T	part;

    کار = کار_alloc();
    إذا (کار == عدم)
	أرجع عدم;

    کار->jv_وضعیت = کار_FAILED;
#اگر_تعریف_نشده USE_ARGV
    ga_init2(&ga, حجم(حرف*), 20);
#پایان_اگر

    إذا (opt_arg != عدم)
	opt = *opt_arg;
    وإلا
    {
	// Default mode is NL.
	clear_کار_options(&opt);
	opt.jo_نما = CH_نما_NL;
	إذا (get_کار_options(&argvars[1], &opt,
		    JO_نما_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_بیرون_IO + JO_بلوک_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    اذهب theend;
    }

    // Check that when io is "file" that there is a file name.
    لكل (part = PART_بیرون; part < PART_شمار; ++part)
	إذا ((opt.jo_مجموعه & (JO_بیرون_IO << (part - PART_بیرون)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_مجموعه & (JO_بیرون_NAME << (part - PART_بیرون)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_مقصد_be_مجموعه));
	    اذهب theend;
	}

    إذا ((opt.jo_مجموعه & JO_درون_IO) && opt.jo_io[PART_درون] == JIO_میانگیر)
    {
	buf_T *buf = عدم;

	// check that we can find the buffer before starting the job
	إذا (opt.jo_مجموعه & JO_درون_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_درون]);
	    إذا (buf == عدم)
		semsg(_(e_میانگیر_nr_does_not_exist),
						 (دراز)opt.jo_io_buf[PART_درون]);
	}
	وإلا إذا (!(opt.jo_مجموعه & JO_درون_NAME))
	{
	    emsg(_(e_درون_io_میانگیر_requires_درون_buf_or_درون_name_مقصد_be_مجموعه));
	}
	وإلا
	    buf = buflist_پیدا_کن_by_name(opt.jo_io_name[PART_درون], FALSE);
	إذا (buf == عدم)
	    اذهب theend;
	إذا (buf->b_ml.ml_mfp == عدم)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    إذا (opt.jo_مجموعه & JO_درون_BUF)
	    {
		sprintf((حرف *)numbuf, "%d", opt.jo_io_buf[PART_درون]);
		s = numbuf;
	    }
	    وإلا
		s = opt.jo_io_name[PART_درون];
	    semsg(_(e_میانگیر_must_be_loaded_str), s);
	    اذهب theend;
	}
	کار->jv_درون_buf = buf;
    }

    کار_مجموعه_options(کار, &opt);

#اگر_تعریف_شده USE_ARGV
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
#پایان_اگر
    إذا (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_آرگومان));
	    اذهب theend;
	}

	إذا (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    اذهب theend;
    }
    وإلا إذا (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == عدم
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_آرگومان));
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
	    emsg(_(e_invalid_آرگومان));
	    اذهب theend;
	}
#اگر_تعریف_نشده USE_ARGV
	إذا (win32_build_cmd(l, &ga) == FAIL)
	    اذهب theend;
	cmd = ga.ga_داده;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_آرگومان));
	    اذهب theend;
	}
#پایان_اگر
    }

    // Save the command used to start the job.
    کار->jv_argv = argv;

    إذا (جمله_کار != عدم)
	*جمله_کار = کار;

#اگر_تعریف_شده USE_ARGV
    إذا (ch_log_فعال())
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
	ch_log(عدم, "Starting job: %s", (حرف *)ga.ga_داده);
	ga_clear(&ga);
    }
    mch_کار_شروع(argv, کار, &opt, جمله_کار != عدم);
#وإلا
    ch_log(عدم, "Starting job: %s", (حرف *)cmd);
    mch_کار_شروع((حرف *)cmd, کار, &opt);
#پایان_اگر

    // If the channel is reading from a buffer, write lines now.
    إذا (کار->jv_channel != عدم)
	channel_write_درون(کار->jv_channel);

theend:
#اگر_تعریف_نشده USE_ARGV
    vim_free(ga.ga_داده);
#پایان_اگر
    إذا (argv != عدم && argv != کار->jv_argv)
    {
	لكل (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_کار_options(&opt);
    أرجع کار;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    حرف *
کار_وضعیت(کار_T *کار)
{
    حرف	*نتیجه;

    إذا (کار->jv_وضعیت >= کار_ENDED)
	// No need to check, dead is dead.
	نتیجه = "dead";
    وإلا إذا (کار->jv_وضعیت == کار_FAILED)
	نتیجه = "fail";
    وإلا
    {
	نتیجه = mch_کار_وضعیت(کار);
	إذا (کار->jv_وضعیت == کار_ENDED)
	    کار_cleanup(کار);
    }
    أرجع نتیجه;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحيح
کار_stop(کار_T *کار, typval_T *argvars, حرف *type)
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
	    emsg(_(e_invalid_آرگومان));
	    أرجع 0;
	}
    }
    إذا (کار->jv_وضعیت == کار_FAILED)
    {
	ch_log(کار->jv_channel, "Job failed to start, job_stop() skipped");
	أرجع 0;
    }
    إذا (کار->jv_وضعیت == کار_ENDED)
    {
	ch_log(کار->jv_channel, "Job has already ended, job_stop() skipped");
	أرجع 0;
    }
    ch_log(کار->jv_channel, "Stopping job with '%s'", (حرف *)arg);
    إذا (mch_signal_کار(کار, arg) == FAIL)
	أرجع 0;

    // Assume that only "kill" will kill the job.
    إذا (کار->jv_channel != عدم && STRCMP(arg, "kill") == 0)
	کار->jv_channel->ch_کار_killed = TRUE;

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
    linenr_T	lnum = curbuf->b_ml.ml_سطر_شمار;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.ستون = 0;

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
invoke_prompt_وقفه(فراغ)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحيح		ret;

    إذا (curbuf->b_prompt_وقفه.cb_name == عدم
	    || *curbuf->b_prompt_وقفه.cb_name == NUL)
	أرجع FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_وقفه, -1, &rettv, 0, argv);
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

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
    متن = ml_get_curline();
    إذا (STRNCMP(متن, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	إذا (*متن == NUL)
	    ml_replace(curbuf->b_ml.ml_سطر_شمار, prompt, TRUE);
	وإلا
	    ml_append(curbuf->b_ml.ml_سطر_شمار, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	coladvance((colnr_T)MAXCOL);
	تغییریافته_بایتها(curbuf->b_ml.ml_سطر_شمار, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    إذا (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.ستون != (صحيح)STRLEN(prompt))
	مجموعه_insstart(curwin->w_cursor.lnum, (صحيح)STRLEN(prompt));

    إذا (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    إذا (curwin->w_cursor.ستون < (صحيح)STRLEN(prompt))
	curwin->w_cursor.ستون = (صحيح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحيح
prompt_curpos_editable(فراغ)
{
    أرجع curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_شمار
	&& curwin->w_cursor.ستون >= (صحيح)STRLEN(prompt_متن());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    فراغ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_callback);
    مجموعه_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    فراغ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_وقفه);
    مجموعه_callback(&buf->b_prompt_وقفه, &callback);
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

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
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
f_prompt_setprompt(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    char_u	*متن;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
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
    ساكن کار_T *
get_کار_arg(typval_T *tv)
{
    کار_T *کار;

    إذا (tv->v_type != VAR_کار)
    {
	semsg(_(e_invalid_آرگومان_str), tv_get_string(tv));
	أرجع عدم;
    }
    کار = tv->vval.v_کار;

    إذا (کار == عدم)
	emsg(_(e_not_valid_کار));
    أرجع کار;
}

/*
 * "job_getchannel()" function
 */
    فراغ
f_کار_getchannel(typval_T *argvars, typval_T *rettv)
{
    کار_T	*کار;

    إذا (درون_vim9script() && check_for_کار_arg(argvars, 0) == FAIL)
	أرجع;

    کار = get_کار_arg(&argvars[0]);
    إذا (کار == عدم)
	أرجع;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = کار->jv_channel;
    إذا (کار->jv_channel != عدم)
	++کار->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ساكن فراغ
کار_info(کار_T *کار, dict_T *dict)
{
    dictitem_T	*قلم;
    varnumber_T	nr;
    list_T	*l;
    صحيح		i;

    dict_جمع_کن_string(dict, "status", (char_u *)کار_وضعیت(کار));

    قلم = dictitem_alloc((char_u *)"channel");
    إذا (قلم == عدم)
	أرجع;
    قلم->di_tv.v_type = VAR_CHANNEL;
    قلم->di_tv.vval.v_channel = کار->jv_channel;
    إذا (کار->jv_channel != عدم)
	++کار->jv_channel->ch_refcount;
    إذا (dict_جمع_کن(dict, قلم) == FAIL)
	dictitem_free(قلم);

#اگر_تعریف_شده UNIX
    nr = کار->jv_pid;
#وإلا
    nr = کار->jv_proc_info.dwProcessId;
#پایان_اگر
    dict_جمع_کن_عدد(dict, "process", nr);
    dict_جمع_کن_string(dict, "tty_in", کار->jv_tty_درون);
    dict_جمع_کن_string(dict, "tty_out", کار->jv_tty_بیرون);

    dict_جمع_کن_عدد(dict, "exitval", کار->jv_exitval);
    dict_جمع_کن_string(dict, "exit_cb", کار->jv_exit_cb.cb_name);
    dict_جمع_کن_string(dict, "stoponexit", کار->jv_stoponexit);
#اگر_تعریف_شده UNIX
    dict_جمع_کن_string(dict, "termsig", کار->jv_termsig);
#پایان_اگر
#اگر_تعریف_شده MSWIN
    dict_جمع_کن_string(dict, "tty_type", کار->jv_tty_type);
#پایان_اگر

    l = list_alloc();
    إذا (l == عدم)
	أرجع;

    إذا (dict_جمع_کن_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	أرجع;
    }
    إذا (کار->jv_argv != عدم)
	لكل (i = 0; کار->jv_argv[i] != عدم; i++)
	    list_append_string(l, (char_u *)کار->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ساكن فراغ
کار_info_all(list_T *l)
{
    کار_T	*کار;
    typval_T	tv;

    FOR_ALL_JOBS(کار)
    {
	tv.v_type = VAR_کار;
	tv.vval.v_کار = کار;

	إذا (list_append_tv(l, &tv) != OK)
	    أرجع;
    }
}

/*
 * "job_info()" function
 */
    فراغ
f_کار_info(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_opt_کار_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	کار_T	*کار;

	کار = get_کار_arg(&argvars[0]);
	إذا (کار != عدم && rettv_dict_alloc(rettv) == OK)
	    کار_info(کار, rettv->vval.v_dict);
    }
    وإلا إذا (rettv_list_alloc(rettv) == OK)
	کار_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    فراغ
f_کار_setoptions(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    کار_T	*کار;
    jobopt_T	opt;

    إذا (درون_vim9script()
	    && (check_for_کار_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    کار = get_کار_arg(&argvars[0]);
    إذا (کار == عدم)
	أرجع;
    clear_کار_options(&opt);
    إذا (get_کار_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	کار_مجموعه_options(کار, &opt);
    free_کار_options(&opt);
}

/*
 * "job_start()" function
 */
    فراغ
f_کار_شروع(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_کار;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_کار = کار_شروع(argvars, عدم, عدم, عدم);
}

/*
 * "job_status()" function
 */
    فراغ
f_کار_وضعیت(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_کار_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_کار && argvars[0].vval.v_کار == عدم)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    وإلا
    {
	کار_T	*کار = get_کار_arg(&argvars[0]);

	إذا (کار != عدم)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)کار_وضعیت(کار));
	}
    }
}

/*
 * "job_stop()" function
 */
    فراغ
f_کار_stop(typval_T *argvars, typval_T *rettv)
{
    کار_T	*کار;

    إذا (درون_vim9script()
	    && (check_for_کار_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    کار = get_کار_arg(&argvars[0]);
    إذا (کار != عدم)
	rettv->vval.v_عدد = کار_stop(کار, argvars, عدم);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
کار_مقصد_string_buf(typval_T *varp, char_u *buf)
{
    کار_T *کار = varp->vval.v_کار;
    حرف  *وضعیت;

    إذا (کار == عدم)
    {
	vim_snprintf((حرف *)buf, NUMBUFLEN, "no process");
	أرجع buf;
    }
    وضعیت = کار->jv_وضعیت == کار_FAILED ? "fail"
		    : کار->jv_وضعیت >= کار_ENDED ? "dead"
		    : "run";
#اگر_تعریف_شده UNIX
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s", (دراز)کار->jv_pid, وضعیت);
#وإلاإذا defined(MSWIN)
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s",
		(دراز)کار->jv_proc_info.dwProcessId,
		وضعیت);
#وإلا
    // fall-back
    vim_snprintf((حرف *)buf, NUMBUFLEN, "process ? %s", وضعیت);
#پایان_اگر
    أرجع buf;
}

#پایان_اگر // FEAT_JOB_CHANNEL
