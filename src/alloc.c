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

#ضمن "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#𐡄𐡍 defined(MEM_PROFILE)

# define MEM_SIZES  8200
ساكن long_u mem_allocs[MEM_SIZES];
ساكن long_u mem_frees[MEM_SIZES];
ساكن long_u mem_allocated;
ساكن long_u mem_freed;
ساكن long_u mem_peak;
ساكن long_u num_alloc;
ساكن long_u num_freed;

    ساكن فراغ
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += حجم(size_t);
}

    ساكن فراغ
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += حجم(size_t);
}

    ساكن فراغ
mem_post_alloc(
    فراغ **pp,
    size_t size)
{
    𐡄𐡍 (*pp == عدم)
	أرجع;
    size -= حجم(size_t);
    *(long_u *)*pp = size;
    𐡄𐡍 (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    وإلا
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    𐡄𐡍 (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    num_alloc++;
    *pp = (فراغ *)((حرف *)*pp + حجم(size_t));
}

    ساكن فراغ
mem_pre_free(فراغ **pp)
{
    long_u size;

    *pp = (فراغ *)((حرف *)*pp - حجم(size_t));
    size = *(size_t *)*pp;
    𐡄𐡍 (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    وإلا
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    num_freed++;
}

/*
 * called on exit via atexit()
 */
    فراغ
vim_mem_profile_dump(فراغ)
{
    صحيح i, j;

    printf("\r\n");
    j = 0;
    لكل (i = 0; i < MEM_SIZES - 1; i++)
    {
	𐡄𐡍 (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    استمر;

	𐡄𐡍 (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	𐡄𐡍 (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    𐡄𐡍 (mem_allocs[i])
    {
	printf("\r\n");
	𐡄𐡍 (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    num_alloc, num_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    صحيح
alloc_does_fail(size_t size)
{
    𐡄𐡍 (alloc_fail_countdown == 0)
    {
	𐡄𐡍 (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_msg(size);
	أرجع TRUE;
    }
    --alloc_fail_countdown;
    أرجع FALSE;
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
    فراغ *
alloc(size_t size)
{
    أرجع lalloc(size, TRUE);
}

#𐡄𐡍 defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    فراغ *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    𐡄𐡍 (alloc_fail_id == id && alloc_does_fail(size))
	أرجع عدم;
# endif
    أرجع lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    فراغ *
alloc_clear(size_t size)
{
    فراغ *p;

    p = lalloc(size, TRUE);
    𐡄𐡍 (p != عدم)
	(فراغ)vim_memset(p, 0, size);
    أرجع p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    فراغ *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    𐡄𐡍 (alloc_fail_id == id && alloc_does_fail(size))
	أرجع عدم;
#endif
    أرجع alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    فراغ *
lalloc_clear(size_t size, صحيح message)
{
    فراغ *p;

    p = lalloc(size, message);
    𐡄𐡍 (p != عدم)
	(فراغ)vim_memset(p, 0, size);
    أرجع p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    فراغ *
lalloc(size_t size, صحيح message)
{
    فراغ	*p;		    // pointer to new storage space
    ساكن صحيح	releasing = FALSE;  // don't do mf_release_all() recursive
    صحيح		try_again;
#𐡄𐡍 defined(HAVE_AVAIL_MEM)
    ساكن size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    𐡄𐡍 (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	أرجع عدم;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    لكل (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	𐡄𐡍 ((p = malloc(size)) != عدم)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    اذهب theend;
#وإلا
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    𐡄𐡍 (allocated < KEEP_ROOM / 2)
		اذهب theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    𐡄𐡍 (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		حرر(p);	// System is low... no go!
		p = عدم;
	    }
	    وإلا
		اذهب theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	𐡄𐡍 (releasing)
	    اكسر;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	𐡄𐡍 (!try_again)
	    اكسر;
    }

    𐡄𐡍 (message && p == عدم)
	do_outofmem_msg(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    أرجع p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#𐡄𐡍 defined(FEAT_SIGNS)
    فراغ *
lalloc_id(size_t size, صحيح message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    𐡄𐡍 (alloc_fail_id == id && alloc_does_fail(size))
	أرجع عدم;
# endif
    أرجع (lalloc(size, message));
}
#endif

#𐡄𐡍 defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    فراغ *
mem_realloc(فراغ *ptr, size_t size)
{
    فراغ *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    أرجع p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    فراغ
do_outofmem_msg(size_t size)
{
    𐡄𐡍 (did_outofmem_msg)
	أرجع;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_msg = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    𐡄𐡍 (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#𐡄𐡍 defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    فراغ
free_all_mem(فراغ)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    𐡄𐡍 (entered_free_all_mem)
	أرجع;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    𐡄𐡍 (first_tabpage != عدم && first_tabpage->tp_next != عدم)
	do_cmdline_cmd((char_u *)"tabonly!");
    𐡄𐡍 (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# 𐡄𐡍 defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# 𐡄𐡍 defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    𐡄𐡍 (curwin != عدم)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(عدم);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    𐡄𐡍 (curbuf != عدم)
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
# 𐡄𐡍 defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# 𐡄𐡍 defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# 𐡄𐡍 defined(FEAT_KEYMAP)
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
    free_search_patterns();
    free_old_sub();
    free_last_insert();
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
    set_expr_line(عدم, عدم);
# endif
# ifdef FEAT_DIFF
    𐡄𐡍 (curtab != عدم)
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
    set_keep_msg(عدم, 0);

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

    𐡄𐡍 (curwin != عدم)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    لكل (buf = firstbuf; buf != عدم; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(عدم, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	𐡄𐡍 (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	وإلا
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# 𐡄𐡍 defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    𐡄𐡍 (first_tabpage != عدم)
    {
	free_tabpage(first_tabpage);
	first_tabpage = عدم;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    لكل (;;)
	𐡄𐡍 (delete_first_msg() == FAIL)
	    اكسر;

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

# 𐡄𐡍 defined(FEAT_SOUND)
    sound_free();
# endif
# 𐡄𐡍 defined(USE_XSMP)
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

    𐡄𐡍 (ret != عدم)
	mch_memmove(ret, p, len);
    أرجع ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    فراغ
vim_free(فراغ *x)
{
    𐡄𐡍 (x != عدم && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	حرر(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    فراغ
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    فراغ
ga_clear_strings(garray_T *gap)
{
    صحيح		i;

    𐡄𐡍 (gap->ga_data != عدم)
	لكل (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#𐡄𐡍 defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    صحيح
ga_copy_strings(garray_T *from, garray_T *to)
{
    صحيح		i;

    ga_init2(to, حجم(char_u *), 1);
    𐡄𐡍 (ga_grow(to, from->ga_len) == FAIL)
	أرجع FAIL;

    لكل (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	𐡄𐡍 (orig == عدم)
	    copy = عدم;
	وإلا
	{
	    copy = vim_strsave(orig);
	    𐡄𐡍 (copy == عدم)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		أرجع FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    أرجع OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    فراغ
ga_init(garray_T *gap)
{
    gap->ga_data = عدم;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    فراغ
ga_init2(garray_T *gap, size_t itemsize, صحيح growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (صحيح)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
ga_grow(garray_T *gap, صحيح n)
{
    𐡄𐡍 (gap->ga_maxlen - gap->ga_len < n)
	أرجع ga_grow_inner(gap, n);
    أرجع OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    صحيح
ga_grow_id(garray_T *gap, صحيح n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    𐡄𐡍 (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	أرجع FAIL;
#endif

    أرجع ga_grow(gap, n);
}

    صحيح
ga_grow_inner(garray_T *gap, صحيح n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    𐡄𐡍 (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    𐡄𐡍 (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    𐡄𐡍 (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	أرجع FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    𐡄𐡍 (pp == عدم)
	أرجع FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    أرجع OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, حرف *sep)
{
    صحيح		i;
    صحيح		len = 0;
    صحيح		sep_len = (صحيح)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    لكل (i = 0; i < gap->ga_len; ++i)
	len += (صحيح)STRLEN(((char_u **)(gap->ga_data))[i]);

    𐡄𐡍 (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    𐡄𐡍 (s == عدم)
	أرجع عدم;

    *s = NUL;
    p = s;
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	𐡄𐡍 (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    أرجع s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    صحيح
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    𐡄𐡍 (cp == عدم)
	أرجع FAIL;

    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	أرجع FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    أرجع OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    صحيح
ga_add_string(garray_T *gap, char_u *p)
{
    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
	أرجع FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    أرجع OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    فراغ
ga_concat(garray_T *gap, char_u *s)
{
    صحيح    len;

    𐡄𐡍 (s == عدم || *s == NUL)
	أرجع;
    len = (صحيح)STRLEN(s);
    𐡄𐡍 (ga_grow(gap, len) == OK)
    {
	mch_memmove((حرف *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    فراغ
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    𐡄𐡍 (s == عدم || *s == NUL || len == 0)
	أرجع;
    𐡄𐡍 (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	أرجع;
    }
    𐡄𐡍 (ga_grow(gap, (صحيح)len) == OK)
    {
	mch_memmove((حرف *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (صحيح)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    صحيح
ga_append(garray_T *gap, صحيح c)
{
    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
	أرجع FAIL;
    *((حرف *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    أرجع OK;
}

#𐡄𐡍 (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    فراغ
append_ga_line(garray_T *gap)
{
    // Remove trailing CR.
    𐡄𐡍 (gap->ga_len > 0
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
    صحيح
ga_concat_bytes(garray_T *gap, ثابت حرف *s, صحيح len)
{
    𐡄𐡍 (ga_grow(gap, len) == FAIL)
	أرجع FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    أرجع OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    صحيح
ga_concat_int(garray_T *gap, صحيح n)
{
    حرف    buf[16];
    صحيح	    len = vim_snprintf(buf, حجم(buf), "%d", n);

    أرجع ga_concat_bytes(gap, buf, len);
}
#endif
