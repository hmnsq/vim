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

#అన్తర్భావ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#ఒకవేళ defined(MEM_PROFILE)

# define MEM_SIZES  8200
స్థిర long_u mem_allocs[MEM_SIZES];
స్థిర long_u mem_frees[MEM_SIZES];
స్థిర long_u mem_allocated;
స్థిర long_u mem_freed;
స్థిర long_u mem_peak;
స్థిర long_u అంకె_alloc;
స్థిర long_u అంకె_freed;

    స్థిర శూన్య
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += పరిమాణం(size_t);
}

    స్థిర శూన్య
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += పరిమాణం(size_t);
}

    స్థిర శూన్య
mem_post_alloc(
    శూన్య **pp,
    size_t కొలత)
{
    ఒకవేళ (*pp == ఖాళీ)
	తిరిగి;
    కొలత -= పరిమాణం(size_t);
    *(long_u *)*pp = కొలత;
    ఒకవేళ (కొలత <= MEM_SIZES-1)
	mem_allocs[కొలత-1]++;
    లేకపోతే
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += కొలత;
    ఒకవేళ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    అంకె_alloc++;
    *pp = (శూన్య *)((అక్షరం *)*pp + పరిమాణం(size_t));
}

    స్థిర శూన్య
mem_pre_free(శూన్య **pp)
{
    long_u కొలత;

    *pp = (శూన్య *)((అక్షరం *)*pp - పరిమాణం(size_t));
    కొలత = *(size_t *)*pp;
    ఒకవేళ (కొలత <= MEM_SIZES-1)
	mem_frees[కొలత-1]++;
    లేకపోతే
	mem_frees[MEM_SIZES-1]++;
    mem_freed += కొలత;
    అంకె_freed++;
}

/*
 * called on exit via atexit()
 */
    శూన్య
