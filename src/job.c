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

#إذا defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    لكل ((job) = first_job; (job) != عدم; (job) = (job)->jv_next)

    ساكن صحيح
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, صحيح jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    إذا (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    وإلا إذا (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    وإلا إذا (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    وإلا إذا (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    وإلا إذا (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    وإلا إذا (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    وإلا إذا (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    وإلا
    {
	semsg(_(e_invalid_argument_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    إذا (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    وإلا إذا (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    وإلا إذا (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    وإلا إذا (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    وإلا إذا (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    وإلا
    {
	semsg(_(e_invalid_argument_str), val);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    فراغ
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    ساكن فراغ
unref_job_callback(callback_T *cb)
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
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    إذا (opt->jo_env != عدم)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ساكن صحيح
part_from_char(صحيح c)
{
    أرجع c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحيح
get_job_options(typval_T *tv, jobopt_T *opt, صحيح supported, صحيح supported2)
{
    typval_T	*item;
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

    todo = (صحيح)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_عناصر(&dict->dv_hashtab, hi, todo)
	إذا (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    إذا (STRCMP(hi->hi_مفتاح, "mode") == 0)
	    {
		إذا (!(supported & JO_MODE))
		    اكسر;
		إذا (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "in_mode") == 0)
	    {
		إذا (!(supported & JO_IN_MODE))
		    اكسر;
		إذا (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "out_mode") == 0)
	    {
		إذا (!(supported & JO_OUT_MODE))
		    اكسر;
		إذا (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "err_mode") == 0)
	    {
		إذا (!(supported & JO_ERR_MODE))
		    اكسر;
		إذا (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "noblock") == 0)
	    {
		إذا (!(supported & JO_MODE))
		    اكسر;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "in_io") == 0
		    || STRCMP(hi->hi_مفتاح, "out_io") == 0
		    || STRCMP(hi->hi_مفتاح, "err_io") == 0)
	    {
		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		إذا (handle_io(item, part_from_char(*hi->hi_مفتاح), opt) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "in_name") == 0
		    || STRCMP(hi->hi_مفتاح, "out_name") == 0
		    || STRCMP(hi->hi_مفتاح, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_مفتاح);

		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "pty") == 0)
	    {
		إذا (!(supported & JO_MODE))
		    اكسر;
		opt->jo_pty = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "in_buf") == 0
		    || STRCMP(hi->hi_مفتاح, "out_buf") == 0
		    || STRCMP(hi->hi_مفتاح, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_مفتاح);

		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_رقم(item);
		إذا (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str),
					      hi->hi_مفتاح, tv_get_string(item));
		    أرجع FAIL;
		}
		إذا (buflist_findnr(opt->jo_io_buf[part]) == عدم)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "out_modifiable") == 0
		    || STRCMP(hi->hi_مفتاح, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_مفتاح);

		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "out_msg") == 0
		    || STRCMP(hi->hi_مفتاح, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_مفتاح);

		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		opt->jo_set2 |= JO2_OUT_رسالة << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "in_top") == 0
		    || STRCMP(hi->hi_مفتاح, "in_bot") == 0)
	    {
		linenr_T *lp;

		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		إذا (hi->hi_مفتاح[3] == 't')
		{
		    lp = &opt->jo_in_قمة;
		    opt->jo_set |= JO_IN_قمة;
		}
		وإلا
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_رقم(item);
		إذا (*lp < 0)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str),
					      hi->hi_مفتاح, tv_get_string(item));
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "channel") == 0)
	    {
		إذا (!(supported & JO_OUT_IO))
		    اكسر;
		opt->jo_set |= JO_CHANNEL;
		إذا (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "channel");
		    أرجع FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "callback") == 0)
	    {
		إذا (!(supported & JO_CALLBACK))
		    اكسر;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		إذا (opt->jo_callback.cb_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "callback");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "out_cb") == 0)
	    {
		إذا (!(supported & JO_OUT_CALLBACK))
		    اكسر;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		إذا (opt->jo_out_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "out_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "err_cb") == 0)
	    {
		إذا (!(supported & JO_ERR_CALLBACK))
		    اكسر;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		إذا (opt->jo_err_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "err_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "close_cb") == 0)
	    {
		إذا (!(supported & JO_CLOSE_CALLBACK))
		    اكسر;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		إذا (opt->jo_close_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "close_cb");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "drop") == 0)
	    {
		صحيح never = FALSE;
		val = tv_get_string(item);

		إذا (STRCMP(val, "never") == 0)
		    never = TRUE;
		وإلا إذا (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str), "drop", val);
		    أرجع FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "exit_cb") == 0)
	    {
		إذا (!(supported & JO_EXIT_CB))
		    اكسر;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		إذا (opt->jo_exit_cb.cb_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "exit_cb");
		    أرجع FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_name") == 0)
	    {
		إذا (!(supported2 & JO2_TERM_NAME))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		إذا (opt->jo_term_name == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_name");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_finish") == 0)
	    {
		إذا (!(supported2 & JO2_TERM_FINISH))
		    اكسر;
		val = tv_get_string(item);
		إذا (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str),
							   "term_finish", val);
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_opencmd") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TERM_OPENCMD))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
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
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_opencmd");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "eof_chars") == 0)
	    {
		إذا (!(supported2 & JO2_EOF_CHARS))
		    اكسر;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		إذا (opt->jo_eof_chars == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "eof_chars");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_rows") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_TERM_ROWS))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_رقم_chk(item, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_rows");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_cols") == 0)
	    {
		صحيح error = FALSE;

		إذا (!(supported2 & JO2_TERM_COLS))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_رقم_chk(item, &error);
		إذا (error)
		    أرجع FAIL;
		إذا (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_cols");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "vertical") == 0)
	    {
		إذا (!(supported2 & JO2_VERTICAL))
		    اكسر;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "curwin") == 0)
	    {
		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "bufnr") == 0)
	    {
		صحيح nr;

		إذا (!(supported2 & JO2_CURWIN))
		    اكسر;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_رقم(item);
		إذا (nr <= 0)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str), hi->hi_مفتاح, tv_get_string(item));
		    أرجع FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		إذا (opt->jo_bufnr_buf == عدم)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    أرجع FAIL;
		}
		إذا (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == عدم)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "hidden") == 0)
	    {
		إذا (!(supported2 & JO2_HIDDEN))
		    اكسر;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "norestore") == 0)
	    {
		إذا (!(supported2 & JO2_NORESTORE))
		    اكسر;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_kill") == 0)
	    {
		إذا (!(supported2 & JO2_TERM_KILL))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		إذا (opt->jo_term_kill == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_kill");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "tty_type") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TTY_TYPE))
		    اكسر;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		إذا (p == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "tty_type");
		    أرجع FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		إذا (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "tty_type");
		    أرجع FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "ansi_colors") == 0)
	    {
		صحيح		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		إذا (!(supported2 & JO2_ANSI_COLORS))
		    اكسر;

		إذا (item == عدم || item->v_type != VAR_LIST
			|| item->vval.v_list == عدم
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "ansi_colors");
		    أرجع FAIL;
		}

		li = item->vval.v_list->lv_first;
		لكل (; li != عدم && n < 16; li = li->li_next, n++)
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
		    semsg(_(e_invalid_قيمة_for_argument_str), "ansi_colors");
		    أرجع FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, حجم(rgb));
	    }
# endif
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_highlight") == 0)
	    {
		char_u *p;

		إذا (!(supported2 & JO2_TERM_HIGHLIGHT))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		إذا (p == عدم || *p == NUL)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_highlight");
		    أرجع FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "term_api") == 0)
	    {
		إذا (!(supported2 & JO2_TERM_API))
		    اكسر;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		إذا (opt->jo_term_api == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "term_api");
		    أرجع FAIL;
		}
	    }
