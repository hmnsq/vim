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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_SESSION)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;

    sname = home_replace_save(ರಿಕ್ತ, name);
    ಯದಿ (sname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	ಪ್ರತಿ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ಯದಿ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // write the result
    ಯದಿ (fputs((ವರ್ಣ *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, ಪೂರ್ಣಾಙ್ಕ add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ಯದಿ (buf->b_sfname != ರಿಕ್ತ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ಅನ್ಯಥಾ
	name = buf->b_ffname;
    ಯದಿ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_arglist(
    FILE	*fd,
    ವರ್ಣ	*cmd,
    garray_T	*gap,
    ಪೂರ್ಣಾಙ್ಕ		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*buf = ರಿಕ್ತ;
    char_u	*s;

    ಯದಿ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (put_ಸಾಲು(fd, ":%argdel") == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ಯದಿ (s != ರಿಕ್ತ)
	{
	    ಯದಿ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ಯದಿ (buf != ರಿಕ್ತ)
		{
		    (ಶೂನ್ಯ)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ಯದಿ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ಯದಿ (bt_terminal(wp->w_buffer))
	ಪ್ರತ್ಯಾಗಮ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ಯದಿ (wp->w_buffer->b_fname == ರಿಕ್ತ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ಪ್ರತ್ಯಾಗಮ (ssop_flags & SSOP_BLANK);
    ಯದಿ (bt_help(wp->w_buffer))
	ಪ್ರತ್ಯಾಗಮ (ssop_flags & SSOP_HELP);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ಯದಿ (fr->fr_layout == FR_LEAF)
	ಪ್ರತ್ಯಾಗಮ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ಯದಿ (ses_do_frame(frc))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ಸ್ಥಿರ frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	ಯದಿ (ses_do_frame(frc))
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

    ಯದಿ (fr->fr_layout == FR_LEAF)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ಯದಿ (frc != ರಿಕ್ತ)
	ಯಾವತ್ ((frc = ses_skipframe(frc->fr_next)) != ರಿಕ್ತ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ಯದಿ (put_ಸಾಲು(fd, "wincmd _ | wincmd |") == FAIL
		    || put_ಸಾಲು(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ++ಎಣಿಕೆ;
	}

    // Go back to the first window.
    ಯದಿ (ಎಣಿಕೆ > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", ಎಣಿಕೆ) < 0
		|| put_eol(fd) == FAIL))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ಯಾವತ್ (frc != ರಿಕ್ತ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ಯದಿ (frc != ರಿಕ್ತ && put_ಸಾಲು(fd, "wincmd w") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ses_winsizes(
    FILE	*fd,
    ಪೂರ್ಣಾಙ್ಕ		restore_size,
    win_T	*tab_firstwin)
{
    ಪೂರ್ಣಾಙ್ಕ		n = 0;
    win_T	*wp;

    ಯದಿ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	ಪ್ರತಿ (wp = tab_firstwin; wp != ರಿಕ್ತ; wp = wp->w_next)
	{
	    ಯದಿ (!ses_do_win(wp))
		ಅನುವರ್ತ;
	    ++n;

	    // restore height when not full height
	    ಯದಿ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    // restore width when not full width
	    ಯದಿ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Just equalise window sizes
	ಯದಿ (put_ಸಾಲು(fd, "wincmd =") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_view_curpos(FILE *fd, win_T *wp, ವರ್ಣ *spaces)
{
    ಪೂರ್ಣಾಙ್ಕ r;

    ಯದಿ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ಅನ್ಯಥಾ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ಪ್ರತ್ಯಾಗಮ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ಪೂರ್ಣಾಙ್ಕ		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    ಪೂರ್ಣಾಙ್ಕ		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ಪೂರ್ಣಾಙ್ಕ		f;
    ಪೂರ್ಣಾಙ್ಕ		do_cursor;
    ಪೂರ್ಣಾಙ್ಕ		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ಯದಿ (wp->w_alist == &global_alist)
    {
	ಯದಿ (put_ಸಾಲು(fd, "argglobal") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ರಿಕ್ತ
			|| wp->w_localdir != ರಿಕ್ತ, flagp) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ಯದಿ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ಯದಿ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ಯದಿ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ಯದಿ (bt_help(wp->w_buffer))
	{
	    ವರ್ಣ *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ಯದಿ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (ವರ್ಣ *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ಯದಿ (put_ಸಾಲು(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
# ifdef FEAT_TERMINAL
	ಅನ್ಯಥಾ ಯದಿ (bt_terminal(wp->w_buffer))
	{
	    ಯದಿ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
# endif
	// Load the file.
	ಅನ್ಯಥಾ ಯದಿ (wp->w_buffer->b_ffname != ರಿಕ್ತ
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ಯದಿ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    // No file in this buffer, just make it empty.
	    ಯದಿ (put_ಸಾಲು(fd, "enew") == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
# ifdef FEAT_QUICKFIX
	    ಯದಿ (wp->w_buffer->b_ffname != ರಿಕ್ತ)
	    {
		// The buffer does have a name, but it's not a file name.
		ಯದಿ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ಯದಿ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ಯದಿ ((flagp == &ssop_flags)
		&& alt != ರಿಕ್ತ
		&& alt->b_fname != ರಿಕ್ತ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Local mappings and abbreviations.
    ಯದಿ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ಯದಿ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ಅನ್ಯಥಾ ಯದಿ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ಅನ್ಯಥಾ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ಯದಿ (f == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ಯದಿ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != ರಿಕ್ತ
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ಯದಿ (put_folds(fd, wp) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ಯದಿ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ಯದಿ (put_ಸಾಲು(fd, "{") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (wp->w_height <= 0)
	{
	    ಯದಿ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (put_eol(fd) == FAIL
		|| put_ಸಾಲು(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_ಸಾಲು(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_ಸಾಲು(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ಯದಿ (wp->w_cursor.col == 0)
	{
	    ಯದಿ (put_ಸಾಲು(fd, "  normal! 0") == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ಯದಿ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_ಸಾಲು(fd, "  endif") == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (put_view_curpos(fd, wp, "  ") == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (put_ಸಾಲು(fd, "}") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ಯದಿ (wp->w_localdir != ರಿಕ್ತ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ಯದಿ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	did_lcd = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

# ifdef FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    char_u	*p, *t;

    todo = (ಪೂರ್ಣಾಙ್ಕ)gvht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(gvht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ಯದಿ ((this_var->di_tv.v_type == VAR_ಸಂಖ್ಯೆ
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ಯದಿ (p == ರಿಕ್ತ)	    // out of memory
		    ಭಙ್ಗ;
		ಪ್ರತಿ (t = p; *t != NUL; ++t)
		    ಯದಿ (*t == '\n')
			*t = 'n';
		    ಅನ್ಯಥಾ ಯದಿ (*t == '\r')
			*t = 'r';
		ಯದಿ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_ಕೀಲಿ,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		vim_free(p);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ಪೂರ್ಣಾಙ್ಕ sign = ' ';

		ಯದಿ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ಯದಿ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_ಕೀಲಿ, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		only_save_windows = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		nr;
    ಪೂರ್ಣಾಙ್ಕ		restore_size = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    ಪೂರ್ಣಾಙ್ಕ		cur_arg_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		next_arg_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ಯದಿ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ಯದಿ (put_ಸಾಲು(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	ಗಚ್ಛ fail;

    ಯದಿ (put_ಸಾಲು(fd, "doautoall SessionLoadPre") == FAIL)
	ಗಚ್ಛ fail;

    ಯದಿ (put_ಸಾಲು(fd, "var save_splitbelow: bool") == FAIL
	    || put_ಸಾಲು(fd, "var save_splitright: bool") == FAIL
	    || put_ಸಾಲು(fd, "var save_winminheight: number") == FAIL
	    || put_ಸಾಲು(fd, "var save_winminwidth: number") == FAIL
	    || put_ಸಾಲು(fd, "var wipebuf: number = -1") == FAIL
	    || put_ಸಾಲು(fd, "var shortmess_save: string") == FAIL)
	ಗಚ್ಛ fail;

    ಯದಿ (ssop_flags & SSOP_GLOBALS)
	ಯದಿ (store_session_globals(fd) == FAIL)
	    ಗಚ್ಛ fail;
# endif

    // Close all windows and tabs but one.
    ಯದಿ (put_ಸಾಲು(fd, "silent only") == FAIL)
	ಗಚ್ಛ fail;
    ಯದಿ ((ssop_flags & SSOP_TABPAGES)
	    && put_ಸಾಲು(fd, "silent tabonly") == FAIL)
	ಗಚ್ಛ fail;

    // Now a :cd command to the session directory or the current directory
    ಯದಿ (ssop_flags & SSOP_SESDIR)
    {
	ಯದಿ (put_ಸಾಲು(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    ಗಚ್ಛ fail;
    }
    ಅನ್ಯಥಾ ಯದಿ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ರಿಕ್ತ, globaldir != ರಿಕ್ತ ? globaldir : dirnow);
	ಯದಿ (sname == ರಿಕ್ತ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    ಗಚ್ಛ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ಯದಿ (put_ಸಾಲು(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	ಗಚ್ಛ fail;
    ಯದಿ (put_ಸಾಲು(fd, "  wipebuf = bufnr('%')") == FAIL)
	ಗಚ್ಛ fail;
    ಯದಿ (put_ಸಾಲು(fd, "endif") == FAIL)
	ಗಚ್ಛ fail;

    // Save 'shortmess' if not storing options.
    ಯದಿ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_ಸಾಲು(fd, "shortmess_save = &shortmess") == FAIL)
	ಗಚ್ಛ fail;

    // Set 'shortmess' for the following.
    ಯದಿ (put_ಸಾಲು(fd, "set shortmess+=aoO") == FAIL)
	ಗಚ್ಛ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ಯದಿ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ರಿಕ್ತ
		&& buf->b_p_bl)
	{
	    ಯದಿ (fprintf(fd, "badd +%ld ", buf->b_wininfo == ರಿಕ್ತ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		ಗಚ್ಛ fail;
	}
    }

    // the global argument list
    ಯದಿ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	ಗಚ್ಛ fail;

    ಯದಿ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ಯದಿ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    ಗಚ್ಛ fail;
    }

# ifdef FEAT_GUI
    ಯದಿ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	ಪೂರ್ಣಾಙ್ಕ	x, y;

	ಯದಿ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ಯದಿ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		ಗಚ್ಛ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ಯದಿ (p_stal == 1 && first_tabpage->tp_next != ರಿಕ್ತ)
    {
	ಯದಿ (put_ಸಾಲು(fd, "set stal=2") == FAIL)
	    ಗಚ್ಛ fail;
	restore_stal = TRUE;
    }

    ಯದಿ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ಯದಿ (tp->tp_next != ರಿಕ್ತ
		  && put_ಸಾಲು(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		ಗಚ್ಛ fail;
	ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ && put_ಸಾಲು(fd, "tabrewind") == FAIL)
	    ಗಚ್ಛ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ಪೂರ್ಣಾಙ್ಕ	need_tabnext = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ಯದಿ ((ssop_flags & SSOP_TABPAGES))
	{
	    ಯದಿ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ಅನ್ಯಥಾ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ಯದಿ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	ಪ್ರತಿ (wp = tab_firstwin; wp != ರಿಕ್ತ; wp = wp->w_next)
	{
	    ಯದಿ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != ರಿಕ್ತ
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ಯದಿ (need_tabnext && put_ಸಾಲು(fd, "tabnext") == FAIL)
		    ಗಚ್ಛ fail;
		need_tabnext = FALSE;

		ಯದಿ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    ಗಚ್ಛ fail;
		ಯದಿ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ಭಙ್ಗ;
	    }
	}

	// If no file got edited create an empty tab page.
	ಯದಿ (need_tabnext && put_ಸಾಲು(fd, "tabnext") == FAIL)
	    ಗಚ್ಛ fail;

	ಯದಿ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ಯದಿ (put_ಸಾಲು(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_ಸಾಲು(fd, "save_splitright = &splitright")
								       == FAIL)
		ಗಚ್ಛ fail;
	    ಯದಿ (put_ಸಾಲು(fd, "set splitbelow splitright") == FAIL)
		ಗಚ್ಛ fail;
	    ಯದಿ (ses_win_rec(fd, tab_topframe) == FAIL)
		ಗಚ್ಛ fail;
	    ಯದಿ (put_ಸಾಲು(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_ಸಾಲು(fd, "&splitright = save_splitright")
								       == FAIL)
		ಗಚ್ಛ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	ಪ್ರತಿ (wp = tab_firstwin; wp != ರಿಕ್ತ; wp = W_NEXT(wp))
	{
	    ಯದಿ (ses_do_win(wp))
		++nr;
	    ಅನ್ಯಥಾ
		restore_size = FALSE;
	    ಯದಿ (curwin == wp)
		cnr = nr;
	}

	ಯದಿ (tab_firstwin->w_next != ರಿಕ್ತ)
	{
	    // Go to the first window.
	    ಯದಿ (put_ಸಾಲು(fd, "wincmd t") == FAIL)
		ಗಚ್ಛ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ಯದಿ (!restore_height_width
		    && (put_ಸಾಲು(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_ಸಾಲು(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		ಗಚ್ಛ fail;
	    ಯದಿ (put_ಸಾಲು(fd, "set winminheight=0") == FAIL
		    || put_ಸಾಲು(fd, "set winheight=1") == FAIL
		    || put_ಸಾಲು(fd, "set winminwidth=0") == FAIL
		    || put_ಸಾಲು(fd, "set winwidth=1") == FAIL)
		ಗಚ್ಛ fail;
	    restore_height_width = TRUE;
	}
	ಯದಿ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    ಗಚ್ಛ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ಯದಿ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ರಿಕ್ತ)
	{
	    ಯದಿ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		ಗಚ್ಛ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	ಪ್ರತಿ (wp = tab_firstwin; wp != ರಿಕ್ತ; wp = wp->w_next)
	{
	    ಯದಿ (!ses_do_win(wp))
		ಅನುವರ್ತ;
	    ಯದಿ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ಅನ್ಯಥಾ
							 ರಿಕ್ತ
# endif
		 ) == FAIL)
		ಗಚ್ಛ fail;
	    ಯದಿ (nr > 1 && put_ಸಾಲು(fd, "wincmd w") == FAIL)
		ಗಚ್ಛ fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ಯದಿ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    ಗಚ್ಛ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ಯದಿ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    ಗಚ್ಛ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ಯದಿ (!(ssop_flags & SSOP_TABPAGES))
	    ಭಙ್ಗ;
    }

    ಯದಿ (ssop_flags & SSOP_TABPAGES)
    {
	ಯದಿ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    ಗಚ್ಛ fail;
    }
    ಯದಿ (restore_stal && put_ಸಾಲು(fd, "set stal=1") == FAIL)
	ಗಚ್ಛ fail;

    // Wipe out an empty unnamed buffer we started in.
    ಯದಿ (put_ಸಾಲು(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	ಗಚ್ಛ fail;
    ಯದಿ (put_ಸಾಲು(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	ಗಚ್ಛ fail;
    ಯದಿ (put_ಸಾಲು(fd, "endif") == FAIL)
	ಗಚ್ಛ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ಯದಿ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	ಗಚ್ಛ fail;

    // Restore 'shortmess'.
    ಯದಿ (ssop_flags & SSOP_OPTIONS)
    {
	ಯದಿ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    ಗಚ್ಛ fail;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (put_ಸಾಲು(fd, "&shortmess = shortmess_save") == FAIL)
	    ಗಚ್ಛ fail;
    }

    ಯದಿ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ಯದಿ (put_ಸಾಲು(fd, "&winminheight = save_winminheight") == FAIL
	      || put_ಸಾಲು(fd, "&winminwidth = save_winminwidth") == FAIL)
	    ಗಚ್ಛ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ಯದಿ (put_ಸಾಲು(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_ಸಾಲು(fd, "if filereadable(sx)") == FAIL
	    || put_ಸಾಲು(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_ಸಾಲು(fd, "endif") == FAIL)
	ಗಚ್ಛ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ಸ್ಥಿರ char_u *
get_view_file(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ಯದಿ (curbuf->b_ffname == ರಿಕ್ತ)
    {
	emsg(_(e_no_file_name));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    sname = home_replace_save(ರಿಕ್ತ, curbuf->b_ffname);
    ಯದಿ (sname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    ಪ್ರತಿ (p = sname; *p; ++p)
	ಯದಿ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ಯದಿ (retval != ರಿಕ್ತ)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	ಪ್ರತಿ (p = sname; *p; ++p)
	{
	    ಯದಿ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ಅನ್ಯಥಾ ಯದಿ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ಯದಿ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ಯದಿ (*p == ':')
		    *s++ = '-';
		ಅನ್ಯಥಾ
# endif
		    *s++ = '+';
	    }
	    ಅನ್ಯಥಾ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * ":loadview [nr]"
 */
    ಶೂನ್ಯ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    (ಶೂನ್ಯ)do_source(fname, FALSE, DOSO_NONE, ರಿಕ್ತ);
    vim_free(fname);
}

# ಯದಿ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ಪೂರ್ಣಾಙ್ಕ
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    ವರ್ಣ	    *mksession_cmdline;
    unsigned ಪೂರ್ಣಾಙ್ಕ    save_ssop_flags;
    ಪೂರ್ಣಾಙ್ಕ		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ಯದಿ (escaped_filename == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ಯದಿ (mksession_cmdline == ರಿಕ್ತ)
    {
	vim_free(escaped_filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಯದಿ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ರಿಕ್ತ
	       || put_ಸಾಲು(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_ಸಾಲು(fd, "unlet g:Save_VV_this_session") == FAIL);

	ಯದಿ (fd != ರಿಕ್ತ && fclose(fd) != 0)
	    failed = TRUE;

	ಯದಿ (failed)
	    mch_remove(filename);
    }

    ಪ್ರತ್ಯಾಗಮ !failed;
}
# endif

#endif // FEAT_SESSION

#ಯದಿ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    ಶೂನ್ಯ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = ರಿಕ್ತ;
#endif
#ifdef FEAT_SESSION
    ಪೂರ್ಣಾಙ್ಕ		view_session = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ರಿಕ್ತ;
    unsigned	*flagp;
#endif
#ಯದಿ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ		sid;
    scriptitem_T *si = ರಿಕ್ತ;
#endif

    ಯದಿ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#ಅನ್ಯಥಾ
	ex_ni(eap);
	ಪ್ರತ್ಯಾಗಮ;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ಯದಿ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ಯದಿ (fname == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (*eap->arg != NUL)
	fname = eap->arg;
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    ಅನ್ಯಥಾ
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ಯದಿ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ರಿಕ್ತ,
		(char_u *)_(BROWSE_FILTER_MACROS), ರಿಕ್ತ);
	ಯದಿ (browseFile == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ಯದಿ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ಯದಿ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ಯದಿ (fd != ರಿಕ್ತ)
    {
#ifdef FEAT_SESSION
	ಯದಿ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ಅನ್ಯಥಾ
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ಯದಿ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ಯದಿ (put_ಸಾಲು(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ಯದಿ (eap->cmdidx == CMD_mkvimrc)
	    (ಶೂನ್ಯ)put_ಸಾಲು(fd, "version 6.0");

#ifdef FEAT_SESSION
	ಯದಿ (eap->cmdidx == CMD_mksession)
	{
	    ಯದಿ (put_ಸಾಲು(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ಯದಿ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ಯದಿ (p_cp)
		(ಶೂನ್ಯ)put_ಸಾಲು(fd, "if !&cp | set cp | endif");
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)put_ಸಾಲು(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ಯದಿ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    ಪೂರ್ಣಾಙ್ಕ flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_ಸಾಲು(fd, "var cpo_save: string") == FAIL;

	    ಯದಿ (eap->cmdidx == CMD_mksession)
	    {
		ಯದಿ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ಯದಿ (do_mappings)
		failed |= (makemap(fd, ರಿಕ್ತ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ಯದಿ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    ಪ್ರತಿ (sid = 1; sid <= script_ವಸ್ತುಗಳು.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ಯದಿ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ಪೂರ್ಣಾಙ್ಕ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    ಪ್ರತಿ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ಯದಿ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ಅನುವರ್ತ;

			// compare prefixes if available
			ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ && b_si->sn_autoload_prefix != ರಿಕ್ತ
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    ಭಙ್ಗ;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ಯದಿ (STRCMP(name, b_name) == 0)
			    ಭಙ್ಗ;
		    }

		    // import the auto script if there are no conflicts
		    ಯದಿ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ಯದಿ (!failed && view_session)
	{
	    ಯದಿ (put_ಸಾಲು(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ಯದಿ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ಯದಿ (dirnow == ರಿಕ್ತ)
		    failed = TRUE;
		ಅನ್ಯಥಾ
		{
		    // Change to session file's dir.
		    ಯದಿ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((ವರ್ಣ *)dirnow) != 0)
			*dirnow = NUL;
		    ಯದಿ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ಯದಿ (vim_chdirfile(fname, ರಿಕ್ತ) == OK)
			    shorten_fnames(TRUE);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ರಿಕ್ತ)
		    {
			ಯದಿ (mch_chdir((ವರ್ಣ *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ಯದಿ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ರಿಕ್ತ)))
		    {
			ಯದಿ (mch_chdir((ವರ್ಣ *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		failed |= put_ಸಾಲು(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ರಿಕ್ತ) == FAIL);
	    }
	    ಯದಿ (put_ಸಾಲು(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	    ಯದಿ (no_hlsearch && put_ಸಾಲು(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ಯದಿ (put_ಸಾಲು(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ಯದಿ (eap->cmdidx == CMD_mksession)
	    {
		ಯದಿ (put_ಸಾಲು(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ಯದಿ (put_ಸಾಲು(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ಯದಿ (failed)
	    emsg(_(e_error_while_writing));
#ಯದಿ defined(FEAT_SESSION)
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ಯದಿ (tbuf != ರಿಕ್ತ)
	    {
		ಯದಿ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
}

#ಯದಿ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ಯದಿ (ASCII_ISUPPER(*p))
    {
	ಯಾವತ್ (*(++p))
	    ಯದಿ (ASCII_ISLOWER(*p))
		ಪ್ರತ್ಯಾಗಮ VAR_FLAVOUR_SESSION;
	ಪ್ರತ್ಯಾಗಮ VAR_FLAVOUR_VIMINFO;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_eol(FILE *fd)
{
    ಯದಿ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_ಸಾಲು(FILE *fd, ವರ್ಣ *s)
{
    ಯದಿ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}
