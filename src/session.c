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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_SESSION)

স্থির পূর্ণাঙ্ক did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    স্থির পূর্ণাঙ্ক
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    পূর্ণাঙ্ক		retval = OK;

    sname = home_replace_save(রিক্ত, name);
    যদি (sname == রিক্ত)
	ঘূৰাই FAIL;

    যদি (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	বাবে (p = sname; *p != NUL; MB_PTR_ADV(p))
	    যদি (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    যদি (p == রিক্ত)
	ঘূৰাই FAIL;

    // write the result
    যদি (fputs((ৱর্ণ *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ঘূৰাই retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    স্থির পূর্ণাঙ্ক
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, পূর্ণাঙ্ক add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    যদি (buf->b_sfname != রিক্ত
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    অন্যথা
	name = buf->b_ffname;
    যদি (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    স্থির পূর্ণাঙ্ক
ses_arglist(
    FILE	*fd,
    ৱর্ণ	*cmd,
    garray_T	*gap,
    পূর্ণাঙ্ক		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    পূর্ণাঙ্ক		i;
    char_u	*buf = রিক্ত;
    char_u	*s;

    যদি (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ঘূৰাই FAIL;
    যদি (put_সাৰি(fd, ":%argdel") == FAIL)
	ঘূৰাই FAIL;
    বাবে (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	যদি (s != রিক্ত)
	{
	    যদি (fullname)
	    {
		buf = alloc(MAXPATHL);
		যদি (buf != রিক্ত)
		{
		    (শূন্য)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    যদি (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ঘূৰাই FAIL;
	    }
	    vim_free(buf);
	}
    }
    ঘূৰাই OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    স্থির পূর্ণাঙ্ক
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    যদি (bt_terminal(wp->w_buffer))
	ঘূৰাই !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    যদি (wp->w_buffer->b_fname == রিক্ত
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ঘূৰাই (ssop_flags & SSOP_BLANK);
    যদি (bt_help(wp->w_buffer))
	ঘূৰাই (ssop_flags & SSOP_HELP);
    ঘূৰাই TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    স্থির পূর্ণাঙ্ক
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    যদি (fr->fr_layout == FR_LEAF)
	ঘূৰাই ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	যদি (ses_do_frame(frc))
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    স্থির frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	যদি (ses_do_frame(frc))
	    ভঙ্গ;
    ঘূৰাই frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    স্থির পূর্ণাঙ্ক
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    পূর্ণাঙ্ক		গুনতি = 0;

    যদি (fr->fr_layout == FR_LEAF)
	ঘূৰাই OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    যদি (frc != রিক্ত)
	যেতিয়ালৈ ((frc = ses_skipframe(frc->fr_next)) != রিক্ত)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    যদি (put_সাৰি(fd, "wincmd _ | wincmd |") == FAIL
		    || put_সাৰি(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ঘূৰাই FAIL;
	    ++গুনতি;
	}

    // Go back to the first window.
    যদি (গুনতি > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", গুনতি) < 0
		|| put_eol(fd) == FAIL))
	ঘূৰাই FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    যেতিয়ালৈ (frc != রিক্ত)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	যদি (frc != রিক্ত && put_সাৰি(fd, "wincmd w") == FAIL)
	    ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

    স্থির পূর্ণাঙ্ক
ses_winsizes(
    FILE	*fd,
    পূর্ণাঙ্ক		restore_size,
    win_T	*tab_firstwin)
{
    পূর্ণাঙ্ক		n = 0;
    win_T	*wp;

    যদি (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	বাবে (wp = tab_firstwin; wp != রিক্ত; wp = wp->w_next)
	{
	    যদি (!ses_do_win(wp))
		অনুৱর্ত;
	    ++n;

	    // restore height when not full height
	    যদি (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ঘূৰাই FAIL;

	    // restore width when not full width
	    যদি (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ঘূৰাই FAIL;
	}
    }
    অন্যথা
    {
	// Just equalise window sizes
	যদি (put_সাৰি(fd, "wincmd =") == FAIL)
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

    স্থির পূর্ণাঙ্ক
put_view_curpos(FILE *fd, win_T *wp, ৱর্ণ *spaces)
{
    পূর্ণাঙ্ক r;

    যদি (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    অন্যথা
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ঘূৰাই r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    স্থির পূর্ণাঙ্ক
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    পূর্ণাঙ্ক		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    পূর্ণাঙ্ক		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    পূর্ণাঙ্ক		f;
    পূর্ণাঙ্ক		do_cursor;
    পূর্ণাঙ্ক		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    যদি (wp->w_alist == &global_alist)
    {
	যদি (put_সাৰি(fd, "argglobal") == FAIL)
	    ঘূৰাই FAIL;
    }
    অন্যথা
    {
	যদি (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != রিক্ত
			|| wp->w_localdir != রিক্ত, flagp) == FAIL)
	    ঘূৰাই FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    যদি (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	যদি (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ঘূৰাই FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    যদি (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	যদি (bt_help(wp->w_buffer))
	{
	    ৱর্ণ *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    যদি (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (ৱর্ণ *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    যদি (put_সাৰি(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ঘূৰাই FAIL;
	}
# ifdef FEAT_TERMINAL
	অন্যথা যদি (bt_terminal(wp->w_buffer))
	{
	    যদি (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ঘূৰাই FAIL;
	}
# endif
	// Load the file.
	অন্যথা যদি (wp->w_buffer->b_ffname != রিক্ত
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    যদি (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা
	{
	    // No file in this buffer, just make it empty.
	    যদি (put_সাৰি(fd, "enew") == FAIL)
		ঘূৰাই FAIL;
# ifdef FEAT_QUICKFIX
	    যদি (wp->w_buffer->b_ffname != রিক্ত)
	    {
		// The buffer does have a name, but it's not a file name.
		যদি (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ঘূৰাই FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    যদি (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	যদি ((flagp == &ssop_flags)
		&& alt != রিক্ত
		&& alt->b_fname != রিক্ত
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ঘূৰাই FAIL;
    }

    // Local mappings and abbreviations.
    যদি ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ঘূৰাই FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    যদি (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    অন্যথা যদি (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    অন্যথা
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    যদি (f == FAIL)
	ঘূৰাই FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    যদি ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != রিক্ত
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	যদি (put_folds(fd, wp) == FAIL)
	    ঘূৰাই FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    যদি (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	যদি (put_সাৰি(fd, "{") == FAIL)
	    ঘূৰাই FAIL;

	যদি (wp->w_height <= 0)
	{
	    যদি (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ঘূৰাই FAIL;

	যদি (put_eol(fd) == FAIL
		|| put_সাৰি(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_সাৰি(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_সাৰি(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ঘূৰাই FAIL;
	// Restore the cursor column and left offset when not wrapping.
	যদি (wp->w_cursor.col == 0)
	{
	    যদি (put_সাৰি(fd, "  normal! 0") == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা
	{
	    যদি (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		যদি (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_সাৰি(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_সাৰি(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_সাৰি(fd, "  endif") == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা যদি (put_view_curpos(fd, wp, "  ") == FAIL)
		ঘূৰাই FAIL;
	}

	যদি (put_সাৰি(fd, "}") == FAIL)
	    ঘূৰাই FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    যদি (wp->w_localdir != রিক্ত
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	যদি (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ঘূৰাই FAIL;
	did_lcd = TRUE;
    }

    ঘূৰাই OK;
}

# ifdef FEAT_EVAL
    স্থির পূর্ণাঙ্ক
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    পূর্ণাঙ্ক		todo;
    char_u	*p, *t;

    todo = (পূর্ণাঙ্ক)gvht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(gvht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    যদি ((this_var->di_tv.v_type == VAR_সংখ্যা
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_চাবি) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		যদি (p == রিক্ত)	    // out of memory
		    ভঙ্গ;
		বাবে (t = p; *t != NUL; ++t)
		    যদি (*t == '\n')
			*t = 'n';
		    অন্যথা যদি (*t == '\r')
			*t = 'r';
		যদি ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_চাবি,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ঘূৰাই FAIL;
		}
		vim_free(p);
	    }
	    অন্যথা যদি (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_চাবি) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		পূর্ণাঙ্ক sign = ' ';

		যদি (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		যদি ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_চাবি, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ঘূৰাই FAIL;
	    }
	}
    }
    ঘূৰাই OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    স্থির পূর্ণাঙ্ক
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    পূর্ণাঙ্ক		only_save_windows = TRUE;
    পূর্ণাঙ্ক		nr;
    পূর্ণাঙ্ক		restore_size = TRUE;
    পূর্ণাঙ্ক		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = রিক্ত;
    পূর্ণাঙ্ক		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    পূর্ণাঙ্ক		cur_arg_idx = 0;
    পূর্ণাঙ্ক		next_arg_idx = 0;
    পূর্ণাঙ্ক		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    যদি (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    যদি (put_সাৰি(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	গচ্ছ fail;

    যদি (put_সাৰি(fd, "doautoall SessionLoadPre") == FAIL)
	গচ্ছ fail;

    যদি (put_সাৰি(fd, "var save_splitbelow: bool") == FAIL
	    || put_সাৰি(fd, "var save_splitright: bool") == FAIL
	    || put_সাৰি(fd, "var save_winminheight: number") == FAIL
	    || put_সাৰি(fd, "var save_winminwidth: number") == FAIL
	    || put_সাৰি(fd, "var wipebuf: number = -1") == FAIL
	    || put_সাৰি(fd, "var shortmess_save: string") == FAIL)
	গচ্ছ fail;

    যদি (ssop_flags & SSOP_GLOBALS)
	যদি (store_session_globals(fd) == FAIL)
	    গচ্ছ fail;
# endif

    // Close all windows and tabs but one.
    যদি (put_সাৰি(fd, "silent only") == FAIL)
	গচ্ছ fail;
    যদি ((ssop_flags & SSOP_TABPAGES)
	    && put_সাৰি(fd, "silent tabonly") == FAIL)
	গচ্ছ fail;

    // Now a :cd command to the session directory or the current directory
    যদি (ssop_flags & SSOP_SESDIR)
    {
	যদি (put_সাৰি(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    গচ্ছ fail;
    }
    অন্যথা যদি (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(রিক্ত, globaldir != রিক্ত ? globaldir : dirnow);
	যদি (sname == রিক্ত
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    গচ্ছ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    যদি (put_সাৰি(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	গচ্ছ fail;
    যদি (put_সাৰি(fd, "  wipebuf = bufnr('%')") == FAIL)
	গচ্ছ fail;
    যদি (put_সাৰি(fd, "endif") == FAIL)
	গচ্ছ fail;

    // Save 'shortmess' if not storing options.
    যদি ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_সাৰি(fd, "shortmess_save = &shortmess") == FAIL)
	গচ্ছ fail;

    // Set 'shortmess' for the following.
    যদি (put_সাৰি(fd, "set shortmess+=aoO") == FAIL)
	গচ্ছ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	যদি (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != রিক্ত
		&& buf->b_p_bl)
	{
	    যদি (fprintf(fd, "badd +%ld ", buf->b_wininfo == রিক্ত ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		গচ্ছ fail;
	}
    }

    // the global argument list
    যদি (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	গচ্ছ fail;

    যদি (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	যদি (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    গচ্ছ fail;
    }

# ifdef FEAT_GUI
    যদি (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	পূর্ণাঙ্ক	x, y;

	যদি (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    যদি (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		গচ্ছ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    যদি (p_stal == 1 && first_tabpage->tp_next != রিক্ত)
    {
	যদি (put_সাৰি(fd, "set stal=2") == FAIL)
	    গচ্ছ fail;
	restore_stal = TRUE;
    }

    যদি ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    যদি (tp->tp_next != রিক্ত
		  && put_সাৰি(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		গচ্ছ fail;
	যদি (first_tabpage->tp_next != রিক্ত && put_সাৰি(fd, "tabrewind") == FAIL)
	    গচ্ছ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	পূর্ণাঙ্ক	need_tabnext = FALSE;
	পূর্ণাঙ্ক	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	যদি ((ssop_flags & SSOP_TABPAGES))
	{
	    যদি (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    অন্যথা
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    যদি (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	অন্যথা
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	বাবে (wp = tab_firstwin; wp != রিক্ত; wp = wp->w_next)
	{
	    যদি (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != রিক্ত
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		যদি (need_tabnext && put_সাৰি(fd, "tabnext") == FAIL)
		    গচ্ছ fail;
		need_tabnext = FALSE;

		যদি (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    গচ্ছ fail;
		যদি (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ভঙ্গ;
	    }
	}

	// If no file got edited create an empty tab page.
	যদি (need_tabnext && put_সাৰি(fd, "tabnext") == FAIL)
	    গচ্ছ fail;

	যদি (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    যদি (put_সাৰি(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_সাৰি(fd, "save_splitright = &splitright")
								       == FAIL)
		গচ্ছ fail;
	    যদি (put_সাৰি(fd, "set splitbelow splitright") == FAIL)
		গচ্ছ fail;
	    যদি (ses_win_rec(fd, tab_topframe) == FAIL)
		গচ্ছ fail;
	    যদি (put_সাৰি(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_সাৰি(fd, "&splitright = save_splitright")
								       == FAIL)
		গচ্ছ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	বাবে (wp = tab_firstwin; wp != রিক্ত; wp = W_NEXT(wp))
	{
	    যদি (ses_do_win(wp))
		++nr;
	    অন্যথা
		restore_size = FALSE;
	    যদি (curwin == wp)
		cnr = nr;
	}

	যদি (tab_firstwin->w_next != রিক্ত)
	{
	    // Go to the first window.
	    যদি (put_সাৰি(fd, "wincmd t") == FAIL)
		গচ্ছ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    যদি (!restore_height_width
		    && (put_সাৰি(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_সাৰি(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		গচ্ছ fail;
	    যদি (put_সাৰি(fd, "set winminheight=0") == FAIL
		    || put_সাৰি(fd, "set winheight=1") == FAIL
		    || put_সাৰি(fd, "set winminwidth=0") == FAIL
		    || put_সাৰি(fd, "set winwidth=1") == FAIL)
		গচ্ছ fail;
	    restore_height_width = TRUE;
	}
	যদি (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    গচ্ছ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	যদি ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != রিক্ত)
	{
	    যদি (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		গচ্ছ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	বাবে (wp = tab_firstwin; wp != রিক্ত; wp = wp->w_next)
	{
	    যদি (!ses_do_win(wp))
		অনুৱর্ত;
	    যদি (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# অন্যথা
							 রিক্ত
# endif
		 ) == FAIL)
		গচ্ছ fail;
	    যদি (nr > 1 && put_সাৰি(fd, "wincmd w") == FAIL)
		গচ্ছ fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	যদি (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    গচ্ছ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	যদি (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    গচ্ছ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	যদি (!(ssop_flags & SSOP_TABPAGES))
	    ভঙ্গ;
    }

    যদি (ssop_flags & SSOP_TABPAGES)
    {
	যদি (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    গচ্ছ fail;
    }
    যদি (restore_stal && put_সাৰি(fd, "set stal=1") == FAIL)
	গচ্ছ fail;

    // Wipe out an empty unnamed buffer we started in.
    যদি (put_সাৰি(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	গচ্ছ fail;
    যদি (put_সাৰি(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	গচ্ছ fail;
    যদি (put_সাৰি(fd, "endif") == FAIL)
	গচ্ছ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    যদি (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	গচ্ছ fail;

    // Restore 'shortmess'.
    যদি (ssop_flags & SSOP_OPTIONS)
    {
	যদি (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    গচ্ছ fail;
    }
    অন্যথা
    {
	যদি (put_সাৰি(fd, "&shortmess = shortmess_save") == FAIL)
	    গচ্ছ fail;
    }

    যদি (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	যদি (put_সাৰি(fd, "&winminheight = save_winminheight") == FAIL
	      || put_সাৰি(fd, "&winminwidth = save_winminwidth") == FAIL)
	    গচ্ছ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    যদি (put_সাৰি(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_সাৰি(fd, "if filereadable(sx)") == FAIL
	    || put_সাৰি(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_সাৰি(fd, "endif") == FAIL)
	গচ্ছ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ঘূৰাই ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    স্থির char_u *
get_view_file(পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    যদি (curbuf->b_ffname == রিক্ত)
    {
	emsg(_(e_no_file_name));
	ঘূৰাই রিক্ত;
    }
    sname = home_replace_save(রিক্ত, curbuf->b_ffname);
    যদি (sname == রিক্ত)
	ঘূৰাই রিক্ত;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    বাবে (p = sname; *p; ++p)
	যদি (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    যদি (retval != রিক্ত)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	বাবে (p = sname; *p; ++p)
	{
	    যদি (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    অন্যথা যদি (vim_ispathsep(*p))
	    {
		*s++ = '=';
# যদি defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		যদি (*p == ':')
		    *s++ = '-';
		অন্যথা
# endif
		    *s++ = '+';
	    }
	    অন্যথা
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ঘূৰাই retval;
}

/*
 * ":loadview [nr]"
 */
    শূন্য
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    যদি (fname == রিক্ত)
	ঘূৰাই;

    (শূন্য)do_source(fname, FALSE, DOSO_NONE, রিক্ত);
    vim_free(fname);
}

# যদি defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    পূর্ণাঙ্ক
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    ৱর্ণ	    *mksession_cmdline;
    unsigned পূর্ণাঙ্ক    save_ssop_flags;
    পূর্ণাঙ্ক		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    যদি (escaped_filename == রিক্ত)
	ঘূৰাই FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    যদি (mksession_cmdline == রিক্ত)
    {
	vim_free(escaped_filename);
	ঘূৰাই FALSE;
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
    যদি (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == রিক্ত
	       || put_সাৰি(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_সাৰি(fd, "unlet g:Save_VV_this_session") == FAIL);

	যদি (fd != রিক্ত && fclose(fd) != 0)
	    failed = TRUE;

	যদি (failed)
	    mch_remove(filename);
    }

    ঘূৰাই !failed;
}
# endif

#endif // FEAT_SESSION

#যদি defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
স্থির পূর্ণাঙ্ক mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    শূন্য
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    পূর্ণাঙ্ক		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = রিক্ত;
#endif
#ifdef FEAT_SESSION
    পূর্ণাঙ্ক		view_session = FALSE;
    পূর্ণাঙ্ক		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = রিক্ত;
    unsigned	*flagp;
#endif
#যদি defined(FEAT_EVAL)
    পূর্ণাঙ্ক		sid;
    scriptitem_T *si = রিক্ত;
#endif

    যদি (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#অন্যথা
	ex_ni(eap);
	ঘূৰাই;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    যদি (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	যদি (fname == রিক্ত)
	    ঘূৰাই;
	viewFile = fname;
	using_vdir = TRUE;
    }
    অন্যথা
#endif
	যদি (*eap->arg != NUL)
	fname = eap->arg;
    অন্যথা যদি (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    অন্যথা যদি (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    অন্যথা
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", রিক্ত,
		(char_u *)_(BROWSE_FILTER_MACROS), রিক্ত);
	যদি (browseFile == রিক্ত)
	    গচ্ছ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#যদি defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    যদি (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    যদি (fd != রিক্ত)
    {
#ifdef FEAT_SESSION
	যদি (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	অন্যথা
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	যদি (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	যদি (put_সাৰি(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	যদি (eap->cmdidx == CMD_mkvimrc)
	    (শূন্য)put_সাৰি(fd, "version 6.0");

#ifdef FEAT_SESSION
	যদি (eap->cmdidx == CMD_mksession)
	{
	    যদি (put_সাৰি(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	যদি (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    যদি (p_cp)
		(শূন্য)put_সাৰি(fd, "if !&cp | set cp | endif");
	    অন্যথা
		(শূন্য)put_সাৰি(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	যদি (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    পূর্ণাঙ্ক flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_সাৰি(fd, "var cpo_save: string") == FAIL;

	    যদি (eap->cmdidx == CMD_mksession)
	    {
		যদি (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    যদি (do_mappings)
		failed |= (makemap(fd, রিক্ত) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#যদি defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    বাবে (sid = 1; sid <= script_বস্তুসমূহ.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		যদি ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    পূর্ণাঙ্ক b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    বাবে (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			যদি ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    অনুৱর্ত;

			// compare prefixes if available
			যদি (si->sn_autoload_prefix != রিক্ত && b_si->sn_autoload_prefix != রিক্ত
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    ভঙ্গ;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			যদি (STRCMP(name, b_name) == 0)
			    ভঙ্গ;
		    }

		    // import the auto script if there are no conflicts
		    যদি (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	যদি (!failed && view_session)
	{
	    যদি (put_সাৰি(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    যদি (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		যদি (dirnow == রিক্ত)
		    failed = TRUE;
		অন্যথা
		{
		    // Change to session file's dir.
		    যদি (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((ৱর্ণ *)dirnow) != 0)
			*dirnow = NUL;
		    যদি (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			যদি (vim_chdirfile(fname, রিক্ত) == OK)
			    shorten_fnames(TRUE);
		    }
		    অন্যথা যদি (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != রিক্ত)
		    {
			যদি (mch_chdir((ৱর্ণ *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    যদি (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != রিক্ত)))
		    {
			যদি (mch_chdir((ৱর্ণ *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    অন্যথা
	    {
		failed |= put_সাৰি(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								রিক্ত) == FAIL);
	    }
	    যদি (put_সাৰি(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_অনুসন্ধান_EXTRA
	    যদি (no_hlsearch && put_সাৰি(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    যদি (put_সাৰি(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    যদি (eap->cmdidx == CMD_mksession)
	    {
		যদি (put_সাৰি(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	যদি (put_সাৰি(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	যদি (failed)
	    emsg(_(e_error_while_writing));
#যদি defined(FEAT_SESSION)
	অন্যথা যদি (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    যদি (tbuf != রিক্ত)
	    {
		যদি (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, রিক্ত, রিক্ত, FALSE, curbuf);
}

#যদি defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    যদি (ASCII_ISUPPER(*p))
    {
	যেতিয়ালৈ (*(++p))
	    যদি (ASCII_ISLOWER(*p))
		ঘূৰাই VAR_FLAVOUR_SESSION;
	ঘূৰাই VAR_FLAVOUR_VIMINFO;
    }
    অন্যথা
	ঘূৰাই VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    পূর্ণাঙ্ক
put_eol(FILE *fd)
{
    যদি (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    পূর্ণাঙ্ক
put_সাৰি(FILE *fd, ৱর্ণ *s)
{
    যদি (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}
