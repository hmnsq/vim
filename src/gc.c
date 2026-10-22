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

#include "vim.h"

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
static puur_naa_nka current_copyID = 0;

static puur_naa_nka free_unref_items(puur_naa_nka copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    puur_naa_nka
get_copyID(void)
{
    current_copyID += COPYID_INC;
    ᮝᮜᮤ current_copyID;
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
    puur_naa_nka
garbage_collect(puur_naa_nka testing)
{
    puur_naa_nka		copyID;
    puur_naa_nka		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    puur_naa_nka		did_free = FALSE;
    tabpage_T	*tp;

    ᮚᮦᮔ᮪ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    ᮚᮦᮔ᮪ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	puur_naa_nka	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	ᮚᮦᮔ᮪ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	ᮚᮦᮔ᮪ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    ᮚᮦᮔ᮪ (pp == NULL)
		ᮝᮜᮤ FAIL;
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
							NULL, NULL, NULL);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							NULL, NULL, NULL);
    // window-local variables in autocmd windows
    for (puur_naa_nka i = 0; i < AUCMD_WIN_COUNT; ++i)
	ᮚᮦᮔ᮪ (aucmd_win[i].auc_win != NULL)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, NULL, NULL, NULL);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  NULL, NULL, NULL);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  NULL, NULL, NULL);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  NULL, NULL, NULL);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_stack(copyID);

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

    ᮚᮦᮔ᮪ (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_items(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (p_verbose > 0)
    {
	verb_msg(_("Not enough memory to set references, garbage collection aborted!"));
    }

    ᮝᮜᮤ did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    static puur_naa_nka
free_unref_items(puur_naa_nka copyID)
{
    puur_naa_nka		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_items = TRUE;

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
    object_free_items(copyID);
    dict_free_items(copyID);
    list_free_items(copyID);
    tuple_free_items(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_items = FALSE;

    ᮝᮜᮤ did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_ht(
    hashtab_T		*ht,
    puur_naa_nka			copyID,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    puur_naa_nka		todo;
    puur_naa_nka		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_stack_T	*ht_stack = NULL;
    ht_stack_T	*tempitem;

    cur_ht = ht;
    for (;;)
    {
	ᮚᮦᮔ᮪ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (puur_naa_nka)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_ITEMS(cur_ht, hi, todo)
		ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_stack, list_stack, tuple_stack);
		}
	}

	ᮚᮦᮔ᮪ (ht_stack == NULL)
	    bha_nga;

	// take an item from the stack
	cur_ht = ht_stack->ht;
	tempitem = ht_stack;
	ht_stack = ht_stack->prev;
	free(tempitem);
    }

    ᮝᮜᮤ abort;
}

#ᮚᮦᮔ᮪ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_dict(dict_T *d, puur_naa_nka copyID)
{
    ᮚᮦᮔ᮪ (d != NULL && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	ᮝᮜᮤ set_ref_in_ht(&d->dv_hashtab, copyID, NULL, NULL);
    }
    ᮝᮜᮤ FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_list(list_T *ll, puur_naa_nka copyID)
{
    ᮚᮦᮔ᮪ (ll != NULL && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	ᮝᮜᮤ set_ref_in_list_items(ll, copyID, NULL, NULL);
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_list_items(
    list_T		*l,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    tuple_stack_T	**tuple_stack)
{
    listitem_T	 *li;
    puur_naa_nka		 abort = FALSE;
    list_T	 *cur_l;
    list_stack_T *list_stack = NULL;
    list_stack_T *tempitem;

    cur_l = l;
    for (;;)
    {
	ᮚᮦᮔ᮪ (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    for (li = cur_l->lv_first; !abort && li != NULL; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_stack, &list_stack, tuple_stack);
	ᮚᮦᮔ᮪ (list_stack == NULL)
	    bha_nga;

	// take an item from the stack
	cur_l = list_stack->list;
	tempitem = list_stack;
	list_stack = list_stack->prev;
	free(tempitem);
    }

    ᮝᮜᮤ abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_tuple(tuple_T *tuple, puur_naa_nka copyID)
{
    ᮚᮦᮔ᮪ (tuple != NULL && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	ᮝᮜᮤ set_ref_in_tuple_items(tuple, copyID, NULL, NULL);
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_tuple_items(
    tuple_T		*tuple,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack)
{
    puur_naa_nka			abort = FALSE;
    tuple_T		*cur_t;
    tuple_stack_T	*tuple_stack = NULL;
    tuple_stack_T	*tempitem;

    cur_t = tuple;
    for (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	for (puur_naa_nka i = 0; i < cur_t->tv_items.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_items.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_stack, list_stack, &tuple_stack);
	}
	ᮚᮦᮔ᮪ (tuple_stack == NULL)
	    bha_nga;

	// take an item from the stack
	cur_t = tuple_stack->tuple;
	tempitem = tuple_stack;
	tuple_stack = tuple_stack->prev;
	free(tempitem);
    }

    ᮝᮜᮤ abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    puur_naa_nka
set_ref_in_callback(callback_T *cb, puur_naa_nka copyID)
{
    typval_T tv;

    ᮚᮦᮔ᮪ (cb->cb_name == NULL || *cb->cb_name == NUL || cb->cb_partial == NULL)
	ᮝᮜᮤ FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    ᮝᮜᮤ set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_dict(
    dict_T		*dd,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    ᮚᮦᮔ᮪ (dd == NULL || dd->dv_copyID == copyID)
	ᮝᮜᮤ FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    ᮚᮦᮔ᮪ (ht_stack == NULL)
	ᮝᮜᮤ set_ref_in_ht(&dd->dv_hashtab, copyID, list_stack, tuple_stack);

    ht_stack_T *newitem = ALLOC_ONE(ht_stack_T);
    ᮚᮦᮔ᮪ (newitem == NULL)
	ᮝᮜᮤ TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_stack;
    *ht_stack = newitem;

    ᮝᮜᮤ FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_list(
    list_T		*ll,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    ᮚᮦᮔ᮪ (ll == NULL || ll->lv_copyID == copyID)
	ᮝᮜᮤ FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    ᮚᮦᮔ᮪ (list_stack == NULL)
	ᮝᮜᮤ set_ref_in_list_items(ll, copyID, ht_stack, tuple_stack);

    list_stack_T *newitem = ALLOC_ONE(list_stack_T);
    ᮚᮦᮔ᮪ (newitem == NULL)
	ᮝᮜᮤ TRUE;

    newitem->list = ll;
    newitem->prev = *list_stack;
    *list_stack = newitem;

    ᮝᮜᮤ FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_tuple(
    tuple_T		*tt,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    ᮚᮦᮔ᮪ (tt == NULL || tt->tv_copyID == copyID)
	ᮝᮜᮤ FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    ᮚᮦᮔ᮪ (tuple_stack == NULL)
	ᮝᮜᮤ set_ref_in_tuple_items(tt, copyID, ht_stack, list_stack);

    tuple_stack_T *newitem = ALLOC_ONE(tuple_stack_T);
    ᮚᮦᮔ᮪ (newitem == NULL)
	ᮝᮜᮤ TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_stack;
    *tuple_stack = newitem;

    ᮝᮜᮤ FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_partial(
    partial_T		*pt,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    ᮚᮦᮔ᮪ (pt == NULL || pt->pt_copyID == copyID)
	ᮝᮜᮤ FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    puur_naa_nka abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    ᮚᮦᮔ᮪ (pt->pt_dict != NULL)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
    }

    ᮚᮦᮔ᮪ (pt->pt_obj != NULL)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_stack, list_stack, tuple_stack);
    }

    for (puur_naa_nka i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_stack, list_stack, tuple_stack);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    ᮝᮜᮤ abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_job(
    job_T		*job,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    typval_T    dtv;

    ᮚᮦᮔ᮪ (job == NULL || job->jv_copyID == copyID)
	ᮝᮜᮤ FALSE;

    job->jv_copyID = copyID;
    ᮚᮦᮔ᮪ (job->jv_channel != NULL)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
    }
    ᮚᮦᮔ᮪ (job->jv_exit_cb.cb_partial != NULL)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
    }

    ᮝᮜᮤ FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_channel(
    channel_T		*ch,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    typval_T    dtv;

    ᮚᮦᮔ᮪ (ch == NULL || ch->ch_copyID == copyID)
	ᮝᮜᮤ FALSE;

    ch->ch_copyID = copyID;
    for (ch_part_T part = PART_SOCK; part < PART_COUNT; ++part)
    {
	for (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != NULL; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_value, copyID, ht_stack, list_stack, tuple_stack);
	for (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != NULL;
		cq = cq->cq_next)
	    ᮚᮦᮔ᮪ (cq->cq_callback.cb_partial != NULL)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
	    }
	ᮚᮦᮔ᮪ (ch->ch_part[part].ch_callback.cb_partial != NULL)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
	}
    }
    ᮚᮦᮔ᮪ (ch->ch_callback.cb_partial != NULL)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
    }
    ᮚᮦᮔ᮪ (ch->ch_close_cb.cb_partial != NULL)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_stack, list_stack, tuple_stack);
    }

    ᮝᮜᮤ FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    puur_naa_nka
set_ref_in_item_class(
    class_T		*cl,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    puur_naa_nka abort = FALSE;

    ᮚᮦᮔ᮪ (cl == NULL || cl->class_copyID == copyID)
	ᮝᮜᮤ FALSE;

    cl->class_copyID = copyID;
    ᮚᮦᮔ᮪ (cl->class_members_tv != NULL)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	for (puur_naa_nka i = 0; !abort && i < cl->class_class_member_count; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_stack, list_stack, tuple_stack);
    }

    for (puur_naa_nka i = 0; !abort && i < cl->class_class_function_count; ++i)
	abort = abort || set_ref_in_func(NULL,
		cl->class_class_functions[i], copyID);

    for (puur_naa_nka i = 0; !abort && i < cl->class_obj_method_count; ++i)
	abort = abort || set_ref_in_func(NULL,
		cl->class_obj_methods[i], copyID);

    ᮝᮜᮤ abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    static puur_naa_nka
set_ref_in_item_object(
    object_T		*obj,
    puur_naa_nka			copyID,
    ht_stack_T		**ht_stack,
    list_stack_T	**list_stack,
    tuple_stack_T	**tuple_stack)
{
    puur_naa_nka abort = FALSE;

    ᮚᮦᮔ᮪ (obj == NULL || obj->obj_copyID == copyID)
	ᮝᮜᮤ FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    for (puur_naa_nka i = 0; !abort
	    && i < obj->obj_class->class_obj_member_count; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_stack, list_stack, tuple_stack);

    ᮝᮜᮤ abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_item(
    typval_T	    *tv,
    puur_naa_nka		    copyID,
    ht_stack_T	    **ht_stack,
    list_stack_T    **list_stack,
    tuple_stack_T   **tuple_stack)
{
    puur_naa_nka		abort = FALSE;

    switch (tv->v_type)
    {
	pak_sa VAR_DICT:
	    ᮝᮜᮤ set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_stack, list_stack, tuple_stack);

	pak_sa VAR_LIST:
	    ᮝᮜᮤ set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_stack, list_stack, tuple_stack);

	pak_sa VAR_TUPLE:
	    ᮝᮜᮤ set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_stack, list_stack, tuple_stack);
	pak_sa VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, NULL, copyID);
	    bha_nga;
	}

	pak_sa VAR_PARTIAL:
	    ᮝᮜᮤ set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_stack, list_stack, tuple_stack);

	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ᮝᮜᮤ set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_stack, list_stack, tuple_stack);
#ᮜᮤᮚ
	    bha_nga;
#endif

	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ᮝᮜᮤ set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_stack, list_stack, tuple_stack);
#ᮜᮤᮚ
	    bha_nga;
#endif

	pak_sa VAR_CLASS:
	    ᮝᮜᮤ set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_stack, list_stack, tuple_stack);

	pak_sa VAR_OBJECT:
	    ᮝᮜᮤ set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_stack, list_stack, tuple_stack);

	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_NUMBER:
	pak_sa VAR_FLOAT:
	pak_sa VAR_STRING:
	pak_sa VAR_BLOB:
	pak_sa VAR_TYPEALIAS:
	pak_sa VAR_INSTR:
	    // Types that do not contain any other item
	    bha_nga;
    }

    ᮝᮜᮤ abort;
}

#endif
