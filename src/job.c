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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    માટે ((job) = first_job; (job) != ખાલી; (job) = (job)->jv_next)

    સ્થિર પૂર્ણાંક
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, પૂર્ણાંક jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    જો (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    નહીંતર જો (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    નહીંતર જો (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    નહીંતર જો (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    નહીંતર જો (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    નહીંતર જો (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    નહીંતર જો (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    નહીંતર
    {
	semsg(_(e_invalid_argument_str), val);
	પાછા FAIL;
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    જો (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    નહીંતર જો (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    નહીંતર જો (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    નહીંતર જો (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    નહીંતર જો (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    નહીંતર
    {
	semsg(_(e_invalid_argument_str), val);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Clear a jobopt_T before using it.
 */
    શૂન્ય
clear_job_options(jobopt_T *opt)
{
    CLEAR_POINTER(opt);
}

    સ્થિર શૂન્ય
unref_job_callback(callback_T *cb)
{
    જો (cb->cb_partial != ખાલી)
	partial_unref(cb->cb_partial);
    નહીંતર જો (cb->cb_name != ખાલી)
    {
	func_unref(cb->cb_name);
	જો (cb->cb_free_name)
	    vim_free(cb->cb_name);
    }
}

/*
 * Free any members of a jobopt_T.
 */
    શૂન્ય
free_job_options(jobopt_T *opt)
{
    unref_job_callback(&opt->jo_callback);
    unref_job_callback(&opt->jo_out_cb);
    unref_job_callback(&opt->jo_err_cb);
    unref_job_callback(&opt->jo_close_cb);
    unref_job_callback(&opt->jo_exit_cb);

    જો (opt->jo_env != ખાલી)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    સ્થિર પૂર્ણાંક
part_from_char(પૂર્ણાંક c)
{
    પાછા c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    પૂર્ણાંક
get_job_options(typval_T *tv, jobopt_T *opt, પૂર્ણાંક supported, પૂર્ણાંક supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    પૂર્ણાંક		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    જો (tv->v_type == VAR_UNKNOWN)
	પાછા OK;
    જો (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	પાછા FAIL;
    }
    dict = tv->vval.v_dict;
    જો (dict == ખાલી)
	પાછા OK;

    todo = (પૂર્ણાંક)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(&dict->dv_hashtab, hi, todo)
	જો (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    જો (STRCMP(hi->hi_ચાવી, "mode") == 0)
	    {
		જો (!(supported & JO_MODE))
		    તોડો;
		જો (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "in_mode") == 0)
	    {
		જો (!(supported & JO_IN_MODE))
		    તોડો;
		જો (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "out_mode") == 0)
	    {
		જો (!(supported & JO_OUT_MODE))
		    તોડો;
		જો (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "err_mode") == 0)
	    {
		જો (!(supported & JO_ERR_MODE))
		    તોડો;
		જો (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "noblock") == 0)
	    {
		જો (!(supported & JO_MODE))
		    તોડો;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "in_io") == 0
		    || STRCMP(hi->hi_ચાવી, "out_io") == 0
		    || STRCMP(hi->hi_ચાવી, "err_io") == 0)
	    {
		જો (!(supported & JO_OUT_IO))
		    તોડો;
		જો (handle_io(item, part_from_char(*hi->hi_ચાવી), opt) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "in_name") == 0
		    || STRCMP(hi->hi_ચાવી, "out_name") == 0
		    || STRCMP(hi->hi_ચાવી, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_ચાવી);

		જો (!(supported & JO_OUT_IO))
		    તોડો;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "pty") == 0)
	    {
		જો (!(supported & JO_MODE))
		    તોડો;
		opt->jo_pty = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "in_buf") == 0
		    || STRCMP(hi->hi_ચાવી, "out_buf") == 0
		    || STRCMP(hi->hi_ચાવી, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_ચાવી);

		જો (!(supported & JO_OUT_IO))
		    તોડો;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_સંખ્યા(item);
		જો (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str),
					      hi->hi_ચાવી, tv_get_string(item));
		    પાછા FAIL;
		}
		જો (buflist_findnr(opt->jo_io_buf[part]) == ખાલી)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "out_modifiable") == 0
		    || STRCMP(hi->hi_ચાવી, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_ચાવી);

		જો (!(supported & JO_OUT_IO))
		    તોડો;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "out_msg") == 0
		    || STRCMP(hi->hi_ચાવી, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_ચાવી);

		જો (!(supported & JO_OUT_IO))
		    તોડો;
		opt->jo_set2 |= JO2_OUT_સંદેશ << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "in_top") == 0
		    || STRCMP(hi->hi_ચાવી, "in_bot") == 0)
	    {
		linenr_T *lp;

		જો (!(supported & JO_OUT_IO))
		    તોડો;
		જો (hi->hi_ચાવી[3] == 't')
		{
		    lp = &opt->jo_in_ટોચ;
		    opt->jo_set |= JO_IN_ટોચ;
		}
		નહીંતર
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_સંખ્યા(item);
		જો (*lp < 0)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str),
					      hi->hi_ચાવી, tv_get_string(item));
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "channel") == 0)
	    {
		જો (!(supported & JO_OUT_IO))
		    તોડો;
		opt->jo_set |= JO_CHANNEL;
		જો (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "channel");
		    પાછા FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "callback") == 0)
	    {
		જો (!(supported & JO_CALLBACK))
		    તોડો;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		જો (opt->jo_callback.cb_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "callback");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "out_cb") == 0)
	    {
		જો (!(supported & JO_OUT_CALLBACK))
		    તોડો;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		જો (opt->jo_out_cb.cb_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "out_cb");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "err_cb") == 0)
	    {
		જો (!(supported & JO_ERR_CALLBACK))
		    તોડો;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		જો (opt->jo_err_cb.cb_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "err_cb");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "close_cb") == 0)
	    {
		જો (!(supported & JO_CLOSE_CALLBACK))
		    તોડો;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		જો (opt->jo_close_cb.cb_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "close_cb");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "drop") == 0)
	    {
		પૂર્ણાંક never = FALSE;
		val = tv_get_string(item);

		જો (STRCMP(val, "never") == 0)
		    never = TRUE;
		નહીંતર જો (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str), "drop", val);
		    પાછા FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "exit_cb") == 0)
	    {
		જો (!(supported & JO_EXIT_CB))
		    તોડો;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		જો (opt->jo_exit_cb.cb_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "exit_cb");
		    પાછા FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_name") == 0)
	    {
		જો (!(supported2 & JO2_TERM_NAME))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		જો (opt->jo_term_name == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_name");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_finish") == 0)
	    {
		જો (!(supported2 & JO2_TERM_FINISH))
		    તોડો;
		val = tv_get_string(item);
		જો (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str),
							   "term_finish", val);
		    પાછા FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_opencmd") == 0)
	    {
		char_u *p;

		જો (!(supported2 & JO2_TERM_OPENCMD))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		જો (p != ખાલી)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    જો (p != ખાલી && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != ખાલી))
			p = ખાલી;
		}
		જો (p == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_opencmd");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "eof_chars") == 0)
	    {
		જો (!(supported2 & JO2_EOF_CHARS))
		    તોડો;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		જો (opt->jo_eof_chars == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "eof_chars");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_rows") == 0)
	    {
		પૂર્ણાંક error = FALSE;

		જો (!(supported2 & JO2_TERM_ROWS))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_સંખ્યા_chk(item, &error);
		જો (error)
		    પાછા FAIL;
		જો (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_rows");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_cols") == 0)
	    {
		પૂર્ણાંક error = FALSE;

		જો (!(supported2 & JO2_TERM_COLS))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_સંખ્યા_chk(item, &error);
		જો (error)
		    પાછા FAIL;
		જો (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_cols");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "vertical") == 0)
	    {
		જો (!(supported2 & JO2_VERTICAL))
		    તોડો;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "curwin") == 0)
	    {
		જો (!(supported2 & JO2_CURWIN))
		    તોડો;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "bufnr") == 0)
	    {
		પૂર્ણાંક nr;

		જો (!(supported2 & JO2_CURWIN))
		    તોડો;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_સંખ્યા(item);
		જો (nr <= 0)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str), hi->hi_ચાવી, tv_get_string(item));
		    પાછા FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		જો (opt->jo_bufnr_buf == ખાલી)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    પાછા FAIL;
		}
		જો (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == ખાલી)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "hidden") == 0)
	    {
		જો (!(supported2 & JO2_HIDDEN))
		    તોડો;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "norestore") == 0)
	    {
		જો (!(supported2 & JO2_NORESTORE))
		    તોડો;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_kill") == 0)
	    {
		જો (!(supported2 & JO2_TERM_KILL))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		જો (opt->jo_term_kill == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_kill");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "tty_type") == 0)
	    {
		char_u *p;

		જો (!(supported2 & JO2_TTY_TYPE))
		    તોડો;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		જો (p == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "tty_type");
		    પાછા FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		જો (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "tty_type");
		    પાછા FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "ansi_colors") == 0)
	    {
		પૂર્ણાંક		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		જો (!(supported2 & JO2_ANSI_COLORS))
		    તોડો;

		જો (item == ખાલી || item->v_type != VAR_LIST
			|| item->vval.v_list == ખાલી
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "ansi_colors");
		    પાછા FAIL;
		}

		li = item->vval.v_list->lv_first;
		માટે (; li != ખાલી && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    પૂર્ણાંક		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    જો (color_name == ખાલી)
			પાછા FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    જો (guicolor == INVALCOLOR)
		    {
			જો (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			પાછા FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		જો (n != 16 || li != ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "ansi_colors");
		    પાછા FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, કદ(rgb));
	    }
# endif
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_highlight") == 0)
	    {
		char_u *p;

		જો (!(supported2 & JO2_TERM_HIGHLIGHT))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		જો (p == ખાલી || *p == NUL)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_highlight");
		    પાછા FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "term_api") == 0)
	    {
		જો (!(supported2 & JO2_TERM_API))
		    તોડો;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		જો (opt->jo_term_api == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "term_api");
		    પાછા FAIL;
		}
	    }
#endif
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "env") == 0)
	    {
		જો (!(supported2 & JO2_ENV))
		    તોડો;
		જો (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "env");
		    પાછા FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		જો (opt->jo_env != ખાલી)
		    ++opt->jo_env->dv_refcount;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "cwd") == 0)
	    {
		જો (!(supported2 & JO2_CWD))
		    તોડો;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		જો (opt->jo_cwd == ખાલી || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((અક્ષર *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "cwd");
		    પાછા FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "waittime") == 0)
	    {
		જો (!(supported & JO_WAITTIME))
		    તોડો;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_સંખ્યા(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "timeout") == 0)
	    {
		જો (!(supported & JO_TIMEOUT))
		    તોડો;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_સંખ્યા(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "out_timeout") == 0)
	    {
		જો (!(supported & JO_OUT_TIMEOUT))
		    તોડો;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_સંખ્યા(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "err_timeout") == 0)
	    {
		જો (!(supported & JO_ERR_TIMEOUT))
		    તોડો;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_સંખ્યા(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "part") == 0)
	    {
		જો (!(supported & JO_PART))
		    તોડો;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		જો (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		નહીંતર જો (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		નહીંતર
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str_str), "part", val);
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "id") == 0)
	    {
		જો (!(supported & JO_ID))
		    તોડો;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_સંખ્યા(item);
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "stoponexit") == 0)
	    {
		જો (!(supported & JO_STOPONEXIT))
		    તોડો;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		જો (opt->jo_stoponexit == ખાલી)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "stoponexit");
		    પાછા FAIL;
		}
	    }
	    નહીંતર જો (STRCMP(hi->hi_ચાવી, "block_write") == 0)
	    {
		જો (!(supported & JO_BLOCK_WRITE))
		    તોડો;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_સંખ્યા(item);
	    }
	    નહીંતર
		તોડો;
	    --todo;
	}
    જો (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_ચાવી);
	પાછા FAIL;
    }

    પાછા OK;
}

