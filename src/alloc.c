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

#அந்தர்பாவ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#எனில் defined(MEM_PROFILE)

# define MEM_SIZES  8200
நிலையான long_u mem_allocs[MEM_SIZES];
நிலையான long_u mem_frees[MEM_SIZES];
நிலையான long_u mem_allocated;
நிலையான long_u mem_freed;
நிலையான long_u mem_peak;
நிலையான long_u இலக்கம்_alloc;
நிலையான long_u இலக்கம்_freed;

    நிலையான வெறுமை
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += அளவு(size_t);
}

    நிலையான வெறுமை
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += அளவு(size_t);
}

    நிலையான வெறுமை
mem_post_alloc(
    வெறுமை **pp,
    size_t size)
{
    எனில் (*pp == வெற்று)
	திரும்பு;
    size -= அளவு(size_t);
    *(long_u *)*pp = size;
    எனில் (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    இல்லையெனில்
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    எனில் (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    இலக்கம்_alloc++;
    *pp = (வெறுமை *)((எழுத்து *)*pp + அளவு(size_t));
}

    நிலையான வெறுமை
mem_pre_free(வெறுமை **pp)
{
    long_u size;

    *pp = (வெறுமை *)((எழுத்து *)*pp - அளவு(size_t));
    size = *(size_t *)*pp;
    எனில் (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    இல்லையெனில்
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    இலக்கம்_freed++;
}

/*
 * called on exit via atexit()
 */
    வெறுமை
vim_mem_profile_dump(வெறுமை)
{
    முழு i, j;

    printf("\r\n");
    j = 0;
    க்கு (i = 0; i < MEM_SIZES - 1; i++)
    {
	எனில் (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    தொடர்;

	எனில் (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	எனில் (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    எனில் (mem_allocs[i])
    {
	printf("\r\n");
	எனில் (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    இலக்கம்_alloc, இலக்கம்_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    முழு
alloc_does_fail(size_t size)
{
    எனில் (alloc_fail_countdown == 0)
    {
	எனில் (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_செய்தி(size);
	திரும்பு TRUE;
    }
    --alloc_fail_countdown;
    திரும்பு FALSE;
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
    வெறுமை *
alloc(size_t size)
{
    திரும்பு lalloc(size, TRUE);
}

#எனில் defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    வெறுமை *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(size))
	திரும்பு வெற்று;
# endif
    திரும்பு lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    வெறுமை *
alloc_clear(size_t size)
{
    வெறுமை *p;

    p = lalloc(size, TRUE);
    எனில் (p != வெற்று)
	(வெறுமை)vim_memset(p, 0, size);
    திரும்பு p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    வெறுமை *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(size))
	திரும்பு வெற்று;
#endif
    திரும்பு alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    வெறுமை *
lalloc_clear(size_t size, முழு message)
{
    வெறுமை *p;

    p = lalloc(size, message);
    எனில் (p != வெற்று)
	(வெறுமை)vim_memset(p, 0, size);
    திரும்பு p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    வெறுமை *
lalloc(size_t size, முழு message)
{
    வெறுமை	*p;		    // pointer to new storage space
    நிலையான முழு	releasing = FALSE;  // don't do mf_release_all() recursive
    முழு		try_again;
#எனில் defined(HAVE_AVAIL_MEM)
    நிலையான size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    எனில் (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	திரும்பு வெற்று;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    க்கு (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	எனில் ((p = malloc(size)) != வெற்று)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    செல் theend;
#இல்லையெனில்
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    எனில் (allocated < KEEP_ROOM / 2)
		செல் theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    எனில் (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		விடுவி(p);	// System is low... no go!
		p = வெற்று;
	    }
	    இல்லையெனில்
		செல் theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	எனில் (releasing)
	    நிறுத்து;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	எனில் (!try_again)
	    நிறுத்து;
    }

    எனில் (message && p == வெற்று)
	do_outofmem_செய்தி(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    திரும்பு p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#எனில் defined(FEAT_SIGNS)
    வெறுமை *
lalloc_id(size_t size, முழு message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(size))
	திரும்பு வெற்று;
# endif
    திரும்பு (lalloc(size, message));
}
#endif

#எனில் defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    வெறுமை *
mem_realloc(வெறுமை *ptr, size_t size)
{
    வெறுமை *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    திரும்பு p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    வெறுமை
do_outofmem_செய்தி(size_t size)
{
    எனில் (did_outofmem_செய்தி)
	திரும்பு;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_செய்தி = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    எனில் (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#எனில் defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    வெறுமை
free_all_mem(வெறுமை)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    எனில் (entered_free_all_mem)
	திரும்பு;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    எனில் (first_tabpage != வெற்று && first_tabpage->tp_next != வெற்று)
	do_cmdline_cmd((char_u *)"tabonly!");
    எனில் (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# எனில் defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# எனில் defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    எனில் (curwin != வெற்று)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(வெற்று);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    எனில் (curbuf != வெற்று)
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
# எனில் defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# எனில் defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# எனில் defined(FEAT_KEYMAP)
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
    free_தேடல்_patterns();
    free_old_sub();
    free_last_செருகு();
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
    set_கோவை_வரி(வெற்று, வெற்று);
# endif
# ifdef FEAT_DIFF
    எனில் (curtab != வெற்று)
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
    set_keep_செய்தி(வெற்று, 0);

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

    எனில் (curwin != வெற்று)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    க்கு (buf = firstbuf; buf != வெற்று; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(வெற்று, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	எனில் (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	இல்லையெனில்
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# எனில் defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    எனில் (first_tabpage != வெற்று)
    {
	free_tabpage(first_tabpage);
	first_tabpage = வெற்று;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    க்கு (;;)
	எனில் (delete_first_செய்தி() == FAIL)
	    நிறுத்து;

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

# எனில் defined(FEAT_SOUND)
    sound_free();
# endif
# எனில் defined(USE_XSMP)
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

    எனில் (ret != வெற்று)
	mch_memmove(ret, p, len);
    திரும்பு ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    வெறுமை
vim_free(வெறுமை *x)
{
    எனில் (x != வெற்று && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	விடுவி(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    வெறுமை
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    வெறுமை
ga_clear_strings(garray_T *gap)
{
    முழு		i;

    எனில் (gap->ga_data != வெற்று)
	க்கு (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#எனில் defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    முழு
ga_copy_strings(garray_T *from, garray_T *to)
{
    முழு		i;

    ga_init2(to, அளவு(char_u *), 1);
    எனில் (ga_grow(to, from->ga_len) == FAIL)
	திரும்பு FAIL;

    க்கு (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	எனில் (orig == வெற்று)
	    copy = வெற்று;
	இல்லையெனில்
	{
	    copy = vim_strsave(orig);
	    எனில் (copy == வெற்று)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		திரும்பு FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    திரும்பு OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    வெறுமை
ga_init(garray_T *gap)
{
    gap->ga_data = வெற்று;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    வெறுமை
ga_init2(garray_T *gap, size_t itemsize, முழு growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (முழு)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    முழு
ga_grow(garray_T *gap, முழு n)
{
    எனில் (gap->ga_maxlen - gap->ga_len < n)
	திரும்பு ga_grow_inner(gap, n);
    திரும்பு OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    முழு
ga_grow_id(garray_T *gap, முழு n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(அளவு(list_T)))
	திரும்பு FAIL;
#endif

    திரும்பு ga_grow(gap, n);
}

    முழு
ga_grow_inner(garray_T *gap, முழு n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    எனில் (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    எனில் (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    எனில் (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	திரும்பு FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    எனில் (pp == வெற்று)
	திரும்பு FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    திரும்பு OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, எழுத்து *sep)
{
    முழு		i;
    முழு		len = 0;
    முழு		sep_len = (முழு)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    க்கு (i = 0; i < gap->ga_len; ++i)
	len += (முழு)STRLEN(((char_u **)(gap->ga_data))[i]);

    எனில் (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    எனில் (s == வெற்று)
	திரும்பு வெற்று;

    *s = NUL;
    p = s;
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	எனில் (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    திரும்பு s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    முழு
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    எனில் (cp == வெற்று)
	திரும்பு FAIL;

    எனில் (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	திரும்பு FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    திரும்பு OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    முழு
ga_add_string(garray_T *gap, char_u *p)
{
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    திரும்பு OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    வெறுமை
ga_concat(garray_T *gap, char_u *s)
{
    முழு    len;

    எனில் (s == வெற்று || *s == NUL)
	திரும்பு;
    len = (முழு)STRLEN(s);
    எனில் (ga_grow(gap, len) == OK)
    {
	mch_memmove((எழுத்து *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    வெறுமை
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    எனில் (s == வெற்று || *s == NUL || len == 0)
	திரும்பு;
    எனில் (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	திரும்பு;
    }
    எனில் (ga_grow(gap, (முழு)len) == OK)
    {
	mch_memmove((எழுத்து *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (முழு)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    முழு
ga_append(garray_T *gap, முழு c)
{
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    *((எழுத்து *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    திரும்பு OK;
}

#எனில் (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    வெறுமை
append_ga_வரி(garray_T *gap)
{
    // Remove trailing CR.
    எனில் (gap->ga_len > 0
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
    முழு
ga_concat_bytes(garray_T *gap, மாறிலி எழுத்து *s, முழு len)
{
    எனில் (ga_grow(gap, len) == FAIL)
	திரும்பு FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    திரும்பு OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    முழு
ga_concat_int(garray_T *gap, முழு n)
{
    எழுத்து    buf[16];
    முழு	    len = vim_snprintf(buf, அளவு(buf), "%d", n);

    திரும்பு ga_concat_bytes(gap, buf, len);
}
#endif
