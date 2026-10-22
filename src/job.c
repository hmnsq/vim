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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_పని_CHANNEL)

#నిర్వచించు FOR_ALL_JOBS(పని) \
    కొరకు ((పని) = మొదటి_పని; (పని) != ఖాళీ; (పని) = (పని)->jv_తదుపరి)

    స్థిర పూర్ణాంకం
handle_బహుళకం(typval_T *అంశం, jobopt_T *opt, ch_బహుళకం_T *modep, పూర్ణాంకం jo)
{
    char_u	*val = tv_get_string(అంశం);

    opt->jo_సమితి |= jo;
    ఒకవేళ (STRCMP(val, "nl") == 0)
	*modep = CH_బహుళకం_NL;
    లేకపోతే ఒకవేళ (STRCMP(val, "raw") == 0)
	*modep = CH_బహుళకం_RAW;
    లేకపోతే ఒకవేళ (STRCMP(val, "blob") == 0)
	*modep = CH_బహుళకం_BLOB;
    లేకపోతే ఒకవేళ (STRCMP(val, "js") == 0)
	*modep = CH_బహుళకం_JS;
    లేకపోతే ఒకవేళ (STRCMP(val, "json") == 0)
	*modep = CH_బహుళకం_JSON;
    లేకపోతే ఒకవేళ (STRCMP(val, "lsp") == 0)
	*modep = CH_బహుళకం_LSP;
    లేకపోతే ఒకవేళ (STRCMP(val, "dap") == 0)
	*modep = CH_బహుళకం_DAP;
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str), val);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
handle_io(typval_T *అంశం, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(అంశం);

    opt->jo_సమితి |= JO_బహిర్గతం_IO << (part - PART_బహిర్గతం);
    ఒకవేళ (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    లేకపోతే ఒకవేళ (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    లేకపోతే ఒకవేళ (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    లేకపోతే ఒకవేళ (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_బఫరు;
    లేకపోతే ఒకవేళ (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_బహిర్గతం;
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str), val);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    శూన్య
clear_పని_options(jobopt_T *opt)
{
    CLEAR_సూచిక(opt);
}

    స్థిర శూన్య
unref_పని_callback(callback_T *cb)
{
    ఒకవేళ (cb->cb_partial != ఖాళీ)
	partial_unref(cb->cb_partial);
    లేకపోతే ఒకవేళ (cb->cb_name != ఖాళీ)
    {
	func_unref(cb->cb_name);
	ఒకవేళ (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    శూన్య
free_పని_options(jobopt_T *opt)
{
    unref_పని_callback(&opt->jo_callback);
    unref_పని_callback(&opt->jo_బహిర్గతం_cb);
    unref_పని_callback(&opt->jo_err_cb);
    unref_పని_callback(&opt->jo_close_cb);
    unref_పని_callback(&opt->jo_exit_cb);

    ఒకవేళ (opt->jo_env != ఖాళీ)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    స్థిర పూర్ణాంకం
part_from_char(పూర్ణాంకం c)
{
    తిరిగి c == 'i' ? PART_లోపల : c == 'o' ? PART_బహిర్గతం: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    పూర్ణాంకం
get_పని_options(typval_T *tv, jobopt_T *opt, పూర్ణాంకం supported, పూర్ణాంకం supported2)
{
    typval_T	*అంశం;
    char_u	*val;
    dict_T	*dict;
    పూర్ణాంకం		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ఒకవేళ (tv->v_type == VAR_UNKNOWN)
	తిరిగి OK;
    ఒకవేళ (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	తిరిగి FAIL;
    }
    dict = tv->vval.v_dict;
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి OK;

    todo = (పూర్ణాంకం)dict->dv_hashtab.ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(&dict->dv_hashtab, hi, todo)
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    అంశం = &dict_lookup(hi)->di_tv;

	    ఒకవేళ (STRCMP(hi->hi_కీలకం, "mode") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహుళకం))
		    ఆపు;
		ఒకవేళ (handle_బహుళకం(అంశం, opt, &opt->jo_బహుళకం, JO_బహుళకం) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "in_mode") == 0)
	    {
		ఒకవేళ (!(supported & JO_లోపల_బహుళకం))
		    ఆపు;
		ఒకవేళ (handle_బహుళకం(అంశం, opt, &opt->jo_లోపల_బహుళకం, JO_లోపల_బహుళకం)
								      == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "out_mode") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహిర్గతం_బహుళకం))
		    ఆపు;
		ఒకవేళ (handle_బహుళకం(అంశం, opt, &opt->jo_బహిర్గతం_బహుళకం, JO_బహిర్గతం_బహుళకం)
								      == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "err_mode") == 0)
	    {
		ఒకవేళ (!(supported & JO_ERR_బహుళకం))
		    ఆపు;
		ఒకవేళ (handle_బహుళకం(అంశం, opt, &opt->jo_err_బహుళకం, JO_ERR_బహుళకం)
								      == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "noblock") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహుళకం))
		    ఆపు;
		opt->jo_noblock = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "in_io") == 0
		    || STRCMP(hi->hi_కీలకం, "out_io") == 0
		    || STRCMP(hi->hi_కీలకం, "err_io") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		ఒకవేళ (handle_io(అంశం, part_from_char(*hi->hi_కీలకం), opt) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "in_name") == 0
		    || STRCMP(hi->hi_కీలకం, "out_name") == 0
		    || STRCMP(hi->hi_కీలకం, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_కీలకం);

		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		opt->jo_సమితి |= JO_బహిర్గతం_NAME << (part - PART_బహిర్గతం);
		opt->jo_io_name[part] = tv_get_string_buf_chk(అంశం,
						   opt->jo_io_name_buf[part]);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "pty") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహుళకం))
		    ఆపు;
		opt->jo_pty = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "in_buf") == 0
		    || STRCMP(hi->hi_కీలకం, "out_buf") == 0
		    || STRCMP(hi->hi_కీలకం, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_కీలకం);

		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		opt->jo_సమితి |= JO_బహిర్గతం_BUF << (part - PART_బహిర్గతం);
		opt->jo_io_buf[part] = tv_get_సంఖ్య(అంశం);
		ఒకవేళ (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str),
					      hi->hi_కీలకం, tv_get_string(అంశం));
		    తిరిగి FAIL;
		}
		ఒకవేళ (buflist_findnr(opt->jo_io_buf[part]) == ఖాళీ)
		{
		    semsg(_(e_బఫరు_nr_does_not_exist),
						   (దీర్ఘ)opt->jo_io_buf[part]);
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "out_modifiable") == 0
		    || STRCMP(hi->hi_కీలకం, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_కీలకం);

		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		opt->jo_సమితి |= JO_బహిర్గతం_MODIFIABLE << (part - PART_బహిర్గతం);
		opt->jo_modifiable[part] = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "out_msg") == 0
		    || STRCMP(hi->hi_కీలకం, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_కీలకం);

		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		opt->jo_set2 |= JO2_బహిర్గతం_సందేశం << (part - PART_బహిర్గతం);
		opt->jo_సందేశం[part] = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "in_top") == 0
		    || STRCMP(hi->hi_కీలకం, "in_bot") == 0)
	    {
		linenr_T *lp;

		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		ఒకవేళ (hi->hi_కీలకం[3] == 't')
		{
		    lp = &opt->jo_లోపల_శిఖరం;
		    opt->jo_సమితి |= JO_లోపల_శిఖరం;
		}
		లేకపోతే
		{
		    lp = &opt->jo_లోపల_bot;
		    opt->jo_సమితి |= JO_లోపల_BOT;
		}
		*lp = tv_get_సంఖ్య(అంశం);
		ఒకవేళ (*lp < 0)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str),
					      hi->hi_కీలకం, tv_get_string(అంశం));
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "channel") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహిర్గతం_IO))
		    ఆపు;
		opt->jo_సమితి |= JO_CHANNEL;
		ఒకవేళ (అంశం->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "channel");
		    తిరిగి FAIL;
		}
		opt->jo_channel = అంశం->vval.v_channel;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "callback") == 0)
	    {
		ఒకవేళ (!(supported & JO_CALLBACK))
		    ఆపు;
		opt->jo_సమితి |= JO_CALLBACK;
		opt->jo_callback = get_callback(అంశం);
		ఒకవేళ (opt->jo_callback.cb_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "callback");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "out_cb") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహిర్గతం_CALLBACK))
		    ఆపు;
		opt->jo_సమితి |= JO_బహిర్గతం_CALLBACK;
		opt->jo_బహిర్గతం_cb = get_callback(అంశం);
		ఒకవేళ (opt->jo_బహిర్గతం_cb.cb_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "out_cb");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "err_cb") == 0)
	    {
		ఒకవేళ (!(supported & JO_ERR_CALLBACK))
		    ఆపు;
		opt->jo_సమితి |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(అంశం);
		ఒకవేళ (opt->jo_err_cb.cb_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "err_cb");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "close_cb") == 0)
	    {
		ఒకవేళ (!(supported & JO_CLOSE_CALLBACK))
		    ఆపు;
		opt->jo_సమితి |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(అంశం);
		ఒకవేళ (opt->jo_close_cb.cb_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "close_cb");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "drop") == 0)
	    {
		పూర్ణాంకం never = FALSE;
		val = tv_get_string(అంశం);

		ఒకవేళ (STRCMP(val, "never") == 0)
		    never = TRUE;
		లేకపోతే ఒకవేళ (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "drop", val);
		    తిరిగి FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "exit_cb") == 0)
	    {
		ఒకవేళ (!(supported & JO_EXIT_CB))
		    ఆపు;
		opt->jo_సమితి |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(అంశం);
		ఒకవేళ (opt->jo_exit_cb.cb_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "exit_cb");
		    తిరిగి FAIL;
		}
	    }
