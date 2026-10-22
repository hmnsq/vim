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

#شامل "vim.h"

#اگر defined(FEAT_SESSION)

ساکن صحیح did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ساکن صحیح
ses_put_fname(FILE *fd, char_u *name, غیرعلامتی *flagp)
{
    char_u	*sname;
    char_u	*p;
    صحیح		retval = OK;

    sname = home_replace_save(تہی, name);
    اگر (sname == تہی)
	واپس FAIL;

    اگر (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	برائے (p = sname; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    اگر (p == تہی)
	واپس FAIL;

    // write the result
    اگر (fputs((حرف *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    واپس retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ساکن صحیح
ses_fname(FILE *fd, buf_T *buf, غیرعلامتی *flagp, صحیح جوڑو_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    اگر (buf->b_sfname != تہی
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    ورنہ
	name = buf->b_ffname;
    اگر (ses_put_fname(fd, name, flagp) == FAIL
	    || (جوڑو_eol && put_eol(fd) == FAIL))
	واپس FAIL;
    واپس OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ساکن صحیح
ses_arglist(
    FILE	*fd,
    حرف	*cmd,
    garray_T	*gap,
    صحیح		fullname,	// TRUE: use full path name
    غیرعلامتی	*flagp)
{
    صحیح		i;
    char_u	*buf = تہی;
    char_u	*s;

    اگر (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	واپس FAIL;
    اگر (put_سطر(fd, ":%argdel") == FAIL)
	واپس FAIL;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_ڈیٹا)[i]);
	اگر (s != تہی)
	{
	    اگر (fullname)
	    {
		buf = alloc(MAXPATHL);
		اگر (buf != تہی)
		{
		    (خالی)vim_بھرا_Name(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    اگر (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		واپس FAIL;
	    }
	    vim_free(buf);
	}
    }
    واپس OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ساکن صحیح
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    اگر (bt_terminal(wp->w_بفر))
	واپس !رکن_is_ختم_شدہ(wp->w_بفر)
	    && (ssop_flags & SSOP_TERMINAL)
	    && رکن_should_restore(wp->w_بفر);
# endif
    اگر (wp->w_بفر->b_fname == تہی
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_بفر))
	واپس (ssop_flags & SSOP_BLANK);
    اگر (bt_help(wp->w_بفر))
	واپس (ssop_flags & SSOP_HELP);
    واپس TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ساکن صحیح
ses_do_چوکھٹا(چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;

    اگر (fr->fr_layout == FR_برگ)
	واپس ses_do_win(fr->fr_win);
    FOR_ALL_چوکھٹے(frc, fr->fr_بچہ)
	اگر (ses_do_چوکھٹا(frc))
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ساکن چوکھٹا_T *
ses_skipframe(چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;

    FOR_ALL_چوکھٹے(frc, fr)
	اگر (ses_do_چوکھٹا(frc))
	    توڑو;
    واپس frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ساکن صحیح
ses_win_rec(FILE *fd, چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;
    صحیح		گنتی = 0;

    اگر (fr->fr_layout == FR_برگ)
	واپس OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_بچہ);
    اگر (frc != تہی)
	جب ((frc = ses_skipframe(frc->fr_اگلا)) != تہی)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    اگر (put_سطر(fd, "wincmd _ | wincmd |") == FAIL
		    || put_سطر(fd, fr->fr_layout == FR_ستون
			? "split" : "vsplit") == FAIL)
		واپس FAIL;
	    ++گنتی;
	}

    // Go back to the first window.
    اگر (گنتی > 0 && (fprintf(fd, fr->fr_layout == FR_ستون
		    ? ":%dwincmd k" : ":%dwincmd h", گنتی) < 0
		|| put_eol(fd) == FAIL))
	واپس FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_بچہ);
    جب (frc != تہی)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_اگلا);
	// Go to next window.
	اگر (frc != تہی && put_سطر(fd, "wincmd w") == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

    ساکن صحیح
ses_winsizes(
    FILE	*fd,
    صحیح		restore_سائز,
    win_T	*tab_firstwin)
{
    صحیح		n = 0;
    win_T	*wp;

    اگر (restore_سائز && (ssop_flags & SSOP_WINSIZE))
    {
	برائے (wp = tab_firstwin; wp != تہی; wp = wp->w_اگلا)
	{
	    اگر (!ses_do_win(wp))
		جاری;
	    ++n;

	    // restore height when not full height
	    اگر (wp->w_اونچائی + wp->w_حالت_اونچائی < topframe->fr_اونچائی
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (طویل)wp->w_اونچائی, صفیں / 2, صفیں) < 0
						  || put_eol(fd) == FAIL))
		واپس FAIL;

	    // restore width when not full width
	    اگر (wp->w_چوڑائی < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (طویل)wp->w_چوڑائی, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		واپس FAIL;
	}
    }
    ورنہ
    {
	// Just equalise window sizes
	اگر (put_سطر(fd, "wincmd =") == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

    ساکن صحیح
put_view_curpos(FILE *fd, win_T *wp, حرف *spaces)
{
    صحیح r;

    اگر (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    ورنہ
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    واپس r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ساکن صحیح
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    صحیح		جوڑو_edit,	     // add ":edit" command to view
    غیرعلامتی	*flagp,		     // vop_flags or ssop_flags
    صحیح		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs غیر_مستعمل) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    صحیح		f;
    صحیح		do_cursor;
    صحیح		did_اگلا = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    اگر (wp->w_alist == &global_alist)
    {
	اگر (put_سطر(fd, "argglobal") == FAIL)
	    واپس FAIL;
    }
    ورنہ
    {
	اگر (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != تہی
			|| wp->w_localdir != تہی, flagp) == FAIL)
	    واپس FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    اگر (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	اگر (fprintf(fd, ":%ldargu", (طویل)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    واپس FAIL;
	did_اگلا = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    اگر (جوڑو_edit && (!did_اگلا || wp->w_arg_idx_invalid))
    {
	اگر (bt_help(wp->w_بفر))
	{
	    حرف *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    اگر (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (حرف *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    اگر (put_سطر(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		واپس FAIL;
	}
# ifdef FEAT_TERMINAL
	ورنہ اگر (bt_terminal(wp->w_بفر))
	{
	    اگر (رکن_write_session(fd, wp, terminal_bufs) == FAIL)
		واپس FAIL;
	}
# endif
	// Load the file.
	ورنہ اگر (wp->w_بفر->b_ffname != تہی
		&& !bt_nofilename(wp->w_بفر))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    اگر (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    // No file in this buffer, just make it empty.
	    اگر (put_سطر(fd, "enew") == FAIL)
		واپس FAIL;
# ifdef FEAT_QUICKFIX
	    اگر (wp->w_بفر->b_ffname != تہی)
	    {
		// The buffer does have a name, but it's not a file name.
		اگر (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_بفر, flagp, TRUE) == FAIL)
		    واپس FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    اگر (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	اگر ((flagp == &ssop_flags)
		&& alt != تہی
		&& alt->b_fname != تہی
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    واپس FAIL;
    }

    // Local mappings and abbreviations.
    اگر ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_بفر) == FAIL)
	واپس FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_بفر;
    اگر (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    ورنہ اگر (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    ورنہ
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_بفر;
    اگر (f == FAIL)
	واپس FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    اگر ((*flagp & SSOP_FOLDS)
	    && wp->w_بفر->b_ffname != تہی
	    && (bt_معمول(wp->w_بفر) || bt_help(wp->w_بفر)))
    {
	اگر (put_folds(fd, wp) == FAIL)
	    واپس FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    اگر (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	اگر (put_سطر(fd, "{") == FAIL)
	    واپس FAIL;

	اگر (wp->w_اونچائی <= 0)
	{
	    اگر (fprintf(fd, "  var l: number = %ld", (طویل)wp->w_cursor.lnum) < 0)
		واپس FAIL;
	}
	ورنہ اگر (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (طویل)wp->w_cursor.lnum,
		    (طویل)(wp->w_cursor.lnum - wp->w_topline),
		    (طویل)wp->w_اونچائی / 2, (طویل)wp->w_اونچائی) < 0)
	    واپس FAIL;

	اگر (put_eol(fd) == FAIL
		|| put_سطر(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_سطر(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_سطر(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (طویل)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    واپس FAIL;
	// Restore the cursor column and left offset when not wrapping.
	اگر (wp->w_cursor.ستون == 0)
	{
	    اگر (put_سطر(fd, "  normal! 0") == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    اگر (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_چوڑائی > 0)
	    {
		اگر (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (طویل)wp->w_virtcol + 1,
			    (طویل)(wp->w_virtcol - wp->w_leftcol),
			    (طویل)wp->w_چوڑائی / 2, (طویل)wp->w_چوڑائی) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (طویل)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_سطر(fd, "  endif") == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (put_view_curpos(fd, wp, "  ") == FAIL)
		واپس FAIL;
	}

	اگر (put_سطر(fd, "}") == FAIL)
	    واپس FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    اگر (wp->w_localdir != تہی
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	اگر (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    واپس FAIL;
	did_lcd = TRUE;
    }

    واپس OK;
}

# ifdef FEAT_EVAL
    ساکن صحیح
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحیح		todo;
    char_u	*p, *t;

    todo = (صحیح)gvht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(gvht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    اگر ((this_var->di_tv.v_type == VAR_نمبر
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		اگر (p == تہی)	    // out of memory
		    توڑو;
		برائے (t = p; *t != NUL; ++t)
		    اگر (*t == '\n')
			*t = 'n';
		    ورنہ اگر (*t == '\r')
			*t = 'r';
		اگر ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_کلید,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    واپس FAIL;
		}
		vim_free(p);
	    }
	    ورنہ اگر (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		صحیح sign = ' ';

		اگر (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		اگر ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_کلید, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    واپس FAIL;
	    }
	}
    }
    واپس OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ساکن صحیح
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    صحیح		only_save_windows = TRUE;
    صحیح		nr;
    صحیح		restore_سائز = TRUE;
    صحیح		restore_اونچائی_چوڑائی = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = تہی;
    صحیح		restore_stal = FALSE;
    win_T	*tab_firstwin;
    چوکھٹا_T	*tab_topframe;
    صحیح		cur_arg_idx = 0;
    صحیح		اگلا_arg_idx = 0;
    صحیح		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    اگر (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    اگر (put_سطر(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	جاؤ fail;

    اگر (put_سطر(fd, "doautoall SessionLoadPre") == FAIL)
	جاؤ fail;

    اگر (put_سطر(fd, "var save_splitbelow: bool") == FAIL
	    || put_سطر(fd, "var save_splitright: bool") == FAIL
	    || put_سطر(fd, "var save_winminheight: number") == FAIL
	    || put_سطر(fd, "var save_winminwidth: number") == FAIL
	    || put_سطر(fd, "var wipebuf: number = -1") == FAIL
	    || put_سطر(fd, "var shortmess_save: string") == FAIL)
	جاؤ fail;

    اگر (ssop_flags & SSOP_GLOBALS)
	اگر (store_session_globals(fd) == FAIL)
	    جاؤ fail;
# endif

    // Close all windows and tabs but one.
    اگر (put_سطر(fd, "silent only") == FAIL)
	جاؤ fail;
    اگر ((ssop_flags & SSOP_TABPAGES)
	    && put_سطر(fd, "silent tabonly") == FAIL)
	جاؤ fail;

    // Now a :cd command to the session directory or the current directory
    اگر (ssop_flags & SSOP_SESDIR)
    {
	اگر (put_سطر(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    جاؤ fail;
    }
    ورنہ اگر (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(تہی, globaldir != تہی ? globaldir : dirnow);
	اگر (sname == تہی
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    جاؤ fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    اگر (put_سطر(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	جاؤ fail;
    اگر (put_سطر(fd, "  wipebuf = bufnr('%')") == FAIL)
	جاؤ fail;
    اگر (put_سطر(fd, "endif") == FAIL)
	جاؤ fail;

    // Save 'shortmess' if not storing options.
    اگر ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_سطر(fd, "shortmess_save = &shortmess") == FAIL)
	جاؤ fail;

    // Set 'shortmess' for the following.
    اگر (put_سطر(fd, "set shortmess+=aoO") == FAIL)
	جاؤ fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	اگر (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != تہی
		&& buf->b_p_bl)
	{
	    اگر (fprintf(fd, "badd +%ld ", buf->b_wininfo == تہی ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		جاؤ fail;
	}
    }

    // the global argument list
    اگر (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	جاؤ fail;

    اگر (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	اگر (fprintf(fd, "set lines=%ld columns=%ld" , صفیں, Columns) < 0
		|| put_eol(fd) == FAIL)
	    جاؤ fail;
    }

# ifdef FEAT_GUI
    اگر (gui.اندر_use && (ssop_flags & SSOP_WINPOS))
    {
	صحیح	x, y;

	اگر (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    اگر (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		جاؤ fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    اگر (p_stal == 1 && پہلا_tabpage->tp_اگلا != تہی)
    {
	اگر (put_سطر(fd, "set stal=2") == FAIL)
	    جاؤ fail;
	restore_stal = TRUE;
    }

    اگر ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    اگر (tp->tp_اگلا != تہی
		  && put_سطر(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		جاؤ fail;
	اگر (پہلا_tabpage->tp_اگلا != تہی && put_سطر(fd, "tabrewind") == FAIL)
	    جاؤ fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	صحیح	need_tabnext = FALSE;
	صحیح	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	اگر ((ssop_flags & SSOP_TABPAGES))
	{
	    اگر (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    ورنہ
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    اگر (tp != پہلا_tabpage)
		need_tabnext = TRUE;
	}
	ورنہ
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	برائے (wp = tab_firstwin; wp != تہی; wp = wp->w_اگلا)
	{
	    اگر (ses_do_win(wp)
		    && wp->w_بفر->b_ffname != تہی
		    && !bt_help(wp->w_بفر)
		    && !bt_nofilename(wp->w_بفر))
	    {
		اگر (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
		    جاؤ fail;
		need_tabnext = FALSE;

		اگر (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_بفر, &ssop_flags, TRUE)
								       == FAIL)
		    جاؤ fail;
		اگر (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		توڑو;
	    }
	}

	// If no file got edited create an empty tab page.
	اگر (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
	    جاؤ fail;

	اگر (tab_topframe->fr_layout != FR_برگ)
	{
	    // Save current window layout.
	    اگر (put_سطر(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_سطر(fd, "save_splitright = &splitright")
								       == FAIL)
		جاؤ fail;
	    اگر (put_سطر(fd, "set splitbelow splitright") == FAIL)
		جاؤ fail;
	    اگر (ses_win_rec(fd, tab_topframe) == FAIL)
		جاؤ fail;
	    اگر (put_سطر(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_سطر(fd, "&splitright = save_splitright")
								       == FAIL)
		جاؤ fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	برائے (wp = tab_firstwin; wp != تہی; wp = W_اگلا(wp))
	{
	    اگر (ses_do_win(wp))
		++nr;
	    ورنہ
		restore_سائز = FALSE;
	    اگر (curwin == wp)
		cnr = nr;
	}

	اگر (tab_firstwin->w_اگلا != تہی)
	{
	    // Go to the first window.
	    اگر (put_سطر(fd, "wincmd t") == FAIL)
		جاؤ fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    اگر (!restore_اونچائی_چوڑائی
		    && (put_سطر(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_سطر(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		جاؤ fail;
	    اگر (put_سطر(fd, "set winminheight=0") == FAIL
		    || put_سطر(fd, "set winheight=1") == FAIL
		    || put_سطر(fd, "set winminwidth=0") == FAIL
		    || put_سطر(fd, "set winwidth=1") == FAIL)
		جاؤ fail;
	    restore_اونچائی_چوڑائی = TRUE;
	}
	اگر (nr > 1 && ses_winsizes(fd, restore_سائز, tab_firstwin) == FAIL)
	    جاؤ fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	اگر ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != تہی)
	{
	    اگر (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		جاؤ fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	برائے (wp = tab_firstwin; wp != تہی; wp = wp->w_اگلا)
	{
	    اگر (!ses_do_win(wp))
		جاری;
	    اگر (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# ورنہ
							 تہی
# endif
		 ) == FAIL)
		جاؤ fail;
	    اگر (nr > 1 && put_سطر(fd, "wincmd w") == FAIL)
		جاؤ fail;
	    اگلا_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = اگلا_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	اگر (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    جاؤ fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	اگر (nr > 1 && ses_winsizes(fd, restore_سائز, tab_firstwin) == FAIL)
	    جاؤ fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	اگر (!(ssop_flags & SSOP_TABPAGES))
	    توڑو;
    }

    اگر (ssop_flags & SSOP_TABPAGES)
    {
	اگر (fprintf(fd, "tabnext %d", tabpage_اشاریہ(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    جاؤ fail;
    }
    اگر (restore_stal && put_سطر(fd, "set stal=1") == FAIL)
	جاؤ fail;

    // Wipe out an empty unnamed buffer we started in.
    اگر (put_سطر(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	جاؤ fail;
    اگر (put_سطر(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	جاؤ fail;
    اگر (put_سطر(fd, "endif") == FAIL)
	جاؤ fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    اگر (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	جاؤ fail;

    // Restore 'shortmess'.
    اگر (ssop_flags & SSOP_OPTIONS)
    {
	اگر (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    جاؤ fail;
    }
    ورنہ
    {
	اگر (put_سطر(fd, "&shortmess = shortmess_save") == FAIL)
	    جاؤ fail;
    }

    اگر (restore_اونچائی_چوڑائی)
    {
	// Restore 'winminheight' and 'winminwidth'.
	اگر (put_سطر(fd, "&winminheight = save_winminheight") == FAIL
	      || put_سطر(fd, "&winminwidth = save_winminwidth") == FAIL)
	    جاؤ fail;
    }

    // Lastly, execute the x.vim file if it exists.
    اگر (put_سطر(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_سطر(fd, "if filereadable(sx)") == FAIL
	    || put_سطر(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_سطر(fd, "endif") == FAIL)
	جاؤ fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    واپس ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ساکن char_u *
get_view_file(صحیح c)
{
    صحیح		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    اگر (curbuf->b_ffname == تہی)
    {
	emsg(_(e_no_file_name));
	واپس تہی;
    }
    sname = home_replace_save(تہی, curbuf->b_ffname);
    اگر (sname == تہی)
	واپس تہی;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    برائے (p = sname; *p; ++p)
	اگر (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    اگر (retval != تہی)
    {
	STRCPY(retval, p_vdir);
	جوڑو_pathsep(retval);
	s = retval + STRLEN(retval);
	برائے (p = sname; *p; ++p)
	{
	    اگر (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    ورنہ اگر (vim_ispathsep(*p))
	    {
		*s++ = '=';
# اگر defined(BACKSLASH_اندر_FILENAME) || defined(AMIGA) || defined(VMS)
		اگر (*p == ':')
		    *s++ = '-';
		ورنہ
# endif
		    *s++ = '+';
	    }
	    ورنہ
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    واپس retval;
}

/*
 * ":loadview [nr]"
 */
    خالی
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    اگر (fname == تہی)
	واپس;

    (خالی)do_ماخذ(fname, FALSE, DOSO_NONE, تہی);
    vim_free(fname);
}

# اگر defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    صحیح
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    حرف	    *mksession_cmdline;
    غیرعلامتی صحیح    save_ssop_flags;
    صحیح		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    اگر (escaped_filename == تہی)
	واپس FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    اگر (mksession_cmdline == تہی)
    {
	vim_free(escaped_filename);
	واپس FALSE;
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
    اگر (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == تہی
	       || put_سطر(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_سطر(fd, "unlet g:Save_VV_this_session") == FAIL);

	اگر (fd != تہی && fclose(fd) != 0)
	    failed = TRUE;

	اگر (failed)
	    mch_remove(filename);
    }

    واپس !failed;
}
# endif

#اگر_ختم // FEAT_SESSION

#اگر defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ساکن صحیح mksession_nl = FALSE;    // use NL only in put_eol()
#اگر_ختم

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    خالی
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    صحیح		failed = FALSE;
    char_u	*fname;
#اگر_تعریف_شدہ FEAT_BROWSE
    char_u	*browseFile = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SESSION
    صحیح		view_session = FALSE;
    صحیح		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = تہی;
    غیرعلامتی	*flagp;
#اگر_ختم
#اگر defined(FEAT_EVAL)
    صحیح		sid;
    scriptitem_T *si = تہی;
#اگر_ختم

    اگر (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#اگر_تعریف_شدہ FEAT_SESSION
	view_session = TRUE;
#ورنہ
	ex_ni(eap);
	واپس;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    اگر (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	اگر (fname == تہی)
	    واپس;
	viewFile = fname;
	using_vdir = TRUE;
    }
    ورنہ
#اگر_ختم
	اگر (*eap->arg != NUL)
	fname = eap->arg;
    ورنہ اگر (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#اگر_تعریف_شدہ FEAT_SESSION
    ورنہ اگر (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#اگر_ختم
    ورنہ
	fname = (char_u *)EXRC_FILE;

#اگر_تعریف_شدہ FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", تہی,
		(char_u *)_(BROWSE_FILTER_MACROS), تہی);
	اگر (browseFile == تہی)
	    جاؤ theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#اگر_ختم

#اگر defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    اگر (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#اگر_ختم

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    اگر (fd != تہی)
    {
#اگر_تعریف_شدہ FEAT_SESSION
	اگر (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	ورنہ
	    flagp = &ssop_flags;
#اگر_ختم

#اگر_تعریف_شدہ MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	اگر (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#اگر_ختم

	// Enforce vim9script
	اگر (put_سطر(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	اگر (eap->cmdidx == CMD_mkvimrc)
	    (خالی)put_سطر(fd, "version 6.0");

#اگر_تعریف_شدہ FEAT_SESSION
	اگر (eap->cmdidx == CMD_mksession)
	{
	    اگر (put_سطر(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	اگر (eap->cmdidx != CMD_mkview)
#اگر_ختم
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    اگر (p_cp)
		(خالی)put_سطر(fd, "if !&cp | set cp | endif");
	    ورنہ
		(خالی)put_سطر(fd, "if &cp | set nocp | endif");
	}

#اگر_تعریف_شدہ FEAT_SESSION
	اگر (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#اگر_ختم
	{
	    bool do_mappings = true;
	    صحیح flags = OPT_GLOBAL;

#اگر_تعریف_شدہ FEAT_SESSION
	    failed |= put_سطر(fd, "var cpo_save: string") == FAIL;

	    اگر (eap->cmdidx == CMD_mksession)
	    {
		اگر (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#اگر_ختم

	    اگر (do_mappings)
		failed |= (makemap(fd, تہی) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#اگر defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    برائے (sid = 1; sid <= script_اشیاء.ga_len; ++sid)
	    {
		si = SCRIPT_شے(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		اگر ((si->sn_autoload_سابقہ || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    صحیح b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    برائے (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_شے(b_sid);

			// Only autoload may conflict. Ignore if missing
			اگر ((!b_si->sn_autoload_سابقہ && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    جاری;

			// compare prefixes if available
			اگر (si->sn_autoload_سابقہ != تہی && b_si->sn_autoload_سابقہ != تہی
				&& (STRCMP(si->sn_autoload_سابقہ, b_si->sn_autoload_سابقہ) == 0))
			    توڑو;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			اگر (STRCMP(name, b_name) == 0)
			    توڑو;
		    }

		    // import the auto script if there are no conflicts
		    اگر (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#اگر_ختم
	}

#اگر_تعریف_شدہ FEAT_SESSION
	اگر (!failed && view_session)
	{
	    اگر (put_سطر(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    اگر (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		اگر (dirnow == تہی)
		    failed = TRUE;
		ورنہ
		{
		    // Change to session file's dir.
		    اگر (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((حرف *)dirnow) != 0)
			*dirnow = NUL;
		    اگر (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			اگر (vim_chdirfile(fname, تہی) == OK)
			    shorten_fnames(TRUE);
		    }
		    ورنہ اگر (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != تہی)
		    {
			اگر (mch_chdir((حرف *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    اگر (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != تہی)))
		    {
			اگر (mch_chdir((حرف *)dirnow) != 0)
			    emsg(_(e_cannot_go_عقب_مقصد_پچھلا_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    ورنہ
	    {
		failed |= put_سطر(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								تہی) == FAIL);
	    }
	    اگر (put_سطر(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_تلاش_EXTRA
	    اگر (no_hlsearch && put_سطر(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    اگر (put_سطر(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    اگر (eap->cmdidx == CMD_mksession)
	    {
		اگر (put_سطر(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#اگر_ختم

	اگر (put_سطر(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	اگر (failed)
	    emsg(_(e_error_while_writing));
#اگر defined(FEAT_SESSION)
	ورنہ اگر (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    اگر (tbuf != تہی)
	    {
		اگر (vim_بھرا_Name(fname, tbuf, MAXPATHL, FALSE) == OK)
		    سیٹ_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#اگر_ختم
#اگر_تعریف_شدہ MKSESSION_NL
	mksession_nl = FALSE;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_BROWSE
theend:
    vim_free(browseFile);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SESSION
    vim_free(viewFile);
#اگر_ختم

    apply_autocmds(EVENT_SESSIONWRITEPOST, تہی, تہی, FALSE, curbuf);
}

#اگر defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    اگر (ASCII_ISUPPER(*p))
    {
	جب (*(++p))
	    اگر (ASCII_ISLOWER(*p))
		واپس VAR_FLAVOUR_SESSION;
	واپس VAR_FLAVOUR_VIMINFO;
    }
    ورنہ
	واپس VAR_FLAVOUR_DEFAULT;
}
#اگر_ختم

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    صحیح
put_eol(FILE *fd)
{
    اگر (
#اگر_تعریف_شدہ USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#اگر_ختم
	    (putc('\n', fd) < 0))
	واپس FAIL;
    واپس OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    صحیح
put_سطر(FILE *fd, حرف *s)
{
    اگر (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	واپس FAIL;
    واپس OK;
}
