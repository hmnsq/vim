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

#إذا defined(FEAT_کم_CHANNEL)

#تعریف_کرو FOR_ALL_JOBS(کم) \
    لكل ((کم) = پہلا_کم; (کم) != عدم; (کم) = (کم)->jv_اگلا)

    ساكن صحيح
handle_منوال(typval_T *شے, jobopt_T *opt, ch_منوال_T *modep, صحيح jo)
{
    char_u	*val = tv_get_string(شے);

    opt->jo_سیٹ |= jo;
    إذا (STRCMP(val, "nl") == 0)
	*modep = CH_منوال_NL;
    وإلا إذا (STRCMP(val, "raw") == 0)
	*modep = CH_منوال_RAW;
    وإلا إذا (STRCMP(val, "blob") == 0)
	*modep = CH_منوال_BLOB;
    وإلا إذا (STRCMP(val, "js") == 0)
	*modep = CH_منوال_JS;
    وإلا إذا (STRCMP(val, "json") == 0)
	*modep = CH_منوال_JSON;
    وإلا إذا (STRCMP(val, "lsp") == 0)
	*modep = CH_منوال_LSP;
    وإلا إذا (STRCMP(val, "dap") == 0)
	*modep = CH_منوال_DAP;
    وإلا
    {
	semsg(_(e_invalid_دلیل_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
handle_io(typval_T *شے, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(شے);

    opt->jo_سیٹ |= JO_باہرلا_IO << (part - PART_باہرلا);
    إذا (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    وإلا إذا (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    وإلا إذا (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    وإلا إذا (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_بفر;
    وإلا إذا (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_باہرلا;
    وإلا
    {
	semsg(_(e_invalid_دلیل_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    فراغ
clear_کم_options(jobopt_T *opt)
{
    CLEAR_اشارہ_گر(opt);
}

    ساكن فراغ
unref_کم_callback(callback_T *cb)
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
free_کم_options(jobopt_T *opt)
{
    unref_کم_callback(&opt->jo_callback);
    unref_کم_callback(&opt->jo_باہرلا_cb);
    unref_کم_callback(&opt->jo_err_cb);
    unref_کم_callback(&opt->jo_close_cb);
    unref_کم_callback(&opt->jo_exit_cb);

    إذا (opt->jo_env != عدم)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ساكن صحيح
part_from_char(صحيح c)
{
    أرجع c == 'i' ? PART_وچ : c == 'o' ? PART_باہرلا: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحيح
get_کم_options(typval_T *tv, jobopt_T *opt, صحيح supported, صحيح supported2)
{
    typval_T	*شے;
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

    todo = (صحيح)dict->dv_hashtab.ht_ورتیا;
    FOR_ALL_HASHTAB_اشیاء(&dict->dv_hashtab, hi, todo)
	إذا (!HASHITEM_خالی_تھاواں(hi))
	{
	    شے = &dict_lookup(hi)->di_tv;

	    إذا (STRCMP(hi->hi_کلید, "mode") == 0)
	    {
		إذا (!(supported & JO_منوال))
		    اكسر;
		إذا (handle_منوال(شے, opt, &opt->jo_منوال, JO_منوال) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_mode") == 0)
	    {
		إذا (!(supported & JO_وچ_منوال))
		    اكسر;
		إذا (handle_منوال(شے, opt, &opt->jo_وچ_منوال, JO_وچ_منوال)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_mode") == 0)
	    {
		إذا (!(supported & JO_باہرلا_منوال))
		    اكسر;
		إذا (handle_منوال(شے, opt, &opt->jo_باہرلا_منوال, JO_باہرلا_منوال)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_mode") == 0)
	    {
		إذا (!(supported & JO_ERR_منوال))
		    اكسر;
		إذا (handle_منوال(شے, opt, &opt->jo_err_منوال, JO_ERR_منوال)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "noblock") == 0)
	    {
		إذا (!(supported & JO_منوال))
		    اكسر;
		opt->jo_noblock = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_io") == 0
		    || STRCMP(hi->hi_کلید, "out_io") == 0
		    || STRCMP(hi->hi_کلید, "err_io") == 0)
	    {
		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		إذا (handle_io(شے, part_from_char(*hi->hi_کلید), opt) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_name") == 0
		    || STRCMP(hi->hi_کلید, "out_name") == 0
		    || STRCMP(hi->hi_کلید, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		opt->jo_سیٹ |= JO_باہرلا_NAME << (part - PART_باہرلا);
		opt->jo_io_name[part] = tv_get_string_buf_chk(شے,
						   opt->jo_io_name_buf[part]);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "pty") == 0)
	    {
		إذا (!(supported & JO_منوال))
		    اكسر;
		opt->jo_pty = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_buf") == 0
		    || STRCMP(hi->hi_کلید, "out_buf") == 0
		    || STRCMP(hi->hi_کلید, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		opt->jo_سیٹ |= JO_باہرلا_BUF << (part - PART_باہرلا);
		opt->jo_io_buf[part] = tv_get_نمبر(شے);
		إذا (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
					      hi->hi_کلید, tv_get_string(شے));
		    أرجع FAIL;
		}
		إذا (buflist_findnr(opt->jo_io_buf[part]) == عدم)
		{
		    semsg(_(e_بفر_nr_does_not_exist),
						   (لما)opt->jo_io_buf[part]);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_modifiable") == 0
		    || STRCMP(hi->hi_کلید, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		opt->jo_سیٹ |= JO_باہرلا_MODIFIABLE << (part - PART_باہرلا);
		opt->jo_modifiable[part] = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_msg") == 0
		    || STRCMP(hi->hi_کلید, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		opt->jo_set2 |= JO2_باہرلا_پیغام << (part - PART_باہرلا);
		opt->jo_سنیہا[part] = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "in_top") == 0
		    || STRCMP(hi->hi_کلید, "in_bot") == 0)
	    {
		linenr_T *lp;

		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		إذا (hi->hi_کلید[3] == 't')
		{
		    lp = &opt->jo_وچ_چوٹی;
		    opt->jo_سیٹ |= JO_وچ_چوٹی;
		}
		وإلا
		{
		    lp = &opt->jo_وچ_bot;
		    opt->jo_سیٹ |= JO_وچ_BOT;
		}
		*lp = tv_get_نمبر(شے);
		إذا (*lp < 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
					      hi->hi_کلید, tv_get_string(شے));
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "channel") == 0)
	    {
		إذا (!(supported & JO_باہرلا_IO))
		    اكسر;
		opt->jo_سیٹ |= JO_CHANNEL;
		إذا (شے->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "channel");
		    أرجع FAIL;
		}
		opt->jo_channel = شے->vval.v_channel;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "callback") == 0)
	    {
		إذا (!(supported & JO_CALLBACK))
		    اكسر;
		opt->jo_سیٹ |= JO_CALLBACK;
		opt->jo_callback = get_callback(شے);
		إذا (opt->jo_callback.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "callback");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_cb") == 0)
	    {
		إذا (!(supported & JO_باہرلا_CALLBACK))
		    اكسر;
		opt->jo_سیٹ |= JO_باہرلا_CALLBACK;
		opt->jo_باہرلا_cb = get_callback(شے);
		إذا (opt->jo_باہرلا_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "out_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_cb") == 0)
	    {
		إذا (!(supported & JO_ERR_CALLBACK))
		    اكسر;
		opt->jo_سیٹ |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(شے);
		إذا (opt->jo_err_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "err_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "close_cb") == 0)
	    {
		إذا (!(supported & JO_CLOSE_CALLBACK))
		    اكسر;
		opt->jo_سیٹ |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(شے);
		إذا (opt->jo_close_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "close_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "drop") == 0)
	    {
		صحيح never = FALSE;
		val = tv_get_string(شے);

		إذا (STRCMP(val, "never") == 0)
		    never = TRUE;
		وإلا إذا (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), "drop", val);
		    أرجع FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "exit_cb") == 0)
	    {
		إذا (!(supported & JO_EXIT_CB))
		    اكسر;
		opt->jo_سیٹ |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(شے);
		إذا (opt->jo_exit_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "exit_cb");
		    أرجع FAIL;
		}
	    }
#جے_تعریف_شدہ FEAT_TERMINAL
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_name") == 0)
	    {
		إذا (!(supported2 & JO2_رکن_NAME))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_NAME;
		opt->jo_رکن_name = tv_get_string_buf_chk(شے,
						       opt->jo_رکن_name_buf);
		إذا (opt->jo_رکن_name == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_name");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_finish") == 0)
	    {
		إذا (!(supported2 & JO2_رکن_انت))
		    اكسر;
		val = tv_get_string(شے);
		إذا (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
							   "term_finish", val);
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_رکن_انت;
		opt->jo_رکن_انت = *val;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_opencmd") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_رکن_OPENCMD))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_OPENCMD;
		p = opt->jo_رکن_opencmd = tv_get_string_buf_chk(شے,
						    opt->jo_رکن_opencmd_buf);
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
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_opencmd");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "eof_chars") == 0)
	    {
		إذا (!(supported2 & JO2_EOF_CHARS))
		    اكسر;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(شے,
						       opt->jo_eof_chars_buf);
		إذا (opt->jo_eof_chars == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "eof_chars");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_rows") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_رکن_صفاں))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_صفاں;
		opt->jo_رکن_صفاں = tv_get_نمبر_chk(شے, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_رکن_صفاں < 0 || opt->jo_رکن_صفاں > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_rows");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_cols") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_رکن_خانے_گنتی))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_خانے_گنتی;
		opt->jo_رکن_خانے_گنتی = tv_get_نمبر_chk(شے, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_رکن_خانے_گنتی < 0 || opt->jo_رکن_خانے_گنتی > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_cols");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "vertical") == 0)
	    {
		إذا (!(supported2 & JO2_VERTICAL))
		    اكسر;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "curwin") == 0)
	    {
		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "bufnr") == 0)
	    {
		صحيح nr;

		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_نمبر(شے);
		إذا (nr <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), hi->hi_کلید, tv_get_string(شے));
		    أرجع FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		إذا (opt->jo_bufnr_buf == عدم)
		{
		    semsg(_(e_بفر_nr_does_not_exist), (لما)nr);
		    أرجع FAIL;
		}
		إذا (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_رکن == عدم)
		{
		    semsg(_(e_invalid_دلیل_str), "bufnr");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "hidden") == 0)
	    {
		إذا (!(supported2 & JO2_HIDDEN))
		    اكسر;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "norestore") == 0)
	    {
		إذا (!(supported2 & JO2_NORESTORE))
		    اكسر;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_رکن_norestore = tv_get_bool(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_kill") == 0)
	    {
		إذا (!(supported2 & JO2_رکن_KILL))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_KILL;
		opt->jo_رکن_kill = tv_get_string_buf_chk(شے,
						       opt->jo_رکن_kill_buf);
		إذا (opt->jo_رکن_kill == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_kill");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "tty_type") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TTY_TYPE))
		    اكسر;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(شے);
		إذا (p == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "tty_type");
		    أرجع FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		إذا (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "tty_type");
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

		إذا (شے == عدم || شے->v_type != VAR_LIST
			|| شے->vval.v_list == عدم
			|| شے->vval.v_list->lv_پہلا == &range_list_شے)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "ansi_colors");
		    أرجع FAIL;
		}

		li = شے->vval.v_list->lv_پہلا;
		لكل (; li != عدم && n < 16; li = li->li_اگلا, n++)
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
		    semsg(_(e_invalid_قدر_for_دلیل_str), "ansi_colors");
		    أرجع FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		حافظہ_نقل(opt->jo_ansi_colors, rgb, حجم(rgb));
	    }
# endif
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_highlight") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_رکن_HIGHLIGHT))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_HIGHLIGHT;
		p = tv_get_string_buf_chk(شے, opt->jo_رکن_highlight_buf);
		إذا (p == عدم || *p == NUL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_highlight");
		    أرجع FAIL;
		}
		opt->jo_رکن_highlight = p;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "term_api") == 0)
	    {
		إذا (!(supported2 & JO2_رکن_API))
		    اكسر;
		opt->jo_set2 |= JO2_رکن_API;
		opt->jo_رکن_api = tv_get_string_buf_chk(شے,
							opt->jo_رکن_api_buf);
		إذا (opt->jo_رکن_api == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_api");
		    أرجع FAIL;
		}
	    }
#جے_انت
	    وإلا إذا (STRCMP(hi->hi_کلید, "env") == 0)
	    {
		إذا (!(supported2 & JO2_ENV))
		    اكسر;
		إذا (شے->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "env");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = شے->vval.v_dict;
		إذا (opt->jo_env != عدم)
		    ++opt->jo_env->dv_refcount;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "cwd") == 0)
	    {
		إذا (!(supported2 & JO2_CWD))
		    اكسر;
		opt->jo_cwd = tv_get_string_buf_chk(شے, opt->jo_cwd_buf);
		إذا (opt->jo_cwd == عدم || !mch_isdir(opt->jo_cwd)
#جے_بنا_تعریف MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((حرف *)opt->jo_cwd, X_OK) != 0
#جے_انت
				)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "cwd");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "waittime") == 0)
	    {
		إذا (!(supported & JO_WAITTIME))
		    اكسر;
		opt->jo_سیٹ |= JO_WAITTIME;
		opt->jo_waittime = tv_get_نمبر(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "timeout") == 0)
	    {
		إذا (!(supported & JO_TIMEOUT))
		    اكسر;
		opt->jo_سیٹ |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_نمبر(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "out_timeout") == 0)
	    {
		إذا (!(supported & JO_باہرلا_TIMEOUT))
		    اكسر;
		opt->jo_سیٹ |= JO_باہرلا_TIMEOUT;
		opt->jo_باہرلا_timeout = tv_get_نمبر(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "err_timeout") == 0)
	    {
		إذا (!(supported & JO_ERR_TIMEOUT))
		    اكسر;
		opt->jo_سیٹ |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_نمبر(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "part") == 0)
	    {
		إذا (!(supported & JO_PART))
		    اكسر;
		opt->jo_سیٹ |= JO_PART;
		val = tv_get_string(شے);
		إذا (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		وإلا إذا (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_باہرلا;
		وإلا
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), "part", val);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "id") == 0)
	    {
		إذا (!(supported & JO_ID))
		    اكسر;
		opt->jo_سیٹ |= JO_ID;
		opt->jo_id = tv_get_نمبر(شے);
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "stoponexit") == 0)
	    {
		إذا (!(supported & JO_STOPONEXIT))
		    اكسر;
		opt->jo_سیٹ |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(شے,
						      opt->jo_stoponexit_buf);
		إذا (opt->jo_stoponexit == عدم)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "stoponexit");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_کلید, "block_write") == 0)
	    {
		إذا (!(supported & JO_بلاک_WRITE))
		    اكسر;
		opt->jo_سیٹ |= JO_بلاک_WRITE;
		opt->jo_بلاک_write = tv_get_نمبر(شے);
	    }
	    وإلا
		اكسر;
	    --todo;
	}
    إذا (todo > 0)
    {
	semsg(_(e_invalid_دلیل_str), hi->hi_کلید);
	أرجع FAIL;
    }

    أرجع OK;
}

ساكن کم_T *پہلا_کم = عدم;

    ساكن فراغ
کم_free_contents(کم_T *کم)
{
    صحيح		i;

    ch_log(کم->jv_channel, "Freeing job");
    إذا (کم->jv_channel != عدم)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	کم->jv_channel->ch_کم = عدم;
	channel_unref(کم->jv_channel);
    }
    mch_clear_کم(کم);

    vim_free(کم->jv_tty_وچ);
    vim_free(کم->jv_tty_باہرلا);
    vim_free(کم->jv_stoponexit);
#جے_تعریف_شدہ UNIX
    vim_free(کم->jv_termsig);
#جے_انت
#جے_تعریف_شدہ MSWIN
    vim_free(کم->jv_tty_type);
#جے_انت
    free_callback(&کم->jv_exit_cb);
    إذا (کم->jv_argv != عدم)
    {
	لكل (i = 0; کم->jv_argv[i] != عدم; i++)
	    vim_free(کم->jv_argv[i]);
	vim_free(کم->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ساكن فراغ
کم_unlink(کم_T *کم)
{
    إذا (کم->jv_اگلا != عدم)
	کم->jv_اگلا->jv_prev = کم->jv_prev;
    إذا (کم->jv_prev == عدم)
	پہلا_کم = کم->jv_اگلا;
    وإلا
	کم->jv_prev->jv_اگلا = کم->jv_اگلا;
}

    ساكن فراغ
کم_free_کم(کم_T *کم)
{
    کم_unlink(کم);
    vim_free(کم);
}

    ساكن فراغ
کم_free(کم_T *کم)
{
    إذا (وچ_free_unref_اشیاء)
	أرجع;

    کم_free_contents(کم);
    کم_free_کم(کم);
}

ساكن کم_T *jobs_منزل_free = عدم;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ساكن فراغ
کم_free_later(کم_T *کم)
{
    کم_unlink(کم);
    کم->jv_اگلا = jobs_منزل_free;
    jobs_منزل_free = کم;
}

    ساكن فراغ
free_jobs_منزل_free_later(فراغ)
{
    کم_T *کم;

    طالما (jobs_منزل_free != عدم)
    {
	کم = jobs_منزل_free;
	jobs_منزل_free = کم->jv_اگلا;
	کم_free_contents(کم);
	vim_free(کم);
    }
}

#إذا defined(EXITFREE)
    فراغ
کم_free_all(فراغ)
{
    طالما (پہلا_کم != عدم)
	کم_free(پہلا_کم);
    free_jobs_منزل_free_later();

# ifdef FEAT_TERMINAL
    free_بن_ورتیا_terminals();
# endif
}
#جے_انت

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ساكن صحيح
کم_need_سرا_check(کم_T *کم)
{
    أرجع کم->jv_حالت == کم_STARTED
	    && (کم->jv_stoponexit != عدم || کم->jv_exit_cb.cb_name != عدم);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ساكن صحيح
کم_channel_still_useful(کم_T *کم)
{
    أرجع کم->jv_channel != عدم && channel_still_useful(کم->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ساكن صحيح
کم_channel_can_close(کم_T *کم)
{
    أرجع کم->jv_channel != عدم && channel_can_close(کم->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ساكن صحيح
کم_still_useful(کم_T *کم)
{
    أرجع کم_need_سرا_check(کم) || کم_channel_still_useful(کم);
}

#إذا defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحيح
کم_any_running(فراغ)
{
    کم_T	*کم;

    FOR_ALL_JOBS(کم)
	إذا (کم_still_useful(کم))
	{
	    ch_log(عدم, "GUI not forking because a job is running");
	    أرجع TRUE;
	}
    أرجع FALSE;
}
#جے_انت

// Unix uses argv[] for the command, other systems use a string.
#إذا defined(UNIX)
# define USE_ARGV
#جے_انت

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
    FOR_ALL_LIST_اشیاء(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	إذا (s == عدم)
	    أرجع FAIL;
	s = win32_escape_arg(s);
	إذا (s == عدم)
	    أرجع FAIL;
	ga_concat(gap, s);
	vim_free(s);
	إذا (li->li_اگلا != عدم)
	    ga_append(gap, ' ');
    }
    أرجع OK;
}
#جے_انت

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    فراغ
کم_cleanup(کم_T *کم)
{
    إذا (کم->jv_حالت != کم_ENDED)
	أرجع;

    // Ready to cleanup the job.
    کم->jv_حالت = کم_مکمل;

    // When only channel-in is kept open, close explicitly.
    إذا (کم->jv_channel != عدم)
	ch_close_part(کم->jv_channel, PART_وچ);

    إذا (کم->jv_exit_cb.cb_name != عدم)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(کم->jv_channel, "Invoking exit callback %s",
						      کم->jv_exit_cb.cb_name);
	++کم->jv_refcount;
	argv[0].v_type = VAR_کم;
	argv[0].vval.v_کم = کم;
	argv[1].v_type = VAR_نمبر;
	argv[1].vval.v_نمبر = کم->jv_exitval;
	call_callback(&کم->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--کم->jv_refcount;
	channel_need_redraw = TRUE;
    }

    إذا (کم->jv_channel != عدم && کم->jv_channel->ch_anonymous_pipe)
	کم->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    إذا (کم->jv_refcount == 0 && !کم_channel_still_useful(کم))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	کم_free_later(کم);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحيح
سیٹ_ref_وچ_کم(صحيح copyID)
{
    صحيح		abort = FALSE;
    کم_T	*کم;
    typval_T	tv;

    لكل (کم = پہلا_کم; !abort && کم != عدم; کم = کم->jv_اگلا)
	إذا (کم_still_useful(کم))
	{
	    tv.v_type = VAR_کم;
	    tv.vval.v_کم = کم;
	    abort = abort || سیٹ_ref_وچ_شے(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    فراغ
کم_unref(کم_T *کم)
{
    إذا (کم == عدم || --کم->jv_refcount > 0)
	أرجع;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    إذا (کم_channel_still_useful(کم))
	أرجع;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    إذا (!کم_need_سرا_check(کم))
    {
	کم_free(کم);
    }
    وإلا إذا (کم->jv_channel != عدم)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(کم->jv_channel, "detaching channel from job");
	کم->jv_channel->ch_کم = عدم;
	channel_unref(کم->jv_channel);
	کم->jv_channel = عدم;
    }
}

    صحيح
free_بن_ورتیا_jobs_contents(صحيح copyID, صحيح نقاب)
{
    صحيح		did_free = FALSE;
    کم_T	*کم;

    FOR_ALL_JOBS(کم)
	إذا ((کم->jv_copyID & نقاب) != (copyID & نقاب)
						    && !کم_still_useful(کم))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    کم_free_contents(کم);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    فراغ
free_بن_ورتیا_jobs(صحيح copyID, صحيح نقاب)
{
    کم_T	*کم;
    کم_T	*کم_اگلا;

    لكل (کم = پہلا_کم; کم != عدم; کم = کم_اگلا)
    {
	کم_اگلا = کم->jv_اگلا;
	إذا ((کم->jv_copyID & نقاب) != (copyID & نقاب)
						    && !کم_still_useful(کم))
	{
	    // Free the job struct itself.
	    کم_free_کم(کم);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    کم_T *
کم_alloc(فراغ)
{
    کم_T *کم;

    کم = ALLOC_CLEAR_ONE(کم_T);
    إذا (کم == عدم)
	أرجع عدم;

    کم->jv_refcount = 1;
    کم->jv_stoponexit = vim_strsave((char_u *)"term");

    إذا (پہلا_کم != عدم)
    {
	پہلا_کم->jv_prev = کم;
	کم->jv_اگلا = پہلا_کم;
    }
    پہلا_کم = کم;
    أرجع کم;
}

    فراغ
کم_سیٹ_options(کم_T *کم, jobopt_T *opt)
{
    إذا (opt->jo_سیٹ & JO_STOPONEXIT)
    {
	vim_free(کم->jv_stoponexit);
	إذا (opt->jo_stoponexit == عدم || *opt->jo_stoponexit == NUL)
	    کم->jv_stoponexit = عدم;
	وإلا
	    کم->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    إذا (opt->jo_سیٹ & JO_EXIT_CB)
    {
	free_callback(&کم->jv_exit_cb);
	إذا (opt->jo_exit_cb.cb_name == عدم || *opt->jo_exit_cb.cb_name == NUL)
	{
	    کم->jv_exit_cb.cb_name = عدم;
	    کم->jv_exit_cb.cb_partial = عدم;
	}
	وإلا
	    copy_callback(&کم->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    فراغ
کم_stop_on_exit(فراغ)
{
    کم_T	*کم;

    FOR_ALL_JOBS(کم)
	إذا (کم->jv_حالت == کم_STARTED && کم->jv_stoponexit != عدم)
	    mch_signal_کم(کم, کم->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحيح
has_pending_کم(فراغ)
{
    کم_T	    *کم;

    FOR_ALL_JOBS(کم)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	إذا ((کم->jv_حالت == کم_STARTED && !کم_channel_still_useful(کم))
		    || (کم->jv_حالت == کم_مکمل
					      && کم_channel_can_close(کم)))
	    أرجع TRUE;
    أرجع FALSE;
}

#تعریف_کرو MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحيح
کم_check_ended(فراغ)
{
    صحيح		i;
    صحيح		did_سرا = FALSE;

    // be quick if there are no jobs to check
    إذا (پہلا_کم == عدم)
	أرجع did_سرا;

    لكل (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	کم_T	*کم = mch_detect_ended_کم(پہلا_کم);

	إذا (کم == عدم)
	    اكسر;
	did_سرا = TRUE;
	کم_cleanup(کم); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_منزل_free_later();

    إذا (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    أرجع did_سرا;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    کم_T *
کم_شروع(
	typval_T    *argvars,
	حرف	    **argv_arg بن_ورتیا,
	jobopt_T    *opt_arg,
	کم_T	    **رکن_کم)
{
    کم_T	*کم;
    char_u	*cmd = عدم;
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;
#جے_بنا_تعریف USE_ARGV
    garray_T	ga;
#جے_انت
    jobopt_T	opt;
    ch_part_T	part;

    کم = کم_alloc();
    إذا (کم == عدم)
	أرجع عدم;

    کم->jv_حالت = کم_FAILED;
#جے_بنا_تعریف USE_ARGV
    ga_init2(&ga, حجم(حرف*), 20);
#جے_انت

    إذا (opt_arg != عدم)
	opt = *opt_arg;
    وإلا
    {
	// Default mode is NL.
	clear_کم_options(&opt);
	opt.jo_منوال = CH_منوال_NL;
	إذا (get_کم_options(&argvars[1], &opt,
		    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_باہرلا_IO + JO_بلاک_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    اذهب theend;
    }

    // Check that when io is "file" that there is a file name.
    لكل (part = PART_باہرلا; part < PART_گنتی; ++part)
	إذا ((opt.jo_سیٹ & (JO_باہرلا_IO << (part - PART_باہرلا)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_سیٹ & (JO_باہرلا_NAME << (part - PART_باہرلا)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_منزل_be_سیٹ));
	    اذهب theend;
	}

    إذا ((opt.jo_سیٹ & JO_وچ_IO) && opt.jo_io[PART_وچ] == JIO_بفر)
    {
	buf_T *buf = عدم;

	// check that we can find the buffer before starting the job
	إذا (opt.jo_سیٹ & JO_وچ_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_وچ]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
						 (لما)opt.jo_io_buf[PART_وچ]);
	}
	وإلا إذا (!(opt.jo_سیٹ & JO_وچ_NAME))
	{
	    emsg(_(e_وچ_io_بفر_requires_وچ_buf_or_وچ_name_منزل_be_سیٹ));
	}
	وإلا
	    buf = buflist_لبھو_by_name(opt.jo_io_name[PART_وچ], FALSE);
	إذا (buf == عدم)
	    اذهب theend;
	إذا (buf->b_ml.ml_mfp == عدم)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    إذا (opt.jo_سیٹ & JO_وچ_BUF)
	    {
		sprintf((حرف *)numbuf, "%d", opt.jo_io_buf[PART_وچ]);
		s = numbuf;
	    }
	    وإلا
		s = opt.jo_io_name[PART_وچ];
	    semsg(_(e_بفر_must_be_loaded_str), s);
	    اذهب theend;
	}
	کم->jv_وچ_buf = buf;
    }

    کم_سیٹ_options(کم, &opt);

#جے_تعریف_شدہ USE_ARGV
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
#جے_انت
    إذا (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    اذهب theend;
	}

	إذا (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    اذهب theend;
    }
    وإلا إذا (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == عدم
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_دلیل));
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
	    emsg(_(e_invalid_دلیل));
	    اذهب theend;
	}
#جے_بنا_تعریف USE_ARGV
	إذا (win32_build_cmd(l, &ga) == FAIL)
	    اذهب theend;
	cmd = ga.ga_انکڑے;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    اذهب theend;
	}
#جے_انت
    }

    // Save the command used to start the job.
    کم->jv_argv = argv;

    إذا (رکن_کم != عدم)
	*رکن_کم = کم;

#جے_تعریف_شدہ USE_ARGV
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
	ch_log(عدم, "Starting job: %s", (حرف *)ga.ga_انکڑے);
	ga_clear(&ga);
    }
    mch_کم_شروع(argv, کم, &opt, رکن_کم != عدم);
#وإلا
    ch_log(عدم, "Starting job: %s", (حرف *)cmd);
    mch_کم_شروع((حرف *)cmd, کم, &opt);
#جے_انت

    // If the channel is reading from a buffer, write lines now.
    إذا (کم->jv_channel != عدم)
	channel_write_وچ(کم->jv_channel);

theend:
#جے_بنا_تعریف USE_ARGV
    vim_free(ga.ga_انکڑے);
#جے_انت
    إذا (argv != عدم && argv != کم->jv_argv)
    {
	لكل (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_کم_options(&opt);
    أرجع کم;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    حرف *
کم_حالت(کم_T *کم)
{
    حرف	*نتیجہ;

    إذا (کم->jv_حالت >= کم_ENDED)
	// No need to check, dead is dead.
	نتیجہ = "dead";
    وإلا إذا (کم->jv_حالت == کم_FAILED)
	نتیجہ = "fail";
    وإلا
    {
	نتیجہ = mch_کم_حالت(کم);
	إذا (کم->jv_حالت == کم_ENDED)
	    کم_cleanup(کم);
    }
    أرجع نتیجہ;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحيح
کم_stop(کم_T *کم, typval_T *argvars, حرف *type)
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
	    emsg(_(e_invalid_دلیل));
	    أرجع 0;
	}
    }
    إذا (کم->jv_حالت == کم_FAILED)
    {
	ch_log(کم->jv_channel, "Job failed to start, job_stop() skipped");
	أرجع 0;
    }
    إذا (کم->jv_حالت == کم_ENDED)
    {
	ch_log(کم->jv_channel, "Job has already ended, job_stop() skipped");
	أرجع 0;
    }
    ch_log(کم->jv_channel, "Stopping job with '%s'", (حرف *)arg);
    إذا (mch_signal_کم(کم, arg) == FAIL)
	أرجع 0;

    // Assume that only "kill" will kill the job.
    إذا (کم->jv_channel != عدم && STRCMP(arg, "kill") == 0)
	کم->jv_channel->ch_کم_killed = TRUE;

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
    linenr_T	lnum = curbuf->b_ml.ml_سطر_گنتی;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.خانہ = 0;

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
invoke_prompt_رکاوٹ(فراغ)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحيح		ret;

    إذا (curbuf->b_prompt_رکاوٹ.cb_name == عدم
	    || *curbuf->b_prompt_رکاوٹ.cb_name == NUL)
	أرجع FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_رکاوٹ, -1, &rettv, 0, argv);
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

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    متن = ml_get_curline();
    إذا (STRNCMP(متن, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	إذا (*متن == NUL)
	    ml_replace(curbuf->b_ml.ml_سطر_گنتی, prompt, TRUE);
	وإلا
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	coladvance((colnr_T)MAXCOL);
	بدلیا_بائٹاں(curbuf->b_ml.ml_سطر_گنتی, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    إذا (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.خانہ != (صحيح)STRLEN(prompt))
	سیٹ_insstart(curwin->w_cursor.lnum, (صحيح)STRLEN(prompt));

    إذا (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    إذا (curwin->w_cursor.خانہ < (صحيح)STRLEN(prompt))
	curwin->w_cursor.خانہ = (صحيح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحيح
prompt_curpos_editable(فراغ)
{
    أرجع curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_گنتی
	&& curwin->w_cursor.خانہ >= (صحيح)STRLEN(prompt_متن());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    فراغ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_callback);
    سیٹ_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    فراغ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_رکاوٹ);
    سیٹ_callback(&buf->b_prompt_رکاوٹ, &callback);
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

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
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
f_prompt_setprompt(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    char_u	*متن;

    إذا (وچ_vim9script()
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
    ساكن کم_T *
get_کم_arg(typval_T *tv)
{
    کم_T *کم;

    إذا (tv->v_type != VAR_کم)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(tv));
	أرجع عدم;
    }
    کم = tv->vval.v_کم;

    إذا (کم == عدم)
	emsg(_(e_not_valid_کم));
    أرجع کم;
}

/*
 * "job_getchannel()" function
 */
    فراغ
f_کم_getchannel(typval_T *argvars, typval_T *rettv)
{
    کم_T	*کم;

    إذا (وچ_vim9script() && check_for_کم_arg(argvars, 0) == FAIL)
	أرجع;

    کم = get_کم_arg(&argvars[0]);
    إذا (کم == عدم)
	أرجع;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = کم->jv_channel;
    إذا (کم->jv_channel != عدم)
	++کم->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ساكن فراغ
کم_info(کم_T *کم, dict_T *dict)
{
    dictitem_T	*شے;
    varnumber_T	nr;
    list_T	*l;
    صحيح		i;

    dict_جوڑو_string(dict, "status", (char_u *)کم_حالت(کم));

    شے = dictitem_alloc((char_u *)"channel");
    إذا (شے == عدم)
	أرجع;
    شے->di_tv.v_type = VAR_CHANNEL;
    شے->di_tv.vval.v_channel = کم->jv_channel;
    إذا (کم->jv_channel != عدم)
	++کم->jv_channel->ch_refcount;
    إذا (dict_جوڑو(dict, شے) == FAIL)
	dictitem_free(شے);

#جے_تعریف_شدہ UNIX
    nr = کم->jv_pid;
#وإلا
    nr = کم->jv_proc_info.dwProcessId;
#جے_انت
    dict_جوڑو_نمبر(dict, "process", nr);
    dict_جوڑو_string(dict, "tty_in", کم->jv_tty_وچ);
    dict_جوڑو_string(dict, "tty_out", کم->jv_tty_باہرلا);

    dict_جوڑو_نمبر(dict, "exitval", کم->jv_exitval);
    dict_جوڑو_string(dict, "exit_cb", کم->jv_exit_cb.cb_name);
    dict_جوڑو_string(dict, "stoponexit", کم->jv_stoponexit);
#جے_تعریف_شدہ UNIX
    dict_جوڑو_string(dict, "termsig", کم->jv_termsig);
#جے_انت
#جے_تعریف_شدہ MSWIN
    dict_جوڑو_string(dict, "tty_type", کم->jv_tty_type);
#جے_انت

    l = list_alloc();
    إذا (l == عدم)
	أرجع;

    إذا (dict_جوڑو_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	أرجع;
    }
    إذا (کم->jv_argv != عدم)
	لكل (i = 0; کم->jv_argv[i] != عدم; i++)
	    list_append_string(l, (char_u *)کم->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ساكن فراغ
کم_info_all(list_T *l)
{
    کم_T	*کم;
    typval_T	tv;

    FOR_ALL_JOBS(کم)
    {
	tv.v_type = VAR_کم;
	tv.vval.v_کم = کم;

	إذا (list_append_tv(l, &tv) != OK)
	    أرجع;
    }
}

/*
 * "job_info()" function
 */
    فراغ
f_کم_info(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script() && check_for_opt_کم_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	کم_T	*کم;

	کم = get_کم_arg(&argvars[0]);
	إذا (کم != عدم && rettv_dict_alloc(rettv) == OK)
	    کم_info(کم, rettv->vval.v_dict);
    }
    وإلا إذا (rettv_list_alloc(rettv) == OK)
	کم_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    فراغ
f_کم_setoptions(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    کم_T	*کم;
    jobopt_T	opt;

    إذا (وچ_vim9script()
	    && (check_for_کم_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    کم = get_کم_arg(&argvars[0]);
    إذا (کم == عدم)
	أرجع;
    clear_کم_options(&opt);
    إذا (get_کم_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	کم_سیٹ_options(کم, &opt);
    free_کم_options(&opt);
}

/*
 * "job_start()" function
 */
    فراغ
f_کم_شروع(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_کم;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_کم = کم_شروع(argvars, عدم, عدم, عدم);
}

/*
 * "job_status()" function
 */
    فراغ
f_کم_حالت(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script() && check_for_کم_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_کم && argvars[0].vval.v_کم == عدم)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    وإلا
    {
	کم_T	*کم = get_کم_arg(&argvars[0]);

	إذا (کم != عدم)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)کم_حالت(کم));
	}
    }
}

/*
 * "job_stop()" function
 */
    فراغ
f_کم_stop(typval_T *argvars, typval_T *rettv)
{
    کم_T	*کم;

    إذا (وچ_vim9script()
	    && (check_for_کم_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_نمبر_arg(argvars, 1) == FAIL))
	أرجع;

    کم = get_کم_arg(&argvars[0]);
    إذا (کم != عدم)
	rettv->vval.v_نمبر = کم_stop(کم, argvars, عدم);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
کم_منزل_string_buf(typval_T *varp, char_u *buf)
{
    کم_T *کم = varp->vval.v_کم;
    حرف  *حالت;

    إذا (کم == عدم)
    {
	vim_snprintf((حرف *)buf, NUMBUFLEN, "no process");
	أرجع buf;
    }
    حالت = کم->jv_حالت == کم_FAILED ? "fail"
		    : کم->jv_حالت >= کم_ENDED ? "dead"
		    : "run";
#جے_تعریف_شدہ UNIX
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s", (لما)کم->jv_pid, حالت);
#وإلاإذا defined(MSWIN)
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s",
		(لما)کم->jv_proc_info.dwProcessId,
		حالت);
#وإلا
    // fall-back
    vim_snprintf((حرف *)buf, NUMBUFLEN, "process ? %s", حالت);
#جے_انت
    أرجع buf;
}

#جے_انت // FEAT_JOB_CHANNEL
