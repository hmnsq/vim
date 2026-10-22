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

#ако defined(FEAT_JOB_CHANNEL)

#define FOR_ALL_JOBS(job) \
    за ((job) = first_job; (job) != NULL; (job) = (job)->jv_next)

    static цяло
handle_mode(typval_T *item, jobopt_T *opt, ch_mode_T *modep, цяло jo)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= jo;
    ако (STRCMP(val, "nl") == 0)
	*modep = CH_MODE_NL;
    иначе ако (STRCMP(val, "raw") == 0)
	*modep = CH_MODE_RAW;
    иначе ако (STRCMP(val, "blob") == 0)
	*modep = CH_MODE_BLOB;
    иначе ако (STRCMP(val, "js") == 0)
	*modep = CH_MODE_JS;
    иначе ако (STRCMP(val, "json") == 0)
	*modep = CH_MODE_JSON;
    иначе ако (STRCMP(val, "lsp") == 0)
	*modep = CH_MODE_LSP;
    иначе ако (STRCMP(val, "dap") == 0)
	*modep = CH_MODE_DAP;
    иначе
    {
	semsg(_(e_invalid_argument_str), val);
	върни FAIL;
    }
    върни OK;
}

    static цяло
handle_io(typval_T *item, ch_part_T part, jobopt_T *opt)
{
    char_u	*val = tv_get_string(item);

    opt->jo_set |= JO_OUT_IO << (part - PART_OUT);
    ако (STRCMP(val, "null") == 0)
	opt->jo_io[part] = JIO_NULL;
    иначе ако (STRCMP(val, "pipe") == 0)
	opt->jo_io[part] = JIO_PIPE;
    иначе ако (STRCMP(val, "file") == 0)
	opt->jo_io[part] = JIO_FILE;
    иначе ако (STRCMP(val, "buffer") == 0)
	opt->jo_io[part] = JIO_BUFFER;
    иначе ако (STRCMP(val, "out") == 0 && part == PART_ERR)
	opt->jo_io[part] = JIO_OUT;
    иначе
    {
	semsg(_(e_invalid_argument_str), val);
	върни FAIL;
    }
    върни OK;
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
    ако (cb->cb_partial != NULL)
	partial_unref(cb->cb_partial);
    иначе ако (cb->cb_name != NULL)
    {
	func_unref(cb->cb_name);
	ако (cb->cb_free_name)
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

    ако (opt->jo_env != NULL)
	dict_unref(opt->jo_env);
}

/*
 * Get the PART_ number from the first character of an option name.
 */
    static цяло
part_from_char(цяло c)
{
    върни c == 'i' ? PART_IN : c == 'o' ? PART_OUT: PART_ERR;
}

/*
 * Get the option entries from the dict in "tv", parse them and put the result
 * in "opt".
 * Only accept JO_ options in "supported" and JO2_ options in "supported2".
 * If an option value is invalid return FAIL.
 */
    цяло
get_job_options(typval_T *tv, jobopt_T *opt, цяло supported, цяло supported2)
{
    typval_T	*item;
    char_u	*val;
    dict_T	*dict;
    цяло		todo;
    hashitem_T	*hi;
    ch_part_T	part;

    ако (tv->v_type == VAR_UNKNOWN)
	върни OK;
    ако (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	върни FAIL;
    }
    dict = tv->vval.v_dict;
    ако (dict == NULL)
	върни OK;

    todo = (цяло)dict->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&dict->dv_hashtab, hi, todo)
	ако (!HASHITEM_EMPTY(hi))
	{
	    item = &dict_lookup(hi)->di_tv;

	    ако (STRCMP(hi->hi_key, "mode") == 0)
	    {
		ако (!(supported & JO_MODE))
		    break;
		ако (handle_mode(item, opt, &opt->jo_mode, JO_MODE) == FAIL)
		    върни FAIL;
	    }
	    иначе ако (STRCMP(hi->hi_key, "in_mode") == 0)
	    {
		ако (!(supported & JO_IN_MODE))
		    break;
		ако (handle_mode(item, opt, &opt->jo_in_mode, JO_IN_MODE)
								      == FAIL)
		    върни FAIL;
	    }
	    иначе ако (STRCMP(hi->hi_key, "out_mode") == 0)
	    {
		ако (!(supported & JO_OUT_MODE))
		    break;
		ако (handle_mode(item, opt, &opt->jo_out_mode, JO_OUT_MODE)
								      == FAIL)
		    върни FAIL;
	    }
	    иначе ако (STRCMP(hi->hi_key, "err_mode") == 0)
	    {
		ако (!(supported & JO_ERR_MODE))
		    break;
		ако (handle_mode(item, opt, &opt->jo_err_mode, JO_ERR_MODE)
								      == FAIL)
		    върни FAIL;
	    }
	    иначе ако (STRCMP(hi->hi_key, "noblock") == 0)
	    {
		ако (!(supported & JO_MODE))
		    break;
		opt->jo_noblock = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "in_io") == 0
		    || STRCMP(hi->hi_key, "out_io") == 0
		    || STRCMP(hi->hi_key, "err_io") == 0)
	    {
		ако (!(supported & JO_OUT_IO))
		    break;
		ако (handle_io(item, part_from_char(*hi->hi_key), opt) == FAIL)
		    върни FAIL;
	    }
	    иначе ако (STRCMP(hi->hi_key, "in_name") == 0
		    || STRCMP(hi->hi_key, "out_name") == 0
		    || STRCMP(hi->hi_key, "err_name") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ако (!(supported & JO_OUT_IO))
		    break;
		opt->jo_set |= JO_OUT_NAME << (part - PART_OUT);
		opt->jo_io_name[part] = tv_get_string_buf_chk(item,
						   opt->jo_io_name_buf[part]);
	    }
	    иначе ако (STRCMP(hi->hi_key, "pty") == 0)
	    {
		ако (!(supported & JO_MODE))
		    break;
		opt->jo_pty = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "in_buf") == 0
		    || STRCMP(hi->hi_key, "out_buf") == 0
		    || STRCMP(hi->hi_key, "err_buf") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ако (!(supported & JO_OUT_IO))
		    break;
		opt->jo_set |= JO_OUT_BUF << (part - PART_OUT);
		opt->jo_io_buf[part] = tv_get_number(item);
		ако (opt->jo_io_buf[part] <= 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
					      hi->hi_key, tv_get_string(item));
		    върни FAIL;
		}
		ако (buflist_findnr(opt->jo_io_buf[part]) == NULL)
		{
		    semsg(_(e_buffer_nr_does_not_exist),
						   (long)opt->jo_io_buf[part]);
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "out_modifiable") == 0
		    || STRCMP(hi->hi_key, "err_modifiable") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ако (!(supported & JO_OUT_IO))
		    break;
		opt->jo_set |= JO_OUT_MODIFIABLE << (part - PART_OUT);
		opt->jo_modifiable[part] = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "out_msg") == 0
		    || STRCMP(hi->hi_key, "err_msg") == 0)
	    {
		part = part_from_char(*hi->hi_key);

		ако (!(supported & JO_OUT_IO))
		    break;
		opt->jo_set2 |= JO2_OUT_MSG << (part - PART_OUT);
		opt->jo_message[part] = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "in_top") == 0
		    || STRCMP(hi->hi_key, "in_bot") == 0)
	    {
		linenr_T *lp;

		ако (!(supported & JO_OUT_IO))
		    break;
		ако (hi->hi_key[3] == 't')
		{
		    lp = &opt->jo_in_top;
		    opt->jo_set |= JO_IN_TOP;
		}
		иначе
		{
		    lp = &opt->jo_in_bot;
		    opt->jo_set |= JO_IN_BOT;
		}
		*lp = tv_get_number(item);
		ако (*lp < 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
					      hi->hi_key, tv_get_string(item));
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "channel") == 0)
	    {
		ако (!(supported & JO_OUT_IO))
		    break;
		opt->jo_set |= JO_CHANNEL;
		ако (item->v_type != VAR_CHANNEL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "channel");
		    върни FAIL;
		}
		opt->jo_channel = item->vval.v_channel;
	    }
	    иначе ако (STRCMP(hi->hi_key, "callback") == 0)
	    {
		ако (!(supported & JO_CALLBACK))
		    break;
		opt->jo_set |= JO_CALLBACK;
		opt->jo_callback = get_callback(item);
		ако (opt->jo_callback.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "callback");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "out_cb") == 0)
	    {
		ако (!(supported & JO_OUT_CALLBACK))
		    break;
		opt->jo_set |= JO_OUT_CALLBACK;
		opt->jo_out_cb = get_callback(item);
		ако (opt->jo_out_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "out_cb");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "err_cb") == 0)
	    {
		ако (!(supported & JO_ERR_CALLBACK))
		    break;
		opt->jo_set |= JO_ERR_CALLBACK;
		opt->jo_err_cb = get_callback(item);
		ако (opt->jo_err_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "err_cb");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "close_cb") == 0)
	    {
		ако (!(supported & JO_CLOSE_CALLBACK))
		    break;
		opt->jo_set |= JO_CLOSE_CALLBACK;
		opt->jo_close_cb = get_callback(item);
		ако (opt->jo_close_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "close_cb");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "drop") == 0)
	    {
		цяло never = FALSE;
		val = tv_get_string(item);

		ако (STRCMP(val, "never") == 0)
		    never = TRUE;
		иначе ако (STRCMP(val, "auto") != 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str), "drop", val);
		    върни FAIL;
		}
		opt->jo_drop_never = never;
	    }
	    иначе ако (STRCMP(hi->hi_key, "exit_cb") == 0)
	    {
		ако (!(supported & JO_EXIT_CB))
		    break;
		opt->jo_set |= JO_EXIT_CB;
		opt->jo_exit_cb = get_callback(item);
		ако (opt->jo_exit_cb.cb_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "exit_cb");
		    върни FAIL;
		}
	    }