#endif
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "env") == 0)
	    {
		إذا (!(supported2 & JO2_ENV))
		    اكسر;
		إذا (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "env");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		إذا (opt->jo_env != عدم)
		    ++opt->jo_env->dv_refcount;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "cwd") == 0)
	    {
		إذا (!(supported2 & JO2_CWD))
		    اكسر;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		إذا (opt->jo_cwd == عدم || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((حرف *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "cwd");
		    أرجع FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "waittime") == 0)
	    {
		إذا (!(supported & JO_WAITTIME))
		    اكسر;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_رقم(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "timeout") == 0)
	    {
		إذا (!(supported & JO_TIMEOUT))
		    اكسر;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_رقم(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "out_timeout") == 0)
	    {
		إذا (!(supported & JO_OUT_TIMEOUT))
		    اكسر;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_رقم(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "err_timeout") == 0)
	    {
		إذا (!(supported & JO_ERR_TIMEOUT))
		    اكسر;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_رقم(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "part") == 0)
	    {
		إذا (!(supported & JO_PART))
		    اكسر;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		إذا (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		وإلا إذا (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		وإلا
		{
		    semsg(_(e_invalid_قيمة_for_argument_str_str), "part", val);
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "id") == 0)
	    {
		إذا (!(supported & JO_ID))
		    اكسر;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_رقم(item);
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "stoponexit") == 0)
	    {
		إذا (!(supported & JO_STOPONEXIT))
		    اكسر;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		إذا (opt->jo_stoponexit == عدم)
		{
		    semsg(_(e_invalid_قيمة_for_argument_str), "stoponexit");
		    أرجع FAIL;
		}
	    }
	    وإلا إذا (STRCMP(hi->hi_مفتاح, "block_write") == 0)
	    {
		إذا (!(supported & JO_BLOCK_WRITE))
		    اكسر;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_رقم(item);
	    }
	    وإلا
		اكسر;
	    --todo;
	}
    إذا (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_مفتاح);
	أرجع FAIL;
    }

    أرجع OK;
}

