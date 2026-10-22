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

#include "vim.h"

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
    void
change_warning(ຈຳນວນເຕັມ col)
{
    static var_na *w_readonly = N_("W10: Warning: Changing a readonly file");

    ຖ້າ (curbuf->b_did_warn
	    || curbufIsChanged()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	ສົ່ງຄືນ;

    ++curbuf_lock;
    apply_autocmds(EVENT_FILECHANGEDRO, NULL, NULL, FALSE, curbuf);
    --curbuf_lock;
    ຖ້າ (!curbuf->b_p_ro)
	ສົ່ງຄືນ;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    msg_start();
    ຖ້າ (msg_row == Rows - 1)
	msg_col = col;
    msg_source(HL_ATTR(HLF_W));
    msg_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | MSG_HIST);
#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#endif
    msg_clr_eos();
    (void)msg_end();
    ຖ້າ (msg_silent == 0 && !silent_mode
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
    ຖ້າ (msg_row < Rows - 1)
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
    void
changed(void)
{
#ຖ້າ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ຖ້າ (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	ຖ້າ (im_is_preediting() && !xim_changed_while_preediting)
	    ສົ່ງຄືນ;
	xim_changed_while_preediting = FALSE;
    }
#endif

    ຖ້າ (!curbuf->b_changed)
    {
	ຈຳນວນເຕັມ	save_msg_scroll = msg_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	ຖ້າ (curbuf->b_may_swap && !bt_dontwrite(curbuf))
	{
	    ຈຳນວນເຕັມ save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    ຖ້າ (need_wait_return && emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		msg_scroll = save_msg_scroll;
	    }
	    ບໍ່ດັ່ງນັ້ນ
		need_wait_return = save_need_wait_return;
	}
	changed_internal();
    }
    ++CHANGEDTICK(curbuf);

#ifdef FEAT_SEARCH_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_match = FALSE;
#endif
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    void
changed_internal(void)
{
    curbuf->b_changed = TRUE;
    ml_setflags(curbuf);
    check_status(curbuf);
    redraw_tabline = TRUE;
#ຖ້າ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

#ifdef FEAT_EVAL
// Set when listener callbacks are being invoked.
static ຈຳນວນເຕັມ recursive = FALSE;

static long next_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
static bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    static void
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    ຖ້າ (prev != NULL)
	prev->lr_next = lnr->lr_next;
    ບໍ່ດັ່ງນັ້ນ
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
    static void
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*next;

    prev = NULL;
    ສຳລັບ (lnr = *list; lnr != NULL; lnr = next)
    {
	next = lnr->lr_next;
	ຖ້າ (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	ບໍ່ດັ່ງນັ້ນ
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    ຖ້າ (buf != NULL)
    {
	ຖ້າ (*list == NULL && buf->b_recorded_changes != NULL)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = NULL;
	    buf->b_recorded_text_size = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    static void
perform_listener_housekeeping(void)
{
    buf_T	*buf;

    ຖ້າ (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(NULL, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    static void
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	long		xtra)
{
    perform_listener_housekeeping();
    ຖ້າ (buf->b_recorded_changes == NULL || xtra == 0)
	ສົ່ງຄືນ;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_ITEMS(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_number(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_number(
		li->li_tv.vval.v_dict, "end");
	ຖ້າ (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    bha_nga;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_TEXT_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    static bool
listeners_want_text(listener_T *list)
{
    listener_T	*lnr;

    ສຳລັບ (lnr = list; lnr != NULL; lnr = lnr->lr_next)
	ຖ້າ (lnr->lr_text)
	    ສົ່ງຄືນ true;
    ສົ່ງຄືນ false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    static size_t
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

    ຖ້າ (l == NULL)
	ສົ່ງຄືນ 0;

    ຖ້າ (below > buf->b_ml.ml_line_count + 1)
	below = buf->b_ml.ml_line_count + 1;
    ສຳລັບ (linenr_T lp = lnum < 1 ? 1 : lnum; lp < below; ++lp)
    {
	char_u	*line = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	ຖ້າ (list_append_string(l, line, len) == FAIL)
	{
	    list_free(l);
	    ສົ່ງຄືນ 0;
	}
	size += (size_t)len + 1;
    }

    ຖ້າ (dict_add_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	ສົ່ງຄືນ 0;
    }
    ສົ່ງຄືນ size;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    static void
may_record_change(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    ຖ້າ (curbuf->b_listener == NULL)
	ສົ່ງຄືນ;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    ຖ້າ (curbuf->b_recorded_changes == NULL)
    {
	curbuf->b_recorded_changes = list_alloc();
	ຖ້າ (curbuf->b_recorded_changes == NULL)  // out of memory
	    ສົ່ງຄືນ;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    ຖ້າ (dict == NULL)
	ສົ່ງຄືນ;
    dict_add_number(dict, "lnum", (varnumber_T)lnum);
    dict_add_number(dict, "end", (varnumber_T)lnume);
    dict_add_number(dict, "added", (varnumber_T)xtra);
    dict_add_number(dict, "col", (varnumber_T)col + 1);
    ຖ້າ (listeners_want_text(curbuf->b_listener))
	curbuf->b_recorded_text_size +=
			  add_change_text(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    ຖ້າ (!recursive && curbuf->b_recorded_text_size > LISTENER_TEXT_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    void
f_listener_add(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_text = false;

    ຖ້າ (check_secure())
	ສົ່ງຄືນ;

    ຖ້າ (recursive)
    {
	emsg(_(e_cannot_add_listener_in_listener_callback));
	ສົ່ງຄືນ;
    }

    ຖ້າ (in_vim9script() && (
	    check_for_opt_buffer_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	ສົ່ງຄືນ;

    callback = get_callback(&argvars[0]);
    ຖ້າ (callback.cb_name == NULL)
	ສົ່ງຄືນ;

    ຖ້າ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	ຖ້າ (buf == NULL)
	{
	    free_callback(&callback);
	    ສົ່ງຄືນ;
	}
	ຖ້າ (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_text = dict_get_bool(d, "text", false);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    ຖ້າ (lnr == NULL)
    {
	free_callback(&callback);
	ສົ່ງຄືນ;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    ຖ້າ (unbuffered)
    {
	lnr->lr_next = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	lnr->lr_next = buf->b_listener;
	buf->b_listener = lnr;
    }

    set_callback(&lnr->lr_callback, &callback);

    lnr->lr_text = want_text;
    lnr->lr_id = ++next_listener_id;
    rettv->vval.v_number = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    void
f_listener_flush(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf = curbuf;

    ຖ້າ (check_secure())
	ສົ່ງຄືນ;

    ຖ້າ (recursive)
	ສົ່ງຄືນ;

    ຖ້າ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    ຖ້າ (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	ຖ້າ (buf == NULL)
	    ສົ່ງຄືນ;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    static listener_T *
find_listener(
    ຈຳນວນເຕັມ        id,
    listener_T *list_start,
    listener_T **prev)
{
    listener_T *next;
    listener_T *lnr;

    *prev = NULL;
    ສຳລັບ (lnr = list_start; lnr != NULL; lnr = next)
    {
	next = lnr->lr_next;
	ຖ້າ (lnr->lr_id == id)
	    ສົ່ງຄືນ lnr;
	*prev = lnr;
    }
    ສົ່ງຄືນ NULL;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    void
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    ຈຳນວນເຕັມ		id;
    buf_T	*buf;

    ຖ້າ (check_secure())
	ສົ່ງຄືນ;

    ຖ້າ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    id = tv_get_number(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = find_listener(id, buf->b_listener, &prev);
	ຖ້າ (lnr == NULL)
	    lnr = find_listener(id, buf->b_sync_listener, &prev);
	ຖ້າ (lnr != NULL)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_number = 1;
	    ສົ່ງຄືນ;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    void
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, ຈຳນວນເຕັມ added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    static void
invoke_listener_set(
    buf_T       *buf,
    linenr_T	start,
    linenr_T	end,
    long	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    ຈຳນວນເຕັມ		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_NUMBER;
    argv[1].vval.v_number = start;
    argv[2].v_type = VAR_NUMBER;
    argv[2].vval.v_number = end;
    argv[3].v_type = VAR_NUMBER;
    argv[3].vval.v_number = added;
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

    ສຳລັບ (lnr = listeners; lnr != NULL; lnr = lnr->lr_next)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    ຖ້າ (save_updating_screen)
	updating_screen = TRUE;
    ບໍ່ດັ່ງນັ້ນ
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    static void
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	start,
    colnr_T	col,
    linenr_T	end,
    long	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    ຖ້າ (recursive || curbuf->b_sync_listener == NULL)
	ສົ່ງຄືນ;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    ຖ້າ (recorded_changes == NULL)  // out of memory
	ສົ່ງຄືນ;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    ຖ້າ (dict == NULL)
    {
	list_unref(recorded_changes);
	ສົ່ງຄືນ;
    }

    dict_add_number(dict, "lnum", (varnumber_T)start);
    dict_add_number(dict, "end", (varnumber_T)end);
    dict_add_number(dict, "added", (varnumber_T)added);
    dict_add_number(dict, "col", (varnumber_T)col + 1);
    ຖ້າ (listeners_want_text(buf->b_sync_listener))
	(void)add_change_text(buf, dict, start, end, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_set(
	buf, start, end, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    void
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	start = MAXLNUM;
    linenr_T	end = 0;
    linenr_T	added = 0;

    ຖ້າ (buf->b_recorded_changes == NULL  // nothing changed
	    || buf->b_listener == NULL	 // no listeners
	    || recursive)		 // already busy
	ສົ່ງຄືນ;

    FOR_ALL_LIST_ITEMS(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_number(li->li_tv.vval.v_dict, "lnum");
	ຖ້າ (start > lnum)
	    start = lnum;
	lnum = dict_get_number(li->li_tv.vval.v_dict, "end");
	ຖ້າ (end < lnum)
	    end = lnum;
	added += dict_get_number(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_set(
	buf, start, end, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = NULL;
    buf->b_recorded_text_size = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    void
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(NULL, &buf->b_sync_listener, TRUE);
}

#endif

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    static void
changed_common(
    linenr_T	lnum,
    colnr_T	col,
    linenr_T	lnume,
    long	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		cols;
    pos_T	*p;
    ຈຳນວນເຕັມ		add;

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
    ຖ້າ (curwin->w_p_diff && diff_internal())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_line(lnum);
    }
#endif

    // set the '. mark
    ຖ້າ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_last_change.lnum = lnum;
	curbuf->b_last_change.col = col;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	ຖ້າ (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    ຖ້າ (curbuf->b_changelistlen == 0)
		add = TRUE;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		ຖ້າ (p->lnum != lnum)
		    add = TRUE;
		ບໍ່ດັ່ງນັ້ນ
		{
		    cols = comp_textwidth(FALSE);
		    ຖ້າ (cols == 0)
			cols = 79;
		    add = (p->col + cols < col || col + cols < p->col);
		}
	    }
	    ຖ້າ (add)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		ຖ້າ (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  parimaa_na(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			ຖ້າ (wp->w_buffer == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    ຖ້າ (wp->w_buffer == curbuf
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

    ຖ້າ (VIsual_active)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ຖ້າ (wp->w_buffer == curbuf)
	{
	    linenr_T last = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    ຖ້າ (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    ຖ້າ (xtra != 0 && wp->w_redraw_top != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    ຖ້າ (wp->w_skipcol > 0
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
	    i = hasFoldingWin(wp, lnum, &lnum, NULL, FALSE, NULL);
	    ຖ້າ (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, last, NULL, &last, FALSE, NULL);
	    ຖ້າ (wp->w_cursor.lnum == last)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    ຖ້າ (wp->w_cursor.lnum <= lnum)
	    {
		i = find_wl_entry(wp, lnum);
		ຖ້າ (i >= 0 && wp->w_cursor.lnum > wp->w_lines[i].wl_lnum)
		    changed_line_abv_curs_win(wp);
	    }
#endif
	    ຖ້າ (wp->w_cursor.lnum > lnum)
		changed_line_abv_curs_win(wp);
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (wp->w_cursor.lnum == lnum && wp->w_cursor.col >= col)
		changed_cline_bef_curs_win(wp);
	    ຖ້າ (wp->w_botline >= lnum)
	    {
		ຖ້າ (xtra < 0)
		    invalidate_botline_win(wp);
		ບໍ່ດັ່ງນັ້ນ
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    ສຳລັບ (i = 0; i < wp->w_lines_valid; ++i)
		ຖ້າ (wp->w_lines[i].wl_valid)
		{
		    ຖ້າ (wp->w_lines[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			ຖ້າ (wp->w_lines[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_lines[i].wl_valid = FALSE;
			}
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (xtra != 0)
			{
			    // line below change
			    wp->w_lines[i].wl_lnum += xtra;
#ifdef FEAT_FOLDING
			    wp->w_lines[i].wl_lastlnum += xtra;
#endif
			}
		    }
#ifdef FEAT_FOLDING
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (wp->w_lines[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_lines[i].wl_valid = FALSE;
		    }
#endif
		}

#ifdef FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    ຖ້າ (hasAnyFolding(wp))
		set_topline(wp, wp->w_topline);
#endif
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    ຖ້າ (wp->w_p_rnu && xtra != 0)
		wp->w_last_cursor_lnum_rnu = 0;

#ifdef FEAT_SYN_HL
	    ຖ້າ (wp->w_p_cul && wp->w_last_cursorline >= lnum)
	    {
		ຖ້າ (wp->w_last_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_last_cursorline = 0;
		ບໍ່ດັ່ງນັ້ນ
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_last_cursorline += xtra;
	    }
#endif
	}
#ifdef FEAT_SEARCH_EXTRA
	ຖ້າ (wp == curwin && xtra != 0 && search_hl_has_cursor_lnum >= lnum)
	    search_hl_has_cursor_lnum += xtra;
#endif
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    set_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    ຖ້າ (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	last_cursormoved.lnum = 0;
}

    static void
changedOneline(buf_T *buf, linenr_T lnum)
{
    ຖ້າ (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	ຖ້າ (lnum < buf->b_mod_top)
	    buf->b_mod_top = lnum;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_set = true;
	buf->b_mod_top = lnum;
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
    void
changed_bytes(linenr_T lnum, colnr_T col)
{
    changedOneline(curbuf, lnum);
    changed_common(lnum, col, lnum + 1, 0L);

#ifdef FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    ຖ້າ (spell_check_window(curwin)
	    && lnum < curbuf->b_ml.ml_line_count
	    && vim_strchr(p_cpo, CPO_DOLLAR) == NULL)
	redrawWinline(curwin, lnum + 1);
#endif
#ifdef FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    ຖ້າ (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ຖ້າ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ຖ້າ (wlnum > 0)
		    changedOneline(wp->w_buffer, wlnum);
	    }
    }
#endif
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    void
inserted_bytes(linenr_T lnum, colnr_T col, ຈຳນວນເຕັມ added UNUSED)
{
#ifdef FEAT_PROP_POPUP
    ຖ້າ (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, col, added, 0);
#endif

    changed_bytes(lnum, col);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    void
appended_lines(linenr_T lnum, long count)
{
    changed_lines(lnum + 1, 0, lnum + 1, count);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    void
appended_lines_mark(linenr_T lnum, long count)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, count, 0L);
    changed_lines(lnum + 1, 0, lnum + 1, count);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    void
deleted_lines(linenr_T lnum, long count)
{
    changed_lines(lnum, 0, lnum + count, -count);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    void
deleted_lines_mark(linenr_T lnum, long count)
{
    mark_adjust(lnum, (linenr_T)(lnum + count - 1), (long)MAXLNUM, -count);
    changed_lines(lnum, 0, lnum + count, -count);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    void
changed_lines_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    ຖ້າ (buf->b_mod_set)
    {
	// find the maximum area that must be redisplayed
	ຖ້າ (lnum < buf->b_mod_top)
	    buf->b_mod_top = lnum;
	ຖ້າ (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    ຖ້າ (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	ຖ້າ (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// set the area that must be redisplayed
	buf->b_mod_set = true;
	buf->b_mod_top = lnum;
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
    void
changed_lines(
    linenr_T	lnum,	    // first line with change
    colnr_T	col,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    long	xtra)	    // number of extra lines (negative when deleting)
{
    changed_lines_buf(curbuf, lnum, lnume, xtra);

#ifdef FEAT_DIFF
    ຖ້າ (xtra == 0 && curwin->w_p_diff && !diff_internal())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ຖ້າ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ຖ້າ (wlnum > 0)
		    changed_lines_buf(wp->w_buffer, wlnum,
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
    void
unchanged(buf_T *buf, ຈຳນວນເຕັມ ff, ຈຳນວນເຕັມ always_inc_changedtick)
{
    ຖ້າ (buf->b_changed || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_changed = 0;
	ml_setflags(buf);
	ຖ້າ (ff)
	    save_file_ff(buf);
	check_status(buf);
	redraw_tabline = TRUE;
#ຖ້າ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (always_inc_changedtick)
	++CHANGEDTICK(buf);
#ifdef FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#endif
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    void
save_file_ff(buf_T *buf)
{
    buf->b_start_ffc = *buf->b_p_ff;
    buf->b_start_eof = buf->b_p_eof;
    buf->b_start_eol = buf->b_p_eol;
    buf->b_start_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    ຖ້າ (buf->b_start_fenc == NULL
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
    ຈຳນວນເຕັມ
file_ff_differs(buf_T *buf, ຈຳນວນເຕັມ ignore_empty)
{
    // In a buffer that was never loaded the options are not valid.
    ຖ້າ (buf->b_flags & BF_NEVERLOADED)
	ສົ່ງຄືນ FALSE;
    ຖ້າ (ignore_empty
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_line_count == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	ສົ່ງຄືນ FALSE;
    ຖ້າ (buf->b_start_ffc != *buf->b_p_ff)
	ສົ່ງຄືນ TRUE;
    ຖ້າ ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_start_eof != buf->b_p_eof
		|| buf->b_start_eol != buf->b_p_eol))
	ສົ່ງຄືນ TRUE;
    ຖ້າ (!buf->b_p_bin && buf->b_start_bomb != buf->b_p_bomb)
	ສົ່ງຄືນ TRUE;
    ຖ້າ (buf->b_start_fenc == NULL)
	ສົ່ງຄືນ (*buf->b_p_fenc != NUL);
    ສົ່ງຄືນ (STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    void
ins_bytes(char_u *p)
{
    ins_bytes_len(p, (ຈຳນວນເຕັມ)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    void
ins_bytes_len(char_u *p, ຈຳນວນເຕັມ len)
{
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		n;

    ຖ້າ (has_mbyte)
	ສຳລັບ (i = 0; i < len; i += n)
	{
	    ຖ້າ (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    ບໍ່ດັ່ງນັ້ນ
		n = (*mb_ptr2len)(p + i);
	    ins_char_bytes(p + i, n);
	}
    ບໍ່ດັ່ງນັ້ນ
	ສຳລັບ (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    void
ins_char(ຈຳນວນເຕັມ c)
{
    char_u	buf[MB_MAXBYTES + 1];
    ຈຳນວນເຕັມ		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    ຖ້າ (buf[0] == 0)
	buf[0] = '\n';

    ins_char_bytes(buf, n);
}

    void
ins_char_bytes(char_u *buf, ຈຳນວນເຕັມ charlen)
{
    ຈຳນວນເຕັມ		c = buf[0];
    ຈຳນວນເຕັມ		newlen;		// nr of bytes inserted
    ຈຳນວນເຕັມ		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    ຈຳນວນເຕັມ		linelen;	// length of old line including NUL
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;
    ຈຳນວນເຕັມ		i;

    // Break tabs if needed.
    ຖ້າ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    linelen = (ຈຳນວນເຕັມ)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    ຖ້າ (State & REPLACE_FLAG)
    {
	ຖ້າ (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    ຈຳນວນເຕັມ		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    ຖ້າ (old_list && vim_strchr(p_cpo, CPO_LISTWM) == NULL)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, NULL, &vcol, NULL, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    ໃນຂະນະທີ່ (oldp[col + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + col + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		ຖ້າ (vcol > new_vcol && oldp[col + oldlen] == TAB)
		    bha_nga;
		oldlen += (*mb_ptr2len)(oldp + col + oldlen);
		// Deleted a bit too much, insert spaces.
		ຖ້າ (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (oldp[col] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + col);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_push(NUL);
	ສຳລັບ (i = 0; i < oldlen; ++i)
	{
	    ຖ້າ (has_mbyte)
		i += replace_push_mb(oldp + col + i) - 1;
	    ບໍ່ດັ່ງນັ້ນ
		replace_push(oldp[col + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    ຖ້າ (newp == NULL)
	ສົ່ງຄືນ;

    // Copy bytes before the cursor.
    ຖ້າ (col > 0)
	mch_memmove(newp, oldp, (size_t)col);

    // Copy bytes after the changed character(s).
    p = newp + col;
    ຖ້າ (linelen > col + oldlen)
	mch_memmove(p + newlen, oldp + col + oldlen,
					    (size_t)(linelen - col - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    ໃນຂະນະທີ່ (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    changed_bytes(lnum, col);
#ifdef FEAT_PROP_POPUP
    ຖ້າ (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, col, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#endif

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    ຖ້າ (p_sm && (State & MODE_INSERT)
	    && msg_silent == 0
	    && !ins_compl_active())
    {
	ຖ້າ (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	ບໍ່ດັ່ງນັ້ນ
	    showmatch(c);
    }

#ifdef FEAT_RIGHTLEFT
    ຖ້າ (!p_ri || (State & REPLACE_FLAG))
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
    void
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    ຈຳນວນເຕັມ		oldlen;
    colnr_T	col;
    linenr_T	lnum = curwin->w_cursor.lnum;

    ຖ້າ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    col = curwin->w_cursor.col;
    oldp = ml_get(lnum);
    oldlen = (ຈຳນວນເຕັມ)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    ຖ້າ (newp == NULL)
	ສົ່ງຄືນ;
    ຖ້າ (col > 0)
	mch_memmove(newp, oldp, (size_t)col);
    mch_memmove(newp + col, s, slen);
    mch_memmove(newp + col + slen, oldp + col, (size_t)(oldlen - col + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_bytes(lnum, col, (ຈຳນວນເຕັມ)slen);
    curwin->w_cursor.col += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ຈຳນວນເຕັມ
del_char(ຈຳນວນເຕັມ fixpos)
{
    ຖ້າ (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	ຖ້າ (*ml_get_cursor() == NUL)
	    ສົ່ງຄືນ FAIL;
	ສົ່ງຄືນ del_chars(1L, fixpos);
    }
    ສົ່ງຄືນ del_bytes(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    ຈຳນວນເຕັມ
del_chars(long count, ຈຳນວນເຕັມ fixpos)
{
    long	bytes = 0;
    long	i;
    char_u	*p;
    ຈຳນວນເຕັມ		l;

    p = ml_get_cursor();
    ສຳລັບ (i = 0; i < count && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	bytes += l;
	p += l;
    }
    ສົ່ງຄືນ del_bytes(bytes, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ຈຳນວນເຕັມ
del_bytes(
    long	count,
    ຈຳນວນເຕັມ		fixpos_arg,
    ຈຳນວນເຕັມ		use_delcombine UNUSED)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    ຈຳນວນເຕັມ		alloc_newp;
    long	movelen;
    ຈຳນວນເຕັມ		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (ຈຳນວນເຕັມ)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    ຖ້າ (col >= oldlen)
	ສົ່ງຄືນ FAIL;

    // If "count" is zero there is nothing to do.
    ຖ້າ (count == 0)
	ສົ່ງຄືນ OK;

    // If "count" is negative the caller must be doing something wrong.
    ຖ້າ (count < 1)
    {
	siemsg(e_invalid_count_for_del_bytes_nr, count);
	ສົ່ງຄືນ FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    ຖ້າ (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + col) >= count)
    {
	ຈຳນວນເຕັມ	cc[MAX_MCO];
	ຈຳນວນເຕັມ	n;

	(void)utfc_ptr2char(oldp + col, cc);
	ຖ້າ (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = col;
	    do
	    {
		col = n;
		count = utf_ptr2len(oldp + n);
		n += count;
	    } ໃນຂະນະທີ່ (UTF_COMPOSINGLIKE(oldp + col, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (long)oldlen - (long)col - count + 1; // includes trailing NUL
    ຖ້າ (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	ຖ້າ (col > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.coladd = 0;
	    ຖ້າ (has_mbyte)
		curwin->w_cursor.col -=
			    (*mb_head_off)(oldp, oldp + curwin->w_cursor.col);
	}
	count = oldlen - col;
	movelen = 1;
    }
    newlen = oldlen - count;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#ifdef FEAT_NETBEANS_INTG
    ຖ້າ (netbeans_active())
	alloc_newp = TRUE;
    ບໍ່ດັ່ງນັ້ນ
#endif
	alloc_newp = !ml_line_alloced();    // check if oldp was allocated
    ຖ້າ (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    ບໍ່ດັ່ງນັ້ນ
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	ຖ້າ (newp == NULL)
	    ສົ່ງຄືນ FAIL;
	mch_memmove(newp, oldp, (size_t)col);
    }
    mch_memmove(newp + col, oldp + col + count, (size_t)movelen);
    ຖ້າ (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    ບໍ່ດັ່ງນັ້ນ
    {
#ifdef FEAT_PROP_POPUP
	// Also move any following text properties.
	ຖ້າ (oldlen + 1 < curbuf->b_ml.ml_line_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_line_len - oldlen - 1);
#endif
	curbuf->b_ml.ml_line_len -= count;
	curbuf->b_ml.ml_line_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, col, -count);

    ສົ່ງຄືນ OK;
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
    ຈຳນວນເຕັມ
open_line(
    ຈຳນວນເຕັມ		dir,		// FORWARD or BACKWARD
    ຈຳນວນເຕັມ		flags,
    ຈຳນວນເຕັມ		second_line_indent,
    ຈຳນວນເຕັມ		*did_do_comment UNUSED)
{
    char_u	*saved_line;		// copy of the original line
    char_u	*next_line = NULL;	// copy of the next line
    char_u	*p_extra = NULL;	// what goes to next line
    ຈຳນວນເຕັມ		less_cols = 0;		// less columns for mark in new line
    ຈຳນວນເຕັມ		less_cols_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    ຈຳນວນເຕັມ		newcol = 0;		// new cursor column
    ຈຳນວນເຕັມ		newindent = 0;		// auto-indent of the new line
    ຈຳນວນເຕັມ		n;
    ຈຳນວນເຕັມ		trunc_line = FALSE;	// truncate current line afterwards
    ຈຳນວນເຕັມ		retval = FAIL;		// return value
    ຈຳນວນເຕັມ		extra_len = 0;		// length of p_extra string
    ຈຳນວນເຕັມ		lead_len;		// length of comment leader
    ຈຳນວນເຕັມ		comment_start = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = NULL;		// copy of comment leader
    char_u	*allocated = NULL;	// allocated memory
    char_u	*p;
    ຈຳນວນເຕັມ		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    ຈຳນວນເຕັມ		do_cindent;
    ຈຳນວນເຕັມ		do_si = may_do_si();
    ຈຳນວນເຕັມ		no_si = FALSE;		// reset did_si afterwards
    ຈຳນວນເຕັມ		first_char = NUL;	// init for GCC
    ຈຳນວນເຕັມ		vreplace_mode;
    ຈຳນວນເຕັມ		did_append;		// appended a new line
    ຈຳນວນເຕັມ		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#ifdef FEAT_PROP_POPUP
    ຈຳນວນເຕັມ		at_eol;			// cursor after last character
#endif

    // make a copy of the current line so we can mess with it
    saved_line = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ຖ້າ (saved_line == NULL)	    // out of memory!
	ສົ່ງຄືນ FALSE;

#ifdef FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.col >= (ຈຳນວນເຕັມ)ml_get_curline_len();
#endif

    ຖ້າ (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	ຖ້າ (curwin->w_cursor.lnum < orig_line_count)
	    next_line = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	ບໍ່ດັ່ງນັ້ນ
	    next_line = vim_strsave((char_u *)"");
	ຖ້າ (next_line == NULL)	    // out of memory!
	    goto theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_push(NUL);  // Call twice because BS over NL expects it
	replace_push(NUL);
	p = saved_line + curwin->w_cursor.col;
	ໃນຂະນະທີ່ (*p != NUL)
	{
	    ຖ້າ (has_mbyte)
		p += replace_push_mb(p);
	    ບໍ່ດັ່ງນັ້ນ
		replace_push(*p++);
	}
	saved_line[curwin->w_cursor.col] = NUL;
    }

    ຖ້າ ((State & MODE_INSERT) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_line + curwin->w_cursor.col;
	ຖ້າ (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    first_char = *p;
	}
	extra_len = (ຈຳນວນເຕັມ)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_col = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    ຖ້າ (dir == FORWARD && did_ai)
	trunc_line = TRUE;

    ຖ້າ ((flags & OPENLINE_FORCE_INDENT) && second_line_indent >= 0)
	newindent = second_line_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#ifdef FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_line, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ບໍ່ດັ່ງນັ້ນ
	newindent = get_indent_str(saved_line, (ຈຳນວນເຕັມ)curbuf->b_p_ts, FALSE);
#endif
	ຖ້າ (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = second_line_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	ຖ້າ (!trunc_line && do_si && *saved_line != NUL
				    && (p_extra == NULL || first_char != '{'))
	{
	    char_u  *ptr;
	    char_u  last_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_line;
	    ຖ້າ (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, NULL, FALSE, TRUE);
	    ບໍ່ດັ່ງນັ້ນ
		lead_len = 0;
	    ຖ້າ (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ຖ້າ ( lead_len == 0 && ptr[0] == '#')
		{
		    ໃນຂະນະທີ່ (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		ຖ້າ (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, NULL, FALSE, TRUE);
		ບໍ່ດັ່ງນັ້ນ
		    lead_len = 0;
		ຖ້າ (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    ຖ້າ (p[0] == '/' && p[1] == '*')
			p++;
		    ຖ້າ (p[0] == '*')
		    {
			ສຳລັບ (p++; *p; p++)
			{
			    ຖ້າ (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.col = (colnr_T)(p - ptr);
				ຖ້າ ((pos = findmatch(NULL, NUL)) != NULL)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    bha_nga;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.col;
			    }
			}
		    }
		}
		ບໍ່ດັ່ງນັ້ນ	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    ໃນຂະນະທີ່ (p > ptr && VIM_ISWHITE(*p))
			--p;
		    last_char = *p;

		    // find the character just before the '{' or ';'
		    ຖ້າ (last_char == '{' || last_char == ';')
		    {
			ຖ້າ (p > ptr)
			    --p;
			ໃນຂະນະທີ່ (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    ຖ້າ (*p == ')')
		    {
			curwin->w_cursor.col = (colnr_T)(p - ptr);
			ຖ້າ ((pos = findmatch(NULL, '(')) != NULL)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    ຖ້າ (last_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (last_char != ';' && last_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    ບໍ່ດັ່ງນັ້ນ // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ຖ້າ (lead_len == 0 && ptr[0] == '#')
		{
		    ຈຳນວນເຕັມ was_backslashed = FALSE;

		    ໃນຂະນະທີ່ ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
		    {
			ຖ້າ (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			ບໍ່ດັ່ງນັ້ນ
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    ຖ້າ (was_backslashed)
			newindent = 0;	    // Got to end of file
		    ບໍ່ດັ່ງນັ້ນ
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		ຖ້າ (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		ບໍ່ດັ່ງນັ້ນ		    // can delete indent when '{' typed
		    can_si_back = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	ຖ້າ (do_si)
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
		? KEY_OPEN_FORW
		: KEY_OPEN_BACK, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    end_comment_pending = NUL;
    ຖ້າ (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_line, &lead_flags,
							dir == BACKWARD, TRUE);
	ຖ້າ (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_start = check_linecomment(saved_line);
	    ຖ້າ (comment_start != MAXCOL)
	    {
		lead_len = get_leader_len(saved_line + comment_start,
						     &lead_flags, FALSE, TRUE);
		ຖ້າ (lead_len != 0)
		{
		    lead_len += comment_start;
		    ຖ້າ (did_do_comment != NULL)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	lead_len = 0;
    ຖ້າ (lead_len > 0)
    {
	char_u	*lead_repl = NULL;	    // replaces comment leader
	ຈຳນວນເຕັມ	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_middle[COM_MAX_LEN];   // middle-comment string
	ຈຳນວນເຕັມ	lead_middle_len;	    // length of the lead_middle
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_end = NULL;	    // where lead_end has been found
	ຈຳນວນເຕັມ	extra_space = FALSE;	    // append extra space
	ຈຳນວນເຕັມ	current_flag;
	ຈຳນວນເຕັມ	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	ສຳລັບ (p = lead_flags; *p && *p != ':'; ++p)
	{
	    ຖ້າ (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		continue;
	    }
	    ຖ້າ (*p == COM_START || *p == COM_MIDDLE)
	    {
		current_flag = *p;
		ຖ້າ (*p == COM_START)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    ຖ້າ (dir == BACKWARD)
		    {
			lead_len = 0;
			bha_nga;
		    }

		    // find start of middle part
		    (void)copy_option_part(&p, lead_middle, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		ໃນຂະນະທີ່ (*p && p[-1] != ':')	// find end of middle flags
		{
		    ຖ້າ (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_middle_len = copy_option_part(&p, lead_middle, COM_MAX_LEN, ",");

		ໃນຂະນະທີ່ (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    ຖ້າ (*p == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

		ຖ້າ (end_comment_pending == -1)	// we can set it now
		    end_comment_pending = lead_end[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		ຖ້າ (dir == FORWARD)
		{
		    ສຳລັບ (p = saved_line + lead_len; *p; ++p)
			ຖ້າ (STRNCMP(p, lead_end, n) == 0)
			{
			    comment_end = p;
			    lead_len = 0;
			    bha_nga;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		ຖ້າ (lead_len > 0)
		{
		    ຖ້າ (current_flag == COM_START)
		    {
			lead_repl = lead_middle;
			lead_repl_len = lead_middle_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    ຖ້າ (!VIM_ISWHITE(saved_line[lead_len - 1])
			    && ((p_extra != NULL
				    && (ຈຳນວນເຕັມ)curwin->w_cursor.col == lead_len)
				|| (p_extra == NULL
				    && saved_line[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		bha_nga;
	    }
	    ຖ້າ (*p == COM_END)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		ຖ້າ (dir == FORWARD)
		{
		    comment_end = skipwhite(saved_line);
		    lead_len = 0;
		    bha_nga;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		ໃນຂະນະທີ່ (p > curbuf->b_p_com && *p != ',')
		    --p;
		ສຳລັບ (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (ຈຳນວນເຕັມ)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		ສຳລັບ (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    ຖ້າ (*p2 == COM_AUTO_END)
			end_comment_pending = -1; // means we want to set it
		}
		ຖ້າ (end_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    ໃນຂະນະທີ່ (*p2 && *p2 != ',')
			p2++;
		    end_comment_pending = p2[-1];
		}
		bha_nga;
	    }
	    ຖ້າ (*p == COM_FIRST)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		ຖ້າ (dir == BACKWARD)
		    lead_len = 0;
		ບໍ່ດັ່ງນັ້ນ
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		bha_nga;
	    }
	}
	ຖ້າ (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (second_line_indent > 0 ? second_line_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    ຖ້າ (leader == NULL)
		lead_len = 0;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຈຳນວນເຕັມ li;

		vim_strncpy(leader, saved_line, lead_len);

		// TODO: handle multi-byte and double width chars
		ສຳລັບ (li = 0; li < comment_start; ++li)
		    ຖ້າ (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		ຖ້າ (lead_repl != NULL)
		{
		    ຈຳນວນເຕັມ		c = 0;
		    ຈຳນວນເຕັມ		off = 0;

		    ສຳລັບ (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			ຖ້າ (*p == COM_RIGHT || *p == COM_LEFT)
			    c = *p++;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			ບໍ່ດັ່ງນັ້ນ
			    ++p;
		    }
		    ຖ້າ (c == COM_RIGHT)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			ສຳລັບ (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    ຈຳນວນເຕັມ	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ຈຳນວນເຕັມ	    old_size = 0;
			    char_u  *endp = p;
			    ຈຳນວນເຕັມ	    l;

			    ໃນຂະນະທີ່ (old_size < repl_size && p > leader)
			    {
				MB_PTR_BACK(leader, p);
				old_size += ptr2cells(p);
			    }
			    l = lead_repl_len - (ຈຳນວນເຕັມ)(endp - p);
			    ຖ້າ (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			ຖ້າ (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			ໃນຂະນະທີ່ (--p >= leader)
			{
			    ຈຳນວນເຕັມ l = mb_head_off(leader, p);

			    ຖ້າ (l > 1)
			    {
				p -= l;
				ຖ້າ (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    ບໍ່ດັ່ງນັ້ນ	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    ຈຳນວນເຕັມ	    repl_size = vim_strnsize(lead_repl,
							       lead_repl_len);
			    ຈຳນວນເຕັມ	    i;
			    ຈຳນວນເຕັມ	    l;

			    ສຳລັບ (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				ຖ້າ (vim_strnsize(p, i + l) > repl_size)
				    bha_nga;
			    }
			    ຖ້າ (i != lead_repl_len)
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
			ສຳລັບ (p += lead_repl_len; p < leader + lead_len; ++p)
			    ຖ້າ (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				ຖ້າ (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				ບໍ່ດັ່ງນັ້ນ
				{
				    ຈຳນວນເຕັມ	    l = (*mb_ptr2len)(p);

				    ຖ້າ (l > 1)
				    {
					ຖ້າ (ptr2cells(p) > 1)
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
		    ຖ້າ (curbuf->b_p_ai || do_si)
#ifdef FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ບໍ່ດັ່ງນັ້ນ
			newindent = get_indent_str(leader,
						   (ຈຳນວນເຕັມ)curbuf->b_p_ts, FALSE);
#endif

		    // Add the indent offset
		    ຖ້າ (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    ບໍ່ດັ່ງນັ້ນ
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    ໃນຂະນະທີ່ (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			ຖ້າ (vim_strchr(skipwhite(leader), '\t') != NULL)
			    bha_nga;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    ຖ້າ (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		ຖ້າ (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		ຖ້າ (newindent || did_si)
		{
		    ໃນຂະນະທີ່ (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (comment_end != NULL)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    ຖ້າ (comment_end[0] == '*' && comment_end[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.col = (colnr_T)(comment_end - saved_line);
		ຖ້າ ((pos = findmatch(NULL, NUL)) != NULL)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    ຖ້າ (p_extra != NULL)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	ຖ້າ (REPLACE_NORMAL(State))
	    replace_push(NUL);	    // end of extra blanks
	ຖ້າ (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    ໃນຂະນະທີ່ ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		ຖ້າ (REPLACE_NORMAL(State))
		    replace_push(*p_extra);
		++p_extra;
		++less_cols_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_cols = (ຈຳນວນເຕັມ)(p_extra - saved_line);
    }

    ຖ້າ (p_extra == NULL)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    ຖ້າ (lead_len)
    {
	ຖ້າ (flags & OPENLINE_COM_LIST && second_line_indent > 0)
	{
	    ຈຳນວນເຕັມ i;
	    ຈຳນວນເຕັມ padding = second_line_indent
					  - (newindent + (ຈຳນວນເຕັມ)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    ສຳລັບ (i = 0; i < padding; i++)
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
    ບໍ່ດັ່ງນັ້ນ
	end_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    ຖ້າ (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    ຖ້າ (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_line_count)
    {
	ຖ້າ (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    goto theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#ifdef FEAT_PROP_POPUP
	ຖ້າ ((State & MODE_INSERT) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_split(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.col + 1, 0, at_eol);
#endif
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	ຖ້າ (curwin->w_cursor.lnum >= Insstart.lnum + vr_lines_changed)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (void)u_save_cursor();		    // errors are ignored!
	    vr_lines_changed++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	changed_bytes(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    ຖ້າ (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	ຖ້າ (did_si)
	{
	    ຈຳນວນເຕັມ sw = (ຈຳນວນເຕັມ)get_sw_value(curbuf);

	    ຖ້າ (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	ຖ້າ (curbuf->b_p_ci)
	{
	    (void)copy_indent(newindent, saved_line);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	ບໍ່ດັ່ງນັ້ນ
	    (void)set_indent(newindent, SIN_INSERT);
	less_cols -= curwin->w_cursor.col;

	ai_col = curwin->w_cursor.col;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	ຖ້າ (REPLACE_NORMAL(State))
	    ສຳລັບ (n = 0; n < (ຈຳນວນເຕັມ)curwin->w_cursor.col; ++n)
		replace_push(NUL);
	newcol += curwin->w_cursor.col;
	ຖ້າ (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    ຖ້າ (REPLACE_NORMAL(State))
	ໃນຂະນະທີ່ (lead_len-- > 0)
	    replace_push(NUL);

    curwin->w_cursor = old_cursor;

    ຖ້າ (dir == FORWARD)
    {
	ຖ້າ (trunc_line || (State & MODE_INSERT))
	{
	    // truncate current line at cursor
	    saved_line[curwin->w_cursor.col] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    ຖ້າ (trunc_line && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_line, curwin->w_cursor.col);
	    ml_replace(curwin->w_cursor.lnum, saved_line, FALSE);
	    saved_line = NULL;
	    ຖ້າ (did_append)
	    {
		changed_lines(curwin->w_cursor.lnum, curwin->w_cursor.col,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		ຖ້າ (flags & OPENLINE_MARKFIX)
		    mark_col_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.col + less_cols_off,
						      1L, (long)-less_cols, 0);
#ifdef FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		ຖ້າ (curbuf->b_has_textprop && less_cols_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_cols_off, 0);
#endif
	    }
	    ບໍ່ດັ່ງນັ້ນ
		changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    ຖ້າ (did_append)
	changed_lines(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.col = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    ຖ້າ (State & VREPLACE_FLAG)
    {
	vreplace_mode = State;	// So we know to put things right later
	State = MODE_INSERT;
    }
    ບໍ່ດັ່ງນັ້ນ
	vreplace_mode = 0;

    ຖ້າ (!p_paste)
    {
	ຖ້າ (leader == NULL
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_col = (colnr_T)getwhitecols_curline();
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_expr_indent();
	    ai_col = (colnr_T)getwhitecols_curline();
	}
    }

    ຖ້າ (vreplace_mode != 0)
	State = vreplace_mode;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    ຖ້າ (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ຖ້າ (p_extra == NULL)
	    goto theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, next_line, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;
	ins_bytes(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	next_line = NULL;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_line);
    vim_free(next_line);
    vim_free(allocated);
    ສົ່ງຄືນ retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ຈຳນວນເຕັມ
truncate_line(ຈຳນວນເຕັມ fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    char_u	*old_line;
    ຈຳນວນເຕັມ		deleted;

    old_line = ml_get(lnum);
    ຖ້າ (col == 0)
	newp = vim_strsave((char_u *)"");
    ບໍ່ດັ່ງນັ້ນ
	newp = vim_strnsave(old_line, col);
    deleted = (ຈຳນວນເຕັມ)ml_get_len(lnum) - col;

    ຖ້າ (newp == NULL)
	ສົ່ງຄືນ FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_bytes(lnum, curwin->w_cursor.col, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    ຖ້າ (fixpos && curwin->w_cursor.col > 0)
	--curwin->w_cursor.col;

    ສົ່ງຄືນ OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    void
del_lines(long nlines, ຈຳນວນເຕັມ undo)
{
    long	n;
    linenr_T	first = curwin->w_cursor.lnum;

    ຖ້າ (nlines <= 0)
	ສົ່ງຄືນ;

    // save the deleted lines for undo
    ຖ້າ (undo && u_savedel(first, nlines) == FAIL)
	ສົ່ງຄືນ;

    ສຳລັບ (n = 0; n < nlines; )
    {
	ຖ້າ (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    bha_nga;

	ml_delete_flags(first, ML_DEL_MESSAGE);
	++n;

	// If we delete the last line in the file, stop
	ຖ້າ (first > curbuf->b_ml.ml_line_count)
	    bha_nga;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.col = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_lines_mark(first, n);
}
