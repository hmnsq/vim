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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
স্থির পূর্ণাঙ্ক current_copyID = 0;

স্থির পূর্ণাঙ্ক free_unref_বস্তুসমূহ(পূর্ণাঙ্ক copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    পূর্ণাঙ্ক
get_copyID(শূন্য)
{
    current_copyID += COPYID_INC;
    ঘূৰাই current_copyID;
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
    পূর্ণাঙ্ক
garbage_collect(পূর্ণাঙ্ক testing)
{
    পূর্ণাঙ্ক		copyID;
    পূর্ণাঙ্ক		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    পূর্ণাঙ্ক		did_free = FALSE;
    tabpage_T	*tp;

    যদি (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    যদি (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	পূর্ণাঙ্ক	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	যদি (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	যদি (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    যদি (pp == রিক্ত)
		ঘূৰাই FAIL;
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
							রিক্ত, রিক্ত, রিক্ত);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							রিক্ত, রিক্ত, রিক্ত);
    // window-local variables in autocmd windows
    বাবে (পূর্ণাঙ্ক i = 0; i < AUCMD_WIN_গুনতি; ++i)
	যদি (aucmd_win[i].auc_win != রিক্ত)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, রিক্ত, রিক্ত, রিক্ত);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  রিক্ত, রিক্ত, রিক্ত);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  রিক্ত, রিক্ত, রিক্ত);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  রিক্ত, রিক্ত, রিক্ত);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_স্তূপ(copyID);

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

    যদি (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_বস্তুসমূহ(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    অন্যথা যদি (p_verbose > 0)
    {
	verb_বাৰ্তা(_("Not enough memory to set references, garbage collection aborted!"));
    }

    ঘূৰাই did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    স্থির পূর্ণাঙ্ক
free_unref_বস্তুসমূহ(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_বস্তুসমূহ = TRUE;

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
    object_free_বস্তুসমূহ(copyID);
    dict_free_বস্তুসমূহ(copyID);
    list_free_বস্তুসমূহ(copyID);
    tuple_free_বস্তুসমূহ(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_বস্তুসমূহ = FALSE;

    ঘূৰাই did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_ht(
    hashtab_T		*ht,
    পূর্ণাঙ্ক			copyID,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_স্তূপ_T	*ht_স্তূপ = রিক্ত;
    ht_স্তূপ_T	*tempitem;

    cur_ht = ht;
    বাবে (;;)
    {
	যদি (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (পূর্ণাঙ্ক)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(cur_ht, hi, todo)
		যদি (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
		}
	}

	যদি (ht_স্তূপ == রিক্ত)
	    ভঙ্গ;

	// take an item from the stack
	cur_ht = ht_স্তূপ->ht;
	tempitem = ht_স্তূপ;
	ht_স্তূপ = ht_স্তূপ->prev;
	মুক্ত(tempitem);
    }

    ঘূৰাই abort;
}

#যদি defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_dict(dict_T *d, পূর্ণাঙ্ক copyID)
{
    যদি (d != রিক্ত && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	ঘূৰাই set_ref_in_ht(&d->dv_hashtab, copyID, রিক্ত, রিক্ত);
    }
    ঘূৰাই FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_list(list_T *ll, পূর্ণাঙ্ক copyID)
{
    যদি (ll != রিক্ত && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	ঘূৰাই set_ref_in_list_বস্তুসমূহ(ll, copyID, রিক্ত, রিক্ত);
    }
    ঘূৰাই FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_list_বস্তুসমূহ(
    list_T		*l,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    listitem_T	 *li;
    পূর্ণাঙ্ক		 abort = FALSE;
    list_T	 *cur_l;
    list_স্তূপ_T *list_স্তূপ = রিক্ত;
    list_স্তূপ_T *tempitem;

    cur_l = l;
    বাবে (;;)
    {
	যদি (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    বাবে (li = cur_l->lv_first; !abort && li != রিক্ত; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_স্তূপ, &list_স্তূপ, tuple_স্তূপ);
	যদি (list_স্তূপ == রিক্ত)
	    ভঙ্গ;

	// take an item from the stack
	cur_l = list_স্তূপ->list;
	tempitem = list_স্তূপ;
	list_স্তূপ = list_স্তূপ->prev;
	মুক্ত(tempitem);
    }

    ঘূৰাই abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_tuple(tuple_T *tuple, পূর্ণাঙ্ক copyID)
{
    যদি (tuple != রিক্ত && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	ঘূৰাই set_ref_in_tuple_বস্তুসমূহ(tuple, copyID, রিক্ত, রিক্ত);
    }
    ঘূৰাই FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_tuple_বস্তুসমূহ(
    tuple_T		*tuple,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ)
{
    পূর্ণাঙ্ক			abort = FALSE;
    tuple_T		*cur_t;
    tuple_স্তূপ_T	*tuple_স্তূপ = রিক্ত;
    tuple_স্তূপ_T	*tempitem;

    cur_t = tuple;
    বাবে (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	বাবে (পূর্ণাঙ্ক i = 0; i < cur_t->tv_বস্তুসমূহ.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_বস্তুসমূহ.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_স্তূপ, list_স্তূপ, &tuple_স্তূপ);
	}
	যদি (tuple_স্তূপ == রিক্ত)
	    ভঙ্গ;

	// take an item from the stack
	cur_t = tuple_স্তূপ->tuple;
	tempitem = tuple_স্তূপ;
	tuple_স্তূপ = tuple_স্তূপ->prev;
	মুক্ত(tempitem);
    }

    ঘূৰাই abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    পূর্ণাঙ্ক
set_ref_in_callback(callback_T *cb, পূর্ণাঙ্ক copyID)
{
    typval_T tv;

    যদি (cb->cb_name == রিক্ত || *cb->cb_name == NUL || cb->cb_partial == রিক্ত)
	ঘূৰাই FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    ঘূৰাই set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_dict(
    dict_T		*dd,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    যদি (dd == রিক্ত || dd->dv_copyID == copyID)
	ঘূৰাই FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    যদি (ht_স্তূপ == রিক্ত)
	ঘূৰাই set_ref_in_ht(&dd->dv_hashtab, copyID, list_স্তূপ, tuple_স্তূপ);

    ht_স্তূপ_T *newitem = ALLOC_ONE(ht_স্তূপ_T);
    যদি (newitem == রিক্ত)
	ঘূৰাই TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_স্তূপ;
    *ht_স্তূপ = newitem;

    ঘূৰাই FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_list(
    list_T		*ll,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    যদি (ll == রিক্ত || ll->lv_copyID == copyID)
	ঘূৰাই FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    যদি (list_স্তূপ == রিক্ত)
	ঘূৰাই set_ref_in_list_বস্তুসমূহ(ll, copyID, ht_স্তূপ, tuple_স্তূপ);

    list_স্তূপ_T *newitem = ALLOC_ONE(list_স্তূপ_T);
    যদি (newitem == রিক্ত)
	ঘূৰাই TRUE;

    newitem->list = ll;
    newitem->prev = *list_স্তূপ;
    *list_স্তূপ = newitem;

    ঘূৰাই FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_tuple(
    tuple_T		*tt,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    যদি (tt == রিক্ত || tt->tv_copyID == copyID)
	ঘূৰাই FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    যদি (tuple_স্তূপ == রিক্ত)
	ঘূৰাই set_ref_in_tuple_বস্তুসমূহ(tt, copyID, ht_স্তূপ, list_স্তূপ);

    tuple_স্তূপ_T *newitem = ALLOC_ONE(tuple_স্তূপ_T);
    যদি (newitem == রিক্ত)
	ঘূৰাই TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_স্তূপ;
    *tuple_স্তূপ = newitem;

    ঘূৰাই FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_partial(
    partial_T		*pt,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    যদি (pt == রিক্ত || pt->pt_copyID == copyID)
	ঘূৰাই FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    পূর্ণাঙ্ক abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    যদি (pt->pt_dict != রিক্ত)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }

    যদি (pt->pt_obj != রিক্ত)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }

    বাবে (পূর্ণাঙ্ক i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    ঘূৰাই abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_job(
    job_T		*job,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    typval_T    dtv;

    যদি (job == রিক্ত || job->jv_copyID == copyID)
	ঘূৰাই FALSE;

    job->jv_copyID = copyID;
    যদি (job->jv_channel != রিক্ত)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }
    যদি (job->jv_exit_cb.cb_partial != রিক্ত)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }

    ঘূৰাই FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_channel(
    channel_T		*ch,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    typval_T    dtv;

    যদি (ch == রিক্ত || ch->ch_copyID == copyID)
	ঘূৰাই FALSE;

    ch->ch_copyID = copyID;
    বাবে (ch_part_T part = PART_SOCK; part < PART_গুনতি; ++part)
    {
	বাবে (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != রিক্ত; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_মান, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
	বাবে (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != রিক্ত;
		cq = cq->cq_next)
	    যদি (cq->cq_callback.cb_partial != রিক্ত)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
	    }
	যদি (ch->ch_part[part].ch_callback.cb_partial != রিক্ত)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
	}
    }
    যদি (ch->ch_callback.cb_partial != রিক্ত)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }
    যদি (ch->ch_close_cb.cb_partial != রিক্ত)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }

    ঘূৰাই FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    পূর্ণাঙ্ক
set_ref_in_item_class(
    class_T		*cl,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    পূর্ণাঙ্ক abort = FALSE;

    যদি (cl == রিক্ত || cl->class_copyID == copyID)
	ঘূৰাই FALSE;

    cl->class_copyID = copyID;
    যদি (cl->class_members_tv != রিক্ত)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	বাবে (পূর্ণাঙ্ক i = 0; !abort && i < cl->class_class_member_গুনতি; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
    }

    বাবে (পূর্ণাঙ্ক i = 0; !abort && i < cl->class_class_function_গুনতি; ++i)
	abort = abort || set_ref_in_func(রিক্ত,
		cl->class_class_functions[i], copyID);

    বাবে (পূর্ণাঙ্ক i = 0; !abort && i < cl->class_obj_method_গুনতি; ++i)
	abort = abort || set_ref_in_func(রিক্ত,
		cl->class_obj_methods[i], copyID);

    ঘূৰাই abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_item_object(
    object_T		*obj,
    পূর্ণাঙ্ক			copyID,
    ht_স্তূপ_T		**ht_স্তূপ,
    list_স্তূপ_T	**list_স্তূপ,
    tuple_স্তূপ_T	**tuple_স্তূপ)
{
    পূর্ণাঙ্ক abort = FALSE;

    যদি (obj == রিক্ত || obj->obj_copyID == copyID)
	ঘূৰাই FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    বাবে (পূর্ণাঙ্ক i = 0; !abort
	    && i < obj->obj_class->class_obj_member_গুনতি; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

    ঘূৰাই abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_item(
    typval_T	    *tv,
    পূর্ণাঙ্ক		    copyID,
    ht_স্তূপ_T	    **ht_স্তূপ,
    list_স্তূপ_T    **list_স্তূপ,
    tuple_স্তূপ_T   **tuple_স্তূপ)
{
    পূর্ণাঙ্ক		abort = FALSE;

    ৱিকল্প (tv->v_type)
    {
	পক্ষ VAR_DICT:
	    ঘূৰাই set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

	পক্ষ VAR_LIST:
	    ঘূৰাই set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

	পক্ষ VAR_TUPLE:
	    ঘূৰাই set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
	পক্ষ VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, রিক্ত, copyID);
	    ভঙ্গ;
	}

	পক্ষ VAR_PARTIAL:
	    ঘূৰাই set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

	পক্ষ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ঘূৰাই set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
#অন্যথা
	    ভঙ্গ;
#endif

	পক্ষ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ঘূৰাই set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);
#অন্যথা
	    ভঙ্গ;
#endif

	পক্ষ VAR_CLASS:
	    ঘূৰাই set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

	পক্ষ VAR_OBJECT:
	    ঘূৰাই set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_স্তূপ, list_স্তূপ, tuple_স্তূপ);

	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_STRING:
	পক্ষ VAR_BLOB:
	পক্ষ VAR_TYPEALIAS:
	পক্ষ VAR_INSTR:
	    // Types that do not contain any other item
	    ভঙ্গ;
    }

    ঘূৰাই abort;
}

#endif
