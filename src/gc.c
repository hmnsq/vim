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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_copyID = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ free_unref_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_copyID(ಶೂನ್ಯ)
{
    current_copyID += COPYID_INC;
    ಪ್ರತ್ಯಾಗಮ current_copyID;
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
    ಪೂರ್ಣಾಙ್ಕ
garbage_collect(ಪೂರ್ಣಾಙ್ಕ testing)
{
    ಪೂರ್ಣಾಙ್ಕ		copyID;
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;
    tabpage_T	*tp;

    ಯದಿ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    ಯದಿ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	ಪೂರ್ಣಾಙ್ಕ	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	ಯದಿ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	ಯದಿ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_data, new_len);
	    ಯದಿ (pp == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
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
							ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	abort = abort || set_ref_in_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
							ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    // window-local variables in autocmd windows
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಯದಿ (aucmd_win[i].auc_win != ರಿಕ್ತ)
	    abort = abort || set_ref_in_item(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		abort = abort || set_ref_in_item(&wp->w_winvar.di_tv, copyID,
								  ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#endif

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || set_ref_in_item(&tp->tp_winvar.di_tv, copyID,
								  ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || set_ref_in_call_ರಾಶಿ(copyID);

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

    ಯದಿ (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_ವಸ್ತುಗಳು(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    ಅನ್ಯಥಾ ಯದಿ (p_verbose > 0)
    {
	verb_ಸಂದೇಶ(_("Not enough memory to set references, garbage collection aborted!"));
    }

    ಪ್ರತ್ಯಾಗಮ did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
free_unref_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    in_free_unref_ವಸ್ತುಗಳು = TRUE;

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
    object_free_ವಸ್ತುಗಳು(copyID);
    dict_free_ವಸ್ತುಗಳು(copyID);
    list_free_ವಸ್ತುಗಳು(copyID);
    tuple_free_ವಸ್ತುಗಳು(copyID);

#ifdef FEAT_JOB_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_unused_jobs(copyID, COPYID_MASK);

    // Go through the list of channels and free items without the copyID.
    free_unused_channels(copyID, COPYID_MASK);
#endif

    in_free_unref_ವಸ್ತುಗಳು = FALSE;

    ಪ್ರತ್ಯಾಗಮ did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_ht(
    hashtab_T		*ht,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ರಾಶಿ_T	*ht_ರಾಶಿ = ರಿಕ್ತ;
    ht_ರಾಶಿ_T	*tempitem;

    cur_ht = ht;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (ಪೂರ್ಣಾಙ್ಕ)cur_ht->ht_used;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(cur_ht, hi, todo)
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    abort = abort
			|| set_ref_in_item(&HI2DI(hi)->di_tv, copyID,
				       &ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
		}
	}

	ಯದಿ (ht_ರಾಶಿ == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// take an item from the stack
	cur_ht = ht_ರಾಶಿ->ht;
	tempitem = ht_ರಾಶಿ;
	ht_ರಾಶಿ = ht_ರಾಶಿ->prev;
	ಮುಕ್ತ(tempitem);
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

#ಯದಿ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_dict(dict_T *d, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಯದಿ (d != ರಿಕ್ತ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	ಪ್ರತ್ಯಾಗಮ set_ref_in_ht(&d->dv_hashtab, copyID, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_list(list_T *ll, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಯದಿ (ll != ರಿಕ್ತ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	ಪ್ರತ್ಯಾಗಮ set_ref_in_list_ವಸ್ತುಗಳು(ll, copyID, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_list_ವಸ್ತುಗಳು(
    list_T		*l,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    listitem_T	 *li;
    ಪೂರ್ಣಾಙ್ಕ		 abort = FALSE;
    list_T	 *cur_l;
    list_ರಾಶಿ_T *list_ರಾಶಿ = ರಿಕ್ತ;
    list_ರಾಶಿ_T *tempitem;

    cur_l = l;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (!abort && cur_l->lv_first != &range_list_item)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    ಪ್ರತಿ (li = cur_l->lv_first; !abort && li != ರಿಕ್ತ; li = li->li_next)
		abort = abort || set_ref_in_item(&li->li_tv, copyID,
				       ht_ರಾಶಿ, &list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಯದಿ (list_ರಾಶಿ == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// take an item from the stack
	cur_l = list_ರಾಶಿ->list;
	tempitem = list_ರಾಶಿ;
	list_ರಾಶಿ = list_ರಾಶಿ->prev;
	ಮುಕ್ತ(tempitem);
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_tuple(tuple_T *tuple, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಯದಿ (tuple != ರಿಕ್ತ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	ಪ್ರತ್ಯಾಗಮ set_ref_in_tuple_ವಸ್ತುಗಳು(tuple, copyID, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_tuple_ವಸ್ತುಗಳು(
    tuple_T		*tuple,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ)
{
    ಪೂರ್ಣಾಙ್ಕ			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ರಾಶಿ_T	*tuple_ರಾಶಿ = ರಿಕ್ತ;
    tuple_ರಾಶಿ_T	*tempitem;

    cur_t = tuple;
    ಪ್ರತಿ (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cur_t->tv_ವಸ್ತುಗಳು.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_ವಸ್ತುಗಳು.ga_data) + i;
	    abort = abort
		|| set_ref_in_item(tv, copyID,
			ht_ರಾಶಿ, list_ರಾಶಿ, &tuple_ರಾಶಿ);
	}
	ಯದಿ (tuple_ರಾಶಿ == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// take an item from the stack
	cur_t = tuple_ರಾಶಿ->tuple;
	tempitem = tuple_ರಾಶಿ;
	tuple_ರಾಶಿ = tuple_ರಾಶಿ->prev;
	ಮುಕ್ತ(tempitem);
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_callback(callback_T *cb, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    typval_T tv;

    ಯದಿ (cb->cb_name == ರಿಕ್ತ || *cb->cb_name == NUL || cb->cb_partial == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    ಪ್ರತ್ಯಾಗಮ set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_dict(
    dict_T		*dd,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಯದಿ (dd == ರಿಕ್ತ || dd->dv_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    ಯದಿ (ht_ರಾಶಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ set_ref_in_ht(&dd->dv_hashtab, copyID, list_ರಾಶಿ, tuple_ರಾಶಿ);

    ht_ರಾಶಿ_T *newitem = ALLOC_ONE(ht_ರಾಶಿ_T);
    ಯದಿ (newitem == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ರಾಶಿ;
    *ht_ರಾಶಿ = newitem;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_list(
    list_T		*ll,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಯದಿ (ll == ರಿಕ್ತ || ll->lv_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    ಯದಿ (list_ರಾಶಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ set_ref_in_list_ವಸ್ತುಗಳು(ll, copyID, ht_ರಾಶಿ, tuple_ರಾಶಿ);

    list_ರಾಶಿ_T *newitem = ALLOC_ONE(list_ರಾಶಿ_T);
    ಯದಿ (newitem == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    newitem->list = ll;
    newitem->prev = *list_ರಾಶಿ;
    *list_ರಾಶಿ = newitem;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_tuple(
    tuple_T		*tt,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಯದಿ (tt == ರಿಕ್ತ || tt->tv_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    ಯದಿ (tuple_ರಾಶಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ set_ref_in_tuple_ವಸ್ತುಗಳು(tt, copyID, ht_ರಾಶಿ, list_ರಾಶಿ);

    tuple_ರಾಶಿ_T *newitem = ALLOC_ONE(tuple_ರಾಶಿ_T);
    ಯದಿ (newitem == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ರಾಶಿ;
    *tuple_ರಾಶಿ = newitem;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_partial(
    partial_T		*pt,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಯದಿ (pt == ರಿಕ್ತ || pt->pt_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    ಪೂರ್ಣಾಙ್ಕ abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

    ಯದಿ (pt->pt_dict != ರಿಕ್ತ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಯದಿ (pt->pt_obj != ರಿಕ್ತ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	set_ref_in_item(&objtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pt->pt_argc; ++i)
	abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
		ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    ಪ್ರತ್ಯಾಗಮ abort;
}

#ifdef FEAT_JOB_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_job(
    job_T		*job,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    typval_T    dtv;

    ಯದಿ (job == ರಿಕ್ತ || job->jv_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    job->jv_copyID = copyID;
    ಯದಿ (job->jv_channel != ರಿಕ್ತ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = job->jv_channel;
	set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }
    ಯದಿ (job->jv_exit_cb.cb_partial != ರಿಕ್ತ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_channel(
    channel_T		*ch,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    typval_T    dtv;

    ಯದಿ (ch == ರಿಕ್ತ || ch->ch_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ch->ch_copyID = copyID;
    ಪ್ರತಿ (ch_part_T part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
    {
	ಪ್ರತಿ (jsonq_T *jq = ch->ch_part[part].ch_json_head.jq_next;
		jq != ರಿಕ್ತ; jq = jq->jq_next)
	    set_ref_in_item(jq->jq_ಮೌಲ್ಯ, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಪ್ರತಿ (cbq_T *cq = ch->ch_part[part].ch_cb_head.cq_next; cq != ರಿಕ್ತ;
		cq = cq->cq_next)
	    ಯದಿ (cq->cq_callback.cb_partial != ರಿಕ್ತ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	    }
	ಯದಿ (ch->ch_part[part].ch_callback.cb_partial != ರಿಕ್ತ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	}
    }
    ಯದಿ (ch->ch_callback.cb_partial != ರಿಕ್ತ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }
    ಯದಿ (ch->ch_close_cb.cb_partial != ರಿಕ್ತ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	set_ref_in_item(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_class(
    class_T		*cl,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಙ್ಕ abort = FALSE;

    ಯದಿ (cl == ರಿಕ್ತ || cl->class_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    cl->class_copyID = copyID;
    ಯದಿ (cl->class_members_tv != ರಿಕ್ತ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; !abort && i < cl->class_class_member_ಎಣಿಕೆ; ++i)
	    abort = abort || set_ref_in_item(
		    &cl->class_members_tv[i],
		    copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; !abort && i < cl->class_class_function_ಎಣಿಕೆ; ++i)
	abort = abort || set_ref_in_func(ರಿಕ್ತ,
		cl->class_class_functions[i], copyID);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; !abort && i < cl->class_obj_method_ಎಣಿಕೆ; ++i)
	abort = abort || set_ref_in_func(ರಿಕ್ತ,
		cl->class_obj_methods[i], copyID);

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item_object(
    object_T		*obj,
    ಪೂರ್ಣಾಙ್ಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಙ್ಕ abort = FALSE;

    ಯದಿ (obj == ರಿಕ್ತ || obj->obj_copyID == copyID)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; !abort
	    && i < obj->obj_class->class_obj_member_ಎಣಿಕೆ; ++i)
	abort = abort || set_ref_in_item(mtv + i, copyID,
		ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_item(
    typval_T	    *tv,
    ಪೂರ್ಣಾಙ್ಕ		    copyID,
    ht_ರಾಶಿ_T	    **ht_ರಾಶಿ,
    list_ರಾಶಿ_T    **list_ರಾಶಿ,
    tuple_ರಾಶಿ_T   **tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_dict(tv->vval.v_dict, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಪಕ್ಷ VAR_LIST:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_list(tv->vval.v_list, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಪಕ್ಷ VAR_TUPLE:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_tuple(tv->vval.v_tuple, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಪಕ್ಷ VAR_FUNC:
	{
	    abort = set_ref_in_func(tv->vval.v_string, ರಿಕ್ತ, copyID);
	    ಭಙ್ಗ;
	}

	ಪಕ್ಷ VAR_PARTIAL:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_partial(tv->vval.v_partial, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_job(tv->vval.v_job, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
#ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_channel(tv->vval.v_channel, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
#ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ VAR_CLASS:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_class(tv->vval.v_class, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಪಕ್ಷ VAR_OBJECT:
	    ಪ್ರತ್ಯಾಗಮ set_ref_in_item_object(tv->vval.v_object, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_BLOB:
	ಪಕ್ಷ VAR_TYPEALIAS:
	ಪಕ್ಷ VAR_INSTR:
	    // Types that do not contain any other item
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

#endif
