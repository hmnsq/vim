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

#include "vim.h"

#ប្រសិនបើ defined(FEAT_SESSION)

static ចំនួនគត់ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    static ចំនួនគត់
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    ចំនួនគត់		retval = OK;

    sname = home_replace_save(NULL, name);
    ប្រសិនបើ (sname == NULL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	សម្រាប់ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ប្រសិនបើ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ប្រសិនបើ (p == NULL)
	ត្រឡប់ FAIL;

    // write the result
    ប្រសិនបើ (fputs((var_na *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ត្រឡប់ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    static ចំនួនគត់
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, ចំនួនគត់ add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ប្រសិនបើ (buf->b_sfname != NULL
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    បើមិនដូច្នេះ
	name = buf->b_ffname;
    ប្រសិនបើ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    static ចំនួនគត់
ses_arglist(
    FILE	*fd,
    var_na	*cmd,
    garray_T	*gap,
    ចំនួនគត់		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    ចំនួនគត់		i;
    char_u	*buf = NULL;
    char_u	*s;

    ប្រសិនបើ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (put_line(fd, ":%argdel") == FAIL)
	ត្រឡប់ FAIL;
    សម្រាប់ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ប្រសិនបើ (s != NULL)
	{
	    ប្រសិនបើ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ប្រសិនបើ (buf != NULL)
		{
		    (void)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ប្រសិនបើ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ត្រឡប់ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ត្រឡប់ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    static ចំនួនគត់
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ប្រសិនបើ (bt_terminal(wp->w_buffer))
	ត្រឡប់ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ប្រសិនបើ (wp->w_buffer->b_fname == NULL
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ត្រឡប់ (ssop_flags & SSOP_BLANK);
    ប្រសិនបើ (bt_help(wp->w_buffer))
	ត្រឡប់ (ssop_flags & SSOP_HELP);
    ត្រឡប់ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    static ចំនួនគត់
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ប្រសិនបើ (fr->fr_layout == FR_LEAF)
	ត្រឡប់ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ប្រសិនបើ (ses_do_frame(frc))
	    ត្រឡប់ TRUE;
    ត្រឡប់ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    static frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	ប្រសិនបើ (ses_do_frame(frc))
	    bha_nga;
    ត្រឡប់ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    static ចំនួនគត់
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    ចំនួនគត់		count = 0;

    ប្រសិនបើ (fr->fr_layout == FR_LEAF)
	ត្រឡប់ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ប្រសិនបើ (frc != NULL)
	ខណៈពេល ((frc = ses_skipframe(frc->fr_next)) != NULL)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ប្រសិនបើ (put_line(fd, "wincmd _ | wincmd |") == FAIL
		    || put_line(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ត្រឡប់ FAIL;
	    ++count;
	}

    // Go back to the first window.
    ប្រសិនបើ (count > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", count) < 0
		|| put_eol(fd) == FAIL))
	ត្រឡប់ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ខណៈពេល (frc != NULL)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ប្រសិនបើ (frc != NULL && put_line(fd, "wincmd w") == FAIL)
	    ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

    static ចំនួនគត់
ses_winsizes(
    FILE	*fd,
    ចំនួនគត់		restore_size,
    win_T	*tab_firstwin)
{
    ចំនួនគត់		n = 0;
    win_T	*wp;

    ប្រសិនបើ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	សម្រាប់ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ប្រសិនបើ (!ses_do_win(wp))
		continue;
	    ++n;

	    // restore height when not full height
	    ប្រសិនបើ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ត្រឡប់ FAIL;

	    // restore width when not full width
	    ប្រសិនបើ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ត្រឡប់ FAIL;
	}
    }
    បើមិនដូច្នេះ
    {
	// Just equalise window sizes
	ប្រសិនបើ (put_line(fd, "wincmd =") == FAIL)
	    ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

    static ចំនួនគត់
put_view_curpos(FILE *fd, win_T *wp, var_na *spaces)
{
    ចំនួនគត់ r;

    ប្រសិនបើ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    បើមិនដូច្នេះ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ត្រឡប់ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    static ចំនួនគត់
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ចំនួនគត់		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    ចំនួនគត់		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ចំនួនគត់		f;
    ចំនួនគត់		do_cursor;
    ចំនួនគត់		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ប្រសិនបើ (wp->w_alist == &global_alist)
    {
	ប្រសិនបើ (put_line(fd, "argglobal") == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != NULL
			|| wp->w_localdir != NULL, flagp) == FAIL)
	    ត្រឡប់ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ប្រសិនបើ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ប្រសិនបើ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ត្រឡប់ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ប្រសិនបើ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ប្រសិនបើ (bt_help(wp->w_buffer))
	{
	    var_na *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ប្រសិនបើ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (var_na *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ប្រសិនបើ (put_line(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ត្រឡប់ FAIL;
	}
# ifdef FEAT_TERMINAL
	បើមិនដូច្នេះ ប្រសិនបើ (bt_terminal(wp->w_buffer))
	{
	    ប្រសិនបើ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ត្រឡប់ FAIL;
	}
# endif
	// Load the file.
	បើមិនដូច្នេះ ប្រសិនបើ (wp->w_buffer->b_ffname != NULL
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ប្រសិនបើ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ
	{
	    // No file in this buffer, just make it empty.
	    ប្រសិនបើ (put_line(fd, "enew") == FAIL)
		ត្រឡប់ FAIL;
# ifdef FEAT_QUICKFIX
	    ប្រសិនបើ (wp->w_buffer->b_ffname != NULL)
	    {
		// The buffer does have a name, but it's not a file name.
		ប្រសិនបើ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ត្រឡប់ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ប្រសិនបើ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ប្រសិនបើ ((flagp == &ssop_flags)
		&& alt != NULL
		&& alt->b_fname != NULL
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ត្រឡប់ FAIL;
    }

    // Local mappings and abbreviations.
    ប្រសិនបើ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ត្រឡប់ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ប្រសិនបើ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    បើមិនដូច្នេះ ប្រសិនបើ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    បើមិនដូច្នេះ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ប្រសិនបើ (f == FAIL)
	ត្រឡប់ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ប្រសិនបើ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != NULL
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ប្រសិនបើ (put_folds(fd, wp) == FAIL)
	    ត្រឡប់ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ប្រសិនបើ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ប្រសិនបើ (put_line(fd, "{") == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (wp->w_height <= 0)
	{
	    ប្រសិនបើ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (put_eol(fd) == FAIL
		|| put_line(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_line(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_line(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ត្រឡប់ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ប្រសិនបើ (wp->w_cursor.col == 0)
	{
	    ប្រសិនបើ (put_line(fd, "  normal! 0") == FAIL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ប្រសិនបើ (fprintf(fd,
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
		    ត្រឡប់ FAIL;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (put_view_curpos(fd, wp, "  ") == FAIL)
		ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (put_line(fd, "}") == FAIL)
	    ត្រឡប់ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ប្រសិនបើ (wp->w_localdir != NULL
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ប្រសិនបើ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ត្រឡប់ FAIL;
	did_lcd = TRUE;
    }

    ត្រឡប់ OK;
}

# ifdef FEAT_EVAL
    static ចំនួនគត់
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ចំនួនគត់		todo;
    char_u	*p, *t;

    todo = (ចំនួនគត់)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	ប្រសិនបើ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ប្រសិនបើ ((this_var->di_tv.v_type == VAR_NUMBER
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ប្រសិនបើ (p == NULL)	    // out of memory
		    bha_nga;
		សម្រាប់ (t = p; *t != NUL; ++t)
		    ប្រសិនបើ (*t == '\n')
			*t = 'n';
		    បើមិនដូច្នេះ ប្រសិនបើ (*t == '\r')
			*t = 'r';
		ប្រសិនបើ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_key,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ត្រឡប់ FAIL;
		}
		vim_free(p);
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ចំនួនគត់ sign = ' ';

		ប្រសិនបើ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ប្រសិនបើ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_key, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	}
    }
    ត្រឡប់ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    static ចំនួនគត់
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ចំនួនគត់		only_save_windows = TRUE;
    ចំនួនគត់		nr;
    ចំនួនគត់		restore_size = TRUE;
    ចំនួនគត់		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = NULL;
    ចំនួនគត់		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    ចំនួនគត់		cur_arg_idx = 0;
    ចំនួនគត់		next_arg_idx = 0;
    ចំនួនគត់		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ប្រសិនបើ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ប្រសិនបើ (put_line(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	goto fail;

    ប្រសិនបើ (put_line(fd, "doautoall SessionLoadPre") == FAIL)
	goto fail;

    ប្រសិនបើ (put_line(fd, "var save_splitbelow: bool") == FAIL
	    || put_line(fd, "var save_splitright: bool") == FAIL
	    || put_line(fd, "var save_winminheight: number") == FAIL
	    || put_line(fd, "var save_winminwidth: number") == FAIL
	    || put_line(fd, "var wipebuf: number = -1") == FAIL
	    || put_line(fd, "var shortmess_save: string") == FAIL)
	goto fail;

    ប្រសិនបើ (ssop_flags & SSOP_GLOBALS)
	ប្រសិនបើ (store_session_globals(fd) == FAIL)
	    goto fail;
# endif

    // Close all windows and tabs but one.
    ប្រសិនបើ (put_line(fd, "silent only") == FAIL)
	goto fail;
    ប្រសិនបើ ((ssop_flags & SSOP_TABPAGES)
	    && put_line(fd, "silent tabonly") == FAIL)
	goto fail;

    // Now a :cd command to the session directory or the current directory
    ប្រសិនបើ (ssop_flags & SSOP_SESDIR)
    {
	ប្រសិនបើ (put_line(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    goto fail;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(NULL, globaldir != NULL ? globaldir : dirnow);
	ប្រសិនបើ (sname == NULL
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    goto fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ប្រសិនបើ (put_line(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	goto fail;
    ប្រសិនបើ (put_line(fd, "  wipebuf = bufnr('%')") == FAIL)
	goto fail;
    ប្រសិនបើ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Save 'shortmess' if not storing options.
    ប្រសិនបើ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_line(fd, "shortmess_save = &shortmess") == FAIL)
	goto fail;

    // Set 'shortmess' for the following.
    ប្រសិនបើ (put_line(fd, "set shortmess+=aoO") == FAIL)
	goto fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ប្រសិនបើ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != NULL
		&& buf->b_p_bl)
	{
	    ប្រសិនបើ (fprintf(fd, "badd +%ld ", buf->b_wininfo == NULL ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		goto fail;
	}
    }

    // the global argument list
    ប្រសិនបើ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	goto fail;

    ប្រសិនបើ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ប្រសិនបើ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }

# ifdef FEAT_GUI
    ប្រសិនបើ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	ចំនួនគត់	x, y;

	ប្រសិនបើ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ប្រសិនបើ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		goto fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ប្រសិនបើ (p_stal == 1 && first_tabpage->tp_next != NULL)
    {
	ប្រសិនបើ (put_line(fd, "set stal=2") == FAIL)
	    goto fail;
	restore_stal = TRUE;
    }

    ប្រសិនបើ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ប្រសិនបើ (tp->tp_next != NULL
		  && put_line(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		goto fail;
	ប្រសិនបើ (first_tabpage->tp_next != NULL && put_line(fd, "tabrewind") == FAIL)
	    goto fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ចំនួនគត់	need_tabnext = FALSE;
	ចំនួនគត់	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ប្រសិនបើ ((ssop_flags & SSOP_TABPAGES))
	{
	    ប្រសិនបើ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    បើមិនដូច្នេះ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ប្រសិនបើ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	បើមិនដូច្នេះ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	សម្រាប់ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ប្រសិនបើ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != NULL
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ប្រសិនបើ (need_tabnext && put_line(fd, "tabnext") == FAIL)
		    goto fail;
		need_tabnext = FALSE;

		ប្រសិនបើ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    goto fail;
		ប្រសិនបើ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		bha_nga;
	    }
	}

	// If no file got edited create an empty tab page.
	ប្រសិនបើ (need_tabnext && put_line(fd, "tabnext") == FAIL)
	    goto fail;

	ប្រសិនបើ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ប្រសិនបើ (put_line(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_line(fd, "save_splitright = &splitright")
								       == FAIL)
		goto fail;
	    ប្រសិនបើ (put_line(fd, "set splitbelow splitright") == FAIL)
		goto fail;
	    ប្រសិនបើ (ses_win_rec(fd, tab_topframe) == FAIL)
		goto fail;
	    ប្រសិនបើ (put_line(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_line(fd, "&splitright = save_splitright")
								       == FAIL)
		goto fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	សម្រាប់ (wp = tab_firstwin; wp != NULL; wp = W_NEXT(wp))
	{
	    ប្រសិនបើ (ses_do_win(wp))
		++nr;
	    បើមិនដូច្នេះ
		restore_size = FALSE;
	    ប្រសិនបើ (curwin == wp)
		cnr = nr;
	}

	ប្រសិនបើ (tab_firstwin->w_next != NULL)
	{
	    // Go to the first window.
	    ប្រសិនបើ (put_line(fd, "wincmd t") == FAIL)
		goto fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ប្រសិនបើ (!restore_height_width
		    && (put_line(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_line(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		goto fail;
	    ប្រសិនបើ (put_line(fd, "set winminheight=0") == FAIL
		    || put_line(fd, "set winheight=1") == FAIL
		    || put_line(fd, "set winminwidth=0") == FAIL
		    || put_line(fd, "set winwidth=1") == FAIL)
		goto fail;
	    restore_height_width = TRUE;
	}
	ប្រសិនបើ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ប្រសិនបើ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != NULL)
	{
	    ប្រសិនបើ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		goto fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	សម្រាប់ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ប្រសិនបើ (!ses_do_win(wp))
		continue;
	    ប្រសិនបើ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# បើមិនដូច្នេះ
							 NULL
# endif
		 ) == FAIL)
		goto fail;
	    ប្រសិនបើ (nr > 1 && put_line(fd, "wincmd w") == FAIL)
		goto fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ប្រសិនបើ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    goto fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ប្រសិនបើ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ប្រសិនបើ (!(ssop_flags & SSOP_TABPAGES))
	    bha_nga;
    }

    ប្រសិនបើ (ssop_flags & SSOP_TABPAGES)
    {
	ប្រសិនបើ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }
    ប្រសិនបើ (restore_stal && put_line(fd, "set stal=1") == FAIL)
	goto fail;

    // Wipe out an empty unnamed buffer we started in.
    ប្រសិនបើ (put_line(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	goto fail;
    ប្រសិនបើ (put_line(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	goto fail;
    ប្រសិនបើ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ប្រសិនបើ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	goto fail;

    // Restore 'shortmess'.
    ប្រសិនបើ (ssop_flags & SSOP_OPTIONS)
    {
	ប្រសិនបើ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    goto fail;
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (put_line(fd, "&shortmess = shortmess_save") == FAIL)
	    goto fail;
    }

    ប្រសិនបើ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ប្រសិនបើ (put_line(fd, "&winminheight = save_winminheight") == FAIL
	      || put_line(fd, "&winminwidth = save_winminwidth") == FAIL)
	    goto fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ប្រសិនបើ (put_line(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_line(fd, "if filereadable(sx)") == FAIL
	    || put_line(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_line(fd, "endif") == FAIL)
	goto fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ត្រឡប់ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    static char_u *
get_view_file(ចំនួនគត់ c)
{
    ចំនួនគត់		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ប្រសិនបើ (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	ត្រឡប់ NULL;
    }
    sname = home_replace_save(NULL, curbuf->b_ffname);
    ប្រសិនបើ (sname == NULL)
	ត្រឡប់ NULL;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    សម្រាប់ (p = sname; *p; ++p)
	ប្រសិនបើ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ប្រសិនបើ (retval != NULL)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	សម្រាប់ (p = sname; *p; ++p)
	{
	    ប្រសិនបើ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ប្រសិនបើ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ប្រសិនបើ (*p == ':')
		    *s++ = '-';
		បើមិនដូច្នេះ
# endif
		    *s++ = '+';
	    }
	    បើមិនដូច្នេះ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ត្រឡប់ retval;
}

/*
 * ":loadview [nr]"
 */
    void
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ប្រសិនបើ (fname == NULL)
	ត្រឡប់;

    (void)do_source(fname, FALSE, DOSO_NONE, NULL);
    vim_free(fname);
}

# ប្រសិនបើ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ចំនួនគត់
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    var_na	    *mksession_cmdline;
    unsigned ចំនួនគត់    save_ssop_flags;
    ចំនួនគត់		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ប្រសិនបើ (escaped_filename == NULL)
	ត្រឡប់ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ប្រសិនបើ (mksession_cmdline == NULL)
    {
	vim_free(escaped_filename);
	ត្រឡប់ FALSE;
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
    ប្រសិនបើ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == NULL
	       || put_line(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_line(fd, "unlet g:Save_VV_this_session") == FAIL);

	ប្រសិនបើ (fd != NULL && fclose(fd) != 0)
	    failed = TRUE;

	ប្រសិនបើ (failed)
	    mch_remove(filename);
    }

    ត្រឡប់ !failed;
}
# endif

#endif // FEAT_SESSION

#ប្រសិនបើ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
static ចំនួនគត់ mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    void
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ចំនួនគត់		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = NULL;
#endif
#ifdef FEAT_SESSION
    ចំនួនគត់		view_session = FALSE;
    ចំនួនគត់		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = NULL;
    unsigned	*flagp;
#endif
#ប្រសិនបើ defined(FEAT_EVAL)
    ចំនួនគត់		sid;
    scriptitem_T *si = NULL;
#endif

    ប្រសិនបើ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#បើមិនដូច្នេះ
	ex_ni(eap);
	ត្រឡប់;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ប្រសិនបើ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ប្រសិនបើ (fname == NULL)
	    ត្រឡប់;
	viewFile = fname;
	using_vdir = TRUE;
    }
    បើមិនដូច្នេះ
#endif
	ប្រសិនបើ (*eap->arg != NUL)
	fname = eap->arg;
    បើមិនដូច្នេះ ប្រសិនបើ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    បើមិនដូច្នេះ ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    បើមិនដូច្នេះ
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ប្រសិនបើ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", NULL,
		(char_u *)_(BROWSE_FILTER_MACROS), NULL);
	ប្រសិនបើ (browseFile == NULL)
	    goto theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ប្រសិនបើ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ប្រសិនបើ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ប្រសិនបើ (fd != NULL)
    {
#ifdef FEAT_SESSION
	ប្រសិនបើ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	បើមិនដូច្នេះ
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ប្រសិនបើ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ប្រសិនបើ (put_line(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ប្រសិនបើ (eap->cmdidx == CMD_mkvimrc)
	    (void)put_line(fd, "version 6.0");

#ifdef FEAT_SESSION
	ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	{
	    ប្រសិនបើ (put_line(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ប្រសិនបើ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ប្រសិនបើ (p_cp)
		(void)put_line(fd, "if !&cp | set cp | endif");
	    បើមិនដូច្នេះ
		(void)put_line(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ប្រសិនបើ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    ចំនួនគត់ flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_line(fd, "var cpo_save: string") == FAIL;

	    ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	    {
		ប្រសិនបើ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ប្រសិនបើ (do_mappings)
		failed |= (makemap(fd, NULL) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ប្រសិនបើ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    សម្រាប់ (sid = 1; sid <= script_items.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ប្រសិនបើ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ចំនួនគត់ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    សម្រាប់ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ប្រសិនបើ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    continue;

			// compare prefixes if available
			ប្រសិនបើ (si->sn_autoload_prefix != NULL && b_si->sn_autoload_prefix != NULL
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    bha_nga;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ប្រសិនបើ (STRCMP(name, b_name) == 0)
			    bha_nga;
		    }

		    // import the auto script if there are no conflicts
		    ប្រសិនបើ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ប្រសិនបើ (!failed && view_session)
	{
	    ប្រសិនបើ (put_line(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ប្រសិនបើ (dirnow == NULL)
		    failed = TRUE;
		បើមិនដូច្នេះ
		{
		    // Change to session file's dir.
		    ប្រសិនបើ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((var_na *)dirnow) != 0)
			*dirnow = NUL;
		    ប្រសិនបើ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ប្រសិនបើ (vim_chdirfile(fname, NULL) == OK)
			    shorten_fnames(TRUE);
		    }
		    បើមិនដូច្នេះ ប្រសិនបើ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != NULL)
		    {
			ប្រសិនបើ (mch_chdir((var_na *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ប្រសិនបើ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != NULL)))
		    {
			ប្រសិនបើ (mch_chdir((var_na *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    បើមិនដូច្នេះ
	    {
		failed |= put_line(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								NULL) == FAIL);
	    }
	    ប្រសិនបើ (put_line(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	    ប្រសិនបើ (no_hlsearch && put_line(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ប្រសិនបើ (put_line(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	    {
		ប្រសិនបើ (put_line(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ប្រសិនបើ (put_line(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ប្រសិនបើ (failed)
	    emsg(_(e_error_while_writing));
#ប្រសិនបើ defined(FEAT_SESSION)
	បើមិនដូច្នេះ ប្រសិនបើ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ប្រសិនបើ (tbuf != NULL)
	    {
		ប្រសិនបើ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, NULL, NULL, FALSE, curbuf);
}

#ប្រសិនបើ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ប្រសិនបើ (ASCII_ISUPPER(*p))
    {
	ខណៈពេល (*(++p))
	    ប្រសិនបើ (ASCII_ISLOWER(*p))
		ត្រឡប់ VAR_FLAVOUR_SESSION;
	ត្រឡប់ VAR_FLAVOUR_VIMINFO;
    }
    បើមិនដូច្នេះ
	ត្រឡប់ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ចំនួនគត់
put_eol(FILE *fd)
{
    ប្រសិនបើ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ចំនួនគត់
put_line(FILE *fd, var_na *s)
{
    ប្រសិនបើ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}
