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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
स्थिर पूर्णाङ्क current_copyID = 0;

स्थिर पूर्णाङ्क free_unref_वस्तूनि(पूर्णाङ्क copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    पूर्णाङ्क
get_copyID(शून्य)
{
    current_copyID += COPYID_INC;
    पटिनिवत्त current_copyID;
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
    पूर्णाङ्क
garbage_collect(पूर्णाङ्क testing)
{
    पूर्णाङ्क		copyID;
    पूर्णाङ्क		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    पूर्णाङ्क		did_free = FALSE;
    tabpage_T	*tp;

    सचे (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    सचे (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	पूर्णाङ्क	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	सचे (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	सचे (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    सचे (pp == रिक्त)
		पटिनिवत्त FAIL;
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
							रिक्त, रिक्त, रिक्त);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							रिक्त, रिक्त, रिक्त);
    // window-local variables in autocmd windows
    अत्थाय (पूर्णाङ्क i = 0; i < AUCMD_WIN_गणनम्; ++i)
	सचे (aucmd_win[i].auc_win != रिक्त)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, रिक्त, रिक्त, रिक्त);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_राशिः(copyID);

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

    सचे (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_वस्तूनि(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    अञ्ञथा सचे (p_verbose > 0)
    {
	verb_सन्देशः(_("Not enough memory to set references, garbage collection aborted!"));
    }

    पटिनिवत्त did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    स्थिर पूर्णाङ्क
free_unref_वस्तूनि(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_वस्तूनि = TRUE;

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
    object_free_वस्तूनि(copyID);
    dict_free_वस्तूनि(copyID);
    list_free_वस्तूनि(copyID);
    tuple_free_वस्तूनि(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_वस्तूनि = FALSE;

    पटिनिवत्त did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_ht(
    hashtab_T		*ht,
    पूर्णाङ्क			copyID,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    पूर्णाङ्क		todo;
    पूर्णाङ्क		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_राशिः_T	*ht_राशिः = रिक्त;
    ht_राशिः_T	*tempitem;

    cur_ht = ht;
    अत्थाय (;;)
    {
	सचे (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (पूर्णाङ्क)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_वस्तूनि(cur_ht, hi, todo)
		सचे (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_राशिः, list_राशिः, tuple_राशिः);
		}
	}

	सचे (ht_राशिः == रिक्त)
	    भङ्ग;

	// take an item from the stack
	cur_ht = ht_राशिः->ht;
	tempitem = ht_राशिः;
	ht_राशिः = ht_राशिः->prev;
	मुक्त(tempitem);
    }

    पटिनिवत्त abort;
}

#सचे defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_dict(dict_T *d, पूर्णाङ्क copyID)
{
    सचे (d != रिक्त && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	पटिनिवत्त set_ref_in_ht(&d->dv_hashtab, copyID, रिक्त, रिक्त);
    }
    पटिनिवत्त FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_list(list_T *ll, पूर्णाङ्क copyID)
{
    सचे (ll != रिक्त && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	पटिनिवत्त set_ref_in_list_वस्तूनि(ll, copyID, रिक्त, रिक्त);
    }
    पटिनिवत्त FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_list_वस्तूनि(
    list_T		*l,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    listitem_T	 *li;
    पूर्णाङ्क		 abort = FALSE;
    list_T	 *cur_l;
    list_राशिः_T *list_राशिः = रिक्त;
    list_राशिः_T *tempitem;

    cur_l = l;
    अत्थाय (;;)
    {
	सचे (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    अत्थाय (li = cur_l->lv_first; !abort && li != रिक्त; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_राशिः, &list_राशिः, tuple_राशिः);
	सचे (list_राशिः == रिक्त)
	    भङ्ग;

	// take an item from the stack
	cur_l = list_राशिः->list;
	tempitem = list_राशिः;
	list_राशिः = list_राशिः->prev;
	मुक्त(tempitem);
    }

    पटिनिवत्त abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_tuple(tuple_T *tuple, पूर्णाङ्क copyID)
{
    सचे (tuple != रिक्त && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	पटिनिवत्त set_ref_in_tuple_वस्तूनि(tuple, copyID, रिक्त, रिक्त);
    }
    पटिनिवत्त FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_tuple_वस्तूनि(
    tuple_T		*tuple,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः)
{
    पूर्णाङ्क			abort = FALSE;
    tuple_T		*cur_t;
    tuple_राशिः_T	*tuple_राशिः = रिक्त;
    tuple_राशिः_T	*tempitem;

    cur_t = tuple;
    अत्थाय (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	अत्थाय (पूर्णाङ्क i = 0; i < cur_t->tv_वस्तूनि.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_वस्तूनि.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_राशिः, list_राशिः, &tuple_राशिः);
	}
	सचे (tuple_राशिः == रिक्त)
	    भङ्ग;

	// take an item from the stack
	cur_t = tuple_राशिः->tuple;
	tempitem = tuple_राशिः;
	tuple_राशिः = tuple_राशिः->prev;
	मुक्त(tempitem);
    }

    पटिनिवत्त abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    पूर्णाङ्क
set_ref_in_callback(callback_T *cb, पूर्णाङ्क copyID)
{
    typval_T tv;

    सचे (cb->cb_name == रिक्त || *cb->cb_name == NUL || cb->cb_partial == रिक्त)
	पटिनिवत्त FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    पटिनिवत्त set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_dict(
    dict_T		*dd,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    सचे (dd == रिक्त || dd->dv_copyID == copyID)
	पटिनिवत्त FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    सचे (ht_राशिः == रिक्त)
	पटिनिवत्त set_ref_in_ht(&dd->dv_hashtab, copyID, list_राशिः, tuple_राशिः);

    ht_राशिः_T *newitem = ALLOC_ONE(ht_राशिः_T);
    सचे (newitem == रिक्त)
	पटिनिवत्त TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_राशिः;
    *ht_राशिः = newitem;

    पटिनिवत्त FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_list(
    list_T		*ll,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    सचे (ll == रिक्त || ll->lv_copyID == copyID)
	पटिनिवत्त FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    सचे (list_राशिः == रिक्त)
	पटिनिवत्त set_ref_in_list_वस्तूनि(ll, copyID, ht_राशिः, tuple_राशिः);

    list_राशिः_T *newitem = ALLOC_ONE(list_राशिः_T);
    सचे (newitem == रिक्त)
	पटिनिवत्त TRUE;

    newitem->list = ll;
    newitem->prev = *list_राशिः;
    *list_राशिः = newitem;

    पटिनिवत्त FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_tuple(
    tuple_T		*tt,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    सचे (tt == रिक्त || tt->tv_copyID == copyID)
	पटिनिवत्त FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    सचे (tuple_राशिः == रिक्त)
	पटिनिवत्त set_ref_in_tuple_वस्तूनि(tt, copyID, ht_राशिः, list_राशिः);

    tuple_राशिः_T *newitem = ALLOC_ONE(tuple_राशिः_T);
    सचे (newitem == रिक्त)
	पटिनिवत्त TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_राशिः;
    *tuple_राशिः = newitem;

    पटिनिवत्त FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_partial(
    partial_T		*pt,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    सचे (pt == रिक्त || pt->pt_copyID == copyID)
	पटिनिवत्त FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    पूर्णाङ्क abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    सचे (pt->pt_dict != रिक्त)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }

    सचे (pt->pt_obj != रिक्त)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }

    अत्थाय (पूर्णाङ्क i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_राशिः, list_राशिः, tuple_राशिः);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    पटिनिवत्त abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_job(
    job_T		*job,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    typval_T    dtv;

    सचे (job == रिक्त || job->jv_copyID == copyID)
	पटिनिवत्त FALSE;

    job->jv_copyID = copyID;
    सचे (job->jv_channel != रिक्त)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }
    सचे (job->jv_exit_cb.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }

    पटिनिवत्त FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_channel(
    channel_T		*ch,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    typval_T    dtv;

    सचे (ch == रिक्त || ch->ch_copyID == copyID)
	पटिनिवत्त FALSE;

    ch->ch_copyID = copyID;
    अत्थाय (ch_part_T part = PART_SOCK; part < PART_गणनम्; ++part)
    {
	अत्थाय (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != रिक्त; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_मानम्, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
	अत्थाय (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != रिक्त;
		cq = cq->cq_next)
	    सचे (cq->cq_callback.cb_partial != रिक्त)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
	    }
	सचे (ch->ch_part[part].ch_callback.cb_partial != रिक्त)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
	}
    }
    सचे (ch->ch_callback.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }
    सचे (ch->ch_close_cb.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }

    पटिनिवत्त FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    पूर्णाङ्क
set_ref_in_item_class(
    class_T		*cl,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    पूर्णाङ्क abort = FALSE;

    सचे (cl == रिक्त || cl->class_copyID == copyID)
	पटिनिवत्त FALSE;

    cl->class_copyID = copyID;
    सचे (cl->class_members_tv != रिक्त)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	अत्थाय (पूर्णाङ्क i = 0; !abort && i < cl->class_class_member_गणनम्; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_राशिः, list_राशिः, tuple_राशिः);
    }

    अत्थाय (पूर्णाङ्क i = 0; !abort && i < cl->class_class_function_गणनम्; ++i)
	abort = abort || set_ref_in_func(रिक्त,
		cl->class_class_functions[i], copyID);

    अत्थाय (पूर्णाङ्क i = 0; !abort && i < cl->class_obj_method_गणनम्; ++i)
	abort = abort || set_ref_in_func(रिक्त,
		cl->class_obj_methods[i], copyID);

    पटिनिवत्त abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णाङ्क
set_ref_in_item_object(
    object_T		*obj,
    पूर्णाङ्क			copyID,
    ht_राशिः_T		**ht_राशिः,
    list_राशिः_T	**list_राशिः,
    tuple_राशिः_T	**tuple_राशिः)
{
    पूर्णाङ्क abort = FALSE;

    सचे (obj == रिक्त || obj->obj_copyID == copyID)
	पटिनिवत्त FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    अत्थाय (पूर्णाङ्क i = 0; !abort
	    && i < obj->obj_class->class_obj_member_गणनम्; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_राशिः, list_राशिः, tuple_राशिः);

    पटिनिवत्त abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_item(
    typval_T	    *tv,
    पूर्णाङ्क		    copyID,
    ht_राशिः_T	    **ht_राशिः,
    list_राशिः_T    **list_राशिः,
    tuple_राशिः_T   **tuple_राशिः)
{
    पूर्णाङ्क		abort = FALSE;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_DICT:
	    पटिनिवत्त set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_राशिः, list_राशिः, tuple_राशिः);

	पक्ष VAR_LIST:
	    पटिनिवत्त set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_राशिः, list_राशिः, tuple_राशिः);

	पक्ष VAR_TUPLE:
	    पटिनिवत्त set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_राशिः, list_राशिः, tuple_राशिः);
	पक्ष VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, रिक्त, copyID);
	    भङ्ग;
	}

	पक्ष VAR_PARTIAL:
	    पटिनिवत्त set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_राशिः, list_राशिः, tuple_राशिः);

	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    पटिनिवत्त set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_राशिः, list_राशिः, tuple_राशिः);
#अञ्ञथा
	    भङ्ग;
#endif

	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    पटिनिवत्त set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_राशिः, list_राशिः, tuple_राशिः);
#अञ्ञथा
	    भङ्ग;
#endif

	पक्ष VAR_CLASS:
	    पटिनिवत्त set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_राशिः, list_राशिः, tuple_राशिः);

	पक्ष VAR_OBJECT:
	    पटिनिवत्त set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_राशिः, list_राशिः, tuple_राशिः);

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_FLOAT:
	पक्ष VAR_STRING:
	पक्ष VAR_BLOB:
	पक्ष VAR_TYPEALIAS:
	पक्ष VAR_INSTR:
	    // Types that do not contain any other item
	    भङ्ग;
    }

    पटिनिवत्त abort;
}

#endif
