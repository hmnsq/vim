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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    জন্য ((job) = first_job; (job) != ফাঁকা; (job) = (job)->jv_পরবর্তী)

    স্থির পূর্ণসংখ্যা
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, পূর্ণসংখ্যা jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    যদি (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    অন্যথা যদি (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    অন্যথা যদি (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    অন্যথা যদি (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    অন্যথা যদি (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    অন্যথা যদি (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    অন্যথা যদি (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), val);
	ফেরত FAIL;
    }
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    যদি (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    অন্যথা যদি (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    অন্যথা যদি (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    অন্যথা যদি (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_বাফার;
    অন্যথা যদি (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), val);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    শূন্য
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    স্থির শূন্য
unref_job_callback(callback_T *cb)
{
    যদি (cb->cb_partial != ফাঁকা)
	partial_unref(cb->cb_partial);
    অন্যথা যদি (cb->cb_name != ফাঁকা)
    {
	func_unref(cb->cb_name);
	যদি (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    শূন্য
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    যদি (opt->jo_env != ফাঁকা)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    স্থির পূর্ণসংখ্যা
part_from_char(পূর্ণসংখ্যা c)
{
    ফেরত c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    পূর্ণসংখ্যা
get_job_options(typval_T *tv, jobopt_T *opt, পূর্ণসংখ্যা supported, পূর্ণসংখ্যা supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    পূর্ণসংখ্যা		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    যদি (tv->v_type == VAR_UNKNOWN)
	ফেরত OK;
    যদি (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ফেরত FAIL;
    }
    dict = tv->vval.v_dict;
    যদি (dict == ফাঁকা)
	ফেরত OK;

    todo = (পূর্ণসংখ্যা)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&dict->dv_hashtab, hi, todo)
	যদি (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    যদি (STRCMP(hi->hi_চাবি, "mode") == 0)
	    {
		যদি (!(supported & JO_MODE))
		    বিরতি;
		যদি (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "in_mode") == 0)
	    {
		যদি (!(supported & JO_IN_MODE))
		    বিরতি;
		যদি (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "out_mode") == 0)
	    {
		যদি (!(supported & JO_OUT_MODE))
		    বিরতি;
		যদি (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "err_mode") == 0)
	    {
		যদি (!(supported & JO_ERR_MODE))
		    বিরতি;
		যদি (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "noblock") == 0)
	    {
		যদি (!(supported & JO_MODE))
		    বিরতি;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "in_io") == 0
		    || STRCMP(hi->hi_চাবি, "out_io") == 0
		    || STRCMP(hi->hi_চাবি, "err_io") == 0)
	    {
		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		যদি (handle_io(item, part_from_char(*hi->hi_চাবি), opt) == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "in_name") == 0
		    || STRCMP(hi->hi_চাবি, "out_name") == 0
		    || STRCMP(hi->hi_চাবি, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_চাবি);

		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "pty") == 0)
	    {
		যদি (!(supported & JO_MODE))
		    বিরতি;
		opt->jo_pty = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "in_buf") == 0
		    || STRCMP(hi->hi_চাবি, "out_buf") == 0
		    || STRCMP(hi->hi_চাবি, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_চাবি);

		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_সংখ্যা(item);
		যদি (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_মান_for_argument_str_str),
					      hi->hi_চাবি, tv_get_string(item));
		    ফেরত FAIL;
		}
		যদি (buflist_findnr(opt->jo_io_buf[part]) == ফাঁকা)
		{
		    semsg(_(e_বাফার_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "out_modifiable") == 0
		    || STRCMP(hi->hi_চাবি, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_চাবি);

		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "out_msg") == 0
		    || STRCMP(hi->hi_চাবি, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_চাবি);

		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		opt->jo_set2 |= JO2_OUT_বার্তা << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "in_top") == 0
		    || STRCMP(hi->hi_চাবি, "in_bot") == 0)
	    {
		linenr_T *lp;

		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		যদি (hi->hi_চাবি[3] == 't')
		{
		    lp = &opt->jo_in_শীর্ষ;
		    opt->jo_set |= JO_IN_শীর্ষ;
		}
		অন্যথা
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_সংখ্যা(item);
		যদি (*lp < 0)
		{
		    semsg(_(e_invalid_মান_for_argument_str_str),
					      hi->hi_চাবি, tv_get_string(item));
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "channel") == 0)
	    {
		যদি (!(supported & JO_OUT_IO))
		    বিরতি;
		opt->jo_set |= JO_CHANNEL;
		যদি (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "channel");
		    ফেরত FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "callback") == 0)
	    {
		যদি (!(supported & JO_CALLBACK))
		    বিরতি;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		যদি (opt->jo_callback.cb_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "callback");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "out_cb") == 0)
	    {
		যদি (!(supported & JO_OUT_CALLBACK))
		    বিরতি;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		যদি (opt->jo_out_cb.cb_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "out_cb");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "err_cb") == 0)
	    {
		যদি (!(supported & JO_ERR_CALLBACK))
		    বিরতি;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		যদি (opt->jo_err_cb.cb_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "err_cb");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "close_cb") == 0)
	    {
		যদি (!(supported & JO_CLOSE_CALLBACK))
		    বিরতি;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		যদি (opt->jo_close_cb.cb_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "close_cb");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "drop") == 0)
	    {
		পূর্ণসংখ্যা never = FALSE;
		val = tv_get_string(item);

		যদি (STRCMP(val, "never") == 0)
		    never = TRUE;
		অন্যথা যদি (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_মান_for_argument_str_str), "drop", val);
		    ফেরত FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "exit_cb") == 0)
	    {
		যদি (!(supported & JO_EXIT_CB))
		    বিরতি;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		যদি (opt->jo_exit_cb.cb_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "exit_cb");
		    ফেরত FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_name") == 0)
	    {
		যদি (!(supported2 & JO2_TERM_NAME))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		যদি (opt->jo_term_name == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_name");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_finish") == 0)
	    {
		যদি (!(supported2 & JO2_TERM_FINISH))
		    বিরতি;
		val = tv_get_string(item);
		যদি (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_মান_for_argument_str_str),
							   "term_finish", val);
		    ফেরত FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_opencmd") == 0)
	    {
		char_u *p;

		যদি (!(supported2 & JO2_TERM_OPENCMD))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		যদি (p != ফাঁকা)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    যদি (p != ফাঁকা && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ফাঁকা))
			p = ফাঁকা;
		}
		যদি (p == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_opencmd");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "eof_chars") == 0)
	    {
		যদি (!(supported2 & JO2_EOF_CHARS))
		    বিরতি;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		যদি (opt->jo_eof_chars == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "eof_chars");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_rows") == 0)
	    {
		পূর্ণসংখ্যা error = FALSE;

		যদি (!(supported2 & JO2_TERM_ROWS))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_সংখ্যা_chk(item, &error);
		যদি (error)
		    ফেরত FAIL;
		যদি (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_rows");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_cols") == 0)
	    {
		পূর্ণসংখ্যা error = FALSE;

		যদি (!(supported2 & JO2_TERM_COLS))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_সংখ্যা_chk(item, &error);
		যদি (error)
		    ফেরত FAIL;
		যদি (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_cols");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "vertical") == 0)
	    {
		যদি (!(supported2 & JO2_VERTICAL))
		    বিরতি;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "curwin") == 0)
	    {
		যদি (!(supported2 & JO2_CURWIN))
		    বিরতি;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "bufnr") == 0)
	    {
		পূর্ণসংখ্যা nr;

		যদি (!(supported2 & JO2_CURWIN))
		    বিরতি;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_সংখ্যা(item);
		যদি (nr <= 0)
		{
		    semsg(_(e_invalid_মান_for_argument_str_str), hi->hi_চাবি, tv_get_string(item));
		    ফেরত FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		যদি (opt->jo_bufnr_buf == ফাঁকা)
		{
		    semsg(_(e_বাফার_nr_does_not_exist), (long)nr);
		    ফেরত FAIL;
		}
		যদি (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == ফাঁকা)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "hidden") == 0)
	    {
		যদি (!(supported2 & JO2_HIDDEN))
		    বিরতি;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "norestore") == 0)
	    {
		যদি (!(supported2 & JO2_NORESTORE))
		    বিরতি;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_kill") == 0)
	    {
		যদি (!(supported2 & JO2_TERM_KILL))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		যদি (opt->jo_term_kill == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_kill");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "tty_type") == 0)
	    {
		char_u *p;

		যদি (!(supported2 & JO2_TTY_TYPE))
		    বিরতি;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		যদি (p == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "tty_type");
		    ফেরত FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		যদি (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_মান_for_argument_str), "tty_type");
		    ফেরত FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "ansi_colors") == 0)
	    {
		পূর্ণসংখ্যা		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		যদি (!(supported2 & JO2_ANSI_COLORS))
		    বিরতি;

		যদি (item == ফাঁকা || item->v_type != VAR_LIST
			|| item->vval.v_list == ফাঁকা
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "ansi_colors");
		    ফেরত FAIL;
		}

		li = item->vval.v_list->lv_first;
		জন্য (; li != ফাঁকা && n < 16; li = li->li_পরবর্তী, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    পূর্ণসংখ্যা		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    যদি (color_name == ফাঁকা)
			ফেরত FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    যদি (guicolor == INVALCOLOR)
		    {
			যদি (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ফেরত FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		যদি (n != 16 || li != ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "ansi_colors");
		    ফেরত FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, আকার(rgb));
	    }
# endif
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_highlight") == 0)
	    {
		char_u *p;

		যদি (!(supported2 & JO2_TERM_HIGHLIGHT))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		যদি (p == ফাঁকা || *p == NUL)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_highlight");
		    ফেরত FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "term_api") == 0)
	    {
		যদি (!(supported2 & JO2_TERM_API))
		    বিরতি;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		যদি (opt->jo_term_api == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "term_api");
		    ফেরত FAIL;
		}
	    }
#endif
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "env") == 0)
	    {
		যদি (!(supported2 & JO2_ENV))
		    বিরতি;
		যদি (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "env");
		    ফেরত FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		যদি (opt->jo_env != ফাঁকা)
		    ++opt->jo_env->dv_refcount;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "cwd") == 0)
	    {
		যদি (!(supported2 & JO2_CWD))
		    বিরতি;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		যদি (opt->jo_cwd == ফাঁকা || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((অক্ষর *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "cwd");
		    ফেরত FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "waittime") == 0)
	    {
		যদি (!(supported & JO_WAITTIME))
		    বিরতি;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_সংখ্যা(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "timeout") == 0)
	    {
		যদি (!(supported & JO_TIMEOUT))
		    বিরতি;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_সংখ্যা(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "out_timeout") == 0)
	    {
		যদি (!(supported & JO_OUT_TIMEOUT))
		    বিরতি;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_সংখ্যা(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "err_timeout") == 0)
	    {
		যদি (!(supported & JO_ERR_TIMEOUT))
		    বিরতি;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_সংখ্যা(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "part") == 0)
	    {
		যদি (!(supported & JO_PART))
		    বিরতি;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		যদি (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		অন্যথা যদি (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		অন্যথা
		{
		    semsg(_(e_invalid_মান_for_argument_str_str), "part", val);
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "id") == 0)
	    {
		যদি (!(supported & JO_ID))
		    বিরতি;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_সংখ্যা(item);
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "stoponexit") == 0)
	    {
		যদি (!(supported & JO_STOPONEXIT))
		    বিরতি;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		যদি (opt->jo_stoponexit == ফাঁকা)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "stoponexit");
		    ফেরত FAIL;
		}
	    }
	    অন্যথা যদি (STRCMP(hi->hi_চাবি, "block_write") == 0)
	    {
		যদি (!(supported & JO_BLOCK_WRITE))
		    বিরতি;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_সংখ্যা(item);
	    }
	    অন্যথা
		বিরতি;
	    --todo;
	}
    যদি (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_চাবি);
	ফেরত FAIL;
    }

    ফেরত OK;
}

