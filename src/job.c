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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    साठी ((job) = first_job; (job) != रिक्त; (job) = (job)->jv_next)

    स्थिर पूर्णांक
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, पूर्णांक jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    जर (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    अन्यथा जर (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    अन्यथा जर (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    अन्यथा जर (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    अन्यथा जर (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    अन्यथा जर (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    अन्यथा जर (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    अन्यथा
    {
	semsg(_(e_invalid_argument_str), val);
	परत FAIL;
    }
    परत OK;
}

    स्थिर पूर्णांक
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    जर (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    अन्यथा जर (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    अन्यथा जर (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    अन्यथा जर (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    अन्यथा जर (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    अन्यथा
    {
	semsg(_(e_invalid_argument_str), val);
	परत FAIL;
    }
    परत OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    शून्य
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    स्थिर शून्य
unref_job_callback(callback_T *cb)
{
    जर (cb->cb_partial != रिक्त)
	partial_unref(cb->cb_partial);
    अन्यथा जर (cb->cb_name != रिक्त)
    {
	func_unref(cb->cb_name);
	जर (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    शून्य
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    जर (opt->jo_env != रिक्त)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    स्थिर पूर्णांक
part_from_char(पूर्णांक c)
{
    परत c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    पूर्णांक
get_job_options(typval_T *tv, jobopt_T *opt, पूर्णांक supported, पूर्णांक supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    पूर्णांक		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    जर (tv->v_type == VAR_UNKNOWN)
	परत OK;
    जर (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	परत FAIL;
    }
    dict = tv->vval.v_dict;
    जर (dict == रिक्त)
	परत OK;

    todo = (पूर्णांक)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(&dict->dv_hashtab, hi, todo)
	जर (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    जर (STRCMP(hi->hi_किल्ली, "mode") == 0)
	    {
		जर (!(supported & JO_MODE))
		    थांबा;
		जर (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "in_mode") == 0)
	    {
		जर (!(supported & JO_IN_MODE))
		    थांबा;
		जर (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "out_mode") == 0)
	    {
		जर (!(supported & JO_OUT_MODE))
		    थांबा;
		जर (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "err_mode") == 0)
	    {
		जर (!(supported & JO_ERR_MODE))
		    थांबा;
		जर (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "noblock") == 0)
	    {
		जर (!(supported & JO_MODE))
		    थांबा;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "in_io") == 0
		    || STRCMP(hi->hi_किल्ली, "out_io") == 0
		    || STRCMP(hi->hi_किल्ली, "err_io") == 0)
	    {
		जर (!(supported & JO_OUT_IO))
		    थांबा;
		जर (handle_io(item, part_from_char(*hi->hi_किल्ली), opt) == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "in_name") == 0
		    || STRCMP(hi->hi_किल्ली, "out_name") == 0
		    || STRCMP(hi->hi_किल्ली, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_किल्ली);

		जर (!(supported & JO_OUT_IO))
		    थांबा;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "pty") == 0)
	    {
		जर (!(supported & JO_MODE))
		    थांबा;
		opt->jo_pty = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "in_buf") == 0
		    || STRCMP(hi->hi_किल्ली, "out_buf") == 0
		    || STRCMP(hi->hi_किल्ली, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_किल्ली);

		जर (!(supported & JO_OUT_IO))
		    थांबा;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_संख्या(item);
		जर (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str),
					      hi->hi_किल्ली, tv_get_string(item));
		    परत FAIL;
		}
		जर (buflist_findnr(opt->jo_io_buf[part]) == रिक्त)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (दीर्घ)opt->jo_io_buf[part]);
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "out_modifiable") == 0
		    || STRCMP(hi->hi_किल्ली, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_किल्ली);

		जर (!(supported & JO_OUT_IO))
		    थांबा;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "out_msg") == 0
		    || STRCMP(hi->hi_किल्ली, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_किल्ली);

		जर (!(supported & JO_OUT_IO))
		    थांबा;
		opt->jo_set2 |= JO2_OUT_संदेश << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "in_top") == 0
		    || STRCMP(hi->hi_किल्ली, "in_bot") == 0)
	    {
		linenr_T *lp;

		जर (!(supported & JO_OUT_IO))
		    थांबा;
		जर (hi->hi_किल्ली[3] == 't')
		{
		    lp = &opt->jo_in_शीर्ष;
		    opt->jo_set |= JO_IN_शीर्ष;
		}
		अन्यथा
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_संख्या(item);
		जर (*lp < 0)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str),
					      hi->hi_किल्ली, tv_get_string(item));
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "channel") == 0)
	    {
		जर (!(supported & JO_OUT_IO))
		    थांबा;
		opt->jo_set |= JO_CHANNEL;
		जर (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "channel");
		    परत FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "callback") == 0)
	    {
		जर (!(supported & JO_CALLBACK))
		    थांबा;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		जर (opt->jo_callback.cb_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "callback");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "out_cb") == 0)
	    {
		जर (!(supported & JO_OUT_CALLBACK))
		    थांबा;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		जर (opt->jo_out_cb.cb_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "out_cb");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "err_cb") == 0)
	    {
		जर (!(supported & JO_ERR_CALLBACK))
		    थांबा;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		जर (opt->jo_err_cb.cb_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "err_cb");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "close_cb") == 0)
	    {
		जर (!(supported & JO_CLOSE_CALLBACK))
		    थांबा;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		जर (opt->jo_close_cb.cb_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "close_cb");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "drop") == 0)
	    {
		पूर्णांक never = FALSE;
		val = tv_get_string(item);

		जर (STRCMP(val, "never") == 0)
		    never = TRUE;
		अन्यथा जर (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str), "drop", val);
		    परत FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "exit_cb") == 0)
	    {
		जर (!(supported & JO_EXIT_CB))
		    थांबा;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		जर (opt->jo_exit_cb.cb_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "exit_cb");
		    परत FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_name") == 0)
	    {
		जर (!(supported2 & JO2_TERM_NAME))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		जर (opt->jo_term_name == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_name");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_finish") == 0)
	    {
		जर (!(supported2 & JO2_TERM_FINISH))
		    थांबा;
		val = tv_get_string(item);
		जर (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str),
							   "term_finish", val);
		    परत FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_opencmd") == 0)
	    {
		char_u *p;

		जर (!(supported2 & JO2_TERM_OPENCMD))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		जर (p != रिक्त)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    जर (p != रिक्त && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != रिक्त))
			p = रिक्त;
		}
		जर (p == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_opencmd");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "eof_chars") == 0)
	    {
		जर (!(supported2 & JO2_EOF_CHARS))
		    थांबा;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		जर (opt->jo_eof_chars == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "eof_chars");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_rows") == 0)
	    {
		पूर्णांक error = FALSE;

		जर (!(supported2 & JO2_TERM_ROWS))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_संख्या_chk(item, &error);
		जर (error)
		    परत FAIL;
		जर (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_rows");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_cols") == 0)
	    {
		पूर्णांक error = FALSE;

		जर (!(supported2 & JO2_TERM_COLS))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_संख्या_chk(item, &error);
		जर (error)
		    परत FAIL;
		जर (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_cols");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "vertical") == 0)
	    {
		जर (!(supported2 & JO2_VERTICAL))
		    थांबा;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "curwin") == 0)
	    {
		जर (!(supported2 & JO2_CURWIN))
		    थांबा;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "bufnr") == 0)
	    {
		पूर्णांक nr;

		जर (!(supported2 & JO2_CURWIN))
		    थांबा;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_संख्या(item);
		जर (nr <= 0)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str), hi->hi_किल्ली, tv_get_string(item));
		    परत FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		जर (opt->jo_bufnr_buf == रिक्त)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (दीर्घ)nr);
		    परत FAIL;
		}
		जर (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == रिक्त)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "hidden") == 0)
	    {
		जर (!(supported2 & JO2_HIDDEN))
		    थांबा;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "norestore") == 0)
	    {
		जर (!(supported2 & JO2_NORESTORE))
		    थांबा;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_kill") == 0)
	    {
		जर (!(supported2 & JO2_TERM_KILL))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		जर (opt->jo_term_kill == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_kill");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "tty_type") == 0)
	    {
		char_u *p;

		जर (!(supported2 & JO2_TTY_TYPE))
		    थांबा;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		जर (p == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "tty_type");
		    परत FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		जर (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "tty_type");
		    परत FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "ansi_colors") == 0)
	    {
		पूर्णांक		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		जर (!(supported2 & JO2_ANSI_COLORS))
		    थांबा;

		जर (item == रिक्त || item->v_type != VAR_LIST
			|| item->vval.v_list == रिक्त
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "ansi_colors");
		    परत FAIL;
		}

		li = item->vval.v_list->lv_first;
		साठी (; li != रिक्त && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    पूर्णांक		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    जर (color_name == रिक्त)
			परत FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    जर (guicolor == INVALCOLOR)
		    {
			जर (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			परत FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		जर (n != 16 || li != रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "ansi_colors");
		    परत FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		स_नकल(opt->jo_ansi_colors, rgb, आकार(rgb));
	    }
# endif
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_highlight") == 0)
	    {
		char_u *p;

		जर (!(supported2 & JO2_TERM_HIGHLIGHT))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		जर (p == रिक्त || *p == NUL)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_highlight");
		    परत FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "term_api") == 0)
	    {
		जर (!(supported2 & JO2_TERM_API))
		    थांबा;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		जर (opt->jo_term_api == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "term_api");
		    परत FAIL;
		}
	    }
#endif
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "env") == 0)
	    {
		जर (!(supported2 & JO2_ENV))
		    थांबा;
		जर (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "env");
		    परत FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		जर (opt->jo_env != रिक्त)
		    ++opt->jo_env->dv_refcount;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "cwd") == 0)
	    {
		जर (!(supported2 & JO2_CWD))
		    थांबा;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		जर (opt->jo_cwd == रिक्त || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((अक्षर *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "cwd");
		    परत FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "waittime") == 0)
	    {
		जर (!(supported & JO_WAITTIME))
		    थांबा;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_संख्या(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "timeout") == 0)
	    {
		जर (!(supported & JO_TIMEOUT))
		    थांबा;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_संख्या(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "out_timeout") == 0)
	    {
		जर (!(supported & JO_OUT_TIMEOUT))
		    थांबा;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_संख्या(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "err_timeout") == 0)
	    {
		जर (!(supported & JO_ERR_TIMEOUT))
		    थांबा;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_संख्या(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "part") == 0)
	    {
		जर (!(supported & JO_PART))
		    थांबा;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		जर (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		अन्यथा जर (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		अन्यथा
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str_str), "part", val);
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "id") == 0)
	    {
		जर (!(supported & JO_ID))
		    थांबा;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_संख्या(item);
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "stoponexit") == 0)
	    {
		जर (!(supported & JO_STOPONEXIT))
		    थांबा;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		जर (opt->jo_stoponexit == रिक्त)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "stoponexit");
		    परत FAIL;
		}
	    }
	    अन्यथा जर (STRCMP(hi->hi_किल्ली, "block_write") == 0)
	    {
		जर (!(supported & JO_BLOCK_WRITE))
		    थांबा;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_संख्या(item);
	    }
	    अन्यथा
		थांबा;
	    --todo;
	}
    जर (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_किल्ली);
	परत FAIL;
    }

    परत OK;
}

