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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    ಪ್ರತಿ ((job) = first_job; (job) != ರಿಕ್ತ; (job) = (job)->jv_next)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, ಪೂರ್ಣಾಙ್ಕ jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    ಯದಿ (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str), val);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    ಯದಿ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str), val);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    ಶೂನ್ಯ
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unref_job_callback(callback_T *cb)
{
    ಯದಿ (cb->cb_partial != ರಿಕ್ತ)
	partial_unref(cb->cb_partial);
    ಅನ್ಯಥಾ ಯದಿ (cb->cb_name != ರಿಕ್ತ)
    {
	func_unref(cb->cb_name);
	ಯದಿ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    ಶೂನ್ಯ
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    ಯದಿ (opt->jo_env != ರಿಕ್ತ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
part_from_char(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_job_options(typval_T *tv, jobopt_T *opt, ಪೂರ್ಣಾಙ್ಕ supported, ಪೂರ್ಣಾಙ್ಕ supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ಯದಿ (tv->v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    dict = tv->vval.v_dict;
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    todo = (ಪೂರ್ಣಾಙ್ಕ)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&dict->dv_hashtab, hi, todo)
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "mode") == 0)
	    {
		ಯದಿ (!(supported & JO_MODE))
		    ಭಙ್ಗ;
		ಯದಿ (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "in_mode") == 0)
	    {
		ಯದಿ (!(supported & JO_IN_MODE))
		    ಭಙ್ಗ;
		ಯದಿ (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "out_mode") == 0)
	    {
		ಯದಿ (!(supported & JO_OUT_MODE))
		    ಭಙ್ಗ;
		ಯದಿ (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "err_mode") == 0)
	    {
		ಯದಿ (!(supported & JO_ERR_MODE))
		    ಭಙ್ಗ;
		ಯದಿ (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "noblock") == 0)
	    {
		ಯದಿ (!(supported & JO_MODE))
		    ಭಙ್ಗ;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "in_io") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_io") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_io") == 0)
	    {
		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		ಯದಿ (handle_io(item, part_from_char(*hi->hi_ಕೀಲಿ), opt) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "in_name") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_name") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "pty") == 0)
	    {
		ಯದಿ (!(supported & JO_MODE))
		    ಭಙ್ಗ;
		opt->jo_pty = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "in_buf") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_buf") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_ಸಂಖ್ಯೆ(item);
		ಯದಿ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str),
					      hi->hi_ಕೀಲಿ, tv_get_string(item));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (buflist_findnr(opt->jo_io_buf[part]) == ರಿಕ್ತ)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "out_modifiable") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "out_msg") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_OUT_ಸಂದೇಶ << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "in_top") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "in_bot") == 0)
	    {
		linenr_T *lp;

		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		ಯದಿ (hi->hi_ಕೀಲಿ[3] == 't')
		{
		    lp = &opt->jo_in_ತುದಿ;
		    opt->jo_set |= JO_IN_ತುದಿ;
		}
		ಅನ್ಯಥಾ
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_ಸಂಖ್ಯೆ(item);
		ಯದಿ (*lp < 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str),
					      hi->hi_ಕೀಲಿ, tv_get_string(item));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "channel") == 0)
	    {
		ಯದಿ (!(supported & JO_OUT_IO))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_CHANNEL;
		ಯದಿ (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "channel");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "callback") == 0)
	    {
		ಯದಿ (!(supported & JO_CALLBACK))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		ಯದಿ (opt->jo_callback.cb_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "callback");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "out_cb") == 0)
	    {
		ಯದಿ (!(supported & JO_OUT_CALLBACK))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		ಯದಿ (opt->jo_out_cb.cb_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "out_cb");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "err_cb") == 0)
	    {
		ಯದಿ (!(supported & JO_ERR_CALLBACK))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		ಯದಿ (opt->jo_err_cb.cb_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "err_cb");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "close_cb") == 0)
	    {
		ಯದಿ (!(supported & JO_CLOSE_CALLBACK))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		ಯದಿ (opt->jo_close_cb.cb_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "close_cb");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "drop") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ never = FALSE;
		val = tv_get_string(item);

		ಯದಿ (STRCMP(val, "never") == 0)
		    never = TRUE;
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "drop", val);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "exit_cb") == 0)
	    {
		ಯದಿ (!(supported & JO_EXIT_CB))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		ಯದಿ (opt->jo_exit_cb.cb_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "exit_cb");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_name") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_TERM_NAME))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		ಯದಿ (opt->jo_term_name == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_name");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_finish") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_TERM_FINISH))
		    ಭಙ್ಗ;
		val = tv_get_string(item);
		ಯದಿ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str),
							   "term_finish", val);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_opencmd") == 0)
	    {
		char_u *p;

		ಯದಿ (!(supported2 & JO2_TERM_OPENCMD))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ಯದಿ (p != ರಿಕ್ತ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ರಿಕ್ತ))
			p = ರಿಕ್ತ;
		}
		ಯದಿ (p == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_opencmd");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "eof_chars") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_EOF_CHARS))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		ಯದಿ (opt->jo_eof_chars == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "eof_chars");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_rows") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ error = FALSE;

		ಯದಿ (!(supported2 & JO2_TERM_ROWS))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_ಸಂಖ್ಯೆ_chk(item, &error);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_rows");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_cols") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ error = FALSE;

		ಯದಿ (!(supported2 & JO2_TERM_COLS))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_ಸಂಖ್ಯೆ_chk(item, &error);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_cols");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "vertical") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_VERTICAL))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "curwin") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_CURWIN))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "bufnr") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ nr;

		ಯದಿ (!(supported2 & JO2_CURWIN))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_ಸಂಖ್ಯೆ(item);
		ಯದಿ (nr <= 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), hi->hi_ಕೀಲಿ, tv_get_string(item));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ಯದಿ (opt->jo_bufnr_buf == ರಿಕ್ತ)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "hidden") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_HIDDEN))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "norestore") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_NORESTORE))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_kill") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_TERM_KILL))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		ಯದಿ (opt->jo_term_kill == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_kill");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "tty_type") == 0)
	    {
		char_u *p;

		ಯದಿ (!(supported2 & JO2_TTY_TYPE))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		ಯದಿ (p == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "tty_type");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ಯದಿ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "tty_type");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "ansi_colors") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ಯದಿ (!(supported2 & JO2_ANSI_COLORS))
		    ಭಙ್ಗ;

		ಯದಿ (item == ರಿಕ್ತ || item->v_type != VAR_LIST
			|| item->vval.v_list == ರಿಕ್ತ
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "ansi_colors");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		li = item->vval.v_list->lv_first;
		ಪ್ರತಿ (; li != ರಿಕ್ತ && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ಯದಿ (color_name == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ಯದಿ (guicolor == INVALCOLOR)
		    {
			ಯದಿ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ಯದಿ (n != 16 || li != ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "ansi_colors");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, ಪರಿಮಾಣ(rgb));
	    }
# endif
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_highlight") == 0)
	    {
		char_u *p;

		ಯದಿ (!(supported2 & JO2_TERM_HIGHLIGHT))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_highlight");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "term_api") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_TERM_API))
		    ಭಙ್ಗ;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		ಯದಿ (opt->jo_term_api == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "term_api");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
#endif
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "env") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_ENV))
		    ಭಙ್ಗ;
		ಯದಿ (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "env");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		ಯದಿ (opt->jo_env != ರಿಕ್ತ)
		    ++opt->jo_env->dv_refcount;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "cwd") == 0)
	    {
		ಯದಿ (!(supported2 & JO2_CWD))
		    ಭಙ್ಗ;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		ಯದಿ (opt->jo_cwd == ರಿಕ್ತ || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((ವರ್ಣ *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "cwd");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "waittime") == 0)
	    {
		ಯದಿ (!(supported & JO_WAITTIME))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "timeout") == 0)
	    {
		ಯದಿ (!(supported & JO_TIMEOUT))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "out_timeout") == 0)
	    {
		ಯದಿ (!(supported & JO_OUT_TIMEOUT))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "err_timeout") == 0)
	    {
		ಯದಿ (!(supported & JO_ERR_TIMEOUT))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "part") == 0)
	    {
		ಯದಿ (!(supported & JO_PART))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		ಯದಿ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		ಅನ್ಯಥಾ
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "part", val);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "id") == 0)
	    {
		ಯದಿ (!(supported & JO_ID))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "stoponexit") == 0)
	    {
		ಯದಿ (!(supported & JO_STOPONEXIT))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		ಯದಿ (opt->jo_stoponexit == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "stoponexit");
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hi->hi_ಕೀಲಿ, "block_write") == 0)
	    {
		ಯದಿ (!(supported & JO_BLOCK_WRITE))
		    ಭಙ್ಗ;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_ಸಂಖ್ಯೆ(item);
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    --todo;
	}
    ಯದಿ (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_ಕೀಲಿ);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ job_T *first_job = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
