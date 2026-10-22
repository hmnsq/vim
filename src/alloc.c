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

#അന്തര്ഭാവ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#എങ്കിൽ defined(MEM_PROFILE)

# define MEM_SIZES  8200
സ്ഥിതം long_u mem_allocs[MEM_SIZES];
സ്ഥിതം long_u mem_frees[MEM_SIZES];
സ്ഥിതം long_u mem_allocated;
സ്ഥിതം long_u mem_freed;
സ്ഥിതം long_u mem_peak;
സ്ഥിതം long_u അക്കം_alloc;
സ്ഥിതം long_u അക്കം_freed;

    സ്ഥിതം ശൂന്യം
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += വലുപ്പം(size_t);
}

    സ്ഥിതം ശൂന്യം
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += വലുപ്പം(size_t);
}

    സ്ഥിതം ശൂന്യം
mem_post_alloc(
    ശൂന്യം **pp,
    size_t size)
{
    എങ്കിൽ (*pp == ശൂന്യ)
	തിരികെ;
    size -= വലുപ്പം(size_t);
    *(long_u *)*pp = size;
    എങ്കിൽ (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    അല്ലെങ്കിൽ
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    എങ്കിൽ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    അക്കം_alloc++;
    *pp = (ശൂന്യം *)((അക്ഷരം *)*pp + വലുപ്പം(size_t));
}

    സ്ഥിതം ശൂന്യം
mem_pre_free(ശൂന്യം **pp)
{
    long_u size;

    *pp = (ശൂന്യം *)((അക്ഷരം *)*pp - വലുപ്പം(size_t));
    size = *(size_t *)*pp;
    എങ്കിൽ (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    അല്ലെങ്കിൽ
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    അക്കം_freed++;
}

/*
 * called on exit via atexit()
 */
    ശൂന്യം
vim_mem_profile_dump(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i, j;

    printf("\r\n");
    j = 0;
    വേണ്ടി (i = 0; i < MEM_SIZES - 1; i++)
    {
	എങ്കിൽ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    തുടരുക;

	എങ്കിൽ (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	എങ്കിൽ (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    എങ്കിൽ (mem_allocs[i])
    {
	printf("\r\n");
	എങ്കിൽ (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    അക്കം_alloc, അക്കം_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ
alloc_does_fail(size_t size)
{
    എങ്കിൽ (alloc_fail_countdown == 0)
    {
	എങ്കിൽ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_സന്ദേശം(size);
	തിരികെ TRUE;
    }
    --alloc_fail_countdown;
    തിരികെ FALSE;
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
    ശൂന്യം *
alloc(size_t size)
{
    തിരികെ lalloc(size, TRUE);
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    ശൂന്യം *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(size))
	തിരികെ ശൂന്യ;
# endif
    തിരികെ lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    ശൂന്യം *
alloc_clear(size_t size)
{
    ശൂന്യം *p;

    p = lalloc(size, TRUE);
    എങ്കിൽ (p != ശൂന്യ)
	(ശൂന്യം)vim_memset(p, 0, size);
    തിരികെ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    ശൂന്യം *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(size))
	തിരികെ ശൂന്യ;
#endif
    തിരികെ alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    ശൂന്യം *
lalloc_clear(size_t size, പൂർണ്ണസംഖ്യ message)
{
    ശൂന്യം *p;

    p = lalloc(size, message);
    എങ്കിൽ (p != ശൂന്യ)
	(ശൂന്യം)vim_memset(p, 0, size);
    തിരികെ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    ശൂന്യം *
lalloc(size_t size, പൂർണ്ണസംഖ്യ message)
{
    ശൂന്യം	*p;		    // pointer to new storage space
    സ്ഥിതം പൂർണ്ണസംഖ്യ	releasing = FALSE;  // don't do mf_release_all() recursive
    പൂർണ്ണസംഖ്യ		try_again;
#എങ്കിൽ defined(HAVE_AVAIL_MEM)
    സ്ഥിതം size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    എങ്കിൽ (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	തിരികെ ശൂന്യ;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    വേണ്ടി (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	എങ്കിൽ ((p = malloc(size)) != ശൂന്യ)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    പോകുക theend;
#അല്ലെങ്കിൽ
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    എങ്കിൽ (allocated < KEEP_ROOM / 2)
		പോകുക theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    എങ്കിൽ (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		സ്വതന്ത്രം(p);	// System is low... no go!
		p = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
		പോകുക theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	എങ്കിൽ (releasing)
	    നിർത്തുക;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	എങ്കിൽ (!try_again)
	    നിർത്തുക;
    }

    എങ്കിൽ (message && p == ശൂന്യ)
	do_outofmem_സന്ദേശം(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    തിരികെ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#എങ്കിൽ defined(FEAT_SIGNS)
    ശൂന്യം *
lalloc_id(size_t size, പൂർണ്ണസംഖ്യ message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(size))
	തിരികെ ശൂന്യ;
# endif
    തിരികെ (lalloc(size, message));
}
#endif

#എങ്കിൽ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    ശൂന്യം *
mem_realloc(ശൂന്യം *ptr, size_t size)
{
    ശൂന്യം *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    തിരികെ p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    ശൂന്യം
do_outofmem_സന്ദേശം(size_t size)
{
    എങ്കിൽ (did_outofmem_സന്ദേശം)
	തിരികെ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_സന്ദേശം = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    എങ്കിൽ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#എങ്കിൽ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    ശൂന്യം
free_all_mem(ശൂന്യം)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    എങ്കിൽ (entered_free_all_mem)
	തിരികെ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    എങ്കിൽ (first_tabpage != ശൂന്യ && first_tabpage->tp_next != ശൂന്യ)
	do_cmdline_cmd((char_u *)"tabonly!");
    എങ്കിൽ (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# എങ്കിൽ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# എങ്കിൽ defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    എങ്കിൽ (curwin != ശൂന്യ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ശൂന്യ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    എങ്കിൽ (curbuf != ശൂന്യ)
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
# എങ്കിൽ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# എങ്കിൽ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# എങ്കിൽ defined(FEAT_KEYMAP)
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
    free_തിരയൽ_patterns();
    free_old_sub();
    free_last_തിരുകുക();
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
    set_വ്യഞ്ജകം_വരി(ശൂന്യ, ശൂന്യ);
# endif
# ifdef FEAT_DIFF
    എങ്കിൽ (curtab != ശൂന്യ)
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
    set_keep_സന്ദേശം(ശൂന്യ, 0);

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

    എങ്കിൽ (curwin != ശൂന്യ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    വേണ്ടി (buf = firstbuf; buf != ശൂന്യ; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(ശൂന്യ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	എങ്കിൽ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	അല്ലെങ്കിൽ
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# എങ്കിൽ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    എങ്കിൽ (first_tabpage != ശൂന്യ)
    {
	free_tabpage(first_tabpage);
	first_tabpage = ശൂന്യ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    വേണ്ടി (;;)
	എങ്കിൽ (delete_first_സന്ദേശം() == FAIL)
	    നിർത്തുക;

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

# എങ്കിൽ defined(FEAT_SOUND)
    sound_free();
# endif
# എങ്കിൽ defined(USE_XSMP)
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

    എങ്കിൽ (ret != ശൂന്യ)
	mch_memmove(ret, p, len);
    തിരികെ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    ശൂന്യം
vim_free(ശൂന്യം *x)
{
    എങ്കിൽ (x != ശൂന്യ && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	സ്വതന്ത്രം(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    ശൂന്യം
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    ശൂന്യം
ga_clear_strings(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (gap->ga_data != ശൂന്യ)
	വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    പൂർണ്ണസംഖ്യ
ga_copy_strings(garray_T *from, garray_T *to)
{
    പൂർണ്ണസംഖ്യ		i;

    ga_init2(to, വലുപ്പം(char_u *), 1);
    എങ്കിൽ (ga_grow(to, from->ga_len) == FAIL)
	തിരികെ FAIL;

    വേണ്ടി (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	എങ്കിൽ (orig == ശൂന്യ)
	    copy = ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    copy = vim_strsave(orig);
	    എങ്കിൽ (copy == ശൂന്യ)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		തിരികെ FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    തിരികെ OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    ശൂന്യം
ga_init(garray_T *gap)
{
    gap->ga_data = ശൂന്യ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    ശൂന്യം
ga_init2(garray_T *gap, size_t itemsize, പൂർണ്ണസംഖ്യ growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (പൂർണ്ണസംഖ്യ)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
ga_grow(garray_T *gap, പൂർണ്ണസംഖ്യ n)
{
    എങ്കിൽ (gap->ga_maxlen - gap->ga_len < n)
	തിരികെ ga_grow_inner(gap, n);
    തിരികെ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    പൂർണ്ണസംഖ്യ
ga_grow_id(garray_T *gap, പൂർണ്ണസംഖ്യ n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(വലുപ്പം(list_T)))
	തിരികെ FAIL;
#endif

    തിരികെ ga_grow(gap, n);
}

    പൂർണ്ണസംഖ്യ
ga_grow_inner(garray_T *gap, പൂർണ്ണസംഖ്യ n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    എങ്കിൽ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    എങ്കിൽ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    എങ്കിൽ (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	തിരികെ FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    എങ്കിൽ (pp == ശൂന്യ)
	തിരികെ FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    തിരികെ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, അക്ഷരം *sep)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		sep_len = (പൂർണ്ണസംഖ്യ)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	len += (പൂർണ്ണസംഖ്യ)STRLEN(((char_u **)(gap->ga_data))[i]);

    എങ്കിൽ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ ശൂന്യ;

    *s = NUL;
    p = s;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	എങ്കിൽ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    തിരികെ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    പൂർണ്ണസംഖ്യ
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    എങ്കിൽ (cp == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	തിരികെ FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    തിരികെ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    പൂർണ്ണസംഖ്യ
ga_add_string(garray_T *gap, char_u *p)
{
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    തിരികെ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    ശൂന്യം
ga_concat(garray_T *gap, char_u *s)
{
    പൂർണ്ണസംഖ്യ    len;

    എങ്കിൽ (s == ശൂന്യ || *s == NUL)
	തിരികെ;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(s);
    എങ്കിൽ (ga_grow(gap, len) == OK)
    {
	mch_memmove((അക്ഷരം *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    ശൂന്യം
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    എങ്കിൽ (s == ശൂന്യ || *s == NUL || len == 0)
	തിരികെ;
    എങ്കിൽ (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	തിരികെ;
    }
    എങ്കിൽ (ga_grow(gap, (പൂർണ്ണസംഖ്യ)len) == OK)
    {
	mch_memmove((അക്ഷരം *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (പൂർണ്ണസംഖ്യ)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    പൂർണ്ണസംഖ്യ
ga_append(garray_T *gap, പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    *((അക്ഷരം *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    തിരികെ OK;
}

#എങ്കിൽ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    ശൂന്യം
append_ga_വരി(garray_T *gap)
{
    // Remove trailing CR.
    എങ്കിൽ (gap->ga_len > 0
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
    പൂർണ്ണസംഖ്യ
ga_concat_bytes(garray_T *gap, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ len)
{
    എങ്കിൽ (ga_grow(gap, len) == FAIL)
	തിരികെ FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    തിരികെ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    പൂർണ്ണസംഖ്യ
ga_concat_int(garray_T *gap, പൂർണ്ണസംഖ്യ n)
{
    അക്ഷരം    buf[16];
    പൂർണ്ണസംഖ്യ	    len = vim_snprintf(buf, വലുപ്പം(buf), "%d", n);

    തിരികെ ga_concat_bytes(gap, buf, len);
}
#endif
