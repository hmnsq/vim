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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
நிலையான முழு current_copyID = 0;

நிலையான முழு free_unref_பொருட்கள்(முழு copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    முழு
get_copyID(வெறுமை)
{
    current_copyID += COPYID_INC;
    திரும்பு current_copyID;
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
    முழு
garbage_collect(முழு testing)
{
    முழு		copyID;
    முழு		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    முழு		did_free = FALSE;
    tabpage_T	*tp;

    எனில் (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    எனில் (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	முழு	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	எனில் (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	எனில் (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    எனில் (pp == வெற்று)
		திரும்பு FAIL;
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
							வெற்று, வெற்று, வெற்று);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							வெற்று, வெற்று, வெற்று);
    // window-local variables in autocmd windows
    க்கு (முழு i = 0; i < AUCMD_WIN_மொத்தம்; ++i)
	எனில் (aucmd_win[i].auc_win != வெற்று)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, வெற்று, வெற்று, வெற்று);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  வெற்று, வெற்று, வெற்று);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  வெற்று, வெற்று, வெற்று);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  வெற்று, வெற்று, வெற்று);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_அடுக்கு(copyID);

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

    எனில் (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_பொருட்கள்(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    இல்லையெனில் எனில் (p_verbose > 0)
    {
	verb_செய்தி(_("Not enough memory to set references, garbage collection aborted!"));
    }

    திரும்பு did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    நிலையான முழு
free_unref_பொருட்கள்(முழு copyID)
{
    முழு		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_பொருட்கள் = TRUE;

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
    object_free_பொருட்கள்(copyID);
    dict_free_பொருட்கள்(copyID);
    list_free_பொருட்கள்(copyID);
    tuple_free_பொருட்கள்(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_பொருட்கள் = FALSE;

    திரும்பு did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_ht(
    hashtab_T		*ht,
    முழு			copyID,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    முழு		todo;
    முழு		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_அடுக்கு_T	*ht_அடுக்கு = வெற்று;
    ht_அடுக்கு_T	*tempitem;

    cur_ht = ht;
    க்கு (;;)
    {
	எனில் (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (முழு)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_பொருட்கள்(cur_ht, hi, todo)
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
		}
	}

	எனில் (ht_அடுக்கு == வெற்று)
	    நிறுத்து;

	// take an item from the stack
	cur_ht = ht_அடுக்கு->ht;
	tempitem = ht_அடுக்கு;
	ht_அடுக்கு = ht_அடுக்கு->prev;
	விடுவி(tempitem);
    }

    திரும்பு abort;
}

#எனில் defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_dict(dict_T *d, முழு copyID)
{
    எனில் (d != வெற்று && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	திரும்பு set_ref_in_ht(&d->dv_hashtab, copyID, வெற்று, வெற்று);
    }
    திரும்பு FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_list(list_T *ll, முழு copyID)
{
    எனில் (ll != வெற்று && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	திரும்பு set_ref_in_list_பொருட்கள்(ll, copyID, வெற்று, வெற்று);
    }
    திரும்பு FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_list_பொருட்கள்(
    list_T		*l,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    listitem_T	 *li;
    முழு		 abort = FALSE;
    list_T	 *cur_l;
    list_அடுக்கு_T *list_அடுக்கு = வெற்று;
    list_அடுக்கு_T *tempitem;

    cur_l = l;
    க்கு (;;)
    {
	எனில் (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    க்கு (li = cur_l->lv_first; !abort && li != வெற்று; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_அடுக்கு, &list_அடுக்கு, tuple_அடுக்கு);
	எனில் (list_அடுக்கு == வெற்று)
	    நிறுத்து;

	// take an item from the stack
	cur_l = list_அடுக்கு->list;
	tempitem = list_அடுக்கு;
	list_அடுக்கு = list_அடுக்கு->prev;
	விடுவி(tempitem);
    }

    திரும்பு abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_tuple(tuple_T *tuple, முழு copyID)
{
    எனில் (tuple != வெற்று && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	திரும்பு set_ref_in_tuple_பொருட்கள்(tuple, copyID, வெற்று, வெற்று);
    }
    திரும்பு FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_tuple_பொருட்கள்(
    tuple_T		*tuple,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு)
{
    முழு			abort = FALSE;
    tuple_T		*cur_t;
    tuple_அடுக்கு_T	*tuple_அடுக்கு = வெற்று;
    tuple_அடுக்கு_T	*tempitem;

    cur_t = tuple;
    க்கு (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	க்கு (முழு i = 0; i < cur_t->tv_பொருட்கள்.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_பொருட்கள்.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_அடுக்கு, list_அடுக்கு, &tuple_அடுக்கு);
	}
	எனில் (tuple_அடுக்கு == வெற்று)
	    நிறுத்து;

	// take an item from the stack
	cur_t = tuple_அடுக்கு->tuple;
	tempitem = tuple_அடுக்கு;
	tuple_அடுக்கு = tuple_அடுக்கு->prev;
	விடுவி(tempitem);
    }

    திரும்பு abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    முழு
set_ref_in_callback(callback_T *cb, முழு copyID)
{
    typval_T tv;

    எனில் (cb->cb_name == வெற்று || *cb->cb_name == NUL || cb->cb_partial == வெற்று)
	திரும்பு FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    திரும்பு set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_dict(
    dict_T		*dd,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    எனில் (dd == வெற்று || dd->dv_copyID == copyID)
	திரும்பு FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    எனில் (ht_அடுக்கு == வெற்று)
	திரும்பு set_ref_in_ht(&dd->dv_hashtab, copyID, list_அடுக்கு, tuple_அடுக்கு);

    ht_அடுக்கு_T *newitem = ALLOC_ONE(ht_அடுக்கு_T);
    எனில் (newitem == வெற்று)
	திரும்பு TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_அடுக்கு;
    *ht_அடுக்கு = newitem;

    திரும்பு FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_list(
    list_T		*ll,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    எனில் (ll == வெற்று || ll->lv_copyID == copyID)
	திரும்பு FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    எனில் (list_அடுக்கு == வெற்று)
	திரும்பு set_ref_in_list_பொருட்கள்(ll, copyID, ht_அடுக்கு, tuple_அடுக்கு);

    list_அடுக்கு_T *newitem = ALLOC_ONE(list_அடுக்கு_T);
    எனில் (newitem == வெற்று)
	திரும்பு TRUE;

    newitem->list = ll;
    newitem->prev = *list_அடுக்கு;
    *list_அடுக்கு = newitem;

    திரும்பு FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_tuple(
    tuple_T		*tt,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    எனில் (tt == வெற்று || tt->tv_copyID == copyID)
	திரும்பு FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    எனில் (tuple_அடுக்கு == வெற்று)
	திரும்பு set_ref_in_tuple_பொருட்கள்(tt, copyID, ht_அடுக்கு, list_அடுக்கு);

    tuple_அடுக்கு_T *newitem = ALLOC_ONE(tuple_அடுக்கு_T);
    எனில் (newitem == வெற்று)
	திரும்பு TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_அடுக்கு;
    *tuple_அடுக்கு = newitem;

    திரும்பு FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_partial(
    partial_T		*pt,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    எனில் (pt == வெற்று || pt->pt_copyID == copyID)
	திரும்பு FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    முழு abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    எனில் (pt->pt_dict != வெற்று)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }

    எனில் (pt->pt_obj != வெற்று)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }

    க்கு (முழு i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    திரும்பு abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_job(
    job_T		*job,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    typval_T    dtv;

    எனில் (job == வெற்று || job->jv_copyID == copyID)
	திரும்பு FALSE;

    job->jv_copyID = copyID;
    எனில் (job->jv_channel != வெற்று)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }
    எனில் (job->jv_exit_cb.cb_partial != வெற்று)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }

    திரும்பு FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_channel(
    channel_T		*ch,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    typval_T    dtv;

    எனில் (ch == வெற்று || ch->ch_copyID == copyID)
	திரும்பு FALSE;

    ch->ch_copyID = copyID;
    க்கு (ch_part_T part = PART_SOCK; part < PART_மொத்தம்; ++part)
    {
	க்கு (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != வெற்று; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_மதிப்பு, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
	க்கு (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != வெற்று;
		cq = cq->cq_next)
	    எனில் (cq->cq_callback.cb_partial != வெற்று)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
	    }
	எனில் (ch->ch_part[part].ch_callback.cb_partial != வெற்று)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
	}
    }
    எனில் (ch->ch_callback.cb_partial != வெற்று)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }
    எனில் (ch->ch_close_cb.cb_partial != வெற்று)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }

    திரும்பு FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    முழு
set_ref_in_item_class(
    class_T		*cl,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    முழு abort = FALSE;

    எனில் (cl == வெற்று || cl->class_copyID == copyID)
	திரும்பு FALSE;

    cl->class_copyID = copyID;
    எனில் (cl->class_members_tv != வெற்று)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	க்கு (முழு i = 0; !abort && i < cl->class_class_member_மொத்தம்; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
    }

    க்கு (முழு i = 0; !abort && i < cl->class_class_function_மொத்தம்; ++i)
	abort = abort || set_ref_in_func(வெற்று,
		cl->class_class_functions[i], copyID);

    க்கு (முழு i = 0; !abort && i < cl->class_obj_method_மொத்தம்; ++i)
	abort = abort || set_ref_in_func(வெற்று,
		cl->class_obj_methods[i], copyID);

    திரும்பு abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    நிலையான முழு
set_ref_in_item_object(
    object_T		*obj,
    முழு			copyID,
    ht_அடுக்கு_T		**ht_அடுக்கு,
    list_அடுக்கு_T	**list_அடுக்கு,
    tuple_அடுக்கு_T	**tuple_அடுக்கு)
{
    முழு abort = FALSE;

    எனில் (obj == வெற்று || obj->obj_copyID == copyID)
	திரும்பு FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    க்கு (முழு i = 0; !abort
	    && i < obj->obj_class->class_obj_member_மொத்தம்; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

    திரும்பு abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_item(
    typval_T	    *tv,
    முழு		    copyID,
    ht_அடுக்கு_T	    **ht_அடுக்கு,
    list_அடுக்கு_T    **list_அடுக்கு,
    tuple_அடுக்கு_T   **tuple_அடுக்கு)
{
    முழு		abort = FALSE;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_DICT:
	    திரும்பு set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

	நிலை VAR_LIST:
	    திரும்பு set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

	நிலை VAR_TUPLE:
	    திரும்பு set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
	நிலை VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, வெற்று, copyID);
	    நிறுத்து;
	}

	நிலை VAR_PARTIAL:
	    திரும்பு set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
#இல்லையெனில்
	    நிறுத்து;
#endif

	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);
#இல்லையெனில்
	    நிறுத்து;
#endif

	நிலை VAR_CLASS:
	    திரும்பு set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

	நிலை VAR_OBJECT:
	    திரும்பு set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_அடுக்கு, list_அடுக்கு, tuple_அடுக்கு);

	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_எண்:
	நிலை VAR_FLOAT:
	நிலை VAR_STRING:
	நிலை VAR_BLOB:
	நிலை VAR_TYPEALIAS:
	நிலை VAR_INSTR:
	    // Types that do not contain any other item
	    நிறுத்து;
    }

    திரும்பு abort;
}

#endif