સ્થિર job_T *first_job = ખાલી;

    સ્થિર શૂન્ય
job_free_contents(job_T *job)
{
    પૂર્ણાંક		i;

    ch_log(job->jv_channel, "Freeing job");
    જો (job->jv_channel != ખાલી)
    {
	// The link from the channel to the job doesn't count as a reference,
	// thus don't decrement the refcount of the job.  The reference from
	// the job to the channel does count the reference, decrement it and
	// NULL the reference.  We don't set ch_job_killed, unreferencing the
	// job doesn't mean it stops running.
	job->jv_channel->ch_job = ખાલી;
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
    જો (job->jv_argv != ખાલી)
    {
	માટે (i = 0; job->jv_argv[i] != ખાલી; i++)
	    vim_free(job->jv_argv[i]);
	vim_free(job->jv_argv);
    }
}

/*
 * Remove "job" from the list of jobs.
 */
    સ્થિર શૂન્ય
job_unlink(job_T *job)
{
    જો (job->jv_next != ખાલી)
	job->jv_next->jv_prev = job->jv_prev;
    જો (job->jv_prev == ખાલી)
	first_job = job->jv_next;
    નહીંતર
	job->jv_prev->jv_next = job->jv_next;
}

    સ્થિર શૂન્ય
job_free_job(job_T *job)
{
    job_unlink(job);
    vim_free(job);
}

    સ્થિર શૂન્ય
job_free(job_T *job)
{
    જો (in_free_unref_વસ્તુઓ)
	પાછા;

    job_free_contents(job);
    job_free_job(job);
}

સ્થિર job_T *jobs_to_free = ખાલી;

/*
 * Put "job" in a list to be freed later, when it's no longer referenced.
 */
    સ્થિર શૂન્ય
job_free_later(job_T *job)
{
    job_unlink(job);
    job->jv_next = jobs_to_free;
    jobs_to_free = job;
}

    સ્થિર શૂન્ય
free_jobs_to_free_later(શૂન્ય)
{
    job_T *job;

    જ્યારે (jobs_to_free != ખાલી)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#જો defined(EXITFREE)
    શૂન્ય
job_free_all(શૂન્ય)
{
    જ્યારે (first_job != ખાલી)
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
    સ્થિર પૂર્ણાંક
job_need_end_check(job_T *job)
{
    પાછા job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != ખાલી || job->jv_exit_cb.cb_name != ખાલી);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    સ્થિર પૂર્ણાંક
job_channel_still_useful(job_T *job)
{
    પાછા job->jv_channel != ખાલી && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    સ્થિર પૂર્ણાંક
job_channel_can_close(job_T *job)
{
    પાછા job->jv_channel != ખાલી && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    સ્થિર પૂર્ણાંક
job_still_useful(job_T *job)
{
    પાછા job_need_end_check(job) || job_channel_still_useful(job);
}

#જો defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    પૂર્ણાંક
job_any_running(શૂન્ય)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	જો (job_still_useful(job))
	{
	    ch_log(ખાલી, "GUI not forking because a job is running");
	    પાછા TRUE;
	}
    પાછા FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#જો defined(UNIX)
# define USE_ARGV
#endif

#જો !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    સ્થિર char_u *
win32_escape_arg(char_u *arg)
{
    પૂર્ણાંક		slen, dlen;
    પૂર્ણાંક		escaping = 0;
    પૂર્ણાંક		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    પૂર્ણાંક		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (પૂર્ણાંક)STRLEN(arg);
    dlen = slen;
    માટે (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	જો (*s == '"' || *s == '\\')
	    ++dlen;
	જો (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    જો (has_spaces)
	dlen += 2;

    જો (dlen == slen)
	પાછા vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    જો (escaped_arg == ખાલી)
	પાછા ખાલી;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    જો (has_spaces)
	*d++ = '"';

    માટે (s = arg; *s != NUL;)
    {
	પસંદગી (*s)
	{
	    સ્થિતિ '"':
		માટે (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		તોડો;
	    સ્થિતિ '\\':
		escaping++;
		*d++ = *s++;
		તોડો;
	    મૂળભૂત:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		તોડો;
	}
    }

    // add terminating quote and finish with a NUL
    જો (has_spaces)
    {
	માટે (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    પાછા escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    પૂર્ણાંક
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	જો (s == ખાલી)
	    પાછા FAIL;
	s = win32_escape_arg(s);
	જો (s == ખાલી)
	    પાછા FAIL;
	ga_concat(gap, s);
	vim_free(s);
	જો (li->li_next != ખાલી)
	    ga_append(gap, ' ');
    }
    પાછા OK;
}
#endif

/*
 * NOTE: Must call job_cleanup() only once right after the status of "job"
 * changed to JOB_ENDED (i.e. after job_status() returned "dead" first or
 * mch_detect_ended_job() returned non-NULL).
 * If the job is no longer used it will be removed from the list of jobs, and
 * deleted a bit later.
 */
    શૂન્ય
job_cleanup(job_T *job)
{
    જો (job->jv_status != JOB_ENDED)
	પાછા;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    જો (job->jv_channel != ખાલી)
	ch_close_part(job->jv_channel, PART_IN);

    જો (job->jv_exit_cb.cb_name != ખાલી)
    {
	typval_T	argv[3];
	typval_T	rettv;

	// Invoke the exit callback. Make sure the refcount is > 0.
	ch_log(job->jv_channel, "Invoking exit callback %s",
						      job->jv_exit_cb.cb_name);
	++job->jv_refcount;
	argv[0].v_type = VAR_JOB;
	argv[0].vval.v_job = job;
	argv[1].v_type = VAR_સંખ્યા;
	argv[1].vval.v_સંખ્યા = job->jv_exitval;
	call_callback(&job->jv_exit_cb, -1, &rettv, 2, argv);
	clear_tv(&rettv);
	--job->jv_refcount;
	channel_need_redraw = TRUE;
    }

    જો (job->jv_channel != ખાલી && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    જો (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    પૂર્ણાંક
set_ref_in_job(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    માટે (job = first_job; !abort && job != ખાલી; job = job->jv_next)
	જો (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
	}
    પાછા abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    શૂન્ય
job_unref(job_T *job)
{
    જો (job == ખાલી || --job->jv_refcount > 0)
	પાછા;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    જો (job_channel_still_useful(job))
	પાછા;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    જો (!job_need_end_check(job))
    {
	job_free(job);
    }
    નહીંતર જો (job->jv_channel != ખાલી)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = ખાલી;
	channel_unref(job->jv_channel);
	job->jv_channel = ખાલી;
    }
}

    પૂર્ણાંક
free_unused_jobs_contents(પૂર્ણાંક copyID, પૂર્ણાંક mask)
{
    પૂર્ણાંક		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	જો ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    પાછા did_free;
}

    શૂન્ય
free_unused_jobs(પૂર્ણાંક copyID, પૂર્ણાંક mask)
{
    job_T	*job;
    job_T	*job_next;

    માટે (job = first_job; job != ખાલી; job = job_next)
    {
	job_next = job->jv_next;
	જો ((job->jv_copyID & mask) != (copyID & mask)
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
job_alloc(શૂન્ય)
{
    job_T *job;

    job = ALLOC_CLEAR_ONE(job_T);
    જો (job == ખાલી)
	પાછા ખાલી;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    જો (first_job != ખાલી)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    પાછા job;
}

    શૂન્ય
job_set_options(job_T *job, jobopt_T *opt)
{
    જો (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	જો (opt->jo_stoponexit == ખાલી || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = ખાલી;
	નહીંતર
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    જો (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	જો (opt->jo_exit_cb.cb_name == ખાલી || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = ખાલી;
	    job->jv_exit_cb.cb_partial = ખાલી;
	}
	નહીંતર
	    copy_callback(&job->jv_exit_cb, &opt->jo_exit_cb);
    }
}

/*
 * Called when Vim is exiting: kill all jobs that have the "stoponexit" flag.
 */
    શૂન્ય
job_stop_on_exit(શૂન્ય)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	જો (job->jv_status == JOB_STARTED && job->jv_stoponexit != ખાલી)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    પૂર્ણાંક
has_pending_job(શૂન્ય)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	જો ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    પાછા TRUE;
    પાછા FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    પૂર્ણાંક
job_check_ended(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		did_end = FALSE;

    // be quick if there are no jobs to check
    જો (first_job == ખાલી)
	પાછા did_end;

    માટે (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	જો (job == ખાલી)
	    તોડો;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    જો (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    પાછા did_end;
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
	અક્ષર	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = ખાલી;
    અક્ષર	**argv = ખાલી;
    પૂર્ણાંક		argc = 0;
    પૂર્ણાંક		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    જો (job == ખાલી)
	પાછા ખાલી;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, કદ(અક્ષર*), 20);
#endif

    જો (opt_arg != ખાલી)
	opt = *opt_arg;
    નહીંતર
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	જો (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    જાઓ theend;
    }

    // Check that when io is "file" that there is a file name.
    માટે (part = PART_OUT; part < PART_ગણતરી; ++part)
	જો ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    જાઓ theend;
	}

    જો ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = ખાલી;

	// check that we can find the buffer before starting the job
	જો (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    જો (buf == ખાલી)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	નહીંતર જો (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	નહીંતર
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	જો (buf == ખાલી)
	    જાઓ theend;
	જો (buf->b_ml.ml_mfp == ખાલી)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    જો (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((અક્ષર *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    નહીંતર
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    જાઓ theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    જો (argv_arg != ખાલી)
    {
	// Make a copy of argv_arg for job->jv_argv.
	માટે (i = 0; argv_arg[i] != ખાલી; i++)
	    argc++;
	argv = ALLOC_MULT(અક્ષર *, argc + 1);
	જો (argv == ખાલી)
	    જાઓ theend;
	માટે (i = 0; i < argc; i++)
	    argv[i] = (અક્ષર *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = ખાલી;
    }
    નહીંતર
#endif
    જો (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	જો (cmd == ખાલી || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    જાઓ theend;
	}

	જો (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    જાઓ theend;
    }
    નહીંતર જો (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == ખાલી
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	જાઓ theend;
    }
    નહીંતર
    {
	list_T *l = argvars[0].vval.v_list;

	જો (build_argv_from_list(l, &argv, &argc) == FAIL)
	    જાઓ theend;

	// Empty command is invalid.
	જો (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    જાઓ theend;
	}
#ifndef USE_ARGV
	જો (win32_build_cmd(l, &ga) == FAIL)
	    જાઓ theend;
	cmd = ga.ga_data;
	જો (cmd == ખાલી || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    જાઓ theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    જો (term_job != ખાલી)
	*term_job = job;

#ifdef USE_ARGV
    જો (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, કદ(અક્ષર), 200);
	માટે (i = 0; i < argc; ++i)
	{
	    જો (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(ખાલી, "Starting job: %s", (અક્ષર *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != ખાલી);
#નહીંતર
    ch_log(ખાલી, "Starting job: %s", (અક્ષર *)cmd);
    mch_job_start((અક્ષર *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    જો (job->jv_channel != ખાલી)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    જો (argv != ખાલી && argv != job->jv_argv)
    {
	માટે (i = 0; argv[i] != ખાલી; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    પાછા job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    અક્ષર *
job_status(job_T *job)
{
    અક્ષર	*result;

    જો (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    નહીંતર જો (job->jv_status == JOB_FAILED)
	result = "fail";
    નહીંતર
    {
	result = mch_job_status(job);
	જો (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    પાછા result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    પૂર્ણાંક
job_stop(job_T *job, typval_T *argvars, અક્ષર *type)
{
    char_u *arg;

    જો (type != ખાલી)
	arg = (char_u *)type;
    નહીંતર જો (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    નહીંતર
    {
	arg = tv_get_string_chk(&argvars[1]);
	જો (arg == ખાલી)
	{
	    emsg(_(e_invalid_argument));
	    પાછા 0;
	}
    }
    જો (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	પાછા 0;
    }
    જો (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	પાછા 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (અક્ષર *)arg);
    જો (mch_signal_job(job, arg) == FAIL)
	પાછા 0;

    // Assume that only "kill" will kill the job.
    જો (job->jv_channel != ખાલી && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    પાછા 1;
}

    શૂન્ય
invoke_prompt_callback(શૂન્ય)
{
    typval_T	rettv;
    typval_T	argv[2];
    char_u	*text;
    char_u	*prompt;
    linenr_T	lnum = curbuf->b_ml.ml_લીટી_ગણતરી;

    // Add a new line for the prompt before invoking the callback, so that
    // text can always be inserted above the last line.
    ml_append(lnum, (char_u  *)"", 0, FALSE);
    curwin->w_cursor.lnum = lnum + 1;
    curwin->w_cursor.col = 0;

    જો (curbuf->b_prompt_callback.cb_name == ખાલી
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	પાછા;
    text = ml_get(lnum);
    prompt = prompt_text();
    જો (STRLEN(text) >= STRLEN(prompt))
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
    પૂર્ણાંક
invoke_prompt_interrupt(શૂન્ય)
{
    typval_T	rettv;
    typval_T	argv[1];
    પૂર્ણાંક		ret;

    જો (curbuf->b_prompt_interrupt.cb_name == ખાલી
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	પાછા FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    પાછા ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    સ્થિર char_u *
buf_prompt_text(buf_T* buf)
{
    જો (buf->b_prompt_text == ખાલી)
	પાછા (char_u *)"% ";
    પાછા buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(શૂન્ય)
{
    પાછા buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    શૂન્ય
init_prompt(પૂર્ણાંક cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
    text = ml_get_curline();
    જો (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	જો (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_લીટી_ગણતરી, prompt, TRUE);
	નહીંતર
	    ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_લીટી_ગણતરી, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    જો (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (પૂર્ણાંક)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (પૂર્ણાંક)STRLEN(prompt));

    જો (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    જો (curwin->w_cursor.col < (પૂર્ણાંક)STRLEN(prompt))
	curwin->w_cursor.col = (પૂર્ણાંક)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    પૂર્ણાંક
prompt_curpos_editable(શૂન્ય)
{
    પાછા curwin->w_cursor.lnum == curbuf->b_ml.ml_લીટી_ગણતરી
	&& curwin->w_cursor.col >= (પૂર્ણાંક)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    શૂન્ય
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    જો (check_secure())
	પાછા;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	પાછા;

    callback = get_callback(&argvars[1]);
    જો (callback.cb_name == ખાલી)
	પાછા;

    free_callback(&buf->b_prompt_callback);
    set_callback(&buf->b_prompt_callback, &callback);
}

/*
 * "prompt_setinterrupt({buffer}, {callback})" function
 */
    શૂન્ય
f_prompt_setinterrupt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    જો (check_secure())
	પાછા;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	પાછા;

    callback = get_callback(&argvars[1]);
    જો (callback.cb_name == ખાલી)
	પાછા;

    free_callback(&buf->b_prompt_interrupt);
    set_callback(&buf->b_prompt_interrupt, &callback);
}


/*
 * "prompt_getprompt({buffer})" function
 */
    શૂન્ય
f_prompt_getprompt(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    // return an empty string by default, e.g. it's not a prompt buffer
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = tv_get_buf_from_arg(&argvars[0]);
    જો (buf == ખાલી)
	પાછા;

    જો (!bt_prompt(buf))
	પાછા;

    rettv->vval.v_string = vim_strsave(buf_prompt_text(buf));
}

/*
 * "prompt_setprompt({buffer}, {text})" function
 */
    શૂન્ય
f_prompt_setprompt(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*text;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    જો (check_secure())
	પાછા;
    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	પાછા;

    text = tv_get_string(&argvars[1]);
    vim_free(buf->b_prompt_text);
    buf->b_prompt_text = vim_strsave(text);
}

/*
 * Get the job from the argument.
 * Returns NULL if the job is invalid.
 */
    સ્થિર job_T *
get_job_arg(typval_T *tv)
{
    job_T *job;

    જો (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	પાછા ખાલી;
    }
    job = tv->vval.v_job;

    જો (job == ખાલી)
	emsg(_(e_not_valid_job));
    પાછા job;
}

/*
 * "job_getchannel()" function
 */
    શૂન્ય
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    જો (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	પાછા;

    job = get_job_arg(&argvars[0]);
    જો (job == ખાલી)
	પાછા;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    જો (job->jv_channel != ખાલી)
	++job->jv_channel->ch_refcount;
}

/*
 * Implementation of job_info().
 */
    સ્થિર શૂન્ય
job_info(job_T *job, dict_T *dict)
{
    dictitem_T	*item;
    varnumber_T	nr;
    list_T	*l;
    પૂર્ણાંક		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    જો (item == ખાલી)
	પાછા;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    જો (job->jv_channel != ખાલી)
	++job->jv_channel->ch_refcount;
    જો (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#નહીંતર
    nr = job->jv_proc_info.dwProcessId;
#endif
    dict_add_સંખ્યા(dict, "process", nr);
    dict_add_string(dict, "tty_in", job->jv_tty_in);
    dict_add_string(dict, "tty_out", job->jv_tty_out);

    dict_add_સંખ્યા(dict, "exitval", job->jv_exitval);
    dict_add_string(dict, "exit_cb", job->jv_exit_cb.cb_name);
    dict_add_string(dict, "stoponexit", job->jv_stoponexit);
#ifdef UNIX
    dict_add_string(dict, "termsig", job->jv_termsig);
#endif
#ifdef MSWIN
    dict_add_string(dict, "tty_type", job->jv_tty_type);
#endif

    l = list_alloc();
    જો (l == ખાલી)
	પાછા;

    જો (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	પાછા;
    }
    જો (job->jv_argv != ખાલી)
	માટે (i = 0; job->jv_argv[i] != ખાલી; i++)
	    list_append_string(l, (char_u *)job->jv_argv[i], -1);
}

/*
 * Implementation of job_info() to return info for all jobs.
 */
    સ્થિર શૂન્ય
job_info_all(list_T *l)
{
    job_T	*job;
    typval_T	tv;

    FOR_ALL_JOBS(job)
    {
	tv.v_type = VAR_JOB;
	tv.vval.v_job = job;

	જો (list_append_tv(l, &tv) != OK)
	    પાછા;
    }
}

/*
 * "job_info()" function
 */
    શૂન્ય
f_job_info(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	જો (job != ખાલી && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    નહીંતર જો (rettv_list_alloc(rettv) == OK)
	job_info_all(rettv->vval.v_list);
}

/*
 * "job_setoptions()" function
 */
    શૂન્ય
f_job_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    job_T	*job;
    jobopt_T	opt;

    જો (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	પાછા;

    job = get_job_arg(&argvars[0]);
    જો (job == ખાલી)
	પાછા;
    clear_job_options(&opt);
    જો (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
	job_set_options(job, &opt);
    free_job_options(&opt);
}

/*
 * "job_start()" function
 */
    શૂન્ય
f_job_start(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_JOB;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_job = job_start(argvars, ખાલી, ખાલી, ખાલી);
}

/*
 * "job_status()" function
 */
    શૂન્ય
f_job_status(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == ખાલી)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    નહીંતર
    {
	job_T	*job = get_job_arg(&argvars[0]);

	જો (job != ખાલી)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = vim_strsave((char_u *)job_status(job));
	}
    }
}

/*
 * "job_stop()" function
 */
    શૂન્ય
f_job_stop(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    જો (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    job = get_job_arg(&argvars[0]);
    જો (job != ખાલી)
	rettv->vval.v_સંખ્યા = job_stop(job, argvars, ખાલી);
}

/*
 * Get a string with information about the job in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
job_to_string_buf(typval_T *varp, char_u *buf)
{
    job_T *job = varp->vval.v_job;
    અક્ષર  *status;

    જો (job == ખાલી)
    {
	vim_snprintf((અક્ષર *)buf, NUMBUFLEN, "no process");
	પાછા buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((અક્ષર *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#અથવાજો defined(MSWIN)
    vim_snprintf((અક્ષર *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#નહીંતર
    // fall-back
    vim_snprintf((અક્ષર *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    પાછા buf;
}

#endif // FEAT_JOB_CHANNEL
