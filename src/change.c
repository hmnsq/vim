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

#అన్తర్భావ "vim.h"

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
    శూన్య
change_warning(పూర్ణాంకం నిలువు)
{
    స్థిర అక్షరం *w_readonly = N_("W10: Warning: Changing a readonly file");

    ఒకవేళ (curbuf->b_did_warn
	    || curbuf_Is_మారింది()
	    || autocmd_busy
	    || !curbuf->b_p_ro)
	తిరిగి;

    ++curbuf_తాళం;
    apply_autocmds(EVENT_FILECHANGEDRO, ఖాళీ, ఖాళీ, FALSE, curbuf);
    --curbuf_తాళం;
    ఒకవేళ (!curbuf->b_p_ro)
	తిరిగి;

    // Do what msg() does, but with a column offset if the warning should
    // be after the mode message.
    సందేశం_ప్రారంభం();
    ఒకవేళ (సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1)
	సందేశం_నిలువు = నిలువు;
    సందేశం_ఆధారం(HL_ATTR(HLF_W));
    సందేశం_puts_attr(_(w_readonly), HL_ATTR(HLF_W) | సందేశం_HIST);
#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_readonly), -1);
#అయితే_ముగింపు
    సందేశం_clr_eos();
    (శూన్య)సందేశం_చివర();
    ఒకవేళ (సందేశం_silent == 0 && !silent_బహుళకం
#నిర్వచితమైతే FEAT_EVAL
	    && సమయం_for_testing != 1
#అయితే_ముగింపు
       )
    {
	బహిర్గతం_flush();
	ui_delay(1002L, TRUE); // give the user time to think about it
    }
    curbuf->b_did_warn = true;
    redraw_cmdline = FALSE;	// don't redraw and erase the message
    ఒకవేళ (సందేశం_అడ్డువరుస < అడ్డువరుసలు - 1)
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
    శూన్య
మారింది(శూన్య)
{
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
    {
	// The text of the preediting area is inserted, but this doesn't
	// mean a change of the buffer yet.  That is delayed until the
	// text is committed. (this means preedit becomes empty)
	ఒకవేళ (im_is_preediting() && !xim_మారింది_while_preediting)
	    తిరిగి;
	xim_మారింది_while_preediting = FALSE;
    }
#అయితే_ముగింపు

    ఒకవేళ (!curbuf->b_మారింది)
    {
	పూర్ణాంకం	save_సందేశం_scroll = సందేశం_scroll;

	// Give a warning about changing a read-only file.  This may also
	// check-out the file, thus change "curbuf"!
	change_warning(0);

	// Create a swap file if that is wanted.
	// Don't do this for "nofile" and "nowrite" buffer types.
	ఒకవేళ (curbuf->b_may_మార్పిడి && !bt_dontwrite(curbuf))
	{
	    పూర్ణాంకం save_need_wait_return = need_wait_return;

	    need_wait_return = FALSE;
	    ml_open_file(curbuf);

	    // The ml_open_file() can cause an ATTENTION message.
	    // Wait two seconds, to make sure the user reads this unexpected
	    // message.  Since we could be anywhere, call wait_return() now,
	    // and don't let the emsg() set msg_scroll.
	    ఒకవేళ (need_wait_return && emsg_silent == 0 && !లోపల_assert_fails)
	    {
		బహిర్గతం_flush();
		ui_delay(2002L, TRUE);
		wait_return(TRUE);
		సందేశం_scroll = save_సందేశం_scroll;
	    }
	    లేకపోతే
		need_wait_return = save_need_wait_return;
	}
	మారింది_అంతర్గత();
    }
    ++CHANGEDTICK(curbuf);

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    // If a pattern is highlighted, the position may now be invalid.
    highlight_సరిపోలిక = FALSE;
#అయితే_ముగింపు
}

/*
 * Internal part of changed(), no user interaction.
 * Also used for recovery.
 */
    శూన్య
మారింది_అంతర్గత(శూన్య)
{
    curbuf->b_మారింది = TRUE;
    ml_setflags(curbuf);
    check_స్థితి(curbuf);
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    need_maketitle = TRUE;	    // set window title later
}

#నిర్వచితమైతే FEAT_EVAL
// Set when listener callbacks are being invoked.
స్థిర పూర్ణాంకం recursive = FALSE;

స్థిర దీర్ఘ తదుపరి_listener_id = 0;

// A flag that is set when any buffer listener housekeeping is required.
// Currently the only condition is when a listener is marked for removal.
స్థిర bool housekeeping_required;

/*
 * Remove a given listener_T entry from its containing list.
 */
    స్థిర శూన్య
remove_listener_from_list(
    listener_T **list,
    listener_T *lnr,
    listener_T *prev)
{
    ఒకవేళ (prev != ఖాళీ)
	prev->lr_తదుపరి = lnr->lr_తదుపరి;
    లేకపోతే
	*list = lnr->lr_తదుపరి;
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
    స్థిర శూన్య
clean_listener_list(buf_T *buf, listener_T **list, bool all)
{
    listener_T	*prev;
    listener_T	*lnr;
    listener_T	*తదుపరి;

    prev = ఖాళీ;
    కొరకు (lnr = *list; lnr != ఖాళీ; lnr = తదుపరి)
    {
	తదుపరి = lnr->lr_తదుపరి;
	ఒకవేళ (all || lnr->lr_id == 0)
	    remove_listener_from_list(list, lnr, prev);
	లేకపోతే
	    prev = lnr;
    }

    // Drop any recorded changes for a buffer with no listeners.
    ఒకవేళ (buf != ఖాళీ)
    {
	ఒకవేళ (*list == ఖాళీ && buf->b_recorded_changes != ఖాళీ)
	{
	    list_unref(buf->b_recorded_changes);
	    buf->b_recorded_changes = ఖాళీ;
	    buf->b_recorded_పాఠ్యం_కొలత = 0;
	}
    }
}

/*
 * Perform housekeeping tasks for buffer change listeners.
 *
 * This does nothing unless the "housekeeping_required" flag has been set.
 */
    స్థిర శూన్య
perform_listener_housekeeping(శూన్య)
{
    buf_T	*buf;

    ఒకవేళ (housekeeping_required)
    {
	FOR_ALL_BUFFERS(buf)
	{
	    clean_listener_list(buf, &buf->b_listener, FALSE);
	    clean_listener_list(ఖాళీ, &buf->b_sync_listener, FALSE);
	}
	housekeeping_required = FALSE;
    }
}

/*
 * Check if the change at "lnum" is above or overlaps with an existing
 * change. If above then flush changes and invoke listeners.
 */
    స్థిర శూన్య
check_recorded_changes(
	buf_T		*buf,
	linenr_T	lnum,
	linenr_T	lnume,
	దీర్ఘ		xtra)
{
    perform_listener_housekeeping();
    ఒకవేళ (buf->b_recorded_changes == ఖాళీ || xtra == 0)
	తిరిగి;

    listitem_T *li;
    linenr_T	prev_lnum;
    linenr_T	prev_lnume;

    FOR_ALL_LIST_వస్తువులు(buf->b_recorded_changes, li)
    {
	prev_lnum = (linenr_T)dict_get_సంఖ్య(
		li->li_tv.vval.v_dict, "lnum");
	prev_lnume = (linenr_T)dict_get_సంఖ్య(
		li->li_tv.vval.v_dict, "end");
	ఒకవేళ (prev_lnum >= lnum || prev_lnum > lnume || prev_lnume >= lnum)
	{
	    // the current change is going to make the line number in
	    // the older change invalid, flush now
	    invoke_listeners(curbuf);
	    ఆపు;
	}
    }
}

// Amount of recorded text after which the listeners are invoked, to bound the
// memory used by a long sequence of changes.
# define LISTENER_పాఠ్యం_MAX (4 * 1024 * 1024)

/*
 * Return true when any listener in "list" asked for the resulting text.
 */
    స్థిర bool
listeners_want_పాఠ్యం(listener_T *list)
{
    listener_T	*lnr;

    కొరకు (lnr = list; lnr != ఖాళీ; lnr = lnr->lr_తదుపరి)
	ఒకవేళ (lnr->lr_పాఠ్యం)
	    తిరిగి true;
    తిరిగి false;
}

/*
 * Store in "dict" the text that occupies the changed region right after the
 * change: the lines from "lnum" up to but not including "lnume" + "xtra".
 * Returns the number of bytes stored.
 */
    స్థిర size_t
కూడు_change_పాఠ్యం(
    buf_T	*buf,
    dict_T	*dict,
    linenr_T	lnum,
    linenr_T	lnume,
    దీర్ఘ	xtra)
{
    list_T	*l = list_alloc();
    linenr_T	క్రింద = lnume + xtra;	// line below the changed region
    size_t	కొలత = 0;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (క్రింద > buf->b_ml.ml_పంక్తి_లెక్క + 1)
	క్రింద = buf->b_ml.ml_పంక్తి_లెక్క + 1;
    కొరకు (linenr_T lp = lnum < 1 ? 1 : lnum; lp < క్రింద; ++lp)
    {
	char_u	*పంక్తి = ml_get_buf(buf, lp, FALSE);
	colnr_T	len = ml_get_buf_len(buf, lp);

	// Rather than storing a part of the text, store none of it.
	ఒకవేళ (list_append_string(l, పంక్తి, len) == FAIL)
	{
	    list_free(l);
	    తిరిగి 0;
	}
	కొలత += (size_t)len + 1;
    }

    ఒకవేళ (dict_కూడు_list(dict, "text", l) == FAIL)
    {
	list_free(l);
	తిరిగి 0;
    }
    తిరిగి కొలత;
}

/*
 * Record a change for listeners added with listener_add().
 * Always for the current buffer.
 *
 * This only deals with listeners that are prepared to accept multiple buffered
 * changes.
 */
    స్థిర శూన్య
may_రికార్డు_change(
    linenr_T	lnum,
    colnr_T	నిలువు,
    linenr_T	lnume,
    దీర్ఘ	xtra)
{
    dict_T	*dict;

    perform_listener_housekeeping();
    ఒకవేళ (curbuf->b_listener == ఖాళీ)
	తిరిగి;

    // If the new change is going to change the line numbers in already listed
    // changes, then flush.
    check_recorded_changes(curbuf, lnum, lnume, xtra);

    ఒకవేళ (curbuf->b_recorded_changes == ఖాళీ)
    {
	curbuf->b_recorded_changes = list_alloc();
	ఒకవేళ (curbuf->b_recorded_changes == ఖాళీ)  // out of memory
	    తిరిగి;
	++curbuf->b_recorded_changes->lv_refcount;
    }

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి;
    dict_కూడు_సంఖ్య(dict, "lnum", (varnumber_T)lnum);
    dict_కూడు_సంఖ్య(dict, "end", (varnumber_T)lnume);
    dict_కూడు_సంఖ్య(dict, "added", (varnumber_T)xtra);
    dict_కూడు_సంఖ్య(dict, "col", (varnumber_T)నిలువు + 1);
    ఒకవేళ (listeners_want_పాఠ్యం(curbuf->b_listener))
	curbuf->b_recorded_పాఠ్యం_కొలత +=
			  కూడు_change_పాఠ్యం(curbuf, dict, lnum, lnume, xtra);

    list_append_dict(curbuf->b_recorded_changes, dict);

    // Invoking the listeners resets the size, do not try while they are busy.
    ఒకవేళ (!recursive && curbuf->b_recorded_పాఠ్యం_కొలత > LISTENER_పాఠ్యం_MAX)
	invoke_listeners(curbuf);
}

/*
 * listener_add() function
 */
    శూన్య
f_listener_కూడు(typval_T *argvars, typval_T *rettv)
{
    callback_T	callback;
    listener_T	*lnr;
    buf_T	*buf = curbuf;
    bool	unbuffered = false;
    bool	want_పాఠ్యం = false;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (recursive)
    {
	emsg(_(e_cannot_కూడు_listener_లోపల_listener_callback));
	తిరిగి;
    }

    ఒకవేళ (లోపల_vim9script() && (
	    check_for_opt_బఫరు_arg(argvars, 1) == FAIL
	    || (argvars[1].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_or_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    callback = get_callback(&argvars[0]);
    ఒకవేళ (callback.cb_name == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[1]);
	ఒకవేళ (buf == ఖాళీ)
	{
	    free_callback(&callback);
	    తిరిగి;
	}
	ఒకవేళ (argvars[2].v_type == VAR_DICT)
	{
	    dict_T *d = argvars[2].vval.v_dict;

	    unbuffered = dict_get_bool(d, "unbuffered", false);
	    want_పాఠ్యం = dict_get_bool(d, "text", false);
	}
	లేకపోతే ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    unbuffered = tv_get_bool(&argvars[2]);
    }

    lnr = ALLOC_CLEAR_ONE(listener_T);
    ఒకవేళ (lnr == ఖాళీ)
    {
	free_callback(&callback);
	తిరిగి;
    }

    // Perform any pending housekeeping and then make sure any buffered change
    // reports are flushed so that the new listener does not see out of date
    // changes.
    perform_listener_housekeeping();
    invoke_listeners(buf);

    ఒకవేళ (unbuffered)
    {
	lnr->lr_తదుపరి = buf->b_sync_listener;
	buf->b_sync_listener = lnr;
    }
    లేకపోతే
    {
	lnr->lr_తదుపరి = buf->b_listener;
	buf->b_listener = lnr;
    }

    సమితి_callback(&lnr->lr_callback, &callback);

    lnr->lr_పాఠ్యం = want_పాఠ్యం;
    lnr->lr_id = ++తదుపరి_listener_id;
    rettv->vval.v_సంఖ్య = lnr->lr_id;
}

/*
 * listener_flush() function
 */
    శూన్య
f_listener_flush(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf = curbuf;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (recursive)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	buf = get_buf_arg(&argvars[0]);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి;
    }
    perform_listener_housekeeping();
    invoke_listeners(buf);
}

/*
 * Find the buffer change listener entry for a given unique ID.
 */
    స్థిర listener_T *
కనుగొను_listener(
    పూర్ణాంకం        id,
    listener_T *list_ప్రారంభం,
    listener_T **prev)
{
    listener_T *తదుపరి;
    listener_T *lnr;

    *prev = ఖాళీ;
    కొరకు (lnr = list_ప్రారంభం; lnr != ఖాళీ; lnr = తదుపరి)
    {
	తదుపరి = lnr->lr_తదుపరి;
	ఒకవేళ (lnr->lr_id == id)
	    తిరిగి lnr;
	*prev = lnr;
    }
    తిరిగి ఖాళీ;
}

/*
 * listener_remove() function
 *
 * This simply marks the listener_T entry as unused, by setting its ID to zero.
 * The listener_T entry gets removed later by housekeeping.
 */
    శూన్య
f_listener_remove(typval_T *argvars, typval_T *rettv)
{
    listener_T	*lnr;
    listener_T	*prev;
    పూర్ణాంకం		id;
    buf_T	*buf;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = tv_get_సంఖ్య(argvars);
    FOR_ALL_BUFFERS(buf)
    {
	lnr = కనుగొను_listener(id, buf->b_listener, &prev);
	ఒకవేళ (lnr == ఖాళీ)
	    lnr = కనుగొను_listener(id, buf->b_sync_listener, &prev);
	ఒకవేళ (lnr != ఖాళీ)
	{
	    // Clear the ID to indicate that the listener is unused flag
	    // housekeeping.
	    lnr->lr_id = 0;
	    housekeeping_required = TRUE;
	    rettv->vval.v_సంఖ్య = 1;
	    తిరిగి;
	}
    }
}

/*
 * Called before inserting a line above "lnum"/"lnume" or deleting line "lnum"
 * to "lnume".
 */
    శూన్య
may_invoke_listeners(buf_T *buf, linenr_T lnum, linenr_T lnume, పూర్ణాంకం added)
{
    check_recorded_changes(buf, lnum, lnume, added);
}

/*
 * Common processing for invoke_listeners and invoke_sync_listeners.
 */
    స్థిర శూన్య
invoke_listener_సమితి(
    buf_T       *buf,
    linenr_T	ప్రారంభం,
    linenr_T	చివర,
    దీర్ఘ	added,
    list_T      *recorded_changes,
    listener_T  *listeners)
{
    పూర్ణాంకం		save_updating_screen = updating_screen;
    listener_T	*lnr;
    typval_T	rettv;
    typval_T	argv[6];
    typval_T    val;

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = buf->b_fnum; // a:bufnr
    argv[1].v_type = VAR_సంఖ్య;
    argv[1].vval.v_సంఖ్య = ప్రారంభం;
    argv[2].v_type = VAR_సంఖ్య;
    argv[2].vval.v_సంఖ్య = చివర;
    argv[3].v_type = VAR_సంఖ్య;
    argv[3].vval.v_సంఖ్య = added;
    argv[4].v_type = VAR_LIST;
    argv[4].vval.v_list = recorded_changes;

    // Ensure the list of changes is locked to prevent any modifications by
    // callback code..
    val.v_type = VAR_LIST;
    val.v_తాళం = 0;
    val.vval.v_list = recorded_changes;
    అంశం_తాళం(&val, -1, TRUE, FALSE);

    // Protect against recursive callbacks, lock the buffer against changes and
    // set the updating_screen flag to prevent channel input processing, which
    // might also try to update the buffer.
    recursive = TRUE;
    ++textlock;
    updating_screen = TRUE;

    కొరకు (lnr = listeners; lnr != ఖాళీ; lnr = lnr->lr_తదుపరి)
    {
	call_callback(&lnr->lr_callback, -1, &rettv, 5, argv);
	clear_tv(&rettv);
    }

    --textlock;
    ఒకవేళ (save_updating_screen)
	updating_screen = TRUE;
    లేకపోతే
	after_updating_screen(TRUE);
    recursive = FALSE;
}

/*
 * Called when any change occurs: invoke listeners added with the "unbuffered"
 * parameter set.
 */
    స్థిర శూన్య
invoke_sync_listeners(
    buf_T	*buf,
    linenr_T	ప్రారంభం,
    colnr_T	నిలువు,
    linenr_T	చివర,
    దీర్ఘ	added)
{
    list_T      *recorded_changes;
    dict_T	*dict;

    ఒకవేళ (recursive || curbuf->b_sync_listener == ఖాళీ)
	తిరిగి;

    // Create a single entry list to store the details of the change (including
    // the column).
    recorded_changes = list_alloc();
    ఒకవేళ (recorded_changes == ఖాళీ)  // out of memory
	తిరిగి;

    ++recorded_changes->lv_refcount;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
    {
	list_unref(recorded_changes);
	తిరిగి;
    }

    dict_కూడు_సంఖ్య(dict, "lnum", (varnumber_T)ప్రారంభం);
    dict_కూడు_సంఖ్య(dict, "end", (varnumber_T)చివర);
    dict_కూడు_సంఖ్య(dict, "added", (varnumber_T)added);
    dict_కూడు_సంఖ్య(dict, "col", (varnumber_T)నిలువు + 1);
    ఒకవేళ (listeners_want_పాఠ్యం(buf->b_sync_listener))
	(శూన్య)కూడు_change_పాఠ్యం(buf, dict, ప్రారంభం, చివర, added);
    list_append_dict(recorded_changes, dict);

    invoke_listener_సమితి(
	buf, ప్రారంభం, చివర, added, recorded_changes, buf->b_sync_listener);

    list_unref(recorded_changes);
}

/*
 * Called when a sequence of changes is done: invoke listeners added with
 * listener_add().
 */
    శూన్య
invoke_listeners(buf_T *buf)
{
    listitem_T	*li;
    linenr_T	ప్రారంభం = MAXLNUM;
    linenr_T	చివర = 0;
    linenr_T	added = 0;

    ఒకవేళ (buf->b_recorded_changes == ఖాళీ  // nothing changed
	    || buf->b_listener == ఖాళీ	 // no listeners
	    || recursive)		 // already busy
	తిరిగి;

    FOR_ALL_LIST_వస్తువులు(buf->b_recorded_changes, li)
    {
	varnumber_T lnum;

	lnum = dict_get_సంఖ్య(li->li_tv.vval.v_dict, "lnum");
	ఒకవేళ (ప్రారంభం > lnum)
	    ప్రారంభం = lnum;
	lnum = dict_get_సంఖ్య(li->li_tv.vval.v_dict, "end");
	ఒకవేళ (చివర < lnum)
	    చివర = lnum;
	added += dict_get_సంఖ్య(li->li_tv.vval.v_dict, "added");
    }

    invoke_listener_సమితి(
	buf, ప్రారంభం, చివర, added, buf->b_recorded_changes, buf->b_listener);

    list_unref(buf->b_recorded_changes);
    buf->b_recorded_changes = ఖాళీ;
    buf->b_recorded_పాఠ్యం_కొలత = 0;
}

/*
 * Remove all listeners associated with "buf".
 */
    శూన్య
remove_listeners(buf_T *buf)
{
    clean_listener_list(buf, &buf->b_listener, TRUE);
    clean_listener_list(ఖాళీ, &buf->b_sync_listener, TRUE);
}

#అయితే_ముగింపు

/*
 * Common code for when a change was made.
 * See changed_lines() for the arguments.
 * Careful: may trigger autocommands that reload the buffer.
 */
    స్థిర శూన్య
మారింది_common(
    linenr_T	lnum,
    colnr_T	నిలువు,
    linenr_T	lnume,
    దీర్ఘ	xtra)
{
    win_T	*wp;
    tabpage_T	*tp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		నిలువుల_సంఖ్య;
    pos_T	*p;
    పూర్ణాంకం		కూడు;

    // mark the buffer as modified
    మారింది();

#నిర్వచితమైతే FEAT_EVAL
    // Immediately send this change to any listeners that require changes not
    // to be buffered.
    invoke_sync_listeners(curbuf, lnum, నిలువు, lnume, xtra);

    // If there are any listeners accepting buffered changes then add changes
    // to the current buffer's list, flushing previous changes first if necessary.
    may_రికార్డు_change(lnum, నిలువు, lnume, xtra);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (curwin->w_p_diff && diff_అంతర్గత())
    {
	curtab->tp_diff_update = TRUE;
	diff_update_పంక్తి(lnum);
    }
#అయితే_ముగింపు

    // set the '. mark
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
    {
	curbuf->b_చివరి_change.lnum = lnum;
	curbuf->b_చివరి_change.నిలువు = నిలువు;

	// Create a new entry if a new undo-able change was started or we
	// don't have an entry yet.
	ఒకవేళ (curbuf->b_new_change || curbuf->b_changelistlen == 0)
	{
	    ఒకవేళ (curbuf->b_changelistlen == 0)
		కూడు = TRUE;
	    లేకపోతే
	    {
		// Don't create a new entry when the line number is the same
		// as the last one and the column is not too far away.	Avoids
		// creating many entries for typing "xxxxx".
		p = &curbuf->b_changelist[curbuf->b_changelistlen - 1];
		ఒకవేళ (p->lnum != lnum)
		    కూడు = TRUE;
		లేకపోతే
		{
		    నిలువుల_సంఖ్య = comp_textwidth(FALSE);
		    ఒకవేళ (నిలువుల_సంఖ్య == 0)
			నిలువుల_సంఖ్య = 79;
		    కూడు = (p->నిలువు + నిలువుల_సంఖ్య < నిలువు || నిలువు + నిలువుల_సంఖ్య < p->నిలువు);
		}
	    }
	    ఒకవేళ (కూడు)
	    {
		// This is the first of a new sequence of undo-able changes
		// and it's at some distance of the last change.  Use a new
		// position in the changelist.
		curbuf->b_new_change = false;

		ఒకవేళ (curbuf->b_changelistlen == JUMPLISTSIZE)
		{
		    // changelist is full: remove oldest entry
		    curbuf->b_changelistlen = JUMPLISTSIZE - 1;
		    mch_memmove(curbuf->b_changelist, curbuf->b_changelist + 1,
					  పరిమాణం(pos_T) * (JUMPLISTSIZE - 1));
		    FOR_ALL_TAB_WINDOWS(tp, wp)
		    {
			// Correct position in changelist for other windows on
			// this buffer.
			ఒకవేళ (wp->w_బఫరు == curbuf && wp->w_changelistidx > 0)
			    --wp->w_changelistidx;
		    }
		}
		FOR_ALL_TAB_WINDOWS(tp, wp)
		{
		    // For other windows, if the position in the changelist is
		    // at the end it stays at the end.
		    ఒకవేళ (wp->w_బఫరు == curbuf
			    && wp->w_changelistidx == curbuf->b_changelistlen)
			++wp->w_changelistidx;
		}
		++curbuf->b_changelistlen;
	    }
	}
	curbuf->b_changelist[curbuf->b_changelistlen - 1] =
							curbuf->b_చివరి_change;
	// The current window is always after the last change, so that "g,"
	// takes you back to it.
	curwin->w_changelistidx = curbuf->b_changelistlen;
    }

    ఒకవేళ (VIsual_చురుకైన)
	check_visual_pos();

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ఒకవేళ (wp->w_బఫరు == curbuf)
	{
	    linenr_T చివరి = lnume + xtra - 1;  // last line after the change

	    // Mark this window to be redrawn later.
	    ఒకవేళ (!redraw_not_allowed && wp->w_redr_type < UPD_VALID)
		wp->w_redr_type = UPD_VALID;

	    // When inserting/deleting lines and the window has specific lines
	    // to be redrawn, w_redraw_top and w_redraw_bot may now be invalid,
	    // so just redraw everything.
	    ఒకవేళ (xtra != 0 && wp->w_redraw_శిఖరం != 0)
		redraw_win_later(wp, UPD_NOT_VALID);

	    // Reset "w_skipcol" if the topline length has become smaller to
	    // such a degree that nothing will be visible anymore, accounting
	    // for 'smoothscroll' <<< or 'listchars' "precedes" marker.
	    ఒకవేళ (wp->w_skipcol > 0
		    && (చివరి < wp->w_topline
			|| (wp->w_topline >= lnum
			    && wp->w_topline < lnume
			    && linetabsize_eol(wp, wp->w_topline)
				    <= wp->w_skipcol + sms_marker_overlap(wp, -1))))
		wp->w_skipcol = 0;

	    // Check if a change in the buffer has invalidated the cached
	    // values for the cursor.
#నిర్వచితమైతే FEAT_FOLDING
	    // Update the folds for this window.  Can't postpone this, because
	    // a following operator might work on the whole fold: ">>dd".
	    foldUpdate(wp, lnum, చివరి);

	    // The change may cause lines above or below the change to become
	    // included in a fold.  Set lnum/lnume to the first/last line that
	    // might be displayed differently.
	    // Set w_cline_folded here as an efficient way to update it when
	    // inserting lines just above a closed fold.
	    i = hasFoldingWin(wp, lnum, &lnum, ఖాళీ, FALSE, ఖాళీ);
	    ఒకవేళ (wp->w_cursor.lnum == lnum)
		wp->w_cline_folded = i;
	    i = hasFoldingWin(wp, చివరి, ఖాళీ, &చివరి, FALSE, ఖాళీ);
	    ఒకవేళ (wp->w_cursor.lnum == చివరి)
		wp->w_cline_folded = i;

	    // If the changed line is in a range of previously folded lines,
	    // compare with the first line in that range.
	    ఒకవేళ (wp->w_cursor.lnum <= lnum)
	    {
		i = కనుగొను_wl_నమోదు(wp, lnum);
		ఒకవేళ (i >= 0 && wp->w_cursor.lnum > wp->w_పంక్తులు[i].wl_lnum)
		    మారింది_పంక్తి_abv_curs_win(wp);
	    }
#అయితే_ముగింపు
	    ఒకవేళ (wp->w_cursor.lnum > lnum)
		మారింది_పంక్తి_abv_curs_win(wp);
	    లేకపోతే ఒకవేళ (wp->w_cursor.lnum == lnum && wp->w_cursor.నిలువు >= నిలువు)
		మారింది_cline_bef_curs_win(wp);
	    ఒకవేళ (wp->w_botline >= lnum)
	    {
		ఒకవేళ (xtra < 0)
		    invalidate_botline_win(wp);
		లేకపోతే
		    // Assume that botline doesn't change (inserted lines make
		    // other lines scroll down below botline).
		    approximate_botline_win(wp);
	    }

	    // Check if any w_lines[] entries have become invalid.
	    // For entries below the change: Correct the lnums for
	    // inserted/deleted lines.	Makes it possible to stop displaying
	    // after the change.
	    కొరకు (i = 0; i < wp->w_పంక్తులు_valid; ++i)
		ఒకవేళ (wp->w_పంక్తులు[i].wl_valid)
		{
		    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum >= lnum)
		    {
			// Do not change wl_lnum at index zero, it is used to
			// compare with w_topline.  Invalidate it instead.
			ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum < lnume || i == 0)
			{
			    // line included in change
			    wp->w_పంక్తులు[i].wl_valid = FALSE;
			}
			లేకపోతే ఒకవేళ (xtra != 0)
			{
			    // line below change
			    wp->w_పంక్తులు[i].wl_lnum += xtra;
#నిర్వచితమైతే FEAT_FOLDING
			    wp->w_పంక్తులు[i].wl_lastlnum += xtra;
#అయితే_ముగింపు
			}
		    }
#నిర్వచితమైతే FEAT_FOLDING
		    లేకపోతే ఒకవేళ (wp->w_పంక్తులు[i].wl_lastlnum >= lnum)
		    {
			// change somewhere inside this range of folded lines,
			// may need to be redrawn
			wp->w_పంక్తులు[i].wl_valid = FALSE;
		    }
#అయితే_ముగింపు
		}

#నిర్వచితమైతే FEAT_FOLDING
	    // Take care of side effects for setting w_topline when folds have
	    // changed.  Esp. when the buffer was changed in another window.
	    ఒకవేళ (hasAnyFolding(wp))
		సమితి_topline(wp, wp->w_topline);
#అయితే_ముగింపు
	    // If lines have been added or removed, relative numbering always
	    // requires an update even if cursor didn't move.
	    ఒకవేళ (wp->w_p_rnu && xtra != 0)
		wp->w_చివరి_cursor_lnum_rnu = 0;

#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (wp->w_p_cul && wp->w_చివరి_cursorline >= lnum)
	    {
		ఒకవేళ (wp->w_చివరి_cursorline < lnume)
		    // If 'cursorline' was inside the change, it has already
		    // been invalidated in w_lines[] by the loop above.
		    wp->w_చివరి_cursorline = 0;
		లేకపోతే
		    // If 'cursorline' was below the change, adjust its lnum.
		    wp->w_చివరి_cursorline += xtra;
	    }
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	ఒకవేళ (wp == curwin && xtra != 0 && శోధన_hl_has_cursor_lnum >= lnum)
	    శోధన_hl_has_cursor_lnum += xtra;
#అయితే_ముగింపు
    }

    // Call update_screen() later, which checks out what needs to be redrawn,
    // since it notices b_mod_set and then uses b_mod_*.
    సమితి_must_redraw(UPD_VALID);

    // when the cursor line is changed always trigger CursorMoved
    ఒకవేళ (lnum <= curwin->w_cursor.lnum
		 && lnume + (xtra < 0 ? -xtra : xtra) > curwin->w_cursor.lnum)
	చివరి_cursormoved.lnum = 0;
}

    స్థిర శూన్య
మారింది_Oneline(buf_T *buf, linenr_T lnum)
{
    ఒకవేళ (buf->b_mod_సమితి)
    {
	// find the maximum area that must be redisplayed
	ఒకవేళ (lnum < buf->b_mod_శిఖరం)
	    buf->b_mod_శిఖరం = lnum;
	లేకపోతే ఒకవేళ (lnum >= buf->b_mod_bot)
	    buf->b_mod_bot = lnum + 1;
    }
    లేకపోతే
    {
	// set the area that must be redisplayed to one line
	buf->b_mod_సమితి = true;
	buf->b_mod_శిఖరం = lnum;
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
    శూన్య
మారింది_బైట్లు(linenr_T lnum, colnr_T నిలువు)
{
    మారింది_Oneline(curbuf, lnum);
    మారింది_common(lnum, నిలువు, lnum + 1, 0L);

#నిర్వచితమైతే FEAT_SPELL
    // When text has been changed at the end of the line, possibly the start of
    // the next line may have SpellCap that should be removed or it needs to be
    // displayed.  Schedule the next line for redrawing just in case.
    // Don't do this when displaying '$' at the end of changed text.
    ఒకవేళ (spell_check_కిటికీ(curwin)
	    && lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
	    && vim_strchr(p_cpo, CPO_DOLLAR) == ఖాళీ)
	redrawWinline(curwin, lnum + 1);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    // Diff highlighting in other diff windows may need to be updated too.
    ఒకవేళ (curwin->w_p_diff)
    {
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ఒకవేళ (wlnum > 0)
		    మారింది_Oneline(wp->w_బఫరు, wlnum);
	    }
    }
#అయితే_ముగింపు
}

/*
 * Like changed_bytes() but also adjust text properties for "added" bytes.
 * When "added" is negative text was deleted.
 */
    శూన్య
inserted_బైట్లు(linenr_T lnum, colnr_T నిలువు, పూర్ణాంకం added వాడని)
{
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (curbuf->b_has_textprop && added != 0)
	adjust_prop_columns(lnum, నిలువు, added, 0);
#అయితే_ముగింపు

    మారింది_బైట్లు(lnum, నిలువు);
}

/*
 * Appended "count" lines below line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    శూన్య
appended_పంక్తులు(linenr_T lnum, దీర్ఘ లెక్క)
{
    మారింది_పంక్తులు(lnum + 1, 0, lnum + 1, లెక్క);
}

/*
 * Like appended_lines(), but adjust marks first.
 */
    శూన్య
appended_పంక్తులు_mark(linenr_T lnum, దీర్ఘ లెక్క)
{
    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, లెక్క, 0L);
    మారింది_పంక్తులు(lnum + 1, 0, lnum + 1, లెక్క);
}

/*
 * Deleted "count" lines at line "lnum" in the current buffer.
 * Must be called AFTER the change and after mark_adjust().
 * Takes care of marking the buffer to be redrawn and sets the changed flag.
 */
    శూన్య
deleted_పంక్తులు(linenr_T lnum, దీర్ఘ లెక్క)
{
    మారింది_పంక్తులు(lnum, 0, lnum + లెక్క, -లెక్క);
}

/*
 * Like deleted_lines(), but adjust marks first.
 * Make sure the cursor is on a valid line before calling, a GUI callback may
 * be triggered to display the cursor.
 */
    శూన్య
deleted_పంక్తులు_mark(linenr_T lnum, దీర్ఘ లెక్క)
{
    mark_adjust(lnum, (linenr_T)(lnum + లెక్క - 1), (దీర్ఘ)MAXLNUM, -లెక్క);
    మారింది_పంక్తులు(lnum, 0, lnum + లెక్క, -లెక్క);
}

/*
 * Marks the area to be redrawn after a change.
 * Consider also calling changed_line_display_buf().
 */
    శూన్య
మారింది_పంక్తులు_buf(
    buf_T	*buf,
    linenr_T	lnum,	    // first line with change
    linenr_T	lnume,	    // line below last changed line
    దీర్ఘ	xtra)	    // number of extra lines (negative when deleting)
{
    ఒకవేళ (buf->b_mod_సమితి)
    {
	// find the maximum area that must be redisplayed
	ఒకవేళ (lnum < buf->b_mod_శిఖరం)
	    buf->b_mod_శిఖరం = lnum;
	ఒకవేళ (lnum < buf->b_mod_bot)
	{
	    // adjust old bot position for xtra lines
	    buf->b_mod_bot += xtra;
	    ఒకవేళ (buf->b_mod_bot < lnum)
		buf->b_mod_bot = lnum;
	}
	ఒకవేళ (lnume + xtra > buf->b_mod_bot)
	    buf->b_mod_bot = lnume + xtra;
	buf->b_mod_xlines += xtra;
    }
    లేకపోతే
    {
	// set the area that must be redisplayed
	buf->b_mod_సమితి = true;
	buf->b_mod_శిఖరం = lnum;
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
    శూన్య
మారింది_పంక్తులు(
    linenr_T	lnum,	    // first line with change
    colnr_T	నిలువు,	    // column in first line with change
    linenr_T	lnume,	    // line below last changed line
    దీర్ఘ	xtra)	    // number of extra lines (negative when deleting)
{
    మారింది_పంక్తులు_buf(curbuf, lnum, lnume, xtra);

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (xtra == 0 && curwin->w_p_diff && !diff_అంతర్గత())
    {
	// When the number of lines doesn't change then mark_adjust() isn't
	// called and other diff buffers still need to be marked for
	// displaying.
	win_T	    *wp;
	linenr_T    wlnum;

	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_p_diff && wp != curwin)
	    {
		redraw_win_later(wp, UPD_VALID);
		wlnum = diff_lnum_win(lnum, wp);
		ఒకవేళ (wlnum > 0)
		    మారింది_పంక్తులు_buf(wp->w_బఫరు, wlnum,
						    lnume - lnum + wlnum, 0L);
	    }
    }
#అయితే_ముగింపు

    మారింది_common(lnum, నిలువు, lnume, xtra);
}

/*
 * Called when the changed flag must be reset for buffer "buf".
 * When "ff" is TRUE also reset 'fileformat'.
 * When "always_inc_changedtick" is TRUE b:changedtick is incremented also when
 * the changed flag was off.
 */
    శూన్య
unchanged(buf_T *buf, పూర్ణాంకం ff, పూర్ణాంకం always_inc_changedtick)
{
    ఒకవేళ (buf->b_మారింది || (ff && file_ff_differs(buf, FALSE)))
    {
	buf->b_మారింది = 0;
	ml_setflags(buf);
	ఒకవేళ (ff)
	    save_file_ff(buf);
	check_స్థితి(buf);
	redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#అయితే_ముగింపు
	need_maketitle = TRUE;	    // set window title later
	++CHANGEDTICK(buf);
    }
    లేకపోతే ఒకవేళ (always_inc_changedtick)
	++CHANGEDTICK(buf);
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_unmodified(buf);
#అయితే_ముగింపు
}

/*
 * Save the current values of 'fileformat' and 'fileencoding', so that we know
 * the file must be considered changed when the value is different.
 */
    శూన్య
save_file_ff(buf_T *buf)
{
    buf->b_ప్రారంభం_ffc = *buf->b_p_ff;
    buf->b_ప్రారంభం_eof = buf->b_p_eof;
    buf->b_ప్రారంభం_eol = buf->b_p_eol;
    buf->b_ప్రారంభం_bomb = buf->b_p_bomb;

    // Only use free/alloc when necessary, they take time.
    ఒకవేళ (buf->b_ప్రారంభం_fenc == ఖాళీ
			     || STRCMP(buf->b_ప్రారంభం_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_ప్రారంభం_fenc);
	buf->b_ప్రారంభం_fenc = vim_strsave(buf->b_p_fenc);
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
    పూర్ణాంకం
file_ff_differs(buf_T *buf, పూర్ణాంకం ignore_ఖాళీలు)
{
    // In a buffer that was never loaded the options are not valid.
    ఒకవేళ (buf->b_flags & BF_NEVERLOADED)
	తిరిగి FALSE;
    ఒకవేళ (ignore_ఖాళీలు
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_పంక్తి_లెక్క == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	తిరిగి FALSE;
    ఒకవేళ (buf->b_ప్రారంభం_ffc != *buf->b_p_ff)
	తిరిగి TRUE;
    ఒకవేళ ((buf->b_p_bin || !buf->b_p_fixeol)
	    && (buf->b_ప్రారంభం_eof != buf->b_p_eof
		|| buf->b_ప్రారంభం_eol != buf->b_p_eol))
	తిరిగి TRUE;
    ఒకవేళ (!buf->b_p_bin && buf->b_ప్రారంభం_bomb != buf->b_p_bomb)
	తిరిగి TRUE;
    ఒకవేళ (buf->b_ప్రారంభం_fenc == ఖాళీ)
	తిరిగి (*buf->b_p_fenc != NUL);
    తిరిగి (STRCMP(buf->b_ప్రారంభం_fenc, buf->b_p_fenc) != 0);
}

/*
 * Insert string "p" at the cursor position.  Stops at a NUL byte.
 * Handles Replace mode and multi-byte characters.
 */
    శూన్య
ins_బైట్లు(char_u *p)
{
    ins_బైట్లు_len(p, (పూర్ణాంకం)STRLEN(p));
}

/*
 * Insert string "p" with length "len" at the cursor position.
 * Handles Replace mode and multi-byte characters.
 */
    శూన్య
ins_బైట్లు_len(char_u *p, పూర్ణాంకం len)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;

    ఒకవేళ (has_mbyte)
	కొరకు (i = 0; i < len; i += n)
	{
	    ఒకవేళ (enc_utf8)
		// avoid reading past p[len]
		n = utfc_ptr2len_len(p + i, len - i);
	    లేకపోతే
		n = (*mb_ptr2len)(p + i);
	    ins_char_బైట్లు(p + i, n);
	}
    లేకపోతే
	కొరకు (i = 0; i < len; ++i)
	    ins_char(p[i]);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in MODE_REPLACE or MODE_VREPLACE state, replace any existing character.
 * Caller must have prepared for undo.
 * For multi-byte characters we get the whole character, the caller must
 * convert bytes to a character.
 */
    శూన్య
ins_char(పూర్ణాంకం c)
{
    char_u	buf[MB_MAXBYTES + 1];
    పూర్ణాంకం		n = (*mb_char2bytes)(c, buf);

    // When "c" is 0x100, 0x200, etc. we don't want to insert a NUL byte.
    // Happens for CTRL-Vu9900.
    ఒకవేళ (buf[0] == 0)
	buf[0] = '\n';

    ins_char_బైట్లు(buf, n);
}

    శూన్య
ins_char_బైట్లు(char_u *buf, పూర్ణాంకం charlen)
{
    పూర్ణాంకం		c = buf[0];
    పూర్ణాంకం		newlen;		// nr of bytes inserted
    పూర్ణాంకం		oldlen;		// nr of bytes deleted (0 when not replacing)
    char_u	*p;
    char_u	*newp;
    char_u	*oldp;
    పూర్ణాంకం		linelen;	// length of old line including NUL
    colnr_T	నిలువు;
    linenr_T	lnum = curwin->w_cursor.lnum;
    పూర్ణాంకం		i;

    // Break tabs if needed.
    ఒకవేళ (virtual_చురుకైన() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    నిలువు = curwin->w_cursor.నిలువు;
    oldp = ml_get(lnum);
    linelen = (పూర్ణాంకం)ml_get_len(lnum) + 1;

    // The lengths default to the values for when not replacing.
    oldlen = 0;
    newlen = charlen;

    ఒకవేళ (State & REPLACE_FLAG)
    {
	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    colnr_T	new_vcol = 0;	// init for GCC
	    colnr_T	vcol;
	    పూర్ణాంకం		old_list;

	    // Disable 'list' temporarily, unless 'cpo' contains the 'L' flag.
	    // Returns the old value of list, so when finished,
	    // curwin->w_p_list should be set back to this.
	    old_list = curwin->w_p_list;
	    ఒకవేళ (old_list && vim_strchr(p_cpo, CPO_LISTWM) == ఖాళీ)
		curwin->w_p_list = FALSE;

	    // In virtual replace mode each character may replace one or more
	    // characters (zero if it's a TAB).  Count the number of bytes to
	    // be deleted to make room for the new character, counting screen
	    // cells.  May result in adding spaces to fill a gap.
	    getvcol(curwin, &curwin->w_cursor, ఖాళీ, &vcol, ఖాళీ, 0);
	    new_vcol = vcol + chartabsize(buf, vcol);
	    అయితే (oldp[నిలువు + oldlen] != NUL && vcol < new_vcol)
	    {
		vcol += chartabsize(oldp + నిలువు + oldlen, vcol);
		// Don't need to remove a TAB that takes us to the right
		// position.
		ఒకవేళ (vcol > new_vcol && oldp[నిలువు + oldlen] == TAB)
		    ఆపు;
		oldlen += (*mb_ptr2len)(oldp + నిలువు + oldlen);
		// Deleted a bit too much, insert spaces.
		ఒకవేళ (vcol > new_vcol)
		    newlen += vcol - new_vcol;
	    }
	    curwin->w_p_list = old_list;
	}
	లేకపోతే ఒకవేళ (oldp[నిలువు] != NUL)
	{
	    // normal replace
	    oldlen = (*mb_ptr2len)(oldp + నిలువు);
	}


	// Push the replaced bytes onto the replace stack, so that they can be
	// put back when BS is used.  The bytes of a multi-byte character are
	// done the other way around, so that the first byte is popped off
	// first (it tells the byte length of the character).
	replace_నెట్టు(NUL);
	కొరకు (i = 0; i < oldlen; ++i)
	{
	    ఒకవేళ (has_mbyte)
		i += replace_నెట్టు_mb(oldp + నిలువు + i) - 1;
	    లేకపోతే
		replace_నెట్టు(oldp[నిలువు + i]);
	}
    }

    newp = alloc(linelen + newlen - oldlen);
    ఒకవేళ (newp == ఖాళీ)
	తిరిగి;

    // Copy bytes before the cursor.
    ఒకవేళ (నిలువు > 0)
	mch_memmove(newp, oldp, (size_t)నిలువు);

    // Copy bytes after the changed character(s).
    p = newp + నిలువు;
    ఒకవేళ (linelen > నిలువు + oldlen)
	mch_memmove(p + newlen, oldp + నిలువు + oldlen,
					    (size_t)(linelen - నిలువు - oldlen));

    // Insert or overwrite the new character.
    mch_memmove(p, buf, charlen);
    i = charlen;

    // Fill with spaces when necessary.
    అయితే (i < newlen)
	p[i++] = ' ';

    // Replace the line in the buffer.
    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    మారింది_బైట్లు(lnum, నిలువు);
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (curbuf->b_has_textprop && newlen != oldlen)
	adjust_prop_columns(lnum, నిలువు, newlen - oldlen,
				    State & REPLACE_FLAG ? APC_SUBSTITUTE : 0);
#అయితే_ముగింపు

    // If we're in Insert or Replace mode and 'showmatch' is set, then briefly
    // show the match for right parens and braces.
    ఒకవేళ (p_sm && (State & బహుళకం_చొప్పించు)
	    && సందేశం_silent == 0
	    && !ins_compl_చురుకైన())
    {
	ఒకవేళ (has_mbyte)
	    showmatch(mb_ptr2char(buf));
	లేకపోతే
	    showmatch(c);
    }

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (!p_ri || (State & REPLACE_FLAG))
#అయితే_ముగింపు
    {
	// Normal insert: move cursor right
	curwin->w_cursor.నిలువు += charlen;
    }

    // TODO: should try to update w_row here, to avoid recomputing it later.
}

/*
 * Insert a string at the cursor position.
 * Note: Does NOT handle Replace mode.
 * Caller must have prepared for undo.
 */
    శూన్య
ins_str(char_u *s, size_t slen)
{
    char_u	*oldp, *newp;
    పూర్ణాంకం		oldlen;
    colnr_T	నిలువు;
    linenr_T	lnum = curwin->w_cursor.lnum;

    ఒకవేళ (virtual_చురుకైన() && curwin->w_cursor.coladd > 0)
	coladvance_force(getviscol());

    నిలువు = curwin->w_cursor.నిలువు;
    oldp = ml_get(lnum);
    oldlen = (పూర్ణాంకం)ml_get_len(lnum);

    newp = alloc(oldlen + slen + 1);
    ఒకవేళ (newp == ఖాళీ)
	తిరిగి;
    ఒకవేళ (నిలువు > 0)
	mch_memmove(newp, oldp, (size_t)నిలువు);
    mch_memmove(newp + నిలువు, s, slen);
    mch_memmove(newp + నిలువు + slen, oldp + నిలువు, (size_t)(oldlen - నిలువు + 1));
    ml_replace(lnum, newp, FALSE);
    inserted_బైట్లు(lnum, నిలువు, (పూర్ణాంకం)slen);
    curwin->w_cursor.నిలువు += (colnr_T)slen;
}

/*
 * Delete one character under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
del_char(పూర్ణాంకం fixpos)
{
    ఒకవేళ (has_mbyte)
    {
	// Make sure the cursor is at the start of a character.
	mb_adjust_cursor();
	ఒకవేళ (*ml_get_cursor() == NUL)
	    తిరిగి FAIL;
	తిరిగి del_chars(1L, fixpos);
    }
    తిరిగి del_బైట్లు(1L, fixpos, TRUE);
}

/*
 * Like del_bytes(), but delete characters instead of bytes.
 */
    పూర్ణాంకం
del_chars(దీర్ఘ లెక్క, పూర్ణాంకం fixpos)
{
    దీర్ఘ	బైట్లు = 0;
    దీర్ఘ	i;
    char_u	*p;
    పూర్ణాంకం		l;

    p = ml_get_cursor();
    కొరకు (i = 0; i < లెక్క && *p != NUL; ++i)
    {
	l = (*mb_ptr2len)(p);
	బైట్లు += l;
	p += l;
    }
    తిరిగి del_బైట్లు(బైట్లు, fixpos, TRUE);
}

/*
 * Delete "count" bytes under the cursor.
 * If "fixpos" is TRUE, don't leave the cursor on the NUL after the line.
 * Caller must have prepared for undo.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
del_బైట్లు(
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		fixpos_arg,
    పూర్ణాంకం		use_delcombine వాడని)	    // 'delcombine' option applies
{
    char_u	*oldp, *newp;
    colnr_T	oldlen;
    colnr_T	newlen;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	నిలువు = curwin->w_cursor.నిలువు;
    పూర్ణాంకం		alloc_newp;
    దీర్ఘ	movelen;
    పూర్ణాంకం		fixpos = fixpos_arg;

    oldp = ml_get(lnum);
    oldlen = (పూర్ణాంకం)ml_get_len(lnum);

    // Can't do anything when the cursor is on the NUL after the line.
    ఒకవేళ (నిలువు >= oldlen)
	తిరిగి FAIL;

    // If "count" is zero there is nothing to do.
    ఒకవేళ (లెక్క == 0)
	తిరిగి OK;

    // If "count" is negative the caller must be doing something wrong.
    ఒకవేళ (లెక్క < 1)
    {
	siemsg(e_invalid_లెక్క_for_del_బైట్లు_nr, లెక్క);
	తిరిగి FAIL;
    }

    // If 'delcombine' is set and deleting (less than) one character, only
    // delete the last combining character.
    ఒకవేళ (p_deco && use_delcombine && enc_utf8
					 && utfc_ptr2len(oldp + నిలువు) >= లెక్క)
    {
	పూర్ణాంకం	cc[MAX_MCO];
	పూర్ణాంకం	n;

	(శూన్య)utfc_ptr2char(oldp + నిలువు, cc);
	ఒకవేళ (cc[0] != NUL)
	{
	    // Find the last composing char, there can be several.
	    n = నిలువు;
	    చేయి
	    {
		నిలువు = n;
		లెక్క = utf_ptr2len(oldp + n);
		n += లెక్క;
	    } అయితే (UTF_COMPOSINGLIKE(oldp + నిలువు, oldp + n));
	    fixpos = 0;
	}
    }

    // When count is too big, reduce it.
    movelen = (దీర్ఘ)oldlen - (దీర్ఘ)నిలువు - లెక్క + 1; // includes trailing NUL
    ఒకవేళ (movelen <= 1)
    {
	// If we just took off the last character of a non-blank line, and
	// fixpos is TRUE, we don't want to end up positioned at the NUL,
	// unless "restart_edit" is set or 'virtualedit' contains "onemore".
	ఒకవేళ (నిలువు > 0 && fixpos && restart_edit == 0
					 && (get_ve_flags() & VE_ONEMORE) == 0)
	{
	    --curwin->w_cursor.నిలువు;
	    curwin->w_cursor.coladd = 0;
	    ఒకవేళ (has_mbyte)
		curwin->w_cursor.నిలువు -=
			    (*mb_తల_off)(oldp, oldp + curwin->w_cursor.నిలువు);
	}
	లెక్క = oldlen - నిలువు;
	movelen = 1;
    }
    newlen = oldlen - లెక్క;

    // If the old line has been allocated the deletion can be done in the
    // existing line. Otherwise a new line has to be allocated
    // Can't do this when using Netbeans, because we would need to invoke
    // netbeans_removed(), which deallocates the line.	Let ml_replace() take
    // care of notifying Netbeans.
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    ఒకవేళ (netbeans_చురుకైన())
	alloc_newp = TRUE;
    లేకపోతే
#అయితే_ముగింపు
	alloc_newp = !ml_పంక్తి_alloced();    // check if oldp was allocated
    ఒకవేళ (!alloc_newp)
	newp = oldp;			    // use same allocated memory
    లేకపోతే
    {					    // need to allocate a new line
	newp = alloc(newlen + 1);
	ఒకవేళ (newp == ఖాళీ)
	    తిరిగి FAIL;
	mch_memmove(newp, oldp, (size_t)నిలువు);
    }
    mch_memmove(newp + నిలువు, oldp + నిలువు + లెక్క, (size_t)movelen);
    ఒకవేళ (alloc_newp)
	ml_replace(lnum, newp, FALSE);
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_PROP_POPUP
	// Also move any following text properties.
	ఒకవేళ (oldlen + 1 < curbuf->b_ml.ml_పంక్తి_len)
	    mch_memmove(newp + newlen + 1, oldp + oldlen + 1,
			       (size_t)curbuf->b_ml.ml_పంక్తి_len - oldlen - 1);
#అయితే_ముగింపు
	curbuf->b_ml.ml_పంక్తి_len -= లెక్క;
	curbuf->b_ml.ml_పంక్తి_textlen = 0;
    }

    // mark the buffer as changed and prepare for displaying
    inserted_బైట్లు(lnum, నిలువు, -లెక్క);

    తిరిగి OK;
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
    పూర్ణాంకం
open_పంక్తి(
    పూర్ణాంకం		dir,		// FORWARD or BACKWARD
    పూర్ణాంకం		flags,
    పూర్ణాంకం		రెండవ_పంక్తి_indent,
    పూర్ణాంకం		*did_do_comment వాడని)
{
    char_u	*saved_పంక్తి;		// copy of the original line
    char_u	*తదుపరి_పంక్తి = ఖాళీ;	// copy of the next line
    char_u	*p_extra = ఖాళీ;	// what goes to next line
    పూర్ణాంకం		less_నిలువుల_సంఖ్య = 0;		// less columns for mark in new line
    పూర్ణాంకం		less_నిలువుల_సంఖ్య_off = 0;	// columns to skip for mark and
					// textprop adjustment
    pos_T	old_cursor;		// old cursor position
    పూర్ణాంకం		newcol = 0;		// new cursor column
    పూర్ణాంకం		newindent = 0;		// auto-indent of the new line
    పూర్ణాంకం		n;
    పూర్ణాంకం		trunc_పంక్తి = FALSE;	// truncate current line afterwards
    పూర్ణాంకం		retval = FAIL;		// return value
    పూర్ణాంకం		extra_len = 0;		// length of p_extra string
    పూర్ణాంకం		lead_len;		// length of comment leader
    పూర్ణాంకం		comment_ప్రారంభం = 0;	// start index of the comment leader
    char_u	*lead_flags;	// position in 'comments' for comment leader
    char_u	*leader = ఖాళీ;		// copy of comment leader
    char_u	*allocated = ఖాళీ;	// allocated memory
    char_u	*p;
    పూర్ణాంకం		saved_char = NUL;	// init for GCC
    pos_T	*pos;
    పూర్ణాంకం		do_cindent;
    పూర్ణాంకం		do_si = may_do_si();
    పూర్ణాంకం		no_si = FALSE;		// reset did_si afterwards
    పూర్ణాంకం		మొదటి_char = NUL;	// init for GCC
    పూర్ణాంకం		vreplace_బహుళకం;
    పూర్ణాంకం		did_append;		// appended a new line
    పూర్ణాంకం		saved_pi = curbuf->b_p_pi; // copy of preserveindent setting
#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		at_eol;			// cursor after last character
#అయితే_ముగింపు

    // make a copy of the current line so we can mess with it
    saved_పంక్తి = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ఒకవేళ (saved_పంక్తి == ఖాళీ)	    // out of memory!
	తిరిగి FALSE;

#నిర్వచితమైతే FEAT_PROP_POPUP
    at_eol = curwin->w_cursor.నిలువు >= (పూర్ణాంకం)ml_get_curline_len();
#అయితే_ముగింపు

    ఒకవేళ (State & VREPLACE_FLAG)
    {
	// With MODE_VREPLACE we make a copy of the next line, which we will be
	// starting to replace.  First make the new line empty and let vim play
	// with the indenting and comment leader to its heart's content.  Then
	// we grab what it ended up putting on the new line, put back the
	// original line, and call ins_char() to put each new character onto
	// the line, replacing what was there before and pushing the right
	// stuff onto the replace stack.  -- webb.
	ఒకవేళ (curwin->w_cursor.lnum < orig_పంక్తి_లెక్క)
	    తదుపరి_పంక్తి = vim_strnsave(ml_get(curwin->w_cursor.lnum + 1), ml_get_len(curwin->w_cursor.lnum + 1));
	లేకపోతే
	    తదుపరి_పంక్తి = vim_strsave((char_u *)"");
	ఒకవేళ (తదుపరి_పంక్తి == ఖాళీ)	    // out of memory!
	    వెళ్ళు theend;

	// In MODE_VREPLACE state, a NL replaces the rest of the line, and
	// starts replacing the next line, so push all of the characters left
	// on the line onto the replace stack.	We'll push any other characters
	// that might be replaced at the start of the next line (due to
	// autoindent etc) a bit later.
	replace_నెట్టు(NUL);  // Call twice because BS over NL expects it
	replace_నెట్టు(NUL);
	p = saved_పంక్తి + curwin->w_cursor.నిలువు;
	అయితే (*p != NUL)
	{
	    ఒకవేళ (has_mbyte)
		p += replace_నెట్టు_mb(p);
	    లేకపోతే
		replace_నెట్టు(*p++);
	}
	saved_పంక్తి[curwin->w_cursor.నిలువు] = NUL;
    }

    ఒకవేళ ((State & బహుళకం_చొప్పించు) && (State & VREPLACE_FLAG) == 0)
    {
	p_extra = saved_పంక్తి + curwin->w_cursor.నిలువు;
	ఒకవేళ (do_si)		// need first char after new line break
	{
	    p = skipwhite(p_extra);
	    మొదటి_char = *p;
	}
	extra_len = (పూర్ణాంకం)STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		// cannot do "U" command when adding lines
    did_si = FALSE;
    ai_నిలువు = 0;

    // If we just did an auto-indent, then we didn't type anything on
    // the prior line, and it should be truncated.  Do this even if 'ai' is not
    // set because automatically inserting a comment leader also sets did_ai.
    ఒకవేళ (dir == FORWARD && did_ai)
	trunc_పంక్తి = TRUE;

    ఒకవేళ ((flags & OPENLINE_FORCE_INDENT) && రెండవ_పంక్తి_indent >= 0)
	newindent = రెండవ_పంక్తి_indent;
    // If 'autoindent' and/or 'smartindent' is set, try to figure out what
    // indent to use for the new line.
    లేకపోతే ఒకవేళ (curbuf->b_p_ai || do_si)
    {
	// count white space on current line
#నిర్వచితమైతే FEAT_VARTABS
	newindent = get_indent_str_vtab(saved_పంక్తి, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#లేకపోతే
	newindent = get_indent_str(saved_పంక్తి, (పూర్ణాంకం)curbuf->b_p_ts, FALSE);
#అయితే_ముగింపు
	ఒకవేళ (newindent == 0 && !(flags & OPENLINE_COM_LIST))
	    newindent = రెండవ_పంక్తి_indent; // for ^^D command in insert mode

	// Do smart indenting.
	// In insert/replace mode (only when dir == FORWARD)
	// we may move some text to the next line. If it starts with '{'
	// don't add an indent. Fixes inserting a NL before '{' in line
	//	"if (condition) {"
	ఒకవేళ (!trunc_పంక్తి && do_si && *saved_పంక్తి != NUL
				    && (p_extra == ఖాళీ || మొదటి_char != '{'))
	{
	    char_u  *ptr;
	    char_u  చివరి_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_పంక్తి;
	    ఒకవేళ (flags & OPENLINE_DO_COM)
		lead_len = get_leader_len(ptr, ఖాళీ, FALSE, TRUE);
	    లేకపోతే
		lead_len = 0;
	    ఒకవేళ (dir == FORWARD)
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ఒకవేళ ( lead_len == 0 && ptr[0] == '#')
		{
		    అయితే (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		ఒకవేళ (flags & OPENLINE_DO_COM)
		    lead_len = get_leader_len(ptr, ఖాళీ, FALSE, TRUE);
		లేకపోతే
		    lead_len = 0;
		ఒకవేళ (lead_len > 0)
		{
		    // This case gets the following right:
		    //	    /*
		    //	     * A comment (read '\' as '/').
		    //	     */
		    // #define IN_THE_WAY
		    //	    This should line up here;
		    p = skipwhite(ptr);
		    ఒకవేళ (p[0] == '/' && p[1] == '*')
			p++;
		    ఒకవేళ (p[0] == '*')
		    {
			కొరకు (p++; *p; p++)
			{
			    ఒకవేళ (p[0] == '/' && p[-1] == '*')
			    {
				// End of C comment, indent should line up
				// with the line containing the start of
				// the comment.
				curwin->w_cursor.నిలువు = (colnr_T)(p - ptr);
				ఒకవేళ ((pos = findmatch(ఖాళీ, NUL)) != ఖాళీ)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				    ఆపు;
				}
				// this may make "ptr" invalid, get it again
				ptr = ml_get(curwin->w_cursor.lnum);
				p = ptr + curwin->w_cursor.నిలువు;
			    }
			}
		    }
		}
		లేకపోతే	// Not a comment line
		{
		    // Find last non-blank in line
		    p = ptr + STRLEN(ptr) - 1;
		    అయితే (p > ptr && VIM_ISWHITE(*p))
			--p;
		    చివరి_char = *p;

		    // find the character just before the '{' or ';'
		    ఒకవేళ (చివరి_char == '{' || చివరి_char == ';')
		    {
			ఒకవేళ (p > ptr)
			    --p;
			అయితే (p > ptr && VIM_ISWHITE(*p))
			    --p;
		    }
		    // Try to catch lines that are split over multiple
		    // lines.  eg:
		    //	    if (condition &&
		    //			condition) {
		    //		Should line up here!
		    //	    }
		    ఒకవేళ (*p == ')')
		    {
			curwin->w_cursor.నిలువు = (colnr_T)(p - ptr);
			ఒకవేళ ((pos = findmatch(ఖాళీ, '(')) != ఖాళీ)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    // If last character is '{' do indent, without
		    // checking for "if" and the like.
		    ఒకవేళ (చివరి_char == '{')
		    {
			did_si = TRUE;	// do indent
			no_si = TRUE;	// don't delete it when '{' typed
		    }
		    // Look for "if" and the like, use 'cinwords'.
		    // Don't do this if the previous line ended in ';' or
		    // '}'.
		    లేకపోతే ఒకవేళ (చివరి_char != ';' && చివరి_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    లేకపోతే // dir == BACKWARD
	    {
		// Skip preprocessor directives, unless they are
		// recognised as comments.
		ఒకవేళ (lead_len == 0 && ptr[0] == '#')
		{
		    పూర్ణాంకం was_backslashed = FALSE;

		    అయితే ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
		    {
			ఒకవేళ (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			లేకపోతే
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    ఒకవేళ (was_backslashed)
			newindent = 0;	    // Got to end of file
		    లేకపోతే
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		ఒకవేళ (*p == '}')	    // if line starts with '}': do indent
		    did_si = TRUE;
		లేకపోతే		    // can delete indent when '{' typed
		    can_si_వెనుక = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	ఒకవేళ (do_si)
	    can_si = TRUE;

	did_ai = TRUE;
    }

    // May do indenting after opening a new line.
    do_cindent = !p_paste && (curbuf->b_p_cin
#నిర్వచితమైతే FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#అయితే_ముగింపు
		)
	    && లోపల_cinkeys(dir == FORWARD
		? కీలకం_OPEN_FORW
		: కీలకం_OPEN_వెనుక, ' ', linewhite(curwin->w_cursor.lnum))
	    && !(flags & OPENLINE_FORCE_INDENT);

    // Find out if the current line starts with a comment leader.
    // This may then be inserted in front of the new line.
    చివర_comment_pending = NUL;
    ఒకవేళ (flags & OPENLINE_DO_COM)
    {
	lead_len = get_leader_len(saved_పంక్తి, &lead_flags,
							dir == BACKWARD, TRUE);
	ఒకవేళ (lead_len == 0 && curbuf->b_p_cin && do_cindent && dir == FORWARD
		&& (!has_format_option(FO_NO_OPEN_COMS)
						 || (flags & OPENLINE_FORMAT)))
	{
	    // Check for a line comment after code.
	    comment_ప్రారంభం = check_linecomment(saved_పంక్తి);
	    ఒకవేళ (comment_ప్రారంభం != MAXCOL)
	    {
		lead_len = get_leader_len(saved_పంక్తి + comment_ప్రారంభం,
						     &lead_flags, FALSE, TRUE);
		ఒకవేళ (lead_len != 0)
		{
		    lead_len += comment_ప్రారంభం;
		    ఒకవేళ (did_do_comment != ఖాళీ)
			*did_do_comment = TRUE;
		}
	    }
	}
    }
    లేకపోతే
	lead_len = 0;
    ఒకవేళ (lead_len > 0)
    {
	char_u	*lead_repl = ఖాళీ;	    // replaces comment leader
	పూర్ణాంకం	lead_repl_len = 0;	    // length of *lead_repl
	char_u	lead_మధ్య[COM_MAX_LEN];   // middle-comment string
	పూర్ణాంకం	lead_మధ్య_len;	    // length of the lead_middle
	char_u	lead_చివర[COM_MAX_LEN];	    // end-comment string
	char_u	*comment_చివర = ఖాళీ;	    // where lead_end has been found
	పూర్ణాంకం	extra_space = FALSE;	    // append extra space
	పూర్ణాంకం	current_flag;
	పూర్ణాంకం	require_blank = FALSE;	    // requires blank after middle
	char_u	*p2;

	// If the comment leader has the start, middle or end flag, it may not
	// be used or may be replaced with the middle leader.
	కొరకు (p = lead_flags; *p && *p != ':'; ++p)
	{
	    ఒకవేళ (*p == COM_BLANK)
	    {
		require_blank = TRUE;
		కొనసాగు;
	    }
	    ఒకవేళ (*p == COM_ప్రారంభం || *p == COM_మధ్య)
	    {
		current_flag = *p;
		ఒకవేళ (*p == COM_ప్రారంభం)
		{
		    // Doing "O" on a start of comment does not insert leader.
		    ఒకవేళ (dir == BACKWARD)
		    {
			lead_len = 0;
			ఆపు;
		    }

		    // find start of middle part
		    (శూన్య)copy_option_part(&p, lead_మధ్య, COM_MAX_LEN, ",");
		    require_blank = FALSE;
		}

		// Isolate the strings of the middle and end leader.
		అయితే (*p && p[-1] != ':')	// find end of middle flags
		{
		    ఒకవేళ (*p == COM_BLANK)
			require_blank = TRUE;
		    ++p;
		}
		lead_మధ్య_len = copy_option_part(&p, lead_మధ్య, COM_MAX_LEN, ",");

		అయితే (*p && p[-1] != ':')	// find end of end flags
		{
		    // Check whether we allow automatic ending of comments
		    ఒకవేళ (*p == COM_AUTO_చివర)
			చివర_comment_pending = -1; // means we want to set it
		    ++p;
		}
		n = copy_option_part(&p, lead_చివర, COM_MAX_LEN, ",");

		ఒకవేళ (చివర_comment_pending == -1)	// we can set it now
		    చివర_comment_pending = lead_చివర[n - 1];

		// If the end of the comment is in the same line, don't use
		// the comment leader.
		ఒకవేళ (dir == FORWARD)
		{
		    కొరకు (p = saved_పంక్తి + lead_len; *p; ++p)
			ఒకవేళ (STRNCMP(p, lead_చివర, n) == 0)
			{
			    comment_చివర = p;
			    lead_len = 0;
			    ఆపు;
			}
		}

		// Doing "o" on a start of comment inserts the middle leader.
		ఒకవేళ (lead_len > 0)
		{
		    ఒకవేళ (current_flag == COM_ప్రారంభం)
		    {
			lead_repl = lead_మధ్య;
			lead_repl_len = lead_మధ్య_len;
		    }

		    // If we have hit RETURN immediately after the start
		    // comment leader, then put a space after the middle
		    // comment leader on the next line.
		    ఒకవేళ (!VIM_ISWHITE(saved_పంక్తి[lead_len - 1])
			    && ((p_extra != ఖాళీ
				    && (పూర్ణాంకం)curwin->w_cursor.నిలువు == lead_len)
				|| (p_extra == ఖాళీ
				    && saved_పంక్తి[lead_len] == NUL)
				|| require_blank))
			extra_space = TRUE;
		}
		ఆపు;
	    }
	    ఒకవేళ (*p == COM_చివర)
	    {
		// Doing "o" on the end of a comment does not insert leader.
		// Remember where the end is, might want to use it to find the
		// start (for C-comments).
		ఒకవేళ (dir == FORWARD)
		{
		    comment_చివర = skipwhite(saved_పంక్తి);
		    lead_len = 0;
		    ఆపు;
		}

		// Doing "O" on the end of a comment inserts the middle leader.
		// Find the string for the middle leader, searching backwards.
		అయితే (p > curbuf->b_p_com && *p != ',')
		    --p;
		కొరకు (lead_repl = p; lead_repl > curbuf->b_p_com
					 && lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = (పూర్ణాంకం)(p - lead_repl);

		// We can probably always add an extra space when doing "O" on
		// the comment-end
		extra_space = TRUE;

		// Check whether we allow automatic ending of comments
		కొరకు (p2 = p; *p2 && *p2 != ':'; p2++)
		{
		    ఒకవేళ (*p2 == COM_AUTO_చివర)
			చివర_comment_pending = -1; // means we want to set it
		}
		ఒకవేళ (చివర_comment_pending == -1)
		{
		    // Find last character in end-comment string
		    అయితే (*p2 && *p2 != ',')
			p2++;
		    చివర_comment_pending = p2[-1];
		}
		ఆపు;
	    }
	    ఒకవేళ (*p == COM_మొదటి)
	    {
		// Comment leader for first line only:	Don't repeat leader
		// when using "O", blank out leader when using "o".
		ఒకవేళ (dir == BACKWARD)
		    lead_len = 0;
		లేకపోతే
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		ఆపు;
	    }
	}
	ఒకవేళ (lead_len)
	{
	    // allocate buffer (may concatenate p_extra later)
	    leader = alloc(lead_len + lead_repl_len + extra_space + extra_len
		     + (రెండవ_పంక్తి_indent > 0 ? రెండవ_పంక్తి_indent : 0) + 1);
	    allocated = leader;		    // remember to free it later

	    ఒకవేళ (leader == ఖాళీ)
		lead_len = 0;
	    లేకపోతే
	    {
		పూర్ణాంకం li;

		vim_strncpy(leader, saved_పంక్తి, lead_len);

		// TODO: handle multi-byte and double width chars
		కొరకు (li = 0; li < comment_ప్రారంభం; ++li)
		    ఒకవేళ (!VIM_ISWHITE(leader[li]))
			leader[li] = ' ';

		// Replace leader with lead_repl, right or left adjusted
		ఒకవేళ (lead_repl != ఖాళీ)
		{
		    పూర్ణాంకం		c = 0;
		    పూర్ణాంకం		off = 0;

		    కొరకు (p = lead_flags; *p != NUL && *p != ':'; )
		    {
			ఒకవేళ (*p == COM_కుడి || *p == COM_ఎడమ)
			    c = *p++;
			లేకపోతే ఒకవేళ (VIM_ISDIGIT(*p) || *p == '-')
			    off = getdigits(&p);
			లేకపోతే
			    ++p;
		    }
		    ఒకవేళ (c == COM_కుడి)    // right adjusted leader
		    {
			// find last non-white in the leader to line up with
			కొరకు (p = leader + lead_len - 1; p > leader
						      && VIM_ISWHITE(*p); --p)
			    ;
			++p;

			// Compute the length of the replaced characters in
			// screen characters, not bytes.
			{
			    పూర్ణాంకం	    repl_కొలత = vim_strnsize(lead_repl,
							       lead_repl_len);
			    పూర్ణాంకం	    old_కొలత = 0;
			    char_u  *endp = p;
			    పూర్ణాంకం	    l;

			    అయితే (old_కొలత < repl_కొలత && p > leader)
			    {
				MB_PTR_వెనుక(leader, p);
				old_కొలత += ptr2cells(p);
			    }
			    l = lead_repl_len - (పూర్ణాంకం)(endp - p);
			    ఒకవేళ (l != 0)
				mch_memmove(endp + l, endp,
					(size_t)((leader + lead_len) - endp));
			    lead_len += l;
			}
			mch_memmove(p, lead_repl, (size_t)lead_repl_len);
			ఒకవేళ (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			// blank-out any other chars from the old leader.
			అయితే (--p >= leader)
			{
			    పూర్ణాంకం l = mb_తల_off(leader, p);

			    ఒకవేళ (l > 1)
			    {
				p -= l;
				ఒకవేళ (ptr2cells(p) > 1)
				{
				    p[1] = ' ';
				    --l;
				}
				mch_memmove(p + 1, p + l + 1,
				   (size_t)((leader + lead_len) - (p + l + 1)));
				lead_len -= l;
				*p = ' ';
			    }
			    లేకపోతే ఒకవేళ (!VIM_ISWHITE(*p))
				*p = ' ';
			}
		    }
		    లేకపోతే	// left adjusted leader
		    {
			p = skipwhite(leader);

			// Compute the length of the replaced characters in
			// screen characters, not bytes. Move the part that is
			// not to be overwritten.
			{
			    పూర్ణాంకం	    repl_కొలత = vim_strnsize(lead_repl,
							       lead_repl_len);
			    పూర్ణాంకం	    i;
			    పూర్ణాంకం	    l;

			    కొరకు (i = 0; i < lead_len && p[i] != NUL; i += l)
			    {
				l = (*mb_ptr2len)(p + i);
				ఒకవేళ (vim_strnsize(p, i + l) > repl_కొలత)
				    ఆపు;
			    }
			    ఒకవేళ (i != lead_repl_len)
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
			కొరకు (p += lead_repl_len; p < leader + lead_len; ++p)
			    ఒకవేళ (!VIM_ISWHITE(*p))
			    {
				// Don't put a space before a TAB.
				ఒకవేళ (p + 1 < leader + lead_len && p[1] == TAB)
				{
				    --lead_len;
				    mch_memmove(p, p + 1,
						     (leader + lead_len) - p);
				}
				లేకపోతే
				{
				    పూర్ణాంకం	    l = (*mb_ptr2len)(p);

				    ఒకవేళ (l > 1)
				    {
					ఒకవేళ (ptr2cells(p) > 1)
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
		    ఒకవేళ (curbuf->b_p_ai || do_si)
#నిర్వచితమైతే FEAT_VARTABS
			newindent = get_indent_str_vtab(leader, curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#లేకపోతే
			newindent = get_indent_str(leader,
						   (పూర్ణాంకం)curbuf->b_p_ts, FALSE);
#అయితే_ముగింపు

		    // Add the indent offset
		    ఒకవేళ (newindent + off < 0)
		    {
			off = -newindent;
			newindent = 0;
		    }
		    లేకపోతే
			newindent += off;

		    // Correct trailing spaces for the shift, so that
		    // alignment remains equal.
		    అయితే (off > 0 && lead_len > 0
					       && leader[lead_len - 1] == ' ')
		    {
			// Don't do it when there is a tab before the space
			ఒకవేళ (vim_strchr(skipwhite(leader), '\t') != ఖాళీ)
			    ఆపు;
			--lead_len;
			--off;
		    }

		    // If the leader ends in white space, don't add an
		    // extra space
		    ఒకవేళ (lead_len > 0 && VIM_ISWHITE(leader[lead_len - 1]))
			extra_space = FALSE;
		    leader[lead_len] = NUL;
		}

		ఒకవేళ (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		// if a new indent will be set below, remove the indent that
		// is in the comment leader
		ఒకవేళ (newindent || did_si)
		{
		    అయితే (lead_len && VIM_ISWHITE(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
	    did_si = can_si = FALSE;
	}
	లేకపోతే ఒకవేళ (comment_చివర != ఖాళీ)
	{
	    // We have finished a comment, so we don't use the leader.
	    // If this was a C-comment and 'ai' or 'si' is set do a normal
	    // indent to align with the line containing the start of the
	    // comment.
	    ఒకవేళ (comment_చివర[0] == '*' && comment_చివర[1] == '/' &&
			(curbuf->b_p_ai || do_si))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.నిలువు = (colnr_T)(comment_చివర - saved_పంక్తి);
		ఒకవేళ ((pos = findmatch(ఖాళీ, NUL)) != ఖాళీ)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    // (State == MODE_INSERT || State == MODE_REPLACE), only when dir == FORWARD
    ఒకవేళ (p_extra != ఖాళీ)
    {
	*p_extra = saved_char;		// restore char that NUL replaced

	// When 'ai' set or "flags" has OPENLINE_DELSPACES, skip to the first
	// non-blank.
	//
	// When in MODE_REPLACE state, put the deleted blanks on the replace
	// stack, preceded by a NUL, so they can be put back when a BS is
	// entered.
	ఒకవేళ (REPLACE_సాధారణ(State))
	    replace_నెట్టు(NUL);	    // end of extra blanks
	ఒకవేళ (curbuf->b_p_ai || (flags & OPENLINE_DELSPACES))
	{
	    అయితే ((*p_extra == ' ' || *p_extra == '\t')
		    && (!enc_utf8
			       || !utf_iscomposing(utf_ptr2char(p_extra + 1))))
	    {
		ఒకవేళ (REPLACE_సాధారణ(State))
		    replace_నెట్టు(*p_extra);
		++p_extra;
		++less_నిలువుల_సంఖ్య_off;
	    }
	}

	// columns for marks adjusted for removed columns
	less_నిలువుల_సంఖ్య = (పూర్ణాంకం)(p_extra - saved_పంక్తి);
    }

    ఒకవేళ (p_extra == ఖాళీ)
	p_extra = (char_u *)"";		    // append empty line

    // concatenate leader and p_extra, if there is a leader
    ఒకవేళ (lead_len)
    {
	ఒకవేళ (flags & OPENLINE_COM_LIST && రెండవ_పంక్తి_indent > 0)
	{
	    పూర్ణాంకం i;
	    పూర్ణాంకం padding = రెండవ_పంక్తి_indent
					  - (newindent + (పూర్ణాంకం)STRLEN(leader));

	    // Here whitespace is inserted after the comment char.
	    // Below, set_indent(newindent, SIN_INSERT) will insert the
	    // whitespace needed before the comment char.
	    కొరకు (i = 0; i < padding; i++)
	    {
		STRCAT(leader, " ");
		less_నిలువుల_సంఖ్య--;
		newcol++;
	    }
	}
	STRCAT(leader, p_extra);
	p_extra = leader;
	did_ai = TRUE;	    // So truncating blanks works with comments
	less_నిలువుల_సంఖ్య -= lead_len;
    }
    లేకపోతే
	చివర_comment_pending = NUL;  // turns out there was no leader

    old_cursor = curwin->w_cursor;
    ఒకవేళ (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    ఒకవేళ (!(State & VREPLACE_FLAG) || old_cursor.lnum >= orig_పంక్తి_లెక్క)
    {
	ఒకవేళ (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_T)0, FALSE)
								      == FAIL)
	    వెళ్ళు theend;
	// Postpone calling changed_lines(), because it would mess up folding
	// with markers.
	mark_adjust(curwin->w_cursor.lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
	did_append = TRUE;
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ ((State & బహుళకం_చొప్పించు) && (State & VREPLACE_FLAG) == 0)
	    // Properties after the split move to the next line.
	    adjust_props_for_విభజించు(curwin->w_cursor.lnum, curwin->w_cursor.lnum,
		    curwin->w_cursor.నిలువు + 1, 0, at_eol);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// In MODE_VREPLACE state we are starting to replace the next line.
	curwin->w_cursor.lnum++;
	ఒకవేళ (curwin->w_cursor.lnum >= Insstart.lnum + vr_పంక్తులు_మారింది)
	{
	    // In case we NL to a new line, BS to the previous one, and NL
	    // again, we don't want to save the new line for undo twice.
	    (శూన్య)u_save_cursor();		    // errors are ignored!
	    vr_పంక్తులు_మారింది++;
	}
	ml_replace(curwin->w_cursor.lnum, p_extra, TRUE);
	మారింది_బైట్లు(curwin->w_cursor.lnum, 0);
	curwin->w_cursor.lnum--;
	did_append = FALSE;
    }

    ఒకవేళ (newindent || did_si)
    {
	++curwin->w_cursor.lnum;
	ఒకవేళ (did_si)
	{
	    పూర్ణాంకం sw = (పూర్ణాంకం)get_sw_విలువ(curbuf);

	    ఒకవేళ (p_sr)
		newindent -= newindent % sw;
	    newindent += sw;
	}
	// Copy the indent
	ఒకవేళ (curbuf->b_p_ci)
	{
	    (శూన్య)copy_indent(newindent, saved_పంక్తి);

	    // Set the 'preserveindent' option so that any further screwing
	    // with the line doesn't entirely destroy our efforts to preserve
	    // it.  It gets restored at the function end.
	    curbuf->b_p_pi = TRUE;
	}
	లేకపోతే
	    (శూన్య)సమితి_indent(newindent, SIN_చొప్పించు);
	less_నిలువుల_సంఖ్య -= curwin->w_cursor.నిలువు;

	ai_నిలువు = curwin->w_cursor.నిలువు;

	// In MODE_REPLACE state, for each character in the new indent, there
	// must be a NUL on the replace stack, for when it is deleted with BS
	ఒకవేళ (REPLACE_సాధారణ(State))
	    కొరకు (n = 0; n < (పూర్ణాంకం)curwin->w_cursor.నిలువు; ++n)
		replace_నెట్టు(NUL);
	newcol += curwin->w_cursor.నిలువు;
	ఒకవేళ (no_si)
	    did_si = FALSE;
    }

    // In MODE_REPLACE state, for each character in the extra leader, there
    // must be a NUL on the replace stack, for when it is deleted with BS.
    ఒకవేళ (REPLACE_సాధారణ(State))
	అయితే (lead_len-- > 0)
	    replace_నెట్టు(NUL);

    curwin->w_cursor = old_cursor;

    ఒకవేళ (dir == FORWARD)
    {
	ఒకవేళ (trunc_పంక్తి || (State & బహుళకం_చొప్పించు))
	{
	    // truncate current line at cursor
	    saved_పంక్తి[curwin->w_cursor.నిలువు] = NUL;
	    // Remove trailing white space, unless OPENLINE_KEEPTRAIL used.
	    ఒకవేళ (trunc_పంక్తి && !(flags & OPENLINE_KEEPTRAIL))
		truncate_spaces(saved_పంక్తి, curwin->w_cursor.నిలువు);
	    ml_replace(curwin->w_cursor.lnum, saved_పంక్తి, FALSE);
	    saved_పంక్తి = ఖాళీ;
	    ఒకవేళ (did_append)
	    {
		మారింది_పంక్తులు(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు,
					       curwin->w_cursor.lnum + 1, 1L);
		did_append = FALSE;

		// Move marks after the line break to the new line.
		ఒకవేళ (flags & OPENLINE_MARKFIX)
		    mark_నిలువు_adjust(curwin->w_cursor.lnum,
					 curwin->w_cursor.నిలువు + less_నిలువుల_సంఖ్య_off,
						      1L, (దీర్ఘ)-less_నిలువుల_సంఖ్య, 0);
#నిర్వచితమైతే FEAT_PROP_POPUP
		// Keep into account the deleted blanks on the new line.
		ఒకవేళ (curbuf->b_has_textprop && less_నిలువుల_సంఖ్య_off != 0)
		    adjust_prop_columns(curwin->w_cursor.lnum + 1, 0,
							    -less_నిలువుల_సంఖ్య_off, 0);
#అయితే_ముగింపు
	    }
	    లేకపోతే
		మారింది_బైట్లు(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు);
	}

	// Put the cursor on the new line.  Careful: the scrollup() above may
	// have moved w_cursor, we must use old_cursor.
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    ఒకవేళ (did_append)
	మారింది_పంక్తులు(curwin->w_cursor.lnum, 0, curwin->w_cursor.lnum, 1L);

    curwin->w_cursor.నిలువు = newcol;
    curwin->w_cursor.coladd = 0;

    // In MODE_VREPLACE state, we are handling the replace stack ourselves, so
    // stop fixthisline() from doing it (via change_indent()) by telling it
    // we're in normal MODE_INSERT state.
    ఒకవేళ (State & VREPLACE_FLAG)
    {
	vreplace_బహుళకం = State;	// So we know to put things right later
	State = బహుళకం_చొప్పించు;
    }
    లేకపోతే
	vreplace_బహుళకం = 0;

    ఒకవేళ (!p_paste)
    {
	ఒకవేళ (leader == ఖాళీ
		&& !use_indentexpr_for_lisp()
		&& curbuf->b_p_lisp
		&& curbuf->b_p_ai)
	{
	    // do lisp indenting
	    fixthisline(get_lisp_indent);
	    ai_నిలువు = (colnr_T)getwhitecols_curline();
	}
	లేకపోతే ఒకవేళ (do_cindent || (curbuf->b_p_ai && use_indentexpr_for_lisp()))
	{
	    // do 'cindent' or 'indentexpr' indenting
	    do_c_సమాసం_indent();
	    ai_నిలువు = (colnr_T)getwhitecols_curline();
	}
    }

    ఒకవేళ (vreplace_బహుళకం != 0)
	State = vreplace_బహుళకం;

    // Finally, MODE_VREPLACE gets the stuff on the new line, then puts back
    // the original line, and inserts the new stuff char by char, pushing old
    // stuff onto the replace stack (via ins_char()).
    ఒకవేళ (State & VREPLACE_FLAG)
    {
	// Put new line in p_extra
	p_extra = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ఒకవేళ (p_extra == ఖాళీ)
	    వెళ్ళు theend;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, తదుపరి_పంక్తి, FALSE);

	// Insert new stuff into line again
	curwin->w_cursor.నిలువు = 0;
	curwin->w_cursor.coladd = 0;
	ins_బైట్లు(p_extra);	// will call changed_bytes()
	vim_free(p_extra);
	తదుపరి_పంక్తి = ఖాళీ;
    }

    retval = OK;		// success!
theend:
    curbuf->b_p_pi = saved_pi;
    vim_free(saved_పంక్తి);
    vim_free(తదుపరి_పంక్తి);
    vim_free(allocated);
    తిరిగి retval;
}

/*
 * Delete from cursor to end of line.
 * Caller must have prepared for undo.
 * If "fixpos" is TRUE fix the cursor position when done.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
truncate_పంక్తి(పూర్ణాంకం fixpos)
{
    char_u	*newp;
    linenr_T	lnum = curwin->w_cursor.lnum;
    colnr_T	నిలువు = curwin->w_cursor.నిలువు;
    char_u	*old_పంక్తి;
    పూర్ణాంకం		deleted;

    old_పంక్తి = ml_get(lnum);
    ఒకవేళ (నిలువు == 0)
	newp = vim_strsave((char_u *)"");
    లేకపోతే
	newp = vim_strnsave(old_పంక్తి, నిలువు);
    deleted = (పూర్ణాంకం)ml_get_len(lnum) - నిలువు;

    ఒకవేళ (newp == ఖాళీ)
	తిరిగి FAIL;

    ml_replace(lnum, newp, FALSE);

    // mark the buffer as changed and prepare for displaying
    inserted_బైట్లు(lnum, curwin->w_cursor.నిలువు, -deleted);

    // If "fixpos" is TRUE we don't want to end up positioned at the NUL.
    ఒకవేళ (fixpos && curwin->w_cursor.నిలువు > 0)
	--curwin->w_cursor.నిలువు;

    తిరిగి OK;
}

/*
 * Delete "nlines" lines at the cursor.
 * Saves the lines for undo first if "undo" is TRUE.
 */
    శూన్య
del_పంక్తులు(దీర్ఘ nlines, పూర్ణాంకం undo)
{
    దీర్ఘ	n;
    linenr_T	మొదటి = curwin->w_cursor.lnum;

    ఒకవేళ (nlines <= 0)
	తిరిగి;

    // save the deleted lines for undo
    ఒకవేళ (undo && u_savedel(మొదటి, nlines) == FAIL)
	తిరిగి;

    కొరకు (n = 0; n < nlines; )
    {
	ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)	    // nothing to delete
	    ఆపు;

	ml_delete_flags(మొదటి, ML_DEL_సందేశం);
	++n;

	// If we delete the last line in the file, stop
	ఒకవేళ (మొదటి > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
    }

    // Correct the cursor position before calling deleted_lines_mark(), it may
    // trigger a callback to display the cursor.
    curwin->w_cursor.నిలువు = 0;
    check_cursor_lnum();

    // adjust marks, mark the buffer as changed and prepare for displaying
    deleted_పంక్తులు_mark(మొదటి, n);
}
