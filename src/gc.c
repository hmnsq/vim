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

#जर defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
स्थिर पूर्णांक current_copyID = 0;

स्थिर पूर्णांक free_unref_वस्तुसंच(पूर्णांक copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    पूर्णांक
get_copyID(शून्य)
{
    current_copyID += COPYID_INC;
    परत current_copyID;
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
    पूर्णांक
garbage_collect(पूर्णांक testing)
{
    पूर्णांक		copyID;
    पूर्णांक		पात = FALSE;
    buf_T	*buf;
    win_T	*wp;
    पूर्णांक		did_free = FALSE;
    tabpage_T	*tp;

    जर (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    जर (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	माप_प्रकार	new_len;
	char_u	*pp;
	पूर्णांक	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	जर (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	जर (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (माप_प्रकार)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    जर (pp == रिक्त)
		परत FAIL;
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
    पात = पात || set_ref_in_previous_funccal(copyID);

    // script-local variables
    पात = पात || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	पात = पात || set_ref_in_item(&buf->b_bufvar.di_tv, copyID,
							रिक्त, रिक्त, रिक्त);
	पात = पात || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	पात = पात || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							रिक्त, रिक्त, रिक्त);
    // window-local variables in autocmd windows
    साठी (पूर्णांक i = 0; i < AUCMD_WIN_मोजणी; ++i)
	जर (aucmd_win[i].auc_win != रिक्त)
	    पात = पात || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, रिक्त, रिक्त, रिक्त);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	पात = पात || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		पात = पात || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	पात = पात || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  रिक्त, रिक्त, रिक्त);
    // global variables
    पात = पात || garbage_collect_globvars(copyID);

    // function-local variables
    पात = पात || set_ref_in_call_चळत(copyID);

    // named functions (matters for closures)
    पात = पात || set_ref_in_functions(copyID);

    // function call arguments, if v:testing is set.
    पात = पात || set_ref_in_func_args(copyID);

    // funcstacks keep variables for closures
    पात = पात || set_ref_in_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    पात = पात || set_ref_in_loopvars(copyID);

    // v: vars
    पात = पात || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    पात = पात || set_ref_in_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    पात = पात || set_ref_in_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    पात = पात || set_ref_in_opfunc(copyID);

    // 'tagfunc' callback
    पात = पात || set_ref_in_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    पात = पात || set_ref_in_im_funcs(copyID);

    // 'findfunc' callback
    पात = पात || set_ref_in_findfunc(copyID);

#ifdef FEAT_LUA
    पात = पात || set_ref_in_lua(copyID);
#endif

#ifdef FEAT_PYTHON
    पात = पात || set_ref_in_python(copyID);
#endif

#ifdef FEAT_PYTHON3
    पात = पात || set_ref_in_python3(copyID);
#endif

#ifdef FEAT_JOB_CHANNEL
    पात = पात || set_ref_in_channel(copyID);
    पात = पात || set_ref_in_job(copyID);
#endif
#ifdef FEAT_NETBEANS_INTG
    पात = पात || set_ref_in_nb_channel(copyID);
#endif
#ifdef FEAT_SOCKETSERVER
    पात = पात || set_ref_in_socketserver_channel(copyID);
#endif

#ifdef FEAT_TIMERS
    पात = पात || set_ref_in_timer(copyID);
#endif

#ifdef FEAT_QUICKFIX
    पात = पात || set_ref_in_quickfix(copyID);
#endif

#ifdef FEAT_TERMINAL
    पात = पात || set_ref_in_term(copyID);
#endif

#ifdef FEAT_PROP_POPUP
    पात = पात || set_ref_in_popups(copyID);
#endif

    पात = पात || set_ref_in_classes(copyID);

    जर (!पात)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_वस्तुसंच(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    अन्यथा जर (p_verbose > 0)
    {
	verb_संदेश(_("Not enough memory to set references, garbage collection aborted!"));
    }

    परत did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    स्थिर पूर्णांक
free_unref_वस्तुसंच(पूर्णांक copyID)
{
    पूर्णांक		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_वस्तुसंच = TRUE;

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
    object_free_वस्तुसंच(copyID);
    dict_free_वस्तुसंच(copyID);
    list_free_वस्तुसंच(copyID);
    tuple_free_वस्तुसंच(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_वस्तुसंच = FALSE;

    परत did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_ht(
    hashtab_T		*ht,
    पूर्णांक			copyID,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    पूर्णांक		todo;
    पूर्णांक		पात = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_चळत_T	*ht_चळत = रिक्त;
    ht_चळत_T	*tempitem;

    cur_ht = ht;
    साठी (;;)
    {
	जर (!पात)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (पूर्णांक)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_वस्तुसंच(cur_ht, hi, todo)
		जर (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    पात = पात
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_चळत, list_चळत, tuple_चळत);
		}
	}

	जर (ht_चळत == रिक्त)
	    थांबा;

	// take an item from the stack
	cur_ht = ht_चळत->ht;
	tempitem = ht_चळत;
	ht_चळत = ht_चळत->prev;
	मुक्त(tempitem);
    }

    परत पात;
}

#जर defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_dict(dict_T *d, पूर्णांक copyID)
{
    जर (d != रिक्त && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	परत set_ref_in_ht(&d->dv_hashtab, copyID, रिक्त, रिक्त);
    }
    परत FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_list(list_T *ll, पूर्णांक copyID)
{
    जर (ll != रिक्त && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	परत set_ref_in_list_वस्तुसंच(ll, copyID, रिक्त, रिक्त);
    }
    परत FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_list_वस्तुसंच(
    list_T		*l,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    listitem_T	 *li;
    पूर्णांक		 पात = FALSE;
    list_T	 *cur_l;
    list_चळत_T *list_चळत = रिक्त;
    list_चळत_T *tempitem;

    cur_l = l;
    साठी (;;)
    {
	जर (!पात && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    साठी (li = cur_l->lv_first; !पात && li != रिक्त; li = li->li_next)
		पात = पात || set_ref_in_item(&li->li_tv, copyID,
				       ht_चळत, &list_चळत, tuple_चळत);
	जर (list_चळत == रिक्त)
	    थांबा;

	// take an item from the stack
	cur_l = list_चळत->list;
	tempitem = list_चळत;
	list_चळत = list_चळत->prev;
	मुक्त(tempitem);
    }

    परत पात;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_tuple(tuple_T *tuple, पूर्णांक copyID)
{
    जर (tuple != रिक्त && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	परत set_ref_in_tuple_वस्तुसंच(tuple, copyID, रिक्त, रिक्त);
    }
    परत FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_tuple_वस्तुसंच(
    tuple_T		*tuple,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत)
{
    पूर्णांक			पात = FALSE;
    tuple_T		*cur_t;
    tuple_चळत_T	*tuple_चळत = रिक्त;
    tuple_चळत_T	*tempitem;

    cur_t = tuple;
    साठी (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	साठी (पूर्णांक i = 0; i < cur_t->tv_वस्तुसंच.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_वस्तुसंच.ga_data) + i;
	    पात = पात
		|| set_ref_in_item(tv, copyID,
			ht_चळत, list_चळत, &tuple_चळत);
	}
	जर (tuple_चळत == रिक्त)
	    थांबा;

	// take an item from the stack
	cur_t = tuple_चळत->tuple;
	tempitem = tuple_चळत;
	tuple_चळत = tuple_चळत->prev;
	मुक्त(tempitem);
    }

    परत पात;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    पूर्णांक
set_ref_in_callback(callback_T *cb, पूर्णांक copyID)
{
    typval_T tv;

    जर (cb->cb_name == रिक्त || *cb->cb_name == NUL || cb->cb_partial == रिक्त)
	परत FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    परत set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_dict(
    dict_T		*dd,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    जर (dd == रिक्त || dd->dv_copyID == copyID)
	परत FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    जर (ht_चळत == रिक्त)
	परत set_ref_in_ht(&dd->dv_hashtab, copyID, list_चळत, tuple_चळत);

    ht_चळत_T *newitem = ALLOC_ONE(ht_चळत_T);
    जर (newitem == रिक्त)
	परत TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_चळत;
    *ht_चळत = newitem;

    परत FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_list(
    list_T		*ll,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    जर (ll == रिक्त || ll->lv_copyID == copyID)
	परत FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    जर (list_चळत == रिक्त)
	परत set_ref_in_list_वस्तुसंच(ll, copyID, ht_चळत, tuple_चळत);

    list_चळत_T *newitem = ALLOC_ONE(list_चळत_T);
    जर (newitem == रिक्त)
	परत TRUE;

    newitem->list = ll;
    newitem->prev = *list_चळत;
    *list_चळत = newitem;

    परत FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_tuple(
    tuple_T		*tt,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    जर (tt == रिक्त || tt->tv_copyID == copyID)
	परत FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    जर (tuple_चळत == रिक्त)
	परत set_ref_in_tuple_वस्तुसंच(tt, copyID, ht_चळत, list_चळत);

    tuple_चळत_T *newitem = ALLOC_ONE(tuple_चळत_T);
    जर (newitem == रिक्त)
	परत TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_चळत;
    *tuple_चळत = newitem;

    परत FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_partial(
    partial_T		*pt,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    जर (pt == रिक्त || pt->pt_copyID == copyID)
	परत FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    पूर्णांक पात = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    जर (pt->pt_dict != रिक्त)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }

    जर (pt->pt_obj != रिक्त)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }

    साठी (पूर्णांक i = 0; i < pt->pt_argc; ++i)
	पात = पात || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_चळत, list_चळत, tuple_चळत);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    परत पात;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_job(
    job_T		*job,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    typval_T    dtv;

    जर (job == रिक्त || job->jv_copyID == copyID)
	परत FALSE;

    job->jv_copyID = copyID;
    जर (job->jv_channel != रिक्त)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }
    जर (job->jv_exit_cb.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }

    परत FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_channel(
    channel_T		*ch,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    typval_T    dtv;

    जर (ch == रिक्त || ch->ch_copyID == copyID)
	परत FALSE;

    ch->ch_copyID = copyID;
    साठी (ch_part_T part = PART_SOCK; part < PART_मोजणी; ++part)
    {
	साठी (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != रिक्त; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_मूल्य, copyID, ht_चळत, list_चळत, tuple_चळत);
	साठी (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != रिक्त;
		cq = cq->cq_next)
	    जर (cq->cq_callback.cb_partial != रिक्त)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
	    }
	जर (ch->ch_part[part].ch_callback.cb_partial != रिक्त)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
	}
    }
    जर (ch->ch_callback.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }
    जर (ch->ch_close_cb.cb_partial != रिक्त)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_चळत, list_चळत, tuple_चळत);
    }

    परत FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    पूर्णांक
set_ref_in_item_class(
    class_T		*cl,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    पूर्णांक पात = FALSE;

    जर (cl == रिक्त || cl->class_copyID == copyID)
	परत FALSE;

    cl->class_copyID = copyID;
    जर (cl->class_members_tv != रिक्त)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	साठी (पूर्णांक i = 0; !पात && i < cl->class_class_member_मोजणी; ++i)
	    पात = पात || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_चळत, list_चळत, tuple_चळत);
    }

    साठी (पूर्णांक i = 0; !पात && i < cl->class_class_function_मोजणी; ++i)
	पात = पात || set_ref_in_func(रिक्त,
		cl->class_class_functions[i], copyID);

    साठी (पूर्णांक i = 0; !पात && i < cl->class_obj_method_मोजणी; ++i)
	पात = पात || set_ref_in_func(रिक्त,
		cl->class_obj_methods[i], copyID);

    परत पात;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    स्थिर पूर्णांक
set_ref_in_item_object(
    object_T		*obj,
    पूर्णांक			copyID,
    ht_चळत_T		**ht_चळत,
    list_चळत_T	**list_चळत,
    tuple_चळत_T	**tuple_चळत)
{
    पूर्णांक पात = FALSE;

    जर (obj == रिक्त || obj->obj_copyID == copyID)
	परत FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    साठी (पूर्णांक i = 0; !पात
	    && i < obj->obj_class->class_obj_member_मोजणी; ++i)
	पात = पात || set_ref_in_item(mtv + i, copyID,
		ht_चळत, list_चळत, tuple_चळत);

    परत पात;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_item(
    typval_T	    *tv,
    पूर्णांक		    copyID,
    ht_चळत_T	    **ht_चळत,
    list_चळत_T    **list_चळत,
    tuple_चळत_T   **tuple_चळत)
{
    पूर्णांक		पात = FALSE;

    निवड (tv->v_type)
    {
	स्थिती VAR_DICT:
	    परत set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_चळत, list_चळत, tuple_चळत);

	स्थिती VAR_LIST:
	    परत set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_चळत, list_चळत, tuple_चळत);

	स्थिती VAR_TUPLE:
	    परत set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_चळत, list_चळत, tuple_चळत);
	स्थिती VAR_FUNC:
	{
	    पात = set_ref_in_func(tv->vval.v_string, रिक्त, copyID);
	    थांबा;
	}

	स्थिती VAR_PARTIAL:
	    परत set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_चळत, list_चळत, tuple_चळत);

	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    परत set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_चळत, list_चळत, tuple_चळत);
#अन्यथा
	    थांबा;
#endif

	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    परत set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_चळत, list_चळत, tuple_चळत);
#अन्यथा
	    थांबा;
#endif

	स्थिती VAR_CLASS:
	    परत set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_चळत, list_चळत, tuple_चळत);

	स्थिती VAR_OBJECT:
	    परत set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_चळत, list_चळत, tuple_चळत);

	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_VOID:
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	स्थिती VAR_संख्या:
	स्थिती VAR_FLOAT:
	स्थिती VAR_STRING:
	स्थिती VAR_BLOB:
	स्थिती VAR_TYPEALIAS:
	स्थिती VAR_INSTR:
	    // Types that do not contain any other item
	    थांबा;
    }

    परत पात;
}

#endif
