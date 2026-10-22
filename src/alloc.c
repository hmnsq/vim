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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#ਜੇ defined(MEM_PROFILE)

# define MEM_SIZES  8200
ਸਥਿਰ long_u mem_allocs[MEM_SIZES];
ਸਥਿਰ long_u mem_frees[MEM_SIZES];
ਸਥਿਰ long_u mem_allocated;
ਸਥਿਰ long_u mem_freed;
ਸਥਿਰ long_u mem_peak;
ਸਥਿਰ long_u ਅੰਕ_alloc;
ਸਥਿਰ long_u ਅੰਕ_freed;

    ਸਥਿਰ ਖਾਲੀ
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += ਆਕਾਰ(size_t);
}

    ਸਥਿਰ ਖਾਲੀ
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += ਆਕਾਰ(size_t);
}

    ਸਥਿਰ ਖਾਲੀ
mem_post_alloc(
    ਖਾਲੀ **pp,
    size_t ਮਾਪ)
{
    ਜੇ (*pp == ਨਿਰਾ)
	ਵਾਪਸ;
    ਮਾਪ -= ਆਕਾਰ(size_t);
    *(long_u *)*pp = ਮਾਪ;
    ਜੇ (ਮਾਪ <= MEM_SIZES-1)
	mem_allocs[ਮਾਪ-1]++;
    ਨਹੀਂਤਾਂ
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += ਮਾਪ;
    ਜੇ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    ਅੰਕ_alloc++;
    *pp = (ਖਾਲੀ *)((ਅੱਖਰ *)*pp + ਆਕਾਰ(size_t));
}

    ਸਥਿਰ ਖਾਲੀ
mem_pre_free(ਖਾਲੀ **pp)
{
    long_u ਮਾਪ;

    *pp = (ਖਾਲੀ *)((ਅੱਖਰ *)*pp - ਆਕਾਰ(size_t));
    ਮਾਪ = *(size_t *)*pp;
    ਜੇ (ਮਾਪ <= MEM_SIZES-1)
	mem_frees[ਮਾਪ-1]++;
    ਨਹੀਂਤਾਂ
	mem_frees[MEM_SIZES-1]++;
    mem_freed += ਮਾਪ;
    ਅੰਕ_freed++;
}

/*
 * called on exit via atexit()
 */
    ਖਾਲੀ
