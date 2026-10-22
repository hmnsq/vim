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
ایستا long_u mem_allocs[MEM_SIZES];
ایستا long_u mem_frees[MEM_SIZES];
ایستا long_u mem_allocated;
ایستا long_u mem_freed;
ایستا long_u mem_peak;
ایستا long_u رقم_alloc;
ایستا long_u رقم_freed;

    ایستا تهی
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += اندازه(size_t);
}

    ایستا تهی
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += اندازه(size_t);
}

    ایستا تهی
mem_post_alloc(
    تهی **pp,
    size_t size)
{
    اگر (*pp == پوچ)
	بازگشت;
    size -= اندازه(size_t);
    *(long_u *)*pp = size;
    اگر (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    وگرنه
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    اگر (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    رقم_alloc++;
    *pp = (تهی *)((نویسه *)*pp + اندازه(size_t));
}

    ایستا تهی
mem_pre_free(تهی **pp)
{
    long_u size;

    *pp = (تهی *)((نویسه *)*pp - اندازه(size_t));
    size = *(size_t *)*pp;
    اگر (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    وگرنه
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    رقم_freed++;
}

/*
 * called on exit via atexit()
 */
    تهی
vim_mem_profile_dump(تهی)
{
    صحیح i, j;

    printf("\r\n");
    j = 0;
    برای (i = 0; i < MEM_SIZES - 1; i++)
    {
	اگر (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ادامه;

	اگر (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	اگر (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    اگر (mem_allocs[i])
    {
	printf("\r\n");
	اگر (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    رقم_alloc, رقم_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    صحیح
alloc_does_fail(size_t size)
{
    اگر (alloc_fail_countdown == 0)
    {
	اگر (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_پیام(size);
	بازگشت TRUE;
    }
    --alloc_fail_countdown;
    بازگشت FALSE;
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
    تهی *
alloc(size_t size)
{
    بازگشت lalloc(size, TRUE);
}

#اگر defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    تهی *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(size))
	بازگشت پوچ;
# endif
    بازگشت lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    تهی *
alloc_clear(size_t size)
{
    تهی *p;

    p = lalloc(size, TRUE);
    اگر (p != پوچ)
	(تهی)vim_memset(p, 0, size);
    بازگشت p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    تهی *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(size))
	بازگشت پوچ;
#endif
    بازگشت alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    تهی *
lalloc_clear(size_t size, صحیح message)
{
    تهی *p;

    p = lalloc(size, message);
    اگر (p != پوچ)
	(تهی)vim_memset(p, 0, size);
    بازگشت p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    تهی *
lalloc(size_t size, صحیح message)
{
    تهی	*p;		    // pointer to new storage space
    ایستا صحیح	releasing = FALSE;  // don't do mf_release_all() recursive
    صحیح		try_again;
#اگر defined(HAVE_AVAIL_MEM)
    ایستا size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    اگر (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	بازگشت پوچ;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    برای (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	اگر ((p = malloc(size)) != پوچ)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    برو theend;
#وگرنه
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    اگر (allocated < KEEP_ROOM / 2)
		برو theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    اگر (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		آزاد(p);	// System is low... no go!
		p = پوچ;
	    }
	    وگرنه
		برو theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	اگر (releasing)
	    بشکن;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	اگر (!try_again)
	    بشکن;
    }

    اگر (message && p == پوچ)
	do_outofmem_پیام(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    بازگشت p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#اگر defined(FEAT_SIGNS)
    تهی *
lalloc_id(size_t size, صحیح message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(size))
	بازگشت پوچ;
# endif
    بازگشت (lalloc(size, message));
}
#endif

#اگر defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    تهی *
mem_realloc(تهی *ptr, size_t size)
{
    تهی *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    بازگشت p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    تهی
do_outofmem_پیام(size_t size)
{
    اگر (did_outofmem_پیام)
	بازگشت;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_پیام = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

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
    تهی
free_all_mem(تهی)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    اگر (entered_free_all_mem)
	بازگشت;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    اگر (first_tabpage != پوچ && first_tabpage->tp_next != پوچ)
	do_cmdline_cmd((char_u *)"tabonly!");
    اگر (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# اگر defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# اگر defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    اگر (curwin != پوچ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(پوچ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    اگر (curbuf != پوچ)
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
    free_جستجو_patterns();
    free_old_sub();
    free_last_درج();
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
    set_عبارت_سطر(پوچ, پوچ);
# endif
# ifdef FEAT_DIFF
    اگر (curtab != پوچ)
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
    set_keep_پیام(پوچ, 0);

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

    اگر (curwin != پوچ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    برای (buf = firstbuf; buf != پوچ; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(پوچ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	اگر (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	وگرنه
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
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    اگر (first_tabpage != پوچ)
    {
	free_tabpage(first_tabpage);
	first_tabpage = پوچ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    برای (;;)
	اگر (delete_first_پیام() == FAIL)
	    بشکن;

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

    اگر (ret != پوچ)
	mch_memmove(ret, p, len);
    بازگشت ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    تهی
vim_free(تهی *x)
{
    اگر (x != پوچ && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	آزاد(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    تهی
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    تهی
ga_clear_strings(garray_T *gap)
{
    صحیح		i;

    اگر (gap->ga_data != پوچ)
	برای (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#اگر defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    صحیح
ga_copy_strings(garray_T *from, garray_T *to)
{
    صحیح		i;

    ga_init2(to, اندازه(char_u *), 1);
    اگر (ga_grow(to, from->ga_len) == FAIL)
	بازگشت FAIL;

    برای (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	اگر (orig == پوچ)
	    copy = پوچ;
	وگرنه
	{
	    copy = vim_strsave(orig);
	    اگر (copy == پوچ)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		بازگشت FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    بازگشت OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    تهی
ga_init(garray_T *gap)
{
    gap->ga_data = پوچ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    تهی
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
	بازگشت ga_grow_inner(gap, n);
    بازگشت OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    صحیح
ga_grow_id(garray_T *gap, صحیح n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(اندازه(list_T)))
	بازگشت FAIL;
#endif

    بازگشت ga_grow(gap, n);
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

    اگر (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	بازگشت FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    اگر (pp == پوچ)
	بازگشت FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    بازگشت OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, نویسه *sep)
{
    صحیح		i;
    صحیح		len = 0;
    صحیح		sep_len = (صحیح)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    برای (i = 0; i < gap->ga_len; ++i)
	len += (صحیح)STRLEN(((char_u **)(gap->ga_data))[i]);

    اگر (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    اگر (s == پوچ)
	بازگشت پوچ;

    *s = NUL;
    p = s;
    برای (i = 0; i < gap->ga_len; ++i)
    {
	اگر (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    بازگشت s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    صحیح
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    اگر (cp == پوچ)
	بازگشت FAIL;

    اگر (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	بازگشت FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    بازگشت OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    صحیح
ga_add_string(garray_T *gap, char_u *p)
{
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    بازگشت OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    تهی
ga_concat(garray_T *gap, char_u *s)
{
    صحیح    len;

    اگر (s == پوچ || *s == NUL)
	بازگشت;
    len = (صحیح)STRLEN(s);
    اگر (ga_grow(gap, len) == OK)
    {
	mch_memmove((نویسه *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    تهی
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    اگر (s == پوچ || *s == NUL || len == 0)
	بازگشت;
    اگر (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	بازگشت;
    }
    اگر (ga_grow(gap, (صحیح)len) == OK)
    {
	mch_memmove((نویسه *)gap->ga_data + gap->ga_len, s, len);
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
	بازگشت FAIL;
    *((نویسه *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    بازگشت OK;
}

#اگر (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    تهی
append_ga_سطر(garray_T *gap)
{
    // Remove trailing CR.
    اگر (gap->ga_len > 0
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
    صحیح
ga_concat_bytes(garray_T *gap, ثابت نویسه *s, صحیح len)
{
    اگر (ga_grow(gap, len) == FAIL)
	بازگشت FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    بازگشت OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    صحیح
ga_concat_int(garray_T *gap, صحیح n)
{
    نویسه    buf[16];
    صحیح	    len = vim_snprintf(buf, اندازه(buf), "%d", n);

    بازگشت ga_concat_bytes(gap, buf, len);
}
#endif
