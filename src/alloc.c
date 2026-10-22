/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * alloc.c: functions for memory management
 */

#شامل "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#اگر defined(MEM_PROFILE)

# define MEM_SIZES  8200
ساکن long_u mem_allocs[MEM_SIZES];
ساکن long_u mem_frees[MEM_SIZES];
ساکن long_u mem_allocated;
ساکن long_u mem_freed;
ساکن long_u mem_peak;
ساکن long_u عدد_alloc;
ساکن long_u عدد_freed;

    ساکن خالی
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += حجم(size_t);
}

    ساکن خالی
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += حجم(size_t);
}

    ساکن خالی
mem_post_alloc(
    خالی **pp,
    size_t سائز)
{
    اگر (*pp == تہی)
	واپس;
    سائز -= حجم(size_t);
    *(long_u *)*pp = سائز;
    اگر (سائز <= MEM_SIZES-1)
	mem_allocs[سائز-1]++;
    ورنہ
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += سائز;
    اگر (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    عدد_alloc++;
    *pp = (خالی *)((حرف *)*pp + حجم(size_t));
}

    ساکن خالی
mem_pre_free(خالی **pp)
{
    long_u سائز;

    *pp = (خالی *)((حرف *)*pp - حجم(size_t));
    سائز = *(size_t *)*pp;
    اگر (سائز <= MEM_SIZES-1)
	mem_frees[سائز-1]++;
    ورنہ
	mem_frees[MEM_SIZES-1]++;
    mem_freed += سائز;
    عدد_freed++;
}

/*
 * called on exit via atexit()
 */
    خالی
vim_mem_profile_dump(خالی)
{
    صحیح i, j;

    لکھو("\r\n");
    j = 0;
    برائے (i = 0; i < MEM_SIZES - 1; i++)
    {
	اگر (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    جاری;

	اگر (mem_frees[i] > mem_allocs[i])
	    لکھو("\r\n%s", _("ERROR: "));
	لکھو("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	اگر (j > 3)
	{
	    j = 0;
	    لکھو("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    اگر (mem_allocs[i])
    {
	لکھو("\r\n");
	اگر (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	لکھو("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    لکھو(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    لکھو(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    عدد_alloc, عدد_freed);
}

#اگر_ختم // MEM_PROFILE

#اگر_تعریف_شدہ FEAT_EVAL
    صحیح
alloc_does_fail(size_t سائز)
{
    اگر (alloc_fail_countdown == 0)
    {
	اگر (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_پیغام(سائز);
	واپس TRUE;
    }
    --alloc_fail_countdown;
    واپس FALSE;
}
#اگر_ختم

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#تعریف_کرو KEEP_گنجائش (2 * 8192L)
#تعریف_کرو KEEP_گنجائش_KB (KEEP_گنجائش / 1024L)

/*
 * The normal way to allocate memory.  This handles an out-of-memory situation
 * as well as possible, still returns NULL when we're completely out.
 */
    خالی *
alloc(size_t سائز)
{
    واپس lalloc(سائز, TRUE);
}

#اگر defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    خالی *
alloc_id(size_t سائز, alloc_id_T id غیر_مستعمل)
{
# ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(سائز))
	واپس تہی;
# endif
    واپس lalloc(سائز, TRUE);
}
#اگر_ختم

/*
 * Allocate memory and set all bytes to zero.
 */
    خالی *
alloc_clear(size_t سائز)
{
    خالی *p;

    p = lalloc(سائز, TRUE);
    اگر (p != تہی)
	(خالی)vim_memset(p, 0, سائز);
    واپس p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    خالی *
alloc_clear_id(size_t سائز, alloc_id_T id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(سائز))
	واپس تہی;
#اگر_ختم
    واپس alloc_clear(سائز);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    خالی *
lalloc_clear(size_t سائز, صحیح پیغام_2)
{
    خالی *p;

    p = lalloc(سائز, پیغام_2);
    اگر (p != تہی)
	(خالی)vim_memset(p, 0, سائز);
    واپس p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    خالی *
lalloc(size_t سائز, صحیح پیغام_2)
{
    خالی	*p;		    // pointer to new storage space
    ساکن صحیح	releasing = FALSE;  // don't do mf_release_all() recursive
    صحیح		try_again;
#اگر defined(HAVE_AVAIL_MEM)
    ساکن size_t allocated = 0;    // allocated since last avail check
#اگر_ختم

    // Safety check for allocating zero bytes
    اگر (سائز == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_اندرونی_error_lalloc_zero);
	واپس تہی;
    }

#اگر_تعریف_شدہ MEM_PROFILE
    mem_pre_alloc_l(&سائز);
#اگر_ختم

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    برائے (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	اگر ((p = حافظہ_دو(سائز)) != تہی)
	{
#اگر_غیر_تعریف_شدہ HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    جاؤ theend;
#ورنہ
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += سائز;
	    اگر (allocated < KEEP_گنجائش / 2)
		جاؤ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    اگر (mch_avail_mem(TRUE) < KEEP_گنجائش_KB && !releasing)
	    {
		آزاد(p);	// System is low... no go!
		p = تہی;
	    }
	    ورنہ
		جاؤ theend;
#اگر_ختم
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	اگر (releasing)
	    توڑو;
	releasing = TRUE;

	clear_sb_متن(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	اگر (!try_again)
	    توڑو;
    }

    اگر (پیغام_2 && p == تہی)
	do_outofmem_پیغام(سائز);

theend:
#اگر_تعریف_شدہ MEM_PROFILE
    mem_post_alloc(&p, سائز);
#اگر_ختم
    واپس p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#اگر defined(FEAT_SIGNS)
    خالی *
lalloc_id(size_t سائز, صحیح پیغام_2, alloc_id_T id غیر_مستعمل)
{
# ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(سائز))
	واپس تہی;
# endif
    واپس (lalloc(سائز, پیغام_2));
}
#اگر_ختم

#اگر defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    خالی *
mem_realloc(خالی *ptr, size_t سائز)
{
    خالی *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&سائز);

    p = realloc(ptr, سائز);

    mem_post_alloc(&p, سائز);

    واپس p;
}
#اگر_ختم

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    خالی
do_outofmem_پیغام(size_t سائز)
{
    اگر (did_outofmem_پیغام)
	واپس;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_پیغام = TRUE;

    semsg(_(e_برآمد_کا_حافظہ_allocating_nr_بائٹیں), (long_u)سائز);

    اگر (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#اگر defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    خالی
free_all_mem(خالی)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    اگر (entered_free_all_mem)
	واپس;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    بلاک_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    اگر (پہلا_tabpage != تہی && پہلا_tabpage->tp_اگلا != تہی)
	do_cmdline_cmd((char_u *)"tabonly!");
    اگر (!ONE_کھڑکی)
	do_cmdline_cmd((char_u *)"only!");

# اگر defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# اگر defined(FEAT_BEVAL_رکن)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    اگر (curwin != تہی)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(تہی);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    اگر (curbuf != تہی)
    {
# ifdef FEAT_MENU
	// Clear menus.
	do_cmdline_cmd((char_u *)"aunmenu *");
	do_cmdline_cmd((char_u *)"tlunmenu *");
#  ifdef FEAT_MULTI_LANG
	do_cmdline_cmd((char_u *)"menutranslate clear");
#  endif
# endif
	// Clear mappings, abbreviations, breakpoints.
	do_cmdline_cmd((char_u *)"lmapclear");
	do_cmdline_cmd((char_u *)"xmapclear");
	do_cmdline_cmd((char_u *)"mapclear");
	do_cmdline_cmd((char_u *)"mapclear!");
	do_cmdline_cmd((char_u *)"abclear");
# اگر defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# اگر defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# اگر defined(FEAT_KEYMAP)
	do_cmdline_cmd((char_u *)"set keymap=");
# endif
    }

    free_titles();
    free_findfile();

    // Obviously named calls.
    free_all_autocmds();
    clear_termcodes();
    free_all_marks();
    alist_clear(&global_alist);
    free_homedir();
    free_users();
    free_تلاش_patterns();
    free_old_sub();
    free_آخری_داخل_کرو();
    free_insexpand_stuff();
    free_پچھلا_shellcmd();
    free_regexp_stuff();
    free_لیبل_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    سیٹ_اظہاریہ_سطر(تہی, تہی);
# endif
# ifdef FEAT_DIFF
    اگر (curtab != تہی)
	diff_clear(curtab);
# endif
    clear_sb_متن(TRUE);	      // free any scrollback text

    // Free some global vars.
    free_username();
# ifdef FEAT_CLIPBOARD
    vim_regfree(clip_exclude_prog);
# endif
    vim_free(آخری_cmdline);
    vim_free(new_آخری_cmdline);
    سیٹ_keep_پیغام(تہی, 0);

    // Clear cmdline history.
    p_hi = 0;
    init_history();
# ifdef FEAT_PROP_POPUP
    clear_global_prop_types();
# endif

# ifdef FEAT_QUICKFIX
    free_quickfix();
# endif

    // Close all script inputs.
    close_all_scripts();

    اگر (curwin != تہی)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    برائے (buf = firstbuf; buf != تہی; )
    {
	bufref_T    bufref;

	سیٹ_bufref(&bufref, buf);
	nextbuf = buf->b_اگلا;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_بفر(تہی, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	اگر (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	ورنہ
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(server_Delayed_آغاز_Name);
# endif

    // highlight info
    free_highlight();

    reset_آخری_sourcing();

    اگر (پہلا_tabpage != تہی)
    {
	free_tabpage(پہلا_tabpage);
	پہلا_tabpage = تہی;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    برائے (;;)
	اگر (delete_پہلا_پیغام() == FAIL)
	    توڑو;

# ifdef FEAT_کام_CHANNEL
    channel_free_all();
# endif
# ifdef FEAT_TIMERS
    timer_free_all();
# endif
# ifdef FEAT_EVAL
    // must be after channel_free_all() with unrefs partials
    eval_clear();
# endif
# ifdef FEAT_کام_CHANNEL
    // must be after eval_clear() with unrefs jobs
    کام_free_all();
# endif

    free_termoptions();
    free_cur_رکن();

    // screenlines (can't display anything now!)
    free_screenlines();

# اگر defined(FEAT_SOUND)
    sound_free();
# endif
# اگر defined(USE_XSMP)
    xsmp_close();
# endif
# ifdef FEAT_GUI_GTK
    gui_mch_free_all();
# endif
# ifdef FEAT_TCL
    vim_tcl_finalize();
# endif
    clear_hl_tables();

    vim_free(IObuff);
    vim_free(NameBuff);
# ifdef FEAT_QUICKFIX
    check_quickfix_busy();
# endif
# ifdef FEAT_EVAL
    free_resub_eval_نتیجہ();
# endif
    free_vbuf();
}
#اگر_ختم

/*
 * Copy "p[len]" into allocated memory, ignoring NUL characters.
 * Returns NULL when out of memory.
 */
    char_u *
vim_memsave(char_u *p, size_t len)
{
    char_u *ret = alloc(len);

    اگر (ret != تہی)
	mch_memmove(ret, p, len);
    واپس ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    خالی
vim_free(خالی *x)
{
    اگر (x != تہی && !really_exiting)
    {
#اگر_تعریف_شدہ MEM_PROFILE
	mem_pre_free(&x);
#اگر_ختم
	آزاد(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    خالی
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_ڈیٹا);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    خالی
ga_clear_لڑیاں(garray_T *gap)
{
    صحیح		i;

    اگر (gap->ga_ڈیٹا != تہی)
	برائے (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_ڈیٹا))[i]);
    ga_clear(gap);
}

#اگر defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    صحیح
ga_copy_لڑیاں(garray_T *from, garray_T *مقصد)
{
    صحیح		i;

    ga_init2(مقصد, حجم(char_u *), 1);
    اگر (ga_grow(مقصد, from->ga_len) == FAIL)
	واپس FAIL;

    برائے (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_ڈیٹا)[i];
	char_u *copy;

	اگر (orig == تہی)
	    copy = تہی;
	ورنہ
	{
	    copy = vim_strsave(orig);
	    اگر (copy == تہی)
	    {
		مقصد->ga_len = i;
		ga_clear_لڑیاں(مقصد);
		واپس FAIL;
	    }
	}
	((char_u **)مقصد->ga_ڈیٹا)[i] = copy;
    }
    مقصد->ga_len = from->ga_len;
    واپس OK;
}
#اگر_ختم

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    خالی
ga_init(garray_T *gap)
{
    gap->ga_ڈیٹا = تہی;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    خالی
ga_init2(garray_T *gap, size_t itemsize, صحیح growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (صحیح)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
ga_grow(garray_T *gap, صحیح n)
{
    اگر (gap->ga_maxlen - gap->ga_len < n)
	واپس ga_grow_inner(gap, n);
    واپس OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    صحیح
ga_grow_id(garray_T *gap, صحیح n, alloc_id_T id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	واپس FAIL;
#اگر_ختم

    واپس ga_grow(gap, n);
}

    صحیح
ga_grow_inner(garray_T *gap, صحیح n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    اگر (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    اگر (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    اگر (n > 0 && (size_t)(gap->ga_len + n) > سائز_MAX / gap->ga_itemsize)
	واپس FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_ڈیٹا, new_len);
    اگر (pp == تہی)
	واپس FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_ڈیٹا = pp;
    واپس OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_لڑیاں(garray_T *gap, حرف *sep)
{
    صحیح		i;
    صحیح		len = 0;
    صحیح		sep_len = (صحیح)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    برائے (i = 0; i < gap->ga_len; ++i)
	len += (صحیح)STRLEN(((char_u **)(gap->ga_ڈیٹا))[i]);

    اگر (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    اگر (s == تہی)
	واپس تہی;

    *s = NUL;
    p = s;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	اگر (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_ڈیٹا))[i]);
	p += STRLEN(p);
    }
    واپس s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    صحیح
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    اگر (cp == تہی)
	واپس FAIL;

    اگر (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	واپس FAIL;
    }
    ((char_u **)(gap->ga_ڈیٹا))[gap->ga_len++] = cp;
    واپس OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    صحیح
ga_جوڑو_string(garray_T *gap, char_u *p)
{
    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    ((char_u **)(gap->ga_ڈیٹا))[gap->ga_len++] = p;
    واپس OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    خالی
ga_concat(garray_T *gap, char_u *s)
{
    صحیح    len;

    اگر (s == تہی || *s == NUL)
	واپس;
    len = (صحیح)STRLEN(s);
    اگر (ga_grow(gap, len) == OK)
    {
	mch_memmove((حرف *)gap->ga_ڈیٹا + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    خالی
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    اگر (s == تہی || *s == NUL || len == 0)
	واپس;
    اگر (len > (size_t)(زیادہ_سے_زیادہ_عدد - gap->ga_len))
    {
	emsg(_(e_resulting_متن_too_long));
	واپس;
    }
    اگر (ga_grow(gap, (صحیح)len) == OK)
    {
	mch_memmove((حرف *)gap->ga_ڈیٹا + gap->ga_len, s, len);
	gap->ga_len += (صحیح)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    صحیح
ga_append(garray_T *gap, صحیح c)
{
    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    *((حرف *)gap->ga_ڈیٹا + gap->ga_len) = c;
    ++gap->ga_len;
    واپس OK;
}

#اگر (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    خالی
append_ga_سطر(garray_T *gap)
{
    // Remove trailing CR.
    اگر (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_ڈیٹا)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_ڈیٹا, 0, FALSE);
    gap->ga_len = 0;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    صحیح
ga_concat_بائٹیں(garray_T *gap, ثابت حرف *s, صحیح len)
{
    اگر (ga_grow(gap, len) == FAIL)
	واپس FAIL;
    mch_memmove((char_u *)gap->ga_ڈیٹا + gap->ga_len, s, len);
    gap->ga_len += len;
    واپس OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    صحیح
ga_concat_int(garray_T *gap, صحیح n)
{
    حرف    buf[16];
    صحیح	    len = vim_snprintf(buf, حجم(buf), "%d", n);

    واپس ga_concat_بائٹیں(gap, buf, len);
}
#اگر_ختم
