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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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
    ਖਾਲੀ
change_warning(ਪੂਰਨ ਖਾਨਾ)
{
    ਸਥਿਰ ਅੱਖਰ *w_readonly = N_("W10: Warning: Changing a readonly file");

    ਜੇ (curbuf->b_did_warn
	    || curbuf_Is_ਬਦਲਿਆ()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	ਵਾਪਸ;

    ++curbuf_ਤਾਲਾ;
    apply_autocmds(EVENT_FILECHANGEDRO, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    --curbuf_ਤਾਲਾ;
    ਜੇ (!curbuf->b_p_ro)
	ਵਾਪਸ;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)
	ਸੁਨੇਹਾ_ਖਾਨਾ = ਖਾਨਾ;
    ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
    ਸੁਨੇਹਾ_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | ਸੁਨੇਹਾ_HIST);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#ਜੇ_ਅੰਤ
    ਸੁਨੇਹਾ_clr_eos();
    (ਖਾਲੀ)ਸੁਨੇਹਾ_ਸਿਰਾ();
    ਜੇ (ਸੁਨੇਹਾ_silent == 0 && !silent_ਬਹੁਲਕ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && ਸਮਾਂ_for_testing != 1
#ਜੇ_ਅੰਤ
       )
    {
	ਬਾਹਰਲਾ_flush();
	ui_delay(1002L, TRUE); // give the user time to think about it
    }
    curbuf->b_did_warn = true;
    redraw_cmdline = FALSE;	// don't redraw and erase the message
    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ < ਪੰਕਤੀਆਂ - 1)
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
    ਖਾਲੀ
