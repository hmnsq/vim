/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * gc.c: Garbage Collection
 */

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ساكن صحيح current_copyID = 0;

ساكن صحيح free_unref_عناصر(صحيح copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    صحيح
get_copyID(فراغ)
{
    current_copyID += COPYID_INC;
    إرجاع current_copyID;
}

/*
 * Garbage collection for lists and dictionaries.
 *
 * We use reference counts to be able to free most items right away when they
 * are no longer used.  But for composite items it's possible that it becomes
 * unused while the reference count is > 0: When there is a recursive
 * reference.  Example:
 *	:let l = [1, 2, 3]
 *	:let d = {9: l}
 *	:let l[1] = d
 *
 * Since this is quite unusual we handle this with garbage collection: every
 * once in a while find out which lists and dicts are not referenced from any
 * variable.
 *
 * Here is a good reference text about garbage collection (refers to Python
 * but it applies to all reference-counting mechanisms):
 *	http://python.ca/nas/python/gc/
 */

/*
 * Do garbage collection for lists and dicts.
 * When "testing" is TRUE this is called from test_garbagecollect_now().
 * Return TRUE if some memory was freed.
 */
    صحيح
garbage_collect(صحيح testing)
{
    صحيح		copyID;
    صحيح		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    صحيح		did_free = FALSE;
    tabpage_T	*tp;

    إذا (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    إذا (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	صحيح	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	إذا (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	إذا (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    إذا (pp == عدم)
		إرجاع FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_data = pp;
	}
    }

    // We advance by two because we add one for items referenced through
    // previous_funccal.
    copyID = get_copyID();

    /*
     * 1. Go through all accessible variables and mark all lists and dicts
     *    with copyID.
     */

    // Don't free variables in the previous_funccal list unless they are only
    // referenced through previous_funccal.  This must be first, because if
    // the item is referenced elsewhere the funccal must not be freed.
    abort = abort || set_ref_in_previous_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || set_ref_in_item(&buf->b_bufvar.di_tv, copyID,
							عدم, عدم, عدم);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							عدم, عدم, عدم);
    // window-local variables in autocmd windows
    لكل (صحيح i = 0; i < AUCMD_WIN_تعداد; ++i)
	إذا (aucmd_win[i].auc_win != عدم)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, عدم, عدم, عدم);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_مكدس(copyID);

    // named functions (matters for closures)
    abort = abort || set_ref_in_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || set_ref_in_func_args(copyID);

    // funcstacks keep variables for closures
    abort = abort || set_ref_in_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || set_ref_in_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || set_ref_in_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || set_ref_in_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || set_ref_in_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || set_ref_in_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || set_ref_in_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || set_ref_in_findfunc(copyID);

#ifdef FEAT_LUA
    abort = abort || set_ref_in_lua(copyID);
#endif

#ifdef FEAT_PYTHON
    abort = abort || set_ref_in_python(copyID);
#endif

#ifdef FEAT_PYTHON3
    abort = abort || set_ref_in_python3(copyID);
#endif

#ifdef FEAT_JOB_CHANNEL
    abort = abort || set_ref_in_channel(copyID);
    abort = abort || set_ref_in_job(copyID);
#endif
#ifdef FEAT_NETBEANS_INTG
    abort = abort || set_ref_in_nb_channel(copyID);
#endif
#ifdef FEAT_SOCKETSERVER
    abort = abort || set_ref_in_socketserver_channel(copyID);
#endif

#ifdef FEAT_TIMERS
    abort = abort || set_ref_in_timer(copyID);
#endif

#ifdef FEAT_QUICKFIX
    abort = abort || set_ref_in_quickfix(copyID);
#endif

#ifdef FEAT_TERMINAL
    abort = abort || set_ref_in_term(copyID);
#endif

#ifdef FEAT_PROP_POPUP
    abort = abort || set_ref_in_popups(copyID);
#endif

    abort = abort || set_ref_in_classes(copyID);

    إذا (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_عناصر(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    وإلا إذا (p_verbose > 0)
    {
	verb_رسالة(_("Not enough memory to set references, garbage collection aborted!"));
    }

    إرجاع did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ساكن صحيح
free_unref_عناصر(صحيح copyID)
{
    صحيح		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_عناصر = TRUE;

    /*
     * PASS 1: free the contents of the items.  We don't free the items
     * themselves yet, so that it is possible to decrement refcount counters
     */

    // Go through the list of dicts and free items without this copyID.
    did_free |= dict_free_nonref(copyID);

    // Go through the list of lists and free items without this copyID.
    did_free |= list_free_nonref(copyID);

    // Go through the list of tuples and free items without this copyID.
    did_free |= tuple_free_nonref(copyID);

    // Go through the list of objects and free items without this copyID.
    did_free |= object_free_nonref(copyID);

    // Go through the list of classes and free items without this copyID.
    did_free |= class_free_nonref(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_unused_jobs_contents(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_unused_channels_contents(copyID, COPYID_MASK);
#endif

    /*
     * PASS 2: free the items themselves.
     */
    object_free_عناصر(copyID);
    dict_free_عناصر(copyID);
    list_free_عناصر(copyID);
    tuple_free_عناصر(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_عناصر = FALSE;

    إرجاع did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_ht(
    hashtab_T		*ht,
    صحيح			copyID,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    صحيح		todo;
    صحيح		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_مكدس_T	*ht_مكدس = عدم;
    ht_مكدس_T	*tempitem;

    cur_ht = ht;
    لكل (;;)
    {
	إذا (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (صحيح)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_عناصر(cur_ht, hi, todo)
		إذا (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_مكدس, list_مكدس, tuple_مكدس);
		}
	}

	إذا (ht_مكدس == عدم)
	    اكسر;

	// take an item from the stack
	cur_ht = ht_مكدس->ht;
	tempitem = ht_مكدس;
	ht_مكدس = ht_مكدس->prev;
	حرر(tempitem);
    }

    إرجاع abort;
}

#إذا defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_dict(dict_T *d, صحيح copyID)
{
    إذا (d != عدم && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	إرجاع set_ref_in_ht(&d->dv_hashtab, copyID, عدم, عدم);
    }
    إرجاع FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_list(list_T *ll, صحيح copyID)
{
    إذا (ll != عدم && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	إرجاع set_ref_in_list_عناصر(ll, copyID, عدم, عدم);
    }
    إرجاع FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_list_عناصر(
    list_T		*l,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    listitem_T	 *li;
    صحيح		 abort = FALSE;
    list_T	 *cur_l;
    list_مكدس_T *list_مكدس = عدم;
    list_مكدس_T *tempitem;

    cur_l = l;
    لكل (;;)
    {
	إذا (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    لكل (li = cur_l->lv_first; !abort && li != عدم; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_مكدس, &list_مكدس, tuple_مكدس);
	إذا (list_مكدس == عدم)
	    اكسر;

	// take an item from the stack
	cur_l = list_مكدس->list;
	tempitem = list_مكدس;
	list_مكدس = list_مكدس->prev;
	حرر(tempitem);
    }

    إرجاع abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_tuple(tuple_T *tuple, صحيح copyID)
{
    إذا (tuple != عدم && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	إرجاع set_ref_in_tuple_عناصر(tuple, copyID, عدم, عدم);
    }
    إرجاع FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_tuple_عناصر(
    tuple_T		*tuple,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس)
{
    صحيح			abort = FALSE;
    tuple_T		*cur_t;
    tuple_مكدس_T	*tuple_مكدس = عدم;
    tuple_مكدس_T	*tempitem;

    cur_t = tuple;
    لكل (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	لكل (صحيح i = 0; i < cur_t->tv_عناصر.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_عناصر.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_مكدس, list_مكدس, &tuple_مكدس);
	}
	إذا (tuple_مكدس == عدم)
	    اكسر;

	// take an item from the stack
	cur_t = tuple_مكدس->tuple;
	tempitem = tuple_مكدس;
	tuple_مكدس = tuple_مكدس->prev;
	حرر(tempitem);
    }

    إرجاع abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    صحيح
set_ref_in_callback(callback_T *cb, صحيح copyID)
{
    typval_T tv;

    إذا (cb->cb_name == عدم || *cb->cb_name == NUL || cb->cb_partial == عدم)
	إرجاع FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    إرجاع set_ref_in_item(&tv, copyID, عدم, عدم, عدم);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_dict(
    dict_T		*dd,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    إذا (dd == عدم || dd->dv_copyID == copyID)
	إرجاع FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    إذا (ht_مكدس == عدم)
	إرجاع set_ref_in_ht(&dd->dv_hashtab, copyID, list_مكدس, tuple_مكدس);

    ht_مكدس_T *newitem = ALLOC_ONE(ht_مكدس_T);
    إذا (newitem == عدم)
	إرجاع TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_مكدس;
    *ht_مكدس = newitem;

    إرجاع FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_list(
    list_T		*ll,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    إذا (ll == عدم || ll->lv_copyID == copyID)
	إرجاع FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    إذا (list_مكدس == عدم)
	إرجاع set_ref_in_list_عناصر(ll, copyID, ht_مكدس, tuple_مكدس);

    list_مكدس_T *newitem = ALLOC_ONE(list_مكدس_T);
    إذا (newitem == عدم)
	إرجاع TRUE;

    newitem->list = ll;
    newitem->prev = *list_مكدس;
    *list_مكدس = newitem;

    إرجاع FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_tuple(
    tuple_T		*tt,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    إذا (tt == عدم || tt->tv_copyID == copyID)
	إرجاع FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    إذا (tuple_مكدس == عدم)
	إرجاع set_ref_in_tuple_عناصر(tt, copyID, ht_مكدس, list_مكدس);

    tuple_مكدس_T *newitem = ALLOC_ONE(tuple_مكدس_T);
    إذا (newitem == عدم)
	إرجاع TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_مكدس;
    *tuple_مكدس = newitem;

    إرجاع FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_partial(
    partial_T		*pt,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    إذا (pt == عدم || pt->pt_copyID == copyID)
	إرجاع FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    صحيح abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    إذا (pt->pt_dict != عدم)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }

    إذا (pt->pt_obj != عدم)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }

    لكل (صحيح i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_مكدس, list_مكدس, tuple_مكدس);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    إرجاع abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_job(
    job_T		*job,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    typval_T    dtv;

    إذا (job == عدم || job->jv_copyID == copyID)
	إرجاع FALSE;

    job->jv_copyID = copyID;
    إذا (job->jv_channel != عدم)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }
    إذا (job->jv_exit_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }

    إرجاع FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_channel(
    channel_T		*ch,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    typval_T    dtv;

    إذا (ch == عدم || ch->ch_copyID == copyID)
	إرجاع FALSE;

    ch->ch_copyID = copyID;
    لكل (ch_part_T part = PART_SOCK; part < PART_تعداد; ++part)
    {
	لكل (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != عدم; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_قيمة, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
	لكل (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != عدم;
		cq = cq->cq_next)
	    إذا (cq->cq_callback.cb_partial != عدم)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
	    }
	إذا (ch->ch_part[part].ch_callback.cb_partial != عدم)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
	}
    }
    إذا (ch->ch_callback.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }
    إذا (ch->ch_close_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }

    إرجاع FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    صحيح
set_ref_in_item_class(
    class_T		*cl,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    صحيح abort = FALSE;

    إذا (cl == عدم || cl->class_copyID == copyID)
	إرجاع FALSE;

    cl->class_copyID = copyID;
    إذا (cl->class_members_tv != عدم)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	لكل (صحيح i = 0; !abort && i < cl->class_class_member_تعداد; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_مكدس, list_مكدس, tuple_مكدس);
    }

    لكل (صحيح i = 0; !abort && i < cl->class_class_function_تعداد; ++i)
	abort = abort || set_ref_in_func(عدم,
		cl->class_class_functions[i], copyID);

    لكل (صحيح i = 0; !abort && i < cl->class_obj_method_تعداد; ++i)
	abort = abort || set_ref_in_func(عدم,
		cl->class_obj_methods[i], copyID);

    إرجاع abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
set_ref_in_item_object(
    object_T		*obj,
    صحيح			copyID,
    ht_مكدس_T		**ht_مكدس,
    list_مكدس_T	**list_مكدس,
    tuple_مكدس_T	**tuple_مكدس)
{
    صحيح abort = FALSE;

    إذا (obj == عدم || obj->obj_copyID == copyID)
	إرجاع FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    لكل (صحيح i = 0; !abort
	    && i < obj->obj_class->class_obj_member_تعداد; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_مكدس, list_مكدس, tuple_مكدس);

    إرجاع abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_item(
    typval_T	    *tv,
    صحيح		    copyID,
    ht_مكدس_T	    **ht_مكدس,
    list_مكدس_T    **list_مكدس,
    tuple_مكدس_T   **tuple_مكدس)
{
    صحيح		abort = FALSE;

    بدل (tv->v_type)
    {
	حالة VAR_DICT:
	    إرجاع set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_مكدس, list_مكدس, tuple_مكدس);

	حالة VAR_LIST:
	    إرجاع set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_مكدس, list_مكدس, tuple_مكدس);

	حالة VAR_TUPLE:
	    إرجاع set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_مكدس, list_مكدس, tuple_مكدس);
	حالة VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, عدم, copyID);
	    اكسر;
	}

	حالة VAR_PARTIAL:
	    إرجاع set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_مكدس, list_مكدس, tuple_مكدس);

	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    إرجاع set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_مكدس, list_مكدس, tuple_مكدس);
#وإلا
	    اكسر;
#endif

	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    إرجاع set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_مكدس, list_مكدس, tuple_مكدس);
#وإلا
	    اكسر;
#endif

	حالة VAR_CLASS:
	    إرجاع set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_مكدس, list_مكدس, tuple_مكدس);

	حالة VAR_OBJECT:
	    إرجاع set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_مكدس, list_مكدس, tuple_مكدس);

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_العدد:
	حالة VAR_FLOAT:
	حالة VAR_STRING:
	حالة VAR_BLOB:
	حالة VAR_TYPEALIAS:
	حالة VAR_INSTR:
	    // Types that do not contain any other item
	    اكسر;
    }

    إرجاع abort;
}

#endif
