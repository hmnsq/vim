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

#ถ้า defined(FEAT_SESSION)

static จำนวนเต็ม did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    static จำนวนเต็ม
ses_put_fname(FILE *fd, char_u *name, unsigned *flagp)
{
    char_u	*sname;
    char_u	*p;
    จำนวนเต็ม		retval = OK;

    sname = home_replace_save(NULL, name);
    ถ้า (sname == NULL)
	คืน FAIL;

    ถ้า (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	สำหรับ (p = sname; *p != NUL; MB_PTR_ADV(p))
	    ถ้า (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    ถ้า (p == NULL)
	คืน FAIL;

    // write the result
    ถ้า (fputs((var_na *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    คืน retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    static จำนวนเต็ม
ses_fname(FILE *fd, buf_T *buf, unsigned *flagp, จำนวนเต็ม add_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    ถ้า (buf->b_sfname != NULL
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    มิฉะนั้น
	name = buf->b_ffname;
    ถ้า (ses_put_fname(fd, name, flagp) == FAIL
	    || (add_eol && put_eol(fd) == FAIL))
	คืน FAIL;
    คืน OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    static จำนวนเต็ม
ses_arglist(
    FILE	*fd,
    var_na	*cmd,
    garray_T	*gap,
    จำนวนเต็ม		fullname,	// TRUE: use full path name
    unsigned	*flagp)
{
    จำนวนเต็ม		i;
    char_u	*buf = NULL;
    char_u	*s;

    ถ้า (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	คืน FAIL;
    ถ้า (put_line(fd, ":%argdel") == FAIL)
	คืน FAIL;
    สำหรับ (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_data)[i]);
	ถ้า (s != NULL)
	{
	    ถ้า (fullname)
	    {
		buf = alloc(MAXPATHL);
		ถ้า (buf != NULL)
		{
		    (void)vim_FullName(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    ถ้า (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		คืน FAIL;
	    }
	    vim_free(buf);
	}
    }
    คืน OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    static จำนวนเต็ม
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    ถ้า (bt_terminal(wp->w_buffer))
	คืน !term_is_finished(wp->w_buffer)
	    && (ssop_flags & SSOP_TERMINAL)
	    && term_should_restore(wp->w_buffer);
# endif
    ถ้า (wp->w_buffer->b_fname == NULL
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_buffer))
	คืน (ssop_flags & SSOP_BLANK);
    ถ้า (bt_help(wp->w_buffer))
	คืน (ssop_flags & SSOP_HELP);
    คืน TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    static จำนวนเต็ม
ses_do_frame(frame_T *fr)
{
    frame_T	*frc;

    ถ้า (fr->fr_layout == FR_LEAF)
	คืน ses_do_win(fr->fr_win);
    FOR_ALL_FRAMES(frc, fr->fr_child)
	ถ้า (ses_do_frame(frc))
	    คืน TRUE;
    คืน FALSE;
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
	ถ้า (ses_do_frame(frc))
	    bha_nga;
    คืน frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    static จำนวนเต็ม
ses_win_rec(FILE *fd, frame_T *fr)
{
    frame_T	*frc;
    จำนวนเต็ม		count = 0;

    ถ้า (fr->fr_layout == FR_LEAF)
	คืน OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_child);
    ถ้า (frc != NULL)
	ขณะ ((frc = ses_skipframe(frc->fr_next)) != NULL)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    ถ้า (put_line(fd, "wincmd _ | wincmd |") == FAIL
		    || put_line(fd, fr->fr_layout == FR_COL
			? "split" : "vsplit") == FAIL)
		คืน FAIL;
	    ++count;
	}

    // Go back to the first window.
    ถ้า (count > 0 && (fprintf(fd, fr->fr_layout == FR_COL
		    ? ":%dwincmd k" : ":%dwincmd h", count) < 0
		|| put_eol(fd) == FAIL))
	คืน FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_child);
    ขณะ (frc != NULL)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_next);
	// Go to next window.
	ถ้า (frc != NULL && put_line(fd, "wincmd w") == FAIL)
	    คืน FAIL;
    }

    คืน OK;
}

    static จำนวนเต็ม
ses_winsizes(
    FILE	*fd,
    จำนวนเต็ม		restore_size,
    win_T	*tab_firstwin)
{
    จำนวนเต็ม		n = 0;
    win_T	*wp;

    ถ้า (restore_size && (ssop_flags & SSOP_WINSIZE))
    {
	สำหรับ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ถ้า (!ses_do_win(wp))
		continue;
	    ++n;

	    // restore height when not full height
	    ถ้า (wp->w_height + wp->w_status_height < topframe->fr_height
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (long)wp->w_height, Rows / 2, Rows) < 0
						  || put_eol(fd) == FAIL))
		คืน FAIL;

	    // restore width when not full width
	    ถ้า (wp->w_width < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (long)wp->w_width, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		คืน FAIL;
	}
    }
    มิฉะนั้น
    {
	// Just equalise window sizes
	ถ้า (put_line(fd, "wincmd =") == FAIL)
	    คืน FAIL;
    }
    คืน OK;
}

    static จำนวนเต็ม
put_view_curpos(FILE *fd, win_T *wp, var_na *spaces)
{
    จำนวนเต็ม r;

    ถ้า (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    มิฉะนั้น
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    คืน r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    static จำนวนเต็ม
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    จำนวนเต็ม		add_edit,	     // add ":edit" command to view
    unsigned	*flagp,		     // vop_flags or ssop_flags
    จำนวนเต็ม		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs UNUSED) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    จำนวนเต็ม		f;
    จำนวนเต็ม		do_cursor;
    จำนวนเต็ม		did_next = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    ถ้า (wp->w_alist == &global_alist)
    {
	ถ้า (put_line(fd, "argglobal") == FAIL)
	    คืน FAIL;
    }
    มิฉะนั้น
    {
	ถ้า (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != NULL
			|| wp->w_localdir != NULL, flagp) == FAIL)
	    คืน FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    ถ้า (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	ถ้า (fprintf(fd, ":%ldargu", (long)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    คืน FAIL;
	did_next = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    ถ้า (add_edit && (!did_next || wp->w_arg_idx_invalid))
    {
	ถ้า (bt_help(wp->w_buffer))
	{
	    var_na *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    ถ้า (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (var_na *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    ถ้า (put_line(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		คืน FAIL;
	}
# ifdef FEAT_TERMINAL
	มิฉะนั้น ถ้า (bt_terminal(wp->w_buffer))
	{
	    ถ้า (term_write_session(fd, wp, terminal_bufs) == FAIL)
		คืน FAIL;
	}
# endif
	// Load the file.
	มิฉะนั้น ถ้า (wp->w_buffer->b_ffname != NULL
		&& !bt_nofilename(wp->w_buffer))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    ถ้า (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_buffer, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		คืน FAIL;
	}
	มิฉะนั้น
	{
	    // No file in this buffer, just make it empty.
	    ถ้า (put_line(fd, "enew") == FAIL)
		คืน FAIL;
# ifdef FEAT_QUICKFIX
	    ถ้า (wp->w_buffer->b_ffname != NULL)
	    {
		// The buffer does have a name, but it's not a file name.
		ถ้า (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_buffer, flagp, TRUE) == FAIL)
		    คืน FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    ถ้า (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	ถ้า ((flagp == &ssop_flags)
		&& alt != NULL
		&& alt->b_fname != NULL
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    คืน FAIL;
    }

    // Local mappings and abbreviations.
    ถ้า ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_buffer) == FAIL)
	คืน FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_buffer;
    ถ้า (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    มิฉะนั้น ถ้า (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    มิฉะนั้น
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    ถ้า (f == FAIL)
	คืน FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    ถ้า ((*flagp & SSOP_FOLDS)
	    && wp->w_buffer->b_ffname != NULL
	    && (bt_normal(wp->w_buffer) || bt_help(wp->w_buffer)))
    {
	ถ้า (put_folds(fd, wp) == FAIL)
	    คืน FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    ถ้า (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	ถ้า (put_line(fd, "{") == FAIL)
	    คืน FAIL;

	ถ้า (wp->w_height <= 0)
	{
	    ถ้า (fprintf(fd, "  var l: number = %ld", (long)wp->w_cursor.lnum) < 0)
		คืน FAIL;
	}
	มิฉะนั้น ถ้า (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (long)wp->w_cursor.lnum,
		    (long)(wp->w_cursor.lnum - wp->w_topline),
		    (long)wp->w_height / 2, (long)wp->w_height) < 0)
	    คืน FAIL;

	ถ้า (put_eol(fd) == FAIL
		|| put_line(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_line(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_line(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (long)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    คืน FAIL;
	// Restore the cursor column and left offset when not wrapping.
	ถ้า (wp->w_cursor.col == 0)
	{
	    ถ้า (put_line(fd, "  normal! 0") == FAIL)
		คืน FAIL;
	}
	มิฉะนั้น
	{
	    ถ้า (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_width > 0)
	    {
		ถ้า (fprintf(fd,
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
		    คืน FAIL;
	    }
	    มิฉะนั้น ถ้า (put_view_curpos(fd, wp, "  ") == FAIL)
		คืน FAIL;
	}

	ถ้า (put_line(fd, "}") == FAIL)
	    คืน FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    ถ้า (wp->w_localdir != NULL
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	ถ้า (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    คืน FAIL;
	did_lcd = TRUE;
    }

    คืน OK;
}

# ifdef FEAT_EVAL
    static จำนวนเต็ม
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    จำนวนเต็ม		todo;
    char_u	*p, *t;

    todo = (จำนวนเต็ม)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ถ้า ((this_var->di_tv.v_type == VAR_NUMBER
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		ถ้า (p == NULL)	    // out of memory
		    bha_nga;
		สำหรับ (t = p; *t != NUL; ++t)
		    ถ้า (*t == '\n')
			*t = 'n';
		    มิฉะนั้น ถ้า (*t == '\r')
			*t = 'r';
		ถ้า ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_key,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    คืน FAIL;
		}
		vim_free(p);
	    }
	    มิฉะนั้น ถ้า (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_key) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		จำนวนเต็ม sign = ' ';

		ถ้า (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		ถ้า ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_key, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    คืน FAIL;
	    }
	}
    }
    คืน OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    static จำนวนเต็ม
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    จำนวนเต็ม		only_save_windows = TRUE;
    จำนวนเต็ม		nr;
    จำนวนเต็ม		restore_size = TRUE;
    จำนวนเต็ม		restore_height_width = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = NULL;
    จำนวนเต็ม		restore_stal = FALSE;
    win_T	*tab_firstwin;
    frame_T	*tab_topframe;
    จำนวนเต็ม		cur_arg_idx = 0;
    จำนวนเต็ม		next_arg_idx = 0;
    จำนวนเต็ม		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    ถ้า (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    ถ้า (put_line(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	goto fail;

    ถ้า (put_line(fd, "doautoall SessionLoadPre") == FAIL)
	goto fail;

    ถ้า (put_line(fd, "var save_splitbelow: bool") == FAIL
	    || put_line(fd, "var save_splitright: bool") == FAIL
	    || put_line(fd, "var save_winminheight: number") == FAIL
	    || put_line(fd, "var save_winminwidth: number") == FAIL
	    || put_line(fd, "var wipebuf: number = -1") == FAIL
	    || put_line(fd, "var shortmess_save: string") == FAIL)
	goto fail;

    ถ้า (ssop_flags & SSOP_GLOBALS)
	ถ้า (store_session_globals(fd) == FAIL)
	    goto fail;
# endif

    // Close all windows and tabs but one.
    ถ้า (put_line(fd, "silent only") == FAIL)
	goto fail;
    ถ้า ((ssop_flags & SSOP_TABPAGES)
	    && put_line(fd, "silent tabonly") == FAIL)
	goto fail;

    // Now a :cd command to the session directory or the current directory
    ถ้า (ssop_flags & SSOP_SESDIR)
    {
	ถ้า (put_line(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    goto fail;
    }
    มิฉะนั้น ถ้า (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(NULL, globaldir != NULL ? globaldir : dirnow);
	ถ้า (sname == NULL
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
    ถ้า (put_line(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	goto fail;
    ถ้า (put_line(fd, "  wipebuf = bufnr('%')") == FAIL)
	goto fail;
    ถ้า (put_line(fd, "endif") == FAIL)
	goto fail;

    // Save 'shortmess' if not storing options.
    ถ้า ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_line(fd, "shortmess_save = &shortmess") == FAIL)
	goto fail;

    // Set 'shortmess' for the following.
    ถ้า (put_line(fd, "set shortmess+=aoO") == FAIL)
	goto fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	ถ้า (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != NULL
		&& buf->b_p_bl)
	{
	    ถ้า (fprintf(fd, "badd +%ld ", buf->b_wininfo == NULL ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		goto fail;
	}
    }

    // the global argument list
    ถ้า (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	goto fail;

    ถ้า (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	ถ้า (fprintf(fd, "set lines=%ld columns=%ld" , Rows, Columns) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }

# ifdef FEAT_GUI
    ถ้า (gui.in_use && (ssop_flags & SSOP_WINPOS))
    {
	จำนวนเต็ม	x, y;

	ถ้า (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    ถ้า (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		goto fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    ถ้า (p_stal == 1 && first_tabpage->tp_next != NULL)
    {
	ถ้า (put_line(fd, "set stal=2") == FAIL)
	    goto fail;
	restore_stal = TRUE;
    }

    ถ้า ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    ถ้า (tp->tp_next != NULL
		  && put_line(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		goto fail;
	ถ้า (first_tabpage->tp_next != NULL && put_line(fd, "tabrewind") == FAIL)
	    goto fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	จำนวนเต็ม	need_tabnext = FALSE;
	จำนวนเต็ม	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	ถ้า ((ssop_flags & SSOP_TABPAGES))
	{
	    ถ้า (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    มิฉะนั้น
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    ถ้า (tp != first_tabpage)
		need_tabnext = TRUE;
	}
	มิฉะนั้น
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	สำหรับ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ถ้า (ses_do_win(wp)
		    && wp->w_buffer->b_ffname != NULL
		    && !bt_help(wp->w_buffer)
		    && !bt_nofilename(wp->w_buffer))
	    {
		ถ้า (need_tabnext && put_line(fd, "tabnext") == FAIL)
		    goto fail;
		need_tabnext = FALSE;

		ถ้า (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_buffer, &ssop_flags, TRUE)
								       == FAIL)
		    goto fail;
		ถ้า (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		bha_nga;
	    }
	}

	// If no file got edited create an empty tab page.
	ถ้า (need_tabnext && put_line(fd, "tabnext") == FAIL)
	    goto fail;

	ถ้า (tab_topframe->fr_layout != FR_LEAF)
	{
	    // Save current window layout.
	    ถ้า (put_line(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_line(fd, "save_splitright = &splitright")
								       == FAIL)
		goto fail;
	    ถ้า (put_line(fd, "set splitbelow splitright") == FAIL)
		goto fail;
	    ถ้า (ses_win_rec(fd, tab_topframe) == FAIL)
		goto fail;
	    ถ้า (put_line(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_line(fd, "&splitright = save_splitright")
								       == FAIL)
		goto fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	สำหรับ (wp = tab_firstwin; wp != NULL; wp = W_NEXT(wp))
	{
	    ถ้า (ses_do_win(wp))
		++nr;
	    มิฉะนั้น
		restore_size = FALSE;
	    ถ้า (curwin == wp)
		cnr = nr;
	}

	ถ้า (tab_firstwin->w_next != NULL)
	{
	    // Go to the first window.
	    ถ้า (put_line(fd, "wincmd t") == FAIL)
		goto fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    ถ้า (!restore_height_width
		    && (put_line(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_line(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		goto fail;
	    ถ้า (put_line(fd, "set winminheight=0") == FAIL
		    || put_line(fd, "set winheight=1") == FAIL
		    || put_line(fd, "set winminwidth=0") == FAIL
		    || put_line(fd, "set winwidth=1") == FAIL)
		goto fail;
	    restore_height_width = TRUE;
	}
	ถ้า (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	ถ้า ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != NULL)
	{
	    ถ้า (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		goto fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	สำหรับ (wp = tab_firstwin; wp != NULL; wp = wp->w_next)
	{
	    ถ้า (!ses_do_win(wp))
		continue;
	    ถ้า (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# มิฉะนั้น
							 NULL
# endif
		 ) == FAIL)
		goto fail;
	    ถ้า (nr > 1 && put_line(fd, "wincmd w") == FAIL)
		goto fail;
	    next_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = next_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	ถ้า (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    goto fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	ถ้า (nr > 1 && ses_winsizes(fd, restore_size, tab_firstwin) == FAIL)
	    goto fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	ถ้า (!(ssop_flags & SSOP_TABPAGES))
	    bha_nga;
    }

    ถ้า (ssop_flags & SSOP_TABPAGES)
    {
	ถ้า (fprintf(fd, "tabnext %d", tabpage_index(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    goto fail;
    }
    ถ้า (restore_stal && put_line(fd, "set stal=1") == FAIL)
	goto fail;

    // Wipe out an empty unnamed buffer we started in.
    ถ้า (put_line(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	goto fail;
    ถ้า (put_line(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	goto fail;
    ถ้า (put_line(fd, "endif") == FAIL)
	goto fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    ถ้า (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	goto fail;

    // Restore 'shortmess'.
    ถ้า (ssop_flags & SSOP_OPTIONS)
    {
	ถ้า (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    goto fail;
    }
    มิฉะนั้น
    {
	ถ้า (put_line(fd, "&shortmess = shortmess_save") == FAIL)
	    goto fail;
    }

    ถ้า (restore_height_width)
    {
	// Restore 'winminheight' and 'winminwidth'.
	ถ้า (put_line(fd, "&winminheight = save_winminheight") == FAIL
	      || put_line(fd, "&winminwidth = save_winminwidth") == FAIL)
	    goto fail;
    }

    // Lastly, execute the x.vim file if it exists.
    ถ้า (put_line(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_line(fd, "if filereadable(sx)") == FAIL
	    || put_line(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_line(fd, "endif") == FAIL)
	goto fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    คืน ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    static char_u *
get_view_file(จำนวนเต็ม c)
{
    จำนวนเต็ม		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    ถ้า (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	คืน NULL;
    }
    sname = home_replace_save(NULL, curbuf->b_ffname);
    ถ้า (sname == NULL)
	คืน NULL;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    สำหรับ (p = sname; *p; ++p)
	ถ้า (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    ถ้า (retval != NULL)
    {
	STRCPY(retval, p_vdir);
	add_pathsep(retval);
	s = retval + STRLEN(retval);
	สำหรับ (p = sname; *p; ++p)
	{
	    ถ้า (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    มิฉะนั้น ถ้า (vim_ispathsep(*p))
	    {
		*s++ = '=';
# ถ้า defined(BACKSLASH_IN_FILENAME) || defined(AMIGA) || defined(VMS)
		ถ้า (*p == ':')
		    *s++ = '-';
		มิฉะนั้น
# endif
		    *s++ = '+';
	    }
	    มิฉะนั้น
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    คืน retval;
}

/*
 * ":loadview [nr]"
 */
    void
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    ถ้า (fname == NULL)
	คืน;

    (void)do_source(fname, FALSE, DOSO_NONE, NULL);
    vim_free(fname);
}

# ถ้า defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    จำนวนเต็ม
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    var_na	    *mksession_cmdline;
    unsigned จำนวนเต็ม    save_ssop_flags;
    จำนวนเต็ม		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    ถ้า (escaped_filename == NULL)
	คืน FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    ถ้า (mksession_cmdline == NULL)
    {
	vim_free(escaped_filename);
	คืน FALSE;
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
    ถ้า (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == NULL
	       || put_line(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_line(fd, "unlet g:Save_VV_this_session") == FAIL);

	ถ้า (fd != NULL && fclose(fd) != 0)
	    failed = TRUE;

	ถ้า (failed)
	    mch_remove(filename);
    }

    คืน !failed;
}
# endif

#endif // FEAT_SESSION

#ถ้า defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
static จำนวนเต็ม mksession_nl = FALSE;    // use NL only in put_eol()
#endif

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    void
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    จำนวนเต็ม		failed = FALSE;
    char_u	*fname;
#ifdef FEAT_BROWSE
    char_u	*browseFile = NULL;
#endif
#ifdef FEAT_SESSION
    จำนวนเต็ม		view_session = FALSE;
    จำนวนเต็ม		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = NULL;
    unsigned	*flagp;
#endif
#ถ้า defined(FEAT_EVAL)
    จำนวนเต็ม		sid;
    scriptitem_T *si = NULL;
#endif

    ถ้า (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#ifdef FEAT_SESSION
	view_session = TRUE;
#มิฉะนั้น
	ex_ni(eap);
	คืน;
#endif
    }

#ifdef FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    ถ้า (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	ถ้า (fname == NULL)
	    คืน;
	viewFile = fname;
	using_vdir = TRUE;
    }
    มิฉะนั้น
#endif
	ถ้า (*eap->arg != NUL)
	fname = eap->arg;
    มิฉะนั้น ถ้า (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#ifdef FEAT_SESSION
    มิฉะนั้น ถ้า (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#endif
    มิฉะนั้น
	fname = (char_u *)EXRC_FILE;

#ifdef FEAT_BROWSE
    ถ้า (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", NULL,
		(char_u *)_(BROWSE_FILTER_MACROS), NULL);
	ถ้า (browseFile == NULL)
	    goto theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#endif

#ถ้า defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    ถ้า (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#endif

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    ถ้า (fd != NULL)
    {
#ifdef FEAT_SESSION
	ถ้า (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	มิฉะนั้น
	    flagp = &ssop_flags;
#endif

#ifdef MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	ถ้า (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#endif

	// Enforce vim9script
	ถ้า (put_line(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	ถ้า (eap->cmdidx == CMD_mkvimrc)
	    (void)put_line(fd, "version 6.0");

#ifdef FEAT_SESSION
	ถ้า (eap->cmdidx == CMD_mksession)
	{
	    ถ้า (put_line(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	ถ้า (eap->cmdidx != CMD_mkview)
#endif
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    ถ้า (p_cp)
		(void)put_line(fd, "if !&cp | set cp | endif");
	    มิฉะนั้น
		(void)put_line(fd, "if &cp | set nocp | endif");
	}

#ifdef FEAT_SESSION
	ถ้า (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#endif
	{
	    bool do_mappings = true;
	    จำนวนเต็ม flags = OPT_GLOBAL;

#ifdef FEAT_SESSION
	    failed |= put_line(fd, "var cpo_save: string") == FAIL;

	    ถ้า (eap->cmdidx == CMD_mksession)
	    {
		ถ้า (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#endif

	    ถ้า (do_mappings)
		failed |= (makemap(fd, NULL) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#ถ้า defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    สำหรับ (sid = 1; sid <= script_items.ga_len; ++sid)
	    {
		si = SCRIPT_ITEM(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		ถ้า ((si->sn_autoload_prefix || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    จำนวนเต็ม b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    สำหรับ (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_ITEM(b_sid);

			// Only autoload may conflict. Ignore if missing
			ถ้า ((!b_si->sn_autoload_prefix && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    continue;

			// compare prefixes if available
			ถ้า (si->sn_autoload_prefix != NULL && b_si->sn_autoload_prefix != NULL
				&& (STRCMP(si->sn_autoload_prefix, b_si->sn_autoload_prefix) == 0))
			    bha_nga;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			ถ้า (STRCMP(name, b_name) == 0)
			    bha_nga;
		    }

		    // import the auto script if there are no conflicts
		    ถ้า (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#endif
	}

#ifdef FEAT_SESSION
	ถ้า (!failed && view_session)
	{
	    ถ้า (put_line(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    ถ้า (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		ถ้า (dirnow == NULL)
		    failed = TRUE;
		มิฉะนั้น
		{
		    // Change to session file's dir.
		    ถ้า (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((var_na *)dirnow) != 0)
			*dirnow = NUL;
		    ถ้า (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			ถ้า (vim_chdirfile(fname, NULL) == OK)
			    shorten_fnames(TRUE);
		    }
		    มิฉะนั้น ถ้า (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != NULL)
		    {
			ถ้า (mch_chdir((var_na *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    ถ้า (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != NULL)))
		    {
			ถ้า (mch_chdir((var_na *)dirnow) != 0)
			    emsg(_(e_cannot_go_back_to_previous_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    มิฉะนั้น
	    {
		failed |= put_line(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								NULL) == FAIL);
	    }
	    ถ้า (put_line(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	    ถ้า (no_hlsearch && put_line(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    ถ้า (put_line(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    ถ้า (eap->cmdidx == CMD_mksession)
	    {
		ถ้า (put_line(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#endif

	ถ้า (put_line(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	ถ้า (failed)
	    emsg(_(e_error_while_writing));
#ถ้า defined(FEAT_SESSION)
	มิฉะนั้น ถ้า (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    ถ้า (tbuf != NULL)
	    {
		ถ้า (vim_FullName(fname, tbuf, MAXPATHL, FALSE) == OK)
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

#ถ้า defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    ถ้า (ASCII_ISUPPER(*p))
    {
	ขณะ (*(++p))
	    ถ้า (ASCII_ISLOWER(*p))
		คืน VAR_FLAVOUR_SESSION;
	คืน VAR_FLAVOUR_VIMINFO;
    }
    มิฉะนั้น
	คืน VAR_FLAVOUR_DEFAULT;
}
#endif

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    จำนวนเต็ม
put_eol(FILE *fd)
{
    ถ้า (
#ifdef USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#endif
	    (putc('\n', fd) < 0))
	คืน FAIL;
    คืน OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    จำนวนเต็ม
put_line(FILE *fd, var_na *s)
{
    ถ้า (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	คืน FAIL;
    คืน OK;
}
