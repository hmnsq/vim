/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * session.c: session related functions
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SESSION)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;

    sname = home_replace_save(ଖାଲି, name);
    ଯଦି (sname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	ପାଇଁ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ଯଦି (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // write the result
    ଯଦି (fputs((ଅକ୍ଷର *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ଫେରନ୍ତୁ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ଯଦି (buf->b_sfname != ଖାଲି
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ଅନ୍ୟଥା
	name = buf->b_ffname;
    ଯଦି (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_arglist(
    FILE	*fd,
    ଅକ୍ଷର	*cmd,
    garray_T	*gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*buf = ଖାଲି;
    char_u	*s;

    ଯଦି (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (put_ଧାଡ଼ି(fd, ":%argdel") == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ଯଦି (s != ଖାଲି)
	{
	    ଯଦି (fullname)
	    {
		buf = alloc(MAXPATHL);
		ଯଦି (buf != ଖାଲି)
		{
		    (ଶୂନ୍ୟ)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ଯଦି (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ଫେରନ୍ତୁ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ଯଦି (bt_terminal(wp->w_buffer))
	ଫେରନ୍ତୁ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ଯଦି (wp->w_buffer->b_fname == ଖାଲି
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ଫେରନ୍ତୁ (ssop_flags & SSOP_BLANK);
    ଯଦି (bt_help(wp->w_buffer))
	ଫେରନ୍ତୁ (ssop_flags & SSOP_HELP);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ଯଦି (fr->fr_layout == FR_LEAF)
	ଫେରନ୍ତୁ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ଯଦି (ses_do_frame(frc))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ସ୍ଥିର frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	ଯଦି (ses_do_frame(frc))
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    ଯଦି (fr->fr_layout == FR_LEAF)
	ଫେରନ୍ତୁ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ଯଦି (frc != ଖାଲି)
	ଯେତେବେଳେ ((frc = ses_skipframe(frc->fr_next)) != ଖାଲି)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ଯଦି (put_ଧାଡ଼ି(fd, "wincmd _ | wincmd |") == FAIL
		    || put_ଧାଡ଼ି(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ++ଗଣନା_2;
	}

    // Go back to the first window.
    ଯଦି (ଗଣନା_2 > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", ଗଣନା_2) < 0
		|| put_eol(fd) == FAIL))
	ଫେରନ୍ତୁ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ଯେତେବେଳେ (frc != ଖାଲି)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ଯଦି (frc != ଖାଲି && put_ଧାଡ଼ି(fd, "wincmd w") == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ses_winsizes(
    FILE	*fd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_size,
    win_T	*tab_firstwin)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
    win_T	*wp;

    ଯଦି (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	ପାଇଁ (wp = tab_firstwin; wp != ଖାଲି; wp = wp->w_next)
	{
	    ଯଦି (!ses_do_win(wp))
		ଜାରି;
	    ++n;

	    // restore height when not full height
	    ଯଦି (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ଫେରନ୍ତୁ FAIL;

	    // restore width when not full width
	    ଯଦି (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
    {
	// Just equalise window sizes
	ଯଦି (put_ଧାଡ଼ି(fd, "wincmd =") == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_view_curpos(FILE *fd, win_T *wp, ଅକ୍ଷର *spaces)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

    ଯଦି (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ଅନ୍ୟଥା
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ଫେରନ୍ତୁ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ଯଦି (wp->w_alist == &global_alist)
    {
	ଯଦି (put_ଧାଡ଼ି(fd, "argglobal") == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ଖାଲି
			|| wp->w_localdir != ଖାଲି, flagp) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ଯଦି (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ଯଦି (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ଯଦି (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ଯଦି (bt_help(wp->w_buffer))
	{
	    ଅକ୍ଷର *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ଯଦି (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (ଅକ୍ଷର *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ଯଦି (put_ଧାଡ଼ି(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
# ifdef FEAT_TERMINAL
	ଅନ୍ୟଥା ଯଦି (bt_terminal(wp->w_buffer))
	{
	    ଯଦି (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
# endif
	// Load the file.
	ଅନ୍ୟଥା ଯଦି (wp->w_buffer->b_ffname != ଖାଲି
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ଯଦି (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    // No file in this buffer, just make it empty.
	    ଯଦି (put_ଧାଡ଼ି(fd, "enew") == FAIL)
		ଫେରନ୍ତୁ FAIL;
# ifdef FEAT_QUICKFIX
	    ଯଦି (wp->w_buffer->b_ffname != ଖାଲି)
	    {
		// The buffer does have a name, but it's not a file name.
		ଯଦି (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ଯଦି (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ଯଦି ((flagp == &ssop_flags)
		&& alt != ଖାଲି
		&& alt->b_fname != ଖାଲି
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ଫେରନ୍ତୁ FAIL;
    }

    // Local mappings and abbreviations.
    ଯଦି ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ଯଦି (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ଅନ୍ୟଥା ଯଦି (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ଅନ୍ୟଥା
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ଯଦି (f == FAIL)
	ଫେରନ୍ତୁ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ଯଦି ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != ଖାଲି
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ଯଦି (put_folds(fd, wp) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ଯଦି (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ଯଦି (put_ଧାଡ଼ି(fd, "{") == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (wp->w_height <= 0)
	{
	    ଯଦି (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (put_eol(fd) == FAIL
		|| put_ଧାଡ଼ି(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_ଧାଡ଼ି(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_ଧାଡ଼ି(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ଯଦି (wp->w_cursor.col == 0)
	{
	    ଯଦି (put_ଧାଡ଼ି(fd, "  normal! 0") == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ଯଦି (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_ଧାଡ଼ି(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_ଧାଡ଼ି(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_ଧାଡ଼ି(fd, "  endif") == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (put_view_curpos(fd, wp, "  ") == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (put_ଧାଡ଼ି(fd, "}") == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ଯଦି (wp->w_localdir != ଖାଲି
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ଯଦି (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	did_lcd = TRUE;
    }

    ଫେରନ୍ତୁ OK;
}

# ifdef FEAT_EVAL
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    char_u	*p, *t;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gvht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(gvht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ଯଦି ((this_var->di_tv.v_type == VAR_ସଂଖ୍ୟା
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_ଚାବି) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ଯଦି (p == ଖାଲି)	    // out of memory
		    ଭାଙ୍ଗ;
		ପାଇଁ (t = p; *t != NUL; ++t)
		    ଯଦି (*t == '\n')
			*t = 'n';
		    ଅନ୍ୟଥା ଯଦି (*t == '\r')
			*t = 'r';
		ଯଦି ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_ଚାବି,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ଫେରନ୍ତୁ FAIL;
		}
		vim_free(p);
	    }
	    ଅନ୍ୟଥା ଯଦି (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_ଚାବି) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign = ' ';

		ଯଦି (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ଯଦି ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_ଚାବି, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
    }
    ଫେରନ୍ତୁ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		only_save_windows = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_size = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_arg_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_arg_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ଯଦି (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ଯଦି (put_ଧାଡ଼ି(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	ଯାଅ fail;

    ଯଦି (put_ଧାଡ଼ି(fd, "doautoall SessionLoadPre") == FAIL)
	ଯାଅ fail;

    ଯଦି (put_ଧାଡ଼ି(fd, "var save_splitbelow: bool") == FAIL
	    || put_ଧାଡ଼ି(fd, "var save_splitright: bool") == FAIL
	    || put_ଧାଡ଼ି(fd, "var save_winminheight: number") == FAIL
	    || put_ଧାଡ଼ି(fd, "var save_winminwidth: number") == FAIL
	    || put_ଧାଡ଼ି(fd, "var wipebuf: number = -1") == FAIL
	    || put_ଧାଡ଼ି(fd, "var shortmess_save: string") == FAIL)
	ଯାଅ fail;

    ଯଦି (ssop_flags & SSOP_GLOBALS)
	ଯଦି (store_session_globals(fd) == FAIL)
	    ଯାଅ fail;
# endif

    // Close all windows and tabs but one.
    ଯଦି (put_ଧାଡ଼ି(fd, "silent only") == FAIL)
	ଯାଅ fail;
    ଯଦି ((ssop_flags & SSOP_TABPAGES)
	    && put_ଧାଡ଼ି(fd, "silent tabonly") == FAIL)
	ଯାଅ fail;

    // Now a :cd command to the session directory or the current directory
    ଯଦି (ssop_flags & SSOP_SESDIR)
    {
	ଯଦି (put_ଧାଡ଼ି(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    ଯାଅ fail;
    }
    ଅନ୍ୟଥା ଯଦି (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ଖାଲି, globaldir != ଖାଲି ? globaldir : dirnow);
	ଯଦି (sname == ଖାଲି
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    ଯାଅ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ଯଦି (put_ଧାଡ଼ି(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	ଯାଅ fail;
    ଯଦି (put_ଧାଡ଼ି(fd, "  wipebuf = bufnr('%')") == FAIL)
	ଯାଅ fail;
    ଯଦି (put_ଧାଡ଼ି(fd, "endif") == FAIL)
	ଯାଅ fail;

    // Save 'shortmess' if not storing options.
    ଯଦି ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_ଧାଡ଼ି(fd, "shortmess_save = &shortmess") == FAIL)
	ଯାଅ fail;

    // Set 'shortmess' for the following.
    ଯଦି (put_ଧାଡ଼ି(fd, "set shortmess+=aoO") == FAIL)
	ଯାଅ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ଯଦି (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ଖାଲି
		&& buf->b_p_bl)
	{
	    ଯଦି (fprintf(fd, "badd +%ld ", buf->b_wininfo == ଖାଲି ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		ଯାଅ fail;
	}
    }

    // the global argument list
    ଯଦି (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	ଯାଅ fail;

    ଯଦି (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ଯଦି (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    ଯାଅ fail;
    }

# ifdef FEAT_GUI
    ଯଦି (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x, y;

	ଯଦି (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ଯଦି (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		ଯାଅ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ଯଦି (p_stal == 1 && first_tabpage->tp_next != ଖାଲି)
    {
	ଯଦି (put_ଧାଡ଼ି(fd, "set stal=2") == FAIL)
	    ଯାଅ fail;
	restore_stal = TRUE;
    }

    ଯଦି ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ଯଦି (tp->tp_next != ଖାଲି
		  && put_ଧାଡ଼ି(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		ଯାଅ fail;
	ଯଦି (first_tabpage->tp_next != ଖାଲି && put_ଧାଡ଼ି(fd, "tabrewind") == FAIL)
	    ଯାଅ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	need_tabnext = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ଯଦି ((ssop_flags & SSOP_TABPAGES))
	{
	    ଯଦି (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ଅନ୍ୟଥା
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ଯଦି (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	ପାଇଁ (wp = tab_firstwin; wp != ଖାଲି; wp = wp->w_next)
	{
	    ଯଦି (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != ଖାଲି
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ଯଦି (need_tabnext && put_ଧାଡ଼ି(fd, "tabnext") == FAIL)
		    ଯାଅ fail;
		need_tabnext = FALSE;

		ଯଦି (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    ଯାଅ fail;
		ଯଦି (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ଭାଙ୍ଗ;
	    }
	}

	// If no file got edited create an empty tab page.
	ଯଦି (need_tabnext && put_ଧାଡ଼ି(fd, "tabnext") == FAIL)
	    ଯାଅ fail;

	ଯଦି (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ଯଦି (put_ଧାଡ଼ି(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_ଧାଡ଼ି(fd, "save_splitright = &splitright")
								       == FAIL)
		ଯାଅ fail;
	    ଯଦି (put_ଧାଡ଼ି(fd, "set splitbelow splitright") == FAIL)
		ଯାଅ fail;
	    ଯଦି (ses_win_rec(fd, tab_topframe) == FAIL)
		ଯାଅ fail;
	    ଯଦି (put_ଧାଡ଼ି(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_ଧାଡ଼ି(fd, "&splitright = save_splitright")
								       == FAIL)
		ଯାଅ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	ପାଇଁ (wp = tab_firstwin; wp != ଖାଲି; wp = W_NEXT(wp))
	{
	    ଯଦି (ses_do_win(wp))
		++nr;
	    ଅନ୍ୟଥା
		restore_size = FALSE;
	    ଯଦି (curwin == wp)
		cnr = nr;
	}

	ଯଦି (tab_firstwin->w_next != ଖାଲି)
	{
	    // Go to the first window.
	    ଯଦି (put_ଧାଡ଼ି(fd, "wincmd t") == FAIL)
		ଯାଅ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ଯଦି (!restore_height_width
		    && (put_ଧାଡ଼ି(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_ଧାଡ଼ି(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		ଯାଅ fail;
	    ଯଦି (put_ଧାଡ଼ି(fd, "set winminheight=0") == FAIL
		    || put_ଧାଡ଼ି(fd, "set winheight=1") == FAIL
		    || put_ଧାଡ଼ି(fd, "set winminwidth=0") == FAIL
		    || put_ଧାଡ଼ି(fd, "set winwidth=1") == FAIL)
		ଯାଅ fail;
	    restore_height_width = TRUE;
	}
	ଯଦି (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    ଯାଅ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ଯଦି ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ଖାଲି)
	{
	    ଯଦି (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		ଯାଅ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	ପାଇଁ (wp = tab_firstwin; wp != ଖାଲି; wp = wp->w_next)
	{
	    ଯଦି (!ses_do_win(wp))
		ଜାରି;
	    ଯଦି (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ଅନ୍ୟଥା
							 ଖାଲି
# endif
		 ) == FAIL)
		ଯାଅ fail;
	    ଯଦି (nr > 1 && put_ଧାଡ଼ି(fd, "wincmd w") == FAIL)
		ଯାଅ fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ଯଦି (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    ଯାଅ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ଯଦି (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    ଯାଅ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ଯଦି (!(ssop_flags & SSOP_TABPAGES))
	    ଭାଙ୍ଗ;
    }

    ଯଦି (ssop_flags & SSOP_TABPAGES)
    {
	ଯଦି (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    ଯାଅ fail;
    }
    ଯଦି (restore_stal && put_ଧାଡ଼ି(fd, "set stal=1") == FAIL)
	ଯାଅ fail;

    // Wipe out an empty unnamed buffer we started in.
    ଯଦି (put_ଧାଡ଼ି(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	ଯାଅ fail;
    ଯଦି (put_ଧାଡ଼ି(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	ଯାଅ fail;
    ଯଦି (put_ଧାଡ଼ି(fd, "endif") == FAIL)
	ଯାଅ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ଯଦି (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	ଯାଅ fail;

    // Restore 'shortmess'.
    ଯଦି (ssop_flags & SSOP_OPTIONS)
    {
	ଯଦି (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    ଯାଅ fail;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (put_ଧାଡ଼ି(fd, "&shortmess = shortmess_save") == FAIL)
	    ଯାଅ fail;
    }

    ଯଦି (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ଯଦି (put_ଧାଡ଼ି(fd, "&winminheight = save_winminheight") == FAIL
	      || put_ଧାଡ଼ି(fd, "&winminwidth = save_winminwidth") == FAIL)
	    ଯାଅ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ଯଦି (put_ଧାଡ଼ି(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_ଧାଡ଼ି(fd, "if filereadable(sx)") == FAIL
	    || put_ଧାଡ଼ି(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_ଧାଡ଼ି(fd, "endif") == FAIL)
	ଯାଅ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ଫେରନ୍ତୁ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ସ୍ଥିର char_u *
get_view_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ଯଦି (curbuf->b_ffname == ଖାଲି)
    {
	emsg(_(e_no_file_name));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    sname = home_replace_save(ଖାଲି, curbuf->b_ffname);
    ଯଦି (sname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    ପାଇଁ (p = sname; *p; ++p)
	ଯଦି (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ଯଦି (retval != ଖାଲି)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	ପାଇଁ (p = sname; *p; ++p)
	{
	    ଯଦି (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ଅନ୍ୟଥା ଯଦି (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ଯଦି defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ଯଦି (*p == ':')
		    *s++ = '-';
		ଅନ୍ୟଥା
# endif
		    *s++ = '+';
	    }
	    ଅନ୍ୟଥା
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ଫେରନ୍ତୁ retval;
}

/*
 * ":loadview [nr]"
 */
    ଶୂନ୍ୟ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    (ଶୂନ୍ୟ)do_source(fname, FALSE, DOSO_NONE, ଖାଲି);
    vim_free(fname);
}

# ଯଦି defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    ଅକ୍ଷର	    *mksession_cmdline;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    save_ssop_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ଯଦି (escaped_filename == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ଯଦି (mksession_cmdline == ଖାଲି)
    {
	vim_free(escaped_filename);
	ଫେରନ୍ତୁ FALSE;
    }
    strcpy(mksession_cmdline, "mksession ");
    STRCAT(mksession_cmdline, escaped_filename);
    vim_free(escaped_filename);

    // Use a reasonable hardcoded set of 'sessionoptions' flags to avoid
    // unpredictable effects when the session is saved automatically.  Also,
    // we definitely need SSOP_GLOBALS to be able to restore v:this_session.
    // Don't use SSOP_BUFFERS to prevent the buffer list from becoming
    // enormously large if the GNOME session feature is used regularly.
    save_ssop_flags = ssop_flags;
    ssop_flags = (SSOP_BLANK|SSOP_CURDIR|SSOP_FOLDS|SSOP_GLOBALS
		  |SSOP_HELP|SSOP_OPTIONS|SSOP_WINSIZE|SSOP_TABPAGES);

    do_cmdline_cmd((char_u *)"let Save_VV_this_session = v:this_session");
    failed = (do_cmdline_cmd((char_u *)mksession_cmdline) == FAIL);
    do_cmdline_cmd((char_u *)"let v:this_session = Save_VV_this_session");
    do_unlet((char_u *)"Save_VV_this_session", TRUE);

    ssop_flags = save_ssop_flags;
    vim_free(mksession_cmdline);

    // Reopen the file and append a command to restore v:this_session,
    // as if this save never happened.	This is to avoid conflicts with
    // the user's own sessions.  FIXME: It's probably less hackish to add
    // a "stealth" flag to 'sessionoptions' -- gotta ask Bram.
    ଯଦି (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ଖାଲି
	       || put_ଧାଡ଼ି(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_ଧାଡ଼ି(fd, "unlet g:Save_VV_this_session") == FAIL);

	ଯଦି (fd != ଖାଲି && fclose(fd) != 0)
	    failed = TRUE;

	ଯଦି (failed)
	    mch_remove(filename);
    }

    ଫେରନ୍ତୁ !failed;
}
# endif

#endif // FEAT_SESSION

#ଯଦି defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    ଶୂନ୍ୟ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = ଖାଲି;
#endif
#ifdef FEAT_SESSION
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		view_session = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ଖାଲି;
    unsigned	*flagp;
#endif
#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sid;
    scriptitem_T *si = ଖାଲି;
#endif

    ଯଦି (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#ଅନ୍ୟଥା
	ex_ni(eap);
	ଫେରନ୍ତୁ;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ଯଦି (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ଯଦି (fname == ଖାଲି)
	    ଫେରନ୍ତୁ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି (*eap->arg != NUL)
	fname = eap->arg;
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    ଅନ୍ୟଥା
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ଖାଲି,
		(char_u *)_(BROWSE_FILTER_MACROS), ଖାଲି);
	ଯଦି (browseFile == ଖାଲି)
	    ଯାଅ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ଯଦି defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ଯଦି (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ଯଦି (fd != ଖାଲି)
    {
#ifdef FEAT_SESSION
	ଯଦି (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ଅନ୍ୟଥା
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ଯଦି (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ଯଦି (put_ଧାଡ଼ି(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ଯଦି (eap->cmdidx == CMD_mkvimrc)
	    (ଶୂନ୍ୟ)put_ଧାଡ଼ି(fd, "version 6.0");

#ifdef FEAT_SESSION
	ଯଦି (eap->cmdidx == CMD_mksession)
	{
	    ଯଦି (put_ଧାଡ଼ି(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ଯଦି (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ଯଦି (p_cp)
		(ଶୂନ୍ୟ)put_ଧାଡ଼ି(fd, "if !&cp | set cp | endif");
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)put_ଧାଡ଼ି(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ଯଦି (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_ଧାଡ଼ି(fd, "var cpo_save: string") == FAIL;

	    ଯଦି (eap->cmdidx == CMD_mksession)
	    {
		ଯଦି (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ଯଦି (do_mappings)
		failed |= (makemap(fd, ଖାଲି) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ଯଦି defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    ପାଇଁ (sid = 1; sid <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ଯଦି ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    ପାଇଁ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ଯଦି ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ଜାରି;

			// compare prefixes if available
			ଯଦି (si->sn_autoload_prefix != ଖାଲି && b_si->sn_autoload_prefix != ଖାଲି
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    ଭାଙ୍ଗ;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ଯଦି (STRCMP(name, b_name) == 0)
			    ଭାଙ୍ଗ;
		    }

		    // import the auto script if there are no conflicts
		    ଯଦି (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ଯଦି (!failed && view_session)
	{
	    ଯଦି (put_ଧାଡ଼ି(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ଯଦି (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ଯଦି (dirnow == ଖାଲି)
		    failed = TRUE;
		ଅନ୍ୟଥା
		{
		    // Change to session file's dir.
		    ଯଦି (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((ଅକ୍ଷର *)dirnow) != 0)
			*dirnow = NUL;
		    ଯଦି (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ଯଦି (vim_chdirfile(fname, ଖାଲି) == OK)
			    shorten_fnames(TRUE);
		    }
		    ଅନ୍ୟଥା ଯଦି (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ଖାଲି)
		    {
			ଯଦି (mch_chdir((ଅକ୍ଷର *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ଯଦି (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ଖାଲି)))
		    {
			ଯଦି (mch_chdir((ଅକ୍ଷର *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		failed |= put_ଧାଡ଼ି(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ଖାଲି) == FAIL);
	    }
	    ଯଦି (put_ଧାଡ଼ି(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_ସନ୍ଧାନ_EXTRA
	    ଯଦି (no_hlsearch && put_ଧାଡ଼ି(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ଯଦି (put_ଧାଡ଼ି(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ଯଦି (eap->cmdidx == CMD_mksession)
	    {
		ଯଦି (put_ଧାଡ଼ି(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ଯଦି (put_ଧାଡ଼ି(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ଯଦି (failed)
	    emsg(_(e_error_while_writing));
#ଯଦି defined(FEAT_SESSION)
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ଯଦି (tbuf != ଖାଲି)
	    {
		ଯଦି (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
		    set_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#endif
#ifdef MKSESSION_NL
	mksession_nl = FALSE;
#endif
    }

#ifdef FEAT_BROWSE
theend:
    vim_free(browseFile);
#endif
#ifdef FEAT_SESSION
    vim_free(viewFile);
#endif

    apply_autocmds(EVENT_SESSIONWRITEPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
}

#ଯଦି defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ଯଦି (ASCII_ISUPPER(*p))
    {
	ଯେତେବେଳେ (*(++p))
	    ଯଦି (ASCII_ISLOWER(*p))
		ଫେରନ୍ତୁ VAR_FLAVOUR_SESSION;
	ଫେରନ୍ତୁ VAR_FLAVOUR_VIMINFO;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_eol(FILE *fd)
{
    ଯଦି (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_ଧାଡ଼ି(FILE *fd, ଅକ୍ଷର *s)
{
    ଯଦି (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}