vim_mem_profile_dump(శూన్య)
{
    పూర్ణాంకం i, j;

    రాయి("\r\n");
    j = 0;
    కొరకు (i = 0; i < MEM_SIZES - 1; i++)
    {
	ఒకవేళ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    కొనసాగు;

	ఒకవేళ (mem_frees[i] > mem_allocs[i])
	    రాయి("\r\n%s", _("ERROR: "));
	రాయి("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	ఒకవేళ (j > 3)
	{
	    j = 0;
	    రాయి("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    ఒకవేళ (mem_allocs[i])
    {
	రాయి("\r\n");
	ఒకవేళ (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	రాయి("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    రాయి(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    రాయి(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    అంకె_alloc, అంకె_freed);
}

#అయితే_ముగింపు // MEM_PROFILE

#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం
alloc_does_fail(size_t కొలత)
{
    ఒకవేళ (alloc_fail_countdown == 0)
    {
	ఒకవేళ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_సందేశం(కొలత);
	తిరిగి TRUE;
    }
    --alloc_fail_countdown;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#నిర్వచించు KEEP_స్థలం (2 * 8192L)
#నిర్వచించు KEEP_స్థలం_KB (KEEP_స్థలం / 1024L)

/*
 * The normal way to allocate memory.  This handles an out-of-memory situation
 * as well as possible, still returns NULL when we're completely out.
 */
    శూన్య *
alloc(size_t కొలత)
{
    తిరిగి lalloc(కొలత, TRUE);
}

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    శూన్య *
alloc_id(size_t కొలత, alloc_id_T id వాడని)
{
# ifdef FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(కొలత))
	తిరిగి ఖాళీ;
# endif
    తిరిగి lalloc(కొలత, TRUE);
}
#అయితే_ముగింపు

/*
 * Allocate memory and set all bytes to zero.
 */
    శూన్య *
alloc_clear(size_t కొలత)
{
    శూన్య *p;

    p = lalloc(కొలత, TRUE);
    ఒకవేళ (p != ఖాళీ)
	(శూన్య)vim_memset(p, 0, కొలత);
    తిరిగి p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    శూన్య *
alloc_clear_id(size_t కొలత, alloc_id_T id వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(కొలత))
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    తిరిగి alloc_clear(కొలత);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    శూన్య *
lalloc_clear(size_t కొలత, పూర్ణాంకం సందేశం_2)
{
    శూన్య *p;

    p = lalloc(కొలత, సందేశం_2);
    ఒకవేళ (p != ఖాళీ)
	(శూన్య)vim_memset(p, 0, కొలత);
    తిరిగి p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    శూన్య *
lalloc(size_t కొలత, పూర్ణాంకం సందేశం_2)
{
    శూన్య	*p;		    // pointer to new storage space
    స్థిర పూర్ణాంకం	releasing = FALSE;  // don't do mf_release_all() recursive
    పూర్ణాంకం		try_again;
#ఒకవేళ defined(HAVE_AVAIL_MEM)
    స్థిర size_t allocated = 0;    // allocated since last avail check
#అయితే_ముగింపు

    // Safety check for allocating zero bytes
    ఒకవేళ (కొలత == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_అంతర్గత_error_lalloc_zero);
	తిరిగి ఖాళీ;
    }

#నిర్వచితమైతే MEM_PROFILE
    mem_pre_alloc_l(&కొలత);
#అయితే_ముగింపు

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    కొరకు (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	ఒకవేళ ((p = జ్ఞాపకం_ఇవ్వు(కొలత)) != ఖాళీ)
	{
#నిర్వచితం_కాకపోతే HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    వెళ్ళు theend;
#లేకపోతే
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += కొలత;
	    ఒకవేళ (allocated < KEEP_స్థలం / 2)
		వెళ్ళు theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    ఒకవేళ (mch_avail_mem(TRUE) < KEEP_స్థలం_KB && !releasing)
	    {
		విడుదల(p);	// System is low... no go!
		p = ఖాళీ;
	    }
	    లేకపోతే
		వెళ్ళు theend;
#అయితే_ముగింపు
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	ఒకవేళ (releasing)
	    ఆపు;
	releasing = TRUE;

	clear_sb_పాఠ్యం(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	ఒకవేళ (!try_again)
	    ఆపు;
    }

    ఒకవేళ (సందేశం_2 && p == ఖాళీ)
	do_outofmem_సందేశం(కొలత);

theend:
#నిర్వచితమైతే MEM_PROFILE
    mem_post_alloc(&p, కొలత);
#అయితే_ముగింపు
    తిరిగి p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#ఒకవేళ defined(FEAT_SIGNS)
    శూన్య *
lalloc_id(size_t కొలత, పూర్ణాంకం సందేశం_2, alloc_id_T id వాడని)
{
# ifdef FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(కొలత))
	తిరిగి ఖాళీ;
# endif
    తిరిగి (lalloc(కొలత, సందేశం_2));
}
#అయితే_ముగింపు

#ఒకవేళ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    శూన్య *
mem_realloc(శూన్య *ptr, size_t కొలత)
{
    శూన్య *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&కొలత);

    p = realloc(ptr, కొలత);

    mem_post_alloc(&p, కొలత);

    తిరిగి p;
}
#అయితే_ముగింపు

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    శూన్య
do_outofmem_సందేశం(size_t కొలత)
{
    ఒకవేళ (did_outofmem_సందేశం)
	తిరిగి;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_సందేశం = TRUE;

    semsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం_allocating_nr_బైట్లు), (long_u)కొలత);

    ఒకవేళ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#ఒకవేళ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    శూన్య
free_all_mem(శూన్య)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    ఒకవేళ (entered_free_all_mem)
	తిరిగి;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    ఖండం_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    ఒకవేళ (మొదటి_tabpage != ఖాళీ && మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	do_cmdline_cmd((char_u *)"tabonly!");
    ఒకవేళ (!ONE_కిటికీ)
	do_cmdline_cmd((char_u *)"only!");

# ఒకవేళ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# ఒకవేళ defined(FEAT_BEVAL_పదం)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    ఒకవేళ (curwin != ఖాళీ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ఖాళీ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    ఒకవేళ (curbuf != ఖాళీ)
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
# ఒకవేళ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# ఒకవేళ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# ఒకవేళ defined(FEAT_KEYMAP)
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
    free_శోధన_patterns();
    free_old_sub();
    free_చివరి_చొప్పించు();
    free_insexpand_stuff();
    free_prev_shellcmd();
    free_regexp_stuff();
    free_ట్యాగ్_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    సమితి_సమాసం_పంక్తి(ఖాళీ, ఖాళీ);
# endif
# ifdef FEAT_DIFF
    ఒకవేళ (curtab != ఖాళీ)
	diff_clear(curtab);
# endif
    clear_sb_పాఠ్యం(TRUE);	      // free any scrollback text

    // Free some global vars.
    free_username();
# ifdef FEAT_CLIPBOARD
    vim_regfree(clip_exclude_prog);
# endif
    vim_free(చివరి_cmdline);
    vim_free(new_చివరి_cmdline);
    సమితి_keep_సందేశం(ఖాళీ, 0);

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

    ఒకవేళ (curwin != ఖాళీ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    కొరకు (buf = firstbuf; buf != ఖాళీ; )
    {
	bufref_T    bufref;

	సమితి_bufref(&bufref, buf);
	nextbuf = buf->b_తదుపరి;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_బఫరు(ఖాళీ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	ఒకవేళ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	లేకపోతే
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(server_Delayed_ప్రారంభం_Name);
# endif

    // highlight info
    free_highlight();

    reset_చివరి_sourcing();

    ఒకవేళ (మొదటి_tabpage != ఖాళీ)
    {
	free_tabpage(మొదటి_tabpage);
	మొదటి_tabpage = ఖాళీ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    కొరకు (;;)
	ఒకవేళ (delete_మొదటి_సందేశం() == FAIL)
	    ఆపు;

# ifdef FEAT_పని_CHANNEL
    channel_free_all();
# endif
# ifdef FEAT_TIMERS
    timer_free_all();
# endif
# ifdef FEAT_EVAL
    // must be after channel_free_all() with unrefs partials
    eval_clear();
# endif
# ifdef FEAT_పని_CHANNEL
    // must be after eval_clear() with unrefs jobs
    పని_free_all();
# endif

    free_termoptions();
    free_cur_పదం();

    // screenlines (can't display anything now!)
    free_screenlines();

# ఒకవేళ defined(FEAT_SOUND)
    sound_free();
# endif
# ఒకవేళ defined(USE_XSMP)
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
    free_resub_eval_ఫలితం();
# endif
    free_vbuf();
}
#అయితే_ముగింపు

/*
 * Copy "p[len]" into allocated memory, ignoring NUL characters.
 * Returns NULL when out of memory.
 */
    char_u *
vim_memsave(char_u *p, size_t len)
{
    char_u *ret = alloc(len);

    ఒకవేళ (ret != ఖాళీ)
	mch_memmove(ret, p, len);
    తిరిగి ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    శూన్య
vim_free(శూన్య *x)
{
    ఒకవేళ (x != ఖాళీ && !really_exiting)
    {
#నిర్వచితమైతే MEM_PROFILE
	mem_pre_free(&x);
#అయితే_ముగింపు
	విడుదల(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    శూన్య
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_దత్తాంశం);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    శూన్య
ga_clear_అక్షరశ్రేణులు(garray_T *gap)
{
    పూర్ణాంకం		i;

    ఒకవేళ (gap->ga_దత్తాంశం != ఖాళీ)
	కొరకు (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_దత్తాంశం))[i]);
    ga_clear(gap);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    పూర్ణాంకం
ga_copy_అక్షరశ్రేణులు(garray_T *from, garray_T *గమ్యం)
{
    పూర్ణాంకం		i;

    ga_init2(గమ్యం, పరిమాణం(char_u *), 1);
    ఒకవేళ (ga_grow(గమ్యం, from->ga_len) == FAIL)
	తిరిగి FAIL;

    కొరకు (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_దత్తాంశం)[i];
	char_u *copy;

	ఒకవేళ (orig == ఖాళీ)
	    copy = ఖాళీ;
	లేకపోతే
	{
	    copy = vim_strsave(orig);
	    ఒకవేళ (copy == ఖాళీ)
	    {
		గమ్యం->ga_len = i;
		ga_clear_అక్షరశ్రేణులు(గమ్యం);
		తిరిగి FAIL;
	    }
	}
	((char_u **)గమ్యం->ga_దత్తాంశం)[i] = copy;
    }
    గమ్యం->ga_len = from->ga_len;
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    శూన్య
ga_init(garray_T *gap)
{
    gap->ga_దత్తాంశం = ఖాళీ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    శూన్య
ga_init2(garray_T *gap, size_t itemsize, పూర్ణాంకం growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (పూర్ణాంకం)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
ga_grow(garray_T *gap, పూర్ణాంకం n)
{
    ఒకవేళ (gap->ga_maxlen - gap->ga_len < n)
	తిరిగి ga_grow_inner(gap, n);
    తిరిగి OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    పూర్ణాంకం
ga_grow_id(garray_T *gap, పూర్ణాంకం n, alloc_id_T id వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(పరిమాణం(list_T)))
	తిరిగి FAIL;
#అయితే_ముగింపు

    తిరిగి ga_grow(gap, n);
}

    పూర్ణాంకం
ga_grow_inner(garray_T *gap, పూర్ణాంకం n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    ఒకవేళ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    ఒకవేళ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    ఒకవేళ (n > 0 && (size_t)(gap->ga_len + n) > కొలత_MAX / gap->ga_itemsize)
	తిరిగి FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_దత్తాంశం, new_len);
    ఒకవేళ (pp == ఖాళీ)
	తిరిగి FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_దత్తాంశం = pp;
    తిరిగి OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_అక్షరశ్రేణులు(garray_T *gap, అక్షరం *sep)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		sep_len = (పూర్ణాంకం)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    కొరకు (i = 0; i < gap->ga_len; ++i)
	len += (పూర్ణాంకం)STRLEN(((char_u **)(gap->ga_దత్తాంశం))[i]);

    ఒకవేళ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి ఖాళీ;

    *s = NUL;
    p = s;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	ఒకవేళ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_దత్తాంశం))[i]);
	p += STRLEN(p);
    }
    తిరిగి s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    పూర్ణాంకం
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    ఒకవేళ (cp == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	తిరిగి FAIL;
    }
    ((char_u **)(gap->ga_దత్తాంశం))[gap->ga_len++] = cp;
    తిరిగి OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    పూర్ణాంకం
ga_కూడు_string(garray_T *gap, char_u *p)
{
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    ((char_u **)(gap->ga_దత్తాంశం))[gap->ga_len++] = p;
    తిరిగి OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    శూన్య
ga_concat(garray_T *gap, char_u *s)
{
    పూర్ణాంకం    len;

    ఒకవేళ (s == ఖాళీ || *s == NUL)
	తిరిగి;
    len = (పూర్ణాంకం)STRLEN(s);
    ఒకవేళ (ga_grow(gap, len) == OK)
    {
	mch_memmove((అక్షరం *)gap->ga_దత్తాంశం + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    శూన్య
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    ఒకవేళ (s == ఖాళీ || *s == NUL || len == 0)
	తిరిగి;
    ఒకవేళ (len > (size_t)(గరిష్ఠ_పూర్ణాంకం - gap->ga_len))
    {
	emsg(_(e_resulting_పాఠ్యం_too_long));
	తిరిగి;
    }
    ఒకవేళ (ga_grow(gap, (పూర్ణాంకం)len) == OK)
    {
	mch_memmove((అక్షరం *)gap->ga_దత్తాంశం + gap->ga_len, s, len);
	gap->ga_len += (పూర్ణాంకం)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    పూర్ణాంకం
ga_append(garray_T *gap, పూర్ణాంకం c)
{
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    *((అక్షరం *)gap->ga_దత్తాంశం + gap->ga_len) = c;
    ++gap->ga_len;
    తిరిగి OK;
}

#ఒకవేళ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    శూన్య
append_ga_పంక్తి(garray_T *gap)
{
    // Remove trailing CR.
    ఒకవేళ (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_దత్తాంశం)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_దత్తాంశం, 0, FALSE);
    gap->ga_len = 0;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    పూర్ణాంకం
ga_concat_బైట్లు(garray_T *gap, స్థిరరాశి అక్షరం *s, పూర్ణాంకం len)
{
    ఒకవేళ (ga_grow(gap, len) == FAIL)
	తిరిగి FAIL;
    mch_memmove((char_u *)gap->ga_దత్తాంశం + gap->ga_len, s, len);
    gap->ga_len += len;
    తిరిగి OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    పూర్ణాంకం
ga_concat_int(garray_T *gap, పూర్ణాంకం n)
{
    అక్షరం    buf[16];
    పూర్ణాంకం	    len = vim_snprintf(buf, పరిమాణం(buf), "%d", n);

    తిరిగి ga_concat_బైట్లు(gap, buf, len);
}
#అయితే_ముగింపు
