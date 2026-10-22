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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    വേണ്ടി ((job) = first_job; (job) != ശൂന്യ; (job) = (job)->jv_next)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, പൂർണ്ണസംഖ്യ jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    എങ്കിൽ (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str), val);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    എങ്കിൽ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str), val);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    ശൂന്യം
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    സ്ഥിതം ശൂന്യം
unref_job_callback(callback_T *cb)
{
    എങ്കിൽ (cb->cb_partial != ശൂന്യ)
	partial_unref(cb->cb_partial);
    അല്ലെങ്കിൽ എങ്കിൽ (cb->cb_name != ശൂന്യ)
    {
	func_unref(cb->cb_name);
	എങ്കിൽ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    ശൂന്യം
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    എങ്കിൽ (opt->jo_env != ശൂന്യ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
part_from_char(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    പൂർണ്ണസംഖ്യ
get_job_options(typval_T *tv, jobopt_T *opt, പൂർണ്ണസംഖ്യ supported, പൂർണ്ണസംഖ്യ supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    എങ്കിൽ (tv->v_type == VAR_UNKNOWN)
	തിരികെ OK;
    എങ്കിൽ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	തിരികെ FAIL;
    }
    dict = tv->vval.v_dict;
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ OK;

    todo = (പൂർണ്ണസംഖ്യ)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&dict->dv_hashtab, hi, todo)
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "mode") == 0)
	    {
		എങ്കിൽ (!(supported & JO_MODE))
		    നിർത്തുക;
		എങ്കിൽ (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "in_mode") == 0)
	    {
		എങ്കിൽ (!(supported & JO_IN_MODE))
		    നിർത്തുക;
		എങ്കിൽ (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "out_mode") == 0)
	    {
		എങ്കിൽ (!(supported & JO_OUT_MODE))
		    നിർത്തുക;
		എങ്കിൽ (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "err_mode") == 0)
	    {
		എങ്കിൽ (!(supported & JO_ERR_MODE))
		    നിർത്തുക;
		എങ്കിൽ (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "noblock") == 0)
	    {
		എങ്കിൽ (!(supported & JO_MODE))
		    നിർത്തുക;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "in_io") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "out_io") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "err_io") == 0)
	    {
		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		എങ്കിൽ (handle_io(item, part_from_char(*hi->hi_താക്കോൽ), opt) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "in_name") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "out_name") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_താക്കോൽ);

		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "pty") == 0)
	    {
		എങ്കിൽ (!(supported & JO_MODE))
		    നിർത്തുക;
		opt->jo_pty = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "in_buf") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "out_buf") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_താക്കോൽ);

		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_സംഖ്യ(item);
		എങ്കിൽ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str),
					      hi->hi_താക്കോൽ, tv_get_string(item));
		    തിരികെ FAIL;
		}
		എങ്കിൽ (buflist_findnr(opt->jo_io_buf[part]) == ശൂന്യ)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "out_modifiable") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_താക്കോൽ);

		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "out_msg") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_താക്കോൽ);

		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_OUT_സന്ദേശം << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "in_top") == 0
		    || STRCMP(hi->hi_താക്കോൽ, "in_bot") == 0)
	    {
		linenr_T *lp;

		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		എങ്കിൽ (hi->hi_താക്കോൽ[3] == 't')
		{
		    lp = &opt->jo_in_മുകൾ;
		    opt->jo_set |= JO_IN_മുകൾ;
		}
		അല്ലെങ്കിൽ
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_സംഖ്യ(item);
		എങ്കിൽ (*lp < 0)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str),
					      hi->hi_താക്കോൽ, tv_get_string(item));
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "channel") == 0)
	    {
		എങ്കിൽ (!(supported & JO_OUT_IO))
		    നിർത്തുക;
		opt->jo_set |= JO_CHANNEL;
		എങ്കിൽ (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "channel");
		    തിരികെ FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "callback") == 0)
	    {
		എങ്കിൽ (!(supported & JO_CALLBACK))
		    നിർത്തുക;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		എങ്കിൽ (opt->jo_callback.cb_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "callback");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "out_cb") == 0)
	    {
		എങ്കിൽ (!(supported & JO_OUT_CALLBACK))
		    നിർത്തുക;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		എങ്കിൽ (opt->jo_out_cb.cb_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "out_cb");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "err_cb") == 0)
	    {
		എങ്കിൽ (!(supported & JO_ERR_CALLBACK))
		    നിർത്തുക;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		എങ്കിൽ (opt->jo_err_cb.cb_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "err_cb");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "close_cb") == 0)
	    {
		എങ്കിൽ (!(supported & JO_CLOSE_CALLBACK))
		    നിർത്തുക;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		എങ്കിൽ (opt->jo_close_cb.cb_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "close_cb");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "drop") == 0)
	    {
		പൂർണ്ണസംഖ്യ never = FALSE;
		val = tv_get_string(item);

		എങ്കിൽ (STRCMP(val, "never") == 0)
		    never = TRUE;
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "drop", val);
		    തിരികെ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "exit_cb") == 0)
	    {
		എങ്കിൽ (!(supported & JO_EXIT_CB))
		    നിർത്തുക;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		എങ്കിൽ (opt->jo_exit_cb.cb_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "exit_cb");
		    തിരികെ FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_name") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_TERM_NAME))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		എങ്കിൽ (opt->jo_term_name == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_name");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_finish") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_TERM_FINISH))
		    നിർത്തുക;
		val = tv_get_string(item);
		എങ്കിൽ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str),
							   "term_finish", val);
		    തിരികെ FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_opencmd") == 0)
	    {
		char_u *p;

		എങ്കിൽ (!(supported2 & JO2_TERM_OPENCMD))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		എങ്കിൽ (p != ശൂന്യ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    എങ്കിൽ (p != ശൂന്യ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ശൂന്യ))
			p = ശൂന്യ;
		}
		എങ്കിൽ (p == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_opencmd");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "eof_chars") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_EOF_CHARS))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		എങ്കിൽ (opt->jo_eof_chars == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "eof_chars");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_rows") == 0)
	    {
		പൂർണ്ണസംഖ്യ error = FALSE;

		എങ്കിൽ (!(supported2 & JO2_TERM_ROWS))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_സംഖ്യ_chk(item, &error);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
		എങ്കിൽ (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_rows");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_cols") == 0)
	    {
		പൂർണ്ണസംഖ്യ error = FALSE;

		എങ്കിൽ (!(supported2 & JO2_TERM_COLS))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_സംഖ്യ_chk(item, &error);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
		എങ്കിൽ (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_cols");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "vertical") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_VERTICAL))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "curwin") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_CURWIN))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "bufnr") == 0)
	    {
		പൂർണ്ണസംഖ്യ nr;

		എങ്കിൽ (!(supported2 & JO2_CURWIN))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_സംഖ്യ(item);
		എങ്കിൽ (nr <= 0)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), hi->hi_താക്കോൽ, tv_get_string(item));
		    തിരികെ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		എങ്കിൽ (opt->jo_bufnr_buf == ശൂന്യ)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    തിരികെ FAIL;
		}
		എങ്കിൽ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == ശൂന്യ)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "hidden") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_HIDDEN))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "norestore") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_NORESTORE))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_kill") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_TERM_KILL))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		എങ്കിൽ (opt->jo_term_kill == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_kill");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "tty_type") == 0)
	    {
		char_u *p;

		എങ്കിൽ (!(supported2 & JO2_TTY_TYPE))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		എങ്കിൽ (p == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "tty_type");
		    തിരികെ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		എങ്കിൽ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "tty_type");
		    തിരികെ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "ansi_colors") == 0)
	    {
		പൂർണ്ണസംഖ്യ		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		എങ്കിൽ (!(supported2 & JO2_ANSI_COLORS))
		    നിർത്തുക;

		എങ്കിൽ (item == ശൂന്യ || item->v_type != VAR_LIST
			|| item->vval.v_list == ശൂന്യ
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "ansi_colors");
		    തിരികെ FAIL;
		}

		li = item->vval.v_list->lv_first;
		വേണ്ടി (; li != ശൂന്യ && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    എങ്കിൽ (color_name == ശൂന്യ)
			തിരികെ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    എങ്കിൽ (guicolor == INVALCOLOR)
		    {
			എങ്കിൽ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			തിരികെ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		എങ്കിൽ (n != 16 || li != ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "ansi_colors");
		    തിരികെ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, വലുപ്പം(rgb));
	    }
