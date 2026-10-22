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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ current_copyID = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ free_unref_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    പൂർണ്ണസംഖ്യ
get_copyID(ശൂന്യം)
{
    current_copyID += COPYID_INC;
    തിരികെ current_copyID;
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
    പൂർണ്ണസംഖ്യ
garbage_collect(പൂർണ്ണസംഖ്യ testing)
{
    പൂർണ്ണസംഖ്യ		copyID;
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		did_free = FALSE;
    tabpage_T	*tp;

    എങ്കിൽ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    എങ്കിൽ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	പൂർണ്ണസംഖ്യ	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	എങ്കിൽ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	എങ്കിൽ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    എങ്കിൽ (pp == ശൂന്യ)
		തിരികെ FAIL;
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
							ശൂന്യ, ശൂന്യ, ശൂന്യ);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							ശൂന്യ, ശൂന്യ, ശൂന്യ);
    // window-local variables in autocmd windows
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < AUCMD_WIN_എണ്ണം; ++i)
	എങ്കിൽ (aucmd_win[i].auc_win != ശൂന്യ)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ശൂന്യ, ശൂന്യ, ശൂന്യ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ശൂന്യ, ശൂന്യ, ശൂന്യ);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  ശൂന്യ, ശൂന്യ, ശൂന്യ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_അട്ടി(copyID);

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

    എങ്കിൽ (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_ഇനങ്ങൾ(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_verbose > 0)
    {
	verb_സന്ദേശം(_("Not enough memory to set references, garbage collection aborted!"));
    }

    തിരികെ did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
free_unref_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_ഇനങ്ങൾ = TRUE;

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
    object_free_ഇനങ്ങൾ(copyID);
    dict_free_ഇനങ്ങൾ(copyID);
    list_free_ഇനങ്ങൾ(copyID);
    tuple_free_ഇനങ്ങൾ(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_ഇനങ്ങൾ = FALSE;

    തിരികെ did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_ht(
    hashtab_T		*ht,
    പൂർണ്ണസംഖ്യ			copyID,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    പൂർണ്ണസംഖ്യ		todo;
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_അട്ടി_T	*ht_അട്ടി = ശൂന്യ;
    ht_അട്ടി_T	*tempitem;

    cur_ht = ht;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (പൂർണ്ണസംഖ്യ)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(cur_ht, hi, todo)
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
		}
	}

	എങ്കിൽ (ht_അട്ടി == ശൂന്യ)
	    നിർത്തുക;

	// take an item from the stack
	cur_ht = ht_അട്ടി->ht;
	tempitem = ht_അട്ടി;
	ht_അട്ടി = ht_അട്ടി->prev;
	സ്വതന്ത്രം(tempitem);
    }

    തിരികെ abort;
}