स्थिर job_T *first_job = रिक्त;

    स्थिर शून्य
job_free_contents(job_T *job)
{
    पूर्णांक		i;

    ch_log(job->jv_channel, "Freeing job");
    जर (job->jv_channel != रिक्त)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = रिक्त;
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
    जर (job->jv_argv != रिक्त)
    {
	साठी (i = 0; job->jv_argv[i] != रिक्त; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    स्थिर शून्य
job_unlink(job_T *job)
{
    जर (job->jv_next != रिक्त)
	job->jv_next->jv_prev = job->jv_prev;
    जर (job->jv_prev == रिक्त)
	first_job = job->jv_next;
    अन्यथा
	job->jv_prev->jv_next = job->jv_next;
}

    स्थिर शून्य
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    स्थिर शून्य
job_free(job_T *job)
{
    जर (in_free_unref_वस्तुसंच)
	परत;

    job_free_contents(job);
    job_free_job(job);
}

स्थिर job_T *jobs_to_free = रिक्त;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    स्थिर शून्य
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    स्थिर शून्य
free_jobs_to_free_later(शून्य)
{
    job_T *job;

    पर्यंत (jobs_to_free != रिक्त)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#जर defined(EXITFREE)
    शून्य
job_free_all(शून्य)
{
    पर्यंत (first_job != रिक्त)
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
    स्थिर पूर्णांक
job_need_end_check(job_T *job)
{
    परत job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != रिक्त || job->jv_exit_cb.cb_name != रिक्त);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    स्थिर पूर्णांक
job_channel_still_useful(job_T *job)
{
    परत job->jv_channel != रिक्त && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    स्थिर पूर्णांक
job_channel_can_close(job_T *job)
{
    परत job->jv_channel != रिक्त && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    स्थिर पूर्णांक
job_still_useful(job_T *job)
{
    परत job_need_end_check(job) || job_channel_still_useful(job);
}

#जर defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    पूर्णांक
job_any_running(शून्य)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	जर (job_still_useful(job))
	{
	    ch_log(रिक्त, "GUI not forking because a job is running");
	    परत TRUE;
	}
    परत FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#जर defined(UNIX)
# define USE_ARGV
#endif

#जर !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    स्थिर char_u *
win32_escape_arg(char_u *arg)
{
    पूर्णांक		slen, dlen;
    पूर्णांक		escaping = 0;
    पूर्णांक		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    पूर्णांक		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (पूर्णांक)STRLEN(arg);
    dlen = slen;
    साठी (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	जर (*s == '"' || *s == '\\')
	    ++dlen;
	जर (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    जर (has_spaces)
	dlen += 2;

    जर (dlen == slen)
	परत vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    जर (escaped_arg == रिक्त)
	परत रिक्त;
    स_रखो(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    जर (has_spaces)
	*d++ = '"';

    साठी (s = arg; *s != NUL;)
    {
	निवड (*s)
	{
	    स्थिती '"':
		साठी (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		थांबा;
	    स्थिती '\\':
		escaping++;
		*d++ = *s++;
		थांबा;
	    पूर्वनिर्धारित:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		थांबा;
	}
    }

    // add terminating quote and finish with a NUL
    जर (has_spaces)
    {
	साठी (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    परत escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    पूर्णांक
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	जर (s == रिक्त)
	    परत FAIL;
	s = win32_escape_arg(s);
	जर (s == रिक्त)
	    परत FAIL;
	ga_concat(gap, s);
	vim_free(s);
	जर (li->li_next != रिक्त)
	    ga_append(gap, ' ');
    }
    परत OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    शून्य
job_cleanup(job_T *job)
{
    जर (job->jv_status != JOB_ENDED)
	परत;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    जर (job->jv_channel != रिक्त)
	ch_close_part(job->jv_channel, PART_IN);

    जर (job->jv_exit_cb.cb_name != रिक्त)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_संख्या;
	argv[1].vval.v_संख्या = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    जर (job->jv_channel != रिक्त && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    जर (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    पूर्णांक
set_ref_in_job(पूर्णांक copyID)
{
    पूर्णांक		पात = FALSE;
    job_T	*job;
    typval_T	tv;

    साठी (job = first_job; !पात && job != रिक्त; job = job->jv_next)
	जर (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    परत पात;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    शून्य
job_unref(job_T *job)
{
    जर (job == रिक्त || --job->jv_refcount > 0)
	परत;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    जर (job_channel_still_useful(job))
	परत;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    जर (!job_need_end_check(job))
    {
	job_free(job);
    }
    अन्यथा जर (job->jv_channel != रिक्त)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = रिक्त;
	channel_unref(job->jv_channel);
	job->jv_channel = रिक्त;
    }
}

    पूर्णांक
free_unused_jobs_contents(पूर्णांक copyID, पूर्णांक mask)
{
    पूर्णांक		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	जर ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    परत did_free;
}

    शून्य
free_unused_jobs(पूर्णांक copyID, पूर्णांक mask)
{
    job_T	*job;
    job_T	*job_next;

    साठी (job = first_job; job != रिक्त; job = job_next)
    {
	job_next = job->jv_next;
	जर ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(शून्य)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    जर (job == रिक्त)
	परत रिक्त;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    जर (first_job != रिक्त)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    परत job;
}

    शून्य
job_set_options(job_T *job, jobopt_T *opt)
{
    जर (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	जर (opt->jo_stoponexit == रिक्त || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = रिक्त;
	अन्यथा
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    जर (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	जर (opt->jo_exit_cb.cb_name == रिक्त || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = रिक्त;
	    job->jv_exit_cb.cb_partial = रिक्त;
	}
	अन्यथा
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    शून्य
job_stop_on_exit(शून्य)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	जर (job->jv_status == JOB_STARTED && job->jv_stoponexit != रिक्त)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    पूर्णांक
has_pending_job(शून्य)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	जर ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    परत TRUE;
    परत FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    पूर्णांक
job_check_ended(शून्य)
{
    पूर्णांक		i;
    पूर्णांक		did_end = FALSE;

    // be quick if there are no jobs to check
    जर (first_job == रिक्त)
	परत did_end;

    साठी (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	जर (job == रिक्त)
	    थांबा;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    जर (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    परत did_end;
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
	अक्षर	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = रिक्त;
    अक्षर	**argv = रिक्त;
    पूर्णांक		argc = 0;
    पूर्णांक		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    जर (job == रिक्त)
	परत रिक्त;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, आकार(अक्षर*), 20);
#endif

    जर (opt_arg != रिक्त)
	opt = *opt_arg;
    अन्यथा
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	जर (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    जा theend;
    }

    // Check that when io is "file" that there is a file name.
    साठी (part = PART_OUT; part < PART_मोजणी; ++part)
	जर ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    जा theend;
	}

    जर ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = रिक्त;

	// check that we can find the buffer before starting the job
	जर (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    जर (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist),
						 (दीर्घ)opt.jo_io_buf[PART_IN]);
	}
	अन्यथा जर (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	अन्यथा
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	जर (buf == रिक्त)
	    जा theend;
	जर (buf->b_ml.ml_mfp == रिक्त)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    जर (opt.jo_set & JO_IN_BUF)
	    {
		प्र_लिखो((अक्षर *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    अन्यथा
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    जा theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    जर (argv_arg != रिक्त)
    {
	// Make a copy of argv_arg for job->jv_argv.
	साठी (i = 0; argv_arg[i] != रिक्त; i++)
	    argc++;
	argv = ALLOC_MULT(अक्षर *, argc + 1);
	जर (argv == रिक्त)
	    जा theend;
	साठी (i = 0; i < argc; i++)
	    argv[i] = (अक्षर *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = रिक्त;
    }
    अन्यथा
#endif
    जर (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	जर (cmd == रिक्त || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    जा theend;
	}

	जर (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    जा theend;
    }
    अन्यथा जर (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == रिक्त
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	जा theend;
    }
    अन्यथा
    {
	list_T *l = argvars[0].vval.v_list;

	जर (build_argv_from_list(l, &argv, &argc) == FAIL)
	    जा theend;

	// Empty command is invalid.
	जर (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    जा theend;
	}
#ifndef USE_ARGV
	जर (win32_build_cmd(l, &ga) == FAIL)
	    जा theend;
	cmd = ga.ga_data;
	जर (cmd == रिक्त || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    जा theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    जर (term_job != रिक्त)
	*term_job = job;

#ifdef USE_ARGV
    जर (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, आकार(अक्षर), 200);
	साठी (i = 0; i < argc; ++i)
	{
	    जर (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(रिक्त, "Starting job: %s", (अक्षर *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != रिक्त);
#अन्यथा
    ch_log(रिक्त, "Starting job: %s", (अक्षर *)cmd);
    mch_job_start((अक्षर *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    जर (job->jv_channel != रिक्त)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    जर (argv != रिक्त && argv != job->jv_argv)
    {
	साठी (i = 0; argv[i] != रिक्त; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    परत job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    अक्षर *
job_status(job_T *job)
{
    अक्षर	*result;

    जर (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    अन्यथा जर (job->jv_status == JOB_FAILED)
	result = "fail";
    अन्यथा
    {
	result = mch_job_status(job);
	जर (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    परत result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    पूर्णांक
job_stop(job_T *job, typval_T *argvars, अक्षर *type)
{
    char_u *arg;

    जर (type != रिक्त)
	arg = (char_u *)type;
    अन्यथा जर (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    अन्यथा
    {
	arg = tv_get_string_chk(&argvars[1]);
	जर (arg == रिक्त)
	{
	    emsg(_(e_invalid_argument));
	    परत 0;
	}
    }
    जर (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	परत 0;
    }
    जर (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	परत 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (अक्षर *)arg);
    जर (mch_signal_job(job, arg) == FAIL)
	परत 0;

    // Assume that only "kill" will kill the job.
    जर (job->jv_channel != रिक्त && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    परत 1;
}

    शून्य
invoke_prompt_callback(शून्य)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_ओळ_मोजणी;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    जर (curbuf->b_prompt_callback.cb_name == रिक्त
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	परत;
    text = ml_get(lnum);
    prompt = prompt_text();
    जर (STRLEN(text) >= STRLEN(prompt))
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
    पूर्णांक
invoke_prompt_interrupt(शून्य)
{
    typval_T	rettv;
    typval_T	argv[1];
    पूर्णांक		ret;

    जर (curbuf->b_prompt_interrupt.cb_name == रिक्त
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	परत FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    परत ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    स्थिर char_u *
buf_prompt_text(buf_T* buf)
{
    जर (buf->b_prompt_text == रिक्त)
	परत (char_u *)"% ";
    परत buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(शून्य)
{
    परत buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    शून्य
init_prompt(पूर्णांक cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    text = ml_get_curline();
    जर (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	जर (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_ओळ_मोजणी, prompt, TRUE);
	अन्यथा
	    ml_append(curbuf->b_ml.ml_ओळ_मोजणी, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_ओळ_मोजणी, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    जर (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (पूर्णांक)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (पूर्णांक)STRLEN(prompt));

    जर (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    जर (curwin->w_cursor.col < (पूर्णांक)STRLEN(prompt))
	curwin->w_cursor.col = (पूर्णांक)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    पूर्णांक
prompt_curpos_editable(शून्य)
{
    परत curwin->w_cursor.lnum == curbuf->b_ml.ml_ओळ_मोजणी
	&& curwin->w_cursor.col >= (पूर्णांक)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    शून्य
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    जर (check_secure())
	परत;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	परत;

    callback = get_callback(&argvars[1]);
    जर (callback.cb_name == रिक्त)
	परत;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    शून्य
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    जर (check_secure())
	परत;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	परत;

    callback = get_callback(&argvars[1]);
    जर (callback.cb_name == रिक्त)
	परत;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    शून्य
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = tv_get_buf_from_arg(&argvars[0]);
    जर (buf == रिक्त)
	परत;

    जर (!bt_prompt(buf))
	परत;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    शून्य
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    जर (check_secure())
	परत;
    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	परत;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    स्थिर job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    जर (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	परत रिक्त;
    }
    job = tv->vval.v_job;

    जर (job == रिक्त)
	emsg(_(e_not_valid_job));
    परत job;
}

/*
 * "job_getchannel()" function
 */
    शून्य
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    जर (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	परत;

    job = get_job_arg(&argvars[0]);
    जर (job == रिक्त)
	परत;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    जर (job->jv_channel != रिक्त)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    स्थिर शून्य
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    पूर्णांक		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    जर (item == रिक्त)
	परत;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    जर (job->jv_channel != रिक्त)
	++job->jv_channel->ch_refcount;
    जर (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#अन्यथा
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_संख्या(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_संख्या(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    जर (l == रिक्त)
	परत;

    जर (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	परत;
    }
    जर (job->jv_argv != रिक्त)
	साठी (i = 0; job->jv_argv[i] != रिक्त; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    स्थिर शून्य
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	जर (list_append_tv(l, &tv) != OK)
	    परत;
    }
}

/*
 * "job_info()" function
 */
    शून्य
f_job_info(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	जर (job != रिक्त && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    अन्यथा जर (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    शून्य
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    जर (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	परत;

    job = get_job_arg(&argvars[0]);
    जर (job == रिक्त)
	परत;
    clear_job_options(&opt);
    जर (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    शून्य
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    rettv->vval.v_job = job_start(argvars, रिक्त, रिक्त, रिक्त);
}

/*
 * "job_status()" function
 */
    शून्य
f_job_status(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == रिक्त)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    अन्यथा
    {
	job_T	*job = get_job_arg(&argvars[0]);

	जर (job != रिक्त)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    शून्य
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    जर (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_संख्या_arg(argvars, 1) == FAIL))
	परत;

    job = get_job_arg(&argvars[0]);
    जर (job != रिक्त)
	rettv->vval.v_संख्या = job_stop(job, argvars, रिक्त);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    अक्षर  *status;

    जर (job == रिक्त)
    {
	vim_snprintf((अक्षर *)buf, NUMBUFLEN, "no process");
	परत buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((अक्षर *)buf, NUMBUFLEN,
		"process %ld %s", (दीर्घ)job->jv_pid, status);
#किंवाजर defined(MSWIN)
    vim_snprintf((अक्षर *)buf, NUMBUFLEN,
		"process %ld %s",
		(दीर्घ)job->jv_proc_info.dwProcessId,
		status);
#अन्यथा
    // fall-back
    vim_snprintf((अक्षर *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    परत buf;
}

#endif // FEAT_JOB_CHANNEL