#ifdef FEAT_TERMINAL
	    иначе ако (STRCMP(hi->hi_key, "term_name") == 0)
	    {
		ако (!(supported2 & JO2_TERM_NAME))
		    break;
		opt->jo_set2 |= JO2_TERM_NAME;
		opt->jo_term_name = tv_get_string_buf_chk(item,
						       opt->jo_term_name_buf);
		ако (opt->jo_term_name == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_name");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_finish") == 0)
	    {
		ако (!(supported2 & JO2_TERM_FINISH))
		    break;
		val = tv_get_string(item);
		ако (STRCMP(val, "open") != 0 && STRCMP(val, "close") != 0
			&& STRCMP(val, "noclose") != 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str),
							   "term_finish", val);
		    върни FAIL;
		}
		opt->jo_set2 |= JO2_TERM_FINISH;
		opt->jo_term_finish = *val;
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_opencmd") == 0)
	    {
		char_u *p;

		ако (!(supported2 & JO2_TERM_OPENCMD))
		    break;
		opt->jo_set2 |= JO2_TERM_OPENCMD;
		p = opt->jo_term_opencmd = tv_get_string_buf_chk(item,
						    opt->jo_term_opencmd_buf);
		ако (p != NULL)
		{
		    // Must have %d and no other %.
		    p = vim_strchr(p, '%');
		    ако (p != NULL && (p[1] != 'd'
					    || vim_strchr(p + 2, '%') != NULL))
			p = NULL;
		}
		ако (p == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_opencmd");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "eof_chars") == 0)
	    {
		ако (!(supported2 & JO2_EOF_CHARS))
		    break;
		opt->jo_set2 |= JO2_EOF_CHARS;
		opt->jo_eof_chars = tv_get_string_buf_chk(item,
						       opt->jo_eof_chars_buf);
		ако (opt->jo_eof_chars == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "eof_chars");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_rows") == 0)
	    {
		цяло error = FALSE;

		ако (!(supported2 & JO2_TERM_ROWS))
		    break;
		opt->jo_set2 |= JO2_TERM_ROWS;
		opt->jo_term_rows = tv_get_number_chk(item, &error);
		ако (error)
		    върни FAIL;
		ако (opt->jo_term_rows < 0 || opt->jo_term_rows > 1000)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_rows");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_cols") == 0)
	    {
		цяло error = FALSE;

		ако (!(supported2 & JO2_TERM_COLS))
		    break;
		opt->jo_set2 |= JO2_TERM_COLS;
		opt->jo_term_cols = tv_get_number_chk(item, &error);
		ако (error)
		    върни FAIL;
		ако (opt->jo_term_cols < 0 || opt->jo_term_cols > 1000)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_cols");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "vertical") == 0)
	    {
		ако (!(supported2 & JO2_VERTICAL))
		    break;
		opt->jo_set2 |= JO2_VERTICAL;
		opt->jo_vertical = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "curwin") == 0)
	    {
		ако (!(supported2 & JO2_CURWIN))
		    break;
		opt->jo_set2 |= JO2_CURWIN;
		opt->jo_curwin = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "bufnr") == 0)
	    {
		цяло nr;

		ако (!(supported2 & JO2_CURWIN))
		    break;
		opt->jo_set2 |= JO2_BUFNR;
		nr = tv_get_number(item);
		ако (nr <= 0)
		{
		    semsg(_(e_invalid_value_for_argument_str_str), hi->hi_key, tv_get_string(item));
		    върни FAIL;
		}
		opt->jo_bufnr_buf = buflist_findnr(nr);
		ако (opt->jo_bufnr_buf == NULL)
		{
		    semsg(_(e_buffer_nr_does_not_exist), (long)nr);
		    върни FAIL;
		}
		ако (opt->jo_bufnr_buf->b_nwindows == 0
			|| opt->jo_bufnr_buf->b_term == NULL)
		{
		    semsg(_(e_invalid_argument_str), "bufnr");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "hidden") == 0)
	    {
		ако (!(supported2 & JO2_HIDDEN))
		    break;
		opt->jo_set2 |= JO2_HIDDEN;
		opt->jo_hidden = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "norestore") == 0)
	    {
		ако (!(supported2 & JO2_NORESTORE))
		    break;
		opt->jo_set2 |= JO2_NORESTORE;
		opt->jo_term_norestore = tv_get_bool(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_kill") == 0)
	    {
		ако (!(supported2 & JO2_TERM_KILL))
		    break;
		opt->jo_set2 |= JO2_TERM_KILL;
		opt->jo_term_kill = tv_get_string_buf_chk(item,
						       opt->jo_term_kill_buf);
		ако (opt->jo_term_kill == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_kill");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "tty_type") == 0)
	    {
		char_u *p;

		ако (!(supported2 & JO2_TTY_TYPE))
		    break;
		opt->jo_set2 |= JO2_TTY_TYPE;
		p = tv_get_string_chk(item);
		ако (p == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "tty_type");
		    върни FAIL;
		}
		// Allow empty string, "winpty", "conpty".
		ако (!(*p == NUL || STRCMP(p, "winpty") == 0
						  || STRCMP(p, "conpty") == 0))
		{
		    semsg(_(e_invalid_value_for_argument_str), "tty_type");
		    върни FAIL;
		}
		opt->jo_tty_type = p[0];
	    }
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    иначе ако (STRCMP(hi->hi_key, "ansi_colors") == 0)
	    {
		цяло		n = 0;
		listitem_T	*li;
		long_u		rgb[16];

		ако (!(supported2 & JO2_ANSI_COLORS))
		    break;

		ако (item == NULL || item->v_type != VAR_LIST
			|| item->vval.v_list == NULL
			|| item->vval.v_list->lv_first == &range_list_item)
		{
		    semsg(_(e_invalid_value_for_argument_str), "ansi_colors");
		    върни FAIL;
		}

		li = item->vval.v_list->lv_first;
		за (; li != NULL && n < 16; li = li->li_next, n++)
		{
		    char_u	*color_name;
		    guicolor_T	guicolor;
		    цяло		called_emsg_before = called_emsg;

		    color_name = tv_get_string_chk(&li->li_tv);
		    ако (color_name == NULL)
			върни FAIL;

		    guicolor = GUI_GET_COLOR(color_name);
		    ако (guicolor == INVALCOLOR)
		    {
			ако (called_emsg_before == called_emsg)
			    // may not get the error if the GUI didn't start
			    semsg(_(e_cannot_allocate_color_str), color_name);
			върни FAIL;
		    }

		    rgb[n] = GUI_MCH_GET_RGB(guicolor);
		}

		ако (n != 16 || li != NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "ansi_colors");
		    върни FAIL;
		}

		opt->jo_set2 |= JO2_ANSI_COLORS;
		memcpy(opt->jo_ansi_colors, rgb, sizeof(rgb));
	    }
# endif
	    иначе ако (STRCMP(hi->hi_key, "term_highlight") == 0)
	    {
		char_u *p;

		ако (!(supported2 & JO2_TERM_HIGHLIGHT))
		    break;
		opt->jo_set2 |= JO2_TERM_HIGHLIGHT;
		p = tv_get_string_buf_chk(item, opt->jo_term_highlight_buf);
		ако (p == NULL || *p == NUL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_highlight");
		    върни FAIL;
		}
		opt->jo_term_highlight = p;
	    }
	    иначе ако (STRCMP(hi->hi_key, "term_api") == 0)
	    {
		ако (!(supported2 & JO2_TERM_API))
		    break;
		opt->jo_set2 |= JO2_TERM_API;
		opt->jo_term_api = tv_get_string_buf_chk(item,
							opt->jo_term_api_buf);
		ако (opt->jo_term_api == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "term_api");
		    върни FAIL;
		}
	    }
#endif
	    иначе ако (STRCMP(hi->hi_key, "env") == 0)
	    {
		ако (!(supported2 & JO2_ENV))
		    break;
		ако (item->v_type != VAR_DICT)
		{
		    semsg(_(e_invalid_value_for_argument_str), "env");
		    върни FAIL;
		}
		opt->jo_set2 |= JO2_ENV;
		opt->jo_env = item->vval.v_dict;
		ако (opt->jo_env != NULL)
		    ++opt->jo_env->dv_refcount;
	    }
	    иначе ако (STRCMP(hi->hi_key, "cwd") == 0)
	    {
		ако (!(supported2 & JO2_CWD))
		    break;
		opt->jo_cwd = tv_get_string_buf_chk(item, opt->jo_cwd_buf);
		ако (opt->jo_cwd == NULL || !mch_isdir(opt->jo_cwd)
#ifndef MSWIN  // Win32 directories don't have the concept of "executable"
				|| mch_access((char *)opt->jo_cwd, X_OK) != 0
#endif
				)
		{
		    semsg(_(e_invalid_value_for_argument_str), "cwd");
		    върни FAIL;
		}
		opt->jo_set2 |= JO2_CWD;
	    }
	    иначе ако (STRCMP(hi->hi_key, "waittime") == 0)
	    {
		ако (!(supported & JO_WAITTIME))
		    break;
		opt->jo_set |= JO_WAITTIME;
		opt->jo_waittime = tv_get_number(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "timeout") == 0)
	    {
		ако (!(supported & JO_TIMEOUT))
		    break;
		opt->jo_set |= JO_TIMEOUT;
		opt->jo_timeout = tv_get_number(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "out_timeout") == 0)
	    {
		ако (!(supported & JO_OUT_TIMEOUT))
		    break;
		opt->jo_set |= JO_OUT_TIMEOUT;
		opt->jo_out_timeout = tv_get_number(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "err_timeout") == 0)
	    {
		ако (!(supported & JO_ERR_TIMEOUT))
		    break;
		opt->jo_set |= JO_ERR_TIMEOUT;
		opt->jo_err_timeout = tv_get_number(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "part") == 0)
	    {
		ако (!(supported & JO_PART))
		    break;
		opt->jo_set |= JO_PART;
		val = tv_get_string(item);
		ако (STRCMP(val, "err") == 0)
		    opt->jo_part = PART_ERR;
		иначе ако (STRCMP(val, "out") == 0)
		    opt->jo_part = PART_OUT;
		иначе
		{
		    semsg(_(e_invalid_value_for_argument_str_str), "part", val);
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "id") == 0)
	    {
		ако (!(supported & JO_ID))
		    break;
		opt->jo_set |= JO_ID;
		opt->jo_id = tv_get_number(item);
	    }
	    иначе ако (STRCMP(hi->hi_key, "stoponexit") == 0)
	    {
		ако (!(supported & JO_STOPONEXIT))
		    break;
		opt->jo_set |= JO_STOPONEXIT;
		opt->jo_stoponexit = tv_get_string_buf_chk(item,
						      opt->jo_stoponexit_buf);
		ако (opt->jo_stoponexit == NULL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "stoponexit");
		    върни FAIL;
		}
	    }
	    иначе ако (STRCMP(hi->hi_key, "block_write") == 0)
	    {
		ако (!(supported & JO_BLOCK_WRITE))
		    break;
		opt->jo_set |= JO_BLOCK_WRITE;
		opt->jo_block_write = tv_get_number(item);
	    }
	    иначе
		break;
	    --todo;
	}
    ако (todo > 0)
    {
	semsg(_(e_invalid_argument_str), hi->hi_key);
	върни FAIL;
    }

    върни OK;
}

