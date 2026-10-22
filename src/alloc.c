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

#ಅನ್ತರ್ಭಾವ "vim.h"

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

#ಆದರೆ defined(MEM_PROFILE)

# define MEM_SIZES  8200
ಸ್ಥಿರ long_u mem_allocs[MEM_SIZES];
ಸ್ಥಿರ long_u mem_frees[MEM_SIZES];
ಸ್ಥಿರ long_u mem_allocated;
ಸ್ಥಿರ long_u mem_freed;
ಸ್ಥಿರ long_u mem_peak;
ಸ್ಥಿರ long_u ಅಂಕೆ_alloc;
ಸ್ಥಿರ long_u ಅಂಕೆ_freed;

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_alloc_s(size_t *sizep)
{
    *sizep += ಗಾತ್ರ(size_t);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_alloc_l(size_t *sizep)
{
    *sizep += ಗಾತ್ರ(size_t);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_post_alloc(
    ಶೂನ್ಯ **pp,
    size_t ಗಾತ್ರ_2)
{
    ಆದರೆ (*pp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾತ್ರ_2 -= ಗಾತ್ರ(size_t);
    *(long_u *)*pp = ಗಾತ್ರ_2;
    ಆದರೆ (ಗಾತ್ರ_2 <= MEM_SIZES-1)
	mem_allocs[ಗಾತ್ರ_2-1]++;
    ಇಲ್ಲದಿದ್ದರೆ
	mem_allocs[MEM_SIZES-1]++;
    mem_allocated += ಗಾತ್ರ_2;
    ಆದರೆ (mem_allocated - mem_freed > mem_peak)
	mem_peak = mem_allocated - mem_freed;
    ಅಂಕೆ_alloc++;
    *pp = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)*pp + ಗಾತ್ರ(size_t));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mem_pre_free(ಶೂನ್ಯ **pp)
{
    long_u ಗಾತ್ರ_2;

    *pp = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)*pp - ಗಾತ್ರ(size_t));
    ಗಾತ್ರ_2 = *(size_t *)*pp;
    ಆದರೆ (ಗಾತ್ರ_2 <= MEM_SIZES-1)
	mem_frees[ಗಾತ್ರ_2-1]++;
    ಇಲ್ಲದಿದ್ದರೆ
	mem_frees[MEM_SIZES-1]++;
    mem_freed += ಗಾತ್ರ_2;
    ಅಂಕೆ_freed++;
}

/*
 * called on exit via atexit()
 */
    ಶೂನ್ಯ
vim_mem_profile_dump(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i, j;

    ಬರೆ("\r\n");
    j = 0;
    ಗಾಗಿ (i = 0; i < MEM_SIZES - 1; i++)
    {
	ಆದರೆ (mem_allocs[i] == 0 && mem_frees[i] == 0)
	    ಮುಂದುವರಿ;

	ಆದರೆ (mem_frees[i] > mem_allocs[i])
	    ಬರೆ("\r\n%s", _("ERROR: "));
	ಬರೆ("[%4d / %4lu-%-4lu] ", i + 1, mem_allocs[i], mem_frees[i]);
	j++;
	ಆದರೆ (j > 3)
	{
	    j = 0;
	    ಬರೆ("\r\n");
	}
    }

    i = MEM_SIZES - 1;
    ಆದರೆ (mem_allocs[i])
    {
	ಬರೆ("\r\n");
	ಆದರೆ (mem_frees[i] > mem_allocs[i])
	    puts(_("ERROR: "));
	ಬರೆ("[>%d / %4lu-%-4lu]", i, mem_allocs[i], mem_frees[i]);
    }

    ಬರೆ(_("\n[bytes] total alloc-freed %lu-%lu, in use %lu, peak use %lu\n"),
	    mem_allocated, mem_freed, mem_allocated - mem_freed, mem_peak);
    ಬರೆ(_("[calls] total re/malloc()'s %lu, total free()'s %lu\n\n"),
	    ಅಂಕೆ_alloc, ಅಂಕೆ_freed);
}

