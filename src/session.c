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

#شامل "vim.h"

#اگر defined(FEAT_SESSION)

ایستا صحیح did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ایستا صحیح
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    صحیح		retval = OK;

    sname = home_replace_save(پوچ, name);
    اگر (sname == پوچ)
	بازگشت FAIL;

    اگر (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	برای (p = sname; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    اگر (p == پوچ)
	بازگشت FAIL;

    // write the result
    اگر (fputs((نویسه *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    بازگشت retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ایستا صحیح
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, صحیح add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    اگر (buf->b_sfname != پوچ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    وگرنه
	name = buf->b_ffname;
    اگر (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ایستا صحیح
ses_arglist(
    FILE	*fd,
    نویسه	*cmd,
    garray_T	*gap,
    صحیح		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    صحیح		i;
    char_u	*buf = پوچ;
    char_u	*s;

    اگر (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	بازگشت FAIL;
    اگر (put_سطر(fd, ":%argdel") == FAIL)
	بازگشت FAIL;
    برای (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	اگر (s != پوچ)
	{
	    اگر (fullname)
	    {
		buf = alloc(MAXPATHL);
		اگر (buf != پوچ)
		{
		    (تهی)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    اگر (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		بازگشت FAIL;
	    }
	    vim_free(buf);
	}
    }
    بازگشت OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ایستا صحیح
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    اگر (bt_terminal(wp->w_buffer))
	بازگشت !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    اگر (wp->w_buffer->b_fname == پوچ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	بازگشت (ssop_flags & SSOP_BLANK);
    اگر (bt_help(wp->w_buffer))
	بازگشت (ssop_flags & SSOP_HELP);
    بازگشت TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ایستا صحیح
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    اگر (fr->fr_layout == FR_LEAF)
	بازگشت ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	اگر (ses_do_frame(frc))
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ایستا frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	اگر (ses_do_frame(frc))
	    بشکن;
    بازگشت frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ایستا صحیح
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    صحیح		شمار = 0;

    اگر (fr->fr_layout == FR_LEAF)
	بازگشت OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    اگر (frc != پوچ)
	وقتی ((frc = ses_skipframe(frc->fr_next)) != پوچ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    اگر (put_سطر(fd, "wincmd _ | wincmd |") == FAIL
		    || put_سطر(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		بازگشت FAIL;
	    ++شمار;
	}

    // Go back to the first window.
    اگر (شمار > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", شمار) < 0
		|| put_eol(fd) == FAIL))
	بازگشت FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    وقتی (frc != پوچ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	اگر (frc != پوچ && put_سطر(fd, "wincmd w") == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

    ایستا صحیح
ses_winsizes(
    FILE	*fd,
    صحیح		restore_size,
    win_T	*tab_firstwin)
{
    صحیح		n = 0;
    win_T	*wp;

    اگر (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	برای (wp = tab_firstwin; wp != پوچ; wp = wp->w_next)
	{
	    اگر (!ses_do_win(wp))
		ادامه;
	    ++n;

	    // restore height when not full height
	    اگر (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		بازگشت FAIL;

	    // restore width when not full width
	    اگر (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		بازگشت FAIL;
	}
    }
    وگرنه
    {
	// Just equalise window sizes
	اگر (put_سطر(fd, "wincmd =") == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

    ایستا صحیح
put_view_curpos(FILE *fd, win_T *wp, نویسه *spaces)
{
    صحیح r;

    اگر (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    وگرنه
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    بازگشت r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ایستا صحیح
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    صحیح		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    صحیح		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    صحیح		f;
    صحیح		do_cursor;
    صحیح		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    اگر (wp->w_alist == &global_alist)
    {
	اگر (put_سطر(fd, "argglobal") == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	اگر (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != پوچ
			|| wp->w_localdir != پوچ, flagp) == FAIL)
	    بازگشت FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    اگر (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	اگر (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    بازگشت FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    اگر (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	اگر (bt_help(wp->w_buffer))
	{
	    نویسه *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    اگر (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (نویسه *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    اگر (put_سطر(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		بازگشت FAIL;
	}
# ifdef FEAT_TERMINAL
	وگرنه اگر (bt_terminal(wp->w_buffer))
	{
	    اگر (term_write_session(fd, wp, terminal_bufs) == FAIL)
		بازگشت FAIL;
	}
# endif
	// Load the file.
	وگرنه اگر (wp->w_buffer->b_ffname != پوچ
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    اگر (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    // No file in this buffer, just make it empty.
	    اگر (put_سطر(fd, "enew") == FAIL)
		بازگشت FAIL;
# ifdef FEAT_QUICKFIX
	    اگر (wp->w_buffer->b_ffname != پوچ)
	    {
		// The buffer does have a name, but it's not a file name.
		اگر (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    بازگشت FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    اگر (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	اگر ((flagp == &ssop_flags)
		&& alt != پوچ
		&& alt->b_fname != پوچ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    بازگشت FAIL;
    }

    // Local mappings and abbreviations.
    اگر ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	بازگشت FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    اگر (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    وگرنه اگر (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    وگرنه
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    اگر (f == FAIL)
	بازگشت FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    اگر ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != پوچ
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	اگر (put_folds(fd, wp) == FAIL)
	    بازگشت FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    اگر (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	اگر (put_سطر(fd, "{") == FAIL)
	    بازگشت FAIL;

	اگر (wp->w_height <= 0)
	{
	    اگر (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		بازگشت FAIL;
	}
	وگرنه اگر (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    بازگشت FAIL;

	اگر (put_eol(fd) == FAIL
		|| put_سطر(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_سطر(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_سطر(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    بازگشت FAIL;
	// Restore the cursor column and left offset when not wrapping.
	اگر (wp->w_cursor.col == 0)
	{
	    اگر (put_سطر(fd, "  normal! 0") == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    اگر (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		اگر (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_سطر(fd, "  endif") == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (put_view_curpos(fd, wp, "  ") == FAIL)
		بازگشت FAIL;
	}

	اگر (put_سطر(fd, "}") == FAIL)
	    بازگشت FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    اگر (wp->w_localdir != پوچ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	اگر (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    بازگشت FAIL;
	did_lcd = TRUE;
    }

    بازگشت OK;
}

# ifdef FEAT_EVAL
    ایستا صحیح
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحیح		todo;
    char_u	*p, *t;

    todo = (صحیح)gvht->ht_used;
    FOR_ALL_HASHTAB_اقلام(gvht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    اگر ((this_var->di_tv.v_type == VAR_عدد
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		اگر (p == پوچ)	    // out of memory
		    بشکن;
		برای (t = p; *t != NUL; ++t)
		    اگر (*t == '\n')
			*t = 'n';
		    وگرنه اگر (*t == '\r')
			*t = 'r';
		اگر ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_کلید,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    بازگشت FAIL;
		}
		vim_free(p);
	    }
	    وگرنه اگر (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		صحیح sign = ' ';

		اگر (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		اگر ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_کلید, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    بازگشت FAIL;
	    }
	}
    }
    بازگشت OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ایستا صحیح
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    صحیح		only_save_windows = TRUE;
    صحیح		nr;
    صحیح		restore_size = TRUE;
    صحیح		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = پوچ;
    صحیح		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    صحیح		cur_arg_idx = 0;
    صحیح		next_arg_idx = 0;
    صحیح		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    اگر (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    اگر (put_سطر(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	برو fail;

    اگر (put_سطر(fd, "doautoall SessionLoadPre") == FAIL)
	برو fail;

    اگر (put_سطر(fd, "var save_splitbelow: bool") == FAIL
	    || put_سطر(fd, "var save_splitright: bool") == FAIL
	    || put_سطر(fd, "var save_winminheight: number") == FAIL
	    || put_سطر(fd, "var save_winminwidth: number") == FAIL
	    || put_سطر(fd, "var wipebuf: number = -1") == FAIL
	    || put_سطر(fd, "var shortmess_save: string") == FAIL)
	برو fail;

    اگر (ssop_flags & SSOP_GLOBALS)
	اگر (store_session_globals(fd) == FAIL)
	    برو fail;
# endif

    // Close all windows and tabs but one.
    اگر (put_سطر(fd, "silent only") == FAIL)
	برو fail;
    اگر ((ssop_flags & SSOP_TABPAGES)
	    && put_سطر(fd, "silent tabonly") == FAIL)
	برو fail;

    // Now a :cd command to the session directory or the current directory
    اگر (ssop_flags & SSOP_SESDIR)
    {
	اگر (put_سطر(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    برو fail;
    }
    وگرنه اگر (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(پوچ, globaldir != پوچ ? globaldir : dirnow);
	اگر (sname == پوچ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    برو fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    اگر (put_سطر(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	برو fail;
    اگر (put_سطر(fd, "  wipebuf = bufnr('%')") == FAIL)
	برو fail;
    اگر (put_سطر(fd, "endif") == FAIL)
	برو fail;

    // Save 'shortmess' if not storing options.
    اگر ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_سطر(fd, "shortmess_save = &shortmess") == FAIL)
	برو fail;

    // Set 'shortmess' for the following.
    اگر (put_سطر(fd, "set shortmess+=aoO") == FAIL)
	برو fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	اگر (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != پوچ
		&& buf->b_p_bl)
	{
	    اگر (fprintf(fd, "badd +%ld ", buf->b_wininfo == پوچ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		برو fail;
	}
    }

    // the global argument list
    اگر (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	برو fail;

    اگر (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	اگر (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    برو fail;
    }

# ifdef FEAT_GUI
    اگر (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	صحیح	x, y;

	اگر (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    اگر (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		برو fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    اگر (p_stal == 1 && first_tabpage->tp_next != پوچ)
    {
	اگر (put_سطر(fd, "set stal=2") == FAIL)
	    برو fail;
	restore_stal = TRUE;
    }

    اگر ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    اگر (tp->tp_next != پوچ
		  && put_سطر(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		برو fail;
	اگر (first_tabpage->tp_next != پوچ && put_سطر(fd, "tabrewind") == FAIL)
	    برو fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	صحیح	need_tabnext = FALSE;
	صحیح	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	اگر ((ssop_flags & SSOP_TABPAGES))
	{
	    اگر (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    وگرنه
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    اگر (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	وگرنه
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	برای (wp = tab_firstwin; wp != پوچ; wp = wp->w_next)
	{
	    اگر (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != پوچ
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		اگر (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
		    برو fail;
		need_tabnext = FALSE;

		اگر (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    برو fail;
		اگر (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		بشکن;
	    }
	}

	// If no file got edited create an empty tab page.
	اگر (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
	    برو fail;

	اگر (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    اگر (put_سطر(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_سطر(fd, "save_splitright = &splitright")
								       == FAIL)
		برو fail;
	    اگر (put_سطر(fd, "set splitbelow splitright") == FAIL)
		برو fail;
	    اگر (ses_win_rec(fd, tab_topframe) == FAIL)
		برو fail;
	    اگر (put_سطر(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_سطر(fd, "&splitright = save_splitright")
								       == FAIL)
		برو fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	برای (wp = tab_firstwin; wp != پوچ; wp = W_NEXT(wp))
	{
	    اگر (ses_do_win(wp))
		++nr;
	    وگرنه
		restore_size = FALSE;
	    اگر (curwin == wp)
		cnr = nr;
	}

	اگر (tab_firstwin->w_next != پوچ)
	{
	    // Go to the first window.
	    اگر (put_سطر(fd, "wincmd t") == FAIL)
		برو fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    اگر (!restore_height_width
		    && (put_سطر(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_سطر(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		برو fail;
	    اگر (put_سطر(fd, "set winminheight=0") == FAIL
		    || put_سطر(fd, "set winheight=1") == FAIL
		    || put_سطر(fd, "set winminwidth=0") == FAIL
		    || put_سطر(fd, "set winwidth=1") == FAIL)
		برو fail;
	    restore_height_width = TRUE;
	}
	اگر (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    برو fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	اگر ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != پوچ)
	{
	    اگر (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		برو fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	برای (wp = tab_firstwin; wp != پوچ; wp = wp->w_next)
	{
	    اگر (!ses_do_win(wp))
		ادامه;
	    اگر (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# وگرنه
							 پوچ
# endif
		 ) == FAIL)
		برو fail;
	    اگر (nr > 1 && put_سطر(fd, "wincmd w") == FAIL)
		برو fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	اگر (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    برو fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	اگر (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    برو fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	اگر (!(ssop_flags & SSOP_TABPAGES))
	    بشکن;
    }

    اگر (ssop_flags & SSOP_TABPAGES)
    {
	اگر (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    برو fail;
    }
    اگر (restore_stal && put_سطر(fd, "set stal=1") == FAIL)
	برو fail;

    // Wipe out an empty unnamed buffer we started in.
    اگر (put_سطر(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	برو fail;
    اگر (put_سطر(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	برو fail;
    اگر (put_سطر(fd, "endif") == FAIL)
	برو fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    اگر (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	برو fail;

    // Restore 'shortmess'.
    اگر (ssop_flags & SSOP_OPTIONS)
    {
	اگر (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    برو fail;
    }
    وگرنه
    {
	اگر (put_سطر(fd, "&shortmess = shortmess_save") == FAIL)
	    برو fail;
    }

    اگر (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	اگر (put_سطر(fd, "&winminheight = save_winminheight") == FAIL
	      || put_سطر(fd, "&winminwidth = save_winminwidth") == FAIL)
	    برو fail;
    }

    // Lastly, execute the x.vim file if it exists.
    اگر (put_سطر(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_سطر(fd, "if filereadable(sx)") == FAIL
	    || put_سطر(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_سطر(fd, "endif") == FAIL)
	برو fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    بازگشت ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ایستا char_u *
get_view_file(صحیح c)
{
    صحیح		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    اگر (curbuf->b_ffname == پوچ)
    {
	emsg(_(e_no_file_name));
	بازگشت پوچ;
    }
    sname = home_replace_save(پوچ, curbuf->b_ffname);
    اگر (sname == پوچ)
	بازگشت پوچ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    برای (p = sname; *p; ++p)
	اگر (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    اگر (retval != پوچ)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	برای (p = sname; *p; ++p)
	{
	    اگر (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    وگرنه اگر (vim_ispathsep(*p))
	    {
		*s++ = '=';
# اگر defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		اگر (*p == ':')
		    *s++ = '-';
		وگرنه
# endif
		    *s++ = '+';
	    }
	    وگرنه
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    بازگشت retval;
}

/*
 * ":loadview [nr]"
 */
    تهی
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    اگر (fname == پوچ)
	بازگشت;

    (تهی)do_source(fname, FALSE, DOSO_NONE, پوچ);
    vim_free(fname);
}

# اگر defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    صحیح
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    نویسه	    *mksession_cmdline;
    unsigned صحیح    save_ssop_flags;
    صحیح		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    اگر (escaped_filename == پوچ)
	بازگشت FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    اگر (mksession_cmdline == پوچ)
    {
	vim_free(escaped_filename);
	بازگشت FALSE;
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
    اگر (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == پوچ
	       || put_سطر(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_سطر(fd, "unlet g:Save_VV_this_session") == FAIL);

	اگر (fd != پوچ && fclose(fd) != 0)
	    failed = TRUE;

	اگر (failed)
	    mch_remove(filename);
    }

    بازگشت !failed;
}
# endif

#endif // FEAT_SESSION

#اگر defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ایستا صحیح mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    تهی
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    صحیح		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = پوچ;
#endif
#ifdef FEAT_SESSION
    صحیح		view_session = FALSE;
    صحیح		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = پوچ;
    unsigned	*flagp;
#endif
#اگر defined(FEAT_EVAL)
    صحیح		sid;
    scriptitem_T *si = پوچ;
#endif

    اگر (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#وگرنه
	ex_ni(eap);
	بازگشت;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    اگر (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	اگر (fname == پوچ)
	    بازگشت;
	viewFile = fname;
	using_vdir = TRUE;
    }
    وگرنه
#endif
	اگر (*eap->arg != NUL)
	fname = eap->arg;
    وگرنه اگر (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    وگرنه اگر (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    وگرنه
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", پوچ,
		(char_u *)_(BROWSE_FILTER_MACROS), پوچ);
	اگر (browseFile == پوچ)
	    برو theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#اگر defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    اگر (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    اگر (fd != پوچ)
    {
#ifdef FEAT_SESSION
	اگر (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	وگرنه
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	اگر (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	اگر (put_سطر(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	اگر (eap->cmdidx == CMD_mkvimrc)
	    (تهی)put_سطر(fd, "version 6.0");

#ifdef FEAT_SESSION
	اگر (eap->cmdidx == CMD_mksession)
	{
	    اگر (put_سطر(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	اگر (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    اگر (p_cp)
		(تهی)put_سطر(fd, "if !&cp | set cp | endif");
	    وگرنه
		(تهی)put_سطر(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	اگر (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    صحیح flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_سطر(fd, "var cpo_save: string") == FAIL;

	    اگر (eap->cmdidx == CMD_mksession)
	    {
		اگر (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    اگر (do_mappings)
		failed |= (makemap(fd, پوچ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#اگر defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    برای (sid = 1; sid <= script_اقلام.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		اگر ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    صحیح b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    برای (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			اگر ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ادامه;

			// compare prefixes if available
			اگر (si->sn_autoload_prefix != پوچ && b_si->sn_autoload_prefix != پوچ
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    بشکن;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			اگر (STRCMP(name, b_name) == 0)
			    بشکن;
		    }

		    // import the auto script if there are no conflicts
		    اگر (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	اگر (!failed && view_session)
	{
	    اگر (put_سطر(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    اگر (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		اگر (dirnow == پوچ)
		    failed = TRUE;
		وگرنه
		{
		    // Change to session file's dir.
		    اگر (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((نویسه *)dirnow) != 0)
			*dirnow = NUL;
		    اگر (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			اگر (vim_chdirfile(fname, پوچ) == OK)
			    shorten_fnames(TRUE);
		    }
		    وگرنه اگر (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != پوچ)
		    {
			اگر (mch_chdir((نویسه *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    اگر (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != پوچ)))
		    {
			اگر (mch_chdir((نویسه *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    وگرنه
	    {
		failed |= put_سطر(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								پوچ) == FAIL);
	    }
	    اگر (put_سطر(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_جستجو_EXTRA
	    اگر (no_hlsearch && put_سطر(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    اگر (put_سطر(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    اگر (eap->cmdidx == CMD_mksession)
	    {
		اگر (put_سطر(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	اگر (put_سطر(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	اگر (failed)
	    emsg(_(e_error_while_writing));
#اگر defined(FEAT_SESSION)
	وگرنه اگر (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    اگر (tbuf != پوچ)
	    {
		اگر (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, پوچ, پوچ, FALSE, curbuf);
}

#اگر defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    اگر (ASCII_ISUPPER(*p))
    {
	وقتی (*(++p))
	    اگر (ASCII_ISLOWER(*p))
		بازگشت VAR_FLAVOUR_SESSION;
	بازگشت VAR_FLAVOUR_VIMINFO;
    }
    وگرنه
	بازگشت VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    صحیح
put_eol(FILE *fd)
{
    اگر (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    صحیح
put_سطر(FILE *fd, نویسه *s)
{
    اگر (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	بازگشت FAIL;
    بازگشت OK;
}