static job_T *first_job = NULL;

    static void
job_free_contents(job_T *job)
{
    цяло		i;

    ch_log(job->jv_channel, "Freeing job");
    ако (job->jv_channel != NULL)
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
    ако (job->jv_argv != NULL)
    {
	за (i = 0; job->jv_argv[i] != NULL; i++)
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
    ако (job->jv_next != NULL)
	job->jv_next->jv_prev = job->jv_prev;
    ако (job->jv_prev == NULL)
	first_job = job->jv_next;
    иначе
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
    ако (in_free_unref_items)
	върни;

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

    докато (jobs_to_free != NULL)
    {
	job = jobs_to_free;
	jobs_to_free = job->jv_next;
	job_free_contents(job);
	vim_free(job);
    }
}

#ако defined(EXITFREE)
    void
job_free_all(void)
{
    докато (first_job != NULL)
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
    static цяло
job_need_end_check(job_T *job)
{
    върни job->jv_status == JOB_STARTED
	    && (job->jv_stoponexit != NULL || job->jv_exit_cb.cb_name != NULL);
}

/*
 * Return TRUE if the channel of "job" is still useful.
 */
    static цяло
job_channel_still_useful(job_T *job)
{
    върни job->jv_channel != NULL && channel_still_useful(job->jv_channel);
}

/*
 * Return TRUE if the channel of "job" is closeable.
 */
    static цяло
job_channel_can_close(job_T *job)
{
    върни job->jv_channel != NULL && channel_can_close(job->jv_channel);
}

/*
 * Return TRUE if the job should not be freed yet.  Do not free the job when
 * it has not ended yet and there is a "stoponexit" flag, an exit callback
 * or when the associated channel will do something with the job output.
 */
    static цяло
job_still_useful(job_T *job)
{
    върни job_need_end_check(job) || job_channel_still_useful(job);
}

#ако defined(GUI_MAY_FORK) || defined(GUI_MAY_SPAWN)
/*
 * Return TRUE when there is any running job that we care about.
 */
    цяло
job_any_running(void)
{
    job_T	*job;

    FOR_ALL_JOBS(job)
	ако (job_still_useful(job))
	{
	    ch_log(NULL, "GUI not forking because a job is running");
	    върни TRUE;
	}
    върни FALSE;
}
#endif

// Unix uses argv[] for the command, other systems use a string.
#ако defined(UNIX)
# define USE_ARGV
#endif

#ако !defined(USE_ARGV)
/*
 * Escape one argument for an external command.
 * Returns the escaped string in allocated memory.  NULL when out of memory.
 */
    static char_u *
win32_escape_arg(char_u *arg)
{
    цяло		slen, dlen;
    цяло		escaping = 0;
    цяло		i;
    char_u	*s, *d;
    char_u	*escaped_arg;
    цяло		has_spaces = FALSE;

    // First count the number of extra bytes required.
    slen = (цяло)STRLEN(arg);
    dlen = slen;
    за (s = arg; *s != NUL; MB_PTR_ADV(s))
    {
	ако (*s == '"' || *s == '\\')
	    ++dlen;
	ако (*s == ' ' || *s == '\t')
	    has_spaces = TRUE;
    }

    ако (has_spaces)
	dlen += 2;

    ако (dlen == slen)
	върни vim_strsave(arg);

    // Allocate memory for the result and fill it.
    escaped_arg = alloc(dlen + 1);
    ако (escaped_arg == NULL)
	върни NULL;
    memset(escaped_arg, 0, dlen+1);

    d = escaped_arg;

    ако (has_spaces)
	*d++ = '"';

    за (s = arg; *s != NUL;)
    {
	switch (*s)
	{
	    case '"':
		за (i = 0; i < escaping; i++)
		    *d++ = '\\';
		escaping = 0;
		*d++ = '\\';
		*d++ = *s++;
		break;
	    case '\\':
		escaping++;
		*d++ = *s++;
		break;
	    default:
		escaping = 0;
		MB_COPY_CHAR(s, d);
		break;
	}
    }

    // add terminating quote and finish with a NUL
    ако (has_spaces)
    {
	за (i = 0; i < escaping; i++)
	    *d++ = '\\';
	*d++ = '"';
    }
    *d = NUL;

    върни escaped_arg;
}

/*
 * Build a command line from a list, taking care of escaping.
 * The result is put in gap->ga_data.
 * Returns FAIL when out of memory.
 */
    цяло
win32_build_cmd(list_T *l, garray_T *gap)
{
    listitem_T  *li;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ако (s == NULL)
	    върни FAIL;
	s = win32_escape_arg(s);
	ако (s == NULL)
	    върни FAIL;
	ga_concat(gap, s);
	vim_free(s);
	ако (li->li_next != NULL)
	    ga_append(gap, ' ');
    }
    върни OK;
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
    ако (job->jv_status != JOB_ENDED)
	върни;

    // Ready to cleanup the job.
    job->jv_status = JOB_FINISHED;

    // When only channel-in is kept open, close explicitly.
    ако (job->jv_channel != NULL)
	ch_close_part(job->jv_channel, PART_IN);

    ако (job->jv_exit_cb.cb_name != NULL)
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

    ако (job->jv_channel != NULL && job->jv_channel->ch_anonymous_pipe)
	job->jv_channel->ch_killing = TRUE;

    // Do not free the job in case the close callback of the associated channel
    // isn't invoked yet and may get information by job_info().
    ако (job->jv_refcount == 0 && !job_channel_still_useful(job))
	// The job was already unreferenced and the associated channel was
	// detached, now that it ended it can be freed. However, a caller might
	// still use it, thus free it a bit later.
	job_free_later(job);
}

/*
 * Mark references in jobs that are still useful.
 */
    цяло
set_ref_in_job(цяло copyID)
{
    цяло		abort = FALSE;
    job_T	*job;
    typval_T	tv;

    за (job = first_job; !abort && job != NULL; job = job->jv_next)
	ако (job_still_useful(job))
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = job;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    върни abort;
}

/*
 * Dereference "job".  Note that after this "job" may have been freed.
 */
    void
job_unref(job_T *job)
{
    ако (job == NULL || --job->jv_refcount > 0)
	върни;

    // Do not free the job if there is a channel where the close callback
    // may get the job info.
    ако (job_channel_still_useful(job))
	върни;

    // Do not free the job when it has not ended yet and there is a
    // "stoponexit" flag or an exit callback.
    ако (!job_need_end_check(job))
    {
	job_free(job);
    }
    иначе ако (job->jv_channel != NULL)
    {
	// Do remove the link to the channel, otherwise it hangs
	// around until Vim exits. See job_free() for refcount.
	ch_log(job->jv_channel, "detaching channel from job");
	job->jv_channel->ch_job = NULL;
	channel_unref(job->jv_channel);
	job->jv_channel = NULL;
    }
}

    цяло
free_unused_jobs_contents(цяло copyID, цяло mask)
{
    цяло		did_free = FALSE;
    job_T	*job;

    FOR_ALL_JOBS(job)
	ако ((job->jv_copyID & mask) != (copyID & mask)
						    && !job_still_useful(job))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    job_free_contents(job);
	    did_free = TRUE;
	}
    върни did_free;
}

    void
