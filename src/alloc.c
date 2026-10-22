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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#ଯଦି defined(MEM_PROFILE)

# define MEM_SIZES  8200
ସ୍ଥିର long_u mem_allocs[MEM_SIZES];
ସ୍ଥିର long_u mem_frees[MEM_SIZES];
ସ୍ଥିର long_u mem_allocated;
ସ୍ଥିର long_u mem_freed;
ସ୍ଥିର long_u mem_peak;
ସ୍ଥିର long_u ଅଙ୍କ_alloc;
ସ୍ଥିର long_u ଅଙ୍କ_freed;

    ସ୍ଥିର ଶୂନ୍ୟ
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += ଆକାର(size_t);
}

    ସ୍ଥିର ଶୂନ୍ୟ
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += ଆକାର(size_t);
}

    ସ୍ଥିର ଶୂନ୍ୟ
mem_post_alloc(
    ଶୂନ୍ୟ **pp,
    size_t size)
{
    ଯଦି (*pp == ଖାଲି)
	ଫେରନ୍ତୁ;
    size -= ଆକାର(size_t);
    *(long_u *)*pp = size;
    ଯଦି (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    ଅନ୍ୟଥା
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    ଯଦି (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    ଅଙ୍କ_alloc++;
    *pp = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)*pp + ଆକାର(size_t));
}

    ସ୍ଥିର ଶୂନ୍ୟ
mem_pre_free(ଶୂନ୍ୟ **pp)
{
    long_u size;

    *pp = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)*pp - ଆକାର(size_t));
    size = *(size_t *)*pp;
    ଯଦି (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    ଅନ୍ୟଥା
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    ଅଙ୍କ_freed++;
}

/*
 * called on exit via atexit()
 */
    ଶୂନ୍ୟ