job_free_contents(job_T *job)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ch_log(job->jv_channel, "Freeing job");
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = ರಿಕ್ತ;
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
    ಯದಿ (job->jv_argv != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; job->jv_argv[i] != ರಿಕ್ತ; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
job_unlink(job_T *job)
{
    ಯದಿ (job->jv_next != ರಿಕ್ತ)
	job->jv_next->jv_prev = job->jv_prev;
    ಯದಿ (job->jv_prev == ರಿಕ್ತ)
	first_job = job->jv_next;
    ಅನ್ಯಥಾ
	job->jv_prev->jv_next = job->jv_next;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
job_free(job_T *job)
{
    ಯದಿ (in_free_unref_ವಸ್ತುಗಳು)
	ಪ್ರತ್ಯಾಗಮ;

    job_free_contents(job);
    job_free_job(job);
}

ಸ್ಥಿರ job_T *jobs_to_free = ರಿಕ್ತ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_jobs_to_free_later(ಶೂನ್ಯ)
{
    job_T *job;

    ಯಾವತ್ (jobs_to_free != ರಿಕ್ತ)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
job_free_all(ಶೂನ್ಯ)
{
    ಯಾವತ್ (first_job != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
job_need_end_check(job_T *job)
{
    ಪ್ರತ್ಯಾಗಮ job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != ರಿಕ್ತ || job->jv_exit_cb.cb_name != ರಿಕ್ತ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
job_channel_still_useful(job_T *job)
{
    ಪ್ರತ್ಯಾಗಮ job->jv_channel != ರಿಕ್ತ && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
job_channel_can_close(job_T *job)
{
    ಪ್ರತ್ಯಾಗಮ job->jv_channel != ರಿಕ್ತ && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
job_still_useful(job_T *job)
{
    ಪ್ರತ್ಯಾಗಮ job_need_end_check(job) || job_channel_still_useful(job);
}

#ಯದಿ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    ಪೂರ್ಣಾಙ್ಕ
job_any_running(ಶೂನ್ಯ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ಯದಿ (job_still_useful(job))
	{
	    ch_log(ರಿಕ್ತ, "GUI not forking because a job is running");
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#ಯದಿ defined(UNIX)
# define USE_ARGV
#endif

#ಯದಿ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
win32_escape_arg(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ		slen, dlen;
    ಪೂರ್ಣಾಙ್ಕ		escaping = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    ಪೂರ್ಣಾಙ್ಕ		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(arg);
    dlen = slen;
    ಪ್ರತಿ (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ಯದಿ (*s == '"' || *s == '\\')
	    ++dlen;
	ಯದಿ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ಯದಿ (has_spaces)
	dlen += 2;

    ಯದಿ (dlen == slen)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ಯದಿ (escaped_arg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ಯದಿ (has_spaces)
	*d++ = '"';

    ಪ್ರತಿ (s = arg; *s != NUL;)
    {
	ವಿಕಲ್ಪ (*s)
	{
	    ಪಕ್ಷ '"':
		ಪ್ರತಿ (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '\\':
		escaping++;
		*d++ = *s++;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		ಭಙ್ಗ;
	}
    }

    // add terminating quote and finish with a NUL
    ಯದಿ (has_spaces)
    {
	ಪ್ರತಿ (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ಪ್ರತ್ಯಾಗಮ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ಯದಿ (s == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	s = win32_escape_arg(s);
	ಯದಿ (s == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ಯದಿ (li->li_next != ರಿಕ್ತ)
	    ga_append(gap, ' ');
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    ಶೂನ್ಯ
job_cleanup(job_T *job)
{
    ಯದಿ (job->jv_status != JOB_ENDED)
	ಪ್ರತ್ಯಾಗಮ;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
	ch_close_part(job->jv_channel, PART_IN);

    ಯದಿ (job->jv_exit_cb.cb_name != ರಿಕ್ತ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ಯದಿ (job->jv_channel != ರಿಕ್ತ && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ಯದಿ (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_job(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    ಪ್ರತಿ (job = first_job; !abort && job != ರಿಕ್ತ; job = job->jv_next)
	ಯದಿ (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    ಶೂನ್ಯ
job_unref(job_T *job)
{
    ಯದಿ (job == ರಿಕ್ತ || --job->jv_refcount > 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ಯದಿ (job_channel_still_useful(job))
	ಪ್ರತ್ಯಾಗಮ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ಯದಿ (!job_need_end_check(job))
    {
	job_free(job);
    }
    ಅನ್ಯಥಾ ಯದಿ (job->jv_channel != ರಿಕ್ತ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = ರಿಕ್ತ;
	channel_unref(job->jv_channel);
	job->jv_channel = ರಿಕ್ತ;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
free_unused_jobs_contents(ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ mask)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	ಯದಿ ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಶೂನ್ಯ
free_unused_jobs(ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ mask)
{
    job_T	*job;
    job_T	*job_next;

    ಪ್ರತಿ (job = first_job; job != ರಿಕ್ತ; job = job_next)
    {
	job_next = job->jv_next;
	ಯದಿ ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(ಶೂನ್ಯ)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    ಯದಿ (job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    ಯದಿ (first_job != ರಿಕ್ತ)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    ಪ್ರತ್ಯಾಗಮ job;
}

    ಶೂನ್ಯ
job_set_options(job_T *job, jobopt_T *opt)
{
    ಯದಿ (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	ಯದಿ (opt->jo_stoponexit == ರಿಕ್ತ || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ಯದಿ (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	ಯದಿ (opt->jo_exit_cb.cb_name == ರಿಕ್ತ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = ರಿಕ್ತ;
	    job->jv_exit_cb.cb_partial = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    ಶೂನ್ಯ
job_stop_on_exit(ಶೂನ್ಯ)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ಯದಿ (job->jv_status == JOB_STARTED && job->jv_stoponexit != ರಿಕ್ತ)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_pending_job(ಶೂನ್ಯ)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ಯದಿ ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    ಪೂರ್ಣಾಙ್ಕ
job_check_ended(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		did_end = FALSE;

    // be quick if there are no jobs to check
    ಯದಿ (first_job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ did_end;

    ಪ್ರತಿ (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	ಯದಿ (job == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    ಯದಿ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ಪ್ರತ್ಯಾಗಮ did_end;
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
	ವರ್ಣ	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = ರಿಕ್ತ;
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    ಯದಿ (job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ*), 20);
#endif

    ಯದಿ (opt_arg != ರಿಕ್ತ)
	opt = *opt_arg;
    ಅನ್ಯಥಾ
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	ಯದಿ (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    ಗಚ್ಛ theend;
    }

    // Check that when io is "file" that there is a file name.
    ಪ್ರತಿ (part = PART_OUT; part < PART_ಎಣಿಕೆ; ++part)
	ಯದಿ ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    ಗಚ್ಛ theend;
	}

    ಯದಿ ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = ರಿಕ್ತ;

	// check that we can find the buffer before starting the job
	ಯದಿ (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    ಯದಿ (buf == ರಿಕ್ತ)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	ಅನ್ಯಥಾ ಯದಿ (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	ಅನ್ಯಥಾ
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ಯದಿ (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((ವರ್ಣ *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    ಅನ್ಯಥಾ
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    ಗಚ್ಛ theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    ಯದಿ (argv_arg != ರಿಕ್ತ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	ಪ್ರತಿ (i = 0; argv_arg[i] != ರಿಕ್ತ; i++)
	    argc++;
	argv = ALLOC_MULT(ವರ್ಣ *, argc + 1);
	ಯದಿ (argv == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	ಪ್ರತಿ (i = 0; i < argc; i++)
	    argv[i] = (ವರ್ಣ *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ಯದಿ (cmd == ರಿಕ್ತ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ಗಚ್ಛ theend;
	}

	ಯದಿ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ರಿಕ್ತ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ
    {
	list_T *l = argvars[0].vval.v_list;

	ಯದಿ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ಗಚ್ಛ theend;

	// Empty command is invalid.
	ಯದಿ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ಗಚ್ಛ theend;
	}
#ifndef USE_ARGV
	ಯದಿ (win32_build_cmd(l, &ga) == FAIL)
	    ಗಚ್ಛ theend;
	cmd = ga.ga_data;
	ಯದಿ (cmd == ರಿಕ್ತ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ಗಚ್ಛ theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    ಯದಿ (term_job != ರಿಕ್ತ)
	*term_job = job;

#ifdef USE_ARGV
    ಯದಿ (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 200);
	ಪ್ರತಿ (i = 0; i < argc; ++i)
	{
	    ಯದಿ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ರಿಕ್ತ, "Starting job: %s", (ವರ್ಣ *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != ರಿಕ್ತ);
#ಅನ್ಯಥಾ
    ch_log(ರಿಕ್ತ, "Starting job: %s", (ವರ್ಣ *)cmd);
    mch_job_start((ವರ್ಣ *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    ಯದಿ (argv != ರಿಕ್ತ && argv != job->jv_argv)
    {
	ಪ್ರತಿ (i = 0; argv[i] != ರಿಕ್ತ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    ಪ್ರತ್ಯಾಗಮ job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    ವರ್ಣ *
job_status(job_T *job)
{
    ವರ್ಣ	*result;

    ಯದಿ (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    ಅನ್ಯಥಾ ಯದಿ (job->jv_status == JOB_FAILED)
	result = "fail";
    ಅನ್ಯಥಾ
    {
	result = mch_job_status(job);
	ಯದಿ (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    ಪೂರ್ಣಾಙ್ಕ
job_stop(job_T *job, typval_T *argvars, ವರ್ಣ *type)
{
    char_u *arg;

    ಯದಿ (type != ರಿಕ್ತ)
	arg = (char_u *)type;
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ಅನ್ಯಥಾ
    {
	arg = tv_get_string_chk(&argvars[1]);
	ಯದಿ (arg == ರಿಕ್ತ)
	{
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }
    ಯದಿ (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (ವರ್ಣ *)arg);
    ಯದಿ (mch_signal_job(job, arg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Assume that only "kill" will kill the job.
    ಯದಿ (job->jv_channel != ರಿಕ್ತ && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಶೂನ್ಯ
invoke_prompt_callback(ಶೂನ್ಯ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    ಯದಿ (curbuf->b_prompt_callback.cb_name == ರಿಕ್ತ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ಪ್ರತ್ಯಾಗಮ;
    text = ml_get(lnum);
    prompt = prompt_text();
    ಯದಿ (STRLEN(text) >= STRLEN(prompt))
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
    ಪೂರ್ಣಾಙ್ಕ
invoke_prompt_interrupt(ಶೂನ್ಯ)
{
    typval_T	rettv;
    typval_T	argv[1];
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (curbuf->b_prompt_interrupt.cb_name == ರಿಕ್ತ
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ಪ್ರತ್ಯಾಗಮ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ಸ್ಥಿರ char_u *
buf_prompt_text(buf_T* buf)
{
    ಯದಿ (buf->b_prompt_text == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"% ";
    ಪ್ರತ್ಯಾಗಮ buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    ಶೂನ್ಯ
init_prompt(ಪೂರ್ಣಾಙ್ಕ cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    text = ml_get_curline();
    ಯದಿ (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ಯದಿ (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, prompt, TRUE);
	ಅನ್ಯಥಾ
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ಯದಿ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (ಪೂರ್ಣಾಙ್ಕ)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(prompt));

    ಯದಿ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ಯದಿ (curwin->w_cursor.col < (ಪೂರ್ಣಾಙ್ಕ)STRLEN(prompt))
	curwin->w_cursor.col = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    ಪೂರ್ಣಾಙ್ಕ
prompt_curpos_editable(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	&& curwin->w_cursor.col >= (ಪೂರ್ಣಾಙ್ಕ)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    ಶೂನ್ಯ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    callback = get_callback(&argvars[1]);
    ಯದಿ (callback.cb_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    ಶೂನ್ಯ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    callback = get_callback(&argvars[1]);
    ಯದಿ (callback.cb_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    ಶೂನ್ಯ
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!bt_prompt(buf))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    ಶೂನ್ಯ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;
    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ಸ್ಥಿರ job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    ಯದಿ (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    job = tv->vval.v_job;

    ಯದಿ (job == ರಿಕ್ತ)
	emsg(_(e_not_valid_job));
    ಪ್ರತ್ಯಾಗಮ job;
}

/*
 * "job_getchannel()" function
 */
    ಶೂನ್ಯ
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ಯದಿ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    job = get_job_arg(&argvars[0]);
    ಯದಿ (job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
	++job->jv_channel->ch_refcount;
    ಯದಿ (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#ಅನ್ಯಥಾ
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_ಸಂಖ್ಯೆ(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_ಸಂಖ್ಯೆ(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (job->jv_argv != ರಿಕ್ತ)
	ಪ್ರತಿ (i = 0; job->jv_argv[i] != ರಿಕ್ತ; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	ಯದಿ (list_append_tv(l, &tv) != OK)
	    ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * "job_info()" function
 */
    ಶೂನ್ಯ
f_job_info(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	ಯದಿ (job != ರಿಕ್ತ && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    ಅನ್ಯಥಾ ಯದಿ (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    ಶೂನ್ಯ
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    ಯದಿ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    job = get_job_arg(&argvars[0]);
    ಯದಿ (job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    clear_job_options(&opt);
    ಯದಿ (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    ಶೂನ್ಯ
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_job = job_start(argvars, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * "job_status()" function
 */
    ಶೂನ್ಯ
f_job_status(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == ರಿಕ್ತ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ಅನ್ಯಥಾ
    {
	job_T	*job = get_job_arg(&argvars[0]);

	ಯದಿ (job != ರಿಕ್ತ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    ಶೂನ್ಯ
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ಯದಿ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    job = get_job_arg(&argvars[0]);
    ಯದಿ (job != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = job_stop(job, argvars, ರಿಕ್ತ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    ವರ್ಣ  *status;

    ಯದಿ (job == ರಿಕ್ತ)
    {
	vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN, "no process");
	ಪ್ರತ್ಯಾಗಮ buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#ನೋಚೇದ್ಯದಿ defined(MSWIN)
    vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#ಅನ್ಯಥಾ
    // fall-back
    vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    ಪ್ರತ್ಯಾಗಮ buf;
}

#endif // FEAT_JOB_CHANNEL