#నిర్వచితమైతే FEAT_TERMINAL
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_name") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_పదం_NAME))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_NAME;
		opt->jo_పదం_name = tv_get_string_buf_chk(అంశం,
						       opt->jo_పదం_name_buf);
		ఒకవేళ (opt->jo_పదం_name == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_name");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_finish") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_పదం_ముగింపు))
		    ఆపు;
		val = tv_get_string(అంశం);
		ఒకవేళ (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str),
							   "term_finish", val);
		    తిరిగి FAIL;
		}
		opt->jo_set2 |= JO2_పదం_ముగింపు;
		opt->jo_పదం_ముగింపు = *val;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_opencmd") == 0)
	    {
		char_u *p;

		ఒకవేళ (!(supported2 & JO2_పదం_OPENCMD))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_OPENCMD;
		p = opt->jo_పదం_opencmd = tv_get_string_buf_chk(అంశం,
						    opt->jo_పదం_opencmd_buf);
		ఒకవేళ (p != ఖాళీ)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ఒకవేళ (p != ఖాళీ && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ఖాళీ))
			p = ఖాళీ;
		}
		ఒకవేళ (p == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_opencmd");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "eof_chars") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_EOF_CHARS))
		    ఆపు;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(అంశం,
						       opt->jo_eof_chars_buf);
		ఒకవేళ (opt->jo_eof_chars == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "eof_chars");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_rows") == 0)
	    {
		పూర్ణాంకం error = FALSE;

		ఒకవేళ (!(supported2 & JO2_పదం_అడ్డువరుసలు))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_అడ్డువరుసలు;
		opt->jo_పదం_అడ్డువరుసలు = tv_get_సంఖ్య_chk(అంశం, &error);
		ఒకవేళ (error)
		    తిరిగి FAIL;
		ఒకవేళ (opt->jo_పదం_అడ్డువరుసలు < 0 || opt->jo_పదం_అడ్డువరుసలు > 1000)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_rows");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_cols") == 0)
	    {
		పూర్ణాంకం error = FALSE;

		ఒకవేళ (!(supported2 & JO2_పదం_నిలువుల_సంఖ్య))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_నిలువుల_సంఖ్య;
		opt->jo_పదం_నిలువుల_సంఖ్య = tv_get_సంఖ్య_chk(అంశం, &error);
		ఒకవేళ (error)
		    తిరిగి FAIL;
		ఒకవేళ (opt->jo_పదం_నిలువుల_సంఖ్య < 0 || opt->jo_పదం_నిలువుల_సంఖ్య > 1000)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_cols");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "vertical") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_VERTICAL))
		    ఆపు;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "curwin") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_CURWIN))
		    ఆపు;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "bufnr") == 0)
	    {
		పూర్ణాంకం nr;

		ఒకవేళ (!(supported2 & JO2_CURWIN))
		    ఆపు;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_సంఖ్య(అంశం);
		ఒకవేళ (nr <= 0)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str), hi->hi_కీలకం, tv_get_string(అంశం));
		    తిరిగి FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ఒకవేళ (opt->jo_bufnr_buf == ఖాళీ)
		{
		    semsg(_(e_బఫరు_nr_does_not_exist), (దీర్ఘ)nr);
		    తిరిగి FAIL;
		}
		ఒకవేళ (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_పదం == ఖాళీ)
		{
		    semsg(_(e_invalid_వాదం_str), "bufnr");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "hidden") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_HIDDEN))
		    ఆపు;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "norestore") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_NORESTORE))
		    ఆపు;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_పదం_norestore = tv_get_bool(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_kill") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_పదం_KILL))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_KILL;
		opt->jo_పదం_kill = tv_get_string_buf_chk(అంశం,
						       opt->jo_పదం_kill_buf);
		ఒకవేళ (opt->jo_పదం_kill == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_kill");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "tty_type") == 0)
	    {
		char_u *p;

		ఒకవేళ (!(supported2 & JO2_TTY_TYPE))
		    ఆపు;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(అంశం);
		ఒకవేళ (p == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "tty_type");
		    తిరిగి FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ఒకవేళ (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "tty_type");
		    తిరిగి FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "ansi_colors") == 0)
	    {
		పూర్ణాంకం		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ఒకవేళ (!(supported2 & JO2_ANSI_COLORS))
		    ఆపు;

		ఒకవేళ (అంశం == ఖాళీ || అంశం->v_type != VAR_LIST
			|| అంశం->vval.v_list == ఖాళీ
			|| అంశం->vval.v_list->lv_మొదటి == &range_list_అంశం)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "ansi_colors");
		    తిరిగి FAIL;
		}

		li = అంశం->vval.v_list->lv_మొదటి;
		కొరకు (; li != ఖాళీ && n < 16; li = li->li_తదుపరి, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    పూర్ణాంకం		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ఒకవేళ (color_name == ఖాళీ)
			తిరిగి FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ఒకవేళ (guicolor == INVALCOLOR)
		    {
			ఒకవేళ (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			తిరిగి FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ఒకవేళ (n != 16 || li != ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "ansi_colors");
		    తిరిగి FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		జ్ఞాపకం_నకలు(opt->jo_ansi_colors, rgb, పరిమాణం(rgb));
	    }
# endif
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_highlight") == 0)
	    {
		char_u *p;

		ఒకవేళ (!(supported2 & JO2_పదం_HIGHLIGHT))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_HIGHLIGHT;
		p = tv_get_string_buf_chk(అంశం, opt->jo_పదం_highlight_buf);
		ఒకవేళ (p == ఖాళీ || *p == NUL)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_highlight");
		    తిరిగి FAIL;
		}
		opt->jo_పదం_highlight = p;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "term_api") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_పదం_API))
		    ఆపు;
		opt->jo_set2 |= JO2_పదం_API;
		opt->jo_పదం_api = tv_get_string_buf_chk(అంశం,
							opt->jo_పదం_api_buf);
		ఒకవేళ (opt->jo_పదం_api == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "term_api");
		    తిరిగి FAIL;
		}
	    }
