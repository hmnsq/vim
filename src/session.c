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

#ᬬᬾᬦ᭄ defined(FEAT_SESSION)

static puur_naa_nka did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    static puur_naa_nka
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    puur_naa_nka		retval = OK;

    sname = home_replace_save(NULL, name);
    ᬬᬾᬦ᭄ (sname == NULL)
	ᬯᬮᬶ FAIL;

    ᬬᬾᬦ᭄ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	for (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ᬬᬾᬦ᭄ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ᬬᬾᬦ᭄ (p == NULL)
	ᬯᬮᬶ FAIL;

    // write the result
    ᬬᬾᬦ᭄ (fputs((var_na *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ᬯᬮᬶ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    static puur_naa_nka
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, puur_naa_nka add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ᬬᬾᬦ᭄ (buf->b_sfname != NULL
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ᬮᬶᬬ
	name = buf->b_ffname;
    ᬬᬾᬦ᭄ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	ᬯᬮᬶ FAIL;
    ᬯᬮᬶ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    static puur_naa_nka
ses_arglist(
    FILE	*fd,
    var_na	*cmd,
    garray_T	*gap,
    puur_naa_nka		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    puur_naa_nka		i;
    char_u	*buf = NULL;
    char_u	*s;

    ᬬᬾᬦ᭄ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ᬯᬮᬶ FAIL;
    ᬬᬾᬦ᭄ (put_line(fd, ":%argdel") == FAIL)
	ᬯᬮᬶ FAIL;
    for (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ᬬᬾᬦ᭄ (s != NULL)
	{
	    ᬬᬾᬦ᭄ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ᬬᬾᬦ᭄ (buf != NULL)
		{
		    (void)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ᬬᬾᬦ᭄ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ᬯᬮᬶ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ᬯᬮᬶ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    static puur_naa_nka
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ᬬᬾᬦ᭄ (bt_terminal(wp->w_buffer))
	ᬯᬮᬶ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ᬬᬾᬦ᭄ (wp->w_buffer->b_fname == NULL
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	ᬯᬮᬶ (ssop_flags & SSOP_BLANK);
    ᬬᬾᬦ᭄ (bt_help(wp->w_buffer))
	ᬯᬮᬶ (ssop_flags & SSOP_HELP);
    ᬯᬮᬶ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    static puur_naa_nka
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ᬬᬾᬦ᭄ (fr->fr_layout == FR_LEAF)
	ᬯᬮᬶ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ᬬᬾᬦ᭄ (ses_do_frame(frc))
	    ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
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
	ᬬᬾᬦ᭄ (ses_do_frame(frc))
	    bha_nga;
    ᬯᬮᬶ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    static puur_naa_nka
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    puur_naa_nka		count = 0;

    ᬬᬾᬦ᭄ (fr->fr_layout == FR_LEAF)
	ᬯᬮᬶ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ᬬᬾᬦ᭄ (frc != NULL)
	ᬲᭂᬤᭂᬮ ((frc = ses_skipframe(frc->fr_next)) != NULL)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ᬬᬾᬦ᭄ (put_line(fd, "wincmd _ | wincmd |") == FAIL
		    || put_line(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		ᬯᬮᬶ FAIL;
	    ++count;
	}

    // Go back to the first window.
    ᬬᬾᬦ᭄ (count > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", count) < 0
		|| put_eol(fd) == FAIL))
	ᬯᬮᬶ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ᬲᭂᬤᭂᬮ (frc != NULL)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ᬬᬾᬦ᭄ (frc != NULL && put_line(fd, "wincmd w") == FAIL)
	    ᬯᬮᬶ FAIL;
    }

    ᬯᬮᬶ OK;
}

    static puur_naa_nka
ses_winsizes(
    FILE	*fd,
    puur_naa_nka		restore_size,
    win_T	*tab_firstwin)
{
    puur_naa_nka		n = 0;
    win_T	*wp;

    ᬬᬾᬦ᭄ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	for (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ᬬᬾᬦ᭄ (!ses_do_win(wp))
		continue;
	    ++n;

	    // restore height when not full height
	    ᬬᬾᬦ᭄ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		ᬯᬮᬶ FAIL;

	    // restore width when not full width
	    ᬬᬾᬦ᭄ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ᬯᬮᬶ FAIL;
	}
    }
    ᬮᬶᬬ
    {
	// Just equalise window sizes
	ᬬᬾᬦ᭄ (put_line(fd, "wincmd =") == FAIL)
	    ᬯᬮᬶ FAIL;
    }
    ᬯᬮᬶ OK;
}

    static puur_naa_nka
put_view_curpos(FILE *fd, win_T *wp, var_na *spaces)
{
    puur_naa_nka r;

    ᬬᬾᬦ᭄ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ᬮᬶᬬ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ᬯᬮᬶ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    static puur_naa_nka
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    puur_naa_nka		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    puur_naa_nka		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    puur_naa_nka		f;
    puur_naa_nka		do_cursor;
    puur_naa_nka		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ᬬᬾᬦ᭄ (wp->w_alist == &global_alist)
    {
	ᬬᬾᬦ᭄ (put_line(fd, "argglobal") == FAIL)
	    ᬯᬮᬶ FAIL;
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != NULL
			|| wp->w_localdir != NULL, flagp) == FAIL)
	    ᬯᬮᬶ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ᬬᬾᬦ᭄ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ᬬᬾᬦ᭄ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ᬯᬮᬶ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ᬬᬾᬦ᭄ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ᬬᬾᬦ᭄ (bt_help(wp->w_buffer))
	{
	    var_na *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ᬬᬾᬦ᭄ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (var_na *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ᬬᬾᬦ᭄ (put_line(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ᬯᬮᬶ FAIL;
	}
# ifdef FEAT_TERMINAL
	ᬮᬶᬬ ᬬᬾᬦ᭄ (bt_terminal(wp->w_buffer))
	{
	    ᬬᬾᬦ᭄ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		ᬯᬮᬶ FAIL;
	}
# endif
	// Load the file.
	ᬮᬶᬬ ᬬᬾᬦ᭄ (wp->w_buffer->b_ffname != NULL
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ᬬᬾᬦ᭄ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ᬯᬮᬶ FAIL;
	}
	ᬮᬶᬬ
	{
	    // No file in this buffer, just make it empty.
	    ᬬᬾᬦ᭄ (put_line(fd, "enew") == FAIL)
		ᬯᬮᬶ FAIL;
# ifdef FEAT_QUICKFIX
	    ᬬᬾᬦ᭄ (wp->w_buffer->b_ffname != NULL)
	    {
		// The buffer does have a name, but it's not a file name.
		ᬬᬾᬦ᭄ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ᬬᬾᬦ᭄ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ᬬᬾᬦ᭄ ((flagp == &ssop_flags)
		&& alt != NULL
		&& alt->b_fname != NULL
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ᬯᬮᬶ FAIL;
    }

    // Local mappings and abbreviations.
    ᬬᬾᬦ᭄ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	ᬯᬮᬶ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ᬬᬾᬦ᭄ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ᬮᬶᬬ ᬬᬾᬦ᭄ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ᬮᬶᬬ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ᬬᬾᬦ᭄ (f == FAIL)
	ᬯᬮᬶ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ᬬᬾᬦ᭄ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != NULL
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ᬬᬾᬦ᭄ (put_folds(fd, wp) == FAIL)
	    ᬯᬮᬶ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ᬬᬾᬦ᭄ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ᬬᬾᬦ᭄ (put_line(fd, "{") == FAIL)
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (wp->w_height <= 0)
	{
	    ᬬᬾᬦ᭄ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		ᬯᬮᬶ FAIL;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (put_eol(fd) == FAIL
		|| put_line(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_line(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_line(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ᬯᬮᬶ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ᬬᬾᬦ᭄ (wp->w_cursor.col == 0)
	{
	    ᬬᬾᬦ᭄ (put_line(fd, "  normal! 0") == FAIL)
		ᬯᬮᬶ FAIL;
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ᬬᬾᬦ᭄ (fprintf(fd,
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
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (put_view_curpos(fd, wp, "  ") == FAIL)
		ᬯᬮᬶ FAIL;
	}

	ᬬᬾᬦ᭄ (put_line(fd, "}") == FAIL)
	    ᬯᬮᬶ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ᬬᬾᬦ᭄ (wp->w_localdir != NULL
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ᬬᬾᬦ᭄ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ᬯᬮᬶ FAIL;
	did_lcd = TRUE;
    }

    ᬯᬮᬶ OK;
}

# ifdef FEAT_EVAL
    static puur_naa_nka
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    puur_naa_nka		todo;
    char_u	*p, *t;

    todo = (puur_naa_nka)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ᬬᬾᬦ᭄ ((this_var->di_tv.v_type == VAR_NUMBER
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ᬬᬾᬦ᭄ (p == NULL)	    // out of memory
		    bha_nga;
		for (t = p; *t != NUL; ++t)
		    ᬬᬾᬦ᭄ (*t == '\n')
			*t = 'n';
		    ᬮᬶᬬ ᬬᬾᬦ᭄ (*t == '\r')
			*t = 'r';
		ᬬᬾᬦ᭄ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_key,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ᬯᬮᬶ FAIL;
		}
		vim_free(p);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		puur_naa_nka sign = ' ';

		ᬬᬾᬦ᭄ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ᬬᬾᬦ᭄ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_key, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	}
    }
    ᬯᬮᬶ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    static puur_naa_nka
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    puur_naa_nka		only_save_windows = TRUE;
    puur_naa_nka		nr;
    puur_naa_nka		restore_size = TRUE;
    puur_naa_nka		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = NULL;
    puur_naa_nka		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    puur_naa_nka		cur_arg_idx = 0;
    puur_naa_nka		next_arg_idx = 0;
    puur_naa_nka		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ᬬᬾᬦ᭄ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ᬬᬾᬦ᭄ (put_line(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	goto fail;

    ᬬᬾᬦ᭄ (put_line(fd, "doautoall SessionLoadPre") == FAIL)
	goto fail;

    ᬬᬾᬦ᭄ (put_line(fd, "var save_splitbelow: bool") == FAIL
	    || put_line(fd, "var save_splitright: bool") == FAIL
	    || put_line(fd, "var save_winminheight: number") == FAIL
	    || put_line(fd, "var save_winminwidth: number") == FAIL
	    || put_line(fd, "var wipebuf: number = -1") == FAIL
	    || put_line(fd, "var shortmess_save: string") == FAIL)
	goto fail;

    ᬬᬾᬦ᭄ (ssop_flags & SSOP_GLOBALS)
	ᬬᬾᬦ᭄ (store_session_globals(fd) == FAIL)
	    goto fail;
# endif

    // Close all windows and tabs but one.
    ᬬᬾᬦ᭄ (put_line(fd, "silent only") == FAIL)
	goto fail;
    ᬬᬾᬦ᭄ ((ssop_flags & SSOP_TABPAGES)
	    && put_line(fd, "silent tabonly") == FAIL)
	goto fail;

    // Now a :cd command to the session directory or the current directory
    ᬬᬾᬦ᭄ (ssop_flags & SSOP_SESDIR)
    {
	ᬬᬾᬦ᭄ (put_line(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    goto fail;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(NULL, globaldir != NULL ? globaldir : dirnow);
	ᬬᬾᬦ᭄ (sname == NULL
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
    ᬬᬾᬦ᭄ (put_line(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	goto fail;
    ᬬᬾᬦ᭄ (put_line(fd, "  wipebuf = bufnr('%')") == FAIL)
	goto fail;
    ᬬᬾᬦ᭄ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Save 'shortmess' if not storing options.
    ᬬᬾᬦ᭄ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_line(fd, "shortmess_save = &shortmess") == FAIL)
	goto fail;

    // Set 'shortmess' for the following.
    ᬬᬾᬦ᭄ (put_line(fd, "set shortmess+=aoO") == FAIL)
	goto fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ᬬᬾᬦ᭄ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != NULL
		&& buf->b_p_bl)
	{
	    ᬬᬾᬦ᭄ (fprintf(fd, "badd +%ld ", buf->b_wininfo == NULL ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		goto fail;
	}
    }

    // the global argument list
    ᬬᬾᬦ᭄ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	goto fail;

    ᬬᬾᬦ᭄ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ᬬᬾᬦ᭄ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }

# ifdef FEAT_GUI
    ᬬᬾᬦ᭄ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	puur_naa_nka	x, y;

	ᬬᬾᬦ᭄ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ᬬᬾᬦ᭄ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		goto fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ᬬᬾᬦ᭄ (p_stal == 1 && first_tabpage->tp_next != NULL)
    {
	ᬬᬾᬦ᭄ (put_line(fd, "set stal=2") == FAIL)
	    goto fail;
	restore_stal = TRUE;
    }

    ᬬᬾᬦ᭄ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ᬬᬾᬦ᭄ (tp->tp_next != NULL
		  && put_line(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		goto fail;
	ᬬᬾᬦ᭄ (first_tabpage->tp_next != NULL && put_line(fd, "tabrewind") == FAIL)
	    goto fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	puur_naa_nka	need_tabnext = FALSE;
	puur_naa_nka	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ᬬᬾᬦ᭄ ((ssop_flags & SSOP_TABPAGES))
	{
	    ᬬᬾᬦ᭄ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ᬮᬶᬬ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ᬬᬾᬦ᭄ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	ᬮᬶᬬ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	for (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ᬬᬾᬦ᭄ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != NULL
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ᬬᬾᬦ᭄ (need_tabnext && put_line(fd, "tabnext") == FAIL)
		    goto fail;
		need_tabnext = FALSE;

		ᬬᬾᬦ᭄ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    goto fail;
		ᬬᬾᬦ᭄ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		bha_nga;
	    }
	}

	// If no file got edited create an empty tab page.
	ᬬᬾᬦ᭄ (need_tabnext && put_line(fd, "tabnext") == FAIL)
	    goto fail;

	ᬬᬾᬦ᭄ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ᬬᬾᬦ᭄ (put_line(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_line(fd, "save_splitright = &splitright")
								       == FAIL)
		goto fail;
	    ᬬᬾᬦ᭄ (put_line(fd, "set splitbelow splitright") == FAIL)
		goto fail;
	    ᬬᬾᬦ᭄ (ses_win_rec(fd, tab_topframe) == FAIL)
		goto fail;
	    ᬬᬾᬦ᭄ (put_line(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_line(fd, "&splitright = save_splitright")
								       == FAIL)
		goto fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	for (wp = tab_firstwin; wp != NULL; wp = W_NEXT(wp))
	{
	    ᬬᬾᬦ᭄ (ses_do_win(wp))
		++nr;
	    ᬮᬶᬬ
		restore_size = FALSE;
	    ᬬᬾᬦ᭄ (curwin == wp)
		cnr = nr;
	}

	ᬬᬾᬦ᭄ (tab_firstwin->w_next != NULL)
	{
	    // Go to the first window.
	    ᬬᬾᬦ᭄ (put_line(fd, "wincmd t") == FAIL)
		goto fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ᬬᬾᬦ᭄ (!restore_height_width
		    && (put_line(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_line(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		goto fail;
	    ᬬᬾᬦ᭄ (put_line(fd, "set winminheight=0") == FAIL
		    || put_line(fd, "set winheight=1") == FAIL
		    || put_line(fd, "set winminwidth=0") == FAIL
		    || put_line(fd, "set winwidth=1") == FAIL)
		goto fail;
	    restore_height_width = TRUE;
	}
	ᬬᬾᬦ᭄ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ᬬᬾᬦ᭄ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != NULL)
	{
	    ᬬᬾᬦ᭄ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		goto fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	for (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ᬬᬾᬦ᭄ (!ses_do_win(wp))
		continue;
	    ᬬᬾᬦ᭄ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ᬮᬶᬬ
							 NULL
# endif
		 ) == FAIL)
		goto fail;
	    ᬬᬾᬦ᭄ (nr > 1 && put_line(fd, "wincmd w") == FAIL)
		goto fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ᬬᬾᬦ᭄ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    goto fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ᬬᬾᬦ᭄ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ᬬᬾᬦ᭄ (!(ssop_flags & SSOP_TABPAGES))
	    bha_nga;
    }

    ᬬᬾᬦ᭄ (ssop_flags & SSOP_TABPAGES)
    {
	ᬬᬾᬦ᭄ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }
    ᬬᬾᬦ᭄ (restore_stal && put_line(fd, "set stal=1") == FAIL)
	goto fail;

    // Wipe out an empty unnamed buffer we started in.
    ᬬᬾᬦ᭄ (put_line(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	goto fail;
    ᬬᬾᬦ᭄ (put_line(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	goto fail;
    ᬬᬾᬦ᭄ (put_line(fd, "endif") == FAIL)
	goto fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ᬬᬾᬦ᭄ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	goto fail;

    // Restore 'shortmess'.
    ᬬᬾᬦ᭄ (ssop_flags & SSOP_OPTIONS)
    {
	ᬬᬾᬦ᭄ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    goto fail;
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (put_line(fd, "&shortmess = shortmess_save") == FAIL)
	    goto fail;
    }

    ᬬᬾᬦ᭄ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ᬬᬾᬦ᭄ (put_line(fd, "&winminheight = save_winminheight") == FAIL
	      || put_line(fd, "&winminwidth = save_winminwidth") == FAIL)
	    goto fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ᬬᬾᬦ᭄ (put_line(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_line(fd, "if filereadable(sx)") == FAIL
	    || put_line(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_line(fd, "endif") == FAIL)
	goto fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ᬯᬮᬶ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    static char_u *
get_view_file(puur_naa_nka c)
{
    puur_naa_nka		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ᬬᬾᬦ᭄ (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	ᬯᬮᬶ NULL;
    }
    sname = home_replace_save(NULL, curbuf->b_ffname);
    ᬬᬾᬦ᭄ (sname == NULL)
	ᬯᬮᬶ NULL;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    for (p = sname; *p; ++p)
	ᬬᬾᬦ᭄ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ᬬᬾᬦ᭄ (retval != NULL)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	for (p = sname; *p; ++p)
	{
	    ᬬᬾᬦ᭄ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ᬬᬾᬦ᭄ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ᬬᬾᬦ᭄ (*p == ':')
		    *s++ = '-';
		ᬮᬶᬬ
# endif
		    *s++ = '+';
	    }
	    ᬮᬶᬬ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ᬯᬮᬶ retval;
}

/*
 * ":loadview [nr]"
 */
    void
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ᬬᬾᬦ᭄ (fname == NULL)
	ᬯᬮᬶ;

    (void)do_source(fname, FALSE, DOSO_NONE, NULL);
    vim_free(fname);
}

# ᬬᬾᬦ᭄ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    puur_naa_nka
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    var_na	    *mksession_cmdline;
    unsigned puur_naa_nka    save_ssop_flags;
    puur_naa_nka		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ᬬᬾᬦ᭄ (escaped_filename == NULL)
	ᬯᬮᬶ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ᬬᬾᬦ᭄ (mksession_cmdline == NULL)
    {
	vim_free(escaped_filename);
	ᬯᬮᬶ FALSE;
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
    ᬬᬾᬦ᭄ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == NULL
	       || put_line(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_line(fd, "unlet g:Save_VV_this_session") == FAIL);

	ᬬᬾᬦ᭄ (fd != NULL && fclose(fd) != 0)
	    failed = TRUE;

	ᬬᬾᬦ᭄ (failed)
	    mch_remove(filename);
    }

    ᬯᬮᬶ !failed;
}
# endif

#endif // FEAT_SESSION

#ᬬᬾᬦ᭄ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
static puur_naa_nka mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    void
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    puur_naa_nka		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = NULL;
#endif
#ifdef FEAT_SESSION
    puur_naa_nka		view_session = FALSE;
    puur_naa_nka		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = NULL;
    unsigned	*flagp;
#endif
#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
    puur_naa_nka		sid;
    scriptitem_T *si = NULL;
#endif

    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#ᬮᬶᬬ
	ex_ni(eap);
	ᬯᬮᬶ;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ᬬᬾᬦ᭄ (fname == NULL)
	    ᬯᬮᬶ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ᬮᬶᬬ
#endif
	ᬬᬾᬦ᭄ (*eap->arg != NUL)
	fname = eap->arg;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    ᬮᬶᬬ ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    ᬮᬶᬬ
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ᬬᬾᬦ᭄ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", NULL,
		(char_u *)_(BROWSE_FILTER_MACROS), NULL);
	ᬬᬾᬦ᭄ (browseFile == NULL)
	    goto theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ᬬᬾᬦ᭄ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ᬬᬾᬦ᭄ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ᬬᬾᬦ᭄ (fd != NULL)
    {
#ifdef FEAT_SESSION
	ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ᬮᬶᬬ
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ᬬᬾᬦ᭄ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ᬬᬾᬦ᭄ (put_line(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mkvimrc)
	    (void)put_line(fd, "version 6.0");

#ifdef FEAT_SESSION
	ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	{
	    ᬬᬾᬦ᭄ (put_line(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ᬬᬾᬦ᭄ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ᬬᬾᬦ᭄ (p_cp)
		(void)put_line(fd, "if !&cp | set cp | endif");
	    ᬮᬶᬬ
		(void)put_line(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ᬬᬾᬦ᭄ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    puur_naa_nka flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_line(fd, "var cpo_save: string") == FAIL;

	    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	    {
		ᬬᬾᬦ᭄ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ᬬᬾᬦ᭄ (do_mappings)
		failed |= (makemap(fd, NULL) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    for (sid = 1; sid <= script_items.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ᬬᬾᬦ᭄ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    puur_naa_nka b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    for (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ᬬᬾᬦ᭄ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    continue;

			// compare prefixes if available
			ᬬᬾᬦ᭄ (si->sn_autoload_prefix != NULL && b_si->sn_autoload_prefix != NULL
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    bha_nga;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ᬬᬾᬦ᭄ (STRCMP(name, b_name) == 0)
			    bha_nga;
		    }

		    // import the auto script if there are no conflicts
		    ᬬᬾᬦ᭄ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ᬬᬾᬦ᭄ (!failed && view_session)
	{
	    ᬬᬾᬦ᭄ (put_line(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ᬬᬾᬦ᭄ (dirnow == NULL)
		    failed = TRUE;
		ᬮᬶᬬ
		{
		    // Change to session file's dir.
		    ᬬᬾᬦ᭄ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((var_na *)dirnow) != 0)
			*dirnow = NUL;
		    ᬬᬾᬦ᭄ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ᬬᬾᬦ᭄ (vim_chdirfile(fname, NULL) == OK)
			    shorten_fnames(TRUE);
		    }
		    ᬮᬶᬬ ᬬᬾᬦ᭄ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != NULL)
		    {
			ᬬᬾᬦ᭄ (mch_chdir((var_na *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ᬬᬾᬦ᭄ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != NULL)))
		    {
			ᬬᬾᬦ᭄ (mch_chdir((var_na *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ᬮᬶᬬ
	    {
		failed |= put_line(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								NULL) == FAIL);
	    }
	    ᬬᬾᬦ᭄ (put_line(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	    ᬬᬾᬦ᭄ (no_hlsearch && put_line(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ᬬᬾᬦ᭄ (put_line(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	    {
		ᬬᬾᬦ᭄ (put_line(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ᬬᬾᬦ᭄ (put_line(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ᬬᬾᬦ᭄ (failed)
	    emsg(_(e_error_while_writing));
#ᬬᬾᬦ᭄ defined(FEAT_SESSION)
	ᬮᬶᬬ ᬬᬾᬦ᭄ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ᬬᬾᬦ᭄ (tbuf != NULL)
	    {
		ᬬᬾᬦ᭄ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

#ᬬᬾᬦ᭄ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ᬬᬾᬦ᭄ (ASCII_ISUPPER(*p))
    {
	ᬲᭂᬤᭂᬮ (*(++p))
	    ᬬᬾᬦ᭄ (ASCII_ISLOWER(*p))
		ᬯᬮᬶ VAR_FLAVOUR_SESSION;
	ᬯᬮᬶ VAR_FLAVOUR_VIMINFO;
    }
    ᬮᬶᬬ
	ᬯᬮᬶ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    puur_naa_nka
put_eol(FILE *fd)
{
    ᬬᬾᬦ᭄ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	ᬯᬮᬶ FAIL;
    ᬯᬮᬶ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    puur_naa_nka
put_line(FILE *fd, var_na *s)
{
    ᬬᬾᬦ᭄ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ᬯᬮᬶ FAIL;
    ᬯᬮᬶ OK;
}