#എങ്കിൽ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_dict(dict_T *d, പൂർണ്ണസംഖ്യ copyID)
{
    എങ്കിൽ (d != ശൂന്യ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	തിരികെ set_ref_in_ht(&d->dv_hashtab, copyID, ശൂന്യ, ശൂന്യ);
    }
    തിരികെ FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_list(list_T *ll, പൂർണ്ണസംഖ്യ copyID)
{
    എങ്കിൽ (ll != ശൂന്യ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	തിരികെ set_ref_in_list_ഇനങ്ങൾ(ll, copyID, ശൂന്യ, ശൂന്യ);
    }
    തിരികെ FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_list_ഇനങ്ങൾ(
    list_T		*l,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    listitem_T	 *li;
    പൂർണ്ണസംഖ്യ		 abort = FALSE;
    list_T	 *cur_l;
    list_അട്ടി_T *list_അട്ടി = ശൂന്യ;
    list_അട്ടി_T *tempitem;

    cur_l = l;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    വേണ്ടി (li = cur_l->lv_first; !abort && li != ശൂന്യ; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_അട്ടി, &list_അട്ടി, tuple_അട്ടി);
	എങ്കിൽ (list_അട്ടി == ശൂന്യ)
	    നിർത്തുക;

	// take an item from the stack
	cur_l = list_അട്ടി->list;
	tempitem = list_അട്ടി;
	list_അട്ടി = list_അട്ടി->prev;
	സ്വതന്ത്രം(tempitem);
    }

    തിരികെ abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_tuple(tuple_T *tuple, പൂർണ്ണസംഖ്യ copyID)
{
    എങ്കിൽ (tuple != ശൂന്യ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	തിരികെ set_ref_in_tuple_ഇനങ്ങൾ(tuple, copyID, ശൂന്യ, ശൂന്യ);
    }
    തിരികെ FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_tuple_ഇനങ്ങൾ(
    tuple_T		*tuple,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി)
{
    പൂർണ്ണസംഖ്യ			abort = FALSE;
    tuple_T		*cur_t;
    tuple_അട്ടി_T	*tuple_അട്ടി = ശൂന്യ;
    tuple_അട്ടി_T	*tempitem;

    cur_t = tuple;
    വേണ്ടി (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cur_t->tv_ഇനങ്ങൾ.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_ഇനങ്ങൾ.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_അട്ടി, list_അട്ടി, &tuple_അട്ടി);
	}
	എങ്കിൽ (tuple_അട്ടി == ശൂന്യ)
	    നിർത്തുക;

	// take an item from the stack
	cur_t = tuple_അട്ടി->tuple;
	tempitem = tuple_അട്ടി;
	tuple_അട്ടി = tuple_അട്ടി->prev;
	സ്വതന്ത്രം(tempitem);
    }

    തിരികെ abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_callback(callback_T *cb, പൂർണ്ണസംഖ്യ copyID)
{
    typval_T tv;

    എങ്കിൽ (cb->cb_name == ശൂന്യ || *cb->cb_name == NUL || cb->cb_partial == ശൂന്യ)
	തിരികെ FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    തിരികെ set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_dict(
    dict_T		*dd,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    എങ്കിൽ (dd == ശൂന്യ || dd->dv_copyID == copyID)
	തിരികെ FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    എങ്കിൽ (ht_അട്ടി == ശൂന്യ)
	തിരികെ set_ref_in_ht(&dd->dv_hashtab, copyID, list_അട്ടി, tuple_അട്ടി);

    ht_അട്ടി_T *newitem = ALLOC_ONE(ht_അട്ടി_T);
    എങ്കിൽ (newitem == ശൂന്യ)
	തിരികെ TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_അട്ടി;
    *ht_അട്ടി = newitem;

    തിരികെ FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_list(
    list_T		*ll,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    എങ്കിൽ (ll == ശൂന്യ || ll->lv_copyID == copyID)
	തിരികെ FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    എങ്കിൽ (list_അട്ടി == ശൂന്യ)
	തിരികെ set_ref_in_list_ഇനങ്ങൾ(ll, copyID, ht_അട്ടി, tuple_അട്ടി);

    list_അട്ടി_T *newitem = ALLOC_ONE(list_അട്ടി_T);
    എങ്കിൽ (newitem == ശൂന്യ)
	തിരികെ TRUE;

    newitem->list = ll;
    newitem->prev = *list_അട്ടി;
    *list_അട്ടി = newitem;

    തിരികെ FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_tuple(
    tuple_T		*tt,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    എങ്കിൽ (tt == ശൂന്യ || tt->tv_copyID == copyID)
	തിരികെ FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    എങ്കിൽ (tuple_അട്ടി == ശൂന്യ)
	തിരികെ set_ref_in_tuple_ഇനങ്ങൾ(tt, copyID, ht_അട്ടി, list_അട്ടി);

    tuple_അട്ടി_T *newitem = ALLOC_ONE(tuple_അട്ടി_T);
    എങ്കിൽ (newitem == ശൂന്യ)
	തിരികെ TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_അട്ടി;
    *tuple_അട്ടി = newitem;

    തിരികെ FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_partial(
    partial_T		*pt,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    എങ്കിൽ (pt == ശൂന്യ || pt->pt_copyID == copyID)
	തിരികെ FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    പൂർണ്ണസംഖ്യ abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    എങ്കിൽ (pt->pt_dict != ശൂന്യ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }

    എങ്കിൽ (pt->pt_obj != ശൂന്യ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    തിരികെ abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_job(
    job_T		*job,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    typval_T    dtv;

    എങ്കിൽ (job == ശൂന്യ || job->jv_copyID == copyID)
	തിരികെ FALSE;

    job->jv_copyID = copyID;
    എങ്കിൽ (job->jv_channel != ശൂന്യ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }
    എങ്കിൽ (job->jv_exit_cb.cb_partial != ശൂന്യ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }

    തിരികെ FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_channel(
    channel_T		*ch,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    typval_T    dtv;

    എങ്കിൽ (ch == ശൂന്യ || ch->ch_copyID == copyID)
	തിരികെ FALSE;

    ch->ch_copyID = copyID;
    വേണ്ടി (ch_part_T part = PART_SOCK; part < PART_എണ്ണം; ++part)
    {
	വേണ്ടി (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != ശൂന്യ; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_മൂല്യം, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
	വേണ്ടി (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != ശൂന്യ;
		cq = cq->cq_next)
	    എങ്കിൽ (cq->cq_callback.cb_partial != ശൂന്യ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
	    }
	എങ്കിൽ (ch->ch_part[part].ch_callback.cb_partial != ശൂന്യ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
	}
    }
    എങ്കിൽ (ch->ch_callback.cb_partial != ശൂന്യ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }
    എങ്കിൽ (ch->ch_close_cb.cb_partial != ശൂന്യ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }

    തിരികെ FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_item_class(
    class_T		*cl,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;

    എങ്കിൽ (cl == ശൂന്യ || cl->class_copyID == copyID)
	തിരികെ FALSE;

    cl->class_copyID = copyID;
    എങ്കിൽ (cl->class_members_tv != ശൂന്യ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; !abort && i < cl->class_class_member_എണ്ണം; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; !abort && i < cl->class_class_function_എണ്ണം; ++i)
	abort = abort || set_ref_in_func(ശൂന്യ,
		cl->class_class_functions[i], copyID);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; !abort && i < cl->class_obj_method_എണ്ണം; ++i)
	abort = abort || set_ref_in_func(ശൂന്യ,
		cl->class_obj_methods[i], copyID);

    തിരികെ abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_item_object(
    object_T		*obj,
    പൂർണ്ണസംഖ്യ			copyID,
    ht_അട്ടി_T		**ht_അട്ടി,
    list_അട്ടി_T	**list_അട്ടി,
    tuple_അട്ടി_T	**tuple_അട്ടി)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;

    എങ്കിൽ (obj == ശൂന്യ || obj->obj_copyID == copyID)
	തിരികെ FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; !abort
	    && i < obj->obj_class->class_obj_member_എണ്ണം; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

    തിരികെ abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_item(
    typval_T	    *tv,
    പൂർണ്ണസംഖ്യ		    copyID,
    ht_അട്ടി_T	    **ht_അട്ടി,
    list_അട്ടി_T    **list_അട്ടി,
    tuple_അട്ടി_T   **tuple_അട്ടി)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_DICT:
	    തിരികെ set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

	അവസ്ഥ VAR_LIST:
	    തിരികെ set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

	അവസ്ഥ VAR_TUPLE:
	    തിരികെ set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
	അവസ്ഥ VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, ശൂന്യ, copyID);
	    നിർത്തുക;
	}

	അവസ്ഥ VAR_PARTIAL:
	    തിരികെ set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
#അല്ലെങ്കിൽ
	    നിർത്തുക;
#endif

	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);
#അല്ലെങ്കിൽ
	    നിർത്തുക;
#endif

	അവസ്ഥ VAR_CLASS:
	    തിരികെ set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

	അവസ്ഥ VAR_OBJECT:
	    തിരികെ set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_അട്ടി, list_അട്ടി, tuple_അട്ടി);

	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_BLOB:
	അവസ്ഥ VAR_TYPEALIAS:
	അവസ്ഥ VAR_INSTR:
	    // Types that do not contain any other item
	    നിർത്തുക;
    }

    തിരികെ abort;
}

#endif
