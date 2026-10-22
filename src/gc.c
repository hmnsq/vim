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

#ಆದರೆ defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_copyID = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ free_unref_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    ಪೂರ್ಣಾಂಕ
get_copyID(ಶೂನ್ಯ)
{
    current_copyID += COPYID_INC;
    ಹಿಂತಿರುಗಿ current_copyID;
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
    ಪೂರ್ಣಾಂಕ
garbage_collect(ಪೂರ್ಣಾಂಕ testing)
{
    ಪೂರ್ಣಾಂಕ		copyID;
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;
    tabpage_T	*tp;

    ಆದರೆ (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    ಆದರೆ (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	ಪೂರ್ಣಾಂಕ	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	ಆದರೆ (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	ಆದರೆ (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_ದತ್ತಾಂಶ, new_len);
	    ಆದರೆ (pp == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_ದತ್ತಾಂಶ = pp;
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
    abort = abort || ಗಣ_ref_ಒಳ_ಹಿಂದಿನ_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&buf->b_bufvar.di_tv, copyID,
							ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	abort = abort || ಗಣ_ref_ಒಳ_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&wp->w_winvar.di_tv, copyID,
							ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    // window-local variables in autocmd windows
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಆದರೆ (aucmd_win[i].auc_win != ಖಾಲಿ)
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&wp->w_winvar.di_tv, copyID,
								  ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
		abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&wp->w_winvar.di_tv, copyID,
								  ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tp->tp_winvar.di_tv, copyID,
								  ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || ಗಣ_ref_ಒಳ_call_ರಾಶಿ(copyID);

    // named functions (matters for closures)
    abort = abort || ಗಣ_ref_ಒಳ_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || ಗಣ_ref_ಒಳ_func_ವಾದಗಳು(copyID);

    // funcstacks keep variables for closures
    abort = abort || ಗಣ_ref_ಒಳ_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || ಗಣ_ref_ಒಳ_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || ಗಣ_ref_ಒಳ_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || ಗಣ_ref_ಒಳ_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || ಗಣ_ref_ಒಳ_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || ಗಣ_ref_ಒಳ_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || ಗಣ_ref_ಒಳ_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || ಗಣ_ref_ಒಳ_findfunc(copyID);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    abort = abort || ಗಣ_ref_ಒಳ_lua(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    abort = abort || ಗಣ_ref_ಒಳ_python(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    abort = abort || ಗಣ_ref_ಒಳ_python3(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    abort = abort || ಗಣ_ref_ಒಳ_channel(copyID);
    abort = abort || ಗಣ_ref_ಒಳ_ಕೆಲಸ(copyID);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    abort = abort || ಗಣ_ref_ಒಳ_nb_channel(copyID);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
    abort = abort || ಗಣ_ref_ಒಳ_socketserver_channel(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    abort = abort || ಗಣ_ref_ಒಳ_timer(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    abort = abort || ಗಣ_ref_ಒಳ_quickfix(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    abort = abort || ಗಣ_ref_ಒಳ_ಪದ(copyID);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    abort = abort || ಗಣ_ref_ಒಳ_popups(copyID);
#ಆದರೆ_ಅಂತ್ಯ

    abort = abort || ಗಣ_ref_ಒಳ_classes(copyID);

    ಆದರೆ (!abort)
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose > 0)
    {
	verb_ಸಂದೇಶ(_("Not enough memory to set references, garbage collection aborted!"));
    }

    ಹಿಂತಿರುಗಿ did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
free_unref_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    ಒಳ_free_unref_ವಸ್ತುಗಳು = TRUE;

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_ಬಳಸದ_jobs_contents(copyID, COPYID_ಮುಖವಾಡ);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_ಬಳಸದ_channels_contents(copyID, COPYID_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * PASS 2: free the items themselves.
     */
    object_free_ವಸ್ತುಗಳು(copyID);
    dict_free_ವಸ್ತುಗಳು(copyID);
    list_free_ವಸ್ತುಗಳು(copyID);
    tuple_free_ವಸ್ತುಗಳು(copyID);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_ಬಳಸದ_jobs(copyID, COPYID_ಮುಖವಾಡ);

    // Go through the list of channels and free items without the copyID.
    free_ಬಳಸದ_channels(copyID, COPYID_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ

    ಒಳ_free_unref_ವಸ್ತುಗಳು = FALSE;

    ಹಿಂತಿರುಗಿ did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ht(
    hashtab_T		*ht,
    ಪೂರ್ಣಾಂಕ			copyID,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ರಾಶಿ_T	*ht_ರಾಶಿ = ಖಾಲಿ;
    ht_ರಾಶಿ_T	*tempitem;

    cur_ht = ht;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (ಪೂರ್ಣಾಂಕ)cur_ht->ht_ಬಳಸಿದ;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(cur_ht, hi, todo)
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    --todo;
		    abort = abort
			|| ಗಣ_ref_ಒಳ_ಅಂಶ(&HI2DI(hi)->di_tv, copyID,
				       &ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
		}
	}

	ಆದರೆ (ht_ರಾಶಿ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// take an item from the stack
	cur_ht = ht_ರಾಶಿ->ht;
	tempitem = ht_ರಾಶಿ;
	ht_ರಾಶಿ = ht_ರಾಶಿ->prev;
	ಬಿಡುಗಡೆ(tempitem);
    }

    ಹಿಂತಿರುಗಿ abort;
}

#ಆದರೆ defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_dict(dict_T *d, ಪೂರ್ಣಾಂಕ copyID)
{
    ಆದರೆ (d != ಖಾಲಿ && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ht(&d->dv_hashtab, copyID, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_list(list_T *ll, ಪೂರ್ಣಾಂಕ copyID)
{
    ಆದರೆ (ll != ಖಾಲಿ && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(ll, copyID, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(
    list_T		*l,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    listitem_T	 *li;
    ಪೂರ್ಣಾಂಕ		 abort = FALSE;
    list_T	 *cur_l;
    list_ರಾಶಿ_T *list_ರಾಶಿ = ಖಾಲಿ;
    list_ರಾಶಿ_T *tempitem;

    cur_l = l;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (!abort && cur_l->lv_ಮೊದಲ != &range_list_ಅಂಶ)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    ಗಾಗಿ (li = cur_l->lv_ಮೊದಲ; !abort && li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ)
		abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&li->li_tv, copyID,
				       ht_ರಾಶಿ, &list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಆದರೆ (list_ರಾಶಿ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// take an item from the stack
	cur_l = list_ರಾಶಿ->list;
	tempitem = list_ರಾಶಿ;
	list_ರಾಶಿ = list_ರಾಶಿ->prev;
	ಬಿಡುಗಡೆ(tempitem);
    }

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_tuple(tuple_T *tuple, ಪೂರ್ಣಾಂಕ copyID)
{
    ಆದರೆ (tuple != ಖಾಲಿ && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_tuple_ವಸ್ತುಗಳು(tuple, copyID, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_tuple_ವಸ್ತುಗಳು(
    tuple_T		*tuple,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ)
{
    ಪೂರ್ಣಾಂಕ			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ರಾಶಿ_T	*tuple_ರಾಶಿ = ಖಾಲಿ;
    tuple_ರಾಶಿ_T	*tempitem;

    cur_t = tuple;
    ಗಾಗಿ (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cur_t->tv_ವಸ್ತುಗಳು.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_ವಸ್ತುಗಳು.ga_ದತ್ತಾಂಶ) + i;
	    abort = abort
		|| ಗಣ_ref_ಒಳ_ಅಂಶ(tv, copyID,
			ht_ರಾಶಿ, list_ರಾಶಿ, &tuple_ರಾಶಿ);
	}
	ಆದರೆ (tuple_ರಾಶಿ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// take an item from the stack
	cur_t = tuple_ರಾಶಿ->tuple;
	tempitem = tuple_ರಾಶಿ;
	tuple_ರಾಶಿ = tuple_ರಾಶಿ->prev;
	ಬಿಡುಗಡೆ(tempitem);
    }

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_callback(callback_T *cb, ಪೂರ್ಣಾಂಕ copyID)
{
    typval_T tv;

    ಆದರೆ (cb->cb_name == ಖಾಲಿ || *cb->cb_name == NUL || cb->cb_partial == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_dict(
    dict_T		*dd,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಆದರೆ (dd == ಖಾಲಿ || dd->dv_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    ಆದರೆ (ht_ರಾಶಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ht(&dd->dv_hashtab, copyID, list_ರಾಶಿ, tuple_ರಾಶಿ);

    ht_ರಾಶಿ_T *newitem = ALLOC_ONE(ht_ರಾಶಿ_T);
    ಆದರೆ (newitem == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->prev = *ht_ರಾಶಿ;
    *ht_ರಾಶಿ = newitem;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_list(
    list_T		*ll,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಆದರೆ (ll == ಖಾಲಿ || ll->lv_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    ಆದರೆ (list_ರಾಶಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(ll, copyID, ht_ರಾಶಿ, tuple_ರಾಶಿ);

    list_ರಾಶಿ_T *newitem = ALLOC_ONE(list_ರಾಶಿ_T);
    ಆದರೆ (newitem == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    newitem->list = ll;
    newitem->prev = *list_ರಾಶಿ;
    *list_ರಾಶಿ = newitem;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_tuple(
    tuple_T		*tt,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಆದರೆ (tt == ಖಾಲಿ || tt->tv_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    ಆದರೆ (tuple_ರಾಶಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_tuple_ವಸ್ತುಗಳು(tt, copyID, ht_ರಾಶಿ, list_ರಾಶಿ);

    tuple_ರಾಶಿ_T *newitem = ALLOC_ONE(tuple_ರಾಶಿ_T);
    ಆದರೆ (newitem == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    newitem->tuple = tt;
    newitem->prev = *tuple_ರಾಶಿ;
    *tuple_ರಾಶಿ = newitem;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_partial(
    partial_T		*pt,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಆದರೆ (pt == ಖಾಲಿ || pt->pt_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    ಪೂರ್ಣಾಂಕ abort = ಗಣ_ref_ಒಳ_func(pt->pt_name, pt->pt_func, copyID);

    ಆದರೆ (pt->pt_dict != ಖಾಲಿ)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಆದರೆ (pt->pt_obj != ಖಾಲಿ)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&objtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pt->pt_argc; ++i)
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&pt->pt_argv[i], copyID,
		ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    ಹಿಂತಿರುಗಿ abort;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_ಕೆಲಸ(
    ಕೆಲಸ_T		*ಕೆಲಸ,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    typval_T    dtv;

    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ || ಕೆಲಸ->jv_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    ಕೆಲಸ->jv_copyID = copyID;
    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = ಕೆಲಸ->jv_channel;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }
    ಆದರೆ (ಕೆಲಸ->jv_exit_cb.cb_partial != ಖಾಲಿ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ಕೆಲಸ->jv_exit_cb.cb_partial;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_channel(
    channel_T		*ch,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    typval_T    dtv;

    ಆದರೆ (ch == ಖಾಲಿ || ch->ch_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    ch->ch_copyID = copyID;
    ಗಾಗಿ (ch_part_T part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
    {
	ಗಾಗಿ (jsonq_T *jq = ch->ch_part[part].ch_json_ತಲೆ.jq_ಮುಂದಿನ;
		jq != ಖಾಲಿ; jq = jq->jq_ಮುಂದಿನ)
	    ಗಣ_ref_ಒಳ_ಅಂಶ(jq->jq_ಮೌಲ್ಯ, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಗಾಗಿ (cbq_T *cq = ch->ch_part[part].ch_cb_ತಲೆ.cq_ಮುಂದಿನ; cq != ಖಾಲಿ;
		cq = cq->cq_ಮುಂದಿನ)
	    ಆದರೆ (cq->cq_callback.cb_partial != ಖಾಲಿ)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	    }
	ಆದರೆ (ch->ch_part[part].ch_callback.cb_partial != ಖಾಲಿ)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	}
    }
    ಆದರೆ (ch->ch_callback.cb_partial != ಖಾಲಿ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }
    ಆದರೆ (ch->ch_close_cb.cb_partial != ಖಾಲಿ)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	ಗಣ_ref_ಒಳ_ಅಂಶ(&dtv, copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_class(
    class_T		*cl,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;

    ಆದರೆ (cl == ಖಾಲಿ || cl->class_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    cl->class_copyID = copyID;
    ಆದರೆ (cl->class_members_tv != ಖಾಲಿ)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; !abort && i < cl->class_class_member_ಎಣಿಕೆ; ++i)
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(
		    &cl->class_members_tv[i],
		    copyID, ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; !abort && i < cl->class_class_function_ಎಣಿಕೆ; ++i)
	abort = abort || ಗಣ_ref_ಒಳ_func(ಖಾಲಿ,
		cl->class_class_functions[i], copyID);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; !abort && i < cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ; ++i)
	abort = abort || ಗಣ_ref_ಒಳ_func(ಖಾಲಿ,
		cl->class_obj_methods[i], copyID);

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ_object(
    object_T		*obj,
    ಪೂರ್ಣಾಂಕ			copyID,
    ht_ರಾಶಿ_T		**ht_ರಾಶಿ,
    list_ರಾಶಿ_T	**list_ರಾಶಿ,
    tuple_ರಾಶಿ_T	**tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;

    ಆದರೆ (obj == ಖಾಲಿ || obj->obj_copyID == copyID)
	ಹಿಂತಿರುಗಿ FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; !abort
	    && i < obj->obj_class->class_obj_member_ಎಣಿಕೆ; ++i)
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(mtv + i, copyID,
		ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಅಂಶ(
    typval_T	    *tv,
    ಪೂರ್ಣಾಂಕ		    copyID,
    ht_ರಾಶಿ_T	    **ht_ರಾಶಿ,
    list_ರಾಶಿ_T    **list_ರಾಶಿ,
    tuple_ರಾಶಿ_T   **tuple_ರಾಶಿ)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_dict(tv->vval.v_dict, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಸ್ಥಿತಿ VAR_LIST:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_list(tv->vval.v_list, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_tuple(tv->vval.v_tuple, copyID,
					 ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
	ಸ್ಥಿತಿ VAR_FUNC:
	{
	    abort = ಗಣ_ref_ಒಳ_func(tv->vval.v_string, ಖಾಲಿ, copyID);
	    ನಿಲ್ಲಿಸು;
	}

	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_partial(tv->vval.v_partial, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_ಕೆಲಸ(tv->vval.v_ಕೆಲಸ, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_channel(tv->vval.v_channel, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ VAR_CLASS:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_class(tv->vval.v_class, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ಅಂಶ_object(tv->vval.v_object, copyID,
					ht_ರಾಶಿ, list_ರಾಶಿ, tuple_ರಾಶಿ);

	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_BLOB:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	ಸ್ಥಿತಿ VAR_INSTR:
	    // Types that do not contain any other item
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ abort;
}

#ಆದರೆ_ಅಂತ್ಯ
