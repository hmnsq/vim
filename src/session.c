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

#ຖ້າ defined(FEAT_SESSION)

static ຈຳນວນເຕັມ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    static ຈຳນວນເຕັມ
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    ຈຳນວນເຕັມ		retval = OK;

    sname = home_replace_save(NULL, name);
    ຖ້າ (sname == NULL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	ສຳລັບ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ຖ້າ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ຖ້າ (p == NULL)
	ສົ່ງຄືນ FAIL;

    // write the result
    ຖ້າ (fputs((var_na *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ສົ່ງຄືນ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    static ຈຳນວນເຕັມ
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, ຈຳນວນເຕັມ add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ຖ້າ (buf->b_sfname != NULL
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ບໍ່ດັ່ງນັ້ນ
	name = buf->b_ffname;
    ຖ້າ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    static ຈຳນວນເຕັມ
ses_arglist(
    FILE	*fd,
    var_na	*cmd,
    garray_T	*gap,
    ຈຳນວນເຕັມ		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    ຈຳນວນເຕັມ		i;
    char_u	*buf = NULL;
    char_u	*s;

    ຖ້າ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (put_line(fd, ":%argdel") == FAIL)
	ສົ່ງຄືນ FAIL;
    ສຳລັບ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ຖ້າ (s != NULL)
	{
	    ຖ້າ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ຖ້າ (buf != NULL)
		{
		    (void)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ຖ້າ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ສົ່ງຄືນ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ສົ່ງຄືນ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    static ຈຳນວນເຕັມ
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ຖ້າ (bt_terminal(wp->w_buffer))
	ສົ່ງຄືນ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ຖ້າ (wp->w_buffer->b_fname == NULL
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ສົ່ງຄືນ (ssop_flags & SSOP_BLANK);
    ຖ້າ (bt_help(wp->w_buffer))
	ສົ່ງຄືນ (ssop_flags & SSOP_HELP);
    ສົ່ງຄືນ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    static ຈຳນວນເຕັມ
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ຖ້າ (fr->fr_layout == FR_LEAF)
	ສົ່ງຄືນ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ຖ້າ (ses_do_frame(frc))
	    ສົ່ງຄືນ TRUE;
    ສົ່ງຄືນ FALSE;
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
	ຖ້າ (ses_do_frame(frc))
	    bha_nga;
    ສົ່ງຄືນ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    static ຈຳນວນເຕັມ
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    ຈຳນວນເຕັມ		count = 0;

    ຖ້າ (fr->fr_layout == FR_LEAF)
	ສົ່ງຄືນ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ຖ້າ (frc != NULL)
	ໃນຂະນະທີ່ ((frc = ses_skipframe(frc->fr_next)) != NULL)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ຖ້າ (put_line(fd, "wincmd _ | wincmd |") == FAIL
		    || put_line(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ສົ່ງຄືນ FAIL;
	    ++count;
	}

    // Go back to the first window.
    ຖ້າ (count > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", count) < 0
		|| put_eol(fd) == FAIL))
	ສົ່ງຄືນ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ໃນຂະນະທີ່ (frc != NULL)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ຖ້າ (frc != NULL && put_line(fd, "wincmd w") == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
ses_winsizes(
    FILE	*fd,
    ຈຳນວນເຕັມ		restore_size,
    win_T	*tab_firstwin)
{
    ຈຳນວນເຕັມ		n = 0;
    win_T	*wp;

    ຖ້າ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	ສຳລັບ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ຖ້າ (!ses_do_win(wp))
		continue;
	    ++n;

	    // restore height when not full height
	    ຖ້າ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ສົ່ງຄືນ FAIL;

	    // restore width when not full width
	    ຖ້າ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ສົ່ງຄືນ FAIL;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Just equalise window sizes
	ຖ້າ (put_line(fd, "wincmd =") == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
put_view_curpos(FILE *fd, win_T *wp, var_na *spaces)
{
    ຈຳນວນເຕັມ r;

    ຖ້າ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ບໍ່ດັ່ງນັ້ນ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ສົ່ງຄືນ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    static ຈຳນວນເຕັມ
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ຈຳນວນເຕັມ		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    ຈຳນວນເຕັມ		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ຈຳນວນເຕັມ		f;
    ຈຳນວນເຕັມ		do_cursor;
    ຈຳນວນເຕັມ		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ຖ້າ (wp->w_alist == &global_alist)
    {
	ຖ້າ (put_line(fd, "argglobal") == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != NULL
			|| wp->w_localdir != NULL, flagp) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ຖ້າ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ຖ້າ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ສົ່ງຄືນ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ຖ້າ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ຖ້າ (bt_help(wp->w_buffer))
	{
	    var_na *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ຖ້າ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (var_na *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ຖ້າ (put_line(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ສົ່ງຄືນ FAIL;
	}
# ifdef FEAT_TERMINAL
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (bt_terminal(wp->w_buffer))
	{
	    ຖ້າ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ສົ່ງຄືນ FAIL;
	}
# endif
	// Load the file.
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (wp->w_buffer->b_ffname != NULL
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ຖ້າ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ສົ່ງຄືນ FAIL;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // No file in this buffer, just make it empty.
	    ຖ້າ (put_line(fd, "enew") == FAIL)
		ສົ່ງຄືນ FAIL;
# ifdef FEAT_QUICKFIX
	    ຖ້າ (wp->w_buffer->b_ffname != NULL)
	    {
		// The buffer does have a name, but it's not a file name.
		ຖ້າ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ສົ່ງຄືນ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ຖ້າ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ຖ້າ ((flagp == &ssop_flags)
		&& alt != NULL
		&& alt->b_fname != NULL
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ສົ່ງຄືນ FAIL;
    }

    // Local mappings and abbreviations.
    ຖ້າ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ສົ່ງຄືນ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ຖ້າ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ບໍ່ດັ່ງນັ້ນ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ຖ້າ (f == FAIL)
	ສົ່ງຄືນ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ຖ້າ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != NULL
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ຖ້າ (put_folds(fd, wp) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ຖ້າ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ຖ້າ (put_line(fd, "{") == FAIL)
	    ສົ່ງຄືນ FAIL;

	ຖ້າ (wp->w_height <= 0)
	{
	    ຖ້າ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ສົ່ງຄືນ FAIL;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ສົ່ງຄືນ FAIL;

	ຖ້າ (put_eol(fd) == FAIL
		|| put_line(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_line(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_line(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ສົ່ງຄືນ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ຖ້າ (wp->w_cursor.col == 0)
	{
	    ຖ້າ (put_line(fd, "  normal! 0") == FAIL)
		ສົ່ງຄືນ FAIL;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ຖ້າ (fprintf(fd,
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
		    ສົ່ງຄືນ FAIL;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (put_view_curpos(fd, wp, "  ") == FAIL)
		ສົ່ງຄືນ FAIL;
	}

	ຖ້າ (put_line(fd, "}") == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ຖ້າ (wp->w_localdir != NULL
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ຖ້າ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ສົ່ງຄືນ FAIL;
	did_lcd = TRUE;
    }

    ສົ່ງຄືນ OK;
}

# ifdef FEAT_EVAL
    static ຈຳນວນເຕັມ
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ຈຳນວນເຕັມ		todo;
    char_u	*p, *t;

    todo = (ຈຳນວນເຕັມ)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	ຖ້າ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ຖ້າ ((this_var->di_tv.v_type == VAR_NUMBER
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ຖ້າ (p == NULL)	    // out of memory
		    bha_nga;
		ສຳລັບ (t = p; *t != NUL; ++t)
		    ຖ້າ (*t == '\n')
			*t = 'n';
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*t == '\r')
			*t = 'r';
		ຖ້າ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_key,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ສົ່ງຄືນ FAIL;
		}
		vim_free(p);
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ຈຳນວນເຕັມ sign = ' ';

		ຖ້າ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ຖ້າ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_key, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ສົ່ງຄືນ FAIL;
	    }
	}
    }
    ສົ່ງຄືນ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    static ຈຳນວນເຕັມ
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ຈຳນວນເຕັມ		only_save_windows = TRUE;
    ຈຳນວນເຕັມ		nr;
    ຈຳນວນເຕັມ		restore_size = TRUE;
    ຈຳນວນເຕັມ		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = NULL;
    ຈຳນວນເຕັມ		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    ຈຳນວນເຕັມ		cur_arg_idx = 0;
    ຈຳນວນເຕັມ		next_arg_idx = 0;
    ຈຳນວນເຕັມ		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ຖ້າ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ຖ້າ (put_line(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	goto fail;

    ຖ້າ (put_line(fd, "doautoall SessionLoadPre") == FAIL)
	goto fail;

    ຖ້າ (put_line(fd, "var save_splitbelow: bool") == FAIL
	    || put_line(fd, "var save_splitright: bool") == FAIL
	    || put_line(fd, "var save_winminheight: number") == FAIL
	    || put_line(fd, "var save_winminwidth: number") == FAIL
	    || put_line(fd, "var wipebuf: number = -1") == FAIL
	    || put_line(fd, "var shortmess_save: string") == FAIL)
	goto fail;

    ຖ້າ (ssop_flags & SSOP_GLOBALS)
	ຖ້າ (store_session_globals(fd) == FAIL)
	    goto fail;
# endif

    // Close all windows and tabs but one.
    ຖ້າ (put_line(fd, "silent only") == FAIL)
	goto fail;
    ຖ້າ ((ssop_flags & SSOP_TABPAGES)
	    && put_line(fd, "silent tabonly") == FAIL)
	goto fail;

    // Now a :cd command to the session directory or the current directory
    ຖ້າ (ssop_flags & SSOP_SESDIR)
    {
	ຖ້າ (put_line(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    goto fail;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(NULL, globaldir != NULL ? globaldir : dirnow);
	ຖ້າ (sname == NULL
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
    ຖ້າ (put_line(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	goto fail;
    ຖ້າ (put_line(fd, "  wipebuf = bufnr('%')") == FAIL)
	goto fail;
    ຖ້າ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Save 'shortmess' if not storing options.
    ຖ້າ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_line(fd, "shortmess_save = &shortmess") == FAIL)
	goto fail;

    // Set 'shortmess' for the following.
    ຖ້າ (put_line(fd, "set shortmess+=aoO") == FAIL)
	goto fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ຖ້າ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != NULL
		&& buf->b_p_bl)
	{
	    ຖ້າ (fprintf(fd, "badd +%ld ", buf->b_wininfo == NULL ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		goto fail;
	}
    }

    // the global argument list
    ຖ້າ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	goto fail;

    ຖ້າ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ຖ້າ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }

# ifdef FEAT_GUI
    ຖ້າ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	ຈຳນວນເຕັມ	x, y;

	ຖ້າ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ຖ້າ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		goto fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ຖ້າ (p_stal == 1 && first_tabpage->tp_next != NULL)
    {
	ຖ້າ (put_line(fd, "set stal=2") == FAIL)
	    goto fail;
	restore_stal = TRUE;
    }

    ຖ້າ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ຖ້າ (tp->tp_next != NULL
		  && put_line(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		goto fail;
	ຖ້າ (first_tabpage->tp_next != NULL && put_line(fd, "tabrewind") == FAIL)
	    goto fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ຈຳນວນເຕັມ	need_tabnext = FALSE;
	ຈຳນວນເຕັມ	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ຖ້າ ((ssop_flags & SSOP_TABPAGES))
	{
	    ຖ້າ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ຖ້າ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	ສຳລັບ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ຖ້າ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != NULL
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ຖ້າ (need_tabnext && put_line(fd, "tabnext") == FAIL)
		    goto fail;
		need_tabnext = FALSE;

		ຖ້າ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    goto fail;
		ຖ້າ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		bha_nga;
	    }
	}

	// If no file got edited create an empty tab page.
	ຖ້າ (need_tabnext && put_line(fd, "tabnext") == FAIL)
	    goto fail;

	ຖ້າ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ຖ້າ (put_line(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_line(fd, "save_splitright = &splitright")
								       == FAIL)
		goto fail;
	    ຖ້າ (put_line(fd, "set splitbelow splitright") == FAIL)
		goto fail;
	    ຖ້າ (ses_win_rec(fd, tab_topframe) == FAIL)
		goto fail;
	    ຖ້າ (put_line(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_line(fd, "&splitright = save_splitright")
								       == FAIL)
		goto fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	ສຳລັບ (wp = tab_firstwin; wp != NULL; wp = W_NEXT(wp))
	{
	    ຖ້າ (ses_do_win(wp))
		++nr;
	    ບໍ່ດັ່ງນັ້ນ
		restore_size = FALSE;
	    ຖ້າ (curwin == wp)
		cnr = nr;
	}

	ຖ້າ (tab_firstwin->w_next != NULL)
	{
	    // Go to the first window.
	    ຖ້າ (put_line(fd, "wincmd t") == FAIL)
		goto fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ຖ້າ (!restore_height_width
		    && (put_line(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_line(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		goto fail;
	    ຖ້າ (put_line(fd, "set winminheight=0") == FAIL
		    || put_line(fd, "set winheight=1") == FAIL
		    || put_line(fd, "set winminwidth=0") == FAIL
		    || put_line(fd, "set winwidth=1") == FAIL)
		goto fail;
	    restore_height_width = TRUE;
	}
	ຖ້າ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ຖ້າ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != NULL)
	{
	    ຖ້າ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		goto fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	ສຳລັບ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ຖ້າ (!ses_do_win(wp))
		continue;
	    ຖ້າ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ບໍ່ດັ່ງນັ້ນ
							 NULL
# endif
		 ) == FAIL)
		goto fail;
	    ຖ້າ (nr > 1 && put_line(fd, "wincmd w") == FAIL)
		goto fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ຖ້າ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    goto fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ຖ້າ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ຖ້າ (!(ssop_flags & SSOP_TABPAGES))
	    bha_nga;
    }

    ຖ້າ (ssop_flags & SSOP_TABPAGES)
    {
	ຖ້າ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }
    ຖ້າ (restore_stal && put_line(fd, "set stal=1") == FAIL)
	goto fail;

    // Wipe out an empty unnamed buffer we started in.
    ຖ້າ (put_line(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	goto fail;
    ຖ້າ (put_line(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	goto fail;
    ຖ້າ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ຖ້າ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	goto fail;

    // Restore 'shortmess'.
    ຖ້າ (ssop_flags & SSOP_OPTIONS)
    {
	ຖ້າ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    goto fail;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (put_line(fd, "&shortmess = shortmess_save") == FAIL)
	    goto fail;
    }

    ຖ້າ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ຖ້າ (put_line(fd, "&winminheight = save_winminheight") == FAIL
	      || put_line(fd, "&winminwidth = save_winminwidth") == FAIL)
	    goto fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ຖ້າ (put_line(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_line(fd, "if filereadable(sx)") == FAIL
	    || put_line(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_line(fd, "endif") == FAIL)
	goto fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ສົ່ງຄືນ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    static char_u *
get_view_file(ຈຳນວນເຕັມ c)
{
    ຈຳນວນເຕັມ		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ຖ້າ (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	ສົ່ງຄືນ NULL;
    }
    sname = home_replace_save(NULL, curbuf->b_ffname);
    ຖ້າ (sname == NULL)
	ສົ່ງຄືນ NULL;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    ສຳລັບ (p = sname; *p; ++p)
	ຖ້າ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ຖ້າ (retval != NULL)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	ສຳລັບ (p = sname; *p; ++p)
	{
	    ຖ້າ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ຖ້າ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ຖ້າ (*p == ':')
		    *s++ = '-';
		ບໍ່ດັ່ງນັ້ນ
# endif
		    *s++ = '+';
	    }
	    ບໍ່ດັ່ງນັ້ນ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ສົ່ງຄືນ retval;
}

/*
 * ":loadview [nr]"
 */
    void
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ຖ້າ (fname == NULL)
	ສົ່ງຄືນ;

    (void)do_source(fname, FALSE, DOSO_NONE, NULL);
    vim_free(fname);
}

# ຖ້າ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ຈຳນວນເຕັມ
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    var_na	    *mksession_cmdline;
    unsigned ຈຳນວນເຕັມ    save_ssop_flags;
    ຈຳນວນເຕັມ		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ຖ້າ (escaped_filename == NULL)
	ສົ່ງຄືນ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ຖ້າ (mksession_cmdline == NULL)
    {
	vim_free(escaped_filename);
	ສົ່ງຄືນ FALSE;
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
    ຖ້າ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == NULL
	       || put_line(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_line(fd, "unlet g:Save_VV_this_session") == FAIL);

	ຖ້າ (fd != NULL && fclose(fd) != 0)
	    failed = TRUE;

	ຖ້າ (failed)
	    mch_remove(filename);
    }

    ສົ່ງຄືນ !failed;
}
# endif

#endif // FEAT_SESSION

#ຖ້າ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
static ຈຳນວນເຕັມ mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    void
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ຈຳນວນເຕັມ		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = NULL;
#endif
#ifdef FEAT_SESSION
    ຈຳນວນເຕັມ		view_session = FALSE;
    ຈຳນວນເຕັມ		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = NULL;
    unsigned	*flagp;
#endif
#ຖ້າ defined(FEAT_EVAL)
    ຈຳນວນເຕັມ		sid;
    scriptitem_T *si = NULL;
#endif

    ຖ້າ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#ບໍ່ດັ່ງນັ້ນ
	ex_ni(eap);
	ສົ່ງຄືນ;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ຖ້າ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ຖ້າ (fname == NULL)
	    ສົ່ງຄືນ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
	ຖ້າ (*eap->arg != NUL)
	fname = eap->arg;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    ບໍ່ດັ່ງນັ້ນ
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ຖ້າ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", NULL,
		(char_u *)_(BROWSE_FILTER_MACROS), NULL);
	ຖ້າ (browseFile == NULL)
	    goto theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ຖ້າ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ຖ້າ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ຖ້າ (fd != NULL)
    {
#ifdef FEAT_SESSION
	ຖ້າ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ບໍ່ດັ່ງນັ້ນ
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ຖ້າ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ຖ້າ (put_line(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ຖ້າ (eap->cmdidx == CMD_mkvimrc)
	    (void)put_line(fd, "version 6.0");

#ifdef FEAT_SESSION
	ຖ້າ (eap->cmdidx == CMD_mksession)
	{
	    ຖ້າ (put_line(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ຖ້າ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ຖ້າ (p_cp)
		(void)put_line(fd, "if !&cp | set cp | endif");
	    ບໍ່ດັ່ງນັ້ນ
		(void)put_line(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ຖ້າ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    ຈຳນວນເຕັມ flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_line(fd, "var cpo_save: string") == FAIL;

	    ຖ້າ (eap->cmdidx == CMD_mksession)
	    {
		ຖ້າ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ຖ້າ (do_mappings)
		failed |= (makemap(fd, NULL) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ຖ້າ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    ສຳລັບ (sid = 1; sid <= script_items.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ຖ້າ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ຈຳນວນເຕັມ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    ສຳລັບ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ຖ້າ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    continue;

			// compare prefixes if available
			ຖ້າ (si->sn_autoload_prefix != NULL && b_si->sn_autoload_prefix != NULL
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    bha_nga;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ຖ້າ (STRCMP(name, b_name) == 0)
			    bha_nga;
		    }

		    // import the auto script if there are no conflicts
		    ຖ້າ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ຖ້າ (!failed && view_session)
	{
	    ຖ້າ (put_line(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ຖ້າ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ຖ້າ (dirnow == NULL)
		    failed = TRUE;
		ບໍ່ດັ່ງນັ້ນ
		{
		    // Change to session file's dir.
		    ຖ້າ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((var_na *)dirnow) != 0)
			*dirnow = NUL;
		    ຖ້າ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ຖ້າ (vim_chdirfile(fname, NULL) == OK)
			    shorten_fnames(TRUE);
		    }
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != NULL)
		    {
			ຖ້າ (mch_chdir((var_na *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ຖ້າ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != NULL)))
		    {
			ຖ້າ (mch_chdir((var_na *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		failed |= put_line(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								NULL) == FAIL);
	    }
	    ຖ້າ (put_line(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	    ຖ້າ (no_hlsearch && put_line(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ຖ້າ (put_line(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ຖ້າ (eap->cmdidx == CMD_mksession)
	    {
		ຖ້າ (put_line(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ຖ້າ (put_line(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ຖ້າ (failed)
	    emsg(_(e_error_while_writing));
#ຖ້າ defined(FEAT_SESSION)
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ຖ້າ (tbuf != NULL)
	    {
		ຖ້າ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

#ຖ້າ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ຖ້າ (ASCII_ISUPPER(*p))
    {
	ໃນຂະນະທີ່ (*(++p))
	    ຖ້າ (ASCII_ISLOWER(*p))
		ສົ່ງຄືນ VAR_FLAVOUR_SESSION;
	ສົ່ງຄືນ VAR_FLAVOUR_VIMINFO;
    }
    ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ຈຳນວນເຕັມ
put_eol(FILE *fd)
{
    ຖ້າ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ຈຳນວນເຕັມ
put_line(FILE *fd, var_na *s)
{
    ຖ້າ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}