# endif
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_highlight") == 0)
	    {
		char_u *p;

		എങ്കിൽ (!(supported2 & JO2_TERM_HIGHLIGHT))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		എങ്കിൽ (p == ശൂന്യ || *p == NUL)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_highlight");
		    തിരികെ FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "term_api") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_TERM_API))
		    നിർത്തുക;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		എങ്കിൽ (opt->jo_term_api == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "term_api");
		    തിരികെ FAIL;
		}
	    }
#endif
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "env") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_ENV))
		    നിർത്തുക;
		എങ്കിൽ (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "env");
		    തിരികെ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		എങ്കിൽ (opt->jo_env != ശൂന്യ)
		    ++opt->jo_env->dv_refcount;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "cwd") == 0)
	    {
		എങ്കിൽ (!(supported2 & JO2_CWD))
		    നിർത്തുക;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		എങ്കിൽ (opt->jo_cwd == ശൂന്യ || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((അക്ഷരം *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "cwd");
		    തിരികെ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "waittime") == 0)
	    {
		എങ്കിൽ (!(supported & JO_WAITTIME))
		    നിർത്തുക;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "timeout") == 0)
	    {
		എങ്കിൽ (!(supported & JO_TIMEOUT))
		    നിർത്തുക;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "out_timeout") == 0)
	    {
		എങ്കിൽ (!(supported & JO_OUT_TIMEOUT))
		    നിർത്തുക;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "err_timeout") == 0)
	    {
		എങ്കിൽ (!(supported & JO_ERR_TIMEOUT))
		    നിർത്തുക;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "part") == 0)
	    {
		എങ്കിൽ (!(supported & JO_PART))
		    നിർത്തുക;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		എങ്കിൽ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		അല്ലെങ്കിൽ
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "part", val);
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "id") == 0)
	    {
		എങ്കിൽ (!(supported & JO_ID))
		    നിർത്തുക;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "stoponexit") == 0)
	    {
		എങ്കിൽ (!(supported & JO_STOPONEXIT))
		    നിർത്തുക;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		എങ്കിൽ (opt->jo_stoponexit == ശൂന്യ)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "stoponexit");
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hi->hi_താക്കോൽ, "block_write") == 0)
	    {
		എങ്കിൽ (!(supported & JO_BLOCK_WRITE))
		    നിർത്തുക;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_സംഖ്യ(item);
	    }
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    --todo;
	}
    എങ്കിൽ (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_താക്കോൽ);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

