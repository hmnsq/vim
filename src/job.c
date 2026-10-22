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

#شامل "vim.h"

#اگر defined(FEAT_کام_CHANNEL)

#تعریف_کرو FOR_ALL_JOBS(کام) \
    برائے ((کام) = پہلا_کام; (کام) != تہی; (کام) = (کام)->jv_اگلا)

    ساکن صحیح
handle_منوال(typval_T *شے, jobopt_T *opt, ch_منوال_T *modep, صحیح jo)
{
    char_u	*val = tv_get_string(شے);

    opt->jo_سیٹ |= jo;
    اگر (STRCMP(val, "nl") == 0)
	*modep = CH_منوال_NL;
    ورنہ اگر (STRCMP(val, "raw") == 0)
	*modep = CH_منوال_RAW;
    ورنہ اگر (STRCMP(val, "blob") == 0)
	*modep = CH_منوال_BLOB;
    ورنہ اگر (STRCMP(val, "js") == 0)
	*modep = CH_منوال_JS;
    ورنہ اگر (STRCMP(val, "json") == 0)
	*modep = CH_منوال_JSON;
    ورنہ اگر (STRCMP(val, "lsp") == 0)
	*modep = CH_منوال_LSP;
    ورنہ اگر (STRCMP(val, "dap") == 0)
	*modep = CH_منوال_DAP;
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str), val);
	واپس FAIL;
    }
    واپس OK;
}

    ساکن صحیح
handle_io(typval_T *شے, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(شے);

    opt->jo_سیٹ |= JO_برآمد_IO << (part - PART_برآمد);
    اگر (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ورنہ اگر (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ورنہ اگر (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ورنہ اگر (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_بفر;
    ورنہ اگر (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_برآمد;
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str), val);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    خالی
clear_کام_options(jobopt_T *opt)
{
    CLEAR_اشارہ_گر(opt);
}

    ساکن خالی
unref_کام_callback(callback_T *cb)
{
    اگر (cb->cb_partial != تہی)
	partial_unref(cb->cb_partial);
    ورنہ اگر (cb->cb_name != تہی)
    {
	func_unref(cb->cb_name);
	اگر (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    خالی
free_کام_options(jobopt_T *opt)
{
    unref_کام_callback(&opt->jo_callback);
    unref_کام_callback(&opt->jo_برآمد_cb);
    unref_کام_callback(&opt->jo_err_cb);
    unref_کام_callback(&opt->jo_close_cb);
    unref_کام_callback(&opt->jo_exit_cb);

    اگر (opt->jo_env != تہی)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ساکن صحیح
part_from_char(صحیح c)
{
    واپس c == 'i' ? PART_اندر : c == 'o' ? PART_برآمد: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحیح
get_کام_options(typval_T *tv, jobopt_T *opt, صحیح supported, صحیح supported2)
{
    typval_T	*شے;
    char_u	*val;
    dict_T	*dict;
    صحیح		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    اگر (tv->v_type == VAR_UNKNOWN)
	واپس OK;
    اگر (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	واپس FAIL;
    }
    dict = tv->vval.v_dict;
    اگر (dict == تہی)
	واپس OK;

    todo = (صحیح)dict->dv_hashtab.ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(&dict->dv_hashtab, hi, todo)
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    شے = &dict_lookup(hi)->di_tv;

	    اگر (STRCMP(hi->hi_کلید, "mode") == 0)
	    {
		اگر (!(supported & JO_منوال))
		    توڑو;
		اگر (handle_منوال(شے, opt, &opt->jo_منوال, JO_منوال) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "in_mode") == 0)
	    {
		اگر (!(supported & JO_اندر_منوال))
		    توڑو;
		اگر (handle_منوال(شے, opt, &opt->jo_اندر_منوال, JO_اندر_منوال)
								      == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "out_mode") == 0)
	    {
		اگر (!(supported & JO_برآمد_منوال))
		    توڑو;
		اگر (handle_منوال(شے, opt, &opt->jo_برآمد_منوال, JO_برآمد_منوال)
								      == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "err_mode") == 0)
	    {
		اگر (!(supported & JO_ERR_منوال))
		    توڑو;
		اگر (handle_منوال(شے, opt, &opt->jo_err_منوال, JO_ERR_منوال)
								      == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "noblock") == 0)
	    {
		اگر (!(supported & JO_منوال))
		    توڑو;
		opt->jo_noblock = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "in_io") == 0
		    || STRCMP(hi->hi_کلید, "out_io") == 0
		    || STRCMP(hi->hi_کلید, "err_io") == 0)
	    {
		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		اگر (handle_io(شے, part_from_char(*hi->hi_کلید), opt) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "in_name") == 0
		    || STRCMP(hi->hi_کلید, "out_name") == 0
		    || STRCMP(hi->hi_کلید, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		opt->jo_سیٹ |= JO_برآمد_NAME << (part - PART_برآمد);
		opt->jo_io_name[part] = tv_get_string_buf_chk(شے,
						   opt->jo_io_name_buf[part]);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "pty") == 0)
	    {
		اگر (!(supported & JO_منوال))
		    توڑو;
		opt->jo_pty = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "in_buf") == 0
		    || STRCMP(hi->hi_کلید, "out_buf") == 0
		    || STRCMP(hi->hi_کلید, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		opt->jo_سیٹ |= JO_برآمد_BUF << (part - PART_برآمد);
		opt->jo_io_buf[part] = tv_get_نمبر(شے);
		اگر (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
					      hi->hi_کلید, tv_get_string(شے));
		    واپس FAIL;
		}
		اگر (buflist_findnr(opt->jo_io_buf[part]) == تہی)
		{
		    semsg(_(e_بفر_nr_does_not_exist),
						   (طویل)opt->jo_io_buf[part]);
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "out_modifiable") == 0
		    || STRCMP(hi->hi_کلید, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		opt->jo_سیٹ |= JO_برآمد_MODIFIABLE << (part - PART_برآمد);
		opt->jo_modifiable[part] = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "out_msg") == 0
		    || STRCMP(hi->hi_کلید, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		opt->jo_set2 |= JO2_برآمد_پیغام << (part - PART_برآمد);
		opt->jo_پیغام[part] = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "in_top") == 0
		    || STRCMP(hi->hi_کلید, "in_bot") == 0)
	    {
		linenr_T *lp;

		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		اگر (hi->hi_کلید[3] == 't')
		{
		    lp = &opt->jo_اندر_چوٹی;
		    opt->jo_سیٹ |= JO_اندر_چوٹی;
		}
		ورنہ
		{
		    lp = &opt->jo_اندر_bot;
		    opt->jo_سیٹ |= JO_اندر_BOT;
		}
		*lp = tv_get_نمبر(شے);
		اگر (*lp < 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
					      hi->hi_کلید, tv_get_string(شے));
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "channel") == 0)
	    {
		اگر (!(supported & JO_برآمد_IO))
		    توڑو;
		opt->jo_سیٹ |= JO_CHANNEL;
		اگر (شے->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "channel");
		    واپس FAIL;
		}
		opt->jo_channel = شے->vval.v_channel;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "callback") == 0)
	    {
		اگر (!(supported & JO_CALLBACK))
		    توڑو;
		opt->jo_سیٹ |= JO_CALLBACK;
		opt->jo_callback = get_callback(شے);
		اگر (opt->jo_callback.cb_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "callback");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "out_cb") == 0)
	    {
		اگر (!(supported & JO_برآمد_CALLBACK))
		    توڑو;
		opt->jo_سیٹ |= JO_برآمد_CALLBACK;
		opt->jo_برآمد_cb = get_callback(شے);
		اگر (opt->jo_برآمد_cb.cb_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "out_cb");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "err_cb") == 0)
	    {
		اگر (!(supported & JO_ERR_CALLBACK))
		    توڑو;
		opt->jo_سیٹ |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(شے);
		اگر (opt->jo_err_cb.cb_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "err_cb");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "close_cb") == 0)
	    {
		اگر (!(supported & JO_CLOSE_CALLBACK))
		    توڑو;
		opt->jo_سیٹ |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(شے);
		اگر (opt->jo_close_cb.cb_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "close_cb");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "drop") == 0)
	    {
		صحیح never = FALSE;
		val = tv_get_string(شے);

		اگر (STRCMP(val, "never") == 0)
		    never = TRUE;
		ورنہ اگر (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), "drop", val);
		    واپس FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "exit_cb") == 0)
	    {
		اگر (!(supported & JO_EXIT_CB))
		    توڑو;
		opt->jo_سیٹ |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(شے);
		اگر (opt->jo_exit_cb.cb_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "exit_cb");
		    واپس FAIL;
		}
	    }