স্থির job_T *first_job = ফাঁকা;

    স্থির শূন্য
job_free_contents(job_T *job)
{
    পূর্ণসংখ্যা		i;

    ch_log(job->jv_channel, "Freeing job");
    যদি (job->jv_channel != ফাঁকা)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = ফাঁকা;
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
    যদি (job->jv_argv != ফাঁকা)
    {
	জন্য (i = 0; job->jv_argv[i] != ফাঁকা; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    স্থির শূন্য
job_unlink(job_T *job)
{
    যদি (job->jv_পরবর্তী != ফাঁকা)
	job->jv_পরবর্তী->jv_পূর্ব = job->jv_পূর্ব;
    যদি (job->jv_পূর্ব == ফাঁকা)
	first_job = job->jv_পরবর্তী;
    অন্যথা
	job->jv_পূর্ব->jv_পরবর্তী = job->jv_পরবর্তী;
}

    স্থির শূন্য
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    স্থির শূন্য
job_free(job_T *job)
{
    যদি (in_free_unref_বস্তুসমূহ)
	ফেরত;

    job_free_contents(job);
    job_free_job(job);
}

স্থির job_T *jobs_to_free = ফাঁকা;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    স্থির শূন্য
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_পরবর্তী = jobs_to_free;
    jobs_to_free = job;
}

    স্থির শূন্য
free_jobs_to_free_later(শূন্য)
{
    job_T *job;

    যখন (jobs_to_free != ফাঁকা)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_পরবর্তী;
	job_free_contents(job);
	vim_free(job);
    }
}

#যদি defined(EXITFREE)
    শূন্য
job_free_all(শূন্য)
{
    যখন (first_job != ফাঁকা)
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
    স্থির পূর্ণসংখ্যা
job_need_end_check(job_T *job)
{
    ফেরত job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != ফাঁকা || job->jv_exit_cb.cb_name != ফাঁকা);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    স্থির পূর্ণসংখ্যা
job_channel_still_useful(job_T *job)
{
    ফেরত job->jv_channel != ফাঁকা && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    স্থির পূর্ণসংখ্যা
job_channel_can_close(job_T *job)
{
    ফেরত job->jv_channel != ফাঁকা && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    স্থির পূর্ণসংখ্যা
job_still_useful(job_T *job)
{
    ফেরত job_need_end_check(job) || job_channel_still_useful(job);
}

#যদি defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    পূর্ণসংখ্যা
job_any_running(শূন্য)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	যদি (job_still_useful(job))
	{
	    ch_log(ফাঁকা, "GUI not forking because a job is running");
	    ফেরত TRUE;
	}
    ফেরত FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#যদি defined(UNIX)
# define USE_ARGV
#endif

#যদি !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    স্থির char_u *
win32_escape_arg(char_u *arg)
{
    পূর্ণসংখ্যা		slen, dlen;
    পূর্ণসংখ্যা		escaping = 0;
    পূর্ণসংখ্যা		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    পূর্ণসংখ্যা		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (পূর্ণসংখ্যা)STRLEN(arg);
    dlen = slen;
    জন্য (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	যদি (*s == '"' || *s == '\\')
	    ++dlen;
	যদি (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    যদি (has_spaces)
	dlen += 2;

    যদি (dlen == slen)
	ফেরত vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    যদি (escaped_arg == ফাঁকা)
	ফেরত ফাঁকা;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    যদি (has_spaces)
	*d++ = '"';

    জন্য (s = arg; *s != NUL;)
    {
	নির্বাচন (*s)
	{
	    ক্ষেত্র '"':
		জন্য (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		বিরতি;
	    ক্ষেত্র '\\':
		escaping++;
		*d++ = *s++;
		বিরতি;
	    পূর্বনির্ধারিত:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		বিরতি;
	}
    }

    // add terminating quote and finish with a NUL
    যদি (has_spaces)
    {
	জন্য (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ফেরত escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	যদি (s == ফাঁকা)
	    ফেরত FAIL;
	s = win32_escape_arg(s);
	যদি (s == ফাঁকা)
	    ফেরত FAIL;
	ga_concat(gap, s);
	vim_free(s);
	যদি (li->li_পরবর্তী != ফাঁকা)
	    ga_append(gap, ' ');
    }
    ফেরত OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    শূন্য
job_cleanup(job_T *job)
{
    যদি (job->jv_status != JOB_ENDED)
	ফেরত;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    যদি (job->jv_channel != ফাঁকা)
	ch_close_part(job->jv_channel, PART_IN);

    যদি (job->jv_exit_cb.cb_name != ফাঁকা)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_সংখ্যা;
	argv[1].vval.v_সংখ্যা = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    যদি (job->jv_channel != ফাঁকা && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    যদি (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    পূর্ণসংখ্যা
set_ref_in_job(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    জন্য (job = first_job; !abort && job != ফাঁকা; job = job->jv_পরবর্তী)
	যদি (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
	}
    ফেরত abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    শূন্য
job_unref(job_T *job)
{
    যদি (job == ফাঁকা || --job->jv_refcount > 0)
	ফেরত;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    যদি (job_channel_still_useful(job))
	ফেরত;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    যদি (!job_need_end_check(job))
    {
	job_free(job);
    }
    অন্যথা যদি (job->jv_channel != ফাঁকা)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = ফাঁকা;
	channel_unref(job->jv_channel);
	job->jv_channel = ফাঁকা;
    }
}

    পূর্ণসংখ্যা
free_unused_jobs_contents(পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা mask)
{
    পূর্ণসংখ্যা		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	যদি ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    ফেরত did_free;
}

    শূন্য
free_unused_jobs(পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা mask)
{
    job_T	*job;
    job_T	*job_পরবর্তী;

    জন্য (job = first_job; job != ফাঁকা; job = job_পরবর্তী)
    {
	job_পরবর্তী = job->jv_পরবর্তী;
	যদি ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(শূন্য)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    যদি (job == ফাঁকা)
	ফেরত ফাঁকা;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    যদি (first_job != ফাঁকা)
    {
	first_job->jv_পূর্ব = job;
	job->jv_পরবর্তী = first_job;
    }
    first_job = job;
    ফেরত job;
}

    শূন্য
job_set_options(job_T *job, jobopt_T *opt)
{
    যদি (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	যদি (opt->jo_stoponexit == ফাঁকা || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = ফাঁকা;
	অন্যথা
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    যদি (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	যদি (opt->jo_exit_cb.cb_name == ফাঁকা || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = ফাঁকা;
	    job->jv_exit_cb.cb_partial = ফাঁকা;
	}
	অন্যথা
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    শূন্য
job_stop_on_exit(শূন্য)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	যদি (job->jv_status == JOB_STARTED && job->jv_stoponexit != ফাঁকা)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    পূর্ণসংখ্যা
has_pending_job(শূন্য)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	যদি ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    ফেরত TRUE;
    ফেরত FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    পূর্ণসংখ্যা
job_check_ended(শূন্য)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		did_end = FALSE;

    // be quick if there are no jobs to check
    যদি (first_job == ফাঁকা)
	ফেরত did_end;

    জন্য (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	যদি (job == ফাঁকা)
	    বিরতি;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    যদি (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ফেরত did_end;
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
	অক্ষর	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = ফাঁকা;
    অক্ষর	**argv = ফাঁকা;
    পূর্ণসংখ্যা		argc = 0;
    পূর্ণসংখ্যা		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    যদি (job == ফাঁকা)
	ফেরত ফাঁকা;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, আকার(অক্ষর*), 20);
#endif

    যদি (opt_arg != ফাঁকা)
	opt = *opt_arg;
    অন্যথা
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	যদি (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    যাও theend;
    }

    // Check that when io is "file" that there is a file name.
    জন্য (part = PART_OUT; part < PART_গুনতি; ++part)
	যদি ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    যাও theend;
	}

    যদি ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_বাফার)
    {
	buf_T *buf = ফাঁকা;

	// check that we can find the buffer before starting the job
	যদি (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    যদি (buf == ফাঁকা)
		semsg(_(e_বাফার_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	অন্যথা যদি (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_বাফার_requires_in_buf_or_in_name_to_be_set));
	}
	অন্যথা
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	যদি (buf == ফাঁকা)
	    যাও theend;
	যদি (buf->b_ml.ml_mfp == ফাঁকা)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    যদি (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((অক্ষর *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    অন্যথা
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_বাফার_must_be_loaded_str), s);
	    যাও theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    যদি (argv_arg != ফাঁকা)
    {
	// Make a copy of argv_arg for job->jv_argv.
	জন্য (i = 0; argv_arg[i] != ফাঁকা; i++)
	    argc++;
	argv = ALLOC_MULT(অক্ষর *, argc + 1);
	যদি (argv == ফাঁকা)
	    যাও theend;
	জন্য (i = 0; i < argc; i++)
	    argv[i] = (অক্ষর *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ফাঁকা;
    }
    অন্যথা
#endif
    যদি (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	যদি (cmd == ফাঁকা || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    যাও theend;
	}

	যদি (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    যাও theend;
    }
    অন্যথা যদি (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ফাঁকা
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	যাও theend;
    }
    অন্যথা
    {
	list_T *l = argvars[0].vval.v_list;

	যদি (build_argv_from_list(l, &argv, &argc) == FAIL)
	    যাও theend;

	// Empty command is invalid.
	যদি (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    যাও theend;
	}
#ifndef USE_ARGV
	যদি (win32_build_cmd(l, &ga) == FAIL)
	    যাও theend;
	cmd = ga.ga_তথ্য;
	যদি (cmd == ফাঁকা || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    যাও theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    যদি (term_job != ফাঁকা)
	*term_job = job;

#ifdef USE_ARGV
    যদি (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, আকার(অক্ষর), 200);
	জন্য (i = 0; i < argc; ++i)
	{
	    যদি (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ফাঁকা, "Starting job: %s", (অক্ষর *)ga.ga_তথ্য);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != ফাঁকা);
#অন্যথা
    ch_log(ফাঁকা, "Starting job: %s", (অক্ষর *)cmd);
    mch_job_start((অক্ষর *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    যদি (job->jv_channel != ফাঁকা)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_তথ্য);
#endif
    যদি (argv != ফাঁকা && argv != job->jv_argv)
    {
	জন্য (i = 0; argv[i] != ফাঁকা; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    ফেরত job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    অক্ষর *
job_status(job_T *job)
{
    অক্ষর	*ফলাফল;

    যদি (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	ফলাফল = "dead";
    অন্যথা যদি (job->jv_status == JOB_FAILED)
	ফলাফল = "fail";
    অন্যথা
    {
	ফলাফল = mch_job_status(job);
	যদি (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    ফেরত ফলাফল;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    পূর্ণসংখ্যা
job_stop(job_T *job, typval_T *argvars, অক্ষর *type)
{
    char_u *arg;

    যদি (type != ফাঁকা)
	arg = (char_u *)type;
    অন্যথা যদি (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    অন্যথা
    {
	arg = tv_get_string_chk(&argvars[1]);
	যদি (arg == ফাঁকা)
	{
	    emsg(_(e_invalid_argument));
	    ফেরত 0;
	}
    }
    যদি (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	ফেরত 0;
    }
    যদি (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	ফেরত 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (অক্ষর *)arg);
    যদি (mch_signal_job(job, arg) == FAIL)
	ফেরত 0;

    // Assume that only "kill" will kill the job.
    যদি (job->jv_channel != ফাঁকা && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ফেরত 1;
}

    শূন্য
invoke_prompt_callback(শূন্য)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_সারি_গুনতি;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    যদি (curbuf->b_prompt_callback.cb_name == ফাঁকা
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ফেরত;
    text = ml_get(lnum);
    prompt = prompt_text();
    যদি (STRLEN(text) >= STRLEN(prompt))
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
    পূর্ণসংখ্যা
invoke_prompt_interrupt(শূন্য)
{
    typval_T	rettv;
    typval_T	argv[1];
    পূর্ণসংখ্যা		ret;

    যদি (curbuf->b_prompt_interrupt.cb_name == ফাঁকা
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	ফেরত FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ফেরত ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    স্থির char_u *
buf_prompt_text(buf_T* buf)
{
    যদি (buf->b_prompt_text == ফাঁকা)
	ফেরত (char_u *)"% ";
    ফেরত buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(শূন্য)
{
    ফেরত buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    শূন্য
init_prompt(পূর্ণসংখ্যা cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
    text = ml_get_curline();
    যদি (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	যদি (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_সারি_গুনতি, prompt, TRUE);
	অন্যথা
	    ml_append(curbuf->b_ml.ml_সারি_গুনতি, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_সারি_গুনতি, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    যদি (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (পূর্ণসংখ্যা)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (পূর্ণসংখ্যা)STRLEN(prompt));

    যদি (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    যদি (curwin->w_cursor.col < (পূর্ণসংখ্যা)STRLEN(prompt))
	curwin->w_cursor.col = (পূর্ণসংখ্যা)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    পূর্ণসংখ্যা
prompt_curpos_editable(শূন্য)
{
    ফেরত curwin->w_cursor.lnum == curbuf->b_ml.ml_সারি_গুনতি
	&& curwin->w_cursor.col >= (পূর্ণসংখ্যা)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    শূন্য
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == ফাঁকা)
	ফেরত;

    callback = get_callback(&argvars[1]);
    যদি (callback.cb_name == ফাঁকা)
	ফেরত;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    শূন্য
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == ফাঁকা)
	ফেরত;

    callback = get_callback(&argvars[1]);
    যদি (callback.cb_name == ফাঁকা)
	ফেরত;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    শূন্য
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = tv_get_buf_from_arg(&argvars[0]);
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি (!bt_prompt(buf))
	ফেরত;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    শূন্য
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (check_secure())
	ফেরত;
    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == ফাঁকা)
	ফেরত;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    স্থির job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    যদি (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ফেরত ফাঁকা;
    }
    job = tv->vval.v_job;

    যদি (job == ফাঁকা)
	emsg(_(e_not_valid_job));
    ফেরত job;
}

/*
 * "job_getchannel()" function
 */
    শূন্য
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    যদি (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ফেরত;

    job = get_job_arg(&argvars[0]);
    যদি (job == ফাঁকা)
	ফেরত;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    যদি (job->jv_channel != ফাঁকা)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    স্থির শূন্য
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    পূর্ণসংখ্যা		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    যদি (item == ফাঁকা)
	ফেরত;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    যদি (job->jv_channel != ফাঁকা)
	++job->jv_channel->ch_refcount;
    যদি (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#অন্যথা
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_সংখ্যা(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_সংখ্যা(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    যদি (l == ফাঁকা)
	ফেরত;

    যদি (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ফেরত;
    }
    যদি (job->jv_argv != ফাঁকা)
	জন্য (i = 0; job->jv_argv[i] != ফাঁকা; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    স্থির শূন্য
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	যদি (list_append_tv(l, &tv) != OK)
	    ফেরত;
    }
}

/*
 * "job_info()" function
 */
    শূন্য
f_job_info(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	যদি (job != ফাঁকা && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    অন্যথা যদি (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    শূন্য
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    যদি (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    job = get_job_arg(&argvars[0]);
    যদি (job == ফাঁকা)
	ফেরত;
    clear_job_options(&opt);
    যদি (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    শূন্য
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_job = job_start(argvars, ফাঁকা, ফাঁকা, ফাঁকা);
}

/*
 * "job_status()" function
 */
    শূন্য
f_job_status(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == ফাঁকা)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    অন্যথা
    {
	job_T	*job = get_job_arg(&argvars[0]);

	যদি (job != ফাঁকা)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    শূন্য
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    যদি (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    job = get_job_arg(&argvars[0]);
    যদি (job != ফাঁকা)
	rettv->vval.v_সংখ্যা = job_stop(job, argvars, ফাঁকা);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    অক্ষর  *status;

    যদি (job == ফাঁকা)
    {
	vim_snprintf((অক্ষর *)buf, NUMBUFLEN, "no process");
	ফেরত buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((অক্ষর *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#নতুবা_যদি defined(MSWIN)
    vim_snprintf((অক্ষর *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#অন্যথা
    // fall-back
    vim_snprintf((অক্ষর *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    ফেরত buf;
}

#endif // FEAT_JOB_CHANNEL
