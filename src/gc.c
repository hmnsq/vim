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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * When recursively copying lists and dicts we need to remember which ones we
 * have done to avoid endless recursiveness.  This unique ID is used for that.
 * The last bit is used for previous_funccal, ignored when comparing.
 */
ساکن صحیح current_copyID = 0;

ساکن صحیح free_unref_اشیاء(صحیح copyID);

/*
 * Return the next (unique) copy ID.
 * Used for serializing nested structures.
 */
    صحیح
get_copyID(خالی)
{
    current_copyID += COPYID_INC;
    واپس current_copyID;
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
    صحیح
garbage_collect(صحیح testing)
{
    صحیح		copyID;
    صحیح		abort = FALSE;
    buf_T	*buf;
    win_T	*wp;
    صحیح		did_free = FALSE;
    tabpage_T	*tp;

    اگر (!testing)
    {
	// Only do this once.
	want_garbage_collect = FALSE;
	may_garbage_collect = FALSE;
	garbage_collect_at_exit = FALSE;
    }

    // The execution stack can grow big, limit the size.
    اگر (exestack.ga_maxlen - exestack.ga_len > 500)
    {
	size_t	new_len;
	char_u	*pp;
	صحیح	n;

	// Keep 150% of the current size, with a minimum of the growth size.
	n = exestack.ga_len / 2;
	اگر (n < exestack.ga_growsize)
	    n = exestack.ga_growsize;

	// Don't make it bigger though.
	اگر (exestack.ga_len + n < exestack.ga_maxlen)
	{
	    new_len = (size_t)exestack.ga_itemsize * (exestack.ga_len + n);
	    pp = vim_realloc(exestack.ga_ڈیٹا, new_len);
	    اگر (pp == تہی)
		واپس FAIL;
	    exestack.ga_maxlen = exestack.ga_len + n;
	    exestack.ga_ڈیٹا = pp;
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
    abort = abort || سیٹ_ref_اندر_پچھلا_funccal(copyID);

    // script-local variables
    abort = abort || garbage_collect_scriptvars(copyID);

    // buffer-local variables
    FOR_ALL_BUFFERS(buf)
    {
	abort = abort || سیٹ_ref_اندر_شے(&buf->b_bufvar.di_tv, copyID,
							تہی, تہی, تہی);
	abort = abort || سیٹ_ref_اندر_list(buf->b_recorded_changes, copyID);
    }

    // window-local variables
    FOR_ALL_TAB_WINDOWS(tp, wp)
	abort = abort || سیٹ_ref_اندر_شے(&wp->w_winvar.di_tv, copyID,
							تہی, تہی, تہی);
    // window-local variables in autocmd windows
    برائے (صحیح i = 0; i < AUCMD_WIN_گنتی; ++i)
	اگر (aucmd_win[i].auc_win != تہی)
	    abort = abort || سیٹ_ref_اندر_شے(
		    &aucmd_win[i].auc_win->w_winvar.di_tv, copyID, تہی, تہی, تہی);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	abort = abort || سیٹ_ref_اندر_شے(&wp->w_winvar.di_tv, copyID,
								  تہی, تہی, تہی);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
		abort = abort || سیٹ_ref_اندر_شے(&wp->w_winvar.di_tv, copyID,
								  تہی, تہی, تہی);
#اگر_ختم

    // tabpage-local variables
    FOR_ALL_TABPAGES(tp)
	abort = abort || سیٹ_ref_اندر_شے(&tp->tp_winvar.di_tv, copyID,
								  تہی, تہی, تہی);
    // global variables
    abort = abort || garbage_collect_globvars(copyID);

    // function-local variables
    abort = abort || سیٹ_ref_اندر_call_ڈھیر(copyID);

    // named functions (matters for closures)
    abort = abort || سیٹ_ref_اندر_functions(copyID);

    // function call arguments, if v:testing is set.
    abort = abort || سیٹ_ref_اندر_func_دلائل(copyID);

    // funcstacks keep variables for closures
    abort = abort || سیٹ_ref_اندر_funcstacks(copyID);

    // loopvars keep variables for loop blocks
    abort = abort || سیٹ_ref_اندر_loopvars(copyID);

    // v: vars
    abort = abort || garbage_collect_vimvars(copyID);

    // callbacks in buffers
    abort = abort || سیٹ_ref_اندر_buffers(copyID);

    // 'completefunc', 'omnifunc' and 'thesaurusfunc' callbacks
    abort = abort || سیٹ_ref_اندر_insexpand_funcs(copyID);

    // 'operatorfunc' callback
    abort = abort || سیٹ_ref_اندر_opfunc(copyID);

    // 'tagfunc' callback
    abort = abort || سیٹ_ref_اندر_tagfunc(copyID);

    // 'imactivatefunc' and 'imstatusfunc' callbacks
    abort = abort || سیٹ_ref_اندر_im_funcs(copyID);

    // 'findfunc' callback
    abort = abort || سیٹ_ref_اندر_findfunc(copyID);

#اگر_تعریف_شدہ FEAT_LUA
    abort = abort || سیٹ_ref_اندر_lua(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
    abort = abort || سیٹ_ref_اندر_python(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    abort = abort || سیٹ_ref_اندر_python3(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    abort = abort || سیٹ_ref_اندر_channel(copyID);
    abort = abort || سیٹ_ref_اندر_کام(copyID);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    abort = abort || سیٹ_ref_اندر_nb_channel(copyID);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
    abort = abort || سیٹ_ref_اندر_socketserver_channel(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TIMERS
    abort = abort || سیٹ_ref_اندر_timer(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    abort = abort || سیٹ_ref_اندر_quickfix(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMINAL
    abort = abort || سیٹ_ref_اندر_رکن(copyID);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    abort = abort || سیٹ_ref_اندر_popups(copyID);
#اگر_ختم

    abort = abort || سیٹ_ref_اندر_classes(copyID);

    اگر (!abort)
    {
	/*
	 * 2. Free lists and dictionaries that are not referenced.
	 */
	did_free = free_unref_اشیاء(copyID);

	/*
	 * 3. Check if any funccal can be freed now.
	 *    This may call us back recursively.
	 */
	free_unref_funccal(copyID, testing);
    }
    ورنہ اگر (p_verbose > 0)
    {
	verb_پیغام(_("Not enough memory to set references, garbage collection aborted!"));
    }

    واپس did_free;
}

/*
 * Free lists, dictionaries, channels and jobs that are no longer referenced.
 */
    ساکن صحیح
free_unref_اشیاء(صحیح copyID)
{
    صحیح		did_free = FALSE;

    // Let all "free" functions know that we are here.  This means no
    // dictionaries, lists, channels or jobs are to be freed, because we will
    // do that here.
    اندر_free_unref_اشیاء = TRUE;

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

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    did_free |= free_غیر_مستعمل_jobs_contents(copyID, COPYID_نقاب);

    // Go through the list of channels and free items without the copyID.
    did_free |= free_غیر_مستعمل_channels_contents(copyID, COPYID_نقاب);
#اگر_ختم

    /*
     * PASS 2: free the items themselves.
     */
    object_free_اشیاء(copyID);
    dict_free_اشیاء(copyID);
    list_free_اشیاء(copyID);
    tuple_free_اشیاء(copyID);

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    // Go through the list of jobs and free items without the copyID. This
    // must happen before doing channels, because jobs refer to channels, but
    // the reference from the channel to the job isn't tracked.
    free_غیر_مستعمل_jobs(copyID, COPYID_نقاب);

    // Go through the list of channels and free items without the copyID.
    free_غیر_مستعمل_channels(copyID, COPYID_نقاب);
#اگر_ختم

    اندر_free_unref_اشیاء = FALSE;

    واپس did_free;
}

/*
 * Mark all lists and dicts referenced through hashtab "ht" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_ht(
    hashtab_T		*ht,
    صحیح			copyID,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    صحیح		todo;
    صحیح		abort = FALSE;
    hashitem_T	*hi;
    hashtab_T	*cur_ht;
    ht_ڈھیر_T	*ht_ڈھیر = تہی;
    ht_ڈھیر_T	*tempitem;

    cur_ht = ht;
    برائے (;;)
    {
	اگر (!abort)
	{
	    // Mark each item in the hashtab.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    todo = (صحیح)cur_ht->ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(cur_ht, hi, todo)
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    --todo;
		    abort = abort
			|| سیٹ_ref_اندر_شے(&HI2DI(hi)->di_tv, copyID,
				       &ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
		}
	}

	اگر (ht_ڈھیر == تہی)
	    توڑو;

	// take an item from the stack
	cur_ht = ht_ڈھیر->ht;
	tempitem = ht_ڈھیر;
	ht_ڈھیر = ht_ڈھیر->پچھلا;
	آزاد(tempitem);
    }

    واپس abort;
}

#اگر defined(FEAT_LUA) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Mark a dict and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_dict(dict_T *d, صحیح copyID)
{
    اگر (d != تہی && d->dv_copyID != copyID)
    {
	d->dv_copyID = copyID;
	واپس سیٹ_ref_اندر_ht(&d->dv_hashtab, copyID, تہی, تہی);
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Mark a list and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_list(list_T *ll, صحیح copyID)
{
    اگر (ll != تہی && ll->lv_copyID != copyID)
    {
	ll->lv_copyID = copyID;
	واپس سیٹ_ref_اندر_list_اشیاء(ll, copyID, تہی, تہی);
    }
    واپس FALSE;
}

/*
 * Mark all lists and dicts referenced through list "l" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_list_اشیاء(
    list_T		*l,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    listitem_T	 *li;
    صحیح		 abort = FALSE;
    list_T	 *cur_l;
    list_ڈھیر_T *list_ڈھیر = تہی;
    list_ڈھیر_T *tempitem;

    cur_l = l;
    برائے (;;)
    {
	اگر (!abort && cur_l->lv_پہلا != &range_list_شے)
	    // Mark each item in the list.  If the item contains a hashtab
	    // it is added to ht_stack, if it contains a list it is added to
	    // list_stack.
	    برائے (li = cur_l->lv_پہلا; !abort && li != تہی; li = li->li_اگلا)
		abort = abort || سیٹ_ref_اندر_شے(&li->li_tv, copyID,
				       ht_ڈھیر, &list_ڈھیر, tuple_ڈھیر);
	اگر (list_ڈھیر == تہی)
	    توڑو;

	// take an item from the stack
	cur_l = list_ڈھیر->list;
	tempitem = list_ڈھیر;
	list_ڈھیر = list_ڈھیر->پچھلا;
	آزاد(tempitem);
    }

    واپس abort;
}

/*
 * Mark a tuple and its items with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_tuple(tuple_T *tuple, صحیح copyID)
{
    اگر (tuple != تہی && tuple->tv_copyID != copyID)
    {
	tuple->tv_copyID = copyID;
	واپس سیٹ_ref_اندر_tuple_اشیاء(tuple, copyID, تہی, تہی);
    }
    واپس FALSE;
}

/*
 * Mark all lists and dicts referenced through tuple "t" with "copyID".
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_tuple_اشیاء(
    tuple_T		*tuple,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر)
{
    صحیح			abort = FALSE;
    tuple_T		*cur_t;
    tuple_ڈھیر_T	*tuple_ڈھیر = تہی;
    tuple_ڈھیر_T	*tempitem;

    cur_t = tuple;
    برائے (;;)
    {
	// Mark each item in the tuple.  If the item contains a hashtab
	// it is added to ht_stack, if it contains a list it is added to
	// list_stack.
	برائے (صحیح i = 0; i < cur_t->tv_اشیاء.ga_len; i++)
	{
	    typval_T *tv = ((typval_T *)cur_t->tv_اشیاء.ga_ڈیٹا) + i;
	    abort = abort
		|| سیٹ_ref_اندر_شے(tv, copyID,
			ht_ڈھیر, list_ڈھیر, &tuple_ڈھیر);
	}
	اگر (tuple_ڈھیر == تہی)
	    توڑو;

	// take an item from the stack
	cur_t = tuple_ڈھیر->tuple;
	tempitem = tuple_ڈھیر;
	tuple_ڈھیر = tuple_ڈھیر->پچھلا;
	آزاد(tempitem);
    }

    واپس abort;
}

/*
 * Mark the partial in callback 'cb' with "copyID".
 */
    صحیح
سیٹ_ref_اندر_callback(callback_T *cb, صحیح copyID)
{
    typval_T tv;

    اگر (cb->cb_name == تہی || *cb->cb_name == NUL || cb->cb_partial == تہی)
	واپس FALSE;

    tv.v_type = VAR_PARTIAL;
    tv.vval.v_partial = cb->cb_partial;
    واپس سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
}

/*
 * Mark the dict "dd" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_dict(
    dict_T		*dd,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    اگر (dd == تہی || dd->dv_copyID == copyID)
	واپس FALSE;

    // Didn't see this dict yet.
    dd->dv_copyID = copyID;
    اگر (ht_ڈھیر == تہی)
	واپس سیٹ_ref_اندر_ht(&dd->dv_hashtab, copyID, list_ڈھیر, tuple_ڈھیر);

    ht_ڈھیر_T *newitem = ALLOC_ONE(ht_ڈھیر_T);
    اگر (newitem == تہی)
	واپس TRUE;

    newitem->ht = &dd->dv_hashtab;
    newitem->پچھلا = *ht_ڈھیر;
    *ht_ڈھیر = newitem;

    واپس FALSE;
}

/*
 * Mark the list "ll" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_list(
    list_T		*ll,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    اگر (ll == تہی || ll->lv_copyID == copyID)
	واپس FALSE;

    // Didn't see this list yet.
    ll->lv_copyID = copyID;
    اگر (list_ڈھیر == تہی)
	واپس سیٹ_ref_اندر_list_اشیاء(ll, copyID, ht_ڈھیر, tuple_ڈھیر);

    list_ڈھیر_T *newitem = ALLOC_ONE(list_ڈھیر_T);
    اگر (newitem == تہی)
	واپس TRUE;

    newitem->list = ll;
    newitem->پچھلا = *list_ڈھیر;
    *list_ڈھیر = newitem;

    واپس FALSE;
}

/*
 * Mark the tuple "tt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_tuple(
    tuple_T		*tt,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    اگر (tt == تہی || tt->tv_copyID == copyID)
	واپس FALSE;

    // Didn't see this tuple yet.
    tt->tv_copyID = copyID;
    اگر (tuple_ڈھیر == تہی)
	واپس سیٹ_ref_اندر_tuple_اشیاء(tt, copyID, ht_ڈھیر, list_ڈھیر);

    tuple_ڈھیر_T *newitem = ALLOC_ONE(tuple_ڈھیر_T);
    اگر (newitem == تہی)
	واپس TRUE;

    newitem->tuple = tt;
    newitem->پچھلا = *tuple_ڈھیر;
    *tuple_ڈھیر = newitem;

    واپس FALSE;
}

/*
 * Mark the partial "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_partial(
    partial_T		*pt,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    اگر (pt == تہی || pt->pt_copyID == copyID)
	واپس FALSE;

    // Didn't see this partial yet.
    pt->pt_copyID = copyID;

    صحیح abort = سیٹ_ref_اندر_func(pt->pt_name, pt->pt_func, copyID);

    اگر (pt->pt_dict != تہی)
    {
	typval_T dtv;

	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }

    اگر (pt->pt_obj != تہی)
    {
	typval_T objtv;

	objtv.v_type = VAR_OBJECT;
	objtv.vval.v_object = pt->pt_obj;
	سیٹ_ref_اندر_شے(&objtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }

    برائے (صحیح i = 0; i < pt->pt_argc; ++i)
	abort = abort || سیٹ_ref_اندر_شے(&pt->pt_argv[i], copyID,
		ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    // pt_funcstack is handled in set_ref_in_funcstacks()
    // pt_loopvars is handled in set_ref_in_loopvars()

    واپس abort;
}

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
/*
 * Mark the job "pt" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_کام(
    کام_T		*کام,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    typval_T    dtv;

    اگر (کام == تہی || کام->jv_copyID == copyID)
	واپس FALSE;

    کام->jv_copyID = copyID;
    اگر (کام->jv_channel != تہی)
    {
	dtv.v_type = VAR_CHANNEL;
	dtv.vval.v_channel = کام->jv_channel;
	سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }
    اگر (کام->jv_exit_cb.cb_partial != تہی)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = کام->jv_exit_cb.cb_partial;
	سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }

    واپس FALSE;
}

/*
 * Mark the channel "ch" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_channel(
    channel_T		*ch,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    typval_T    dtv;

    اگر (ch == تہی || ch->ch_copyID == copyID)
	واپس FALSE;

    ch->ch_copyID = copyID;
    برائے (ch_part_T part = PART_SOCK; part < PART_گنتی; ++part)
    {
	برائے (jsonq_T *jq = ch->ch_part[part].ch_json_سر.jq_اگلا;
		jq != تہی; jq = jq->jq_اگلا)
	    سیٹ_ref_اندر_شے(jq->jq_قدر, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
	برائے (cbq_T *cq = ch->ch_part[part].ch_cb_سر.cq_اگلا; cq != تہی;
		cq = cq->cq_اگلا)
	    اگر (cq->cq_callback.cb_partial != تہی)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = cq->cq_callback.cb_partial;
		سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
	    }
	اگر (ch->ch_part[part].ch_callback.cb_partial != تہی)
	{
	    dtv.v_type = VAR_PARTIAL;
	    dtv.vval.v_partial = ch->ch_part[part].ch_callback.cb_partial;
	    سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
	}
    }
    اگر (ch->ch_callback.cb_partial != تہی)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_callback.cb_partial;
	سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }
    اگر (ch->ch_close_cb.cb_partial != تہی)
    {
	dtv.v_type = VAR_PARTIAL;
	dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
	سیٹ_ref_اندر_شے(&dtv, copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }

    واپس FALSE;
}
#اگر_ختم

/*
 * Mark the class "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    صحیح
سیٹ_ref_اندر_شے_class(
    class_T		*cl,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    صحیح abort = FALSE;

    اگر (cl == تہی || cl->class_copyID == copyID)
	واپس FALSE;

    cl->class_copyID = copyID;
    اگر (cl->class_members_tv != تہی)
    {
	// The "class_members_tv" table is allocated only for regular classes
	// and not for interfaces.
	برائے (صحیح i = 0; !abort && i < cl->class_class_member_گنتی; ++i)
	    abort = abort || سیٹ_ref_اندر_شے(
		    &cl->class_members_tv[i],
		    copyID, ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
    }

    برائے (صحیح i = 0; !abort && i < cl->class_class_function_گنتی; ++i)
	abort = abort || سیٹ_ref_اندر_func(تہی,
		cl->class_class_functions[i], copyID);

    برائے (صحیح i = 0; !abort && i < cl->class_obj_طریقہ_گنتی; ++i)
	abort = abort || سیٹ_ref_اندر_func(تہی,
		cl->class_obj_methods[i], copyID);

    واپس abort;
}

/*
 * Mark the object "cl" with "copyID".
 * Also see set_ref_in_item().
 */
    ساکن صحیح
سیٹ_ref_اندر_شے_object(
    object_T		*obj,
    صحیح			copyID,
    ht_ڈھیر_T		**ht_ڈھیر,
    list_ڈھیر_T	**list_ڈھیر,
    tuple_ڈھیر_T	**tuple_ڈھیر)
{
    صحیح abort = FALSE;

    اگر (obj == تہی || obj->obj_copyID == copyID)
	واپس FALSE;

    obj->obj_copyID = copyID;

    // The typval_T array is right after the object_T.
    typval_T *mtv = (typval_T *)(obj + 1);
    برائے (صحیح i = 0; !abort
	    && i < obj->obj_class->class_obj_member_گنتی; ++i)
	abort = abort || سیٹ_ref_اندر_شے(mtv + i, copyID,
		ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

    واپس abort;
}

/*
 * Mark all lists, dicts and other container types referenced through typval
 * "tv" with "copyID".
 * "list_stack" is used to add lists to be marked.  Can be NULL.
 * "ht_stack" is used to add hashtabs to be marked.  Can be NULL.
 *
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_شے(
    typval_T	    *tv,
    صحیح		    copyID,
    ht_ڈھیر_T	    **ht_ڈھیر,
    list_ڈھیر_T    **list_ڈھیر,
    tuple_ڈھیر_T   **tuple_ڈھیر)
{
    صحیح		abort = FALSE;

    انتخاب (tv->v_type)
    {
	حالت VAR_DICT:
	    واپس سیٹ_ref_اندر_شے_dict(tv->vval.v_dict, copyID,
					 ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

	حالت VAR_LIST:
	    واپس سیٹ_ref_اندر_شے_list(tv->vval.v_list, copyID,
					 ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

	حالت VAR_TUPLE:
	    واپس سیٹ_ref_اندر_شے_tuple(tv->vval.v_tuple, copyID,
					 ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
	حالت VAR_FUNC:
	{
	    abort = سیٹ_ref_اندر_func(tv->vval.v_string, تہی, copyID);
	    توڑو;
	}

	حالت VAR_PARTIAL:
	    واپس سیٹ_ref_اندر_شے_partial(tv->vval.v_partial, copyID,
					ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس سیٹ_ref_اندر_شے_کام(tv->vval.v_کام, copyID,
					ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
#ورنہ
	    توڑو;
#اگر_ختم

	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس سیٹ_ref_اندر_شے_channel(tv->vval.v_channel, copyID,
					ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);
#ورنہ
	    توڑو;
#اگر_ختم

	حالت VAR_CLASS:
	    واپس سیٹ_ref_اندر_شے_class(tv->vval.v_class, copyID,
					ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

	حالت VAR_OBJECT:
	    واپس سیٹ_ref_اندر_شے_object(tv->vval.v_object, copyID,
					ht_ڈھیر, list_ڈھیر, tuple_ڈھیر);

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_نمبر:
	حالت VAR_FLOAT:
	حالت VAR_STRING:
	حالت VAR_BLOB:
	حالت VAR_TYPEALIAS:
	حالت VAR_INSTR:
	    // Types that do not contain any other item
	    توڑو;
    }

    واپس abort;
}

#اگر_ختم