free_unused_jobs(цяло copyID, цяло mask)
{
    job_T	*job;
    job_T	*job_next;

    за (job = first_job; job != NULL; job = job_next)
    {
	job_next = job->jv_next;
	ако ((job->jv_copyID & mask) != (copyID & mask)
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
    ако (job == NULL)
	върни NULL;

    job->jv_refcount = 1;
    job->jv_stoponexit = vim_strsave((char_u *)"term");

    ако (first_job != NULL)
    {
	first_job->jv_prev = job;
	job->jv_next = first_job;
    }
    first_job = job;
    върни job;
}

    void
job_set_options(job_T *job, jobopt_T *opt)
{
    ако (opt->jo_set & JO_STOPONEXIT)
    {
	vim_free(job->jv_stoponexit);
	ако (opt->jo_stoponexit == NULL || *opt->jo_stoponexit == NUL)
	    job->jv_stoponexit = NULL;
	иначе
	    job->jv_stoponexit = vim_strsave(opt->jo_stoponexit);
    }
    ако (opt->jo_set & JO_EXIT_CB)
    {
	free_callback(&job->jv_exit_cb);
	ако (opt->jo_exit_cb.cb_name == NULL || *opt->jo_exit_cb.cb_name == NUL)
	{
	    job->jv_exit_cb.cb_name = NULL;
	    job->jv_exit_cb.cb_partial = NULL;
	}
	иначе
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
	ако (job->jv_status == JOB_STARTED && job->jv_stoponexit != NULL)
	    mch_signal_job(job, job->jv_stoponexit);
}

/*
 * Return TRUE when there is any job that has an exit callback and might exit,
 * which means job_check_ended() should be called more often.
 */
    цяло
has_pending_job(void)
{
    job_T	    *job;

    FOR_ALL_JOBS(job)
	// Only should check if the channel has been closed, if the channel is
	// open the job won't exit.
	ако ((job->jv_status == JOB_STARTED && !job_channel_still_useful(job))
		    || (job->jv_status == JOB_FINISHED
					      && job_channel_can_close(job)))
	    върни TRUE;
    върни FALSE;
}

#define MAX_CHECK_ENDED 8

/*
 * Called once in a while: check if any jobs that seem useful have ended.
 * Returns TRUE if a job did end.
 */
    цяло
job_check_ended(void)
{
    цяло		i;
    цяло		did_end = FALSE;

    // be quick if there are no jobs to check
    ако (first_job == NULL)
	върни did_end;

    за (i = 0; i < MAX_CHECK_ENDED; ++i)
    {
	// NOTE: mch_detect_ended_job() must only return a job of which the
	// status was just set to JOB_ENDED.
	job_T	*job = mch_detect_ended_job(first_job);

	ако (job == NULL)
	    break;
	did_end = TRUE;
	job_cleanup(job); // may add "job" to jobs_to_free
    }

    // Actually free jobs that were cleaned up.
    free_jobs_to_free_later();

    ако (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }
    върни did_end;
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
	char	    **argv_arg UNUSED,
	jobopt_T    *opt_arg,
	job_T	    **term_job)
{
    job_T	*job;
    char_u	*cmd = NULL;
    char	**argv = NULL;
    цяло		argc = 0;
    цяло		i;
#ifndef USE_ARGV
    garray_T	ga;
#endif
    jobopt_T	opt;
    ch_part_T	part;

    job = job_alloc();
    ако (job == NULL)
	върни NULL;

    job->jv_status = JOB_FAILED;
#ifndef USE_ARGV
    ga_init2(&ga, sizeof(char*), 20);
#endif

    ако (opt_arg != NULL)
	opt = *opt_arg;
    иначе
    {
	// Default mode is NL.
	clear_job_options(&opt);
	opt.jo_mode = CH_MODE_NL;
	ако (get_job_options(&argvars[1], &opt,
		    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL + JO_STOPONEXIT
			 + JO_EXIT_CB + JO_OUT_IO + JO_BLOCK_WRITE,
		     JO2_ENV + JO2_CWD) == FAIL)
	    goto theend;
    }

    // Check that when io is "file" that there is a file name.
    за (part = PART_OUT; part < PART_COUNT; ++part)
	ако ((opt.jo_set & (JO_OUT_IO << (part - PART_OUT)))
		&& opt.jo_io[part] == JIO_FILE
		&& (!(opt.jo_set & (JO_OUT_NAME << (part - PART_OUT)))
		    || *opt.jo_io_name[part] == NUL))
	{
	    emsg(_(e_io_file_requires_name_to_be_set));
	    goto theend;
	}

    ако ((opt.jo_set & JO_IN_IO) && opt.jo_io[PART_IN] == JIO_BUFFER)
    {
	buf_T *buf = NULL;

	// check that we can find the buffer before starting the job
	ако (opt.jo_set & JO_IN_BUF)
	{
	    buf = buflist_findnr(opt.jo_io_buf[PART_IN]);
	    ако (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
						 (long)opt.jo_io_buf[PART_IN]);
	}
	иначе ако (!(opt.jo_set & JO_IN_NAME))
	{
	    emsg(_(e_in_io_buffer_requires_in_buf_or_in_name_to_be_set));
	}
	иначе
	    buf = buflist_find_by_name(opt.jo_io_name[PART_IN], FALSE);
	ако (buf == NULL)
	    goto theend;
	ако (buf->b_ml.ml_mfp == NULL)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*s;

	    ако (opt.jo_set & JO_IN_BUF)
	    {
		sprintf((char *)numbuf, "%d", opt.jo_io_buf[PART_IN]);
		s = numbuf;
	    }
	    иначе
		s = opt.jo_io_name[PART_IN];
	    semsg(_(e_buffer_must_be_loaded_str), s);
	    goto theend;
	}
	job->jv_in_buf = buf;
    }

    job_set_options(job, &opt);

#ifdef USE_ARGV
    ако (argv_arg != NULL)
    {
	// Make a copy of argv_arg for job->jv_argv.
	за (i = 0; argv_arg[i] != NULL; i++)
	    argc++;
	argv = ALLOC_MULT(char *, argc + 1);
	ако (argv == NULL)
	    goto theend;
	за (i = 0; i < argc; i++)
	    argv[i] = (char *)vim_strsave((char_u *)argv_arg[i]);
	argv[argc] = NULL;
    }
    иначе
#endif
    ако (argvars[0].v_type == VAR_STRING)
    {
	// Command is a string.
	cmd = argvars[0].vval.v_string;
	ако (cmd == NULL || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}

	ако (build_argv_from_string(cmd, &argv, &argc) == FAIL)
	    goto theend;
    }
    иначе ако (argvars[0].v_type != VAR_LIST
	    || argvars[0].vval.v_list == NULL
	    || argvars[0].vval.v_list->lv_len < 1)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }
    иначе
    {
	list_T *l = argvars[0].vval.v_list;

	ако (build_argv_from_list(l, &argv, &argc) == FAIL)
	    goto theend;

	// Empty command is invalid.
	ако (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}
#ifndef USE_ARGV
	ако (win32_build_cmd(l, &ga) == FAIL)
	    goto theend;
	cmd = ga.ga_data;
	ако (cmd == NULL || *skipwhite(cmd) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto theend;
	}
#endif
    }

    // Save the command used to start the job.
    job->jv_argv = argv;

    ако (term_job != NULL)
	*term_job = job;

#ifdef USE_ARGV
    ако (ch_log_active())
    {
	garray_T    ga;

	ga_init2(&ga, sizeof(char), 200);
	за (i = 0; i < argc; ++i)
	{
	    ако (i > 0)
		GA_CONCAT_LITERAL(&ga, "  ");
	    ga_concat(&ga, (char_u *)argv[i]);
	}
	ga_append(&ga, NUL);
	ch_log(NULL, "Starting job: %s", (char *)ga.ga_data);
	ga_clear(&ga);
    }
    mch_job_start(argv, job, &opt, term_job != NULL);
#иначе
    ch_log(NULL, "Starting job: %s", (char *)cmd);
    mch_job_start((char *)cmd, job, &opt);
#endif

    // If the channel is reading from a buffer, write lines now.
    ако (job->jv_channel != NULL)
	channel_write_in(job->jv_channel);

theend:
#ifndef USE_ARGV
    vim_free(ga.ga_data);
#endif
    ако (argv != NULL && argv != job->jv_argv)
    {
	за (i = 0; argv[i] != NULL; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    free_job_options(&opt);
    върни job;
}

/*
 * Get the status of "job" and invoke the exit callback when needed.
 * The returned string is not allocated.
 */
    char *
job_status(job_T *job)
{
    char	*result;

    ако (job->jv_status >= JOB_ENDED)
	// No need to check, dead is dead.
	result = "dead";
    иначе ако (job->jv_status == JOB_FAILED)
	result = "fail";
    иначе
    {
	result = mch_job_status(job);
	ако (job->jv_status == JOB_ENDED)
	    job_cleanup(job);
    }
    върни result;
}

/*
 * Send a signal to "job".  Implements job_stop().
 * When "type" is not NULL use this for the type.
 * Otherwise use argvars[1] for the type.
 */
    цяло
job_stop(job_T *job, typval_T *argvars, char *type)
{
    char_u *arg;

    ако (type != NULL)
	arg = (char_u *)type;
    иначе ако (argvars[1].v_type == VAR_UNKNOWN)
	arg = (char_u *)"";
    иначе
    {
	arg = tv_get_string_chk(&argvars[1]);
	ако (arg == NULL)
	{
	    emsg(_(e_invalid_argument));
	    върни 0;
	}
    }
    ако (job->jv_status == JOB_FAILED)
    {
	ch_log(job->jv_channel, "Job failed to start, job_stop() skipped");
	върни 0;
    }
    ако (job->jv_status == JOB_ENDED)
    {
	ch_log(job->jv_channel, "Job has already ended, job_stop() skipped");
	върни 0;
    }
    ch_log(job->jv_channel, "Stopping job with '%s'", (char *)arg);
    ако (mch_signal_job(job, arg) == FAIL)
	върни 0;

    // Assume that only "kill" will kill the job.
    ако (job->jv_channel != NULL && STRCMP(arg, "kill") == 0)
	job->jv_channel->ch_job_killed = TRUE;

    // We don't try freeing the job, obviously the caller still has a
    // reference to it.
    върни 1;
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

    ако (curbuf->b_prompt_callback.cb_name == NULL
	    || *curbuf->b_prompt_callback.cb_name == NUL)
	върни;
    text = ml_get(lnum);
    prompt = prompt_text();
    ако (STRLEN(text) >= STRLEN(prompt))
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
    цяло
invoke_prompt_interrupt(void)
{
    typval_T	rettv;
    typval_T	argv[1];
    цяло		ret;

    ако (curbuf->b_prompt_interrupt.cb_name == NULL
	    || *curbuf->b_prompt_interrupt.cb_name == NUL)
	върни FALSE;
    argv[0].v_type = VAR_UNKNOWN;

    got_int = FALSE; // don't skip executing commands
    ret = call_callback(&curbuf->b_prompt_interrupt, -1, &rettv, 0, argv);
    clear_tv(&rettv);
    върни ret == FAIL ? FALSE : TRUE;
}

/*
 * Return the effective prompt for the specified buffer.
 */
    static char_u *
buf_prompt_text(buf_T* buf)
{
    ако (buf->b_prompt_text == NULL)
	върни (char_u *)"% ";
    върни buf->b_prompt_text;
}

/*
 * Return the effective prompt for the current buffer.
 */
    char_u *
prompt_text(void)
{
    върни buf_prompt_text(curbuf);
}


/*
 * Prepare for prompt mode: Make sure the last line has the prompt text.
 * Move the cursor to this line.
 */
    void
init_prompt(цяло cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    text = ml_get_curline();
    ако (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	ако (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_line_count, prompt, TRUE);
	иначе
	    ml_append(curbuf->b_ml.ml_line_count, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_line_count, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    ако (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (цяло)STRLEN(prompt))
	set_insstart(curwin->w_cursor.lnum, (цяло)STRLEN(prompt));

    ако (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    ако (curwin->w_cursor.col < (цяло)STRLEN(prompt))
	curwin->w_cursor.col = (цяло)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}

/*
 * Return TRUE if the cursor is in the editable position of the prompt line.
 */
    цяло
prompt_curpos_editable(void)
{
    върни curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count
	&& curwin->w_cursor.col >= (цяло)STRLEN(prompt_text());
}

/*
 * "prompt_setcallback({buffer}, {callback})" function
 */
    void
f_prompt_setcallback(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    callback_T	callback;

    ако (check_secure())
	върни;

    ако (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	върни;

    buf = tv_get_buf(&argvars[0], FALSE);
    ако (buf == NULL)
	върни;

    callback = get_callback(&argvars[1]);
    ако (callback.cb_name == NULL)
	върни;

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

    ако (check_secure())
	върни;

    ако (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	върни;

    buf = tv_get_buf(&argvars[0], FALSE);
    ако (buf == NULL)
	върни;

    callback = get_callback(&argvars[1]);
    ако (callback.cb_name == NULL)
	върни;

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

    ако (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	върни;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ако (buf == NULL)
	върни;

    ако (!bt_prompt(buf))
	върни;

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

    ако (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	върни;

    ако (check_secure())
	върни;
    buf = tv_get_buf(&argvars[0], FALSE);
    ако (buf == NULL)
	върни;

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

    ако (tv->v_type != VAR_JOB)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	върни NULL;
    }
    job = tv->vval.v_job;

    ако (job == NULL)
	emsg(_(e_not_valid_job));
    върни job;
}

/*
 * "job_getchannel()" function
 */
    void
f_job_getchannel(typval_T *argvars, typval_T *rettv)
{
    job_T	*job;

    ако (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	върни;

    job = get_job_arg(&argvars[0]);
    ако (job == NULL)
	върни;

    rettv->v_type = VAR_CHANNEL;
    rettv->vval.v_channel = job->jv_channel;
    ако (job->jv_channel != NULL)
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
    цяло		i;

    dict_add_string(dict, "status", (char_u *)job_status(job));

    item = dictitem_alloc((char_u *)"channel");
    ако (item == NULL)
	върни;
    item->di_tv.v_type = VAR_CHANNEL;
    item->di_tv.vval.v_channel = job->jv_channel;
    ако (job->jv_channel != NULL)
	++job->jv_channel->ch_refcount;
    ако (dict_add(dict, item) == FAIL)
	dictitem_free(item);

#ifdef UNIX
    nr = job->jv_pid;
#иначе
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
    ако (l == NULL)
	върни;

    ако (dict_add_list(dict, "cmd", l) == FAIL)
    {
	list_unref(l);
	върни;
    }
    ако (job->jv_argv != NULL)
	за (i = 0; job->jv_argv[i] != NULL; i++)
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

	ако (list_append_tv(l, &tv) != OK)
	    върни;
    }
}

/*
 * "job_info()" function
 */
    void
f_job_info(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script() && check_for_opt_job_arg(argvars, 0) == FAIL)
	върни;

    ако (argvars[0].v_type != VAR_UNKNOWN)
    {
	job_T	*job;

	job = get_job_arg(&argvars[0]);
	ако (job != NULL && rettv_dict_alloc(rettv) == OK)
	    job_info(job, rettv->vval.v_dict);
    }
    иначе ако (rettv_list_alloc(rettv) == OK)
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

    ако (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	върни;

    job = get_job_arg(&argvars[0]);
    ако (job == NULL)
	върни;
    clear_job_options(&opt);
    ако (get_job_options(&argvars[1], &opt, JO_STOPONEXIT + JO_EXIT_CB, 0) == OK)
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
    ако (check_restricted() || check_secure())
	върни;

    ако (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	върни;

    rettv->vval.v_job = job_start(argvars, NULL, NULL, NULL);
}

/*
 * "job_status()" function
 */
    void
f_job_status(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script() && check_for_job_arg(argvars, 0) == FAIL)
	върни;

    ако (argvars[0].v_type == VAR_JOB && argvars[0].vval.v_job == NULL)
    {
	// A job that never started returns "fail".
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave((char_u *)"fail");
    }
    иначе
    {
	job_T	*job = get_job_arg(&argvars[0]);

	ако (job != NULL)
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

    ако (in_vim9script()
	    && (check_for_job_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	върни;

    job = get_job_arg(&argvars[0]);
    ако (job != NULL)
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
    char  *status;

    ако (job == NULL)
    {
	vim_snprintf((char *)buf, NUMBUFLEN, "no process");
	върни buf;
    }
    status = job->jv_status == JOB_FAILED ? "fail"
		    : job->jv_status >= JOB_ENDED ? "dead"
		    : "run";
#ifdef UNIX
    vim_snprintf((char *)buf, NUMBUFLEN,
		"process %ld %s", (long)job->jv_pid, status);
#elif defined(MSWIN)
    vim_snprintf((char *)buf, NUMBUFLEN,
		"process %ld %s",
		(long)job->jv_proc_info.dwProcessId,
		status);
#иначе
    // fall-back
    vim_snprintf((char *)buf, NUMBUFLEN, "process ? %s", status);
#endif
    върни buf;
}

#endif // FEAT_JOB_CHANNEL
