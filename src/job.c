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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_JOBS(ਕੰਮ) \
    ਲਈ ((ਕੰਮ) = ਪਹਿਲਾ_ਕੰਮ; (ਕੰਮ) != ਨਿਰਾ; (ਕੰਮ) = (ਕੰਮ)->jv_ਅਗਲਾ)

    ਸਥਿਰ ਪੂਰਨ
handle_ਬਹੁਲਕ(typval_T *ਵਸਤੂ, jobopt_T *opt, ch_ਬਹੁਲਕ_T *modep, ਪੂਰਨ jo)
{
    char_u	*val = tv_get_string(ਵਸਤੂ);

    opt->jo_ਸੈੱਟ |= jo;
    ਜੇ (STRCMP(val, "nl") == 0)
	*modep = CH_ਬਹੁਲਕ_NL;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "raw") == 0)
	*modep = CH_ਬਹੁਲਕ_RAW;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "blob") == 0)
	*modep = CH_ਬਹੁਲਕ_BLOB;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "js") == 0)
	*modep = CH_ਬਹੁਲਕ_JS;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "json") == 0)
	*modep = CH_ਬਹੁਲਕ_JSON;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "lsp") == 0)
	*modep = CH_ਬਹੁਲਕ_LSP;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "dap") == 0)
	*modep = CH_ਬਹੁਲਕ_DAP;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), val);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
handle_io(typval_T *ਵਸਤੂ, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(ਵਸਤੂ);

    opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_IO << (part - PART_ਬਾਹਰਲਾ);
    ਜੇ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_ਬਫਰ;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_ਬਾਹਰਲਾ;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), val);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    ਖਾਲੀ
clear_ਕੰਮ_options(jobopt_T *opt)
{
    CLEAR_ਸੂਚਕ(opt);
}

    ਸਥਿਰ ਖਾਲੀ