vim_mem_profile_dump(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;

    printf("\r\n");
    j = 0;
    ପାଇଁ (i = 0; i < MEM_SIZES - 1; i++)
    {
	ଯଦି (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ଜାରି;

	ଯଦି (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	ଯଦି (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    ଯଦି (mem_allocs[i])
    {
	printf("\r\n");
	ଯଦି (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    ଅଙ୍କ_alloc, ଅଙ୍କ_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
alloc_does_fail(size_t size)
{
    ଯଦି (alloc_fail_countdown == 0)
    {
	ଯଦି (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_ସନ୍ଦେଶ(size);
	ଫେରନ୍ତୁ TRUE;
    }
    --alloc_fail_countdown;
    ଫେରନ୍ତୁ FALSE;
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
    ଶୂନ୍ୟ *
alloc(size_t size)
{
    ଫେରନ୍ତୁ lalloc(size, TRUE);
}

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    ଶୂନ୍ୟ *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(size))
	ଫେରନ୍ତୁ ଖାଲି;
# endif
    ଫେରନ୍ତୁ lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    ଶୂନ୍ୟ *
alloc_clear(size_t size)
{
    ଶୂନ୍ୟ *p;

    p = lalloc(size, TRUE);
    ଯଦି (p != ଖାଲି)
	(ଶୂନ୍ୟ)vim_memset(p, 0, size);
    ଫେରନ୍ତୁ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    ଶୂନ୍ୟ *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(size))
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    ଫେରନ୍ତୁ alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    ଶୂନ୍ୟ *
lalloc_clear(size_t size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା message)
{
    ଶୂନ୍ୟ *p;

    p = lalloc(size, message);
    ଯଦି (p != ଖାଲି)
	(ଶୂନ୍ୟ)vim_memset(p, 0, size);
    ଫେରନ୍ତୁ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    ଶୂନ୍ୟ *
lalloc(size_t size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା message)
{
    ଶୂନ୍ୟ	*p;		    // pointer to new storage space
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	releasing = FALSE;  // don't do mf_release_all() recursive
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try_again;
#ଯଦି defined(HAVE_AVAIL_MEM)
    ସ୍ଥିର size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    ଯଦି (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    ପାଇଁ (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	ଯଦି ((p = malloc(size)) != ଖାଲି)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    ଯାଅ theend;
#ଅନ୍ୟଥା
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    ଯଦି (allocated < KEEP_ROOM / 2)
		ଯାଅ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    ଯଦି (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		ମୁକ୍ତ(p);	// System is low... no go!
		p = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		ଯାଅ theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	ଯଦି (releasing)
	    ଭାଙ୍ଗ;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	ଯଦି (!try_again)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (message && p == ଖାଲି)
	do_outofmem_ସନ୍ଦେଶ(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    ଫେରନ୍ତୁ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#ଯଦି defined(FEAT_SIGNS)
    ଶୂନ୍ୟ *
lalloc_id(size_t size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(size))
	ଫେରନ୍ତୁ ଖାଲି;
# endif
    ଫେରନ୍ତୁ (lalloc(size, message));
}
#endif

#ଯଦି defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    ଶୂନ୍ୟ *
mem_realloc(ଶୂନ୍ୟ *ptr, size_t size)
{
    ଶୂନ୍ୟ *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    ଫେରନ୍ତୁ p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    ଶୂନ୍ୟ
do_outofmem_ସନ୍ଦେଶ(size_t size)
{
    ଯଦି (did_outofmem_ସନ୍ଦେଶ)
	ଫେରନ୍ତୁ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_ସନ୍ଦେଶ = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    ଯଦି (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#ଯଦି defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    ଶୂନ୍ୟ
free_all_mem(ଶୂନ୍ୟ)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    ଯଦି (entered_free_all_mem)
	ଫେରନ୍ତୁ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    ଯଦି (first_tabpage != ଖାଲି && first_tabpage->tp_next != ଖାଲି)
	do_cmdline_cmd((char_u *)"tabonly!");
    ଯଦି (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# ଯଦି defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# ଯଦି defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    ଯଦି (curwin != ଖାଲି)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ଖାଲି);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    ଯଦି (curbuf != ଖାଲି)
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
# ଯଦି defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# ଯଦି defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# ଯଦି defined(FEAT_KEYMAP)
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
    free_ସନ୍ଧାନ_patterns();
    free_old_sub();
    free_last_ଭର୍ତ୍ତି();
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
    set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(ଖାଲି, ଖାଲି);
# endif
# ifdef FEAT_DIFF
    ଯଦି (curtab != ଖାଲି)
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
    set_keep_ସନ୍ଦେଶ(ଖାଲି, 0);

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

    ଯଦି (curwin != ଖାଲି)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    ପାଇଁ (buf = firstbuf; buf != ଖାଲି; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(ଖାଲି, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	ଯଦି (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	ଅନ୍ୟଥା
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# ଯଦି defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    ଯଦି (first_tabpage != ଖାଲି)
    {
	free_tabpage(first_tabpage);
	first_tabpage = ଖାଲି;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    ପାଇଁ (;;)
	ଯଦି (delete_first_ସନ୍ଦେଶ() == FAIL)
	    ଭାଙ୍ଗ;

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

# ଯଦି defined(FEAT_SOUND)
    sound_free();
# endif
# ଯଦି defined(USE_XSMP)
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

    ଯଦି (ret != ଖାଲି)
	mch_memmove(ret, p, len);
    ଫେରନ୍ତୁ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    ଶୂନ୍ୟ
vim_free(ଶୂନ୍ୟ *x)
{
    ଯଦି (x != ଖାଲି && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	ମୁକ୍ତ(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    ଶୂନ୍ୟ
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    ଶୂନ୍ୟ
ga_clear_strings(garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (gap->ga_data != ଖାଲି)
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_copy_strings(garray_T *from, garray_T *to)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ga_init2(to, ଆକାର(char_u *), 1);
    ଯଦି (ga_grow(to, from->ga_len) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	ଯଦି (orig == ଖାଲି)
	    copy = ଖାଲି;
	ଅନ୍ୟଥା
	{
	    copy = vim_strsave(orig);
	    ଯଦି (copy == ଖାଲି)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    ଶୂନ୍ୟ
ga_init(garray_T *gap)
{
    gap->ga_data = ଖାଲି;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    ଶୂନ୍ୟ
ga_init2(garray_T *gap, size_t itemsize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_grow(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ଯଦି (gap->ga_maxlen - gap->ga_len < n)
	ଫେରନ୍ତୁ ga_grow_inner(gap, n);
    ଫେରନ୍ତୁ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_grow_id(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(ଆକାର(list_T)))
	ଫେରନ୍ତୁ FAIL;
#endif

    ଫେରନ୍ତୁ ga_grow(gap, n);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_grow_inner(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    ଯଦି (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    ଯଦି (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    ଯଦି (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	ଫେରନ୍ତୁ FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    ଯଦି (pp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    ଫେରନ୍ତୁ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, ଅକ୍ଷର *sep)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sep_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(((char_u **)(gap->ga_data))[i]);

    ଯଦି (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    *s = NUL;
    p = s;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	ଯଦି (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    ଫେରନ୍ତୁ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    ଯଦି (cp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ଫେରନ୍ତୁ FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    ଫେରନ୍ତୁ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_add_string(garray_T *gap, char_u *p)
{
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    ଫେରନ୍ତୁ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    ଶୂନ୍ୟ
ga_concat(garray_T *gap, char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    len;

    ଯଦି (s == ଖାଲି || *s == NUL)
	ଫେରନ୍ତୁ;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
    ଯଦି (ga_grow(gap, len) == OK)
    {
	mch_memmove((ଅକ୍ଷର *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    ଶୂନ୍ୟ
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    ଯଦି (s == ଖାଲି || *s == NUL || len == 0)
	ଫେରନ୍ତୁ;
    ଯଦି (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len) == OK)
    {
	mch_memmove((ଅକ୍ଷର *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_append(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    *((ଅକ୍ଷର *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    ଫେରନ୍ତୁ OK;
}

#ଯଦି (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    ଶୂନ୍ୟ
append_ga_ଧାଡ଼ି(garray_T *gap)
{
    // Remove trailing CR.
    ଯଦି (gap->ga_len > 0
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_concat_bytes(garray_T *gap, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯଦି (ga_grow(gap, len) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_concat_int(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ଅକ୍ଷର    buf[16];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = vim_snprintf(buf, ଆକାର(buf), "%d", n);

    ଫେରନ୍ତୁ ga_concat_bytes(gap, buf, len);
}
#endif
