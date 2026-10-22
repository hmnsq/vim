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

#include "vim.h"

#ᬬᬾᬦ᭄ defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    for ((job) = first_job; (job) != NULL; (job) = (job)->jv_next)

    static puur_naa_nka
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, puur_naa_nka jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    ᬬᬾᬦ᭄ (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    ᬮᬶᬬ
    {
	semsg(_(e_invalid_argument_str), val);
	ᬯᬮᬶ FAIL;
    }
    ᬯᬮᬶ OK;
}

    static puur_naa_nka
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    ᬬᬾᬦ᭄ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    ᬮᬶᬬ
    {
	semsg(_(e_invalid_argument_str), val);
	ᬯᬮᬶ FAIL;
    }
    ᬯᬮᬶ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    void
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    static void
unref_job_callback(callback_T *cb)
{
    ᬬᬾᬦ᭄ (cb->cb_partial != NULL)
	partial_unref(cb->cb_partial);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (cb->cb_name != NULL)
    {
	func_unref(cb->cb_name);
	ᬬᬾᬦ᭄ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    void
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    ᬬᬾᬦ᭄ (opt->jo_env != NULL)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    static puur_naa_nka
part_from_char(puur_naa_nka c)
{
    ᬯᬮᬶ c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    puur_naa_nka
get_job_options(typval_T *tv, jobopt_T *opt, puur_naa_nka supported, puur_naa_nka supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    puur_naa_nka		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ᬬᬾᬦ᭄ (tv->v_type == VAR_UNKNOWN)
	ᬯᬮᬶ OK;
    ᬬᬾᬦ᭄ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ᬯᬮᬶ FAIL;
    }
    dict = tv->vval.v_dict;
    ᬬᬾᬦ᭄ (dict == NULL)
	ᬯᬮᬶ OK;

    todo = (puur_naa_nka)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&dict->dv_hashtab, hi, todo)
	ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "mode") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_MODE))
		    bha_nga;
		ᬬᬾᬦ᭄ (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "in_mode") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_IN_MODE))
		    bha_nga;
		ᬬᬾᬦ᭄ (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "out_mode") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_OUT_MODE))
		    bha_nga;
		ᬬᬾᬦ᭄ (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "err_mode") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_ERR_MODE))
		    bha_nga;
		ᬬᬾᬦ᭄ (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "noblock") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_MODE))
		    bha_nga;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "in_io") == 0
		    || STRCMP(hi->hi_key, "out_io") == 0
		    || STRCMP(hi->hi_key, "err_io") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		ᬬᬾᬦ᭄ (handle_io(item, part_from_char(*hi->hi_key), opt) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "in_name") == 0
		    || STRCMP(hi->hi_key, "out_name") == 0
		    || STRCMP(hi->hi_key, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "pty") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_MODE))
		    bha_nga;
		opt->jo_pty = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "in_buf") == 0
		    || STRCMP(hi->hi_key, "out_buf") == 0
		    || STRCMP(hi->hi_key, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_number(item);
		ᬬᬾᬦ᭄ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
					      hi->hi_key, tv_get_string(item));
		    ᬯᬮᬶ FAIL;
		}
		ᬬᬾᬦ᭄ (buflist_findnr(opt->jo_io_buf[part]) == NULL)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "out_modifiable") == 0
		    || STRCMP(hi->hi_key, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "out_msg") == 0
		    || STRCMP(hi->hi_key, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		opt->jo_set2 |= JO2_OUT_MSG << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "in_top") == 0
		    || STRCMP(hi->hi_key, "in_bot") == 0)
	    {
		linenr_T *lp;

		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		ᬬᬾᬦ᭄ (hi->hi_key[3] == 't')
		{
		    lp = &opt->jo_in_top;
		    opt->jo_set |= JO_IN_TOP;
		}
		ᬮᬶᬬ
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_number(item);
		ᬬᬾᬦ᭄ (*lp < 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
					      hi->hi_key, tv_get_string(item));
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "channel") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_OUT_IO))
		    bha_nga;
		opt->jo_set |= JO_CHANNEL;
		ᬬᬾᬦ᭄ (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "channel");
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "callback") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_CALLBACK))
		    bha_nga;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		ᬬᬾᬦ᭄ (opt->jo_callback.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "callback");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "out_cb") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_OUT_CALLBACK))
		    bha_nga;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		ᬬᬾᬦ᭄ (opt->jo_out_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "out_cb");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "err_cb") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_ERR_CALLBACK))
		    bha_nga;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		ᬬᬾᬦ᭄ (opt->jo_err_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "err_cb");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "close_cb") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_CLOSE_CALLBACK))
		    bha_nga;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		ᬬᬾᬦ᭄ (opt->jo_close_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "close_cb");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "drop") == 0)
	    {
		puur_naa_nka never = FALSE;
		val = tv_get_string(item);

		ᬬᬾᬦ᭄ (STRCMP(val, "never") == 0)
		    never = TRUE;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str), "drop", val);
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "exit_cb") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_EXIT_CB))
		    bha_nga;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		ᬬᬾᬦ᭄ (opt->jo_exit_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "exit_cb");
		    ᬯᬮᬶ FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_name") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_NAME))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		ᬬᬾᬦ᭄ (opt->jo_term_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_name");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_finish") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_FINISH))
		    bha_nga;
		val = tv_get_string(item);
		ᬬᬾᬦ᭄ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
							   "term_finish", val);
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_opencmd") == 0)
	    {
		char_u *p;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_OPENCMD))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		ᬬᬾᬦ᭄ (p != NULL)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ᬬᬾᬦ᭄ (p != NULL && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != NULL))
			p = NULL;
		}
		ᬬᬾᬦ᭄ (p == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_opencmd");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "eof_chars") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_EOF_CHARS))
		    bha_nga;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		ᬬᬾᬦ᭄ (opt->jo_eof_chars == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "eof_chars");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_rows") == 0)
	    {
		puur_naa_nka error = FALSE;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_ROWS))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_number_chk(item, &error);
		ᬬᬾᬦ᭄ (error)
		    ᬯᬮᬶ FAIL;
		ᬬᬾᬦ᭄ (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_rows");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_cols") == 0)
	    {
		puur_naa_nka error = FALSE;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_COLS))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_number_chk(item, &error);
		ᬬᬾᬦ᭄ (error)
		    ᬯᬮᬶ FAIL;
		ᬬᬾᬦ᭄ (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_cols");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "vertical") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_VERTICAL))
		    bha_nga;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "curwin") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_CURWIN))
		    bha_nga;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "bufnr") == 0)
	    {
		puur_naa_nka nr;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_CURWIN))
		    bha_nga;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_number(item);
		ᬬᬾᬦ᭄ (nr <= 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str), hi->hi_key, tv_get_string(item));
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ᬬᬾᬦ᭄ (opt->jo_bufnr_buf == NULL)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    ᬯᬮᬶ FAIL;
		}
		ᬬᬾᬦ᭄ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == NULL)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "hidden") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_HIDDEN))
		    bha_nga;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "norestore") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_NORESTORE))
		    bha_nga;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_kill") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_KILL))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		ᬬᬾᬦ᭄ (opt->jo_term_kill == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_kill");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "tty_type") == 0)
	    {
		char_u *p;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_TTY_TYPE))
		    bha_nga;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		ᬬᬾᬦ᭄ (p == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "tty_type");
		    ᬯᬮᬶ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ᬬᬾᬦ᭄ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_value_for_argument_str), "tty_type");
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ᬬᬾᬦ᭄ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "ansi_colors") == 0)
	    {
		puur_naa_nka		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ᬬᬾᬦ᭄ (!(supported2 & JO2_ANSI_COLORS))
		    bha_nga;

		ᬬᬾᬦ᭄ (item == NULL || item->v_type != VAR_LIST
			|| item->vval.v_list == NULL
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_value_for_argument_str), "ansi_colors");
		    ᬯᬮᬶ FAIL;
		}

		li = item->vval.v_list->lv_first;
		for (; li != NULL && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    puur_naa_nka		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ᬬᬾᬦ᭄ (color_name == NULL)
			ᬯᬮᬶ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ᬬᬾᬦ᭄ (guicolor == INVALCOLOR)
		    {
			ᬬᬾᬦ᭄ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ᬯᬮᬶ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ᬬᬾᬦ᭄ (n != 16 || li != NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "ansi_colors");
		    ᬯᬮᬶ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, parimaa_na(rgb));
	    }
