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

#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_SESSION)

ساكن صحيح did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ساكن صحيح
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    صحيح		retval = OK;

    sname = home_replace_save(عدم, name);
    𐡄𐡍 (sname == عدم)
	أرجع FAIL;

    𐡄𐡍 (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	لكل (p = sname; *p != NUL; MB_PTR_ADV(p))
	    𐡄𐡍 (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    𐡄𐡍 (p == عدم)
	أرجع FAIL;

    // write the result
    𐡄𐡍 (fputs((حرف *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    أرجع retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ساكن صحيح
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, صحيح add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    𐡄𐡍 (buf->b_sfname != عدم
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    وإلا
	name = buf->b_ffname;
    𐡄𐡍 (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	أرجع FAIL;
    أرجع OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ساكن صحيح
ses_arglist(
    FILE	*fd,
    حرف	*cmd,
    garray_T	*gap,
    صحيح		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    صحيح		i;
    char_u	*buf = عدم;
    char_u	*s;

    𐡄𐡍 (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	أرجع FAIL;
    𐡄𐡍 (put_line(fd, ":%argdel") == FAIL)
	أرجع FAIL;
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	𐡄𐡍 (s != عدم)
	{
	    𐡄𐡍 (fullname)
	    {
		buf = alloc(MAXPATHL);
		𐡄𐡍 (buf != عدم)
		{
		    (فراغ)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    𐡄𐡍 (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		أرجع FAIL;
	    }
	    vim_free(buf);
	}
    }
    أرجع OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ساكن صحيح
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    𐡄𐡍 (bt_terminal(wp->w_buffer))
	أرجع !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    𐡄𐡍 (wp->w_buffer->b_fname == عدم
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	أرجع (ssop_flags & SSOP_BLANK);
    𐡄𐡍 (bt_help(wp->w_buffer))
	أرجع (ssop_flags & SSOP_HELP);
    أرجع TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ساكن صحيح
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    𐡄𐡍 (fr->fr_layout == FR_LEAF)
	أرجع ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	𐡄𐡍 (ses_do_frame(frc))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ساكن frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	𐡄𐡍 (ses_do_frame(frc))
	    اكسر;
    أرجع frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ساكن صحيح
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    صحيح		count = 0;

    𐡄𐡍 (fr->fr_layout == FR_LEAF)
	أرجع OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    𐡄𐡍 (frc != عدم)
	طالما ((frc = ses_skipframe(frc->fr_next)) != عدم)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    𐡄𐡍 (put_line(fd, "wincmd _ | wincmd |") == FAIL
		    || put_line(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		أرجع FAIL;
	    ++count;
	}

    // Go back to the first window.
    𐡄𐡍 (count > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", count) < 0
		|| put_eol(fd) == FAIL))
	أرجع FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    طالما (frc != عدم)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	𐡄𐡍 (frc != عدم && put_line(fd, "wincmd w") == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

    ساكن صحيح
ses_winsizes(
    FILE	*fd,
    صحيح		restore_size,
    win_T	*tab_firstwin)
{
    صحيح		n = 0;
    win_T	*wp;

    𐡄𐡍 (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_next)
	{
	    𐡄𐡍 (!ses_do_win(wp))
		استمر;
	    ++n;

	    // restore height when not full height
	    𐡄𐡍 (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		أرجع FAIL;

	    // restore width when not full width
	    𐡄𐡍 (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		أرجع FAIL;
	}
    }
    وإلا
    {
	// Just equalise window sizes
	𐡄𐡍 (put_line(fd, "wincmd =") == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
put_view_curpos(FILE *fd, win_T *wp, حرف *spaces)
{
    صحيح r;

    𐡄𐡍 (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    وإلا
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    أرجع r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ساكن صحيح
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    صحيح		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    صحيح		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    صحيح		f;
    صحيح		do_cursor;
    صحيح		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    𐡄𐡍 (wp->w_alist == &global_alist)
    {
	𐡄𐡍 (put_line(fd, "argglobal") == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	𐡄𐡍 (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != عدم
			|| wp->w_localdir != عدم, flagp) == FAIL)
	    أرجع FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    𐡄𐡍 (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	𐡄𐡍 (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    𐡄𐡍 (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	𐡄𐡍 (bt_help(wp->w_buffer))
	{
	    حرف *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    𐡄𐡍 (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (حرف *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    𐡄𐡍 (put_line(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		أرجع FAIL;
	}
# ifdef FEAT_TERMINAL
	وإلا 𐡄𐡍 (bt_terminal(wp->w_buffer))
	{
	    𐡄𐡍 (term_write_session(fd, wp, terminal_bufs) == FAIL)
		أرجع FAIL;
	}
# endif
	// Load the file.
	وإلا 𐡄𐡍 (wp->w_buffer->b_ffname != عدم
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    𐡄𐡍 (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    // No file in this buffer, just make it empty.
	    𐡄𐡍 (put_line(fd, "enew") == FAIL)
		أرجع FAIL;
# ifdef FEAT_QUICKFIX
	    𐡄𐡍 (wp->w_buffer->b_ffname != عدم)
	    {
		// The buffer does have a name, but it's not a file name.
		𐡄𐡍 (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    أرجع FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    𐡄𐡍 (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	𐡄𐡍 ((flagp == &ssop_flags)
		&& alt != عدم
		&& alt->b_fname != عدم
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    أرجع FAIL;
    }

    // Local mappings and abbreviations.
    𐡄𐡍 ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	أرجع FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    𐡄𐡍 (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    وإلا 𐡄𐡍 (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    وإلا
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    𐡄𐡍 (f == FAIL)
	أرجع FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    𐡄𐡍 ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != عدم
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	𐡄𐡍 (put_folds(fd, wp) == FAIL)
	    أرجع FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    𐡄𐡍 (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	𐡄𐡍 (put_line(fd, "{") == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (wp->w_height <= 0)
	{
	    𐡄𐡍 (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		أرجع FAIL;
	}
	وإلا 𐡄𐡍 (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    أرجع FAIL;

	𐡄𐡍 (put_eol(fd) == FAIL
		|| put_line(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_line(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_line(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	// Restore the cursor column and left offset when not wrapping.
	𐡄𐡍 (wp->w_cursor.col == 0)
	{
	    𐡄𐡍 (put_line(fd, "  normal! 0") == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    𐡄𐡍 (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		𐡄𐡍 (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_line(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_line(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_line(fd, "  endif") == FAIL)
		    أرجع FAIL;
	    }
	    وإلا 𐡄𐡍 (put_view_curpos(fd, wp, "  ") == FAIL)
		أرجع FAIL;
	}

	𐡄𐡍 (put_line(fd, "}") == FAIL)
	    أرجع FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    𐡄𐡍 (wp->w_localdir != عدم
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	𐡄𐡍 (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	did_lcd = TRUE;
    }

    أرجع OK;
}

# ifdef FEAT_EVAL
    ساكن صحيح
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحيح		todo;
    char_u	*p, *t;

    todo = (صحيح)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	𐡄𐡍 (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    𐡄𐡍 ((this_var->di_tv.v_type == VAR_NUMBER
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		𐡄𐡍 (p == عدم)	    // out of memory
		    اكسر;
		لكل (t = p; *t != NUL; ++t)
		    𐡄𐡍 (*t == '\n')
			*t = 'n';
		    وإلا 𐡄𐡍 (*t == '\r')
			*t = 'r';
		𐡄𐡍 ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_key,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    أرجع FAIL;
		}
		vim_free(p);
	    }
	    وإلا 𐡄𐡍 (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		صحيح sign = ' ';

		𐡄𐡍 (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		𐡄𐡍 ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_key, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    أرجع FAIL;
	    }
	}
    }
    أرجع OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ساكن صحيح
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    صحيح		only_save_windows = TRUE;
    صحيح		nr;
    صحيح		restore_size = TRUE;
    صحيح		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = عدم;
    صحيح		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    صحيح		cur_arg_idx = 0;
    صحيح		next_arg_idx = 0;
    صحيح		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    𐡄𐡍 (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    𐡄𐡍 (put_line(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	اذهب fail;

    𐡄𐡍 (put_line(fd, "doautoall SessionLoadPre") == FAIL)
	اذهب fail;

    𐡄𐡍 (put_line(fd, "var save_splitbelow: bool") == FAIL
	    || put_line(fd, "var save_splitright: bool") == FAIL
	    || put_line(fd, "var save_winminheight: number") == FAIL
	    || put_line(fd, "var save_winminwidth: number") == FAIL
	    || put_line(fd, "var wipebuf: number = -1") == FAIL
	    || put_line(fd, "var shortmess_save: string") == FAIL)
	اذهب fail;

    𐡄𐡍 (ssop_flags & SSOP_GLOBALS)
	𐡄𐡍 (store_session_globals(fd) == FAIL)
	    اذهب fail;
# endif

    // Close all windows and tabs but one.
    𐡄𐡍 (put_line(fd, "silent only") == FAIL)
	اذهب fail;
    𐡄𐡍 ((ssop_flags & SSOP_TABPAGES)
	    && put_line(fd, "silent tabonly") == FAIL)
	اذهب fail;

    // Now a :cd command to the session directory or the current directory
    𐡄𐡍 (ssop_flags & SSOP_SESDIR)
    {
	𐡄𐡍 (put_line(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    اذهب fail;
    }
    وإلا 𐡄𐡍 (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(عدم, globaldir != عدم ? globaldir : dirnow);
	𐡄𐡍 (sname == عدم
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    اذهب fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    𐡄𐡍 (put_line(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	اذهب fail;
    𐡄𐡍 (put_line(fd, "  wipebuf = bufnr('%')") == FAIL)
	اذهب fail;
    𐡄𐡍 (put_line(fd, "endif") == FAIL)
	اذهب fail;

    // Save 'shortmess' if not storing options.
    𐡄𐡍 ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_line(fd, "shortmess_save = &shortmess") == FAIL)
	اذهب fail;

    // Set 'shortmess' for the following.
    𐡄𐡍 (put_line(fd, "set shortmess+=aoO") == FAIL)
	اذهب fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	𐡄𐡍 (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != عدم
		&& buf->b_p_bl)
	{
	    𐡄𐡍 (fprintf(fd, "badd +%ld ", buf->b_wininfo == عدم ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		اذهب fail;
	}
    }

    // the global argument list
    𐡄𐡍 (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	اذهب fail;

    𐡄𐡍 (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	𐡄𐡍 (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    اذهب fail;
    }

# ifdef FEAT_GUI
    𐡄𐡍 (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	صحيح	x, y;

	𐡄𐡍 (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    𐡄𐡍 (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		اذهب fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    𐡄𐡍 (p_stal == 1 && first_tabpage->tp_next != عدم)
    {
	𐡄𐡍 (put_line(fd, "set stal=2") == FAIL)
	    اذهب fail;
	restore_stal = TRUE;
    }

    𐡄𐡍 ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    𐡄𐡍 (tp->tp_next != عدم
		  && put_line(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		اذهب fail;
	𐡄𐡍 (first_tabpage->tp_next != عدم && put_line(fd, "tabrewind") == FAIL)
	    اذهب fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	صحيح	need_tabnext = FALSE;
	صحيح	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	𐡄𐡍 ((ssop_flags & SSOP_TABPAGES))
	{
	    𐡄𐡍 (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    وإلا
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    𐡄𐡍 (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	وإلا
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_next)
	{
	    𐡄𐡍 (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != عدم
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		𐡄𐡍 (need_tabnext && put_line(fd, "tabnext") == FAIL)
		    اذهب fail;
		need_tabnext = FALSE;

		𐡄𐡍 (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    اذهب fail;
		𐡄𐡍 (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		اكسر;
	    }
	}

	// If no file got edited create an empty tab page.
	𐡄𐡍 (need_tabnext && put_line(fd, "tabnext") == FAIL)
	    اذهب fail;

	𐡄𐡍 (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    𐡄𐡍 (put_line(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_line(fd, "save_splitright = &splitright")
								       == FAIL)
		اذهب fail;
	    𐡄𐡍 (put_line(fd, "set splitbelow splitright") == FAIL)
		اذهب fail;
	    𐡄𐡍 (ses_win_rec(fd, tab_topframe) == FAIL)
		اذهب fail;
	    𐡄𐡍 (put_line(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_line(fd, "&splitright = save_splitright")
								       == FAIL)
		اذهب fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	لكل (wp = tab_firstwin; wp != عدم; wp = W_NEXT(wp))
	{
	    𐡄𐡍 (ses_do_win(wp))
		++nr;
	    وإلا
		restore_size = FALSE;
	    𐡄𐡍 (curwin == wp)
		cnr = nr;
	}

	𐡄𐡍 (tab_firstwin->w_next != عدم)
	{
	    // Go to the first window.
	    𐡄𐡍 (put_line(fd, "wincmd t") == FAIL)
		اذهب fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    𐡄𐡍 (!restore_height_width
		    && (put_line(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_line(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		اذهب fail;
	    𐡄𐡍 (put_line(fd, "set winminheight=0") == FAIL
		    || put_line(fd, "set winheight=1") == FAIL
		    || put_line(fd, "set winminwidth=0") == FAIL
		    || put_line(fd, "set winwidth=1") == FAIL)
		اذهب fail;
	    restore_height_width = TRUE;
	}
	𐡄𐡍 (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    اذهب fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	𐡄𐡍 ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != عدم)
	{
	    𐡄𐡍 (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		اذهب fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_next)
	{
	    𐡄𐡍 (!ses_do_win(wp))
		استمر;
	    𐡄𐡍 (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# وإلا
							 عدم
# endif
		 ) == FAIL)
		اذهب fail;
	    𐡄𐡍 (nr > 1 && put_line(fd, "wincmd w") == FAIL)
		اذهب fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	𐡄𐡍 (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    اذهب fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	𐡄𐡍 (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    اذهب fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	𐡄𐡍 (!(ssop_flags & SSOP_TABPAGES))
	    اكسر;
    }

    𐡄𐡍 (ssop_flags & SSOP_TABPAGES)
    {
	𐡄𐡍 (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    اذهب fail;
    }
    𐡄𐡍 (restore_stal && put_line(fd, "set stal=1") == FAIL)
	اذهب fail;

    // Wipe out an empty unnamed buffer we started in.
    𐡄𐡍 (put_line(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	اذهب fail;
    𐡄𐡍 (put_line(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	اذهب fail;
    𐡄𐡍 (put_line(fd, "endif") == FAIL)
	اذهب fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    𐡄𐡍 (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	اذهب fail;

    // Restore 'shortmess'.
    𐡄𐡍 (ssop_flags & SSOP_OPTIONS)
    {
	𐡄𐡍 (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    اذهب fail;
    }
    وإلا
    {
	𐡄𐡍 (put_line(fd, "&shortmess = shortmess_save") == FAIL)
	    اذهب fail;
    }

    𐡄𐡍 (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	𐡄𐡍 (put_line(fd, "&winminheight = save_winminheight") == FAIL
	      || put_line(fd, "&winminwidth = save_winminwidth") == FAIL)
	    اذهب fail;
    }

    // Lastly, execute the x.vim file if it exists.
    𐡄𐡍 (put_line(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_line(fd, "if filereadable(sx)") == FAIL
	    || put_line(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_line(fd, "endif") == FAIL)
	اذهب fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    أرجع ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ساكن char_u *
get_view_file(صحيح c)
{
    صحيح		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    𐡄𐡍 (curbuf->b_ffname == عدم)
    {
	emsg(_(e_no_file_name));
	أرجع عدم;
    }
    sname = home_replace_save(عدم, curbuf->b_ffname);
    𐡄𐡍 (sname == عدم)
	أرجع عدم;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    لكل (p = sname; *p; ++p)
	𐡄𐡍 (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    𐡄𐡍 (retval != عدم)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	لكل (p = sname; *p; ++p)
	{
	    𐡄𐡍 (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    وإلا 𐡄𐡍 (vim_ispathsep(*p))
	    {
		*s++ = '=';
# 𐡄𐡍 defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		𐡄𐡍 (*p == ':')
		    *s++ = '-';
		وإلا
# endif
		    *s++ = '+';
	    }
	    وإلا
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    أرجع retval;
}

/*
 * ":loadview [nr]"
 */
    فراغ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    𐡄𐡍 (fname == عدم)
	أرجع;

    (فراغ)do_source(fname, FALSE, DOSO_NONE, عدم);
    vim_free(fname);
}

# 𐡄𐡍 defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    صحيح
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    حرف	    *mksession_cmdline;
    unsigned صحيح    save_ssop_flags;
    صحيح		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    𐡄𐡍 (escaped_filename == عدم)
	أرجع FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    𐡄𐡍 (mksession_cmdline == عدم)
    {
	vim_free(escaped_filename);
	أرجع FALSE;
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
    𐡄𐡍 (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == عدم
	       || put_line(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_line(fd, "unlet g:Save_VV_this_session") == FAIL);

	𐡄𐡍 (fd != عدم && fclose(fd) != 0)
	    failed = TRUE;

	𐡄𐡍 (failed)
	    mch_remove(filename);
    }

    أرجع !failed;
}
# endif

#endif // FEAT_SESSION

#𐡄𐡍 defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ساكن صحيح mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    فراغ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    صحيح		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = عدم;
#endif
#ifdef FEAT_SESSION
    صحيح		view_session = FALSE;
    صحيح		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = عدم;
    unsigned	*flagp;
#endif
#𐡄𐡍 defined(FEAT_EVAL)
    صحيح		sid;
    scriptitem_T *si = عدم;
#endif

    𐡄𐡍 (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#وإلا
	ex_ni(eap);
	أرجع;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    𐡄𐡍 (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	𐡄𐡍 (fname == عدم)
	    أرجع;
	viewFile = fname;
	using_vdir = TRUE;
    }
    وإلا
#endif
	𐡄𐡍 (*eap->arg != NUL)
	fname = eap->arg;
    وإلا 𐡄𐡍 (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    وإلا 𐡄𐡍 (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    وإلا
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    𐡄𐡍 (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", عدم,
		(char_u *)_(BROWSE_FILTER_MACROS), عدم);
	𐡄𐡍 (browseFile == عدم)
	    اذهب theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#𐡄𐡍 defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    𐡄𐡍 (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    𐡄𐡍 (fd != عدم)
    {
#ifdef FEAT_SESSION
	𐡄𐡍 (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	وإلا
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	𐡄𐡍 (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	𐡄𐡍 (put_line(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	𐡄𐡍 (eap->cmdidx == CMD_mkvimrc)
	    (فراغ)put_line(fd, "version 6.0");

#ifdef FEAT_SESSION
	𐡄𐡍 (eap->cmdidx == CMD_mksession)
	{
	    𐡄𐡍 (put_line(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	𐡄𐡍 (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    𐡄𐡍 (p_cp)
		(فراغ)put_line(fd, "if !&cp | set cp | endif");
	    وإلا
		(فراغ)put_line(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	𐡄𐡍 (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    صحيح flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_line(fd, "var cpo_save: string") == FAIL;

	    𐡄𐡍 (eap->cmdidx == CMD_mksession)
	    {
		𐡄𐡍 (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    𐡄𐡍 (do_mappings)
		failed |= (makemap(fd, عدم) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#𐡄𐡍 defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    لكل (sid = 1; sid <= script_items.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		𐡄𐡍 ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    صحيح b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    لكل (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			𐡄𐡍 ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    استمر;

			// compare prefixes if available
			𐡄𐡍 (si->sn_autoload_prefix != عدم && b_si->sn_autoload_prefix != عدم
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    اكسر;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			𐡄𐡍 (STRCMP(name, b_name) == 0)
			    اكسر;
		    }

		    // import the auto script if there are no conflicts
		    𐡄𐡍 (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	𐡄𐡍 (!failed && view_session)
	{
	    𐡄𐡍 (put_line(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    𐡄𐡍 (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		𐡄𐡍 (dirnow == عدم)
		    failed = TRUE;
		وإلا
		{
		    // Change to session file's dir.
		    𐡄𐡍 (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((حرف *)dirnow) != 0)
			*dirnow = NUL;
		    𐡄𐡍 (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			𐡄𐡍 (vim_chdirfile(fname, عدم) == OK)
			    shorten_fnames(TRUE);
		    }
		    وإلا 𐡄𐡍 (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != عدم)
		    {
			𐡄𐡍 (mch_chdir((حرف *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    𐡄𐡍 (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != عدم)))
		    {
			𐡄𐡍 (mch_chdir((حرف *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    وإلا
	    {
		failed |= put_line(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								عدم) == FAIL);
	    }
	    𐡄𐡍 (put_line(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	    𐡄𐡍 (no_hlsearch && put_line(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    𐡄𐡍 (put_line(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    𐡄𐡍 (eap->cmdidx == CMD_mksession)
	    {
		𐡄𐡍 (put_line(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	𐡄𐡍 (put_line(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	𐡄𐡍 (failed)
	    emsg(_(e_error_while_writing));
#𐡄𐡍 defined(FEAT_SESSION)
	وإلا 𐡄𐡍 (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    𐡄𐡍 (tbuf != عدم)
	    {
		𐡄𐡍 (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, عدم, عدم, FALSE, curbuf);
}

#𐡄𐡍 defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    𐡄𐡍 (ASCII_ISUPPER(*p))
    {
	طالما (*(++p))
	    𐡄𐡍 (ASCII_ISLOWER(*p))
		أرجع VAR_FLAVOUR_SESSION;
	أرجع VAR_FLAVOUR_VIMINFO;
    }
    وإلا
	أرجع VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    صحيح
put_eol(FILE *fd)
{
    𐡄𐡍 (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	أرجع FAIL;
    أرجع OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    صحيح
put_line(FILE *fd, حرف *s)
{
    𐡄𐡍 (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	أرجع FAIL;
    أرجع OK;
}
