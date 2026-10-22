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

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_JOBS(ಕೆಲಸ) \
    ಗಾಗಿ ((ಕೆಲಸ) = ಮೊದಲ_ಕೆಲಸ; (ಕೆಲಸ) != ಖಾಲಿ; (ಕೆಲಸ) = (ಕೆಲಸ)->jv_ಮುಂದಿನ)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_ಬಹುಲಕ(typval_T *ಅಂಶ, jobopt_T *opt, ch_ಬಹುಲಕ_T *modep, ಪೂರ್ಣಾಂಕ jo)
{
    char_u	*val = tv_get_string(ಅಂಶ);

    opt->jo_ಗಣ |= jo;
    ಆದರೆ (STRCMP(val, "nl") == 0)
	*modep = CH_ಬಹುಲಕ_NL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "raw") == 0)
	*modep = CH_ಬಹುಲಕ_RAW;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "blob") == 0)
	*modep = CH_ಬಹುಲಕ_BLOB;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "js") == 0)
	*modep = CH_ಬಹುಲಕ_JS;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "json") == 0)
	*modep = CH_ಬಹುಲಕ_JSON;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "lsp") == 0)
	*modep = CH_ಬಹುಲಕ_LSP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "dap") == 0)
	*modep = CH_ಬಹುಲಕ_DAP;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str), val);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_io(typval_T *ಅಂಶ, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(ಅಂಶ);

    opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_IO << (part - PART_ಹೊರಹೋಗು);
    ಆದರೆ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_ಬಫರ್;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_ಹೊರಹೋಗು;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str), val);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    ಶೂನ್ಯ
