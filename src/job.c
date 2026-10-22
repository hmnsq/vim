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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    க்கு ((job) = first_job; (job) != வெற்று; (job) = (job)->jv_next)

    நிலையான முழு
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, முழு jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    எனில் (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    இல்லையெனில் எனில் (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    இல்லையெனில் எனில் (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    இல்லையெனில் எனில் (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    இல்லையெனில் எனில் (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    இல்லையெனில் எனில் (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    இல்லையெனில் எனில் (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str), val);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

    நிலையான முழு
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    எனில் (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    இல்லையெனில் எனில் (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    இல்லையெனில் எனில் (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    இல்லையெனில் எனில் (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    இல்லையெனில் எனில் (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str), val);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    வெறுமை
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    நிலையான வெறுமை
unref_job_callback(callback_T *cb)
{
    எனில் (cb->cb_partial != வெற்று)
	partial_unref(cb->cb_partial);
    இல்லையெனில் எனில் (cb->cb_name != வெற்று)
    {
	func_unref(cb->cb_name);
	எனில் (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    வெறுமை
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    எனில் (opt->jo_env != வெற்று)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    நிலையான முழு
part_from_char(முழு c)
{
    திரும்பு c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    முழு
get_job_options(typval_T *tv, jobopt_T *opt, முழு supported, முழு supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    முழு		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    எனில் (tv->v_type == VAR_UNKNOWN)
	திரும்பு OK;
    எனில் (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	திரும்பு FAIL;
    }
    dict = tv->vval.v_dict;
    எனில் (dict == வெற்று)
	திரும்பு OK;

    todo = (முழு)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(&dict->dv_hashtab, hi, todo)
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    எனில் (STRCMP(hi->hi_சாவி, "mode") == 0)
	    {
		எனில் (!(supported & JO_MODE))
		    நிறுத்து;
		எனில் (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "in_mode") == 0)
	    {
		எனில் (!(supported & JO_IN_MODE))
		    நிறுத்து;
		எனில் (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "out_mode") == 0)
	    {
		எனில் (!(supported & JO_OUT_MODE))
		    நிறுத்து;
		எனில் (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "err_mode") == 0)
	    {
		எனில் (!(supported & JO_ERR_MODE))
		    நிறுத்து;
		எனில் (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "noblock") == 0)
	    {
		எனில் (!(supported & JO_MODE))
		    நிறுத்து;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "in_io") == 0
		    || STRCMP(hi->hi_சாவி, "out_io") == 0
		    || STRCMP(hi->hi_சாவி, "err_io") == 0)
	    {
		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		எனில் (handle_io(item, part_from_char(*hi->hi_சாவி), opt) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "in_name") == 0
		    || STRCMP(hi->hi_சாவி, "out_name") == 0
		    || STRCMP(hi->hi_சாவி, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_சாவி);

		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "pty") == 0)
	    {
		எனில் (!(supported & JO_MODE))
		    நிறுத்து;
		opt->jo_pty = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "in_buf") == 0
		    || STRCMP(hi->hi_சாவி, "out_buf") == 0
		    || STRCMP(hi->hi_சாவி, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_சாவி);

		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_எண்(item);
		எனில் (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str),
					      hi->hi_சாவி, tv_get_string(item));
		    திரும்பு FAIL;
		}
		எனில் (buflist_findnr(opt->jo_io_buf[part]) == வெற்று)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "out_modifiable") == 0
		    || STRCMP(hi->hi_சாவி, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_சாவி);

		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "out_msg") == 0
		    || STRCMP(hi->hi_சாவி, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_சாவி);

		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		opt->jo_set2 |= JO2_OUT_செய்தி << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "in_top") == 0
		    || STRCMP(hi->hi_சாவி, "in_bot") == 0)
	    {
		linenr_T *lp;

		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		எனில் (hi->hi_சாவி[3] == 't')
		{
		    lp = &opt->jo_in_உச்சி;
		    opt->jo_set |= JO_IN_உச்சி;
		}
		இல்லையெனில்
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_எண்(item);
		எனில் (*lp < 0)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str),
					      hi->hi_சாவி, tv_get_string(item));
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "channel") == 0)
	    {
		எனில் (!(supported & JO_OUT_IO))
		    நிறுத்து;
		opt->jo_set |= JO_CHANNEL;
		எனில் (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "channel");
		    திரும்பு FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "callback") == 0)
	    {
		எனில் (!(supported & JO_CALLBACK))
		    நிறுத்து;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		எனில் (opt->jo_callback.cb_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "callback");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "out_cb") == 0)
	    {
		எனில் (!(supported & JO_OUT_CALLBACK))
		    நிறுத்து;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		எனில் (opt->jo_out_cb.cb_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "out_cb");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "err_cb") == 0)
	    {
		எனில் (!(supported & JO_ERR_CALLBACK))
		    நிறுத்து;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		எனில் (opt->jo_err_cb.cb_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "err_cb");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "close_cb") == 0)
	    {
		எனில் (!(supported & JO_CLOSE_CALLBACK))
		    நிறுத்து;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		எனில் (opt->jo_close_cb.cb_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "close_cb");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "drop") == 0)
	    {
		முழு never = FALSE;
		val = tv_get_string(item);

		எனில் (STRCMP(val, "never") == 0)
		    never = TRUE;
		இல்லையெனில் எனில் (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "drop", val);
		    திரும்பு FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "exit_cb") == 0)
	    {
		எனில் (!(supported & JO_EXIT_CB))
		    நிறுத்து;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		எனில் (opt->jo_exit_cb.cb_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "exit_cb");
		    திரும்பு FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_name") == 0)
	    {
		எனில் (!(supported2 & JO2_TERM_NAME))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		எனில் (opt->jo_term_name == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_name");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_finish") == 0)
	    {
		எனில் (!(supported2 & JO2_TERM_FINISH))
		    நிறுத்து;
		val = tv_get_string(item);
		எனில் (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str),
							   "term_finish", val);
		    திரும்பு FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_opencmd") == 0)
	    {
		char_u *p;

		எனில் (!(supported2 & JO2_TERM_OPENCMD))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		எனில் (p != வெற்று)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    எனில் (p != வெற்று && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != வெற்று))
			p = வெற்று;
		}
		எனில் (p == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_opencmd");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "eof_chars") == 0)
	    {
		எனில் (!(supported2 & JO2_EOF_CHARS))
		    நிறுத்து;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		எனில் (opt->jo_eof_chars == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "eof_chars");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_rows") == 0)
	    {
		முழு error = FALSE;

		எனில் (!(supported2 & JO2_TERM_ROWS))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_எண்_chk(item, &error);
		எனில் (error)
		    திரும்பு FAIL;
		எனில் (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_rows");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_cols") == 0)
	    {
		முழு error = FALSE;

		எனில் (!(supported2 & JO2_TERM_COLS))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_எண்_chk(item, &error);
		எனில் (error)
		    திரும்பு FAIL;
		எனில் (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_cols");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "vertical") == 0)
	    {
		எனில் (!(supported2 & JO2_VERTICAL))
		    நிறுத்து;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "curwin") == 0)
	    {
		எனில் (!(supported2 & JO2_CURWIN))
		    நிறுத்து;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "bufnr") == 0)
	    {
		முழு nr;

		எனில் (!(supported2 & JO2_CURWIN))
		    நிறுத்து;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_எண்(item);
		எனில் (nr <= 0)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), hi->hi_சாவி, tv_get_string(item));
		    திரும்பு FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		எனில் (opt->jo_bufnr_buf == வெற்று)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    திரும்பு FAIL;
		}
		எனில் (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == வெற்று)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "hidden") == 0)
	    {
		எனில் (!(supported2 & JO2_HIDDEN))
		    நிறுத்து;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "norestore") == 0)
	    {
		எனில் (!(supported2 & JO2_NORESTORE))
		    நிறுத்து;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_kill") == 0)
	    {
		எனில் (!(supported2 & JO2_TERM_KILL))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		எனில் (opt->jo_term_kill == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_kill");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "tty_type") == 0)
	    {
		char_u *p;

		எனில் (!(supported2 & JO2_TTY_TYPE))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		எனில் (p == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "tty_type");
		    திரும்பு FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		எனில் (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "tty_type");
		    திரும்பு FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "ansi_colors") == 0)
	    {
		முழு		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		எனில் (!(supported2 & JO2_ANSI_COLORS))
		    நிறுத்து;

		எனில் (item == வெற்று || item->v_type != VAR_LIST
			|| item->vval.v_list == வெற்று
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "ansi_colors");
		    திரும்பு FAIL;
		}

		li = item->vval.v_list->lv_first;
		க்கு (; li != வெற்று && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    முழு		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    எனில் (color_name == வெற்று)
			திரும்பு FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    எனில் (guicolor == INVALCOLOR)
		    {
			எனில் (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			திரும்பு FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		எனில் (n != 16 || li != வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "ansi_colors");
		    திரும்பு FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, அளவு(rgb));
	    }
# endif
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_highlight") == 0)
	    {
		char_u *p;

		எனில் (!(supported2 & JO2_TERM_HIGHLIGHT))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		எனில் (p == வெற்று || *p == NUL)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_highlight");
		    திரும்பு FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "term_api") == 0)
	    {
		எனில் (!(supported2 & JO2_TERM_API))
		    நிறுத்து;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		எனில் (opt->jo_term_api == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "term_api");
		    திரும்பு FAIL;
		}
	    }
#endif
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "env") == 0)
	    {
		எனில் (!(supported2 & JO2_ENV))
		    நிறுத்து;
		எனில் (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "env");
		    திரும்பு FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		எனில் (opt->jo_env != வெற்று)
		    ++opt->jo_env->dv_refcount;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "cwd") == 0)
	    {
		எனில் (!(supported2 & JO2_CWD))
		    நிறுத்து;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		எனில் (opt->jo_cwd == வெற்று || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((எழுத்து *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "cwd");
		    திரும்பு FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "waittime") == 0)
	    {
		எனில் (!(supported & JO_WAITTIME))
		    நிறுத்து;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_எண்(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "timeout") == 0)
	    {
		எனில் (!(supported & JO_TIMEOUT))
		    நிறுத்து;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_எண்(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "out_timeout") == 0)
	    {
		எனில் (!(supported & JO_OUT_TIMEOUT))
		    நிறுத்து;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_எண்(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "err_timeout") == 0)
	    {
		எனில் (!(supported & JO_ERR_TIMEOUT))
		    நிறுத்து;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_எண்(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "part") == 0)
	    {
		எனில் (!(supported & JO_PART))
		    நிறுத்து;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		எனில் (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		இல்லையெனில் எனில் (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		இல்லையெனில்
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "part", val);
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "id") == 0)
	    {
		எனில் (!(supported & JO_ID))
		    நிறுத்து;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_எண்(item);
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "stoponexit") == 0)
	    {
		எனில் (!(supported & JO_STOPONEXIT))
		    நிறுத்து;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		எனில் (opt->jo_stoponexit == வெற்று)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "stoponexit");
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(hi->hi_சாவி, "block_write") == 0)
	    {
		எனில் (!(supported & JO_BLOCK_WRITE))
		    நிறுத்து;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_எண்(item);
	    }
	    இல்லையெனில்
		நிறுத்து;
	    --todo;
	}
    எனில் (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_சாவி);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

