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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    ପାଇଁ ((job) = first_job; (job) != ଖାଲି; (job) = (job)->jv_next)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    ଯଦି (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str), val);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    ଯଦି (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    ଅନ୍ୟଥା ଯଦି (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str), val);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    ଶୂନ୍ୟ
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    ସ୍ଥିର ଶୂନ୍ୟ
unref_job_callback(callback_T *cb)
{
    ଯଦି (cb->cb_partial != ଖାଲି)
	partial_unref(cb->cb_partial);
    ଅନ୍ୟଥା ଯଦି (cb->cb_name != ଖାଲି)
    {
	func_unref(cb->cb_name);
	ଯଦି (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    ଶୂନ୍ୟ
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    ଯଦି (opt->jo_env != ଖାଲି)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
part_from_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_job_options(typval_T *tv, jobopt_T *opt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା supported, ପୂର୍ଣ୍ଣସଂଖ୍ୟା supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ଯଦି (tv->v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ OK;
    ଯଦି (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ଫେରନ୍ତୁ FAIL;
    }
    dict = tv->vval.v_dict;
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&dict->dv_hashtab, hi, todo)
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    ଯଦି (STRCMP(hi->hi_ଚାବି, "mode") == 0)
	    {
		ଯଦି (!(supported & JO_MODE))
		    ଭାଙ୍ଗ;
		ଯଦି (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "in_mode") == 0)
	    {
		ଯଦି (!(supported & JO_IN_MODE))
		    ଭାଙ୍ଗ;
		ଯଦି (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "out_mode") == 0)
	    {
		ଯଦି (!(supported & JO_OUT_MODE))
		    ଭାଙ୍ଗ;
		ଯଦି (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "err_mode") == 0)
	    {
		ଯଦି (!(supported & JO_ERR_MODE))
		    ଭାଙ୍ଗ;
		ଯଦି (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "noblock") == 0)
	    {
		ଯଦି (!(supported & JO_MODE))
		    ଭାଙ୍ଗ;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "in_io") == 0
		    || STRCMP(hi->hi_ଚାବି, "out_io") == 0
		    || STRCMP(hi->hi_ଚାବି, "err_io") == 0)
	    {
		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		ଯଦି (handle_io(item, part_from_char(*hi->hi_ଚାବି), opt) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "in_name") == 0
		    || STRCMP(hi->hi_ଚାବି, "out_name") == 0
		    || STRCMP(hi->hi_ଚାବି, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ଚାବି);

		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "pty") == 0)
	    {
		ଯଦି (!(supported & JO_MODE))
		    ଭାଙ୍ଗ;
		opt->jo_pty = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "in_buf") == 0
		    || STRCMP(hi->hi_ଚାବି, "out_buf") == 0
		    || STRCMP(hi->hi_ଚାବି, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ଚାବି);

		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_ସଂଖ୍ୟା(item);
		ଯଦି (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str),
					      hi->hi_ଚାବି, tv_get_string(item));
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (buflist_findnr(opt->jo_io_buf[part]) == ଖାଲି)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "out_modifiable") == 0
		    || STRCMP(hi->hi_ଚାବି, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ଚାବି);

		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "out_msg") == 0
		    || STRCMP(hi->hi_ଚାବି, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ଚାବି);

		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_OUT_ସନ୍ଦେଶ << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "in_top") == 0
		    || STRCMP(hi->hi_ଚାବି, "in_bot") == 0)
	    {
		linenr_T *lp;

		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		ଯଦି (hi->hi_ଚାବି[3] == 't')
		{
		    lp = &opt->jo_in_ଶୀର୍ଷ;
		    opt->jo_set |= JO_IN_ଶୀର୍ଷ;
		}
		ଅନ୍ୟଥା
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_ସଂଖ୍ୟା(item);
		ଯଦି (*lp < 0)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str),
					      hi->hi_ଚାବି, tv_get_string(item));
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "channel") == 0)
	    {
		ଯଦି (!(supported & JO_OUT_IO))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_CHANNEL;
		ଯଦି (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "channel");
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "callback") == 0)
	    {
		ଯଦି (!(supported & JO_CALLBACK))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		ଯଦି (opt->jo_callback.cb_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "callback");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "out_cb") == 0)
	    {
		ଯଦି (!(supported & JO_OUT_CALLBACK))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		ଯଦି (opt->jo_out_cb.cb_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "out_cb");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "err_cb") == 0)
	    {
		ଯଦି (!(supported & JO_ERR_CALLBACK))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		ଯଦି (opt->jo_err_cb.cb_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "err_cb");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "close_cb") == 0)
	    {
		ଯଦି (!(supported & JO_CLOSE_CALLBACK))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		ଯଦି (opt->jo_close_cb.cb_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "close_cb");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "drop") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା never = FALSE;
		val = tv_get_string(item);

		ଯଦି (STRCMP(val, "never") == 0)
		    never = TRUE;
		ଅନ୍ୟଥା ଯଦି (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "drop", val);
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "exit_cb") == 0)
	    {
		ଯଦି (!(supported & JO_EXIT_CB))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		ଯଦି (opt->jo_exit_cb.cb_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "exit_cb");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_name") == 0)
	    {
		ଯଦି (!(supported2 & JO2_TERM_NAME))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		ଯଦି (opt->jo_term_name == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_name");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_finish") == 0)
	    {
		ଯଦି (!(supported2 & JO2_TERM_FINISH))
		    ଭାଙ୍ଗ;
		val = tv_get_string(item);
		ଯଦି (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str),
							   "term_finish", val);
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_opencmd") == 0)
	    {
		char_u *p;

		ଯଦି (!(supported2 & JO2_TERM_OPENCMD))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		ଯଦି (p != ଖାଲି)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ଯଦି (p != ଖାଲି && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ଖାଲି))
			p = ଖାଲି;
		}
		ଯଦି (p == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_opencmd");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "eof_chars") == 0)
	    {
		ଯଦି (!(supported2 & JO2_EOF_CHARS))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		ଯଦି (opt->jo_eof_chars == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "eof_chars");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_rows") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

		ଯଦି (!(supported2 & JO2_TERM_ROWS))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_ସଂଖ୍ୟା_chk(item, &error);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_rows");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_cols") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

		ଯଦି (!(supported2 & JO2_TERM_COLS))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_ସଂଖ୍ୟା_chk(item, &error);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_cols");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "vertical") == 0)
	    {
		ଯଦି (!(supported2 & JO2_VERTICAL))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "curwin") == 0)
	    {
		ଯଦି (!(supported2 & JO2_CURWIN))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "bufnr") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr;

		ଯଦି (!(supported2 & JO2_CURWIN))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_ସଂଖ୍ୟା(item);
		ଯଦି (nr <= 0)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), hi->hi_ଚାବି, tv_get_string(item));
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ଯଦି (opt->jo_bufnr_buf == ଖାଲି)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == ଖାଲି)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "hidden") == 0)
	    {
		ଯଦି (!(supported2 & JO2_HIDDEN))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "norestore") == 0)
	    {
		ଯଦି (!(supported2 & JO2_NORESTORE))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_kill") == 0)
	    {
		ଯଦି (!(supported2 & JO2_TERM_KILL))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		ଯଦି (opt->jo_term_kill == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_kill");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "tty_type") == 0)
	    {
		char_u *p;

		ଯଦି (!(supported2 & JO2_TTY_TYPE))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		ଯଦି (p == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "tty_type");
		    ଫେରନ୍ତୁ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ଯଦି (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "tty_type");
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "ansi_colors") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ଯଦି (!(supported2 & JO2_ANSI_COLORS))
		    ଭାଙ୍ଗ;

		ଯଦି (item == ଖାଲି || item->v_type != VAR_LIST
			|| item->vval.v_list == ଖାଲି
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "ansi_colors");
		    ଫେରନ୍ତୁ FAIL;
		}

		li = item->vval.v_list->lv_first;
		ପାଇଁ (; li != ଖାଲି && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ଯଦି (color_name == ଖାଲି)
			ଫେରନ୍ତୁ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ଯଦି (guicolor == INVALCOLOR)
		    {
			ଯଦି (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ଫେରନ୍ତୁ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ଯଦି (n != 16 || li != ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "ansi_colors");
		    ଫେରନ୍ତୁ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, ଆକାର(rgb));
	    }
# endif
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_highlight") == 0)
	    {
		char_u *p;

		ଯଦି (!(supported2 & JO2_TERM_HIGHLIGHT))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		ଯଦି (p == ଖାଲି || *p == NUL)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_highlight");
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "term_api") == 0)
	    {
		ଯଦି (!(supported2 & JO2_TERM_API))
		    ଭାଙ୍ଗ;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		ଯଦି (opt->jo_term_api == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "term_api");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
#endif
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "env") == 0)
	    {
		ଯଦି (!(supported2 & JO2_ENV))
		    ଭାଙ୍ଗ;
		ଯଦି (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "env");
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		ଯଦି (opt->jo_env != ଖାଲି)
		    ++opt->jo_env->dv_refcount;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "cwd") == 0)
	    {
		ଯଦି (!(supported2 & JO2_CWD))
		    ଭାଙ୍ଗ;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		ଯଦି (opt->jo_cwd == ଖାଲି || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((ଅକ୍ଷର *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "cwd");
		    ଫେରନ୍ତୁ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "waittime") == 0)
	    {
		ଯଦି (!(supported & JO_WAITTIME))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "timeout") == 0)
	    {
		ଯଦି (!(supported & JO_TIMEOUT))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "out_timeout") == 0)
	    {
		ଯଦି (!(supported & JO_OUT_TIMEOUT))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "err_timeout") == 0)
	    {
		ଯଦି (!(supported & JO_ERR_TIMEOUT))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "part") == 0)
	    {
		ଯଦି (!(supported & JO_PART))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		ଯଦି (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ଅନ୍ୟଥା ଯଦି (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		ଅନ୍ୟଥା
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "part", val);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "id") == 0)
	    {
		ଯଦି (!(supported & JO_ID))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "stoponexit") == 0)
	    {
		ଯଦି (!(supported & JO_STOPONEXIT))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		ଯଦି (opt->jo_stoponexit == ଖାଲି)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "stoponexit");
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hi->hi_ଚାବି, "block_write") == 0)
	    {
		ଯଦି (!(supported & JO_BLOCK_WRITE))
		    ଭାଙ୍ଗ;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_ସଂଖ୍ୟା(item);
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    --todo;
	}
    ଯଦି (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_ଚାବି);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର job_T *first_job = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
