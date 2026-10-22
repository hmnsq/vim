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

#ضمن "vim.h"

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
    فراغ
change_warning(صحيح col)
{
    ساكن حرف *w_readonly = N_("W10: Warning: Changing a readonly file");

    إذا (curbuf->b_did_warn
	    || curbufIsChanged()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	أرجع;

    ++curbuf_lock;
    apply_autocmds(EVENT_FILECHANGEDRO, عدم, عدم, FALSE, curbuf);
    --curbuf_lock;
    إذا (!curbuf->b_p_ro)
	أرجع;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    سكو_start();
    إذا (سكو_row == Rows - 1)
	سكو_col = col;
    سكو_source(HL_ATTR(HLF_W));
    سكو_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | سكو_HIST);
#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#endif
    سكو_clr_eos();
    (فراغ)سكو_end();
    إذا (سكو_silent == 0 && !silent_mode
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
    إذا (سكو_row < Rows - 1)
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
    فراغ
changed(فراغ)
{
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    إذا (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	إذا (im_is_preediting() && !xim_changed_while_preediting)
	    أرجع;
	xim_changed_while_preediting = FALSE;
    }
#endif

    إذا (!curbuf->b_changed)
    {
	صحيح	save_سكو_scroll = سكو_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	إذا (curbuf->b_may_swap && !bt_dontwrite(curbuf))
	{
	    صحيح save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    إذا (need_wait_return && emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		سكو_scroll = save_سكو_scroll;
	    }
	    وإلا
		need_wait_return = save_need_wait_return;
	}
	changed_internal();
    }
    ++CHANGEDTICK(curbuf);

#ifdef FEAT_نيما_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_match = FALSE;
#endif
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    فراغ
changed_internal(فراغ)
{
    curbuf->b_changed = TRUE;
    ml_setflags(curbuf);
    check_status(curbuf);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

#ifdef FEAT_EVAL
// Set when listener callbacks are being invoked.
ساكن صحيح recursive = FALSE;

ساكن long next_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
ساكن bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    ساكن فراغ
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    إذا (prev != عدم)
	prev->lr_next = lnr->lr_next;
    وإلا
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
    ساكن فراغ
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*next;

    prev = عدم;
    لكل (lnr = *list; lnr != عدم; lnr = next)
    {
	next = lnr->lr_next;
	إذا (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	وإلا
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    إذا (buf != عدم)
    {
	إذا (*list == عدم && buf->b_recorded_changes != عدم)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = عدم;
	    buf->b_recorded_text_size = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    ساكن فراغ
perform_listener_housekeeping(فراغ)
{
    buf_T	*buf;

    إذا (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(عدم, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    ساكن فراغ
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	long		xtra)
{
    perform_listener_housekeeping();
    إذا (buf->b_recorded_changes == عدم || xtra == 0)
	أرجع;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_ابوبو(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_لمب(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_لمب(
		li->li_tv.vval.v_dict, "end");
	إذا (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    اكسر;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_TEXT_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    ساكن bool
listeners_want_text(listener_T *list)
{
    listener_T	*lnr;

    لكل (lnr = list; lnr != عدم; lnr = lnr->lr_next)
	إذا (lnr->lr_text)
	    أرجع true;
    أرجع false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    ساكن size_t
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

    إذا (l == عدم)
	أرجع 0;

    إذا (below > buf->b_ml.ml_لي_كدي + 1)
	below = buf->b_ml.ml_لي_كدي + 1;
    لكل (linenr_T lp = lnum < 1 ? 1 : lnum; lp < below; ++lp)
    {
	char_u	*لي = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	إذا (list_append_string(l, لي, len) == FAIL)
	{
	    list_free(l);
	    أرجع 0;
	}
	size += (size_t)len + 1;
    }

    إذا (dict_add_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	أرجع 0;
    }
    أرجع size;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    ساكن فراغ
may_record_change(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    إذا (curbuf->b_listener == عدم)
	أرجع;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    إذا (curbuf->b_recorded_changes == عدم)
    {
	curbuf->b_recorded_changes = list_alloc();
	إذا (curbuf->b_recorded_changes == عدم)  // out of memory
	    أرجع;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    إذا (dict == عدم)
	أرجع;
    dict_add_لمب(dict, "lnum", (varnumber_T)lnum);
    dict_add_لمب(dict, "end", (varnumber_T)lnume);
    dict_add_لمب(dict, "added", (varnumber_T)xtra);
    dict_add_لمب(dict, "col", (varnumber_T)col + 1);
    إذا (listeners_want_text(curbuf->b_listener))
	curbuf->b_recorded_text_size +=
			  add_change_text(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    إذا (!recursive && curbuf->b_recorded_text_size > LISTENER_TEXT_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    فراغ
f_listener_add(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_text = false;

    إذا (check_secure())
	أرجع;

    إذا (recursive)
    {
	emsg(_(e_cannot_add_listener_in_listener_callback));
	أرجع;
    }

    إذا (in_vim9script() && (
	    check_for_opt_buffer_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    callback = get_callback(&argvars[0]);
    إذا (callback.cb_name == عدم)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	إذا (buf == عدم)
	{
	    free_callback(&callback);
	    أرجع;
	}
	إذا (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_text = dict_get_bool(d, "text", false);
	}
	وإلا إذا (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    إذا (lnr == عدم)
    {
	free_callback(&callback);
	أرجع;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    إذا (unbuffered)
    {
	lnr->lr_next = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    وإلا
    {
	lnr->lr_next = buf->b_listener;
	buf->b_listener = lnr;
    }

    set_callback(&lnr->lr_callback, &callback);

    lnr->lr_text = want_text;
    lnr->lr_id = ++next_listener_id;
    rettv->vval.v_لمب = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    فراغ
f_listener_flush(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf = curbuf;

    إذا (check_secure())
	أرجع;

    إذا (recursive)
	أرجع;

    إذا (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	إذا (buf == عدم)
	    أرجع;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    ساكن listener_T *
find_listener(
    صحيح        id,
    listener_T *list_start,
    listener_T **prev)
{
    listener_T *next;
    listener_T *lnr;

    *prev = عدم;
    لكل (lnr = list_start; lnr != عدم; lnr = next)
    {
	next = lnr->lr_next;
	إذا (lnr->lr_id == id)
	    أرجع lnr;
	*prev = lnr;
    }
    أرجع عدم;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    فراغ
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    صحيح		id;
    buf_T	*buf;

    إذا (check_secure())
	أرجع;

    إذا (in_vim9script() && check_for_لمب_arg(argvars, 0) == FAIL)
	أرجع;

    id = tv_get_لمب(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = find_listener(id, buf->b_listener, &prev);
	إذا (lnr == عدم)
	    lnr = find_listener(id, buf->b_sync_listener, &prev);
	إذا (lnr != عدم)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_لمب = 1;
	    أرجع;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    فراغ
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, صحيح added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    ساكن فراغ
invoke_listener_set(
    buf_T       *buf,
    linenr_T	start,
    linenr_T	end,
    long	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    صحيح		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_لمب;
    argv[0].vval.v_لمب = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_لمب;
    argv[1].vval.v_لمب = start;
    argv[2].v_type = VAR_لمب;
    argv[2].vval.v_لمب = end;
    argv[3].v_type = VAR_لمب;
    argv[3].vval.v_لمب = added;
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

    لكل (lnr = listeners; lnr != عدم; lnr = lnr->lr_next)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    إذا (save_updating_screen)
	updating_screen = TRUE;
    وإلا
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    ساكن فراغ
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	start,
    colnr_T	col,
    linenr_T	end,
    long	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    إذا (recursive || curbuf->b_sync_listener == عدم)
	أرجع;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    إذا (recorded_changes == عدم)  // out of memory
	أرجع;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    إذا (dict == عدم)
    {
	list_unref(recorded_changes);
	أرجع;
    }

    dict_add_لمب(dict, "lnum", (varnumber_T)start);
    dict_add_لمب(dict, "end", (varnumber_T)end);
    dict_add_لمب(dict, "added", (varnumber_T)added);
    dict_add_لمب(dict, "col", (varnumber_T)col + 1);
    إذا (listeners_want_text(buf->b_sync_listener))
	(فراغ)add_change_text(buf, dict, start, end, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_set(
	buf, start, end, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    فراغ
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	start = MAXLNUM;
    linenr_T	end = 0;
    linenr_T	added = 0;

    إذا (buf->b_recorded_changes == عدم  // nothing changed
	    || buf->b_listener == عدم	 // no listeners
	    || recursive)		 // already busy
	أرجع;

    FOR_ALL_LIST_ابوبو(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_لمب(li->li_tv.vval.v_dict, "lnum");
	إذا (start > lnum)
	    start = lnum;
	lnum = dict_get_لمب(li->li_tv.vval.v_dict, "end");
	إذا (end < lnum)
	    end = lnum;
	added += dict_get_لمب(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_set(
	buf, start, end, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = عدم;
    buf->b_recorded_text_size = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    فراغ
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(عدم, &buf->b_sync_listener, TRUE);
}

#endif

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ساكن فراغ
changed_common(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    صحيح		i;
    صحيح		cols;
    pos_T	*p;
    صحيح		add;

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
    إذا (curwin->w_p_diff && diff_internal())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_لي(lnum);
    }
#endif

    // set the '. mark
    إذا ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_last_change.lnum = lnum;
	curbuf->b_last_change.col = col;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	إذا (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    إذا (curbuf->b_changelistlen == 0)
		add = TRUE;
	    وإلا
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		إذا (p->lnum != lnum)
		    add = TRUE;
		وإلا
		{
		    cols = comp_textwidth(FALSE);
		    إذا (cols == 0)
			cols = 79;
		    add = (p->col + cols < col || col + cols < p->col);
		}
	    }
	    إذا (add)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		إذا (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  حجم(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			إذا (wp->w_buffer == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    إذا (wp->w_buffer == curbuf
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

    إذا (VIsual_active)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_buffer == curbuf)
	{
	    linenr_T last = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    إذا (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    إذا (xtra != 0 && wp->w_redraw_كولي != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    إذا (wp->w_skipcol > 0
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
	    i = hasFoldingWin(wp, lnum, &lnum, عدم, FALSE, عدم);
	    إذا (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, last, عدم, &last, FALSE, عدم);
	    إذا (wp->w_cursor.lnum == last)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    إذا (wp->w_cursor.lnum <= lnum)
	    {
		i = find_wl_entry(wp, lnum);
		إذا (i >= 0 && wp->w_cursor.lnum > wp->w_ليوك[i].wl_lnum)
		    changed_لي_abv_curs_win(wp);
	    }
#endif
	    إذا (wp->w_cursor.lnum > lnum)
		changed_لي_abv_curs_win(wp);
	    وإلا إذا (wp->w_cursor.lnum == lnum && wp->w_cursor.col >= col)
		changed_cline_bef_curs_win(wp);
	    إذا (wp->w_botline >= lnum)
	    {
		إذا (xtra < 0)
		    invalidate_botline_win(wp);
		وإلا
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    لكل (i = 0; i < wp->w_ليوك_valid; ++i)
		إذا (wp->w_ليوك[i].wl_valid)
		{
		    إذا (wp->w_ليوك[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			إذا (wp->w_ليوك[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_ليوك[i].wl_valid = FALSE;
			}
			وإلا إذا (xtra != 0)
			{
			    // line below change
			    wp->w_ليوك[i].wl_lnum += xtra;
#ifdef FEAT_FOLDING
			    wp->w_ليوك[i].wl_lastlnum += xtra;
#endif
			}
		    }
#ifdef FEAT_FOLDING
		    وإلا إذا (wp->w_ليوك[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_ليوك[i].wl_valid = FALSE;
		    }
#endif
		}

#ifdef FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    إذا (hasAnyFolding(wp))
		set_topline(wp, wp->w_topline);
#endif
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    إذا (wp->w_p_rnu && xtra != 0)
		wp->w_last_cursor_lnum_rnu = 0;

#ifdef FEAT_SYN_HL
	    إذا (wp->w_p_cul && wp->w_last_cursorline >= lnum)
	    {
		إذا (wp->w_last_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_last_cursorline = 0;
		وإلا
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_last_cursorline += xtra;
	    }
#endif
	}
#ifdef FEAT_نيما_EXTRA
	إذا (wp == curwin && xtra != 0 && نيما_hl_has_cursor_lnum >= lnum)
	    نيما_hl_has_cursor_lnum += xtra;
#endif
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    set_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    إذا (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	last_cursormoved.lnum = 0;
}

    ساكن فراغ
changedOneline(buf_T *buf, linenr_T lnum)
{
    إذا (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	إذا (lnum < buf->b_mod_كولي)
	    buf->b_mod_كولي = lnum;
	وإلا إذا (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    وإلا
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_set = true;
	buf->b_mod_كولي = lnum;
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
    فراغ
changed_bytes(linenr_T lnum, colnr_T col)
{
    changedOneline(curbuf, lnum);
    changed_common(lnum, col, lnum + 1, 0L);

#ifdef FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    إذا (spell_check_window(curwin)
	    && lnum < curbuf->b_ml.ml_لي_كدي
	    && vim_strchr(p_cpo, CPO_DOLLAR) == عدم)
	redrawWinline(curwin, lnum + 1);
#endif
#ifdef FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    إذا (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		إذا (wlnum > 0)
		    changedOneline(wp->w_buffer, wlnum);
	    }
    }
#endif
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    فراغ
inserted_bytes(linenr_T lnum, colnr_T col, صحيح added UNUSED)
{
#ifdef FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, col, added, 0);
#endif

    changed_bytes(lnum, col);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    فراغ
appended_ليوك(linenr_T lnum, long كدي)
{
    changed_ليوك(lnum + 1, 0, lnum + 1, كدي);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    فراغ
appended_ليوك_mark(linenr_T lnum, long كدي)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, كدي, 0L);
    changed_ليوك(lnum + 1, 0, lnum + 1, كدي);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    فراغ
deleted_ليوك(linenr_T lnum, long كدي)
{
    changed_ليوك(lnum, 0, lnum + كدي, -كدي);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    فراغ
deleted_ليوك_mark(linenr_T lnum, long كدي)
{
    mark_adjust(lnum, (linenr_T)(lnum + كدي - 1), (long)MAXLNUM, -كدي);
    changed_ليوك(lnum, 0, lnum + كدي, -كدي);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    فراغ
changed_ليوك_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    إذا (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	إذا (lnum < buf->b_mod_كولي)
	    buf->b_mod_كولي = lnum;
	إذا (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    إذا (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	إذا (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    وإلا
    {
	// set the area that must be redisplayed
	buf->b_mod_set = true;
	buf->b_mod_كولي = lnum;
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
    فراغ
changed_ليوك(
    linenr_T	lnum,	    // first line with change
    colnr_T	col,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    changed_ليوك_buf(curbuf, lnum, lnume, xtra);

#ifdef FEAT_DIFF
    إذا (xtra == 0 && curwin->w_p_diff && !diff_internal())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		إذا (wlnum > 0)
		    changed_ليوك_buf(wp->w_buffer, wlnum,
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
    فراغ
unchanged(buf_T *buf, صحيح ff, صحيح always_inc_changedtick)
{
    إذا (buf->b_changed || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_changed = 0;
	ml_setflags(buf);
	إذا (ff)
	    save_file_ff(buf);
	check_status(buf);
	redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    وإلا إذا (always_inc_changedtick)
	++CHANGEDTICK(buf);
#ifdef FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#endif
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    فراغ
save_file_ff(buf_T *buf)
{
    buf->b_start_ffc = *buf->b_p_ff;
    buf->b_start_eof = buf->b_p_eof;
    buf->b_start_eol = buf->b_p_eol;
    buf->b_start_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    إذا (buf->b_start_fenc == عدم
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
    صحيح
file_ff_differs(buf_T *buf, صحيح ignore_empty)
{
    // In a buffer that was never loaded the options are not valid.
    إذا (buf->b_flags & BF_NEVERLOADED)
	أرجع FALSE;
    إذا (ignore_empty
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_لي_كدي == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	أرجع FALSE;
    إذا (buf->b_start_ffc != *buf->b_p_ff)
	أرجع TRUE;
    إذا ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_start_eof != buf->b_p_eof
		|| buf->b_start_eol != buf->b_p_eol))
	أرجع TRUE;
    إذا (!buf->b_p_bin && buf->b_start_bomb != buf->b_p_bomb)
	أرجع TRUE;
    إذا (buf->b_start_fenc == عدم)
	أرجع (*buf->b_p_fenc != NUL);
    أرجع (STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    فراغ
ins_bytes(char_u *p)
{
    ins_bytes_len(p, (صحيح)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    فراغ
ins_bytes_len(char_u *p, صحيح len)
{
    صحيح		i;
    صحيح		n;

    إذا (has_mbyte)
	لكل (i = 0; i < len; i += n)
	{
	    إذا (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    وإلا
		n = (*mb_ptr2len)(p + i);
	    ins_char_bytes(p + i, n);
	}
    وإلا
	لكل (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    فراغ
ins_char(صحيح c)
{
    char_u	buf[MB_MAXBYTES + 1];
    صحيح		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    إذا (buf[0] == 0)
	buf[0] = '\n';

    ins_char_bytes(buf, n);
}

    فراغ
ins_char_bytes(char_u *buf, صحيح charlen)
{
    صحيح		c = buf[0];
    صحيح		newlen;		// nr of bytes inserted
    صحيح		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    صحيح		linelen;	// length of old line including NUL
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;
    صحيح		i;

    // Break tabs if needed.
    إذا (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    linelen = (صحيح)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    إذا (State & REPLACE_FLAG)
    {
	إذا (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    صحيح		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    إذا (old_list && vim_strchr(p_cpo, CPO_LISTWM) == عدم)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, عدم, &vcol, عدم, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    طالما (oldp[col + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + col + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		إذا (vcol > new_vcol && oldp[col + oldlen] == TAB)
		    اكسر;
		oldlen += (*mb_ptr2len)(oldp + col + oldlen);
		// Deleted a bit too much, insert spaces.
		إذا (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	وإلا إذا (oldp[col] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + col);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_تور(NUL);
	لكل (i = 0; i < oldlen; ++i)
	{
	    إذا (has_mbyte)
		i += replace_تور_mb(oldp + col + i) - 1;
	    وإلا
		replace_تور(oldp[col + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    إذا (newp == عدم)
	أرجع;

    // Copy bytes before the cursor.
    إذا (col > 0)
	mch_memmove(newp, oldp, (size_t)col);

    // Copy bytes after the changed character(s).
    p = newp + col;
    إذا (linelen > col + oldlen)
	mch_memmove(p + newlen, oldp + col + oldlen,
					    (size_t)(linelen - col - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    طالما (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    changed_bytes(lnum, col);
#ifdef FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, col, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#endif

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    إذا (p_sm && (State & MODE_سك)
	    && سكو_silent == 0
	    && !ins_compl_active())
    {
	إذا (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	وإلا
	    showmatch(c);
    }

#ifdef FEAT_RIGHTLEFT
    إذا (!p_ri || (State & REPLACE_FLAG))
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
    فراغ
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    صحيح		oldlen;
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;

    إذا (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    oldlen = (صحيح)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    إذا (newp == عدم)
	أرجع;
    إذا (col > 0)
	mch_memmove(newp, oldp, (size_t)col);
    mch_memmove(newp + col, s, slen);
    mch_memmove(newp + col + slen, oldp + col, (size_t)(oldlen - col + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_bytes(lnum, col, (صحيح)slen);
    curwin->w_cursor.col += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
del_char(صحيح fixpos)
{
    إذا (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	إذا (*ml_get_cursor() == NUL)
	    أرجع FAIL;
	أرجع del_chars(1L, fixpos);
    }
    أرجع del_bytes(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    صحيح
del_chars(long كدي, صحيح fixpos)
{
    long	bytes = 0;
    long	i;
    char_u	*p;
    صحيح		l;

    p = ml_get_cursor();
    لكل (i = 0; i < كدي && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	bytes += l;
	p += l;
    }
    أرجع del_bytes(bytes, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
del_bytes(
    long	كدي,
    صحيح		fixpos_arg,
    صحيح		use_delcombine UNUSED)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    صحيح		alloc_newp;
    long	movelen;
    صحيح		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (صحيح)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    إذا (col >= oldlen)
	أرجع FAIL;

    // If "count" is zero there is nothing to do.
    إذا (كدي == 0)
	أرجع OK;

    // If "count" is negative the caller must be doing something wrong.
    إذا (كدي < 1)
    {
	siemsg(e_invalid_كدي_for_del_bytes_nr, كدي);
	أرجع FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    إذا (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + col) >= كدي)
    {
	صحيح	cc[MAX_MCO];
	صحيح	n;

	(فراغ)utfc_ptr2char(oldp + col, cc);
	إذا (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = col;
	    افعل
	    {
		col = n;
		كدي = utf_ptr2len(oldp + n);
		n += كدي;
	    } طالما (UTF_COMPOSINGLIKE(oldp + col, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (long)oldlen - (long)col - كدي + 1; // includes trailing NUL
    إذا (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	إذا (col > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.coladd = 0;
	    إذا (has_mbyte)
		curwin->w_cursor.col -=
			    (*mb_head_off)(oldp, oldp + curwin->w_cursor.col);
	}
	كدي = oldlen - col;
	movelen = 1;
    }
    newlen = oldlen - كدي;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#ifdef FEAT_NETBEANS_INTG
    إذا (netbeans_active())
	alloc_newp = TRUE;
    وإلا
#endif
	alloc_newp = !ml_لي_alloced();    // check if oldp was allocated
    إذا (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    وإلا
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	إذا (newp == عدم)
	    أرجع FAIL;
	mch_memmove(newp, oldp, (size_t)col);
    }
    mch_memmove(newp + col, oldp + col + كدي, (size_t)movelen);
    إذا (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    وإلا
    {
#ifdef FEAT_PROP_POPUP
	// Also move any following text properties.
	إذا (oldlen + 1 < curbuf->b_ml.ml_لي_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_لي_len - oldlen - 1);
#endif
	curbuf->b_ml.ml_لي_len -= كدي;
	curbuf->b_ml.ml_لي_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, col, -كدي);

    أرجع OK;
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
    صحيح
open_لي(
    صحيح		dir,		// FORWARD or BACKWARD
    صحيح		flags,
    صحيح		second_لي_indent,
    صحيح		*did_do_comment UNUSED)
{
    char_u	*saved_لي;		// copy of the original line
    char_u	*next_لي = عدم;	// copy of the next line
    char_u	*p_extra = عدم;	// what goes to next line
    صحيح		less_cols = 0;		// less columns for mark in new line
    صحيح		less_cols_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    صحيح		newcol = 0;		// new cursor column
    صحيح		newindent = 0;		// auto-indent of the new line
    صحيح		n;
    صحيح		trunc_لي = FALSE;	// truncate current line afterwards
    صحيح		retval = FAIL;		// return value
    صحيح		extra_len = 0;		// length of p_extra string
    صحيح		lead_len;		// length of comment leader
    صحيح		comment_start = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = عدم;		// copy of comment leader
    char_u	*allocated = عدم;	// allocated memory
    char_u	*p;
    صحيح		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    صحيح		do_cindent;
    صحيح		do_si = may_do_si();
    صحيح		no_si = FALSE;		// reset did_si afterwards
    صحيح		first_char = NUL;	// init for GCC
    صحيح		vreplace_mode;
    صحيح		did_append;		// appended a new line
    صحيح		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#ifdef FEAT_PROP_POPUP
    صحيح		at_eol;			// cursor after last character
#endif

    // make a copy of the current line so we can mess with it
    saved_لي = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    إذا (saved_لي == عدم)	    // out of memory!
	أرجع FALSE;

#ifdef FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.col >= (صحيح)ml_get_curline_len();
#endif

    إذا (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	إذا (curwin->w_cursor.lnum < orig_لي_كدي)
	    next_لي = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	وإلا
	    next_لي = vim_strsave((char_u *)"");
	إذا (next_لي == عدم)	    // out of memory!
	    اذهب theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_تور(NUL);  // Call twice because BS over NL expects it
	replace_تور(NUL);
	p = saved_لي + curwin->w_cursor.col;
	طالما (*p != NUL)
	{
	    إذا (has_mbyte)
		p += replace_تور_mb(p);
	    وإلا
		replace_تور(*p++);
	}
	saved_لي[curwin->w_cursor.col] = NUL;
    }

    إذا ((State & MODE_سك) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_لي + curwin->w_cursor.col;
	إذا (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    first_char = *p;
	}
	extra_len = (صحيح)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_col = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    إذا (dir == FORWARD && did_ai)
	trunc_لي = TRUE;

    إذا ((flags & OPENLINE_FORCE_INDENT) && second_لي_indent >= 0)
	newindent = second_لي_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    وإلا إذا (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#ifdef FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_لي, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وإلا
	newindent = get_indent_str(saved_لي, (صحيح)curbuf->b_p_ts, FALSE);
#endif
	إذا (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = second_لي_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	إذا (!trunc_لي && do_si && *saved_لي != NUL
				    && (p_extra == عدم || first_char != '{'))
	{
	    char_u  *ptr;
	    char_u  last_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_لي;
	    إذا (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, عدم, FALSE, TRUE);
	    وإلا
		lead_len = 0;
	    إذا (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		إذا ( lead_len == 0 && ptr[0] == '#')
		{
		    طالما (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		إذا (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, عدم, FALSE, TRUE);
		وإلا
		    lead_len = 0;
		إذا (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    إذا (p[0] == '/' && p[1] == '*')
			p++;
		    إذا (p[0] == '*')
		    {
			لكل (p++; *p; p++)
			{
			    إذا (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.col = (colnr_T)(p - ptr);
				إذا ((pos = findmatch(عدم, NUL)) != عدم)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    اكسر;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.col;
			    }
			}
		    }
		}
		وإلا	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    طالما (p > ptr && VIM_ISWHITE(*p))
			--p;
		    last_char = *p;

		    // find the character just before the '{' or ';'
		    إذا (last_char == '{' || last_char == ';')
		    {
			إذا (p > ptr)
			    --p;
			طالما (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    إذا (*p == ')')
		    {
			curwin->w_cursor.col = (colnr_T)(p - ptr);
			إذا ((pos = findmatch(عدم, '(')) != عدم)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    إذا (last_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    وإلا إذا (last_char != ';' && last_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    وإلا // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		إذا (lead_len == 0 && ptr[0] == '#')
		{
		    صحيح was_backslashed = FALSE;

		    طالما ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_لي_كدي)
		    {
			إذا (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			وإلا
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    إذا (was_backslashed)
			newindent = 0;	    // Got to end of file
		    وإلا
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		إذا (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		وإلا		    // can delete indent when '{' typed
		    can_si_back = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	إذا (do_si)
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
		? مبودي_OPEN_FORW
		: مبودي_OPEN_BACK, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    end_comment_pending = NUL;
    إذا (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_لي, &lead_flags,
							dir == BACKWARD, TRUE);
	إذا (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_start = check_linecomment(saved_لي);
	    إذا (comment_start != MAXCOL)
	    {
		lead_len = get_leader_len(saved_لي + comment_start,
						     &lead_flags, FALSE, TRUE);
		إذا (lead_len != 0)
		{
		    lead_len += comment_start;
		    إذا (did_do_comment != عدم)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    وإلا
	lead_len = 0;
    إذا (lead_len > 0)
    {
	char_u	*lead_repl = عدم;	    // replaces comment leader
	صحيح	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_ڟكيا[COM_MAX_LEN];   // middle-comment string
	صحيح	lead_ڟكيا_len;	    // length of the lead_middle
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_end = عدم;	    // where lead_end has been found
	صحيح	extra_space = FALSE;	    // append extra space
	صحيح	current_flag;
	صحيح	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	لكل (p = lead_flags; *p && *p != ':'; ++p)
	{
	    إذا (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		استمر;
	    }
	    إذا (*p == COM_START || *p == COM_ڟكيا)
	    {
		current_flag = *p;
		إذا (*p == COM_START)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    إذا (dir == BACKWARD)
		    {
			lead_len = 0;
			اكسر;
		    }

		    // find start of middle part
		    (فراغ)copy_option_part(&p, lead_ڟكيا, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		طالما (*p && p[-1] != ':')	// find end of middle flags
		{
		    إذا (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_ڟكيا_len = copy_option_part(&p, lead_ڟكيا, COM_MAX_LEN, ",");

		طالما (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    إذا (*p == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

		إذا (end_comment_pending == -1)	// we can set it now
		    end_comment_pending = lead_end[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		إذا (dir == FORWARD)
		{
		    لكل (p = saved_لي + lead_len; *p; ++p)
			إذا (STRNCMP(p, lead_end, n) == 0)
			{
			    comment_end = p;
			    lead_len = 0;
			    اكسر;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		إذا (lead_len > 0)
		{
		    إذا (current_flag == COM_START)
		    {
			lead_repl = lead_ڟكيا;
			lead_repl_len = lead_ڟكيا_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    إذا (!VIM_ISWHITE(saved_لي[lead_len - 1])
			    && ((p_extra != عدم
				    && (صحيح)curwin->w_cursor.col == lead_len)
				|| (p_extra == عدم
				    && saved_لي[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		اكسر;
	    }
	    إذا (*p == COM_END)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		إذا (dir == FORWARD)
		{
		    comment_end = skipwhite(saved_لي);
		    lead_len = 0;
		    اكسر;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		طالما (p > curbuf->b_p_com && *p != ',')
		    --p;
		لكل (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (صحيح)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		لكل (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    إذا (*p2 == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		}
		إذا (end_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    طالما (*p2 && *p2 != ',')
			p2++;
		    end_comment_pending = p2[-1];
		}
		اكسر;
	    }
	    إذا (*p == COM_FIRST)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		إذا (dir == BACKWARD)
		    lead_len = 0;
		وإلا
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		اكسر;
	    }
	}
	إذا (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (second_لي_indent > 0 ? second_لي_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    إذا (leader == عدم)
		lead_len = 0;
	    وإلا
	    {
		صحيح li;

		vim_strncpy(leader, saved_لي, lead_len);

		// TODO: handle multi-byte and double width chars
		لكل (li = 0; li < comment_start; ++li)
		    إذا (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		إذا (lead_repl != عدم)
		{
		    صحيح		c = 0;
		    صحيح		off = 0;

		    لكل (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			إذا (*p == COM_دام || *p == COM_هغو)
			    c = *p++;
			وإلا إذا (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			وإلا
			    ++p;
		    }
		    إذا (c == COM_دام)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			لكل (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    صحيح	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    صحيح	    old_size = 0;
			    char_u  *endp = p;
			    صحيح	    l;

			    طالما (old_size < repl_size && p > leader)
			    {
				MB_PTR_BACK(leader, p);
				old_size += ptr2cells(p);
			    }
			    l = lead_repl_len - (صحيح)(endp - p);
			    إذا (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			إذا (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			طالما (--p >= leader)
			{
			    صحيح l = mb_head_off(leader, p);

			    إذا (l > 1)
			    {
				p -= l;
				إذا (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    وإلا إذا (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    وإلا	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    صحيح	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    صحيح	    i;
			    صحيح	    l;

			    لكل (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				إذا (vim_strnsize(p, i + l) > repl_size)
				    اكسر;
			    }
			    إذا (i != lead_repl_len)
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
			لكل (p += lead_repl_len; p < leader + lead_len; ++p)
			    إذا (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				إذا (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				وإلا
				{
				    صحيح	    l = (*mb_ptr2len)(p);

				    إذا (l > 1)
				    {
					إذا (ptr2cells(p) > 1)
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
		    إذا (curbuf->b_p_ai || do_si)
#ifdef FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وإلا
			newindent = get_indent_str(leader,
						   (صحيح)curbuf->b_p_ts, FALSE);
#endif

		    // Add the indent offset
		    إذا (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    وإلا
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    طالما (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			إذا (vim_strchr(skipwhite(leader), '\t') != عدم)
			    اكسر;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    إذا (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		إذا (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		إذا (newindent || did_si)
		{
		    طالما (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	وإلا إذا (comment_end != عدم)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    إذا (comment_end[0] == '*' && comment_end[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.col = (colnr_T)(comment_end - saved_لي);
		إذا ((pos = findmatch(عدم, NUL)) != عدم)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    إذا (p_extra != عدم)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	إذا (REPLACE_NORMAL(State))
	    replace_تور(NUL);	    // end of extra blanks
	إذا (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    طالما ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		إذا (REPLACE_NORMAL(State))
		    replace_تور(*p_extra);
		++p_extra;
		++less_cols_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_cols = (صحيح)(p_extra - saved_لي);
    }

    إذا (p_extra == عدم)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    إذا (lead_len)
    {
	إذا (flags & OPENLINE_COM_LIST && second_لي_indent > 0)
	{
	    صحيح i;
	    صحيح padding = second_لي_indent
					  - (newindent + (صحيح)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    لكل (i = 0; i < padding; i++)
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
    وإلا
	end_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    إذا (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    إذا (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_لي_كدي)
    {
	إذا (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    اذهب theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#ifdef FEAT_PROP_POPUP
	إذا ((State & MODE_سك) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_split(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.col + 1, 0, at_eol);
#endif
    }
    وإلا
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	إذا (curwin->w_cursor.lnum >= Insstart.lnum + vr_ليوك_changed)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (فراغ)u_save_cursor();		    // errors are ignored!
	    vr_ليوك_changed++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	changed_bytes(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    إذا (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	إذا (did_si)
	{
	    صحيح sw = (صحيح)get_sw_درج(curbuf);

	    إذا (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	إذا (curbuf->b_p_ci)
	{
	    (فراغ)copy_indent(newindent, saved_لي);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	وإلا
	    (فراغ)set_indent(newindent, SIN_سك);
	less_cols -= curwin->w_cursor.col;

	ai_col = curwin->w_cursor.col;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	إذا (REPLACE_NORMAL(State))
	    لكل (n = 0; n < (صحيح)curwin->w_cursor.col; ++n)
		replace_تور(NUL);
	newcol += curwin->w_cursor.col;
	إذا (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    إذا (REPLACE_NORMAL(State))
	طالما (lead_len-- > 0)
	    replace_تور(NUL);

    curwin->w_cursor = old_cursor;

    إذا (dir == FORWARD)
    {
	إذا (trunc_لي || (State & MODE_سك))
	{
	    // truncate current line at cursor
	    saved_لي[curwin->w_cursor.col] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    إذا (trunc_لي && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_لي, curwin->w_cursor.col);
	    ml_replace(curwin->w_cursor.lnum, saved_لي, FALSE);
	    saved_لي = عدم;
	    إذا (did_append)
	    {
		changed_ليوك(curwin->w_cursor.lnum, curwin->w_cursor.col,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		إذا (flags & OPENLINE_MARKFIX)
		    mark_col_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.col + less_cols_off,
						      1L, (long)-less_cols, 0);
#ifdef FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		إذا (curbuf->b_has_textprop && less_cols_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_cols_off, 0);
#endif
	    }
	    وإلا
		changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    إذا (did_append)
	changed_ليوك(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.col = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    إذا (State & VREPLACE_FLAG)
    {
	vreplace_mode = State;	// So we know to put things right later
	State = MODE_سك;
    }
    وإلا
	vreplace_mode = 0;

    إذا (!p_paste)
    {
	إذا (leader == عدم
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_col = (colnr_T)getwhitecols_curline();
	}
	وإلا إذا (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_فرچ_indent();
	    ai_col = (colnr_T)getwhitecols_curline();
	}
    }

    إذا (vreplace_mode != 0)
	State = vreplace_mode;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    إذا (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	إذا (p_extra == عدم)
	    اذهب theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, next_لي, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;
	ins_bytes(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	next_لي = عدم;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_لي);
    vim_free(next_لي);
    vim_free(allocated);
    أرجع retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
truncate_لي(صحيح fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    char_u	*old_لي;
    صحيح		deleted;

    old_لي = ml_get(lnum);
    إذا (col == 0)
	newp = vim_strsave((char_u *)"");
    وإلا
	newp = vim_strnsave(old_لي, col);
    deleted = (صحيح)ml_get_len(lnum) - col;

    إذا (newp == عدم)
	أرجع FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, curwin->w_cursor.col, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    إذا (fixpos && curwin->w_cursor.col > 0)
	--curwin->w_cursor.col;

    أرجع OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    فراغ
del_ليوك(long nlines, صحيح undo)
{
    long	n;
    linenr_T	first = curwin->w_cursor.lnum;

    إذا (nlines <= 0)
	أرجع;

    // save the deleted lines for undo
    إذا (undo && u_savedel(first, nlines) == FAIL)
	أرجع;

    لكل (n = 0; n < nlines; )
    {
	إذا (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    اكسر;

	ml_delete_flags(first, ML_DEL_MESSAGE);
	++n;

	// If we delete the last line in the file, stop
	إذا (first > curbuf->b_ml.ml_لي_كدي)
	    اكسر;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.col = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_ليوك_mark(first, n);
}