நிலையான job_T *first_job = வெற்று;

    நிலையான வெறுமை
job_free_contents(job_T *job)
{
    முழு		i;

    ch_log(job->jv_channel, "Freeing job");
    எனில் (job->jv_channel != வெற்று)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = வெற்று;
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
    எனில் (job->jv_argv != வெற்று)
    {
	க்கு (i = 0; job->jv_argv[i] != வெற்று; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    நிலையான வெறுமை
job_unlink(job_T *job)
{
    எனில் (job->jv_next != வெற்று)
	job->jv_next->jv_prev = job->jv_prev;
    எனில் (job->jv_prev == வெற்று)
	first_job = job->jv_next;
    இல்லையெனில்
	job->jv_prev->jv_next = job->jv_next;
}

    நிலையான வெறுமை
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    நிலையான வெறுமை
job_free(job_T *job)
{
    எனில் (in_free_unref_பொருட்கள்)
	திரும்பு;

    job_free_contents(job);
    job_free_job(job);
}

நிலையான job_T *jobs_to_free = வெற்று;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    நிலையான வெறுமை
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    நிலையான வெறுமை
free_jobs_to_free_later(வெறுமை)
{
    job_T *job;

    போது (jobs_to_free != வெற்று)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#எனில் defined(EXITFREE)
    வெறுமை
job_free_all(வெறுமை)
{
    போது (first_job != வெற்று)
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
    நிலையான முழு
job_need_end_check(job_T *job)
{
    திரும்பு job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != வெற்று || job->jv_exit_cb.cb_name != வெற்று);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    நிலையான முழு
job_channel_still_useful(job_T *job)
{
    திரும்பு job->jv_channel != வெற்று && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    நிலையான முழு
job_channel_can_close(job_T *job)
{
    திரும்பு job->jv_channel != வெற்று && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    நிலையான முழு
job_still_useful(job_T *job)
{
    திரும்பு job_need_end_check(job) || job_channel_still_useful(job);
}

#எனில் defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    முழு
job_any_running(வெறுமை)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	எனில் (job_still_useful(job))
	{
	    ch_log(வெற்று, "GUI not forking because a job is running");
	    திரும்பு TRUE;
	}
    திரும்பு FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#எனில் defined(UNIX)
# define USE_ARGV
#endif

#எனில் !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    நிலையான char_u *
win32_escape_arg(char_u *arg)
{
    முழு		slen, dlen;
    முழு		escaping = 0;
    முழு		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    முழு		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (முழு)STRLEN(arg);
    dlen = slen;
    க்கு (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	எனில் (*s == '"' || *s == '\\')
	    ++dlen;
	எனில் (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    எனில் (has_spaces)
	dlen += 2;

    எனில் (dlen == slen)
	திரும்பு vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    எனில் (escaped_arg == வெற்று)
	திரும்பு வெற்று;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    எனில் (has_spaces)
	*d++ = '"';

    க்கு (s = arg; *s != NUL;)
    {
	தேர்வு (*s)
	{
	    நிலை '"':
		க்கு (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		நிறுத்து;
	    நிலை '\\':
		escaping++;
		*d++ = *s++;
		நிறுத்து;
	    இயல்பு:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		நிறுத்து;
	}
    }

    // add terminating quote and finish with a NUL
    எனில் (has_spaces)
    {
	க்கு (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    திரும்பு escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    முழு
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	எனில் (s == வெற்று)
	    திரும்பு FAIL;
	s = win32_escape_arg(s);
	எனில் (s == வெற்று)
	    திரும்பு FAIL;
	ga_concat(gap, s);
	vim_free(s);
	எனில் (li->li_next != வெற்று)
	    ga_append(gap, ' ');
    }
    திரும்பு OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    வெறுமை
job_cleanup(job_T *job)
{
    எனில் (job->jv_status != JOB_ENDED)
	திரும்பு;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    எனில் (job->jv_channel != வெற்று)
	ch_close_part(job->jv_channel, PART_IN);

    எனில் (job->jv_exit_cb.cb_name != வெற்று)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_எண்;
	argv[1].vval.v_எண் = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    எனில் (job->jv_channel != வெற்று && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    எனில் (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    முழு
set_ref_in_job(முழு copyID)
{
    முழு		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    க்கு (job = first_job; !abort && job != வெற்று; job = job->jv_next)
	எனில் (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
	}
    திரும்பு abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    வெறுமை
job_unref(job_T *job)
{
    எனில் (job == வெற்று || --job->jv_refcount > 0)
	திரும்பு;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    எனில் (job_channel_still_useful(job))
	திரும்பு;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    எனில் (!job_need_end_check(job))
    {
	job_free(job);
    }
    இல்லையெனில் எனில் (job->jv_channel != வெற்று)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = வெற்று;
	channel_unref(job->jv_channel);
	job->jv_channel = வெற்று;
    }
}

    முழு
free_unused_jobs_contents(முழு copyID, முழு mask)
{
    முழு		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	எனில் ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    திரும்பு did_free;
}

    வெறுமை
free_unused_jobs(முழு copyID, முழு mask)
{
    job_T	*job;
    job_T	*job_next;

    க்கு (job = first_job; job != வெற்று; job = job_next)
    {
	job_next = job->jv_next;
	எனில் ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(வெறுமை)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    எனில் (job == வெற்று)
	திரும்பு வெற்று;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    எனில் (first_job != வெற்று)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    திரும்பு job;
}

    வெறுமை
job_set_options(job_T *job, jobopt_T *opt)
{
    எனில் (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	எனில் (opt->jo_stoponexit == வெற்று || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = வெற்று;
	இல்லையெனில்
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    எனில் (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	எனில் (opt->jo_exit_cb.cb_name == வெற்று || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = வெற்று;
	    job->jv_exit_cb.cb_partial = வெற்று;
	}
	இல்லையெனில்
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    வெறுமை
job_stop_on_exit(வெறுமை)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	எனில் (job->jv_status == JOB_STARTED && job->jv_stoponexit != வெற்று)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    முழு
has_pending_job(வெறுமை)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	எனில் ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    முழு
job_check_ended(வெறுமை)
{
    முழு		i;
    முழு		did_end = FALSE;

    // be quick if there are no jobs to check
    எனில் (first_job == வெற்று)
	திரும்பு did_end;

    க்கு (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	எனில் (job == வெற்று)
	    நிறுத்து;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    எனில் (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    திரும்பு did_end;
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
	எழுத்து	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = வெற்று;
    எழுத்து	**argv = வெற்று;
    முழு		argc = 0;
    முழு		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    எனில் (job == வெற்று)
	திரும்பு வெற்று;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, அளவு(எழுத்து*), 20);
#endif

    எனில் (opt_arg != வெற்று)
	opt = *opt_arg;
    இல்லையெனில்
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	எனில் (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    செல் theend;
    }

    // Check that when io is "file" that there is a file name.
    க்கு (part = PART_OUT; part < PART_மொத்தம்; ++part)
	எனில் ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    செல் theend;
	}

    எனில் ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = வெற்று;

	// check that we can find the buffer before starting the job
	எனில் (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    எனில் (buf == வெற்று)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	இல்லையெனில் எனில் (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	இல்லையெனில்
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	எனில் (buf == வெற்று)
	    செல் theend;
	எனில் (buf->b_ml.ml_mfp == வெற்று)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    எனில் (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((எழுத்து *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    இல்லையெனில்
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    செல் theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    எனில் (argv_arg != வெற்று)
    {
	// Make a copy of argv_arg for job->jv_argv.
	க்கு (i = 0; argv_arg[i] != வெற்று; i++)
	    argc++;
	argv = ALLOC_MULT(எழுத்து *, argc + 1);
	எனில் (argv == வெற்று)
	    செல் theend;
	க்கு (i = 0; i < argc; i++)
	    argv[i] = (எழுத்து *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = வெற்று;
    }
    இல்லையெனில்
#endif
    எனில் (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	எனில் (cmd == வெற்று || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    செல் theend;
	}

	எனில் (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    செல் theend;
    }
    இல்லையெனில் எனில் (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == வெற்று
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	செல் theend;
    }
    இல்லையெனில்
    {
	list_T *l = argvars[0].vval.v_list;

	எனில் (build_argv_from_list(l, &argv, &argc) == FAIL)
	    செல் theend;

	// Empty command is invalid.
	எனில் (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    செல் theend;
	}
#ifndef USE_ARGV
	எனில் (win32_build_cmd(l, &ga) == FAIL)
	    செல் theend;
	cmd = ga.ga_data;
	எனில் (cmd == வெற்று || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    செல் theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    எனில் (term_job != வெற்று)
	*term_job = job;

#ifdef USE_ARGV
    எனில் (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, அளவு(எழுத்து), 200);
	க்கு (i = 0; i < argc; ++i)
	{
	    எனில் (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(வெற்று, "Starting job: %s", (எழுத்து *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != வெற்று);
#இல்லையெனில்
    ch_log(வெற்று, "Starting job: %s", (எழுத்து *)cmd);
    mch_job_start((எழுத்து *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    எனில் (job->jv_channel != வெற்று)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    எனில் (argv != வெற்று && argv != job->jv_argv)
    {
	க்கு (i = 0; argv[i] != வெற்று; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    திரும்பு job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    எழுத்து *
job_status(job_T *job)
{
    எழுத்து	*result;

    எனில் (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    இல்லையெனில் எனில் (job->jv_status == JOB_FAILED)
	result = "fail";
    இல்லையெனில்
    {
	result = mch_job_status(job);
	எனில் (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    திரும்பு result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    முழு
job_stop(job_T *job, typval_T *argvars, எழுத்து *type)
{
    char_u *arg;

    எனில் (type != வெற்று)
	arg = (char_u *)type;
    இல்லையெனில் எனில் (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    இல்லையெனில்
    {
	arg = tv_get_string_chk(&argvars[1]);
	எனில் (arg == வெற்று)
	{
	    emsg(_(e_invalid_argument));
	    திரும்பு 0;
	}
    }
    எனில் (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	திரும்பு 0;
    }
    எனில் (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	திரும்பு 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (எழுத்து *)arg);
    எனில் (mch_signal_job(job, arg) == FAIL)
	திரும்பு 0;

    // Assume that only "kill" will kill the job.
    எனில் (job->jv_channel != வெற்று && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    திரும்பு 1;
}

    வெறுமை
invoke_prompt_callback(வெறுமை)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_வரி_மொத்தம்;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    எனில் (curbuf->b_prompt_callback.cb_name == வெற்று
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	திரும்பு;
    text = ml_get(lnum);
    prompt = prompt_text();
    எனில் (STRLEN(text) >= STRLEN(prompt))
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
    முழு
invoke_prompt_interrupt(வெறுமை)
{
    typval_T	rettv;
    typval_T	argv[1];
    முழு		ret;

    எனில் (curbuf->b_prompt_interrupt.cb_name == வெற்று
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	திரும்பு FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    திரும்பு ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    நிலையான char_u *
buf_prompt_text(buf_T* buf)
{
    எனில் (buf->b_prompt_text == வெற்று)
	திரும்பு (char_u *)"% ";
    திரும்பு buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(வெறுமை)
{
    திரும்பு buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    வெறுமை
init_prompt(முழு cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
    text = ml_get_curline();
    எனில் (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	எனில் (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_வரி_மொத்தம், prompt, TRUE);
	இல்லையெனில்
	    ml_append(curbuf->b_ml.ml_வரி_மொத்தம், prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_வரி_மொத்தம், 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    எனில் (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (முழு)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (முழு)STRLEN(prompt));

    எனில் (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    எனில் (curwin->w_cursor.col < (முழு)STRLEN(prompt))
	curwin->w_cursor.col = (முழு)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    முழு
prompt_curpos_editable(வெறுமை)
{
    திரும்பு curwin->w_cursor.lnum == curbuf->b_ml.ml_வரி_மொத்தம்
	&& curwin->w_cursor.col >= (முழு)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    வெறுமை
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	திரும்பு;

    callback = get_callback(&argvars[1]);
    எனில் (callback.cb_name == வெற்று)
	திரும்பு;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    வெறுமை
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	திரும்பு;

    callback = get_callback(&argvars[1]);
    எனில் (callback.cb_name == வெற்று)
	திரும்பு;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    வெறுமை
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = tv_get_buf_from_arg(&argvars[0]);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (!bt_prompt(buf))
	திரும்பு;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    வெறுமை
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (check_secure())
	திரும்பு;
    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	திரும்பு;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    நிலையான job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    எனில் (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	திரும்பு வெற்று;
    }
    job = tv->vval.v_job;

    எனில் (job == வெற்று)
	emsg(_(e_not_valid_job));
    திரும்பு job;
}

/*
 * "job_getchannel()" function
 */
    வெறுமை
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    எனில் (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    job = get_job_arg(&argvars[0]);
    எனில் (job == வெற்று)
	திரும்பு;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    எனில் (job->jv_channel != வெற்று)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    நிலையான வெறுமை
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    முழு		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    எனில் (item == வெற்று)
	திரும்பு;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    எனில் (job->jv_channel != வெற்று)
	++job->jv_channel->ch_refcount;
    எனில் (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#இல்லையெனில்
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_எண்(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_எண்(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு;

    எனில் (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	திரும்பு;
    }
    எனில் (job->jv_argv != வெற்று)
	க்கு (i = 0; job->jv_argv[i] != வெற்று; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    நிலையான வெறுமை
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	எனில் (list_append_tv(l, &tv) != OK)
	    திரும்பு;
    }
}

/*
 * "job_info()" function
 */
    வெறுமை
f_job_info(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	எனில் (job != வெற்று && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    இல்லையெனில் எனில் (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    வெறுமை
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    எனில் (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    job = get_job_arg(&argvars[0]);
    எனில் (job == வெற்று)
	திரும்பு;
    clear_job_options(&opt);
    எனில் (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    வெறுமை
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_job = job_start(argvars, வெற்று, வெற்று, வெற்று);
}

/*
 * "job_status()" function
 */
    வெறுமை
f_job_status(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == வெற்று)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    இல்லையெனில்
    {
	job_T	*job = get_job_arg(&argvars[0]);

	எனில் (job != வெற்று)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    வெறுமை
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    எனில் (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    job = get_job_arg(&argvars[0]);
    எனில் (job != வெற்று)
	rettv->vval.v_எண் = job_stop(job, argvars, வெற்று);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    எழுத்து  *status;

    எனில் (job == வெற்று)
    {
	vim_snprintf((எழுத்து *)buf, NUMBUFLEN, "no process");
	திரும்பு buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((எழுத்து *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#அல்லதெனில் defined(MSWIN)
    vim_snprintf((எழுத்து *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#இல்லையெனில்
    // fall-back
    vim_snprintf((எழுத்து *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    திரும்பு buf;
}

#endif // FEAT_JOB_CHANNEL
