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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SESSION)

స్థిర పూర్ణాంకం did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    స్థిర పూర్ణాంకం
ses_put_fname(FILE *fd, char_u *name, గుర్తులేని *flagp)
{
    char_u	*sname;
    char_u	*p;
    పూర్ణాంకం		retval = OK;

    sname = home_replace_save(ఖాళీ, name);
    ఒకవేళ (sname == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	కొరకు (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ఒకవేళ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;

    // write the result
    ఒకవేళ (fputs((అక్షరం *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    తిరిగి retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    స్థిర పూర్ణాంకం
ses_fname(FILE *fd, buf_T *buf, గుర్తులేని *flagp, పూర్ణాంకం కూడు_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ఒకవేళ (buf->b_sfname != ఖాళీ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    లేకపోతే
	name = buf->b_ffname;
    ఒకవేళ (ses_put_fname(fd, name, flagp) == FAIL
	    || (కూడు_eol && put_eol(fd) == FAIL))
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    స్థిర పూర్ణాంకం
ses_arglist(
    FILE	*fd,
    అక్షరం	*cmd,
    garray_T	*gap,
    పూర్ణాంకం		fullname,	// TRUE: use full path name
    గుర్తులేని	*flagp)
{
    పూర్ణాంకం		i;
    char_u	*buf = ఖాళీ;
    char_u	*s;

    ఒకవేళ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (put_పంక్తి(fd, ":%argdel") == FAIL)
	తిరిగి FAIL;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_దత్తాంశం)[i]);
	ఒకవేళ (s != ఖాళీ)
	{
	    ఒకవేళ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ఒకవేళ (buf != ఖాళీ)
		{
		    (శూన్య)vim_నిండిన_Name(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ఒకవేళ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		తిరిగి FAIL;
	    }
	    vim_free(buf);
	}
    }
    తిరిగి OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    స్థిర పూర్ణాంకం
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ఒకవేళ (bt_terminal(wp->w_బఫరు))
	తిరిగి !పదం_is_ముగిసిన(wp->w_బఫరు)
	    && (ssop_flags & SSOP_TERMINAL)
	    && పదం_should_restore(wp->w_బఫరు);
# endif
    ఒకవేళ (wp->w_బఫరు->b_fname == ఖాళీ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_బఫరు))
	తిరిగి (ssop_flags & SSOP_BLANK);
    ఒకవేళ (bt_help(wp->w_బఫరు))
	తిరిగి (ssop_flags & SSOP_HELP);
    తిరిగి TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    స్థిర పూర్ణాంకం
ses_do_చట్రం(చట్రం_T *fr)
{
    చట్రం_T	*frc;

    ఒకవేళ (fr->fr_layout == FR_ఆకు)
	తిరిగి ses_do_win(fr->fr_win);
    FOR_ALL_చట్రాలు(frc, fr->fr_బిడ్డ)
	ఒకవేళ (ses_do_చట్రం(frc))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    స్థిర చట్రం_T *
ses_skipframe(చట్రం_T *fr)
{
    చట్రం_T	*frc;

    FOR_ALL_చట్రాలు(frc, fr)
	ఒకవేళ (ses_do_చట్రం(frc))
	    ఆపు;
    తిరిగి frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    స్థిర పూర్ణాంకం
ses_win_rec(FILE *fd, చట్రం_T *fr)
{
    చట్రం_T	*frc;
    పూర్ణాంకం		లెక్క = 0;

    ఒకవేళ (fr->fr_layout == FR_ఆకు)
	తిరిగి OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_బిడ్డ);
    ఒకవేళ (frc != ఖాళీ)
	అయితే ((frc = ses_skipframe(frc->fr_తదుపరి)) != ఖాళీ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ఒకవేళ (put_పంక్తి(fd, "wincmd _ | wincmd |") == FAIL
		    || put_పంక్తి(fd, fr->fr_layout == FR_నిలువు
			? "split" : "vsplit") == FAIL)
		తిరిగి FAIL;
	    ++లెక్క;
	}

    // Go back to the first window.
    ఒకవేళ (లెక్క > 0 && (fprintf(fd, fr->fr_layout == FR_నిలువు
		    ? ":%dwincmd k" : ":%dwincmd h", లెక్క) < 0
		|| put_eol(fd) == FAIL))
	తిరిగి FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_బిడ్డ);
    అయితే (frc != ఖాళీ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_తదుపరి);
	// Go to next window.
	ఒకవేళ (frc != ఖాళీ && put_పంక్తి(fd, "wincmd w") == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
ses_winsizes(
    FILE	*fd,
    పూర్ణాంకం		restore_కొలత,
    win_T	*tab_firstwin)
{
    పూర్ణాంకం		n = 0;
    win_T	*wp;

    ఒకవేళ (restore_కొలత && (ssop_flags & SSOP_WINSIZE))
    {
	కొరకు (wp = tab_firstwin; wp != ఖాళీ; wp = wp->w_తదుపరి)
	{
	    ఒకవేళ (!ses_do_win(wp))
		కొనసాగు;
	    ++n;

	    // restore height when not full height
	    ఒకవేళ (wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు < topframe->fr_ఎత్తు
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (దీర్ఘ)wp->w_ఎత్తు, అడ్డువరుసలు / 2, అడ్డువరుసలు) < 0
						  || put_eol(fd) == FAIL))
		తిరిగి FAIL;

	    // restore width when not full width
	    ఒకవేళ (wp->w_వెడల్పు < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (దీర్ఘ)wp->w_వెడల్పు, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		తిరిగి FAIL;
	}
    }
    లేకపోతే
    {
	// Just equalise window sizes
	ఒకవేళ (put_పంక్తి(fd, "wincmd =") == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
put_view_curpos(FILE *fd, win_T *wp, అక్షరం *spaces)
{
    పూర్ణాంకం r;

    ఒకవేళ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    లేకపోతే
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    తిరిగి r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    స్థిర పూర్ణాంకం
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    పూర్ణాంకం		కూడు_edit,	     // add ":edit" command to view
    గుర్తులేని	*flagp,		     // vop_flags or ssop_flags
    పూర్ణాంకం		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs వాడని) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    పూర్ణాంకం		f;
    పూర్ణాంకం		do_cursor;
    పూర్ణాంకం		did_తదుపరి = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ఒకవేళ (wp->w_alist == &global_alist)
    {
	ఒకవేళ (put_పంక్తి(fd, "argglobal") == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	ఒకవేళ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ఖాళీ
			|| wp->w_localdir != ఖాళీ, flagp) == FAIL)
	    తిరిగి FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ఒకవేళ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ఒకవేళ (fprintf(fd, ":%ldargu", (దీర్ఘ)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    తిరిగి FAIL;
	did_తదుపరి = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ఒకవేళ (కూడు_edit && (!did_తదుపరి || wp->w_arg_idx_invalid))
    {
	ఒకవేళ (bt_help(wp->w_బఫరు))
	{
	    అక్షరం *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ఒకవేళ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (అక్షరం *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ఒకవేళ (put_పంక్తి(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		తిరిగి FAIL;
	}
# ifdef FEAT_TERMINAL
	లేకపోతే ఒకవేళ (bt_terminal(wp->w_బఫరు))
	{
	    ఒకవేళ (పదం_write_session(fd, wp, terminal_bufs) == FAIL)
		తిరిగి FAIL;
	}
# endif
	// Load the file.
	లేకపోతే ఒకవేళ (wp->w_బఫరు->b_ffname != ఖాళీ
		&& !bt_nofilename(wp->w_బఫరు))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ఒకవేళ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_బఫరు, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_బఫరు, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_బఫరు, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    // No file in this buffer, just make it empty.
	    ఒకవేళ (put_పంక్తి(fd, "enew") == FAIL)
		తిరిగి FAIL;
# ifdef FEAT_QUICKFIX
	    ఒకవేళ (wp->w_బఫరు->b_ffname != ఖాళీ)
	    {
		// The buffer does have a name, but it's not a file name.
		ఒకవేళ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_బఫరు, flagp, TRUE) == FAIL)
		    తిరిగి FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ఒకవేళ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ఒకవేళ ((flagp == &ssop_flags)
		&& alt != ఖాళీ
		&& alt->b_fname != ఖాళీ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    తిరిగి FAIL;
    }

    // Local mappings and abbreviations.
    ఒకవేళ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_బఫరు) == FAIL)
	తిరిగి FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_బఫరు;
    ఒకవేళ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    లేకపోతే ఒకవేళ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    లేకపోతే
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_బఫరు;
    ఒకవేళ (f == FAIL)
	తిరిగి FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ఒకవేళ ((*flagp & SSOP_FOLDS)
	    && wp->w_బఫరు->b_ffname != ఖాళీ
	    && (bt_సాధారణ(wp->w_బఫరు) || bt_help(wp->w_బఫరు)))
    {
	ఒకవేళ (put_folds(fd, wp) == FAIL)
	    తిరిగి FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ఒకవేళ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ఒకవేళ (put_పంక్తి(fd, "{") == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (wp->w_ఎత్తు <= 0)
	{
	    ఒకవేళ (fprintf(fd, "  var l: number = %ld", (దీర్ఘ)wp->w_cursor.lnum) < 0)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (దీర్ఘ)wp->w_cursor.lnum,
		    (దీర్ఘ)(wp->w_cursor.lnum - wp->w_topline),
		    (దీర్ఘ)wp->w_ఎత్తు / 2, (దీర్ఘ)wp->w_ఎత్తు) < 0)
	    తిరిగి FAIL;

	ఒకవేళ (put_eol(fd) == FAIL
		|| put_పంక్తి(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_పంక్తి(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_పంక్తి(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (దీర్ఘ)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    తిరిగి FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ఒకవేళ (wp->w_cursor.నిలువు == 0)
	{
	    ఒకవేళ (put_పంక్తి(fd, "  normal! 0") == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    ఒకవేళ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_వెడల్పు > 0)
	    {
		ఒకవేళ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (దీర్ఘ)wp->w_virtcol + 1,
			    (దీర్ఘ)(wp->w_virtcol - wp->w_leftcol),
			    (దీర్ఘ)wp->w_వెడల్పు / 2, (దీర్ఘ)wp->w_వెడల్పు) < 0
			|| put_eol(fd) == FAIL
			|| put_పంక్తి(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (దీర్ఘ)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_పంక్తి(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_పంక్తి(fd, "  endif") == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (put_view_curpos(fd, wp, "  ") == FAIL)
		తిరిగి FAIL;
	}

	ఒకవేళ (put_పంక్తి(fd, "}") == FAIL)
	    తిరిగి FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ఒకవేళ (wp->w_localdir != ఖాళీ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ఒకవేళ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    తిరిగి FAIL;
	did_lcd = TRUE;
    }

    తిరిగి OK;
}

# ifdef FEAT_EVAL
    స్థిర పూర్ణాంకం
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    పూర్ణాంకం		todo;
    char_u	*p, *t;

    todo = (పూర్ణాంకం)gvht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(gvht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ఒకవేళ ((this_var->di_tv.v_type == VAR_సంఖ్య
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_కీలకం) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ఒకవేళ (p == ఖాళీ)	    // out of memory
		    ఆపు;
		కొరకు (t = p; *t != NUL; ++t)
		    ఒకవేళ (*t == '\n')
			*t = 'n';
		    లేకపోతే ఒకవేళ (*t == '\r')
			*t = 'r';
		ఒకవేళ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_కీలకం,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    తిరిగి FAIL;
		}
		vim_free(p);
	    }
	    లేకపోతే ఒకవేళ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_కీలకం) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		పూర్ణాంకం sign = ' ';

		ఒకవేళ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ఒకవేళ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_కీలకం, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    తిరిగి FAIL;
	    }
	}
    }
    తిరిగి OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    స్థిర పూర్ణాంకం
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    పూర్ణాంకం		only_save_windows = TRUE;
    పూర్ణాంకం		nr;
    పూర్ణాంకం		restore_కొలత = TRUE;
    పూర్ణాంకం		restore_ఎత్తు_వెడల్పు = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ఖాళీ;
    పూర్ణాంకం		restore_stal = FALSE;
    win_T	*tab_firstwin;
    చట్రం_T	*tab_topframe;
    పూర్ణాంకం		cur_arg_idx = 0;
    పూర్ణాంకం		తదుపరి_arg_idx = 0;
    పూర్ణాంకం		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ఒకవేళ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ఒకవేళ (put_పంక్తి(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	వెళ్ళు fail;

    ఒకవేళ (put_పంక్తి(fd, "doautoall SessionLoadPre") == FAIL)
	వెళ్ళు fail;

    ఒకవేళ (put_పంక్తి(fd, "var save_splitbelow: bool") == FAIL
	    || put_పంక్తి(fd, "var save_splitright: bool") == FAIL
	    || put_పంక్తి(fd, "var save_winminheight: number") == FAIL
	    || put_పంక్తి(fd, "var save_winminwidth: number") == FAIL
	    || put_పంక్తి(fd, "var wipebuf: number = -1") == FAIL
	    || put_పంక్తి(fd, "var shortmess_save: string") == FAIL)
	వెళ్ళు fail;

    ఒకవేళ (ssop_flags & SSOP_GLOBALS)
	ఒకవేళ (store_session_globals(fd) == FAIL)
	    వెళ్ళు fail;
# endif

    // Close all windows and tabs but one.
    ఒకవేళ (put_పంక్తి(fd, "silent only") == FAIL)
	వెళ్ళు fail;
    ఒకవేళ ((ssop_flags & SSOP_TABPAGES)
	    && put_పంక్తి(fd, "silent tabonly") == FAIL)
	వెళ్ళు fail;

    // Now a :cd command to the session directory or the current directory
    ఒకవేళ (ssop_flags & SSOP_SESDIR)
    {
	ఒకవేళ (put_పంక్తి(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    వెళ్ళు fail;
    }
    లేకపోతే ఒకవేళ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ఖాళీ, globaldir != ఖాళీ ? globaldir : dirnow);
	ఒకవేళ (sname == ఖాళీ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    వెళ్ళు fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ఒకవేళ (put_పంక్తి(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	వెళ్ళు fail;
    ఒకవేళ (put_పంక్తి(fd, "  wipebuf = bufnr('%')") == FAIL)
	వెళ్ళు fail;
    ఒకవేళ (put_పంక్తి(fd, "endif") == FAIL)
	వెళ్ళు fail;

    // Save 'shortmess' if not storing options.
    ఒకవేళ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_పంక్తి(fd, "shortmess_save = &shortmess") == FAIL)
	వెళ్ళు fail;

    // Set 'shortmess' for the following.
    ఒకవేళ (put_పంక్తి(fd, "set shortmess+=aoO") == FAIL)
	వెళ్ళు fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ఒకవేళ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ఖాళీ
		&& buf->b_p_bl)
	{
	    ఒకవేళ (fprintf(fd, "badd +%ld ", buf->b_wininfo == ఖాళీ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		వెళ్ళు fail;
	}
    }

    // the global argument list
    ఒకవేళ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	వెళ్ళు fail;

    ఒకవేళ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ఒకవేళ (fprintf(fd, "set lines=%ld columns=%ld" , అడ్డువరుసలు, Columns) < 0
		|| put_eol(fd) == FAIL)
	    వెళ్ళు fail;
    }

# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use && (ssop_flags & SSOP_WINPOS))
    {
	పూర్ణాంకం	x, y;

	ఒకవేళ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ఒకవేళ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		వెళ్ళు fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ఒకవేళ (p_stal == 1 && మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
    {
	ఒకవేళ (put_పంక్తి(fd, "set stal=2") == FAIL)
	    వెళ్ళు fail;
	restore_stal = TRUE;
    }

    ఒకవేళ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ఒకవేళ (tp->tp_తదుపరి != ఖాళీ
		  && put_పంక్తి(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		వెళ్ళు fail;
	ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ && put_పంక్తి(fd, "tabrewind") == FAIL)
	    వెళ్ళు fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	పూర్ణాంకం	need_tabnext = FALSE;
	పూర్ణాంకం	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ఒకవేళ ((ssop_flags & SSOP_TABPAGES))
	{
	    ఒకవేళ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    లేకపోతే
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ఒకవేళ (tp != మొదటి_tabpage)
		need_tabnext = TRUE;
	}
	లేకపోతే
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	కొరకు (wp = tab_firstwin; wp != ఖాళీ; wp = wp->w_తదుపరి)
	{
	    ఒకవేళ (ses_do_win(wp)
		    && wp->w_బఫరు->b_ffname != ఖాళీ
		    && !bt_help(wp->w_బఫరు)
		    && !bt_nofilename(wp->w_బఫరు))
	    {
		ఒకవేళ (need_tabnext && put_పంక్తి(fd, "tabnext") == FAIL)
		    వెళ్ళు fail;
		need_tabnext = FALSE;

		ఒకవేళ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_బఫరు, &ssop_flags, TRUE)
								       == FAIL)
		    వెళ్ళు fail;
		ఒకవేళ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ఆపు;
	    }
	}

	// If no file got edited create an empty tab page.
	ఒకవేళ (need_tabnext && put_పంక్తి(fd, "tabnext") == FAIL)
	    వెళ్ళు fail;

	ఒకవేళ (tab_topframe->fr_layout != FR_ఆకు)
	{
	    // Save current window layout.
	    ఒకవేళ (put_పంక్తి(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_పంక్తి(fd, "save_splitright = &splitright")
								       == FAIL)
		వెళ్ళు fail;
	    ఒకవేళ (put_పంక్తి(fd, "set splitbelow splitright") == FAIL)
		వెళ్ళు fail;
	    ఒకవేళ (ses_win_rec(fd, tab_topframe) == FAIL)
		వెళ్ళు fail;
	    ఒకవేళ (put_పంక్తి(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_పంక్తి(fd, "&splitright = save_splitright")
								       == FAIL)
		వెళ్ళు fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	కొరకు (wp = tab_firstwin; wp != ఖాళీ; wp = W_తదుపరి(wp))
	{
	    ఒకవేళ (ses_do_win(wp))
		++nr;
	    లేకపోతే
		restore_కొలత = FALSE;
	    ఒకవేళ (curwin == wp)
		cnr = nr;
	}

	ఒకవేళ (tab_firstwin->w_తదుపరి != ఖాళీ)
	{
	    // Go to the first window.
	    ఒకవేళ (put_పంక్తి(fd, "wincmd t") == FAIL)
		వెళ్ళు fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ఒకవేళ (!restore_ఎత్తు_వెడల్పు
		    && (put_పంక్తి(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_పంక్తి(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		వెళ్ళు fail;
	    ఒకవేళ (put_పంక్తి(fd, "set winminheight=0") == FAIL
		    || put_పంక్తి(fd, "set winheight=1") == FAIL
		    || put_పంక్తి(fd, "set winminwidth=0") == FAIL
		    || put_పంక్తి(fd, "set winwidth=1") == FAIL)
		వెళ్ళు fail;
	    restore_ఎత్తు_వెడల్పు = TRUE;
	}
	ఒకవేళ (nr > 1 && ses_winsizes(fd, restore_కొలత, tab_firstwin) == FAIL)
	    వెళ్ళు fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ఒకవేళ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ఖాళీ)
	{
	    ఒకవేళ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		వెళ్ళు fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	కొరకు (wp = tab_firstwin; wp != ఖాళీ; wp = wp->w_తదుపరి)
	{
	    ఒకవేళ (!ses_do_win(wp))
		కొనసాగు;
	    ఒకవేళ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# లేకపోతే
							 ఖాళీ
# endif
		 ) == FAIL)
		వెళ్ళు fail;
	    ఒకవేళ (nr > 1 && put_పంక్తి(fd, "wincmd w") == FAIL)
		వెళ్ళు fail;
	    తదుపరి_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = తదుపరి_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ఒకవేళ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    వెళ్ళు fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ఒకవేళ (nr > 1 && ses_winsizes(fd, restore_కొలత, tab_firstwin) == FAIL)
	    వెళ్ళు fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ఒకవేళ (!(ssop_flags & SSOP_TABPAGES))
	    ఆపు;
    }

    ఒకవేళ (ssop_flags & SSOP_TABPAGES)
    {
	ఒకవేళ (fprintf(fd, "tabnext %d", tabpage_సూచీ(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    వెళ్ళు fail;
    }
    ఒకవేళ (restore_stal && put_పంక్తి(fd, "set stal=1") == FAIL)
	వెళ్ళు fail;

    // Wipe out an empty unnamed buffer we started in.
    ఒకవేళ (put_పంక్తి(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	వెళ్ళు fail;
    ఒకవేళ (put_పంక్తి(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	వెళ్ళు fail;
    ఒకవేళ (put_పంక్తి(fd, "endif") == FAIL)
	వెళ్ళు fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ఒకవేళ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	వెళ్ళు fail;

    // Restore 'shortmess'.
    ఒకవేళ (ssop_flags & SSOP_OPTIONS)
    {
	ఒకవేళ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    వెళ్ళు fail;
    }
    లేకపోతే
    {
	ఒకవేళ (put_పంక్తి(fd, "&shortmess = shortmess_save") == FAIL)
	    వెళ్ళు fail;
    }

    ఒకవేళ (restore_ఎత్తు_వెడల్పు)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ఒకవేళ (put_పంక్తి(fd, "&winminheight = save_winminheight") == FAIL
	      || put_పంక్తి(fd, "&winminwidth = save_winminwidth") == FAIL)
	    వెళ్ళు fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ఒకవేళ (put_పంక్తి(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_పంక్తి(fd, "if filereadable(sx)") == FAIL
	    || put_పంక్తి(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_పంక్తి(fd, "endif") == FAIL)
	వెళ్ళు fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    తిరిగి ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    స్థిర char_u *
get_view_file(పూర్ణాంకం c)
{
    పూర్ణాంకం		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ఒకవేళ (curbuf->b_ffname == ఖాళీ)
    {
	emsg(_(e_no_file_name));
	తిరిగి ఖాళీ;
    }
    sname = home_replace_save(ఖాళీ, curbuf->b_ffname);
    ఒకవేళ (sname == ఖాళీ)
	తిరిగి ఖాళీ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    కొరకు (p = sname; *p; ++p)
	ఒకవేళ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ఒకవేళ (retval != ఖాళీ)
    {
	STRCPY(retval, p_vdir);
	కూడు_pathsep(retval);
	s = retval + STRLEN(retval);
	కొరకు (p = sname; *p; ++p)
	{
	    ఒకవేళ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    లేకపోతే ఒకవేళ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ఒకవేళ defined(BACKSLASH_లోపల_FILENAME) || defined(AMIGA) || defined(VMS)
		ఒకవేళ (*p == ':')
		    *s++ = '-';
		లేకపోతే
# endif
		    *s++ = '+';
	    }
	    లేకపోతే
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    తిరిగి retval;
}

/*
 * ":loadview [nr]"
 */
    శూన్య
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    (శూన్య)do_ఆధారం(fname, FALSE, DOSO_NONE, ఖాళీ);
    vim_free(fname);
}

# ఒకవేళ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    పూర్ణాంకం
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    అక్షరం	    *mksession_cmdline;
    గుర్తులేని పూర్ణాంకం    save_ssop_flags;
    పూర్ణాంకం		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ఒకవేళ (escaped_filename == ఖాళీ)
	తిరిగి FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ఒకవేళ (mksession_cmdline == ఖాళీ)
    {
	vim_free(escaped_filename);
	తిరిగి FALSE;
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
    ఒకవేళ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ఖాళీ
	       || put_పంక్తి(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_పంక్తి(fd, "unlet g:Save_VV_this_session") == FAIL);

	ఒకవేళ (fd != ఖాళీ && fclose(fd) != 0)
	    failed = TRUE;

	ఒకవేళ (failed)
	    mch_remove(filename);
    }

    తిరిగి !failed;
}
# endif

#అయితే_ముగింపు // FEAT_SESSION

#ఒకవేళ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
స్థిర పూర్ణాంకం mksession_nl = FALSE;    // use NL only in put_eol()
#అయితే_ముగింపు

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    శూన్య
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    పూర్ణాంకం		failed = FALSE;
    char_u	*fname;
#నిర్వచితమైతే FEAT_BROWSE
    char_u	*browseFile = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SESSION
    పూర్ణాంకం		view_session = FALSE;
    పూర్ణాంకం		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ఖాళీ;
    గుర్తులేని	*flagp;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం		sid;
    scriptitem_T *si = ఖాళీ;
#అయితే_ముగింపు

    ఒకవేళ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#నిర్వచితమైతే FEAT_SESSION
	view_session = TRUE;
#లేకపోతే
	ex_ni(eap);
	తిరిగి;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ఒకవేళ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ఒకవేళ (fname == ఖాళీ)
	    తిరిగి;
	viewFile = fname;
	using_vdir = TRUE;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (*eap->arg != NUL)
	fname = eap->arg;
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#నిర్వచితమైతే FEAT_SESSION
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#అయితే_ముగింపు
    లేకపోతే
	fname = (char_u *)EXRC_FILE;

#నిర్వచితమైతే FEAT_BROWSE
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ఖాళీ,
		(char_u *)_(BROWSE_FILTER_MACROS), ఖాళీ);
	ఒకవేళ (browseFile == ఖాళీ)
	    వెళ్ళు theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ఒకవేళ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#అయితే_ముగింపు

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ఒకవేళ (fd != ఖాళీ)
    {
#నిర్వచితమైతే FEAT_SESSION
	ఒకవేళ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	లేకపోతే
	    flagp = &ssop_flags;
#అయితే_ముగింపు

#నిర్వచితమైతే MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ఒకవేళ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#అయితే_ముగింపు

	// Enforce vim9script
	ఒకవేళ (put_పంక్తి(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ఒకవేళ (eap->cmdidx == CMD_mkvimrc)
	    (శూన్య)put_పంక్తి(fd, "version 6.0");

#నిర్వచితమైతే FEAT_SESSION
	ఒకవేళ (eap->cmdidx == CMD_mksession)
	{
	    ఒకవేళ (put_పంక్తి(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ఒకవేళ (eap->cmdidx != CMD_mkview)
#అయితే_ముగింపు
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ఒకవేళ (p_cp)
		(శూన్య)put_పంక్తి(fd, "if !&cp | set cp | endif");
	    లేకపోతే
		(శూన్య)put_పంక్తి(fd, "if &cp | set nocp | endif");
	}

#నిర్వచితమైతే FEAT_SESSION
	ఒకవేళ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#అయితే_ముగింపు
	{
	    bool do_mappings = true;
	    పూర్ణాంకం flags = OPT_GLOBAL;

#నిర్వచితమైతే FEAT_SESSION
	    failed |= put_పంక్తి(fd, "var cpo_save: string") == FAIL;

	    ఒకవేళ (eap->cmdidx == CMD_mksession)
	    {
		ఒకవేళ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#అయితే_ముగింపు

	    ఒకవేళ (do_mappings)
		failed |= (makemap(fd, ఖాళీ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ఒకవేళ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    కొరకు (sid = 1; sid <= script_వస్తువులు.ga_len; ++sid)
	    {
		si = SCRIPT_అంశం(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ఒకవేళ ((si->sn_autoload_ఉపసర్గ || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    పూర్ణాంకం b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    కొరకు (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_అంశం(b_sid);

			// Only autoload may conflict. Ignore if missing
			ఒకవేళ ((!b_si->sn_autoload_ఉపసర్గ && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    కొనసాగు;

			// compare prefixes if available
			ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ && b_si->sn_autoload_ఉపసర్గ != ఖాళీ
				&& (STRCMP(si->sn_autoload_ఉపసర్గ, b_si->sn_autoload_ఉపసర్గ) == 0))
			    ఆపు;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ఒకవేళ (STRCMP(name, b_name) == 0)
			    ఆపు;
		    }

		    // import the auto script if there are no conflicts
		    ఒకవేళ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#అయితే_ముగింపు
	}

#నిర్వచితమైతే FEAT_SESSION
	ఒకవేళ (!failed && view_session)
	{
	    ఒకవేళ (put_పంక్తి(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ఒకవేళ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ఒకవేళ (dirnow == ఖాళీ)
		    failed = TRUE;
		లేకపోతే
		{
		    // Change to session file's dir.
		    ఒకవేళ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((అక్షరం *)dirnow) != 0)
			*dirnow = NUL;
		    ఒకవేళ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ఒకవేళ (vim_chdirfile(fname, ఖాళీ) == OK)
			    shorten_fnames(TRUE);
		    }
		    లేకపోతే ఒకవేళ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ఖాళీ)
		    {
			ఒకవేళ (mch_chdir((అక్షరం *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ఒకవేళ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ఖాళీ)))
		    {
			ఒకవేళ (mch_chdir((అక్షరం *)dirnow) != 0)
			    emsg(_(e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    లేకపోతే
	    {
		failed |= put_పంక్తి(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ఖాళీ) == FAIL);
	    }
	    ఒకవేళ (put_పంక్తి(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_శోధన_EXTRA
	    ఒకవేళ (no_hlsearch && put_పంక్తి(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ఒకవేళ (put_పంక్తి(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ఒకవేళ (eap->cmdidx == CMD_mksession)
	    {
		ఒకవేళ (put_పంక్తి(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#అయితే_ముగింపు

	ఒకవేళ (put_పంక్తి(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ఒకవేళ (failed)
	    emsg(_(e_error_while_writing));
#ఒకవేళ defined(FEAT_SESSION)
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ఒకవేళ (tbuf != ఖాళీ)
	    {
		ఒకవేళ (vim_నిండిన_Name(fname, tbuf, MAXPATHL, FALSE) == OK)
		    సమితి_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#అయితే_ముగింపు
#నిర్వచితమైతే MKSESSION_NL
	mksession_nl = FALSE;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_BROWSE
theend:
    vim_free(browseFile);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SESSION
    vim_free(viewFile);
#అయితే_ముగింపు

    apply_autocmds(EVENT_SESSIONWRITEPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
}

#ఒకవేళ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ఒకవేళ (ASCII_ISUPPER(*p))
    {
	అయితే (*(++p))
	    ఒకవేళ (ASCII_ISLOWER(*p))
		తిరిగి VAR_FLAVOUR_SESSION;
	తిరిగి VAR_FLAVOUR_VIMINFO;
    }
    లేకపోతే
	తిరిగి VAR_FLAVOUR_DEFAULT;
}
#అయితే_ముగింపు

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    పూర్ణాంకం
put_eol(FILE *fd)
{
    ఒకవేళ (
#నిర్వచితమైతే USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#అయితే_ముగింపు
	    (putc('\n', fd) < 0))
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    పూర్ణాంకం
put_పంక్తి(FILE *fd, అక్షరం *s)
{
    ఒకవేళ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	తిరిగి FAIL;
    తిరిగి OK;
}