ساكن job_T *first_job = عدم;

    ساكن فراغ
job_free_contents(job_T *job)
{
    صحيح		i;

    ch_log(job->jv_channel, "Freeing job");
    إذا (job->jv_channel != عدم)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = عدم;
	channel_unref(job->jv_channel);
    }
    mch_clear_job(job);

    vim_free(job->jv_tty_in);
    vim_free(job->jv_tty_out);
    vim_free(job->jv_stoponexit);
#ifdef UNIX
    vim_free(job->jv_termsig);
#endif
#ifdef MSWIN
    vim_free(job->jv_tty_type);
#endif
    free_callback(&job->jv_exit_cb);
    إذا (job->jv_argv != عدم)
    {
	لكل (i = 0; job->jv_argv[i] != عدم; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ساكن فراغ
job_unlink(job_T *job)
{
    إذا (job->jv_next != عدم)
	job->jv_next->jv_prev = job->jv_prev;
    إذا (job->jv_prev == عدم)
	first_job = job->jv_next;
    وإلا
	job->jv_prev->jv_next = job->jv_next;
}

    ساكن فراغ
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    ساكن فراغ
job_free(job_T *job)
{
    إذا (in_free_unref_عناصر)
	أرجع;

    job_free_contents(job);
    job_free_job(job);
}

ساكن job_T *jobs_to_free = عدم;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ساكن فراغ
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    ساكن فراغ
free_jobs_to_free_later(فراغ)
{
    job_T *job;

    طالما (jobs_to_free != عدم)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#إذا defined(EXITFREE)
    فراغ
job_free_all(فراغ)
{
    طالما (first_job != عدم)
	job_free(first_job);
    free_jobs_to_free_later();

# ifdef FEAT_TERMINAL
    free_unused_terminals();
# endif
}
#endif

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ساكن صحيح
job_need_end_check(job_T *job)
{
    أرجع job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != عدم || job->jv_exit_cb.cb_name != عدم);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ساكن صحيح
job_channel_still_useful(job_T *job)
{
    أرجع job->jv_channel != عدم && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ساكن صحيح
job_channel_can_close(job_T *job)
{
    أرجع job->jv_channel != عدم && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ساكن صحيح
job_still_useful(job_T *job)
{
    أرجع job_need_end_check(job) || job_channel_still_useful(job);
}

#إذا defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحيح
job_any_running(فراغ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	إذا (job_still_useful(job))
	{
	    ch_log(عدم, "GUI not forking because a job is running");
	    أرجع TRUE;
	}
    أرجع FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#إذا defined(UNIX)
# define USE_ARGV
#endif

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
    FOR_ALL_LIST_عناصر(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	إذا (s == عدم)
	    أرجع FAIL;
	s = win32_escape_arg(s);
	إذا (s == عدم)
	    أرجع FAIL;
	ga_concat(gap, s);
	vim_free(s);
	إذا (li->li_next != عدم)
	    ga_append(gap, ' ');
    }
    أرجع OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    فراغ
job_cleanup(job_T *job)
{
    إذا (job->jv_status != JOB_ENDED)
	أرجع;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    إذا (job->jv_channel != عدم)
	ch_close_part(job->jv_channel, PART_IN);

    إذا (job->jv_exit_cb.cb_name != عدم)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_رقم;
	argv[1].vval.v_رقم = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    إذا (job->jv_channel != عدم && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    إذا (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحيح
set_ref_in_job(صحيح copyID)
{
    صحيح		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    لكل (job = first_job; !abort && job != عدم; job = job->jv_next)
	إذا (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    فراغ
job_unref(job_T *job)
{
    إذا (job == عدم || --job->jv_refcount > 0)
	أرجع;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    إذا (job_channel_still_useful(job))
	أرجع;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    إذا (!job_need_end_check(job))
    {
	job_free(job);
    }
    وإلا إذا (job->jv_channel != عدم)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = عدم;
	channel_unref(job->jv_channel);
	job->jv_channel = عدم;
    }
}

    صحيح
free_unused_jobs_contents(صحيح copyID, صحيح mask)
{
    صحيح		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	إذا ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    فراغ
free_unused_jobs(صحيح copyID, صحيح mask)
{
    job_T	*job;
    job_T	*job_next;

    لكل (job = first_job; job != عدم; job = job_next)
    {
	job_next = job->jv_next;
	إذا ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the job struct itself.
	    job_free_job(job);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    job_T *
job_alloc(فراغ)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    إذا (job == عدم)
	أرجع عدم;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    إذا (first_job != عدم)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    أرجع job;
}

    فراغ
job_set_options(job_T *job, jobopt_T *opt)
{
    إذا (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	إذا (opt->jo_stoponexit == عدم || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = عدم;
	وإلا
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    إذا (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	إذا (opt->jo_exit_cb.cb_name == عدم || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = عدم;
	    job->jv_exit_cb.cb_partial = عدم;
	}
	وإلا
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    فراغ
job_stop_on_exit(فراغ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	إذا (job->jv_status == JOB_STARTED && job->jv_stoponexit != عدم)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحيح
has_pending_job(فراغ)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	إذا ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    أرجع TRUE;
    أرجع FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحيح
job_check_ended(فراغ)
{
    صحيح		i;
    صحيح		did_end = FALSE;

    // be quick if there are no jobs to check
    إذا (first_job == عدم)
	أرجع did_end;

    لكل (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	إذا (job == عدم)
	    اكسر;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    إذا (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    أرجع did_end;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    job_T *
job_start(
	typval_T    *argvars,
	حرف	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = عدم;
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    إذا (job == عدم)
	أرجع عدم;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, حجم(حرف*), 20);
#endif

    إذا (opt_arg != عدم)
	opt = *opt_arg;
    وإلا
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	إذا (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    اذهب theend;
    }

    // Check that when io is "file" that there is a file name.
    لكل (part = PART_OUT; part < PART_عدد; ++part)
	إذا ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    اذهب theend;
	}

    إذا ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = عدم;

	// check that we can find the buffer before starting the job
	إذا (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    إذا (buf == عدم)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	وإلا إذا (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	وإلا
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	إذا (buf == عدم)
	    اذهب theend;
	إذا (buf->b_ml.ml_mfp == عدم)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    إذا (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((حرف *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    وإلا
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    اذهب theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
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
#endif
    إذا (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    اذهب theend;
	}

	إذا (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    اذهب theend;
    }
    وإلا إذا (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == عدم
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
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
	    emsg(_(e_invalid_argument));
	    اذهب theend;
	}
#ifndef USE_ARGV
	إذا (win32_build_cmd(l, &ga) == FAIL)
	    اذهب theend;
	cmd = ga.ga_data;
	إذا (cmd == عدم || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    اذهب theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    إذا (term_job != عدم)
	*term_job = job;

#ifdef USE_ARGV
    إذا (ch_log_active())
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
	ch_log(عدم, "Starting job: %s", (حرف *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != عدم);
#وإلا
    ch_log(عدم, "Starting job: %s", (حرف *)cmd);
    mch_job_start((حرف *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    إذا (job->jv_channel != عدم)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    إذا (argv != عدم && argv != job->jv_argv)
    {
	لكل (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    أرجع job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    حرف *
job_status(job_T *job)
{
    حرف	*result;

    إذا (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    وإلا إذا (job->jv_status == JOB_FAILED)
	result = "fail";
    وإلا
    {
	result = mch_job_status(job);
	إذا (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    أرجع result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحيح
job_stop(job_T *job, typval_T *argvars, حرف *type)
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
	    emsg(_(e_invalid_argument));
	    أرجع 0;
	}
    }
    إذا (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	أرجع 0;
    }
    إذا (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	أرجع 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (حرف *)arg);
    إذا (mch_signal_job(job, arg) == FAIL)
	أرجع 0;

    // Assume that only "kill" will kill the job.
    إذا (job->jv_channel != عدم && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    أرجع 1;
}

    فراغ
invoke_prompt_callback(فراغ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_سطر_عدد;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    إذا (curbuf->b_prompt_callback.cb_name == عدم
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	أرجع;
    text = ml_get(lnum);
    prompt = prompt_text();
    إذا (STRLEN(text) >= STRLEN(prompt))
	text += STRLEN(prompt);
    argv[0].v_type = VAR_STRING;
    argv[0].vval.v_string = vim_strsave(text);
    argv[1].v_type = VAR_UNKNOWN;

    call_callback(&curbuf->b_prompt_callback, -1, &rettv, 1, argv);
    clear_tv(&argv[0]);
    clear_tv(&rettv);
}

/*
 * Return TRUE when the interrupt callback was invoked.
 */
    صحيح
invoke_prompt_interrupt(فراغ)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحيح		ret;

    إذا (curbuf->b_prompt_interrupt.cb_name == عدم
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	أرجع FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    أرجع ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ساكن char_u *
buf_prompt_text(buf_T* buf)
{
    إذا (buf->b_prompt_text == عدم)
	أرجع (char_u *)"% ";
    أرجع buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(فراغ)
{
    أرجع buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    فراغ
init_prompt(صحيح cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_عدد;
    text = ml_get_curline();
    إذا (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	إذا (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_سطر_عدد, prompt, TRUE);
	وإلا
	    ml_append(curbuf->b_ml.ml_سطر_عدد, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_عدد;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_سطر_عدد, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    إذا (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (صحيح)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (صحيح)STRLEN(prompt));

    إذا (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    إذا (curwin->w_cursor.col < (صحيح)STRLEN(prompt))
	curwin->w_cursor.col = (صحيح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحيح
prompt_curpos_editable(فراغ)
{
    أرجع curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_عدد
	&& curwin->w_cursor.col >= (صحيح)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    فراغ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    فراغ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    إذا (check_secure())
	أرجع;

    إذا (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
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

    إذا (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[0]);
    إذا (buf == عدم)
	أرجع;

    إذا (!bt_prompt(buf))
	أرجع;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    فراغ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    إذا (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_secure())
	أرجع;
    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ساكن job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    إذا (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	أرجع عدم;
    }
    job = tv->vval.v_job;

    إذا (job == عدم)
	emsg(_(e_not_valid_job));
    أرجع job;
}

/*
 * "job_getchannel()" function
 */
    فراغ
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    إذا (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	أرجع;

    job = get_job_arg(&argvars[0]);
    إذا (job == عدم)
	أرجع;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    إذا (job->jv_channel != عدم)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ساكن فراغ
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    صحيح		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    إذا (item == عدم)
	أرجع;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    إذا (job->jv_channel != عدم)
	++job->jv_channel->ch_refcount;
    إذا (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#وإلا
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_رقم(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_رقم(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    إذا (l == عدم)
	أرجع;

    إذا (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	أرجع;
    }
    إذا (job->jv_argv != عدم)
	لكل (i = 0; job->jv_argv[i] != عدم; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ساكن فراغ
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	إذا (list_append_tv(l, &tv) != OK)
	    أرجع;
    }
}

/*
 * "job_info()" function
 */
    فراغ
f_job_info(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	إذا (job != عدم && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    وإلا إذا (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    فراغ
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    إذا (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    job = get_job_arg(&argvars[0]);
    إذا (job == عدم)
	أرجع;
    clear_job_options(&opt);
    إذا (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    فراغ
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_job = job_start(argvars, عدم, عدم, عدم);
}

/*
 * "job_status()" function
 */
    فراغ
f_job_status(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == عدم)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    وإلا
    {
	job_T	*job = get_job_arg(&argvars[0]);

	إذا (job != عدم)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    فراغ
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    إذا (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_رقم_arg(argvars, 1) == FAIL))
	أرجع;

    job = get_job_arg(&argvars[0]);
    إذا (job != عدم)
	rettv->vval.v_رقم = job_stop(job, argvars, عدم);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    حرف  *status;

    إذا (job == عدم)
    {
	vim_snprintf((حرف *)buf, NUMBUFLEN, "no process");
	أرجع buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#وإلاإذا defined(MSWIN)
    vim_snprintf((حرف *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#وإلا
    // fall-back
    vim_snprintf((حرف *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    أرجع buf;
}

#endif // FEAT_JOB_CHANNEL
