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

#जर defined(FEAT_SESSION)

स्थिर पूर्णांक did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    स्थिर पूर्णांक
ses_put_fname(खाता *fd, char_u *name, अचिन्हित *flagp)
{
    char_u	*sname;
    char_u	*p;
    पूर्णांक		retval = OK;

    sname = home_replace_save(रिक्त, name);
    जर (sname == रिक्त)
	परत FAIL;

    जर (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	साठी (p = sname; *p != NUL; MB_PTR_ADV(p))
	    जर (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    जर (p == रिक्त)
	परत FAIL;

    // write the result
    जर (ख_माला_दो((अक्षर *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    परत retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    स्थिर पूर्णांक
ses_fname(खाता *fd, buf_T *buf, अचिन्हित *flagp, पूर्णांक add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    जर (buf->b_sfname != रिक्त
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    अन्यथा
	name = buf->b_ffname;
    जर (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	परत FAIL;
    परत OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    स्थिर पूर्णांक
ses_arglist(
    खाता	*fd,
    अक्षर	*cmd,
    garray_T	*gap,
    पूर्णांक		fullname,	// TRUE: use full path name
    अचिन्हित	*flagp)
{
    पूर्णांक		i;
    char_u	*buf = रिक्त;
    char_u	*s;

    जर (ख_माला_दो(cmd, fd) < 0 || put_eol(fd) == FAIL)
	परत FAIL;
    जर (put_ओळ(fd, ":%argdel") == FAIL)
	परत FAIL;
    साठी (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	जर (s != रिक्त)
	{
	    जर (fullname)
	    {
		buf = alloc(MAXPATHL);
		जर (buf != रिक्त)
		{
		    (शून्य)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    जर (ख_माला_दो(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		परत FAIL;
	    }
	    vim_free(buf);
	}
    }
    परत OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    स्थिर पूर्णांक
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    जर (bt_terminal(wp->w_buffer))
	परत !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    जर (wp->w_buffer->b_fname == रिक्त
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	परत (ssop_flags & SSOP_BLANK);
    जर (bt_help(wp->w_buffer))
	परत (ssop_flags & SSOP_HELP);
    परत TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    स्थिर पूर्णांक
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    जर (fr->fr_layout == FR_LEAF)
	परत ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	जर (ses_do_frame(frc))
	    परत TRUE;
    परत FALSE;
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
	जर (ses_do_frame(frc))
	    थांबा;
    परत frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    स्थिर पूर्णांक
ses_win_rec(खाता *fd, frame_T *fr)
{
    frame_T	*frc;
    पूर्णांक		मोजणी = 0;

    जर (fr->fr_layout == FR_LEAF)
	परत OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    जर (frc != रिक्त)
	पर्यंत ((frc = ses_skipframe(frc->fr_next)) != रिक्त)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    जर (put_ओळ(fd, "wincmd _ | wincmd |") == FAIL
		    || put_ओळ(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		परत FAIL;
	    ++मोजणी;
	}

    // Go back to the first window.
    जर (मोजणी > 0 && (ख_लिखो(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", मोजणी) < 0
		|| put_eol(fd) == FAIL))
	परत FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    पर्यंत (frc != रिक्त)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	जर (frc != रिक्त && put_ओळ(fd, "wincmd w") == FAIL)
	    परत FAIL;
    }

    परत OK;
}

    स्थिर पूर्णांक
ses_winsizes(
    खाता	*fd,
    पूर्णांक		restore_size,
    win_T	*tab_firstwin)
{
    पूर्णांक		n = 0;
    win_T	*wp;

    जर (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	साठी (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जर (!ses_do_win(wp))
		सुरूठेवा;
	    ++n;

	    // restore height when not full height
	    जर (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (ख_लिखो(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (दीर्घ)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		परत FAIL;

	    // restore width when not full width
	    जर (wp->w_width < Columns && (ख_लिखो(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (दीर्घ)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		परत FAIL;
	}
    }
    अन्यथा
    {
	// Just equalise window sizes
	जर (put_ओळ(fd, "wincmd =") == FAIL)
	    परत FAIL;
    }
    परत OK;
}

    स्थिर पूर्णांक
put_view_curpos(खाता *fd, win_T *wp, अक्षर *spaces)
{
    पूर्णांक r;

    जर (wp->w_curswant == MAXCOL)
	r = ख_लिखो(fd, "%snormal! $", spaces);
    अन्यथा
	r = ख_लिखो(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    परत r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    स्थिर पूर्णांक
put_view(
    खाता	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    पूर्णांक		add_edit,	     // add ":edit" command to view
    अचिन्हित	*flagp,		     // vop_flags or ssop_flags
    पूर्णांक		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    पूर्णांक		f;
    पूर्णांक		do_cursor;
    पूर्णांक		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    जर (wp->w_alist == &global_alist)
    {
	जर (put_ओळ(fd, "argglobal") == FAIL)
	    परत FAIL;
    }
    अन्यथा
    {
	जर (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != रिक्त
			|| wp->w_localdir != रिक्त, flagp) == FAIL)
	    परत FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    जर (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	जर (ख_लिखो(fd, ":%ldargu", (दीर्घ)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    परत FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    जर (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	जर (bt_help(wp->w_buffer))
	{
	    अक्षर *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    जर (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (अक्षर *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    जर (put_ओळ(fd, "enew | setl bt=help") == FAIL
		    || ख_लिखो(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		परत FAIL;
	}
# ifdef FEAT_TERMINAL
	अन्यथा जर (bt_terminal(wp->w_buffer))
	{
	    जर (term_write_session(fd, wp, terminal_bufs) == FAIL)
		परत FAIL;
	}
# endif
	// Load the file.
	अन्यथा जर (wp->w_buffer->b_ffname != रिक्त
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    जर (ख_माला_दो("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || ख_माला_दो("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || ख_माला_दो(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || ख_माला_दो(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    // No file in this buffer, just make it empty.
	    जर (put_ओळ(fd, "enew") == FAIL)
		परत FAIL;
# ifdef FEAT_QUICKFIX
	    जर (wp->w_buffer->b_ffname != रिक्त)
	    {
		// The buffer does have a name, but it's not a file name.
		जर (ख_माला_दो("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    परत FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    जर (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	जर ((flagp == &ssop_flags)
		&& alt != रिक्त
		&& alt->b_fname != रिक्त
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (ख_माला_दो("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    परत FAIL;
    }

    // Local mappings and abbreviations.
    जर ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	परत FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    जर (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    अन्यथा जर (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    अन्यथा
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    जर (f == FAIL)
	परत FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    जर ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != रिक्त
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	जर (put_folds(fd, wp) == FAIL)
	    परत FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    जर (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	जर (put_ओळ(fd, "{") == FAIL)
	    परत FAIL;

	जर (wp->w_height <= 0)
	{
	    जर (ख_लिखो(fd, "  var l: number = %ld", (दीर्घ)wp->w_cursor.lnum) < 0)
		परत FAIL;
	}
	अन्यथा जर (ख_लिखो(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (दीर्घ)wp->w_cursor.lnum,
		    (दीर्घ)(wp->w_cursor.lnum - wp->w_topline),
		    (दीर्घ)wp->w_height / 2, (दीर्घ)wp->w_height) < 0)
	    परत FAIL;

	जर (put_eol(fd) == FAIL
		|| put_ओळ(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_ओळ(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_ओळ(fd, "  normal! zt") == FAIL
		|| ख_लिखो(fd, "  keepjumps :%ld", (दीर्घ)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    परत FAIL;
	// Restore the cursor column and left offset when not wrapping.
	जर (wp->w_cursor.col == 0)
	{
	    जर (put_ओळ(fd, "  normal! 0") == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    जर (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		जर (ख_लिखो(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (दीर्घ)wp->w_virtcol + 1,
			    (दीर्घ)(wp->w_virtcol - wp->w_leftcol),
			    (दीर्घ)wp->w_width / 2, (दीर्घ)wp->w_width) < 0
			|| put_eol(fd) == FAIL
			|| put_ओळ(fd, "  if c > 0") == FAIL
			|| ख_लिखो(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (दीर्घ)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_ओळ(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_ओळ(fd, "  endif") == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (put_view_curpos(fd, wp, "  ") == FAIL)
		परत FAIL;
	}

	जर (put_ओळ(fd, "}") == FAIL)
	    परत FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    जर (wp->w_localdir != रिक्त
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	जर (ख_माला_दो("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    परत FAIL;
	did_lcd = TRUE;
    }

    परत OK;
}

# ifdef FEAT_EVAL
    स्थिर पूर्णांक
store_session_globals(खाता *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    पूर्णांक		todo;
    char_u	*p, *t;

    todo = (पूर्णांक)gvht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(gvht, hi, todo)
    {
	जर (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    जर ((this_var->di_tv.v_type == VAR_संख्या
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_किल्ली) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		जर (p == रिक्त)	    // out of memory
		    थांबा;
		साठी (t = p; *t != NUL; ++t)
		    जर (*t == '\n')
			*t = 'n';
		    अन्यथा जर (*t == '\r')
			*t = 'r';
		जर ((ख_लिखो(fd, "g:%s = %c%s%c",
				this_var->di_किल्ली,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    परत FAIL;
		}
		vim_free(p);
	    }
	    अन्यथा जर (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_किल्ली) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		पूर्णांक sign = ' ';

		जर (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		जर ((ख_लिखो(fd, "g:%s = %c%f",
					       this_var->di_किल्ली, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    परत FAIL;
	    }
	}
    }
    परत OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    स्थिर पूर्णांक
makeopens(
    खाता	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    पूर्णांक		only_save_windows = TRUE;
    पूर्णांक		nr;
    पूर्णांक		restore_size = TRUE;
    पूर्णांक		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = रिक्त;
    पूर्णांक		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    पूर्णांक		cur_arg_idx = 0;
    पूर्णांक		next_arg_idx = 0;
    पूर्णांक		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    जर (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    जर (put_ओळ(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	जा fail;

    जर (put_ओळ(fd, "doautoall SessionLoadPre") == FAIL)
	जा fail;

    जर (put_ओळ(fd, "var save_splitbelow: bool") == FAIL
	    || put_ओळ(fd, "var save_splitright: bool") == FAIL
	    || put_ओळ(fd, "var save_winminheight: number") == FAIL
	    || put_ओळ(fd, "var save_winminwidth: number") == FAIL
	    || put_ओळ(fd, "var wipebuf: number = -1") == FAIL
	    || put_ओळ(fd, "var shortmess_save: string") == FAIL)
	जा fail;

    जर (ssop_flags & SSOP_GLOBALS)
	जर (store_session_globals(fd) == FAIL)
	    जा fail;
# endif

    // Close all windows and tabs but one.
    जर (put_ओळ(fd, "silent only") == FAIL)
	जा fail;
    जर ((ssop_flags & SSOP_TABPAGES)
	    && put_ओळ(fd, "silent tabonly") == FAIL)
	जा fail;

    // Now a :cd command to the session directory or the current directory
    जर (ssop_flags & SSOP_SESDIR)
    {
	जर (put_ओळ(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    जा fail;
    }
    अन्यथा जर (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(रिक्त, globaldir != रिक्त ? globaldir : dirnow);
	जर (sname == रिक्त
		|| ख_माला_दो("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    जा fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    जर (put_ओळ(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	जा fail;
    जर (put_ओळ(fd, "  wipebuf = bufnr('%')") == FAIL)
	जा fail;
    जर (put_ओळ(fd, "endif") == FAIL)
	जा fail;

    // Save 'shortmess' if not storing options.
    जर ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_ओळ(fd, "shortmess_save = &shortmess") == FAIL)
	जा fail;

    // Set 'shortmess' for the following.
    जर (put_ओळ(fd, "set shortmess+=aoO") == FAIL)
	जा fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	जर (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != रिक्त
		&& buf->b_p_bl)
	{
	    जर (ख_लिखो(fd, "badd +%ld ", buf->b_wininfo == रिक्त ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		जा fail;
	}
    }

    // the global argument list
    जर (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	जा fail;

    जर (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	जर (ख_लिखो(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    जा fail;
    }

# ifdef FEAT_GUI
    जर (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	पूर्णांक	x, y;

	जर (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    जर (ख_लिखो(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		जा fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    जर (p_stal == 1 && first_tabpage->tp_next != रिक्त)
    {
	जर (put_ओळ(fd, "set stal=2") == FAIL)
	    जा fail;
	restore_stal = TRUE;
    }

    जर ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    जर (tp->tp_next != रिक्त
		  && put_ओळ(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		जा fail;
	जर (first_tabpage->tp_next != रिक्त && put_ओळ(fd, "tabrewind") == FAIL)
	    जा fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	पूर्णांक	need_tabnext = FALSE;
	पूर्णांक	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	जर ((ssop_flags & SSOP_TABPAGES))
	{
	    जर (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    अन्यथा
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    जर (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	अन्यथा
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	साठी (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जर (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != रिक्त
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		जर (need_tabnext && put_ओळ(fd, "tabnext") == FAIL)
		    जा fail;
		need_tabnext = FALSE;

		जर (ख_माला_दो("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    जा fail;
		जर (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		थांबा;
	    }
	}

	// If no file got edited create an empty tab page.
	जर (need_tabnext && put_ओळ(fd, "tabnext") == FAIL)
	    जा fail;

	जर (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    जर (put_ओळ(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_ओळ(fd, "save_splitright = &splitright")
								       == FAIL)
		जा fail;
	    जर (put_ओळ(fd, "set splitbelow splitright") == FAIL)
		जा fail;
	    जर (ses_win_rec(fd, tab_topframe) == FAIL)
		जा fail;
	    जर (put_ओळ(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_ओळ(fd, "&splitright = save_splitright")
								       == FAIL)
		जा fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	साठी (wp = tab_firstwin; wp != रिक्त; wp = W_NEXT(wp))
	{
	    जर (ses_do_win(wp))
		++nr;
	    अन्यथा
		restore_size = FALSE;
	    जर (curwin == wp)
		cnr = nr;
	}

	जर (tab_firstwin->w_next != रिक्त)
	{
	    // Go to the first window.
	    जर (put_ओळ(fd, "wincmd t") == FAIL)
		जा fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    जर (!restore_height_width
		    && (put_ओळ(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_ओळ(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		जा fail;
	    जर (put_ओळ(fd, "set winminheight=0") == FAIL
		    || put_ओळ(fd, "set winheight=1") == FAIL
		    || put_ओळ(fd, "set winminwidth=0") == FAIL
		    || put_ओळ(fd, "set winwidth=1") == FAIL)
		जा fail;
	    restore_height_width = TRUE;
	}
	जर (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    जा fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	जर ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != रिक्त)
	{
	    जर (ख_माला_दो("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		जा fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	साठी (wp = tab_firstwin; wp != रिक्त; wp = wp->w_next)
	{
	    जर (!ses_do_win(wp))
		सुरूठेवा;
	    जर (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# अन्यथा
							 रिक्त
# endif
		 ) == FAIL)
		जा fail;
	    जर (nr > 1 && put_ओळ(fd, "wincmd w") == FAIL)
		जा fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	जर (cnr > 1 && (ख_लिखो(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    जा fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	जर (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    जा fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	जर (!(ssop_flags & SSOP_TABPAGES))
	    थांबा;
    }

    जर (ssop_flags & SSOP_TABPAGES)
    {
	जर (ख_लिखो(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    जा fail;
    }
    जर (restore_stal && put_ओळ(fd, "set stal=1") == FAIL)
	जा fail;

    // Wipe out an empty unnamed buffer we started in.
    जर (put_ओळ(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	जा fail;
    जर (put_ओळ(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	जा fail;
    जर (put_ओळ(fd, "endif") == FAIL)
	जा fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    जर (ख_लिखो(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || ख_लिखो(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	जा fail;

    // Restore 'shortmess'.
    जर (ssop_flags & SSOP_OPTIONS)
    {
	जर (ख_लिखो(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    जा fail;
    }
    अन्यथा
    {
	जर (put_ओळ(fd, "&shortmess = shortmess_save") == FAIL)
	    जा fail;
    }

    जर (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	जर (put_ओळ(fd, "&winminheight = save_winminheight") == FAIL
	      || put_ओळ(fd, "&winminwidth = save_winminwidth") == FAIL)
	    जा fail;
    }

    // Lastly, execute the x.vim file if it exists.
    जर (put_ओळ(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_ओळ(fd, "if filereadable(sx)") == FAIL
	    || put_ओळ(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_ओळ(fd, "endif") == FAIL)
	जा fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    परत ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    स्थिर char_u *
get_view_file(पूर्णांक c)
{
    पूर्णांक		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    जर (curbuf->b_ffname == रिक्त)
    {
	emsg(_(e_no_file_name));
	परत रिक्त;
    }
    sname = home_replace_save(रिक्त, curbuf->b_ffname);
    जर (sname == रिक्त)
	परत रिक्त;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    साठी (p = sname; *p; ++p)
	जर (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    जर (retval != रिक्त)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	साठी (p = sname; *p; ++p)
	{
	    जर (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    अन्यथा जर (vim_ispathsep(*p))
	    {
		*s++ = '=';
# जर defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		जर (*p == ':')
		    *s++ = '-';
		अन्यथा
# endif
		    *s++ = '+';
	    }
	    अन्यथा
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    परत retval;
}

/*
 * ":loadview [nr]"
 */
    शून्य
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    जर (fname == रिक्त)
	परत;

    (शून्य)do_source(fname, FALSE, DOSO_NONE, रिक्त);
    vim_free(fname);
}

# जर defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    पूर्णांक
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    अक्षर	    *mksession_cmdline;
    अचिन्हित पूर्णांक    save_ssop_flags;
    पूर्णांक		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    जर (escaped_filename == रिक्त)
	परत FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    जर (mksession_cmdline == रिक्त)
    {
	vim_free(escaped_filename);
	परत FALSE;
    }
    म_नकल(mksession_cmdline, "mksession ");
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
    जर (!failed)
    {
	खाता *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == रिक्त
	       || put_ओळ(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_ओळ(fd, "unlet g:Save_VV_this_session") == FAIL);

	जर (fd != रिक्त && ख_बंद(fd) != 0)
	    failed = TRUE;

	जर (failed)
	    mch_remove(filename);
    }

    परत !failed;
}
# endif

#endif // FEAT_SESSION

#जर defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
स्थिर पूर्णांक mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    शून्य
ex_mkrc(exarg_T	*eap)
{
    खाता	*fd;
    पूर्णांक		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = रिक्त;
#endif
#ifdef FEAT_SESSION
    पूर्णांक		view_session = FALSE;
    पूर्णांक		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = रिक्त;
    अचिन्हित	*flagp;
#endif
#जर defined(FEAT_EVAL)
    पूर्णांक		sid;
    scriptitem_T *si = रिक्त;
#endif

    जर (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#अन्यथा
	ex_ni(eap);
	परत;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    जर (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	जर (fname == रिक्त)
	    परत;
	viewFile = fname;
	using_vdir = TRUE;
    }
    अन्यथा
#endif
	जर (*eap->arg != NUL)
	fname = eap->arg;
    अन्यथा जर (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    अन्यथा जर (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    अन्यथा
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    जर (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", रिक्त,
		(char_u *)_(BROWSE_FILTER_MACROS), रिक्त);
	जर (browseFile == रिक्त)
	    जा theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#जर defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    जर (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    जर (fd != रिक्त)
    {
#ifdef FEAT_SESSION
	जर (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	अन्यथा
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	जर (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	जर (put_ओळ(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	जर (eap->cmdidx == CMD_mkvimrc)
	    (शून्य)put_ओळ(fd, "version 6.0");

#ifdef FEAT_SESSION
	जर (eap->cmdidx == CMD_mksession)
	{
	    जर (put_ओळ(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	जर (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    जर (p_cp)
		(शून्य)put_ओळ(fd, "if !&cp | set cp | endif");
	    अन्यथा
		(शून्य)put_ओळ(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	जर (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    पूर्णांक flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_ओळ(fd, "var cpo_save: string") == FAIL;

	    जर (eap->cmdidx == CMD_mksession)
	    {
		जर (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    जर (do_mappings)
		failed |= (makemap(fd, रिक्त) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#जर defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    साठी (sid = 1; sid <= script_वस्तुसंच.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		जर ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    पूर्णांक b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    साठी (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			जर ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    सुरूठेवा;

			// compare prefixes if available
			जर (si->sn_autoload_prefix != रिक्त && b_si->sn_autoload_prefix != रिक्त
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    थांबा;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			जर (STRCMP(name, b_name) == 0)
			    थांबा;
		    }

		    // import the auto script if there are no conflicts
		    जर (ख_लिखो(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	जर (!failed && view_session)
	{
	    जर (put_ओळ(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    जर (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		जर (dirnow == रिक्त)
		    failed = TRUE;
		अन्यथा
		{
		    // Change to session file's dir.
		    जर (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((अक्षर *)dirnow) != 0)
			*dirnow = NUL;
		    जर (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			जर (vim_chdirfile(fname, रिक्त) == OK)
			    shorten_fnames(TRUE);
		    }
		    अन्यथा जर (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != रिक्त)
		    {
			जर (mch_chdir((अक्षर *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    जर (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != रिक्त)))
		    {
			जर (mch_chdir((अक्षर *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    अन्यथा
	    {
		failed |= put_ओळ(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								रिक्त) == FAIL);
	    }
	    जर (put_ओळ(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_शोध_EXTRA
	    जर (no_hlsearch && put_ओळ(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    जर (put_ओळ(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    जर (eap->cmdidx == CMD_mksession)
	    {
		जर (put_ओळ(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	जर (put_ओळ(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= ख_बंद(fd);

	जर (failed)
	    emsg(_(e_error_while_writing));
#जर defined(FEAT_SESSION)
	अन्यथा जर (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    जर (tbuf != रिक्त)
	    {
		जर (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

#जर defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    जर (ASCII_ISUPPER(*p))
    {
	पर्यंत (*(++p))
	    जर (ASCII_ISLOWER(*p))
		परत VAR_FLAVOUR_SESSION;
	परत VAR_FLAVOUR_VIMINFO;
    }
    अन्यथा
	परत VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    पूर्णांक
put_eol(खाता *fd)
{
    जर (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (अ_दो('\r', fd) < 0)) ||
#endif
	    (अ_दो('\n', fd) < 0))
	परत FAIL;
    परत OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    पूर्णांक
put_ओळ(खाता *fd, अक्षर *s)
{
    जर (ख_माला_दो(s, fd) < 0 || put_eol(fd) == FAIL)
	परत FAIL;
    परत OK;
}
