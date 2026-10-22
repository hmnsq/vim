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

#અન્તર્ભાવ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#જો defined(MEM_PROFILE)

# define MEM_SIZES  8200
સ્થિર long_u mem_allocs[MEM_SIZES];
સ્થિર long_u mem_frees[MEM_SIZES];
સ્થિર long_u mem_allocated;
સ્થિર long_u mem_freed;
સ્થિર long_u mem_peak;
સ્થિર long_u અંક_alloc;
સ્થિર long_u અંક_freed;

    સ્થિર શૂન્ય
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += કદ(size_t);
}

    સ્થિર શૂન્ય
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += કદ(size_t);
}

    સ્થિર શૂન્ય
mem_post_alloc(
    શૂન્ય **pp,
    size_t size)
{
    જો (*pp == ખાલી)
	પાછા;
    size -= કદ(size_t);
    *(long_u *)*pp = size;
    જો (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    નહીંતર
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    જો (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    અંક_alloc++;
    *pp = (શૂન્ય *)((અક્ષર *)*pp + કદ(size_t));
}

    સ્થિર શૂન્ય
mem_pre_free(શૂન્ય **pp)
{
    long_u size;

    *pp = (શૂન્ય *)((અક્ષર *)*pp - કદ(size_t));
    size = *(size_t *)*pp;
    જો (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    નહીંતર
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    અંક_freed++;
}

/*
 * called on exit via atexit()
 */
    શૂન્ય
vim_mem_profile_dump(શૂન્ય)
{
    પૂર્ણાંક i, j;

    printf("\r\n");
    j = 0;
    માટે (i = 0; i < MEM_SIZES - 1; i++)
    {
	જો (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ચાલુ;

	જો (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	જો (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    જો (mem_allocs[i])
    {
	printf("\r\n");
	જો (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	printf("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    printf(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    printf(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    અંક_alloc, અંક_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    પૂર્ણાંક
alloc_does_fail(size_t size)
{
    જો (alloc_fail_countdown == 0)
    {
	જો (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_સંદેશ(size);
	પાછા TRUE;
    }
    --alloc_fail_countdown;
    પાછા FALSE;
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
    શૂન્ય *
alloc(size_t size)
{
    પાછા lalloc(size, TRUE);
}

#જો defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    શૂન્ય *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(size))
	પાછા ખાલી;
# endif
    પાછા lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    શૂન્ય *
alloc_clear(size_t size)
{
    શૂન્ય *p;

    p = lalloc(size, TRUE);
    જો (p != ખાલી)
	(શૂન્ય)vim_memset(p, 0, size);
    પાછા p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    શૂન્ય *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(size))
	પાછા ખાલી;
#endif
    પાછા alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    શૂન્ય *
lalloc_clear(size_t size, પૂર્ણાંક message)
{
    શૂન્ય *p;

    p = lalloc(size, message);
    જો (p != ખાલી)
	(શૂન્ય)vim_memset(p, 0, size);
    પાછા p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    શૂન્ય *
lalloc(size_t size, પૂર્ણાંક message)
{
    શૂન્ય	*p;		    // pointer to new storage space
    સ્થિર પૂર્ણાંક	releasing = FALSE;  // don't do mf_release_all() recursive
    પૂર્ણાંક		try_again;
#જો defined(HAVE_AVAIL_MEM)
    સ્થિર size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    જો (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	પાછા ખાલી;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    માટે (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	જો ((p = malloc(size)) != ખાલી)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    જાઓ theend;
#નહીંતર
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    જો (allocated < KEEP_ROOM / 2)
		જાઓ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    જો (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		મુક્ત(p);	// System is low... no go!
		p = ખાલી;
	    }
	    નહીંતર
		જાઓ theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	જો (releasing)
	    તોડો;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	જો (!try_again)
	    તોડો;
    }

    જો (message && p == ખાલી)
	do_outofmem_સંદેશ(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    પાછા p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#જો defined(FEAT_SIGNS)
    શૂન્ય *
lalloc_id(size_t size, પૂર્ણાંક message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(size))
	પાછા ખાલી;
# endif
    પાછા (lalloc(size, message));
}
#endif

#જો defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    શૂન્ય *
mem_realloc(શૂન્ય *ptr, size_t size)
{
    શૂન્ય *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    પાછા p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    શૂન્ય
do_outofmem_સંદેશ(size_t size)
{
    જો (did_outofmem_સંદેશ)
	પાછા;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_સંદેશ = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    જો (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#જો defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    શૂન્ય
free_all_mem(શૂન્ય)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    જો (entered_free_all_mem)
	પાછા;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    જો (first_tabpage != ખાલી && first_tabpage->tp_next != ખાલી)
	do_cmdline_cmd((char_u *)"tabonly!");
    જો (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# જો defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# જો defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    જો (curwin != ખાલી)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ખાલી);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    જો (curbuf != ખાલી)
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
# જો defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# જો defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# જો defined(FEAT_KEYMAP)
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
    free_શોધ_patterns();
    free_old_sub();
    free_last_ઉમેરો();
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
    set_પદાવલિ_લીટી(ખાલી, ખાલી);
# endif
# ifdef FEAT_DIFF
    જો (curtab != ખાલી)
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
    set_keep_સંદેશ(ખાલી, 0);

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

    જો (curwin != ખાલી)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    માટે (buf = firstbuf; buf != ખાલી; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(ખાલી, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	જો (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	નહીંતર
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# જો defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    જો (first_tabpage != ખાલી)
    {
	free_tabpage(first_tabpage);
	first_tabpage = ખાલી;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    માટે (;;)
	જો (delete_first_સંદેશ() == FAIL)
	    તોડો;

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

# જો defined(FEAT_SOUND)
    sound_free();
# endif
# જો defined(USE_XSMP)
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

    જો (ret != ખાલી)
	mch_memmove(ret, p, len);
    પાછા ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    શૂન્ય
vim_free(શૂન્ય *x)
{
    જો (x != ખાલી && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	મુક્ત(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    શૂન્ય
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    શૂન્ય
ga_clear_strings(garray_T *gap)
{
    પૂર્ણાંક		i;

    જો (gap->ga_data != ખાલી)
	માટે (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#જો defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    પૂર્ણાંક
ga_copy_strings(garray_T *from, garray_T *to)
{
    પૂર્ણાંક		i;

    ga_init2(to, કદ(char_u *), 1);
    જો (ga_grow(to, from->ga_len) == FAIL)
	પાછા FAIL;

    માટે (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	જો (orig == ખાલી)
	    copy = ખાલી;
	નહીંતર
	{
	    copy = vim_strsave(orig);
	    જો (copy == ખાલી)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		પાછા FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    પાછા OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    શૂન્ય
ga_init(garray_T *gap)
{
    gap->ga_data = ખાલી;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    શૂન્ય
ga_init2(garray_T *gap, size_t itemsize, પૂર્ણાંક growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (પૂર્ણાંક)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
ga_grow(garray_T *gap, પૂર્ણાંક n)
{
    જો (gap->ga_maxlen - gap->ga_len < n)
	પાછા ga_grow_inner(gap, n);
    પાછા OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    પૂર્ણાંક
ga_grow_id(garray_T *gap, પૂર્ણાંક n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(કદ(list_T)))
	પાછા FAIL;
#endif

    પાછા ga_grow(gap, n);
}

    પૂર્ણાંક
ga_grow_inner(garray_T *gap, પૂર્ણાંક n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    જો (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    જો (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    જો (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	પાછા FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    જો (pp == ખાલી)
	પાછા FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    પાછા OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, અક્ષર *sep)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		sep_len = (પૂર્ણાંક)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    માટે (i = 0; i < gap->ga_len; ++i)
	len += (પૂર્ણાંક)STRLEN(((char_u **)(gap->ga_data))[i]);

    જો (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    જો (s == ખાલી)
	પાછા ખાલી;

    *s = NUL;
    p = s;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	જો (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    પાછા s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    પૂર્ણાંક
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    જો (cp == ખાલી)
	પાછા FAIL;

    જો (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	પાછા FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    પાછા OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    પૂર્ણાંક
ga_add_string(garray_T *gap, char_u *p)
{
    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    પાછા OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    શૂન્ય
ga_concat(garray_T *gap, char_u *s)
{
    પૂર્ણાંક    len;

    જો (s == ખાલી || *s == NUL)
	પાછા;
    len = (પૂર્ણાંક)STRLEN(s);
    જો (ga_grow(gap, len) == OK)
    {
	mch_memmove((અક્ષર *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    શૂન્ય
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    જો (s == ખાલી || *s == NUL || len == 0)
	પાછા;
    જો (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	પાછા;
    }
    જો (ga_grow(gap, (પૂર્ણાંક)len) == OK)
    {
	mch_memmove((અક્ષર *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (પૂર્ણાંક)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    પૂર્ણાંક
ga_append(garray_T *gap, પૂર્ણાંક c)
{
    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    *((અક્ષર *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    પાછા OK;
}

#જો (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    શૂન્ય
append_ga_લીટી(garray_T *gap)
{
    // Remove trailing CR.
    જો (gap->ga_len > 0
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
    પૂર્ણાંક
ga_concat_bytes(garray_T *gap, અચલ અક્ષર *s, પૂર્ણાંક len)
{
    જો (ga_grow(gap, len) == FAIL)
	પાછા FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    પાછા OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    પૂર્ણાંક
ga_concat_int(garray_T *gap, પૂર્ણાંક n)
{
    અક્ષર    buf[16];
    પૂર્ણાંક	    len = vim_snprintf(buf, કદ(buf), "%d", n);

    પાછા ga_concat_bytes(gap, buf, len);
}
#endif