# endif
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_highlight") == 0)
	    {
		char_u *p;

		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_HIGHLIGHT))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		ᬬᬾᬦ᭄ (p == NULL || *p == NUL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_highlight");
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "term_api") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_TERM_API))
		    bha_nga;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		ᬬᬾᬦ᭄ (opt->jo_term_api == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_api");
		    ᬯᬮᬶ FAIL;
		}
	    }
#endif
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "env") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_ENV))
		    bha_nga;
		ᬬᬾᬦ᭄ (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_value_for_argument_str), "env");
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		ᬬᬾᬦ᭄ (opt->jo_env != NULL)
		    ++opt->jo_env->dv_refcount;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "cwd") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported2 & JO2_CWD))
		    bha_nga;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		ᬬᬾᬦ᭄ (opt->jo_cwd == NULL || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((var_na *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_value_for_argument_str), "cwd");
		    ᬯᬮᬶ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "waittime") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_WAITTIME))
		    bha_nga;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_number(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "timeout") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_TIMEOUT))
		    bha_nga;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_number(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "out_timeout") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_OUT_TIMEOUT))
		    bha_nga;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_number(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "err_timeout") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_ERR_TIMEOUT))
		    bha_nga;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_number(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "part") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_PART))
		    bha_nga;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		ᬬᬾᬦ᭄ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		ᬮᬶᬬ
		{
		    semsg(_(e_invalid_value_for_argument_str_str), "part", val);
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "id") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_ID))
		    bha_nga;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_number(item);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "stoponexit") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_STOPONEXIT))
		    bha_nga;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		ᬬᬾᬦ᭄ (opt->jo_stoponexit == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "stoponexit");
		    ᬯᬮᬶ FAIL;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRCMP(hi->hi_key, "block_write") == 0)
	    {
		ᬬᬾᬦ᭄ (!(supported & JO_BLOCK_WRITE))
		    bha_nga;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_number(item);
	    }
	    ᬮᬶᬬ
		bha_nga;
	    --todo;
	}
    ᬬᬾᬦ᭄ (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_key);
	ᬯᬮᬶ FAIL;
    }

    ᬯᬮᬶ OK;
}