#ಆದರೆ_ಅಂತ್ಯ // MEM_PROFILE

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ
alloc_does_fail(size_t ಗಾತ್ರ_2)
{
    ಆದರೆ (alloc_fail_countdown == 0)
    {
	ಆದರೆ (--alloc_fail_repeat <= 0)
	    alloc_fail_id = 0;
	do_outofmem_ಸಂದೇಶ(ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    --alloc_fail_countdown;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KEEP_ಸ್ಥಳ (2 * 8192L)
#ವ್ಯಾಖ್ಯಾನಿಸು KEEP_ಸ್ಥಳ_KB (KEEP_ಸ್ಥಳ / 1024L)

/*
 * The normal way to allocate memory.  This handles an out-of-memory situation
 * as well as possible, still returns NULL when we're completely out.
 */
    ಶೂನ್ಯ *
alloc(size_t ಗಾತ್ರ_2)
{
    ಹಿಂತಿರುಗಿ lalloc(ಗಾತ್ರ_2, TRUE);
}

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * alloc() with an ID for alloc_fail().
 */
    ಶೂನ್ಯ *
alloc_id(size_t ಗಾತ್ರ_2, alloc_id_T id ಬಳಸದ)
{
# ifdef FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ_2))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
# endif
    ಹಿಂತಿರುಗಿ lalloc(ಗಾತ್ರ_2, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Allocate memory and set all bytes to zero.
 */
    ಶೂನ್ಯ *
alloc_clear(size_t ಗಾತ್ರ_2)
{
    ಶೂನ್ಯ *p;

    p = lalloc(ಗಾತ್ರ_2, TRUE);
    ಆದರೆ (p != ಖಾಲಿ)
	(ಶೂನ್ಯ)vim_memset(p, 0, ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Same as alloc_clear() but with allocation id for testing
 */
    ಶೂನ್ಯ *
alloc_clear_id(size_t ಗಾತ್ರ_2, alloc_id_T id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ_2))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ alloc_clear(ಗಾತ್ರ_2);
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    ಶೂನ್ಯ *
lalloc_clear(size_t ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_2)
{
    ಶೂನ್ಯ *p;

    p = lalloc(ಗಾತ್ರ_2, ಸಂದೇಶ_2);
    ಆದರೆ (p != ಖಾಲಿ)
	(ಶೂನ್ಯ)vim_memset(p, 0, ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Low level memory allocation function.
 * This is used often, KEEP IT FAST!
 */
    ಶೂನ್ಯ *
lalloc(size_t ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_2)
{
    ಶೂನ್ಯ	*p;		    // pointer to new storage space
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	releasing = FALSE;  // don't do mf_release_all() recursive
    ಪೂರ್ಣಾಂಕ		try_again;
#ಆದರೆ defined(HAVE_AVAIL_MEM)
    ಸ್ಥಿರ size_t allocated = 0;    // allocated since last avail check
#ಆದರೆ_ಅಂತ್ಯ

    // Safety check for allocating zero bytes
    ಆದರೆ (ಗಾತ್ರ_2 == 0)
    {
	// Don't hide this message
	emsg_silent = 0;
	iemsg(e_ಆಂತರಿಕ_error_lalloc_zero);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MEM_PROFILE
    mem_pre_alloc_l(&ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ

    // Loop when out of memory: Try to release some memfile blocks and
    // if some blocks are released call malloc again.
    ಗಾಗಿ (;;)
    {
	// Handle three kinds of systems:
	// 1. No check for available memory: Just return.
	// 2. Slow check for available memory: call mch_avail_mem() after
	//    allocating KEEP_ROOM amount of memory.
	// 3. Strict check for available memory: call mch_avail_mem()
	ಆದರೆ ((p = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2)) != ಖಾಲಿ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_AVAIL_MEM
	    // 1. No check for available memory: Just return.
	    ಹೋಗು theend;
#ಇಲ್ಲದಿದ್ದರೆ
	    // 2. Slow check for available memory: call mch_avail_mem() after
	    //    allocating (KEEP_ROOM / 2) amount of memory.
	    allocated += ಗಾತ್ರ_2;
	    ಆದರೆ (allocated < KEEP_ಸ್ಥಳ / 2)
		ಹೋಗು theend;
	    allocated = 0;

	    // 3. check for available memory: call mch_avail_mem()
	    ಆದರೆ (mch_avail_mem(TRUE) < KEEP_ಸ್ಥಳ_KB && !releasing)
	    {
		ಬಿಡುಗಡೆ(p);	// System is low... no go!
		p = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ
	}
	// Remember that mf_release_all() is being called to avoid an endless
	// loop, because mf_release_all() may call alloc() recursively.
	ಆದರೆ (releasing)
	    ನಿಲ್ಲಿಸು;
	releasing = TRUE;

	clear_sb_ಪಠ್ಯ(TRUE);	      // free any scrollback text
	try_again = mf_release_all(); // release as many blocks as possible

	releasing = FALSE;
	ಆದರೆ (!try_again)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ಸಂದೇಶ_2 && p == ಖಾಲಿ)
	do_outofmem_ಸಂದೇಶ(ಗಾತ್ರ_2);

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MEM_PROFILE
    mem_post_alloc(&p, ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ p;
}

/*
 * lalloc() with an ID for alloc_fail().
 */
#ಆದರೆ defined(FEAT_SIGNS)
    ಶೂನ್ಯ *
lalloc_id(size_t ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_2, alloc_id_T id ಬಳಸದ)
{
# ifdef FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ_2))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
# endif
    ಹಿಂತಿರುಗಿ (lalloc(ಗಾತ್ರ_2, ಸಂದೇಶ_2));
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MEM_PROFILE)
/*
 * realloc() with memory profiling.
 */
    ಶೂನ್ಯ *
mem_realloc(ಶೂನ್ಯ *ptr, size_t ಗಾತ್ರ_2)
{
    ಶೂನ್ಯ *p;

    mem_pre_free(&ptr);
    mem_pre_alloc_s(&ಗಾತ್ರ_2);

    p = realloc(ptr, ಗಾತ್ರ_2);

    mem_post_alloc(&p, ಗಾತ್ರ_2);

    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
* Avoid repeating the error message many times (they take 1 second each).
* Did_outofmem_msg is reset when a character is read.
*/
    ಶೂನ್ಯ
do_outofmem_ಸಂದೇಶ(size_t ಗಾತ್ರ_2)
{
    ಆದರೆ (did_outofmem_ಸಂದೇಶ)
	ಹಿಂತಿರುಗಿ;

    // Don't hide this message
    emsg_silent = 0;

    // Must come first to avoid coming back here when printing the error
    // message fails, e.g. when setting v:errmsg.
    did_outofmem_ಸಂದೇಶ = TRUE;

    semsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ_allocating_nr_ಬೈಟುಗಳು), (long_u)ಗಾತ್ರ_2);

    ಆದರೆ (starting == NO_SCREEN)
	// Not even finished with initializations and already out of
	// memory?  Then nothing is going to work, exit.
	mch_exit(123);
}

#ಆದರೆ defined(EXITFREE)

/*
 * Free everything that we allocated.
 * Can be used to detect memory leaks, e.g., with ccmalloc.
 * NOTE: This is tricky!  Things are freed that functions depend on.  Don't be
 * surprised if Vim crashes...
 * Some things can't be freed, esp. things local to a library function.
 */
    ಶೂನ್ಯ
free_all_mem(ಶೂನ್ಯ)
{
    buf_T	*buf, *nextbuf;

    // When we cause a crash here it is caught and Vim tries to exit cleanly.
    // Don't try freeing everything again.
    ಆದರೆ (entered_free_all_mem)
	ಹಿಂತಿರುಗಿ;
    entered_free_all_mem = TRUE;
    // Don't want to trigger autocommands from here on.
    ಖಂಡ_autocmds();

    // Close all tabs and windows.  Reset 'equalalways' to avoid redraws.
    p_ea = FALSE;
    ಆದರೆ (ಮೊದಲ_tabpage != ಖಾಲಿ && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	do_cmdline_cmd((char_u *)"tabonly!");
    ಆದರೆ (!ONE_ಕಿಟಕಿ)
	do_cmdline_cmd((char_u *)"only!");

# ಆದರೆ defined(FEAT_SPELL)
    // Free all spell info.
    spell_free_all();
# endif

# ಆದರೆ defined(FEAT_BEVAL_ಪದ)
    ui_remove_balloon();
# endif
# ifdef FEAT_PROP_POPUP
    ಆದರೆ (curwin != ಖಾಲಿ)
	close_all_popups(TRUE);
# endif

    // Clear user commands (before deleting buffers).
    ex_comclear(ಖಾಲಿ);

    // When exiting from mainerr_arg_missing curbuf has not been initialized,
    // and not much else.
    ಆದರೆ (curbuf != ಖಾಲಿ)
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
# ಆದರೆ defined(FEAT_EVAL)
	do_cmdline_cmd((char_u *)"breakdel *");
# endif
# ಆದರೆ defined(FEAT_PROFILE)
	do_cmdline_cmd((char_u *)"profdel *");
# endif
# ಆದರೆ defined(FEAT_KEYMAP)
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
    free_ಹುಡುಕು_patterns();
    free_old_sub();
    free_ಕೊನೆಯ_ಸೇರಿಸು();
    free_insexpand_stuff();
    free_prev_shellcmd();
    free_regexp_stuff();
    free_ಟ್ಯಾಗ್_stuff();
    free_xim_stuff();
    free_cd_dir();
# ifdef FEAT_SIGNS
    free_signs();
# endif
# ifdef FEAT_EVAL
    free_eval_regcomp_cache();
    ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(ಖಾಲಿ, ಖಾಲಿ);
# endif
# ifdef FEAT_DIFF
    ಆದರೆ (curtab != ಖಾಲಿ)
	diff_clear(curtab);
# endif
    clear_sb_ಪಠ್ಯ(TRUE);	      // free any scrollback text

    // Free some global vars.
    free_username();
# ifdef FEAT_CLIPBOARD
    vim_regfree(clip_exclude_prog);
# endif
    vim_free(ಕೊನೆಯ_cmdline);
    vim_free(new_ಕೊನೆಯ_cmdline);
    ಗಣ_keep_ಸಂದೇಶ(ಖಾಲಿ, 0);

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

    ಆದರೆ (curwin != ಖಾಲಿ)
	// Destroy all windows.  Must come before freeing buffers.
	win_free_all();

    // Free all option values.  Must come after closing windows.
    free_all_options();

    // Free all buffers.  Reset 'autochdir' to avoid accessing things that
    // were freed already.
# ifdef FEAT_AUTOCHDIR
    p_acd = FALSE;
# endif
    ಗಾಗಿ (buf = firstbuf; buf != ಖಾಲಿ; )
    {
	bufref_T    bufref;

	ಗಣ_bufref(&bufref, buf);
	nextbuf = buf->b_ಮುಂದಿನ;
	// All windows were freed.  Reset b_nwindows so buffers can be wiped.
	buf->b_nwindows = 0;
	close_ಬಫರ್(ಖಾಲಿ, buf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	ಆದರೆ (bufref_valid(&bufref))
	    buf = nextbuf;	// didn't work, try next one
	ಇಲ್ಲದಿದ್ದರೆ
	    buf = firstbuf;
    }

# ifdef FEAT_ARABIC
    free_arshape_buf();
# endif

    // Clear registers.
    clear_registers();
    ResetRedobuff();
    ResetRedobuff();

# ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    vim_free(server_Delayed_ಆರಂಭ_Name);
# endif

    // highlight info
    free_highlight();

    reset_ಕೊನೆಯ_sourcing();

    ಆದರೆ (ಮೊದಲ_tabpage != ಖಾಲಿ)
    {
	free_tabpage(ಮೊದಲ_tabpage);
	ಮೊದಲ_tabpage = ಖಾಲಿ;
    }

# ifdef UNIX
    // Machine-specific free.
    mch_free_mem();
# endif

    // message history
    ಗಾಗಿ (;;)
	ಆದರೆ (delete_ಮೊದಲ_ಸಂದೇಶ() == FAIL)
	    ನಿಲ್ಲಿಸು;

# ifdef FEAT_ಕೆಲಸ_CHANNEL
    channel_free_all();
# endif
# ifdef FEAT_TIMERS
    timer_free_all();
# endif
# ifdef FEAT_EVAL
    // must be after channel_free_all() with unrefs partials
    eval_clear();
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
    // must be after eval_clear() with unrefs jobs
    ಕೆಲಸ_free_all();
# endif

    free_termoptions();
    free_cur_ಪದ();

    // screenlines (can't display anything now!)
    free_screenlines();

# ಆದರೆ defined(FEAT_SOUND)
    sound_free();
# endif
# ಆದರೆ defined(USE_XSMP)
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
    free_resub_eval_ಫಲಿತಾಂಶ();
# endif
    free_vbuf();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Copy "p[len]" into allocated memory, ignoring NUL characters.
 * Returns NULL when out of memory.
 */
    char_u *
vim_memsave(char_u *p, size_t len)
{
    char_u *ret = alloc(len);

    ಆದರೆ (ret != ಖಾಲಿ)
	mch_memmove(ret, p, len);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Replacement for free() that ignores NULL pointers.
 * Also skip free() when exiting for sure, this helps when we caught a deadly
 * signal that was caused by a crash in free().
 * If you want to set NULL after calling this function, you should use
 * VIM_CLEAR() instead.
 */
    ಶೂನ್ಯ
vim_free(ಶೂನ್ಯ *x)
{
    ಆದರೆ (x != ಖಾಲಿ && !really_exiting)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MEM_PROFILE
	mem_pre_free(&x);
#ಆದರೆ_ಅಂತ್ಯ
	ಬಿಡುಗಡೆ(x);
    }
}

/************************************************************************
 * Functions for handling growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    ಶೂನ್ಯ
ga_clear(garray_T *gap)
{
    vim_free(gap->ga_ದತ್ತಾಂಶ);
    ga_init(gap);
}

/*
 * Clear a growing array that contains a list of strings.
 */
    ಶೂನ್ಯ
ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (gap->ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	    vim_free(((char_u **)(gap->ga_ದತ್ತಾಂಶ))[i]);
    ga_clear(gap);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Copy a growing array that contains a list of strings.
 */
    ಪೂರ್ಣಾಂಕ
ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(garray_T *from, garray_T *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ga_init2(ಗಮ್ಯ, ಗಾತ್ರ(char_u *), 1);
    ಆದರೆ (ga_grow(ಗಮ್ಯ, from->ga_len) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (i = 0; i < from->ga_len; ++i)
    {
	char_u *orig = ((char_u **)from->ga_ದತ್ತಾಂಶ)[i];
	char_u *copy;

	ಆದರೆ (orig == ಖಾಲಿ)
	    copy = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    copy = vim_strsave(orig);
	    ಆದರೆ (copy == ಖಾಲಿ)
	    {
		ಗಮ್ಯ->ga_len = i;
		ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(ಗಮ್ಯ);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	((char_u **)ಗಮ್ಯ->ga_ದತ್ತಾಂಶ)[i] = copy;
    }
    ಗಮ್ಯ->ga_len = from->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and
 * ga_growsize!  Or use ga_init2().
 */
    ಶೂನ್ಯ
ga_init(garray_T *gap)
{
    gap->ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    gap->ga_maxlen = 0;
    gap->ga_len = 0;
}

    ಶೂನ್ಯ
ga_init2(garray_T *gap, size_t itemsize, ಪೂರ್ಣಾಂಕ growsize)
{
    ga_init(gap);
    gap->ga_itemsize = (ಪೂರ್ಣಾಂಕ)itemsize;
    gap->ga_growsize = growsize;
}

/*
 * Make room in growing array "gap" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ga_grow(garray_T *gap, ಪೂರ್ಣಾಂಕ n)
{
    ಆದರೆ (gap->ga_maxlen - gap->ga_len < n)
	ಹಿಂತಿರುಗಿ ga_grow_inner(gap, n);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Same as ga_grow() but uses an allocation id for testing.
 */
    ಪೂರ್ಣಾಂಕ
ga_grow_id(garray_T *gap, ಪೂರ್ಣಾಂಕ n, alloc_id_T id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ(list_T)))
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ga_grow(gap, n);
}

    ಪೂರ್ಣಾಂಕ
ga_grow_inner(garray_T *gap, ಪೂರ್ಣಾಂಕ n)
{
    size_t	old_len;
    size_t	new_len;
    char_u	*pp;

    ಆದರೆ (n < gap->ga_growsize)
	n = gap->ga_growsize;

    // A linear growth is very inefficient when the array grows big.  This
    // is a compromise between allocating memory that won't be used and too
    // many copy operations. A factor of 1.5 seems reasonable.
    ಆದರೆ (n < gap->ga_len / 2)
	n = gap->ga_len / 2;

    ಆದರೆ (n > 0 && (size_t)(gap->ga_len + n) > ಗಾತ್ರ_MAX / gap->ga_itemsize)
	ಹಿಂತಿರುಗಿ FAIL;
    new_len = (size_t)gap->ga_itemsize * (gap->ga_len + n);
    pp = vim_realloc(gap->ga_ದತ್ತಾಂಶ, new_len);
    ಆದರೆ (pp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    old_len = (size_t)gap->ga_itemsize * gap->ga_maxlen;
    vim_memset(pp + old_len, 0, new_len - old_len);
    gap->ga_maxlen = gap->ga_len + n;
    gap->ga_ದತ್ತಾಂಶ = pp;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * For a growing array that contains a list of strings: concatenate all the
 * strings with a separating "sep".
 * Returns NULL when out of memory.
 */
    char_u *
ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(garray_T *gap, ಅಕ್ಷರ *sep)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		sep_len = (ಪೂರ್ಣಾಂಕ)STRLEN(sep);
    char_u	*s;
    char_u	*p;

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	len += (ಪೂರ್ಣಾಂಕ)STRLEN(((char_u **)(gap->ga_ದತ್ತಾಂಶ))[i]);

    ಆದರೆ (gap->ga_len > 1)
	len += (gap->ga_len - 1) * sep_len;

    s = alloc(len + 1);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *s = NUL;
    p = s;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	ಆದರೆ (p != s)
	{
	    STRCPY(p, sep);
	    p += sep_len;
	}
	STRCPY(p, ((char_u **)(gap->ga_ದತ್ತಾಂಶ))[i]);
	p += STRLEN(p);
    }
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Make a copy of string "p" and add it to "gap".
 * When out of memory nothing changes and FAIL is returned.
 */
    ಪೂರ್ಣಾಂಕ
ga_copy_string(garray_T *gap, char_u *p)
{
    char_u *cp = vim_strsave(p);

    ಆದರೆ (cp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ga_grow(gap, 1) == FAIL)
    {
	vim_free(cp);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ((char_u **)(gap->ga_ದತ್ತಾಂಶ))[gap->ga_len++] = cp;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add string "p" to "gap".
 * When out of memory FAIL is returned (caller may want to free "p").
 */
    ಪೂರ್ಣಾಂಕ
ga_ಕೂಡಿಸು_string(garray_T *gap, char_u *p)
{
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ((char_u **)(gap->ga_ದತ್ತಾಂಶ))[gap->ga_len++] = p;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Concatenate a string to a growarray which contains bytes.
 * When "s" is NULL memory allocation fails does not do anything.
 * Note: Does NOT copy the NUL at the end!
 */
    ಶೂನ್ಯ
ga_concat(garray_T *gap, char_u *s)
{
    ಪೂರ್ಣಾಂಕ    len;

    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	ಹಿಂತಿರುಗಿ;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
    ಆದರೆ (ga_grow(gap, len) == OK)
    {
	mch_memmove((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len, s, (size_t)len);
	gap->ga_len += len;
    }
}

/*
 * Concatenate 'len' bytes from string 's' to a growarray.
 * When "s" is NULL does not do anything.
 */
    ಶೂನ್ಯ
ga_concat_len(garray_T *gap, char_u *s, size_t len)
{
    ಆದರೆ (s == ಖಾಲಿ || *s == NUL || len == 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (len > (size_t)(ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - gap->ga_len))
    {
	emsg(_(e_resulting_ಪಠ್ಯ_too_long));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ga_grow(gap, (ಪೂರ್ಣಾಂಕ)len) == OK)
    {
	mch_memmove((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len, s, len);
	gap->ga_len += (ಪೂರ್ಣಾಂಕ)len;
    }
}

/*
 * Append one byte to a growarray which contains bytes.
 */
    ಪೂರ್ಣಾಂಕ
ga_append(garray_T *gap, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    *((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len) = c;
    ++gap->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ (defined(UNIX) && !defined(USE_SYSTEM)) || defined(MSWIN)
/*
 * Append the text in "gap" below the cursor line and clear "gap".
 */
    ಶೂನ್ಯ
append_ga_ಸಾಲು(garray_T *gap)
{
    // Remove trailing CR.
    ಆದರೆ (gap->ga_len > 0
	    && !curbuf->b_p_bin
	    && ((char_u *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len - 1] == CAR)
	--gap->ga_len;
    ga_append(gap, NUL);
    ml_append(curwin->w_cursor.lnum++, gap->ga_ದತ್ತಾಂಶ, 0, FALSE);
    gap->ga_len = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
/*
 * Append a string to a growarray of bytes.  Returns FAIL on OOM.
 */
    ಪೂರ್ಣಾಂಕ
ga_concat_ಬೈಟುಗಳು(garray_T *gap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (ga_grow(gap, len) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    mch_memmove((char_u *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len, s, len);
    gap->ga_len += len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Append the string form of an integer to "gap". Returns FAIL on OOM.
 */
    ಪೂರ್ಣಾಂಕ
ga_concat_int(garray_T *gap, ಪೂರ್ಣಾಂಕ n)
{
    ಅಕ್ಷರ    buf[16];
    ಪೂರ್ಣಾಂಕ	    len = vim_snprintf(buf, ಗಾತ್ರ(buf), "%d", n);

    ಹಿಂತಿರುಗಿ ga_concat_ಬೈಟುಗಳು(gap, buf, len);
}
#ಆದರೆ_ಅಂತ್ಯ
