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

#شامل "vim.h"

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
    خالی
change_warning(صحیح ستون)
{
    ساکن حرف *w_readonly = N_("W10: Warning: Changing a readonly file");

    اگر (curbuf->b_did_warn
	    || curbuf_Is_بدلا()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	واپس;

    ++curbuf_تالا;
    apply_autocmds(EVENT_FILECHANGEDRO, تہی, تہی, FALSE, curbuf);
    --curbuf_تالا;
    اگر (!curbuf->b_p_ro)
	واپس;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    پیغام_آغاز();
    اگر (پیغام_صف == صفیں - 1)
	پیغام_ستون = ستون;
    پیغام_ماخذ(HL_ATTR(HLF_W));
    پیغام_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | پیغام_HIST);
#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#اگر_ختم
    پیغام_clr_eos();
    (خالی)پیغام_انتہا();
    اگر (پیغام_silent == 0 && !silent_منوال
#اگر_تعریف_شدہ FEAT_EVAL
	    && وقت_for_testing != 1
#اگر_ختم
       )
    {
	برآمد_flush();
	ui_delay(1002L, TRUE); // give the user time to think about it
    }
    curbuf->b_did_warn = true;
    redraw_cmdline = FALSE;	// don't redraw and erase the message
    اگر (پیغام_صف < صفیں - 1)
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
    خالی
بدلا(خالی)
{
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    اگر (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	اگر (im_is_preediting() && !xim_بدلا_while_preediting)
	    واپس;
	xim_بدلا_while_preediting = FALSE;
    }
#اگر_ختم

    اگر (!curbuf->b_بدلا)
    {
	صحیح	save_پیغام_scroll = پیغام_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	اگر (curbuf->b_may_تبادلہ && !bt_dontwrite(curbuf))
	{
	    صحیح save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    اگر (need_wait_return && emsg_silent == 0 && !اندر_assert_fails)
	    {
		برآمد_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		پیغام_scroll = save_پیغام_scroll;
	    }
	    ورنہ
		need_wait_return = save_need_wait_return;
	}
	بدلا_اندرونی();
    }
    ++CHANGEDTICK(curbuf);

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_مطابقت = FALSE;
#اگر_ختم
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    خالی
بدلا_اندرونی(خالی)
{
    curbuf->b_بدلا = TRUE;
    ml_setflags(curbuf);
    check_حالت(curbuf);
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
    need_maketitle = TRUE;	    // set window title later
}

#اگر_تعریف_شدہ FEAT_EVAL
// Set when listener callbacks are being invoked.
ساکن صحیح recursive = FALSE;

ساکن طویل اگلا_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
ساکن bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    ساکن خالی
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *پچھلا)
{
    اگر (پچھلا != تہی)
	پچھلا->lr_اگلا = lnr->lr_اگلا;
    ورنہ
	*list = lnr->lr_اگلا;
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
    ساکن خالی
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*پچھلا;
    listener_T	*lnr;
    listener_T	*اگلا;

    پچھلا = تہی;
    برائے (lnr = *list; lnr != تہی; lnr = اگلا)
    {
	اگلا = lnr->lr_اگلا;
	اگر (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, پچھلا);
	ورنہ
	    پچھلا = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    اگر (buf != تہی)
    {
	اگر (*list == تہی && buf->b_recorded_changes != تہی)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = تہی;
	    buf->b_recorded_متن_سائز = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    ساکن خالی
perform_listener_housekeeping(خالی)
{
    buf_T	*buf;

    اگر (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(تہی, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    ساکن خالی
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	طویل		xtra)
{
    perform_listener_housekeeping();
    اگر (buf->b_recorded_changes == تہی || xtra == 0)
	واپس;

    listitem_T *li;
    linenr_T	پچھلا_lnum;
    linenr_T	پچھلا_lnume;

    FOR_ALL_LIST_اشیاء(buf->b_recorded_changes, li)
    {
	پچھلا_lnum = (linenr_T)dict_get_نمبر(
		li->li_tv.vval.v_dict, "lnum");
	پچھلا_lnume = (linenr_T)dict_get_نمبر(
		li->li_tv.vval.v_dict, "end");
	اگر (پچھلا_lnum >= lnum || پچھلا_lnum > lnume || پچھلا_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    توڑو;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_متن_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    ساکن bool
listeners_want_متن(listener_T *list)
{
    listener_T	*lnr;

    برائے (lnr = list; lnr != تہی; lnr = lnr->lr_اگلا)
	اگر (lnr->lr_متن)
	    واپس true;
    واپس false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    ساکن size_t
جوڑو_change_متن(
    buf_T	*buf,
    dict_T	*dict,
    linenr_T	lnum,
    linenr_T	lnume,
    طویل	xtra)
{
    list_T	*l = list_alloc();
    linenr_T	نیچے = lnume + xtra;	// line below the changed region
    size_t	سائز = 0;

    اگر (l == تہی)
	واپس 0;

    اگر (نیچے > buf->b_ml.ml_سطر_گنتی + 1)
	نیچے = buf->b_ml.ml_سطر_گنتی + 1;
    برائے (linenr_T lp = lnum < 1 ? 1 : lnum; lp < نیچے; ++lp)
    {
	char_u	*سطر = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	اگر (list_append_string(l, سطر, len) == FAIL)
	{
	    list_free(l);
	    واپس 0;
	}
	سائز += (size_t)len + 1;
    }

    اگر (dict_جوڑو_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	واپس 0;
    }
    واپس سائز;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    ساکن خالی
may_ریکارڈ_change(
    linenr_T	lnum,
    colnr_T	ستون,
    linenr_T	lnume,
    طویل	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    اگر (curbuf->b_listener == تہی)
	واپس;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    اگر (curbuf->b_recorded_changes == تہی)
    {
	curbuf->b_recorded_changes = list_alloc();
	اگر (curbuf->b_recorded_changes == تہی)  // out of memory
	    واپس;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس;
    dict_جوڑو_نمبر(dict, "lnum", (varnumber_T)lnum);
    dict_جوڑو_نمبر(dict, "end", (varnumber_T)lnume);
    dict_جوڑو_نمبر(dict, "added", (varnumber_T)xtra);
    dict_جوڑو_نمبر(dict, "col", (varnumber_T)ستون + 1);
    اگر (listeners_want_متن(curbuf->b_listener))
	curbuf->b_recorded_متن_سائز +=
			  جوڑو_change_متن(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    اگر (!recursive && curbuf->b_recorded_متن_سائز > LISTENER_متن_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    خالی
f_listener_جوڑو(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_متن = false;

    اگر (check_secure())
	واپس;

    اگر (recursive)
    {
	emsg(_(e_cannot_جوڑو_listener_اندر_listener_callback));
	واپس;
    }

    اگر (اندر_vim9script() && (
	    check_for_opt_بفر_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	واپس;

    callback = get_callback(&argvars[0]);
    اگر (callback.cb_name == تہی)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	اگر (buf == تہی)
	{
	    free_callback(&callback);
	    واپس;
	}
	اگر (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_متن = dict_get_bool(d, "text", false);
	}
	ورنہ اگر (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    اگر (lnr == تہی)
    {
	free_callback(&callback);
	واپس;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    اگر (unbuffered)
    {
	lnr->lr_اگلا = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    ورنہ
    {
	lnr->lr_اگلا = buf->b_listener;
	buf->b_listener = lnr;
    }

    سیٹ_callback(&lnr->lr_callback, &callback);

    lnr->lr_متن = want_متن;
    lnr->lr_id = ++اگلا_listener_id;
    rettv->vval.v_نمبر = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    خالی
f_listener_flush(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf = curbuf;

    اگر (check_secure())
	واپس;

    اگر (recursive)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	اگر (buf == تہی)
	    واپس;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    ساکن listener_T *
ڈھونڈو_listener(
    صحیح        id,
    listener_T *list_آغاز,
    listener_T **پچھلا)
{
    listener_T *اگلا;
    listener_T *lnr;

    *پچھلا = تہی;
    برائے (lnr = list_آغاز; lnr != تہی; lnr = اگلا)
    {
	اگلا = lnr->lr_اگلا;
	اگر (lnr->lr_id == id)
	    واپس lnr;
	*پچھلا = lnr;
    }
    واپس تہی;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    خالی
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*پچھلا;
    صحیح		id;
    buf_T	*buf;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = tv_get_نمبر(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = ڈھونڈو_listener(id, buf->b_listener, &پچھلا);
	اگر (lnr == تہی)
	    lnr = ڈھونڈو_listener(id, buf->b_sync_listener, &پچھلا);
	اگر (lnr != تہی)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_نمبر = 1;
	    واپس;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    خالی
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, صحیح added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    ساکن خالی
invoke_listener_سیٹ(
    buf_T       *buf,
    linenr_T	آغاز,
    linenr_T	انتہا,
    طویل	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    صحیح		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_نمبر;
    argv[1].vval.v_نمبر = آغاز;
    argv[2].v_type = VAR_نمبر;
    argv[2].vval.v_نمبر = انتہا;
    argv[3].v_type = VAR_نمبر;
    argv[3].vval.v_نمبر = added;
    argv[4].v_type = VAR_LIST;
    argv[4].vval.v_list = recorded_changes;

    // Ensure the list of changes is locked to prevent any modifications by
    // callback code..
    val.v_type = VAR_LIST;
    val.v_تالا = 0;
    val.vval.v_list = recorded_changes;
    شے_تالا(&val, -1, TRUE, FALSE);

    // Protect against recursive callbacks, lock the buffer against changes and
    // set the updating_screen flag to prevent channel input processing, which
    // might also try to update the buffer.
    recursive = TRUE;
    ++textlock;
    updating_screen = TRUE;

    برائے (lnr = listeners; lnr != تہی; lnr = lnr->lr_اگلا)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    اگر (save_updating_screen)
	updating_screen = TRUE;
    ورنہ
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    ساکن خالی
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	آغاز,
    colnr_T	ستون,
    linenr_T	انتہا,
    طویل	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    اگر (recursive || curbuf->b_sync_listener == تہی)
	واپس;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    اگر (recorded_changes == تہی)  // out of memory
	واپس;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    اگر (dict == تہی)
    {
	list_unref(recorded_changes);
	واپس;
    }

    dict_جوڑو_نمبر(dict, "lnum", (varnumber_T)آغاز);
    dict_جوڑو_نمبر(dict, "end", (varnumber_T)انتہا);
    dict_جوڑو_نمبر(dict, "added", (varnumber_T)added);
    dict_جوڑو_نمبر(dict, "col", (varnumber_T)ستون + 1);
    اگر (listeners_want_متن(buf->b_sync_listener))
	(خالی)جوڑو_change_متن(buf, dict, آغاز, انتہا, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_سیٹ(
	buf, آغاز, انتہا, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    خالی
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	آغاز = MAXLNUM;
    linenr_T	انتہا = 0;
    linenr_T	added = 0;

    اگر (buf->b_recorded_changes == تہی  // nothing changed
	    || buf->b_listener == تہی	 // no listeners
	    || recursive)		 // already busy
	واپس;

    FOR_ALL_LIST_اشیاء(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_نمبر(li->li_tv.vval.v_dict, "lnum");
	اگر (آغاز > lnum)
	    آغاز = lnum;
	lnum = dict_get_نمبر(li->li_tv.vval.v_dict, "end");
	اگر (انتہا < lnum)
	    انتہا = lnum;
	added += dict_get_نمبر(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_سیٹ(
	buf, آغاز, انتہا, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = تہی;
    buf->b_recorded_متن_سائز = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    خالی
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(تہی, &buf->b_sync_listener, TRUE);
}

#اگر_ختم

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ساکن خالی
بدلا_common(
    linenr_T	lnum,
    colnr_T	ستون,
    linenr_T	lnume,
    طویل	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    صحیح		i;
    صحیح		ستون_تعداد;
    pos_T	*p;
    صحیح		جوڑو;

    // mark the buffer as modified
    بدلا();

#اگر_تعریف_شدہ FEAT_EVAL
    // Immediately send this change to any listeners that require changes not
    // to be buffered.
    invoke_sync_listeners(curbuf, lnum, ستون, lnume, xtra);

    // If there are any listeners accepting buffered changes then add changes
    // to the current buffer's list, flushing previous changes first if necessary.
    may_ریکارڈ_change(lnum, ستون, lnume, xtra);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (curwin->w_p_diff && diff_اندرونی())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_سطر(lnum);
    }
#اگر_ختم

    // set the '. mark
    اگر ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_آخری_change.lnum = lnum;
	curbuf->b_آخری_change.ستون = ستون;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	اگر (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    اگر (curbuf->b_changelistlen == 0)
		جوڑو = TRUE;
	    ورنہ
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		اگر (p->lnum != lnum)
		    جوڑو = TRUE;
		ورنہ
		{
		    ستون_تعداد = comp_textwidth(FALSE);
		    اگر (ستون_تعداد == 0)
			ستون_تعداد = 79;
		    جوڑو = (p->ستون + ستون_تعداد < ستون || ستون + ستون_تعداد < p->ستون);
		}
	    }
	    اگر (جوڑو)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		اگر (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  حجم(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			اگر (wp->w_بفر == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    اگر (wp->w_بفر == curbuf
			    && wp->w_changelistidx == curbuf->b_changelistlen)
			++wp->w_changelistidx;
		}
		++curbuf->b_changelistlen;
	    }
	}
	curbuf->b_changelist[curbuf->b_changelistlen - 1] =
							curbuf->b_آخری_change;
	// The current window is always after the last change, so that "g,"
	// takes you back to it.
	curwin->w_changelistidx = curbuf->b_changelistlen;
    }

    اگر (VIsual_فعال)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (wp->w_بفر == curbuf)
	{
	    linenr_T آخری = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    اگر (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    اگر (xtra != 0 && wp->w_redraw_چوٹی != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    اگر (wp->w_skipcol > 0
		    && (آخری < wp->w_topline
			|| (wp->w_topline >= lnum
			    && wp->w_topline < lnume
			    && linetabsize_eol(wp, wp->w_topline)
				    <= wp->w_skipcol + sms_marker_overlap(wp, -1))))
		wp->w_skipcol = 0;

	    // Check if a change in the buffer has invalidated the cached
	    // values for the cursor.
#اگر_تعریف_شدہ FEAT_FOLDING
	    // Update the folds for this window.  Can't postpone this, because
	    // a following operator might work on the whole fold: ">>dd".
	    foldUpdate(wp, lnum, آخری);

	    // The change may cause lines above or below the change to become
	    // included in a fold.  Set lnum/lnume to the first/last line that
	    // might be displayed differently.
	    // Set w_cline_folded here as an efficient way to update it when
	    // inserting lines just above a closed fold.
	    i = hasFoldingWin(wp, lnum, &lnum, تہی, FALSE, تہی);
	    اگر (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, آخری, تہی, &آخری, FALSE, تہی);
	    اگر (wp->w_cursor.lnum == آخری)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    اگر (wp->w_cursor.lnum <= lnum)
	    {
		i = ڈھونڈو_wl_اندراج(wp, lnum);
		اگر (i >= 0 && wp->w_cursor.lnum > wp->w_سطریں[i].wl_lnum)
		    بدلا_سطر_abv_curs_win(wp);
	    }
#اگر_ختم
	    اگر (wp->w_cursor.lnum > lnum)
		بدلا_سطر_abv_curs_win(wp);
	    ورنہ اگر (wp->w_cursor.lnum == lnum && wp->w_cursor.ستون >= ستون)
		بدلا_cline_bef_curs_win(wp);
	    اگر (wp->w_botline >= lnum)
	    {
		اگر (xtra < 0)
		    invalidate_botline_win(wp);
		ورنہ
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    برائے (i = 0; i < wp->w_سطریں_valid; ++i)
		اگر (wp->w_سطریں[i].wl_valid)
		{
		    اگر (wp->w_سطریں[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			اگر (wp->w_سطریں[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_سطریں[i].wl_valid = FALSE;
			}
			ورنہ اگر (xtra != 0)
			{
			    // line below change
			    wp->w_سطریں[i].wl_lnum += xtra;
#اگر_تعریف_شدہ FEAT_FOLDING
			    wp->w_سطریں[i].wl_lastlnum += xtra;
#اگر_ختم
			}
		    }
#اگر_تعریف_شدہ FEAT_FOLDING
		    ورنہ اگر (wp->w_سطریں[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_سطریں[i].wl_valid = FALSE;
		    }
#اگر_ختم
		}

#اگر_تعریف_شدہ FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    اگر (hasAnyFolding(wp))
		سیٹ_topline(wp, wp->w_topline);
#اگر_ختم
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    اگر (wp->w_p_rnu && xtra != 0)
		wp->w_آخری_cursor_lnum_rnu = 0;

#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (wp->w_p_cul && wp->w_آخری_cursorline >= lnum)
	    {
		اگر (wp->w_آخری_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_آخری_cursorline = 0;
		ورنہ
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_آخری_cursorline += xtra;
	    }
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	اگر (wp == curwin && xtra != 0 && تلاش_hl_has_cursor_lnum >= lnum)
	    تلاش_hl_has_cursor_lnum += xtra;
#اگر_ختم
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    سیٹ_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    اگر (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	آخری_cursormoved.lnum = 0;
}

    ساکن خالی
بدلا_Oneline(buf_T *buf, linenr_T lnum)
{
    اگر (buf->b_mod_سیٹ)
    {
	// find the maximum area that must be redisplayed
	اگر (lnum < buf->b_mod_چوٹی)
	    buf->b_mod_چوٹی = lnum;
	ورنہ اگر (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    ورنہ
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_سیٹ = true;
	buf->b_mod_چوٹی = lnum;
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
    خالی
بدلا_بائٹیں(linenr_T lnum, colnr_T ستون)
{
    بدلا_Oneline(curbuf, lnum);
    بدلا_common(lnum, ستون, lnum + 1, 0L);

#اگر_تعریف_شدہ FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    اگر (spell_check_کھڑکی(curwin)
	    && lnum < curbuf->b_ml.ml_سطر_گنتی
	    && vim_strchr(p_cpo, CPO_DOLLAR) == تہی)
	redrawWinline(curwin, lnum + 1);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    اگر (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		اگر (wlnum > 0)
		    بدلا_Oneline(wp->w_بفر, wlnum);
	    }
    }
#اگر_ختم
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    خالی
inserted_بائٹیں(linenr_T lnum, colnr_T ستون, صحیح added غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, ستون, added, 0);
#اگر_ختم

    بدلا_بائٹیں(lnum, ستون);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    خالی
appended_سطریں(linenr_T lnum, طویل گنتی)
{
    بدلا_سطریں(lnum + 1, 0, lnum + 1, گنتی);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    خالی
appended_سطریں_mark(linenr_T lnum, طویل گنتی)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, گنتی, 0L);
    بدلا_سطریں(lnum + 1, 0, lnum + 1, گنتی);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    خالی
deleted_سطریں(linenr_T lnum, طویل گنتی)
{
    بدلا_سطریں(lnum, 0, lnum + گنتی, -گنتی);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    خالی
deleted_سطریں_mark(linenr_T lnum, طویل گنتی)
{
    mark_adjust(lnum, (linenr_T)(lnum + گنتی - 1), (طویل)MAXLNUM, -گنتی);
    بدلا_سطریں(lnum, 0, lnum + گنتی, -گنتی);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    خالی
بدلا_سطریں_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    طویل	xtra)	    // number of extra lines (negative when deleting)
{
    اگر (buf->b_mod_سیٹ)
    {
	// find the maximum area that must be redisplayed
	اگر (lnum < buf->b_mod_چوٹی)
	    buf->b_mod_چوٹی = lnum;
	اگر (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    اگر (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	اگر (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    ورنہ
    {
	// set the area that must be redisplayed
	buf->b_mod_سیٹ = true;
	buf->b_mod_چوٹی = lnum;
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
    خالی
بدلا_سطریں(
    linenr_T	lnum,	    // first line with change
    colnr_T	ستون,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    طویل	xtra)	    // number of extra lines (negative when deleting)
{
    بدلا_سطریں_buf(curbuf, lnum, lnume, xtra);

#اگر_تعریف_شدہ FEAT_DIFF
    اگر (xtra == 0 && curwin->w_p_diff && !diff_اندرونی())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		اگر (wlnum > 0)
		    بدلا_سطریں_buf(wp->w_بفر, wlnum,
						    lnume - lnum + wlnum, 0L);
	    }
    }
#اگر_ختم

    بدلا_common(lnum, ستون, lnume, xtra);
}

/*
 * Called when the changed flag must be reset for buffer "buf".
 * When "ff" is TRUE also reset 'fileformat'.
 * When "always_inc_changedtick" is TRUE b:changedtick is incremented also when
 * the changed flag was off.
 */
    خالی
unchanged(buf_T *buf, صحیح ff, صحیح always_inc_changedtick)
{
    اگر (buf->b_بدلا || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_بدلا = 0;
	ml_setflags(buf);
	اگر (ff)
	    save_file_ff(buf);
	check_حالت(buf);
	redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#اگر_ختم
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    ورنہ اگر (always_inc_changedtick)
	++CHANGEDTICK(buf);
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#اگر_ختم
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    خالی
save_file_ff(buf_T *buf)
{
    buf->b_آغاز_ffc = *buf->b_p_ff;
    buf->b_آغاز_eof = buf->b_p_eof;
    buf->b_آغاز_eol = buf->b_p_eol;
    buf->b_آغاز_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    اگر (buf->b_آغاز_fenc == تہی
			     || STRCMP(buf->b_آغاز_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_آغاز_fenc);
	buf->b_آغاز_fenc = vim_strsave(buf->b_p_fenc);
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
    صحیح
file_ff_differs(buf_T *buf, صحیح ignore_خالی_خانے)
{
    // In a buffer that was never loaded the options are not valid.
    اگر (buf->b_flags & BF_NEVERLOADED)
	واپس FALSE;
    اگر (ignore_خالی_خانے
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_سطر_گنتی == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	واپس FALSE;
    اگر (buf->b_آغاز_ffc != *buf->b_p_ff)
	واپس TRUE;
    اگر ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_آغاز_eof != buf->b_p_eof
		|| buf->b_آغاز_eol != buf->b_p_eol))
	واپس TRUE;
    اگر (!buf->b_p_bin && buf->b_آغاز_bomb != buf->b_p_bomb)
	واپس TRUE;
    اگر (buf->b_آغاز_fenc == تہی)
	واپس (*buf->b_p_fenc != NUL);
    واپس (STRCMP(buf->b_آغاز_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    خالی
ins_بائٹیں(char_u *p)
{
    ins_بائٹیں_len(p, (صحیح)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    خالی
ins_بائٹیں_len(char_u *p, صحیح len)
{
    صحیح		i;
    صحیح		n;

    اگر (has_mbyte)
	برائے (i = 0; i < len; i += n)
	{
	    اگر (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    ورنہ
		n = (*mb_ptr2len)(p + i);
	    ins_char_بائٹیں(p + i, n);
	}
    ورنہ
	برائے (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    خالی
ins_char(صحیح c)
{
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    اگر (buf[0] == 0)
	buf[0] = '\n';

    ins_char_بائٹیں(buf, n);
}

    خالی
ins_char_بائٹیں(char_u *buf, صحیح charlen)
{
    صحیح		c = buf[0];
    صحیح		newlen;		// nr of bytes inserted
    صحیح		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    صحیح		linelen;	// length of old line including NUL
    colnr_T	ستون;
    linenr_T	lnum = curwin->w_cursor.lnum;
    صحیح		i;

    // Break tabs if needed.
    اگر (virtual_فعال() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ستون = curwin->w_cursor.ستون;
    oldp = ml_get(lnum);
    linelen = (صحیح)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    اگر (State & REPLACE_FLAG)
    {
	اگر (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    صحیح		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    اگر (old_list && vim_strchr(p_cpo, CPO_LISTWM) == تہی)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, تہی, &vcol, تہی, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    جب (oldp[ستون + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + ستون + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		اگر (vcol > new_vcol && oldp[ستون + oldlen] == TAB)
		    توڑو;
		oldlen += (*mb_ptr2len)(oldp + ستون + oldlen);
		// Deleted a bit too much, insert spaces.
		اگر (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	ورنہ اگر (oldp[ستون] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + ستون);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_ڈالو(NUL);
	برائے (i = 0; i < oldlen; ++i)
	{
	    اگر (has_mbyte)
		i += replace_ڈالو_mb(oldp + ستون + i) - 1;
	    ورنہ
		replace_ڈالو(oldp[ستون + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    اگر (newp == تہی)
	واپس;

    // Copy bytes before the cursor.
    اگر (ستون > 0)
	mch_memmove(newp, oldp, (size_t)ستون);

    // Copy bytes after the changed character(s).
    p = newp + ستون;
    اگر (linelen > ستون + oldlen)
	mch_memmove(p + newlen, oldp + ستون + oldlen,
					    (size_t)(linelen - ستون - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    جب (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    بدلا_بائٹیں(lnum, ستون);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, ستون, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#اگر_ختم

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    اگر (p_sm && (State & منوال_داخل_کرو)
	    && پیغام_silent == 0
	    && !ins_compl_فعال())
    {
	اگر (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	ورنہ
	    showmatch(c);
    }

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (!p_ri || (State & REPLACE_FLAG))
#اگر_ختم
    {
	// Normal insert: move cursor right
	curwin->w_cursor.ستون += charlen;
    }

    // TODO: should try to update w_row here, to avoid recomputing it later.
}

/*
 * Insert a string at the cursor position.
 * Note: Does NOT handle Replace mode.
 * Caller must have prepared for undo.
 */
    خالی
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    صحیح		oldlen;
    colnr_T	ستون;
    linenr_T	lnum = curwin->w_cursor.lnum;

    اگر (virtual_فعال() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    ستون = curwin->w_cursor.ستون;
    oldp = ml_get(lnum);
    oldlen = (صحیح)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    اگر (newp == تہی)
	واپس;
    اگر (ستون > 0)
	mch_memmove(newp, oldp, (size_t)ستون);
    mch_memmove(newp + ستون, s, slen);
    mch_memmove(newp + ستون + slen, oldp + ستون, (size_t)(oldlen - ستون + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_بائٹیں(lnum, ستون, (صحیح)slen);
    curwin->w_cursor.ستون += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
del_char(صحیح fixpos)
{
    اگر (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	اگر (*ml_get_cursor() == NUL)
	    واپس FAIL;
	واپس del_chars(1L, fixpos);
    }
    واپس del_بائٹیں(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    صحیح
del_chars(طویل گنتی, صحیح fixpos)
{
    طویل	بائٹیں = 0;
    طویل	i;
    char_u	*p;
    صحیح		l;

    p = ml_get_cursor();
    برائے (i = 0; i < گنتی && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	بائٹیں += l;
	p += l;
    }
    واپس del_بائٹیں(بائٹیں, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
del_بائٹیں(
    طویل	گنتی,
    صحیح		fixpos_arg,
    صحیح		use_delcombine غیر_مستعمل)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ستون = curwin->w_cursor.ستون;
    صحیح		alloc_newp;
    طویل	movelen;
    صحیح		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (صحیح)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    اگر (ستون >= oldlen)
	واپس FAIL;

    // If "count" is zero there is nothing to do.
    اگر (گنتی == 0)
	واپس OK;

    // If "count" is negative the caller must be doing something wrong.
    اگر (گنتی < 1)
    {
	siemsg(e_invalid_گنتی_for_del_بائٹیں_nr, گنتی);
	واپس FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    اگر (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + ستون) >= گنتی)
    {
	صحیح	cc[MAX_MCO];
	صحیح	n;

	(خالی)utfc_ptr2char(oldp + ستون, cc);
	اگر (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = ستون;
	    کرو
	    {
		ستون = n;
		گنتی = utf_ptr2len(oldp + n);
		n += گنتی;
	    } جب (UTF_COMPOSINGLIKE(oldp + ستون, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (طویل)oldlen - (طویل)ستون - گنتی + 1; // includes trailing NUL
    اگر (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	اگر (ستون > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.ستون;
	    curwin->w_cursor.coladd = 0;
	    اگر (has_mbyte)
		curwin->w_cursor.ستون -=
			    (*mb_سر_off)(oldp, oldp + curwin->w_cursor.ستون);
	}
	گنتی = oldlen - ستون;
	movelen = 1;
    }
    newlen = oldlen - گنتی;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (netbeans_فعال())
	alloc_newp = TRUE;
    ورنہ
#اگر_ختم
	alloc_newp = !ml_سطر_alloced();    // check if oldp was allocated
    اگر (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    ورنہ
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	اگر (newp == تہی)
	    واپس FAIL;
	mch_memmove(newp, oldp, (size_t)ستون);
    }
    mch_memmove(newp + ستون, oldp + ستون + گنتی, (size_t)movelen);
    اگر (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// Also move any following text properties.
	اگر (oldlen + 1 < curbuf->b_ml.ml_سطر_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_سطر_len - oldlen - 1);
#اگر_ختم
	curbuf->b_ml.ml_سطر_len -= گنتی;
	curbuf->b_ml.ml_سطر_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_بائٹیں(lnum, ستون, -گنتی);

    واپس OK;
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
    صحیح
open_سطر(
    صحیح		dir,		// FORWARD or BACKWARD
    صحیح		flags,
    صحیح		دوسرا_سطر_indent,
    صحیح		*did_do_comment غیر_مستعمل)
{
    char_u	*saved_سطر;		// copy of the original line
    char_u	*اگلا_سطر = تہی;	// copy of the next line
    char_u	*p_extra = تہی;	// what goes to next line
    صحیح		less_ستون_تعداد = 0;		// less columns for mark in new line
    صحیح		less_ستون_تعداد_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    صحیح		newcol = 0;		// new cursor column
    صحیح		newindent = 0;		// auto-indent of the new line
    صحیح		n;
    صحیح		trunc_سطر = FALSE;	// truncate current line afterwards
    صحیح		retval = FAIL;		// return value
    صحیح		extra_len = 0;		// length of p_extra string
    صحیح		lead_len;		// length of comment leader
    صحیح		comment_آغاز = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = تہی;		// copy of comment leader
    char_u	*allocated = تہی;	// allocated memory
    char_u	*p;
    صحیح		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    صحیح		do_cindent;
    صحیح		do_si = may_do_si();
    صحیح		no_si = FALSE;		// reset did_si afterwards
    صحیح		پہلا_char = NUL;	// init for GCC
    صحیح		vreplace_منوال;
    صحیح		did_append;		// appended a new line
    صحیح		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		at_eol;			// cursor after last character
#اگر_ختم

    // make a copy of the current line so we can mess with it
    saved_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    اگر (saved_سطر == تہی)	    // out of memory!
	واپس FALSE;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.ستون >= (صحیح)ml_get_curline_len();
#اگر_ختم

    اگر (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	اگر (curwin->w_cursor.lnum < orig_سطر_گنتی)
	    اگلا_سطر = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	ورنہ
	    اگلا_سطر = vim_strsave((char_u *)"");
	اگر (اگلا_سطر == تہی)	    // out of memory!
	    جاؤ theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_ڈالو(NUL);  // Call twice because BS over NL expects it
	replace_ڈالو(NUL);
	p = saved_سطر + curwin->w_cursor.ستون;
	جب (*p != NUL)
	{
	    اگر (has_mbyte)
		p += replace_ڈالو_mb(p);
	    ورنہ
		replace_ڈالو(*p++);
	}
	saved_سطر[curwin->w_cursor.ستون] = NUL;
    }

    اگر ((State & منوال_داخل_کرو) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_سطر + curwin->w_cursor.ستون;
	اگر (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    پہلا_char = *p;
	}
	extra_len = (صحیح)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_ستون = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    اگر (dir == FORWARD && did_ai)
	trunc_سطر = TRUE;

    اگر ((flags & OPENLINE_FORCE_INDENT) && دوسرا_سطر_indent >= 0)
	newindent = دوسرا_سطر_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    ورنہ اگر (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#اگر_تعریف_شدہ FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_سطر, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ورنہ
	newindent = get_indent_str(saved_سطر, (صحیح)curbuf->b_p_ts, FALSE);
#اگر_ختم
	اگر (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = دوسرا_سطر_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	اگر (!trunc_سطر && do_si && *saved_سطر != NUL
				    && (p_extra == تہی || پہلا_char != '{'))
	{
	    char_u  *ptr;
	    char_u  آخری_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_سطر;
	    اگر (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, تہی, FALSE, TRUE);
	    ورنہ
		lead_len = 0;
	    اگر (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		اگر ( lead_len == 0 && ptr[0] == '#')
		{
		    جب (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		اگر (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, تہی, FALSE, TRUE);
		ورنہ
		    lead_len = 0;
		اگر (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    اگر (p[0] == '/' && p[1] == '*')
			p++;
		    اگر (p[0] == '*')
		    {
			برائے (p++; *p; p++)
			{
			    اگر (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.ستون = (colnr_T)(p - ptr);
				اگر ((pos = findmatch(تہی, NUL)) != تہی)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    توڑو;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.ستون;
			    }
			}
		    }
		}
		ورنہ	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    جب (p > ptr && VIM_ISWHITE(*p))
			--p;
		    آخری_char = *p;

		    // find the character just before the '{' or ';'
		    اگر (آخری_char == '{' || آخری_char == ';')
		    {
			اگر (p > ptr)
			    --p;
			جب (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    اگر (*p == ')')
		    {
			curwin->w_cursor.ستون = (colnr_T)(p - ptr);
			اگر ((pos = findmatch(تہی, '(')) != تہی)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    اگر (آخری_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    ورنہ اگر (آخری_char != ';' && آخری_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    ورنہ // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		اگر (lead_len == 0 && ptr[0] == '#')
		{
		    صحیح was_backslashed = FALSE;

		    جب ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
		    {
			اگر (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			ورنہ
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    اگر (was_backslashed)
			newindent = 0;	    // Got to end of file
		    ورنہ
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		اگر (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		ورنہ		    // can delete indent when '{' typed
		    can_si_عقب = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	اگر (do_si)
	    can_si = TRUE;

	did_ai = TRUE;
    }

    // May do indenting after opening a new line.
    do_cindent = !p_paste && (curbuf->b_p_cin
#اگر_تعریف_شدہ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#اگر_ختم
		)
	    && اندر_cinkeys(dir == FORWARD
		? کلید_OPEN_FORW
		: کلید_OPEN_عقب, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    انتہا_comment_pending = NUL;
    اگر (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_سطر, &lead_flags,
							dir == BACKWARD, TRUE);
	اگر (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_آغاز = check_linecomment(saved_سطر);
	    اگر (comment_آغاز != MAXCOL)
	    {
		lead_len = get_leader_len(saved_سطر + comment_آغاز,
						     &lead_flags, FALSE, TRUE);
		اگر (lead_len != 0)
		{
		    lead_len += comment_آغاز;
		    اگر (did_do_comment != تہی)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    ورنہ
	lead_len = 0;
    اگر (lead_len > 0)
    {
	char_u	*lead_repl = تہی;	    // replaces comment leader
	صحیح	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_درمیان[COM_MAX_LEN];   // middle-comment string
	صحیح	lead_درمیان_len;	    // length of the lead_middle
	char_u	lead_انتہا[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_انتہا = تہی;	    // where lead_end has been found
	صحیح	extra_space = FALSE;	    // append extra space
	صحیح	current_flag;
	صحیح	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	برائے (p = lead_flags; *p && *p != ':'; ++p)
	{
	    اگر (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		جاری;
	    }
	    اگر (*p == COM_آغاز || *p == COM_درمیان)
	    {
		current_flag = *p;
		اگر (*p == COM_آغاز)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    اگر (dir == BACKWARD)
		    {
			lead_len = 0;
			توڑو;
		    }

		    // find start of middle part
		    (خالی)copy_option_part(&p, lead_درمیان, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		جب (*p && p[-1] != ':')	// find end of middle flags
		{
		    اگر (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_درمیان_len = copy_option_part(&p, lead_درمیان, COM_MAX_LEN, ",");

		جب (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    اگر (*p == COM_AUTO_انتہا)
			انتہا_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_انتہا, COM_MAX_LEN, ",");

		اگر (انتہا_comment_pending == -1)	// we can set it now
		    انتہا_comment_pending = lead_انتہا[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		اگر (dir == FORWARD)
		{
		    برائے (p = saved_سطر + lead_len; *p; ++p)
			اگر (STRNCMP(p, lead_انتہا, n) == 0)
			{
			    comment_انتہا = p;
			    lead_len = 0;
			    توڑو;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		اگر (lead_len > 0)
		{
		    اگر (current_flag == COM_آغاز)
		    {
			lead_repl = lead_درمیان;
			lead_repl_len = lead_درمیان_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    اگر (!VIM_ISWHITE(saved_سطر[lead_len - 1])
			    && ((p_extra != تہی
				    && (صحیح)curwin->w_cursor.ستون == lead_len)
				|| (p_extra == تہی
				    && saved_سطر[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		توڑو;
	    }
	    اگر (*p == COM_انتہا)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		اگر (dir == FORWARD)
		{
		    comment_انتہا = skipwhite(saved_سطر);
		    lead_len = 0;
		    توڑو;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		جب (p > curbuf->b_p_com && *p != ',')
		    --p;
		برائے (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (صحیح)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		برائے (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    اگر (*p2 == COM_AUTO_انتہا)
			انتہا_comment_pending = -1; // means we want to set it
		}
		اگر (انتہا_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    جب (*p2 && *p2 != ',')
			p2++;
		    انتہا_comment_pending = p2[-1];
		}
		توڑو;
	    }
	    اگر (*p == COM_پہلا)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		اگر (dir == BACKWARD)
		    lead_len = 0;
		ورنہ
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		توڑو;
	    }
	}
	اگر (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (دوسرا_سطر_indent > 0 ? دوسرا_سطر_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    اگر (leader == تہی)
		lead_len = 0;
	    ورنہ
	    {
		صحیح li;

		vim_strncpy(leader, saved_سطر, lead_len);

		// TODO: handle multi-byte and double width chars
		برائے (li = 0; li < comment_آغاز; ++li)
		    اگر (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		اگر (lead_repl != تہی)
		{
		    صحیح		c = 0;
		    صحیح		off = 0;

		    برائے (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			اگر (*p == COM_دایاں || *p == COM_بایاں)
			    c = *p++;
			ورنہ اگر (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			ورنہ
			    ++p;
		    }
		    اگر (c == COM_دایاں)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			برائے (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    صحیح	    repl_سائز = vim_strnsize(lead_repl,
							       lead_repl_len);
			    صحیح	    old_سائز = 0;
			    char_u  *endp = p;
			    صحیح	    l;

			    جب (old_سائز < repl_سائز && p > leader)
			    {
				MB_PTR_عقب(leader, p);
				old_سائز += ptr2cells(p);
			    }
			    l = lead_repl_len - (صحیح)(endp - p);
			    اگر (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			اگر (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			جب (--p >= leader)
			{
			    صحیح l = mb_سر_off(leader, p);

			    اگر (l > 1)
			    {
				p -= l;
				اگر (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    ورنہ اگر (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    ورنہ	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    صحیح	    repl_سائز = vim_strnsize(lead_repl,
							       lead_repl_len);
			    صحیح	    i;
			    صحیح	    l;

			    برائے (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				اگر (vim_strnsize(p, i + l) > repl_سائز)
				    توڑو;
			    }
			    اگر (i != lead_repl_len)
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
			برائے (p += lead_repl_len; p < leader + lead_len; ++p)
			    اگر (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				اگر (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				ورنہ
				{
				    صحیح	    l = (*mb_ptr2len)(p);

				    اگر (l > 1)
				    {
					اگر (ptr2cells(p) > 1)
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
		    اگر (curbuf->b_p_ai || do_si)
#اگر_تعریف_شدہ FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ورنہ
			newindent = get_indent_str(leader,
						   (صحیح)curbuf->b_p_ts, FALSE);
#اگر_ختم

		    // Add the indent offset
		    اگر (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    ورنہ
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    جب (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			اگر (vim_strchr(skipwhite(leader), '\t') != تہی)
			    توڑو;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    اگر (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		اگر (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		اگر (newindent || did_si)
		{
		    جب (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	ورنہ اگر (comment_انتہا != تہی)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    اگر (comment_انتہا[0] == '*' && comment_انتہا[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.ستون = (colnr_T)(comment_انتہا - saved_سطر);
		اگر ((pos = findmatch(تہی, NUL)) != تہی)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    اگر (p_extra != تہی)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	اگر (REPLACE_معمول(State))
	    replace_ڈالو(NUL);	    // end of extra blanks
	اگر (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    جب ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		اگر (REPLACE_معمول(State))
		    replace_ڈالو(*p_extra);
		++p_extra;
		++less_ستون_تعداد_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_ستون_تعداد = (صحیح)(p_extra - saved_سطر);
    }

    اگر (p_extra == تہی)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    اگر (lead_len)
    {
	اگر (flags & OPENLINE_COM_LIST && دوسرا_سطر_indent > 0)
	{
	    صحیح i;
	    صحیح padding = دوسرا_سطر_indent
					  - (newindent + (صحیح)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    برائے (i = 0; i < padding; i++)
	    {
		STRCAT(leader, " ");
		less_ستون_تعداد--;
		newcol++;
	    }
	}
	STRCAT(leader, p_extra);
	p_extra = leader;
	did_ai = TRUE;	    // So truncating blanks works with comments
	less_ستون_تعداد -= lead_len;
    }
    ورنہ
	انتہا_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    اگر (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    اگر (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_سطر_گنتی)
    {
	اگر (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    جاؤ theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر ((State & منوال_داخل_کرو) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_بانٹو(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.ستون + 1, 0, at_eol);
#اگر_ختم
    }
    ورنہ
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	اگر (curwin->w_cursor.lnum >= Insstart.lnum + vr_سطریں_بدلا)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (خالی)u_save_cursor();		    // errors are ignored!
	    vr_سطریں_بدلا++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	بدلا_بائٹیں(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    اگر (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	اگر (did_si)
	{
	    صحیح sw = (صحیح)get_sw_قدر(curbuf);

	    اگر (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	اگر (curbuf->b_p_ci)
	{
	    (خالی)copy_indent(newindent, saved_سطر);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	ورنہ
	    (خالی)سیٹ_indent(newindent, SIN_داخل_کرو);
	less_ستون_تعداد -= curwin->w_cursor.ستون;

	ai_ستون = curwin->w_cursor.ستون;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	اگر (REPLACE_معمول(State))
	    برائے (n = 0; n < (صحیح)curwin->w_cursor.ستون; ++n)
		replace_ڈالو(NUL);
	newcol += curwin->w_cursor.ستون;
	اگر (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    اگر (REPLACE_معمول(State))
	جب (lead_len-- > 0)
	    replace_ڈالو(NUL);

    curwin->w_cursor = old_cursor;

    اگر (dir == FORWARD)
    {
	اگر (trunc_سطر || (State & منوال_داخل_کرو))
	{
	    // truncate current line at cursor
	    saved_سطر[curwin->w_cursor.ستون] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    اگر (trunc_سطر && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_سطر, curwin->w_cursor.ستون);
	    ml_replace(curwin->w_cursor.lnum, saved_سطر, FALSE);
	    saved_سطر = تہی;
	    اگر (did_append)
	    {
		بدلا_سطریں(curwin->w_cursor.lnum, curwin->w_cursor.ستون,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		اگر (flags & OPENLINE_MARKFIX)
		    mark_ستون_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.ستون + less_ستون_تعداد_off,
						      1L, (طویل)-less_ستون_تعداد, 0);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		اگر (curbuf->b_has_textprop && less_ستون_تعداد_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_ستون_تعداد_off, 0);
#اگر_ختم
	    }
	    ورنہ
		بدلا_بائٹیں(curwin->w_cursor.lnum, curwin->w_cursor.ستون);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    اگر (did_append)
	بدلا_سطریں(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.ستون = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    اگر (State & VREPLACE_FLAG)
    {
	vreplace_منوال = State;	// So we know to put things right later
	State = منوال_داخل_کرو;
    }
    ورنہ
	vreplace_منوال = 0;

    اگر (!p_paste)
    {
	اگر (leader == تہی
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_ستون = (colnr_T)getwhitecols_curline();
	}
	ورنہ اگر (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_اظہاریہ_indent();
	    ai_ستون = (colnr_T)getwhitecols_curline();
	}
    }

    اگر (vreplace_منوال != 0)
	State = vreplace_منوال;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    اگر (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	اگر (p_extra == تہی)
	    جاؤ theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, اگلا_سطر, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.ستون = 0;
	curwin->w_cursor.coladd = 0;
	ins_بائٹیں(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	اگلا_سطر = تہی;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_سطر);
    vim_free(اگلا_سطر);
    vim_free(allocated);
    واپس retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
truncate_سطر(صحیح fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	ستون = curwin->w_cursor.ستون;
    char_u	*old_سطر;
    صحیح		deleted;

    old_سطر = ml_get(lnum);
    اگر (ستون == 0)
	newp = vim_strsave((char_u *)"");
    ورنہ
	newp = vim_strnsave(old_سطر, ستون);
    deleted = (صحیح)ml_get_len(lnum) - ستون;

    اگر (newp == تہی)
	واپس FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_بائٹیں(lnum, curwin->w_cursor.ستون, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    اگر (fixpos && curwin->w_cursor.ستون > 0)
	--curwin->w_cursor.ستون;

    واپس OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    خالی
del_سطریں(طویل nlines, صحیح undo)
{
    طویل	n;
    linenr_T	پہلا = curwin->w_cursor.lnum;

    اگر (nlines <= 0)
	واپس;

    // save the deleted lines for undo
    اگر (undo && u_savedel(پہلا, nlines) == FAIL)
	واپس;

    برائے (n = 0; n < nlines; )
    {
	اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)	    // nothing to delete
	    توڑو;

	ml_delete_flags(پہلا, ML_DEL_پیغام);
	++n;

	// If we delete the last line in the file, stop
	اگر (پہلا > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.ستون = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_سطریں_mark(پہلا, n);
}
