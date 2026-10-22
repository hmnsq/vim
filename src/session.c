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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SESSION)

நிலையான முழு did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    நிலையான முழு
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    முழு		retval = OK;

    sname = home_replace_save(வெற்று, name);
    எனில் (sname == வெற்று)
	திரும்பு FAIL;

    எனில் (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	க்கு (p = sname; *p != NUL; MB_PTR_ADV(p))
	    எனில் (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    எனில் (p == வெற்று)
	திரும்பு FAIL;

    // write the result
    எனில் (fputs((எழுத்து *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    திரும்பு retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    நிலையான முழு
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, முழு add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    எனில் (buf->b_sfname != வெற்று
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    இல்லையெனில்
	name = buf->b_ffname;
    எனில் (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    நிலையான முழு
ses_arglist(
    FILE	*fd,
    எழுத்து	*cmd,
    garray_T	*gap,
    முழு		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    முழு		i;
    char_u	*buf = வெற்று;
    char_u	*s;

    எனில் (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	திரும்பு FAIL;
    எனில் (put_வரி(fd, ":%argdel") == FAIL)
	திரும்பு FAIL;
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	எனில் (s != வெற்று)
	{
	    எனில் (fullname)
	    {
		buf = alloc(MAXPATHL);
		எனில் (buf != வெற்று)
		{
		    (வெறுமை)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    எனில் (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		திரும்பு FAIL;
	    }
	    vim_free(buf);
	}
    }
    திரும்பு OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    நிலையான முழு
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    எனில் (bt_terminal(wp->w_buffer))
	திரும்பு !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    எனில் (wp->w_buffer->b_fname == வெற்று
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	திரும்பு (ssop_flags & SSOP_BLANK);
    எனில் (bt_help(wp->w_buffer))
	திரும்பு (ssop_flags & SSOP_HELP);
    திரும்பு TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    நிலையான முழு
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    எனில் (fr->fr_layout == FR_LEAF)
	திரும்பு ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	எனில் (ses_do_frame(frc))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    நிலையான frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	எனில் (ses_do_frame(frc))
	    நிறுத்து;
    திரும்பு frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    நிலையான முழு
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    முழு		மொத்தம் = 0;

    எனில் (fr->fr_layout == FR_LEAF)
	திரும்பு OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    எனில் (frc != வெற்று)
	போது ((frc = ses_skipframe(frc->fr_next)) != வெற்று)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    எனில் (put_வரி(fd, "wincmd _ | wincmd |") == FAIL
		    || put_வரி(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		திரும்பு FAIL;
	    ++மொத்தம்;
	}

    // Go back to the first window.
    எனில் (மொத்தம் > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", மொத்தம்) < 0
		|| put_eol(fd) == FAIL))
	திரும்பு FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    போது (frc != வெற்று)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	எனில் (frc != வெற்று && put_வரி(fd, "wincmd w") == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

    நிலையான முழு
ses_winsizes(
    FILE	*fd,
    முழு		restore_size,
    win_T	*tab_firstwin)
{
    முழு		n = 0;
    win_T	*wp;

    எனில் (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	க்கு (wp = tab_firstwin; wp != வெற்று; wp = wp->w_next)
	{
	    எனில் (!ses_do_win(wp))
		தொடர்;
	    ++n;

	    // restore height when not full height
	    எனில் (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		திரும்பு FAIL;

	    // restore width when not full width
	    எனில் (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		திரும்பு FAIL;
	}
    }
    இல்லையெனில்
    {
	// Just equalise window sizes
	எனில் (put_வரி(fd, "wincmd =") == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

    நிலையான முழு
put_view_curpos(FILE *fd, win_T *wp, எழுத்து *spaces)
{
    முழு r;

    எனில் (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    இல்லையெனில்
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    திரும்பு r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    நிலையான முழு
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    முழு		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    முழு		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    முழு		f;
    முழு		do_cursor;
    முழு		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    எனில் (wp->w_alist == &global_alist)
    {
	எனில் (put_வரி(fd, "argglobal") == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	எனில் (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != வெற்று
			|| wp->w_localdir != வெற்று, flagp) == FAIL)
	    திரும்பு FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    எனில் (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	எனில் (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    திரும்பு FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    எனில் (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	எனில் (bt_help(wp->w_buffer))
	{
	    எழுத்து *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    எனில் (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (எழுத்து *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    எனில் (put_வரி(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		திரும்பு FAIL;
	}
# ifdef FEAT_TERMINAL
	இல்லையெனில் எனில் (bt_terminal(wp->w_buffer))
	{
	    எனில் (term_write_session(fd, wp, terminal_bufs) == FAIL)
		திரும்பு FAIL;
	}
# endif
	// Load the file.
	இல்லையெனில் எனில் (wp->w_buffer->b_ffname != வெற்று
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    எனில் (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    // No file in this buffer, just make it empty.
	    எனில் (put_வரி(fd, "enew") == FAIL)
		திரும்பு FAIL;
# ifdef FEAT_QUICKFIX
	    எனில் (wp->w_buffer->b_ffname != வெற்று)
	    {
		// The buffer does have a name, but it's not a file name.
		எனில் (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    திரும்பு FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    எனில் (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	எனில் ((flagp == &ssop_flags)
		&& alt != வெற்று
		&& alt->b_fname != வெற்று
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    திரும்பு FAIL;
    }

    // Local mappings and abbreviations.
    எனில் ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	திரும்பு FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    எனில் (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    இல்லையெனில் எனில் (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    இல்லையெனில்
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    எனில் (f == FAIL)
	திரும்பு FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    எனில் ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != வெற்று
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	எனில் (put_folds(fd, wp) == FAIL)
	    திரும்பு FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    எனில் (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	எனில் (put_வரி(fd, "{") == FAIL)
	    திரும்பு FAIL;

	எனில் (wp->w_height <= 0)
	{
	    எனில் (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    திரும்பு FAIL;

	எனில் (put_eol(fd) == FAIL
		|| put_வரி(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_வரி(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_வரி(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    திரும்பு FAIL;
	// Restore the cursor column and left offset when not wrapping.
	எனில் (wp->w_cursor.col == 0)
	{
	    எனில் (put_வரி(fd, "  normal! 0") == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    எனில் (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		எனில் (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_வரி(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_வரி(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_வரி(fd, "  endif") == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (put_view_curpos(fd, wp, "  ") == FAIL)
		திரும்பு FAIL;
	}

	எனில் (put_வரி(fd, "}") == FAIL)
	    திரும்பு FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    எனில் (wp->w_localdir != வெற்று
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	எனில் (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    திரும்பு FAIL;
	did_lcd = TRUE;
    }

    திரும்பு OK;
}

# ifdef FEAT_EVAL
    நிலையான முழு
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    முழு		todo;
    char_u	*p, *t;

    todo = (முழு)gvht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(gvht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    எனில் ((this_var->di_tv.v_type == VAR_எண்
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_சாவி) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		எனில் (p == வெற்று)	    // out of memory
		    நிறுத்து;
		க்கு (t = p; *t != NUL; ++t)
		    எனில் (*t == '\n')
			*t = 'n';
		    இல்லையெனில் எனில் (*t == '\r')
			*t = 'r';
		எனில் ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_சாவி,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    திரும்பு FAIL;
		}
		vim_free(p);
	    }
	    இல்லையெனில் எனில் (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_சாவி) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		முழு sign = ' ';

		எனில் (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		எனில் ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_சாவி, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    திரும்பு FAIL;
	    }
	}
    }
    திரும்பு OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    நிலையான முழு
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    முழு		only_save_windows = TRUE;
    முழு		nr;
    முழு		restore_size = TRUE;
    முழு		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = வெற்று;
    முழு		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    முழு		cur_arg_idx = 0;
    முழு		next_arg_idx = 0;
    முழு		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    எனில் (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    எனில் (put_வரி(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	செல் fail;

    எனில் (put_வரி(fd, "doautoall SessionLoadPre") == FAIL)
	செல் fail;

    எனில் (put_வரி(fd, "var save_splitbelow: bool") == FAIL
	    || put_வரி(fd, "var save_splitright: bool") == FAIL
	    || put_வரி(fd, "var save_winminheight: number") == FAIL
	    || put_வரி(fd, "var save_winminwidth: number") == FAIL
	    || put_வரி(fd, "var wipebuf: number = -1") == FAIL
	    || put_வரி(fd, "var shortmess_save: string") == FAIL)
	செல் fail;

    எனில் (ssop_flags & SSOP_GLOBALS)
	எனில் (store_session_globals(fd) == FAIL)
	    செல் fail;
# endif

    // Close all windows and tabs but one.
    எனில் (put_வரி(fd, "silent only") == FAIL)
	செல் fail;
    எனில் ((ssop_flags & SSOP_TABPAGES)
	    && put_வரி(fd, "silent tabonly") == FAIL)
	செல் fail;

    // Now a :cd command to the session directory or the current directory
    எனில் (ssop_flags & SSOP_SESDIR)
    {
	எனில் (put_வரி(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    செல் fail;
    }
    இல்லையெனில் எனில் (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(வெற்று, globaldir != வெற்று ? globaldir : dirnow);
	எனில் (sname == வெற்று
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    செல் fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    எனில் (put_வரி(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	செல் fail;
    எனில் (put_வரி(fd, "  wipebuf = bufnr('%')") == FAIL)
	செல் fail;
    எனில் (put_வரி(fd, "endif") == FAIL)
	செல் fail;

    // Save 'shortmess' if not storing options.
    எனில் ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_வரி(fd, "shortmess_save = &shortmess") == FAIL)
	செல் fail;

    // Set 'shortmess' for the following.
    எனில் (put_வரி(fd, "set shortmess+=aoO") == FAIL)
	செல் fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	எனில் (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != வெற்று
		&& buf->b_p_bl)
	{
	    எனில் (fprintf(fd, "badd +%ld ", buf->b_wininfo == வெற்று ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		செல் fail;
	}
    }

    // the global argument list
    எனில் (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	செல் fail;

    எனில் (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	எனில் (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    செல் fail;
    }

# ifdef FEAT_GUI
    எனில் (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	முழு	x, y;

	எனில் (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    எனில் (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		செல் fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    எனில் (p_stal == 1 && first_tabpage->tp_next != வெற்று)
    {
	எனில் (put_வரி(fd, "set stal=2") == FAIL)
	    செல் fail;
	restore_stal = TRUE;
    }

    எனில் ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    எனில் (tp->tp_next != வெற்று
		  && put_வரி(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		செல் fail;
	எனில் (first_tabpage->tp_next != வெற்று && put_வரி(fd, "tabrewind") == FAIL)
	    செல் fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	முழு	need_tabnext = FALSE;
	முழு	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	எனில் ((ssop_flags & SSOP_TABPAGES))
	{
	    எனில் (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    இல்லையெனில்
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    எனில் (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	இல்லையெனில்
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	க்கு (wp = tab_firstwin; wp != வெற்று; wp = wp->w_next)
	{
	    எனில் (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != வெற்று
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		எனில் (need_tabnext && put_வரி(fd, "tabnext") == FAIL)
		    செல் fail;
		need_tabnext = FALSE;

		எனில் (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    செல் fail;
		எனில் (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		நிறுத்து;
	    }
	}

	// If no file got edited create an empty tab page.
	எனில் (need_tabnext && put_வரி(fd, "tabnext") == FAIL)
	    செல் fail;

	எனில் (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    எனில் (put_வரி(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_வரி(fd, "save_splitright = &splitright")
								       == FAIL)
		செல் fail;
	    எனில் (put_வரி(fd, "set splitbelow splitright") == FAIL)
		செல் fail;
	    எனில் (ses_win_rec(fd, tab_topframe) == FAIL)
		செல் fail;
	    எனில் (put_வரி(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_வரி(fd, "&splitright = save_splitright")
								       == FAIL)
		செல் fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	க்கு (wp = tab_firstwin; wp != வெற்று; wp = W_NEXT(wp))
	{
	    எனில் (ses_do_win(wp))
		++nr;
	    இல்லையெனில்
		restore_size = FALSE;
	    எனில் (curwin == wp)
		cnr = nr;
	}

	எனில் (tab_firstwin->w_next != வெற்று)
	{
	    // Go to the first window.
	    எனில் (put_வரி(fd, "wincmd t") == FAIL)
		செல் fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    எனில் (!restore_height_width
		    && (put_வரி(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_வரி(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		செல் fail;
	    எனில் (put_வரி(fd, "set winminheight=0") == FAIL
		    || put_வரி(fd, "set winheight=1") == FAIL
		    || put_வரி(fd, "set winminwidth=0") == FAIL
		    || put_வரி(fd, "set winwidth=1") == FAIL)
		செல் fail;
	    restore_height_width = TRUE;
	}
	எனில் (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    செல் fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	எனில் ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != வெற்று)
	{
	    எனில் (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		செல் fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	க்கு (wp = tab_firstwin; wp != வெற்று; wp = wp->w_next)
	{
	    எனில் (!ses_do_win(wp))
		தொடர்;
	    எனில் (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# இல்லையெனில்
							 வெற்று
# endif
		 ) == FAIL)
		செல் fail;
	    எனில் (nr > 1 && put_வரி(fd, "wincmd w") == FAIL)
		செல் fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	எனில் (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    செல் fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	எனில் (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    செல் fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	எனில் (!(ssop_flags & SSOP_TABPAGES))
	    நிறுத்து;
    }

    எனில் (ssop_flags & SSOP_TABPAGES)
    {
	எனில் (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    செல் fail;
    }
    எனில் (restore_stal && put_வரி(fd, "set stal=1") == FAIL)
	செல் fail;

    // Wipe out an empty unnamed buffer we started in.
    எனில் (put_வரி(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	செல் fail;
    எனில் (put_வரி(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	செல் fail;
    எனில் (put_வரி(fd, "endif") == FAIL)
	செல் fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    எனில் (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	செல் fail;

    // Restore 'shortmess'.
    எனில் (ssop_flags & SSOP_OPTIONS)
    {
	எனில் (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    செல் fail;
    }
    இல்லையெனில்
    {
	எனில் (put_வரி(fd, "&shortmess = shortmess_save") == FAIL)
	    செல் fail;
    }

    எனில் (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	எனில் (put_வரி(fd, "&winminheight = save_winminheight") == FAIL
	      || put_வரி(fd, "&winminwidth = save_winminwidth") == FAIL)
	    செல் fail;
    }

    // Lastly, execute the x.vim file if it exists.
    எனில் (put_வரி(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_வரி(fd, "if filereadable(sx)") == FAIL
	    || put_வரி(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_வரி(fd, "endif") == FAIL)
	செல் fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    திரும்பு ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    நிலையான char_u *
get_view_file(முழு c)
{
    முழு		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    எனில் (curbuf->b_ffname == வெற்று)
    {
	emsg(_(e_no_file_name));
	திரும்பு வெற்று;
    }
    sname = home_replace_save(வெற்று, curbuf->b_ffname);
    எனில் (sname == வெற்று)
	திரும்பு வெற்று;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    க்கு (p = sname; *p; ++p)
	எனில் (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    எனில் (retval != வெற்று)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	க்கு (p = sname; *p; ++p)
	{
	    எனில் (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    இல்லையெனில் எனில் (vim_ispathsep(*p))
	    {
		*s++ = '=';
# எனில் defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		எனில் (*p == ':')
		    *s++ = '-';
		இல்லையெனில்
# endif
		    *s++ = '+';
	    }
	    இல்லையெனில்
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    திரும்பு retval;
}

/*
 * ":loadview [nr]"
 */
    வெறுமை
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    எனில் (fname == வெற்று)
	திரும்பு;

    (வெறுமை)do_source(fname, FALSE, DOSO_NONE, வெற்று);
    vim_free(fname);
}

# எனில் defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    முழு
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    எழுத்து	    *mksession_cmdline;
    unsigned முழு    save_ssop_flags;
    முழு		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    எனில் (escaped_filename == வெற்று)
	திரும்பு FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    எனில் (mksession_cmdline == வெற்று)
    {
	vim_free(escaped_filename);
	திரும்பு FALSE;
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
    எனில் (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == வெற்று
	       || put_வரி(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_வரி(fd, "unlet g:Save_VV_this_session") == FAIL);

	எனில் (fd != வெற்று && fclose(fd) != 0)
	    failed = TRUE;

	எனில் (failed)
	    mch_remove(filename);
    }

    திரும்பு !failed;
}
# endif

#endif // FEAT_SESSION

#எனில் defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
நிலையான முழு mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    வெறுமை
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    முழு		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = வெற்று;
#endif
#ifdef FEAT_SESSION
    முழு		view_session = FALSE;
    முழு		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = வெற்று;
    unsigned	*flagp;
#endif
#எனில் defined(FEAT_EVAL)
    முழு		sid;
    scriptitem_T *si = வெற்று;
#endif

    எனில் (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#இல்லையெனில்
	ex_ni(eap);
	திரும்பு;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    எனில் (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	எனில் (fname == வெற்று)
	    திரும்பு;
	viewFile = fname;
	using_vdir = TRUE;
    }
    இல்லையெனில்
#endif
	எனில் (*eap->arg != NUL)
	fname = eap->arg;
    இல்லையெனில் எனில் (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    இல்லையெனில் எனில் (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    இல்லையெனில்
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", வெற்று,
		(char_u *)_(BROWSE_FILTER_MACROS), வெற்று);
	எனில் (browseFile == வெற்று)
	    செல் theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#எனில் defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    எனில் (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    எனில் (fd != வெற்று)
    {
#ifdef FEAT_SESSION
	எனில் (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	இல்லையெனில்
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	எனில் (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	எனில் (put_வரி(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	எனில் (eap->cmdidx == CMD_mkvimrc)
	    (வெறுமை)put_வரி(fd, "version 6.0");

#ifdef FEAT_SESSION
	எனில் (eap->cmdidx == CMD_mksession)
	{
	    எனில் (put_வரி(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	எனில் (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    எனில் (p_cp)
		(வெறுமை)put_வரி(fd, "if !&cp | set cp | endif");
	    இல்லையெனில்
		(வெறுமை)put_வரி(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	எனில் (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    முழு flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_வரி(fd, "var cpo_save: string") == FAIL;

	    எனில் (eap->cmdidx == CMD_mksession)
	    {
		எனில் (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    எனில் (do_mappings)
		failed |= (makemap(fd, வெற்று) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#எனில் defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    க்கு (sid = 1; sid <= script_பொருட்கள்.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		எனில் ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    முழு b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    க்கு (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			எனில் ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    தொடர்;

			// compare prefixes if available
			எனில் (si->sn_autoload_prefix != வெற்று && b_si->sn_autoload_prefix != வெற்று
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    நிறுத்து;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			எனில் (STRCMP(name, b_name) == 0)
			    நிறுத்து;
		    }

		    // import the auto script if there are no conflicts
		    எனில் (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	எனில் (!failed && view_session)
	{
	    எனில் (put_வரி(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    எனில் (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		எனில் (dirnow == வெற்று)
		    failed = TRUE;
		இல்லையெனில்
		{
		    // Change to session file's dir.
		    எனில் (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((எழுத்து *)dirnow) != 0)
			*dirnow = NUL;
		    எனில் (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			எனில் (vim_chdirfile(fname, வெற்று) == OK)
			    shorten_fnames(TRUE);
		    }
		    இல்லையெனில் எனில் (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != வெற்று)
		    {
			எனில் (mch_chdir((எழுத்து *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    எனில் (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != வெற்று)))
		    {
			எனில் (mch_chdir((எழுத்து *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    இல்லையெனில்
	    {
		failed |= put_வரி(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								வெற்று) == FAIL);
	    }
	    எனில் (put_வரி(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_தேடல்_EXTRA
	    எனில் (no_hlsearch && put_வரி(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    எனில் (put_வரி(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    எனில் (eap->cmdidx == CMD_mksession)
	    {
		எனில் (put_வரி(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	எனில் (put_வரி(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	எனில் (failed)
	    emsg(_(e_error_while_writing));
#எனில் defined(FEAT_SESSION)
	இல்லையெனில் எனில் (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    எனில் (tbuf != வெற்று)
	    {
		எனில் (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, வெற்று, வெற்று, FALSE, curbuf);
}

#எனில் defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    எனில் (ASCII_ISUPPER(*p))
    {
	போது (*(++p))
	    எனில் (ASCII_ISLOWER(*p))
		திரும்பு VAR_FLAVOUR_SESSION;
	திரும்பு VAR_FLAVOUR_VIMINFO;
    }
    இல்லையெனில்
	திரும்பு VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    முழு
put_eol(FILE *fd)
{
    எனில் (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    முழு
put_வரி(FILE *fd, எழுத்து *s)
{
    எனில் (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	திரும்பு FAIL;
    திரும்பு OK;
}
