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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
స్థిర పూర్ణాంకం current_copyID = 0;

స్థిర పూర్ణాంకం free_unref_వస్తువులు(పూర్ణాంకం copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    పూర్ణాంకం
get_copyID(శూన్య)
{
    current_copyID += COPYID_INC;
    తిరిగి current_copyID;
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
    పూర్ణాంకం
garbage_collect(పూర్ణాంకం testing)
{
    పూర్ణాంకం		copyID;
    పూర్ణాంకం		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    పూర్ణాంకం		did_free = FALSE;
    tabpage_T	*tp;

    ఒకవేళ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    ఒకవేళ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	పూర్ణాంకం	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	ఒకవేళ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	ఒకవేళ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_దత్తాంశం, new_len);
	    ఒకవేళ (pp == ఖాళీ)
		తిరిగి FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_దత్తాంశం = pp;
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
    abort = abort || సమితి_ref_లోపల_మునుపటి_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || సమితి_ref_లోపల_అంశం(&buf->b_bufvar.di_tv, copyID,
							ఖాళీ, ఖాళీ, ఖాళీ);
	abort = abort || సమితి_ref_లోపల_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || సమితి_ref_లోపల_అంశం(&wp->w_winvar.di_tv, copyID,
							ఖాళీ, ఖాళీ, ఖాళీ);
    // window-local variables in autocmd windows
    కొరకు (పూర్ణాంకం i = 0; i < AUCMD_WIN_లెక్క; ++i)
	ఒకవేళ (aucmd_win[i].auc_win != ఖాళీ)
	    abort = abort || సమితి_ref_లోపల_అంశం(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
#నిర్వచితమైతే FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || సమితి_ref_లోపల_అంశం(&wp->w_winvar.di_tv, copyID,
								  ఖాళీ, ఖాళీ, ఖాళీ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
		abort = abort || సమితి_ref_లోపల_అంశం(&wp->w_winvar.di_tv, copyID,
								  ఖాళీ, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || సమితి_ref_లోపల_అంశం(&tp->tp_winvar.di_tv, copyID,
								  ఖాళీ, ఖాళీ, ఖాళీ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || సమితి_ref_లోపల_call_దొంతర(copyID);

    // named functions (matters for closures)
    abort = abort || సమితి_ref_లోపల_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || సమితి_ref_లోపల_func_వాదనలు(copyID);

    // funcstacks keep variables for closures
    abort = abort || సమితి_ref_లోపల_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || సమితి_ref_లోపల_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || సమితి_ref_లోపల_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || సమితి_ref_లోపల_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || సమితి_ref_లోపల_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || సమితి_ref_లోపల_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || సమితి_ref_లోపల_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || సమితి_ref_లోపల_findfunc(copyID);

#నిర్వచితమైతే FEAT_LUA
    abort = abort || సమితి_ref_లోపల_lua(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
    abort = abort || సమితి_ref_లోపల_python(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    abort = abort || సమితి_ref_లోపల_python3(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_పని_CHANNEL
    abort = abort || సమితి_ref_లోపల_channel(copyID);
    abort = abort || సమితి_ref_లోపల_పని(copyID);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    abort = abort || సమితి_ref_లోపల_nb_channel(copyID);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOCKETSERVER
    abort = abort || సమితి_ref_లోపల_socketserver_channel(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TIMERS
    abort = abort || సమితి_ref_లోపల_timer(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    abort = abort || సమితి_ref_లోపల_quickfix(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMINAL
    abort = abort || సమితి_ref_లోపల_పదం(copyID);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    abort = abort || సమితి_ref_లోపల_popups(copyID);
#అయితే_ముగింపు

    abort = abort || సమితి_ref_లోపల_classes(copyID);

    ఒకవేళ (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_వస్తువులు(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    లేకపోతే ఒకవేళ (p_verbose > 0)
    {
	verb_సందేశం(_("Not enough memory to set references, garbage collection aborted!"));
    }

    తిరిగి did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    స్థిర పూర్ణాంకం
free_unref_వస్తువులు(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    లోపల_free_unref_వస్తువులు = TRUE;

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

#నిర్వచితమైతే FEAT_పని_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_వాడని_jobs_contents(copyID, COPYID_ముసుగు);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_వాడని_channels_contents(copyID, COPYID_ముసుగు);
#అయితే_ముగింపు

    /*
     * PASS 2: free the items themselves.
     */
    object_free_వస్తువులు(copyID);
    dict_free_వస్తువులు(copyID);
    list_free_వస్తువులు(copyID);
    tuple_free_వస్తువులు(copyID);

#నిర్వచితమైతే FEAT_పని_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_వాడని_jobs(copyID, COPYID_ముసుగు);

    // Go through the list of channels and free items without the copyID.
    free_వాడని_channels(copyID, COPYID_ముసుగు);
#అయితే_ముగింపు

    లోపల_free_unref_వస్తువులు = FALSE;

    తిరిగి did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_ht(
    hashtab_T		*ht,
    పూర్ణాంకం			copyID,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    పూర్ణాంకం		todo;
    పూర్ణాంకం		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_దొంతర_T	*ht_దొంతర = ఖాళీ;
    ht_దొంతర_T	*tempitem;

    cur_ht = ht;
    కొరకు (;;)
    {
	ఒకవేళ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (పూర్ణాంకం)cur_ht->ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(cur_ht, hi, todo)
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    --todo;
		    abort = abort
			|| సమితి_ref_లోపల_అంశం(&HI2DI(hi)->di_tv, copyID,
				       &ht_దొంతర, list_దొంతర, tuple_దొంతర);
		}
	}

	ఒకవేళ (ht_దొంతర == ఖాళీ)
	    ఆపు;

	// take an item from the stack
	cur_ht = ht_దొంతర->ht;
	tempitem = ht_దొంతర;
	ht_దొంతర = ht_దొంతర->prev;
	విడుదల(tempitem);
    }

    తిరిగి abort;
}

#ఒకవేళ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_dict(dict_T *d, పూర్ణాంకం copyID)
{
    ఒకవేళ (d != ఖాళీ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	తిరిగి సమితి_ref_లోపల_ht(&d->dv_hashtab, copyID, ఖాళీ, ఖాళీ);
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_list(list_T *ll, పూర్ణాంకం copyID)
{
    ఒకవేళ (ll != ఖాళీ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	తిరిగి సమితి_ref_లోపల_list_వస్తువులు(ll, copyID, ఖాళీ, ఖాళీ);
    }
    తిరిగి FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_list_వస్తువులు(
    list_T		*l,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    listitem_T	 *li;
    పూర్ణాంకం		 abort = FALSE;
    list_T	 *cur_l;
    list_దొంతర_T *list_దొంతర = ఖాళీ;
    list_దొంతర_T *tempitem;

    cur_l = l;
    కొరకు (;;)
    {
	ఒకవేళ (!abort && cur_l->lv_మొదటి != &range_list_అంశం)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    కొరకు (li = cur_l->lv_మొదటి; !abort && li != ఖాళీ; li = li->li_తదుపరి)
		abort = abort || సమితి_ref_లోపల_అంశం(&li->li_tv, copyID,
				       ht_దొంతర, &list_దొంతర, tuple_దొంతర);
	ఒకవేళ (list_దొంతర == ఖాళీ)
	    ఆపు;

	// take an item from the stack
	cur_l = list_దొంతర->list;
	tempitem = list_దొంతర;
	list_దొంతర = list_దొంతర->prev;
	విడుదల(tempitem);
    }

    తిరిగి abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_tuple(tuple_T *tuple, పూర్ణాంకం copyID)
{
    ఒకవేళ (tuple != ఖాళీ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	తిరిగి సమితి_ref_లోపల_tuple_వస్తువులు(tuple, copyID, ఖాళీ, ఖాళీ);
    }
    తిరిగి FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_tuple_వస్తువులు(
    tuple_T		*tuple,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర)
{
    పూర్ణాంకం			abort = FALSE;
    tuple_T		*cur_t;
    tuple_దొంతర_T	*tuple_దొంతర = ఖాళీ;
    tuple_దొంతర_T	*tempitem;

    cur_t = tuple;
    కొరకు (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	కొరకు (పూర్ణాంకం i = 0; i < cur_t->tv_వస్తువులు.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_వస్తువులు.ga_దత్తాంశం) + i;
	    abort = abort
		|| సమితి_ref_లోపల_అంశం(tv, copyID,
			ht_దొంతర, list_దొంతర, &tuple_దొంతర);
	}
	ఒకవేళ (tuple_దొంతర == ఖాళీ)
	    ఆపు;

	// take an item from the stack
	cur_t = tuple_దొంతర->tuple;
	tempitem = tuple_దొంతర;
	tuple_దొంతర = tuple_దొంతర->prev;
	విడుదల(tempitem);
    }

    తిరిగి abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    పూర్ణాంకం
సమితి_ref_లోపల_callback(callback_T *cb, పూర్ణాంకం copyID)
{
    typval_T tv;

    ఒకవేళ (cb->cb_name == ఖాళీ || *cb->cb_name == NUL || cb->cb_partial == ఖాళీ)
	తిరిగి FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    తిరిగి సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_dict(
    dict_T		*dd,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    ఒకవేళ (dd == ఖాళీ || dd->dv_copyID == copyID)
	తిరిగి FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    ఒకవేళ (ht_దొంతర == ఖాళీ)
	తిరిగి సమితి_ref_లోపల_ht(&dd->dv_hashtab, copyID, list_దొంతర, tuple_దొంతర);

    ht_దొంతర_T *newitem = ALLOC_ONE(ht_దొంతర_T);
    ఒకవేళ (newitem == ఖాళీ)
	తిరిగి TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_దొంతర;
    *ht_దొంతర = newitem;

    తిరిగి FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_list(
    list_T		*ll,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    ఒకవేళ (ll == ఖాళీ || ll->lv_copyID == copyID)
	తిరిగి FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    ఒకవేళ (list_దొంతర == ఖాళీ)
	తిరిగి సమితి_ref_లోపల_list_వస్తువులు(ll, copyID, ht_దొంతర, tuple_దొంతర);

    list_దొంతర_T *newitem = ALLOC_ONE(list_దొంతర_T);
    ఒకవేళ (newitem == ఖాళీ)
	తిరిగి TRUE;

    newitem->list = ll;
    newitem->prev = *list_దొంతర;
    *list_దొంతర = newitem;

    తిరిగి FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_tuple(
    tuple_T		*tt,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    ఒకవేళ (tt == ఖాళీ || tt->tv_copyID == copyID)
	తిరిగి FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    ఒకవేళ (tuple_దొంతర == ఖాళీ)
	తిరిగి సమితి_ref_లోపల_tuple_వస్తువులు(tt, copyID, ht_దొంతర, list_దొంతర);

    tuple_దొంతర_T *newitem = ALLOC_ONE(tuple_దొంతర_T);
    ఒకవేళ (newitem == ఖాళీ)
	తిరిగి TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_దొంతర;
    *tuple_దొంతర = newitem;

    తిరిగి FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_partial(
    partial_T		*pt,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    ఒకవేళ (pt == ఖాళీ || pt->pt_copyID == copyID)
	తిరిగి FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    పూర్ణాంకం abort = సమితి_ref_లోపల_func(pt->pt_name, pt->pt_func, copyID);

    ఒకవేళ (pt->pt_dict != ఖాళీ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }

    ఒకవేళ (pt->pt_obj != ఖాళీ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	సమితి_ref_లోపల_అంశం(&objtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }

    కొరకు (పూర్ణాంకం i = 0; i < pt->pt_argc; ++i)
	abort = abort || సమితి_ref_లోపల_అంశం(&pt->pt_argv[i], copyID,
		ht_దొంతర, list_దొంతర, tuple_దొంతర);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    తిరిగి abort;
}

#నిర్వచితమైతే FEAT_పని_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_పని(
    పని_T		*పని,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    typval_T    dtv;

    ఒకవేళ (పని == ఖాళీ || పని->jv_copyID == copyID)
	తిరిగి FALSE;

    పని->jv_copyID = copyID;
    ఒకవేళ (పని->jv_channel != ఖాళీ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = పని->jv_channel;
	సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }
    ఒకవేళ (పని->jv_exit_cb.cb_partial != ఖాళీ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = పని->jv_exit_cb.cb_partial;
	సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }

    తిరిగి FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_channel(
    channel_T		*ch,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    typval_T    dtv;

    ఒకవేళ (ch == ఖాళీ || ch->ch_copyID == copyID)
	తిరిగి FALSE;

    ch->ch_copyID = copyID;
    కొరకు (ch_part_T part = PART_SOCK; part < PART_లెక్క; ++part)
    {
	కొరకు (jsonq_T *jq = ch->ch_part[part].ch_json_తల.jq_తదుపరి;
		jq != ఖాళీ; jq = jq->jq_తదుపరి)
	    సమితి_ref_లోపల_అంశం(jq->jq_విలువ, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
	కొరకు (cbq_T *cq = ch->ch_part[part].ch_cb_తల.cq_తదుపరి; cq != ఖాళీ;
		cq = cq->cq_తదుపరి)
	    ఒకవేళ (cq->cq_callback.cb_partial != ఖాళీ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
	    }
	ఒకవేళ (ch->ch_part[part].ch_callback.cb_partial != ఖాళీ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
	}
    }
    ఒకవేళ (ch->ch_callback.cb_partial != ఖాళీ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }
    ఒకవేళ (ch->ch_close_cb.cb_partial != ఖాళీ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	సమితి_ref_లోపల_అంశం(&dtv, copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }

    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    పూర్ణాంకం
సమితి_ref_లోపల_అంశం_class(
    class_T		*cl,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    పూర్ణాంకం abort = FALSE;

    ఒకవేళ (cl == ఖాళీ || cl->class_copyID == copyID)
	తిరిగి FALSE;

    cl->class_copyID = copyID;
    ఒకవేళ (cl->class_members_tv != ఖాళీ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	కొరకు (పూర్ణాంకం i = 0; !abort && i < cl->class_class_member_లెక్క; ++i)
	    abort = abort || సమితి_ref_లోపల_అంశం(
		    &cl->class_members_tv[i],
		    copyID, ht_దొంతర, list_దొంతర, tuple_దొంతర);
    }

    కొరకు (పూర్ణాంకం i = 0; !abort && i < cl->class_class_function_లెక్క; ++i)
	abort = abort || సమితి_ref_లోపల_func(ఖాళీ,
		cl->class_class_functions[i], copyID);

    కొరకు (పూర్ణాంకం i = 0; !abort && i < cl->class_obj_పద్ధతి_లెక్క; ++i)
	abort = abort || సమితి_ref_లోపల_func(ఖాళీ,
		cl->class_obj_methods[i], copyID);

    తిరిగి abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_అంశం_object(
    object_T		*obj,
    పూర్ణాంకం			copyID,
    ht_దొంతర_T		**ht_దొంతర,
    list_దొంతర_T	**list_దొంతర,
    tuple_దొంతర_T	**tuple_దొంతర)
{
    పూర్ణాంకం abort = FALSE;

    ఒకవేళ (obj == ఖాళీ || obj->obj_copyID == copyID)
	తిరిగి FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    కొరకు (పూర్ణాంకం i = 0; !abort
	    && i < obj->obj_class->class_obj_member_లెక్క; ++i)
	abort = abort || సమితి_ref_లోపల_అంశం(mtv + i, copyID,
		ht_దొంతర, list_దొంతర, tuple_దొంతర);

    తిరిగి abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_అంశం(
    typval_T	    *tv,
    పూర్ణాంకం		    copyID,
    ht_దొంతర_T	    **ht_దొంతర,
    list_దొంతర_T    **list_దొంతర,
    tuple_దొంతర_T   **tuple_దొంతర)
{
    పూర్ణాంకం		abort = FALSE;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_DICT:
	    తిరిగి సమితి_ref_లోపల_అంశం_dict(tv->vval.v_dict, copyID,
					 ht_దొంతర, list_దొంతర, tuple_దొంతర);

	స్థితి VAR_LIST:
	    తిరిగి సమితి_ref_లోపల_అంశం_list(tv->vval.v_list, copyID,
					 ht_దొంతర, list_దొంతర, tuple_దొంతర);

	స్థితి VAR_TUPLE:
	    తిరిగి సమితి_ref_లోపల_అంశం_tuple(tv->vval.v_tuple, copyID,
					 ht_దొంతర, list_దొంతర, tuple_దొంతర);
	స్థితి VAR_FUNC:
	{
	    abort = సమితి_ref_లోపల_func(tv->vval.v_string, ఖాళీ, copyID);
	    ఆపు;
	}

	స్థితి VAR_PARTIAL:
	    తిరిగి సమితి_ref_లోపల_అంశం_partial(tv->vval.v_partial, copyID,
					ht_దొంతర, list_దొంతర, tuple_దొంతర);

	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి సమితి_ref_లోపల_అంశం_పని(tv->vval.v_పని, copyID,
					ht_దొంతర, list_దొంతర, tuple_దొంతర);
#లేకపోతే
	    ఆపు;
#అయితే_ముగింపు

	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి సమితి_ref_లోపల_అంశం_channel(tv->vval.v_channel, copyID,
					ht_దొంతర, list_దొంతర, tuple_దొంతర);
#లేకపోతే
	    ఆపు;
#అయితే_ముగింపు

	స్థితి VAR_CLASS:
	    తిరిగి సమితి_ref_లోపల_అంశం_class(tv->vval.v_class, copyID,
					ht_దొంతర, list_దొంతర, tuple_దొంతర);

	స్థితి VAR_OBJECT:
	    తిరిగి సమితి_ref_లోపల_అంశం_object(tv->vval.v_object, copyID,
					ht_దొంతర, list_దొంతర, tuple_దొంతర);

	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_FLOAT:
	స్థితి VAR_STRING:
	స్థితి VAR_BLOB:
	స్థితి VAR_TYPEALIAS:
	స్థితి VAR_INSTR:
	    // Types that do not contain any other item
	    ఆపు;
    }

    తిరిగి abort;
}

#అయితే_ముగింపు
