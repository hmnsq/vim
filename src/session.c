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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SESSION)

સ્થિર પૂર્ણાંક did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    સ્થિર પૂર્ણાંક
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    પૂર્ણાંક		retval = OK;

    sname = home_replace_save(ખાલી, name);
    જો (sname == ખાલી)
	પાછા FAIL;

    જો (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	માટે (p = sname; *p != NUL; MB_PTR_ADV(p))
	    જો (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    જો (p == ખાલી)
	પાછા FAIL;

    // write the result
    જો (fputs((અક્ષર *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    પાછા retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    સ્થિર પૂર્ણાંક
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, પૂર્ણાંક add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    જો (buf->b_sfname != ખાલી
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    નહીંતર
	name = buf->b_ffname;
    જો (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	પાછા FAIL;
    પાછા OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    સ્થિર પૂર્ણાંક
ses_arglist(
    FILE	*fd,
    અક્ષર	*cmd,
    garray_T	*gap,
    પૂર્ણાંક		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    પૂર્ણાંક		i;
    char_u	*buf = ખાલી;
    char_u	*s;

    જો (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	પાછા FAIL;
    જો (put_લીટી(fd, ":%argdel") == FAIL)
	પાછા FAIL;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	જો (s != ખાલી)
	{
	    જો (fullname)
	    {
		buf = alloc(MAXPATHL);
		જો (buf != ખાલી)
		{
		    (શૂન્ય)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    જો (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		પાછા FAIL;
	    }
	    vim_free(buf);
	}
    }
    પાછા OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    સ્થિર પૂર્ણાંક
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    જો (bt_terminal(wp->w_buffer))
	પાછા !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    જો (wp->w_buffer->b_fname == ખાલી
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	પાછા (ssop_flags & SSOP_BLANK);
    જો (bt_help(wp->w_buffer))
	પાછા (ssop_flags & SSOP_HELP);
    પાછા TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    સ્થિર પૂર્ણાંક
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    જો (fr->fr_layout == FR_LEAF)
	પાછા ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	જો (ses_do_frame(frc))
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    સ્થિર frame_T *
ses_skipframe(frame_T *fr)
{
    frame_T	*frc;

    FOR_ALL_FRAMES(frc, fr)
	જો (ses_do_frame(frc))
	    તોડો;
    પાછા frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    સ્થિર પૂર્ણાંક
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    પૂર્ણાંક		ગણતરી = 0;

    જો (fr->fr_layout == FR_LEAF)
	પાછા OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    જો (frc != ખાલી)
	જ્યારે ((frc = ses_skipframe(frc->fr_next)) != ખાલી)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    જો (put_લીટી(fd, "wincmd _ | wincmd |") == FAIL
		    || put_લીટી(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		પાછા FAIL;
	    ++ગણતરી;
	}

    // Go back to the first window.
    જો (ગણતરી > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", ગણતરી) < 0
		|| put_eol(fd) == FAIL))
	પાછા FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    જ્યારે (frc != ખાલી)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	જો (frc != ખાલી && put_લીટી(fd, "wincmd w") == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
ses_winsizes(
    FILE	*fd,
    પૂર્ણાંક		restore_size,
    win_T	*tab_firstwin)
{
    પૂર્ણાંક		n = 0;
    win_T	*wp;

    જો (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	માટે (wp = tab_firstwin; wp != ખાલી; wp = wp->w_next)
	{
	    જો (!ses_do_win(wp))
		ચાલુ;
	    ++n;

	    // restore height when not full height
	    જો (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		પાછા FAIL;

	    // restore width when not full width
	    જો (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		પાછા FAIL;
	}
    }
    નહીંતર
    {
	// Just equalise window sizes
	જો (put_લીટી(fd, "wincmd =") == FAIL)
	    પાછા FAIL;
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
put_view_curpos(FILE *fd, win_T *wp, અક્ષર *spaces)
{
    પૂર્ણાંક r;

    જો (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    નહીંતર
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    પાછા r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    સ્થિર પૂર્ણાંક
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    પૂર્ણાંક		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    પૂર્ણાંક		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    પૂર્ણાંક		f;
    પૂર્ણાંક		do_cursor;
    પૂર્ણાંક		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    જો (wp->w_alist == &global_alist)
    {
	જો (put_લીટી(fd, "argglobal") == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
    {
	જો (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ખાલી
			|| wp->w_localdir != ખાલી, flagp) == FAIL)
	    પાછા FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    જો (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	જો (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    પાછા FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    જો (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	જો (bt_help(wp->w_buffer))
	{
	    અક્ષર *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    જો (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (અક્ષર *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    જો (put_લીટી(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		પાછા FAIL;
	}
# ifdef FEAT_TERMINAL
	નહીંતર જો (bt_terminal(wp->w_buffer))
	{
	    જો (term_write_session(fd, wp, terminal_bufs) == FAIL)
		પાછા FAIL;
	}
# endif
	// Load the file.
	નહીંતર જો (wp->w_buffer->b_ffname != ખાલી
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    જો (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    // No file in this buffer, just make it empty.
	    જો (put_લીટી(fd, "enew") == FAIL)
		પાછા FAIL;
# ifdef FEAT_QUICKFIX
	    જો (wp->w_buffer->b_ffname != ખાલી)
	    {
		// The buffer does have a name, but it's not a file name.
		જો (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    પાછા FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    જો (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	જો ((flagp == &ssop_flags)
		&& alt != ખાલી
		&& alt->b_fname != ખાલી
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    પાછા FAIL;
    }

    // Local mappings and abbreviations.
    જો ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	પાછા FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    જો (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    નહીંતર જો (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    નહીંતર
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    જો (f == FAIL)
	પાછા FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    જો ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != ખાલી
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	જો (put_folds(fd, wp) == FAIL)
	    પાછા FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    જો (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	જો (put_લીટી(fd, "{") == FAIL)
	    પાછા FAIL;

	જો (wp->w_height <= 0)
	{
	    જો (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		પાછા FAIL;
	}
	નહીંતર જો (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    પાછા FAIL;

	જો (put_eol(fd) == FAIL
		|| put_લીટી(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_લીટી(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_લીટી(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    પાછા FAIL;
	// Restore the cursor column and left offset when not wrapping.
	જો (wp->w_cursor.col == 0)
	{
	    જો (put_લીટી(fd, "  normal! 0") == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    જો (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		જો (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (long)wp->w_virtcol + 1,
			    (long)(wp->w_virtcol - wp->w_leftcol),
			    (long)wp->w_width / 2, (long)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_લીટી(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (long)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_લીટી(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_લીટી(fd, "  endif") == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (put_view_curpos(fd, wp, "  ") == FAIL)
		પાછા FAIL;
	}

	જો (put_લીટી(fd, "}") == FAIL)
	    પાછા FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    જો (wp->w_localdir != ખાલી
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	જો (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    પાછા FAIL;
	did_lcd = TRUE;
    }

    પાછા OK;
}

# ifdef FEAT_EVAL
    સ્થિર પૂર્ણાંક
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    પૂર્ણાંક		todo;
    char_u	*p, *t;

    todo = (પૂર્ણાંક)gvht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(gvht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    જો ((this_var->di_tv.v_type == VAR_સંખ્યા
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_ચાવી) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		જો (p == ખાલી)	    // out of memory
		    તોડો;
		માટે (t = p; *t != NUL; ++t)
		    જો (*t == '\n')
			*t = 'n';
		    નહીંતર જો (*t == '\r')
			*t = 'r';
		જો ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_ચાવી,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    પાછા FAIL;
		}
		vim_free(p);
	    }
	    નહીંતર જો (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_ચાવી) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		પૂર્ણાંક sign = ' ';

		જો (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		જો ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_ચાવી, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    પાછા FAIL;
	    }
	}
    }
    પાછા OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    સ્થિર પૂર્ણાંક
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    પૂર્ણાંક		only_save_windows = TRUE;
    પૂર્ણાંક		nr;
    પૂર્ણાંક		restore_size = TRUE;
    પૂર્ણાંક		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ખાલી;
    પૂર્ણાંક		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    પૂર્ણાંક		cur_arg_idx = 0;
    પૂર્ણાંક		next_arg_idx = 0;
    પૂર્ણાંક		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    જો (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    જો (put_લીટી(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	જાઓ fail;

    જો (put_લીટી(fd, "doautoall SessionLoadPre") == FAIL)
	જાઓ fail;

    જો (put_લીટી(fd, "var save_splitbelow: bool") == FAIL
	    || put_લીટી(fd, "var save_splitright: bool") == FAIL
	    || put_લીટી(fd, "var save_winminheight: number") == FAIL
	    || put_લીટી(fd, "var save_winminwidth: number") == FAIL
	    || put_લીટી(fd, "var wipebuf: number = -1") == FAIL
	    || put_લીટી(fd, "var shortmess_save: string") == FAIL)
	જાઓ fail;

    જો (ssop_flags & SSOP_GLOBALS)
	જો (store_session_globals(fd) == FAIL)
	    જાઓ fail;
# endif

    // Close all windows and tabs but one.
    જો (put_લીટી(fd, "silent only") == FAIL)
	જાઓ fail;
    જો ((ssop_flags & SSOP_TABPAGES)
	    && put_લીટી(fd, "silent tabonly") == FAIL)
	જાઓ fail;

    // Now a :cd command to the session directory or the current directory
    જો (ssop_flags & SSOP_SESDIR)
    {
	જો (put_લીટી(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    જાઓ fail;
    }
    નહીંતર જો (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ખાલી, globaldir != ખાલી ? globaldir : dirnow);
	જો (sname == ખાલી
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    જાઓ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    જો (put_લીટી(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	જાઓ fail;
    જો (put_લીટી(fd, "  wipebuf = bufnr('%')") == FAIL)
	જાઓ fail;
    જો (put_લીટી(fd, "endif") == FAIL)
	જાઓ fail;

    // Save 'shortmess' if not storing options.
    જો ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_લીટી(fd, "shortmess_save = &shortmess") == FAIL)
	જાઓ fail;

    // Set 'shortmess' for the following.
    જો (put_લીટી(fd, "set shortmess+=aoO") == FAIL)
	જાઓ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	જો (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ખાલી
		&& buf->b_p_bl)
	{
	    જો (fprintf(fd, "badd +%ld ", buf->b_wininfo == ખાલી ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		જાઓ fail;
	}
    }

    // the global argument list
    જો (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	જાઓ fail;

    જો (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	જો (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    જાઓ fail;
    }

# ifdef FEAT_GUI
    જો (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	પૂર્ણાંક	x, y;

	જો (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    જો (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		જાઓ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    જો (p_stal == 1 && first_tabpage->tp_next != ખાલી)
    {
	જો (put_લીટી(fd, "set stal=2") == FAIL)
	    જાઓ fail;
	restore_stal = TRUE;
    }

    જો ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    જો (tp->tp_next != ખાલી
		  && put_લીટી(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		જાઓ fail;
	જો (first_tabpage->tp_next != ખાલી && put_લીટી(fd, "tabrewind") == FAIL)
	    જાઓ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	પૂર્ણાંક	need_tabnext = FALSE;
	પૂર્ણાંક	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	જો ((ssop_flags & SSOP_TABPAGES))
	{
	    જો (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    નહીંતર
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    જો (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	નહીંતર
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	માટે (wp = tab_firstwin; wp != ખાલી; wp = wp->w_next)
	{
	    જો (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != ખાલી
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		જો (need_tabnext && put_લીટી(fd, "tabnext") == FAIL)
		    જાઓ fail;
		need_tabnext = FALSE;

		જો (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    જાઓ fail;
		જો (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		તોડો;
	    }
	}

	// If no file got edited create an empty tab page.
	જો (need_tabnext && put_લીટી(fd, "tabnext") == FAIL)
	    જાઓ fail;

	જો (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    જો (put_લીટી(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_લીટી(fd, "save_splitright = &splitright")
								       == FAIL)
		જાઓ fail;
	    જો (put_લીટી(fd, "set splitbelow splitright") == FAIL)
		જાઓ fail;
	    જો (ses_win_rec(fd, tab_topframe) == FAIL)
		જાઓ fail;
	    જો (put_લીટી(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_લીટી(fd, "&splitright = save_splitright")
								       == FAIL)
		જાઓ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	માટે (wp = tab_firstwin; wp != ખાલી; wp = W_NEXT(wp))
	{
	    જો (ses_do_win(wp))
		++nr;
	    નહીંતર
		restore_size = FALSE;
	    જો (curwin == wp)
		cnr = nr;
	}

	જો (tab_firstwin->w_next != ખાલી)
	{
	    // Go to the first window.
	    જો (put_લીટી(fd, "wincmd t") == FAIL)
		જાઓ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    જો (!restore_height_width
		    && (put_લીટી(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_લીટી(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		જાઓ fail;
	    જો (put_લીટી(fd, "set winminheight=0") == FAIL
		    || put_લીટી(fd, "set winheight=1") == FAIL
		    || put_લીટી(fd, "set winminwidth=0") == FAIL
		    || put_લીટી(fd, "set winwidth=1") == FAIL)
		જાઓ fail;
	    restore_height_width = TRUE;
	}
	જો (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    જાઓ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	જો ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ખાલી)
	{
	    જો (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		જાઓ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	માટે (wp = tab_firstwin; wp != ખાલી; wp = wp->w_next)
	{
	    જો (!ses_do_win(wp))
		ચાલુ;
	    જો (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# નહીંતર
							 ખાલી
# endif
		 ) == FAIL)
		જાઓ fail;
	    જો (nr > 1 && put_લીટી(fd, "wincmd w") == FAIL)
		જાઓ fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	જો (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    જાઓ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	જો (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    જાઓ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	જો (!(ssop_flags & SSOP_TABPAGES))
	    તોડો;
    }

    જો (ssop_flags & SSOP_TABPAGES)
    {
	જો (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    જાઓ fail;
    }
    જો (restore_stal && put_લીટી(fd, "set stal=1") == FAIL)
	જાઓ fail;

    // Wipe out an empty unnamed buffer we started in.
    જો (put_લીટી(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	જાઓ fail;
    જો (put_લીટી(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	જાઓ fail;
    જો (put_લીટી(fd, "endif") == FAIL)
	જાઓ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    જો (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	જાઓ fail;

    // Restore 'shortmess'.
    જો (ssop_flags & SSOP_OPTIONS)
    {
	જો (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    જાઓ fail;
    }
    નહીંતર
    {
	જો (put_લીટી(fd, "&shortmess = shortmess_save") == FAIL)
	    જાઓ fail;
    }

    જો (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	જો (put_લીટી(fd, "&winminheight = save_winminheight") == FAIL
	      || put_લીટી(fd, "&winminwidth = save_winminwidth") == FAIL)
	    જાઓ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    જો (put_લીટી(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_લીટી(fd, "if filereadable(sx)") == FAIL
	    || put_લીટી(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_લીટી(fd, "endif") == FAIL)
	જાઓ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    પાછા ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    સ્થિર char_u *
get_view_file(પૂર્ણાંક c)
{
    પૂર્ણાંક		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    જો (curbuf->b_ffname == ખાલી)
    {
	emsg(_(e_no_file_name));
	પાછા ખાલી;
    }
    sname = home_replace_save(ખાલી, curbuf->b_ffname);
    જો (sname == ખાલી)
	પાછા ખાલી;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    માટે (p = sname; *p; ++p)
	જો (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    જો (retval != ખાલી)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	માટે (p = sname; *p; ++p)
	{
	    જો (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    નહીંતર જો (vim_ispathsep(*p))
	    {
		*s++ = '=';
# જો defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		જો (*p == ':')
		    *s++ = '-';
		નહીંતર
# endif
		    *s++ = '+';
	    }
	    નહીંતર
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    પાછા retval;
}

/*
 * ":loadview [nr]"
 */
    શૂન્ય
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    જો (fname == ખાલી)
	પાછા;

    (શૂન્ય)do_source(fname, FALSE, DOSO_NONE, ખાલી);
    vim_free(fname);
}

# જો defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    પૂર્ણાંક
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    અક્ષર	    *mksession_cmdline;
    unsigned પૂર્ણાંક    save_ssop_flags;
    પૂર્ણાંક		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    જો (escaped_filename == ખાલી)
	પાછા FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    જો (mksession_cmdline == ખાલી)
    {
	vim_free(escaped_filename);
	પાછા FALSE;
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
    જો (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ખાલી
	       || put_લીટી(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_લીટી(fd, "unlet g:Save_VV_this_session") == FAIL);

	જો (fd != ખાલી && fclose(fd) != 0)
	    failed = TRUE;

	જો (failed)
	    mch_remove(filename);
    }

    પાછા !failed;
}
# endif

#endif // FEAT_SESSION

#જો defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
સ્થિર પૂર્ણાંક mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    શૂન્ય
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    પૂર્ણાંક		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = ખાલી;
#endif
#ifdef FEAT_SESSION
    પૂર્ણાંક		view_session = FALSE;
    પૂર્ણાંક		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ખાલી;
    unsigned	*flagp;
#endif
#જો defined(FEAT_EVAL)
    પૂર્ણાંક		sid;
    scriptitem_T *si = ખાલી;
#endif

    જો (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#નહીંતર
	ex_ni(eap);
	પાછા;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    જો (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	જો (fname == ખાલી)
	    પાછા;
	viewFile = fname;
	using_vdir = TRUE;
    }
    નહીંતર
#endif
	જો (*eap->arg != NUL)
	fname = eap->arg;
    નહીંતર જો (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    નહીંતર જો (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    નહીંતર
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    જો (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ખાલી,
		(char_u *)_(BROWSE_FILTER_MACROS), ખાલી);
	જો (browseFile == ખાલી)
	    જાઓ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#જો defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    જો (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    જો (fd != ખાલી)
    {
#ifdef FEAT_SESSION
	જો (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	નહીંતર
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	જો (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	જો (put_લીટી(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	જો (eap->cmdidx == CMD_mkvimrc)
	    (શૂન્ય)put_લીટી(fd, "version 6.0");

#ifdef FEAT_SESSION
	જો (eap->cmdidx == CMD_mksession)
	{
	    જો (put_લીટી(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	જો (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    જો (p_cp)
		(શૂન્ય)put_લીટી(fd, "if !&cp | set cp | endif");
	    નહીંતર
		(શૂન્ય)put_લીટી(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	જો (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    પૂર્ણાંક flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_લીટી(fd, "var cpo_save: string") == FAIL;

	    જો (eap->cmdidx == CMD_mksession)
	    {
		જો (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    જો (do_mappings)
		failed |= (makemap(fd, ખાલી) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#જો defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    માટે (sid = 1; sid <= script_વસ્તુઓ.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		જો ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    પૂર્ણાંક b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    માટે (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			જો ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ચાલુ;

			// compare prefixes if available
			જો (si->sn_autoload_prefix != ખાલી && b_si->sn_autoload_prefix != ખાલી
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    તોડો;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			જો (STRCMP(name, b_name) == 0)
			    તોડો;
		    }

		    // import the auto script if there are no conflicts
		    જો (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	જો (!failed && view_session)
	{
	    જો (put_લીટી(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    જો (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		જો (dirnow == ખાલી)
		    failed = TRUE;
		નહીંતર
		{
		    // Change to session file's dir.
		    જો (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((અક્ષર *)dirnow) != 0)
			*dirnow = NUL;
		    જો (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			જો (vim_chdirfile(fname, ખાલી) == OK)
			    shorten_fnames(TRUE);
		    }
		    નહીંતર જો (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ખાલી)
		    {
			જો (mch_chdir((અક્ષર *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    જો (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ખાલી)))
		    {
			જો (mch_chdir((અક્ષર *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    નહીંતર
	    {
		failed |= put_લીટી(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ખાલી) == FAIL);
	    }
	    જો (put_લીટી(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_શોધ_EXTRA
	    જો (no_hlsearch && put_લીટી(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    જો (put_લીટી(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    જો (eap->cmdidx == CMD_mksession)
	    {
		જો (put_લીટી(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	જો (put_લીટી(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	જો (failed)
	    emsg(_(e_error_while_writing));
#જો defined(FEAT_SESSION)
	નહીંતર જો (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    જો (tbuf != ખાલી)
	    {
		જો (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

    apply_autocmds(EVENT_SESSIONWRITEPOST, ખાલી, ખાલી, FALSE, curbuf);
}

#જો defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    જો (ASCII_ISUPPER(*p))
    {
	જ્યારે (*(++p))
	    જો (ASCII_ISLOWER(*p))
		પાછા VAR_FLAVOUR_SESSION;
	પાછા VAR_FLAVOUR_VIMINFO;
    }
    નહીંતર
	પાછા VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    પૂર્ણાંક
put_eol(FILE *fd)
{
    જો (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	પાછા FAIL;
    પાછા OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    પૂર્ણાંક
put_લીટી(FILE *fd, અક્ષર *s)
{
    જો (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	પાછા FAIL;
    પાછા OK;
}
