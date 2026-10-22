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

ساكن صحيح free_unref_توکي(صحيح copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    صحيح
get_copyID(فراغ)
{
    current_copyID += COPYID_INC;
    أرجع current_copyID;
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
	    pp = vim_realloc(exestack.ga_معلومات, new_len);
	    إذا (pp == عدم)
		أرجع FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_معلومات = pp;
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
    abort = abort || سېټ_ref_دننه_پخوانی_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || سېټ_ref_دننه_توکی(&buf->b_bufvar.di_tv, copyID,
							عدم, عدم, عدم);
	abort = abort || سېټ_ref_دننه_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || سېټ_ref_دننه_توکی(&wp->w_winvar.di_tv, copyID,
							عدم, عدم, عدم);
    // window-local variables in autocmd windows
    لكل (صحيح i = 0; i < AUCMD_WIN_شمېر; ++i)
	إذا (aucmd_win[i].auc_win != عدم)
	    abort = abort || سېټ_ref_دننه_توکی(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, عدم, عدم, عدم);
#که_تعریف_شوی FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || سېټ_ref_دننه_توکی(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_دننه_TAB(tp, wp)
		abort = abort || سېټ_ref_دننه_توکی(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
#د_که_پای

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || سېټ_ref_دننه_توکی(&tp->tp_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || سېټ_ref_دننه_call_ډېری(copyID);

    // named functions (matters for closures)
    abort = abort || سېټ_ref_دننه_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || سېټ_ref_دننه_func_دلیلونه(copyID);

    // funcstacks keep variables for closures
    abort = abort || سېټ_ref_دننه_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || سېټ_ref_دننه_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || سېټ_ref_دننه_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || سېټ_ref_دننه_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || سېټ_ref_دننه_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || سېټ_ref_دننه_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || سېټ_ref_دننه_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || سېټ_ref_دننه_findfunc(copyID);

#که_تعریف_شوی FEAT_LUA
    abort = abort || سېټ_ref_دننه_lua(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_PYTHON
    abort = abort || سېټ_ref_دننه_python(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_PYTHON3
    abort = abort || سېټ_ref_دننه_python3(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_دنده_CHANNEL
    abort = abort || سېټ_ref_دننه_channel(copyID);
    abort = abort || سېټ_ref_دننه_دنده(copyID);
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
    abort = abort || سېټ_ref_دننه_nb_channel(copyID);
#د_که_پای
#که_تعریف_شوی FEAT_SOCKETSERVER
    abort = abort || سېټ_ref_دننه_socketserver_channel(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_TIMERS
    abort = abort || سېټ_ref_دننه_timer(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_QUICKFIX
    abort = abort || سېټ_ref_دننه_quickfix(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_TERMINAL
    abort = abort || سېټ_ref_دننه_غړی(copyID);
#د_که_پای

#که_تعریف_شوی FEAT_PROP_POPUP
    abort = abort || سېټ_ref_دننه_popups(copyID);
#د_که_پای

    abort = abort || سېټ_ref_دننه_classes(copyID);

    إذا (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_توکي(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    وإلا إذا (p_verbose > 0)
    {
	verb_پیغام(_("Not enough memory to set references, garbage collection aborted!"));
    }

    أرجع did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ساكن صحيح
free_unref_توکي(صحيح copyID)
{
    صحيح		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    دننه_free_unref_توکي = TRUE;

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

#که_تعریف_شوی FEAT_دنده_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_ناکارول_شوی_jobs_contents(copyID, COPYID_ماسک);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_ناکارول_شوی_channels_contents(copyID, COPYID_ماسک);
#د_که_پای

    /*
     * PASS 2: free the items themselves.
     */
    object_free_توکي(copyID);
    dict_free_توکي(copyID);
    list_free_توکي(copyID);
    tuple_free_توکي(copyID);

#که_تعریف_شوی FEAT_دنده_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_ناکارول_شوی_jobs(copyID, COPYID_ماسک);

    // Go through the list of channels and free items without the copyID.
    free_ناکارول_شوی_channels(copyID, COPYID_ماسک);
#د_که_پای

    دننه_free_unref_توکي = FALSE;

    أرجع did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_ht(
    hashtab_T		*ht,
    صحيح			copyID,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    صحيح		todo;
    صحيح		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ډېری_T	*ht_ډېری = عدم;
    ht_ډېری_T	*tempitem;

    cur_ht = ht;
    لكل (;;)
    {
	إذا (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (صحيح)cur_ht->ht_کارول_شوی;
	    FOR_ALL_HASHTAB_توکي(cur_ht, hi, todo)
		إذا (!HASHITEM_تشه(hi))
		{
		    --todo;
		    abort = abort
			|| سېټ_ref_دننه_توکی(&HI2DI(hi)->di_tv, copyID,
				       &ht_ډېری, list_ډېری, tuple_ډېری);
		}
	}

	إذا (ht_ډېری == عدم)
	    اكسر;

	// take an item from the stack
	cur_ht = ht_ډېری->ht;
	tempitem = ht_ډېری;
	ht_ډېری = ht_ډېری->prev;
	حرر(tempitem);
    }

    أرجع abort;
}

#إذا defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_dict(dict_T *d, صحيح copyID)
{
    إذا (d != عدم && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	أرجع سېټ_ref_دننه_ht(&d->dv_hashtab, copyID, عدم, عدم);
    }
    أرجع FALSE;
}
#د_که_پای

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_list(list_T *ll, صحيح copyID)
{
    إذا (ll != عدم && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	أرجع سېټ_ref_دننه_list_توکي(ll, copyID, عدم, عدم);
    }
    أرجع FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_list_توکي(
    list_T		*l,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    listitem_T	 *li;
    صحيح		 abort = FALSE;
    list_T	 *cur_l;
    list_ډېری_T *list_ډېری = عدم;
    list_ډېری_T *tempitem;

    cur_l = l;
    لكل (;;)
    {
	إذا (!abort && cur_l->lv_لومړی != &range_list_توکی)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    لكل (li = cur_l->lv_لومړی; !abort && li != عدم; li = li->li_بل)
		abort = abort || سېټ_ref_دننه_توکی(&li->li_tv, copyID,
				       ht_ډېری, &list_ډېری, tuple_ډېری);
	إذا (list_ډېری == عدم)
	    اكسر;

	// take an item from the stack
	cur_l = list_ډېری->list;
	tempitem = list_ډېری;
	list_ډېری = list_ډېری->prev;
	حرر(tempitem);
    }

    أرجع abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_tuple(tuple_T *tuple, صحيح copyID)
{
    إذا (tuple != عدم && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	أرجع سېټ_ref_دننه_tuple_توکي(tuple, copyID, عدم, عدم);
    }
    أرجع FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سېټ_ref_دننه_tuple_توکي(
    tuple_T		*tuple,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری)
{
    صحيح			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ډېری_T	*tuple_ډېری = عدم;
    tuple_ډېری_T	*tempitem;

    cur_t = tuple;
    لكل (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	لكل (صحيح i = 0; i < cur_t->tv_توکي.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_توکي.ga_معلومات) + i;
	    abort = abort
		|| سېټ_ref_دننه_توکی(tv, copyID,
			ht_ډېری, list_ډېری, &tuple_ډېری);
	}
	إذا (tuple_ډېری == عدم)
	    اكسر;

	// take an item from the stack
	cur_t = tuple_ډېری->tuple;
	tempitem = tuple_ډېری;
	tuple_ډېری = tuple_ډېری->prev;
	حرر(tempitem);
    }

    أرجع abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    صحيح
سېټ_ref_دننه_callback(callback_T *cb, صحيح copyID)
{
    typval_T tv;

    إذا (cb->cb_name == عدم || *cb->cb_name == NUL || cb->cb_partial == عدم)
	أرجع FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    أرجع سېټ_ref_دننه_توکی(&tv, copyID, عدم, عدم, عدم);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_dict(
    dict_T		*dd,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    إذا (dd == عدم || dd->dv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    إذا (ht_ډېری == عدم)
	أرجع سېټ_ref_دننه_ht(&dd->dv_hashtab, copyID, list_ډېری, tuple_ډېری);

    ht_ډېری_T *newitem = ALLOC_ONE(ht_ډېری_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ډېری;
    *ht_ډېری = newitem;

    أرجع FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_list(
    list_T		*ll,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    إذا (ll == عدم || ll->lv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    إذا (list_ډېری == عدم)
	أرجع سېټ_ref_دننه_list_توکي(ll, copyID, ht_ډېری, tuple_ډېری);

    list_ډېری_T *newitem = ALLOC_ONE(list_ډېری_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->list = ll;
    newitem->prev = *list_ډېری;
    *list_ډېری = newitem;

    أرجع FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_tuple(
    tuple_T		*tt,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    إذا (tt == عدم || tt->tv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    إذا (tuple_ډېری == عدم)
	أرجع سېټ_ref_دننه_tuple_توکي(tt, copyID, ht_ډېری, list_ډېری);

    tuple_ډېری_T *newitem = ALLOC_ONE(tuple_ډېری_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ډېری;
    *tuple_ډېری = newitem;

    أرجع FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_partial(
    partial_T		*pt,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    إذا (pt == عدم || pt->pt_copyID == copyID)
	أرجع FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    صحيح abort = سېټ_ref_دننه_func(pt->pt_name, pt->pt_func, copyID);

    إذا (pt->pt_dict != عدم)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }

    إذا (pt->pt_obj != عدم)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	سېټ_ref_دننه_توکی(&objtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }

    لكل (صحيح i = 0; i < pt->pt_argc; ++i)
	abort = abort || سېټ_ref_دننه_توکی(&pt->pt_argv[i], copyID,
		ht_ډېری, list_ډېری, tuple_ډېری);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    أرجع abort;
}

#که_تعریف_شوی FEAT_دنده_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_دنده(
    دنده_T		*دنده,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    typval_T    dtv;

    إذا (دنده == عدم || دنده->jv_copyID == copyID)
	أرجع FALSE;

    دنده->jv_copyID = copyID;
    إذا (دنده->jv_channel != عدم)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = دنده->jv_channel;
	سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }
    إذا (دنده->jv_exit_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = دنده->jv_exit_cb.cb_partial;
	سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }

    أرجع FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_channel(
    channel_T		*ch,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    typval_T    dtv;

    إذا (ch == عدم || ch->ch_copyID == copyID)
	أرجع FALSE;

    ch->ch_copyID = copyID;
    لكل (ch_part_T part = PART_SOCK; part < PART_شمېر; ++part)
    {
	لكل (jsonq_T *jq = ch->ch_part[part].ch_json_سر.jq_بل;
		jq != عدم; jq = jq->jq_بل)
	    سېټ_ref_دننه_توکی(jq->jq_ارزښت, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
	لكل (cbq_T *cq = ch->ch_part[part].ch_cb_سر.cq_بل; cq != عدم;
		cq = cq->cq_بل)
	    إذا (cq->cq_callback.cb_partial != عدم)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
	    }
	إذا (ch->ch_part[part].ch_callback.cb_partial != عدم)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
	}
    }
    إذا (ch->ch_callback.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }
    إذا (ch->ch_close_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	سېټ_ref_دننه_توکی(&dtv, copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }

    أرجع FALSE;
}
#د_که_پای

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    صحيح
سېټ_ref_دننه_توکی_class(
    class_T		*cl,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    صحيح abort = FALSE;

    إذا (cl == عدم || cl->class_copyID == copyID)
	أرجع FALSE;

    cl->class_copyID = copyID;
    إذا (cl->class_members_tv != عدم)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	لكل (صحيح i = 0; !abort && i < cl->class_class_member_شمېر; ++i)
	    abort = abort || سېټ_ref_دننه_توکی(
		    &cl->class_members_tv[i],
		    copyID, ht_ډېری, list_ډېری, tuple_ډېری);
    }

    لكل (صحيح i = 0; !abort && i < cl->class_class_function_شمېر; ++i)
	abort = abort || سېټ_ref_دننه_func(عدم,
		cl->class_class_functions[i], copyID);

    لكل (صحيح i = 0; !abort && i < cl->class_obj_لاره_شمېر; ++i)
	abort = abort || سېټ_ref_دننه_func(عدم,
		cl->class_obj_methods[i], copyID);

    أرجع abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سېټ_ref_دننه_توکی_object(
    object_T		*obj,
    صحيح			copyID,
    ht_ډېری_T		**ht_ډېری,
    list_ډېری_T	**list_ډېری,
    tuple_ډېری_T	**tuple_ډېری)
{
    صحيح abort = FALSE;

    إذا (obj == عدم || obj->obj_copyID == copyID)
	أرجع FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    لكل (صحيح i = 0; !abort
	    && i < obj->obj_class->class_obj_member_شمېر; ++i)
	abort = abort || سېټ_ref_دننه_توکی(mtv + i, copyID,
		ht_ډېری, list_ډېری, tuple_ډېری);

    أرجع abort;
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
سېټ_ref_دننه_توکی(
    typval_T	    *tv,
    صحيح		    copyID,
    ht_ډېری_T	    **ht_ډېری,
    list_ډېری_T    **list_ډېری,
    tuple_ډېری_T   **tuple_ډېری)
{
    صحيح		abort = FALSE;

    بدل (tv->v_type)
    {
	حالة VAR_DICT:
	    أرجع سېټ_ref_دننه_توکی_dict(tv->vval.v_dict, copyID,
					 ht_ډېری, list_ډېری, tuple_ډېری);

	حالة VAR_LIST:
	    أرجع سېټ_ref_دننه_توکی_list(tv->vval.v_list, copyID,
					 ht_ډېری, list_ډېری, tuple_ډېری);

	حالة VAR_TUPLE:
	    أرجع سېټ_ref_دننه_توکی_tuple(tv->vval.v_tuple, copyID,
					 ht_ډېری, list_ډېری, tuple_ډېری);
	حالة VAR_FUNC:
	{
	    abort = سېټ_ref_دننه_func(tv->vval.v_string, عدم, copyID);
	    اكسر;
	}

	حالة VAR_PARTIAL:
	    أرجع سېټ_ref_دننه_توکی_partial(tv->vval.v_partial, copyID,
					ht_ډېری, list_ډېری, tuple_ډېری);

	حالة VAR_دنده:
#که_تعریف_شوی FEAT_دنده_CHANNEL
	    أرجع سېټ_ref_دننه_توکی_دنده(tv->vval.v_دنده, copyID,
					ht_ډېری, list_ډېری, tuple_ډېری);
#وإلا
	    اكسر;
#د_که_پای

	حالة VAR_CHANNEL:
#که_تعریف_شوی FEAT_دنده_CHANNEL
	    أرجع سېټ_ref_دننه_توکی_channel(tv->vval.v_channel, copyID,
					ht_ډېری, list_ډېری, tuple_ډېری);
#وإلا
	    اكسر;
#د_که_پای

	حالة VAR_CLASS:
	    أرجع سېټ_ref_دننه_توکی_class(tv->vval.v_class, copyID,
					ht_ډېری, list_ډېری, tuple_ډېری);

	حالة VAR_OBJECT:
	    أرجع سېټ_ref_دننه_توکی_object(tv->vval.v_object, copyID,
					ht_ډېری, list_ډېری, tuple_ډېری);

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_شمېره:
	حالة VAR_FLOAT:
	حالة VAR_STRING:
	حالة VAR_BLOB:
	حالة VAR_TYPEALIAS:
	حالة VAR_INSTR:
	    // Types that do not contain any other item
	    اكسر;
    }

    أرجع abort;
}

#د_که_پای
