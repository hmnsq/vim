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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
સ્થિર પૂર્ણાંક current_copyID = 0;

સ્થિર પૂર્ણાંક free_unref_વસ્તુઓ(પૂર્ણાંક copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    પૂર્ણાંક
get_copyID(શૂન્ય)
{
    current_copyID += COPYID_INC;
    પાછા current_copyID;
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
    પૂર્ણાંક
garbage_collect(પૂર્ણાંક testing)
{
    પૂર્ણાંક		copyID;
    પૂર્ણાંક		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    પૂર્ણાંક		did_free = FALSE;
    tabpage_T	*tp;

    જો (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    જો (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	પૂર્ણાંક	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	જો (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	જો (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    જો (pp == ખાલી)
		પાછા FAIL;
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
							ખાલી, ખાલી, ખાલી);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							ખાલી, ખાલી, ખાલી);
    // window-local variables in autocmd windows
    માટે (પૂર્ણાંક i = 0; i < AUCMD_WIN_ગણતરી; ++i)
	જો (aucmd_win[i].auc_win != ખાલી)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ખાલી, ખાલી, ખાલી);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ખાલી, ખાલી, ખાલી);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ખાલી, ખાલી, ખાલી);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  ખાલી, ખાલી, ખાલી);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_ઢગલો(copyID);

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

    જો (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_વસ્તુઓ(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    નહીંતર જો (p_verbose > 0)
    {
	verb_સંદેશ(_("Not enough memory to set references, garbage collection aborted!"));
    }

    પાછા did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    સ્થિર પૂર્ણાંક
free_unref_વસ્તુઓ(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_વસ્તુઓ = TRUE;

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
    object_free_વસ્તુઓ(copyID);
    dict_free_વસ્તુઓ(copyID);
    list_free_વસ્તુઓ(copyID);
    tuple_free_વસ્તુઓ(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_વસ્તુઓ = FALSE;

    પાછા did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_ht(
    hashtab_T		*ht,
    પૂર્ણાંક			copyID,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    પૂર્ણાંક		todo;
    પૂર્ણાંક		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ઢગલો_T	*ht_ઢગલો = ખાલી;
    ht_ઢગલો_T	*tempitem;

    cur_ht = ht;
    માટે (;;)
    {
	જો (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (પૂર્ણાંક)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(cur_ht, hi, todo)
		જો (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
		}
	}

	જો (ht_ઢગલો == ખાલી)
	    તોડો;

	// take an item from the stack
	cur_ht = ht_ઢગલો->ht;
	tempitem = ht_ઢગલો;
	ht_ઢગલો = ht_ઢગલો->prev;
	મુક્ત(tempitem);
    }

    પાછા abort;
}

#જો defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_dict(dict_T *d, પૂર્ણાંક copyID)
{
    જો (d != ખાલી && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	પાછા set_ref_in_ht(&d->dv_hashtab, copyID, ખાલી, ખાલી);
    }
    પાછા FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_list(list_T *ll, પૂર્ણાંક copyID)
{
    જો (ll != ખાલી && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	પાછા set_ref_in_list_વસ્તુઓ(ll, copyID, ખાલી, ખાલી);
    }
    પાછા FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_list_વસ્તુઓ(
    list_T		*l,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    listitem_T	 *li;
    પૂર્ણાંક		 abort = FALSE;
    list_T	 *cur_l;
    list_ઢગલો_T *list_ઢગલો = ખાલી;
    list_ઢગલો_T *tempitem;

    cur_l = l;
    માટે (;;)
    {
	જો (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    માટે (li = cur_l->lv_first; !abort && li != ખાલી; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_ઢગલો, &list_ઢગલો, tuple_ઢગલો);
	જો (list_ઢગલો == ખાલી)
	    તોડો;

	// take an item from the stack
	cur_l = list_ઢગલો->list;
	tempitem = list_ઢગલો;
	list_ઢગલો = list_ઢગલો->prev;
	મુક્ત(tempitem);
    }

    પાછા abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_tuple(tuple_T *tuple, પૂર્ણાંક copyID)
{
    જો (tuple != ખાલી && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	પાછા set_ref_in_tuple_વસ્તુઓ(tuple, copyID, ખાલી, ખાલી);
    }
    પાછા FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_tuple_વસ્તુઓ(
    tuple_T		*tuple,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો)
{
    પૂર્ણાંક			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ઢગલો_T	*tuple_ઢગલો = ખાલી;
    tuple_ઢગલો_T	*tempitem;

    cur_t = tuple;
    માટે (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	માટે (પૂર્ણાંક i = 0; i < cur_t->tv_વસ્તુઓ.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_વસ્તુઓ.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_ઢગલો, list_ઢગલો, &tuple_ઢગલો);
	}
	જો (tuple_ઢગલો == ખાલી)
	    તોડો;

	// take an item from the stack
	cur_t = tuple_ઢગલો->tuple;
	tempitem = tuple_ઢગલો;
	tuple_ઢગલો = tuple_ઢગલો->prev;
	મુક્ત(tempitem);
    }

    પાછા abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    પૂર્ણાંક
set_ref_in_callback(callback_T *cb, પૂર્ણાંક copyID)
{
    typval_T tv;

    જો (cb->cb_name == ખાલી || *cb->cb_name == NUL || cb->cb_partial == ખાલી)
	પાછા FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    પાછા set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_dict(
    dict_T		*dd,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    જો (dd == ખાલી || dd->dv_copyID == copyID)
	પાછા FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    જો (ht_ઢગલો == ખાલી)
	પાછા set_ref_in_ht(&dd->dv_hashtab, copyID, list_ઢગલો, tuple_ઢગલો);

    ht_ઢગલો_T *newitem = ALLOC_ONE(ht_ઢગલો_T);
    જો (newitem == ખાલી)
	પાછા TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ઢગલો;
    *ht_ઢગલો = newitem;

    પાછા FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_list(
    list_T		*ll,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    જો (ll == ખાલી || ll->lv_copyID == copyID)
	પાછા FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    જો (list_ઢગલો == ખાલી)
	પાછા set_ref_in_list_વસ્તુઓ(ll, copyID, ht_ઢગલો, tuple_ઢગલો);

    list_ઢગલો_T *newitem = ALLOC_ONE(list_ઢગલો_T);
    જો (newitem == ખાલી)
	પાછા TRUE;

    newitem->list = ll;
    newitem->prev = *list_ઢગલો;
    *list_ઢગલો = newitem;

    પાછા FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_tuple(
    tuple_T		*tt,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    જો (tt == ખાલી || tt->tv_copyID == copyID)
	પાછા FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    જો (tuple_ઢગલો == ખાલી)
	પાછા set_ref_in_tuple_વસ્તુઓ(tt, copyID, ht_ઢગલો, list_ઢગલો);

    tuple_ઢગલો_T *newitem = ALLOC_ONE(tuple_ઢગલો_T);
    જો (newitem == ખાલી)
	પાછા TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ઢગલો;
    *tuple_ઢગલો = newitem;

    પાછા FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_partial(
    partial_T		*pt,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    જો (pt == ખાલી || pt->pt_copyID == copyID)
	પાછા FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    પૂર્ણાંક abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    જો (pt->pt_dict != ખાલી)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }

    જો (pt->pt_obj != ખાલી)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }

    માટે (પૂર્ણાંક i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    પાછા abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_job(
    job_T		*job,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    typval_T    dtv;

    જો (job == ખાલી || job->jv_copyID == copyID)
	પાછા FALSE;

    job->jv_copyID = copyID;
    જો (job->jv_channel != ખાલી)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }
    જો (job->jv_exit_cb.cb_partial != ખાલી)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }

    પાછા FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_channel(
    channel_T		*ch,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    typval_T    dtv;

    જો (ch == ખાલી || ch->ch_copyID == copyID)
	પાછા FALSE;

    ch->ch_copyID = copyID;
    માટે (ch_part_T part = PART_SOCK; part < PART_ગણતરી; ++part)
    {
	માટે (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != ખાલી; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_કિંમત, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
	માટે (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != ખાલી;
		cq = cq->cq_next)
	    જો (cq->cq_callback.cb_partial != ખાલી)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
	    }
	જો (ch->ch_part[part].ch_callback.cb_partial != ખાલી)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
	}
    }
    જો (ch->ch_callback.cb_partial != ખાલી)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }
    જો (ch->ch_close_cb.cb_partial != ખાલી)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }

    પાછા FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    પૂર્ણાંક
set_ref_in_item_class(
    class_T		*cl,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    પૂર્ણાંક abort = FALSE;

    જો (cl == ખાલી || cl->class_copyID == copyID)
	પાછા FALSE;

    cl->class_copyID = copyID;
    જો (cl->class_members_tv != ખાલી)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	માટે (પૂર્ણાંક i = 0; !abort && i < cl->class_class_member_ગણતરી; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
    }

    માટે (પૂર્ણાંક i = 0; !abort && i < cl->class_class_function_ગણતરી; ++i)
	abort = abort || set_ref_in_func(ખાલી,
		cl->class_class_functions[i], copyID);

    માટે (પૂર્ણાંક i = 0; !abort && i < cl->class_obj_method_ગણતરી; ++i)
	abort = abort || set_ref_in_func(ખાલી,
		cl->class_obj_methods[i], copyID);

    પાછા abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    સ્થિર પૂર્ણાંક
set_ref_in_item_object(
    object_T		*obj,
    પૂર્ણાંક			copyID,
    ht_ઢગલો_T		**ht_ઢગલો,
    list_ઢગલો_T	**list_ઢગલો,
    tuple_ઢગલો_T	**tuple_ઢગલો)
{
    પૂર્ણાંક abort = FALSE;

    જો (obj == ખાલી || obj->obj_copyID == copyID)
	પાછા FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    માટે (પૂર્ણાંક i = 0; !abort
	    && i < obj->obj_class->class_obj_member_ગણતરી; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

    પાછા abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_item(
    typval_T	    *tv,
    પૂર્ણાંક		    copyID,
    ht_ઢગલો_T	    **ht_ઢગલો,
    list_ઢગલો_T    **list_ઢગલો,
    tuple_ઢગલો_T   **tuple_ઢગલો)
{
    પૂર્ણાંક		abort = FALSE;

    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_DICT:
	    પાછા set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

	સ્થિતિ VAR_LIST:
	    પાછા set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

	સ્થિતિ VAR_TUPLE:
	    પાછા set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
	સ્થિતિ VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, ખાલી, copyID);
	    તોડો;
	}

	સ્થિતિ VAR_PARTIAL:
	    પાછા set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    પાછા set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
#નહીંતર
	    તોડો;
#endif

	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    પાછા set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);
#નહીંતર
	    તોડો;
#endif

	સ્થિતિ VAR_CLASS:
	    પાછા set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

	સ્થિતિ VAR_OBJECT:
	    પાછા set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_ઢગલો, list_ઢગલો, tuple_ઢગલો);

	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_BLOB:
	સ્થિતિ VAR_TYPEALIAS:
	સ્થિતિ VAR_INSTR:
	    // Types that do not contain any other item
	    તોડો;
    }

    પાછા abort;
}

#endif
