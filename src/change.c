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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
    ಶೂನ್ಯ
change_warning(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *w_readonly = N_("W10: Warning: Changing a readonly file");

    ಆದರೆ (curbuf->b_did_warn
	    || curbuf_Is_ಬದಲಾಯಿತು()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	ಹಿಂತಿರುಗಿ;

    ++curbuf_ಬೀಗ;
    apply_autocmds(EVENT_FILECHANGEDRO, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    --curbuf_ಬೀಗ;
    ಆದರೆ (!curbuf->b_p_ro)
	ಹಿಂತಿರುಗಿ;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    ಸಂದೇಶ_ಆರಂಭ();
    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
    ಸಂದೇಶ_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | ಸಂದೇಶ_HIST);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#ಆದರೆ_ಅಂತ್ಯ
    ಸಂದೇಶ_clr_eos();
    (ಶೂನ್ಯ)ಸಂದೇಶ_ಕೊನೆ();
    ಆದರೆ (ಸಂದೇಶ_silent == 0 && !silent_ಬಹುಲಕ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && ಸಮಯ_for_testing != 1
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಹೊರಹೋಗು_flush();
	ui_delay(1002L, TRUE); // give the user time to think about it
    }
    curbuf->b_did_warn = true;
    redraw_cmdline = FALSE;	// don't redraw and erase the message
    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು - 1)
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
    ಶೂನ್ಯ
ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	ಆದರೆ (im_is_preediting() && !xim_ಬದಲಾಯಿತು_while_preediting)
	    ಹಿಂತಿರುಗಿ;
	xim_ಬದಲಾಯಿತು_while_preediting = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!curbuf->b_ಬದಲಾಯಿತು)
    {
	ಪೂರ್ಣಾಂಕ	save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	ಆದರೆ (curbuf->b_may_ಅದಲುಬದಲು && !bt_dontwrite(curbuf))
	{
	    ಪೂರ್ಣಾಂಕ save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    ಆದರೆ (need_wait_return && emsg_silent == 0 && !ಒಳ_assert_fails)
	    {
		ಹೊರಹೋಗು_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		need_wait_return = save_need_wait_return;
	}
	ಬದಲಾಯಿತು_ಆಂತರಿಕ();
    }
    ++CHANGEDTICK(curbuf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_ಹೊಂದಿಕೆ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಆಂತರಿಕ(ಶೂನ್ಯ)
{
    curbuf->b_ಬದಲಾಯಿತು = TRUE;
    ml_setflags(curbuf);
    check_ಸ್ಥಿತಿ(curbuf);
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    need_maketitle = TRUE;	    // set window title later
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Set when listener callbacks are being invoked.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recursive = FALSE;

ಸ್ಥಿರ ದೀರ್ಘ ಮುಂದಿನ_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
ಸ್ಥಿರ bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    ಆದರೆ (prev != ಖಾಲಿ)
	prev->lr_ಮುಂದಿನ = lnr->lr_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	*list = lnr->lr_ಮುಂದಿನ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*ಮುಂದಿನ;

    prev = ಖಾಲಿ;
    ಗಾಗಿ (lnr = *list; lnr != ಖಾಲಿ; lnr = ಮುಂದಿನ)
    {
	ಮುಂದಿನ = lnr->lr_ಮುಂದಿನ;
	ಆದರೆ (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	ಇಲ್ಲದಿದ್ದರೆ
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	ಆದರೆ (*list == ಖಾಲಿ && buf->b_recorded_changes != ಖಾಲಿ)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = ಖಾಲಿ;
	    buf->b_recorded_ಪಠ್ಯ_ಗಾತ್ರ = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
perform_listener_housekeeping(ಶೂನ್ಯ)
{
    buf_T	*buf;

    ಆದರೆ (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(ಖಾಲಿ, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	ದೀರ್ಘ		xtra)
{
    perform_listener_housekeeping();
    ಆದರೆ (buf->b_recorded_changes == ಖಾಲಿ || xtra == 0)
	ಹಿಂತಿರುಗಿ;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_ಸಂಖ್ಯೆ(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_ಸಂಖ್ಯೆ(
		li->li_tv.vval.v_dict, "end");
	ಆದರೆ (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    ನಿಲ್ಲಿಸು;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_ಪಠ್ಯ_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    ಸ್ಥಿರ bool
listeners_want_ಪಠ್ಯ(listener_T *list)
{
    listener_T	*lnr;

    ಗಾಗಿ (lnr = list; lnr != ಖಾಲಿ; lnr = lnr->lr_ಮುಂದಿನ)
	ಆದರೆ (lnr->lr_ಪಠ್ಯ)
	    ಹಿಂತಿರುಗಿ true;
    ಹಿಂತಿರುಗಿ false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    ಸ್ಥಿರ size_t
ಕೂಡಿಸು_change_ಪಠ್ಯ(
    buf_T	*buf,
    dict_T	*dict,
    linenr_T	lnum,
    linenr_T	lnume,
    ದೀರ್ಘ	xtra)
{
    list_T	*l = list_alloc();
    linenr_T	ಕೆಳಗೆ = lnume + xtra;	// line below the changed region
    size_t	ಗಾತ್ರ_2 = 0;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (ಕೆಳಗೆ > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಕೆಳಗೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
    ಗಾಗಿ (linenr_T lp = lnum < 1 ? 1 : lnum; lp < ಕೆಳಗೆ; ++lp)
    {
	char_u	*ಸಾಲು = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	ಆದರೆ (list_append_string(l, ಸಾಲು, len) == FAIL)
	{
	    list_free(l);
	    ಹಿಂತಿರುಗಿ 0;
	}
	ಗಾತ್ರ_2 += (size_t)len + 1;
    }

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_ದಾಖಲೆ_change(
    linenr_T	lnum,
    colnr_T	ಲಂಬಸಾಲು,
    linenr_T	lnume,
    ದೀರ್ಘ	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    ಆದರೆ (curbuf->b_listener == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    ಆದರೆ (curbuf->b_recorded_changes == ಖಾಲಿ)
    {
	curbuf->b_recorded_changes = list_alloc();
	ಆದರೆ (curbuf->b_recorded_changes == ಖಾಲಿ)  // out of memory
	    ಹಿಂತಿರುಗಿ;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", (varnumber_T)lnum);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "end", (varnumber_T)lnume);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "added", (varnumber_T)xtra);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", (varnumber_T)ಲಂಬಸಾಲು + 1);
    ಆದರೆ (listeners_want_ಪಠ್ಯ(curbuf->b_listener))
	curbuf->b_recorded_ಪಠ್ಯ_ಗಾತ್ರ +=
			  ಕೂಡಿಸು_change_ಪಠ್ಯ(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    ಆದರೆ (!recursive && curbuf->b_recorded_ಪಠ್ಯ_ಗಾತ್ರ > LISTENER_ಪಠ್ಯ_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    ಶೂನ್ಯ
f_listener_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_ಪಠ್ಯ = false;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (recursive)
    {
	emsg(_(e_cannot_ಕೂಡಿಸು_listener_ಒಳ_listener_callback));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಒಳ_vim9script() && (
	    check_for_opt_ಬಫರ್_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    callback = get_callback(&argvars[0]);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    free_callback(&callback);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_ಪಠ್ಯ = dict_get_bool(d, "text", false);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    ಆದರೆ (lnr == ಖಾಲಿ)
    {
	free_callback(&callback);
	ಹಿಂತಿರುಗಿ;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    ಆದರೆ (unbuffered)
    {
	lnr->lr_ಮುಂದಿನ = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	lnr->lr_ಮುಂದಿನ = buf->b_listener;
	buf->b_listener = lnr;
    }

    ಗಣ_callback(&lnr->lr_callback, &callback);

    lnr->lr_ಪಠ್ಯ = want_ಪಠ್ಯ;
    lnr->lr_id = ++ಮುಂದಿನ_listener_id;
    rettv->vval.v_ಸಂಖ್ಯೆ = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    ಶೂನ್ಯ
f_listener_flush(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf = curbuf;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    ಸ್ಥಿರ listener_T *
ಕಂಡುಹಿಡಿ_listener(
    ಪೂರ್ಣಾಂಕ        id,
    listener_T *list_ಆರಂಭ,
    listener_T **prev)
{
    listener_T *ಮುಂದಿನ;
    listener_T *lnr;

    *prev = ಖಾಲಿ;
    ಗಾಗಿ (lnr = list_ಆರಂಭ; lnr != ಖಾಲಿ; lnr = ಮುಂದಿನ)
    {
	ಮುಂದಿನ = lnr->lr_ಮುಂದಿನ;
	ಆದರೆ (lnr->lr_id == id)
	    ಹಿಂತಿರುಗಿ lnr;
	*prev = lnr;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    ಶೂನ್ಯ
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    ಪೂರ್ಣಾಂಕ		id;
    buf_T	*buf;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = tv_get_ಸಂಖ್ಯೆ(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = ಕಂಡುಹಿಡಿ_listener(id, buf->b_listener, &prev);
	ಆದರೆ (lnr == ಖಾಲಿ)
	    lnr = ಕಂಡುಹಿಡಿ_listener(id, buf->b_sync_listener, &prev);
	ಆದರೆ (lnr != ಖಾಲಿ)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    ಶೂನ್ಯ
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, ಪೂರ್ಣಾಂಕ added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_listener_ಗಣ(
    buf_T       *buf,
    linenr_T	ಆರಂಭ,
    linenr_T	ಕೊನೆ,
    ದೀರ್ಘ	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    ಪೂರ್ಣಾಂಕ		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[1].vval.v_ಸಂಖ್ಯೆ = ಆರಂಭ;
    argv[2].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[2].vval.v_ಸಂಖ್ಯೆ = ಕೊನೆ;
    argv[3].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[3].vval.v_ಸಂಖ್ಯೆ = added;
    argv[4].v_type = VAR_LIST;
    argv[4].vval.v_list = recorded_changes;

    // Ensure the list of changes is locked to prevent any modifications by
    // callback code..
    val.v_type = VAR_LIST;
    val.v_ಬೀಗ = 0;
    val.vval.v_list = recorded_changes;
    ಅಂಶ_ಬೀಗ(&val, -1, TRUE, FALSE);

    // Protect against recursive callbacks, lock the buffer against changes and
    // set the updating_screen flag to prevent channel input processing, which
    // might also try to update the buffer.
    recursive = TRUE;
    ++textlock;
    updating_screen = TRUE;

    ಗಾಗಿ (lnr = listeners; lnr != ಖಾಲಿ; lnr = lnr->lr_ಮುಂದಿನ)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    ಆದರೆ (save_updating_screen)
	updating_screen = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	ಆರಂಭ,
    colnr_T	ಲಂಬಸಾಲು,
    linenr_T	ಕೊನೆ,
    ದೀರ್ಘ	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    ಆದರೆ (recursive || curbuf->b_sync_listener == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    ಆದರೆ (recorded_changes == ಖಾಲಿ)  // out of memory
	ಹಿಂತಿರುಗಿ;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
    {
	list_unref(recorded_changes);
	ಹಿಂತಿರುಗಿ;
    }

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", (varnumber_T)ಆರಂಭ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "end", (varnumber_T)ಕೊನೆ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "added", (varnumber_T)added);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", (varnumber_T)ಲಂಬಸಾಲು + 1);
    ಆದರೆ (listeners_want_ಪಠ್ಯ(buf->b_sync_listener))
	(ಶೂನ್ಯ)ಕೂಡಿಸು_change_ಪಠ್ಯ(buf, dict, ಆರಂಭ, ಕೊನೆ, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_ಗಣ(
	buf, ಆರಂಭ, ಕೊನೆ, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    ಶೂನ್ಯ
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	ಆರಂಭ = MAXLNUM;
    linenr_T	ಕೊನೆ = 0;
    linenr_T	added = 0;

    ಆದರೆ (buf->b_recorded_changes == ಖಾಲಿ  // nothing changed
	    || buf->b_listener == ಖಾಲಿ	 // no listeners
	    || recursive)		 // already busy
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_ಸಂಖ್ಯೆ(li->li_tv.vval.v_dict, "lnum");
	ಆದರೆ (ಆರಂಭ > lnum)
	    ಆರಂಭ = lnum;
	lnum = dict_get_ಸಂಖ್ಯೆ(li->li_tv.vval.v_dict, "end");
	ಆದರೆ (ಕೊನೆ < lnum)
	    ಕೊನೆ = lnum;
	added += dict_get_ಸಂಖ್ಯೆ(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_ಗಣ(
	buf, ಆರಂಭ, ಕೊನೆ, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = ಖಾಲಿ;
    buf->b_recorded_ಪಠ್ಯ_ಗಾತ್ರ = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    ಶೂನ್ಯ
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(ಖಾಲಿ, &buf->b_sync_listener, TRUE);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಬದಲಾಯಿತು_common(
    linenr_T	lnum,
    colnr_T	ಲಂಬಸಾಲು,
    linenr_T	lnume,
    ದೀರ್ಘ	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    pos_T	*p;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು;

    // mark the buffer as modified
    ಬದಲಾಯಿತು();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Immediately send this change to any listeners that require changes not
    // to be buffered.
    invoke_sync_listeners(curbuf, lnum, ಲಂಬಸಾಲು, lnume, xtra);

    // If there are any listeners accepting buffered changes then add changes
    // to the current buffer's list, flushing previous changes first if necessary.
    may_ದಾಖಲೆ_change(lnum, ಲಂಬಸಾಲು, lnume, xtra);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (curwin->w_p_diff && diff_ಆಂತರಿಕ())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_ಸಾಲು(lnum);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // set the '. mark
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_ಕೊನೆಯ_change.lnum = lnum;
	curbuf->b_ಕೊನೆಯ_change.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	ಆದರೆ (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    ಆದರೆ (curbuf->b_changelistlen == 0)
		ಕೂಡಿಸು = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		ಆದರೆ (p->lnum != lnum)
		    ಕೂಡಿಸು = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = comp_textwidth(FALSE);
		    ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0)
			ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 79;
		    ಕೂಡಿಸು = (p->ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < ಲಂಬಸಾಲು || ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < p->ಲಂಬಸಾಲು);
		}
	    }
	    ಆದರೆ (ಕೂಡಿಸು)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		ಆದರೆ (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  ಗಾತ್ರ(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			ಆದರೆ (wp->w_ಬಫರ್ == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    ಆದರೆ (wp->w_ಬಫರ್ == curbuf
			    && wp->w_changelistidx == curbuf->b_changelistlen)
			++wp->w_changelistidx;
		}
		++curbuf->b_changelistlen;
	    }
	}
	curbuf->b_changelist[curbuf->b_changelistlen - 1] =
							curbuf->b_ಕೊನೆಯ_change;
	// The current window is always after the last change, so that "g,"
	// takes you back to it.
	curwin->w_changelistidx = curbuf->b_changelistlen;
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್ == curbuf)
	{
	    linenr_T ಕೊನೆಯ = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    ಆದರೆ (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    ಆದರೆ (xtra != 0 && wp->w_redraw_ತುದಿ != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    ಆದರೆ (wp->w_skipcol > 0
		    && (ಕೊನೆಯ < wp->w_topline
			|| (wp->w_topline >= lnum
			    && wp->w_topline < lnume
			    && linetabsize_eol(wp, wp->w_topline)
				    <= wp->w_skipcol + sms_marker_overlap(wp, -1))))
		wp->w_skipcol = 0;

	    // Check if a change in the buffer has invalidated the cached
	    // values for the cursor.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // Update the folds for this window.  Can't postpone this, because
	    // a following operator might work on the whole fold: ">>dd".
	    foldUpdate(wp, lnum, ಕೊನೆಯ);

	    // The change may cause lines above or below the change to become
	    // included in a fold.  Set lnum/lnume to the first/last line that
	    // might be displayed differently.
	    // Set w_cline_folded here as an efficient way to update it when
	    // inserting lines just above a closed fold.
	    i = hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, FALSE, ಖಾಲಿ);
	    ಆದರೆ (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, ಕೊನೆಯ, ಖಾಲಿ, &ಕೊನೆಯ, FALSE, ಖಾಲಿ);
	    ಆದರೆ (wp->w_cursor.lnum == ಕೊನೆಯ)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    ಆದರೆ (wp->w_cursor.lnum <= lnum)
	    {
		i = ಕಂಡುಹಿಡಿ_wl_ನಮೂದು(wp, lnum);
		ಆದರೆ (i >= 0 && wp->w_cursor.lnum > wp->w_ಸಾಲುಗಳು[i].wl_lnum)
		    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(wp);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (wp->w_cursor.lnum > lnum)
		ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(wp);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum == lnum && wp->w_cursor.ಲಂಬಸಾಲು >= ಲಂಬಸಾಲು)
		ಬದಲಾಯಿತು_cline_bef_curs_win(wp);
	    ಆದರೆ (wp->w_botline >= lnum)
	    {
		ಆದರೆ (xtra < 0)
		    invalidate_botline_win(wp);
		ಇಲ್ಲದಿದ್ದರೆ
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    ಗಾಗಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_valid)
		{
		    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_ಸಾಲುಗಳು[i].wl_valid = FALSE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xtra != 0)
			{
			    // line below change
			    wp->w_ಸಾಲುಗಳು[i].wl_lnum += xtra;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    wp->w_ಸಾಲುಗಳು[i].wl_lastlnum += xtra;
#ಆದರೆ_ಅಂತ್ಯ
			}
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_ಸಾಲುಗಳು[i].wl_valid = FALSE;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    ಆದರೆ (hasAnyFolding(wp))
		ಗಣ_topline(wp, wp->w_topline);
#ಆದರೆ_ಅಂತ್ಯ
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    ಆದರೆ (wp->w_p_rnu && xtra != 0)
		wp->w_ಕೊನೆಯ_cursor_lnum_rnu = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (wp->w_p_cul && wp->w_ಕೊನೆಯ_cursorline >= lnum)
	    {
		ಆದರೆ (wp->w_ಕೊನೆಯ_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_ಕೊನೆಯ_cursorline = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_ಕೊನೆಯ_cursorline += xtra;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಆದರೆ (wp == curwin && xtra != 0 && ಹುಡುಕು_hl_has_cursor_lnum >= lnum)
	    ಹುಡುಕು_hl_has_cursor_lnum += xtra;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    ಗಣ_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    ಆದರೆ (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	ಕೊನೆಯ_cursormoved.lnum = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಬದಲಾಯಿತು_Oneline(buf_T *buf, linenr_T lnum)
{
    ಆದರೆ (buf->b_mod_ಗಣ)
    {
	// find the maximum area that must be redisplayed
	ಆದರೆ (lnum < buf->b_mod_ತುದಿ)
	    buf->b_mod_ತುದಿ = lnum;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_ಗಣ = true;
	buf->b_mod_ತುದಿ = lnum;
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
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಬೈಟುಗಳು(linenr_T lnum, colnr_T ಲಂಬಸಾಲು)
{
    ಬದಲಾಯಿತು_Oneline(curbuf, lnum);
    ಬದಲಾಯಿತು_common(lnum, ಲಂಬಸಾಲು, lnum + 1, 0L);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    ಆದರೆ (spell_check_ಕಿಟಕಿ(curwin)
	    && lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && vim_strchr(p_cpo, CPO_DOLLAR) == ಖಾಲಿ)
	redrawWinline(curwin, lnum + 1);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    ಆದರೆ (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ಆದರೆ (wlnum > 0)
		    ಬದಲಾಯಿತು_Oneline(wp->w_ಬಫರ್, wlnum);
	    }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    ಶೂನ್ಯ
inserted_ಬೈಟುಗಳು(linenr_T lnum, colnr_T ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ added ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, ಲಂಬಸಾಲು, added, 0);
#ಆದರೆ_ಅಂತ್ಯ

    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(lnum, ಲಂಬಸಾಲು);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    ಶೂನ್ಯ
appended_ಸಾಲುಗಳು(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum + 1, 0, lnum + 1, ಎಣಿಕೆ_2);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    ಶೂನ್ಯ
appended_ಸಾಲುಗಳು_mark(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, ಎಣಿಕೆ_2, 0L);
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum + 1, 0, lnum + 1, ಎಣಿಕೆ_2);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    ಶೂನ್ಯ
deleted_ಸಾಲುಗಳು(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum, 0, lnum + ಎಣಿಕೆ_2, -ಎಣಿಕೆ_2);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    ಶೂನ್ಯ
deleted_ಸಾಲುಗಳು_mark(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    mark_adjust(lnum, (linenr_T)(lnum + ಎಣಿಕೆ_2 - 1), (ದೀರ್ಘ)MAXLNUM, -ಎಣಿಕೆ_2);
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum, 0, lnum + ಎಣಿಕೆ_2, -ಎಣಿಕೆ_2);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಸಾಲುಗಳು_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    ದೀರ್ಘ	xtra)	    // number of extra lines (negative when deleting)
{
    ಆದರೆ (buf->b_mod_ಗಣ)
    {
	// find the maximum area that must be redisplayed
	ಆದರೆ (lnum < buf->b_mod_ತುದಿ)
	    buf->b_mod_ತುದಿ = lnum;
	ಆದರೆ (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    ಆದರೆ (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	ಆದರೆ (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// set the area that must be redisplayed
	buf->b_mod_ಗಣ = true;
	buf->b_mod_ತುದಿ = lnum;
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
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಸಾಲುಗಳು(
    linenr_T	lnum,	    // first line with change
    colnr_T	ಲಂಬಸಾಲು,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    ದೀರ್ಘ	xtra)	    // number of extra lines (negative when deleting)
{
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು_buf(curbuf, lnum, lnume, xtra);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (xtra == 0 && curwin->w_p_diff && !diff_ಆಂತರಿಕ())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ಆದರೆ (wlnum > 0)
		    ಬದಲಾಯಿತು_ಸಾಲುಗಳು_buf(wp->w_ಬಫರ್, wlnum,
						    lnume - lnum + wlnum, 0L);
	    }
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಬದಲಾಯಿತು_common(lnum, ಲಂಬಸಾಲು, lnume, xtra);
}

/*
 * Called when the changed flag must be reset for buffer "buf".
 * When "ff" is TRUE also reset 'fileformat'.
 * When "always_inc_changedtick" is TRUE b:changedtick is incremented also when
 * the changed flag was off.
 */
    ಶೂನ್ಯ
unchanged(buf_T *buf, ಪೂರ್ಣಾಂಕ ff, ಪೂರ್ಣಾಂಕ always_inc_changedtick)
{
    ಆದರೆ (buf->b_ಬದಲಾಯಿತು || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_ಬದಲಾಯಿತು = 0;
	ml_setflags(buf);
	ಆದರೆ (ff)
	    save_file_ff(buf);
	check_ಸ್ಥಿತಿ(buf);
	redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (always_inc_changedtick)
	++CHANGEDTICK(buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    ಶೂನ್ಯ
save_file_ff(buf_T *buf)
{
    buf->b_ಆರಂಭ_ffc = *buf->b_p_ff;
    buf->b_ಆರಂಭ_eof = buf->b_p_eof;
    buf->b_ಆರಂಭ_eol = buf->b_p_eol;
    buf->b_ಆರಂಭ_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    ಆದರೆ (buf->b_ಆರಂಭ_fenc == ಖಾಲಿ
			     || STRCMP(buf->b_ಆರಂಭ_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_ಆರಂಭ_fenc);
	buf->b_ಆರಂಭ_fenc = vim_strsave(buf->b_p_fenc);
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
    ಪೂರ್ಣಾಂಕ
file_ff_differs(buf_T *buf, ಪೂರ್ಣಾಂಕ ignore_ಖಾಲಿಗಳು)
{
    // In a buffer that was never loaded the options are not valid.
    ಆದರೆ (buf->b_flags & BF_NEVERLOADED)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (ignore_ಖಾಲಿಗಳು
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (buf->b_ಆರಂಭ_ffc != *buf->b_p_ff)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_ಆರಂಭ_eof != buf->b_p_eof
		|| buf->b_ಆರಂಭ_eol != buf->b_p_eol))
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (!buf->b_p_bin && buf->b_ಆರಂಭ_bomb != buf->b_p_bomb)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (buf->b_ಆರಂಭ_fenc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (*buf->b_p_fenc != NUL);
    ಹಿಂತಿರುಗಿ (STRCMP(buf->b_ಆರಂಭ_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    ಶೂನ್ಯ
ins_ಬೈಟುಗಳು(char_u *p)
{
    ins_ಬೈಟುಗಳು_len(p, (ಪೂರ್ಣಾಂಕ)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    ಶೂನ್ಯ
ins_ಬೈಟುಗಳು_len(char_u *p, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (has_mbyte)
	ಗಾಗಿ (i = 0; i < len; i += n)
	{
	    ಆದರೆ (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    ಇಲ್ಲದಿದ್ದರೆ
		n = (*mb_ptr2len)(p + i);
	    ins_char_ಬೈಟುಗಳು(p + i, n);
	}
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    ಶೂನ್ಯ
ins_char(ಪೂರ್ಣಾಂಕ c)
{
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    ಆದರೆ (buf[0] == 0)
	buf[0] = '\n';

    ins_char_ಬೈಟುಗಳು(buf, n);
}

    ಶೂನ್ಯ
ins_char_ಬೈಟುಗಳು(char_u *buf, ಪೂರ್ಣಾಂಕ charlen)
{
    ಪೂರ್ಣಾಂಕ		c = buf[0];
    ಪೂರ್ಣಾಂಕ		newlen;		// nr of bytes inserted
    ಪೂರ್ಣಾಂಕ		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    ಪೂರ್ಣಾಂಕ		linelen;	// length of old line including NUL
    colnr_T	ಲಂಬಸಾಲು;
    linenr_T	lnum = curwin->w_cursor.lnum;
    ಪೂರ್ಣಾಂಕ		i;

    // Break tabs if needed.
    ಆದರೆ (virtual_ಸಕ್ರಿಯ() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    oldp = ml_get(lnum);
    linelen = (ಪೂರ್ಣಾಂಕ)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    ಆದರೆ (State & REPLACE_FLAG)
    {
	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    ಪೂರ್ಣಾಂಕ		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    ಆದರೆ (old_list && vim_strchr(p_cpo, CPO_LISTWM) == ಖಾಲಿ)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, ಖಾಲಿ, &vcol, ಖಾಲಿ, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    ಆಗ (oldp[ಲಂಬಸಾಲು + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + ಲಂಬಸಾಲು + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		ಆದರೆ (vcol > new_vcol && oldp[ಲಂಬಸಾಲು + oldlen] == TAB)
		    ನಿಲ್ಲಿಸು;
		oldlen += (*mb_ptr2len)(oldp + ಲಂಬಸಾಲು + oldlen);
		// Deleted a bit too much, insert spaces.
		ಆದರೆ (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oldp[ಲಂಬಸಾಲು] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + ಲಂಬಸಾಲು);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_ತಳ್ಳು(NUL);
	ಗಾಗಿ (i = 0; i < oldlen; ++i)
	{
	    ಆದರೆ (has_mbyte)
		i += replace_ತಳ್ಳು_mb(oldp + ಲಂಬಸಾಲು + i) - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		replace_ತಳ್ಳು(oldp[ಲಂಬಸಾಲು + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    ಆದರೆ (newp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Copy bytes before the cursor.
    ಆದರೆ (ಲಂಬಸಾಲು > 0)
	mch_memmove(newp, oldp, (size_t)ಲಂಬಸಾಲು);

    // Copy bytes after the changed character(s).
    p = newp + ಲಂಬಸಾಲು;
    ಆದರೆ (linelen > ಲಂಬಸಾಲು + oldlen)
	mch_memmove(p + newlen, oldp + ಲಂಬಸಾಲು + oldlen,
					    (size_t)(linelen - ಲಂಬಸಾಲು - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    ಆಗ (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(lnum, ಲಂಬಸಾಲು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, ಲಂಬಸಾಲು, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#ಆದರೆ_ಅಂತ್ಯ

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    ಆದರೆ (p_sm && (State & ಬಹುಲಕ_ಸೇರಿಸು)
	    && ಸಂದೇಶ_silent == 0
	    && !ins_compl_ಸಕ್ರಿಯ())
    {
	ಆದರೆ (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	ಇಲ್ಲದಿದ್ದರೆ
	    showmatch(c);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (!p_ri || (State & REPLACE_FLAG))
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Normal insert: move cursor right
	curwin->w_cursor.ಲಂಬಸಾಲು += charlen;
    }

    // TODO: should try to update w_row here, to avoid recomputing it later.
}

/*
 * Insert a string at the cursor position.
 * Note: Does NOT handle Replace mode.
 * Caller must have prepared for undo.
 */
    ಶೂನ್ಯ
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    ಪೂರ್ಣಾಂಕ		oldlen;
    colnr_T	ಲಂಬಸಾಲು;
    linenr_T	lnum = curwin->w_cursor.lnum;

    ಆದರೆ (virtual_ಸಕ್ರಿಯ() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    oldp = ml_get(lnum);
    oldlen = (ಪೂರ್ಣಾಂಕ)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    ಆದರೆ (newp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಲಂಬಸಾಲು > 0)
	mch_memmove(newp, oldp, (size_t)ಲಂಬಸಾಲು);
    mch_memmove(newp + ಲಂಬಸಾಲು, s, slen);
    mch_memmove(newp + ಲಂಬಸಾಲು + slen, oldp + ಲಂಬಸಾಲು, (size_t)(oldlen - ಲಂಬಸಾಲು + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_ಬೈಟುಗಳು(lnum, ಲಂಬಸಾಲು, (ಪೂರ್ಣಾಂಕ)slen);
    curwin->w_cursor.ಲಂಬಸಾಲು += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
del_char(ಪೂರ್ಣಾಂಕ fixpos)
{
    ಆದರೆ (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	ಆದರೆ (*ml_get_cursor() == NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ del_chars(1L, fixpos);
    }
    ಹಿಂತಿರುಗಿ del_ಬೈಟುಗಳು(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    ಪೂರ್ಣಾಂಕ
del_chars(ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ fixpos)
{
    ದೀರ್ಘ	ಬೈಟುಗಳು = 0;
    ದೀರ್ಘ	i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		l;

    p = ml_get_cursor();
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2 && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	ಬೈಟುಗಳು += l;
	p += l;
    }
    ಹಿಂತಿರುಗಿ del_ಬೈಟುಗಳು(ಬೈಟುಗಳು, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
del_ಬೈಟುಗಳು(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		fixpos_arg,
    ಪೂರ್ಣಾಂಕ		use_delcombine ಬಳಸದ)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		alloc_newp;
    ದೀರ್ಘ	movelen;
    ಪೂರ್ಣಾಂಕ		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (ಪೂರ್ಣಾಂಕ)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    ಆದರೆ (ಲಂಬಸಾಲು >= oldlen)
	ಹಿಂತಿರುಗಿ FAIL;

    // If "count" is zero there is nothing to do.
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ OK;

    // If "count" is negative the caller must be doing something wrong.
    ಆದರೆ (ಎಣಿಕೆ_2 < 1)
    {
	siemsg(e_invalid_ಎಣಿಕೆ_for_del_ಬೈಟುಗಳು_nr, ಎಣಿಕೆ_2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    ಆದರೆ (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + ಲಂಬಸಾಲು) >= ಎಣಿಕೆ_2)
    {
	ಪೂರ್ಣಾಂಕ	cc[MAX_MCO];
	ಪೂರ್ಣಾಂಕ	n;

	(ಶೂನ್ಯ)utfc_ptr2char(oldp + ಲಂಬಸಾಲು, cc);
	ಆದರೆ (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = ಲಂಬಸಾಲು;
	    ಮಾಡು
	    {
		ಲಂಬಸಾಲು = n;
		ಎಣಿಕೆ_2 = utf_ptr2len(oldp + n);
		n += ಎಣಿಕೆ_2;
	    } ಆಗ (UTF_COMPOSINGLIKE(oldp + ಲಂಬಸಾಲು, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (ದೀರ್ಘ)oldlen - (ದೀರ್ಘ)ಲಂಬಸಾಲು - ಎಣಿಕೆ_2 + 1; // includes trailing NUL
    ಆದರೆ (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	ಆದರೆ (ಲಂಬಸಾಲು > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.ಲಂಬಸಾಲು;
	    curwin->w_cursor.coladd = 0;
	    ಆದರೆ (has_mbyte)
		curwin->w_cursor.ಲಂಬಸಾಲು -=
			    (*mb_ತಲೆ_off)(oldp, oldp + curwin->w_cursor.ಲಂಬಸಾಲು);
	}
	ಎಣಿಕೆ_2 = oldlen - ಲಂಬಸಾಲು;
	movelen = 1;
    }
    newlen = oldlen - ಎಣಿಕೆ_2;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
	alloc_newp = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	alloc_newp = !ml_ಸಾಲು_alloced();    // check if oldp was allocated
    ಆದರೆ (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    ಇಲ್ಲದಿದ್ದರೆ
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	ಆದರೆ (newp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	mch_memmove(newp, oldp, (size_t)ಲಂಬಸಾಲು);
    }
    mch_memmove(newp + ಲಂಬಸಾಲು, oldp + ಲಂಬಸಾಲು + ಎಣಿಕೆ_2, (size_t)movelen);
    ಆದರೆ (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// Also move any following text properties.
	ಆದರೆ (oldlen + 1 < curbuf->b_ml.ml_ಸಾಲು_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_ಸಾಲು_len - oldlen - 1);
#ಆದರೆ_ಅಂತ್ಯ
	curbuf->b_ml.ml_ಸಾಲು_len -= ಎಣಿಕೆ_2;
	curbuf->b_ml.ml_ಸಾಲು_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_ಬೈಟುಗಳು(lnum, ಲಂಬಸಾಲು, -ಎಣಿಕೆ_2);

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ
open_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ		dir,		// FORWARD or BACKWARD
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		ಎರಡನೇ_ಸಾಲು_indent,
    ಪೂರ್ಣಾಂಕ		*did_do_comment ಬಳಸದ)
{
    char_u	*saved_ಸಾಲು;		// copy of the original line
    char_u	*ಮುಂದಿನ_ಸಾಲು = ಖಾಲಿ;	// copy of the next line
    char_u	*p_extra = ಖಾಲಿ;	// what goes to next line
    ಪೂರ್ಣಾಂಕ		less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;		// less columns for mark in new line
    ಪೂರ್ಣಾಂಕ		less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    ಪೂರ್ಣಾಂಕ		newcol = 0;		// new cursor column
    ಪೂರ್ಣಾಂಕ		newindent = 0;		// auto-indent of the new line
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		trunc_ಸಾಲು = FALSE;	// truncate current line afterwards
    ಪೂರ್ಣಾಂಕ		retval = FAIL;		// return value
    ಪೂರ್ಣಾಂಕ		extra_len = 0;		// length of p_extra string
    ಪೂರ್ಣಾಂಕ		lead_len;		// length of comment leader
    ಪೂರ್ಣಾಂಕ		comment_ಆರಂಭ = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = ಖಾಲಿ;		// copy of comment leader
    char_u	*allocated = ಖಾಲಿ;	// allocated memory
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    ಪೂರ್ಣಾಂಕ		do_cindent;
    ಪೂರ್ಣಾಂಕ		do_si = may_do_si();
    ಪೂರ್ಣಾಂಕ		no_si = FALSE;		// reset did_si afterwards
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_char = NUL;	// init for GCC
    ಪೂರ್ಣಾಂಕ		vreplace_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		did_append;		// appended a new line
    ಪೂರ್ಣಾಂಕ		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		at_eol;			// cursor after last character
#ಆದರೆ_ಅಂತ್ಯ

    // make a copy of the current line so we can mess with it
    saved_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ಆದರೆ (saved_ಸಾಲು == ಖಾಲಿ)	    // out of memory!
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)ml_get_curline_len();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	ಆದರೆ (curwin->w_cursor.lnum < orig_ಸಾಲು_ಎಣಿಕೆ)
	    ಮುಂದಿನ_ಸಾಲು = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮುಂದಿನ_ಸಾಲು = vim_strsave((char_u *)"");
	ಆದರೆ (ಮುಂದಿನ_ಸಾಲು == ಖಾಲಿ)	    // out of memory!
	    ಹೋಗು theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_ತಳ್ಳು(NUL);  // Call twice because BS over NL expects it
	replace_ತಳ್ಳು(NUL);
	p = saved_ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (has_mbyte)
		p += replace_ತಳ್ಳು_mb(p);
	    ಇಲ್ಲದಿದ್ದರೆ
		replace_ತಳ್ಳು(*p++);
	}
	saved_ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು] = NUL;
    }

    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;
	ಆದರೆ (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    ಮೊದಲ_char = *p;
	}
	extra_len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_ಲಂಬಸಾಲು = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    ಆದರೆ (dir == FORWARD && did_ai)
	trunc_ಸಾಲು = TRUE;

    ಆದರೆ ((flags & OPENLINE_FORCE_INDENT) && ಎರಡನೇ_ಸಾಲು_indent >= 0)
	newindent = ಎರಡನೇ_ಸಾಲು_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_ಸಾಲು, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
	newindent = get_indent_str(saved_ಸಾಲು, (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = ಎರಡನೇ_ಸಾಲು_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	ಆದರೆ (!trunc_ಸಾಲು && do_si && *saved_ಸಾಲು != NUL
				    && (p_extra == ಖಾಲಿ || ಮೊದಲ_char != '{'))
	{
	    char_u  *ptr;
	    char_u  ಕೊನೆಯ_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_ಸಾಲು;
	    ಆದರೆ (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, ಖಾಲಿ, FALSE, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
		lead_len = 0;
	    ಆದರೆ (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ಆದರೆ ( lead_len == 0 && ptr[0] == '#')
		{
		    ಆಗ (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		ಆದರೆ (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, ಖಾಲಿ, FALSE, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		    lead_len = 0;
		ಆದರೆ (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    ಆದರೆ (p[0] == '/' && p[1] == '*')
			p++;
		    ಆದರೆ (p[0] == '*')
		    {
			ಗಾಗಿ (p++; *p; p++)
			{
			    ಆದರೆ (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(p - ptr);
				ಆದರೆ ((pos = findmatch(ಖಾಲಿ, NUL)) != ಖಾಲಿ)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    ನಿಲ್ಲಿಸು;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.ಲಂಬಸಾಲು;
			    }
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    ಆಗ (p > ptr && VIM_ISWHITE(*p))
			--p;
		    ಕೊನೆಯ_char = *p;

		    // find the character just before the '{' or ';'
		    ಆದರೆ (ಕೊನೆಯ_char == '{' || ಕೊನೆಯ_char == ';')
		    {
			ಆದರೆ (p > ptr)
			    --p;
			ಆಗ (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    ಆದರೆ (*p == ')')
		    {
			curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(p - ptr);
			ಆದರೆ ((pos = findmatch(ಖಾಲಿ, '(')) != ಖಾಲಿ)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    ಆದರೆ (ಕೊನೆಯ_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆಯ_char != ';' && ಕೊನೆಯ_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ಆದರೆ (lead_len == 0 && ptr[0] == '#')
		{
		    ಪೂರ್ಣಾಂಕ was_backslashed = FALSE;

		    ಆಗ ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    {
			ಆದರೆ (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    ಆದರೆ (was_backslashed)
			newindent = 0;	    // Got to end of file
		    ಇಲ್ಲದಿದ್ದರೆ
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		ಆದರೆ (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ		    // can delete indent when '{' typed
		    can_si_ಹಿಂಭಾಗ = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	ಆದರೆ (do_si)
	    can_si = TRUE;

	did_ai = TRUE;
    }

    // May do indenting after opening a new line.
    do_cindent = !p_paste && (curbuf->b_p_cin
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#ಆದರೆ_ಅಂತ್ಯ
		)
	    && ಒಳ_cinkeys(dir == FORWARD
		? ಕೀಲಿ_OPEN_FORW
		: ಕೀಲಿ_OPEN_ಹಿಂಭಾಗ, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    ಕೊನೆ_comment_pending = NUL;
    ಆದರೆ (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_ಸಾಲು, &lead_flags,
							dir == BACKWARD, TRUE);
	ಆದರೆ (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_ಆರಂಭ = check_linecomment(saved_ಸಾಲು);
	    ಆದರೆ (comment_ಆರಂಭ != MAXCOL)
	    {
		lead_len = get_leader_len(saved_ಸಾಲು + comment_ಆರಂಭ,
						     &lead_flags, FALSE, TRUE);
		ಆದರೆ (lead_len != 0)
		{
		    lead_len += comment_ಆರಂಭ;
		    ಆದರೆ (did_do_comment != ಖಾಲಿ)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lead_len = 0;
    ಆದರೆ (lead_len > 0)
    {
	char_u	*lead_repl = ಖಾಲಿ;	    // replaces comment leader
	ಪೂರ್ಣಾಂಕ	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_ಮಧ್ಯ[COM_MAX_LEN];   // middle-comment string
	ಪೂರ್ಣಾಂಕ	lead_ಮಧ್ಯ_len;	    // length of the lead_middle
	char_u	lead_ಕೊನೆ[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_ಕೊನೆ = ಖಾಲಿ;	    // where lead_end has been found
	ಪೂರ್ಣಾಂಕ	extra_space = FALSE;	    // append extra space
	ಪೂರ್ಣಾಂಕ	current_flag;
	ಪೂರ್ಣಾಂಕ	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	ಗಾಗಿ (p = lead_flags; *p && *p != ':'; ++p)
	{
	    ಆದರೆ (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (*p == COM_ಆರಂಭ || *p == COM_ಮಧ್ಯ)
	    {
		current_flag = *p;
		ಆದರೆ (*p == COM_ಆರಂಭ)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    ಆದರೆ (dir == BACKWARD)
		    {
			lead_len = 0;
			ನಿಲ್ಲಿಸು;
		    }

		    // find start of middle part
		    (ಶೂನ್ಯ)copy_option_part(&p, lead_ಮಧ್ಯ, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		ಆಗ (*p && p[-1] != ':')	// find end of middle flags
		{
		    ಆದರೆ (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_ಮಧ್ಯ_len = copy_option_part(&p, lead_ಮಧ್ಯ, COM_MAX_LEN, ",");

		ಆಗ (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    ಆದರೆ (*p == COM_AUTO_ಕೊನೆ)
			ಕೊನೆ_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_ಕೊನೆ, COM_MAX_LEN, ",");

		ಆದರೆ (ಕೊನೆ_comment_pending == -1)	// we can set it now
		    ಕೊನೆ_comment_pending = lead_ಕೊನೆ[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		ಆದರೆ (dir == FORWARD)
		{
		    ಗಾಗಿ (p = saved_ಸಾಲು + lead_len; *p; ++p)
			ಆದರೆ (STRNCMP(p, lead_ಕೊನೆ, n) == 0)
			{
			    comment_ಕೊನೆ = p;
			    lead_len = 0;
			    ನಿಲ್ಲಿಸು;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		ಆದರೆ (lead_len > 0)
		{
		    ಆದರೆ (current_flag == COM_ಆರಂಭ)
		    {
			lead_repl = lead_ಮಧ್ಯ;
			lead_repl_len = lead_ಮಧ್ಯ_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    ಆದರೆ (!VIM_ISWHITE(saved_ಸಾಲು[lead_len - 1])
			    && ((p_extra != ಖಾಲಿ
				    && (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು == lead_len)
				|| (p_extra == ಖಾಲಿ
				    && saved_ಸಾಲು[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (*p == COM_ಕೊನೆ)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		ಆದರೆ (dir == FORWARD)
		{
		    comment_ಕೊನೆ = skipwhite(saved_ಸಾಲು);
		    lead_len = 0;
		    ನಿಲ್ಲಿಸು;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		ಆಗ (p > curbuf->b_p_com && *p != ',')
		    --p;
		ಗಾಗಿ (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (ಪೂರ್ಣಾಂಕ)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		ಗಾಗಿ (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    ಆದರೆ (*p2 == COM_AUTO_ಕೊನೆ)
			ಕೊನೆ_comment_pending = -1; // means we want to set it
		}
		ಆದರೆ (ಕೊನೆ_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    ಆಗ (*p2 && *p2 != ',')
			p2++;
		    ಕೊನೆ_comment_pending = p2[-1];
		}
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (*p == COM_ಮೊದಲ)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		ಆದರೆ (dir == BACKWARD)
		    lead_len = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (ಎರಡನೇ_ಸಾಲು_indent > 0 ? ಎರಡನೇ_ಸಾಲು_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    ಆದರೆ (leader == ಖಾಲಿ)
		lead_len = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ li;

		vim_strncpy(leader, saved_ಸಾಲು, lead_len);

		// TODO: handle multi-byte and double width chars
		ಗಾಗಿ (li = 0; li < comment_ಆರಂಭ; ++li)
		    ಆದರೆ (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		ಆದರೆ (lead_repl != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ		c = 0;
		    ಪೂರ್ಣಾಂಕ		off = 0;

		    ಗಾಗಿ (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			ಆದರೆ (*p == COM_ಬಲ || *p == COM_ಎಡ)
			    c = *p++;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			ಇಲ್ಲದಿದ್ದರೆ
			    ++p;
		    }
		    ಆದರೆ (c == COM_ಬಲ)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			ಗಾಗಿ (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    ಪೂರ್ಣಾಂಕ	    repl_ಗಾತ್ರ = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ಪೂರ್ಣಾಂಕ	    old_ಗಾತ್ರ = 0;
			    char_u  *endp = p;
			    ಪೂರ್ಣಾಂಕ	    l;

			    ಆಗ (old_ಗಾತ್ರ < repl_ಗಾತ್ರ && p > leader)
			    {
				MB_PTR_ಹಿಂಭಾಗ(leader, p);
				old_ಗಾತ್ರ += ptr2cells(p);
			    }
			    l = lead_repl_len - (ಪೂರ್ಣಾಂಕ)(endp - p);
			    ಆದರೆ (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			ಆದರೆ (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			ಆಗ (--p >= leader)
			{
			    ಪೂರ್ಣಾಂಕ l = mb_ತಲೆ_off(leader, p);

			    ಆದರೆ (l > 1)
			    {
				p -= l;
				ಆದರೆ (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    ಪೂರ್ಣಾಂಕ	    repl_ಗಾತ್ರ = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ಪೂರ್ಣಾಂಕ	    i;
			    ಪೂರ್ಣಾಂಕ	    l;

			    ಗಾಗಿ (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				ಆದರೆ (vim_strnsize(p, i + l) > repl_ಗಾತ್ರ)
				    ನಿಲ್ಲಿಸು;
			    }
			    ಆದರೆ (i != lead_repl_len)
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
			ಗಾಗಿ (p += lead_repl_len; p < leader + lead_len; ++p)
			    ಆದರೆ (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				ಆದರೆ (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    ಪೂರ್ಣಾಂಕ	    l = (*mb_ptr2len)(p);

				    ಆದರೆ (l > 1)
				    {
					ಆದರೆ (ptr2cells(p) > 1)
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
		    ಆದರೆ (curbuf->b_p_ai || do_si)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
			newindent = get_indent_str(leader,
						   (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

		    // Add the indent offset
		    ಆದರೆ (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    ಆಗ (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			ಆದರೆ (vim_strchr(skipwhite(leader), '\t') != ಖಾಲಿ)
			    ನಿಲ್ಲಿಸು;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    ಆದರೆ (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		ಆದರೆ (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		ಆದರೆ (newindent || did_si)
		{
		    ಆಗ (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (comment_ಕೊನೆ != ಖಾಲಿ)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    ಆದರೆ (comment_ಕೊನೆ[0] == '*' && comment_ಕೊನೆ[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(comment_ಕೊನೆ - saved_ಸಾಲು);
		ಆದರೆ ((pos = findmatch(ಖಾಲಿ, NUL)) != ಖಾಲಿ)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    ಆದರೆ (p_extra != ಖಾಲಿ)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	ಆದರೆ (REPLACE_ಸಾಮಾನ್ಯ(State))
	    replace_ತಳ್ಳು(NUL);	    // end of extra blanks
	ಆದರೆ (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    ಆಗ ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		ಆದರೆ (REPLACE_ಸಾಮಾನ್ಯ(State))
		    replace_ತಳ್ಳು(*p_extra);
		++p_extra;
		++less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (ಪೂರ್ಣಾಂಕ)(p_extra - saved_ಸಾಲು);
    }

    ಆದರೆ (p_extra == ಖಾಲಿ)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    ಆದರೆ (lead_len)
    {
	ಆದರೆ (flags & OPENLINE_COM_LIST && ಎರಡನೇ_ಸಾಲು_indent > 0)
	{
	    ಪೂರ್ಣಾಂಕ i;
	    ಪೂರ್ಣಾಂಕ padding = ಎರಡನೇ_ಸಾಲು_indent
					  - (newindent + (ಪೂರ್ಣಾಂಕ)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    ಗಾಗಿ (i = 0; i < padding; i++)
	    {
		STRCAT(leader, " ");
		less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ--;
		newcol++;
	    }
	}
	STRCAT(leader, p_extra);
	p_extra = leader;
	did_ai = TRUE;	    // So truncating blanks works with comments
	less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ -= lead_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    ಆದರೆ (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    ಆದರೆ (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಆದರೆ (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    ಹೋಗು theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_ವಿಭಜಿಸು(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.ಲಂಬಸಾಲು + 1, 0, at_eol);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	ಆದರೆ (curwin->w_cursor.lnum >= Insstart.lnum + vr_ಸಾಲುಗಳು_ಬದಲಾಯಿತು)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (ಶೂನ್ಯ)u_save_cursor();		    // errors are ignored!
	    vr_ಸಾಲುಗಳು_ಬದಲಾಯಿತು++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    ಆದರೆ (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	ಆದರೆ (did_si)
	{
	    ಪೂರ್ಣಾಂಕ sw = (ಪೂರ್ಣಾಂಕ)get_sw_ಮೌಲ್ಯ(curbuf);

	    ಆದರೆ (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	ಆದರೆ (curbuf->b_p_ci)
	{
	    (ಶೂನ್ಯ)copy_indent(newindent, saved_ಸಾಲು);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    (ಶೂನ್ಯ)ಗಣ_indent(newindent, SIN_ಸೇರಿಸು);
	less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ -= curwin->w_cursor.ಲಂಬಸಾಲು;

	ai_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	ಆದರೆ (REPLACE_ಸಾಮಾನ್ಯ(State))
	    ಗಾಗಿ (n = 0; n < (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು; ++n)
		replace_ತಳ್ಳು(NUL);
	newcol += curwin->w_cursor.ಲಂಬಸಾಲು;
	ಆದರೆ (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    ಆದರೆ (REPLACE_ಸಾಮಾನ್ಯ(State))
	ಆಗ (lead_len-- > 0)
	    replace_ತಳ್ಳು(NUL);

    curwin->w_cursor = old_cursor;

    ಆದರೆ (dir == FORWARD)
    {
	ಆದರೆ (trunc_ಸಾಲು || (State & ಬಹುಲಕ_ಸೇರಿಸು))
	{
	    // truncate current line at cursor
	    saved_ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    ಆದರೆ (trunc_ಸಾಲು && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_ಸಾಲು, curwin->w_cursor.ಲಂಬಸಾಲು);
	    ml_replace(curwin->w_cursor.lnum, saved_ಸಾಲು, FALSE);
	    saved_ಸಾಲು = ಖಾಲಿ;
	    ಆದರೆ (did_append)
	    {
		ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		ಆದರೆ (flags & OPENLINE_MARKFIX)
		    mark_ಲಂಬಸಾಲು_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.ಲಂಬಸಾಲು + less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_off,
						      1L, (ದೀರ್ಘ)-less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		ಆದರೆ (curbuf->b_has_textprop && less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_off, 0);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    ಆದರೆ (did_append)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.ಲಂಬಸಾಲು = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    ಆದರೆ (State & VREPLACE_FLAG)
    {
	vreplace_ಬಹುಲಕ = State;	// So we know to put things right later
	State = ಬಹುಲಕ_ಸೇರಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vreplace_ಬಹುಲಕ = 0;

    ಆದರೆ (!p_paste)
    {
	ಆದರೆ (leader == ಖಾಲಿ
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_ಲಂಬಸಾಲು = (colnr_T)getwhitecols_curline();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	    ai_ಲಂಬಸಾಲು = (colnr_T)getwhitecols_curline();
	}
    }

    ಆದರೆ (vreplace_ಬಹುಲಕ != 0)
	State = vreplace_ಬಹುಲಕ;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    ಆದರೆ (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ಆದರೆ (p_extra == ಖಾಲಿ)
	    ಹೋಗು theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, ಮುಂದಿನ_ಸಾಲು, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	curwin->w_cursor.coladd = 0;
	ins_ಬೈಟುಗಳು(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	ಮುಂದಿನ_ಸಾಲು = ಖಾಲಿ;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_ಸಾಲು);
    vim_free(ಮುಂದಿನ_ಸಾಲು);
    vim_free(allocated);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
truncate_ಸಾಲು(ಪೂರ್ಣಾಂಕ fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    char_u	*old_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		deleted;

    old_ಸಾಲು = ml_get(lnum);
    ಆದರೆ (ಲಂಬಸಾಲು == 0)
	newp = vim_strsave((char_u *)"");
    ಇಲ್ಲದಿದ್ದರೆ
	newp = vim_strnsave(old_ಸಾಲು, ಲಂಬಸಾಲು);
    deleted = (ಪೂರ್ಣಾಂಕ)ml_get_len(lnum) - ಲಂಬಸಾಲು;

    ಆದರೆ (newp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_ಬೈಟುಗಳು(lnum, curwin->w_cursor.ಲಂಬಸಾಲು, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    ಆದರೆ (fixpos && curwin->w_cursor.ಲಂಬಸಾಲು > 0)
	--curwin->w_cursor.ಲಂಬಸಾಲು;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    ಶೂನ್ಯ
del_ಸಾಲುಗಳು(ದೀರ್ಘ nlines, ಪೂರ್ಣಾಂಕ undo)
{
    ದೀರ್ಘ	n;
    linenr_T	ಮೊದಲ = curwin->w_cursor.lnum;

    ಆದರೆ (nlines <= 0)
	ಹಿಂತಿರುಗಿ;

    // save the deleted lines for undo
    ಆದರೆ (undo && u_savedel(ಮೊದಲ, nlines) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (n = 0; n < nlines; )
    {
	ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)	    // nothing to delete
	    ನಿಲ್ಲಿಸು;

	ml_delete_flags(ಮೊದಲ, ML_DEL_ಸಂದೇಶ);
	++n;

	// If we delete the last line in the file, stop
	ಆದರೆ (ಮೊದಲ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_ಸಾಲುಗಳು_mark(ಮೊದಲ, n);
}