static job_T *first_job = NULL;

    static void
job_free_contents(job_T *job)
{
    puur_naa_nka		i;

    ch_log(job->jv_channel, "Freeing job");
    ᬬᬾᬦ᭄ (job->jv_channel != NULL)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = NULL;
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
    ᬬᬾᬦ᭄ (job->jv_argv != NULL)
    {
	for (i = 0; job->jv_argv[i] != NULL; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    static void
job_unlink(job_T *job)
{
    ᬬᬾᬦ᭄ (job->jv_next != NULL)
	job->jv_next->jv_prev = job->jv_prev;
    ᬬᬾᬦ᭄ (job->jv_prev == NULL)
	first_job = job->jv_next;
    ᬮᬶᬬ
	job->jv_prev->jv_next = job->jv_next;
}

    static void
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    static void
job_free(job_T *job)
{
    ᬬᬾᬦ᭄ (in_free_unref_items)
	ᬯᬮᬶ;

    job_free_contents(job);
    job_free_job(job);
}

static job_T *jobs_to_free = NULL;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    static void
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    static void
free_jobs_to_free_later(void)
{
    job_T *job;

    ᬲᭂᬤᭂᬮ (jobs_to_free != NULL)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#ᬬᬾᬦ᭄ defined(EXITFREE)
    void
job_free_all(void)
{
    ᬲᭂᬤᭂᬮ (first_job != NULL)
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
    static puur_naa_nka
job_need_end_check(job_T *job)
{
    ᬯᬮᬶ job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != NULL || job->jv_exit_cb.cb_name != NULL);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    static puur_naa_nka
job_channel_still_useful(job_T *job)
{
    ᬯᬮᬶ job->jv_channel != NULL && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    static puur_naa_nka
job_channel_can_close(job_T *job)
{
    ᬯᬮᬶ job->jv_channel != NULL && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    static puur_naa_nka
job_still_useful(job_T *job)
{
    ᬯᬮᬶ job_need_end_check(job) || job_channel_still_useful(job);
}

#ᬬᬾᬦ᭄ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    puur_naa_nka
job_any_running(void)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ᬬᬾᬦ᭄ (job_still_useful(job))
	{
	    ch_log(NULL, "GUI not forking because a job is running");
	    ᬯᬮᬶ TRUE;
	}
    ᬯᬮᬶ FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#ᬬᬾᬦ᭄ defined(UNIX)
# define USE_ARGV
#endif

#ᬬᬾᬦ᭄ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    static char_u *
win32_escape_arg(char_u *arg)
{
    puur_naa_nka		slen, dlen;
    puur_naa_nka		escaping = 0;
    puur_naa_nka		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    puur_naa_nka		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (puur_naa_nka)STRLEN(arg);
    dlen = slen;
    for (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ᬬᬾᬦ᭄ (*s == '"' || *s == '\\')
	    ++dlen;
	ᬬᬾᬦ᭄ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ᬬᬾᬦ᭄ (has_spaces)
	dlen += 2;

    ᬬᬾᬦ᭄ (dlen == slen)
	ᬯᬮᬶ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ᬬᬾᬦ᭄ (escaped_arg == NULL)
	ᬯᬮᬶ NULL;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ᬬᬾᬦ᭄ (has_spaces)
	*d++ = '"';

    for (s = arg; *s != NUL;)
    {
	switch (*s)
	{
	    pak_sa '"':
		for (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		bha_nga;
	    pak_sa '\\':
		escaping++;
		*d++ = *s++;
		bha_nga;
	    default:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		bha_nga;
	}
    }

    // add terminating quote and finish with a NUL
    ᬬᬾᬦ᭄ (has_spaces)
    {
	for (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ᬯᬮᬶ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ᬬᬾᬦ᭄ (s == NULL)
	    ᬯᬮᬶ FAIL;
	s = win32_escape_arg(s);
	ᬬᬾᬦ᭄ (s == NULL)
	    ᬯᬮᬶ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ᬬᬾᬦ᭄ (li->li_next != NULL)
	    ga_append(gap, ' ');
    }
    ᬯᬮᬶ OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    void
job_cleanup(job_T *job)
{
    ᬬᬾᬦ᭄ (job->jv_status != JOB_ENDED)
	ᬯᬮᬶ;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    ᬬᬾᬦ᭄ (job->jv_channel != NULL)
	ch_close_part(job->jv_channel, PART_IN);

    ᬬᬾᬦ᭄ (job->jv_exit_cb.cb_name != NULL)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ᬬᬾᬦ᭄ (job->jv_channel != NULL && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ᬬᬾᬦ᭄ (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    puur_naa_nka
set_ref_in_job(puur_naa_nka copyID)
{
    puur_naa_nka		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    for (job = first_job; !abort && job != NULL; job = job->jv_next)
	ᬬᬾᬦ᭄ (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    ᬯᬮᬶ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    void
job_unref(job_T *job)
{
    ᬬᬾᬦ᭄ (job == NULL || --job->jv_refcount > 0)
	ᬯᬮᬶ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ᬬᬾᬦ᭄ (job_channel_still_useful(job))
	ᬯᬮᬶ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ᬬᬾᬦ᭄ (!job_need_end_check(job))
    {
	job_free(job);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (job->jv_channel != NULL)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = NULL;
	channel_unref(job->jv_channel);
	job->jv_channel = NULL;
    }
}

    puur_naa_nka
free_unused_jobs_contents(puur_naa_nka copyID, puur_naa_nka mask)
{
    puur_naa_nka		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	ᬬᬾᬦ᭄ ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    ᬯᬮᬶ did_free;
}

    void
free_unused_jobs(puur_naa_nka copyID, puur_naa_nka mask)
{
    job_T	*job;
    job_T	*job_next;

    for (job = first_job; job != NULL; job = job_next)
    {
	job_next = job->jv_next;
	ᬬᬾᬦ᭄ ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(void)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    ᬬᬾᬦ᭄ (job == NULL)
	ᬯᬮᬶ NULL;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    ᬬᬾᬦ᭄ (first_job != NULL)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    ᬯᬮᬶ job;
}

    void
job_set_options(job_T *job, jobopt_T *opt)
{
    ᬬᬾᬦ᭄ (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	ᬬᬾᬦ᭄ (opt->jo_stoponexit == NULL || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = NULL;
	ᬮᬶᬬ
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ᬬᬾᬦ᭄ (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	ᬬᬾᬦ᭄ (opt->jo_exit_cb.cb_name == NULL || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = NULL;
	    job->jv_exit_cb.cb_partial = NULL;
	}
	ᬮᬶᬬ
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    void
job_stop_on_exit(void)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ᬬᬾᬦ᭄ (job->jv_status == JOB_STARTED && job->jv_stoponexit != NULL)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    puur_naa_nka
has_pending_job(void)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ᬬᬾᬦ᭄ ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    puur_naa_nka
job_check_ended(void)
{
    puur_naa_nka		i;
    puur_naa_nka		did_end = FALSE;

    // be quick if there are no jobs to check
    ᬬᬾᬦ᭄ (first_job == NULL)
	ᬯᬮᬶ did_end;

    for (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	ᬬᬾᬦ᭄ (job == NULL)
	    bha_nga;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    ᬬᬾᬦ᭄ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ᬯᬮᬶ did_end;
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
	var_na	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = NULL;
    var_na	**argv = NULL;
    puur_naa_nka		argc = 0;
    puur_naa_nka		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    ᬬᬾᬦ᭄ (job == NULL)
	ᬯᬮᬶ NULL;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, parimaa_na(var_na*), 20);
#endif

    ᬬᬾᬦ᭄ (opt_arg != NULL)
	opt = *opt_arg;
    ᬮᬶᬬ
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	ᬬᬾᬦ᭄ (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    goto theend;
    }

    // Check that when io is "file" that there is a file name.
    for (part = PART_OUT; part < PART_COUNT; ++part)
	ᬬᬾᬦ᭄ ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    goto theend;
	}

    ᬬᬾᬦ᭄ ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = NULL;

	// check that we can find the buffer before starting the job
	ᬬᬾᬦ᭄ (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    ᬬᬾᬦ᭄ (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	ᬮᬶᬬ
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	ᬬᬾᬦ᭄ (buf == NULL)
	    goto theend;
	ᬬᬾᬦ᭄ (buf->b_ml.ml_mfp == NULL)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ᬬᬾᬦ᭄ (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((var_na *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    ᬮᬶᬬ
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    goto theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    ᬬᬾᬦ᭄ (argv_arg != NULL)
    {
	// Make a copy of argv_arg for job->jv_argv.
	for (i = 0; argv_arg[i] != NULL; i++)
	    argc++;
	argv = ALLOC_MULT(var_na *, argc + 1);
	ᬬᬾᬦ᭄ (argv == NULL)
	    goto theend;
	for (i = 0; i < argc; i++)
	    argv[i] = (var_na *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = NULL;
    }
    ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ᬬᬾᬦ᭄ (cmd == NULL || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}

	ᬬᬾᬦ᭄ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    goto theend;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == NULL
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }
    ᬮᬶᬬ
    {
	list_T *l = argvars[0].vval.v_list;

	ᬬᬾᬦ᭄ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    goto theend;

	// Empty command is invalid.
	ᬬᬾᬦ᭄ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}
#ifndef USE_ARGV
	ᬬᬾᬦ᭄ (win32_build_cmd(l, &ga) == FAIL)
	    goto theend;
	cmd = ga.ga_data;
	ᬬᬾᬦ᭄ (cmd == NULL || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    ᬬᬾᬦ᭄ (term_job != NULL)
	*term_job = job;

#ifdef USE_ARGV
    ᬬᬾᬦ᭄ (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, parimaa_na(var_na), 200);
	for (i = 0; i < argc; ++i)
	{
	    ᬬᬾᬦ᭄ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(NULL, "Starting job: %s", (var_na *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != NULL);
#ᬮᬶᬬ
    ch_log(NULL, "Starting job: %s", (var_na *)cmd);
    mch_job_start((var_na *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    ᬬᬾᬦ᭄ (job->jv_channel != NULL)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    ᬬᬾᬦ᭄ (argv != NULL && argv != job->jv_argv)
    {
	for (i = 0; argv[i] != NULL; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    ᬯᬮᬶ job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    var_na *
job_status(job_T *job)
{
    var_na	*result;

    ᬬᬾᬦ᭄ (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    ᬮᬶᬬ ᬬᬾᬦ᭄ (job->jv_status == JOB_FAILED)
	result = "fail";
    ᬮᬶᬬ
    {
	result = mch_job_status(job);
	ᬬᬾᬦ᭄ (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    ᬯᬮᬶ result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    puur_naa_nka
job_stop(job_T *job, typval_T *argvars, var_na *type)
{
    char_u *arg;

    ᬬᬾᬦ᭄ (type != NULL)
	arg = (char_u *)type;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ᬮᬶᬬ
    {
	arg = tv_get_string_chk(&argvars[1]);
	ᬬᬾᬦ᭄ (arg == NULL)
	{
	    emsg(_(e_invalid_argument));
	    ᬯᬮᬶ 0;
	}
    }
    ᬬᬾᬦ᭄ (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄ (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	ᬯᬮᬶ 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (var_na *)arg);
    ᬬᬾᬦ᭄ (mch_signal_job(job, arg) == FAIL)
	ᬯᬮᬶ 0;

    // Assume that only "kill" will kill the job.
    ᬬᬾᬦ᭄ (job->jv_channel != NULL && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ᬯᬮᬶ 1;
}

    void
invoke_prompt_callback(void)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_line_count;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    ᬬᬾᬦ᭄ (curbuf->b_prompt_callback.cb_name == NULL
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ᬯᬮᬶ;
    text = ml_get(lnum);
    prompt = prompt_text();
    ᬬᬾᬦ᭄ (STRLEN(text) >= STRLEN(prompt))
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
    puur_naa_nka
invoke_prompt_interrupt(void)
{
    typval_T	rettv;
    typval_T	argv[1];
    puur_naa_nka		ret;

    ᬬᬾᬦ᭄ (curbuf->b_prompt_interrupt.cb_name == NULL
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	ᬯᬮᬶ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ᬯᬮᬶ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    static char_u *
buf_prompt_text(buf_T* buf)
{
    ᬬᬾᬦ᭄ (buf->b_prompt_text == NULL)
	ᬯᬮᬶ (char_u *)"% ";
    ᬯᬮᬶ buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(void)
{
    ᬯᬮᬶ buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    void
init_prompt(puur_naa_nka cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    text = ml_get_curline();
    ᬬᬾᬦ᭄ (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ᬬᬾᬦ᭄ (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_line_count, prompt, TRUE);
	ᬮᬶᬬ
	    ml_append(curbuf->b_ml.ml_line_count, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_line_count, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ᬬᬾᬦ᭄ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (puur_naa_nka)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (puur_naa_nka)STRLEN(prompt));

    ᬬᬾᬦ᭄ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ᬬᬾᬦ᭄ (curwin->w_cursor.col < (puur_naa_nka)STRLEN(prompt))
	curwin->w_cursor.col = (puur_naa_nka)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    puur_naa_nka
prompt_curpos_editable(void)
{
    ᬯᬮᬶ curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count
	&& curwin->w_cursor.col >= (puur_naa_nka)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    void
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ᬬᬾᬦ᭄ (check_secure())
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ᬬᬾᬦ᭄ (buf == NULL)
	ᬯᬮᬶ;

    callback = get_callback(&argvars[1]);
    ᬬᬾᬦ᭄ (callback.cb_name == NULL)
	ᬯᬮᬶ;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    void
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ᬬᬾᬦ᭄ (check_secure())
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ᬬᬾᬦ᭄ (buf == NULL)
	ᬯᬮᬶ;

    callback = get_callback(&argvars[1]);
    ᬬᬾᬦ᭄ (callback.cb_name == NULL)
	ᬯᬮᬶ;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    void
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ᬬᬾᬦ᭄ (buf == NULL)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (!bt_prompt(buf))
	ᬯᬮᬶ;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    void
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    ᬬᬾᬦ᭄ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (check_secure())
	ᬯᬮᬶ;
    buf = tv_get_buf(&argvars[0], FALSE);
    ᬬᬾᬦ᭄ (buf == NULL)
	ᬯᬮᬶ;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    static job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    ᬬᬾᬦ᭄ (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ᬯᬮᬶ NULL;
    }
    job = tv->vval.v_job;

    ᬬᬾᬦ᭄ (job == NULL)
	emsg(_(e_not_valid_job));
    ᬯᬮᬶ job;
}

/*
 * "job_getchannel()" function
 */
    void
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    job = get_job_arg(&argvars[0]);
    ᬬᬾᬦ᭄ (job == NULL)
	ᬯᬮᬶ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    ᬬᬾᬦ᭄ (job->jv_channel != NULL)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    static void
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    puur_naa_nka		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    ᬬᬾᬦ᭄ (item == NULL)
	ᬯᬮᬶ;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    ᬬᬾᬦ᭄ (job->jv_channel != NULL)
	++job->jv_channel->ch_refcount;
    ᬬᬾᬦ᭄ (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#ᬮᬶᬬ
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_number(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_number(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    ᬬᬾᬦ᭄ (l == NULL)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄ (job->jv_argv != NULL)
	for (i = 0; job->jv_argv[i] != NULL; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    static void
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	ᬬᬾᬦ᭄ (list_append_tv(l, &tv) != OK)
	    ᬯᬮᬶ;
    }
}

/*
 * "job_info()" function
 */
    void
f_job_info(typval_T *argvars, typval_T *rettv)
{
    ᬬᬾᬦ᭄ (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	ᬬᬾᬦ᭄ (job != NULL && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    void
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    ᬬᬾᬦ᭄ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ᬯᬮᬶ;

    job = get_job_arg(&argvars[0]);
    ᬬᬾᬦ᭄ (job == NULL)
	ᬯᬮᬶ;
    clear_job_options(&opt);
    ᬬᬾᬦ᭄ (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    void
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    ᬬᬾᬦ᭄ (check_restricted() || check_secure())
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ᬯᬮᬶ;

    rettv->vval.v_job = job_start(argvars, NULL, NULL, NULL);
}

/*
 * "job_status()" function
 */
    void
f_job_status(typval_T *argvars, typval_T *rettv)
{
    ᬬᬾᬦ᭄ (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == NULL)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ᬮᬶᬬ
    {
	job_T	*job = get_job_arg(&argvars[0]);

	ᬬᬾᬦ᭄ (job != NULL)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    void
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ᬬᬾᬦ᭄ (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ᬯᬮᬶ;

    job = get_job_arg(&argvars[0]);
    ᬬᬾᬦ᭄ (job != NULL)
	rettv->vval.v_number = job_stop(job, argvars, NULL);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    var_na  *status;

    ᬬᬾᬦ᭄ (job == NULL)
    {
	vim_snprintf((var_na *)buf, NUMBUFLEN, "no process");
	ᬯᬮᬶ buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((var_na *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#elif defined(MSWIN)
    vim_snprintf((var_na *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#ᬮᬶᬬ
    // fall-back
    vim_snprintf((var_na *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    ᬯᬮᬶ buf;
}

#endif // FEAT_JOB_CHANNEL
