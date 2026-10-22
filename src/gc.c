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

ساكن صحيح free_unref_شيون(صحيح copyID);

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
    abort = abort || سيٽ_ref_اندريون_اڳيون_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || سيٽ_ref_اندريون_شي(&buf->b_bufvar.di_tv, copyID,
							عدم, عدم, عدم);
	abort = abort || سيٽ_ref_اندريون_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || سيٽ_ref_اندريون_شي(&wp->w_winvar.di_tv, copyID,
							عدم, عدم, عدم);
    // window-local variables in autocmd windows
    لكل (صحيح i = 0; i < AUCMD_WIN_ڳڻپ; ++i)
	إذا (aucmd_win[i].auc_win != عدم)
	    abort = abort || سيٽ_ref_اندريون_شي(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, عدم, عدم, عدم);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || سيٽ_ref_اندريون_شي(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندريون_TAB(tp, wp)
		abort = abort || سيٽ_ref_اندريون_شي(&wp->w_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
#جيڪڏهن_پورو

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || سيٽ_ref_اندريون_شي(&tp->tp_winvar.di_tv, copyID,
								  عدم, عدم, عدم);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || سيٽ_ref_اندريون_call_ڍير(copyID);

    // named functions (matters for closures)
    abort = abort || سيٽ_ref_اندريون_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || سيٽ_ref_اندريون_func_دليلون(copyID);

    // funcstacks keep variables for closures
    abort = abort || سيٽ_ref_اندريون_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || سيٽ_ref_اندريون_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || سيٽ_ref_اندريون_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || سيٽ_ref_اندريون_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || سيٽ_ref_اندريون_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || سيٽ_ref_اندريون_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || سيٽ_ref_اندريون_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || سيٽ_ref_اندريون_findfunc(copyID);

#جيڪڏهن_وصف_ٿيل FEAT_LUA
    abort = abort || سيٽ_ref_اندريون_lua(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON
    abort = abort || سيٽ_ref_اندريون_python(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON3
    abort = abort || سيٽ_ref_اندريون_python3(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    abort = abort || سيٽ_ref_اندريون_channel(copyID);
    abort = abort || سيٽ_ref_اندريون_ڪم(copyID);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
    abort = abort || سيٽ_ref_اندريون_nb_channel(copyID);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
    abort = abort || سيٽ_ref_اندريون_socketserver_channel(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    abort = abort || سيٽ_ref_اندريون_timer(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    abort = abort || سيٽ_ref_اندريون_quickfix(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    abort = abort || سيٽ_ref_اندريون_رڪن(copyID);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    abort = abort || سيٽ_ref_اندريون_popups(copyID);
#جيڪڏهن_پورو

    abort = abort || سيٽ_ref_اندريون_classes(copyID);

    إذا (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_شيون(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    وإلا إذا (p_verbose > 0)
    {
	verb_نياپو(_("Not enough memory to set references, garbage collection aborted!"));
    }

    أرجع did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ساكن صحيح
free_unref_شيون(صحيح copyID)
{
    صحيح		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    اندريون_free_unref_شيون = TRUE;

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

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_اڻ_استعمال_ٿيل_jobs_contents(copyID, COPYID_نقاب);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_اڻ_استعمال_ٿيل_channels_contents(copyID, COPYID_نقاب);
#جيڪڏهن_پورو

    /*
     * PASS 2: free the items themselves.
     */
    object_free_شيون(copyID);
    dict_free_شيون(copyID);
    list_free_شيون(copyID);
    tuple_free_شيون(copyID);

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_اڻ_استعمال_ٿيل_jobs(copyID, COPYID_نقاب);

    // Go through the list of channels and free items without the copyID.
    free_اڻ_استعمال_ٿيل_channels(copyID, COPYID_نقاب);
#جيڪڏهن_پورو

    اندريون_free_unref_شيون = FALSE;

    أرجع did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سيٽ_ref_اندريون_ht(
    hashtab_T		*ht,
    صحيح			copyID,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    صحيح		todo;
    صحيح		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ڍير_T	*ht_ڍير = عدم;
    ht_ڍير_T	*tempitem;

    cur_ht = ht;
    لكل (;;)
    {
	إذا (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (صحيح)cur_ht->ht_استعمال_ٿيل;
	    FOR_ALL_HASHTAB_شيون(cur_ht, hi, todo)
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    --todo;
		    abort = abort
			|| سيٽ_ref_اندريون_شي(&HI2DI(hi)->di_tv, copyID,
				       &ht_ڍير, list_ڍير, tuple_ڍير);
		}
	}

	إذا (ht_ڍير == عدم)
	    اكسر;

	// take an item from the stack
	cur_ht = ht_ڍير->ht;
	tempitem = ht_ڍير;
	ht_ڍير = ht_ڍير->prev;
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
سيٽ_ref_اندريون_dict(dict_T *d, صحيح copyID)
{
    إذا (d != عدم && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	أرجع سيٽ_ref_اندريون_ht(&d->dv_hashtab, copyID, عدم, عدم);
    }
    أرجع FALSE;
}
#جيڪڏهن_پورو

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سيٽ_ref_اندريون_list(list_T *ll, صحيح copyID)
{
    إذا (ll != عدم && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	أرجع سيٽ_ref_اندريون_list_شيون(ll, copyID, عدم, عدم);
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
سيٽ_ref_اندريون_list_شيون(
    list_T		*l,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    listitem_T	 *li;
    صحيح		 abort = FALSE;
    list_T	 *cur_l;
    list_ڍير_T *list_ڍير = عدم;
    list_ڍير_T *tempitem;

    cur_l = l;
    لكل (;;)
    {
	إذا (!abort && cur_l->lv_پهريون != &range_list_شي)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    لكل (li = cur_l->lv_پهريون; !abort && li != عدم; li = li->li_ايندڙ)
		abort = abort || سيٽ_ref_اندريون_شي(&li->li_tv, copyID,
				       ht_ڍير, &list_ڍير, tuple_ڍير);
	إذا (list_ڍير == عدم)
	    اكسر;

	// take an item from the stack
	cur_l = list_ڍير->list;
	tempitem = list_ڍير;
	list_ڍير = list_ڍير->prev;
	حرر(tempitem);
    }

    أرجع abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
سيٽ_ref_اندريون_tuple(tuple_T *tuple, صحيح copyID)
{
    إذا (tuple != عدم && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	أرجع سيٽ_ref_اندريون_tuple_شيون(tuple, copyID, عدم, عدم);
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
سيٽ_ref_اندريون_tuple_شيون(
    tuple_T		*tuple,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير)
{
    صحيح			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ڍير_T	*tuple_ڍير = عدم;
    tuple_ڍير_T	*tempitem;

    cur_t = tuple;
    لكل (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	لكل (صحيح i = 0; i < cur_t->tv_شيون.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_شيون.ga_معلومات) + i;
	    abort = abort
		|| سيٽ_ref_اندريون_شي(tv, copyID,
			ht_ڍير, list_ڍير, &tuple_ڍير);
	}
	إذا (tuple_ڍير == عدم)
	    اكسر;

	// take an item from the stack
	cur_t = tuple_ڍير->tuple;
	tempitem = tuple_ڍير;
	tuple_ڍير = tuple_ڍير->prev;
	حرر(tempitem);
    }

    أرجع abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    صحيح
سيٽ_ref_اندريون_callback(callback_T *cb, صحيح copyID)
{
    typval_T tv;

    إذا (cb->cb_name == عدم || *cb->cb_name == NUL || cb->cb_partial == عدم)
	أرجع FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    أرجع سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_dict(
    dict_T		*dd,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    إذا (dd == عدم || dd->dv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    إذا (ht_ڍير == عدم)
	أرجع سيٽ_ref_اندريون_ht(&dd->dv_hashtab, copyID, list_ڍير, tuple_ڍير);

    ht_ڍير_T *newitem = ALLOC_ONE(ht_ڍير_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ڍير;
    *ht_ڍير = newitem;

    أرجع FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_list(
    list_T		*ll,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    إذا (ll == عدم || ll->lv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    إذا (list_ڍير == عدم)
	أرجع سيٽ_ref_اندريون_list_شيون(ll, copyID, ht_ڍير, tuple_ڍير);

    list_ڍير_T *newitem = ALLOC_ONE(list_ڍير_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->list = ll;
    newitem->prev = *list_ڍير;
    *list_ڍير = newitem;

    أرجع FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_tuple(
    tuple_T		*tt,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    إذا (tt == عدم || tt->tv_copyID == copyID)
	أرجع FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    إذا (tuple_ڍير == عدم)
	أرجع سيٽ_ref_اندريون_tuple_شيون(tt, copyID, ht_ڍير, list_ڍير);

    tuple_ڍير_T *newitem = ALLOC_ONE(tuple_ڍير_T);
    إذا (newitem == عدم)
	أرجع TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ڍير;
    *tuple_ڍير = newitem;

    أرجع FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_partial(
    partial_T		*pt,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    إذا (pt == عدم || pt->pt_copyID == copyID)
	أرجع FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    صحيح abort = سيٽ_ref_اندريون_func(pt->pt_name, pt->pt_func, copyID);

    إذا (pt->pt_dict != عدم)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }

    إذا (pt->pt_obj != عدم)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	سيٽ_ref_اندريون_شي(&objtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }

    لكل (صحيح i = 0; i < pt->pt_argc; ++i)
	abort = abort || سيٽ_ref_اندريون_شي(&pt->pt_argv[i], copyID,
		ht_ڍير, list_ڍير, tuple_ڍير);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    أرجع abort;
}

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_ڪم(
    ڪم_T		*ڪم,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    typval_T    dtv;

    إذا (ڪم == عدم || ڪم->jv_copyID == copyID)
	أرجع FALSE;

    ڪم->jv_copyID = copyID;
    إذا (ڪم->jv_channel != عدم)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = ڪم->jv_channel;
	سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }
    إذا (ڪم->jv_exit_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ڪم->jv_exit_cb.cb_partial;
	سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }

    أرجع FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_channel(
    channel_T		*ch,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    typval_T    dtv;

    إذا (ch == عدم || ch->ch_copyID == copyID)
	أرجع FALSE;

    ch->ch_copyID = copyID;
    لكل (ch_part_T part = PART_SOCK; part < PART_ڳڻپ; ++part)
    {
	لكل (jsonq_T *jq = ch->ch_part[part].ch_json_سر.jq_ايندڙ;
		jq != عدم; jq = jq->jq_ايندڙ)
	    سيٽ_ref_اندريون_شي(jq->jq_قدر, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
	لكل (cbq_T *cq = ch->ch_part[part].ch_cb_سر.cq_ايندڙ; cq != عدم;
		cq = cq->cq_ايندڙ)
	    إذا (cq->cq_callback.cb_partial != عدم)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
	    }
	إذا (ch->ch_part[part].ch_callback.cb_partial != عدم)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
	}
    }
    إذا (ch->ch_callback.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }
    إذا (ch->ch_close_cb.cb_partial != عدم)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	سيٽ_ref_اندريون_شي(&dtv, copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }

    أرجع FALSE;
}
#جيڪڏهن_پورو

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    صحيح
سيٽ_ref_اندريون_شي_class(
    class_T		*cl,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    صحيح abort = FALSE;

    إذا (cl == عدم || cl->class_copyID == copyID)
	أرجع FALSE;

    cl->class_copyID = copyID;
    إذا (cl->class_members_tv != عدم)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	لكل (صحيح i = 0; !abort && i < cl->class_class_member_ڳڻپ; ++i)
	    abort = abort || سيٽ_ref_اندريون_شي(
		    &cl->class_members_tv[i],
		    copyID, ht_ڍير, list_ڍير, tuple_ڍير);
    }

    لكل (صحيح i = 0; !abort && i < cl->class_class_function_ڳڻپ; ++i)
	abort = abort || سيٽ_ref_اندريون_func(عدم,
		cl->class_class_functions[i], copyID);

    لكل (صحيح i = 0; !abort && i < cl->class_obj_طريقو_ڳڻپ; ++i)
	abort = abort || سيٽ_ref_اندريون_func(عدم,
		cl->class_obj_methods[i], copyID);

    أرجع abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ساكن صحيح
سيٽ_ref_اندريون_شي_object(
    object_T		*obj,
    صحيح			copyID,
    ht_ڍير_T		**ht_ڍير,
    list_ڍير_T	**list_ڍير,
    tuple_ڍير_T	**tuple_ڍير)
{
    صحيح abort = FALSE;

    إذا (obj == عدم || obj->obj_copyID == copyID)
	أرجع FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    لكل (صحيح i = 0; !abort
	    && i < obj->obj_class->class_obj_member_ڳڻپ; ++i)
	abort = abort || سيٽ_ref_اندريون_شي(mtv + i, copyID,
		ht_ڍير, list_ڍير, tuple_ڍير);

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
سيٽ_ref_اندريون_شي(
    typval_T	    *tv,
    صحيح		    copyID,
    ht_ڍير_T	    **ht_ڍير,
    list_ڍير_T    **list_ڍير,
    tuple_ڍير_T   **tuple_ڍير)
{
    صحيح		abort = FALSE;

    بدل (tv->v_type)
    {
	حالة VAR_DICT:
	    أرجع سيٽ_ref_اندريون_شي_dict(tv->vval.v_dict, copyID,
					 ht_ڍير, list_ڍير, tuple_ڍير);

	حالة VAR_LIST:
	    أرجع سيٽ_ref_اندريون_شي_list(tv->vval.v_list, copyID,
					 ht_ڍير, list_ڍير, tuple_ڍير);

	حالة VAR_TUPLE:
	    أرجع سيٽ_ref_اندريون_شي_tuple(tv->vval.v_tuple, copyID,
					 ht_ڍير, list_ڍير, tuple_ڍير);
	حالة VAR_FUNC:
	{
	    abort = سيٽ_ref_اندريون_func(tv->vval.v_string, عدم, copyID);
	    اكسر;
	}

	حالة VAR_PARTIAL:
	    أرجع سيٽ_ref_اندريون_شي_partial(tv->vval.v_partial, copyID,
					ht_ڍير, list_ڍير, tuple_ڍير);

	حالة VAR_ڪم:
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	    أرجع سيٽ_ref_اندريون_شي_ڪم(tv->vval.v_ڪم, copyID,
					ht_ڍير, list_ڍير, tuple_ڍير);
#وإلا
	    اكسر;
#جيڪڏهن_پورو

	حالة VAR_CHANNEL:
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	    أرجع سيٽ_ref_اندريون_شي_channel(tv->vval.v_channel, copyID,
					ht_ڍير, list_ڍير, tuple_ڍير);
#وإلا
	    اكسر;
#جيڪڏهن_پورو

	حالة VAR_CLASS:
	    أرجع سيٽ_ref_اندريون_شي_class(tv->vval.v_class, copyID,
					ht_ڍير, list_ڍير, tuple_ڍير);

	حالة VAR_OBJECT:
	    أرجع سيٽ_ref_اندريون_شي_object(tv->vval.v_object, copyID,
					ht_ڍير, list_ڍير, tuple_ڍير);

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_عدد:
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

#جيڪڏهن_پورو
