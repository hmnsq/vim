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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ਸਥਿਰ ਪੂਰਨ current_copyID = 0;

ਸਥਿਰ ਪੂਰਨ free_unref_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    ਪੂਰਨ
get_copyID(ਖਾਲੀ)
{
    current_copyID += COPYID_INC;
    ਵਾਪਸ current_copyID;
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
    ਪੂਰਨ
garbage_collect(ਪੂਰਨ testing)
{
    ਪੂਰਨ		copyID;
    ਪੂਰਨ		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    ਪੂਰਨ		did_free = FALSE;
    tabpage_T	*tp;

    ਜੇ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    ਜੇ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	ਪੂਰਨ	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	ਜੇ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	ਜੇ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_ਅੰਕੜੇ, new_len);
	    ਜੇ (pp == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_ਅੰਕੜੇ = pp;
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
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਪਿਛਲਾ_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&buf->b_bufvar.di_tv, copyID,
							ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&wp->w_winvar.di_tv, copyID,
							ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    // window-local variables in autocmd windows
    ਲਈ (ਪੂਰਨ i = 0; i < AUCMD_WIN_ਗਿਣਤੀ; ++i)
	ਜੇ (aucmd_win[i].auc_win != ਨਿਰਾ)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&wp->w_winvar.di_tv, copyID,
								  ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
		abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&wp->w_winvar.di_tv, copyID,
								  ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tp->tp_winvar.di_tv, copyID,
								  ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_call_ਢੇਰ(copyID);

    // named functions (matters for closures)
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_func_ਦਲੀਲਾਂ(copyID);

    // funcstacks keep variables for closures
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_findfunc(copyID);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_lua(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_python(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_python3(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_channel(copyID);
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਕੰਮ(copyID);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_nb_channel(copyID);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_socketserver_channel(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_timer(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_quickfix(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਪਦ(copyID);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_popups(copyID);
#ਜੇ_ਅੰਤ

    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_classes(copyID);

    ਜੇ (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_ਚੀਜ਼ਾਂ(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_verbose > 0)
    {
	verb_ਸੁਨੇਹਾ(_("Not enough memory to set references, garbage collection aborted!"));
    }

    ਵਾਪਸ did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ਸਥਿਰ ਪੂਰਨ
free_unref_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ = TRUE;

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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_ਅਣਵਰਤਿਆ_jobs_contents(copyID, COPYID_ਨਕਾਬ);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_ਅਣਵਰਤਿਆ_channels_contents(copyID, COPYID_ਨਕਾਬ);
#ਜੇ_ਅੰਤ

    /*
     * PASS 2: free the items themselves.
     */
    object_free_ਚੀਜ਼ਾਂ(copyID);
    dict_free_ਚੀਜ਼ਾਂ(copyID);
    list_free_ਚੀਜ਼ਾਂ(copyID);
    tuple_free_ਚੀਜ਼ਾਂ(copyID);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_ਅਣਵਰਤਿਆ_jobs(copyID, COPYID_ਨਕਾਬ);

    // Go through the list of channels and free items without the copyID.
    free_ਅਣਵਰਤਿਆ_channels(copyID, COPYID_ਨਕਾਬ);
#ਜੇ_ਅੰਤ

    ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ = FALSE;

    ਵਾਪਸ did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ht(
    hashtab_T		*ht,
    ਪੂਰਨ			copyID,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਪੂਰਨ		todo;
    ਪੂਰਨ		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ਢੇਰ_T	*ht_ਢੇਰ = ਨਿਰਾ;
    ht_ਢੇਰ_T	*tempitem;

    cur_ht = ht;
    ਲਈ (;;)
    {
	ਜੇ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (ਪੂਰਨ)cur_ht->ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(cur_ht, hi, todo)
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    --todo;
		    abort = abort
			|| ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&HI2DI(hi)->di_tv, copyID,
				       &ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
		}
	}

	ਜੇ (ht_ਢੇਰ == ਨਿਰਾ)
	    ਰੋਕੋ;

	// take an item from the stack
	cur_ht = ht_ਢੇਰ->ht;
	tempitem = ht_ਢੇਰ;
	ht_ਢੇਰ = ht_ਢੇਰ->prev;
	ਮੁਕਤ(tempitem);
    }

    ਵਾਪਸ abort;
}

#ਜੇ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_dict(dict_T *d, ਪੂਰਨ copyID)
{
    ਜੇ (d != ਨਿਰਾ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ht(&d->dv_hashtab, copyID, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_list(list_T *ll, ਪੂਰਨ copyID)
{
    ਜੇ (ll != ਨਿਰਾ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(ll, copyID, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(
    list_T		*l,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    listitem_T	 *li;
    ਪੂਰਨ		 abort = FALSE;
    list_T	 *cur_l;
    list_ਢੇਰ_T *list_ਢੇਰ = ਨਿਰਾ;
    list_ਢੇਰ_T *tempitem;

    cur_l = l;
    ਲਈ (;;)
    {
	ਜੇ (!abort && cur_l->lv_ਪਹਿਲਾ != &range_list_ਵਸਤੂ)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    ਲਈ (li = cur_l->lv_ਪਹਿਲਾ; !abort && li != ਨਿਰਾ; li = li->li_ਅਗਲਾ)
		abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&li->li_tv, copyID,
				       ht_ਢੇਰ, &list_ਢੇਰ, tuple_ਢੇਰ);
	ਜੇ (list_ਢੇਰ == ਨਿਰਾ)
	    ਰੋਕੋ;

	// take an item from the stack
	cur_l = list_ਢੇਰ->list;
	tempitem = list_ਢੇਰ;
	list_ਢੇਰ = list_ਢੇਰ->prev;
	ਮੁਕਤ(tempitem);
    }

    ਵਾਪਸ abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_tuple(tuple_T *tuple, ਪੂਰਨ copyID)
{
    ਜੇ (tuple != ਨਿਰਾ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_tuple_ਚੀਜ਼ਾਂ(tuple, copyID, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_tuple_ਚੀਜ਼ਾਂ(
    tuple_T		*tuple,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ)
{
    ਪੂਰਨ			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ਢੇਰ_T	*tuple_ਢੇਰ = ਨਿਰਾ;
    tuple_ਢੇਰ_T	*tempitem;

    cur_t = tuple;
    ਲਈ (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	ਲਈ (ਪੂਰਨ i = 0; i < cur_t->tv_ਚੀਜ਼ਾਂ.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_ਚੀਜ਼ਾਂ.ga_ਅੰਕੜੇ) + i;
	    abort = abort
		|| ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(tv, copyID,
			ht_ਢੇਰ, list_ਢੇਰ, &tuple_ਢੇਰ);
	}
	ਜੇ (tuple_ਢੇਰ == ਨਿਰਾ)
	    ਰੋਕੋ;

	// take an item from the stack
	cur_t = tuple_ਢੇਰ->tuple;
	tempitem = tuple_ਢੇਰ;
	tuple_ਢੇਰ = tuple_ਢੇਰ->prev;
	ਮੁਕਤ(tempitem);
    }

    ਵਾਪਸ abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_callback(callback_T *cb, ਪੂਰਨ copyID)
{
    typval_T tv;

    ਜੇ (cb->cb_name == ਨਿਰਾ || *cb->cb_name == NUL || cb->cb_partial == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_dict(
    dict_T		*dd,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਜੇ (dd == ਨਿਰਾ || dd->dv_copyID == copyID)
	ਵਾਪਸ FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    ਜੇ (ht_ਢੇਰ == ਨਿਰਾ)
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ht(&dd->dv_hashtab, copyID, list_ਢੇਰ, tuple_ਢੇਰ);

    ht_ਢੇਰ_T *newitem = ALLOC_ONE(ht_ਢੇਰ_T);
    ਜੇ (newitem == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ਢੇਰ;
    *ht_ਢੇਰ = newitem;

    ਵਾਪਸ FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_list(
    list_T		*ll,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਜੇ (ll == ਨਿਰਾ || ll->lv_copyID == copyID)
	ਵਾਪਸ FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    ਜੇ (list_ਢੇਰ == ਨਿਰਾ)
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(ll, copyID, ht_ਢੇਰ, tuple_ਢੇਰ);

    list_ਢੇਰ_T *newitem = ALLOC_ONE(list_ਢੇਰ_T);
    ਜੇ (newitem == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    newitem->list = ll;
    newitem->prev = *list_ਢੇਰ;
    *list_ਢੇਰ = newitem;

    ਵਾਪਸ FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_tuple(
    tuple_T		*tt,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਜੇ (tt == ਨਿਰਾ || tt->tv_copyID == copyID)
	ਵਾਪਸ FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    ਜੇ (tuple_ਢੇਰ == ਨਿਰਾ)
	ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_tuple_ਚੀਜ਼ਾਂ(tt, copyID, ht_ਢੇਰ, list_ਢੇਰ);

    tuple_ਢੇਰ_T *newitem = ALLOC_ONE(tuple_ਢੇਰ_T);
    ਜੇ (newitem == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ਢੇਰ;
    *tuple_ਢੇਰ = newitem;

    ਵਾਪਸ FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_partial(
    partial_T		*pt,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਜੇ (pt == ਨਿਰਾ || pt->pt_copyID == copyID)
	ਵਾਪਸ FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    ਪੂਰਨ abort = ਸੈੱਟ_ref_ਵਿੱਚ_func(pt->pt_name, pt->pt_func, copyID);

    ਜੇ (pt->pt_dict != ਨਿਰਾ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }

    ਜੇ (pt->pt_obj != ਨਿਰਾ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&objtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }

    ਲਈ (ਪੂਰਨ i = 0; i < pt->pt_argc; ++i)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&pt->pt_argv[i], copyID,
		ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    ਵਾਪਸ abort;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_ਕੰਮ(
    ਕੰਮ_T		*ਕੰਮ,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    typval_T    dtv;

    ਜੇ (ਕੰਮ == ਨਿਰਾ || ਕੰਮ->jv_copyID == copyID)
	ਵਾਪਸ FALSE;

    ਕੰਮ->jv_copyID = copyID;
    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = ਕੰਮ->jv_channel;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }
    ਜੇ (ਕੰਮ->jv_exit_cb.cb_partial != ਨਿਰਾ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ਕੰਮ->jv_exit_cb.cb_partial;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }

    ਵਾਪਸ FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_channel(
    channel_T		*ch,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    typval_T    dtv;

    ਜੇ (ch == ਨਿਰਾ || ch->ch_copyID == copyID)
	ਵਾਪਸ FALSE;

    ch->ch_copyID = copyID;
    ਲਈ (ch_part_T part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
    {
	ਲਈ (jsonq_T *jq = ch->ch_part[part].ch_json_ਸਿਰ.jq_ਅਗਲਾ;
		jq != ਨਿਰਾ; jq = jq->jq_ਅਗਲਾ)
	    ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(jq->jq_ਮੁੱਲ, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
	ਲਈ (cbq_T *cq = ch->ch_part[part].ch_cb_ਸਿਰ.cq_ਅਗਲਾ; cq != ਨਿਰਾ;
		cq = cq->cq_ਅਗਲਾ)
	    ਜੇ (cq->cq_callback.cb_partial != ਨਿਰਾ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
	    }
	ਜੇ (ch->ch_part[part].ch_callback.cb_partial != ਨਿਰਾ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
	}
    }
    ਜੇ (ch->ch_callback.cb_partial != ਨਿਰਾ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }
    ਜੇ (ch->ch_close_cb.cb_partial != ਨਿਰਾ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&dtv, copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }

    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_class(
    class_T		*cl,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਪੂਰਨ abort = FALSE;

    ਜੇ (cl == ਨਿਰਾ || cl->class_copyID == copyID)
	ਵਾਪਸ FALSE;

    cl->class_copyID = copyID;
    ਜੇ (cl->class_members_tv != ਨਿਰਾ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	ਲਈ (ਪੂਰਨ i = 0; !abort && i < cl->class_class_member_ਗਿਣਤੀ; ++i)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(
		    &cl->class_members_tv[i],
		    copyID, ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
    }

    ਲਈ (ਪੂਰਨ i = 0; !abort && i < cl->class_class_function_ਗਿਣਤੀ; ++i)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_func(ਨਿਰਾ,
		cl->class_class_functions[i], copyID);

    ਲਈ (ਪੂਰਨ i = 0; !abort && i < cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ; ++i)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_func(ਨਿਰਾ,
		cl->class_obj_methods[i], copyID);

    ਵਾਪਸ abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_object(
    object_T		*obj,
    ਪੂਰਨ			copyID,
    ht_ਢੇਰ_T		**ht_ਢੇਰ,
    list_ਢੇਰ_T	**list_ਢੇਰ,
    tuple_ਢੇਰ_T	**tuple_ਢੇਰ)
{
    ਪੂਰਨ abort = FALSE;

    ਜੇ (obj == ਨਿਰਾ || obj->obj_copyID == copyID)
	ਵਾਪਸ FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    ਲਈ (ਪੂਰਨ i = 0; !abort
	    && i < obj->obj_class->class_obj_member_ਗਿਣਤੀ; ++i)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(mtv + i, copyID,
		ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

    ਵਾਪਸ abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(
    typval_T	    *tv,
    ਪੂਰਨ		    copyID,
    ht_ਢੇਰ_T	    **ht_ਢੇਰ,
    list_ਢੇਰ_T    **list_ਢੇਰ,
    tuple_ਢੇਰ_T   **tuple_ਢੇਰ)
{
    ਪੂਰਨ		abort = FALSE;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_dict(tv->vval.v_dict, copyID,
					 ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

	ਹਾਲਤ VAR_LIST:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_list(tv->vval.v_list, copyID,
					 ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

	ਹਾਲਤ VAR_TUPLE:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_tuple(tv->vval.v_tuple, copyID,
					 ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
	ਹਾਲਤ VAR_FUNC:
	{
	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_func(tv->vval.v_string, ਨਿਰਾ, copyID);
	    ਰੋਕੋ;
	}

	ਹਾਲਤ VAR_PARTIAL:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_partial(tv->vval.v_partial, copyID,
					ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_ਕੰਮ(tv->vval.v_ਕੰਮ, copyID,
					ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
#ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_channel(tv->vval.v_channel, copyID,
					ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);
#ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ VAR_CLASS:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_class(tv->vval.v_class, copyID,
					ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

	ਹਾਲਤ VAR_OBJECT:
	    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_object(tv->vval.v_object, copyID,
					ht_ਢੇਰ, list_ਢੇਰ, tuple_ਢੇਰ);

	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_BLOB:
	ਹਾਲਤ VAR_TYPEALIAS:
	ਹਾਲਤ VAR_INSTR:
	    // Types that do not contain any other item
	    ਰੋਕੋ;
    }

    ਵਾਪਸ abort;
}

#ਜੇ_ਅੰਤ