job_free_contents(job_T *job)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ch_log(job->jv_channel, "Freeing job");
    ଯଦି (job->jv_channel != ଖାଲି)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = ଖାଲି;
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
    ଯଦି (job->jv_argv != ଖାଲି)
    {
	ପାଇଁ (i = 0; job->jv_argv[i] != ଖାଲି; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
job_unlink(job_T *job)
{
    ଯଦି (job->jv_next != ଖାଲି)
	job->jv_next->jv_prev = job->jv_prev;
    ଯଦି (job->jv_prev == ଖାଲି)
	first_job = job->jv_next;
    ଅନ୍ୟଥା
	job->jv_prev->jv_next = job->jv_next;
}

    ସ୍ଥିର ଶୂନ୍ୟ
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    ସ୍ଥିର ଶୂନ୍ୟ
job_free(job_T *job)
{
    ଯଦି (in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
	ଫେରନ୍ତୁ;

    job_free_contents(job);
    job_free_job(job);
}

ସ୍ଥିର job_T *jobs_to_free = ଖାଲି;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    ସ୍ଥିର ଶୂନ୍ୟ
free_jobs_to_free_later(ଶୂନ୍ୟ)
{
    job_T *job;

    ଯେତେବେଳେ (jobs_to_free != ଖାଲି)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
job_free_all(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (first_job != ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_need_end_check(job_T *job)
{
    ଫେରନ୍ତୁ job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != ଖାଲି || job->jv_exit_cb.cb_name != ଖାଲି);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_channel_still_useful(job_T *job)
{
    ଫେରନ୍ତୁ job->jv_channel != ଖାଲି && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_channel_can_close(job_T *job)
{
    ଫେରନ୍ତୁ job->jv_channel != ଖାଲି && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_still_useful(job_T *job)
{
    ଫେରନ୍ତୁ job_need_end_check(job) || job_channel_still_useful(job);
}

#ଯଦି defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_any_running(ଶୂନ୍ୟ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ଯଦି (job_still_useful(job))
	{
	    ch_log(ଖାଲି, "GUI not forking because a job is running");
	    ଫେରନ୍ତୁ TRUE;
	}
    ଫେରନ୍ତୁ FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#ଯଦି defined(UNIX)
# define USE_ARGV
#endif

#ଯଦି !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ସ୍ଥିର char_u *
win32_escape_arg(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen, dlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaping = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(arg);
    dlen = slen;
    ପାଇଁ (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ଯଦି (*s == '"' || *s == '\\')
	    ++dlen;
	ଯଦି (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ଯଦି (has_spaces)
	dlen += 2;

    ଯଦି (dlen == slen)
	ଫେରନ୍ତୁ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ଯଦି (escaped_arg == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ଯଦି (has_spaces)
	*d++ = '"';

    ପାଇଁ (s = arg; *s != NUL;)
    {
	ବାଛ (*s)
	{
	    ସ୍ଥିତି '"':
		ପାଇଁ (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '\\':
		escaping++;
		*d++ = *s++;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		ଭାଙ୍ଗ;
	}
    }

    // add terminating quote and finish with a NUL
    ଯଦି (has_spaces)
    {
	ପାଇଁ (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ଫେରନ୍ତୁ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ଯଦି (s == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	s = win32_escape_arg(s);
	ଯଦି (s == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ଯଦି (li->li_next != ଖାଲି)
	    ga_append(gap, ' ');
    }
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    ଶୂନ୍ୟ
job_cleanup(job_T *job)
{
    ଯଦି (job->jv_status != JOB_ENDED)
	ଫେରନ୍ତୁ;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    ଯଦି (job->jv_channel != ଖାଲି)
	ch_close_part(job->jv_channel, PART_IN);

    ଯଦି (job->jv_exit_cb.cb_name != ଖାଲି)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_ସଂଖ୍ୟା;
	argv[1].vval.v_ସଂଖ୍ୟା = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ଯଦି (job->jv_channel != ଖାଲି && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ଯଦି (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_job(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    ପାଇଁ (job = first_job; !abort && job != ଖାଲି; job = job->jv_next)
	ଯଦି (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    ଫେରନ୍ତୁ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    ଶୂନ୍ୟ
job_unref(job_T *job)
{
    ଯଦି (job == ଖାଲି || --job->jv_refcount > 0)
	ଫେରନ୍ତୁ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ଯଦି (job_channel_still_useful(job))
	ଫେରନ୍ତୁ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ଯଦି (!job_need_end_check(job))
    {
	job_free(job);
    }
    ଅନ୍ୟଥା ଯଦି (job->jv_channel != ଖାଲି)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = ଖାଲି;
	channel_unref(job->jv_channel);
	job->jv_channel = ଖାଲି;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
free_unused_jobs_contents(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	ଯଦି ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    ଫେରନ୍ତୁ did_free;
}

    ଶୂନ୍ୟ
free_unused_jobs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask)
{
    job_T	*job;
    job_T	*job_next;

    ପାଇଁ (job = first_job; job != ଖାଲି; job = job_next)
    {
	job_next = job->jv_next;
	ଯଦି ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(ଶୂନ୍ୟ)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    ଯଦି (job == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    ଯଦି (first_job != ଖାଲି)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    ଫେରନ୍ତୁ job;
}

    ଶୂନ୍ୟ
job_set_options(job_T *job, jobopt_T *opt)
{
    ଯଦି (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	ଯଦି (opt->jo_stoponexit == ଖାଲି || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = ଖାଲି;
	ଅନ୍ୟଥା
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ଯଦି (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	ଯଦି (opt->jo_exit_cb.cb_name == ଖାଲି || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = ଖାଲି;
	    job->jv_exit_cb.cb_partial = ଖାଲି;
	}
	ଅନ୍ୟଥା
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    ଶୂନ୍ୟ
job_stop_on_exit(ଶୂନ୍ୟ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ଯଦି (job->jv_status == JOB_STARTED && job->jv_stoponexit != ଖାଲି)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_pending_job(ଶୂନ୍ୟ)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ଯଦି ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_check_ended(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_end = FALSE;

    // be quick if there are no jobs to check
    ଯଦି (first_job == ଖାଲି)
	ଫେରନ୍ତୁ did_end;

    ପାଇଁ (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	ଯଦି (job == ଖାଲି)
	    ଭାଙ୍ଗ;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    ଯଦି (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ଫେରନ୍ତୁ did_end;
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
	ଅକ୍ଷର	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = ଖାଲି;
    ଅକ୍ଷର	**argv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    ଯଦି (job == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର*), 20);
#endif

    ଯଦି (opt_arg != ଖାଲି)
	opt = *opt_arg;
    ଅନ୍ୟଥା
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	ଯଦି (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    ଯାଅ theend;
    }

    // Check that when io is "file" that there is a file name.
    ପାଇଁ (part = PART_OUT; part < PART_ଗଣନା; ++part)
	ଯଦି ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    ଯାଅ theend;
	}

    ଯଦି ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = ଖାଲି;

	// check that we can find the buffer before starting the job
	ଯଦି (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    ଯଦି (buf == ଖାଲି)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	ଅନ୍ୟଥା ଯଦି (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	ଅନ୍ୟଥା
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	ଯଦି (buf == ଖାଲି)
	    ଯାଅ theend;
	ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ଯଦି (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((ଅକ୍ଷର *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    ଅନ୍ୟଥା
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    ଯାଅ theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    ଯଦି (argv_arg != ଖାଲି)
    {
	// Make a copy of argv_arg for job->jv_argv.
	ପାଇଁ (i = 0; argv_arg[i] != ଖାଲି; i++)
	    argc++;
	argv = ALLOC_MULT(ଅକ୍ଷର *, argc + 1);
	ଯଦି (argv == ଖାଲି)
	    ଯାଅ theend;
	ପାଇଁ (i = 0; i < argc; i++)
	    argv[i] = (ଅକ୍ଷର *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ଖାଲି;
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ଯଦି (cmd == ଖାଲି || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ଯାଅ theend;
	}

	ଯଦି (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ଖାଲି
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ theend;
    }
    ଅନ୍ୟଥା
    {
	list_T *l = argvars[0].vval.v_list;

	ଯଦି (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ଯାଅ theend;

	// Empty command is invalid.
	ଯଦି (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ଯାଅ theend;
	}
#ifndef USE_ARGV
	ଯଦି (win32_build_cmd(l, &ga) == FAIL)
	    ଯାଅ theend;
	cmd = ga.ga_data;
	ଯଦି (cmd == ଖାଲି || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ଯାଅ theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    ଯଦି (term_job != ଖାଲି)
	*term_job = job;

#ifdef USE_ARGV
    ଯଦି (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 200);
	ପାଇଁ (i = 0; i < argc; ++i)
	{
	    ଯଦି (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ଖାଲି, "Starting job: %s", (ଅକ୍ଷର *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != ଖାଲି);
#ଅନ୍ୟଥା
    ch_log(ଖାଲି, "Starting job: %s", (ଅକ୍ଷର *)cmd);
    mch_job_start((ଅକ୍ଷର *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    ଯଦି (job->jv_channel != ଖାଲି)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    ଯଦି (argv != ଖାଲି && argv != job->jv_argv)
    {
	ପାଇଁ (i = 0; argv[i] != ଖାଲି; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    ଫେରନ୍ତୁ job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    ଅକ୍ଷର *
job_status(job_T *job)
{
    ଅକ୍ଷର	*result;

    ଯଦି (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    ଅନ୍ୟଥା ଯଦି (job->jv_status == JOB_FAILED)
	result = "fail";
    ଅନ୍ୟଥା
    {
	result = mch_job_status(job);
	ଯଦି (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    ଫେରନ୍ତୁ result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
job_stop(job_T *job, typval_T *argvars, ଅକ୍ଷର *type)
{
    char_u *arg;

    ଯଦି (type != ଖାଲି)
	arg = (char_u *)type;
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ଅନ୍ୟଥା
    {
	arg = tv_get_string_chk(&argvars[1]);
	ଯଦି (arg == ଖାଲି)
	{
	    emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ 0;
	}
    }
    ଯଦି (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	ଫେରନ୍ତୁ 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (ଅକ୍ଷର *)arg);
    ଯଦି (mch_signal_job(job, arg) == FAIL)
	ଫେରନ୍ତୁ 0;

    // Assume that only "kill" will kill the job.
    ଯଦି (job->jv_channel != ଖାଲି && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ଫେରନ୍ତୁ 1;
}

    ଶୂନ୍ୟ
invoke_prompt_callback(ଶୂନ୍ୟ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    ଯଦି (curbuf->b_prompt_callback.cb_name == ଖାଲି
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ଫେରନ୍ତୁ;
    text = ml_get(lnum);
    prompt = prompt_text();
    ଯଦି (STRLEN(text) >= STRLEN(prompt))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
invoke_prompt_interrupt(ଶୂନ୍ୟ)
{
    typval_T	rettv;
    typval_T	argv[1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (curbuf->b_prompt_interrupt.cb_name == ଖାଲି
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	ଫେରନ୍ତୁ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ଫେରନ୍ତୁ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ସ୍ଥିର char_u *
buf_prompt_text(buf_T* buf)
{
    ଯଦି (buf->b_prompt_text == ଖାଲି)
	ଫେରନ୍ତୁ (char_u *)"% ";
    ଫେରନ୍ତୁ buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    ଶୂନ୍ୟ
init_prompt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    text = ml_get_curline();
    ଯଦି (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ଯଦି (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, prompt, TRUE);
	ଅନ୍ୟଥା
	    ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ଯଦି (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prompt));

    ଯଦି (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ଯଦି (curwin->w_cursor.col < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prompt))
	curwin->w_cursor.col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prompt_curpos_editable(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curwin->w_cursor.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	&& curwin->w_cursor.col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    ଶୂନ୍ୟ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    callback = get_callback(&argvars[1]);
    ଯଦି (callback.cb_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    ଶୂନ୍ୟ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    callback = get_callback(&argvars[1]);
    ଯଦି (callback.cb_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    ଶୂନ୍ୟ
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (!bt_prompt(buf))
	ଫେରନ୍ତୁ;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    ଶୂନ୍ୟ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;
    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ସ୍ଥିର job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    ଯଦି (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    job = tv->vval.v_job;

    ଯଦି (job == ଖାଲି)
	emsg(_(e_not_valid_job));
    ଫେରନ୍ତୁ job;
}

/*
 * "job_getchannel()" function
 */
    ଶୂନ୍ୟ
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ଯଦି (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    job = get_job_arg(&argvars[0]);
    ଯଦି (job == ଖାଲି)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    ଯଦି (job->jv_channel != ଖାଲି)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    ଯଦି (job->jv_channel != ଖାଲି)
	++job->jv_channel->ch_refcount;
    ଯଦି (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#ଅନ୍ୟଥା
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_ସଂଖ୍ୟା(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_ସଂଖ୍ୟା(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (job->jv_argv != ଖାଲି)
	ପାଇଁ (i = 0; job->jv_argv[i] != ଖାଲି; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	ଯଦି (list_append_tv(l, &tv) != OK)
	    ଫେରନ୍ତୁ;
    }
}

/*
 * "job_info()" function
 */
    ଶୂନ୍ୟ
f_job_info(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	ଯଦି (job != ଖାଲି && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    ଅନ୍ୟଥା ଯଦି (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    ଶୂନ୍ୟ
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    ଯଦି (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    job = get_job_arg(&argvars[0]);
    ଯଦି (job == ଖାଲି)
	ଫେରନ୍ତୁ;
    clear_job_options(&opt);
    ଯଦି (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    ଶୂନ୍ୟ
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_job = job_start(argvars, ଖାଲି, ଖାଲି, ଖାଲି);
}

/*
 * "job_status()" function
 */
    ଶୂନ୍ୟ
f_job_status(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == ଖାଲି)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ଅନ୍ୟଥା
    {
	job_T	*job = get_job_arg(&argvars[0]);

	ଯଦି (job != ଖାଲି)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    ଶୂନ୍ୟ
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ଯଦି (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    job = get_job_arg(&argvars[0]);
    ଯଦି (job != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = job_stop(job, argvars, ଖାଲି);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    ଅକ୍ଷର  *status;

    ଯଦି (job == ଖାଲି)
    {
	vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN, "no process");
	ଫେରନ୍ତୁ buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#ନଚେତଯଦି defined(MSWIN)
    vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#ଅନ୍ୟଥା
    // fall-back
    vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    ଫେରନ୍ତୁ buf;
}

#endif // FEAT_JOB_CHANNEL
