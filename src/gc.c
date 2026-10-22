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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ایستا صحیح current_copyID = 0;

ایستا صحیح free_unref_اقلام(صحیح copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    صحیح
get_copyID(تهی)
{
    current_copyID += COPYID_INC;
    بازگشت current_copyID;
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
    صحیح
garbage_collect(صحیح testing)
{
    صحیح		copyID;
    صحیح		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    صحیح		did_free = FALSE;
    tabpage_T	*tp;

    اگر (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    اگر (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	صحیح	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	اگر (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	اگر (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    اگر (pp == پوچ)
		بازگشت FAIL;
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
							پوچ, پوچ, پوچ);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							پوچ, پوچ, پوچ);
    // window-local variables in autocmd windows
    برای (صحیح i = 0; i < AUCMD_WIN_شمار; ++i)
	اگر (aucmd_win[i].auc_win != پوچ)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, پوچ, پوچ, پوچ);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  پوچ, پوچ, پوچ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  پوچ, پوچ, پوچ);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  پوچ, پوچ, پوچ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_پشته(copyID);

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

    اگر (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_اقلام(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    وگرنه اگر (p_verbose > 0)
    {
	verb_پیام(_("Not enough memory to set references, garbage collection aborted!"));
    }

    بازگشت did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ایستا صحیح
free_unref_اقلام(صحیح copyID)
{
    صحیح		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_اقلام = TRUE;

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
    object_free_اقلام(copyID);
    dict_free_اقلام(copyID);
    list_free_اقلام(copyID);
    tuple_free_اقلام(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_اقلام = FALSE;

    بازگشت did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_ht(
    hashtab_T		*ht,
    صحیح			copyID,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    صحیح		todo;
    صحیح		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_پشته_T	*ht_پشته = پوچ;
    ht_پشته_T	*tempitem;

    cur_ht = ht;
    برای (;;)
    {
	اگر (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (صحیح)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_اقلام(cur_ht, hi, todo)
		اگر (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_پشته, list_پشته, tuple_پشته);
		}
	}

	اگر (ht_پشته == پوچ)
	    بشکن;

	// take an item from the stack
	cur_ht = ht_پشته->ht;
	tempitem = ht_پشته;
	ht_پشته = ht_پشته->prev;
	آزاد(tempitem);
    }

    بازگشت abort;
}

#اگر defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_dict(dict_T *d, صحیح copyID)
{
    اگر (d != پوچ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	بازگشت set_ref_in_ht(&d->dv_hashtab, copyID, پوچ, پوچ);
    }
    بازگشت FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_list(list_T *ll, صحیح copyID)
{
    اگر (ll != پوچ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	بازگشت set_ref_in_list_اقلام(ll, copyID, پوچ, پوچ);
    }
    بازگشت FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_list_اقلام(
    list_T		*l,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    listitem_T	 *li;
    صحیح		 abort = FALSE;
    list_T	 *cur_l;
    list_پشته_T *list_پشته = پوچ;
    list_پشته_T *tempitem;

    cur_l = l;
    برای (;;)
    {
	اگر (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    برای (li = cur_l->lv_first; !abort && li != پوچ; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_پشته, &list_پشته, tuple_پشته);
	اگر (list_پشته == پوچ)
	    بشکن;

	// take an item from the stack
	cur_l = list_پشته->list;
	tempitem = list_پشته;
	list_پشته = list_پشته->prev;
	آزاد(tempitem);
    }

    بازگشت abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_tuple(tuple_T *tuple, صحیح copyID)
{
    اگر (tuple != پوچ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	بازگشت set_ref_in_tuple_اقلام(tuple, copyID, پوچ, پوچ);
    }
    بازگشت FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_tuple_اقلام(
    tuple_T		*tuple,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته)
{
    صحیح			abort = FALSE;
    tuple_T		*cur_t;
    tuple_پشته_T	*tuple_پشته = پوچ;
    tuple_پشته_T	*tempitem;

    cur_t = tuple;
    برای (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	برای (صحیح i = 0; i < cur_t->tv_اقلام.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_اقلام.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_پشته, list_پشته, &tuple_پشته);
	}
	اگر (tuple_پشته == پوچ)
	    بشکن;

	// take an item from the stack
	cur_t = tuple_پشته->tuple;
	tempitem = tuple_پشته;
	tuple_پشته = tuple_پشته->prev;
	آزاد(tempitem);
    }

    بازگشت abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    صحیح
set_ref_in_callback(callback_T *cb, صحیح copyID)
{
    typval_T tv;

    اگر (cb->cb_name == پوچ || *cb->cb_name == NUL || cb->cb_partial == پوچ)
	بازگشت FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    بازگشت set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_dict(
    dict_T		*dd,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    اگر (dd == پوچ || dd->dv_copyID == copyID)
	بازگشت FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    اگر (ht_پشته == پوچ)
	بازگشت set_ref_in_ht(&dd->dv_hashtab, copyID, list_پشته, tuple_پشته);

    ht_پشته_T *newitem = ALLOC_ONE(ht_پشته_T);
    اگر (newitem == پوچ)
	بازگشت TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_پشته;
    *ht_پشته = newitem;

    بازگشت FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_list(
    list_T		*ll,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    اگر (ll == پوچ || ll->lv_copyID == copyID)
	بازگشت FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    اگر (list_پشته == پوچ)
	بازگشت set_ref_in_list_اقلام(ll, copyID, ht_پشته, tuple_پشته);

    list_پشته_T *newitem = ALLOC_ONE(list_پشته_T);
    اگر (newitem == پوچ)
	بازگشت TRUE;

    newitem->list = ll;
    newitem->prev = *list_پشته;
    *list_پشته = newitem;

    بازگشت FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_tuple(
    tuple_T		*tt,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    اگر (tt == پوچ || tt->tv_copyID == copyID)
	بازگشت FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    اگر (tuple_پشته == پوچ)
	بازگشت set_ref_in_tuple_اقلام(tt, copyID, ht_پشته, list_پشته);

    tuple_پشته_T *newitem = ALLOC_ONE(tuple_پشته_T);
    اگر (newitem == پوچ)
	بازگشت TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_پشته;
    *tuple_پشته = newitem;

    بازگشت FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_partial(
    partial_T		*pt,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    اگر (pt == پوچ || pt->pt_copyID == copyID)
	بازگشت FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    صحیح abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    اگر (pt->pt_dict != پوچ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }

    اگر (pt->pt_obj != پوچ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }

    برای (صحیح i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_پشته, list_پشته, tuple_پشته);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    بازگشت abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_job(
    job_T		*job,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    typval_T    dtv;

    اگر (job == پوچ || job->jv_copyID == copyID)
	بازگشت FALSE;

    job->jv_copyID = copyID;
    اگر (job->jv_channel != پوچ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }
    اگر (job->jv_exit_cb.cb_partial != پوچ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }

    بازگشت FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_channel(
    channel_T		*ch,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    typval_T    dtv;

    اگر (ch == پوچ || ch->ch_copyID == copyID)
	بازگشت FALSE;

    ch->ch_copyID = copyID;
    برای (ch_part_T part = PART_SOCK; part < PART_شمار; ++part)
    {
	برای (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != پوچ; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_مقدار, copyID, ht_پشته, list_پشته, tuple_پشته);
	برای (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != پوچ;
		cq = cq->cq_next)
	    اگر (cq->cq_callback.cb_partial != پوچ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
	    }
	اگر (ch->ch_part[part].ch_callback.cb_partial != پوچ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
	}
    }
    اگر (ch->ch_callback.cb_partial != پوچ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }
    اگر (ch->ch_close_cb.cb_partial != پوچ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_پشته, list_پشته, tuple_پشته);
    }

    بازگشت FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    صحیح
set_ref_in_item_class(
    class_T		*cl,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    صحیح abort = FALSE;

    اگر (cl == پوچ || cl->class_copyID == copyID)
	بازگشت FALSE;

    cl->class_copyID = copyID;
    اگر (cl->class_members_tv != پوچ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	برای (صحیح i = 0; !abort && i < cl->class_class_member_شمار; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_پشته, list_پشته, tuple_پشته);
    }

    برای (صحیح i = 0; !abort && i < cl->class_class_function_شمار; ++i)
	abort = abort || set_ref_in_func(پوچ,
		cl->class_class_functions[i], copyID);

    برای (صحیح i = 0; !abort && i < cl->class_obj_method_شمار; ++i)
	abort = abort || set_ref_in_func(پوچ,
		cl->class_obj_methods[i], copyID);

    بازگشت abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ایستا صحیح
set_ref_in_item_object(
    object_T		*obj,
    صحیح			copyID,
    ht_پشته_T		**ht_پشته,
    list_پشته_T	**list_پشته,
    tuple_پشته_T	**tuple_پشته)
{
    صحیح abort = FALSE;

    اگر (obj == پوچ || obj->obj_copyID == copyID)
	بازگشت FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    برای (صحیح i = 0; !abort
	    && i < obj->obj_class->class_obj_member_شمار; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_پشته, list_پشته, tuple_پشته);

    بازگشت abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_item(
    typval_T	    *tv,
    صحیح		    copyID,
    ht_پشته_T	    **ht_پشته,
    list_پشته_T    **list_پشته,
    tuple_پشته_T   **tuple_پشته)
{
    صحیح		abort = FALSE;

    انتخاب (tv->v_type)
    {
	حالت VAR_DICT:
	    بازگشت set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_پشته, list_پشته, tuple_پشته);

	حالت VAR_LIST:
	    بازگشت set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_پشته, list_پشته, tuple_پشته);

	حالت VAR_TUPLE:
	    بازگشت set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_پشته, list_پشته, tuple_پشته);
	حالت VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, پوچ, copyID);
	    بشکن;
	}

	حالت VAR_PARTIAL:
	    بازگشت set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_پشته, list_پشته, tuple_پشته);

	حالت VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    بازگشت set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_پشته, list_پشته, tuple_پشته);
#وگرنه
	    بشکن;
#endif

	حالت VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    بازگشت set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_پشته, list_پشته, tuple_پشته);
#وگرنه
	    بشکن;
#endif

	حالت VAR_CLASS:
	    بازگشت set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_پشته, list_پشته, tuple_پشته);

	حالت VAR_OBJECT:
	    بازگشت set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_پشته, list_پشته, tuple_پشته);

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_عدد:
	حالت VAR_FLOAT:
	حالت VAR_STRING:
	حالت VAR_BLOB:
	حالت VAR_TYPEALIAS:
	حالت VAR_INSTR:
	    // Types that do not contain any other item
	    بشکن;
    }

    بازگشت abort;
}

#endif