#اگر_تعریف_شدہ FEAT_TERMINAL
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_name") == 0)
	    {
		اگر (!(supported2 & JO2_رکن_NAME))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_NAME;
		opt->jo_رکن_name = tv_get_string_buf_chk(شے,
						       opt->jo_رکن_name_buf);
		اگر (opt->jo_رکن_name == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_name");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_finish") == 0)
	    {
		اگر (!(supported2 & JO2_رکن_اختتام))
		    توڑو;
		val = tv_get_string(شے);
		اگر (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str),
							   "term_finish", val);
		    واپس FAIL;
		}
		opt->jo_set2 |= JO2_رکن_اختتام;
		opt->jo_رکن_اختتام = *val;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_opencmd") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_رکن_OPENCMD))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_OPENCMD;
		p = opt->jo_رکن_opencmd = tv_get_string_buf_chk(شے,
						    opt->jo_رکن_opencmd_buf);
		اگر (p != تہی)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    اگر (p != تہی && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != تہی))
			p = تہی;
		}
		اگر (p == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_opencmd");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "eof_chars") == 0)
	    {
		اگر (!(supported2 & JO2_EOF_CHARS))
		    توڑو;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(شے,
						       opt->jo_eof_chars_buf);
		اگر (opt->jo_eof_chars == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "eof_chars");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_rows") == 0)
	    {
		صحیح error = FALSE;

		اگر (!(supported2 & JO2_رکن_صفیں))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_صفیں;
		opt->jo_رکن_صفیں = tv_get_نمبر_chk(شے, &error);
		اگر (error)
		    واپس FAIL;
		اگر (opt->jo_رکن_صفیں < 0 || opt->jo_رکن_صفیں > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_rows");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_cols") == 0)
	    {
		صحیح error = FALSE;

		اگر (!(supported2 & JO2_رکن_ستون_تعداد))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_ستون_تعداد;
		opt->jo_رکن_ستون_تعداد = tv_get_نمبر_chk(شے, &error);
		اگر (error)
		    واپس FAIL;
		اگر (opt->jo_رکن_ستون_تعداد < 0 || opt->jo_رکن_ستون_تعداد > 1000)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_cols");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "vertical") == 0)
	    {
		اگر (!(supported2 & JO2_VERTICAL))
		    توڑو;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "curwin") == 0)
	    {
		اگر (!(supported2 & JO2_CURWIN))
		    توڑو;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "bufnr") == 0)
	    {
		صحیح nr;

		اگر (!(supported2 & JO2_CURWIN))
		    توڑو;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_نمبر(شے);
		اگر (nr <= 0)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), hi->hi_کلید, tv_get_string(شے));
		    واپس FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		اگر (opt->jo_bufnr_buf == تہی)
		{
		    semsg(_(e_بفر_nr_does_not_exist), (طویل)nr);
		    واپس FAIL;
		}
		اگر (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_رکن == تہی)
		{
		    semsg(_(e_invalid_دلیل_str), "bufnr");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "hidden") == 0)
	    {
		اگر (!(supported2 & JO2_HIDDEN))
		    توڑو;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "norestore") == 0)
	    {
		اگر (!(supported2 & JO2_NORESTORE))
		    توڑو;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_رکن_norestore = tv_get_bool(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_kill") == 0)
	    {
		اگر (!(supported2 & JO2_رکن_KILL))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_KILL;
		opt->jo_رکن_kill = tv_get_string_buf_chk(شے,
						       opt->jo_رکن_kill_buf);
		اگر (opt->jo_رکن_kill == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_kill");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "tty_type") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_TTY_TYPE))
		    توڑو;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(شے);
		اگر (p == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "tty_type");
		    واپس FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		اگر (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "tty_type");
		    واپس FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ورنہ اگر (STRCMP(hi->hi_کلید, "ansi_colors") == 0)
	    {
		صحیح		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		اگر (!(supported2 & JO2_ANSI_COLORS))
		    توڑو;

		اگر (شے == تہی || شے->v_type != VAR_LIST
			|| شے->vval.v_list == تہی
			|| شے->vval.v_list->lv_پہلا == &range_list_شے)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "ansi_colors");
		    واپس FAIL;
		}

		li = شے->vval.v_list->lv_پہلا;
		برائے (; li != تہی && n < 16; li = li->li_اگلا, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    صحیح		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    اگر (color_name == تہی)
			واپس FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    اگر (guicolor == INVALCOLOR)
		    {
			اگر (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			واپس FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		اگر (n != 16 || li != تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "ansi_colors");
		    واپس FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		حافظہ_نقل(opt->jo_ansi_colors, rgb, حجم(rgb));
	    }
# endif
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_highlight") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_رکن_HIGHLIGHT))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_HIGHLIGHT;
		p = tv_get_string_buf_chk(شے, opt->jo_رکن_highlight_buf);
		اگر (p == تہی || *p == NUL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_highlight");
		    واپس FAIL;
		}
		opt->jo_رکن_highlight = p;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "term_api") == 0)
	    {
		اگر (!(supported2 & JO2_رکن_API))
		    توڑو;
		opt->jo_set2 |= JO2_رکن_API;
		opt->jo_رکن_api = tv_get_string_buf_chk(شے,
							opt->jo_رکن_api_buf);
		اگر (opt->jo_رکن_api == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "term_api");
		    واپس FAIL;
		}
	    }
#اگر_ختم
	    ورنہ اگر (STRCMP(hi->hi_کلید, "env") == 0)
	    {
		اگر (!(supported2 & JO2_ENV))
		    توڑو;
		اگر (شے->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "env");
		    واپس FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = شے->vval.v_dict;
		اگر (opt->jo_env != تہی)
		    ++opt->jo_env->dv_refcount;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "cwd") == 0)
	    {
		اگر (!(supported2 & JO2_CWD))
		    توڑو;
		opt->jo_cwd = tv_get_string_buf_chk(شے, opt->jo_cwd_buf);
		اگر (opt->jo_cwd == تہی || !mch_isdir(opt->jo_cwd)
#اگر_غیر_تعریف_شدہ MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((حرف *)opt->jo_cwd, X_OK) != 0
#اگر_ختم
				)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "cwd");
		    واپس FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "waittime") == 0)
	    {
		اگر (!(supported & JO_WAITTIME))
		    توڑو;
		opt->jo_سیٹ |= JO_WAITTIME;
		opt->jo_waittime = tv_get_نمبر(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "timeout") == 0)
	    {
		اگر (!(supported & JO_TIMEOUT))
		    توڑو;
		opt->jo_سیٹ |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_نمبر(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "out_timeout") == 0)
	    {
		اگر (!(supported & JO_برآمد_TIMEOUT))
		    توڑو;
		opt->jo_سیٹ |= JO_برآمد_TIMEOUT;
		opt->jo_برآمد_timeout = tv_get_نمبر(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "err_timeout") == 0)
	    {
		اگر (!(supported & JO_ERR_TIMEOUT))
		    توڑو;
		opt->jo_سیٹ |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_نمبر(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "part") == 0)
	    {
		اگر (!(supported & JO_PART))
		    توڑو;
		opt->jo_سیٹ |= JO_PART;
		val = tv_get_string(شے);
		اگر (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ورنہ اگر (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_برآمد;
		ورنہ
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str_str), "part", val);
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "id") == 0)
	    {
		اگر (!(supported & JO_ID))
		    توڑو;
		opt->jo_سیٹ |= JO_ID;
		opt->jo_id = tv_get_نمبر(شے);
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "stoponexit") == 0)
	    {
		اگر (!(supported & JO_STOPONEXIT))
		    توڑو;
		opt->jo_سیٹ |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(شے,
						      opt->jo_stoponexit_buf);
		اگر (opt->jo_stoponexit == تہی)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "stoponexit");
		    واپس FAIL;
		}
	    }
	    ورنہ اگر (STRCMP(hi->hi_کلید, "block_write") == 0)
	    {
		اگر (!(supported & JO_بلاک_WRITE))
		    توڑو;
		opt->jo_سیٹ |= JO_بلاک_WRITE;
		opt->jo_بلاک_write = tv_get_نمبر(شے);
	    }
	    ورنہ
		توڑو;
	    --todo;
	}
    اگر (todo > 0)
    {
	semsg(_(e_invalid_دلیل_str), hi->hi_کلید);
	واپس FAIL;
    }

    واپس OK;
}

