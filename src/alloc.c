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

#অন্তর্ভাব "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#যদি defined(MEM_PROFILE)

# define MEM_SIZES  8200
স্থির long_u mem_allocs[MEM_SIZES];
স্থির long_u mem_frees[MEM_SIZES];
স্থির long_u mem_allocated;
স্থির long_u mem_freed;
স্থির long_u mem_peak;
স্থির long_u অঙ্ক_alloc;
স্থির long_u অঙ্ক_freed;

    স্থির শূন্য
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += আকার(size_t);
}

    স্থির শূন্য
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += আকার(size_t);
}

    স্থির শূন্য
mem_post_alloc(
    শূন্য **pp,
    size_t আকার_2)
{
    যদি (*pp == ফাঁকা)
	ফেরত;
    আকার_2 -= আকার(size_t);
    *(long_u *)*pp = আকার_2;
    যদি (আকার_2 <= MEM_SIZES-1)
	mem_allocs[আকার_2-1]++;
    অন্যথা
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += আকার_2;
    যদি (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    অঙ্ক_alloc++;
    *pp = (শূন্য *)((অক্ষর *)*pp + আকার(size_t));
}

    স্থির শূন্য
mem_pre_free(শূন্য **pp)
{
    long_u আকার_2;

    *pp = (শূন্য *)((অক্ষর *)*pp - আকার(size_t));
    আকার_2 = *(size_t *)*pp;
    যদি (আকার_2 <= MEM_SIZES-1)
	mem_frees[আকার_2-1]++;
    অন্যথা
	mem_frees[MEM_SIZES-1]++;
    mem_freed += আকার_2;
    অঙ্ক_freed++;
}

/*
 * called on exit via atexit()
 */
    শূন্য
vim_mem_profile_dump(শূন্য)
{
    পূর্ণসংখ্যা i, j;

    printf("\r\n");
    j = 0;
    জন্য (i = 0; i < MEM_SIZES - 1; i++)
    {
	যদি (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    চালু;

	যদি (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	যদি (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    যদি (mem_allocs[i])
    {
	printf("\r\n");
	যদি (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    অঙ্ক_alloc, অঙ্ক_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    পূর্ণসংখ্যা
alloc_does_fail(size_t আকার_2)
{
    যদি (alloc_fail_countdown == 0)
    {
	যদি (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_বার্তা(আকার_2);
	ফেরত TRUE;
    }
    --alloc_fail_countdown;
    ফেরত FALSE;
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
    শূন্য *
alloc(size_t আকার_2)
{
    ফেরত lalloc(আকার_2, TRUE);
}

#যদি defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    শূন্য *
alloc_id(size_t আকার_2, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার_2))
	ফেরত ফাঁকা;
# endif
    ফেরত lalloc(আকার_2, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    শূন্য *
alloc_clear(size_t আকার_2)
{
    শূন্য *p;

    p = lalloc(আকার_2, TRUE);
    যদি (p != ফাঁকা)
	(শূন্য)vim_memset(p, 0, আকার_2);
    ফেরত p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    শূন্য *
alloc_clear_id(size_t আকার_2, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার_2))
	ফেরত ফাঁকা;
#endif
    ফেরত alloc_clear(আকার_2);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    শূন্য *
lalloc_clear(size_t আকার_2, পূর্ণসংখ্যা message)
{
    শূন্য *p;

    p = lalloc(আকার_2, message);
    যদি (p != ফাঁকা)
	(শূন্য)vim_memset(p, 0, আকার_2);
    ফেরত p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    শূন্য *
lalloc(size_t আকার_2, পূর্ণসংখ্যা message)
{
    শূন্য	*p;		    // pointer to new storage space
    স্থির পূর্ণসংখ্যা	releasing = FALSE;  // don't do mf_release_all() recursive
    পূর্ণসংখ্যা		try_again;
#যদি defined(HAVE_AVAIL_MEM)
    স্থির size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    যদি (আকার_2 == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	ফেরত ফাঁকা;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&আকার_2);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    জন্য (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	যদি ((p = malloc(আকার_2)) != ফাঁকা)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    যাও theend;
#অন্যথা
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += আকার_2;
	    যদি (allocated < KEEP_ROOM / 2)
		যাও theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    যদি (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		মুক্ত(p);	// System is low... no go!
		p = ফাঁকা;
	    }
	    অন্যথা
		যাও theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	যদি (releasing)
	    বিরতি;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	যদি (!try_again)
	    বিরতি;
    }

    যদি (message && p == ফাঁকা)
	do_outofmem_বার্তা(আকার_2);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, আকার_2);
#endif
    ফেরত p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#যদি defined(FEAT_SIGNS)
    শূন্য *
lalloc_id(size_t আকার_2, পূর্ণসংখ্যা message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার_2))
	ফেরত ফাঁকা;
# endif
    ফেরত (lalloc(আকার_2, message));
}
#endif

#যদি defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    শূন্য *
mem_realloc(শূন্য *ptr, size_t আকার_2)
{
    শূন্য *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&আকার_2);

    p = realloc(ptr, আকার_2);

    mem_post_alloc(&p, আকার_2);

    ফেরত p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    শূন্য
do_outofmem_বার্তা(size_t আকার_2)
{
    যদি (did_outofmem_বার্তা)
	ফেরত;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_বার্তা = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)আকার_2);

    যদি (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#যদি defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    শূন্য
free_all_mem(শূন্য)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    যদি (entered_free_all_mem)
	ফেরত;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    যদি (first_tabpage != ফাঁকা && first_tabpage->tp_পরবর্তী != ফাঁকা)
	do_cmdline_cmd((char_u *)"tabonly!");
    যদি (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# যদি defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# যদি defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    যদি (curwin != ফাঁকা)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ফাঁকা);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    যদি (curbuf != ফাঁকা)
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
# যদি defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# যদি defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# যদি defined(FEAT_KEYMAP)
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
    free_অনুসন্ধান_patterns();
    free_old_sub();
    free_last_ঢোকাও();
    free_insexpand_stuff();
    free_পূর্ব_shellcmd();
    free_regexp_stuff();
    free_tag_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    set_রাশিমালা_সারি(ফাঁকা, ফাঁকা);
# endif
# ifdef FEAT_DIFF
    যদি (curtab != ফাঁকা)
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
    set_keep_বার্তা(ফাঁকা, 0);

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

    যদি (curwin != ফাঁকা)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    জন্য (buf = firstbuf; buf != ফাঁকা; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_পরবর্তী;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_বাফার(ফাঁকা, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	যদি (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	অন্যথা
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# যদি defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    যদি (first_tabpage != ফাঁকা)
    {
	free_tabpage(first_tabpage);
	first_tabpage = ফাঁকা;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    জন্য (;;)
	যদি (delete_first_বার্তা() == FAIL)
	    বিরতি;

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

# যদি defined(FEAT_SOUND)
    sound_free();
# endif
# যদি defined(USE_XSMP)
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
    free_resub_eval_ফলাফল();
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

    যদি (ret != ফাঁকা)
	mch_memmove(ret, p, len);
    ফেরত ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    শূন্য
vim_free(শূন্য *x)
{
    যদি (x != ফাঁকা && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	মুক্ত(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    শূন্য
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_তথ্য);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    শূন্য
ga_clear_strings(garray_T *gap)
{
    পূর্ণসংখ্যা		i;

    যদি (gap->ga_তথ্য != ফাঁকা)
	জন্য (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_তথ্য))[i]);
    ga_clear(gap);
}

#যদি defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    পূর্ণসংখ্যা
ga_copy_strings(garray_T *from, garray_T *to)
{
    পূর্ণসংখ্যা		i;

    ga_init2(to, আকার(char_u *), 1);
    যদি (ga_grow(to, from->ga_len) == FAIL)
	ফেরত FAIL;

    জন্য (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_তথ্য)[i];
	char_u *copy;

	যদি (orig == ফাঁকা)
	    copy = ফাঁকা;
	অন্যথা
	{
	    copy = vim_strsave(orig);
	    যদি (copy == ফাঁকা)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		ফেরত FAIL;
	    }
	}
	((char_u **)to->ga_তথ্য)[i] = copy;
    }
    to->ga_len = from->ga_len;
    ফেরত OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    শূন্য
ga_init(garray_T *gap)
{
    gap->ga_তথ্য = ফাঁকা;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    শূন্য
ga_init2(garray_T *gap, size_t itemsize, পূর্ণসংখ্যা growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (পূর্ণসংখ্যা)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণসংখ্যা
ga_grow(garray_T *gap, পূর্ণসংখ্যা n)
{
    যদি (gap->ga_maxlen - gap->ga_len < n)
	ফেরত ga_grow_inner(gap, n);
    ফেরত OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    পূর্ণসংখ্যা
ga_grow_id(garray_T *gap, পূর্ণসংখ্যা n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার(list_T)))
	ফেরত FAIL;
#endif

    ফেরত ga_grow(gap, n);
}

    পূর্ণসংখ্যা
ga_grow_inner(garray_T *gap, পূর্ণসংখ্যা n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    যদি (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    যদি (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    যদি (n > 0 && (size_t)(gap->ga_len + n) > আকার_MAX / gap->ga_itemsize)
	ফেরত FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_তথ্য, new_len);
    যদি (pp == ফাঁকা)
	ফেরত FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_তথ্য = pp;
    ফেরত OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, অক্ষর *sep)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		sep_len = (পূর্ণসংখ্যা)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    জন্য (i = 0; i < gap->ga_len; ++i)
	len += (পূর্ণসংখ্যা)STRLEN(((char_u **)(gap->ga_তথ্য))[i]);

    যদি (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    যদি (s == ফাঁকা)
	ফেরত ফাঁকা;

    *s = NUL;
    p = s;
    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	যদি (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_তথ্য))[i]);
	p += STRLEN(p);
    }
    ফেরত s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    পূর্ণসংখ্যা
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    যদি (cp == ফাঁকা)
	ফেরত FAIL;

    যদি (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ফেরত FAIL;
    }
    ((char_u **)(gap->ga_তথ্য))[gap->ga_len++] = cp;
    ফেরত OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    পূর্ণসংখ্যা
ga_add_string(garray_T *gap, char_u *p)
{
    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত FAIL;
    ((char_u **)(gap->ga_তথ্য))[gap->ga_len++] = p;
    ফেরত OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    শূন্য
ga_concat(garray_T *gap, char_u *s)
{
    পূর্ণসংখ্যা    len;

    যদি (s == ফাঁকা || *s == NUL)
	ফেরত;
    len = (পূর্ণসংখ্যা)STRLEN(s);
    যদি (ga_grow(gap, len) == OK)
    {
	mch_memmove((অক্ষর *)gap->ga_তথ্য + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    শূন্য
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    যদি (s == ফাঁকা || *s == NUL || len == 0)
	ফেরত;
    যদি (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	ফেরত;
    }
    যদি (ga_grow(gap, (পূর্ণসংখ্যা)len) == OK)
    {
	mch_memmove((অক্ষর *)gap->ga_তথ্য + gap->ga_len, s, len);
	gap->ga_len += (পূর্ণসংখ্যা)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    পূর্ণসংখ্যা
ga_append(garray_T *gap, পূর্ণসংখ্যা c)
{
    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত FAIL;
    *((অক্ষর *)gap->ga_তথ্য + gap->ga_len) = c;
    ++gap->ga_len;
    ফেরত OK;
}

#যদি (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    শূন্য
append_ga_সারি(garray_T *gap)
{
    // Remove trailing CR.
    যদি (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_তথ্য)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_তথ্য, 0, FALSE);
    gap->ga_len = 0;
}
#endif

#ifdef FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    পূর্ণসংখ্যা
ga_concat_bytes(garray_T *gap, ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা len)
{
    যদি (ga_grow(gap, len) == FAIL)
	ফেরত FAIL;
    mch_memmove((char_u *)gap->ga_তথ্য + gap->ga_len, s, len);
    gap->ga_len += len;
    ফেরত OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    পূর্ণসংখ্যা
ga_concat_int(garray_T *gap, পূর্ণসংখ্যা n)
{
    অক্ষর    buf[16];
    পূর্ণসংখ্যা	    len = vim_snprintf(buf, আকার(buf), "%d", n);

    ফেরত ga_concat_bytes(gap, buf, len);
}
#endif