unref_ਕੰਮ_callback(callback_T *cb)
{
    ਜੇ (cb->cb_partial != ਨਿਰਾ)
	partial_unref(cb->cb_partial);
    ਨਹੀਂਤਾਂ ਜੇ (cb->cb_name != ਨਿਰਾ)
    {
	func_unref(cb->cb_name);
	ਜੇ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    ਖਾਲੀ
free_ਕੰਮ_options(jobopt_T *opt)
{
    unref_ਕੰਮ_callback(&opt->jo_callback);
    unref_ਕੰਮ_callback(&opt->jo_ਬਾਹਰਲਾ_cb);
    unref_ਕੰਮ_callback(&opt->jo_err_cb);
    unref_ਕੰਮ_callback(&opt->jo_close_cb);
    unref_ਕੰਮ_callback(&opt->jo_exit_cb);

    ਜੇ (opt->jo_env != ਨਿਰਾ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ਸਥਿਰ ਪੂਰਨ
part_from_char(ਪੂਰਨ c)
{
    ਵਾਪਸ c == 'i' ? PART_ਵਿੱਚ : c == 'o' ? PART_ਬਾਹਰਲਾ: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    ਪੂਰਨ
get_ਕੰਮ_options(typval_T *tv, jobopt_T *opt, ਪੂਰਨ supported, ਪੂਰਨ supported2)
{
    typval_T	*ਵਸਤੂ;
    char_u	*val;
    dict_T	*dict;
    ਪੂਰਨ		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ਜੇ (tv->v_type == VAR_UNKNOWN)
	ਵਾਪਸ OK;
    ਜੇ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ਵਾਪਸ FAIL;
    }
    dict = tv->vval.v_dict;
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ OK;

    todo = (ਪੂਰਨ)dict->dv_hashtab.ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&dict->dv_hashtab, hi, todo)
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਵਸਤੂ = &dict_lookup(hi)->di_tv;

	    ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "mode") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		ਜੇ (handle_ਬਹੁਲਕ(ਵਸਤੂ, opt, &opt->jo_ਬਹੁਲਕ, JO_ਬਹੁਲਕ) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "in_mode") == 0)
	    {
		ਜੇ (!(supported & JO_ਵਿੱਚ_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		ਜੇ (handle_ਬਹੁਲਕ(ਵਸਤੂ, opt, &opt->jo_ਵਿੱਚ_ਬਹੁਲਕ, JO_ਵਿੱਚ_ਬਹੁਲਕ)
								      == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "out_mode") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		ਜੇ (handle_ਬਹੁਲਕ(ਵਸਤੂ, opt, &opt->jo_ਬਾਹਰਲਾ_ਬਹੁਲਕ, JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ)
								      == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "err_mode") == 0)
	    {
		ਜੇ (!(supported & JO_ERR_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		ਜੇ (handle_ਬਹੁਲਕ(ਵਸਤੂ, opt, &opt->jo_err_ਬਹੁਲਕ, JO_ERR_ਬਹੁਲਕ)
								      == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "noblock") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		opt->jo_noblock = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "in_io") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "out_io") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "err_io") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		ਜੇ (handle_io(ਵਸਤੂ, part_from_char(*hi->hi_ਕੁੰਜੀ), opt) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "in_name") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "out_name") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ਕੁੰਜੀ);

		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_NAME << (part - PART_ਬਾਹਰਲਾ);
		opt->jo_io_name[part] = tv_get_string_buf_chk(ਵਸਤੂ,
						   opt->jo_io_name_buf[part]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "pty") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਹੁਲਕ))
		    ਰੋਕੋ;
		opt->jo_pty = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "in_buf") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "out_buf") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ਕੁੰਜੀ);

		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_BUF << (part - PART_ਬਾਹਰਲਾ);
		opt->jo_io_buf[part] = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
		ਜੇ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str),
					      hi->hi_ਕੁੰਜੀ, tv_get_string(ਵਸਤੂ));
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (buflist_findnr(opt->jo_io_buf[part]) == ਨਿਰਾ)
		{
		    semsg(_(e_ਬਫਰ_nr_does_not_exist),
						   (ਲੰਮਾ)opt->jo_io_buf[part]);
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "out_modifiable") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ਕੁੰਜੀ);

		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_MODIFIABLE << (part - PART_ਬਾਹਰਲਾ);
		opt->jo_modifiable[part] = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "out_msg") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ਕੁੰਜੀ);

		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ << (part - PART_ਬਾਹਰਲਾ);
		opt->jo_ਸੁਨੇਹਾ[part] = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "in_top") == 0
		    || STRCMP(hi->hi_ਕੁੰਜੀ, "in_bot") == 0)
	    {
		linenr_T *lp;

		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		ਜੇ (hi->hi_ਕੁੰਜੀ[3] == 't')
		{
		    lp = &opt->jo_ਵਿੱਚ_ਸਿਖਰ;
		    opt->jo_ਸੈੱਟ |= JO_ਵਿੱਚ_ਸਿਖਰ;
		}
		ਨਹੀਂਤਾਂ
		{
		    lp = &opt->jo_ਵਿੱਚ_bot;
		    opt->jo_ਸੈੱਟ |= JO_ਵਿੱਚ_BOT;
		}
		*lp = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
		ਜੇ (*lp < 0)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str),
					      hi->hi_ਕੁੰਜੀ, tv_get_string(ਵਸਤੂ));
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "channel") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_IO))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_CHANNEL;
		ਜੇ (ਵਸਤੂ->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "channel");
		    ਵਾਪਸ FAIL;
		}
		opt->jo_channel = ਵਸਤੂ->vval.v_channel;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "callback") == 0)
	    {
		ਜੇ (!(supported & JO_CALLBACK))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_CALLBACK;
		opt->jo_callback = get_callback(ਵਸਤੂ);
		ਜੇ (opt->jo_callback.cb_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "callback");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "out_cb") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_CALLBACK))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_CALLBACK;
		opt->jo_ਬਾਹਰਲਾ_cb = get_callback(ਵਸਤੂ);
		ਜੇ (opt->jo_ਬਾਹਰਲਾ_cb.cb_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "out_cb");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "err_cb") == 0)
	    {
		ਜੇ (!(supported & JO_ERR_CALLBACK))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(ਵਸਤੂ);
		ਜੇ (opt->jo_err_cb.cb_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "err_cb");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "close_cb") == 0)
	    {
		ਜੇ (!(supported & JO_CLOSE_CALLBACK))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(ਵਸਤੂ);
		ਜੇ (opt->jo_close_cb.cb_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "close_cb");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "drop") == 0)
	    {
		ਪੂਰਨ never = FALSE;
		val = tv_get_string(ਵਸਤੂ);

		ਜੇ (STRCMP(val, "never") == 0)
		    never = TRUE;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "drop", val);
		    ਵਾਪਸ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "exit_cb") == 0)
	    {
		ਜੇ (!(supported & JO_EXIT_CB))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(ਵਸਤੂ);
		ਜੇ (opt->jo_exit_cb.cb_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "exit_cb");
		    ਵਾਪਸ FAIL;
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_name") == 0)
	    {
		ਜੇ (!(supported2 & JO2_ਪਦ_NAME))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_NAME;
		opt->jo_ਪਦ_name = tv_get_string_buf_chk(ਵਸਤੂ,
						       opt->jo_ਪਦ_name_buf);
		ਜੇ (opt->jo_ਪਦ_name == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_name");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_finish") == 0)
	    {
		ਜੇ (!(supported2 & JO2_ਪਦ_ਅੰਤ))
		    ਰੋਕੋ;
		val = tv_get_string(ਵਸਤੂ);
		ਜੇ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str),
							   "term_finish", val);
		    ਵਾਪਸ FAIL;
		}
		opt->jo_set2 |= JO2_ਪਦ_ਅੰਤ;
		opt->jo_ਪਦ_ਅੰਤ = *val;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_opencmd") == 0)
	    {
		char_u *p;

		ਜੇ (!(supported2 & JO2_ਪਦ_OPENCMD))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_OPENCMD;
		p = opt->jo_ਪਦ_opencmd = tv_get_string_buf_chk(ਵਸਤੂ,
						    opt->jo_ਪਦ_opencmd_buf);
		ਜੇ (p != ਨਿਰਾ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ਜੇ (p != ਨਿਰਾ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ਨਿਰਾ))
			p = ਨਿਰਾ;
		}
		ਜੇ (p == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_opencmd");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "eof_chars") == 0)
	    {
		ਜੇ (!(supported2 & JO2_EOF_CHARS))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(ਵਸਤੂ,
						       opt->jo_eof_chars_buf);
		ਜੇ (opt->jo_eof_chars == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "eof_chars");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_rows") == 0)
	    {
		ਪੂਰਨ error = FALSE;

		ਜੇ (!(supported2 & JO2_ਪਦ_ਪੰਕਤੀਆਂ))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_ਪੰਕਤੀਆਂ;
		opt->jo_ਪਦ_ਪੰਕਤੀਆਂ = tv_get_ਸੰਖਿਆ_chk(ਵਸਤੂ, &error);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
		ਜੇ (opt->jo_ਪਦ_ਪੰਕਤੀਆਂ < 0 || opt->jo_ਪਦ_ਪੰਕਤੀਆਂ > 1000)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_rows");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_cols") == 0)
	    {
		ਪੂਰਨ error = FALSE;

		ਜੇ (!(supported2 & JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ;
		opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ = tv_get_ਸੰਖਿਆ_chk(ਵਸਤੂ, &error);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
		ਜੇ (opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ < 0 || opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ > 1000)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_cols");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "vertical") == 0)
	    {
		ਜੇ (!(supported2 & JO2_VERTICAL))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "curwin") == 0)
	    {
		ਜੇ (!(supported2 & JO2_CURWIN))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "bufnr") == 0)
	    {
		ਪੂਰਨ nr;

		ਜੇ (!(supported2 & JO2_CURWIN))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
		ਜੇ (nr <= 0)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), hi->hi_ਕੁੰਜੀ, tv_get_string(ਵਸਤੂ));
		    ਵਾਪਸ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ਜੇ (opt->jo_bufnr_buf == ਨਿਰਾ)
		{
		    semsg(_(e_ਬਫਰ_nr_does_not_exist), (ਲੰਮਾ)nr);
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_ਪਦ == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਦਲੀਲ_str), "bufnr");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "hidden") == 0)
	    {
		ਜੇ (!(supported2 & JO2_HIDDEN))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "norestore") == 0)
	    {
		ਜੇ (!(supported2 & JO2_NORESTORE))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_ਪਦ_norestore = tv_get_bool(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_kill") == 0)
	    {
		ਜੇ (!(supported2 & JO2_ਪਦ_KILL))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_KILL;
		opt->jo_ਪਦ_kill = tv_get_string_buf_chk(ਵਸਤੂ,
						       opt->jo_ਪਦ_kill_buf);
		ਜੇ (opt->jo_ਪਦ_kill == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_kill");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "tty_type") == 0)
	    {
		char_u *p;

		ਜੇ (!(supported2 & JO2_TTY_TYPE))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(ਵਸਤੂ);
		ਜੇ (p == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "tty_type");
		    ਵਾਪਸ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ਜੇ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "tty_type");
		    ਵਾਪਸ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "ansi_colors") == 0)
	    {
		ਪੂਰਨ		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ਜੇ (!(supported2 & JO2_ANSI_COLORS))
		    ਰੋਕੋ;

		ਜੇ (ਵਸਤੂ == ਨਿਰਾ || ਵਸਤੂ->v_type != VAR_LIST
			|| ਵਸਤੂ->vval.v_list == ਨਿਰਾ
			|| ਵਸਤੂ->vval.v_list->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "ansi_colors");
		    ਵਾਪਸ FAIL;
		}

		li = ਵਸਤੂ->vval.v_list->lv_ਪਹਿਲਾ;
		ਲਈ (; li != ਨਿਰਾ && n < 16; li = li->li_ਅਗਲਾ, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    ਪੂਰਨ		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ਜੇ (color_name == ਨਿਰਾ)
			ਵਾਪਸ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ਜੇ (guicolor == INVALCOLOR)
		    {
			ਜੇ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ਵਾਪਸ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ਜੇ (n != 16 || li != ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "ansi_colors");
		    ਵਾਪਸ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(opt->jo_ansi_colors, rgb, ਆਕਾਰ(rgb));
	    }
# endif
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_highlight") == 0)
	    {
		char_u *p;

		ਜੇ (!(supported2 & JO2_ਪਦ_HIGHLIGHT))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_HIGHLIGHT;
		p = tv_get_string_buf_chk(ਵਸਤੂ, opt->jo_ਪਦ_highlight_buf);
		ਜੇ (p == ਨਿਰਾ || *p == NUL)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_highlight");
		    ਵਾਪਸ FAIL;
		}
		opt->jo_ਪਦ_highlight = p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "term_api") == 0)
	    {
		ਜੇ (!(supported2 & JO2_ਪਦ_API))
		    ਰੋਕੋ;
		opt->jo_set2 |= JO2_ਪਦ_API;
		opt->jo_ਪਦ_api = tv_get_string_buf_chk(ਵਸਤੂ,
							opt->jo_ਪਦ_api_buf);
		ਜੇ (opt->jo_ਪਦ_api == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "term_api");
		    ਵਾਪਸ FAIL;
		}
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "env") == 0)
	    {
		ਜੇ (!(supported2 & JO2_ENV))
		    ਰੋਕੋ;
		ਜੇ (ਵਸਤੂ->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "env");
		    ਵਾਪਸ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = ਵਸਤੂ->vval.v_dict;
		ਜੇ (opt->jo_env != ਨਿਰਾ)
		    ++opt->jo_env->dv_refcount;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "cwd") == 0)
	    {
		ਜੇ (!(supported2 & JO2_CWD))
		    ਰੋਕੋ;
		opt->jo_cwd = tv_get_string_buf_chk(ਵਸਤੂ, opt->jo_cwd_buf);
		ਜੇ (opt->jo_cwd == ਨਿਰਾ || !mch_isdir(opt->jo_cwd)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((ਅੱਖਰ *)opt->jo_cwd, X_OK) != 0
#ਜੇ_ਅੰਤ
				)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "cwd");
		    ਵਾਪਸ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "waittime") == 0)
	    {
		ਜੇ (!(supported & JO_WAITTIME))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_WAITTIME;
		opt->jo_waittime = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "timeout") == 0)
	    {
		ਜੇ (!(supported & JO_TIMEOUT))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "out_timeout") == 0)
	    {
		ਜੇ (!(supported & JO_ਬਾਹਰਲਾ_TIMEOUT))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_TIMEOUT;
		opt->jo_ਬਾਹਰਲਾ_timeout = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "err_timeout") == 0)
	    {
		ਜੇ (!(supported & JO_ERR_TIMEOUT))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "part") == 0)
	    {
		ਜੇ (!(supported & JO_PART))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_PART;
		val = tv_get_string(ਵਸਤੂ);
		ਜੇ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_ਬਾਹਰਲਾ;
		ਨਹੀਂਤਾਂ
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "part", val);
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "id") == 0)
	    {
		ਜੇ (!(supported & JO_ID))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ID;
		opt->jo_id = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "stoponexit") == 0)
	    {
		ਜੇ (!(supported & JO_STOPONEXIT))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(ਵਸਤੂ,
						      opt->jo_stoponexit_buf);
		ਜੇ (opt->jo_stoponexit == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "stoponexit");
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hi->hi_ਕੁੰਜੀ, "block_write") == 0)
	    {
		ਜੇ (!(supported & JO_ਖੰਡ_WRITE))
		    ਰੋਕੋ;
		opt->jo_ਸੈੱਟ |= JO_ਖੰਡ_WRITE;
		opt->jo_ਖੰਡ_write = tv_get_ਸੰਖਿਆ(ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    --todo;
	}
    ਜੇ (todo > 0)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), hi->hi_ਕੁੰਜੀ);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

