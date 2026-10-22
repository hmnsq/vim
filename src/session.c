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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_SESSION)

स्थिर पूर्णाङ्क did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    स्थिर पूर्णाङ्क
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    पूर्णाङ्क		retval = OK;

    sname = home_replace_save(रिक्त, name);
    जइ (sname == रिक्त)
	प्रत्यागम FAIL;

    जइ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	प्रति (p = sname; *p != NUL; MB_PTR_ADV(p))
	    जइ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    जइ (p == रिक्त)
	प्रत्यागम FAIL;

    // write the result
    जइ (fputs((वर्ण *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    प्रत्यागम retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    स्थिर पूर्णाङ्क
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, पूर्णाङ्क add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    जइ (buf->b_sfname != रिक्त
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    अण्णहा
	name = buf->b_ffname;
    जइ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    स्थिर पूर्णाङ्क
ses_arglist(
    FILE	*fd,
    वर्ण	*cmd,
    garray_T	*gap,
    पूर्णाङ्क		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    पूर्णाङ्क		i;
    char_u	*buf = रिक्त;
    char_u	*s;

    जइ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	प्रत्यागम FAIL;
    जइ (put_पङ्क्तिः(fd, ":%argdel") == FAIL)
	प्रत्यागम FAIL;
    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	जइ (s != रिक्त)
	{
	    जइ (fullname)
	    {
		buf = alloc(MAXPATHL);
		जइ (buf != रिक्त)
		{
		    (शून्य)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    जइ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		प्रत्यागम FAIL;
	    }
	    vim_free(buf);
	}
    }
    प्रत्यागम OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    स्थिर पूर्णाङ्क
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    जइ (bt_terminal(wp->w_buffer))
	प्रत्यागम !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    जइ (wp->w_buffer->b_fname == रिक्त
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	प्रत्यागम (ssop_flags & SSOP_BLANK);
    जइ (bt_help(wp->w_buffer))
	प्रत्यागम (ssop_flags & SSOP_HELP);
    प्रत्यागम TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    स्थिर पूर्णाङ्क
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    जइ (fr->fr_layout == FR_LEAF)
	प्रत्यागम ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	जइ (ses_do_frame(frc))
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    स्थिर frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	जइ (ses_do_frame(frc))
	    भङ्ग;
    प्रत्यागम frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    स्थिर पूर्णाङ्क
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    पूर्णाङ्क		गणनम् = 0;

    जइ (fr->fr_layout == FR_LEAF)
	प्रत्यागम OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    जइ (frc != रिक्त)
	जाव ((frc = ses_skipframe(frc->fr_next)) != रिक्त)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    जइ (put_पङ्क्तिः(fd, "wincmd _ | wincmd |") == FAIL
		    || put_पङ्क्तिः(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		प्रत्यागम FAIL;
	    ++गणनम्;
	}

    // Go back to the first window.
    जइ (गणनम् > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", गणनम्) < 0
		|| put_eol(fd) == FAIL))
	प्रत्यागम FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    जाव (frc != रिक्त)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	जइ (frc != रिक्त && put_पङ्क्तिः(fd, "wincmd w") == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
ses_winsizes(
    FILE	*fd,
    पूर्णाङ्क		restore_size,
    win_T	*tab_firstwin)
{
    पूर्णाङ्क		n = 0;
    win_T	*wp;

    जइ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	प्रति (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जइ (!ses_do_win(wp))
		अनुवर्त;
	    ++n;

	    // restore height when not full height
	    जइ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		प्रत्यागम FAIL;

	    // restore width when not full width
	    जइ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		प्रत्यागम FAIL;
	}
    }
    अण्णहा
    {
	// Just equalise window sizes
	जइ (put_पङ्क्तिः(fd, "wincmd =") == FAIL)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
put_view_curpos(FILE *fd, win_T *wp, वर्ण *spaces)
{
    पूर्णाङ्क r;

    जइ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    अण्णहा
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    प्रत्यागम r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    स्थिर पूर्णाङ्क
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    पूर्णाङ्क		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    पूर्णाङ्क		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    पूर्णाङ्क		f;
    पूर्णाङ्क		do_cursor;
    पूर्णाङ्क		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    जइ (wp->w_alist == &global_alist)
    {
	जइ (put_पङ्क्तिः(fd, "argglobal") == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा
    {
	जइ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != रिक्त
			|| wp->w_localdir != रिक्त, flagp) == FAIL)
	    प्रत्यागम FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    जइ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	जइ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    प्रत्यागम FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    जइ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	जइ (bt_help(wp->w_buffer))
	{
	    वर्ण *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    जइ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (वर्ण *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    जइ (put_पङ्क्तिः(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		प्रत्यागम FAIL;
	}
# ifdef FEAT_TERMINAL
	अण्णहा जइ (bt_terminal(wp->w_buffer))
	{
	    जइ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		प्रत्यागम FAIL;
	}
# endif
	// Load the file.
	अण्णहा जइ (wp->w_buffer->b_ffname != रिक्त
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    जइ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    // No file in this buffer, just make it empty.
	    जइ (put_पङ्क्तिः(fd, "enew") == FAIL)
		प्रत्यागम FAIL;
# ifdef FEAT_QUICKFIX
	    जइ (wp->w_buffer->b_ffname != रिक्त)
	    {
		// The buffer does have a name, but it's not a file name.
		जइ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    प्रत्यागम FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    जइ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	जइ ((flagp == &ssop_flags)
		&& alt != रिक्त
		&& alt->b_fname != रिक्त
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    प्रत्यागम FAIL;
    }

    // Local mappings and abbreviations.
    जइ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	प्रत्यागम FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    जइ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    अण्णहा जइ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    अण्णहा
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    जइ (f == FAIL)
	प्रत्यागम FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    जइ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != रिक्त
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	जइ (put_folds(fd, wp) == FAIL)
	    प्रत्यागम FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    जइ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	जइ (put_पङ्क्तिः(fd, "{") == FAIL)
	    प्रत्यागम FAIL;

	जइ (wp->w_height <= 0)
	{
	    जइ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    प्रत्यागम FAIL;

	जइ (put_eol(fd) == FAIL
		|| put_पङ्क्तिः(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_पङ्क्तिः(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_पङ्क्तिः(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    प्रत्यागम FAIL;
	// Restore the cursor column and left offset when not wrapping.
	जइ (wp->w_cursor.col == 0)
	{
	    जइ (put_पङ्क्तिः(fd, "  normal! 0") == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    जइ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		जइ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_पङ्क्तिः(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_पङ्क्तिः(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_पङ्क्तिः(fd, "  endif") == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा जइ (put_view_curpos(fd, wp, "  ") == FAIL)
		प्रत्यागम FAIL;
	}

	जइ (put_पङ्क्तिः(fd, "}") == FAIL)
	    प्रत्यागम FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    जइ (wp->w_localdir != रिक्त
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	जइ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    प्रत्यागम FAIL;
	did_lcd = TRUE;
    }

    प्रत्यागम OK;
}

# ifdef FEAT_EVAL
    स्थिर पूर्णाङ्क
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    पूर्णाङ्क		todo;
    char_u	*p, *t;

    todo = (पूर्णाङ्क)gvht->ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(gvht, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    जइ ((this_var->di_tv.v_type == VAR_सङ्ख्या
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_कुञ्चिका) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		जइ (p == रिक्त)	    // out of memory
		    भङ्ग;
		प्रति (t = p; *t != NUL; ++t)
		    जइ (*t == '\n')
			*t = 'n';
		    अण्णहा जइ (*t == '\r')
			*t = 'r';
		जइ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_कुञ्चिका,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    प्रत्यागम FAIL;
		}
		vim_free(p);
	    }
	    अण्णहा जइ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_कुञ्चिका) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		पूर्णाङ्क sign = ' ';

		जइ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		जइ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_कुञ्चिका, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}
    }
    प्रत्यागम OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    स्थिर पूर्णाङ्क
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    पूर्णाङ्क		only_save_windows = TRUE;
    पूर्णाङ्क		nr;
    पूर्णाङ्क		restore_size = TRUE;
    पूर्णाङ्क		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = रिक्त;
    पूर्णाङ्क		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    पूर्णाङ्क		cur_arg_idx = 0;
    पूर्णाङ्क		next_arg_idx = 0;
    पूर्णाङ्क		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    जइ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    जइ (put_पङ्क्तिः(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	गच्छ fail;

    जइ (put_पङ्क्तिः(fd, "doautoall SessionLoadPre") == FAIL)
	गच्छ fail;

    जइ (put_पङ्क्तिः(fd, "var save_splitbelow: bool") == FAIL
	    || put_पङ्क्तिः(fd, "var save_splitright: bool") == FAIL
	    || put_पङ्क्तिः(fd, "var save_winminheight: number") == FAIL
	    || put_पङ्क्तिः(fd, "var save_winminwidth: number") == FAIL
	    || put_पङ्क्तिः(fd, "var wipebuf: number = -1") == FAIL
	    || put_पङ्क्तिः(fd, "var shortmess_save: string") == FAIL)
	गच्छ fail;

    जइ (ssop_flags & SSOP_GLOBALS)
	जइ (store_session_globals(fd) == FAIL)
	    गच्छ fail;
# endif

    // Close all windows and tabs but one.
    जइ (put_पङ्क्तिः(fd, "silent only") == FAIL)
	गच्छ fail;
    जइ ((ssop_flags & SSOP_TABPAGES)
	    && put_पङ्क्तिः(fd, "silent tabonly") == FAIL)
	गच्छ fail;

    // Now a :cd command to the session directory or the current directory
    जइ (ssop_flags & SSOP_SESDIR)
    {
	जइ (put_पङ्क्तिः(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    गच्छ fail;
    }
    अण्णहा जइ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(रिक्त, globaldir != रिक्त ? globaldir : dirnow);
	जइ (sname == रिक्त
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    गच्छ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    जइ (put_पङ्क्तिः(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	गच्छ fail;
    जइ (put_पङ्क्तिः(fd, "  wipebuf = bufnr('%')") == FAIL)
	गच्छ fail;
    जइ (put_पङ्क्तिः(fd, "endif") == FAIL)
	गच्छ fail;

    // Save 'shortmess' if not storing options.
    जइ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_पङ्क्तिः(fd, "shortmess_save = &shortmess") == FAIL)
	गच्छ fail;

    // Set 'shortmess' for the following.
    जइ (put_पङ्क्तिः(fd, "set shortmess+=aoO") == FAIL)
	गच्छ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	जइ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != रिक्त
		&& buf->b_p_bl)
	{
	    जइ (fprintf(fd, "badd +%ld ", buf->b_wininfo == रिक्त ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		गच्छ fail;
	}
    }

    // the global argument list
    जइ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	गच्छ fail;

    जइ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	जइ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    गच्छ fail;
    }

# ifdef FEAT_GUI
    जइ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	पूर्णाङ्क	x, y;

	जइ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    जइ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		गच्छ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    जइ (p_stal == 1 && first_tabpage->tp_next != रिक्त)
    {
	जइ (put_पङ्क्तिः(fd, "set stal=2") == FAIL)
	    गच्छ fail;
	restore_stal = TRUE;
    }

    जइ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    जइ (tp->tp_next != रिक्त
		  && put_पङ्क्तिः(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		गच्छ fail;
	जइ (first_tabpage->tp_next != रिक्त && put_पङ्क्तिः(fd, "tabrewind") == FAIL)
	    गच्छ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	पूर्णाङ्क	need_tabnext = FALSE;
	पूर्णाङ्क	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	जइ ((ssop_flags & SSOP_TABPAGES))
	{
	    जइ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    अण्णहा
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    जइ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	अण्णहा
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	प्रति (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जइ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != रिक्त
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		जइ (need_tabnext && put_पङ्क्तिः(fd, "tabnext") == FAIL)
		    गच्छ fail;
		need_tabnext = FALSE;

		जइ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    गच्छ fail;
		जइ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		भङ्ग;
	    }
	}

	// If no file got edited create an empty tab page.
	जइ (need_tabnext && put_पङ्क्तिः(fd, "tabnext") == FAIL)
	    गच्छ fail;

	जइ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    जइ (put_पङ्क्तिः(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_पङ्क्तिः(fd, "save_splitright = &splitright")
								       == FAIL)
		गच्छ fail;
	    जइ (put_पङ्क्तिः(fd, "set splitbelow splitright") == FAIL)
		गच्छ fail;
	    जइ (ses_win_rec(fd, tab_topframe) == FAIL)
		गच्छ fail;
	    जइ (put_पङ्क्तिः(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_पङ्क्तिः(fd, "&splitright = save_splitright")
								       == FAIL)
		गच्छ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	प्रति (wp = tab_firstwin; wp != रिक्त; wp = W_NEXT(wp))
	{
	    जइ (ses_do_win(wp))
		++nr;
	    अण्णहा
		restore_size = FALSE;
	    जइ (curwin == wp)
		cnr = nr;
	}

	जइ (tab_firstwin->w_next != रिक्त)
	{
	    // Go to the first window.
	    जइ (put_पङ्क्तिः(fd, "wincmd t") == FAIL)
		गच्छ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    जइ (!restore_height_width
		    && (put_पङ्क्तिः(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_पङ्क्तिः(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		गच्छ fail;
	    जइ (put_पङ्क्तिः(fd, "set winminheight=0") == FAIL
		    || put_पङ्क्तिः(fd, "set winheight=1") == FAIL
		    || put_पङ्क्तिः(fd, "set winminwidth=0") == FAIL
		    || put_पङ्क्तिः(fd, "set winwidth=1") == FAIL)
		गच्छ fail;
	    restore_height_width = TRUE;
	}
	जइ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    गच्छ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	जइ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != रिक्त)
	{
	    जइ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		गच्छ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	प्रति (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जइ (!ses_do_win(wp))
		अनुवर्त;
	    जइ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# अण्णहा
							 रिक्त
# endif
		 ) == FAIL)
		गच्छ fail;
	    जइ (nr > 1 && put_पङ्क्तिः(fd, "wincmd w") == FAIL)
		गच्छ fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	जइ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    गच्छ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	जइ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    गच्छ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	जइ (!(ssop_flags & SSOP_TABPAGES))
	    भङ्ग;
    }

    जइ (ssop_flags & SSOP_TABPAGES)
    {
	जइ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    गच्छ fail;
    }
    जइ (restore_stal && put_पङ्क्तिः(fd, "set stal=1") == FAIL)
	गच्छ fail;

    // Wipe out an empty unnamed buffer we started in.
    जइ (put_पङ्क्तिः(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	गच्छ fail;
    जइ (put_पङ्क्तिः(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	गच्छ fail;
    जइ (put_पङ्क्तिः(fd, "endif") == FAIL)
	गच्छ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    जइ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	गच्छ fail;

    // Restore 'shortmess'.
    जइ (ssop_flags & SSOP_OPTIONS)
    {
	जइ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    गच्छ fail;
    }
    अण्णहा
    {
	जइ (put_पङ्क्तिः(fd, "&shortmess = shortmess_save") == FAIL)
	    गच्छ fail;
    }

    जइ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	जइ (put_पङ्क्तिः(fd, "&winminheight = save_winminheight") == FAIL
	      || put_पङ्क्तिः(fd, "&winminwidth = save_winminwidth") == FAIL)
	    गच्छ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    जइ (put_पङ्क्तिः(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_पङ्क्तिः(fd, "if filereadable(sx)") == FAIL
	    || put_पङ्क्तिः(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_पङ्क्तिः(fd, "endif") == FAIL)
	गच्छ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    प्रत्यागम ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    स्थिर char_u *
get_view_file(पूर्णाङ्क c)
{
    पूर्णाङ्क		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    जइ (curbuf->b_ffname == रिक्त)
    {
	emsg(_(e_no_file_name));
	प्रत्यागम रिक्त;
    }
    sname = home_replace_save(रिक्त, curbuf->b_ffname);
    जइ (sname == रिक्त)
	प्रत्यागम रिक्त;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    प्रति (p = sname; *p; ++p)
	जइ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    जइ (retval != रिक्त)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	प्रति (p = sname; *p; ++p)
	{
	    जइ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    अण्णहा जइ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# जइ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		जइ (*p == ':')
		    *s++ = '-';
		अण्णहा
# endif
		    *s++ = '+';
	    }
	    अण्णहा
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    प्रत्यागम retval;
}

/*
 * ":loadview [nr]"
 */
    शून्य
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    जइ (fname == रिक्त)
	प्रत्यागम;

    (शून्य)do_source(fname, FALSE, DOSO_NONE, रिक्त);
    vim_free(fname);
}

# जइ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    पूर्णाङ्क
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    वर्ण	    *mksession_cmdline;
    unsigned पूर्णाङ्क    save_ssop_flags;
    पूर्णाङ्क		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    जइ (escaped_filename == रिक्त)
	प्रत्यागम FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    जइ (mksession_cmdline == रिक्त)
    {
	vim_free(escaped_filename);
	प्रत्यागम FALSE;
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
    जइ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == रिक्त
	       || put_पङ्क्तिः(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_पङ्क्तिः(fd, "unlet g:Save_VV_this_session") == FAIL);

	जइ (fd != रिक्त && fclose(fd) != 0)
	    failed = TRUE;

	जइ (failed)
	    mch_remove(filename);
    }

    प्रत्यागम !failed;
}
# endif

#endif // FEAT_SESSION

#जइ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
स्थिर पूर्णाङ्क mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    शून्य
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    पूर्णाङ्क		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = रिक्त;
#endif
#ifdef FEAT_SESSION
    पूर्णाङ्क		view_session = FALSE;
    पूर्णाङ्क		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = रिक्त;
    unsigned	*flagp;
#endif
#जइ defined(FEAT_EVAL)
    पूर्णाङ्क		sid;
    scriptitem_T *si = रिक्त;
#endif

    जइ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#अण्णहा
	ex_ni(eap);
	प्रत्यागम;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    जइ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	जइ (fname == रिक्त)
	    प्रत्यागम;
	viewFile = fname;
	using_vdir = TRUE;
    }
    अण्णहा
#endif
	जइ (*eap->arg != NUL)
	fname = eap->arg;
    अण्णहा जइ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    अण्णहा जइ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    अण्णहा
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    जइ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", रिक्त,
		(char_u *)_(BROWSE_FILTER_MACROS), रिक्त);
	जइ (browseFile == रिक्त)
	    गच्छ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#जइ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    जइ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    जइ (fd != रिक्त)
    {
#ifdef FEAT_SESSION
	जइ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	अण्णहा
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	जइ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	जइ (put_पङ्क्तिः(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	जइ (eap->cmdidx == CMD_mkvimrc)
	    (शून्य)put_पङ्क्तिः(fd, "version 6.0");

#ifdef FEAT_SESSION
	जइ (eap->cmdidx == CMD_mksession)
	{
	    जइ (put_पङ्क्तिः(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	जइ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    जइ (p_cp)
		(शून्य)put_पङ्क्तिः(fd, "if !&cp | set cp | endif");
	    अण्णहा
		(शून्य)put_पङ्क्तिः(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	जइ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    पूर्णाङ्क flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_पङ्क्तिः(fd, "var cpo_save: string") == FAIL;

	    जइ (eap->cmdidx == CMD_mksession)
	    {
		जइ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    जइ (do_mappings)
		failed |= (makemap(fd, रिक्त) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#जइ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    प्रति (sid = 1; sid <= script_वस्तूनि.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		जइ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    पूर्णाङ्क b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    प्रति (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			जइ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    अनुवर्त;

			// compare prefixes if available
			जइ (si->sn_autoload_prefix != रिक्त && b_si->sn_autoload_prefix != रिक्त
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    भङ्ग;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			जइ (STRCMP(name, b_name) == 0)
			    भङ्ग;
		    }

		    // import the auto script if there are no conflicts
		    जइ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	जइ (!failed && view_session)
	{
	    जइ (put_पङ्क्तिः(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    जइ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		जइ (dirnow == रिक्त)
		    failed = TRUE;
		अण्णहा
		{
		    // Change to session file's dir.
		    जइ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((वर्ण *)dirnow) != 0)
			*dirnow = NUL;
		    जइ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			जइ (vim_chdirfile(fname, रिक्त) == OK)
			    shorten_fnames(TRUE);
		    }
		    अण्णहा जइ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != रिक्त)
		    {
			जइ (mch_chdir((वर्ण *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    जइ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != रिक्त)))
		    {
			जइ (mch_chdir((वर्ण *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    अण्णहा
	    {
		failed |= put_पङ्क्तिः(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								रिक्त) == FAIL);
	    }
	    जइ (put_पङ्क्तिः(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_अन्वेषणम्_EXTRA
	    जइ (no_hlsearch && put_पङ्क्तिः(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    जइ (put_पङ्क्तिः(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    जइ (eap->cmdidx == CMD_mksession)
	    {
		जइ (put_पङ्क्तिः(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	जइ (put_पङ्क्तिः(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	जइ (failed)
	    emsg(_(e_error_while_writing));
#जइ defined(FEAT_SESSION)
	अण्णहा जइ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    जइ (tbuf != रिक्त)
	    {
		जइ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, रिक्त, रिक्त, FALSE, curbuf);
}

#जइ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    जइ (ASCII_ISUPPER(*p))
    {
	जाव (*(++p))
	    जइ (ASCII_ISLOWER(*p))
		प्रत्यागम VAR_FLAVOUR_SESSION;
	प्रत्यागम VAR_FLAVOUR_VIMINFO;
    }
    अण्णहा
	प्रत्यागम VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    पूर्णाङ्क
put_eol(FILE *fd)
{
    जइ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    पूर्णाङ्क
put_पङ्क्तिः(FILE *fd, वर्ण *s)
{
    जइ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}