vim_mem_profile_dump(ਖਾਲੀ)
{
    ਪੂਰਨ i, j;

    ਲਿਖੋ("\r\n");
    j = 0;
    ਲਈ (i = 0; i < MEM_SIZES - 1; i++)
    {
	ਜੇ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ਜਾਰੀ;

	ਜੇ (mem_frees[i] > mem_allocs[i])
	    ਲਿਖੋ("\r\n%s", _("ERROR: "));
	ਲਿਖੋ("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	ਜੇ (j > 3)
	{
	    j = 0;
	    ਲਿਖੋ("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    ਜੇ (mem_allocs[i])
    {
	ਲਿਖੋ("\r\n");
	ਜੇ (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	ਲਿਖੋ("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    ਲਿਖੋ(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    ਲਿਖੋ(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    ਅੰਕ_alloc, ਅੰਕ_freed);
}

#ਜੇ_ਅੰਤ // MEM_PROFILE

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ
alloc_does_fail(size_t ਮਾਪ)
{
    ਜੇ (alloc_fail_countdown == 0)
    {
	ਜੇ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_ਸੁਨੇਹਾ(ਮਾਪ);
	ਵਾਪਸ TRUE;
    }
    --alloc_fail_countdown;
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEEP_ਗੁੰਜਾਇਸ਼ (2 * 8192L)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEEP_ਗੁੰਜਾਇਸ਼_KB (KEEP_ਗੁੰਜਾਇਸ਼ / 1024L)

/*
 * The normal way to allocate memory.  This handles an out-of-memory situation
 * as well as possible, still returns NULL when we're completely out.
 */
    ਖਾਲੀ *
alloc(size_t ਮਾਪ)
{
    ਵਾਪਸ lalloc(ਮਾਪ, TRUE);
}

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    ਖਾਲੀ *
alloc_id(size_t ਮਾਪ, alloc_id_T id ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਮਾਪ))
	ਵਾਪਸ ਨਿਰਾ;
# endif
    ਵਾਪਸ lalloc(ਮਾਪ, TRUE);
}
#ਜੇ_ਅੰਤ

/*
 * Allocate memory and set all bytes to zero.
 */
    ਖਾਲੀ *
alloc_clear(size_t ਮਾਪ)
{
    ਖਾਲੀ *p;

    p = lalloc(ਮਾਪ, TRUE);
    ਜੇ (p != ਨਿਰਾ)
	(ਖਾਲੀ)vim_memset(p, 0, ਮਾਪ);
    ਵਾਪਸ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    ਖਾਲੀ *
alloc_clear_id(size_t ਮਾਪ, alloc_id_T id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਮਾਪ))
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ alloc_clear(ਮਾਪ);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    ਖਾਲੀ *
lalloc_clear(size_t ਮਾਪ, ਪੂਰਨ ਸੁਨੇਹਾ_2)
{
    ਖਾਲੀ *p;

    p = lalloc(ਮਾਪ, ਸੁਨੇਹਾ_2);
    ਜੇ (p != ਨਿਰਾ)
	(ਖਾਲੀ)vim_memset(p, 0, ਮਾਪ);
    ਵਾਪਸ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    ਖਾਲੀ *
lalloc(size_t ਮਾਪ, ਪੂਰਨ ਸੁਨੇਹਾ_2)
{
    ਖਾਲੀ	*p;		    // pointer to new storage space
    ਸਥਿਰ ਪੂਰਨ	releasing = FALSE;  // don't do mf_release_all() recursive
    ਪੂਰਨ		try_again;
#ਜੇ defined(HAVE_AVAIL_MEM)
    ਸਥਿਰ size_t allocated = 0;    // allocated since last avail check
#ਜੇ_ਅੰਤ

    // Safety check for allocating zero bytes
    ਜੇ (ਮਾਪ == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_ਅੰਦਰੂਨੀ_error_lalloc_zero);
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MEM_PROFILE
    mem_pre_alloc_l(&ਮਾਪ);
#ਜੇ_ਅੰਤ

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    ਲਈ (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	ਜੇ ((p = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ)) != ਨਿਰਾ)
	{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    ਜਾਓ theend;
#ਨਹੀਂਤਾਂ
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += ਮਾਪ;
	    ਜੇ (allocated < KEEP_ਗੁੰਜਾਇਸ਼ / 2)
		ਜਾਓ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    ਜੇ (mch_avail_mem(TRUE) < KEEP_ਗੁੰਜਾਇਸ਼_KB && !releasing)
	    {
		ਮੁਕਤ(p);	// System is low... no go!
		p = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		ਜਾਓ theend;
#ਜੇ_ਅੰਤ
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	ਜੇ (releasing)
	    ਰੋਕੋ;
	releasing = TRUE;

	clear_sb_ਪਾਠ(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	ਜੇ (!try_again)
	    ਰੋਕੋ;
    }

    ਜੇ (ਸੁਨੇਹਾ_2 && p == ਨਿਰਾ)
	do_outofmem_ਸੁਨੇਹਾ(ਮਾਪ);

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MEM_PROFILE
    mem_post_alloc(&p, ਮਾਪ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#ਜੇ defined(FEAT_SIGNS)
    ਖਾਲੀ *
lalloc_id(size_t ਮਾਪ, ਪੂਰਨ ਸੁਨੇਹਾ_2, alloc_id_T id ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਮਾਪ))
	ਵਾਪਸ ਨਿਰਾ;
# endif
    ਵਾਪਸ (lalloc(ਮਾਪ, ਸੁਨੇਹਾ_2));
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    ਖਾਲੀ *
mem_realloc(ਖਾਲੀ *ptr, size_t ਮਾਪ)
{
    ਖਾਲੀ *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&ਮਾਪ);

    p = realloc(ptr, ਮਾਪ);

    mem_post_alloc(&p, ਮਾਪ);

    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    ਖਾਲੀ
do_outofmem_ਸੁਨੇਹਾ(size_t ਮਾਪ)
{
    ਜੇ (did_outofmem_ਸੁਨੇਹਾ)
	ਵਾਪਸ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_ਸੁਨੇਹਾ = TRUE;

    semsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ_allocating_nr_ਬਾਈਟਾਂ), (long_u)ਮਾਪ);

    ਜੇ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#ਜੇ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    ਖਾਲੀ
free_all_mem(ਖਾਲੀ)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    ਜੇ (entered_free_all_mem)
	ਵਾਪਸ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    ਖੰਡ_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    ਜੇ (ਪਹਿਲਾ_tabpage != ਨਿਰਾ && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	do_cmdline_cmd((char_u *)"tabonly!");
    ਜੇ (!ONE_ਖਿੜਕੀ)
	do_cmdline_cmd((char_u *)"only!");

# ਜੇ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# ਜੇ defined(FEAT_BEVAL_ਪਦ)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    ਜੇ (curwin != ਨਿਰਾ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ਨਿਰਾ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    ਜੇ (curbuf != ਨਿਰਾ)
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
# ਜੇ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# ਜੇ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# ਜੇ defined(FEAT_KEYMAP)
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
    free_ਖੋਜ_patterns();
    free_old_sub();
    free_ਆਖਰੀ_ਪਾਓ();
    free_insexpand_stuff();
    free_prev_shellcmd();
    free_regexp_stuff();
    free_ਟੈਗ_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(ਨਿਰਾ, ਨਿਰਾ);
# endif
# ifdef FEAT_DIFF
    ਜੇ (curtab != ਨਿਰਾ)
	diff_clear(curtab);
# endif
    clear_sb_ਪਾਠ(TRUE);	      // free any scrollback text

    // Free some global vars.
    free_username();
# ifdef FEAT_CLIPBOARD
    vim_regfree(clip_exclude_prog);
# endif
    vim_free(ਆਖਰੀ_cmdline);
    vim_free(new_ਆਖਰੀ_cmdline);
    ਸੈੱਟ_keep_ਸੁਨੇਹਾ(ਨਿਰਾ, 0);

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

    ਜੇ (curwin != ਨਿਰਾ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    ਲਈ (buf = firstbuf; buf != ਨਿਰਾ; )
    {
	bufref_T    bufref;

	ਸੈੱਟ_bufref(&bufref, buf);
	nextbuf = buf->b_ਅਗਲਾ;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_ਬਫਰ(ਨਿਰਾ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	ਜੇ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	ਨਹੀਂਤਾਂ
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# ਜੇ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(server_Delayed_ਸ਼ੁਰੂ_Name);
# endif

    // highlight info
    free_highlight();

    reset_ਆਖਰੀ_sourcing();

    ਜੇ (ਪਹਿਲਾ_tabpage != ਨਿਰਾ)
    {
	free_tabpage(ਪਹਿਲਾ_tabpage);
	ਪਹਿਲਾ_tabpage = ਨਿਰਾ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    ਲਈ (;;)
	ਜੇ (delete_ਪਹਿਲਾ_ਸੁਨੇਹਾ() == FAIL)
	    ਰੋਕੋ;

# ifdef FEAT_ਕੰਮ_CHANNEL
    channel_free_all();
# endif
# ifdef FEAT_TIMERS
    timer_free_all();
# endif
# ifdef FEAT_EVAL
    // must be after channel_free_all() with unrefs partials
    eval_clear();
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
    // must be after eval_clear() with unrefs jobs
    ਕੰਮ_free_all();
# endif

    free_termoptions();
    free_cur_ਪਦ();

    // screenlines (can't display anything now!)
    free_screenlines();

# ਜੇ defined(FEAT_SOUND)
    sound_free();
# endif
# ਜੇ defined(USE_XSMP)
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
    free_resub_eval_ਨਤੀਜਾ();
# endif
    free_vbuf();
}
#ਜੇ_ਅੰਤ

/*
 * Copy "p[len]" into allocated memory, ignoring NUL characters.
 * Returns NULL when out of memory.
 */
    char_u *
vim_memsave(char_u *p, size_t len)
{
    char_u *ret = alloc(len);

    ਜੇ (ret != ਨਿਰਾ)
	mch_memmove(ret, p, len);
    ਵਾਪਸ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    ਖਾਲੀ
vim_free(ਖਾਲੀ *x)
{
    ਜੇ (x != ਨਿਰਾ && !really_exiting)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MEM_PROFILE
	mem_pre_free(&x);
#ਜੇ_ਅੰਤ
	ਮੁਕਤ(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    ਖਾਲੀ
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_ਅੰਕੜੇ);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    ਖਾਲੀ
ga_clear_ਲੜੀਆਂ(garray_T *gap)
{
    ਪੂਰਨ		i;

    ਜੇ (gap->ga_ਅੰਕੜੇ != ਨਿਰਾ)
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_ਅੰਕੜੇ))[i]);
    ga_clear(gap);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    ਪੂਰਨ
ga_copy_ਲੜੀਆਂ(garray_T *from, garray_T *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		i;

    ga_init2(ਮੰਜ਼ਿਲ, ਆਕਾਰ(char_u *), 1);
    ਜੇ (ga_grow(ਮੰਜ਼ਿਲ, from->ga_len) == FAIL)
	ਵਾਪਸ FAIL;

    ਲਈ (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_ਅੰਕੜੇ)[i];
	char_u *copy;

	ਜੇ (orig == ਨਿਰਾ)
	    copy = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    copy = vim_strsave(orig);
	    ਜੇ (copy == ਨਿਰਾ)
	    {
		ਮੰਜ਼ਿਲ->ga_len = i;
		ga_clear_ਲੜੀਆਂ(ਮੰਜ਼ਿਲ);
		ਵਾਪਸ FAIL;
	    }
	}
	((char_u **)ਮੰਜ਼ਿਲ->ga_ਅੰਕੜੇ)[i] = copy;
    }
    ਮੰਜ਼ਿਲ->ga_len = from->ga_len;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    ਖਾਲੀ
ga_init(garray_T *gap)
{
    gap->ga_ਅੰਕੜੇ = ਨਿਰਾ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    ਖਾਲੀ
ga_init2(garray_T *gap, size_t itemsize, ਪੂਰਨ growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (ਪੂਰਨ)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
ga_grow(garray_T *gap, ਪੂਰਨ n)
{
    ਜੇ (gap->ga_maxlen - gap->ga_len < n)
	ਵਾਪਸ ga_grow_inner(gap, n);
    ਵਾਪਸ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    ਪੂਰਨ
ga_grow_id(garray_T *gap, ਪੂਰਨ n, alloc_id_T id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਆਕਾਰ(list_T)))
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ga_grow(gap, n);
}

    ਪੂਰਨ
ga_grow_inner(garray_T *gap, ਪੂਰਨ n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    ਜੇ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    ਜੇ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    ਜੇ (n > 0 && (size_t)(gap->ga_len + n) > ਮਾਪ_MAX / gap->ga_itemsize)
	ਵਾਪਸ FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_ਅੰਕੜੇ, new_len);
    ਜੇ (pp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_ਅੰਕੜੇ = pp;
    ਵਾਪਸ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_ਲੜੀਆਂ(garray_T *gap, ਅੱਖਰ *sep)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		sep_len = (ਪੂਰਨ)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    ਲਈ (i = 0; i < gap->ga_len; ++i)
	len += (ਪੂਰਨ)STRLEN(((char_u **)(gap->ga_ਅੰਕੜੇ))[i]);

    ਜੇ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    *s = NUL;
    p = s;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	ਜੇ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_ਅੰਕੜੇ))[i]);
	p += STRLEN(p);
    }
    ਵਾਪਸ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    ਪੂਰਨ
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    ਜੇ (cp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ਵਾਪਸ FAIL;
    }
    ((char_u **)(gap->ga_ਅੰਕੜੇ))[gap->ga_len++] = cp;
    ਵਾਪਸ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    ਪੂਰਨ
ga_ਜੋੜੋ_string(garray_T *gap, char_u *p)
{
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ((char_u **)(gap->ga_ਅੰਕੜੇ))[gap->ga_len++] = p;
    ਵਾਪਸ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    ਖਾਲੀ
ga_concat(garray_T *gap, char_u *s)
{
    ਪੂਰਨ    len;

    ਜੇ (s == ਨਿਰਾ || *s == NUL)
	ਵਾਪਸ;
    len = (ਪੂਰਨ)STRLEN(s);
    ਜੇ (ga_grow(gap, len) == OK)
    {
	mch_memmove((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    ਖਾਲੀ
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    ਜੇ (s == ਨਿਰਾ || *s == NUL || len == 0)
	ਵਾਪਸ;
    ਜੇ (len > (size_t)(ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - gap->ga_len))
    {
	emsg(_(e_resulting_ਪਾਠ_too_long));
	ਵਾਪਸ;
    }
    ਜੇ (ga_grow(gap, (ਪੂਰਨ)len) == OK)
    {
	mch_memmove((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + gap->ga_len, s, len);
	gap->ga_len += (ਪੂਰਨ)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    ਪੂਰਨ
ga_append(garray_T *gap, ਪੂਰਨ c)
{
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    *((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + gap->ga_len) = c;
    ++gap->ga_len;
    ਵਾਪਸ OK;
}

#ਜੇ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    ਖਾਲੀ
append_ga_ਸਤਰ(garray_T *gap)
{
    // Remove trailing CR.
    ਜੇ (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_ਅੰਕੜੇ)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_ਅੰਕੜੇ, 0, FALSE);
    gap->ga_len = 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    ਪੂਰਨ
ga_concat_ਬਾਈਟਾਂ(garray_T *gap, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ len)
{
    ਜੇ (ga_grow(gap, len) == FAIL)
	ਵਾਪਸ FAIL;
    mch_memmove((char_u *)gap->ga_ਅੰਕੜੇ + gap->ga_len, s, len);
    gap->ga_len += len;
    ਵਾਪਸ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    ਪੂਰਨ
ga_concat_int(garray_T *gap, ਪੂਰਨ n)
{
    ਅੱਖਰ    buf[16];
    ਪੂਰਨ	    len = vim_snprintf(buf, ਆਕਾਰ(buf), "%d", n);

    ਵਾਪਸ ga_concat_ਬਾਈਟਾਂ(gap, buf, len);
}
#ਜੇ_ਅੰਤ
