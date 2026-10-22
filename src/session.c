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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_SESSION)

സ്ഥിതം പൂർണ്ണസംഖ്യ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		retval = OK;

    sname = home_replace_save(ശൂന്യ, name);
    എങ്കിൽ (sname == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	വേണ്ടി (p = sname; *p != NUL; MB_PTR_ADV(p))
	    എങ്കിൽ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;

    // write the result
    എങ്കിൽ (fputs((അക്ഷരം *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    തിരികെ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, പൂർണ്ണസംഖ്യ add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    എങ്കിൽ (buf->b_sfname != ശൂന്യ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    അല്ലെങ്കിൽ
	name = buf->b_ffname;
    എങ്കിൽ (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_arglist(
    FILE	*fd,
    അക്ഷരം	*cmd,
    garray_T	*gap,
    പൂർണ്ണസംഖ്യ		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*buf = ശൂന്യ;
    char_u	*s;

    എങ്കിൽ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (put_വരി(fd, ":%argdel") == FAIL)
	തിരികെ FAIL;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	എങ്കിൽ (s != ശൂന്യ)
	{
	    എങ്കിൽ (fullname)
	    {
		buf = alloc(MAXPATHL);
		എങ്കിൽ (buf != ശൂന്യ)
		{
		    (ശൂന്യം)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    എങ്കിൽ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		തിരികെ FAIL;
	    }
	    vim_free(buf);
	}
    }
    തിരികെ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    എങ്കിൽ (bt_terminal(wp->w_buffer))
	തിരികെ !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    എങ്കിൽ (wp->w_buffer->b_fname == ശൂന്യ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	തിരികെ (ssop_flags & SSOP_BLANK);
    എങ്കിൽ (bt_help(wp->w_buffer))
	തിരികെ (ssop_flags & SSOP_HELP);
    തിരികെ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    എങ്കിൽ (fr->fr_layout == FR_LEAF)
	തിരികെ ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	എങ്കിൽ (ses_do_frame(frc))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    സ്ഥിതം frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	എങ്കിൽ (ses_do_frame(frc))
	    നിർത്തുക;
    തിരികെ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    എങ്കിൽ (fr->fr_layout == FR_LEAF)
	തിരികെ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    എങ്കിൽ (frc != ശൂന്യ)
	ആകുമ്പോൾ ((frc = ses_skipframe(frc->fr_next)) != ശൂന്യ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    എങ്കിൽ (put_വരി(fd, "wincmd _ | wincmd |") == FAIL
		    || put_വരി(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		തിരികെ FAIL;
	    ++എണ്ണം;
	}

    // Go back to the first window.
    എങ്കിൽ (എണ്ണം > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", എണ്ണം) < 0
		|| put_eol(fd) == FAIL))
	തിരികെ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ആകുമ്പോൾ (frc != ശൂന്യ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	എങ്കിൽ (frc != ശൂന്യ && put_വരി(fd, "wincmd w") == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ses_winsizes(
    FILE	*fd,
    പൂർണ്ണസംഖ്യ		restore_size,
    win_T	*tab_firstwin)
{
    പൂർണ്ണസംഖ്യ		n = 0;
    win_T	*wp;

    എങ്കിൽ (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	വേണ്ടി (wp = tab_firstwin; wp != ശൂന്യ; wp = wp->w_next)
	{
	    എങ്കിൽ (!ses_do_win(wp))
		തുടരുക;
	    ++n;

	    // restore height when not full height
	    എങ്കിൽ (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		തിരികെ FAIL;

	    // restore width when not full width
	    എങ്കിൽ (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Just equalise window sizes
	എങ്കിൽ (put_വരി(fd, "wincmd =") == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_view_curpos(FILE *fd, win_T *wp, അക്ഷരം *spaces)
{
    പൂർണ്ണസംഖ്യ r;

    എങ്കിൽ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    അല്ലെങ്കിൽ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    തിരികെ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    പൂർണ്ണസംഖ്യ		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    പൂർണ്ണസംഖ്യ		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    പൂർണ്ണസംഖ്യ		f;
    പൂർണ്ണസംഖ്യ		do_cursor;
    പൂർണ്ണസംഖ്യ		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    എങ്കിൽ (wp->w_alist == &global_alist)
    {
	എങ്കിൽ (put_വരി(fd, "argglobal") == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ശൂന്യ
			|| wp->w_localdir != ശൂന്യ, flagp) == FAIL)
	    തിരികെ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    എങ്കിൽ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	എങ്കിൽ (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    തിരികെ FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    എങ്കിൽ (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	എങ്കിൽ (bt_help(wp->w_buffer))
	{
	    അക്ഷരം *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    എങ്കിൽ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (അക്ഷരം *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    എങ്കിൽ (put_വരി(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		തിരികെ FAIL;
	}
# ifdef FEAT_TERMINAL
	അല്ലെങ്കിൽ എങ്കിൽ (bt_terminal(wp->w_buffer))
	{
	    എങ്കിൽ (term_write_session(fd, wp, terminal_bufs) == FAIL)
		തിരികെ FAIL;
	}
# endif
	// Load the file.
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_buffer->b_ffname != ശൂന്യ
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    എങ്കിൽ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    // No file in this buffer, just make it empty.
	    എങ്കിൽ (put_വരി(fd, "enew") == FAIL)
		തിരികെ FAIL;
# ifdef FEAT_QUICKFIX
	    എങ്കിൽ (wp->w_buffer->b_ffname != ശൂന്യ)
	    {
		// The buffer does have a name, but it's not a file name.
		എങ്കിൽ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    തിരികെ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    എങ്കിൽ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	എങ്കിൽ ((flagp == &ssop_flags)
		&& alt != ശൂന്യ
		&& alt->b_fname != ശൂന്യ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    തിരികെ FAIL;
    }

    // Local mappings and abbreviations.
    എങ്കിൽ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	തിരികെ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    എങ്കിൽ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    അല്ലെങ്കിൽ എങ്കിൽ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    അല്ലെങ്കിൽ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    എങ്കിൽ (f == FAIL)
	തിരികെ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    എങ്കിൽ ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != ശൂന്യ
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	എങ്കിൽ (put_folds(fd, wp) == FAIL)
	    തിരികെ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    എങ്കിൽ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	എങ്കിൽ (put_വരി(fd, "{") == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (wp->w_height <= 0)
	{
	    എങ്കിൽ (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    തിരികെ FAIL;

	എങ്കിൽ (put_eol(fd) == FAIL
		|| put_വരി(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_വരി(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_വരി(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    തിരികെ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	എങ്കിൽ (wp->w_cursor.col == 0)
	{
	    എങ്കിൽ (put_വരി(fd, "  normal! 0") == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		എങ്കിൽ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_വരി(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_വരി(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_വരി(fd, "  endif") == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (put_view_curpos(fd, wp, "  ") == FAIL)
		തിരികെ FAIL;
	}

	എങ്കിൽ (put_വരി(fd, "}") == FAIL)
	    തിരികെ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    എങ്കിൽ (wp->w_localdir != ശൂന്യ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	എങ്കിൽ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    തിരികെ FAIL;
	did_lcd = TRUE;
    }

    തിരികെ OK;
}

# ifdef FEAT_EVAL
    സ്ഥിതം പൂർണ്ണസംഖ്യ
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    പൂർണ്ണസംഖ്യ		todo;
    char_u	*p, *t;

    todo = (പൂർണ്ണസംഖ്യ)gvht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(gvht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    എങ്കിൽ ((this_var->di_tv.v_type == VAR_സംഖ്യ
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_താക്കോൽ) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		എങ്കിൽ (p == ശൂന്യ)	    // out of memory
		    നിർത്തുക;
		വേണ്ടി (t = p; *t != NUL; ++t)
		    എങ്കിൽ (*t == '\n')
			*t = 'n';
		    അല്ലെങ്കിൽ എങ്കിൽ (*t == '\r')
			*t = 'r';
		എങ്കിൽ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_താക്കോൽ,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    തിരികെ FAIL;
		}
		vim_free(p);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_താക്കോൽ) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		പൂർണ്ണസംഖ്യ sign = ' ';

		എങ്കിൽ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		എങ്കിൽ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_താക്കോൽ, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    തിരികെ FAIL;
	    }
	}
    }
    തിരികെ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		only_save_windows = TRUE;
    പൂർണ്ണസംഖ്യ		nr;
    പൂർണ്ണസംഖ്യ		restore_size = TRUE;
    പൂർണ്ണസംഖ്യ		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    പൂർണ്ണസംഖ്യ		cur_arg_idx = 0;
    പൂർണ്ണസംഖ്യ		next_arg_idx = 0;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    എങ്കിൽ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    എങ്കിൽ (put_വരി(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	പോകുക fail;

    എങ്കിൽ (put_വരി(fd, "doautoall SessionLoadPre") == FAIL)
	പോകുക fail;

    എങ്കിൽ (put_വരി(fd, "var save_splitbelow: bool") == FAIL
	    || put_വരി(fd, "var save_splitright: bool") == FAIL
	    || put_വരി(fd, "var save_winminheight: number") == FAIL
	    || put_വരി(fd, "var save_winminwidth: number") == FAIL
	    || put_വരി(fd, "var wipebuf: number = -1") == FAIL
	    || put_വരി(fd, "var shortmess_save: string") == FAIL)
	പോകുക fail;

    എങ്കിൽ (ssop_flags & SSOP_GLOBALS)
	എങ്കിൽ (store_session_globals(fd) == FAIL)
	    പോകുക fail;
# endif

    // Close all windows and tabs but one.
    എങ്കിൽ (put_വരി(fd, "silent only") == FAIL)
	പോകുക fail;
    എങ്കിൽ ((ssop_flags & SSOP_TABPAGES)
	    && put_വരി(fd, "silent tabonly") == FAIL)
	പോകുക fail;

    // Now a :cd command to the session directory or the current directory
    എങ്കിൽ (ssop_flags & SSOP_SESDIR)
    {
	എങ്കിൽ (put_വരി(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    പോകുക fail;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ശൂന്യ, globaldir != ശൂന്യ ? globaldir : dirnow);
	എങ്കിൽ (sname == ശൂന്യ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    പോകുക fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    എങ്കിൽ (put_വരി(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	പോകുക fail;
    എങ്കിൽ (put_വരി(fd, "  wipebuf = bufnr('%')") == FAIL)
	പോകുക fail;
    എങ്കിൽ (put_വരി(fd, "endif") == FAIL)
	പോകുക fail;

    // Save 'shortmess' if not storing options.
    എങ്കിൽ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_വരി(fd, "shortmess_save = &shortmess") == FAIL)
	പോകുക fail;

    // Set 'shortmess' for the following.
    എങ്കിൽ (put_വരി(fd, "set shortmess+=aoO") == FAIL)
	പോകുക fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	എങ്കിൽ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ശൂന്യ
		&& buf->b_p_bl)
	{
	    എങ്കിൽ (fprintf(fd, "badd +%ld ", buf->b_wininfo == ശൂന്യ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		പോകുക fail;
	}
    }

    // the global argument list
    എങ്കിൽ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	പോകുക fail;

    എങ്കിൽ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	എങ്കിൽ (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    പോകുക fail;
    }

# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	പൂർണ്ണസംഖ്യ	x, y;

	എങ്കിൽ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    എങ്കിൽ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		പോകുക fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    എങ്കിൽ (p_stal == 1 && first_tabpage->tp_next != ശൂന്യ)
    {
	എങ്കിൽ (put_വരി(fd, "set stal=2") == FAIL)
	    പോകുക fail;
	restore_stal = TRUE;
    }

    എങ്കിൽ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    എങ്കിൽ (tp->tp_next != ശൂന്യ
		  && put_വരി(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		പോകുക fail;
	എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ && put_വരി(fd, "tabrewind") == FAIL)
	    പോകുക fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	പൂർണ്ണസംഖ്യ	need_tabnext = FALSE;
	പൂർണ്ണസംഖ്യ	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	എങ്കിൽ ((ssop_flags & SSOP_TABPAGES))
	{
	    എങ്കിൽ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    അല്ലെങ്കിൽ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    എങ്കിൽ (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	വേണ്ടി (wp = tab_firstwin; wp != ശൂന്യ; wp = wp->w_next)
	{
	    എങ്കിൽ (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != ശൂന്യ
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		എങ്കിൽ (need_tabnext && put_വരി(fd, "tabnext") == FAIL)
		    പോകുക fail;
		need_tabnext = FALSE;

		എങ്കിൽ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    പോകുക fail;
		എങ്കിൽ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		നിർത്തുക;
	    }
	}

	// If no file got edited create an empty tab page.
	എങ്കിൽ (need_tabnext && put_വരി(fd, "tabnext") == FAIL)
	    പോകുക fail;

	എങ്കിൽ (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    എങ്കിൽ (put_വരി(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_വരി(fd, "save_splitright = &splitright")
								       == FAIL)
		പോകുക fail;
	    എങ്കിൽ (put_വരി(fd, "set splitbelow splitright") == FAIL)
		പോകുക fail;
	    എങ്കിൽ (ses_win_rec(fd, tab_topframe) == FAIL)
		പോകുക fail;
	    എങ്കിൽ (put_വരി(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_വരി(fd, "&splitright = save_splitright")
								       == FAIL)
		പോകുക fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	വേണ്ടി (wp = tab_firstwin; wp != ശൂന്യ; wp = W_NEXT(wp))
	{
	    എങ്കിൽ (ses_do_win(wp))
		++nr;
	    അല്ലെങ്കിൽ
		restore_size = FALSE;
	    എങ്കിൽ (curwin == wp)
		cnr = nr;
	}

	എങ്കിൽ (tab_firstwin->w_next != ശൂന്യ)
	{
	    // Go to the first window.
	    എങ്കിൽ (put_വരി(fd, "wincmd t") == FAIL)
		പോകുക fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    എങ്കിൽ (!restore_height_width
		    && (put_വരി(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_വരി(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		പോകുക fail;
	    എങ്കിൽ (put_വരി(fd, "set winminheight=0") == FAIL
		    || put_വരി(fd, "set winheight=1") == FAIL
		    || put_വരി(fd, "set winminwidth=0") == FAIL
		    || put_വരി(fd, "set winwidth=1") == FAIL)
		പോകുക fail;
	    restore_height_width = TRUE;
	}
	എങ്കിൽ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    പോകുക fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	എങ്കിൽ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ശൂന്യ)
	{
	    എങ്കിൽ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		പോകുക fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	വേണ്ടി (wp = tab_firstwin; wp != ശൂന്യ; wp = wp->w_next)
	{
	    എങ്കിൽ (!ses_do_win(wp))
		തുടരുക;
	    എങ്കിൽ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# അല്ലെങ്കിൽ
							 ശൂന്യ
# endif
		 ) == FAIL)
		പോകുക fail;
	    എങ്കിൽ (nr > 1 && put_വരി(fd, "wincmd w") == FAIL)
		പോകുക fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	എങ്കിൽ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    പോകുക fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	എങ്കിൽ (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    പോകുക fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	എങ്കിൽ (!(ssop_flags & SSOP_TABPAGES))
	    നിർത്തുക;
    }

    എങ്കിൽ (ssop_flags & SSOP_TABPAGES)
    {
	എങ്കിൽ (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    പോകുക fail;
    }
    എങ്കിൽ (restore_stal && put_വരി(fd, "set stal=1") == FAIL)
	പോകുക fail;

    // Wipe out an empty unnamed buffer we started in.
    എങ്കിൽ (put_വരി(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	പോകുക fail;
    എങ്കിൽ (put_വരി(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	പോകുക fail;
    എങ്കിൽ (put_വരി(fd, "endif") == FAIL)
	പോകുക fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    എങ്കിൽ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	പോകുക fail;

    // Restore 'shortmess'.
    എങ്കിൽ (ssop_flags & SSOP_OPTIONS)
    {
	എങ്കിൽ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    പോകുക fail;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (put_വരി(fd, "&shortmess = shortmess_save") == FAIL)
	    പോകുക fail;
    }

    എങ്കിൽ (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	എങ്കിൽ (put_വരി(fd, "&winminheight = save_winminheight") == FAIL
	      || put_വരി(fd, "&winminwidth = save_winminwidth") == FAIL)
	    പോകുക fail;
    }

    // Lastly, execute the x.vim file if it exists.
    എങ്കിൽ (put_വരി(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_വരി(fd, "if filereadable(sx)") == FAIL
	    || put_വരി(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_വരി(fd, "endif") == FAIL)
	പോകുക fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    തിരികെ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    സ്ഥിതം char_u *
get_view_file(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    എങ്കിൽ (curbuf->b_ffname == ശൂന്യ)
    {
	emsg(_(e_no_file_name));
	തിരികെ ശൂന്യ;
    }
    sname = home_replace_save(ശൂന്യ, curbuf->b_ffname);
    എങ്കിൽ (sname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    വേണ്ടി (p = sname; *p; ++p)
	എങ്കിൽ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    എങ്കിൽ (retval != ശൂന്യ)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	വേണ്ടി (p = sname; *p; ++p)
	{
	    എങ്കിൽ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# എങ്കിൽ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		എങ്കിൽ (*p == ':')
		    *s++ = '-';
		അല്ലെങ്കിൽ
# endif
		    *s++ = '+';
	    }
	    അല്ലെങ്കിൽ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    തിരികെ retval;
}

/*
 * ":loadview [nr]"
 */
    ശൂന്യം
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    (ശൂന്യം)do_source(fname, FALSE, DOSO_NONE, ശൂന്യ);
    vim_free(fname);
}

# എങ്കിൽ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    പൂർണ്ണസംഖ്യ
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    അക്ഷരം	    *mksession_cmdline;
    unsigned പൂർണ്ണസംഖ്യ    save_ssop_flags;
    പൂർണ്ണസംഖ്യ		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    എങ്കിൽ (escaped_filename == ശൂന്യ)
	തിരികെ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    എങ്കിൽ (mksession_cmdline == ശൂന്യ)
    {
	vim_free(escaped_filename);
	തിരികെ FALSE;
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
    എങ്കിൽ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ശൂന്യ
	       || put_വരി(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_വരി(fd, "unlet g:Save_VV_this_session") == FAIL);

	എങ്കിൽ (fd != ശൂന്യ && fclose(fd) != 0)
	    failed = TRUE;

	എങ്കിൽ (failed)
	    mch_remove(filename);
    }

    തിരികെ !failed;
}
# endif

#endif // FEAT_SESSION

#എങ്കിൽ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
സ്ഥിതം പൂർണ്ണസംഖ്യ mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    ശൂന്യം
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    പൂർണ്ണസംഖ്യ		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = ശൂന്യ;
#endif
#ifdef FEAT_SESSION
    പൂർണ്ണസംഖ്യ		view_session = FALSE;
    പൂർണ്ണസംഖ്യ		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ശൂന്യ;
    unsigned	*flagp;
#endif
#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ		sid;
    scriptitem_T *si = ശൂന്യ;
#endif

    എങ്കിൽ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#അല്ലെങ്കിൽ
	ex_ni(eap);
	തിരികെ;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    എങ്കിൽ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	എങ്കിൽ (fname == ശൂന്യ)
	    തിരികെ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (*eap->arg != NUL)
	fname = eap->arg;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    അല്ലെങ്കിൽ
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ശൂന്യ,
		(char_u *)_(BROWSE_FILTER_MACROS), ശൂന്യ);
	എങ്കിൽ (browseFile == ശൂന്യ)
	    പോകുക theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#എങ്കിൽ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    എങ്കിൽ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    എങ്കിൽ (fd != ശൂന്യ)
    {
#ifdef FEAT_SESSION
	എങ്കിൽ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	അല്ലെങ്കിൽ
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	എങ്കിൽ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	എങ്കിൽ (put_വരി(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	എങ്കിൽ (eap->cmdidx == CMD_mkvimrc)
	    (ശൂന്യം)put_വരി(fd, "version 6.0");

#ifdef FEAT_SESSION
	എങ്കിൽ (eap->cmdidx == CMD_mksession)
	{
	    എങ്കിൽ (put_വരി(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	എങ്കിൽ (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    എങ്കിൽ (p_cp)
		(ശൂന്യം)put_വരി(fd, "if !&cp | set cp | endif");
	    അല്ലെങ്കിൽ
		(ശൂന്യം)put_വരി(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	എങ്കിൽ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    പൂർണ്ണസംഖ്യ flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_വരി(fd, "var cpo_save: string") == FAIL;

	    എങ്കിൽ (eap->cmdidx == CMD_mksession)
	    {
		എങ്കിൽ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    എങ്കിൽ (do_mappings)
		failed |= (makemap(fd, ശൂന്യ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#എങ്കിൽ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    വേണ്ടി (sid = 1; sid <= script_ഇനങ്ങൾ.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		എങ്കിൽ ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    പൂർണ്ണസംഖ്യ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    വേണ്ടി (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			എങ്കിൽ ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    തുടരുക;

			// compare prefixes if available
			എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ && b_si->sn_autoload_prefix != ശൂന്യ
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    നിർത്തുക;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			എങ്കിൽ (STRCMP(name, b_name) == 0)
			    നിർത്തുക;
		    }

		    // import the auto script if there are no conflicts
		    എങ്കിൽ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	എങ്കിൽ (!failed && view_session)
	{
	    എങ്കിൽ (put_വരി(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    എങ്കിൽ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		എങ്കിൽ (dirnow == ശൂന്യ)
		    failed = TRUE;
		അല്ലെങ്കിൽ
		{
		    // Change to session file's dir.
		    എങ്കിൽ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((അക്ഷരം *)dirnow) != 0)
			*dirnow = NUL;
		    എങ്കിൽ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			എങ്കിൽ (vim_chdirfile(fname, ശൂന്യ) == OK)
			    shorten_fnames(TRUE);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ശൂന്യ)
		    {
			എങ്കിൽ (mch_chdir((അക്ഷരം *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    എങ്കിൽ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ശൂന്യ)))
		    {
			എങ്കിൽ (mch_chdir((അക്ഷരം *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		failed |= put_വരി(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ശൂന്യ) == FAIL);
	    }
	    എങ്കിൽ (put_വരി(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_തിരയൽ_EXTRA
	    എങ്കിൽ (no_hlsearch && put_വരി(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    എങ്കിൽ (put_വരി(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    എങ്കിൽ (eap->cmdidx == CMD_mksession)
	    {
		എങ്കിൽ (put_വരി(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	എങ്കിൽ (put_വരി(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	എങ്കിൽ (failed)
	    emsg(_(e_error_while_writing));
#എങ്കിൽ defined(FEAT_SESSION)
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    എങ്കിൽ (tbuf != ശൂന്യ)
	    {
		എങ്കിൽ (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
}

#എങ്കിൽ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    എങ്കിൽ (ASCII_ISUPPER(*p))
    {
	ആകുമ്പോൾ (*(++p))
	    എങ്കിൽ (ASCII_ISLOWER(*p))
		തിരികെ VAR_FLAVOUR_SESSION;
	തിരികെ VAR_FLAVOUR_VIMINFO;
    }
    അല്ലെങ്കിൽ
	തിരികെ VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    പൂർണ്ണസംഖ്യ
put_eol(FILE *fd)
{
    എങ്കിൽ (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    പൂർണ്ണസംഖ്യ
put_വരി(FILE *fd, അക്ഷരം *s)
{
    എങ്കിൽ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	തിരികെ FAIL;
    തിരികെ OK;
}
