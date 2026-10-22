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

#ಅನ್ತರ್ಭಾವ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#ಯದಿ defined(MEM_PROFILE)

# define MEM_SIZES  8200
ಸ್ಥಿರ long_u mem_allocs[MEM_SIZES];
ಸ್ಥಿರ long_u mem_frees[MEM_SIZES];
ಸ್ಥಿರ long_u mem_allocated;
ಸ್ಥಿರ long_u mem_freed;
ಸ್ಥಿರ long_u mem_peak;
ಸ್ಥಿರ long_u ಅಂಕೆ_alloc;
ಸ್ಥಿರ long_u ಅಂಕೆ_freed;

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += ಪರಿಮಾಣ(size_t);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += ಪರಿಮಾಣ(size_t);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_post_alloc(
    ಶೂನ್ಯ **pp,
    size_t size)
{
    ಯದಿ (*pp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    size -= ಪರಿಮಾಣ(size_t);
    *(long_u *)*pp = size;
    ಯದಿ (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    ಅನ್ಯಥಾ
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    ಯದಿ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    ಅಂಕೆ_alloc++;
    *pp = (ಶೂನ್ಯ *)((ವರ್ಣ *)*pp + ಪರಿಮಾಣ(size_t));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_free(ಶೂನ್ಯ **pp)
{
    long_u size;

    *pp = (ಶೂನ್ಯ *)((ವರ್ಣ *)*pp - ಪರಿಮಾಣ(size_t));
    size = *(size_t *)*pp;
    ಯದಿ (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    ಅನ್ಯಥಾ
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    ಅಂಕೆ_freed++;
}

/*
 * called on exit via atexit()
 */
    ಶೂನ್ಯ
vim_mem_profile_dump(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i, j;

    printf("\r\n");
    j = 0;
    ಪ್ರತಿ (i = 0; i < MEM_SIZES - 1; i++)
    {
	ಯದಿ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ಅನುವರ್ತ;

	ಯದಿ (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	ಯದಿ (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    ಯದಿ (mem_allocs[i])
    {
	printf("\r\n");
	ಯದಿ (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    ಅಂಕೆ_alloc, ಅಂಕೆ_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ
alloc_does_fail(size_t size)
{
    ಯದಿ (alloc_fail_countdown == 0)
    {
	ಯದಿ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_ಸಂದೇಶ(size);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    --alloc_fail_countdown;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#define KEEP_ROOM (2 * 8192L)
#define KEEP_ROOM_KB (KEEP_ROOM / 1024L)

/*
 * The normal way to allocate memory.  This handles an out-of-memory situation
 * as well as possible, still returns NULL when we're completely out.
 */
    ಶೂನ್ಯ *
alloc(size_t size)
{
    ಪ್ರತ್ಯಾಗಮ lalloc(size, TRUE);
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    ಶೂನ್ಯ *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(size))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
# endif
    ಪ್ರತ್ಯಾಗಮ lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    ಶೂನ್ಯ *
alloc_clear(size_t size)
{
    ಶೂನ್ಯ *p;

    p = lalloc(size, TRUE);
    ಯದಿ (p != ರಿಕ್ತ)
	(ಶೂನ್ಯ)vim_memset(p, 0, size);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    ಶೂನ್ಯ *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(size))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    ಪ್ರತ್ಯಾಗಮ alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    ಶೂನ್ಯ *
lalloc_clear(size_t size, ಪೂರ್ಣಾಙ್ಕ message)
{
    ಶೂನ್ಯ *p;

    p = lalloc(size, message);
    ಯದಿ (p != ರಿಕ್ತ)
	(ಶೂನ್ಯ)vim_memset(p, 0, size);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    ಶೂನ್ಯ *
lalloc(size_t size, ಪೂರ್ಣಾಙ್ಕ message)
{
    ಶೂನ್ಯ	*p;		    // pointer to new storage space
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	releasing = FALSE;  // don't do mf_release_all() recursive
    ಪೂರ್ಣಾಙ್ಕ		try_again;
#ಯದಿ defined(HAVE_AVAIL_MEM)
    ಸ್ಥಿರ size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    ಯದಿ (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    ಪ್ರತಿ (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	ಯದಿ ((p = malloc(size)) != ರಿಕ್ತ)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    ಗಚ್ಛ theend;
#ಅನ್ಯಥಾ
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    ಯದಿ (allocated < KEEP_ROOM / 2)
		ಗಚ್ಛ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    ಯದಿ (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		ಮುಕ್ತ(p);	// System is low... no go!
		p = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		ಗಚ್ಛ theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	ಯದಿ (releasing)
	    ಭಙ್ಗ;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	ಯದಿ (!try_again)
	    ಭಙ್ಗ;
    }

    ಯದಿ (message && p == ರಿಕ್ತ)
	do_outofmem_ಸಂದೇಶ(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#ಯದಿ defined(FEAT_SIGNS)
    ಶೂನ್ಯ *
lalloc_id(size_t size, ಪೂರ್ಣಾಙ್ಕ message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(size))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
# endif
    ಪ್ರತ್ಯಾಗಮ (lalloc(size, message));
}
#endif

#ಯದಿ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    ಶೂನ್ಯ *
mem_realloc(ಶೂನ್ಯ *ptr, size_t size)
{
    ಶೂನ್ಯ *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    ಪ್ರತ್ಯಾಗಮ p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    ಶೂನ್ಯ
do_outofmem_ಸಂದೇಶ(size_t size)
{
    ಯದಿ (did_outofmem_ಸಂದೇಶ)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_ಸಂದೇಶ = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    ಯದಿ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#ಯದಿ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    ಶೂನ್ಯ
free_all_mem(ಶೂನ್ಯ)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    ಯದಿ (entered_free_all_mem)
	ಪ್ರತ್ಯಾಗಮ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    ಯದಿ (first_tabpage != ರಿಕ್ತ && first_tabpage->tp_next != ರಿಕ್ತ)
	do_cmdline_cmd((char_u *)"tabonly!");
    ಯದಿ (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# ಯದಿ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# ಯದಿ defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    ಯದಿ (curwin != ರಿಕ್ತ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ರಿಕ್ತ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    ಯದಿ (curbuf != ರಿಕ್ತ)
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
# ಯದಿ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# ಯದಿ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# ಯದಿ defined(FEAT_KEYMAP)
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
    free_ಹುಡುಕು_patterns();
    free_old_sub();
    free_last_ಸೇರಿಸು();
    free_insexpand_stuff();
    free_prev_shellcmd();
    free_regexp_stuff();
    free_tag_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(ರಿಕ್ತ, ರಿಕ್ತ);
# endif
# ifdef FEAT_DIFF
    ಯದಿ (curtab != ರಿಕ್ತ)
	diff_clear(curtab);
# endif
    clear_sb_text(TRUE);	      // free any scrollback text

    // Free some global vars.
    free_username();
# ifdef FEAT_CLIPBOARD
    vim_regfree(clip_exclude_prog);
# endif
    vim_free(last_cmdline);
    vim_free(new_last_cmdline);
    set_keep_ಸಂದೇಶ(ರಿಕ್ತ, 0);

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

    ಯದಿ (curwin != ರಿಕ್ತ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    ಪ್ರತಿ (buf = firstbuf; buf != ರಿಕ್ತ; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(ರಿಕ್ತ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	ಯದಿ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	ಅನ್ಯಥಾ
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# ಯದಿ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    ಯದಿ (first_tabpage != ರಿಕ್ತ)
    {
	free_tabpage(first_tabpage);
	first_tabpage = ರಿಕ್ತ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    ಪ್ರತಿ (;;)
	ಯದಿ (delete_first_ಸಂದೇಶ() == FAIL)
	    ಭಙ್ಗ;

# ifdef FEAT_JOB_CHANNEL
    channel_free_all();
# endif
# ifdef FEAT_TIMERS
    timer_free_all();
# endif
# ifdef FEAT_EVAL
    // must be after channel_free_all() with unrefs partials
    eval_clear();
# endif
# ifdef FEAT_JOB_CHANNEL
    // must be after eval_clear() with unrefs jobs
    job_free_all();
# endif

    free_termoptions();
    free_cur_term();

    // screenlines (can't display anything now!)
    free_screenlines();

# ಯದಿ defined(FEAT_SOUND)
    sound_free();
# endif
# ಯದಿ defined(USE_XSMP)
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
    free_resub_eval_result();
# endif
    free_vbuf();
}
#endif

/*
 * Copy "p[len]" into allocated memory, ignoring NUL characters.
 * Returns NULL when out of memory.
 */
    char_u *
vim_memsave(char_u *p, size_t len)
{
    char_u *ret = alloc(len);

    ಯದಿ (ret != ರಿಕ್ತ)
	mch_memmove(ret, p, len);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    ಶೂನ್ಯ
vim_free(ಶೂನ್ಯ *x)
{
    ಯದಿ (x != ರಿಕ್ತ && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	ಮುಕ್ತ(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    ಶೂನ್ಯ
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    ಶೂನ್ಯ
ga_clear_strings(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (gap->ga_data != ರಿಕ್ತ)
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_copy_strings(garray_T *from, garray_T *to)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ga_init2(to, ಪರಿಮಾಣ(char_u *), 1);
    ಯದಿ (ga_grow(to, from->ga_len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	ಯದಿ (orig == ರಿಕ್ತ)
	    copy = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    copy = vim_strsave(orig);
	    ಯದಿ (copy == ರಿಕ್ತ)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    ಶೂನ್ಯ
ga_init(garray_T *gap)
{
    gap->ga_data = ರಿಕ್ತ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    ಶೂನ್ಯ
ga_init2(garray_T *gap, size_t itemsize, ಪೂರ್ಣಾಙ್ಕ growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (ಪೂರ್ಣಾಙ್ಕ)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_grow(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ n)
{
    ಯದಿ (gap->ga_maxlen - gap->ga_len < n)
	ಪ್ರತ್ಯಾಗಮ ga_grow_inner(gap, n);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_grow_id(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(ಪರಿಮಾಣ(list_T)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

    ಪ್ರತ್ಯಾಗಮ ga_grow(gap, n);
}

    ಪೂರ್ಣಾಙ್ಕ
ga_grow_inner(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    ಯದಿ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    ಯದಿ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    ಯದಿ (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    ಯದಿ (pp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, ವರ್ಣ *sep)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		sep_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(((char_u **)(gap->ga_data))[i]);

    ಯದಿ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *s = NUL;
    p = s;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	ಯದಿ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    ಯದಿ (cp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_add_string(garray_T *gap, char_u *p)
{
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    ಶೂನ್ಯ
ga_concat(garray_T *gap, char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ    len;

    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಪ್ರತ್ಯಾಗಮ;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
    ಯದಿ (ga_grow(gap, len) == OK)
    {
	mch_memmove((ವರ್ಣ *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    ಶೂನ್ಯ
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    ಯದಿ (s == ರಿಕ್ತ || *s == NUL || len == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)len) == OK)
    {
	mch_memmove((ವರ್ಣ *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (ಪೂರ್ಣಾಙ್ಕ)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_append(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *((ವರ್ಣ *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    ಶೂನ್ಯ
append_ga_ಸಾಲು(garray_T *gap)
{
    // Remove trailing CR.
    ಯದಿ (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_data)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_data, 0, FALSE);
    gap->ga_len = 0;
}
#endif

#ifdef FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_concat_bytes(garray_T *gap, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯದಿ (ga_grow(gap, len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    ಪೂರ್ಣಾಙ್ಕ
ga_concat_int(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ n)
{
    ವರ್ಣ    buf[16];
    ಪೂರ್ಣಾಙ್ಕ	    len = vim_snprintf(buf, ಪರಿಮಾಣ(buf), "%d", n);

    ಪ್ರತ್ಯಾಗಮ ga_concat_bytes(gap, buf, len);
}
#endif
