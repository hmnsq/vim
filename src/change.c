/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * change.c: functions related to changing text
 */

#अन्तर्भाव "vim.h"

/*
 * If the file is readonly, give a warning message with the first change.
 * Don't do this for autocommands.
 * Doesn't use emsg(), because it flushes the macro buffer.
 * If we have undone all changes b_changed will be FALSE, but "b_did_warn"
 * will be TRUE.
 * "col" is the column for the message; non-zero when in insert mode and
 * 'showmode' is on.
 * Careful: may trigger autocommands that reload the buffer.
 */
    शून्य
change_warning(पूर्णाङ्क col)
{
    स्थिर वर्ण *w_readonly = N_("W10: Warning: Changing a readonly file");

    जइ (curbuf->b_did_warn
	    || curbufIsChanged()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	प्रत्यागम;

    ++curbuf_lock;
    apply_autocmds(EVENT_FILECHANGEDRO, रिक्त, रिक्त, FALSE, curbuf);
    --curbuf_lock;
    जइ (!curbuf->b_p_ro)
	प्रत्यागम;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    सन्देशः_start();
    जइ (सन्देशः_row == Rows - 1)
	सन्देशः_col = col;
    सन्देशः_source(HL_ATTR(HLF_W));
    सन्देशः_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | सन्देशः_HIST);
#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#endif
    सन्देशः_clr_eos();
    (शून्य)सन्देशः_end();
    जइ (सन्देशः_silent == 0 && !silent_mode
#ifdef FEAT_EVAL
	    && time_for_testing != 1
#endif
       )
    {
	out_flush();
	ui_delay(1002L, TRUE); // give the user time to think about it
    }
    curbuf->b_did_warn = true;
    redraw_cmdline = FALSE;	// don't redraw and erase the message
    जइ (सन्देशः_row < Rows - 1)
	showmode();
}

/*
 * Call this function when something in the current buffer is changed.
 *
 * Most often called through changed_bytes() and changed_lines(), which also
 * mark the area of the display to be redrawn.
 *
 * Careful: may trigger autocommands that reload the buffer.
 */
    शून्य
changed(शून्य)
{
#जइ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    जइ (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	जइ (im_is_preediting() && !xim_changed_while_preediting)
	    प्रत्यागम;
	xim_changed_while_preediting = FALSE;
    }
#endif

    जइ (!curbuf->b_changed)
    {
	पूर्णाङ्क	save_सन्देशः_scroll = सन्देशः_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	जइ (curbuf->b_may_swap && !bt_dontwrite(curbuf))
	{
	    पूर्णाङ्क save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    जइ (need_wait_return && emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		सन्देशः_scroll = save_सन्देशः_scroll;
	    }
	    अण्णहा
		need_wait_return = save_need_wait_return;
	}
	changed_internal();
    }
    ++CHANGEDTICK(curbuf);

#ifdef FEAT_अन्वेषणम्_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_match = FALSE;
#endif
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    शून्य
changed_internal(शून्य)
{
    curbuf->b_changed = TRUE;
    ml_setflags(curbuf);
    check_status(curbuf);
    redraw_tabline = TRUE;
#जइ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

#ifdef FEAT_EVAL
// Set when listener callbacks are being invoked.
स्थिर पूर्णाङ्क recursive = FALSE;

स्थिर long next_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
स्थिर bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    स्थिर शून्य
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    जइ (prev != रिक्त)
	prev->lr_next = lnr->lr_next;
    अण्णहा
	*list = lnr->lr_next;
    free_callback(&lnr->lr_callback);
    vim_free(lnr);
}

