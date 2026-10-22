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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SESSION)

ਸਥਿਰ ਪੂਰਨ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_put_fname(FILE *fd, char_u *name, ਬਿਨਾਨਿਸ਼ਾਨੀ *flagp)
{
    char_u	*sname;
    char_u	*p;
    ਪੂਰਨ		retval = OK;

    sname = home_replace_save(ਨਿਰਾ, name);
    ਜੇ (sname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	ਲਈ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ਜੇ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // write the result
    ਜੇ (fputs((ਅੱਖਰ *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ਵਾਪਸ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_fname(FILE *fd, buf_T *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ *flagp, ਪੂਰਨ ਜੋੜੋ_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ਜੇ (buf->b_sfname != ਨਿਰਾ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ਨਹੀਂਤਾਂ
	name = buf->b_ffname;
    ਜੇ (ses_put_fname(fd, name, flagp) == FAIL
	    || (ਜੋੜੋ_eol && put_eol(fd) == FAIL))
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_arglist(
    FILE	*fd,
    ਅੱਖਰ	*cmd,
    garray_T	*gap,
    ਪੂਰਨ		fullname,	// TRUE: use full path name
    ਬਿਨਾਨਿਸ਼ਾਨੀ	*flagp)
{
    ਪੂਰਨ		i;
    char_u	*buf = ਨਿਰਾ;
    char_u	*s;

    ਜੇ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (put_ਸਤਰ(fd, ":%argdel") == FAIL)
	ਵਾਪਸ FAIL;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_ਅੰਕੜੇ)[i]);
	ਜੇ (s != ਨਿਰਾ)
	{
	    ਜੇ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ਜੇ (buf != ਨਿਰਾ)
		{
		    (ਖਾਲੀ)vim_ਭਰਿਆ_Name(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ਜੇ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ਵਾਪਸ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ਜੇ (bt_terminal(wp->w_ਬਫਰ))
	ਵਾਪਸ !ਪਦ_is_ਮੁਕੰਮਲ(wp->w_ਬਫਰ)
	    && (ssop_flags & SSOP_TERMINAL)
	    && ਪਦ_should_restore(wp->w_ਬਫਰ);
# endif
    ਜੇ (wp->w_ਬਫਰ->b_fname == ਨਿਰਾ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_ਬਫਰ))
	ਵਾਪਸ (ssop_flags & SSOP_BLANK);
    ਜੇ (bt_help(wp->w_ਬਫਰ))
	ਵਾਪਸ (ssop_flags & SSOP_HELP);
    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_do_ਚੌਖਟਾ(ਚੌਖਟਾ_T *fr)
{
    ਚੌਖਟਾ_T	*frc;

    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ)
	ਵਾਪਸ ses_do_win(fr->fr_win);
    FOR_ALL_ਚੌਖਟੇ(frc, fr->fr_ਬੱਚਾ)
	ਜੇ (ses_do_ਚੌਖਟਾ(frc))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ਸਥਿਰ ਚੌਖਟਾ_T *
ses_skipframe(ਚੌਖਟਾ_T *fr)
{
    ਚੌਖਟਾ_T	*frc;

    FOR_ALL_ਚੌਖਟੇ(frc, fr)
	ਜੇ (ses_do_ਚੌਖਟਾ(frc))
	    ਰੋਕੋ;
    ਵਾਪਸ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ਸਥਿਰ ਪੂਰਨ
ses_win_rec(FILE *fd, ਚੌਖਟਾ_T *fr)
{
    ਚੌਖਟਾ_T	*frc;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ)
	ਵਾਪਸ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_ਬੱਚਾ);
    ਜੇ (frc != ਨਿਰਾ)
	ਜਦੋਂ ((frc = ses_skipframe(frc->fr_ਅਗਲਾ)) != ਨਿਰਾ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ਜੇ (put_ਸਤਰ(fd, "wincmd _ | wincmd |") == FAIL
		    || put_ਸਤਰ(fd, fr->fr_layout == FR_ਖਾਨਾ
			? "split" : "vsplit") == FAIL)
		ਵਾਪਸ FAIL;
	    ++ਗਿਣਤੀ_2;
	}

    // Go back to the first window.
    ਜੇ (ਗਿਣਤੀ_2 > 0 && (fprintf(fd, fr->fr_layout == FR_ਖਾਨਾ
		    ? ":%dwincmd k" : ":%dwincmd h", ਗਿਣਤੀ_2) < 0
		|| put_eol(fd) == FAIL))
	ਵਾਪਸ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_ਬੱਚਾ);
    ਜਦੋਂ (frc != ਨਿਰਾ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_ਅਗਲਾ);
	// Go to next window.
	ਜੇ (frc != ਨਿਰਾ && put_ਸਤਰ(fd, "wincmd w") == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
ses_winsizes(
    FILE	*fd,
    ਪੂਰਨ		restore_ਮਾਪ,
    win_T	*tab_firstwin)
{
    ਪੂਰਨ		n = 0;
    win_T	*wp;

    ਜੇ (restore_ਮਾਪ && (ssop_flags & SSOP_WINSIZE))
    {
	ਲਈ (wp = tab_firstwin; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	{
	    ਜੇ (!ses_do_win(wp))
		ਜਾਰੀ;
	    ++n;

	    // restore height when not full height
	    ਜੇ (wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ < topframe->fr_ਉਚਾਈ
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (ਲੰਮਾ)wp->w_ਉਚਾਈ, ਪੰਕਤੀਆਂ / 2, ਪੰਕਤੀਆਂ) < 0
						  || put_eol(fd) == FAIL))
		ਵਾਪਸ FAIL;

	    // restore width when not full width
	    ਜੇ (wp->w_ਚੌੜਾਈ < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (ਲੰਮਾ)wp->w_ਚੌੜਾਈ, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Just equalise window sizes
	ਜੇ (put_ਸਤਰ(fd, "wincmd =") == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
put_view_curpos(FILE *fd, win_T *wp, ਅੱਖਰ *spaces)
{
    ਪੂਰਨ r;

    ਜੇ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ਨਹੀਂਤਾਂ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ਵਾਪਸ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ਸਥਿਰ ਪੂਰਨ
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ਪੂਰਨ		ਜੋੜੋ_edit,	     // add ":edit" command to view
    ਬਿਨਾਨਿਸ਼ਾਨੀ	*flagp,		     // vop_flags or ssop_flags
    ਪੂਰਨ		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs ਅਣਵਰਤਿਆ) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ਪੂਰਨ		f;
    ਪੂਰਨ		do_cursor;
    ਪੂਰਨ		did_ਅਗਲਾ = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ਜੇ (wp->w_alist == &global_alist)
    {
	ਜੇ (put_ਸਤਰ(fd, "argglobal") == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ਨਿਰਾ
			|| wp->w_localdir != ਨਿਰਾ, flagp) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ਜੇ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ਜੇ (fprintf(fd, ":%ldargu", (ਲੰਮਾ)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ਵਾਪਸ FAIL;
	did_ਅਗਲਾ = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ਜੇ (ਜੋੜੋ_edit && (!did_ਅਗਲਾ || wp->w_arg_idx_invalid))
    {
	ਜੇ (bt_help(wp->w_ਬਫਰ))
	{
	    ਅੱਖਰ *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ਜੇ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (ਅੱਖਰ *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ਜੇ (put_ਸਤਰ(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ਵਾਪਸ FAIL;
	}
# ifdef FEAT_TERMINAL
	ਨਹੀਂਤਾਂ ਜੇ (bt_terminal(wp->w_ਬਫਰ))
	{
	    ਜੇ (ਪਦ_write_session(fd, wp, terminal_bufs) == FAIL)
		ਵਾਪਸ FAIL;
	}
# endif
	// Load the file.
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਬਫਰ->b_ffname != ਨਿਰਾ
		&& !bt_nofilename(wp->w_ਬਫਰ))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ਜੇ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_ਬਫਰ, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_ਬਫਰ, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_ਬਫਰ, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    // No file in this buffer, just make it empty.
	    ਜੇ (put_ਸਤਰ(fd, "enew") == FAIL)
		ਵਾਪਸ FAIL;
# ifdef FEAT_QUICKFIX
	    ਜੇ (wp->w_ਬਫਰ->b_ffname != ਨਿਰਾ)
	    {
		// The buffer does have a name, but it's not a file name.
		ਜੇ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_ਬਫਰ, flagp, TRUE) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ਜੇ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ਜੇ ((flagp == &ssop_flags)
		&& alt != ਨਿਰਾ
		&& alt->b_fname != ਨਿਰਾ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ਵਾਪਸ FAIL;
    }

    // Local mappings and abbreviations.
    ਜੇ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_ਬਫਰ) == FAIL)
	ਵਾਪਸ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_ਬਫਰ;
    ਜੇ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ਨਹੀਂਤਾਂ ਜੇ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ਨਹੀਂਤਾਂ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_ਬਫਰ;
    ਜੇ (f == FAIL)
	ਵਾਪਸ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ਜੇ ((*flagp & SSOP_FOLDS)
	    && wp->w_ਬਫਰ->b_ffname != ਨਿਰਾ
	    && (bt_ਸਾਧਾਰਨ(wp->w_ਬਫਰ) || bt_help(wp->w_ਬਫਰ)))
    {
	ਜੇ (put_folds(fd, wp) == FAIL)
	    ਵਾਪਸ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ਜੇ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ਜੇ (put_ਸਤਰ(fd, "{") == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (wp->w_ਉਚਾਈ <= 0)
	{
	    ਜੇ (fprintf(fd, "  var l: number = %ld", (ਲੰਮਾ)wp->w_cursor.lnum) < 0)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (ਲੰਮਾ)wp->w_cursor.lnum,
		    (ਲੰਮਾ)(wp->w_cursor.lnum - wp->w_topline),
		    (ਲੰਮਾ)wp->w_ਉਚਾਈ / 2, (ਲੰਮਾ)wp->w_ਉਚਾਈ) < 0)
	    ਵਾਪਸ FAIL;

	ਜੇ (put_eol(fd) == FAIL
		|| put_ਸਤਰ(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_ਸਤਰ(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_ਸਤਰ(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (ਲੰਮਾ)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ਵਾਪਸ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ਜੇ (wp->w_cursor.ਖਾਨਾ == 0)
	{
	    ਜੇ (put_ਸਤਰ(fd, "  normal! 0") == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_ਚੌੜਾਈ > 0)
	    {
		ਜੇ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (ਲੰਮਾ)wp->w_virtcol + 1,
			    (ਲੰਮਾ)(wp->w_virtcol - wp->w_leftcol),
			    (ਲੰਮਾ)wp->w_ਚੌੜਾਈ / 2, (ਲੰਮਾ)wp->w_ਚੌੜਾਈ) < 0
			|| put_eol(fd) == FAIL
			|| put_ਸਤਰ(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (ਲੰਮਾ)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_ਸਤਰ(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_ਸਤਰ(fd, "  endif") == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (put_view_curpos(fd, wp, "  ") == FAIL)
		ਵਾਪਸ FAIL;
	}

	ਜੇ (put_ਸਤਰ(fd, "}") == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ਜੇ (wp->w_localdir != ਨਿਰਾ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ਜੇ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ਵਾਪਸ FAIL;
	did_lcd = TRUE;
    }

    ਵਾਪਸ OK;
}

# ifdef FEAT_EVAL
    ਸਥਿਰ ਪੂਰਨ
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ਪੂਰਨ		todo;
    char_u	*p, *t;

    todo = (ਪੂਰਨ)gvht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(gvht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ਜੇ ((this_var->di_tv.v_type == VAR_ਸੰਖਿਆ
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_ਕੁੰਜੀ) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ਜੇ (p == ਨਿਰਾ)	    // out of memory
		    ਰੋਕੋ;
		ਲਈ (t = p; *t != NUL; ++t)
		    ਜੇ (*t == '\n')
			*t = 'n';
		    ਨਹੀਂਤਾਂ ਜੇ (*t == '\r')
			*t = 'r';
		ਜੇ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_ਕੁੰਜੀ,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ਵਾਪਸ FAIL;
		}
		vim_free(p);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_ਕੁੰਜੀ) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ਪੂਰਨ sign = ' ';

		ਜੇ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ਜੇ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_ਕੁੰਜੀ, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
    }
    ਵਾਪਸ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ਪੂਰਨ		only_save_windows = TRUE;
    ਪੂਰਨ		nr;
    ਪੂਰਨ		restore_ਮਾਪ = TRUE;
    ਪੂਰਨ		restore_ਉਚਾਈ_ਚੌੜਾਈ = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ਨਿਰਾ;
    ਪੂਰਨ		restore_stal = FALSE;
    win_T	*tab_firstwin;
    ਚੌਖਟਾ_T	*tab_topframe;
    ਪੂਰਨ		cur_arg_idx = 0;
    ਪੂਰਨ		ਅਗਲਾ_arg_idx = 0;
    ਪੂਰਨ		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ਜੇ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ਜੇ (put_ਸਤਰ(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	ਜਾਓ fail;

    ਜੇ (put_ਸਤਰ(fd, "doautoall SessionLoadPre") == FAIL)
	ਜਾਓ fail;

    ਜੇ (put_ਸਤਰ(fd, "var save_splitbelow: bool") == FAIL
	    || put_ਸਤਰ(fd, "var save_splitright: bool") == FAIL
	    || put_ਸਤਰ(fd, "var save_winminheight: number") == FAIL
	    || put_ਸਤਰ(fd, "var save_winminwidth: number") == FAIL
	    || put_ਸਤਰ(fd, "var wipebuf: number = -1") == FAIL
	    || put_ਸਤਰ(fd, "var shortmess_save: string") == FAIL)
	ਜਾਓ fail;

    ਜੇ (ssop_flags & SSOP_GLOBALS)
	ਜੇ (store_session_globals(fd) == FAIL)
	    ਜਾਓ fail;
# endif

    // Close all windows and tabs but one.
    ਜੇ (put_ਸਤਰ(fd, "silent only") == FAIL)
	ਜਾਓ fail;
    ਜੇ ((ssop_flags & SSOP_TABPAGES)
	    && put_ਸਤਰ(fd, "silent tabonly") == FAIL)
	ਜਾਓ fail;

    // Now a :cd command to the session directory or the current directory
    ਜੇ (ssop_flags & SSOP_SESDIR)
    {
	ਜੇ (put_ਸਤਰ(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    ਜਾਓ fail;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ਨਿਰਾ, globaldir != ਨਿਰਾ ? globaldir : dirnow);
	ਜੇ (sname == ਨਿਰਾ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    ਜਾਓ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ਜੇ (put_ਸਤਰ(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	ਜਾਓ fail;
    ਜੇ (put_ਸਤਰ(fd, "  wipebuf = bufnr('%')") == FAIL)
	ਜਾਓ fail;
    ਜੇ (put_ਸਤਰ(fd, "endif") == FAIL)
	ਜਾਓ fail;

    // Save 'shortmess' if not storing options.
    ਜੇ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_ਸਤਰ(fd, "shortmess_save = &shortmess") == FAIL)
	ਜਾਓ fail;

    // Set 'shortmess' for the following.
    ਜੇ (put_ਸਤਰ(fd, "set shortmess+=aoO") == FAIL)
	ਜਾਓ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ਜੇ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ਨਿਰਾ
		&& buf->b_p_bl)
	{
	    ਜੇ (fprintf(fd, "badd +%ld ", buf->b_wininfo == ਨਿਰਾ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		ਜਾਓ fail;
	}
    }

    // the global argument list
    ਜੇ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	ਜਾਓ fail;

    ਜੇ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ਜੇ (fprintf(fd, "set lines=%ld columns=%ld" , ਪੰਕਤੀਆਂ, Columns) < 0
		|| put_eol(fd) == FAIL)
	    ਜਾਓ fail;
    }

# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use && (ssop_flags & SSOP_WINPOS))
    {
	ਪੂਰਨ	x, y;

	ਜੇ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ਜੇ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		ਜਾਓ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ਜੇ (p_stal == 1 && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
    {
	ਜੇ (put_ਸਤਰ(fd, "set stal=2") == FAIL)
	    ਜਾਓ fail;
	restore_stal = TRUE;
    }

    ਜੇ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ਜੇ (tp->tp_ਅਗਲਾ != ਨਿਰਾ
		  && put_ਸਤਰ(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		ਜਾਓ fail;
	ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ && put_ਸਤਰ(fd, "tabrewind") == FAIL)
	    ਜਾਓ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ਪੂਰਨ	need_tabnext = FALSE;
	ਪੂਰਨ	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ਜੇ ((ssop_flags & SSOP_TABPAGES))
	{
	    ਜੇ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ਜੇ (tp != ਪਹਿਲਾ_tabpage)
		need_tabnext = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	ਲਈ (wp = tab_firstwin; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	{
	    ਜੇ (ses_do_win(wp)
		    && wp->w_ਬਫਰ->b_ffname != ਨਿਰਾ
		    && !bt_help(wp->w_ਬਫਰ)
		    && !bt_nofilename(wp->w_ਬਫਰ))
	    {
		ਜੇ (need_tabnext && put_ਸਤਰ(fd, "tabnext") == FAIL)
		    ਜਾਓ fail;
		need_tabnext = FALSE;

		ਜੇ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_ਬਫਰ, &ssop_flags, TRUE)
								       == FAIL)
		    ਜਾਓ fail;
		ਜੇ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ਰੋਕੋ;
	    }
	}

	// If no file got edited create an empty tab page.
	ਜੇ (need_tabnext && put_ਸਤਰ(fd, "tabnext") == FAIL)
	    ਜਾਓ fail;

	ਜੇ (tab_topframe->fr_layout != FR_ਪੱਤੀ)
	{
	    // Save current window layout.
	    ਜੇ (put_ਸਤਰ(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_ਸਤਰ(fd, "save_splitright = &splitright")
								       == FAIL)
		ਜਾਓ fail;
	    ਜੇ (put_ਸਤਰ(fd, "set splitbelow splitright") == FAIL)
		ਜਾਓ fail;
	    ਜੇ (ses_win_rec(fd, tab_topframe) == FAIL)
		ਜਾਓ fail;
	    ਜੇ (put_ਸਤਰ(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_ਸਤਰ(fd, "&splitright = save_splitright")
								       == FAIL)
		ਜਾਓ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	ਲਈ (wp = tab_firstwin; wp != ਨਿਰਾ; wp = W_ਅਗਲਾ(wp))
	{
	    ਜੇ (ses_do_win(wp))
		++nr;
	    ਨਹੀਂਤਾਂ
		restore_ਮਾਪ = FALSE;
	    ਜੇ (curwin == wp)
		cnr = nr;
	}

	ਜੇ (tab_firstwin->w_ਅਗਲਾ != ਨਿਰਾ)
	{
	    // Go to the first window.
	    ਜੇ (put_ਸਤਰ(fd, "wincmd t") == FAIL)
		ਜਾਓ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ਜੇ (!restore_ਉਚਾਈ_ਚੌੜਾਈ
		    && (put_ਸਤਰ(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_ਸਤਰ(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		ਜਾਓ fail;
	    ਜੇ (put_ਸਤਰ(fd, "set winminheight=0") == FAIL
		    || put_ਸਤਰ(fd, "set winheight=1") == FAIL
		    || put_ਸਤਰ(fd, "set winminwidth=0") == FAIL
		    || put_ਸਤਰ(fd, "set winwidth=1") == FAIL)
		ਜਾਓ fail;
	    restore_ਉਚਾਈ_ਚੌੜਾਈ = TRUE;
	}
	ਜੇ (nr > 1 && ses_winsizes(fd, restore_ਮਾਪ, tab_firstwin) == FAIL)
	    ਜਾਓ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ਜੇ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ਨਿਰਾ)
	{
	    ਜੇ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		ਜਾਓ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	ਲਈ (wp = tab_firstwin; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	{
	    ਜੇ (!ses_do_win(wp))
		ਜਾਰੀ;
	    ਜੇ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ਨਹੀਂਤਾਂ
							 ਨਿਰਾ
# endif
		 ) == FAIL)
		ਜਾਓ fail;
	    ਜੇ (nr > 1 && put_ਸਤਰ(fd, "wincmd w") == FAIL)
		ਜਾਓ fail;
	    ਅਗਲਾ_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = ਅਗਲਾ_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ਜੇ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    ਜਾਓ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ਜੇ (nr > 1 && ses_winsizes(fd, restore_ਮਾਪ, tab_firstwin) == FAIL)
	    ਜਾਓ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ਜੇ (!(ssop_flags & SSOP_TABPAGES))
	    ਰੋਕੋ;
    }

    ਜੇ (ssop_flags & SSOP_TABPAGES)
    {
	ਜੇ (fprintf(fd, "tabnext %d", tabpage_ਸੂਚਕਾਂਕ(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    ਜਾਓ fail;
    }
    ਜੇ (restore_stal && put_ਸਤਰ(fd, "set stal=1") == FAIL)
	ਜਾਓ fail;

    // Wipe out an empty unnamed buffer we started in.
    ਜੇ (put_ਸਤਰ(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	ਜਾਓ fail;
    ਜੇ (put_ਸਤਰ(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	ਜਾਓ fail;
    ਜੇ (put_ਸਤਰ(fd, "endif") == FAIL)
	ਜਾਓ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ਜੇ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	ਜਾਓ fail;

    // Restore 'shortmess'.
    ਜੇ (ssop_flags & SSOP_OPTIONS)
    {
	ਜੇ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    ਜਾਓ fail;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (put_ਸਤਰ(fd, "&shortmess = shortmess_save") == FAIL)
	    ਜਾਓ fail;
    }

    ਜੇ (restore_ਉਚਾਈ_ਚੌੜਾਈ)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ਜੇ (put_ਸਤਰ(fd, "&winminheight = save_winminheight") == FAIL
	      || put_ਸਤਰ(fd, "&winminwidth = save_winminwidth") == FAIL)
	    ਜਾਓ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ਜੇ (put_ਸਤਰ(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_ਸਤਰ(fd, "if filereadable(sx)") == FAIL
	    || put_ਸਤਰ(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_ਸਤਰ(fd, "endif") == FAIL)
	ਜਾਓ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ਵਾਪਸ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ਸਥਿਰ char_u *
get_view_file(ਪੂਰਨ c)
{
    ਪੂਰਨ		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
    {
	emsg(_(e_no_file_name));
	ਵਾਪਸ ਨਿਰਾ;
    }
    sname = home_replace_save(ਨਿਰਾ, curbuf->b_ffname);
    ਜੇ (sname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    ਲਈ (p = sname; *p; ++p)
	ਜੇ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ਜੇ (retval != ਨਿਰਾ)
    {
	STRCPY(retval, p_vdir);
	ਜੋੜੋ_pathsep(retval);
	s = retval + STRLEN(retval);
	ਲਈ (p = sname; *p; ++p)
	{
	    ਜੇ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME) || defined(AMIGA) || defined(VMS)
		ਜੇ (*p == ':')
		    *s++ = '-';
		ਨਹੀਂਤਾਂ
# endif
		    *s++ = '+';
	    }
	    ਨਹੀਂਤਾਂ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ਵਾਪਸ retval;
}

/*
 * ":loadview [nr]"
 */
    ਖਾਲੀ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    (ਖਾਲੀ)do_ਸਰੋਤ(fname, FALSE, DOSO_NONE, ਨਿਰਾ);
    vim_free(fname);
}

# ਜੇ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ਪੂਰਨ
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    ਅੱਖਰ	    *mksession_cmdline;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    save_ssop_flags;
    ਪੂਰਨ		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ਜੇ (escaped_filename == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ਜੇ (mksession_cmdline == ਨਿਰਾ)
    {
	vim_free(escaped_filename);
	ਵਾਪਸ FALSE;
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
    ਜੇ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ਨਿਰਾ
	       || put_ਸਤਰ(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_ਸਤਰ(fd, "unlet g:Save_VV_this_session") == FAIL);

	ਜੇ (fd != ਨਿਰਾ && fclose(fd) != 0)
	    failed = TRUE;

	ਜੇ (failed)
	    mch_remove(filename);
    }

    ਵਾਪਸ !failed;
}
# endif

#ਜੇ_ਅੰਤ // FEAT_SESSION

#ਜੇ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ਸਥਿਰ ਪੂਰਨ mksession_nl = FALSE;    // use NL only in put_eol()
#ਜੇ_ਅੰਤ

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    ਖਾਲੀ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ਪੂਰਨ		failed = FALSE;
    char_u	*fname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    char_u	*browseFile = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
    ਪੂਰਨ		view_session = FALSE;
    ਪੂਰਨ		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ਨਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	*flagp;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ		sid;
    scriptitem_T *si = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	view_session = TRUE;
#ਨਹੀਂਤਾਂ
	ex_ni(eap);
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ਜੇ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ਜੇ (fname == ਨਿਰਾ)
	    ਵਾਪਸ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (*eap->arg != NUL)
	fname = eap->arg;
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
	fname = (char_u *)EXRC_FILE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ਨਿਰਾ,
		(char_u *)_(BROWSE_FILTER_MACROS), ਨਿਰਾ);
	ਜੇ (browseFile == ਨਿਰਾ)
	    ਜਾਓ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ਜੇ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#ਜੇ_ਅੰਤ

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ਜੇ (fd != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	ਜੇ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ਨਹੀਂਤਾਂ
	    flagp = &ssop_flags;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ਜੇ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#ਜੇ_ਅੰਤ

	// Enforce vim9script
	ਜੇ (put_ਸਤਰ(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ਜੇ (eap->cmdidx == CMD_mkvimrc)
	    (ਖਾਲੀ)put_ਸਤਰ(fd, "version 6.0");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	ਜੇ (eap->cmdidx == CMD_mksession)
	{
	    ਜੇ (put_ਸਤਰ(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ਜੇ (eap->cmdidx != CMD_mkview)
#ਜੇ_ਅੰਤ
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ਜੇ (p_cp)
		(ਖਾਲੀ)put_ਸਤਰ(fd, "if !&cp | set cp | endif");
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)put_ਸਤਰ(fd, "if &cp | set nocp | endif");
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	ਜੇ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#ਜੇ_ਅੰਤ
	{
	    bool do_mappings = true;
	    ਪੂਰਨ flags = OPT_GLOBAL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	    failed |= put_ਸਤਰ(fd, "var cpo_save: string") == FAIL;

	    ਜੇ (eap->cmdidx == CMD_mksession)
	    {
		ਜੇ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#ਜੇ_ਅੰਤ

	    ਜੇ (do_mappings)
		failed |= (makemap(fd, ਨਿਰਾ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ਜੇ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    ਲਈ (sid = 1; sid <= script_ਚੀਜ਼ਾਂ.ga_len; ++sid)
	    {
		si = SCRIPT_ਵਸਤੂ(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ਜੇ ((si->sn_autoload_ਅਗੇਤਰ || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ਪੂਰਨ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    ਲਈ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ਵਸਤੂ(b_sid);

			// Only autoload may conflict. Ignore if missing
			ਜੇ ((!b_si->sn_autoload_ਅਗੇਤਰ && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ਜਾਰੀ;

			// compare prefixes if available
			ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ && b_si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ
				&& (STRCMP(si->sn_autoload_ਅਗੇਤਰ, b_si->sn_autoload_ਅਗੇਤਰ) == 0))
			    ਰੋਕੋ;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ਜੇ (STRCMP(name, b_name) == 0)
			    ਰੋਕੋ;
		    }

		    // import the auto script if there are no conflicts
		    ਜੇ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#ਜੇ_ਅੰਤ
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	ਜੇ (!failed && view_session)
	{
	    ਜੇ (put_ਸਤਰ(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ਜੇ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ਜੇ (dirnow == ਨਿਰਾ)
		    failed = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    // Change to session file's dir.
		    ਜੇ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((ਅੱਖਰ *)dirnow) != 0)
			*dirnow = NUL;
		    ਜੇ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ਜੇ (vim_chdirfile(fname, ਨਿਰਾ) == OK)
			    shorten_fnames(TRUE);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ਨਿਰਾ)
		    {
			ਜੇ (mch_chdir((ਅੱਖਰ *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ਜੇ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ਨਿਰਾ)))
		    {
			ਜੇ (mch_chdir((ਅੱਖਰ *)dirnow) != 0)
			    emsg(_(e_cannot_go_ਪਿੱਠ_ਮੰਜ਼ਿਲ_ਪਿਛਲਾ_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		failed |= put_ਸਤਰ(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ਨਿਰਾ) == FAIL);
	    }
	    ਜੇ (put_ਸਤਰ(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_ਖੋਜ_EXTRA
	    ਜੇ (no_hlsearch && put_ਸਤਰ(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ਜੇ (put_ਸਤਰ(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ਜੇ (eap->cmdidx == CMD_mksession)
	    {
		ਜੇ (put_ਸਤਰ(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#ਜੇ_ਅੰਤ

	ਜੇ (put_ਸਤਰ(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ਜੇ (failed)
	    emsg(_(e_error_while_writing));
#ਜੇ defined(FEAT_SESSION)
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ਜੇ (tbuf != ਨਿਰਾ)
	    {
		ਜੇ (vim_ਭਰਿਆ_Name(fname, tbuf, MAXPATHL, FALSE) == OK)
		    ਸੈੱਟ_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MKSESSION_NL
	mksession_nl = FALSE;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
theend:
    vim_free(browseFile);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
    vim_free(viewFile);
#ਜੇ_ਅੰਤ

    apply_autocmds(EVENT_SESSIONWRITEPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
}

#ਜੇ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ਜੇ (ASCII_ISUPPER(*p))
    {
	ਜਦੋਂ (*(++p))
	    ਜੇ (ASCII_ISLOWER(*p))
		ਵਾਪਸ VAR_FLAVOUR_SESSION;
	ਵਾਪਸ VAR_FLAVOUR_VIMINFO;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ VAR_FLAVOUR_DEFAULT;
}
#ਜੇ_ਅੰਤ

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ਪੂਰਨ
put_eol(FILE *fd)
{
    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#ਜੇ_ਅੰਤ
	    (putc('\n', fd) < 0))
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ਪੂਰਨ
put_ਸਤਰ(FILE *fd, ਅੱਖਰ *s)
{
    ਜੇ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}
