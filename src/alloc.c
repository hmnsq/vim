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

#अन्तर्भाव "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#यदि defined(MEM_PROFILE)

# define MEM_SIZES  8200
स्थिर long_u mem_allocs[MEM_SIZES];
स्थिर long_u mem_frees[MEM_SIZES];
स्थिर long_u mem_allocated;
स्थिर long_u mem_freed;
स्थिर long_u mem_peak;
स्थिर long_u अङ्क_alloc;
स्थिर long_u अङ्क_freed;

    स्थिर शून्य
mem_pre_alloc_s(माप_प्रकार *sizep)
{
    *sizep += परिमाण(माप_प्रकार);
}

    स्थिर शून्य
mem_pre_alloc_l(माप_प्रकार *sizep)
{
    *sizep += परिमाण(माप_प्रकार);
}

    स्थिर शून्य
mem_post_alloc(
    शून्य **pp,
    माप_प्रकार size)
{
    यदि (*pp == रिक्त)
	फर्काऊ;
    size -= परिमाण(माप_प्रकार);
    *(long_u *)*pp = size;
    यदि (size <= MEM_SIZES-1)
	mem_allocs[size-1]++;
    अन्यथा
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += size;
    यदि (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    अङ्क_alloc++;
    *pp = (शून्य *)((वर्ण *)*pp + परिमाण(माप_प्रकार));
}

    स्थिर शून्य
mem_pre_free(शून्य **pp)
{
    long_u size;

    *pp = (शून्य *)((वर्ण *)*pp - परिमाण(माप_प्रकार));
    size = *(माप_प्रकार *)*pp;
    यदि (size <= MEM_SIZES-1)
	mem_frees[size-1]++;
    अन्यथा
	mem_frees[MEM_SIZES-1]++;
    mem_freed += size;
    अङ्क_freed++;
}

/*
 * called on exit via atexit()
 */
    शून्य
vim_mem_profile_dump(शून्य)
{
    पूर्णाङ्क i, j;

    म_लिखो("\r\n");
    j = 0;
    लागि (i = 0; i < MEM_SIZES - 1; i++)
    {
	यदि (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    जारी;

	यदि (mem_frees[i] > mem_allocs[i])
	    म_लिखो("\r\n%s", _("ERROR: "));
	म_लिखो("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	यदि (j > 3)
	{
	    j = 0;
	    म_लिखो("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    यदि (mem_allocs[i])
    {
	म_लिखो("\r\n");
	यदि (mem_frees[i] > mem_allocs[i])
	    माला_दो(_("ERROR: "));
	म_लिखो("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    म_लिखो(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    म_लिखो(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    अङ्क_alloc, अङ्क_freed);
}

#endif // MEM_PROFILE

#ifdef FEAT_EVAL
    पूर्णाङ्क
alloc_does_fail(माप_प्रकार size)
{
    यदि (alloc_fail_countdown == 0)
    {
	यदि (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_सन्देश(size);
	फर्काऊ TRUE;
    }
    --alloc_fail_countdown;
    फर्काऊ FALSE;
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
    शून्य *
alloc(माप_प्रकार size)
{
    फर्काऊ lalloc(size, TRUE);
}

#यदि defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    शून्य *
alloc_id(माप_प्रकार size, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(size))
	फर्काऊ रिक्त;
# endif
    फर्काऊ lalloc(size, TRUE);
}
#endif

/*
 * Allocate memory and set all bytes to zero.
 */
    शून्य *
alloc_clear(माप_प्रकार size)
{
    शून्य *p;

    p = lalloc(size, TRUE);
    यदि (p != रिक्त)
	(शून्य)vim_memset(p, 0, size);
    फर्काऊ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    शून्य *
alloc_clear_id(माप_प्रकार size, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(size))
	फर्काऊ रिक्त;
#endif
    फर्काऊ alloc_clear(size);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    शून्य *
lalloc_clear(माप_प्रकार size, पूर्णाङ्क message)
{
    शून्य *p;

    p = lalloc(size, message);
    यदि (p != रिक्त)
	(शून्य)vim_memset(p, 0, size);
    फर्काऊ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    शून्य *
lalloc(माप_प्रकार size, पूर्णाङ्क message)
{
    शून्य	*p;		    // pointer to new storage space
    स्थिर पूर्णाङ्क	releasing = FALSE;  // don't do mf_release_all() recursive
    पूर्णाङ्क		try_again;
#यदि defined(HAVE_AVAIL_MEM)
    स्थिर माप_प्रकार allocated = 0;    // allocated since last avail check
#endif

    // Safety check for allocating zero bytes
    यदि (size == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_internal_error_lalloc_zero);
	फर्काऊ रिक्त;
    }

#ifdef MEM_PROFILE
    mem_pre_alloc_l(&size);
#endif

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    लागि (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	यदि ((p = दो_स्मृति(size)) != रिक्त)
	{
#ifndef HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    गच्छ theend;
#अन्यथा
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += size;
	    यदि (allocated < KEEP_ROOM / 2)
		गच्छ theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    यदि (mch_avail_mem(TRUE) < KEEP_ROOM_KB && !releasing)
	    {
		मुक्त(p);	// System is low... no go!
		p = रिक्त;
	    }
	    अन्यथा
		गच्छ theend;
#endif
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	यदि (releasing)
	    रोक;
	releasing = TRUE;

	clear_sb_text(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	यदि (!try_again)
	    रोक;
    }

    यदि (message && p == रिक्त)
	do_outofmem_सन्देश(size);

theend:
#ifdef MEM_PROFILE
    mem_post_alloc(&p, size);
#endif
    फर्काऊ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#यदि defined(FEAT_SIGNS)
    शून्य *
lalloc_id(माप_प्रकार size, पूर्णाङ्क message, alloc_id_T id UNUSED)
{
# ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(size))
	फर्काऊ रिक्त;
# endif
    फर्काऊ (lalloc(size, message));
}
#endif

#यदि defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    शून्य *
mem_realloc(शून्य *ptr, माप_प्रकार size)
{
    शून्य *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&size);

    p = पुनः_स्मृति(ptr, size);

    mem_post_alloc(&p, size);

    फर्काऊ p;
}
#endif

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    शून्य
do_outofmem_सन्देश(माप_प्रकार size)
{
    यदि (did_outofmem_सन्देश)
	फर्काऊ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_सन्देश = TRUE;

    semsg(_(e_out_of_memory_allocating_nr_bytes), (long_u)size);

    यदि (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#यदि defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    शून्य
free_all_mem(शून्य)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    यदि (entered_free_all_mem)
	फर्काऊ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    block_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    यदि (first_tabpage != रिक्त && first_tabpage->tp_next != रिक्त)
	do_cmdline_cmd((char_u *)"tabonly!");
    यदि (!ONE_WINDOW)
	do_cmdline_cmd((char_u *)"only!");

# यदि defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# यदि defined(FEAT_BEVAL_TERM)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    यदि (curwin != रिक्त)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(रिक्त);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    यदि (curbuf != रिक्त)
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
# यदि defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# यदि defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# यदि defined(FEAT_KEYMAP)
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
    free_खोज_patterns();
    free_old_sub();
    free_last_घुसाउ();
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
    set_अभिव्यक्ति_पङ्क्ति(रिक्त, रिक्त);
# endif
# ifdef FEAT_DIFF
    यदि (curtab != रिक्त)
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
    set_keep_सन्देश(रिक्त, 0);

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

    यदि (curwin != रिक्त)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    लागि (buf = firstbuf; buf != रिक्त; )
    {
	bufref_T    bufref;

	set_bufref(&bufref, buf);
	nextbuf = buf->b_next;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_buffer(रिक्त, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	यदि (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	अन्यथा
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# यदि defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(serverDelayedStartName);
# endif

    // highlight info
    free_highlight();

    reset_last_sourcing();

    यदि (first_tabpage != रिक्त)
    {
	free_tabpage(first_tabpage);
	first_tabpage = रिक्त;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    लागि (;;)
	यदि (delete_first_सन्देश() == FAIL)
	    रोक;

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

# यदि defined(FEAT_SOUND)
    sound_free();
# endif
# यदि defined(USE_XSMP)
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
vim_memsave(char_u *p, माप_प्रकार len)
{
    char_u *ret = alloc(len);

    यदि (ret != रिक्त)
	mch_memmove(ret, p, len);
    फर्काऊ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    शून्य
vim_free(शून्य *x)
{
    यदि (x != रिक्त && !really_exiting)
    {
#ifdef MEM_PROFILE
	mem_pre_free(&x);
#endif
	मुक्त(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    शून्य
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_data);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    शून्य
ga_clear_strings(garray_T *gap)
{
    पूर्णाङ्क		i;

    यदि (gap->ga_data != रिक्त)
	लागि (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_data))[i]);
    ga_clear(gap);
}

#यदि defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    पूर्णाङ्क
ga_copy_strings(garray_T *from, garray_T *to)
{
    पूर्णाङ्क		i;

    ga_init2(to, परिमाण(char_u *), 1);
    यदि (ga_grow(to, from->ga_len) == FAIL)
	फर्काऊ FAIL;

    लागि (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_data)[i];
	char_u *copy;

	यदि (orig == रिक्त)
	    copy = रिक्त;
	अन्यथा
	{
	    copy = vim_strsave(orig);
	    यदि (copy == रिक्त)
	    {
		to->ga_len = i;
		ga_clear_strings(to);
		फर्काऊ FAIL;
	    }
	}
	((char_u **)to->ga_data)[i] = copy;
    }
    to->ga_len = from->ga_len;
    फर्काऊ OK;
}
#endif

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    शून्य
ga_init(garray_T *gap)
{
    gap->ga_data = रिक्त;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    शून्य
ga_init2(garray_T *gap, माप_प्रकार itemsize, पूर्णाङ्क growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (पूर्णाङ्क)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
ga_grow(garray_T *gap, पूर्णाङ्क n)
{
    यदि (gap->ga_maxlen - gap->ga_len < n)
	फर्काऊ ga_grow_inner(gap, n);
    फर्काऊ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    पूर्णाङ्क
ga_grow_id(garray_T *gap, पूर्णाङ्क n, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	फर्काऊ FAIL;
#endif

    फर्काऊ ga_grow(gap, n);
}

    पूर्णाङ्क
ga_grow_inner(garray_T *gap, पूर्णाङ्क n)
{
    माप_प्रकार	old_len;
    माप_प्रकार	new_len;
    char_u	*pp;

    यदि (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    यदि (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    यदि (n > 0 && (माप_प्रकार)(gap->ga_len + n) > SIZE_MAX / gap->ga_itemsize)
	फर्काऊ FAIL;
    new_len = (माप_प्रकार)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_data, new_len);
    यदि (pp == रिक्त)
	फर्काऊ FAIL;
    old_len = (माप_प्रकार)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_data = pp;
    फर्काऊ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_strings(garray_T *gap, वर्ण *sep)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		sep_len = (पूर्णाङ्क)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    लागि (i = 0; i < gap->ga_len; ++i)
	len += (पूर्णाङ्क)STRLEN(((char_u **)(gap->ga_data))[i]);

    यदि (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    यदि (s == रिक्त)
	फर्काऊ रिक्त;

    *s = NUL;
    p = s;
    लागि (i = 0; i < gap->ga_len; ++i)
    {
	यदि (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_data))[i]);
	p += STRLEN(p);
    }
    फर्काऊ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    पूर्णाङ्क
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    यदि (cp == रिक्त)
	फर्काऊ FAIL;

    यदि (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	फर्काऊ FAIL;
    }
    ((char_u **)(gap->ga_data))[gap->ga_len++] = cp;
    फर्काऊ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    पूर्णाङ्क
ga_add_string(garray_T *gap, char_u *p)
{
    यदि (ga_grow(gap, 1) == FAIL)
	फर्काऊ FAIL;
    ((char_u **)(gap->ga_data))[gap->ga_len++] = p;
    फर्काऊ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    शून्य
ga_concat(garray_T *gap, char_u *s)
{
    पूर्णाङ्क    len;

    यदि (s == रिक्त || *s == NUL)
	फर्काऊ;
    len = (पूर्णाङ्क)STRLEN(s);
    यदि (ga_grow(gap, len) == OK)
    {
	mch_memmove((वर्ण *)gap->ga_data + gap->ga_len, s, (माप_प्रकार)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    शून्य
ga_concat_len(garray_T *gap, char_u *s, माप_प्रकार len)
{
    यदि (s == रिक्त || *s == NUL || len == 0)
	फर्काऊ;
    यदि (len > (माप_प्रकार)(पूर्णांक_उच्च - gap->ga_len))
    {
	emsg(_(e_resulting_text_too_long));
	फर्काऊ;
    }
    यदि (ga_grow(gap, (पूर्णाङ्क)len) == OK)
    {
	mch_memmove((वर्ण *)gap->ga_data + gap->ga_len, s, len);
	gap->ga_len += (पूर्णाङ्क)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    पूर्णाङ्क
ga_append(garray_T *gap, पूर्णाङ्क c)
{
    यदि (ga_grow(gap, 1) == FAIL)
	फर्काऊ FAIL;
    *((वर्ण *)gap->ga_data + gap->ga_len) = c;
    ++gap->ga_len;
    फर्काऊ OK;
}

#यदि (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    शून्य
append_ga_पङ्क्ति(garray_T *gap)
{
    // Remove trailing CR.
    यदि (gap->ga_len > 0
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
    पूर्णाङ्क
ga_concat_bytes(garray_T *gap, अचर वर्ण *s, पूर्णाङ्क len)
{
    यदि (ga_grow(gap, len) == FAIL)
	फर्काऊ FAIL;
    mch_memmove((char_u *)gap->ga_data + gap->ga_len, s, len);
    gap->ga_len += len;
    फर्काऊ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    पूर्णाङ्क
ga_concat_int(garray_T *gap, पूर्णाङ्क n)
{
    वर्ण    buf[16];
    पूर्णाङ्क	    len = vim_snprintf(buf, परिमाण(buf), "%d", n);

    फर्काऊ ga_concat_bytes(gap, buf, len);
}
#endif