ਬਦਲਿਆ(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ਜੇ (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	ਜੇ (im_is_preediting() && !xim_ਬਦਲਿਆ_while_preediting)
	    ਵਾਪਸ;
	xim_ਬਦਲਿਆ_while_preediting = FALSE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (!curbuf->b_ਬਦਲਿਆ)
    {
	ਪੂਰਨ	save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	ਜੇ (curbuf->b_may_ਅਦਲਾ_ਬਦਲੀ && !bt_dontwrite(curbuf))
	{
	    ਪੂਰਨ save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    ਜੇ (need_wait_return && emsg_silent == 0 && !ਵਿੱਚ_assert_fails)
	    {
		ਬਾਹਰਲਾ_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;
	    }
	    ਨਹੀਂਤਾਂ
		need_wait_return = save_need_wait_return;
	}
	ਬਦਲਿਆ_ਅੰਦਰੂਨੀ();
    }
    ++CHANGEDTICK(curbuf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_ਮੇਲ = FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਅੰਦਰੂਨੀ(ਖਾਲੀ)
{
    curbuf->b_ਬਦਲਿਆ = TRUE;
    ml_setflags(curbuf);
    check_ਸਥਿਤੀ(curbuf);
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    need_maketitle = TRUE;	    // set window title later
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Set when listener callbacks are being invoked.
ਸਥਿਰ ਪੂਰਨ recursive = FALSE;

ਸਥਿਰ ਲੰਮਾ ਅਗਲਾ_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
ਸਥਿਰ bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    ਜੇ (prev != ਨਿਰਾ)
	prev->lr_ਅਗਲਾ = lnr->lr_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	*list = lnr->lr_ਅਗਲਾ;
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
    ਸਥਿਰ ਖਾਲੀ
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*ਅਗਲਾ;

    prev = ਨਿਰਾ;
    ਲਈ (lnr = *list; lnr != ਨਿਰਾ; lnr = ਅਗਲਾ)
    {
	ਅਗਲਾ = lnr->lr_ਅਗਲਾ;
	ਜੇ (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	ਨਹੀਂਤਾਂ
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    ਜੇ (buf != ਨਿਰਾ)
    {
	ਜੇ (*list == ਨਿਰਾ && buf->b_recorded_changes != ਨਿਰਾ)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = ਨਿਰਾ;
	    buf->b_recorded_ਪਾਠ_ਮਾਪ = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    ਸਥਿਰ ਖਾਲੀ
perform_listener_housekeeping(ਖਾਲੀ)
{
    buf_T	*buf;

    ਜੇ (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(ਨਿਰਾ, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    ਸਥਿਰ ਖਾਲੀ
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	ਲੰਮਾ		xtra)
{
    perform_listener_housekeeping();
    ਜੇ (buf->b_recorded_changes == ਨਿਰਾ || xtra == 0)
	ਵਾਪਸ;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_ਸੰਖਿਆ(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_ਸੰਖਿਆ(
		li->li_tv.vval.v_dict, "end");
	ਜੇ (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    ਰੋਕੋ;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_ਪਾਠ_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    ਸਥਿਰ bool
listeners_want_ਪਾਠ(listener_T *list)
{
    listener_T	*lnr;

    ਲਈ (lnr = list; lnr != ਨਿਰਾ; lnr = lnr->lr_ਅਗਲਾ)
	ਜੇ (lnr->lr_ਪਾਠ)
	    ਵਾਪਸ true;
    ਵਾਪਸ false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    ਸਥਿਰ size_t
ਜੋੜੋ_change_ਪਾਠ(
    buf_T	*buf,
    dict_T	*dict,
    linenr_T	lnum,
    linenr_T	lnume,
    ਲੰਮਾ	xtra)
{
    list_T	*l = list_alloc();
    linenr_T	ਹੇਠਾਂ = lnume + xtra;	// line below the changed region
    size_t	ਮਾਪ = 0;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (ਹੇਠਾਂ > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	ਹੇਠਾਂ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
    ਲਈ (linenr_T lp = lnum < 1 ? 1 : lnum; lp < ਹੇਠਾਂ; ++lp)
    {
	char_u	*ਸਤਰ = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	ਜੇ (list_append_string(l, ਸਤਰ, len) == FAIL)
	{
	    list_free(l);
	    ਵਾਪਸ 0;
	}
	ਮਾਪ += (size_t)len + 1;
    }

    ਜੇ (dict_ਜੋੜੋ_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ ਮਾਪ;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    ਸਥਿਰ ਖਾਲੀ
may_ਰਿਕਾਰਡ_change(
    linenr_T	lnum,
    colnr_T	ਖਾਨਾ,
    linenr_T	lnume,
    ਲੰਮਾ	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    ਜੇ (curbuf->b_listener == ਨਿਰਾ)
	ਵਾਪਸ;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    ਜੇ (curbuf->b_recorded_changes == ਨਿਰਾ)
    {
	curbuf->b_recorded_changes = list_alloc();
	ਜੇ (curbuf->b_recorded_changes == ਨਿਰਾ)  // out of memory
	    ਵਾਪਸ;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ;
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", (varnumber_T)lnum);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "end", (varnumber_T)lnume);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "added", (varnumber_T)xtra);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", (varnumber_T)ਖਾਨਾ + 1);
    ਜੇ (listeners_want_ਪਾਠ(curbuf->b_listener))
	curbuf->b_recorded_ਪਾਠ_ਮਾਪ +=
			  ਜੋੜੋ_change_ਪਾਠ(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    ਜੇ (!recursive && curbuf->b_recorded_ਪਾਠ_ਮਾਪ > LISTENER_ਪਾਠ_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    ਖਾਲੀ
f_listener_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_ਪਾਠ = false;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (recursive)
    {
	emsg(_(e_cannot_ਜੋੜੋ_listener_ਵਿੱਚ_listener_callback));
	ਵਾਪਸ;
    }

    ਜੇ (ਵਿੱਚ_vim9script() && (
	    check_for_opt_ਬਫਰ_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    callback = get_callback(&argvars[0]);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    free_callback(&callback);
	    ਵਾਪਸ;
	}
	ਜੇ (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_ਪਾਠ = dict_get_bool(d, "text", false);
	}
	ਨਹੀਂਤਾਂ ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    ਜੇ (lnr == ਨਿਰਾ)
    {
	free_callback(&callback);
	ਵਾਪਸ;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    ਜੇ (unbuffered)
    {
	lnr->lr_ਅਗਲਾ = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    ਨਹੀਂਤਾਂ
    {
	lnr->lr_ਅਗਲਾ = buf->b_listener;
	buf->b_listener = lnr;
    }

    ਸੈੱਟ_callback(&lnr->lr_callback, &callback);

    lnr->lr_ਪਾਠ = want_ਪਾਠ;
    lnr->lr_id = ++ਅਗਲਾ_listener_id;
    rettv->vval.v_ਸੰਖਿਆ = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    ਖਾਲੀ
f_listener_flush(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf = curbuf;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (recursive)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    ਸਥਿਰ listener_T *
ਲੱਭੋ_listener(
    ਪੂਰਨ        id,
    listener_T *list_ਸ਼ੁਰੂ,
    listener_T **prev)
{
    listener_T *ਅਗਲਾ;
    listener_T *lnr;

    *prev = ਨਿਰਾ;
    ਲਈ (lnr = list_ਸ਼ੁਰੂ; lnr != ਨਿਰਾ; lnr = ਅਗਲਾ)
    {
	ਅਗਲਾ = lnr->lr_ਅਗਲਾ;
	ਜੇ (lnr->lr_id == id)
	    ਵਾਪਸ lnr;
	*prev = lnr;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    ਖਾਲੀ
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    ਪੂਰਨ		id;
    buf_T	*buf;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = tv_get_ਸੰਖਿਆ(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = ਲੱਭੋ_listener(id, buf->b_listener, &prev);
	ਜੇ (lnr == ਨਿਰਾ)
	    lnr = ਲੱਭੋ_listener(id, buf->b_sync_listener, &prev);
	ਜੇ (lnr != ਨਿਰਾ)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_ਸੰਖਿਆ = 1;
	    ਵਾਪਸ;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    ਖਾਲੀ
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, ਪੂਰਨ added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_listener_ਸੈੱਟ(
    buf_T       *buf,
    linenr_T	ਸ਼ੁਰੂ,
    linenr_T	ਸਿਰਾ,
    ਲੰਮਾ	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    ਪੂਰਨ		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_ਸੰਖਿਆ;
    argv[1].vval.v_ਸੰਖਿਆ = ਸ਼ੁਰੂ;
    argv[2].v_type = VAR_ਸੰਖਿਆ;
    argv[2].vval.v_ਸੰਖਿਆ = ਸਿਰਾ;
    argv[3].v_type = VAR_ਸੰਖਿਆ;
    argv[3].vval.v_ਸੰਖਿਆ = added;
    argv[4].v_type = VAR_LIST;
    argv[4].vval.v_list = recorded_changes;

    // Ensure the list of changes is locked to prevent any modifications by
    // callback code..
    val.v_type = VAR_LIST;
    val.v_ਤਾਲਾ = 0;
    val.vval.v_list = recorded_changes;
    ਵਸਤੂ_ਤਾਲਾ(&val, -1, TRUE, FALSE);

    // Protect against recursive callbacks, lock the buffer against changes and
    // set the updating_screen flag to prevent channel input processing, which
    // might also try to update the buffer.
    recursive = TRUE;
    ++textlock;
    updating_screen = TRUE;

    ਲਈ (lnr = listeners; lnr != ਨਿਰਾ; lnr = lnr->lr_ਅਗਲਾ)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    ਜੇ (save_updating_screen)
	updating_screen = TRUE;
    ਨਹੀਂਤਾਂ
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	ਸ਼ੁਰੂ,
    colnr_T	ਖਾਨਾ,
    linenr_T	ਸਿਰਾ,
    ਲੰਮਾ	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    ਜੇ (recursive || curbuf->b_sync_listener == ਨਿਰਾ)
	ਵਾਪਸ;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    ਜੇ (recorded_changes == ਨਿਰਾ)  // out of memory
	ਵਾਪਸ;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
    {
	list_unref(recorded_changes);
	ਵਾਪਸ;
    }

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", (varnumber_T)ਸ਼ੁਰੂ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "end", (varnumber_T)ਸਿਰਾ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "added", (varnumber_T)added);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", (varnumber_T)ਖਾਨਾ + 1);
    ਜੇ (listeners_want_ਪਾਠ(buf->b_sync_listener))
	(ਖਾਲੀ)ਜੋੜੋ_change_ਪਾਠ(buf, dict, ਸ਼ੁਰੂ, ਸਿਰਾ, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_ਸੈੱਟ(
	buf, ਸ਼ੁਰੂ, ਸਿਰਾ, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    ਖਾਲੀ
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	ਸ਼ੁਰੂ = MAXLNUM;
    linenr_T	ਸਿਰਾ = 0;
    linenr_T	added = 0;

    ਜੇ (buf->b_recorded_changes == ਨਿਰਾ  // nothing changed
	    || buf->b_listener == ਨਿਰਾ	 // no listeners
	    || recursive)		 // already busy
	ਵਾਪਸ;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_ਸੰਖਿਆ(li->li_tv.vval.v_dict, "lnum");
	ਜੇ (ਸ਼ੁਰੂ > lnum)
	    ਸ਼ੁਰੂ = lnum;
	lnum = dict_get_ਸੰਖਿਆ(li->li_tv.vval.v_dict, "end");
	ਜੇ (ਸਿਰਾ < lnum)
	    ਸਿਰਾ = lnum;
	added += dict_get_ਸੰਖਿਆ(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_ਸੈੱਟ(
	buf, ਸ਼ੁਰੂ, ਸਿਰਾ, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = ਨਿਰਾ;
    buf->b_recorded_ਪਾਠ_ਮਾਪ = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    ਖਾਲੀ
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(ਨਿਰਾ, &buf->b_sync_listener, TRUE);
}

#ਜੇ_ਅੰਤ

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
ਬਦਲਿਆ_common(
    linenr_T	lnum,
    colnr_T	ਖਾਨਾ,
    linenr_T	lnume,
    ਲੰਮਾ	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਖਾਨੇ_ਗਿਣਤੀ;
    pos_T	*p;
    ਪੂਰਨ		ਜੋੜੋ;

    // mark the buffer as modified
    ਬਦਲਿਆ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Immediately send this change to any listeners that require changes not
    // to be buffered.
    invoke_sync_listeners(curbuf, lnum, ਖਾਨਾ, lnume, xtra);

    // If there are any listeners accepting buffered changes then add changes
    // to the current buffer's list, flushing previous changes first if necessary.
    may_ਰਿਕਾਰਡ_change(lnum, ਖਾਨਾ, lnume, xtra);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (curwin->w_p_diff && diff_ਅੰਦਰੂਨੀ())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_ਸਤਰ(lnum);
    }
#ਜੇ_ਅੰਤ

    // set the '. mark
    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_ਆਖਰੀ_change.lnum = lnum;
	curbuf->b_ਆਖਰੀ_change.ਖਾਨਾ = ਖਾਨਾ;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	ਜੇ (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    ਜੇ (curbuf->b_changelistlen == 0)
		ਜੋੜੋ = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		ਜੇ (p->lnum != lnum)
		    ਜੋੜੋ = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    ਖਾਨੇ_ਗਿਣਤੀ = comp_textwidth(FALSE);
		    ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ == 0)
			ਖਾਨੇ_ਗਿਣਤੀ = 79;
		    ਜੋੜੋ = (p->ਖਾਨਾ + ਖਾਨੇ_ਗਿਣਤੀ < ਖਾਨਾ || ਖਾਨਾ + ਖਾਨੇ_ਗਿਣਤੀ < p->ਖਾਨਾ);
		}
	    }
	    ਜੇ (ਜੋੜੋ)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		ਜੇ (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  ਆਕਾਰ(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			ਜੇ (wp->w_ਬਫਰ == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    ਜੇ (wp->w_ਬਫਰ == curbuf
			    && wp->w_changelistidx == curbuf->b_changelistlen)
			++wp->w_changelistidx;
		}
		++curbuf->b_changelistlen;
	    }
	}
	curbuf->b_changelist[curbuf->b_changelistlen - 1] =
							curbuf->b_ਆਖਰੀ_change;
	// The current window is always after the last change, so that "g,"
	// takes you back to it.
	curwin->w_changelistidx = curbuf->b_changelistlen;
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ਜੇ (wp->w_ਬਫਰ == curbuf)
	{
	    linenr_T ਆਖਰੀ = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    ਜੇ (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    ਜੇ (xtra != 0 && wp->w_redraw_ਸਿਖਰ != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    ਜੇ (wp->w_skipcol > 0
		    && (ਆਖਰੀ < wp->w_topline
			|| (wp->w_topline >= lnum
			    && wp->w_topline < lnume
			    && linetabsize_eol(wp, wp->w_topline)
				    <= wp->w_skipcol + sms_marker_overlap(wp, -1))))
		wp->w_skipcol = 0;

	    // Check if a change in the buffer has invalidated the cached
	    // values for the cursor.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // Update the folds for this window.  Can't postpone this, because
	    // a following operator might work on the whole fold: ">>dd".
	    foldUpdate(wp, lnum, ਆਖਰੀ);

	    // The change may cause lines above or below the change to become
	    // included in a fold.  Set lnum/lnume to the first/last line that
	    // might be displayed differently.
	    // Set w_cline_folded here as an efficient way to update it when
	    // inserting lines just above a closed fold.
	    i = hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, FALSE, ਨਿਰਾ);
	    ਜੇ (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, ਆਖਰੀ, ਨਿਰਾ, &ਆਖਰੀ, FALSE, ਨਿਰਾ);
	    ਜੇ (wp->w_cursor.lnum == ਆਖਰੀ)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    ਜੇ (wp->w_cursor.lnum <= lnum)
	    {
		i = ਲੱਭੋ_wl_ਇੰਦਰਾਜ(wp, lnum);
		ਜੇ (i >= 0 && wp->w_cursor.lnum > wp->w_ਸਤਰਾਂ[i].wl_lnum)
		    ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(wp);
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (wp->w_cursor.lnum > lnum)
		ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(wp);
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum == lnum && wp->w_cursor.ਖਾਨਾ >= ਖਾਨਾ)
		ਬਦਲਿਆ_cline_bef_curs_win(wp);
	    ਜੇ (wp->w_botline >= lnum)
	    {
		ਜੇ (xtra < 0)
		    invalidate_botline_win(wp);
		ਨਹੀਂਤਾਂ
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    ਲਈ (i = 0; i < wp->w_ਸਤਰਾਂ_valid; ++i)
		ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_valid)
		{
		    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_ਸਤਰਾਂ[i].wl_valid = FALSE;
			}
			ਨਹੀਂਤਾਂ ਜੇ (xtra != 0)
			{
			    // line below change
			    wp->w_ਸਤਰਾਂ[i].wl_lnum += xtra;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    wp->w_ਸਤਰਾਂ[i].wl_lastlnum += xtra;
#ਜੇ_ਅੰਤ
			}
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_ਸਤਰਾਂ[i].wl_valid = FALSE;
		    }
#ਜੇ_ਅੰਤ
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    ਜੇ (hasAnyFolding(wp))
		ਸੈੱਟ_topline(wp, wp->w_topline);
#ਜੇ_ਅੰਤ
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    ਜੇ (wp->w_p_rnu && xtra != 0)
		wp->w_ਆਖਰੀ_cursor_lnum_rnu = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (wp->w_p_cul && wp->w_ਆਖਰੀ_cursorline >= lnum)
	    {
		ਜੇ (wp->w_ਆਖਰੀ_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_ਆਖਰੀ_cursorline = 0;
		ਨਹੀਂਤਾਂ
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_ਆਖਰੀ_cursorline += xtra;
	    }
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਜੇ (wp == curwin && xtra != 0 && ਖੋਜ_hl_has_cursor_lnum >= lnum)
	    ਖੋਜ_hl_has_cursor_lnum += xtra;
#ਜੇ_ਅੰਤ
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    ਸੈੱਟ_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    ਜੇ (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	ਆਖਰੀ_cursormoved.lnum = 0;
}

    ਸਥਿਰ ਖਾਲੀ
ਬਦਲਿਆ_Oneline(buf_T *buf, linenr_T lnum)
{
    ਜੇ (buf->b_mod_ਸੈੱਟ)
    {
	// find the maximum area that must be redisplayed
	ਜੇ (lnum < buf->b_mod_ਸਿਖਰ)
	    buf->b_mod_ਸਿਖਰ = lnum;
	ਨਹੀਂਤਾਂ ਜੇ (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_ਸੈੱਟ = true;
	buf->b_mod_ਸਿਖਰ = lnum;
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
    ਖਾਲੀ
ਬਦਲਿਆ_ਬਾਈਟਾਂ(linenr_T lnum, colnr_T ਖਾਨਾ)
{
    ਬਦਲਿਆ_Oneline(curbuf, lnum);
    ਬਦਲਿਆ_common(lnum, ਖਾਨਾ, lnum + 1, 0L);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    ਜੇ (spell_check_ਖਿੜਕੀ(curwin)
	    && lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && vim_strchr(p_cpo, CPO_DOLLAR) == ਨਿਰਾ)
	redrawWinline(curwin, lnum + 1);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    ਜੇ (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ਜੇ (wlnum > 0)
		    ਬਦਲਿਆ_Oneline(wp->w_ਬਫਰ, wlnum);
	    }
    }
#ਜੇ_ਅੰਤ
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    ਖਾਲੀ
inserted_ਬਾਈਟਾਂ(linenr_T lnum, colnr_T ਖਾਨਾ, ਪੂਰਨ added ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, ਖਾਨਾ, added, 0);
#ਜੇ_ਅੰਤ

    ਬਦਲਿਆ_ਬਾਈਟਾਂ(lnum, ਖਾਨਾ);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    ਖਾਲੀ
appended_ਸਤਰਾਂ(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਬਦਲਿਆ_ਸਤਰਾਂ(lnum + 1, 0, lnum + 1, ਗਿਣਤੀ_2);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    ਖਾਲੀ
appended_ਸਤਰਾਂ_mark(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, ਗਿਣਤੀ_2, 0L);
    ਬਦਲਿਆ_ਸਤਰਾਂ(lnum + 1, 0, lnum + 1, ਗਿਣਤੀ_2);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    ਖਾਲੀ
deleted_ਸਤਰਾਂ(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਬਦਲਿਆ_ਸਤਰਾਂ(lnum, 0, lnum + ਗਿਣਤੀ_2, -ਗਿਣਤੀ_2);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    ਖਾਲੀ
deleted_ਸਤਰਾਂ_mark(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    mark_adjust(lnum, (linenr_T)(lnum + ਗਿਣਤੀ_2 - 1), (ਲੰਮਾ)MAXLNUM, -ਗਿਣਤੀ_2);
    ਬਦਲਿਆ_ਸਤਰਾਂ(lnum, 0, lnum + ਗਿਣਤੀ_2, -ਗਿਣਤੀ_2);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਸਤਰਾਂ_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    ਲੰਮਾ	xtra)	    // number of extra lines (negative when deleting)
{
    ਜੇ (buf->b_mod_ਸੈੱਟ)
    {
	// find the maximum area that must be redisplayed
	ਜੇ (lnum < buf->b_mod_ਸਿਖਰ)
	    buf->b_mod_ਸਿਖਰ = lnum;
	ਜੇ (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    ਜੇ (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	ਜੇ (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    ਨਹੀਂਤਾਂ
    {
	// set the area that must be redisplayed
	buf->b_mod_ਸੈੱਟ = true;
	buf->b_mod_ਸਿਖਰ = lnum;
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
    ਖਾਲੀ
ਬਦਲਿਆ_ਸਤਰਾਂ(
    linenr_T	lnum,	    // first line with change
    colnr_T	ਖਾਨਾ,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    ਲੰਮਾ	xtra)	    // number of extra lines (negative when deleting)
{
    ਬਦਲਿਆ_ਸਤਰਾਂ_buf(curbuf, lnum, lnume, xtra);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (xtra == 0 && curwin->w_p_diff && !diff_ਅੰਦਰੂਨੀ())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ਜੇ (wlnum > 0)
		    ਬਦਲਿਆ_ਸਤਰਾਂ_buf(wp->w_ਬਫਰ, wlnum,
						    lnume - lnum + wlnum, 0L);
	    }
    }
#ਜੇ_ਅੰਤ

    ਬਦਲਿਆ_common(lnum, ਖਾਨਾ, lnume, xtra);
}

/*
 * Called when the changed flag must be reset for buffer "buf".
 * When "ff" is TRUE also reset 'fileformat'.
 * When "always_inc_changedtick" is TRUE b:changedtick is incremented also when
 * the changed flag was off.
 */
    ਖਾਲੀ
unchanged(buf_T *buf, ਪੂਰਨ ff, ਪੂਰਨ always_inc_changedtick)
{
    ਜੇ (buf->b_ਬਦਲਿਆ || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_ਬਦਲਿਆ = 0;
	ml_setflags(buf);
	ਜੇ (ff)
	    save_file_ff(buf);
	check_ਸਥਿਤੀ(buf);
	redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    ਨਹੀਂਤਾਂ ਜੇ (always_inc_changedtick)
	++CHANGEDTICK(buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#ਜੇ_ਅੰਤ
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    ਖਾਲੀ
save_file_ff(buf_T *buf)
{
    buf->b_ਸ਼ੁਰੂ_ffc = *buf->b_p_ff;
    buf->b_ਸ਼ੁਰੂ_eof = buf->b_p_eof;
    buf->b_ਸ਼ੁਰੂ_eol = buf->b_p_eol;
    buf->b_ਸ਼ੁਰੂ_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    ਜੇ (buf->b_ਸ਼ੁਰੂ_fenc == ਨਿਰਾ
			     || STRCMP(buf->b_ਸ਼ੁਰੂ_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_ਸ਼ੁਰੂ_fenc);
	buf->b_ਸ਼ੁਰੂ_fenc = vim_strsave(buf->b_p_fenc);
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
    ਪੂਰਨ
file_ff_differs(buf_T *buf, ਪੂਰਨ ignore_ਖਾਲੀ_ਥਾਵਾਂ)
{
    // In a buffer that was never loaded the options are not valid.
    ਜੇ (buf->b_flags & BF_NEVERLOADED)
	ਵਾਪਸ FALSE;
    ਜੇ (ignore_ਖਾਲੀ_ਥਾਵਾਂ
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	ਵਾਪਸ FALSE;
    ਜੇ (buf->b_ਸ਼ੁਰੂ_ffc != *buf->b_p_ff)
	ਵਾਪਸ TRUE;
    ਜੇ ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_ਸ਼ੁਰੂ_eof != buf->b_p_eof
		|| buf->b_ਸ਼ੁਰੂ_eol != buf->b_p_eol))
	ਵਾਪਸ TRUE;
    ਜੇ (!buf->b_p_bin && buf->b_ਸ਼ੁਰੂ_bomb != buf->b_p_bomb)
	ਵਾਪਸ TRUE;
    ਜੇ (buf->b_ਸ਼ੁਰੂ_fenc == ਨਿਰਾ)
	ਵਾਪਸ (*buf->b_p_fenc != NUL);
    ਵਾਪਸ (STRCMP(buf->b_ਸ਼ੁਰੂ_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    ਖਾਲੀ
ins_ਬਾਈਟਾਂ(char_u *p)
{
    ins_ਬਾਈਟਾਂ_len(p, (ਪੂਰਨ)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    ਖਾਲੀ
ins_ਬਾਈਟਾਂ_len(char_u *p, ਪੂਰਨ len)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;

    ਜੇ (has_mbyte)
	ਲਈ (i = 0; i < len; i += n)
	{
	    ਜੇ (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    ਨਹੀਂਤਾਂ
		n = (*mb_ptr2len)(p + i);
	    ins_char_ਬਾਈਟਾਂ(p + i, n);
	}
    ਨਹੀਂਤਾਂ
	ਲਈ (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    ਖਾਲੀ
ins_char(ਪੂਰਨ c)
{
    char_u	buf[MB_MAXBYTES + 1];
    ਪੂਰਨ		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    ਜੇ (buf[0] == 0)
	buf[0] = '\n';

    ins_char_ਬਾਈਟਾਂ(buf, n);
}

    ਖਾਲੀ
ins_char_ਬਾਈਟਾਂ(char_u *buf, ਪੂਰਨ charlen)
{
    ਪੂਰਨ		c = buf[0];
    ਪੂਰਨ		newlen;		// nr of bytes inserted
    ਪੂਰਨ		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    ਪੂਰਨ		linelen;	// length of old line including NUL
    colnr_T	ਖਾਨਾ;
    linenr_T	lnum = curwin->w_cursor.lnum;
    ਪੂਰਨ		i;

    // Break tabs if needed.
    ਜੇ (virtual_ਸਰਗਰਮ() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    oldp = ml_get(lnum);
    linelen = (ਪੂਰਨ)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    ਜੇ (State & REPLACE_FLAG)
    {
	ਜੇ (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    ਪੂਰਨ		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    ਜੇ (old_list && vim_strchr(p_cpo, CPO_LISTWM) == ਨਿਰਾ)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, ਨਿਰਾ, &vcol, ਨਿਰਾ, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    ਜਦੋਂ (oldp[ਖਾਨਾ + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + ਖਾਨਾ + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		ਜੇ (vcol > new_vcol && oldp[ਖਾਨਾ + oldlen] == TAB)
		    ਰੋਕੋ;
		oldlen += (*mb_ptr2len)(oldp + ਖਾਨਾ + oldlen);
		// Deleted a bit too much, insert spaces.
		ਜੇ (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	ਨਹੀਂਤਾਂ ਜੇ (oldp[ਖਾਨਾ] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + ਖਾਨਾ);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_ਧੱਕੋ(NUL);
	ਲਈ (i = 0; i < oldlen; ++i)
	{
	    ਜੇ (has_mbyte)
		i += replace_ਧੱਕੋ_mb(oldp + ਖਾਨਾ + i) - 1;
	    ਨਹੀਂਤਾਂ
		replace_ਧੱਕੋ(oldp[ਖਾਨਾ + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    ਜੇ (newp == ਨਿਰਾ)
	ਵਾਪਸ;

    // Copy bytes before the cursor.
    ਜੇ (ਖਾਨਾ > 0)
	mch_memmove(newp, oldp, (size_t)ਖਾਨਾ);

    // Copy bytes after the changed character(s).
    p = newp + ਖਾਨਾ;
    ਜੇ (linelen > ਖਾਨਾ + oldlen)
	mch_memmove(p + newlen, oldp + ਖਾਨਾ + oldlen,
					    (size_t)(linelen - ਖਾਨਾ - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    ਜਦੋਂ (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    ਬਦਲਿਆ_ਬਾਈਟਾਂ(lnum, ਖਾਨਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, ਖਾਨਾ, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#ਜੇ_ਅੰਤ

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    ਜੇ (p_sm && (State & ਬਹੁਲਕ_ਪਾਓ)
	    && ਸੁਨੇਹਾ_silent == 0
	    && !ins_compl_ਸਰਗਰਮ())
    {
	ਜੇ (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	ਨਹੀਂਤਾਂ
	    showmatch(c);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (!p_ri || (State & REPLACE_FLAG))
#ਜੇ_ਅੰਤ
    {
	// Normal insert: move cursor right
	curwin->w_cursor.ਖਾਨਾ += charlen;
    }

    // TODO: should try to update w_row here, to avoid recomputing it later.
}

/*
 * Insert a string at the cursor position.
 * Note: Does NOT handle Replace mode.
 * Caller must have prepared for undo.
 */
    ਖਾਲੀ
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    ਪੂਰਨ		oldlen;
    colnr_T	ਖਾਨਾ;
    linenr_T	lnum = curwin->w_cursor.lnum;

    ਜੇ (virtual_ਸਰਗਰਮ() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    oldp = ml_get(lnum);
    oldlen = (ਪੂਰਨ)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    ਜੇ (newp == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (ਖਾਨਾ > 0)
	mch_memmove(newp, oldp, (size_t)ਖਾਨਾ);
    mch_memmove(newp + ਖਾਨਾ, s, slen);
    mch_memmove(newp + ਖਾਨਾ + slen, oldp + ਖਾਨਾ, (size_t)(oldlen - ਖਾਨਾ + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_ਬਾਈਟਾਂ(lnum, ਖਾਨਾ, (ਪੂਰਨ)slen);
    curwin->w_cursor.ਖਾਨਾ += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
del_char(ਪੂਰਨ fixpos)
{
    ਜੇ (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	ਜੇ (*ml_get_cursor() == NUL)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ del_chars(1L, fixpos);
    }
    ਵਾਪਸ del_ਬਾਈਟਾਂ(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    ਪੂਰਨ
del_chars(ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ fixpos)
{
    ਲੰਮਾ	ਬਾਈਟਾਂ = 0;
    ਲੰਮਾ	i;
    char_u	*p;
    ਪੂਰਨ		l;

    p = ml_get_cursor();
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2 && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	ਬਾਈਟਾਂ += l;
	p += l;
    }
    ਵਾਪਸ del_ਬਾਈਟਾਂ(ਬਾਈਟਾਂ, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
del_ਬਾਈਟਾਂ(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		fixpos_arg,
    ਪੂਰਨ		use_delcombine ਅਣਵਰਤਿਆ)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    ਪੂਰਨ		alloc_newp;
    ਲੰਮਾ	movelen;
    ਪੂਰਨ		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (ਪੂਰਨ)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    ਜੇ (ਖਾਨਾ >= oldlen)
	ਵਾਪਸ FAIL;

    // If "count" is zero there is nothing to do.
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ OK;

    // If "count" is negative the caller must be doing something wrong.
    ਜੇ (ਗਿਣਤੀ_2 < 1)
    {
	siemsg(e_invalid_ਗਿਣਤੀ_for_del_ਬਾਈਟਾਂ_nr, ਗਿਣਤੀ_2);
	ਵਾਪਸ FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    ਜੇ (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + ਖਾਨਾ) >= ਗਿਣਤੀ_2)
    {
	ਪੂਰਨ	cc[MAX_MCO];
	ਪੂਰਨ	n;

	(ਖਾਲੀ)utfc_ptr2char(oldp + ਖਾਨਾ, cc);
	ਜੇ (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = ਖਾਨਾ;
	    ਕਰੋ
	    {
		ਖਾਨਾ = n;
		ਗਿਣਤੀ_2 = utf_ptr2len(oldp + n);
		n += ਗਿਣਤੀ_2;
	    } ਜਦੋਂ (UTF_COMPOSINGLIKE(oldp + ਖਾਨਾ, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (ਲੰਮਾ)oldlen - (ਲੰਮਾ)ਖਾਨਾ - ਗਿਣਤੀ_2 + 1; // includes trailing NUL
    ਜੇ (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	ਜੇ (ਖਾਨਾ > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.ਖਾਨਾ;
	    curwin->w_cursor.coladd = 0;
	    ਜੇ (has_mbyte)
		curwin->w_cursor.ਖਾਨਾ -=
			    (*mb_ਸਿਰ_off)(oldp, oldp + curwin->w_cursor.ਖਾਨਾ);
	}
	ਗਿਣਤੀ_2 = oldlen - ਖਾਨਾ;
	movelen = 1;
    }
    newlen = oldlen - ਗਿਣਤੀ_2;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (netbeans_ਸਰਗਰਮ())
	alloc_newp = TRUE;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	alloc_newp = !ml_ਸਤਰ_alloced();    // check if oldp was allocated
    ਜੇ (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    ਨਹੀਂਤਾਂ
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	ਜੇ (newp == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	mch_memmove(newp, oldp, (size_t)ਖਾਨਾ);
    }
    mch_memmove(newp + ਖਾਨਾ, oldp + ਖਾਨਾ + ਗਿਣਤੀ_2, (size_t)movelen);
    ਜੇ (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// Also move any following text properties.
	ਜੇ (oldlen + 1 < curbuf->b_ml.ml_ਸਤਰ_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_ਸਤਰ_len - oldlen - 1);
#ਜੇ_ਅੰਤ
	curbuf->b_ml.ml_ਸਤਰ_len -= ਗਿਣਤੀ_2;
	curbuf->b_ml.ml_ਸਤਰ_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_ਬਾਈਟਾਂ(lnum, ਖਾਨਾ, -ਗਿਣਤੀ_2);

    ਵਾਪਸ OK;
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
    ਪੂਰਨ
open_ਸਤਰ(
    ਪੂਰਨ		dir,		// FORWARD or BACKWARD
    ਪੂਰਨ		flags,
    ਪੂਰਨ		ਦੂਜਾ_ਸਤਰ_indent,
    ਪੂਰਨ		*did_do_comment ਅਣਵਰਤਿਆ)
{
    char_u	*saved_ਸਤਰ;		// copy of the original line
    char_u	*ਅਗਲਾ_ਸਤਰ = ਨਿਰਾ;	// copy of the next line
    char_u	*p_extra = ਨਿਰਾ;	// what goes to next line
    ਪੂਰਨ		less_ਖਾਨੇ_ਗਿਣਤੀ = 0;		// less columns for mark in new line
    ਪੂਰਨ		less_ਖਾਨੇ_ਗਿਣਤੀ_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    ਪੂਰਨ		newcol = 0;		// new cursor column
    ਪੂਰਨ		newindent = 0;		// auto-indent of the new line
    ਪੂਰਨ		n;
    ਪੂਰਨ		trunc_ਸਤਰ = FALSE;	// truncate current line afterwards
    ਪੂਰਨ		retval = FAIL;		// return value
    ਪੂਰਨ		extra_len = 0;		// length of p_extra string
    ਪੂਰਨ		lead_len;		// length of comment leader
    ਪੂਰਨ		comment_ਸ਼ੁਰੂ = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = ਨਿਰਾ;		// copy of comment leader
    char_u	*allocated = ਨਿਰਾ;	// allocated memory
    char_u	*p;
    ਪੂਰਨ		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    ਪੂਰਨ		do_cindent;
    ਪੂਰਨ		do_si = may_do_si();
    ਪੂਰਨ		no_si = FALSE;		// reset did_si afterwards
    ਪੂਰਨ		ਪਹਿਲਾ_char = NUL;	// init for GCC
    ਪੂਰਨ		vreplace_ਬਹੁਲਕ;
    ਪੂਰਨ		did_append;		// appended a new line
    ਪੂਰਨ		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		at_eol;			// cursor after last character
#ਜੇ_ਅੰਤ

    // make a copy of the current line so we can mess with it
    saved_ਸਤਰ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ਜੇ (saved_ਸਤਰ == ਨਿਰਾ)	    // out of memory!
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.ਖਾਨਾ >= (ਪੂਰਨ)ml_get_curline_len();
#ਜੇ_ਅੰਤ

    ਜੇ (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	ਜੇ (curwin->w_cursor.lnum < orig_ਸਤਰ_ਗਿਣਤੀ)
	    ਅਗਲਾ_ਸਤਰ = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	ਨਹੀਂਤਾਂ
	    ਅਗਲਾ_ਸਤਰ = vim_strsave((char_u *)"");
	ਜੇ (ਅਗਲਾ_ਸਤਰ == ਨਿਰਾ)	    // out of memory!
	    ਜਾਓ theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_ਧੱਕੋ(NUL);  // Call twice because BS over NL expects it
	replace_ਧੱਕੋ(NUL);
	p = saved_ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ (has_mbyte)
		p += replace_ਧੱਕੋ_mb(p);
	    ਨਹੀਂਤਾਂ
		replace_ਧੱਕੋ(*p++);
	}
	saved_ਸਤਰ[curwin->w_cursor.ਖਾਨਾ] = NUL;
    }

    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;
	ਜੇ (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    ਪਹਿਲਾ_char = *p;
	}
	extra_len = (ਪੂਰਨ)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_ਖਾਨਾ = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    ਜੇ (dir == FORWARD && did_ai)
	trunc_ਸਤਰ = TRUE;

    ਜੇ ((flags & OPENLINE_FORCE_INDENT) && ਦੂਜਾ_ਸਤਰ_indent >= 0)
	newindent = ਦੂਜਾ_ਸਤਰ_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_ਸਤਰ, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ਨਹੀਂਤਾਂ
	newindent = get_indent_str(saved_ਸਤਰ, (ਪੂਰਨ)curbuf->b_p_ts, FALSE);
#ਜੇ_ਅੰਤ
	ਜੇ (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = ਦੂਜਾ_ਸਤਰ_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	ਜੇ (!trunc_ਸਤਰ && do_si && *saved_ਸਤਰ != NUL
				    && (p_extra == ਨਿਰਾ || ਪਹਿਲਾ_char != '{'))
	{
	    char_u  *ptr;
	    char_u  ਆਖਰੀ_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_ਸਤਰ;
	    ਜੇ (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, ਨਿਰਾ, FALSE, TRUE);
	    ਨਹੀਂਤਾਂ
		lead_len = 0;
	    ਜੇ (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ਜੇ ( lead_len == 0 && ptr[0] == '#')
		{
		    ਜਦੋਂ (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		ਜੇ (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, ਨਿਰਾ, FALSE, TRUE);
		ਨਹੀਂਤਾਂ
		    lead_len = 0;
		ਜੇ (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    ਜੇ (p[0] == '/' && p[1] == '*')
			p++;
		    ਜੇ (p[0] == '*')
		    {
			ਲਈ (p++; *p; p++)
			{
			    ਜੇ (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.ਖਾਨਾ = (colnr_T)(p - ptr);
				ਜੇ ((pos = findmatch(ਨਿਰਾ, NUL)) != ਨਿਰਾ)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    ਰੋਕੋ;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.ਖਾਨਾ;
			    }
			}
		    }
		}
		ਨਹੀਂਤਾਂ	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    ਜਦੋਂ (p > ptr && VIM_ISWHITE(*p))
			--p;
		    ਆਖਰੀ_char = *p;

		    // find the character just before the '{' or ';'
		    ਜੇ (ਆਖਰੀ_char == '{' || ਆਖਰੀ_char == ';')
		    {
			ਜੇ (p > ptr)
			    --p;
			ਜਦੋਂ (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    ਜੇ (*p == ')')
		    {
			curwin->w_cursor.ਖਾਨਾ = (colnr_T)(p - ptr);
			ਜੇ ((pos = findmatch(ਨਿਰਾ, '(')) != ਨਿਰਾ)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    ਜੇ (ਆਖਰੀ_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    ਨਹੀਂਤਾਂ ਜੇ (ਆਖਰੀ_char != ';' && ਆਖਰੀ_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ਜੇ (lead_len == 0 && ptr[0] == '#')
		{
		    ਪੂਰਨ was_backslashed = FALSE;

		    ਜਦੋਂ ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    {
			ਜੇ (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			ਨਹੀਂਤਾਂ
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    ਜੇ (was_backslashed)
			newindent = 0;	    // Got to end of file
		    ਨਹੀਂਤਾਂ
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		ਜੇ (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		ਨਹੀਂਤਾਂ		    // can delete indent when '{' typed
		    can_si_ਪਿੱਠ = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	ਜੇ (do_si)
	    can_si = TRUE;

	did_ai = TRUE;
    }

    // May do indenting after opening a new line.
    do_cindent = !p_paste && (curbuf->b_p_cin
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#ਜੇ_ਅੰਤ
		)
	    && ਵਿੱਚ_cinkeys(dir == FORWARD
		? ਕੁੰਜੀ_OPEN_FORW
		: ਕੁੰਜੀ_OPEN_ਪਿੱਠ, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    ਸਿਰਾ_comment_pending = NUL;
    ਜੇ (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_ਸਤਰ, &lead_flags,
							dir == BACKWARD, TRUE);
	ਜੇ (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_ਸ਼ੁਰੂ = check_linecomment(saved_ਸਤਰ);
	    ਜੇ (comment_ਸ਼ੁਰੂ != MAXCOL)
	    {
		lead_len = get_leader_len(saved_ਸਤਰ + comment_ਸ਼ੁਰੂ,
						     &lead_flags, FALSE, TRUE);
		ਜੇ (lead_len != 0)
		{
		    lead_len += comment_ਸ਼ੁਰੂ;
		    ਜੇ (did_do_comment != ਨਿਰਾ)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	lead_len = 0;
    ਜੇ (lead_len > 0)
    {
	char_u	*lead_repl = ਨਿਰਾ;	    // replaces comment leader
	ਪੂਰਨ	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_ਵਿਚਕਾਰ[COM_MAX_LEN];   // middle-comment string
	ਪੂਰਨ	lead_ਵਿਚਕਾਰ_len;	    // length of the lead_middle
	char_u	lead_ਸਿਰਾ[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_ਸਿਰਾ = ਨਿਰਾ;	    // where lead_end has been found
	ਪੂਰਨ	extra_space = FALSE;	    // append extra space
	ਪੂਰਨ	current_flag;
	ਪੂਰਨ	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	ਲਈ (p = lead_flags; *p && *p != ':'; ++p)
	{
	    ਜੇ (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		ਜਾਰੀ;
	    }
	    ਜੇ (*p == COM_ਸ਼ੁਰੂ || *p == COM_ਵਿਚਕਾਰ)
	    {
		current_flag = *p;
		ਜੇ (*p == COM_ਸ਼ੁਰੂ)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    ਜੇ (dir == BACKWARD)
		    {
			lead_len = 0;
			ਰੋਕੋ;
		    }

		    // find start of middle part
		    (ਖਾਲੀ)copy_option_part(&p, lead_ਵਿਚਕਾਰ, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		ਜਦੋਂ (*p && p[-1] != ':')	// find end of middle flags
		{
		    ਜੇ (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_ਵਿਚਕਾਰ_len = copy_option_part(&p, lead_ਵਿਚਕਾਰ, COM_MAX_LEN, ",");

		ਜਦੋਂ (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    ਜੇ (*p == COM_AUTO_ਸਿਰਾ)
			ਸਿਰਾ_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_ਸਿਰਾ, COM_MAX_LEN, ",");

		ਜੇ (ਸਿਰਾ_comment_pending == -1)	// we can set it now
		    ਸਿਰਾ_comment_pending = lead_ਸਿਰਾ[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		ਜੇ (dir == FORWARD)
		{
		    ਲਈ (p = saved_ਸਤਰ + lead_len; *p; ++p)
			ਜੇ (STRNCMP(p, lead_ਸਿਰਾ, n) == 0)
			{
			    comment_ਸਿਰਾ = p;
			    lead_len = 0;
			    ਰੋਕੋ;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		ਜੇ (lead_len > 0)
		{
		    ਜੇ (current_flag == COM_ਸ਼ੁਰੂ)
		    {
			lead_repl = lead_ਵਿਚਕਾਰ;
			lead_repl_len = lead_ਵਿਚਕਾਰ_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    ਜੇ (!VIM_ISWHITE(saved_ਸਤਰ[lead_len - 1])
			    && ((p_extra != ਨਿਰਾ
				    && (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ == lead_len)
				|| (p_extra == ਨਿਰਾ
				    && saved_ਸਤਰ[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		ਰੋਕੋ;
	    }
	    ਜੇ (*p == COM_ਸਿਰਾ)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		ਜੇ (dir == FORWARD)
		{
		    comment_ਸਿਰਾ = skipwhite(saved_ਸਤਰ);
		    lead_len = 0;
		    ਰੋਕੋ;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		ਜਦੋਂ (p > curbuf->b_p_com && *p != ',')
		    --p;
		ਲਈ (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (ਪੂਰਨ)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		ਲਈ (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    ਜੇ (*p2 == COM_AUTO_ਸਿਰਾ)
			ਸਿਰਾ_comment_pending = -1; // means we want to set it
		}
		ਜੇ (ਸਿਰਾ_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    ਜਦੋਂ (*p2 && *p2 != ',')
			p2++;
		    ਸਿਰਾ_comment_pending = p2[-1];
		}
		ਰੋਕੋ;
	    }
	    ਜੇ (*p == COM_ਪਹਿਲਾ)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		ਜੇ (dir == BACKWARD)
		    lead_len = 0;
		ਨਹੀਂਤਾਂ
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		ਰੋਕੋ;
	    }
	}
	ਜੇ (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (ਦੂਜਾ_ਸਤਰ_indent > 0 ? ਦੂਜਾ_ਸਤਰ_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    ਜੇ (leader == ਨਿਰਾ)
		lead_len = 0;
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ li;

		vim_strncpy(leader, saved_ਸਤਰ, lead_len);

		// TODO: handle multi-byte and double width chars
		ਲਈ (li = 0; li < comment_ਸ਼ੁਰੂ; ++li)
		    ਜੇ (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		ਜੇ (lead_repl != ਨਿਰਾ)
		{
		    ਪੂਰਨ		c = 0;
		    ਪੂਰਨ		off = 0;

		    ਲਈ (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			ਜੇ (*p == COM_ਸੱਜਾ || *p == COM_ਖੱਬਾ)
			    c = *p++;
			ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			ਨਹੀਂਤਾਂ
			    ++p;
		    }
		    ਜੇ (c == COM_ਸੱਜਾ)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			ਲਈ (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    ਪੂਰਨ	    repl_ਮਾਪ = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ਪੂਰਨ	    old_ਮਾਪ = 0;
			    char_u  *endp = p;
			    ਪੂਰਨ	    l;

			    ਜਦੋਂ (old_ਮਾਪ < repl_ਮਾਪ && p > leader)
			    {
				MB_PTR_ਪਿੱਠ(leader, p);
				old_ਮਾਪ += ptr2cells(p);
			    }
			    l = lead_repl_len - (ਪੂਰਨ)(endp - p);
			    ਜੇ (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			ਜੇ (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			ਜਦੋਂ (--p >= leader)
			{
			    ਪੂਰਨ l = mb_ਸਿਰ_off(leader, p);

			    ਜੇ (l > 1)
			    {
				p -= l;
				ਜੇ (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    ਨਹੀਂਤਾਂ	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    ਪੂਰਨ	    repl_ਮਾਪ = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ਪੂਰਨ	    i;
			    ਪੂਰਨ	    l;

			    ਲਈ (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				ਜੇ (vim_strnsize(p, i + l) > repl_ਮਾਪ)
				    ਰੋਕੋ;
			    }
			    ਜੇ (i != lead_repl_len)
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
			ਲਈ (p += lead_repl_len; p < leader + lead_len; ++p)
			    ਜੇ (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				ਜੇ (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				ਨਹੀਂਤਾਂ
				{
				    ਪੂਰਨ	    l = (*mb_ptr2len)(p);

				    ਜੇ (l > 1)
				    {
					ਜੇ (ptr2cells(p) > 1)
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
		    ਜੇ (curbuf->b_p_ai || do_si)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ਨਹੀਂਤਾਂ
			newindent = get_indent_str(leader,
						   (ਪੂਰਨ)curbuf->b_p_ts, FALSE);
#ਜੇ_ਅੰਤ

		    // Add the indent offset
		    ਜੇ (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    ਨਹੀਂਤਾਂ
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    ਜਦੋਂ (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			ਜੇ (vim_strchr(skipwhite(leader), '\t') != ਨਿਰਾ)
			    ਰੋਕੋ;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    ਜੇ (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		ਜੇ (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		ਜੇ (newindent || did_si)
		{
		    ਜਦੋਂ (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (comment_ਸਿਰਾ != ਨਿਰਾ)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    ਜੇ (comment_ਸਿਰਾ[0] == '*' && comment_ਸਿਰਾ[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.ਖਾਨਾ = (colnr_T)(comment_ਸਿਰਾ - saved_ਸਤਰ);
		ਜੇ ((pos = findmatch(ਨਿਰਾ, NUL)) != ਨਿਰਾ)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    ਜੇ (p_extra != ਨਿਰਾ)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	ਜੇ (REPLACE_ਸਾਧਾਰਨ(State))
	    replace_ਧੱਕੋ(NUL);	    // end of extra blanks
	ਜੇ (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    ਜਦੋਂ ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		ਜੇ (REPLACE_ਸਾਧਾਰਨ(State))
		    replace_ਧੱਕੋ(*p_extra);
		++p_extra;
		++less_ਖਾਨੇ_ਗਿਣਤੀ_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_ਖਾਨੇ_ਗਿਣਤੀ = (ਪੂਰਨ)(p_extra - saved_ਸਤਰ);
    }

    ਜੇ (p_extra == ਨਿਰਾ)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    ਜੇ (lead_len)
    {
	ਜੇ (flags & OPENLINE_COM_LIST && ਦੂਜਾ_ਸਤਰ_indent > 0)
	{
	    ਪੂਰਨ i;
	    ਪੂਰਨ padding = ਦੂਜਾ_ਸਤਰ_indent
					  - (newindent + (ਪੂਰਨ)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    ਲਈ (i = 0; i < padding; i++)
	    {
		STRCAT(leader, " ");
		less_ਖਾਨੇ_ਗਿਣਤੀ--;
		newcol++;
	    }
	}
	STRCAT(leader, p_extra);
	p_extra = leader;
	did_ai = TRUE;	    // So truncating blanks works with comments
	less_ਖਾਨੇ_ਗਿਣਤੀ -= lead_len;
    }
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    ਜੇ (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    ਜੇ (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਜੇ (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    ਜਾਓ theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_ਵੰਡੋ(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.ਖਾਨਾ + 1, 0, at_eol);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	ਜੇ (curwin->w_cursor.lnum >= Insstart.lnum + vr_ਸਤਰਾਂ_ਬਦਲਿਆ)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (ਖਾਲੀ)u_save_cursor();		    // errors are ignored!
	    vr_ਸਤਰਾਂ_ਬਦਲਿਆ++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    ਜੇ (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	ਜੇ (did_si)
	{
	    ਪੂਰਨ sw = (ਪੂਰਨ)get_sw_ਮੁੱਲ(curbuf);

	    ਜੇ (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	ਜੇ (curbuf->b_p_ci)
	{
	    (ਖਾਲੀ)copy_indent(newindent, saved_ਸਤਰ);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    (ਖਾਲੀ)ਸੈੱਟ_indent(newindent, SIN_ਪਾਓ);
	less_ਖਾਨੇ_ਗਿਣਤੀ -= curwin->w_cursor.ਖਾਨਾ;

	ai_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	ਜੇ (REPLACE_ਸਾਧਾਰਨ(State))
	    ਲਈ (n = 0; n < (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ; ++n)
		replace_ਧੱਕੋ(NUL);
	newcol += curwin->w_cursor.ਖਾਨਾ;
	ਜੇ (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    ਜੇ (REPLACE_ਸਾਧਾਰਨ(State))
	ਜਦੋਂ (lead_len-- > 0)
	    replace_ਧੱਕੋ(NUL);

    curwin->w_cursor = old_cursor;

    ਜੇ (dir == FORWARD)
    {
	ਜੇ (trunc_ਸਤਰ || (State & ਬਹੁਲਕ_ਪਾਓ))
	{
	    // truncate current line at cursor
	    saved_ਸਤਰ[curwin->w_cursor.ਖਾਨਾ] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    ਜੇ (trunc_ਸਤਰ && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_ਸਤਰ, curwin->w_cursor.ਖਾਨਾ);
	    ml_replace(curwin->w_cursor.lnum, saved_ਸਤਰ, FALSE);
	    saved_ਸਤਰ = ਨਿਰਾ;
	    ਜੇ (did_append)
	    {
		ਬਦਲਿਆ_ਸਤਰਾਂ(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		ਜੇ (flags & OPENLINE_MARKFIX)
		    mark_ਖਾਨਾ_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.ਖਾਨਾ + less_ਖਾਨੇ_ਗਿਣਤੀ_off,
						      1L, (ਲੰਮਾ)-less_ਖਾਨੇ_ਗਿਣਤੀ, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		ਜੇ (curbuf->b_has_textprop && less_ਖਾਨੇ_ਗਿਣਤੀ_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_ਖਾਨੇ_ਗਿਣਤੀ_off, 0);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
		ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    ਜੇ (did_append)
	ਬਦਲਿਆ_ਸਤਰਾਂ(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.ਖਾਨਾ = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    ਜੇ (State & VREPLACE_FLAG)
    {
	vreplace_ਬਹੁਲਕ = State;	// So we know to put things right later
	State = ਬਹੁਲਕ_ਪਾਓ;
    }
    ਨਹੀਂਤਾਂ
	vreplace_ਬਹੁਲਕ = 0;

    ਜੇ (!p_paste)
    {
	ਜੇ (leader == ਨਿਰਾ
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_ਖਾਨਾ = (colnr_T)getwhitecols_curline();
	}
	ਨਹੀਂਤਾਂ ਜੇ (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_ਵਿਅੰਜਕ_indent();
	    ai_ਖਾਨਾ = (colnr_T)getwhitecols_curline();
	}
    }

    ਜੇ (vreplace_ਬਹੁਲਕ != 0)
	State = vreplace_ਬਹੁਲਕ;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    ਜੇ (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ਜੇ (p_extra == ਨਿਰਾ)
	    ਜਾਓ theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, ਅਗਲਾ_ਸਤਰ, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.ਖਾਨਾ = 0;
	curwin->w_cursor.coladd = 0;
	ins_ਬਾਈਟਾਂ(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	ਅਗਲਾ_ਸਤਰ = ਨਿਰਾ;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_ਸਤਰ);
    vim_free(ਅਗਲਾ_ਸਤਰ);
    vim_free(allocated);
    ਵਾਪਸ retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
truncate_ਸਤਰ(ਪੂਰਨ fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    char_u	*old_ਸਤਰ;
    ਪੂਰਨ		deleted;

    old_ਸਤਰ = ml_get(lnum);
    ਜੇ (ਖਾਨਾ == 0)
	newp = vim_strsave((char_u *)"");
    ਨਹੀਂਤਾਂ
	newp = vim_strnsave(old_ਸਤਰ, ਖਾਨਾ);
    deleted = (ਪੂਰਨ)ml_get_len(lnum) - ਖਾਨਾ;

    ਜੇ (newp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_ਬਾਈਟਾਂ(lnum, curwin->w_cursor.ਖਾਨਾ, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    ਜੇ (fixpos && curwin->w_cursor.ਖਾਨਾ > 0)
	--curwin->w_cursor.ਖਾਨਾ;

    ਵਾਪਸ OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    ਖਾਲੀ
del_ਸਤਰਾਂ(ਲੰਮਾ nlines, ਪੂਰਨ undo)
{
    ਲੰਮਾ	n;
    linenr_T	ਪਹਿਲਾ = curwin->w_cursor.lnum;

    ਜੇ (nlines <= 0)
	ਵਾਪਸ;

    // save the deleted lines for undo
    ਜੇ (undo && u_savedel(ਪਹਿਲਾ, nlines) == FAIL)
	ਵਾਪਸ;

    ਲਈ (n = 0; n < nlines; )
    {
	ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)	    // nothing to delete
	    ਰੋਕੋ;

	ml_delete_flags(ਪਹਿਲਾ, ML_DEL_ਸੁਨੇਹਾ);
	++n;

	// If we delete the last line in the file, stop
	ਜੇ (ਪਹਿਲਾ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.ਖਾਨਾ = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_ਸਤਰਾਂ_mark(ਪਹਿਲਾ, n);
}
