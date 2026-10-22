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

#include "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#အကယ်၍ defined(MEM_PROFILE)

# define MEM_SIZES  8200
static long_u mem_allocs[MEM_SIZES];
static long_u mem_frees[MEM_SIZES];
static long_u mem_allocated;
static long_u mem_freed;
static long_u mem_peak;
static long_u num_alloc;
static long_u num_freed;

    static void
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += parimaa_na(size_t);
}

    static void
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += parimaa_na(size_t);
}

    static void
mem_post_alloc(
    void **pp,
    size_t size)
{
    အကယ်၍ (*pp == NULL)
	ပြန်ပေး;
    size -= parimaa_na(size_t);
    *(long_u *)*pp = size;
    အကယ်၍ (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    မဟုတ်ပါက
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    အကယ်၍ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    num_alloc++;
    *pp = (void *)((var_na *)*pp + parimaa_na(size_t));
}

    static void
mem_pre_free(void **pp)
{
    long_u size;

    *pp = (void *)((var_na *)*pp - parimaa_na(size_t));
    size = *(size_t *)*pp;
    အကယ်၍ (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    မဟုတ်ပါက
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    num_freed++;
}

/*
 * called on exit via atexit()
 */
    void
vim_mem_profile_dump(void)
{
    ကိန်းပြည့် i, j;

    printf("\r\n");
    j = 0;
    အတွက် (i = 0; i < MEM_SIZES - 1; i++)
    {
	အကယ်၍ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    continue;

	အကယ်၍ (mem_frees[i] > mem_allocs[i])
	    printf("\r\n%s", _("ERROR: "));
	printf("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	အကယ်၍ (j > 3)
	{
	    j = 0;
	    printf("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    အကယ်၍ (mem_allocs[i])
    {
	printf("\r\n");
	အကယ်၍ (mem_frees[i] > mem_allocs[i])
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
    ကိန်းပြည့်
alloc_does_fail(size_t size)
{
    အကယ်၍ (alloc_fail_countdown == 0)
    {
	အကယ်၍ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_msg(size);
	ပြန်ပေး TRUE;
    }
    --alloc_fail_countdown;
    ပြန်ပေး FALSE;
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
    void *
alloc(size_t size)
{
    ပြန်ပေး lalloc(size, TRUE);
}

#အကယ်၍ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    void *
alloc_id(size_t size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    အကယ်၍ (alloc_fail_id == id && alloc_does_fail(size))
	ပြန်ပေး NULL;
# endif
    ပြန်ပေး lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    void *
alloc_clear(size_t size)
{
    void *p;

    p = lalloc(size, TRUE);
    အကယ်၍ (p != NULL)
	(void)vim_memset(p, 0, size);
    ပြန်ပေး p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    void *
alloc_clear_id(size_t size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    အကယ်၍ (alloc_fail_id == id && alloc_does_fail(size))
	ပြန်ပေး NULL;
#endif
    ပြန်ပေး alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    void *
lalloc_clear(size_t size, ကိန်းပြည့် message)
{
    void *p;

    p = lalloc(size, message);
    အကယ်၍ (p != NULL)
	(void)vim_memset(p, 0, size);
    ပြန်ပေး p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    void *
lalloc(size_t size, ကိန်းပြည့် message)
{
    void	*p;		    // pointer to new storage space
    static ကိန်းပြည့်	releasing = FALSE;  // don't do mf_release_all() recursive
    ကိန်းပြည့်		try_again;
#အကယ်၍ defined(HAVE_AVAIL_MEM)
    static size_t allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    အကယ်၍ (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	ပြန်ပေး NULL;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    အတွက် (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	အကယ်၍ ((p = malloc(size)) != NULL)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    goto theend;
#မဟုတ်ပါက
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    အကယ်၍ (allocated < KEEP_ROOM / 2)
		goto theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    အကယ်၍ (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		free(p);	// System is low... no go!
		p = NULL;
	    }
	    မဟုတ်ပါက
		goto theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	အကယ်၍ (releasing)
	    bha_nga;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	အကယ်၍ (!try_again)
	    bha_nga;
    }

    အကယ်၍ (message && p == NULL)
	do_outofmem_msg(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    ပြန်ပေး p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#အကယ်၍ defined(FEAT_SIGNS)
    void *
lalloc_id(size_t size, ကိန်းပြည့် message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    အကယ်၍ (alloc_fail_id == id && alloc_does_fail(size))
	ပြန်ပေး NULL;
# endif
    ပြန်ပေး (lalloc(size, message));
}
#endif

#အကယ်၍ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    void *
mem_realloc(void *ptr, size_t size)
{
    void *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = realloc(ptr, size);

    mem_post_alloc(&p, size);

    ပြန်ပေး p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    void
do_outofmem_msg(size_t size)
{
    အကယ်၍ (did_outofmem_msg)
	ပြန်ပေး;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_msg = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    အကယ်၍ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#အကယ်၍ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    void
free_all_mem(void)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    အကယ်၍ (entered_free_all_mem)
	ပြန်ပေး;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    အကယ်၍ (first_tabpage != NULL && first_tabpage->tp_next != NULL)
	do_cmdline_cmd((char_u *)"tabonly!");
    အကယ်၍ (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# အကယ်၍ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# အကယ်၍ defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    အကယ်၍ (curwin != NULL)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(NULL);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    အကယ်၍ (curbuf != NULL)
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
# အကယ်၍ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# အကယ်၍ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# အကယ်၍ defined(FEAT_KEYMAP)
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
    set_expr_line(NULL, NULL);
# endif
# ifdef FEAT_DIFF
    အကယ်၍ (curtab != NULL)
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
    set_keep_msg(NULL, 0);

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

    အကယ်၍ (curwin != NULL)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    အတွက် (buf = firstbuf; buf != NULL; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(NULL, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	အကယ်၍ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	မဟုတ်ပါက
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# အကယ်၍ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    အကယ်၍ (first_tabpage != NULL)
    {
	free_tabpage(first_tabpage);
	first_tabpage = NULL;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    အတွက် (;;)
	အကယ်၍ (delete_first_msg() == FAIL)
	    bha_nga;

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

# အကယ်၍ defined(FEAT_SOUND)
    sound_free();
# endif
# အကယ်၍ defined(USE_XSMP)
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

    အကယ်၍ (ret != NULL)
	mch_memmove(ret, p, len);
    ပြန်ပေး ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    void
vim_free(void *x)
{
    အကယ်၍ (x != NULL && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	free(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    void
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    void
ga_clear_strings(garray_T *gap)
{
    ကိန်းပြည့်		i;

    အကယ်၍ (gap->ga_data != NULL)
	အတွက် (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    ကိန်းပြည့်
ga_copy_strings(garray_T *from, garray_T *to)
{
    ကိန်းပြည့်		i;

    ga_init2(to, parimaa_na(char_u *), 1);
    အကယ်၍ (ga_grow(to, from->ga_len) == FAIL)
	ပြန်ပေး FAIL;

    အတွက် (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	အကယ်၍ (orig == NULL)
	    copy = NULL;
	မဟုတ်ပါက
	{
	    copy = vim_strsave(orig);
	    အကယ်၍ (copy == NULL)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		ပြန်ပေး FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    ပြန်ပေး OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    void
ga_init(garray_T *gap)
{
    gap->ga_data = NULL;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    void
ga_init2(garray_T *gap, size_t itemsize, ကိန်းပြည့် growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (ကိန်းပြည့်)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    ကိန်းပြည့်
ga_grow(garray_T *gap, ကိန်းပြည့် n)
{
    အကယ်၍ (gap->ga_maxlen - gap->ga_len < n)
	ပြန်ပေး ga_grow_inner(gap, n);
    ပြန်ပေး OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    ကိန်းပြည့်
ga_grow_id(garray_T *gap, ကိန်းပြည့် n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    အကယ်၍ (alloc_fail_id == id && alloc_does_fail(parimaa_na(list_T)))
	ပြန်ပေး FAIL;
#endif

    ပြန်ပေး ga_grow(gap, n);
}

    ကိန်းပြည့်
ga_grow_inner(garray_T *gap, ကိန်းပြည့် n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    အကယ်၍ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    အကယ်၍ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    အကယ်၍ (n > 0 && (size_t)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	ပြန်ပေး FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    အကယ်၍ (pp == NULL)
	ပြန်ပေး FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    ပြန်ပေး OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, var_na *sep)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		len = 0;
    ကိန်းပြည့်		sep_len = (ကိန်းပြည့်)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    အတွက် (i = 0; i < gap->ga_len; ++i)
	len += (ကိန်းပြည့်)STRLEN(((char_u **)(gap->ga_data))[i]);

    အကယ်၍ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    အကယ်၍ (s == NULL)
	ပြန်ပေး NULL;

    *s = NUL;
    p = s;
    အတွက် (i = 0; i < gap->ga_len; ++i)
    {
	အကယ်၍ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    ပြန်ပေး s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    ကိန်းပြည့်
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    အကယ်၍ (cp == NULL)
	ပြန်ပေး FAIL;

    အကယ်၍ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ပြန်ပေး FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    ပြန်ပေး OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    ကိန်းပြည့်
ga_add_string(garray_T *gap, char_u *p)
{
    အကယ်၍ (ga_grow(gap, 1) == FAIL)
	ပြန်ပေး FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    ပြန်ပေး OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    void
ga_concat(garray_T *gap, char_u *s)
{
    ကိန်းပြည့်    len;

    အကယ်၍ (s == NULL || *s == NUL)
	ပြန်ပေး;
    len = (ကိန်းပြည့်)STRLEN(s);
    အကယ်၍ (ga_grow(gap, len) == OK)
    {
	mch_memmove((var_na *)gap->ga_data + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    void
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    အကယ်၍ (s == NULL || *s == NUL || len == 0)
	ပြန်ပေး;
    အကယ်၍ (len > (size_t)(INT_MAX - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	ပြန်ပေး;
    }
    အကယ်၍ (ga_grow(gap, (ကိန်းပြည့်)len) == OK)
    {
	mch_memmove((var_na *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (ကိန်းပြည့်)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    ကိန်းပြည့်
ga_append(garray_T *gap, ကိန်းပြည့် c)
{
    အကယ်၍ (ga_grow(gap, 1) == FAIL)
	ပြန်ပေး FAIL;
    *((var_na *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    ပြန်ပေး OK;
}

#အကယ်၍ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    void
append_ga_line(garray_T *gap)
{
    // Remove trailing CR.
    အကယ်၍ (gap->ga_len > 0
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
    ကိန်းပြည့်
ga_concat_bytes(garray_T *gap, const var_na *s, ကိန်းပြည့် len)
{
    အကယ်၍ (ga_grow(gap, len) == FAIL)
	ပြန်ပေး FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    ပြန်ပေး OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    ကိန်းပြည့်
ga_concat_int(garray_T *gap, ကိန်းပြည့် n)
{
    var_na    buf[16];
    ကိန်းပြည့်	    len = vim_snprintf(buf, parimaa_na(buf), "%d", n);

    ပြန်ပေး ga_concat_bytes(gap, buf, len);
}
#endif