ساکن کام_T *پہلا_کام = تہی;

    ساکن خالی
کام_free_contents(کام_T *کام)
{
    صحیح		i;

    ch_log(کام->jv_channel, "Freeing job");
    اگر (کام->jv_channel != تہی)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	کام->jv_channel->ch_کام = تہی;
	channel_unref(کام->jv_channel);
    }
    mch_clear_کام(کام);

    vim_free(کام->jv_tty_اندر);
    vim_free(کام->jv_tty_برآمد);
    vim_free(کام->jv_stoponexit);
#اگر_تعریف_شدہ UNIX
    vim_free(کام->jv_termsig);
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    vim_free(کام->jv_tty_type);
#اگر_ختم
    free_callback(&کام->jv_exit_cb);
    اگر (کام->jv_argv != تہی)
    {
	برائے (i = 0; کام->jv_argv[i] != تہی; i++)
	    vim_free(کام->jv_argv[i]);
	vim_free(کام->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ساکن خالی
کام_unlink(کام_T *کام)
{
    اگر (کام->jv_اگلا != تہی)
	کام->jv_اگلا->jv_پچھلا = کام->jv_پچھلا;
    اگر (کام->jv_پچھلا == تہی)
	پہلا_کام = کام->jv_اگلا;
    ورنہ
	کام->jv_پچھلا->jv_اگلا = کام->jv_اگلا;
}

    ساکن خالی
کام_free_کام(کام_T *کام)
{
    کام_unlink(کام);
    vim_free(کام);
}

    ساکن خالی
کام_free(کام_T *کام)
{
    اگر (اندر_free_unref_اشیاء)
	واپس;

    کام_free_contents(کام);
    کام_free_کام(کام);
}

ساکن کام_T *jobs_مقصد_free = تہی;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ساکن خالی
کام_free_later(کام_T *کام)
{
    کام_unlink(کام);
    کام->jv_اگلا = jobs_مقصد_free;
    jobs_مقصد_free = کام;
}

    ساکن خالی
free_jobs_مقصد_free_later(خالی)
{
    کام_T *کام;

    جب (jobs_مقصد_free != تہی)
    {
	کام = jobs_مقصد_free;
	jobs_مقصد_free = کام->jv_اگلا;
	کام_free_contents(کام);
	vim_free(کام);
    }
}

#اگر defined(EXITFREE)
    خالی
کام_free_all(خالی)
{
    جب (پہلا_کام != تہی)
	کام_free(پہلا_کام);
    free_jobs_مقصد_free_later();

# ifdef FEAT_TERMINAL
    free_غیر_مستعمل_terminals();
# endif
}
#اگر_ختم

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ساکن صحیح
کام_need_انتہا_check(کام_T *کام)
{
    واپس کام->jv_حالت == کام_STARTED
	    && (کام->jv_stoponexit != تہی || کام->jv_exit_cb.cb_name != تہی);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ساکن صحیح
کام_channel_still_useful(کام_T *کام)
{
    واپس کام->jv_channel != تہی && channel_still_useful(کام->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ساکن صحیح
کام_channel_can_close(کام_T *کام)
{
    واپس کام->jv_channel != تہی && channel_can_close(کام->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ساکن صحیح
کام_still_useful(کام_T *کام)
{
    واپس کام_need_انتہا_check(کام) || کام_channel_still_useful(کام);
}

#اگر defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحیح
کام_any_running(خالی)
{
    کام_T	*کام;

    FOR_ALL_JOBS(کام)
	اگر (کام_still_useful(کام))
	{
	    ch_log(تہی, "GUI not forking because a job is running");
	    واپس TRUE;
	}
    واپس FALSE;
}
#اگر_ختم

// Unix uses argv[] for the command, other systems use a string.
#اگر defined(UNIX)
# define USE_ARGV
#اگر_ختم

#اگر !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ساکن char_u *
win32_escape_arg(char_u *arg)
{
    صحیح		slen, dlen;
    صحیح		escaping = 0;
    صحیح		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    صحیح		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (صحیح)STRLEN(arg);
    dlen = slen;
    برائے (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	اگر (*s == '"' || *s == '\\')
	    ++dlen;
	اگر (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    اگر (has_spaces)
	dlen += 2;

    اگر (dlen == slen)
	واپس vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    اگر (escaped_arg == تہی)
	واپس تہی;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    اگر (has_spaces)
	*d++ = '"';

    برائے (s = arg; *s != NUL;)
    {
	انتخاب (*s)
	{
	    حالت '"':
		برائے (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		توڑو;
	    حالت '\\':
		escaping++;
		*d++ = *s++;
		توڑو;
	    طےشدہ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		توڑو;
	}
    }

    // add terminating quote and finish with a NUL
    اگر (has_spaces)
    {
	برائے (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    واپس escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    صحیح
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	اگر (s == تہی)
	    واپس FAIL;
	s = win32_escape_arg(s);
	اگر (s == تہی)
	    واپس FAIL;
	ga_concat(gap, s);
	vim_free(s);
	اگر (li->li_اگلا != تہی)
	    ga_append(gap, ' ');
    }
    واپس OK;
}
#اگر_ختم

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    خالی
کام_cleanup(کام_T *کام)
{
    اگر (کام->jv_حالت != کام_ENDED)
	واپس;

    // Ready to cleanup the job.
    کام->jv_حالت = کام_ختم_شدہ;

    // When only channel-in is kept open, close explicitly.
    اگر (کام->jv_channel != تہی)
	ch_close_part(کام->jv_channel, PART_اندر);

    اگر (کام->jv_exit_cb.cb_name != تہی)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(کام->jv_channel, "Invoking exit callback %s",
						      کام->jv_exit_cb.cb_name);
	++کام->jv_refcount;
	argv[0].v_type = VAR_کام;
	argv[0].vval.v_کام = کام;
	argv[1].v_type = VAR_نمبر;
	argv[1].vval.v_نمبر = کام->jv_exitval;
	call_callback(&کام->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--کام->jv_refcount;
	channel_need_redraw = TRUE;
    }

    اگر (کام->jv_channel != تہی && کام->jv_channel->ch_anonymous_pipe)
	کام->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    اگر (کام->jv_refcount == 0 && !کام_channel_still_useful(کام))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	کام_free_later(کام);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحیح
سیٹ_ref_اندر_کام(صحیح copyID)
{
    صحیح		abort = FALSE;
    کام_T	*کام;
    typval_T	tv;

    برائے (کام = پہلا_کام; !abort && کام != تہی; کام = کام->jv_اگلا)
	اگر (کام_still_useful(کام))
	{
	    tv.v_type = VAR_کام;
	    tv.vval.v_کام = کام;
	    abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
	}
    واپس abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    خالی
کام_unref(کام_T *کام)
{
    اگر (کام == تہی || --کام->jv_refcount > 0)
	واپس;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    اگر (کام_channel_still_useful(کام))
	واپس;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    اگر (!کام_need_انتہا_check(کام))
    {
	کام_free(کام);
    }
    ورنہ اگر (کام->jv_channel != تہی)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(کام->jv_channel, "detaching channel from job");
	کام->jv_channel->ch_کام = تہی;
	channel_unref(کام->jv_channel);
	کام->jv_channel = تہی;
    }
}

    صحیح
free_غیر_مستعمل_jobs_contents(صحیح copyID, صحیح نقاب)
{
    صحیح		did_free = FALSE;
    کام_T	*کام;

    FOR_ALL_JOBS(کام)
	اگر ((کام->jv_copyID & نقاب) != (copyID & نقاب)
						    && !کام_still_useful(کام))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    کام_free_contents(کام);
	    did_free = TRUE;
	}
    واپس did_free;
}

    خالی
free_غیر_مستعمل_jobs(صحیح copyID, صحیح نقاب)
{
    کام_T	*کام;
    کام_T	*کام_اگلا;

    برائے (کام = پہلا_کام; کام != تہی; کام = کام_اگلا)
    {
	کام_اگلا = کام->jv_اگلا;
	اگر ((کام->jv_copyID & نقاب) != (copyID & نقاب)
						    && !کام_still_useful(کام))
	{
	    // Free the job struct itself.
	    کام_free_کام(کام);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    کام_T *
کام_alloc(خالی)
{
    کام_T *کام;

    کام = ALLOC_CLEAR_ONE(کام_T);
    اگر (کام == تہی)
	واپس تہی;

    کام->jv_refcount = 1;
    کام->jv_stoponexit = vim_strsave((char_u *)"term");

    اگر (پہلا_کام != تہی)
    {
	پہلا_کام->jv_پچھلا = کام;
	کام->jv_اگلا = پہلا_کام;
    }
    پہلا_کام = کام;
    واپس کام;
}

    خالی
کام_سیٹ_options(کام_T *کام, jobopt_T *opt)
{
    اگر (opt->jo_سیٹ & JO_STOPONEXIT)
    {
	vim_free(کام->jv_stoponexit);
	اگر (opt->jo_stoponexit == تہی || *opt->jo_stoponexit == NUL)
	    کام->jv_stoponexit = تہی;
	ورنہ
	    کام->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    اگر (opt->jo_سیٹ & JO_EXIT_CB)
    {
	free_callback(&کام->jv_exit_cb);
	اگر (opt->jo_exit_cb.cb_name == تہی || *opt->jo_exit_cb.cb_name == NUL)
	{
	    کام->jv_exit_cb.cb_name = تہی;
	    کام->jv_exit_cb.cb_partial = تہی;
	}
	ورنہ
	    copy_callback(&کام->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    خالی
کام_stop_on_exit(خالی)
{
    کام_T	*کام;

    FOR_ALL_JOBS(کام)
	اگر (کام->jv_حالت == کام_STARTED && کام->jv_stoponexit != تہی)
	    mch_signal_کام(کام, کام->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحیح
has_pending_کام(خالی)
{
    کام_T	    *کام;

    FOR_ALL_JOBS(کام)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	اگر ((کام->jv_حالت == کام_STARTED && !کام_channel_still_useful(کام))
		    || (کام->jv_حالت == کام_ختم_شدہ
					      && کام_channel_can_close(کام)))
	    واپس TRUE;
    واپس FALSE;
}

#تعریف_کرو MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحیح
کام_check_ended(خالی)
{
    صحیح		i;
    صحیح		did_انتہا = FALSE;

    // be quick if there are no jobs to check
    اگر (پہلا_کام == تہی)
	واپس did_انتہا;

    برائے (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	کام_T	*کام = mch_detect_ended_کام(پہلا_کام);

	اگر (کام == تہی)
	    توڑو;
	did_انتہا = TRUE;
	کام_cleanup(کام); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_مقصد_free_later();

    اگر (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    واپس did_انتہا;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    کام_T *
کام_آغاز(
	typval_T    *argvars,
	حرف	    **argv_arg غیر_مستعمل,
	jobopt_T    *opt_arg,
	کام_T	    **رکن_کام)
{
    کام_T	*کام;
    char_u	*cmd = تہی;
    حرف	**argv = تہی;
    صحیح		argc = 0;
    صحیح		i;
#اگر_غیر_تعریف_شدہ USE_ARGV
    garray_T	ga;
#اگر_ختم
    jobopt_T	opt;
    ch_part_T	part;

    کام = کام_alloc();
    اگر (کام == تہی)
	واپس تہی;

    کام->jv_حالت = کام_FAILED;
#اگر_غیر_تعریف_شدہ USE_ARGV
    ga_init2(&ga, حجم(حرف*), 20);
#اگر_ختم

    اگر (opt_arg != تہی)
	opt = *opt_arg;
    ورنہ
    {
	// Default mode is NL.
	clear_کام_options(&opt);
	opt.jo_منوال = CH_منوال_NL;
	اگر (get_کام_options(&argvars[1], &opt,
		    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_برآمد_IO + JO_بلاک_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    جاؤ theend;
    }

    // Check that when io is "file" that there is a file name.
    برائے (part = PART_برآمد; part < PART_گنتی; ++part)
	اگر ((opt.jo_سیٹ & (JO_برآمد_IO << (part - PART_برآمد)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_سیٹ & (JO_برآمد_NAME << (part - PART_برآمد)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_مقصد_be_سیٹ));
	    جاؤ theend;
	}

    اگر ((opt.jo_سیٹ & JO_اندر_IO) && opt.jo_io[PART_اندر] == JIO_بفر)
    {
	buf_T *buf = تہی;

	// check that we can find the buffer before starting the job
	اگر (opt.jo_سیٹ & JO_اندر_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_اندر]);
	    اگر (buf == تہی)
		semsg(_(e_بفر_nr_does_not_exist),
						 (طویل)opt.jo_io_buf[PART_اندر]);
	}
	ورنہ اگر (!(opt.jo_سیٹ & JO_اندر_NAME))
	{
	    emsg(_(e_اندر_io_بفر_requires_اندر_buf_or_اندر_name_مقصد_be_سیٹ));
	}
	ورنہ
	    buf = buflist_ڈھونڈو_by_name(opt.jo_io_name[PART_اندر], FALSE);
	اگر (buf == تہی)
	    جاؤ theend;
	اگر (buf->b_ml.ml_mfp == تہی)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    اگر (opt.jo_سیٹ & JO_اندر_BUF)
	    {
		sprintf((حرف *)numbuf, "%d", opt.jo_io_buf[PART_اندر]);
		s = numbuf;
	    }
	    ورنہ
		s = opt.jo_io_name[PART_اندر];
	    semsg(_(e_بفر_must_be_loaded_str), s);
	    جاؤ theend;
	}
	کام->jv_اندر_buf = buf;
    }

    کام_سیٹ_options(کام, &opt);

#اگر_تعریف_شدہ USE_ARGV
    اگر (argv_arg != تہی)
    {
	// Make a copy of argv_arg for job->jv_argv.
	برائے (i = 0; argv_arg[i] != تہی; i++)
	    argc++;
	argv = ALLOC_MULT(حرف *, argc + 1);
	اگر (argv == تہی)
	    جاؤ theend;
	برائے (i = 0; i < argc; i++)
	    argv[i] = (حرف *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = تہی;
    }
    ورنہ
#اگر_ختم
    اگر (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	اگر (cmd == تہی || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    جاؤ theend;
	}

	اگر (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    جاؤ theend;
    }
    ورنہ اگر (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == تہی
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ theend;
    }
    ورنہ
    {
	list_T *l = argvars[0].vval.v_list;

	اگر (build_argv_from_list(l, &argv, &argc) == FAIL)
	    جاؤ theend;

	// Empty command is invalid.
	اگر (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    جاؤ theend;
	}
#اگر_غیر_تعریف_شدہ USE_ARGV
	اگر (win32_build_cmd(l, &ga) == FAIL)
	    جاؤ theend;
	cmd = ga.ga_ڈیٹا;
	اگر (cmd == تہی || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    جاؤ theend;
	}
#اگر_ختم
    }

    // Save the command used to start the job.
    کام->jv_argv = argv;

    اگر (رکن_کام != تہی)
	*رکن_کام = کام;

#اگر_تعریف_شدہ USE_ARGV
    اگر (ch_log_فعال())
    {
	garray_T    ga;

	ga_init2(&ga, حجم(حرف), 200);
	برائے (i = 0; i < argc; ++i)
	{
	    اگر (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(تہی, "Starting job: %s", (حرف *)ga.ga_ڈیٹا);
	ga_clear(&ga);
    }
    mch_کام_آغاز(argv, کام, &opt, رکن_کام != تہی);
#ورنہ
    ch_log(تہی, "Starting job: %s", (حرف *)cmd);
    mch_کام_آغاز((حرف *)cmd, کام, &opt);
#اگر_ختم

    // If the channel is reading from a buffer, write lines now.
    اگر (کام->jv_channel != تہی)
	channel_write_اندر(کام->jv_channel);

theend:
#اگر_غیر_تعریف_شدہ USE_ARGV
    vim_free(ga.ga_ڈیٹا);
#اگر_ختم
    اگر (argv != تہی && argv != کام->jv_argv)
    {
	برائے (i = 0; argv[i] != تہی; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_کام_options(&opt);
    واپس کام;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    حرف *
کام_حالت(کام_T *کام)
{
    حرف	*نتیجہ;

    اگر (کام->jv_حالت >= کام_ENDED)
	// No need to check, dead is dead.
	نتیجہ = "dead";
    ورنہ اگر (کام->jv_حالت == کام_FAILED)
	نتیجہ = "fail";
    ورنہ
    {
	نتیجہ = mch_کام_حالت(کام);
	اگر (کام->jv_حالت == کام_ENDED)
	    کام_cleanup(کام);
    }
    واپس نتیجہ;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحیح
کام_stop(کام_T *کام, typval_T *argvars, حرف *type)
{
    char_u *arg;

    اگر (type != تہی)
	arg = (char_u *)type;
    ورنہ اگر (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ورنہ
    {
	arg = tv_get_string_chk(&argvars[1]);
	اگر (arg == تہی)
	{
	    emsg(_(e_invalid_دلیل));
	    واپس 0;
	}
    }
    اگر (کام->jv_حالت == کام_FAILED)
    {
	ch_log(کام->jv_channel, "Job failed to start, job_stop() skipped");
	واپس 0;
    }
    اگر (کام->jv_حالت == کام_ENDED)
    {
	ch_log(کام->jv_channel, "Job has already ended, job_stop() skipped");
	واپس 0;
    }
    ch_log(کام->jv_channel, "Stopping job with '%s'", (حرف *)arg);
    اگر (mch_signal_کام(کام, arg) == FAIL)
	واپس 0;

    // Assume that only "kill" will kill the job.
    اگر (کام->jv_channel != تہی && STRCMP(arg, "kill") == 0)
	کام->jv_channel->ch_کام_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    واپس 1;
}

    خالی
invoke_prompt_callback(خالی)
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
    curwin->w_cursor.ستون = 0;

    اگر (curbuf->b_prompt_callback.cb_name == تہی
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	واپس;
    متن = ml_get(lnum);
    prompt = prompt_متن();
    اگر (STRLEN(متن) >= STRLEN(prompt))
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
    صحیح
invoke_prompt_مداخلت(خالی)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحیح		ret;

    اگر (curbuf->b_prompt_مداخلت.cb_name == تہی
	    || *curbuf->b_prompt_مداخلت.cb_name == NUL)
	واپس FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_مداخلت, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    واپس ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ساکن char_u *
buf_prompt_متن(buf_T* buf)
{
    اگر (buf->b_prompt_متن == تہی)
	واپس (char_u *)"% ";
    واپس buf->b_prompt_متن;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_متن(خالی)
{
    واپس buf_prompt_متن(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    خالی
init_prompt(صحیح cmdchar_todo)
{
    char_u *prompt = prompt_متن();
    char_u *متن;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    متن = ml_get_curline();
    اگر (STRNCMP(متن, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	اگر (*متن == NUL)
	    ml_replace(curbuf->b_ml.ml_سطر_گنتی, prompt, TRUE);
	ورنہ
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	coladvance((colnr_T)MAXCOL);
	بدلا_بائٹیں(curbuf->b_ml.ml_سطر_گنتی, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    اگر (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.ستون != (صحیح)STRLEN(prompt))
	سیٹ_insstart(curwin->w_cursor.lnum, (صحیح)STRLEN(prompt));

    اگر (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    اگر (curwin->w_cursor.ستون < (صحیح)STRLEN(prompt))
	curwin->w_cursor.ستون = (صحیح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحیح
prompt_curpos_editable(خالی)
{
    واپس curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_گنتی
	&& curwin->w_cursor.ستون >= (صحیح)STRLEN(prompt_متن());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    خالی
f_prompt_setcallback(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    callback_T	callback;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	واپس;

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == تہی)
	واپس;

    free_callback(&buf->b_prompt_callback);
    سیٹ_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    خالی
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    callback_T	callback;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	واپس;

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == تہی)
	واپس;

    free_callback(&buf->b_prompt_مداخلت);
    سیٹ_callback(&buf->b_prompt_مداخلت, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    خالی
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = tv_get_buf_from_arg(&argvars[0]);
    اگر (buf == تہی)
	واپس;

    اگر (!bt_prompt(buf))
	واپس;

    rettv->vval.v_string = vim_strsave(buf_prompt_متن(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    خالی
f_prompt_setprompt(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    char_u	*متن;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    اگر (check_secure())
	واپس;
    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	واپس;

    متن = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_متن);
    buf->b_prompt_متن = vim_strsave(متن);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ساکن کام_T *
get_کام_arg(typval_T *tv)
{
    کام_T *کام;

    اگر (tv->v_type != VAR_کام)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(tv));
	واپس تہی;
    }
    کام = tv->vval.v_کام;

    اگر (کام == تہی)
	emsg(_(e_not_valid_کام));
    واپس کام;
}

/*
 * "job_getchannel()" function
 */
    خالی
f_کام_getchannel(typval_T *argvars, typval_T *rettv)
{
    کام_T	*کام;

    اگر (اندر_vim9script() && check_for_کام_arg(argvars, 0) == FAIL)
	واپس;

    کام = get_کام_arg(&argvars[0]);
    اگر (کام == تہی)
	واپس;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = کام->jv_channel;
    اگر (کام->jv_channel != تہی)
	++کام->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ساکن خالی
کام_info(کام_T *کام, dict_T *dict)
{
    dictitem_T	*شے;
    varnumber_T	nr;
    list_T	*l;
    صحیح		i;

    dict_جوڑو_string(dict, "status", (char_u *)کام_حالت(کام));

    شے = dictitem_alloc((char_u *)"channel");
    اگر (شے == تہی)
	واپس;
    شے->di_tv.v_type = VAR_CHANNEL;
    شے->di_tv.vval.v_channel = کام->jv_channel;
    اگر (کام->jv_channel != تہی)
	++کام->jv_channel->ch_refcount;
    اگر (dict_جوڑو(dict, شے) == FAIL)
	dictitem_free(شے);

#اگر_تعریف_شدہ UNIX
    nr = کام->jv_pid;
#ورنہ
    nr = کام->jv_proc_info.dwProcessId;
#اگر_ختم
    dict_جوڑو_نمبر(dict, "process", nr);
    dict_جوڑو_string(dict, "tty_in", کام->jv_tty_اندر);
    dict_جوڑو_string(dict, "tty_out", کام->jv_tty_برآمد);

    dict_جوڑو_نمبر(dict, "exitval", کام->jv_exitval);
    dict_جوڑو_string(dict, "exit_cb", کام->jv_exit_cb.cb_name);
    dict_جوڑو_string(dict, "stoponexit", کام->jv_stoponexit);
#اگر_تعریف_شدہ UNIX
    dict_جوڑو_string(dict, "termsig", کام->jv_termsig);
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    dict_جوڑو_string(dict, "tty_type", کام->jv_tty_type);
#اگر_ختم

    l = list_alloc();
    اگر (l == تہی)
	واپس;

    اگر (dict_جوڑو_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	واپس;
    }
    اگر (کام->jv_argv != تہی)
	برائے (i = 0; کام->jv_argv[i] != تہی; i++)
	    list_append_string(l, (char_u *)کام->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ساکن خالی
کام_info_all(list_T *l)
{
    کام_T	*کام;
    typval_T	tv;

    FOR_ALL_JOBS(کام)
    {
	tv.v_type = VAR_کام;
	tv.vval.v_کام = کام;

	اگر (list_append_tv(l, &tv) != OK)
	    واپس;
    }
}

/*
 * "job_info()" function
 */
    خالی
f_کام_info(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_opt_کام_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	کام_T	*کام;

	کام = get_کام_arg(&argvars[0]);
	اگر (کام != تہی && rettv_dict_alloc(rettv) == OK)
	    کام_info(کام, rettv->vval.v_dict);
    }
    ورنہ اگر (rettv_list_alloc(rettv) == OK)
	کام_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    خالی
f_کام_setoptions(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    کام_T	*کام;
    jobopt_T	opt;

    اگر (اندر_vim9script()
	    && (check_for_کام_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	واپس;

    کام = get_کام_arg(&argvars[0]);
    اگر (کام == تہی)
	واپس;
    clear_کام_options(&opt);
    اگر (get_کام_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	کام_سیٹ_options(کام, &opt);
    free_کام_options(&opt);
}

/*
 * "job_start()" function
 */
    خالی
f_کام_آغاز(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_کام;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_کام = کام_آغاز(argvars, تہی, تہی, تہی);
}

/*
 * "job_status()" function
 */
    خالی
f_کام_حالت(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_کام_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_کام && argvars[0].vval.v_کام == تہی)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ورنہ
    {
	کام_T	*کام = get_کام_arg(&argvars[0]);

	اگر (کام != تہی)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)کام_حالت(کام));
	}
    }
}

/*
 * "job_stop()" function
 */
    خالی
f_کام_stop(typval_T *argvars, typval_T *rettv)
{
    کام_T	*کام;

    اگر (اندر_vim9script()
	    && (check_for_کام_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    کام = get_کام_arg(&argvars[0]);
    اگر (کام != تہی)
	rettv->vval.v_نمبر = کام_stop(کام, argvars, تہی);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
کام_مقصد_string_buf(typval_T *varp, char_u *buf)
{
    کام_T *کام = varp->vval.v_کام;
    حرف  *حالت_2;

    اگر (کام == تہی)
    {
	vim_snprintf((حرف *)buf, NUMBUFLEN, "no process");
	واپس buf;
    }
    حالت_2 = کام->jv_حالت == کام_FAILED ? "fail"
		    : کام->jv_حالت >= کام_ENDED ? "dead"
		    : "run";
#اگر_تعریف_شدہ UNIX
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s", (طویل)کام->jv_pid, حالت_2);
#وگرنہ_اگر defined(MSWIN)
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s",
		(طویل)کام->jv_proc_info.dwProcessId,
		حالت_2);
#ورنہ
    // fall-back
    vim_snprintf((حرف *)buf, NUMBUFLEN, "process ? %s", حالت_2);
#اگر_ختم
    واپس buf;
}

#اگر_ختم // FEAT_JOB_CHANNEL