ਸਥਿਰ ਕੰਮ_T *ਪਹਿਲਾ_ਕੰਮ = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_free_contents(ਕੰਮ_T *ਕੰਮ)
{
    ਪੂਰਨ		i;

    ch_log(ਕੰਮ->jv_channel, "Freeing job");
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	ਕੰਮ->jv_channel->ch_ਕੰਮ = ਨਿਰਾ;
	channel_unref(ਕੰਮ->jv_channel);
    }
    mch_clear_ਕੰਮ(ਕੰਮ);

    vim_free(ਕੰਮ->jv_tty_ਵਿੱਚ);
    vim_free(ਕੰਮ->jv_tty_ਬਾਹਰਲਾ);
    vim_free(ਕੰਮ->jv_stoponexit);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    vim_free(ਕੰਮ->jv_termsig);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    vim_free(ਕੰਮ->jv_tty_type);
#ਜੇ_ਅੰਤ
    free_callback(&ਕੰਮ->jv_exit_cb);
    ਜੇ (ਕੰਮ->jv_argv != ਨਿਰਾ)
    {
	ਲਈ (i = 0; ਕੰਮ->jv_argv[i] != ਨਿਰਾ; i++)
	    vim_free(ਕੰਮ->jv_argv[i]);
	vim_free(ਕੰਮ->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_unlink(ਕੰਮ_T *ਕੰਮ)
{
    ਜੇ (ਕੰਮ->jv_ਅਗਲਾ != ਨਿਰਾ)
	ਕੰਮ->jv_ਅਗਲਾ->jv_prev = ਕੰਮ->jv_prev;
    ਜੇ (ਕੰਮ->jv_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_ਕੰਮ = ਕੰਮ->jv_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	ਕੰਮ->jv_prev->jv_ਅਗਲਾ = ਕੰਮ->jv_ਅਗਲਾ;
}

    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_free_ਕੰਮ(ਕੰਮ_T *ਕੰਮ)
{
    ਕੰਮ_unlink(ਕੰਮ);
    vim_free(ਕੰਮ);
}

    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_free(ਕੰਮ_T *ਕੰਮ)
{
    ਜੇ (ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
	ਵਾਪਸ;

    ਕੰਮ_free_contents(ਕੰਮ);
    ਕੰਮ_free_ਕੰਮ(ਕੰਮ);
}

ਸਥਿਰ ਕੰਮ_T *jobs_ਮੰਜ਼ਿਲ_free = ਨਿਰਾ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_free_later(ਕੰਮ_T *ਕੰਮ)
{
    ਕੰਮ_unlink(ਕੰਮ);
    ਕੰਮ->jv_ਅਗਲਾ = jobs_ਮੰਜ਼ਿਲ_free;
    jobs_ਮੰਜ਼ਿਲ_free = ਕੰਮ;
}

    ਸਥਿਰ ਖਾਲੀ
free_jobs_ਮੰਜ਼ਿਲ_free_later(ਖਾਲੀ)
{
    ਕੰਮ_T *ਕੰਮ;

    ਜਦੋਂ (jobs_ਮੰਜ਼ਿਲ_free != ਨਿਰਾ)
    {
	ਕੰਮ = jobs_ਮੰਜ਼ਿਲ_free;
	jobs_ਮੰਜ਼ਿਲ_free = ਕੰਮ->jv_ਅਗਲਾ;
	ਕੰਮ_free_contents(ਕੰਮ);
	vim_free(ਕੰਮ);
    }
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
ਕੰਮ_free_all(ਖਾਲੀ)
{
    ਜਦੋਂ (ਪਹਿਲਾ_ਕੰਮ != ਨਿਰਾ)
	ਕੰਮ_free(ਪਹਿਲਾ_ਕੰਮ);
    free_jobs_ਮੰਜ਼ਿਲ_free_later();

# ifdef FEAT_TERMINAL
    free_ਅਣਵਰਤਿਆ_terminals();
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ਸਥਿਰ ਪੂਰਨ
ਕੰਮ_need_ਸਿਰਾ_check(ਕੰਮ_T *ਕੰਮ)
{
    ਵਾਪਸ ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_STARTED
	    && (ਕੰਮ->jv_stoponexit != ਨਿਰਾ || ਕੰਮ->jv_exit_cb.cb_name != ਨਿਰਾ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ਸਥਿਰ ਪੂਰਨ
ਕੰਮ_channel_still_useful(ਕੰਮ_T *ਕੰਮ)
{
    ਵਾਪਸ ਕੰਮ->jv_channel != ਨਿਰਾ && channel_still_useful(ਕੰਮ->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ਸਥਿਰ ਪੂਰਨ
ਕੰਮ_channel_can_close(ਕੰਮ_T *ਕੰਮ)
{
    ਵਾਪਸ ਕੰਮ->jv_channel != ਨਿਰਾ && channel_can_close(ਕੰਮ->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ਸਥਿਰ ਪੂਰਨ
ਕੰਮ_still_useful(ਕੰਮ_T *ਕੰਮ)
{
    ਵਾਪਸ ਕੰਮ_need_ਸਿਰਾ_check(ਕੰਮ) || ਕੰਮ_channel_still_useful(ਕੰਮ);
}

#ਜੇ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    ਪੂਰਨ
ਕੰਮ_any_running(ਖਾਲੀ)
{
    ਕੰਮ_T	*ਕੰਮ;

    FOR_ALL_JOBS(ਕੰਮ)
	ਜੇ (ਕੰਮ_still_useful(ਕੰਮ))
	{
	    ch_log(ਨਿਰਾ, "GUI not forking because a job is running");
	    ਵਾਪਸ TRUE;
	}
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

// Unix uses argv[] for the command, other systems use a string.
#ਜੇ defined(UNIX)
# define USE_ARGV
#ਜੇ_ਅੰਤ

#ਜੇ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ਸਥਿਰ char_u *
win32_escape_arg(char_u *arg)
{
    ਪੂਰਨ		slen, dlen;
    ਪੂਰਨ		escaping = 0;
    ਪੂਰਨ		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    ਪੂਰਨ		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (ਪੂਰਨ)STRLEN(arg);
    dlen = slen;
    ਲਈ (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ਜੇ (*s == '"' || *s == '\\')
	    ++dlen;
	ਜੇ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ਜੇ (has_spaces)
	dlen += 2;

    ਜੇ (dlen == slen)
	ਵਾਪਸ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ਜੇ (escaped_arg == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ਜੇ (has_spaces)
	*d++ = '"';

    ਲਈ (s = arg; *s != NUL;)
    {
	ਚੋਣ (*s)
	{
	    ਹਾਲਤ '"':
		ਲਈ (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		ਰੋਕੋ;
	    ਹਾਲਤ '\\':
		escaping++;
		*d++ = *s++;
		ਰੋਕੋ;
	    ਮੂਲ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		ਰੋਕੋ;
	}
    }

    // add terminating quote and finish with a NUL
    ਜੇ (has_spaces)
    {
	ਲਈ (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ਵਾਪਸ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ਜੇ (s == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	s = win32_escape_arg(s);
	ਜੇ (s == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ਜੇ (li->li_ਅਗਲਾ != ਨਿਰਾ)
	    ga_append(gap, ' ');
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    ਖਾਲੀ
ਕੰਮ_cleanup(ਕੰਮ_T *ਕੰਮ)
{
    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_ENDED)
	ਵਾਪਸ;

    // Ready to cleanup the job.
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ਮੁਕੰਮਲ;

    // When only channel-in is kept open, close explicitly.
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
	ch_close_part(ਕੰਮ->jv_channel, PART_ਵਿੱਚ);

    ਜੇ (ਕੰਮ->jv_exit_cb.cb_name != ਨਿਰਾ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(ਕੰਮ->jv_channel, "Invoking exit callback %s",
						      ਕੰਮ->jv_exit_cb.cb_name);
	++ਕੰਮ->jv_refcount;
	argv[0].v_type = VAR_ਕੰਮ;
	argv[0].vval.v_ਕੰਮ = ਕੰਮ;
	argv[1].v_type = VAR_ਸੰਖਿਆ;
	argv[1].vval.v_ਸੰਖਿਆ = ਕੰਮ->jv_exitval;
	call_callback(&ਕੰਮ->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--ਕੰਮ->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ && ਕੰਮ->jv_channel->ch_anonymous_pipe)
	ਕੰਮ->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ਜੇ (ਕੰਮ->jv_refcount == 0 && !ਕੰਮ_channel_still_useful(ਕੰਮ))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	ਕੰਮ_free_later(ਕੰਮ);
}

/*
 * Mark references in jobs that are still useful.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਕੰਮ(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    ਕੰਮ_T	*ਕੰਮ;
    typval_T	tv;

    ਲਈ (ਕੰਮ = ਪਹਿਲਾ_ਕੰਮ; !abort && ਕੰਮ != ਨਿਰਾ; ਕੰਮ = ਕੰਮ->jv_ਅਗਲਾ)
	ਜੇ (ਕੰਮ_still_useful(ਕੰਮ))
	{
	    tv.v_type = VAR_ਕੰਮ;
	    tv.vval.v_ਕੰਮ = ਕੰਮ;
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    ਵਾਪਸ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    ਖਾਲੀ
ਕੰਮ_unref(ਕੰਮ_T *ਕੰਮ)
{
    ਜੇ (ਕੰਮ == ਨਿਰਾ || --ਕੰਮ->jv_refcount > 0)
	ਵਾਪਸ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ਜੇ (ਕੰਮ_channel_still_useful(ਕੰਮ))
	ਵਾਪਸ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ਜੇ (!ਕੰਮ_need_ਸਿਰਾ_check(ਕੰਮ))
    {
	ਕੰਮ_free(ਕੰਮ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(ਕੰਮ->jv_channel, "detaching channel from job");
	ਕੰਮ->jv_channel->ch_ਕੰਮ = ਨਿਰਾ;
	channel_unref(ਕੰਮ->jv_channel);
	ਕੰਮ->jv_channel = ਨਿਰਾ;
    }
}

    ਪੂਰਨ
free_ਅਣਵਰਤਿਆ_jobs_contents(ਪੂਰਨ copyID, ਪੂਰਨ ਨਕਾਬ)
{
    ਪੂਰਨ		did_free = FALSE;
    ਕੰਮ_T	*ਕੰਮ;

    FOR_ALL_JOBS(ਕੰਮ)
	ਜੇ ((ਕੰਮ->jv_copyID & ਨਕਾਬ) != (copyID & ਨਕਾਬ)
						    && !ਕੰਮ_still_useful(ਕੰਮ))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    ਕੰਮ_free_contents(ਕੰਮ);
	    did_free = TRUE;
	}
    ਵਾਪਸ did_free;
}

    ਖਾਲੀ
free_ਅਣਵਰਤਿਆ_jobs(ਪੂਰਨ copyID, ਪੂਰਨ ਨਕਾਬ)
{
    ਕੰਮ_T	*ਕੰਮ;
    ਕੰਮ_T	*ਕੰਮ_ਅਗਲਾ;

    ਲਈ (ਕੰਮ = ਪਹਿਲਾ_ਕੰਮ; ਕੰਮ != ਨਿਰਾ; ਕੰਮ = ਕੰਮ_ਅਗਲਾ)
    {
	ਕੰਮ_ਅਗਲਾ = ਕੰਮ->jv_ਅਗਲਾ;
	ਜੇ ((ਕੰਮ->jv_copyID & ਨਕਾਬ) != (copyID & ਨਕਾਬ)
						    && !ਕੰਮ_still_useful(ਕੰਮ))
	{
	    // Free the job struct itself.
	    ਕੰਮ_free_ਕੰਮ(ਕੰਮ);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    ਕੰਮ_T *
ਕੰਮ_alloc(ਖਾਲੀ)
{
    ਕੰਮ_T *ਕੰਮ;

    ਕੰਮ = ALLOC_CLEAR_ONE(ਕੰਮ_T);
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਕੰਮ->jv_refcount = 1;
    ਕੰਮ->jv_stoponexit = vim_strsave((char_u *)"term");

    ਜੇ (ਪਹਿਲਾ_ਕੰਮ != ਨਿਰਾ)
    {
	ਪਹਿਲਾ_ਕੰਮ->jv_prev = ਕੰਮ;
	ਕੰਮ->jv_ਅਗਲਾ = ਪਹਿਲਾ_ਕੰਮ;
    }
    ਪਹਿਲਾ_ਕੰਮ = ਕੰਮ;
    ਵਾਪਸ ਕੰਮ;
}

    ਖਾਲੀ
ਕੰਮ_ਸੈੱਟ_options(ਕੰਮ_T *ਕੰਮ, jobopt_T *opt)
{
    ਜੇ (opt->jo_ਸੈੱਟ & JO_STOPONEXIT)
    {
	vim_free(ਕੰਮ->jv_stoponexit);
	ਜੇ (opt->jo_stoponexit == ਨਿਰਾ || *opt->jo_stoponexit == NUL)
	    ਕੰਮ->jv_stoponexit = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    ਕੰਮ->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ਜੇ (opt->jo_ਸੈੱਟ & JO_EXIT_CB)
    {
	free_callback(&ਕੰਮ->jv_exit_cb);
	ਜੇ (opt->jo_exit_cb.cb_name == ਨਿਰਾ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    ਕੰਮ->jv_exit_cb.cb_name = ਨਿਰਾ;
	    ਕੰਮ->jv_exit_cb.cb_partial = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	    copy_callback(&ਕੰਮ->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    ਖਾਲੀ
ਕੰਮ_stop_on_exit(ਖਾਲੀ)
{
    ਕੰਮ_T	*ਕੰਮ;

    FOR_ALL_JOBS(ਕੰਮ)
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_STARTED && ਕੰਮ->jv_stoponexit != ਨਿਰਾ)
	    mch_signal_ਕੰਮ(ਕੰਮ, ਕੰਮ->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    ਪੂਰਨ
has_pending_ਕੰਮ(ਖਾਲੀ)
{
    ਕੰਮ_T	    *ਕੰਮ;

    FOR_ALL_JOBS(ਕੰਮ)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ਜੇ ((ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_STARTED && !ਕੰਮ_channel_still_useful(ਕੰਮ))
		    || (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_ਮੁਕੰਮਲ
					      && ਕੰਮ_channel_can_close(ਕੰਮ)))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    ਪੂਰਨ
ਕੰਮ_check_ended(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		did_ਸਿਰਾ = FALSE;

    // be quick if there are no jobs to check
    ਜੇ (ਪਹਿਲਾ_ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ did_ਸਿਰਾ;

    ਲਈ (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	ਕੰਮ_T	*ਕੰਮ = mch_detect_ended_ਕੰਮ(ਪਹਿਲਾ_ਕੰਮ);

	ਜੇ (ਕੰਮ == ਨਿਰਾ)
	    ਰੋਕੋ;
	did_ਸਿਰਾ = TRUE;
	ਕੰਮ_cleanup(ਕੰਮ); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_ਮੰਜ਼ਿਲ_free_later();

    ਜੇ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ਵਾਪਸ did_ਸਿਰਾ;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    ਕੰਮ_T *
ਕੰਮ_ਸ਼ੁਰੂ(
	typval_T    *argvars,
	ਅੱਖਰ	    **argv_arg ਅਣਵਰਤਿਆ,
	jobopt_T    *opt_arg,
	ਕੰਮ_T	    **ਪਦ_ਕੰਮ)
{
    ਕੰਮ_T	*ਕੰਮ;
    char_u	*cmd = ਨਿਰਾ;
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    ਪੂਰਨ		argc = 0;
    ਪੂਰਨ		i;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_ARGV
    garray_T	ga;
#ਜੇ_ਅੰਤ
    jobopt_T	opt;
    ch_part_T	part;

    ਕੰਮ = ਕੰਮ_alloc();
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_FAILED;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_ARGV
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ*), 20);
#ਜੇ_ਅੰਤ

    ਜੇ (opt_arg != ਨਿਰਾ)
	opt = *opt_arg;
    ਨਹੀਂਤਾਂ
    {
	// Default mode is NL.
	clear_ਕੰਮ_options(&opt);
	opt.jo_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_NL;
	ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt,
		    JO_ਬਹੁਲਕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_ਬਾਹਰਲਾ_IO + JO_ਖੰਡ_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    ਜਾਓ theend;
    }

    // Check that when io is "file" that there is a file name.
    ਲਈ (part = PART_ਬਾਹਰਲਾ; part < PART_ਗਿਣਤੀ; ++part)
	ਜੇ ((opt.jo_ਸੈੱਟ & (JO_ਬਾਹਰਲਾ_IO << (part - PART_ਬਾਹਰਲਾ)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_ਸੈੱਟ & (JO_ਬਾਹਰਲਾ_NAME << (part - PART_ਬਾਹਰਲਾ)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_ਮੰਜ਼ਿਲ_be_ਸੈੱਟ));
	    ਜਾਓ theend;
	}

    ਜੇ ((opt.jo_ਸੈੱਟ & JO_ਵਿੱਚ_IO) && opt.jo_io[PART_ਵਿੱਚ] == JIO_ਬਫਰ)
    {
	buf_T *buf = ਨਿਰਾ;

	// check that we can find the buffer before starting the job
	ਜੇ (opt.jo_ਸੈੱਟ & JO_ਵਿੱਚ_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_ਵਿੱਚ]);
	    ਜੇ (buf == ਨਿਰਾ)
		semsg(_(e_ਬਫਰ_nr_does_not_exist),
						 (ਲੰਮਾ)opt.jo_io_buf[PART_ਵਿੱਚ]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (!(opt.jo_ਸੈੱਟ & JO_ਵਿੱਚ_NAME))
	{
	    emsg(_(e_ਵਿੱਚ_io_ਬਫਰ_requires_ਵਿੱਚ_buf_or_ਵਿੱਚ_name_ਮੰਜ਼ਿਲ_be_ਸੈੱਟ));
	}
	ਨਹੀਂਤਾਂ
	    buf = buflist_ਲੱਭੋ_by_name(opt.jo_io_name[PART_ਵਿੱਚ], FALSE);
	ਜੇ (buf == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ਜੇ (opt.jo_ਸੈੱਟ & JO_ਵਿੱਚ_BUF)
	    {
		sprintf((ਅੱਖਰ *)numbuf, "%d", opt.jo_io_buf[PART_ਵਿੱਚ]);
		s = numbuf;
	    }
	    ਨਹੀਂਤਾਂ
		s = opt.jo_io_name[PART_ਵਿੱਚ];
	    semsg(_(e_ਬਫਰ_must_be_loaded_str), s);
	    ਜਾਓ theend;
	}
	ਕੰਮ->jv_ਵਿੱਚ_buf = buf;
    }

    ਕੰਮ_ਸੈੱਟ_options(ਕੰਮ, &opt);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARGV
    ਜੇ (argv_arg != ਨਿਰਾ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	ਲਈ (i = 0; argv_arg[i] != ਨਿਰਾ; i++)
	    argc++;
	argv = ALLOC_MULT(ਅੱਖਰ *, argc + 1);
	ਜੇ (argv == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਲਈ (i = 0; i < argc; i++)
	    argv[i] = (ਅੱਖਰ *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ਜੇ (cmd == ਨਿਰਾ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਜਾਓ theend;
	}

	ਜੇ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ਨਿਰਾ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ
    {
	list_T *l = argvars[0].vval.v_list;

	ਜੇ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ਜਾਓ theend;

	// Empty command is invalid.
	ਜੇ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਜਾਓ theend;
	}
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_ARGV
	ਜੇ (win32_build_cmd(l, &ga) == FAIL)
	    ਜਾਓ theend;
	cmd = ga.ga_ਅੰਕੜੇ;
	ਜੇ (cmd == ਨਿਰਾ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਜਾਓ theend;
	}
#ਜੇ_ਅੰਤ
    }

    // Save the command used to start the job.
    ਕੰਮ->jv_argv = argv;

    ਜੇ (ਪਦ_ਕੰਮ != ਨਿਰਾ)
	*ਪਦ_ਕੰਮ = ਕੰਮ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARGV
    ਜੇ (ch_log_ਸਰਗਰਮ())
    {
	garray_T    ga;

	ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 200);
	ਲਈ (i = 0; i < argc; ++i)
	{
	    ਜੇ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ਨਿਰਾ, "Starting job: %s", (ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ);
	ga_clear(&ga);
    }
    mch_ਕੰਮ_ਸ਼ੁਰੂ(argv, ਕੰਮ, &opt, ਪਦ_ਕੰਮ != ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    ch_log(ਨਿਰਾ, "Starting job: %s", (ਅੱਖਰ *)cmd);
    mch_ਕੰਮ_ਸ਼ੁਰੂ((ਅੱਖਰ *)cmd, ਕੰਮ, &opt);
#ਜੇ_ਅੰਤ

    // If the channel is reading from a buffer, write lines now.
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
	channel_write_ਵਿੱਚ(ਕੰਮ->jv_channel);

theend:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_ARGV
    vim_free(ga.ga_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
    ਜੇ (argv != ਨਿਰਾ && argv != ਕੰਮ->jv_argv)
    {
	ਲਈ (i = 0; argv[i] != ਨਿਰਾ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_ਕੰਮ_options(&opt);
    ਵਾਪਸ ਕੰਮ;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    ਅੱਖਰ *
ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ_T *ਕੰਮ)
{
    ਅੱਖਰ	*ਨਤੀਜਾ;

    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ >= ਕੰਮ_ENDED)
	// No need to check, dead is dead.
	ਨਤੀਜਾ = "dead";
    ਨਹੀਂਤਾਂ ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_FAILED)
	ਨਤੀਜਾ = "fail";
    ਨਹੀਂਤਾਂ
    {
	ਨਤੀਜਾ = mch_ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ);
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_ENDED)
	    ਕੰਮ_cleanup(ਕੰਮ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    ਪੂਰਨ
ਕੰਮ_stop(ਕੰਮ_T *ਕੰਮ, typval_T *argvars, ਅੱਖਰ *type)
{
    char_u *arg;

    ਜੇ (type != ਨਿਰਾ)
	arg = (char_u *)type;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ਨਹੀਂਤਾਂ
    {
	arg = tv_get_string_chk(&argvars[1]);
	ਜੇ (arg == ਨਿਰਾ)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ 0;
	}
    }
    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_FAILED)
    {
	ch_log(ਕੰਮ->jv_channel, "Job failed to start, job_stop() skipped");
	ਵਾਪਸ 0;
    }
    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_ENDED)
    {
	ch_log(ਕੰਮ->jv_channel, "Job has already ended, job_stop() skipped");
	ਵਾਪਸ 0;
    }
    ch_log(ਕੰਮ->jv_channel, "Stopping job with '%s'", (ਅੱਖਰ *)arg);
    ਜੇ (mch_signal_ਕੰਮ(ਕੰਮ, arg) == FAIL)
	ਵਾਪਸ 0;

    // Assume that only "kill" will kill the job.
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ && STRCMP(arg, "kill") == 0)
	ਕੰਮ->jv_channel->ch_ਕੰਮ_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ਵਾਪਸ 1;
}

    ਖਾਲੀ
invoke_prompt_callback(ਖਾਲੀ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*ਪਾਠ;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.ਖਾਨਾ = 0;

    ਜੇ (curbuf->b_prompt_callback.cb_name == ਨਿਰਾ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ਵਾਪਸ;
    ਪਾਠ = ml_get(lnum);
    prompt = prompt_ਪਾਠ();
    ਜੇ (STRLEN(ਪਾਠ) >= STRLEN(prompt))
	ਪਾਠ += STRLEN(prompt);
    argv[0].v_type = VAR_STRING;
    argv[0].vval.v_string = vim_strsave(ਪਾਠ);
    argv[1].v_type = VAR_UNKNOWN;

    call_callback(&curbuf->b_prompt_callback, -1, &rettv, 1, argv);
    clear_tv(&argv[0]);
    clear_tv(&rettv);
}

/*
 * Return TRUE when the interrupt callback was invoked.
 */
    ਪੂਰਨ
invoke_prompt_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    typval_T	rettv;
    typval_T	argv[1];
    ਪੂਰਨ		ret;

    ਜੇ (curbuf->b_prompt_ਰੁਕਾਵਟ.cb_name == ਨਿਰਾ
	    || *curbuf->b_prompt_ਰੁਕਾਵਟ.cb_name == NUL)
	ਵਾਪਸ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_ਰੁਕਾਵਟ, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ਵਾਪਸ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ਸਥਿਰ char_u *
buf_prompt_ਪਾਠ(buf_T* buf)
{
    ਜੇ (buf->b_prompt_ਪਾਠ == ਨਿਰਾ)
	ਵਾਪਸ (char_u *)"% ";
    ਵਾਪਸ buf->b_prompt_ਪਾਠ;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_ਪਾਠ(ਖਾਲੀ)
{
    ਵਾਪਸ buf_prompt_ਪਾਠ(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    ਖਾਲੀ
init_prompt(ਪੂਰਨ cmdchar_todo)
{
    char_u *prompt = prompt_ਪਾਠ();
    char_u *ਪਾਠ;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਪਾਠ = ml_get_curline();
    ਜੇ (STRNCMP(ਪਾਠ, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ਜੇ (*ਪਾਠ == NUL)
	    ml_replace(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, prompt, TRUE);
	ਨਹੀਂਤਾਂ
	    ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	coladvance((colnr_T)MAXCOL);
	ਬਦਲਿਆ_ਬਾਈਟਾਂ(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ਜੇ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.ਖਾਨਾ != (ਪੂਰਨ)STRLEN(prompt))
	ਸੈੱਟ_insstart(curwin->w_cursor.lnum, (ਪੂਰਨ)STRLEN(prompt));

    ਜੇ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ਜੇ (curwin->w_cursor.ਖਾਨਾ < (ਪੂਰਨ)STRLEN(prompt))
	curwin->w_cursor.ਖਾਨਾ = (ਪੂਰਨ)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    ਪੂਰਨ
prompt_curpos_editable(ਖਾਲੀ)
{
    ਵਾਪਸ curwin->w_cursor.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	&& curwin->w_cursor.ਖਾਨਾ >= (ਪੂਰਨ)STRLEN(prompt_ਪਾਠ());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    ਖਾਲੀ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    callback_T	callback;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    callback = get_callback(&argvars[1]);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ;

    free_callback(&buf->b_prompt_callback);
    ਸੈੱਟ_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    ਖਾਲੀ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    callback_T	callback;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    callback = get_callback(&argvars[1]);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ;

    free_callback(&buf->b_prompt_ਰੁਕਾਵਟ);
    ਸੈੱਟ_callback(&buf->b_prompt_ਰੁਕਾਵਟ, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    ਖਾਲੀ
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (!bt_prompt(buf))
	ਵਾਪਸ;

    rettv->vval.v_string = vim_strsave(buf_prompt_ਪਾਠ(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    ਖਾਲੀ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    char_u	*ਪਾਠ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_secure())
	ਵਾਪਸ;
    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪਾਠ = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_ਪਾਠ);
    buf->b_prompt_ਪਾਠ = vim_strsave(ਪਾਠ);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ਸਥਿਰ ਕੰਮ_T *
get_ਕੰਮ_arg(typval_T *tv)
{
    ਕੰਮ_T *ਕੰਮ;

    ਜੇ (tv->v_type != VAR_ਕੰਮ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(tv));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਕੰਮ = tv->vval.v_ਕੰਮ;

    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	emsg(_(e_not_valid_ਕੰਮ));
    ਵਾਪਸ ਕੰਮ;
}

/*
 * "job_getchannel()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_getchannel(typval_T *argvars, typval_T *rettv)
{
    ਕੰਮ_T	*ਕੰਮ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਕੰਮ = get_ਕੰਮ_arg(&argvars[0]);
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ਕੰਮ->jv_channel;
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
	++ਕੰਮ->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_info(ਕੰਮ_T *ਕੰਮ, dict_T *dict)
{
    dictitem_T	*ਵਸਤੂ;
    varnumber_T	nr;
    list_T	*l;
    ਪੂਰਨ		i;

    dict_ਜੋੜੋ_string(dict, "status", (char_u *)ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ));

    ਵਸਤੂ = dictitem_alloc((char_u *)"channel");
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ;
    ਵਸਤੂ->di_tv.v_type = VAR_CHANNEL;
    ਵਸਤੂ->di_tv.vval.v_channel = ਕੰਮ->jv_channel;
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
	++ਕੰਮ->jv_channel->ch_refcount;
    ਜੇ (dict_ਜੋੜੋ(dict, ਵਸਤੂ) == FAIL)
	dictitem_free(ਵਸਤੂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    nr = ਕੰਮ->jv_pid;
#ਨਹੀਂਤਾਂ
    nr = ਕੰਮ->jv_proc_info.dwProcessId;
#ਜੇ_ਅੰਤ
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "process", nr);
    dict_ਜੋੜੋ_string(dict, "tty_in", ਕੰਮ->jv_tty_ਵਿੱਚ);
    dict_ਜੋੜੋ_string(dict, "tty_out", ਕੰਮ->jv_tty_ਬਾਹਰਲਾ);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "exitval", ਕੰਮ->jv_exitval);
    dict_ਜੋੜੋ_string(dict, "exit_cb", ਕੰਮ->jv_exit_cb.cb_name);
    dict_ਜੋੜੋ_string(dict, "stoponexit", ਕੰਮ->jv_stoponexit);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    dict_ਜੋੜੋ_string(dict, "termsig", ਕੰਮ->jv_termsig);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    dict_ਜੋੜੋ_string(dict, "tty_type", ਕੰਮ->jv_tty_type);
#ਜੇ_ਅੰਤ

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ਵਾਪਸ;
    }
    ਜੇ (ਕੰਮ->jv_argv != ਨਿਰਾ)
	ਲਈ (i = 0; ਕੰਮ->jv_argv[i] != ਨਿਰਾ; i++)
	    list_append_string(l, (char_u *)ਕੰਮ->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ਸਥਿਰ ਖਾਲੀ
ਕੰਮ_info_all(list_T *l)
{
    ਕੰਮ_T	*ਕੰਮ;
    typval_T	tv;

    FOR_ALL_JOBS(ਕੰਮ)
    {
	tv.v_type = VAR_ਕੰਮ;
	tv.vval.v_ਕੰਮ = ਕੰਮ;

	ਜੇ (list_append_tv(l, &tv) != OK)
	    ਵਾਪਸ;
    }
}

/*
 * "job_info()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_info(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ਕੰਮ_T	*ਕੰਮ;

	ਕੰਮ = get_ਕੰਮ_arg(&argvars[0]);
	ਜੇ (ਕੰਮ != ਨਿਰਾ && rettv_dict_alloc(rettv) == OK)
	    ਕੰਮ_info(ਕੰਮ, rettv->vval.v_dict);
    }
    ਨਹੀਂਤਾਂ ਜੇ (rettv_list_alloc(rettv) == OK)
	ਕੰਮ_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_setoptions(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਕੰਮ_T	*ਕੰਮ;
    jobopt_T	opt;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਕੰਮ = get_ਕੰਮ_arg(&argvars[0]);
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ;
    clear_ਕੰਮ_options(&opt);
    ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	ਕੰਮ_ਸੈੱਟ_options(ਕੰਮ, &opt);
    free_ਕੰਮ_options(&opt);
}

/*
 * "job_start()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_ਸ਼ੁਰੂ(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_ਕੰਮ;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਕੰਮ = ਕੰਮ_ਸ਼ੁਰੂ(argvars, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * "job_status()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_ਸਥਿਤੀ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_ਕੰਮ && argvars[0].vval.v_ਕੰਮ == ਨਿਰਾ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ਨਹੀਂਤਾਂ
    {
	ਕੰਮ_T	*ਕੰਮ = get_ਕੰਮ_arg(&argvars[0]);

	ਜੇ (ਕੰਮ != ਨਿਰਾ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ));
	}
    }
}

/*
 * "job_stop()" function
 */
    ਖਾਲੀ
f_ਕੰਮ_stop(typval_T *argvars, typval_T *rettv)
{
    ਕੰਮ_T	*ਕੰਮ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਕੰਮ = get_ਕੰਮ_arg(&argvars[0]);
    ਜੇ (ਕੰਮ != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = ਕੰਮ_stop(ਕੰਮ, argvars, ਨਿਰਾ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
ਕੰਮ_ਮੰਜ਼ਿਲ_string_buf(typval_T *varp, char_u *buf)
{
    ਕੰਮ_T *ਕੰਮ = varp->vval.v_ਕੰਮ;
    ਅੱਖਰ  *ਸਥਿਤੀ;

    ਜੇ (ਕੰਮ == ਨਿਰਾ)
    {
	vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN, "no process");
	ਵਾਪਸ buf;
    }
    ਸਥਿਤੀ = ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_FAILED ? "fail"
		    : ਕੰਮ->jv_ਸਥਿਤੀ >= ਕੰਮ_ENDED ? "dead"
		    : "run";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN,
		"process %ld %s", (ਲੰਮਾ)ਕੰਮ->jv_pid, ਸਥਿਤੀ);
#ਜਾਂਜੇ defined(MSWIN)
    vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN,
		"process %ld %s",
		(ਲੰਮਾ)ਕੰਮ->jv_proc_info.dwProcessId,
		ਸਥਿਤੀ);
#ਨਹੀਂਤਾਂ
    // fall-back
    vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN, "process ? %s", ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ buf;
}

#ਜੇ_ਅੰਤ // FEAT_JOB_CHANNEL