/*
 * Clean up a buffer change listener list.
 *
 * If "all" is TRUE then all entries are removed. Otherwise only those with an ID
 * of zero are removed. If "buf" is non-NULL then the buffer's recorded changes
 * will be discarded in the event that all listeners were removed.
 *
 */
    स्थिर शून्य
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*next;

    prev = रिक्त;
    प्रति (lnr = *list; lnr != रिक्त; lnr = next)
    {
	next = lnr->lr_next;
	जइ (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	अण्णहा
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    जइ (buf != रिक्त)
    {
	जइ (*list == रिक्त && buf->b_recorded_changes != रिक्त)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = रिक्त;
	    buf->b_recorded_text_size = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    स्थिर शून्य
perform_listener_housekeeping(शून्य)
{
    buf_T	*buf;

    जइ (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(रिक्त, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    स्थिर शून्य
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	long		xtra)
{
    perform_listener_housekeeping();
    जइ (buf->b_recorded_changes == रिक्त || xtra == 0)
	प्रत्यागम;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_वस्तूनि(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_सङ्ख्या(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_सङ्ख्या(
		li->li_tv.vval.v_dict, "end");
	जइ (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    भङ्ग;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_TEXT_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    स्थिर bool
listeners_want_text(listener_T *list)
{
    listener_T	*lnr;

    प्रति (lnr = list; lnr != रिक्त; lnr = lnr->lr_next)
	जइ (lnr->lr_text)
	    प्रत्यागम true;
    प्रत्यागम false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    स्थिर size_t
add_change_text(
    buf_T	*buf,
    dict_T	*dict,
    linenr_T	lnum,
    linenr_T	lnume,
    long	xtra)
{
    list_T	*l = list_alloc();
    linenr_T	below = lnume + xtra;	// line below the changed region
    size_t	size = 0;

    जइ (l == रिक्त)
	प्रत्यागम 0;

    जइ (below > buf->b_ml.ml_पङ्क्तिः_गणनम् + 1)
	below = buf->b_ml.ml_पङ्क्तिः_गणनम् + 1;
    प्रति (linenr_T lp = lnum < 1 ? 1 : lnum; lp < below; ++lp)
    {
	char_u	*पङ्क्तिः = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	जइ (list_append_string(l, पङ्क्तिः, len) == FAIL)
	{
	    list_free(l);
	    प्रत्यागम 0;
	}
	size += (size_t)len + 1;
    }

    जइ (dict_add_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	प्रत्यागम 0;
    }
    प्रत्यागम size;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    स्थिर शून्य
may_record_change(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    जइ (curbuf->b_listener == रिक्त)
	प्रत्यागम;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    जइ (curbuf->b_recorded_changes == रिक्त)
    {
	curbuf->b_recorded_changes = list_alloc();
	जइ (curbuf->b_recorded_changes == रिक्त)  // out of memory
	    प्रत्यागम;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    जइ (dict == रिक्त)
	प्रत्यागम;
    dict_add_सङ्ख्या(dict, "lnum", (varnumber_T)lnum);
    dict_add_सङ्ख्या(dict, "end", (varnumber_T)lnume);
    dict_add_सङ्ख्या(dict, "added", (varnumber_T)xtra);
    dict_add_सङ्ख्या(dict, "col", (varnumber_T)col + 1);
    जइ (listeners_want_text(curbuf->b_listener))
	curbuf->b_recorded_text_size +=
			  add_change_text(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    जइ (!recursive && curbuf->b_recorded_text_size > LISTENER_TEXT_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    शून्य
f_listener_add(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_text = false;

    जइ (check_secure())
	प्रत्यागम;

    जइ (recursive)
    {
	emsg(_(e_cannot_add_listener_in_listener_callback));
	प्रत्यागम;
    }

    जइ (in_vim9script() && (
	    check_for_opt_buffer_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    callback = get_callback(&argvars[0]);
    जइ (callback.cb_name == रिक्त)
	प्रत्यागम;

    जइ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	जइ (buf == रिक्त)
	{
	    free_callback(&callback);
	    प्रत्यागम;
	}
	जइ (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_text = dict_get_bool(d, "text", false);
	}
	अण्णहा जइ (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    जइ (lnr == रिक्त)
    {
	free_callback(&callback);
	प्रत्यागम;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    जइ (unbuffered)
    {
	lnr->lr_next = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    अण्णहा
    {
	lnr->lr_next = buf->b_listener;
	buf->b_listener = lnr;
    }

    set_callback(&lnr->lr_callback, &callback);

    lnr->lr_text = want_text;
    lnr->lr_id = ++next_listener_id;
    rettv->vval.v_सङ्ख्या = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    शून्य
f_listener_flush(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf = curbuf;

    जइ (check_secure())
	प्रत्यागम;

    जइ (recursive)
	प्रत्यागम;

    जइ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	जइ (buf == रिक्त)
	    प्रत्यागम;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    स्थिर listener_T *
find_listener(
    पूर्णाङ्क        id,
    listener_T *list_start,
    listener_T **prev)
{
    listener_T *next;
    listener_T *lnr;

    *prev = रिक्त;
    प्रति (lnr = list_start; lnr != रिक्त; lnr = next)
    {
	next = lnr->lr_next;
	जइ (lnr->lr_id == id)
	    प्रत्यागम lnr;
	*prev = lnr;
    }
    प्रत्यागम रिक्त;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    शून्य
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    पूर्णाङ्क		id;
    buf_T	*buf;

    जइ (check_secure())
	प्रत्यागम;

    जइ (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = tv_get_सङ्ख्या(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = find_listener(id, buf->b_listener, &prev);
	जइ (lnr == रिक्त)
	    lnr = find_listener(id, buf->b_sync_listener, &prev);
	जइ (lnr != रिक्त)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_सङ्ख्या = 1;
	    प्रत्यागम;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    शून्य
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, पूर्णाङ्क added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    स्थिर शून्य
invoke_listener_set(
    buf_T       *buf,
    linenr_T	start,
    linenr_T	end,
    long	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    पूर्णाङ्क		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_सङ्ख्या;
    argv[0].vval.v_सङ्ख्या = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_सङ्ख्या;
    argv[1].vval.v_सङ्ख्या = start;
    argv[2].v_type = VAR_सङ्ख्या;
    argv[2].vval.v_सङ्ख्या = end;
    argv[3].v_type = VAR_सङ्ख्या;
    argv[3].vval.v_सङ्ख्या = added;
    argv[4].v_type = VAR_LIST;
    argv[4].vval.v_list = recorded_changes;

    // Ensure the list of changes is locked to prevent any modifications by
    // callback code..
    val.v_type = VAR_LIST;
    val.v_lock = 0;
    val.vval.v_list = recorded_changes;
    item_lock(&val, -1, TRUE, FALSE);

    // Protect against recursive callbacks, lock the buffer against changes and
    // set the updating_screen flag to prevent channel input processing, which
    // might also try to update the buffer.
    recursive = TRUE;
    ++textlock;
    updating_screen = TRUE;

    प्रति (lnr = listeners; lnr != रिक्त; lnr = lnr->lr_next)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    जइ (save_updating_screen)
	updating_screen = TRUE;
    अण्णहा
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    स्थिर शून्य
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	start,
    colnr_T	col,
    linenr_T	end,
    long	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    जइ (recursive || curbuf->b_sync_listener == रिक्त)
	प्रत्यागम;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    जइ (recorded_changes == रिक्त)  // out of memory
	प्रत्यागम;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    जइ (dict == रिक्त)
    {
	list_unref(recorded_changes);
	प्रत्यागम;
    }

    dict_add_सङ्ख्या(dict, "lnum", (varnumber_T)start);
    dict_add_सङ्ख्या(dict, "end", (varnumber_T)end);
    dict_add_सङ्ख्या(dict, "added", (varnumber_T)added);
    dict_add_सङ्ख्या(dict, "col", (varnumber_T)col + 1);
    जइ (listeners_want_text(buf->b_sync_listener))
	(शून्य)add_change_text(buf, dict, start, end, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_set(
	buf, start, end, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    शून्य
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	start = MAXLNUM;
    linenr_T	end = 0;
    linenr_T	added = 0;

    जइ (buf->b_recorded_changes == रिक्त  // nothing changed
	    || buf->b_listener == रिक्त	 // no listeners
	    || recursive)		 // already busy
	प्रत्यागम;

    FOR_ALL_LIST_वस्तूनि(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_सङ्ख्या(li->li_tv.vval.v_dict, "lnum");
	जइ (start > lnum)
	    start = lnum;
	lnum = dict_get_सङ्ख्या(li->li_tv.vval.v_dict, "end");
	जइ (end < lnum)
	    end = lnum;
	added += dict_get_सङ्ख्या(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_set(
	buf, start, end, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = रिक्त;
    buf->b_recorded_text_size = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    शून्य
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(रिक्त, &buf->b_sync_listener, TRUE);
}

#endif

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    स्थिर शून्य
changed_common(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		cols;
    pos_T	*p;
    पूर्णाङ्क		add;

    // mark the buffer as modified
    changed();

#ifdef FEAT_EVAL
    // Immediately send this change to any listeners that require changes not
    // to be buffered.
    invoke_sync_listeners(curbuf, lnum, col, lnume, xtra);

    // If there are any listeners accepting buffered changes then add changes
    // to the current buffer's list, flushing previous changes first if necessary.
    may_record_change(lnum, col, lnume, xtra);
#endif
#ifdef FEAT_DIFF
    जइ (curwin->w_p_diff && diff_internal())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_पङ्क्तिः(lnum);
    }
#endif

    // set the '. mark
    जइ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_last_change.lnum = lnum;
	curbuf->b_last_change.col = col;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	जइ (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    जइ (curbuf->b_changelistlen == 0)
		add = TRUE;
	    अण्णहा
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		जइ (p->lnum != lnum)
		    add = TRUE;
		अण्णहा
		{
		    cols = comp_textwidth(FALSE);
		    जइ (cols == 0)
			cols = 79;
		    add = (p->col + cols < col || col + cols < p->col);
		}
	    }
	    जइ (add)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		जइ (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  परिमाण(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			जइ (wp->w_buffer == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    जइ (wp->w_buffer == curbuf
			    && wp->w_changelistidx == curbuf->b_changelistlen)
			++wp->w_changelistidx;
		}
		++curbuf->b_changelistlen;
	    }
	}
	curbuf->b_changelist[curbuf->b_changelistlen - 1] =
							curbuf->b_last_change;
	// The current window is always after the last change, so that "g,"
	// takes you back to it.
	curwin->w_changelistidx = curbuf->b_changelistlen;
    }

    जइ (VIsual_active)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जइ (wp->w_buffer == curbuf)
	{
	    linenr_T last = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    जइ (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    जइ (xtra != 0 && wp->w_redraw_शिखरम् != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    जइ (wp->w_skipcol > 0
		    && (last < wp->w_topline
			|| (wp->w_topline >= lnum
			    && wp->w_topline < lnume
			    && linetabsize_eol(wp, wp->w_topline)
				    <= wp->w_skipcol + sms_marker_overlap(wp, -1))))
		wp->w_skipcol = 0;

	    // Check if a change in the buffer has invalidated the cached
	    // values for the cursor.
#ifdef FEAT_FOLDING
	    // Update the folds for this window.  Can't postpone this, because
	    // a following operator might work on the whole fold: ">>dd".
	    foldUpdate(wp, lnum, last);

	    // The change may cause lines above or below the change to become
	    // included in a fold.  Set lnum/lnume to the first/last line that
	    // might be displayed differently.
	    // Set w_cline_folded here as an efficient way to update it when
	    // inserting lines just above a closed fold.
	    i = hasFoldingWin(wp, lnum, &lnum, रिक्त, FALSE, रिक्त);
	    जइ (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, last, रिक्त, &last, FALSE, रिक्त);
	    जइ (wp->w_cursor.lnum == last)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    जइ (wp->w_cursor.lnum <= lnum)
	    {
		i = find_wl_entry(wp, lnum);
		जइ (i >= 0 && wp->w_cursor.lnum > wp->w_पङ्क्तयः[i].wl_lnum)
		    changed_पङ्क्तिः_abv_curs_win(wp);
	    }
#endif
	    जइ (wp->w_cursor.lnum > lnum)
		changed_पङ्क्तिः_abv_curs_win(wp);
	    अण्णहा जइ (wp->w_cursor.lnum == lnum && wp->w_cursor.col >= col)
		changed_cline_bef_curs_win(wp);
	    जइ (wp->w_botline >= lnum)
	    {
		जइ (xtra < 0)
		    invalidate_botline_win(wp);
		अण्णहा
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    प्रति (i = 0; i < wp->w_पङ्क्तयः_valid; ++i)
		जइ (wp->w_पङ्क्तयः[i].wl_valid)
		{
		    जइ (wp->w_पङ्क्तयः[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			जइ (wp->w_पङ्क्तयः[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_पङ्क्तयः[i].wl_valid = FALSE;
			}
			अण्णहा जइ (xtra != 0)
			{
			    // line below change
			    wp->w_पङ्क्तयः[i].wl_lnum += xtra;
#ifdef FEAT_FOLDING
			    wp->w_पङ्क्तयः[i].wl_lastlnum += xtra;
#endif
			}
		    }
#ifdef FEAT_FOLDING
		    अण्णहा जइ (wp->w_पङ्क्तयः[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_पङ्क्तयः[i].wl_valid = FALSE;
		    }
#endif
		}

#ifdef FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    जइ (hasAnyFolding(wp))
		set_topline(wp, wp->w_topline);
#endif
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    जइ (wp->w_p_rnu && xtra != 0)
		wp->w_last_cursor_lnum_rnu = 0;

#ifdef FEAT_SYN_HL
	    जइ (wp->w_p_cul && wp->w_last_cursorline >= lnum)
	    {
		जइ (wp->w_last_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_last_cursorline = 0;
		अण्णहा
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_last_cursorline += xtra;
	    }
#endif
	}
#ifdef FEAT_अन्वेषणम्_EXTRA
	जइ (wp == curwin && xtra != 0 && अन्वेषणम्_hl_has_cursor_lnum >= lnum)
	    अन्वेषणम्_hl_has_cursor_lnum += xtra;
#endif
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    set_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    जइ (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	last_cursormoved.lnum = 0;
}

    स्थिर शून्य
changedOneline(buf_T *buf, linenr_T lnum)
{
    जइ (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	जइ (lnum < buf->b_mod_शिखरम्)
	    buf->b_mod_शिखरम् = lnum;
	अण्णहा जइ (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    अण्णहा
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_set = true;
	buf->b_mod_शिखरम् = lnum;
	buf->b_mod_bot = lnum + 1;
	buf->b_mod_xlines = 0;
    }
}

/*
 * Changed bytes within a single line for the current buffer.
 * - marks the windows on this buffer to be redisplayed
 * - marks the buffer changed by calling changed()
 * - invalidates cached values
 * Careful: may trigger autocommands that reload the buffer.
 */
    शून्य
changed_bytes(linenr_T lnum, colnr_T col)
{
    changedOneline(curbuf, lnum);
    changed_common(lnum, col, lnum + 1, 0L);

#ifdef FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    जइ (spell_check_window(curwin)
	    && lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्
	    && vim_strchr(p_cpo, CPO_DOLLAR) == रिक्त)
	redrawWinline(curwin, lnum + 1);
#endif
#ifdef FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    जइ (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    जइ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		जइ (wlnum > 0)
		    changedOneline(wp->w_buffer, wlnum);
	    }
    }
#endif
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    शून्य
inserted_bytes(linenr_T lnum, colnr_T col, पूर्णाङ्क added UNUSED)
{
#ifdef FEAT_PROP_POPUP
    जइ (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, col, added, 0);
#endif

    changed_bytes(lnum, col);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    शून्य
appended_पङ्क्तयः(linenr_T lnum, long गणनम्)
{
    changed_पङ्क्तयः(lnum + 1, 0, lnum + 1, गणनम्);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    शून्य
appended_पङ्क्तयः_mark(linenr_T lnum, long गणनम्)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, गणनम्, 0L);
    changed_पङ्क्तयः(lnum + 1, 0, lnum + 1, गणनम्);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    शून्य
deleted_पङ्क्तयः(linenr_T lnum, long गणनम्)
{
    changed_पङ्क्तयः(lnum, 0, lnum + गणनम्, -गणनम्);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    शून्य
deleted_पङ्क्तयः_mark(linenr_T lnum, long गणनम्)
{
    mark_adjust(lnum, (linenr_T)(lnum + गणनम् - 1), (long)MAXLNUM, -गणनम्);
    changed_पङ्क्तयः(lnum, 0, lnum + गणनम्, -गणनम्);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    शून्य
changed_पङ्क्तयः_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    जइ (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	जइ (lnum < buf->b_mod_शिखरम्)
	    buf->b_mod_शिखरम् = lnum;
	जइ (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    जइ (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	जइ (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    अण्णहा
    {
	// set the area that must be redisplayed
	buf->b_mod_set = true;
	buf->b_mod_शिखरम् = lnum;
	buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines = xtra;
    }
}

/*
 * Changed lines for the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * - mark the buffer changed by calling changed()
 * - mark the windows on this buffer to be redisplayed
 * - invalidate cached values
 * "lnum" is the first line that needs displaying, "lnume" the first line
 * below the changed lines (BEFORE the change).
 * When only inserting lines, "lnum" and "lnume" are equal.
 * Takes care of calling changed() and updating b_mod_*.
 * Careful: may trigger autocommands that reload the buffer.
 */
    शून्य
changed_पङ्क्तयः(
    linenr_T	lnum,	    // first line with change
    colnr_T	col,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    changed_पङ्क्तयः_buf(curbuf, lnum, lnume, xtra);

#ifdef FEAT_DIFF
    जइ (xtra == 0 && curwin->w_p_diff && !diff_internal())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    जइ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		जइ (wlnum > 0)
		    changed_पङ्क्तयः_buf(wp->w_buffer, wlnum,
						    lnume - lnum + wlnum, 0L);
	    }
    }
#endif

    changed_common(lnum, col, lnume, xtra);
}

/*
 * Called when the changed flag must be reset for buffer "buf".
 * When "ff" is TRUE also reset 'fileformat'.
 * When "always_inc_changedtick" is TRUE b:changedtick is incremented also when
 * the changed flag was off.
 */
    शून्य
unchanged(buf_T *buf, पूर्णाङ्क ff, पूर्णाङ्क always_inc_changedtick)
{
    जइ (buf->b_changed || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_changed = 0;
	ml_setflags(buf);
	जइ (ff)
	    save_file_ff(buf);
	check_status(buf);
	redraw_tabline = TRUE;
#जइ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    अण्णहा जइ (always_inc_changedtick)
	++CHANGEDTICK(buf);
#ifdef FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#endif
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    शून्य
save_file_ff(buf_T *buf)
{
    buf->b_start_ffc = *buf->b_p_ff;
    buf->b_start_eof = buf->b_p_eof;
    buf->b_start_eol = buf->b_p_eol;
    buf->b_start_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    जइ (buf->b_start_fenc == रिक्त
			     || STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_start_fenc);
	buf->b_start_fenc = vim_strsave(buf->b_p_fenc);
    }
}

/*
 * Return TRUE if 'fileformat' and/or 'fileencoding' has a different value
 * from when editing started (save_file_ff() called).
 * Also when 'endofline' was changed and 'binary' is set, or when 'bomb' was
 * changed and 'binary' is not set.
 * Also when 'endofline' was changed and 'fixeol' is not set.
 * When "ignore_empty" is true don't consider a new, empty buffer to be
 * changed.
 */
    पूर्णाङ्क
file_ff_differs(buf_T *buf, पूर्णाङ्क ignore_empty)
{
    // In a buffer that was never loaded the options are not valid.
    जइ (buf->b_flags & BF_NEVERLOADED)
	प्रत्यागम FALSE;
    जइ (ignore_empty
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_पङ्क्तिः_गणनम् == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	प्रत्यागम FALSE;
    जइ (buf->b_start_ffc != *buf->b_p_ff)
	प्रत्यागम TRUE;
    जइ ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_start_eof != buf->b_p_eof
		|| buf->b_start_eol != buf->b_p_eol))
	प्रत्यागम TRUE;
    जइ (!buf->b_p_bin && buf->b_start_bomb != buf->b_p_bomb)
	प्रत्यागम TRUE;
    जइ (buf->b_start_fenc == रिक्त)
	प्रत्यागम (*buf->b_p_fenc != NUL);
    प्रत्यागम (STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    शून्य
ins_bytes(char_u *p)
{
    ins_bytes_len(p, (पूर्णाङ्क)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    शून्य
ins_bytes_len(char_u *p, पूर्णाङ्क len)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;

    जइ (has_mbyte)
	प्रति (i = 0; i < len; i += n)
	{
	    जइ (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    अण्णहा
		n = (*mb_ptr2len)(p + i);
	    ins_char_bytes(p + i, n);
	}
    अण्णहा
	प्रति (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    शून्य
ins_char(पूर्णाङ्क c)
{
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णाङ्क		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    जइ (buf[0] == 0)
	buf[0] = '\n';

    ins_char_bytes(buf, n);
}

    शून्य
ins_char_bytes(char_u *buf, पूर्णाङ्क charlen)
{
    पूर्णाङ्क		c = buf[0];
    पूर्णाङ्क		newlen;		// nr of bytes inserted
    पूर्णाङ्क		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    पूर्णाङ्क		linelen;	// length of old line including NUL
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;
    पूर्णाङ्क		i;

    // Break tabs if needed.
    जइ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    linelen = (पूर्णाङ्क)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    जइ (State & REPLACE_FLAG)
    {
	जइ (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    पूर्णाङ्क		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    जइ (old_list && vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, रिक्त, &vcol, रिक्त, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    जाव (oldp[col + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + col + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		जइ (vcol > new_vcol && oldp[col + oldlen] == TAB)
		    भङ्ग;
		oldlen += (*mb_ptr2len)(oldp + col + oldlen);
		// Deleted a bit too much, insert spaces.
		जइ (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	अण्णहा जइ (oldp[col] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + col);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_प्रक्षिप(NUL);
	प्रति (i = 0; i < oldlen; ++i)
	{
	    जइ (has_mbyte)
		i += replace_प्रक्षिप_mb(oldp + col + i) - 1;
	    अण्णहा
		replace_प्रक्षिप(oldp[col + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    जइ (newp == रिक्त)
	प्रत्यागम;

    // Copy bytes before the cursor.
    जइ (col > 0)
	mch_memmove(newp, oldp, (size_t)col);

    // Copy bytes after the changed character(s).
    p = newp + col;
    जइ (linelen > col + oldlen)
	mch_memmove(p + newlen, oldp + col + oldlen,
					    (size_t)(linelen - col - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    जाव (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    changed_bytes(lnum, col);
#ifdef FEAT_PROP_POPUP
    जइ (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, col, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#endif

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    जइ (p_sm && (State & MODE_निवेशय)
	    && सन्देशः_silent == 0
	    && !ins_compl_active())
    {
	जइ (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	अण्णहा
	    showmatch(c);
    }

#ifdef FEAT_RIGHTLEFT
    जइ (!p_ri || (State & REPLACE_FLAG))
#endif
    {
	// Normal insert: move cursor right
	curwin->w_cursor.col += charlen;
    }

    // TODO: should try to update w_row here, to avoid recomputing it later.
}

/*
 * Insert a string at the cursor position.
 * Note: Does NOT handle Replace mode.
 * Caller must have prepared for undo.
 */
    शून्य
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    पूर्णाङ्क		oldlen;
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;

    जइ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    oldlen = (पूर्णाङ्क)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    जइ (newp == रिक्त)
	प्रत्यागम;
    जइ (col > 0)
	mch_memmove(newp, oldp, (size_t)col);
    mch_memmove(newp + col, s, slen);
    mch_memmove(newp + col + slen, oldp + col, (size_t)(oldlen - col + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_bytes(lnum, col, (पूर्णाङ्क)slen);
    curwin->w_cursor.col += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
del_char(पूर्णाङ्क fixpos)
{
    जइ (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	जइ (*ml_get_cursor() == NUL)
	    प्रत्यागम FAIL;
	प्रत्यागम del_chars(1L, fixpos);
    }
    प्रत्यागम del_bytes(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    पूर्णाङ्क
del_chars(long गणनम्, पूर्णाङ्क fixpos)
{
    long	bytes = 0;
    long	i;
    char_u	*p;
    पूर्णाङ्क		l;

    p = ml_get_cursor();
    प्रति (i = 0; i < गणनम् && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	bytes += l;
	p += l;
    }
    प्रत्यागम del_bytes(bytes, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
del_bytes(
    long	गणनम्,
    पूर्णाङ्क		fixpos_arg,
    पूर्णाङ्क		use_delcombine UNUSED)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    पूर्णाङ्क		alloc_newp;
    long	movelen;
    पूर्णाङ्क		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (पूर्णाङ्क)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    जइ (col >= oldlen)
	प्रत्यागम FAIL;

    // If "count" is zero there is nothing to do.
    जइ (गणनम् == 0)
	प्रत्यागम OK;

    // If "count" is negative the caller must be doing something wrong.
    जइ (गणनम् < 1)
    {
	siemsg(e_invalid_गणनम्_for_del_bytes_nr, गणनम्);
	प्रत्यागम FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    जइ (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + col) >= गणनम्)
    {
	पूर्णाङ्क	cc[MAX_MCO];
	पूर्णाङ्क	n;

	(शून्य)utfc_ptr2char(oldp + col, cc);
	जइ (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = col;
	    करेहि
	    {
		col = n;
		गणनम् = utf_ptr2len(oldp + n);
		n += गणनम्;
	    } जाव (UTF_COMPOSINGLIKE(oldp + col, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (long)oldlen - (long)col - गणनम् + 1; // includes trailing NUL
    जइ (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	जइ (col > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.coladd = 0;
	    जइ (has_mbyte)
		curwin->w_cursor.col -=
			    (*mb_head_off)(oldp, oldp + curwin->w_cursor.col);
	}
	गणनम् = oldlen - col;
	movelen = 1;
    }
    newlen = oldlen - गणनम्;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#ifdef FEAT_NETBEANS_INTG
    जइ (netbeans_active())
	alloc_newp = TRUE;
    अण्णहा
#endif
	alloc_newp = !ml_पङ्क्तिः_alloced();    // check if oldp was allocated
    जइ (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    अण्णहा
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	जइ (newp == रिक्त)
	    प्रत्यागम FAIL;
	mch_memmove(newp, oldp, (size_t)col);
    }
    mch_memmove(newp + col, oldp + col + गणनम्, (size_t)movelen);
    जइ (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    अण्णहा
    {
#ifdef FEAT_PROP_POPUP
	// Also move any following text properties.
	जइ (oldlen + 1 < curbuf->b_ml.ml_पङ्क्तिः_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_पङ्क्तिः_len - oldlen - 1);
#endif
	curbuf->b_ml.ml_पङ्क्तिः_len -= गणनम्;
	curbuf->b_ml.ml_पङ्क्तिः_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, col, -गणनम्);

    प्रत्यागम OK;
}

/*
 * open_line: Add a new line below or above the current line.
 *
 * For MODE_VREPLACE state, we only add a new line when we get to the end of
 * the file, otherwise we just start replacing the next line.
 *
 * Caller must take care of undo.  Since MODE_VREPLACE may affect any number of
 * lines however, it may call u_save_cursor() again when starting to change a
 * new line.
 * "flags": OPENLINE_DELSPACES	delete spaces after cursor
 *	    OPENLINE_DO_COM	format comments
 *	    OPENLINE_KEEPTRAIL	keep trailing spaces
 *	    OPENLINE_MARKFIX	adjust mark positions after the line break
 *	    OPENLINE_COM_LIST	format comments with list or 2nd line indent
 *	    OPENLINE_FORCE_INDENT  set indent from second_line_indent, ignore
 *				   'autoindent'
 *
 * "second_line_indent": indent for after ^^D in Insert mode or if flag
 *			  OPENLINE_COM_LIST
 * "did_do_comment" is set to TRUE when intentionally putting the comment
 * leader in front of the new line.
 *
 * Return OK for success, FAIL for failure
 */
    पूर्णाङ्क
open_पङ्क्तिः(
    पूर्णाङ्क		dir,		// FORWARD or BACKWARD
    पूर्णाङ्क		flags,
    पूर्णाङ्क		second_पङ्क्तिः_indent,
    पूर्णाङ्क		*did_do_comment UNUSED)
{
    char_u	*saved_पङ्क्तिः;		// copy of the original line
    char_u	*next_पङ्क्तिः = रिक्त;	// copy of the next line
    char_u	*p_extra = रिक्त;	// what goes to next line
    पूर्णाङ्क		less_cols = 0;		// less columns for mark in new line
    पूर्णाङ्क		less_cols_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    पूर्णाङ्क		newcol = 0;		// new cursor column
    पूर्णाङ्क		newindent = 0;		// auto-indent of the new line
    पूर्णाङ्क		n;
    पूर्णाङ्क		trunc_पङ्क्तिः = FALSE;	// truncate current line afterwards
    पूर्णाङ्क		retval = FAIL;		// return value
    पूर्णाङ्क		extra_len = 0;		// length of p_extra string
    पूर्णाङ्क		lead_len;		// length of comment leader
    पूर्णाङ्क		comment_start = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = रिक्त;		// copy of comment leader
    char_u	*allocated = रिक्त;	// allocated memory
    char_u	*p;
    पूर्णाङ्क		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    पूर्णाङ्क		do_cindent;
    पूर्णाङ्क		do_si = may_do_si();
    पूर्णाङ्क		no_si = FALSE;		// reset did_si afterwards
    पूर्णाङ्क		first_char = NUL;	// init for GCC
    पूर्णाङ्क		vreplace_mode;
    पूर्णाङ्क		did_append;		// appended a new line
    पूर्णाङ्क		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#ifdef FEAT_PROP_POPUP
    पूर्णाङ्क		at_eol;			// cursor after last character
#endif

    // make a copy of the current line so we can mess with it
    saved_पङ्क्तिः = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    जइ (saved_पङ्क्तिः == रिक्त)	    // out of memory!
	प्रत्यागम FALSE;

#ifdef FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.col >= (पूर्णाङ्क)ml_get_curline_len();
#endif

    जइ (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	जइ (curwin->w_cursor.lnum < orig_पङ्क्तिः_गणनम्)
	    next_पङ्क्तिः = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	अण्णहा
	    next_पङ्क्तिः = vim_strsave((char_u *)"");
	जइ (next_पङ्क्तिः == रिक्त)	    // out of memory!
	    गच्छ theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_प्रक्षिप(NUL);  // Call twice because BS over NL expects it
	replace_प्रक्षिप(NUL);
	p = saved_पङ्क्तिः + curwin->w_cursor.col;
	जाव (*p != NUL)
	{
	    जइ (has_mbyte)
		p += replace_प्रक्षिप_mb(p);
	    अण्णहा
		replace_प्रक्षिप(*p++);
	}
	saved_पङ्क्तिः[curwin->w_cursor.col] = NUL;
    }

    जइ ((State & MODE_निवेशय) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_पङ्क्तिः + curwin->w_cursor.col;
	जइ (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    first_char = *p;
	}
	extra_len = (पूर्णाङ्क)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_col = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    जइ (dir == FORWARD && did_ai)
	trunc_पङ्क्तिः = TRUE;

    जइ ((flags & OPENLINE_FORCE_INDENT) && second_पङ्क्तिः_indent >= 0)
	newindent = second_पङ्क्तिः_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    अण्णहा जइ (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#ifdef FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_पङ्क्तिः, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अण्णहा
	newindent = get_indent_str(saved_पङ्क्तिः, (पूर्णाङ्क)curbuf->b_p_ts, FALSE);
#endif
	जइ (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = second_पङ्क्तिः_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	जइ (!trunc_पङ्क्तिः && do_si && *saved_पङ्क्तिः != NUL
				    && (p_extra == रिक्त || first_char != '{'))
	{
	    char_u  *ptr;
	    char_u  last_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_पङ्क्तिः;
	    जइ (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, रिक्त, FALSE, TRUE);
	    अण्णहा
		lead_len = 0;
	    जइ (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		जइ ( lead_len == 0 && ptr[0] == '#')
		{
		    जाव (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		जइ (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, रिक्त, FALSE, TRUE);
		अण्णहा
		    lead_len = 0;
		जइ (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    जइ (p[0] == '/' && p[1] == '*')
			p++;
		    जइ (p[0] == '*')
		    {
			प्रति (p++; *p; p++)
			{
			    जइ (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.col = (colnr_T)(p - ptr);
				जइ ((pos = findmatch(रिक्त, NUL)) != रिक्त)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    भङ्ग;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.col;
			    }
			}
		    }
		}
		अण्णहा	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    जाव (p > ptr && VIM_ISWHITE(*p))
			--p;
		    last_char = *p;

		    // find the character just before the '{' or ';'
		    जइ (last_char == '{' || last_char == ';')
		    {
			जइ (p > ptr)
			    --p;
			जाव (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    जइ (*p == ')')
		    {
			curwin->w_cursor.col = (colnr_T)(p - ptr);
			जइ ((pos = findmatch(रिक्त, '(')) != रिक्त)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    जइ (last_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    अण्णहा जइ (last_char != ';' && last_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    अण्णहा // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		जइ (lead_len == 0 && ptr[0] == '#')
		{
		    पूर्णाङ्क was_backslashed = FALSE;

		    जाव ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		    {
			जइ (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			अण्णहा
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    जइ (was_backslashed)
			newindent = 0;	    // Got to end of file
		    अण्णहा
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		जइ (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		अण्णहा		    // can delete indent when '{' typed
		    can_si_back = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	जइ (do_si)
	    can_si = TRUE;

	did_ai = TRUE;
    }

    // May do indenting after opening a new line.
    do_cindent = !p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		)
	    && in_cinkeys(dir == FORWARD
		? कुञ्चिका_OPEN_FORW
		: कुञ्चिका_OPEN_BACK, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    end_comment_pending = NUL;
    जइ (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_पङ्क्तिः, &lead_flags,
							dir == BACKWARD, TRUE);
	जइ (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_start = check_linecomment(saved_पङ्क्तिः);
	    जइ (comment_start != MAXCOL)
	    {
		lead_len = get_leader_len(saved_पङ्क्तिः + comment_start,
						     &lead_flags, FALSE, TRUE);
		जइ (lead_len != 0)
		{
		    lead_len += comment_start;
		    जइ (did_do_comment != रिक्त)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    अण्णहा
	lead_len = 0;
    जइ (lead_len > 0)
    {
	char_u	*lead_repl = रिक्त;	    // replaces comment leader
	पूर्णाङ्क	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_मध्यः[COM_MAX_LEN];   // middle-comment string
	पूर्णाङ्क	lead_मध्यः_len;	    // length of the lead_middle
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_end = रिक्त;	    // where lead_end has been found
	पूर्णाङ्क	extra_space = FALSE;	    // append extra space
	पूर्णाङ्क	current_flag;
	पूर्णाङ्क	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	प्रति (p = lead_flags; *p && *p != ':'; ++p)
	{
	    जइ (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		अनुवर्त;
	    }
	    जइ (*p == COM_START || *p == COM_मध्यः)
	    {
		current_flag = *p;
		जइ (*p == COM_START)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    जइ (dir == BACKWARD)
		    {
			lead_len = 0;
			भङ्ग;
		    }

		    // find start of middle part
		    (शून्य)copy_option_part(&p, lead_मध्यः, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		जाव (*p && p[-1] != ':')	// find end of middle flags
		{
		    जइ (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_मध्यः_len = copy_option_part(&p, lead_मध्यः, COM_MAX_LEN, ",");

		जाव (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    जइ (*p == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

		जइ (end_comment_pending == -1)	// we can set it now
		    end_comment_pending = lead_end[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		जइ (dir == FORWARD)
		{
		    प्रति (p = saved_पङ्क्तिः + lead_len; *p; ++p)
			जइ (STRNCMP(p, lead_end, n) == 0)
			{
			    comment_end = p;
			    lead_len = 0;
			    भङ्ग;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		जइ (lead_len > 0)
		{
		    जइ (current_flag == COM_START)
		    {
			lead_repl = lead_मध्यः;
			lead_repl_len = lead_मध्यः_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    जइ (!VIM_ISWHITE(saved_पङ्क्तिः[lead_len - 1])
			    && ((p_extra != रिक्त
				    && (पूर्णाङ्क)curwin->w_cursor.col == lead_len)
				|| (p_extra == रिक्त
				    && saved_पङ्क्तिः[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		भङ्ग;
	    }
	    जइ (*p == COM_END)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		जइ (dir == FORWARD)
		{
		    comment_end = skipwhite(saved_पङ्क्तिः);
		    lead_len = 0;
		    भङ्ग;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		जाव (p > curbuf->b_p_com && *p != ',')
		    --p;
		प्रति (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (पूर्णाङ्क)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		प्रति (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    जइ (*p2 == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		}
		जइ (end_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    जाव (*p2 && *p2 != ',')
			p2++;
		    end_comment_pending = p2[-1];
		}
		भङ्ग;
	    }
	    जइ (*p == COM_FIRST)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		जइ (dir == BACKWARD)
		    lead_len = 0;
		अण्णहा
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		भङ्ग;
	    }
	}
	जइ (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (second_पङ्क्तिः_indent > 0 ? second_पङ्क्तिः_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    जइ (leader == रिक्त)
		lead_len = 0;
	    अण्णहा
	    {
		पूर्णाङ्क li;

		vim_strncpy(leader, saved_पङ्क्तिः, lead_len);

		// TODO: handle multi-byte and double width chars
		प्रति (li = 0; li < comment_start; ++li)
		    जइ (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		जइ (lead_repl != रिक्त)
		{
		    पूर्णाङ्क		c = 0;
		    पूर्णाङ्क		off = 0;

		    प्रति (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			जइ (*p == COM_दक्षिणः || *p == COM_वामः)
			    c = *p++;
			अण्णहा जइ (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			अण्णहा
			    ++p;
		    }
		    जइ (c == COM_दक्षिणः)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			प्रति (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    पूर्णाङ्क	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    पूर्णाङ्क	    old_size = 0;
			    char_u  *endp = p;
			    पूर्णाङ्क	    l;

			    जाव (old_size < repl_size && p > leader)
			    {
				MB_PTR_BACK(leader, p);
				old_size += ptr2cells(p);
			    }
			    l = lead_repl_len - (पूर्णाङ्क)(endp - p);
			    जइ (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			जइ (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			जाव (--p >= leader)
			{
			    पूर्णाङ्क l = mb_head_off(leader, p);

			    जइ (l > 1)
			    {
				p -= l;
				जइ (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    अण्णहा जइ (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    अण्णहा	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    पूर्णाङ्क	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    पूर्णाङ्क	    i;
			    पूर्णाङ्क	    l;

			    प्रति (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				जइ (vim_strnsize(p, i + l) > repl_size)
				    भङ्ग;
			    }
			    जइ (i != lead_repl_len)
			    {
				mch_memmove(p + lead_repl_len, p + i,
				       (size_t)(lead_len - i - (p - leader)));
				lead_len += lead_repl_len - i;
			    }
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);

			// Replace any remaining non-white chars in the old
			// leader by spaces.  Keep Tabs, the indent must
			// remain the same.
			प्रति (p += lead_repl_len; p < leader + lead_len; ++p)
			    जइ (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				जइ (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				अण्णहा
				{
				    पूर्णाङ्क	    l = (*mb_ptr2len)(p);

				    जइ (l > 1)
				    {
					जइ (ptr2cells(p) > 1)
					{
					    // Replace a double-wide char with
					    // two spaces
					    --l;
					    *p++ = ' ';
					}
					mch_memmove(p + 1, p + l,
						     (leader + lead_len) - p);
					lead_len -= l - 1;
				    }
				    *p = ' ';
				}
			    }
			*p = NUL;
		    }

		    // Recompute the indent, it may have changed.
		    जइ (curbuf->b_p_ai || do_si)
#ifdef FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अण्णहा
			newindent = get_indent_str(leader,
						   (पूर्णाङ्क)curbuf->b_p_ts, FALSE);
#endif

		    // Add the indent offset
		    जइ (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    अण्णहा
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    जाव (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			जइ (vim_strchr(skipwhite(leader), '\t') != रिक्त)
			    भङ्ग;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    जइ (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		जइ (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		जइ (newindent || did_si)
		{
		    जाव (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	अण्णहा जइ (comment_end != रिक्त)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    जइ (comment_end[0] == '*' && comment_end[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.col = (colnr_T)(comment_end - saved_पङ्क्तिः);
		जइ ((pos = findmatch(रिक्त, NUL)) != रिक्त)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    जइ (p_extra != रिक्त)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	जइ (REPLACE_NORMAL(State))
	    replace_प्रक्षिप(NUL);	    // end of extra blanks
	जइ (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    जाव ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		जइ (REPLACE_NORMAL(State))
		    replace_प्रक्षिप(*p_extra);
		++p_extra;
		++less_cols_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_cols = (पूर्णाङ्क)(p_extra - saved_पङ्क्तिः);
    }

    जइ (p_extra == रिक्त)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    जइ (lead_len)
    {
	जइ (flags & OPENLINE_COM_LIST && second_पङ्क्तिः_indent > 0)
	{
	    पूर्णाङ्क i;
	    पूर्णाङ्क padding = second_पङ्क्तिः_indent
					  - (newindent + (पूर्णाङ्क)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    प्रति (i = 0; i < padding; i++)
	    {
		STRCAT(leader, " ");
		less_cols--;
		newcol++;
	    }
	}
	STRCAT(leader, p_extra);
	p_extra = leader;
	did_ai = TRUE;	    // So truncating blanks works with comments
	less_cols -= lead_len;
    }
    अण्णहा
	end_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    जइ (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    जइ (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_पङ्क्तिः_गणनम्)
    {
	जइ (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    गच्छ theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#ifdef FEAT_PROP_POPUP
	जइ ((State & MODE_निवेशय) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_split(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.col + 1, 0, at_eol);
#endif
    }
    अण्णहा
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	जइ (curwin->w_cursor.lnum >= Insstart.lnum + vr_पङ्क्तयः_changed)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (शून्य)u_save_cursor();		    // errors are ignored!
	    vr_पङ्क्तयः_changed++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	changed_bytes(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    जइ (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	जइ (did_si)
	{
	    पूर्णाङ्क sw = (पूर्णाङ्क)get_sw_मानम्(curbuf);

	    जइ (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	जइ (curbuf->b_p_ci)
	{
	    (शून्य)copy_indent(newindent, saved_पङ्क्तिः);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	अण्णहा
	    (शून्य)set_indent(newindent, SIN_निवेशय);
	less_cols -= curwin->w_cursor.col;

	ai_col = curwin->w_cursor.col;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	जइ (REPLACE_NORMAL(State))
	    प्रति (n = 0; n < (पूर्णाङ्क)curwin->w_cursor.col; ++n)
		replace_प्रक्षिप(NUL);
	newcol += curwin->w_cursor.col;
	जइ (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    जइ (REPLACE_NORMAL(State))
	जाव (lead_len-- > 0)
	    replace_प्रक्षिप(NUL);

    curwin->w_cursor = old_cursor;

    जइ (dir == FORWARD)
    {
	जइ (trunc_पङ्क्तिः || (State & MODE_निवेशय))
	{
	    // truncate current line at cursor
	    saved_पङ्क्तिः[curwin->w_cursor.col] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    जइ (trunc_पङ्क्तिः && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_पङ्क्तिः, curwin->w_cursor.col);
	    ml_replace(curwin->w_cursor.lnum, saved_पङ्क्तिः, FALSE);
	    saved_पङ्क्तिः = रिक्त;
	    जइ (did_append)
	    {
		changed_पङ्क्तयः(curwin->w_cursor.lnum, curwin->w_cursor.col,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		जइ (flags & OPENLINE_MARKFIX)
		    mark_col_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.col + less_cols_off,
						      1L, (long)-less_cols, 0);
#ifdef FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		जइ (curbuf->b_has_textprop && less_cols_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_cols_off, 0);
#endif
	    }
	    अण्णहा
		changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    जइ (did_append)
	changed_पङ्क्तयः(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.col = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    जइ (State & VREPLACE_FLAG)
    {
	vreplace_mode = State;	// So we know to put things right later
	State = MODE_निवेशय;
    }
    अण्णहा
	vreplace_mode = 0;

    जइ (!p_paste)
    {
	जइ (leader == रिक्त
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_col = (colnr_T)getwhitecols_curline();
	}
	अण्णहा जइ (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_व्यञ्जकम्_indent();
	    ai_col = (colnr_T)getwhitecols_curline();
	}
    }

    जइ (vreplace_mode != 0)
	State = vreplace_mode;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    जइ (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	जइ (p_extra == रिक्त)
	    गच्छ theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, next_पङ्क्तिः, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;
	ins_bytes(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	next_पङ्क्तिः = रिक्त;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_पङ्क्तिः);
    vim_free(next_पङ्क्तिः);
    vim_free(allocated);
    प्रत्यागम retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
truncate_पङ्क्तिः(पूर्णाङ्क fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    char_u	*old_पङ्क्तिः;
    पूर्णाङ्क		deleted;

    old_पङ्क्तिः = ml_get(lnum);
    जइ (col == 0)
	newp = vim_strsave((char_u *)"");
    अण्णहा
	newp = vim_strnsave(old_पङ्क्तिः, col);
    deleted = (पूर्णाङ्क)ml_get_len(lnum) - col;

    जइ (newp == रिक्त)
	प्रत्यागम FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, curwin->w_cursor.col, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    जइ (fixpos && curwin->w_cursor.col > 0)
	--curwin->w_cursor.col;

    प्रत्यागम OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    शून्य
del_पङ्क्तयः(long nlines, पूर्णाङ्क undo)
{
    long	n;
    linenr_T	first = curwin->w_cursor.lnum;

    जइ (nlines <= 0)
	प्रत्यागम;

    // save the deleted lines for undo
    जइ (undo && u_savedel(first, nlines) == FAIL)
	प्रत्यागम;

    प्रति (n = 0; n < nlines; )
    {
	जइ (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    भङ्ग;

	ml_delete_flags(first, ML_DEL_MESSAGE);
	++n;

	// If we delete the last line in the file, stop
	जइ (first > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.col = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_पङ्क्तयः_mark(first, n);
}
