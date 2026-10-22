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

#اگر defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    برای ((job) = first_job; (job) != پوچ; (job) = (job)->jv_next)

    ایستا صحیح
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, صحیح jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    اگر (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    وگرنه اگر (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    وگرنه اگر (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    وگرنه اگر (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    وگرنه اگر (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    وگرنه اگر (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    وگرنه اگر (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    وگرنه
    {
	semsg(_(e_invalid_argument_str), val);
	بازگشت FAIL;
    }
    بازگشت OK;
}

    ایستا صحیح
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    اگر (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    وگرنه اگر (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    وگرنه اگر (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    وگرنه اگر (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    وگرنه اگر (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    وگرنه
    {
	semsg(_(e_invalid_argument_str), val);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    تهی
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    ایستا تهی
unref_job_callback(callback_T *cb)
{
    اگر (cb->cb_partial != پوچ)
	partial_unref(cb->cb_partial);
    وگرنه اگر (cb->cb_name != پوچ)
    {
	func_unref(cb->cb_name);
	اگر (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    تهی
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    اگر (opt->jo_env != پوچ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ایستا صحیح
part_from_char(صحیح c)
{
    بازگشت c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    صحیح
get_job_options(typval_T *tv, jobopt_T *opt, صحیح supported, صحیح supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    صحیح		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    اگر (tv->v_type == VAR_UNKNOWN)
	بازگشت OK;
    اگر (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	بازگشت FAIL;
    }
    dict = tv->vval.v_dict;
    اگر (dict == پوچ)
	بازگشت OK;

    todo = (صحیح)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_اقلام(&dict->dv_hashtab, hi, todo)
	اگر (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    اگر (STRCMP(hi->hi_کلید, "mode") == 0)
	    {
		اگر (!(supported & JO_MODE))
		    بشکن;
		اگر (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "in_mode") == 0)
	    {
		اگر (!(supported & JO_IN_MODE))
		    بشکن;
		اگر (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "out_mode") == 0)
	    {
		اگر (!(supported & JO_OUT_MODE))
		    بشکن;
		اگر (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "err_mode") == 0)
	    {
		اگر (!(supported & JO_ERR_MODE))
		    بشکن;
		اگر (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "noblock") == 0)
	    {
		اگر (!(supported & JO_MODE))
		    بشکن;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "in_io") == 0
		    || STRCMP(hi->hi_کلید, "out_io") == 0
		    || STRCMP(hi->hi_کلید, "err_io") == 0)
	    {
		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		اگر (handle_io(item, part_from_char(*hi->hi_کلید), opt) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "in_name") == 0
		    || STRCMP(hi->hi_کلید, "out_name") == 0
		    || STRCMP(hi->hi_کلید, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "pty") == 0)
	    {
		اگر (!(supported & JO_MODE))
		    بشکن;
		opt->jo_pty = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "in_buf") == 0
		    || STRCMP(hi->hi_کلید, "out_buf") == 0
		    || STRCMP(hi->hi_کلید, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_عدد(item);
		اگر (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str),
					      hi->hi_کلید, tv_get_string(item));
		    بازگشت FAIL;
		}
		اگر (buflist_findnr(opt->jo_io_buf[part]) == پوچ)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "out_modifiable") == 0
		    || STRCMP(hi->hi_کلید, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "out_msg") == 0
		    || STRCMP(hi->hi_کلید, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_کلید);

		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		opt->jo_set2 |= JO2_OUT_پیام << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "in_top") == 0
		    || STRCMP(hi->hi_کلید, "in_bot") == 0)
	    {
		linenr_T *lp;

		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		اگر (hi->hi_کلید[3] == 't')
		{
		    lp = &opt->jo_in_بالاترین;
		    opt->jo_set |= JO_IN_بالاترین;
		}
		وگرنه
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_عدد(item);
		اگر (*lp < 0)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str),
					      hi->hi_کلید, tv_get_string(item));
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "channel") == 0)
	    {
		اگر (!(supported & JO_OUT_IO))
		    بشکن;
		opt->jo_set |= JO_CHANNEL;
		اگر (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "channel");
		    بازگشت FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "callback") == 0)
	    {
		اگر (!(supported & JO_CALLBACK))
		    بشکن;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		اگر (opt->jo_callback.cb_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "callback");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "out_cb") == 0)
	    {
		اگر (!(supported & JO_OUT_CALLBACK))
		    بشکن;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		اگر (opt->jo_out_cb.cb_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "out_cb");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "err_cb") == 0)
	    {
		اگر (!(supported & JO_ERR_CALLBACK))
		    بشکن;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		اگر (opt->jo_err_cb.cb_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "err_cb");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "close_cb") == 0)
	    {
		اگر (!(supported & JO_CLOSE_CALLBACK))
		    بشکن;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		اگر (opt->jo_close_cb.cb_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "close_cb");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "drop") == 0)
	    {
		صحیح never = FALSE;
		val = tv_get_string(item);

		اگر (STRCMP(val, "never") == 0)
		    never = TRUE;
		وگرنه اگر (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str), "drop", val);
		    بازگشت FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "exit_cb") == 0)
	    {
		اگر (!(supported & JO_EXIT_CB))
		    بشکن;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		اگر (opt->jo_exit_cb.cb_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "exit_cb");
		    بازگشت FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_name") == 0)
	    {
		اگر (!(supported2 & JO2_TERM_NAME))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		اگر (opt->jo_term_name == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_name");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_finish") == 0)
	    {
		اگر (!(supported2 & JO2_TERM_FINISH))
		    بشکن;
		val = tv_get_string(item);
		اگر (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str),
							   "term_finish", val);
		    بازگشت FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_opencmd") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_TERM_OPENCMD))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		اگر (p != پوچ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    اگر (p != پوچ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != پوچ))
			p = پوچ;
		}
		اگر (p == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_opencmd");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "eof_chars") == 0)
	    {
		اگر (!(supported2 & JO2_EOF_CHARS))
		    بشکن;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		اگر (opt->jo_eof_chars == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "eof_chars");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_rows") == 0)
	    {
		صحیح error = FALSE;

		اگر (!(supported2 & JO2_TERM_ROWS))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_عدد_chk(item, &error);
		اگر (error)
		    بازگشت FAIL;
		اگر (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_rows");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_cols") == 0)
	    {
		صحیح error = FALSE;

		اگر (!(supported2 & JO2_TERM_COLS))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_عدد_chk(item, &error);
		اگر (error)
		    بازگشت FAIL;
		اگر (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_cols");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "vertical") == 0)
	    {
		اگر (!(supported2 & JO2_VERTICAL))
		    بشکن;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "curwin") == 0)
	    {
		اگر (!(supported2 & JO2_CURWIN))
		    بشکن;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "bufnr") == 0)
	    {
		صحیح nr;

		اگر (!(supported2 & JO2_CURWIN))
		    بشکن;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_عدد(item);
		اگر (nr <= 0)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str), hi->hi_کلید, tv_get_string(item));
		    بازگشت FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		اگر (opt->jo_bufnr_buf == پوچ)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    بازگشت FAIL;
		}
		اگر (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == پوچ)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "hidden") == 0)
	    {
		اگر (!(supported2 & JO2_HIDDEN))
		    بشکن;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "norestore") == 0)
	    {
		اگر (!(supported2 & JO2_NORESTORE))
		    بشکن;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_kill") == 0)
	    {
		اگر (!(supported2 & JO2_TERM_KILL))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		اگر (opt->jo_term_kill == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_kill");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "tty_type") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_TTY_TYPE))
		    بشکن;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		اگر (p == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "tty_type");
		    بازگشت FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		اگر (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "tty_type");
		    بازگشت FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    وگرنه اگر (STRCMP(hi->hi_کلید, "ansi_colors") == 0)
	    {
		صحیح		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		اگر (!(supported2 & JO2_ANSI_COLORS))
		    بشکن;

		اگر (item == پوچ || item->v_type != VAR_LIST
			|| item->vval.v_list == پوچ
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "ansi_colors");
		    بازگشت FAIL;
		}

		li = item->vval.v_list->lv_first;
		برای (; li != پوچ && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    صحیح		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    اگر (color_name == پوچ)
			بازگشت FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    اگر (guicolor == INVALCOLOR)
		    {
			اگر (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			بازگشت FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		اگر (n != 16 || li != پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "ansi_colors");
		    بازگشت FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, اندازه(rgb));
	    }
# endif
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_highlight") == 0)
	    {
		char_u *p;

		اگر (!(supported2 & JO2_TERM_HIGHLIGHT))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		اگر (p == پوچ || *p == NUL)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_highlight");
		    بازگشت FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "term_api") == 0)
	    {
		اگر (!(supported2 & JO2_TERM_API))
		    بشکن;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		اگر (opt->jo_term_api == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "term_api");
		    بازگشت FAIL;
		}
	    }
#endif
	    وگرنه اگر (STRCMP(hi->hi_کلید, "env") == 0)
	    {
		اگر (!(supported2 & JO2_ENV))
		    بشکن;
		اگر (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "env");
		    بازگشت FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		اگر (opt->jo_env != پوچ)
		    ++opt->jo_env->dv_refcount;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "cwd") == 0)
	    {
		اگر (!(supported2 & JO2_CWD))
		    بشکن;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		اگر (opt->jo_cwd == پوچ || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((نویسه *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "cwd");
		    بازگشت FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "waittime") == 0)
	    {
		اگر (!(supported & JO_WAITTIME))
		    بشکن;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_عدد(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "timeout") == 0)
	    {
		اگر (!(supported & JO_TIMEOUT))
		    بشکن;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_عدد(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "out_timeout") == 0)
	    {
		اگر (!(supported & JO_OUT_TIMEOUT))
		    بشکن;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_عدد(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "err_timeout") == 0)
	    {
		اگر (!(supported & JO_ERR_TIMEOUT))
		    بشکن;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_عدد(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "part") == 0)
	    {
		اگر (!(supported & JO_PART))
		    بشکن;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		اگر (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		وگرنه اگر (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		وگرنه
		{
		    semsg(_(e_invalid_مقدار_for_argument_str_str), "part", val);
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "id") == 0)
	    {
		اگر (!(supported & JO_ID))
		    بشکن;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_عدد(item);
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "stoponexit") == 0)
	    {
		اگر (!(supported & JO_STOPONEXIT))
		    بشکن;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		اگر (opt->jo_stoponexit == پوچ)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "stoponexit");
		    بازگشت FAIL;
		}
	    }
	    وگرنه اگر (STRCMP(hi->hi_کلید, "block_write") == 0)
	    {
		اگر (!(supported & JO_BLOCK_WRITE))
		    بشکن;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_عدد(item);
	    }
	    وگرنه
		بشکن;
	    --todo;
	}
    اگر (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_کلید);
	بازگشت FAIL;
    }

    بازگشت OK;
}