#అయితే_ముగింపు
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "env") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_ENV))
		    ఆపు;
		ఒకవేళ (అంశం->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "env");
		    తిరిగి FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = అంశం->vval.v_dict;
		ఒకవేళ (opt->jo_env != ఖాళీ)
		    ++opt->jo_env->dv_refcount;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "cwd") == 0)
	    {
		ఒకవేళ (!(supported2 & JO2_CWD))
		    ఆపు;
		opt->jo_cwd = tv_get_string_buf_chk(అంశం, opt->jo_cwd_buf);
		ఒకవేళ (opt->jo_cwd == ఖాళీ || !mch_isdir(opt->jo_cwd)
#నిర్వచితం_కాకపోతే MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((అక్షరం *)opt->jo_cwd, X_OK) != 0
#అయితే_ముగింపు
				)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "cwd");
		    తిరిగి FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "waittime") == 0)
	    {
		ఒకవేళ (!(supported & JO_WAITTIME))
		    ఆపు;
		opt->jo_సమితి |= JO_WAITTIME;
		opt->jo_waittime = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "timeout") == 0)
	    {
		ఒకవేళ (!(supported & JO_TIMEOUT))
		    ఆపు;
		opt->jo_సమితి |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "out_timeout") == 0)
	    {
		ఒకవేళ (!(supported & JO_బహిర్గతం_TIMEOUT))
		    ఆపు;
		opt->jo_సమితి |= JO_బహిర్గతం_TIMEOUT;
		opt->jo_బహిర్గతం_timeout = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "err_timeout") == 0)
	    {
		ఒకవేళ (!(supported & JO_ERR_TIMEOUT))
		    ఆపు;
		opt->jo_సమితి |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "part") == 0)
	    {
		ఒకవేళ (!(supported & JO_PART))
		    ఆపు;
		opt->jo_సమితి |= JO_PART;
		val = tv_get_string(అంశం);
		ఒకవేళ (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		లేకపోతే ఒకవేళ (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_బహిర్గతం;
		లేకపోతే
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "part", val);
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "id") == 0)
	    {
		ఒకవేళ (!(supported & JO_ID))
		    ఆపు;
		opt->jo_సమితి |= JO_ID;
		opt->jo_id = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "stoponexit") == 0)
	    {
		ఒకవేళ (!(supported & JO_STOPONEXIT))
		    ఆపు;
		opt->jo_సమితి |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(అంశం,
						      opt->jo_stoponexit_buf);
		ఒకవేళ (opt->jo_stoponexit == ఖాళీ)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "stoponexit");
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(hi->hi_కీలకం, "block_write") == 0)
	    {
		ఒకవేళ (!(supported & JO_ఖండం_WRITE))
		    ఆపు;
		opt->jo_సమితి |= JO_ఖండం_WRITE;
		opt->jo_ఖండం_write = tv_get_సంఖ్య(అంశం);
	    }
	    లేకపోతే
		ఆపు;
	    --todo;
	}
    ఒకవేళ (todo > 0)
    {
	semsg(_(e_invalid_వాదం_str), hi->hi_కీలకం);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

స్థిర పని_T *మొదటి_పని = ఖాళీ;

    స్థిర శూన్య
పని_free_contents(పని_T *పని)
{
    పూర్ణాంకం		i;

    ch_log(పని->jv_channel, "Freeing job");
    ఒకవేళ (పని->jv_channel != ఖాళీ)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	పని->jv_channel->ch_పని = ఖాళీ;
	channel_unref(పని->jv_channel);
    }
    mch_clear_పని(పని);

    vim_free(పని->jv_tty_లోపల);
    vim_free(పని->jv_tty_బహిర్గతం);
    vim_free(పని->jv_stoponexit);
#నిర్వచితమైతే UNIX
    vim_free(పని->jv_termsig);
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    vim_free(పని->jv_tty_type);
#అయితే_ముగింపు
    free_callback(&పని->jv_exit_cb);
    ఒకవేళ (పని->jv_argv != ఖాళీ)
    {
	కొరకు (i = 0; పని->jv_argv[i] != ఖాళీ; i++)
	    vim_free(పని->jv_argv[i]);
	vim_free(పని->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    స్థిర శూన్య
పని_unlink(పని_T *పని)
{
    ఒకవేళ (పని->jv_తదుపరి != ఖాళీ)
	పని->jv_తదుపరి->jv_prev = పని->jv_prev;
    ఒకవేళ (పని->jv_prev == ఖాళీ)
	మొదటి_పని = పని->jv_తదుపరి;
    లేకపోతే
	పని->jv_prev->jv_తదుపరి = పని->jv_తదుపరి;
}

    స్థిర శూన్య
పని_free_పని(పని_T *పని)
{
    పని_unlink(పని);
    vim_free(పని);
}

    స్థిర శూన్య
పని_free(పని_T *పని)
{
    ఒకవేళ (లోపల_free_unref_వస్తువులు)
	తిరిగి;

    పని_free_contents(పని);
    పని_free_పని(పని);
}

స్థిర పని_T *jobs_గమ్యం_free = ఖాళీ;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    స్థిర శూన్య
పని_free_later(పని_T *పని)
{
    పని_unlink(పని);
    పని->jv_తదుపరి = jobs_గమ్యం_free;
    jobs_గమ్యం_free = పని;
}

    స్థిర శూన్య
free_jobs_గమ్యం_free_later(శూన్య)
{
    పని_T *పని;

    అయితే (jobs_గమ్యం_free != ఖాళీ)
    {
	పని = jobs_గమ్యం_free;
	jobs_గమ్యం_free = పని->jv_తదుపరి;
	పని_free_contents(పని);
	vim_free(పని);
    }
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
పని_free_all(శూన్య)
{
    అయితే (మొదటి_పని != ఖాళీ)
	పని_free(మొదటి_పని);
    free_jobs_గమ్యం_free_later();

# ifdef FEAT_TERMINAL
    free_వాడని_terminals();
# endif
}
#అయితే_ముగింపు

/*
 * Return TRUE if we need to check if the process of "job" has ended.
 */
    స్థిర పూర్ణాంకం
పని_need_చివర_check(పని_T *పని)
{
    తిరిగి పని->jv_స్థితి == పని_STARTED
	    && (పని->jv_stoponexit != ఖాళీ || పని->jv_exit_cb.cb_name != ఖాళీ);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    స్థిర పూర్ణాంకం
పని_channel_still_useful(పని_T *పని)
{
    తిరిగి పని->jv_channel != ఖాళీ && channel_still_useful(పని->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    స్థిర పూర్ణాంకం
పని_channel_can_close(పని_T *పని)
{
    తిరిగి పని->jv_channel != ఖాళీ && channel_can_close(పని->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    స్థిర పూర్ణాంకం
పని_still_useful(పని_T *పని)
{
    తిరిగి పని_need_చివర_check(పని) || పని_channel_still_useful(పని);
}

#ఒకవేళ defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    పూర్ణాంకం
పని_any_running(శూన్య)
{
    పని_T	*పని;

    FOR_ALL_JOBS(పని)
	ఒకవేళ (పని_still_useful(పని))
	{
	    ch_log(ఖాళీ, "GUI not forking because a job is running");
	    తిరిగి TRUE;
	}
    తిరిగి FALSE;
}
#అయితే_ముగింపు

// Unix uses argv[] for the command, other systems use a string.
#ఒకవేళ defined(UNIX)
# define USE_ARGV
#అయితే_ముగింపు

#ఒకవేళ !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    స్థిర char_u *
win32_escape_arg(char_u *arg)
{
    పూర్ణాంకం		slen, dlen;
    పూర్ణాంకం		escaping = 0;
    పూర్ణాంకం		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    పూర్ణాంకం		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (పూర్ణాంకం)STRLEN(arg);
    dlen = slen;
    కొరకు (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ఒకవేళ (*s == '"' || *s == '\\')
	    ++dlen;
	ఒకవేళ (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ఒకవేళ (has_spaces)
	dlen += 2;

    ఒకవేళ (dlen == slen)
	తిరిగి vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ఒకవేళ (escaped_arg == ఖాళీ)
	తిరిగి ఖాళీ;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ఒకవేళ (has_spaces)
	*d++ = '"';

    కొరకు (s = arg; *s != NUL;)
    {
	ఎంపిక (*s)
	{
	    స్థితి '"':
		కొరకు (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		ఆపు;
	    స్థితి '\\':
		escaping++;
		*d++ = *s++;
		ఆపు;
	    అప్రమేయ:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		ఆపు;
	}
    }

    // add terminating quote and finish with a NUL
    ఒకవేళ (has_spaces)
    {
	కొరకు (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    తిరిగి escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ఒకవేళ (s == ఖాళీ)
	    తిరిగి FAIL;
	s = win32_escape_arg(s);
	ఒకవేళ (s == ఖాళీ)
	    తిరిగి FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ఒకవేళ (li->li_తదుపరి != ఖాళీ)
	    ga_append(gap, ' ');
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    శూన్య
పని_cleanup(పని_T *పని)
{
    ఒకవేళ (పని->jv_స్థితి != పని_ENDED)
	తిరిగి;

    // Ready to cleanup the job.
    పని->jv_స్థితి = పని_ముగిసిన;

    // When only channel-in is kept open, close explicitly.
    ఒకవేళ (పని->jv_channel != ఖాళీ)
	ch_close_part(పని->jv_channel, PART_లోపల);

    ఒకవేళ (పని->jv_exit_cb.cb_name != ఖాళీ)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(పని->jv_channel, "Invoking exit callback %s",
						      పని->jv_exit_cb.cb_name);
	++పని->jv_refcount;
	argv[0].v_type = VAR_పని;
	argv[0].vval.v_పని = పని;
	argv[1].v_type = VAR_సంఖ్య;
	argv[1].vval.v_సంఖ్య = పని->jv_exitval;
	call_callback(&పని->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--పని->jv_refcount;
	channel_need_redraw = TRUE;
    }

    ఒకవేళ (పని->jv_channel != ఖాళీ && పని->jv_channel->ch_anonymous_pipe)
	పని->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ఒకవేళ (పని->jv_refcount == 0 && !పని_channel_still_useful(పని))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	పని_free_later(పని);
}

/*
 * Mark references in jobs that are still useful.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_పని(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    పని_T	*పని;
    typval_T	tv;

    కొరకు (పని = మొదటి_పని; !abort && పని != ఖాళీ; పని = పని->jv_తదుపరి)
	ఒకవేళ (పని_still_useful(పని))
	{
	    tv.v_type = VAR_పని;
	    tv.vval.v_పని = పని;
	    abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    తిరిగి abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    శూన్య
పని_unref(పని_T *పని)
{
    ఒకవేళ (పని == ఖాళీ || --పని->jv_refcount > 0)
	తిరిగి;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ఒకవేళ (పని_channel_still_useful(పని))
	తిరిగి;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ఒకవేళ (!పని_need_చివర_check(పని))
    {
	పని_free(పని);
    }
    లేకపోతే ఒకవేళ (పని->jv_channel != ఖాళీ)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(పని->jv_channel, "detaching channel from job");
	పని->jv_channel->ch_పని = ఖాళీ;
	channel_unref(పని->jv_channel);
	పని->jv_channel = ఖాళీ;
    }
}

    పూర్ణాంకం
free_వాడని_jobs_contents(పూర్ణాంకం copyID, పూర్ణాంకం ముసుగు)
{
    పూర్ణాంకం		did_free = FALSE;
    పని_T	*పని;

    FOR_ALL_JOBS(పని)
	ఒకవేళ ((పని->jv_copyID & ముసుగు) != (copyID & ముసుగు)
						    && !పని_still_useful(పని))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    పని_free_contents(పని);
	    did_free = TRUE;
	}
    తిరిగి did_free;
}

    శూన్య
free_వాడని_jobs(పూర్ణాంకం copyID, పూర్ణాంకం ముసుగు)
{
    పని_T	*పని;
    పని_T	*పని_తదుపరి;

    కొరకు (పని = మొదటి_పని; పని != ఖాళీ; పని = పని_తదుపరి)
    {
	పని_తదుపరి = పని->jv_తదుపరి;
	ఒకవేళ ((పని->jv_copyID & ముసుగు) != (copyID & ముసుగు)
						    && !పని_still_useful(పని))
	{
	    // Free the job struct itself.
	    పని_free_పని(పని);
	}
    }
}

/*
 * Allocate a job.  Sets the refcount to one and sets options default.
 */
    పని_T *
పని_alloc(శూన్య)
{
    పని_T *పని;

    పని = ALLOC_CLEAR_ONE(పని_T);
    ఒకవేళ (పని == ఖాళీ)
	తిరిగి ఖాళీ;

    పని->jv_refcount = 1;
    పని->jv_stoponexit = vim_strsave((char_u *)"term");

    ఒకవేళ (మొదటి_పని != ఖాళీ)
    {
	మొదటి_పని->jv_prev = పని;
	పని->jv_తదుపరి = మొదటి_పని;
    }
    మొదటి_పని = పని;
    తిరిగి పని;
}

    శూన్య
పని_సమితి_options(పని_T *పని, jobopt_T *opt)
{
    ఒకవేళ (opt->jo_సమితి & JO_STOPONEXIT)
    {
	vim_free(పని->jv_stoponexit);
	ఒకవేళ (opt->jo_stoponexit == ఖాళీ || *opt->jo_stoponexit == NUL)
	    పని->jv_stoponexit = ఖాళీ;
	లేకపోతే
	    పని->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ఒకవేళ (opt->jo_సమితి & JO_EXIT_CB)
    {
	free_callback(&పని->jv_exit_cb);
	ఒకవేళ (opt->jo_exit_cb.cb_name == ఖాళీ || *opt->jo_exit_cb.cb_name == NUL)
	{
	    పని->jv_exit_cb.cb_name = ఖాళీ;
	    పని->jv_exit_cb.cb_partial = ఖాళీ;
	}
	లేకపోతే
	    copy_callback(&పని->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    శూన్య
పని_stop_on_exit(శూన్య)
{
    పని_T	*పని;

    FOR_ALL_JOBS(పని)
	ఒకవేళ (పని->jv_స్థితి == పని_STARTED && పని->jv_stoponexit != ఖాళీ)
	    mch_signal_పని(పని, పని->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    పూర్ణాంకం
has_pending_పని(శూన్య)
{
    పని_T	    *పని;

    FOR_ALL_JOBS(పని)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ఒకవేళ ((పని->jv_స్థితి == పని_STARTED && !పని_channel_still_useful(పని))
		    || (పని->jv_స్థితి == పని_ముగిసిన
					      && పని_channel_can_close(పని)))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

#నిర్వచించు MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    పూర్ణాంకం
పని_check_ended(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		did_చివర = FALSE;

    // be quick if there are no jobs to check
    ఒకవేళ (మొదటి_పని == ఖాళీ)
	తిరిగి did_చివర;

    కొరకు (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	పని_T	*పని = mch_detect_ended_పని(మొదటి_పని);

	ఒకవేళ (పని == ఖాళీ)
	    ఆపు;
	did_చివర = TRUE;
	పని_cleanup(పని); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_గమ్యం_free_later();

    ఒకవేళ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    తిరిగి did_చివర;
}

/*
 * Create a job and return it.  Implements job_start().
 * "argv_arg" is only for Unix.
 * When "argv_arg" is NULL then "argvars" is used.
 * The returned job has a refcount of one.
 * Returns NULL when out of memory.
 */
    పని_T *
పని_ప్రారంభం(
	typval_T    *argvars,
	అక్షరం	    **argv_arg వాడని,
	jobopt_T    *opt_arg,
	పని_T	    **పదం_పని)
{
    పని_T	*పని;
    char_u	*cmd = ఖాళీ;
    అక్షరం	**argv = ఖాళీ;
    పూర్ణాంకం		argc = 0;
    పూర్ణాంకం		i;
#నిర్వచితం_కాకపోతే USE_ARGV
    garray_T	ga;
#అయితే_ముగింపు
    jobopt_T	opt;
    ch_part_T	part;

    పని = పని_alloc();
    ఒకవేళ (పని == ఖాళీ)
	తిరిగి ఖాళీ;

    పని->jv_స్థితి = పని_FAILED;
#నిర్వచితం_కాకపోతే USE_ARGV
    ga_init2(&ga, పరిమాణం(అక్షరం*), 20);
#అయితే_ముగింపు

    ఒకవేళ (opt_arg != ఖాళీ)
	opt = *opt_arg;
    లేకపోతే
    {
	// Default mode is NL.
	clear_పని_options(&opt);
	opt.jo_బహుళకం = CH_బహుళకం_NL;
	ఒకవేళ (get_పని_options(&argvars[1], &opt,
		    JO_బహుళకం_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_బహిర్గతం_IO + JO_ఖండం_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    వెళ్ళు theend;
    }

    // Check that when io is "file" that there is a file name.
    కొరకు (part = PART_బహిర్గతం; part < PART_లెక్క; ++part)
	ఒకవేళ ((opt.jo_సమితి & (JO_బహిర్గతం_IO << (part - PART_బహిర్గతం)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_సమితి & (JO_బహిర్గతం_NAME << (part - PART_బహిర్గతం)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_గమ్యం_be_సమితి));
	    వెళ్ళు theend;
	}

    ఒకవేళ ((opt.jo_సమితి & JO_లోపల_IO) && opt.jo_io[PART_లోపల] == JIO_బఫరు)
    {
	buf_T *buf = ఖాళీ;

	// check that we can find the buffer before starting the job
	ఒకవేళ (opt.jo_సమితి & JO_లోపల_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_లోపల]);
	    ఒకవేళ (buf == ఖాళీ)
		semsg(_(e_బఫరు_nr_does_not_exist),
						 (దీర్ఘ)opt.jo_io_buf[PART_లోపల]);
	}
	లేకపోతే ఒకవేళ (!(opt.jo_సమితి & JO_లోపల_NAME))
	{
	    emsg(_(e_లోపల_io_బఫరు_requires_లోపల_buf_or_లోపల_name_గమ్యం_be_సమితి));
	}
	లేకపోతే
	    buf = buflist_కనుగొను_by_name(opt.jo_io_name[PART_లోపల], FALSE);
	ఒకవేళ (buf == ఖాళీ)
	    వెళ్ళు theend;
	ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ఒకవేళ (opt.jo_సమితి & JO_లోపల_BUF)
	    {
		sprintf((అక్షరం *)numbuf, "%d", opt.jo_io_buf[PART_లోపల]);
		s = numbuf;
	    }
	    లేకపోతే
		s = opt.jo_io_name[PART_లోపల];
	    semsg(_(e_బఫరు_must_be_loaded_str), s);
	    వెళ్ళు theend;
	}
	పని->jv_లోపల_buf = buf;
    }

    పని_సమితి_options(పని, &opt);

#నిర్వచితమైతే USE_ARGV
    ఒకవేళ (argv_arg != ఖాళీ)
    {
	// Make a copy of argv_arg for job->jv_argv.
	కొరకు (i = 0; argv_arg[i] != ఖాళీ; i++)
	    argc++;
	argv = ALLOC_MULT(అక్షరం *, argc + 1);
	ఒకవేళ (argv == ఖాళీ)
	    వెళ్ళు theend;
	కొరకు (i = 0; i < argc; i++)
	    argv[i] = (అక్షరం *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ఖాళీ;
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ఒకవేళ (cmd == ఖాళీ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_వాదం));
	    వెళ్ళు theend;
	}

	ఒకవేళ (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ఖాళీ
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు theend;
    }
    లేకపోతే
    {
	list_T *l = argvars[0].vval.v_list;

	ఒకవేళ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    వెళ్ళు theend;

	// Empty command is invalid.
	ఒకవేళ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_వాదం));
	    వెళ్ళు theend;
	}
#నిర్వచితం_కాకపోతే USE_ARGV
	ఒకవేళ (win32_build_cmd(l, &ga) == FAIL)
	    వెళ్ళు theend;
	cmd = ga.ga_దత్తాంశం;
	ఒకవేళ (cmd == ఖాళీ || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_వాదం));
	    వెళ్ళు theend;
	}
#అయితే_ముగింపు
    }

    // Save the command used to start the job.
    పని->jv_argv = argv;

    ఒకవేళ (పదం_పని != ఖాళీ)
	*పదం_పని = పని;

#నిర్వచితమైతే USE_ARGV
    ఒకవేళ (ch_log_చురుకైన())
    {
	garray_T    ga;

	ga_init2(&ga, పరిమాణం(అక్షరం), 200);
	కొరకు (i = 0; i < argc; ++i)
	{
	    ఒకవేళ (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ఖాళీ, "Starting job: %s", (అక్షరం *)ga.ga_దత్తాంశం);
	ga_clear(&ga);
    }
    mch_పని_ప్రారంభం(argv, పని, &opt, పదం_పని != ఖాళీ);
#లేకపోతే
    ch_log(ఖాళీ, "Starting job: %s", (అక్షరం *)cmd);
    mch_పని_ప్రారంభం((అక్షరం *)cmd, పని, &opt);
#అయితే_ముగింపు

    // If the channel is reading from a buffer, write lines now.
    ఒకవేళ (పని->jv_channel != ఖాళీ)
	channel_write_లోపల(పని->jv_channel);

theend:
#నిర్వచితం_కాకపోతే USE_ARGV
    vim_free(ga.ga_దత్తాంశం);
#అయితే_ముగింపు
    ఒకవేళ (argv != ఖాళీ && argv != పని->jv_argv)
    {
	కొరకు (i = 0; argv[i] != ఖాళీ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_పని_options(&opt);
    తిరిగి పని;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    అక్షరం *
పని_స్థితి(పని_T *పని)
{
    అక్షరం	*ఫలితం;

    ఒకవేళ (పని->jv_స్థితి >= పని_ENDED)
	// No need to check, dead is dead.
	ఫలితం = "dead";
    లేకపోతే ఒకవేళ (పని->jv_స్థితి == పని_FAILED)
	ఫలితం = "fail";
    లేకపోతే
    {
	ఫలితం = mch_పని_స్థితి(పని);
	ఒకవేళ (పని->jv_స్థితి == పని_ENDED)
	    పని_cleanup(పని);
    }
    తిరిగి ఫలితం;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    పూర్ణాంకం
పని_stop(పని_T *పని, typval_T *argvars, అక్షరం *type)
{
    char_u *arg;

    ఒకవేళ (type != ఖాళీ)
	arg = (char_u *)type;
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    లేకపోతే
    {
	arg = tv_get_string_chk(&argvars[1]);
	ఒకవేళ (arg == ఖాళీ)
	{
	    emsg(_(e_invalid_వాదం));
	    తిరిగి 0;
	}
    }
    ఒకవేళ (పని->jv_స్థితి == పని_FAILED)
    {
	ch_log(పని->jv_channel, "Job failed to start, job_stop() skipped");
	తిరిగి 0;
    }
    ఒకవేళ (పని->jv_స్థితి == పని_ENDED)
    {
	ch_log(పని->jv_channel, "Job has already ended, job_stop() skipped");
	తిరిగి 0;
    }
    ch_log(పని->jv_channel, "Stopping job with '%s'", (అక్షరం *)arg);
    ఒకవేళ (mch_signal_పని(పని, arg) == FAIL)
	తిరిగి 0;

    // Assume that only "kill" will kill the job.
    ఒకవేళ (పని->jv_channel != ఖాళీ && STRCMP(arg, "kill") == 0)
	పని->jv_channel->ch_పని_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    తిరిగి 1;
}

    శూన్య
invoke_prompt_callback(శూన్య)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*పాఠ్యం;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.నిలువు = 0;

    ఒకవేళ (curbuf->b_prompt_callback.cb_name == ఖాళీ
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	తిరిగి;
    పాఠ్యం = ml_get(lnum);
    prompt = prompt_పాఠ్యం();
    ఒకవేళ (STRLEN(పాఠ్యం) >= STRLEN(prompt))
	పాఠ్యం += STRLEN(prompt);
    argv[0].v_type = VAR_STRING;
    argv[0].vval.v_string = vim_strsave(పాఠ్యం);
    argv[1].v_type = VAR_UNKNOWN;

    call_callback(&curbuf->b_prompt_callback, -1, &rettv, 1, argv);
    clear_tv(&argv[0]);
    clear_tv(&rettv);
}

/*
 * Return TRUE when the interrupt callback was invoked.
 */
    పూర్ణాంకం
invoke_prompt_అంతరాయం(శూన్య)
{
    typval_T	rettv;
    typval_T	argv[1];
    పూర్ణాంకం		ret;

    ఒకవేళ (curbuf->b_prompt_అంతరాయం.cb_name == ఖాళీ
	    || *curbuf->b_prompt_అంతరాయం.cb_name == NUL)
	తిరిగి FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_అంతరాయం, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    తిరిగి ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    స్థిర char_u *
buf_prompt_పాఠ్యం(buf_T* buf)
{
    ఒకవేళ (buf->b_prompt_పాఠ్యం == ఖాళీ)
	తిరిగి (char_u *)"% ";
    తిరిగి buf->b_prompt_పాఠ్యం;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_పాఠ్యం(శూన్య)
{
    తిరిగి buf_prompt_పాఠ్యం(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    శూన్య
init_prompt(పూర్ణాంకం cmdchar_todo)
{
    char_u *prompt = prompt_పాఠ్యం();
    char_u *పాఠ్యం;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    పాఠ్యం = ml_get_curline();
    ఒకవేళ (STRNCMP(పాఠ్యం, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ఒకవేళ (*పాఠ్యం == NUL)
	    ml_replace(curbuf->b_ml.ml_పంక్తి_లెక్క, prompt, TRUE);
	లేకపోతే
	    ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	coladvance((colnr_T)MAXCOL);
	మారింది_బైట్లు(curbuf->b_ml.ml_పంక్తి_లెక్క, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ఒకవేళ (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.నిలువు != (పూర్ణాంకం)STRLEN(prompt))
	సమితి_insstart(curwin->w_cursor.lnum, (పూర్ణాంకం)STRLEN(prompt));

    ఒకవేళ (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ఒకవేళ (curwin->w_cursor.నిలువు < (పూర్ణాంకం)STRLEN(prompt))
	curwin->w_cursor.నిలువు = (పూర్ణాంకం)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    పూర్ణాంకం
prompt_curpos_editable(శూన్య)
{
    తిరిగి curwin->w_cursor.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క
	&& curwin->w_cursor.నిలువు >= (పూర్ణాంకం)STRLEN(prompt_పాఠ్యం());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    శూన్య
f_prompt_setcallback(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    callback_T	callback;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    callback = get_callback(&argvars[1]);
    ఒకవేళ (callback.cb_name == ఖాళీ)
	తిరిగి;

    free_callback(&buf->b_prompt_callback);
    సమితి_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    శూన్య
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    callback_T	callback;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    callback = get_callback(&argvars[1]);
    ఒకవేళ (callback.cb_name == ఖాళీ)
	తిరిగి;

    free_callback(&buf->b_prompt_అంతరాయం);
    సమితి_callback(&buf->b_prompt_అంతరాయం, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    శూన్య
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (!bt_prompt(buf))
	తిరిగి;

    rettv->vval.v_string = vim_strsave(buf_prompt_పాఠ్యం(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    శూన్య
f_prompt_setprompt(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    char_u	*పాఠ్యం;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (check_secure())
	తిరిగి;
    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    పాఠ్యం = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_పాఠ్యం);
    buf->b_prompt_పాఠ్యం = vim_strsave(పాఠ్యం);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    స్థిర పని_T *
get_పని_arg(typval_T *tv)
{
    పని_T *పని;

    ఒకవేళ (tv->v_type != VAR_పని)
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(tv));
	తిరిగి ఖాళీ;
    }
    పని = tv->vval.v_పని;

    ఒకవేళ (పని == ఖాళీ)
	emsg(_(e_not_valid_పని));
    తిరిగి పని;
}

/*
 * "job_getchannel()" function
 */
    శూన్య
f_పని_getchannel(typval_T *argvars, typval_T *rettv)
{
    పని_T	*పని;

    ఒకవేళ (లోపల_vim9script() && check_for_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    పని = get_పని_arg(&argvars[0]);
    ఒకవేళ (పని == ఖాళీ)
	తిరిగి;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = పని->jv_channel;
    ఒకవేళ (పని->jv_channel != ఖాళీ)
	++పని->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    స్థిర శూన్య
పని_info(పని_T *పని, dict_T *dict)
{
    dictitem_T	*అంశం;
    varnumber_T	nr;
    list_T	*l;
    పూర్ణాంకం		i;

    dict_కూడు_string(dict, "status", (char_u *)పని_స్థితి(పని));

    అంశం = dictitem_alloc((char_u *)"channel");
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి;
    అంశం->di_tv.v_type = VAR_CHANNEL;
    అంశం->di_tv.vval.v_channel = పని->jv_channel;
    ఒకవేళ (పని->jv_channel != ఖాళీ)
	++పని->jv_channel->ch_refcount;
    ఒకవేళ (dict_కూడు(dict, అంశం) == FAIL)
	dictitem_free(అంశం);

#నిర్వచితమైతే UNIX
    nr = పని->jv_pid;
#లేకపోతే
    nr = పని->jv_proc_info.dwProcessId;
#అయితే_ముగింపు
    dict_కూడు_సంఖ్య(dict, "process", nr);
    dict_కూడు_string(dict, "tty_in", పని->jv_tty_లోపల);
    dict_కూడు_string(dict, "tty_out", పని->jv_tty_బహిర్గతం);

    dict_కూడు_సంఖ్య(dict, "exitval", పని->jv_exitval);
    dict_కూడు_string(dict, "exit_cb", పని->jv_exit_cb.cb_name);
    dict_కూడు_string(dict, "stoponexit", పని->jv_stoponexit);
#నిర్వచితమైతే UNIX
    dict_కూడు_string(dict, "termsig", పని->jv_termsig);
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    dict_కూడు_string(dict, "tty_type", పని->jv_tty_type);
#అయితే_ముగింపు

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    ఒకవేళ (dict_కూడు_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	తిరిగి;
    }
    ఒకవేళ (పని->jv_argv != ఖాళీ)
	కొరకు (i = 0; పని->jv_argv[i] != ఖాళీ; i++)
	    list_append_string(l, (char_u *)పని->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    స్థిర శూన్య
పని_info_all(list_T *l)
{
    పని_T	*పని;
    typval_T	tv;

    FOR_ALL_JOBS(పని)
    {
	tv.v_type = VAR_పని;
	tv.vval.v_పని = పని;

	ఒకవేళ (list_append_tv(l, &tv) != OK)
	    తిరిగి;
    }
}

/*
 * "job_info()" function
 */
    శూన్య
f_పని_info(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	పని_T	*పని;

	పని = get_పని_arg(&argvars[0]);
	ఒకవేళ (పని != ఖాళీ && rettv_dict_alloc(rettv) == OK)
	    పని_info(పని, rettv->vval.v_dict);
    }
    లేకపోతే ఒకవేళ (rettv_list_alloc(rettv) == OK)
	పని_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    శూన్య
f_పని_setoptions(typval_T *argvars, typval_T *rettv వాడని)
{
    పని_T	*పని;
    jobopt_T	opt;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_పని_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    పని = get_పని_arg(&argvars[0]);
    ఒకవేళ (పని == ఖాళీ)
	తిరిగి;
    clear_పని_options(&opt);
    ఒకవేళ (get_పని_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	పని_సమితి_options(పని, &opt);
    free_పని_options(&opt);
}

/*
 * "job_start()" function
 */
    శూన్య
f_పని_ప్రారంభం(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_పని;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_పని = పని_ప్రారంభం(argvars, ఖాళీ, ఖాళీ, ఖాళీ);
}

/*
 * "job_status()" function
 */
    శూన్య
f_పని_స్థితి(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_పని && argvars[0].vval.v_పని == ఖాళీ)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    లేకపోతే
    {
	పని_T	*పని = get_పని_arg(&argvars[0]);

	ఒకవేళ (పని != ఖాళీ)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)పని_స్థితి(పని));
	}
    }
}

/*
 * "job_stop()" function
 */
    శూన్య
f_పని_stop(typval_T *argvars, typval_T *rettv)
{
    పని_T	*పని;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_పని_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    పని = get_పని_arg(&argvars[0]);
    ఒకవేళ (పని != ఖాళీ)
	rettv->vval.v_సంఖ్య = పని_stop(పని, argvars, ఖాళీ);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
పని_గమ్యం_string_buf(typval_T *varp, char_u *buf)
{
    పని_T *పని = varp->vval.v_పని;
    అక్షరం  *స్థితి_2;

    ఒకవేళ (పని == ఖాళీ)
    {
	vim_snprintf((అక్షరం *)buf, NUMBUFLEN, "no process");
	తిరిగి buf;
    }
    స్థితి_2 = పని->jv_స్థితి == పని_FAILED ? "fail"
		    : పని->jv_స్థితి >= పని_ENDED ? "dead"
		    : "run";
#నిర్వచితమైతే UNIX
    vim_snprintf((అక్షరం *)buf, NUMBUFLEN,
		"process %ld %s", (దీర్ఘ)పని->jv_pid, స్థితి_2);
#లేదావేళ defined(MSWIN)
    vim_snprintf((అక్షరం *)buf, NUMBUFLEN,
		"process %ld %s",
		(దీర్ఘ)పని->jv_proc_info.dwProcessId,
		స్థితి_2);
#లేకపోతే
    // fall-back
    vim_snprintf((అక్షరం *)buf, NUMBUFLEN, "process ? %s", స్థితి_2);
#అయితే_ముగింపు
    తిరిగి buf;
}

#అయితే_ముగింపు // FEAT_JOB_CHANNEL
