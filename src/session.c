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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SESSION)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_put_fname(FILE *fd, char_u *name, ಚಿಹ್ನೆರಹಿತ *flagp)
{
    char_u	*sname;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		retval = OK;

    sname = home_replace_save(ಖಾಲಿ, name);
    ಆದರೆ (sname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	ಗಾಗಿ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ಆದರೆ (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // write the result
    ಆದರೆ (fputs((ಅಕ್ಷರ *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_fname(FILE *fd, buf_T *buf, ಚಿಹ್ನೆರಹಿತ *flagp, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ಆದರೆ (buf->b_sfname != ಖಾಲಿ
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ಇಲ್ಲದಿದ್ದರೆ
	name = buf->b_ffname;
    ಆದರೆ (ses_put_fname(fd, name, flagp) == FAIL
	    || (ಕೂಡಿಸು_eol && put_eol(fd) == FAIL))
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_arglist(
    FILE	*fd,
    ಅಕ್ಷರ	*cmd,
    garray_T	*gap,
    ಪೂರ್ಣಾಂಕ		fullname,	// TRUE: use full path name
    ಚಿಹ್ನೆರಹಿತ	*flagp)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*buf = ಖಾಲಿ;
    char_u	*s;

    ಆದರೆ (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (put_ಸಾಲು(fd, ":%argdel") == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_ದತ್ತಾಂಶ)[i]);
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    ಆದರೆ (fullname)
	    {
		buf = alloc(MAXPATHL);
		ಆದರೆ (buf != ಖಾಲಿ)
		{
		    (ಶೂನ್ಯ)vim_ತುಂಬಿದ_Name(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ಆದರೆ (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    vim_free(buf);
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ಆದರೆ (bt_terminal(wp->w_ಬಫರ್))
	ಹಿಂತಿರುಗಿ !ಪದ_is_ಪೂರ್ಣಗೊಂಡ(wp->w_ಬಫರ್)
	    && (ssop_flags & SSOP_TERMINAL)
	    && ಪದ_should_restore(wp->w_ಬಫರ್);
# endif
    ಆದರೆ (wp->w_ಬಫರ್->b_fname == ಖಾಲಿ
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_ಬಫರ್))
	ಹಿಂತಿರುಗಿ (ssop_flags & SSOP_BLANK);
    ಆದರೆ (bt_help(wp->w_ಬಫರ್))
	ಹಿಂತಿರುಗಿ (ssop_flags & SSOP_HELP);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_do_ಚೌಕಟ್ಟು(ಚೌಕಟ್ಟು_T *fr)
{
    ಚೌಕಟ್ಟು_T	*frc;

    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ)
	ಹಿಂತಿರುಗಿ ses_do_win(fr->fr_win);
    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frc, fr->fr_ಮಗು)
	ಆದರೆ (ses_do_ಚೌಕಟ್ಟು(frc))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ಸ್ಥಿರ ಚೌಕಟ್ಟು_T *
ses_skipframe(ಚೌಕಟ್ಟು_T *fr)
{
    ಚೌಕಟ್ಟು_T	*frc;

    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frc, fr)
	ಆದರೆ (ses_do_ಚೌಕಟ್ಟು(frc))
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_win_rec(FILE *fd, ಚೌಕಟ್ಟು_T *fr)
{
    ಚೌಕಟ್ಟು_T	*frc;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ)
	ಹಿಂತಿರುಗಿ OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_ಮಗು);
    ಆದರೆ (frc != ಖಾಲಿ)
	ಆಗ ((frc = ses_skipframe(frc->fr_ಮುಂದಿನ)) != ಖಾಲಿ)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ಆದರೆ (put_ಸಾಲು(fd, "wincmd _ | wincmd |") == FAIL
		    || put_ಸಾಲು(fd, fr->fr_layout == FR_ಲಂಬಸಾಲು
			? "split" : "vsplit") == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ++ಎಣಿಕೆ_2;
	}

    // Go back to the first window.
    ಆದರೆ (ಎಣಿಕೆ_2 > 0 && (fprintf(fd, fr->fr_layout == FR_ಲಂಬಸಾಲು
		    ? ":%dwincmd k" : ":%dwincmd h", ಎಣಿಕೆ_2) < 0
		|| put_eol(fd) == FAIL))
	ಹಿಂತಿರುಗಿ FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_ಮಗು);
    ಆಗ (frc != ಖಾಲಿ)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_ಮುಂದಿನ);
	// Go to next window.
	ಆದರೆ (frc != ಖಾಲಿ && put_ಸಾಲು(fd, "wincmd w") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ses_winsizes(
    FILE	*fd,
    ಪೂರ್ಣಾಂಕ		restore_ಗಾತ್ರ,
    win_T	*tab_firstwin)
{
    ಪೂರ್ಣಾಂಕ		n = 0;
    win_T	*wp;

    ಆದರೆ (restore_ಗಾತ್ರ && (ssop_flags & SSOP_WINSIZE))
    {
	ಗಾಗಿ (wp = tab_firstwin; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	{
	    ಆದರೆ (!ses_do_win(wp))
		ಮುಂದುವರಿ;
	    ++n;

	    // restore height when not full height
	    ಆದರೆ (wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ < topframe->fr_ಎತ್ತರ
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (ದೀರ್ಘ)wp->w_ಎತ್ತರ, ಅಡ್ಡಸಾಲುಗಳು / 2, ಅಡ್ಡಸಾಲುಗಳು) < 0
						  || put_eol(fd) == FAIL))
		ಹಿಂತಿರುಗಿ FAIL;

	    // restore width when not full width
	    ಆದರೆ (wp->w_ಅಗಲ < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (ದೀರ್ಘ)wp->w_ಅಗಲ, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Just equalise window sizes
	ಆದರೆ (put_ಸಾಲು(fd, "wincmd =") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_view_curpos(FILE *fd, win_T *wp, ಅಕ್ಷರ *spaces)
{
    ಪೂರ್ಣಾಂಕ r;

    ಆದರೆ (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ಇಲ್ಲದಿದ್ದರೆ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    ಹಿಂತಿರುಗಿ r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_edit,	     // add ":edit" command to view
    ಚಿಹ್ನೆರಹಿತ	*flagp,		     // vop_flags or ssop_flags
    ಪೂರ್ಣಾಂಕ		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs ಬಳಸದ) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    ಪೂರ್ಣಾಂಕ		f;
    ಪೂರ್ಣಾಂಕ		do_cursor;
    ಪೂರ್ಣಾಂಕ		did_ಮುಂದಿನ = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ಆದರೆ (wp->w_alist == &global_alist)
    {
	ಆದರೆ (put_ಸಾಲು(fd, "argglobal") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != ಖಾಲಿ
			|| wp->w_localdir != ಖಾಲಿ, flagp) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ಆದರೆ (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ಆದರೆ (fprintf(fd, ":%ldargu", (ದೀರ್ಘ)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	did_ಮುಂದಿನ = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ಆದರೆ (ಕೂಡಿಸು_edit && (!did_ಮುಂದಿನ || wp->w_arg_idx_invalid))
    {
	ಆದರೆ (bt_help(wp->w_ಬಫರ್))
	{
	    ಅಕ್ಷರ *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ಆದರೆ (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (ಅಕ್ಷರ *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ಆದರೆ (put_ಸಾಲು(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
# ifdef FEAT_TERMINAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bt_terminal(wp->w_ಬಫರ್))
	{
	    ಆದರೆ (ಪದ_write_session(fd, wp, terminal_bufs) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
# endif
	// Load the file.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಬಫರ್->b_ffname != ಖಾಲಿ
		&& !bt_nofilename(wp->w_ಬಫರ್))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ಆದರೆ (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_ಬಫರ್, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_ಬಫರ್, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_ಬಫರ್, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // No file in this buffer, just make it empty.
	    ಆದರೆ (put_ಸಾಲು(fd, "enew") == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
# ifdef FEAT_QUICKFIX
	    ಆದರೆ (wp->w_ಬಫರ್->b_ffname != ಖಾಲಿ)
	    {
		// The buffer does have a name, but it's not a file name.
		ಆದರೆ (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_ಬಫರ್, flagp, TRUE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ಆದರೆ (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ಆದರೆ ((flagp == &ssop_flags)
		&& alt != ಖಾಲಿ
		&& alt->b_fname != ಖಾಲಿ
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Local mappings and abbreviations.
    ಆದರೆ ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_ಬಫರ್) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_ಬಫರ್;
    ಆದರೆ (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ಇಲ್ಲದಿದ್ದರೆ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_ಬಫರ್;
    ಆದರೆ (f == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ಆದರೆ ((*flagp & SSOP_FOLDS)
	    && wp->w_ಬಫರ್->b_ffname != ಖಾಲಿ
	    && (bt_ಸಾಮಾನ್ಯ(wp->w_ಬಫರ್) || bt_help(wp->w_ಬಫರ್)))
    {
	ಆದರೆ (put_folds(fd, wp) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ಆದರೆ (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ಆದರೆ (put_ಸಾಲು(fd, "{") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (wp->w_ಎತ್ತರ <= 0)
	{
	    ಆದರೆ (fprintf(fd, "  var l: number = %ld", (ದೀರ್ಘ)wp->w_cursor.lnum) < 0)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (ದೀರ್ಘ)wp->w_cursor.lnum,
		    (ದೀರ್ಘ)(wp->w_cursor.lnum - wp->w_topline),
		    (ದೀರ್ಘ)wp->w_ಎತ್ತರ / 2, (ದೀರ್ಘ)wp->w_ಎತ್ತರ) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (put_eol(fd) == FAIL
		|| put_ಸಾಲು(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_ಸಾಲು(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_ಸಾಲು(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (ದೀರ್ಘ)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ಆದರೆ (wp->w_cursor.ಲಂಬಸಾಲು == 0)
	{
	    ಆದರೆ (put_ಸಾಲು(fd, "  normal! 0") == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_ಅಗಲ > 0)
	    {
		ಆದರೆ (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (ದೀರ್ಘ)wp->w_virtcol + 1,
			    (ದೀರ್ಘ)(wp->w_virtcol - wp->w_leftcol),
			    (ದೀರ್ಘ)wp->w_ಅಗಲ / 2, (ದೀರ್ಘ)wp->w_ಅಗಲ) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (ದೀರ್ಘ)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_ಸಾಲು(fd, "  endif") == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (put_view_curpos(fd, wp, "  ") == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (put_ಸಾಲು(fd, "}") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ಆದರೆ (wp->w_localdir != ಖಾಲಿ
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ಆದರೆ (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	did_lcd = TRUE;
    }

    ಹಿಂತಿರುಗಿ OK;
}

# ifdef FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ಪೂರ್ಣಾಂಕ		todo;
    char_u	*p, *t;

    todo = (ಪೂರ್ಣಾಂಕ)gvht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(gvht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ಆದರೆ ((this_var->di_tv.v_type == VAR_ಸಂಖ್ಯೆ
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ಆದರೆ (p == ಖಾಲಿ)	    // out of memory
		    ನಿಲ್ಲಿಸು;
		ಗಾಗಿ (t = p; *t != NUL; ++t)
		    ಆದರೆ (*t == '\n')
			*t = 'n';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*t == '\r')
			*t = 'r';
		ಆದರೆ ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_ಕೀಲಿ,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		vim_free(p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		ಪೂರ್ಣಾಂಕ sign = ' ';

		ಆದರೆ (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ಆದರೆ ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_ಕೀಲಿ, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		only_save_windows = TRUE;
    ಪೂರ್ಣಾಂಕ		nr;
    ಪೂರ್ಣಾಂಕ		restore_ಗಾತ್ರ = TRUE;
    ಪೂರ್ಣಾಂಕ		restore_ಎತ್ತರ_ಅಗಲ = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		restore_stal = FALSE;
    win_T	*tab_firstwin;
    ಚೌಕಟ್ಟು_T	*tab_topframe;
    ಪೂರ್ಣಾಂಕ		cur_arg_idx = 0;
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_arg_idx = 0;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ಆದರೆ (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ಆದರೆ (put_ಸಾಲು(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	ಹೋಗು fail;

    ಆದರೆ (put_ಸಾಲು(fd, "doautoall SessionLoadPre") == FAIL)
	ಹೋಗು fail;

    ಆದರೆ (put_ಸಾಲು(fd, "var save_splitbelow: bool") == FAIL
	    || put_ಸಾಲು(fd, "var save_splitright: bool") == FAIL
	    || put_ಸಾಲು(fd, "var save_winminheight: number") == FAIL
	    || put_ಸಾಲು(fd, "var save_winminwidth: number") == FAIL
	    || put_ಸಾಲು(fd, "var wipebuf: number = -1") == FAIL
	    || put_ಸಾಲು(fd, "var shortmess_save: string") == FAIL)
	ಹೋಗು fail;

    ಆದರೆ (ssop_flags & SSOP_GLOBALS)
	ಆದರೆ (store_session_globals(fd) == FAIL)
	    ಹೋಗು fail;
# endif

    // Close all windows and tabs but one.
    ಆದರೆ (put_ಸಾಲು(fd, "silent only") == FAIL)
	ಹೋಗು fail;
    ಆದರೆ ((ssop_flags & SSOP_TABPAGES)
	    && put_ಸಾಲು(fd, "silent tabonly") == FAIL)
	ಹೋಗು fail;

    // Now a :cd command to the session directory or the current directory
    ಆದರೆ (ssop_flags & SSOP_SESDIR)
    {
	ಆದರೆ (put_ಸಾಲು(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    ಹೋಗು fail;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(ಖಾಲಿ, globaldir != ಖಾಲಿ ? globaldir : dirnow);
	ಆದರೆ (sname == ಖಾಲಿ
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    ಹೋಗು fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    ಆದರೆ (put_ಸಾಲು(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	ಹೋಗು fail;
    ಆದರೆ (put_ಸಾಲು(fd, "  wipebuf = bufnr('%')") == FAIL)
	ಹೋಗು fail;
    ಆದರೆ (put_ಸಾಲು(fd, "endif") == FAIL)
	ಹೋಗು fail;

    // Save 'shortmess' if not storing options.
    ಆದರೆ ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_ಸಾಲು(fd, "shortmess_save = &shortmess") == FAIL)
	ಹೋಗು fail;

    // Set 'shortmess' for the following.
    ಆದರೆ (put_ಸಾಲು(fd, "set shortmess+=aoO") == FAIL)
	ಹೋಗು fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ಆದರೆ (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != ಖಾಲಿ
		&& buf->b_p_bl)
	{
	    ಆದರೆ (fprintf(fd, "badd +%ld ", buf->b_wininfo == ಖಾಲಿ ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		ಹೋಗು fail;
	}
    }

    // the global argument list
    ಆದರೆ (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	ಹೋಗು fail;

    ಆದರೆ (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ಆದರೆ (fprintf(fd, "set lines=%ld columns=%ld" , ಅಡ್ಡಸಾಲುಗಳು, Columns) < 0
		|| put_eol(fd) == FAIL)
	    ಹೋಗು fail;
    }

# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use && (ssop_flags & SSOP_WINPOS))
    {
	ಪೂರ್ಣಾಂಕ	x, y;

	ಆದರೆ (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ಆದರೆ (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		ಹೋಗು fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ಆದರೆ (p_stal == 1 && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
    {
	ಆದರೆ (put_ಸಾಲು(fd, "set stal=2") == FAIL)
	    ಹೋಗು fail;
	restore_stal = TRUE;
    }

    ಆದರೆ ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ಆದರೆ (tp->tp_ಮುಂದಿನ != ಖಾಲಿ
		  && put_ಸಾಲು(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		ಹೋಗು fail;
	ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ && put_ಸಾಲು(fd, "tabrewind") == FAIL)
	    ಹೋಗು fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	ಪೂರ್ಣಾಂಕ	need_tabnext = FALSE;
	ಪೂರ್ಣಾಂಕ	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ಆದರೆ ((ssop_flags & SSOP_TABPAGES))
	{
	    ಆದರೆ (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ಆದರೆ (tp != ಮೊದಲ_tabpage)
		need_tabnext = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	ಗಾಗಿ (wp = tab_firstwin; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	{
	    ಆದರೆ (ses_do_win(wp)
		    && wp->w_ಬಫರ್->b_ffname != ಖಾಲಿ
		    && !bt_help(wp->w_ಬಫರ್)
		    && !bt_nofilename(wp->w_ಬಫರ್))
	    {
		ಆದರೆ (need_tabnext && put_ಸಾಲು(fd, "tabnext") == FAIL)
		    ಹೋಗು fail;
		need_tabnext = FALSE;

		ಆದರೆ (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_ಬಫರ್, &ssop_flags, TRUE)
								       == FAIL)
		    ಹೋಗು fail;
		ಆದರೆ (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// If no file got edited create an empty tab page.
	ಆದರೆ (need_tabnext && put_ಸಾಲು(fd, "tabnext") == FAIL)
	    ಹೋಗು fail;

	ಆದರೆ (tab_topframe->fr_layout != FR_ಎಲೆ)
	{
	    // Save current window layout.
	    ಆದರೆ (put_ಸಾಲು(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_ಸಾಲು(fd, "save_splitright = &splitright")
								       == FAIL)
		ಹೋಗು fail;
	    ಆದರೆ (put_ಸಾಲು(fd, "set splitbelow splitright") == FAIL)
		ಹೋಗು fail;
	    ಆದರೆ (ses_win_rec(fd, tab_topframe) == FAIL)
		ಹೋಗು fail;
	    ಆದರೆ (put_ಸಾಲು(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_ಸಾಲು(fd, "&splitright = save_splitright")
								       == FAIL)
		ಹೋಗು fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	ಗಾಗಿ (wp = tab_firstwin; wp != ಖಾಲಿ; wp = W_ಮುಂದಿನ(wp))
	{
	    ಆದರೆ (ses_do_win(wp))
		++nr;
	    ಇಲ್ಲದಿದ್ದರೆ
		restore_ಗಾತ್ರ = FALSE;
	    ಆದರೆ (curwin == wp)
		cnr = nr;
	}

	ಆದರೆ (tab_firstwin->w_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    // Go to the first window.
	    ಆದರೆ (put_ಸಾಲು(fd, "wincmd t") == FAIL)
		ಹೋಗು fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ಆದರೆ (!restore_ಎತ್ತರ_ಅಗಲ
		    && (put_ಸಾಲು(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_ಸಾಲು(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		ಹೋಗು fail;
	    ಆದರೆ (put_ಸಾಲು(fd, "set winminheight=0") == FAIL
		    || put_ಸಾಲು(fd, "set winheight=1") == FAIL
		    || put_ಸಾಲು(fd, "set winminwidth=0") == FAIL
		    || put_ಸಾಲು(fd, "set winwidth=1") == FAIL)
		ಹೋಗು fail;
	    restore_ಎತ್ತರ_ಅಗಲ = TRUE;
	}
	ಆದರೆ (nr > 1 && ses_winsizes(fd, restore_ಗಾತ್ರ, tab_firstwin) == FAIL)
	    ಹೋಗು fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ಆದರೆ ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != ಖಾಲಿ)
	{
	    ಆದರೆ (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		ಹೋಗು fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	ಗಾಗಿ (wp = tab_firstwin; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	{
	    ಆದರೆ (!ses_do_win(wp))
		ಮುಂದುವರಿ;
	    ಆದರೆ (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ಇಲ್ಲದಿದ್ದರೆ
							 ಖಾಲಿ
# endif
		 ) == FAIL)
		ಹೋಗು fail;
	    ಆದರೆ (nr > 1 && put_ಸಾಲು(fd, "wincmd w") == FAIL)
		ಹೋಗು fail;
	    ಮುಂದಿನ_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = ಮುಂದಿನ_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ಆದರೆ (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    ಹೋಗು fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ಆದರೆ (nr > 1 && ses_winsizes(fd, restore_ಗಾತ್ರ, tab_firstwin) == FAIL)
	    ಹೋಗು fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ಆದರೆ (!(ssop_flags & SSOP_TABPAGES))
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ssop_flags & SSOP_TABPAGES)
    {
	ಆದರೆ (fprintf(fd, "tabnext %d", tabpage_ಸೂಚ್ಯಂಕ(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    ಹೋಗು fail;
    }
    ಆದರೆ (restore_stal && put_ಸಾಲು(fd, "set stal=1") == FAIL)
	ಹೋಗು fail;

    // Wipe out an empty unnamed buffer we started in.
    ಆದರೆ (put_ಸಾಲು(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	ಹೋಗು fail;
    ಆದರೆ (put_ಸಾಲು(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	ಹೋಗು fail;
    ಆದರೆ (put_ಸಾಲು(fd, "endif") == FAIL)
	ಹೋಗು fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ಆದರೆ (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	ಹೋಗು fail;

    // Restore 'shortmess'.
    ಆದರೆ (ssop_flags & SSOP_OPTIONS)
    {
	ಆದರೆ (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    ಹೋಗು fail;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (put_ಸಾಲು(fd, "&shortmess = shortmess_save") == FAIL)
	    ಹೋಗು fail;
    }

    ಆದರೆ (restore_ಎತ್ತರ_ಅಗಲ)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ಆದರೆ (put_ಸಾಲು(fd, "&winminheight = save_winminheight") == FAIL
	      || put_ಸಾಲು(fd, "&winminwidth = save_winminwidth") == FAIL)
	    ಹೋಗು fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ಆದರೆ (put_ಸಾಲು(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_ಸಾಲು(fd, "if filereadable(sx)") == FAIL
	    || put_ಸಾಲು(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_ಸಾಲು(fd, "endif") == FAIL)
	ಹೋಗು fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ಸ್ಥಿರ char_u *
get_view_file(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
    {
	emsg(_(e_no_file_name));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    sname = home_replace_save(ಖಾಲಿ, curbuf->b_ffname);
    ಆದರೆ (sname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    ಗಾಗಿ (p = sname; *p; ++p)
	ಆದರೆ (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ಆದರೆ (retval != ಖಾಲಿ)
    {
	STRCPY(retval, p_vdir);
	ಕೂಡಿಸು_pathsep(retval);
	s = retval + STRLEN(retval);
	ಗಾಗಿ (p = sname; *p; ++p)
	{
	    ಆದರೆ (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME) || defined(AMIGA) || defined(VMS)
		ಆದರೆ (*p == ':')
		    *s++ = '-';
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		    *s++ = '+';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * ":loadview [nr]"
 */
    ಶೂನ್ಯ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    (ಶೂನ್ಯ)do_ಮೂಲ(fname, FALSE, DOSO_NONE, ಖಾಲಿ);
    vim_free(fname);
}

# ಆದರೆ defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    ಪೂರ್ಣಾಂಕ
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    ಅಕ್ಷರ	    *mksession_cmdline;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    save_ssop_flags;
    ಪೂರ್ಣಾಂಕ		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ಆದರೆ (escaped_filename == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ಆದರೆ (mksession_cmdline == ಖಾಲಿ)
    {
	vim_free(escaped_filename);
	ಹಿಂತಿರುಗಿ FALSE;
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
    ಆದರೆ (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == ಖಾಲಿ
	       || put_ಸಾಲು(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_ಸಾಲು(fd, "unlet g:Save_VV_this_session") == FAIL);

	ಆದರೆ (fd != ಖಾಲಿ && fclose(fd) != 0)
	    failed = TRUE;

	ಆದರೆ (failed)
	    mch_remove(filename);
    }

    ಹಿಂತಿರುಗಿ !failed;
}
# endif

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SESSION

#ಆದರೆ defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mksession_nl = FALSE;    // use NL only in put_eol()
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    ಶೂನ್ಯ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;
    char_u	*fname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    char_u	*browseFile = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
    ಪೂರ್ಣಾಂಕ		view_session = FALSE;
    ಪೂರ್ಣಾಂಕ		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = ಖಾಲಿ;
    ಚಿಹ್ನೆರಹಿತ	*flagp;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ		sid;
    scriptitem_T *si = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	view_session = TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
	ex_ni(eap);
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ಆದರೆ (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ಆದರೆ (fname == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (*eap->arg != NUL)
	fname = eap->arg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
	fname = (char_u *)EXRC_FILE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", ಖಾಲಿ,
		(char_u *)_(BROWSE_FILTER_MACROS), ಖಾಲಿ);
	ಆದರೆ (browseFile == ಖಾಲಿ)
	    ಹೋಗು theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ಆದರೆ (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#ಆದರೆ_ಅಂತ್ಯ

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ಆದರೆ (fd != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	ಆದರೆ (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ಇಲ್ಲದಿದ್ದರೆ
	    flagp = &ssop_flags;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ಆದರೆ (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

	// Enforce vim9script
	ಆದರೆ (put_ಸಾಲು(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ಆದರೆ (eap->cmdidx == CMD_mkvimrc)
	    (ಶೂನ್ಯ)put_ಸಾಲು(fd, "version 6.0");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	ಆದರೆ (eap->cmdidx == CMD_mksession)
	{
	    ಆದರೆ (put_ಸಾಲು(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ಆದರೆ (eap->cmdidx != CMD_mkview)
#ಆದರೆ_ಅಂತ್ಯ
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ಆದರೆ (p_cp)
		(ಶೂನ್ಯ)put_ಸಾಲು(fd, "if !&cp | set cp | endif");
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)put_ಸಾಲು(fd, "if &cp | set nocp | endif");
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	ಆದರೆ (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#ಆದರೆ_ಅಂತ್ಯ
	{
	    bool do_mappings = true;
	    ಪೂರ್ಣಾಂಕ flags = OPT_GLOBAL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	    failed |= put_ಸಾಲು(fd, "var cpo_save: string") == FAIL;

	    ಆದರೆ (eap->cmdidx == CMD_mksession)
	    {
		ಆದರೆ (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (do_mappings)
		failed |= (makemap(fd, ಖಾಲಿ) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ಆದರೆ defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    ಗಾಗಿ (sid = 1; sid <= script_ವಸ್ತುಗಳು.ga_len; ++sid)
	    {
		si = SCRIPT_ಅಂಶ(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ಆದರೆ ((si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    ಪೂರ್ಣಾಂಕ b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    ಗಾಗಿ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ಅಂಶ(b_sid);

			// Only autoload may conflict. Ignore if missing
			ಆದರೆ ((!b_si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    ಮುಂದುವರಿ;

			// compare prefixes if available
			ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ && b_si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ
				&& (STRCMP(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, b_si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ) == 0))
			    ನಿಲ್ಲಿಸು;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ಆದರೆ (STRCMP(name, b_name) == 0)
			    ನಿಲ್ಲಿಸು;
		    }

		    // import the auto script if there are no conflicts
		    ಆದರೆ (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	ಆದರೆ (!failed && view_session)
	{
	    ಆದರೆ (put_ಸಾಲು(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ಆದರೆ (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ಆದರೆ (dirnow == ಖಾಲಿ)
		    failed = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Change to session file's dir.
		    ಆದರೆ (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((ಅಕ್ಷರ *)dirnow) != 0)
			*dirnow = NUL;
		    ಆದರೆ (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ಆದರೆ (vim_chdirfile(fname, ಖಾಲಿ) == OK)
			    shorten_fnames(TRUE);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != ಖಾಲಿ)
		    {
			ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ಆದರೆ (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != ಖಾಲಿ)))
		    {
			ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)dirnow) != 0)
			    emsg(_(e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		failed |= put_ಸಾಲು(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								ಖಾಲಿ) == FAIL);
	    }
	    ಆದರೆ (put_ಸಾಲು(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	    ಆದರೆ (no_hlsearch && put_ಸಾಲು(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ಆದರೆ (put_ಸಾಲು(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ಆದರೆ (eap->cmdidx == CMD_mksession)
	    {
		ಆದರೆ (put_ಸಾಲು(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (put_ಸಾಲು(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ಆದರೆ (failed)
	    emsg(_(e_error_while_writing));
#ಆದರೆ defined(FEAT_SESSION)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ಆದರೆ (tbuf != ಖಾಲಿ)
	    {
		ಆದರೆ (vim_ತುಂಬಿದ_Name(fname, tbuf, MAXPATHL, FALSE) == OK)
		    ಗಣ_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MKSESSION_NL
	mksession_nl = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
theend:
    vim_free(browseFile);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
    vim_free(viewFile);
#ಆದರೆ_ಅಂತ್ಯ

    apply_autocmds(EVENT_SESSIONWRITEPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
}

#ಆದರೆ defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ಆದರೆ (ASCII_ISUPPER(*p))
    {
	ಆಗ (*(++p))
	    ಆದರೆ (ASCII_ISLOWER(*p))
		ಹಿಂತಿರುಗಿ VAR_FLAVOUR_SESSION;
	ಹಿಂತಿರುಗಿ VAR_FLAVOUR_VIMINFO;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ VAR_FLAVOUR_DEFAULT;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    ಪೂರ್ಣಾಂಕ
put_eol(FILE *fd)
{
    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#ಆದರೆ_ಅಂತ್ಯ
	    (putc('\n', fd) < 0))
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    ಪೂರ್ಣಾಂಕ
put_ಸಾಲು(FILE *fd, ಅಕ್ಷರ *s)
{
    ಆದರೆ (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}