clear_ಕೆಲಸ_options(jobopt_T *opt)
{
    CLEAR_ಸೂಚಕ(opt);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unref_ಕೆಲಸ_callback(callback_T *cb)
{
    ಆದರೆ (cb->cb_partial != ಖಾಲಿ)
	partial_unref(cb->cb_partial);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cb->cb_name != ಖಾಲಿ)
    {
	func_unref(cb->cb_name);
	ಆದರೆ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    ಶೂನ್ಯ
free_ಕೆಲಸ_options(jobopt_T *opt)
{
    unref_ಕೆಲಸ_callback(&opt->jo_callback);
    unref_ಕೆಲಸ_callback(&opt->jo_ಹೊರಹೋಗು_cb);
    unref_ಕೆಲಸ_callback(&opt->jo_err_cb);
    unref_ಕೆಲಸ_callback(&opt->jo_close_cb);
    unref_ಕೆಲಸ_callback(&opt->jo_exit_cb);

    ಆದರೆ (opt->jo_env != ಖಾಲಿ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
part_from_char(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ c == 'i' ? PART_ಒಳ : c == 'o' ? PART_ಹೊರಹೋಗು: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_ಕೆಲಸ_options(typval_T *tv, jobopt_T *opt, ಪೂರ್ಣಾಂಕ supported, ಪೂರ್ಣಾಂಕ supported2)
{
    typval_T	*ಅಂಶ;
    char_u	*val;
    dict_T	*dict;
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ಆದರೆ (tv->v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    dict = tv->vval.v_dict;
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    todo = (ಪೂರ್ಣಾಂಕ)dict->dv_hashtab.ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&dict->dv_hashtab, hi, todo)
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ಅಂಶ = &dict_lookup(hi)->di_tv;

	    ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "mode") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (handle_ಬಹುಲಕ(ಅಂಶ, opt, &opt->jo_ಬಹುಲಕ, JO_ಬಹುಲಕ) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "in_mode") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಒಳ_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (handle_ಬಹುಲಕ(ಅಂಶ, opt, &opt->jo_ಒಳ_ಬಹುಲಕ, JO_ಒಳ_ಬಹುಲಕ)
								      == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "out_mode") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (handle_ಬಹುಲಕ(ಅಂಶ, opt, &opt->jo_ಹೊರಹೋಗು_ಬಹುಲಕ, JO_ಹೊರಹೋಗು_ಬಹುಲಕ)
								      == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "err_mode") == 0)
	    {
		ಆದರೆ (!(supported & JO_ERR_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (handle_ಬಹುಲಕ(ಅಂಶ, opt, &opt->jo_err_ಬಹುಲಕ, JO_ERR_ಬಹುಲಕ)
								      == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "noblock") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		opt->jo_noblock = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "in_io") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_io") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_io") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (handle_io(ಅಂಶ, part_from_char(*hi->hi_ಕೀಲಿ), opt) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "in_name") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_name") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_NAME << (part - PART_ಹೊರಹೋಗು);
		opt->jo_io_name[part] = tv_get_string_buf_chk(ಅಂಶ,
						   opt->jo_io_name_buf[part]);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "pty") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಬಹುಲಕ))
		    ನಿಲ್ಲಿಸು;
		opt->jo_pty = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "in_buf") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "out_buf") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_BUF << (part - PART_ಹೊರಹೋಗು);
		opt->jo_io_buf[part] = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
		ಆದರೆ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str),
					      hi->hi_ಕೀಲಿ, tv_get_string(ಅಂಶ));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (buflist_findnr(opt->jo_io_buf[part]) == ಖಾಲಿ)
		{
		    semsg(_(e_ಬಫರ್_nr_does_not_exist),
						   (ದೀರ್ಘ)opt->jo_io_buf[part]);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "out_modifiable") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_MODIFIABLE << (part - PART_ಹೊರಹೋಗು);
		opt->jo_modifiable[part] = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "out_msg") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ಕೀಲಿ);

		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಹೊರಹೋಗು_ಸಂದೇಶ << (part - PART_ಹೊರಹೋಗು);
		opt->jo_ಸಂದೇಶ[part] = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "in_top") == 0
		    || STRCMP(hi->hi_ಕೀಲಿ, "in_bot") == 0)
	    {
		linenr_T *lp;

		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (hi->hi_ಕೀಲಿ[3] == 't')
		{
		    lp = &opt->jo_ಒಳ_ತುದಿ;
		    opt->jo_ಗಣ |= JO_ಒಳ_ತುದಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    lp = &opt->jo_ಒಳ_bot;
		    opt->jo_ಗಣ |= JO_ಒಳ_BOT;
		}
		*lp = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
		ಆದರೆ (*lp < 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str),
					      hi->hi_ಕೀಲಿ, tv_get_string(ಅಂಶ));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "channel") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_IO))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_CHANNEL;
		ಆದರೆ (ಅಂಶ->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "channel");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_channel = ಅಂಶ->vval.v_channel;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "callback") == 0)
	    {
		ಆದರೆ (!(supported & JO_CALLBACK))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_CALLBACK;
		opt->jo_callback = get_callback(ಅಂಶ);
		ಆದರೆ (opt->jo_callback.cb_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "callback");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "out_cb") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_CALLBACK))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_CALLBACK;
		opt->jo_ಹೊರಹೋಗು_cb = get_callback(ಅಂಶ);
		ಆದರೆ (opt->jo_ಹೊರಹೋಗು_cb.cb_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "out_cb");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "err_cb") == 0)
	    {
		ಆದರೆ (!(supported & JO_ERR_CALLBACK))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(ಅಂಶ);
		ಆದರೆ (opt->jo_err_cb.cb_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "err_cb");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "close_cb") == 0)
	    {
		ಆದರೆ (!(supported & JO_CLOSE_CALLBACK))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(ಅಂಶ);
		ಆದರೆ (opt->jo_close_cb.cb_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "close_cb");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "drop") == 0)
	    {
		ಪೂರ್ಣಾಂಕ never = FALSE;
		val = tv_get_string(ಅಂಶ);

		ಆದರೆ (STRCMP(val, "never") == 0)
		    never = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "drop", val);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "exit_cb") == 0)
	    {
		ಆದರೆ (!(supported & JO_EXIT_CB))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(ಅಂಶ);
		ಆದರೆ (opt->jo_exit_cb.cb_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "exit_cb");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_name") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_ಪದ_NAME))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_NAME;
		opt->jo_ಪದ_name = tv_get_string_buf_chk(ಅಂಶ,
						       opt->jo_ಪದ_name_buf);
		ಆದರೆ (opt->jo_ಪದ_name == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_name");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_finish") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_ಪದ_ಮುಕ್ತಾಯ))
		    ನಿಲ್ಲಿಸು;
		val = tv_get_string(ಅಂಶ);
		ಆದರೆ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str),
							   "term_finish", val);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_set2 |= JO2_ಪದ_ಮುಕ್ತಾಯ;
		opt->jo_ಪದ_ಮುಕ್ತಾಯ = *val;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_opencmd") == 0)
	    {
		char_u *p;

		ಆದರೆ (!(supported2 & JO2_ಪದ_OPENCMD))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_OPENCMD;
		p = opt->jo_ಪದ_opencmd = tv_get_string_buf_chk(ಅಂಶ,
						    opt->jo_ಪದ_opencmd_buf);
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ಆದರೆ (p != ಖಾಲಿ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ಖಾಲಿ))
			p = ಖಾಲಿ;
		}
		ಆದರೆ (p == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_opencmd");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "eof_chars") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_EOF_CHARS))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(ಅಂಶ,
						       opt->jo_eof_chars_buf);
		ಆದರೆ (opt->jo_eof_chars == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "eof_chars");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_rows") == 0)
	    {
		ಪೂರ್ಣಾಂಕ error = FALSE;

		ಆದರೆ (!(supported2 & JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು;
		opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು = tv_get_ಸಂಖ್ಯೆ_chk(ಅಂಶ, &error);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು < 0 || opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು > 1000)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_rows");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_cols") == 0)
	    {
		ಪೂರ್ಣಾಂಕ error = FALSE;

		ಆದರೆ (!(supported2 & JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
		opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(ಅಂಶ, &error);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0 || opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 1000)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_cols");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "vertical") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_VERTICAL))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "curwin") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_CURWIN))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "bufnr") == 0)
	    {
		ಪೂರ್ಣಾಂಕ nr;

		ಆದರೆ (!(supported2 & JO2_CURWIN))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
		ಆದರೆ (nr <= 0)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), hi->hi_ಕೀಲಿ, tv_get_string(ಅಂಶ));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ಆದರೆ (opt->jo_bufnr_buf == ಖಾಲಿ)
		{
		    semsg(_(e_ಬಫರ್_nr_does_not_exist), (ದೀರ್ಘ)nr);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_ಪದ == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ವಾದ_str), "bufnr");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "hidden") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_HIDDEN))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "norestore") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_NORESTORE))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_ಪದ_norestore = tv_get_bool(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_kill") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_ಪದ_KILL))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_KILL;
		opt->jo_ಪದ_kill = tv_get_string_buf_chk(ಅಂಶ,
						       opt->jo_ಪದ_kill_buf);
		ಆದರೆ (opt->jo_ಪದ_kill == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_kill");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "tty_type") == 0)
	    {
		char_u *p;

		ಆದರೆ (!(supported2 & JO2_TTY_TYPE))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(ಅಂಶ);
		ಆದರೆ (p == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "tty_type");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ಆದರೆ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "tty_type");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "ansi_colors") == 0)
	    {
		ಪೂರ್ಣಾಂಕ		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ಆದರೆ (!(supported2 & JO2_ANSI_COLORS))
		    ನಿಲ್ಲಿಸು;

		ಆದರೆ (ಅಂಶ == ಖಾಲಿ || ಅಂಶ->v_type != VAR_LIST
			|| ಅಂಶ->vval.v_list == ಖಾಲಿ
			|| ಅಂಶ->vval.v_list->lv_ಮೊದಲ == &range_list_ಅಂಶ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "ansi_colors");
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		li = ಅಂಶ->vval.v_list->lv_ಮೊದಲ;
		ಗಾಗಿ (; li != ಖಾಲಿ && n < 16; li = li->li_ಮುಂದಿನ, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ಆದರೆ (color_name == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ಆದರೆ (guicolor == INVALCOLOR)
		    {
			ಆದರೆ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			ಹಿಂತಿರುಗಿ FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ಆದರೆ (n != 16 || li != ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "ansi_colors");
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		ಸ್ಮರಣೆ_ನಕಲು(opt->jo_ansi_colors, rgb, ಗಾತ್ರ(rgb));
	    }
# endif
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_highlight") == 0)
	    {
		char_u *p;

		ಆದರೆ (!(supported2 & JO2_ಪದ_HIGHLIGHT))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_HIGHLIGHT;
		p = tv_get_string_buf_chk(ಅಂಶ, opt->jo_ಪದ_highlight_buf);
		ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_highlight");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_ಪದ_highlight = p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "term_api") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_ಪದ_API))
		    ನಿಲ್ಲಿಸು;
		opt->jo_set2 |= JO2_ಪದ_API;
		opt->jo_ಪದ_api = tv_get_string_buf_chk(ಅಂಶ,
							opt->jo_ಪದ_api_buf);
		ಆದರೆ (opt->jo_ಪದ_api == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "term_api");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "env") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_ENV))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (ಅಂಶ->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "env");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = ಅಂಶ->vval.v_dict;
		ಆದರೆ (opt->jo_env != ಖಾಲಿ)
		    ++opt->jo_env->dv_refcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "cwd") == 0)
	    {
		ಆದರೆ (!(supported2 & JO2_CWD))
		    ನಿಲ್ಲಿಸು;
		opt->jo_cwd = tv_get_string_buf_chk(ಅಂಶ, opt->jo_cwd_buf);
		ಆದರೆ (opt->jo_cwd == ಖಾಲಿ || !mch_isdir(opt->jo_cwd)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((ಅಕ್ಷರ *)opt->jo_cwd, X_OK) != 0
#ಆದರೆ_ಅಂತ್ಯ
				)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "cwd");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "waittime") == 0)
	    {
		ಆದರೆ (!(supported & JO_WAITTIME))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_WAITTIME;
		opt->jo_waittime = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "timeout") == 0)
	    {
		ಆದರೆ (!(supported & JO_TIMEOUT))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "out_timeout") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಹೊರಹೋಗು_TIMEOUT))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_TIMEOUT;
		opt->jo_ಹೊರಹೋಗು_timeout = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "err_timeout") == 0)
	    {
		ಆದರೆ (!(supported & JO_ERR_TIMEOUT))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "part") == 0)
	    {
		ಆದರೆ (!(supported & JO_PART))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_PART;
		val = tv_get_string(ಅಂಶ);
		ಆದರೆ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_ಹೊರಹೋಗು;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "part", val);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "id") == 0)
	    {
		ಆದರೆ (!(supported & JO_ID))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ID;
		opt->jo_id = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "stoponexit") == 0)
	    {
		ಆದರೆ (!(supported & JO_STOPONEXIT))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(ಅಂಶ,
						      opt->jo_stoponexit_buf);
		ಆದರೆ (opt->jo_stoponexit == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "stoponexit");
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hi->hi_ಕೀಲಿ, "block_write") == 0)
	    {
		ಆದರೆ (!(supported & JO_ಖಂಡ_WRITE))
		    ನಿಲ್ಲಿಸು;
		opt->jo_ಗಣ |= JO_ಖಂಡ_WRITE;
		opt->jo_ಖಂಡ_write = tv_get_ಸಂಖ್ಯೆ(ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    --todo;
	}
    ಆದರೆ (todo > 0)
    {
	semsg(_(e_invalid_ವಾದ_str), hi->hi_ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಕೆಲಸ_T *ಮೊದಲ_ಕೆಲಸ = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_free_contents(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ch_log(ಕೆಲಸ->jv_channel, "Freeing job");
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	ಕೆಲಸ->jv_channel->ch_ಕೆಲಸ = ಖಾಲಿ;
	channel_unref(ಕೆಲಸ->jv_channel);
    }
    mch_clear_ಕೆಲಸ(ಕೆಲಸ);

    vim_free(ಕೆಲಸ->jv_tty_ಒಳ);
    vim_free(ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು);
    vim_free(ಕೆಲಸ->jv_stoponexit);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    vim_free(ಕೆಲಸ->jv_termsig);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    vim_free(ಕೆಲಸ->jv_tty_type);
#ಆದರೆ_ಅಂತ್ಯ
    free_callback(&ಕೆಲಸ->jv_exit_cb);
    ಆದರೆ (ಕೆಲಸ->jv_argv != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; ಕೆಲಸ->jv_argv[i] != ಖಾಲಿ; i++)
	    vim_free(ಕೆಲಸ->jv_argv[i]);
	vim_free(ಕೆಲಸ->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_unlink(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಆದರೆ (ಕೆಲಸ->jv_ಮುಂದಿನ != ಖಾಲಿ)
	ಕೆಲಸ->jv_ಮುಂದಿನ->jv_prev = ಕೆಲಸ->jv_prev;
    ಆದರೆ (ಕೆಲಸ->jv_prev == ಖಾಲಿ)
	ಮೊದಲ_ಕೆಲಸ = ಕೆಲಸ->jv_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೆಲಸ->jv_prev->jv_ಮುಂದಿನ = ಕೆಲಸ->jv_ಮುಂದಿನ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_free_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಕೆಲಸ_unlink(ಕೆಲಸ);
    vim_free(ಕೆಲಸ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_free(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಆದರೆ (ಒಳ_free_unref_ವಸ್ತುಗಳು)
	ಹಿಂತಿರುಗಿ;

    ಕೆಲಸ_free_contents(ಕೆಲಸ);
    ಕೆಲಸ_free_ಕೆಲಸ(ಕೆಲಸ);
}

ಸ್ಥಿರ ಕೆಲಸ_T *jobs_ಗಮ್ಯ_free = ಖಾಲಿ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_free_later(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಕೆಲಸ_unlink(ಕೆಲಸ);
    ಕೆಲಸ->jv_ಮುಂದಿನ = jobs_ಗಮ್ಯ_free;
    jobs_ಗಮ್ಯ_free = ಕೆಲಸ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_jobs_ಗಮ್ಯ_free_later(ಶೂನ್ಯ)
{
    ಕೆಲಸ_T *ಕೆಲಸ;

    ಆಗ (jobs_ಗಮ್ಯ_free != ಖಾಲಿ)
    {
	ಕೆಲಸ = jobs_ಗಮ್ಯ_free;
	jobs_ಗಮ್ಯ_free = ಕೆಲಸ->jv_ಮುಂದಿನ;
	ಕೆಲಸ_free_contents(ಕೆಲಸ);
	vim_free(ಕೆಲಸ);
    }
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
ಕೆಲಸ_free_all(ಶೂನ್ಯ)
{
    ಆಗ (ಮೊದಲ_ಕೆಲಸ != ಖಾಲಿ)
	ಕೆಲಸ_free(ಮೊದಲ_ಕೆಲಸ);
    free_jobs_ಗಮ್ಯ_free_later();

# ifdef FEAT_TERMINAL
    free_ಬಳಸದ_terminals();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_need_ಕೊನೆ_check(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಹಿಂತಿರುಗಿ ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_STARTED
	    && (ಕೆಲಸ->jv_stoponexit != ಖಾಲಿ || ಕೆಲಸ->jv_exit_cb.cb_name != ಖಾಲಿ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_channel_still_useful(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಹಿಂತಿರುಗಿ ಕೆಲಸ->jv_channel != ಖಾಲಿ && channel_still_useful(ಕೆಲಸ->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_channel_can_close(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಹಿಂತಿರುಗಿ ಕೆಲಸ->jv_channel != ಖಾಲಿ && channel_can_close(ಕೆಲಸ->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_still_useful(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಹಿಂತಿರುಗಿ ಕೆಲಸ_need_ಕೊನೆ_check(ಕೆಲಸ) || ಕೆಲಸ_channel_still_useful(ಕೆಲಸ);
}

#ಆದರೆ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_any_running(ಶೂನ್ಯ)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;

    FOR_ALL_JOBS(ಕೆಲಸ)
	ಆದರೆ (ಕೆಲಸ_still_useful(ಕೆಲಸ))
	{
	    ch_log(ಖಾಲಿ, "GUI not forking because a job is running");
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

// Unix uses argv[] for the command, other systems use a string.
#ಆದರೆ defined(UNIX)
# define USE_ARGV
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
win32_escape_arg(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ		slen, dlen;
    ಪೂರ್ಣಾಂಕ		escaping = 0;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    ಪೂರ್ಣಾಂಕ		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (ಪೂರ್ಣಾಂಕ)STRLEN(arg);
    dlen = slen;
    ಗಾಗಿ (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ಆದರೆ (*s == '"' || *s == '\\')
	    ++dlen;
	ಆದರೆ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ಆದರೆ (has_spaces)
	dlen += 2;

    ಆದರೆ (dlen == slen)
	ಹಿಂತಿರುಗಿ vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ಆದರೆ (escaped_arg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ಆದರೆ (has_spaces)
	*d++ = '"';

    ಗಾಗಿ (s = arg; *s != NUL;)
    {
	ಆಯ್ಕೆ (*s)
	{
	    ಸ್ಥಿತಿ '"':
		ಗಾಗಿ (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '\\':
		escaping++;
		*d++ = *s++;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		ನಿಲ್ಲಿಸು;
	}
    }

    // add terminating quote and finish with a NUL
    ಆದರೆ (has_spaces)
    {
	ಗಾಗಿ (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    ಹಿಂತಿರುಗಿ escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ಆದರೆ (s == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	s = win32_escape_arg(s);
	ಆದರೆ (s == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ಆದರೆ (li->li_ಮುಂದಿನ != ಖಾಲಿ)
	    ga_append(gap, ' ');
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    ಶೂನ್ಯ
ಕೆಲಸ_cleanup(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_ENDED)
	ಹಿಂತಿರುಗಿ;

    // Ready to cleanup the job.
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ;

    // When only channel-in is kept open, close explicitly.
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
	ch_close_part(ಕೆಲಸ->jv_channel, PART_ಒಳ);

    ಆದರೆ (ಕೆಲಸ->jv_exit_cb.cb_name != ಖಾಲಿ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(ಕೆಲಸ->jv_channel, "Invoking exit callback %s",
						      ಕೆಲಸ->jv_exit_cb.cb_name);
	++ಕೆಲಸ->jv_refcount;
	argv[0].v_type = VAR_ಕೆಲಸ;
	argv[0].vval.v_ಕೆಲಸ = ಕೆಲಸ;
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = ಕೆಲಸ->jv_exitval;
	call_callback(&ಕೆಲಸ->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--ಕೆಲಸ->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ && ಕೆಲಸ->jv_channel->ch_anonymous_pipe)
	ಕೆಲಸ->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ಆದರೆ (ಕೆಲಸ->jv_refcount == 0 && !ಕೆಲಸ_channel_still_useful(ಕೆಲಸ))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	ಕೆಲಸ_free_later(ಕೆಲಸ);
}

/*
 * Mark references in jobs that are still useful.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಕೆಲಸ(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    ಕೆಲಸ_T	*ಕೆಲಸ;
    typval_T	tv;

    ಗಾಗಿ (ಕೆಲಸ = ಮೊದಲ_ಕೆಲಸ; !abort && ಕೆಲಸ != ಖಾಲಿ; ಕೆಲಸ = ಕೆಲಸ->jv_ಮುಂದಿನ)
	ಆದರೆ (ಕೆಲಸ_still_useful(ಕೆಲಸ))
	{
	    tv.v_type = VAR_ಕೆಲಸ;
	    tv.vval.v_ಕೆಲಸ = ಕೆಲಸ;
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    ಶೂನ್ಯ
ಕೆಲಸ_unref(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ || --ಕೆಲಸ->jv_refcount > 0)
	ಹಿಂತಿರುಗಿ;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ಆದರೆ (ಕೆಲಸ_channel_still_useful(ಕೆಲಸ))
	ಹಿಂತಿರುಗಿ;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ಆದರೆ (!ಕೆಲಸ_need_ಕೊನೆ_check(ಕೆಲಸ))
    {
	ಕೆಲಸ_free(ಕೆಲಸ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(ಕೆಲಸ->jv_channel, "detaching channel from job");
	ಕೆಲಸ->jv_channel->ch_ಕೆಲಸ = ಖಾಲಿ;
	channel_unref(ಕೆಲಸ->jv_channel);
	ಕೆಲಸ->jv_channel = ಖಾಲಿ;
    }
}

    ಪೂರ್ಣಾಂಕ
free_ಬಳಸದ_jobs_contents(ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;
    ಕೆಲಸ_T	*ಕೆಲಸ;

    FOR_ALL_JOBS(ಕೆಲಸ)
	ಆದರೆ ((ಕೆಲಸ->jv_copyID & ಮುಖವಾಡ) != (copyID & ಮುಖವಾಡ)
						    && !ಕೆಲಸ_still_useful(ಕೆಲಸ))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    ಕೆಲಸ_free_contents(ಕೆಲಸ);
	    did_free = TRUE;
	}
    ಹಿಂತಿರುಗಿ did_free;
}

    ಶೂನ್ಯ
free_ಬಳಸದ_jobs(ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;
    ಕೆಲಸ_T	*ಕೆಲಸ_ಮುಂದಿನ;

    ಗಾಗಿ (ಕೆಲಸ = ಮೊದಲ_ಕೆಲಸ; ಕೆಲಸ != ಖಾಲಿ; ಕೆಲಸ = ಕೆಲಸ_ಮುಂದಿನ)
    {
	ಕೆಲಸ_ಮುಂದಿನ = ಕೆಲಸ->jv_ಮುಂದಿನ;
	ಆದರೆ ((ಕೆಲಸ->jv_copyID & ಮುಖವಾಡ) != (copyID & ಮುಖವಾಡ)
						    && !ಕೆಲಸ_still_useful(ಕೆಲಸ))
	{
	    // Free the job struct itself.
	    ಕೆಲಸ_free_ಕೆಲಸ(ಕೆಲಸ);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    ಕೆಲಸ_T *
ಕೆಲಸ_alloc(ಶೂನ್ಯ)
{
    ಕೆಲಸ_T *ಕೆಲಸ;

    ಕೆಲಸ = ALLOC_CLEAR_ONE(ಕೆಲಸ_T);
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಕೆಲಸ->jv_refcount = 1;
    ಕೆಲಸ->jv_stoponexit = vim_strsave((char_u *)"term");

    ಆದರೆ (ಮೊದಲ_ಕೆಲಸ != ಖಾಲಿ)
    {
	ಮೊದಲ_ಕೆಲಸ->jv_prev = ಕೆಲಸ;
	ಕೆಲಸ->jv_ಮುಂದಿನ = ಮೊದಲ_ಕೆಲಸ;
    }
    ಮೊದಲ_ಕೆಲಸ = ಕೆಲಸ;
    ಹಿಂತಿರುಗಿ ಕೆಲಸ;
}

    ಶೂನ್ಯ
ಕೆಲಸ_ಗಣ_options(ಕೆಲಸ_T *ಕೆಲಸ, jobopt_T *opt)
{
    ಆದರೆ (opt->jo_ಗಣ & JO_STOPONEXIT)
    {
	vim_free(ಕೆಲಸ->jv_stoponexit);
	ಆದರೆ (opt->jo_stoponexit == ಖಾಲಿ || *opt->jo_stoponexit == NUL)
	    ಕೆಲಸ->jv_stoponexit = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೆಲಸ->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ಆದರೆ (opt->jo_ಗಣ & JO_EXIT_CB)
    {
	free_callback(&ಕೆಲಸ->jv_exit_cb);
	ಆದರೆ (opt->jo_exit_cb.cb_name == ಖಾಲಿ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    ಕೆಲಸ->jv_exit_cb.cb_name = ಖಾಲಿ;
	    ಕೆಲಸ->jv_exit_cb.cb_partial = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    copy_callback(&ಕೆಲಸ->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    ಶೂನ್ಯ
ಕೆಲಸ_stop_on_exit(ಶೂನ್ಯ)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;

    FOR_ALL_JOBS(ಕೆಲಸ)
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_STARTED && ಕೆಲಸ->jv_stoponexit != ಖಾಲಿ)
	    mch_signal_ಕೆಲಸ(ಕೆಲಸ, ಕೆಲಸ->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    ಪೂರ್ಣಾಂಕ
has_pending_ಕೆಲಸ(ಶೂನ್ಯ)
{
    ಕೆಲಸ_T	    *ಕೆಲಸ;

    FOR_ALL_JOBS(ಕೆಲಸ)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ಆದರೆ ((ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_STARTED && !ಕೆಲಸ_channel_still_useful(ಕೆಲಸ))
		    || (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ
					      && ಕೆಲಸ_channel_can_close(ಕೆಲಸ)))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_check_ended(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		did_ಕೊನೆ = FALSE;

    // be quick if there are no jobs to check
    ಆದರೆ (ಮೊದಲ_ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ did_ಕೊನೆ;

    ಗಾಗಿ (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	ಕೆಲಸ_T	*ಕೆಲಸ = mch_detect_ended_ಕೆಲಸ(ಮೊದಲ_ಕೆಲಸ);

	ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	did_ಕೊನೆ = TRUE;
	ಕೆಲಸ_cleanup(ಕೆಲಸ); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_ಗಮ್ಯ_free_later();

    ಆದರೆ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    ಹಿಂತಿರುಗಿ did_ಕೊನೆ;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    ಕೆಲಸ_T *
ಕೆಲಸ_ಆರಂಭ(
	typval_T    *argvars,
	ಅಕ್ಷರ	    **argv_arg ಬಳಸದ,
	jobopt_T    *opt_arg,
	ಕೆಲಸ_T	    **ಪದ_ಕೆಲಸ)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;
    char_u	*cmd = ಖಾಲಿ;
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argc = 0;
    ಪೂರ್ಣಾಂಕ		i;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_ARGV
    garray_T	ga;
#ಆದರೆ_ಅಂತ್ಯ
    jobopt_T	opt;
    ch_part_T	part;

    ಕೆಲಸ = ಕೆಲಸ_alloc();
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_FAILED;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_ARGV
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ*), 20);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (opt_arg != ಖಾಲಿ)
	opt = *opt_arg;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Default mode is NL.
	clear_ಕೆಲಸ_options(&opt);
	opt.jo_ಬಹುಲಕ = CH_ಬಹುಲಕ_NL;
	ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt,
		    JO_ಬಹುಲಕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_ಹೊರಹೋಗು_IO + JO_ಖಂಡ_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    ಹೋಗು theend;
    }

    // Check that when io is "file" that there is a file name.
    ಗಾಗಿ (part = PART_ಹೊರಹೋಗು; part < PART_ಎಣಿಕೆ; ++part)
	ಆದರೆ ((opt.jo_ಗಣ & (JO_ಹೊರಹೋಗು_IO << (part - PART_ಹೊರಹೋಗು)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_ಗಣ & (JO_ಹೊರಹೋಗು_NAME << (part - PART_ಹೊರಹೋಗು)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_ಗಮ್ಯ_be_ಗಣ));
	    ಹೋಗು theend;
	}

    ಆದರೆ ((opt.jo_ಗಣ & JO_ಒಳ_IO) && opt.jo_io[PART_ಒಳ] == JIO_ಬಫರ್)
    {
	buf_T *buf = ಖಾಲಿ;

	// check that we can find the buffer before starting the job
	ಆದರೆ (opt.jo_ಗಣ & JO_ಒಳ_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_ಒಳ]);
	    ಆದರೆ (buf == ಖಾಲಿ)
		semsg(_(e_ಬಫರ್_nr_does_not_exist),
						 (ದೀರ್ಘ)opt.jo_io_buf[PART_ಒಳ]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(opt.jo_ಗಣ & JO_ಒಳ_NAME))
	{
	    emsg(_(e_ಒಳ_io_ಬಫರ್_requires_ಒಳ_buf_or_ಒಳ_name_ಗಮ್ಯ_be_ಗಣ));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf = buflist_ಕಂಡುಹಿಡಿ_by_name(opt.jo_io_name[PART_ಒಳ], FALSE);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ಆದರೆ (opt.jo_ಗಣ & JO_ಒಳ_BUF)
	    {
		sprintf((ಅಕ್ಷರ *)numbuf, "%d", opt.jo_io_buf[PART_ಒಳ]);
		s = numbuf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		s = opt.jo_io_name[PART_ಒಳ];
	    semsg(_(e_ಬಫರ್_must_be_loaded_str), s);
	    ಹೋಗು theend;
	}
	ಕೆಲಸ->jv_ಒಳ_buf = buf;
    }

    ಕೆಲಸ_ಗಣ_options(ಕೆಲಸ, &opt);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARGV
    ಆದರೆ (argv_arg != ಖಾಲಿ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	ಗಾಗಿ (i = 0; argv_arg[i] != ಖಾಲಿ; i++)
	    argc++;
	argv = ALLOC_MULT(ಅಕ್ಷರ *, argc + 1);
	ಆದರೆ (argv == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಗಾಗಿ (i = 0; i < argc; i++)
	    argv[i] = (ಅಕ್ಷರ *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ಆದರೆ (cmd == ಖಾಲಿ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹೋಗು theend;
	}

	ಆದರೆ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ಖಾಲಿ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	list_T *l = argvars[0].vval.v_list;

	ಆದರೆ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ಹೋಗು theend;

	// Empty command is invalid.
	ಆದರೆ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹೋಗು theend;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_ARGV
	ಆದರೆ (win32_build_cmd(l, &ga) == FAIL)
	    ಹೋಗು theend;
	cmd = ga.ga_ದತ್ತಾಂಶ;
	ಆದರೆ (cmd == ಖಾಲಿ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹೋಗು theend;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Save the command used to start the job.
    ಕೆಲಸ->jv_argv = argv;

    ಆದರೆ (ಪದ_ಕೆಲಸ != ಖಾಲಿ)
	*ಪದ_ಕೆಲಸ = ಕೆಲಸ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARGV
    ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
    {
	garray_T    ga;

	ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 200);
	ಗಾಗಿ (i = 0; i < argc; ++i)
	{
	    ಆದರೆ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ಖಾಲಿ, "Starting job: %s", (ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ);
	ga_clear(&ga);
    }
    mch_ಕೆಲಸ_ಆರಂಭ(argv, ಕೆಲಸ, &opt, ಪದ_ಕೆಲಸ != ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    ch_log(ಖಾಲಿ, "Starting job: %s", (ಅಕ್ಷರ *)cmd);
    mch_ಕೆಲಸ_ಆರಂಭ((ಅಕ್ಷರ *)cmd, ಕೆಲಸ, &opt);
#ಆದರೆ_ಅಂತ್ಯ

    // If the channel is reading from a buffer, write lines now.
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
	channel_write_ಒಳ(ಕೆಲಸ->jv_channel);

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_ARGV
    vim_free(ga.ga_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (argv != ಖಾಲಿ && argv != ಕೆಲಸ->jv_argv)
    {
	ಗಾಗಿ (i = 0; argv[i] != ಖಾಲಿ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_ಕೆಲಸ_options(&opt);
    ಹಿಂತಿರುಗಿ ಕೆಲಸ;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    ಅಕ್ಷರ *
ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಅಕ್ಷರ	*ಫಲಿತಾಂಶ;

    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ >= ಕೆಲಸ_ENDED)
	// No need to check, dead is dead.
	ಫಲಿತಾಂಶ = "dead";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_FAILED)
	ಫಲಿತಾಂಶ = "fail";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಫಲಿತಾಂಶ = mch_ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ);
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_ENDED)
	    ಕೆಲಸ_cleanup(ಕೆಲಸ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    ಪೂರ್ಣಾಂಕ
ಕೆಲಸ_stop(ಕೆಲಸ_T *ಕೆಲಸ, typval_T *argvars, ಅಕ್ಷರ *type)
{
    char_u *arg;

    ಆದರೆ (type != ಖಾಲಿ)
	arg = (char_u *)type;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	arg = tv_get_string_chk(&argvars[1]);
	ಆದರೆ (arg == ಖಾಲಿ)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ 0;
	}
    }
    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_FAILED)
    {
	ch_log(ಕೆಲಸ->jv_channel, "Job failed to start, job_stop() skipped");
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_ENDED)
    {
	ch_log(ಕೆಲಸ->jv_channel, "Job has already ended, job_stop() skipped");
	ಹಿಂತಿರುಗಿ 0;
    }
    ch_log(ಕೆಲಸ->jv_channel, "Stopping job with '%s'", (ಅಕ್ಷರ *)arg);
    ಆದರೆ (mch_signal_ಕೆಲಸ(ಕೆಲಸ, arg) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    // Assume that only "kill" will kill the job.
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ && STRCMP(arg, "kill") == 0)
	ಕೆಲಸ->jv_channel->ch_ಕೆಲಸ_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    ಹಿಂತಿರುಗಿ 1;
}

    ಶೂನ್ಯ
invoke_prompt_callback(ಶೂನ್ಯ)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*ಪಠ್ಯ;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಆದರೆ (curbuf->b_prompt_callback.cb_name == ಖಾಲಿ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	ಹಿಂತಿರುಗಿ;
    ಪಠ್ಯ = ml_get(lnum);
    prompt = prompt_ಪಠ್ಯ();
    ಆದರೆ (STRLEN(ಪಠ್ಯ) >= STRLEN(prompt))
	ಪಠ್ಯ += STRLEN(prompt);
    argv[0].v_type = VAR_STRING;
    argv[0].vval.v_string = vim_strsave(ಪಠ್ಯ);
    argv[1].v_type = VAR_UNKNOWN;

    call_callback(&curbuf->b_prompt_callback, -1, &rettv, 1, argv);
    clear_tv(&argv[0]);
    clear_tv(&rettv);
}

/*
 * Return TRUE when the interrupt callback was invoked.
 */
    ಪೂರ್ಣಾಂಕ
invoke_prompt_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    typval_T	rettv;
    typval_T	argv[1];
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (curbuf->b_prompt_ಅಡಚಣೆ.cb_name == ಖಾಲಿ
	    || *curbuf->b_prompt_ಅಡಚಣೆ.cb_name == NUL)
	ಹಿಂತಿರುಗಿ FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_ಅಡಚಣೆ, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    ಹಿಂತಿರುಗಿ ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    ಸ್ಥಿರ char_u *
buf_prompt_ಪಠ್ಯ(buf_T* buf)
{
    ಆದರೆ (buf->b_prompt_ಪಠ್ಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (char_u *)"% ";
    ಹಿಂತಿರುಗಿ buf->b_prompt_ಪಠ್ಯ;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_ಪಠ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ buf_prompt_ಪಠ್ಯ(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    ಶೂನ್ಯ
init_prompt(ಪೂರ್ಣಾಂಕ cmdchar_todo)
{
    char_u *prompt = prompt_ಪಠ್ಯ();
    char_u *ಪಠ್ಯ;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪಠ್ಯ = ml_get_curline();
    ಆದರೆ (STRNCMP(ಪಠ್ಯ, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ಆದರೆ (*ಪಠ್ಯ == NUL)
	    ml_replace(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, prompt, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	coladvance((colnr_T)MAXCOL);
	ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ಆದರೆ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.ಲಂಬಸಾಲು != (ಪೂರ್ಣಾಂಕ)STRLEN(prompt))
	ಗಣ_insstart(curwin->w_cursor.lnum, (ಪೂರ್ಣಾಂಕ)STRLEN(prompt));

    ಆದರೆ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < (ಪೂರ್ಣಾಂಕ)STRLEN(prompt))
	curwin->w_cursor.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    ಪೂರ್ಣಾಂಕ
prompt_curpos_editable(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	&& curwin->w_cursor.ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)STRLEN(prompt_ಪಠ್ಯ());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    ಶೂನ್ಯ
f_prompt_setcallback(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    callback_T	callback;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    callback = get_callback(&argvars[1]);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    free_callback(&buf->b_prompt_callback);
    ಗಣ_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    ಶೂನ್ಯ
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    callback_T	callback;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    callback = get_callback(&argvars[1]);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    free_callback(&buf->b_prompt_ಅಡಚಣೆ);
    ಗಣ_callback(&buf->b_prompt_ಅಡಚಣೆ, &callback);
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
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!bt_prompt(buf))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string = vim_strsave(buf_prompt_ಪಠ್ಯ(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    ಶೂನ್ಯ
f_prompt_setprompt(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    char_u	*ಪಠ್ಯ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;
    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪಠ್ಯ = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_ಪಠ್ಯ);
    buf->b_prompt_ಪಠ್ಯ = vim_strsave(ಪಠ್ಯ);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    ಸ್ಥಿರ ಕೆಲಸ_T *
get_ಕೆಲಸ_arg(typval_T *tv)
{
    ಕೆಲಸ_T *ಕೆಲಸ;

    ಆದರೆ (tv->v_type != VAR_ಕೆಲಸ)
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(tv));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಕೆಲಸ = tv->vval.v_ಕೆಲಸ;

    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	emsg(_(e_not_valid_ಕೆಲಸ));
    ಹಿಂತಿರುಗಿ ಕೆಲಸ;
}

/*
 * "job_getchannel()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_getchannel(typval_T *argvars, typval_T *rettv)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಕೆಲಸ = get_ಕೆಲಸ_arg(&argvars[0]);
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = ಕೆಲಸ->jv_channel;
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
	++ಕೆಲಸ->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_info(ಕೆಲಸ_T *ಕೆಲಸ, dict_T *dict)
{
    dictitem_T	*ಅಂಶ;
    varnumber_T	nr;
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		i;

    dict_ಕೂಡಿಸು_string(dict, "status", (char_u *)ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ));

    ಅಂಶ = dictitem_alloc((char_u *)"channel");
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಅಂಶ->di_tv.v_type = VAR_CHANNEL;
    ಅಂಶ->di_tv.vval.v_channel = ಕೆಲಸ->jv_channel;
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
	++ಕೆಲಸ->jv_channel->ch_refcount;
    ಆದರೆ (dict_ಕೂಡಿಸು(dict, ಅಂಶ) == FAIL)
	dictitem_free(ಅಂಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    nr = ಕೆಲಸ->jv_pid;
#ಇಲ್ಲದಿದ್ದರೆ
    nr = ಕೆಲಸ->jv_proc_info.dwProcessId;
#ಆದರೆ_ಅಂತ್ಯ
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "process", nr);
    dict_ಕೂಡಿಸು_string(dict, "tty_in", ಕೆಲಸ->jv_tty_ಒಳ);
    dict_ಕೂಡಿಸು_string(dict, "tty_out", ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "exitval", ಕೆಲಸ->jv_exitval);
    dict_ಕೂಡಿಸು_string(dict, "exit_cb", ಕೆಲಸ->jv_exit_cb.cb_name);
    dict_ಕೂಡಿಸು_string(dict, "stoponexit", ಕೆಲಸ->jv_stoponexit);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    dict_ಕೂಡಿಸು_string(dict, "termsig", ಕೆಲಸ->jv_termsig);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    dict_ಕೂಡಿಸು_string(dict, "tty_type", ಕೆಲಸ->jv_tty_type);
#ಆದರೆ_ಅಂತ್ಯ

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಕೆಲಸ->jv_argv != ಖಾಲಿ)
	ಗಾಗಿ (i = 0; ಕೆಲಸ->jv_argv[i] != ಖಾಲಿ; i++)
	    list_append_string(l, (char_u *)ಕೆಲಸ->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೆಲಸ_info_all(list_T *l)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;
    typval_T	tv;

    FOR_ALL_JOBS(ಕೆಲಸ)
    {
	tv.v_type = VAR_ಕೆಲಸ;
	tv.vval.v_ಕೆಲಸ = ಕೆಲಸ;

	ಆದರೆ (list_append_tv(l, &tv) != OK)
	    ಹಿಂತಿರುಗಿ;
    }
}

/*
 * "job_info()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_info(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಕೆಲಸ_T	*ಕೆಲಸ;

	ಕೆಲಸ = get_ಕೆಲಸ_arg(&argvars[0]);
	ಆದರೆ (ಕೆಲಸ != ಖಾಲಿ && rettv_dict_alloc(rettv) == OK)
	    ಕೆಲಸ_info(ಕೆಲಸ, rettv->vval.v_dict);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv_list_alloc(rettv) == OK)
	ಕೆಲಸ_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_setoptions(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;
    jobopt_T	opt;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಕೆಲಸ = get_ಕೆಲಸ_arg(&argvars[0]);
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    clear_ಕೆಲಸ_options(&opt);
    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	ಕೆಲಸ_ಗಣ_options(ಕೆಲಸ, &opt);
    free_ಕೆಲಸ_options(&opt);
}

/*
 * "job_start()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_ಆರಂಭ(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_ಕೆಲಸ;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಕೆಲಸ = ಕೆಲಸ_ಆರಂಭ(argvars, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * "job_status()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_ಸ್ಥಿತಿ(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_ಕೆಲಸ && argvars[0].vval.v_ಕೆಲಸ == ಖಾಲಿ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೆಲಸ_T	*ಕೆಲಸ = get_ಕೆಲಸ_arg(&argvars[0]);

	ಆದರೆ (ಕೆಲಸ != ಖಾಲಿ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ));
	}
    }
}

/*
 * "job_stop()" function
 */
    ಶೂನ್ಯ
f_ಕೆಲಸ_stop(typval_T *argvars, typval_T *rettv)
{
    ಕೆಲಸ_T	*ಕೆಲಸ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಕೆಲಸ = get_ಕೆಲಸ_arg(&argvars[0]);
    ಆದರೆ (ಕೆಲಸ != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = ಕೆಲಸ_stop(ಕೆಲಸ, argvars, ಖಾಲಿ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
ಕೆಲಸ_ಗಮ್ಯ_string_buf(typval_T *varp, char_u *buf)
{
    ಕೆಲಸ_T *ಕೆಲಸ = varp->vval.v_ಕೆಲಸ;
    ಅಕ್ಷರ  *ಸ್ಥಿತಿ_2;

    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
    {
	vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN, "no process");
	ಹಿಂತಿರುಗಿ buf;
    }
    ಸ್ಥಿತಿ_2 = ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_FAILED ? "fail"
		    : ಕೆಲಸ->jv_ಸ್ಥಿತಿ >= ಕೆಲಸ_ENDED ? "dead"
		    : "run";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN,
		"process %ld %s", (ದೀರ್ಘ)ಕೆಲಸ->jv_pid, ಸ್ಥಿತಿ_2);
#ಅಥವಾಆದರೆ defined(MSWIN)
    vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN,
		"process %ld %s",
		(ದೀರ್ಘ)ಕೆಲಸ->jv_proc_info.dwProcessId,
		ಸ್ಥಿತಿ_2);
#ಇಲ್ಲದಿದ್ದರೆ
    // fall-back
    vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN, "process ? %s", ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ buf;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_JOB_CHANNEL