സ്ഥിതം job_T *first_job = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
job_free_contents(job_T *job)
{
    പൂർണ്ണസംഖ്യ		i;

    ch_log(job->jv_channel, "Freeing job");
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = ശൂന്യ;
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
    എങ്കിൽ (job->jv_argv != ശൂന്യ)
    {
	വേണ്ടി (i = 0; job->jv_argv[i] != ശൂന്യ; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    സ്ഥിതം ശൂന്യം
job_unlink(job_T *job)
{
    എങ്കിൽ (job->jv_next != ശൂന്യ)
	job->jv_next->jv_prev = job->jv_prev;
    എങ്കിൽ (job->jv_prev == ശൂന്യ)
	first_job = job->jv_next;
    അല്ലെങ്കിൽ
	job->jv_prev->jv_next = job->jv_next;
}

    സ്ഥിതം ശൂന്യം
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    സ്ഥിതം ശൂന്യം
job_free(job_T *job)
{
    എങ്കിൽ (in_free_unref_ഇനങ്ങൾ)
	തിരികെ;

    job_free_contents(job);
    job_free_job(job);
}

സ്ഥിതം job_T *jobs_to_free = ശൂന്യ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    സ്ഥിതം ശൂന്യം
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    സ്ഥിതം ശൂന്യം
free_jobs_to_free_later(ശൂന്യം)
{
    job_T *job;

    ആകുമ്പോൾ (jobs_to_free != ശൂന്യ)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
job_free_all(ശൂന്യം)
{
    ആകുമ്പോൾ (first_job != ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
job_need_end_check(job_T *job)
{
    തിരികെ job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != ശൂന്യ || job->jv_exit_cb.cb_name != ശൂന്യ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
job_channel_still_useful(job_T *job)
{
    തിരികെ job->jv_channel != ശൂന്യ && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
job_channel_can_close(job_T *job)
{
    തിരികെ job->jv_channel != ശൂന്യ && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
job_still_useful(job_T *job)
{
    തിരികെ job_need_end_check(job) || job_channel_still_useful(job);
}

#എങ്കിൽ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    പൂർണ്ണസംഖ്യ
job_any_running(ശൂന്യം)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	എങ്കിൽ (job_still_useful(job))
	{
	    ch_log(ശൂന്യ, "GUI not forking because a job is running");
	    തിരികെ TRUE;
	}
    തിരികെ FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#എങ്കിൽ defined(UNIX)
# define USE_ARGV
#endif

#എങ്കിൽ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    സ്ഥിതം char_u *
win32_escape_arg(char_u *arg)
{
    പൂർണ്ണസംഖ്യ		slen, dlen;
    പൂർണ്ണസംഖ്യ		escaping = 0;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    പൂർണ്ണസംഖ്യ		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (പൂർണ്ണസംഖ്യ)STRLEN(arg);
    dlen = slen;
    വേണ്ടി (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	എങ്കിൽ (*s == '"' || *s == '\\')
	    ++dlen;
	എങ്കിൽ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    എങ്കിൽ (has_spaces)
	dlen += 2;

    എങ്കിൽ (dlen == slen)
	തിരികെ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    എങ്കിൽ (escaped_arg == ശൂന്യ)
	തിരികെ ശൂന്യ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    എങ്കിൽ (has_spaces)
	*d++ = '"';

    വേണ്ടി (s = arg; *s != NUL;)
    {
	തിരഞ്ഞെടുപ്പ് (*s)
	{
	    അവസ്ഥ '"':
		വേണ്ടി (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		നിർത്തുക;
	    അവസ്ഥ '\\':
		escaping++;
		*d++ = *s++;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		നിർത്തുക;
	}
    }

    // add terminating quote and finish with a NUL
    എങ്കിൽ (has_spaces)
    {
	വേണ്ടി (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    തിരികെ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	എങ്കിൽ (s == ശൂന്യ)
	    തിരികെ FAIL;
	s = win32_escape_arg(s);
	എങ്കിൽ (s == ശൂന്യ)
	    തിരികെ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	എങ്കിൽ (li->li_next != ശൂന്യ)
	    ga_append(gap, ' ');
    }
    തിരികെ OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    ശൂന്യം
job_cleanup(job_T *job)
{
    എങ്കിൽ (job->jv_status != JOB_ENDED)
	തിരികെ;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
	ch_close_part(job->jv_channel, PART_IN);

    എങ്കിൽ (job->jv_exit_cb.cb_name != ശൂന്യ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_സംഖ്യ;
	argv[1].vval.v_സംഖ്യ = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    എങ്കിൽ (job->jv_channel != ശൂന്യ && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    എങ്കിൽ (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_job(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    വേണ്ടി (job = first_job; !abort && job != ശൂന്യ; job = job->jv_next)
	എങ്കിൽ (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    തിരികെ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    ശൂന്യം
job_unref(job_T *job)
{
    എങ്കിൽ (job == ശൂന്യ || --job->jv_refcount > 0)
	തിരികെ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    എങ്കിൽ (job_channel_still_useful(job))
	തിരികെ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    എങ്കിൽ (!job_need_end_check(job))
    {
	job_free(job);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (job->jv_channel != ശൂന്യ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = ശൂന്യ;
	channel_unref(job->jv_channel);
	job->jv_channel = ശൂന്യ;
    }
}

    പൂർണ്ണസംഖ്യ
free_unused_jobs_contents(പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ mask)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	എങ്കിൽ ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    തിരികെ did_free;
}

    ശൂന്യം
free_unused_jobs(പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ mask)
{
    job_T	*job;
    job_T	*job_next;

    വേണ്ടി (job = first_job; job != ശൂന്യ; job = job_next)
    {
	job_next = job->jv_next;
	എങ്കിൽ ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(ശൂന്യം)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    എങ്കിൽ (job == ശൂന്യ)
	തിരികെ ശൂന്യ;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    എങ്കിൽ (first_job != ശൂന്യ)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    തിരികെ job;
}

    ശൂന്യം
job_set_options(job_T *job, jobopt_T *opt)
{
    എങ്കിൽ (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	എങ്കിൽ (opt->jo_stoponexit == ശൂന്യ || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = ശൂന്യ;
	അല്ലെങ്കിൽ
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    എങ്കിൽ (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	എങ്കിൽ (opt->jo_exit_cb.cb_name == ശൂന്യ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = ശൂന്യ;
	    job->jv_exit_cb.cb_partial = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    ശൂന്യം
job_stop_on_exit(ശൂന്യം)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	എങ്കിൽ (job->jv_status == JOB_STARTED && job->jv_stoponexit != ശൂന്യ)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    പൂർണ്ണസംഖ്യ
has_pending_job(ശൂന്യം)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	എങ്കിൽ ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    പൂർണ്ണസംഖ്യ
job_check_ended(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		did_end = FALSE;

    // be quick if there are no jobs to check
    എങ്കിൽ (first_job == ശൂന്യ)
	തിരികെ did_end;

    വേണ്ടി (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	എങ്കിൽ (job == ശൂന്യ)
	    നിർത്തുക;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    എങ്കിൽ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    തിരികെ did_end;
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
	അക്ഷരം	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = ശൂന്യ;
    അക്ഷരം	**argv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argc = 0;
    പൂർണ്ണസംഖ്യ		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    എങ്കിൽ (job == ശൂന്യ)
	തിരികെ ശൂന്യ;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, വലുപ്പം(അക്ഷരം*), 20);
#endif

    എങ്കിൽ (opt_arg != ശൂന്യ)
	opt = *opt_arg;
    അല്ലെങ്കിൽ
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	എങ്കിൽ (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    പോകുക theend;
    }

    // Check that when io is "file" that there is a file name.
    വേണ്ടി (part = PART_OUT; part < PART_എണ്ണം; ++part)
	എങ്കിൽ ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    പോകുക theend;
	}

    എങ്കിൽ ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = ശൂന്യ;

	// check that we can find the buffer before starting the job
	എങ്കിൽ (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    എങ്കിൽ (buf == ശൂന്യ)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	അല്ലെങ്കിൽ
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	എങ്കിൽ (buf == ശൂന്യ)
	    പോകുക theend;
	എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    എങ്കിൽ (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((അക്ഷരം *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    അല്ലെങ്കിൽ
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    പോകുക theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    എങ്കിൽ (argv_arg != ശൂന്യ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	വേണ്ടി (i = 0; argv_arg[i] != ശൂന്യ; i++)
	    argc++;
	argv = ALLOC_MULT(അക്ഷരം *, argc + 1);
	എങ്കിൽ (argv == ശൂന്യ)
	    പോകുക theend;
	വേണ്ടി (i = 0; i < argc; i++)
	    argv[i] = (അക്ഷരം *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	എങ്കിൽ (cmd == ശൂന്യ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    പോകുക theend;
	}

	എങ്കിൽ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ശൂന്യ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	പോകുക theend;
    }
    അല്ലെങ്കിൽ
    {
	list_T *l = argvars[0].vval.v_list;

	എങ്കിൽ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    പോകുക theend;

	// Empty command is invalid.
	എങ്കിൽ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    പോകുക theend;
	}
#ifndef USE_ARGV
	എങ്കിൽ (win32_build_cmd(l, &ga) == FAIL)
	    പോകുക theend;
	cmd = ga.ga_data;
	എങ്കിൽ (cmd == ശൂന്യ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    പോകുക theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    എങ്കിൽ (term_job != ശൂന്യ)
	*term_job = job;

#ifdef USE_ARGV
    എങ്കിൽ (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, വലുപ്പം(അക്ഷരം), 200);
	വേണ്ടി (i = 0; i < argc; ++i)
	{
	    എങ്കിൽ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ശൂന്യ, "Starting job: %s", (അക്ഷരം *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != ശൂന്യ);
#അല്ലെങ്കിൽ
    ch_log(ശൂന്യ, "Starting job: %s", (അക്ഷരം *)cmd);
    mch_job_start((അക്ഷരം *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    എങ്കിൽ (argv != ശൂന്യ && argv != job->jv_argv)
    {
	വേണ്ടി (i = 0; argv[i] != ശൂന്യ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    തിരികെ job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    അക്ഷരം *
job_status(job_T *job)
{
    അക്ഷരം	*result;

    എങ്കിൽ (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    അല്ലെങ്കിൽ എങ്കിൽ (job->jv_status == JOB_FAILED)
	result = "fail";
    അല്ലെങ്കിൽ
    {
	result = mch_job_status(job);
	എങ്കിൽ (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    തിരികെ result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    പൂർണ്ണസംഖ്യ
job_stop(job_T *job, typval_T *argvars, അക്ഷരം *type)
{
    char_u *arg;

    എങ്കിൽ (type != ശൂന്യ)
	arg = (char_u *)type;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    അല്ലെങ്കിൽ
    {
	arg = tv_get_string_chk(&argvars[1]);
	എങ്കിൽ (arg == ശൂന്യ)
	{
	    emsg(_(e_invalid_argument));
	    തിരികെ 0;
	}
    }
    എങ്കിൽ (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	തിരികെ 0;
    }
    എങ്കിൽ (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	തിരികെ 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (അക്ഷരം *)arg);
    എങ്കിൽ (mch_signal_job(job, arg) == FAIL)
	തിരികെ 0;

    // Assume that only "kill" will kill the job.
    എങ്കിൽ (job->jv_channel != ശൂന്യ && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    തിരികെ 1;
}

    ശൂന്യം
invoke_prompt_callback(ശൂന്യം)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_വരി_എണ്ണം;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    എങ്കിൽ (curbuf->b_prompt_callback.cb_name == ശൂന്യ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	തിരികെ;
    text = ml_get(lnum);
    prompt = prompt_text();
    എങ്കിൽ (STRLEN(text) >= STRLEN(prompt))
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
    പൂർണ്ണസംഖ്യ
invoke_prompt_interrupt(ശൂന്യം)
{
    typval_T	rettv;
    typval_T	argv[1];
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (curbuf->b_prompt_interrupt.cb_name == ശൂന്യ
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	തിരികെ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    തിരികെ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    സ്ഥിതം char_u *
buf_prompt_text(buf_T* buf)
{
    എങ്കിൽ (buf->b_prompt_text == ശൂന്യ)
	തിരികെ (char_u *)"% ";
    തിരികെ buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(ശൂന്യം)
{
    തിരികെ buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    ശൂന്യം
init_prompt(പൂർണ്ണസംഖ്യ cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    text = ml_get_curline();
    എങ്കിൽ (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	എങ്കിൽ (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_വരി_എണ്ണം, prompt, TRUE);
	അല്ലെങ്കിൽ
	    ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_വരി_എണ്ണം, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    എങ്കിൽ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (പൂർണ്ണസംഖ്യ)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (പൂർണ്ണസംഖ്യ)STRLEN(prompt));

    എങ്കിൽ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    എങ്കിൽ (curwin->w_cursor.col < (പൂർണ്ണസംഖ്യ)STRLEN(prompt))
	curwin->w_cursor.col = (പൂർണ്ണസംഖ്യ)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    പൂർണ്ണസംഖ്യ
prompt_curpos_editable(ശൂന്യം)
{
    തിരികെ curwin->w_cursor.lnum == curbuf->b_ml.ml_വരി_എണ്ണം
	&& curwin->w_cursor.col >= (പൂർണ്ണസംഖ്യ)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    ശൂന്യം
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    callback = get_callback(&argvars[1]);
    എങ്കിൽ (callback.cb_name == ശൂന്യ)
	തിരികെ;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    ശൂന്യം
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    callback = get_callback(&argvars[1]);
    എങ്കിൽ (callback.cb_name == ശൂന്യ)
	തിരികെ;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    ശൂന്യം
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (!bt_prompt(buf))
	തിരികെ;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    ശൂന്യം
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_secure())
	തിരികെ;
    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    സ്ഥിതം job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    എങ്കിൽ (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	തിരികെ ശൂന്യ;
    }
    job = tv->vval.v_job;

    എങ്കിൽ (job == ശൂന്യ)
	emsg(_(e_not_valid_job));
    തിരികെ job;
}

/*
 * "job_getchannel()" function
 */
    ശൂന്യം
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    എങ്കിൽ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    job = get_job_arg(&argvars[0]);
    എങ്കിൽ (job == ശൂന്യ)
	തിരികെ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    സ്ഥിതം ശൂന്യം
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    പൂർണ്ണസംഖ്യ		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
	++job->jv_channel->ch_refcount;
    എങ്കിൽ (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#അല്ലെങ്കിൽ
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_സംഖ്യ(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_സംഖ്യ(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	തിരികെ;
    }
    എങ്കിൽ (job->jv_argv != ശൂന്യ)
	വേണ്ടി (i = 0; job->jv_argv[i] != ശൂന്യ; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    സ്ഥിതം ശൂന്യം
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	എങ്കിൽ (list_append_tv(l, &tv) != OK)
	    തിരികെ;
    }
}

/*
 * "job_info()" function
 */
    ശൂന്യം
f_job_info(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	എങ്കിൽ (job != ശൂന്യ && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    ശൂന്യം
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    എങ്കിൽ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    job = get_job_arg(&argvars[0]);
    എങ്കിൽ (job == ശൂന്യ)
	തിരികെ;
    clear_job_options(&opt);
    എങ്കിൽ (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    ശൂന്യം
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_job = job_start(argvars, ശൂന്യ, ശൂന്യ, ശൂന്യ);
}

/*
 * "job_status()" function
 */
    ശൂന്യം
f_job_status(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == ശൂന്യ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    അല്ലെങ്കിൽ
    {
	job_T	*job = get_job_arg(&argvars[0]);

	എങ്കിൽ (job != ശൂന്യ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    ശൂന്യം
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    എങ്കിൽ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    job = get_job_arg(&argvars[0]);
    എങ്കിൽ (job != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = job_stop(job, argvars, ശൂന്യ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    അക്ഷരം  *status;

    എങ്കിൽ (job == ശൂന്യ)
    {
	vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN, "no process");
	തിരികെ buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#അല്ലെങ്കിലെങ്കിൽ defined(MSWIN)
    vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#അല്ലെങ്കിൽ
    // fall-back
    vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    തിരികെ buf;
}

#endif // FEAT_JOB_CHANNEL