ایستا job_T *first_job = پوچ;

    ایستا تهی
job_free_contents(job_T *job)
{
    صحیح		i;

    ch_log(job->jv_channel, "Freeing job");
    اگر (job->jv_channel != پوچ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = پوچ;
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
    اگر (job->jv_argv != پوچ)
    {
	برای (i = 0; job->jv_argv[i] != پوچ; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ایستا تهی
job_unlink(job_T *job)
{
    اگر (job->jv_next != پوچ)
	job->jv_next->jv_prev = job->jv_prev;
    اگر (job->jv_prev == پوچ)
	first_job = job->jv_next;
    وگرنه
	job->jv_prev->jv_next = job->jv_next;
}

    ایستا تهی
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    ایستا تهی
job_free(job_T *job)
{
    اگر (in_free_unref_اقلام)
	بازگشت;

    job_free_contents(job);
    job_free_job(job);
}

ایستا job_T *jobs_to_free = پوچ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ایستا تهی
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    ایستا تهی
free_jobs_to_free_later(تهی)
{
    job_T *job;

    وقتی (jobs_to_free != پوچ)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#اگر defined(EXITFREE)
    تهی
job_free_all(تهی)
{
    وقتی (first_job != پوچ)
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
    ایستا صحیح
job_need_end_check(job_T *job)
{
    بازگشت job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != پوچ || job->jv_exit_cb.cb_name != پوچ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ایستا صحیح
job_channel_still_useful(job_T *job)
{
    بازگشت job->jv_channel != پوچ && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ایستا صحیح
job_channel_can_close(job_T *job)
{
    بازگشت job->jv_channel != پوچ && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ایستا صحیح
job_still_useful(job_T *job)
{
    بازگشت job_need_end_check(job) || job_channel_still_useful(job);
}

#اگر defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    صحیح
job_any_running(تهی)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	اگر (job_still_useful(job))
	{
	    ch_log(پوچ, "GUI not forking because a job is running");
	    بازگشت TRUE;
	}
    بازگشت FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#اگر defined(UNIX)
# define USE_ARGV
#endif

#اگر !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ایستا char_u *
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
    برای (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	اگر (*s == '"' || *s == '\\')
	    ++dlen;
	اگر (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    اگر (has_spaces)
	dlen += 2;

    اگر (dlen == slen)
	بازگشت vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    اگر (escaped_arg == پوچ)
	بازگشت پوچ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    اگر (has_spaces)
	*d++ = '"';

    برای (s = arg; *s != NUL;)
    {
	انتخاب (*s)
	{
	    حالت '"':
		برای (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		بشکن;
	    حالت '\\':
		escaping++;
		*d++ = *s++;
		بشکن;
	    پیشفرض:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		بشکن;
	}
    }

    // add terminating quote and finish with a NUL
    اگر (has_spaces)
    {
	برای (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    بازگشت escaped_arg;
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
    FOR_ALL_LIST_اقلام(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	اگر (s == پوچ)
	    بازگشت FAIL;
	s = win32_escape_arg(s);
	اگر (s == پوچ)
	    بازگشت FAIL;
	ga_concat(gap, s);
	vim_free(s);
	اگر (li->li_next != پوچ)
	    ga_append(gap, ' ');
    }
    بازگشت OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    تهی
job_cleanup(job_T *job)
{
    اگر (job->jv_status != JOB_ENDED)
	بازگشت;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    اگر (job->jv_channel != پوچ)
	ch_close_part(job->jv_channel, PART_IN);

    اگر (job->jv_exit_cb.cb_name != پوچ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    اگر (job->jv_channel != پوچ && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    اگر (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    صحیح
set_ref_in_job(صحیح copyID)
{
    صحیح		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    برای (job = first_job; !abort && job != پوچ; job = job->jv_next)
	اگر (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
	}
    بازگشت abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    تهی
job_unref(job_T *job)
{
    اگر (job == پوچ || --job->jv_refcount > 0)
	بازگشت;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    اگر (job_channel_still_useful(job))
	بازگشت;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    اگر (!job_need_end_check(job))
    {
	job_free(job);
    }
    وگرنه اگر (job->jv_channel != پوچ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = پوچ;
	channel_unref(job->jv_channel);
	job->jv_channel = پوچ;
    }
}

    صحیح
free_unused_jobs_contents(صحیح copyID, صحیح mask)
{
    صحیح		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	اگر ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    بازگشت did_free;
}

    تهی
free_unused_jobs(صحیح copyID, صحیح mask)
{
    job_T	*job;
    job_T	*job_next;

    برای (job = first_job; job != پوچ; job = job_next)
    {
	job_next = job->jv_next;
	اگر ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(تهی)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    اگر (job == پوچ)
	بازگشت پوچ;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    اگر (first_job != پوچ)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    بازگشت job;
}

    تهی
job_set_options(job_T *job, jobopt_T *opt)
{
    اگر (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	اگر (opt->jo_stoponexit == پوچ || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = پوچ;
	وگرنه
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    اگر (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	اگر (opt->jo_exit_cb.cb_name == پوچ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = پوچ;
	    job->jv_exit_cb.cb_partial = پوچ;
	}
	وگرنه
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    تهی
job_stop_on_exit(تهی)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	اگر (job->jv_status == JOB_STARTED && job->jv_stoponexit != پوچ)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    صحیح
has_pending_job(تهی)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	اگر ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    بازگشت TRUE;
    بازگشت FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    صحیح
job_check_ended(تهی)
{
    صحیح		i;
    صحیح		did_end = FALSE;

    // be quick if there are no jobs to check
    اگر (first_job == پوچ)
	بازگشت did_end;

    برای (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	اگر (job == پوچ)
	    بشکن;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    اگر (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    بازگشت did_end;
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
	نویسه	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = پوچ;
    نویسه	**argv = پوچ;
    صحیح		argc = 0;
    صحیح		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    اگر (job == پوچ)
	بازگشت پوچ;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, اندازه(نویسه*), 20);
#endif

    اگر (opt_arg != پوچ)
	opt = *opt_arg;
    وگرنه
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	اگر (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    برو theend;
    }

    // Check that when io is "file" that there is a file name.
    برای (part = PART_OUT; part < PART_شمار; ++part)
	اگر ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    برو theend;
	}

    اگر ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = پوچ;

	// check that we can find the buffer before starting the job
	اگر (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    اگر (buf == پوچ)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	وگرنه اگر (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	وگرنه
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	اگر (buf == پوچ)
	    برو theend;
	اگر (buf->b_ml.ml_mfp == پوچ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    اگر (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((نویسه *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    وگرنه
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    برو theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    اگر (argv_arg != پوچ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	برای (i = 0; argv_arg[i] != پوچ; i++)
	    argc++;
	argv = ALLOC_MULT(نویسه *, argc + 1);
	اگر (argv == پوچ)
	    برو theend;
	برای (i = 0; i < argc; i++)
	    argv[i] = (نویسه *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = پوچ;
    }
    وگرنه
#endif
    اگر (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	اگر (cmd == پوچ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    برو theend;
	}

	اگر (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    برو theend;
    }
    وگرنه اگر (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == پوچ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	برو theend;
    }
    وگرنه
    {
	list_T *l = argvars[0].vval.v_list;

	اگر (build_argv_from_list(l, &argv, &argc) == FAIL)
	    برو theend;

	// Empty command is invalid.
	اگر (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    برو theend;
	}
#ifndef USE_ARGV
	اگر (win32_build_cmd(l, &ga) == FAIL)
	    برو theend;
	cmd = ga.ga_data;
	اگر (cmd == پوچ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    برو theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    اگر (term_job != پوچ)
	*term_job = job;

#ifdef USE_ARGV
    اگر (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, اندازه(نویسه), 200);
	برای (i = 0; i < argc; ++i)
	{
	    اگر (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(پوچ, "Starting job: %s", (نویسه *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != پوچ);
#وگرنه
    ch_log(پوچ, "Starting job: %s", (نویسه *)cmd);
    mch_job_start((نویسه *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    اگر (job->jv_channel != پوچ)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    اگر (argv != پوچ && argv != job->jv_argv)
    {
	برای (i = 0; argv[i] != پوچ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    بازگشت job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    نویسه *
job_status(job_T *job)
{
    نویسه	*result;

    اگر (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    وگرنه اگر (job->jv_status == JOB_FAILED)
	result = "fail";
    وگرنه
    {
	result = mch_job_status(job);
	اگر (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    بازگشت result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    صحیح
job_stop(job_T *job, typval_T *argvars, نویسه *type)
{
    char_u *arg;

    اگر (type != پوچ)
	arg = (char_u *)type;
    وگرنه اگر (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    وگرنه
    {
	arg = tv_get_string_chk(&argvars[1]);
	اگر (arg == پوچ)
	{
	    emsg(_(e_invalid_argument));
	    بازگشت 0;
	}
    }
    اگر (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	بازگشت 0;
    }
    اگر (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	بازگشت 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (نویسه *)arg);
    اگر (mch_signal_job(job, arg) == FAIL)
	بازگشت 0;

    // Assume that only "kill" will kill the job.
    اگر (job->jv_channel != پوچ && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    بازگشت 1;
}

    تهی
invoke_prompt_callback(تهی)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_سطر_شمار;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    اگر (curbuf->b_prompt_callback.cb_name == پوچ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	بازگشت;
    text = ml_get(lnum);
    prompt = prompt_text();
    اگر (STRLEN(text) >= STRLEN(prompt))
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
    صحیح
invoke_prompt_interrupt(تهی)
{
    typval_T	rettv;
    typval_T	argv[1];
    صحیح		ret;

    اگر (curbuf->b_prompt_interrupt.cb_name == پوچ
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	بازگشت FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    بازگشت ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ایستا char_u *
buf_prompt_text(buf_T* buf)
{
    اگر (buf->b_prompt_text == پوچ)
	بازگشت (char_u *)"% ";
    بازگشت buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(تهی)
{
    بازگشت buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    تهی
init_prompt(صحیح cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
    text = ml_get_curline();
    اگر (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	اگر (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_سطر_شمار, prompt, TRUE);
	وگرنه
	    ml_append(curbuf->b_ml.ml_سطر_شمار, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_سطر_شمار, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    اگر (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (صحیح)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (صحیح)STRLEN(prompt));

    اگر (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    اگر (curwin->w_cursor.col < (صحیح)STRLEN(prompt))
	curwin->w_cursor.col = (صحیح)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    صحیح
prompt_curpos_editable(تهی)
{
    بازگشت curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_شمار
	&& curwin->w_cursor.col >= (صحیح)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    تهی
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == پوچ)
	بازگشت;

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == پوچ)
	بازگشت;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    تهی
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == پوچ)
	بازگشت;

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == پوچ)
	بازگشت;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    تهی
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = tv_get_buf_from_arg(&argvars[0]);
    اگر (buf == پوچ)
	بازگشت;

    اگر (!bt_prompt(buf))
	بازگشت;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    تهی
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (check_secure())
	بازگشت;
    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == پوچ)
	بازگشت;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ایستا job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    اگر (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	بازگشت پوچ;
    }
    job = tv->vval.v_job;

    اگر (job == پوچ)
	emsg(_(e_not_valid_job));
    بازگشت job;
}

/*
 * "job_getchannel()" function
 */
    تهی
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    اگر (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	بازگشت;

    job = get_job_arg(&argvars[0]);
    اگر (job == پوچ)
	بازگشت;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    اگر (job->jv_channel != پوچ)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ایستا تهی
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    صحیح		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    اگر (item == پوچ)
	بازگشت;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    اگر (job->jv_channel != پوچ)
	++job->jv_channel->ch_refcount;
    اگر (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#وگرنه
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_عدد(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_عدد(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    اگر (l == پوچ)
	بازگشت;

    اگر (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	بازگشت;
    }
    اگر (job->jv_argv != پوچ)
	برای (i = 0; job->jv_argv[i] != پوچ; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ایستا تهی
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	اگر (list_append_tv(l, &tv) != OK)
	    بازگشت;
    }
}

/*
 * "job_info()" function
 */
    تهی
f_job_info(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	اگر (job != پوچ && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    وگرنه اگر (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    تهی
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    اگر (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    job = get_job_arg(&argvars[0]);
    اگر (job == پوچ)
	بازگشت;
    clear_job_options(&opt);
    اگر (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    تهی
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_job = job_start(argvars, پوچ, پوچ, پوچ);
}

/*
 * "job_status()" function
 */
    تهی
f_job_status(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == پوچ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    وگرنه
    {
	job_T	*job = get_job_arg(&argvars[0]);

	اگر (job != پوچ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    تهی
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    اگر (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    job = get_job_arg(&argvars[0]);
    اگر (job != پوچ)
	rettv->vval.v_عدد = job_stop(job, argvars, پوچ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    نویسه  *status;

    اگر (job == پوچ)
    {
	vim_snprintf((نویسه *)buf, NUMBUFLEN, "no process");
	بازگشت buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((نویسه *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#وگرنهاگر defined(MSWIN)
    vim_snprintf((نویسه *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#وگرنه
    // fall-back
    vim_snprintf((نویسه *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    بازگشت buf;
}

#endif // FEAT_JOB_CHANNEL
