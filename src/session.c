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

#ضمن "vim.h"

#إذا defined(FEAT_SESSION)

ساكن صحيح did_lcd;	// whether ":lcd" was produced for a session

/*
 * Write a file name to the session file.
 * Takes care of the "slash" option in 'sessionoptions' and escapes special
 * characters.
 * Returns FAIL if writing fails or out of memory.
 */
    ساكن صحيح
ses_put_fname(FILE *fd, char_u *name, بنانشانی *flagp)
{
    char_u	*sname;
    char_u	*p;
    صحيح		retval = OK;

    sname = home_replace_save(عدم, name);
    إذا (sname == عدم)
	أرجع FAIL;

    إذا (*flagp & SSOP_SLASH)
    {
	// change all backslashes to forward slashes
	لكل (p = sname; *p != NUL; MB_PTR_ADV(p))
	    إذا (*p == '\\')
		*p = '/';
    }

    // escape special characters
    p = vim_strsave_fnameescape(sname, VSE_NONE);
    vim_free(sname);
    إذا (p == عدم)
	أرجع FAIL;

    // write the result
    إذا (fputs((حرف *)p, fd) < 0)
	retval = FAIL;

    vim_free(p);
    أرجع retval;
}

/*
 * Write a buffer name to the session file.
 * Also ends the line, if "add_eol" is TRUE.
 * Returns FAIL if writing fails.
 */
    ساكن صحيح
ses_fname(FILE *fd, buf_T *buf, بنانشانی *flagp, صحيح جوڑو_eol)
{
    char_u	*name;

    // Use the short file name if the current directory is known at the time
    // the session file will be sourced.
    // Don't do this for ":mkview", we don't know the current directory.
    // Don't do this after ":lcd", we don't keep track of what the current
    // directory is.
    إذا (buf->b_sfname != عدم
	    && flagp == &ssop_flags
	    && (ssop_flags & (SSOP_CURDIR | SSOP_SESDIR))
# ifdef FEAT_AUTOCHDIR
	    && !p_acd
# endif
	    && !did_lcd)
	name = buf->b_sfname;
    وإلا
	name = buf->b_ffname;
    إذا (ses_put_fname(fd, name, flagp) == FAIL
	    || (جوڑو_eol && put_eol(fd) == FAIL))
	أرجع FAIL;
    أرجع OK;
}

/*
 * Write an argument list to the session file.
 * Returns FAIL if writing fails.
 */
    ساكن صحيح
ses_arglist(
    FILE	*fd,
    حرف	*cmd,
    garray_T	*gap,
    صحيح		fullname,	// TRUE: use full path name
    بنانشانی	*flagp)
{
    صحيح		i;
    char_u	*buf = عدم;
    char_u	*s;

    إذا (fputs(cmd, fd) < 0 || put_eol(fd) == FAIL)
	أرجع FAIL;
    إذا (put_سطر(fd, ":%argdel") == FAIL)
	أرجع FAIL;
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	// NULL file names are skipped (only happens when out of memory).
	s = alist_name(&((aentry_T *)gap->ga_انکڑے)[i]);
	إذا (s != عدم)
	{
	    إذا (fullname)
	    {
		buf = alloc(MAXPATHL);
		إذا (buf != عدم)
		{
		    (فراغ)vim_بھریا_Name(s, buf, MAXPATHL, FALSE);
		    s = buf;
		}
	    }
	    إذا (fputs(":$argadd ", fd) < 0
		    || ses_put_fname(fd, s, flagp) == FAIL
		    || put_eol(fd) == FAIL)
	    {
		vim_free(buf);
		أرجع FAIL;
	    }
	    vim_free(buf);
	}
    }
    أرجع OK;
}

/*
 * Return non-zero if window "wp" is to be stored in the Session.
 */
    ساكن صحيح
ses_do_win(win_T *wp)
{
# ifdef FEAT_TERMINAL
    إذا (bt_terminal(wp->w_بفر))
	أرجع !رکن_is_مکمل(wp->w_بفر)
	    && (ssop_flags & SSOP_TERMINAL)
	    && رکن_should_restore(wp->w_بفر);
# endif
    إذا (wp->w_بفر->b_fname == عدم
	    // When 'buftype' is "nofile" can't restore the window contents.
	    || bt_nofilename(wp->w_بفر))
	أرجع (ssop_flags & SSOP_BLANK);
    إذا (bt_help(wp->w_بفر))
	أرجع (ssop_flags & SSOP_HELP);
    أرجع TRUE;
}

/*
 * Return TRUE if frame "fr" has a window somewhere that we want to save in
 * the Session.
 */
    ساكن صحيح
ses_do_چوکھٹا(چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;

    إذا (fr->fr_layout == FR_پتی)
	أرجع ses_do_win(fr->fr_win);
    FOR_ALL_چوکھٹے(frc, fr->fr_بچہ)
	إذا (ses_do_چوکھٹا(frc))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Skip frames that don't contain windows we want to save in the Session.
 * Returns NULL when there none.
 */
    ساكن چوکھٹا_T *
ses_skipframe(چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;

    FOR_ALL_چوکھٹے(frc, fr)
	إذا (ses_do_چوکھٹا(frc))
	    اكسر;
    أرجع frc;
}

/*
 * Write commands to "fd" to recursively create windows for frame "fr",
 * horizontally and vertically split.
 * After the commands the last window in the frame is the current window.
 * Returns FAIL when writing the commands to "fd" fails.
 */
    ساكن صحيح
ses_win_rec(FILE *fd, چوکھٹا_T *fr)
{
    چوکھٹا_T	*frc;
    صحيح		گنتی = 0;

    إذا (fr->fr_layout == FR_پتی)
	أرجع OK;

    // Find first frame that's not skipped and then create a window for
    // each following one (first frame is already there).
    frc = ses_skipframe(fr->fr_بچہ);
    إذا (frc != عدم)
	طالما ((frc = ses_skipframe(frc->fr_اگلا)) != عدم)
	{
	    // Make window as big as possible so that we have lots of room
	    // to split.
	    إذا (put_سطر(fd, "wincmd _ | wincmd |") == FAIL
		    || put_سطر(fd, fr->fr_layout == FR_خانہ
			? "split" : "vsplit") == FAIL)
		أرجع FAIL;
	    ++گنتی;
	}

    // Go back to the first window.
    إذا (گنتی > 0 && (fprintf(fd, fr->fr_layout == FR_خانہ
		    ? ":%dwincmd k" : ":%dwincmd h", گنتی) < 0
		|| put_eol(fd) == FAIL))
	أرجع FAIL;

    // Recursively create frames/windows in each window of this column or
    // row.
    frc = ses_skipframe(fr->fr_بچہ);
    طالما (frc != عدم)
    {
	ses_win_rec(fd, frc);
	frc = ses_skipframe(frc->fr_اگلا);
	// Go to next window.
	إذا (frc != عدم && put_سطر(fd, "wincmd w") == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

    ساكن صحيح
ses_winsizes(
    FILE	*fd,
    صحيح		restore_ناپ,
    win_T	*tab_firstwin)
{
    صحيح		n = 0;
    win_T	*wp;

    إذا (restore_ناپ && (ssop_flags & SSOP_WINSIZE))
    {
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_اگلا)
	{
	    إذا (!ses_do_win(wp))
		استمر;
	    ++n;

	    // restore height when not full height
	    إذا (wp->w_اچائی + wp->w_حالت_اچائی < topframe->fr_اچائی
		    && (fprintf(fd,
			  "exe ':%dresize ' .. ((&lines * %ld + %ld) / %ld)",
			    n, (لما)wp->w_اچائی, صفاں / 2, صفاں) < 0
						  || put_eol(fd) == FAIL))
		أرجع FAIL;

	    // restore width when not full width
	    إذا (wp->w_چوڑائی < Columns && (fprintf(fd,
		   "exe 'vert :%dresize ' .. ((&columns * %ld + %ld) / %ld)",
			    n, (لما)wp->w_چوڑائی, Columns / 2, Columns) < 0
						  || put_eol(fd) == FAIL))
		أرجع FAIL;
	}
    }
    وإلا
    {
	// Just equalise window sizes
	إذا (put_سطر(fd, "wincmd =") == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
put_view_curpos(FILE *fd, win_T *wp, حرف *spaces)
{
    صحيح r;

    إذا (wp->w_curswant == MAXCOL)
	r = fprintf(fd, "%snormal! $", spaces);
    وإلا
	r = fprintf(fd, "%snormal! 0%d|", spaces, wp->w_virtcol + 1);
    أرجع r < 0 || put_eol(fd) == FAIL ? FALSE : OK;
}

/*
 * Write commands to "fd" to restore the view of a window.
 * Caller must make sure 'scrolloff' is zero.
 */
    ساكن صحيح
put_view(
    FILE	*fd,
    win_T	*wp,
    tabpage_T	*tp,
    صحيح		جوڑو_edit,	     // add ":edit" command to view
    بنانشانی	*flagp,		     // vop_flags or ssop_flags
    صحيح		current_arg_idx,     // current argument index of the window,
				     // use -1 if unknown
    hashtab_T *terminal_bufs بن_ورتیا) // already encountered terminal buffers,
				     // can be NULL
{
    win_T	*save_curwin;
    صحيح		f;
    صحيح		do_cursor;
    صحيح		did_اگلا = FALSE;

    // Always restore cursor position for ":mksession".  For ":mkview" only
    // when 'viewoptions' contains "cursor".
    do_cursor = (flagp == &ssop_flags || *flagp & SSOP_CURSOR);

    // Local argument list.
    إذا (wp->w_alist == &global_alist)
    {
	إذا (put_سطر(fd, "argglobal") == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	إذا (ses_arglist(fd, "arglocal", &wp->w_alist->al_ga,
			flagp == &vop_flags
			|| !(*flagp & SSOP_CURDIR)
			|| tp->tp_localdir != عدم
			|| wp->w_localdir != عدم, flagp) == FAIL)
	    أرجع FAIL;
    }

    // Only when part of a session: restore the argument index.  Some
    // arguments may have been deleted, check if the index is valid.
    إذا (wp->w_arg_idx != current_arg_idx && wp->w_arg_idx < WARGCOUNT(wp)
						      && flagp == &ssop_flags)
    {
	إذا (fprintf(fd, ":%ldargu", (لما)wp->w_arg_idx + 1) < 0
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	did_اگلا = TRUE;
    }

    // Edit the file.  Skip this when ":next" already did it.
    إذا (جوڑو_edit && (!did_اگلا || wp->w_arg_idx_invalid))
    {
	إذا (bt_help(wp->w_بفر))
	{
	    حرف *curtag = "";

	    // A help buffer needs some options to be set.
	    // First, create a new empty buffer with "buftype=help".
	    // Then ":help" will re-use both the buffer and the window and set
	    // the options, even when "options" is not in 'sessionoptions'.
	    إذا (0 < wp->w_tagstackidx
		    && wp->w_tagstackidx <= wp->w_tagstacklen)
		curtag = (حرف *)wp->w_tagstack[wp->w_tagstackidx - 1].tagname;

	    إذا (put_سطر(fd, "enew | setl bt=help") == FAIL
		    || fprintf(fd, "help %s", curtag) < 0
		    || put_eol(fd) == FAIL)
		أرجع FAIL;
	}
# ifdef FEAT_TERMINAL
	وإلا إذا (bt_terminal(wp->w_بفر))
	{
	    إذا (رکن_write_session(fd, wp, terminal_bufs) == FAIL)
		أرجع FAIL;
	}
# endif
	// Load the file.
	وإلا إذا (wp->w_بفر->b_ffname != عدم
		&& !bt_nofilename(wp->w_بفر))
	{
	    // Editing a file in this buffer: use ":edit file".
	    // This may have side effects! (e.g., compressed or network file).
	    //
	    // Note, if a buffer for that file already exists, use :badd to
	    // edit that buffer, to not lose folding information (:edit resets
	    // folds in other buffers)
	    إذا (fputs("if bufexists(fnamemodify(\"", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs("\", \":p\")) | buffer ", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs(" | else | edit ", fd) < 0
		    || ses_fname(fd, wp->w_بفر, flagp, FALSE) == FAIL
		    || fputs(" | endif", fd) < 0
		    || put_eol(fd) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    // No file in this buffer, just make it empty.
	    إذا (put_سطر(fd, "enew") == FAIL)
		أرجع FAIL;
# ifdef FEAT_QUICKFIX
	    إذا (wp->w_بفر->b_ffname != عدم)
	    {
		// The buffer does have a name, but it's not a file name.
		إذا (fputs("file ", fd) < 0
			|| ses_fname(fd, wp->w_بفر, flagp, TRUE) == FAIL)
		    أرجع FAIL;
	    }
# endif
	    do_cursor = FALSE;
	}
    }

    إذا (wp->w_alt_fnum)
    {
	buf_T *alt = buflist_findnr(wp->w_alt_fnum);

	// Set the alternate file if the buffer is listed.
	إذا ((flagp == &ssop_flags)
		&& alt != عدم
		&& alt->b_fname != عدم
		&& *alt->b_fname != NUL
		&& alt->b_p_bl
		&& (fputs("balt ", fd) < 0
		|| ses_fname(fd, alt, flagp, TRUE) == FAIL))
	    أرجع FAIL;
    }

    // Local mappings and abbreviations.
    إذا ((*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
					 && makemap(fd, wp->w_بفر) == FAIL)
	أرجع FAIL;

    // Local options.  Need to go to the window temporarily.
    // Store only local values when using ":mkview" and when ":mksession" is
    // used and 'sessionoptions' doesn't include "options".
    // Some folding options are always stored when "folds" is included,
    // otherwise the folds would not be restored correctly.
    save_curwin = curwin;
    curwin = wp;
    curbuf = curwin->w_بفر;
    إذا (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))
	f = makeset(fd, OPT_LOCAL,
			     flagp == &vop_flags || !(*flagp & SSOP_OPTIONS));
# ifdef FEAT_FOLDING
    وإلا إذا (*flagp & SSOP_FOLDS)
	f = makefoldset(fd);
# endif
    وإلا
	f = OK;
    curwin = save_curwin;
    curbuf = curwin->w_بفر;
    إذا (f == FAIL)
	أرجع FAIL;

# ifdef FEAT_FOLDING
    // Save Folds when 'buftype' is empty and for help files.
    إذا ((*flagp & SSOP_FOLDS)
	    && wp->w_بفر->b_ffname != عدم
	    && (bt_عام(wp->w_بفر) || bt_help(wp->w_بفر)))
    {
	إذا (put_folds(fd, wp) == FAIL)
	    أرجع FAIL;
    }
# endif

    // Set the cursor after creating folds, since that moves the cursor.
    إذا (do_cursor)
    {

	// Restore the cursor line in the file and relatively in the
	// window.  Don't use "G", it changes the jumplist.
	إذا (put_سطر(fd, "{") == FAIL)
	    أرجع FAIL;

	إذا (wp->w_اچائی <= 0)
	{
	    إذا (fprintf(fd, "  var l: number = %ld", (لما)wp->w_cursor.lnum) < 0)
		أرجع FAIL;
	}
	وإلا إذا (fprintf(fd,
		    "  var l: number = %ld - ((%ld * winheight(0) + %ld) / %ld)",
		    (لما)wp->w_cursor.lnum,
		    (لما)(wp->w_cursor.lnum - wp->w_topline),
		    (لما)wp->w_اچائی / 2, (لما)wp->w_اچائی) < 0)
	    أرجع FAIL;

	إذا (put_eol(fd) == FAIL
		|| put_سطر(fd, "  if l < 1 | l = 1 | endif") == FAIL
		|| put_سطر(fd, "  keepjumps exe \":\" .. l") == FAIL
		|| put_سطر(fd, "  normal! zt") == FAIL
		|| fprintf(fd, "  keepjumps :%ld", (لما)wp->w_cursor.lnum) < 0
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	// Restore the cursor column and left offset when not wrapping.
	إذا (wp->w_cursor.خانہ == 0)
	{
	    إذا (put_سطر(fd, "  normal! 0") == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (!wp->w_p_wrap && wp->w_leftcol > 0 && wp->w_چوڑائی > 0)
	    {
		إذا (fprintf(fd,
			  "  var c: number = %ld - ((%ld * winwidth(0) + %ld) / %ld)",
			    (لما)wp->w_virtcol + 1,
			    (لما)(wp->w_virtcol - wp->w_leftcol),
			    (لما)wp->w_چوڑائی / 2, (لما)wp->w_چوڑائی) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  if c > 0") == FAIL
			|| fprintf(fd,
			    "    exe 'normal! ' .. c .. '|zs' .. %ld .. '|'",
			    (لما)wp->w_virtcol + 1) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "  else") == FAIL
			|| put_view_curpos(fd, wp, "    ") == FAIL
			|| put_سطر(fd, "  endif") == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (put_view_curpos(fd, wp, "  ") == FAIL)
		أرجع FAIL;
	}

	إذا (put_سطر(fd, "}") == FAIL)
	    أرجع FAIL;
    }

    // Local directory, if the current flag is not view options or the "curdir"
    // option is included.
    إذا (wp->w_localdir != عدم
			    && (flagp != &vop_flags || (*flagp & SSOP_CURDIR)))
    {
	إذا (fputs("lcd ", fd) < 0
		|| ses_put_fname(fd, wp->w_localdir, flagp) == FAIL
		|| put_eol(fd) == FAIL)
	    أرجع FAIL;
	did_lcd = TRUE;
    }

    أرجع OK;
}

# ifdef FEAT_EVAL
    ساكن صحيح
store_session_globals(FILE *fd)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحيح		todo;
    char_u	*p, *t;

    todo = (صحيح)gvht->ht_ورتیا;
    FOR_ALL_HASHTAB_اشیاء(gvht, hi, todo)
    {
	إذا (!HASHITEM_خالی_تھاواں(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    إذا ((this_var->di_tv.v_type == VAR_نمبر
			|| this_var->di_tv.v_type == VAR_STRING)
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		// Escape special characters with a backslash.  Turn a LF and
		// CR into \n and \r.
		p = vim_strsave_escaped(tv_get_string(&this_var->di_tv),
							(char_u *)"\\\"\n\r");
		إذا (p == عدم)	    // out of memory
		    اكسر;
		لكل (t = p; *t != NUL; ++t)
		    إذا (*t == '\n')
			*t = 'n';
		    وإلا إذا (*t == '\r')
			*t = 'r';
		إذا ((fprintf(fd, "g:%s = %c%s%c",
				this_var->di_کلید,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
									: ' ',
				p,
				(this_var->di_tv.v_type == VAR_STRING) ? '"'
								   : ' ') < 0)
			|| put_eol(fd) == FAIL)
		{
		    vim_free(p);
		    أرجع FAIL;
		}
		vim_free(p);
	    }
	    وإلا إذا (this_var->di_tv.v_type == VAR_FLOAT
		    && var_flavour(this_var->di_کلید) == VAR_FLAVOUR_SESSION)
	    {
		float_T f = this_var->di_tv.vval.v_float;
		صحيح sign = ' ';

		إذا (f < 0)
		{
		    f = -f;
		    sign = '-';
		}
		إذا ((fprintf(fd, "g:%s = %c%f",
					       this_var->di_کلید, sign, f) < 0)
			|| put_eol(fd) == FAIL)
		    أرجع FAIL;
	    }
	}
    }
    أرجع OK;
}
# endif

/*
 * Write openfile commands for the current buffers to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ساكن صحيح
makeopens(
    FILE	*fd,
    char_u	*dirnow)	// Current directory name
{
    buf_T	*buf;
    صحيح		only_save_windows = TRUE;
    صحيح		nr;
    صحيح		restore_ناپ = TRUE;
    صحيح		restore_اچائی_چوڑائی = FALSE;
    win_T	*wp;
    char_u	*sname;
    win_T	*edited_win = عدم;
    صحيح		restore_stal = FALSE;
    win_T	*tab_firstwin;
    چوکھٹا_T	*tab_topframe;
    صحيح		cur_arg_idx = 0;
    صحيح		اگلا_arg_idx = 0;
    صحيح		ret = FAIL;
    tabpage_T	*tp;
# ifdef FEAT_TERMINAL
    hashtab_T	terminal_bufs;

    hash_init(&terminal_bufs);
# endif

    إذا (ssop_flags & SSOP_BUFFERS)
	only_save_windows = FALSE;		// Save ALL buffers

    // Begin by setting the this_session variable, and then other
    // sessionable variables.
# ifdef FEAT_EVAL

    إذا (put_سطر(fd, "v:this_session = expand(\"<sfile>:p\")") == FAIL)
	اذهب fail;

    إذا (put_سطر(fd, "doautoall SessionLoadPre") == FAIL)
	اذهب fail;

    إذا (put_سطر(fd, "var save_splitbelow: bool") == FAIL
	    || put_سطر(fd, "var save_splitright: bool") == FAIL
	    || put_سطر(fd, "var save_winminheight: number") == FAIL
	    || put_سطر(fd, "var save_winminwidth: number") == FAIL
	    || put_سطر(fd, "var wipebuf: number = -1") == FAIL
	    || put_سطر(fd, "var shortmess_save: string") == FAIL)
	اذهب fail;

    إذا (ssop_flags & SSOP_GLOBALS)
	إذا (store_session_globals(fd) == FAIL)
	    اذهب fail;
# endif

    // Close all windows and tabs but one.
    إذا (put_سطر(fd, "silent only") == FAIL)
	اذهب fail;
    إذا ((ssop_flags & SSOP_TABPAGES)
	    && put_سطر(fd, "silent tabonly") == FAIL)
	اذهب fail;

    // Now a :cd command to the session directory or the current directory
    إذا (ssop_flags & SSOP_SESDIR)
    {
	إذا (put_سطر(fd, "exe \"cd \" .. escape(expand(\"<sfile>:p:h\"), ' ')")
								      == FAIL)
	    اذهب fail;
    }
    وإلا إذا (ssop_flags & SSOP_CURDIR)
    {
	sname = home_replace_save(عدم, globaldir != عدم ? globaldir : dirnow);
	إذا (sname == عدم
		|| fputs("cd ", fd) < 0
		|| ses_put_fname(fd, sname, &ssop_flags) == FAIL
		|| put_eol(fd) == FAIL)
	{
	    vim_free(sname);
	    اذهب fail;
	}
	vim_free(sname);
    }

    // If there is an empty, unnamed buffer we will wipe it out later.
    // Remember the buffer number.
    إذا (put_سطر(fd, "if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''") == FAIL)
	اذهب fail;
    إذا (put_سطر(fd, "  wipebuf = bufnr('%')") == FAIL)
	اذهب fail;
    إذا (put_سطر(fd, "endif") == FAIL)
	اذهب fail;

    // Save 'shortmess' if not storing options.
    إذا ((ssop_flags & SSOP_OPTIONS) == 0
	    && put_سطر(fd, "shortmess_save = &shortmess") == FAIL)
	اذهب fail;

    // Set 'shortmess' for the following.
    إذا (put_سطر(fd, "set shortmess+=aoO") == FAIL)
	اذهب fail;

    // Now save the current files, current buffer first.
    // Put all buffers into the buffer list.
    // Do it very early to preserve buffer order after loading session (which
    // can be disrupted by prior `edit` or `tabedit` calls).
    FOR_ALL_BUFFERS(buf)
    {
	إذا (!(only_save_windows && buf->b_nwindows == 0)
		&& !(buf->b_help && !(ssop_flags & SSOP_HELP))
		// Skip terminal buffers: finished ones are not useful, others
		// will be resurrected and result in a new buffer.
		&& !bt_terminal(buf)
		&& buf->b_fname != عدم
		&& buf->b_p_bl)
	{
	    إذا (fprintf(fd, "badd +%ld ", buf->b_wininfo == عدم ? 1L
					   : buf->b_wininfo->wi_fpos.lnum) < 0
		    || ses_fname(fd, buf, &ssop_flags, TRUE) == FAIL)
		اذهب fail;
	}
    }

    // the global argument list
    إذا (ses_arglist(fd, "argglobal", &global_alist.al_ga,
			    !(ssop_flags & SSOP_CURDIR), &ssop_flags) == FAIL)
	اذهب fail;

    إذا (ssop_flags & SSOP_RESIZE)
    {
	// Note: after the restore we still check it worked!
	إذا (fprintf(fd, "set lines=%ld columns=%ld" , صفاں, Columns) < 0
		|| put_eol(fd) == FAIL)
	    اذهب fail;
    }

# ifdef FEAT_GUI
    إذا (gui.وچ_use && (ssop_flags & SSOP_WINPOS))
    {
	صحيح	x, y;

	إذا (gui_mch_get_winpos(&x, &y) == OK)
	{
	    // Note: after the restore we still check it worked!
	    إذا (fprintf(fd, "winpos %d %d", x, y) < 0 || put_eol(fd) == FAIL)
		اذهب fail;
	}
    }
# endif

    // When there are two or more tabpages and 'showtabline' is 1 the tabline
    // will be displayed when creating the next tab.  That resizes the windows
    // in the first tab, which may cause problems.  Set 'showtabline' to 2
    // temporarily to avoid that.
    إذا (p_stal == 1 && پہلا_tabpage->tp_اگلا != عدم)
    {
	إذا (put_سطر(fd, "set stal=2") == FAIL)
	    اذهب fail;
	restore_stal = TRUE;
    }

    إذا ((ssop_flags & SSOP_TABPAGES))
    {
	// "tabpages" is in 'sessionoptions': Similar to ses_win_rec() below,
	// populate the tab pages first so later local options won't be copied
	// to the new tabs.
	FOR_ALL_TABPAGES(tp)
	    // Use `bufhidden=wipe` to remove empty "placeholder" buffers once
	    // they are not needed. This prevents creating extra buffers (see
	    // cause of patch 8.1.0829)
	    إذا (tp->tp_اگلا != عدم
		  && put_سطر(fd, "tabnew +setlocal\\ bufhidden=wipe") == FAIL)
		اذهب fail;
	إذا (پہلا_tabpage->tp_اگلا != عدم && put_سطر(fd, "tabrewind") == FAIL)
	    اذهب fail;
    }

    // Assume "tabpages" is in 'sessionoptions'.  If not then we only do
    // "curtab" and bail out of the loop.
    FOR_ALL_TABPAGES(tp)
    {
	صحيح	need_tabnext = FALSE;
	صحيح	cnr = 1;

	// May repeat putting Windows for each tab, when "tabpages" is in
	// 'sessionoptions'.
	// Don't use goto_tabpage(), it may change directory and trigger
	// autocommands.
	إذا ((ssop_flags & SSOP_TABPAGES))
	{
	    إذا (tp == curtab)
	    {
		tab_firstwin = firstwin;
		tab_topframe = topframe;
	    }
	    وإلا
	    {
		tab_firstwin = tp->tp_firstwin;
		tab_topframe = tp->tp_topframe;
	    }
	    إذا (tp != پہلا_tabpage)
		need_tabnext = TRUE;
	}
	وإلا
	{
	    tp = curtab;
	    tab_firstwin = firstwin;
	    tab_topframe = topframe;
	}

	// Before creating the window layout, try loading one file.  If this
	// is aborted we don't end up with a number of useless windows.
	// This may have side effects! (e.g., compressed or network file).
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_اگلا)
	{
	    إذا (ses_do_win(wp)
		    && wp->w_بفر->b_ffname != عدم
		    && !bt_help(wp->w_بفر)
		    && !bt_nofilename(wp->w_بفر))
	    {
		إذا (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
		    اذهب fail;
		need_tabnext = FALSE;

		إذا (fputs("edit ", fd) < 0
			      || ses_fname(fd, wp->w_بفر, &ssop_flags, TRUE)
								       == FAIL)
		    اذهب fail;
		إذا (!wp->w_arg_idx_invalid)
		    edited_win = wp;
		اكسر;
	    }
	}

	// If no file got edited create an empty tab page.
	إذا (need_tabnext && put_سطر(fd, "tabnext") == FAIL)
	    اذهب fail;

	إذا (tab_topframe->fr_layout != FR_پتی)
	{
	    // Save current window layout.
	    إذا (put_سطر(fd, "save_splitbelow = &splitbelow") == FAIL
		    || put_سطر(fd, "save_splitright = &splitright")
								       == FAIL)
		اذهب fail;
	    إذا (put_سطر(fd, "set splitbelow splitright") == FAIL)
		اذهب fail;
	    إذا (ses_win_rec(fd, tab_topframe) == FAIL)
		اذهب fail;
	    إذا (put_سطر(fd, "&splitbelow = save_splitbelow") == FAIL
		    || put_سطر(fd, "&splitright = save_splitright")
								       == FAIL)
		اذهب fail;
	}

	// Check if window sizes can be restored (no windows omitted).
	// Remember the window number of the current window after restoring.
	nr = 0;
	لكل (wp = tab_firstwin; wp != عدم; wp = W_اگلا(wp))
	{
	    إذا (ses_do_win(wp))
		++nr;
	    وإلا
		restore_ناپ = FALSE;
	    إذا (curwin == wp)
		cnr = nr;
	}

	إذا (tab_firstwin->w_اگلا != عدم)
	{
	    // Go to the first window.
	    إذا (put_سطر(fd, "wincmd t") == FAIL)
		اذهب fail;

	    // If more than one window, see if sizes can be restored.
	    // First set 'winheight' and 'winwidth' to 1 to avoid the windows
	    // being resized when moving between windows.
	    // Do this before restoring the view, so that the topline and the
	    // cursor can be set.  This is done again below.
	    // winminheight and winminwidth need to be set to avoid an error if
	    // the user has set winheight or winwidth.
	    إذا (!restore_اچائی_چوڑائی
		    && (put_سطر(fd, "save_winminheight = &winminheight")
									== FAIL
			|| put_سطر(fd, "save_winminwidth = &winminwidth")
								       == FAIL))
		اذهب fail;
	    إذا (put_سطر(fd, "set winminheight=0") == FAIL
		    || put_سطر(fd, "set winheight=1") == FAIL
		    || put_سطر(fd, "set winminwidth=0") == FAIL
		    || put_سطر(fd, "set winwidth=1") == FAIL)
		اذهب fail;
	    restore_اچائی_چوڑائی = TRUE;
	}
	إذا (nr > 1 && ses_winsizes(fd, restore_ناپ, tab_firstwin) == FAIL)
	    اذهب fail;

	// Restore the tab-local working directory if specified
	// Do this before the windows, so that the window-local directory can
	// override the tab-local directory.
	إذا ((ssop_flags & SSOP_CURDIR) && tp->tp_localdir != عدم)
	{
	    إذا (fputs("tcd ", fd) < 0
		     || ses_put_fname(fd, tp->tp_localdir, &ssop_flags) == FAIL
		     || put_eol(fd) == FAIL)
		اذهب fail;
	    did_lcd = TRUE;
	}

	// Restore the view of the window (options, file, cursor, etc.).
	لكل (wp = tab_firstwin; wp != عدم; wp = wp->w_اگلا)
	{
	    إذا (!ses_do_win(wp))
		استمر;
	    إذا (put_view(fd, wp, tp, wp != edited_win, &ssop_flags,
							 cur_arg_idx,
# ifdef FEAT_TERMINAL
							 &terminal_bufs
# وإلا
							 عدم
# endif
		 ) == FAIL)
		اذهب fail;
	    إذا (nr > 1 && put_سطر(fd, "wincmd w") == FAIL)
		اذهب fail;
	    اگلا_arg_idx = wp->w_arg_idx;
	}

	// The argument index in the first tab page is zero, need to set it in
	// each window.  For further tab pages it's the window where we do
	// "tabedit".
	cur_arg_idx = اگلا_arg_idx;

	// Restore cursor to the current window if it's not the first one.
	إذا (cnr > 1 && (fprintf(fd, ":%dwincmd w", cnr) < 0
						      || put_eol(fd) == FAIL))
	    اذهب fail;

	// Restore window sizes again after jumping around in windows, because
	// the current window has a minimum size while others may not.
	إذا (nr > 1 && ses_winsizes(fd, restore_ناپ, tab_firstwin) == FAIL)
	    اذهب fail;

	// Don't continue in another tab page when doing only the current one
	// or when at the last tab page.
	إذا (!(ssop_flags & SSOP_TABPAGES))
	    اكسر;
    }

    إذا (ssop_flags & SSOP_TABPAGES)
    {
	إذا (fprintf(fd, "tabnext %d", tabpage_اشاریہ(curtab)) < 0
		|| put_eol(fd) == FAIL)
	    اذهب fail;
    }
    إذا (restore_stal && put_سطر(fd, "set stal=1") == FAIL)
	اذهب fail;

    // Wipe out an empty unnamed buffer we started in.
    إذا (put_سطر(fd, "if wipebuf != -1 && len(win_findbuf(wipebuf)) == 0")
								       == FAIL)
	اذهب fail;
    إذا (put_سطر(fd, "  silent exe 'bwipe ' .. wipebuf") == FAIL)
	اذهب fail;
    إذا (put_سطر(fd, "endif") == FAIL)
	اذهب fail;

    // Re-apply 'winheight' and 'winwidth', but honor 'winminheight' and
    // 'winminwidth' settings we saved from the original user context.
    إذا (fprintf(fd, "&winheight = max([%ld, save_winminheight])", p_wh) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "&winwidth = max([%ld, save_winminwidth])", p_wiw) < 0
	    || put_eol(fd) == FAIL)
	اذهب fail;

    // Restore 'shortmess'.
    إذا (ssop_flags & SSOP_OPTIONS)
    {
	إذا (fprintf(fd, "set shortmess=%s", p_shm) < 0 || put_eol(fd) == FAIL)
	    اذهب fail;
    }
    وإلا
    {
	إذا (put_سطر(fd, "&shortmess = shortmess_save") == FAIL)
	    اذهب fail;
    }

    إذا (restore_اچائی_چوڑائی)
    {
	// Restore 'winminheight' and 'winminwidth'.
	إذا (put_سطر(fd, "&winminheight = save_winminheight") == FAIL
	      || put_سطر(fd, "&winminwidth = save_winminwidth") == FAIL)
	    اذهب fail;
    }

    // Lastly, execute the x.vim file if it exists.
    إذا (put_سطر(fd, "var sx: string = expand(\"<sfile>:p:r\") .. \"x.vim\"") == FAIL
	    || put_سطر(fd, "if filereadable(sx)") == FAIL
	    || put_سطر(fd, "  exe \"source \" .. fnameescape(sx)") == FAIL
	    || put_سطر(fd, "endif") == FAIL)
	اذهب fail;

    ret = OK;
fail:
# ifdef FEAT_TERMINAL
    hash_clear_all(&terminal_bufs, 0);
# endif
    أرجع ret;
}

/*
 * Get the name of the view file for the current buffer.
 */
    ساكن char_u *
get_view_file(صحيح c)
{
    صحيح		len = 0;
    char_u	*p, *s;
    char_u	*retval;
    char_u	*sname;

    إذا (curbuf->b_ffname == عدم)
    {
	emsg(_(e_no_file_name));
	أرجع عدم;
    }
    sname = home_replace_save(عدم, curbuf->b_ffname);
    إذا (sname == عدم)
	أرجع عدم;

    // We want a file name without separators, because we're not going to make
    // a directory.
    // "normal" path separator	-> "=+"
    // "="			-> "=="
    // ":" path separator	-> "=-"
    لكل (p = sname; *p; ++p)
	إذا (*p == '=' || vim_ispathsep(*p))
	    ++len;
    retval = alloc(STRLEN(sname) + len + STRLEN(p_vdir) + 9);
    إذا (retval != عدم)
    {
	STRCPY(retval, p_vdir);
	جوڑو_pathsep(retval);
	s = retval + STRLEN(retval);
	لكل (p = sname; *p; ++p)
	{
	    إذا (*p == '=')
	    {
		*s++ = '=';
		*s++ = '=';
	    }
	    وإلا إذا (vim_ispathsep(*p))
	    {
		*s++ = '=';
# إذا defined(BACKSLASH_وچ_FILENAME) || defined(AMIGA) || defined(VMS)
		إذا (*p == ':')
		    *s++ = '-';
		وإلا
# endif
		    *s++ = '+';
	    }
	    وإلا
		*s++ = *p;
	}
	*s++ = '=';
	*s++ = c;
	STRCPY(s, ".vim");
    }

    vim_free(sname);
    أرجع retval;
}

/*
 * ":loadview [nr]"
 */
    فراغ
ex_loadview(exarg_T *eap)
{
    char_u	*fname;

    fname = get_view_file(*eap->arg);
    إذا (fname == عدم)
	أرجع;

    (فراغ)do_سروت(fname, FALSE, DOSO_NONE, عدم);
    vim_free(fname);
}

# إذا defined(FEAT_GUI_GNOME) \
	|| (defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD))
/*
 * Generate a script that can be used to restore the current editing session.
 * Save the value of v:this_session before running :mksession in order to make
 * automagic session save fully transparent.  Return TRUE on success.
 */
    صحيح
write_session_file(char_u *filename)
{
    char_u	    *escaped_filename;
    حرف	    *mksession_cmdline;
    بنانشانی صحيح    save_ssop_flags;
    صحيح		    failed;

    // Build an ex command line to create a script that restores the current
    // session if executed.  Escape the filename to avoid nasty surprises.
    escaped_filename = vim_strsave_escaped(filename, escape_chars);
    إذا (escaped_filename == عدم)
	أرجع FALSE;
    mksession_cmdline = alloc(10 + STRLEN(escaped_filename) + 1);
    إذا (mksession_cmdline == عدم)
    {
	vim_free(escaped_filename);
	أرجع FALSE;
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
    إذا (!failed)
    {
	FILE *fd;

	fd = open_exfile(filename, TRUE, APPENDBIN);

	failed = (fd == عدم
	       || put_سطر(fd, "v:this_session = g:Save_VV_this_session")
									== FAIL
	       || put_سطر(fd, "unlet g:Save_VV_this_session") == FAIL);

	إذا (fd != عدم && fclose(fd) != 0)
	    failed = TRUE;

	إذا (failed)
	    mch_remove(filename);
    }

    أرجع !failed;
}
# endif

#جے_انت // FEAT_SESSION

#إذا defined(FEAT_SESSION) && defined(USE_CRNL)
# define MKSESSION_NL
ساكن صحيح mksession_nl = FALSE;    // use NL only in put_eol()
#جے_انت

/*
 * ":mkexrc", ":mkvimrc", ":mkview" and ":mksession".
 */
    فراغ
ex_mkrc(exarg_T	*eap)
{
    FILE	*fd;
    صحيح		failed = FALSE;
    char_u	*fname;
#جے_تعریف_شدہ FEAT_BROWSE
    char_u	*browseFile = عدم;
#جے_انت
#جے_تعریف_شدہ FEAT_SESSION
    صحيح		view_session = FALSE;
    صحيح		using_vdir = FALSE;	// using 'viewdir'?
    char_u	*viewFile = عدم;
    بنانشانی	*flagp;
#جے_انت
#إذا defined(FEAT_EVAL)
    صحيح		sid;
    scriptitem_T *si = عدم;
#جے_انت

    إذا (eap->cmdidx == CMD_mksession || eap->cmdidx == CMD_mkview)
    {
#جے_تعریف_شدہ FEAT_SESSION
	view_session = TRUE;
#وإلا
	ex_ni(eap);
	أرجع;
#جے_انت
    }

#جے_تعریف_شدہ FEAT_SESSION
    // Use the short file name until ":lcd" is used.  We also don't use the
    // short file name when 'acd' is set, that is checked later.
    did_lcd = FALSE;

    // ":mkview" or ":mkview 9": generate file name with 'viewdir'
    إذا (eap->cmdidx == CMD_mkview
	    && (*eap->arg == NUL
		|| (vim_isdigit(*eap->arg) && eap->arg[1] == NUL)))
    {
	eap->forceit = TRUE;
	fname = get_view_file(*eap->arg);
	إذا (fname == عدم)
	    أرجع;
	viewFile = fname;
	using_vdir = TRUE;
    }
    وإلا
#جے_انت
	إذا (*eap->arg != NUL)
	fname = eap->arg;
    وإلا إذا (eap->cmdidx == CMD_mkvimrc)
	fname = (char_u *)VIMRC_FILE;
#جے_تعریف_شدہ FEAT_SESSION
    وإلا إذا (eap->cmdidx == CMD_mksession)
	fname = (char_u *)SESSION_FILE;
#جے_انت
    وإلا
	fname = (char_u *)EXRC_FILE;

#جے_تعریف_شدہ FEAT_BROWSE
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(BROWSE_SAVE,
# ifdef FEAT_SESSION
		eap->cmdidx == CMD_mkview ? (char_u *)_("Save View") :
		eap->cmdidx == CMD_mksession ? (char_u *)_("Save Session") :
# endif
		(char_u *)_("Save Setup"),
		fname, (char_u *)"vim", عدم,
		(char_u *)_(BROWSE_FILTER_MACROS), عدم);
	إذا (browseFile == عدم)
	    اذهب theend;
	fname = browseFile;
	eap->forceit = TRUE;	// since dialog already asked
    }
#جے_انت

#إذا defined(FEAT_SESSION) && defined(vim_mkdir)
    // When using 'viewdir' may have to create the directory.
    إذا (using_vdir && !mch_isdir(p_vdir))
	vim_mkdir_emsg(p_vdir, 0755);
#جے_انت

    fd = open_exfile(fname, eap->forceit, WRITEBIN);
    إذا (fd != عدم)
    {
#جے_تعریف_شدہ FEAT_SESSION
	إذا (eap->cmdidx == CMD_mkview)
	    flagp = &vop_flags;
	وإلا
	    flagp = &ssop_flags;
#جے_انت

#جے_تعریف_شدہ MKSESSION_NL
	// "unix" in 'sessionoptions': use NL line separator
	إذا (view_session && (*flagp & SSOP_UNIX))
	    mksession_nl = TRUE;
#جے_انت

	// Enforce vim9script
	إذا (put_سطر(fd, "vim9script") == FAIL)
	    failed = TRUE;

	// Write the version command for :mkvimrc
	إذا (eap->cmdidx == CMD_mkvimrc)
	    (فراغ)put_سطر(fd, "version 6.0");

#جے_تعریف_شدہ FEAT_SESSION
	إذا (eap->cmdidx == CMD_mksession)
	{
	    إذا (put_سطر(fd, "g:SessionLoad = 1") == FAIL)
		failed = TRUE;
	}

	إذا (eap->cmdidx != CMD_mkview)
#جے_انت
	{
	    // Write setting 'compatible' first, because it has side effects.
	    // For that same reason only do it when needed.
	    إذا (p_cp)
		(فراغ)put_سطر(fd, "if !&cp | set cp | endif");
	    وإلا
		(فراغ)put_سطر(fd, "if &cp | set nocp | endif");
	}

#جے_تعریف_شدہ FEAT_SESSION
	إذا (!view_session
		|| (eap->cmdidx == CMD_mksession
		    && (*flagp & (SSOP_OPTIONS | SSOP_LOCALOPTIONS))))
#جے_انت
	{
	    bool do_mappings = true;
	    صحيح flags = OPT_GLOBAL;

#جے_تعریف_شدہ FEAT_SESSION
	    failed |= put_سطر(fd, "var cpo_save: string") == FAIL;

	    إذا (eap->cmdidx == CMD_mksession)
	    {
		إذا (*flagp & SSOP_SKIP_RTP)
		    flags |= OPT_SKIPRTP;

		// SSOP_LOCALOPTIONS requires only local mappings
		do_mappings = *flagp & SSOP_OPTIONS;
	    }
#جے_انت

	    إذا (do_mappings)
		failed |= (makemap(fd, عدم) == FAIL
					 || makeset(fd, flags, FALSE) == FAIL);

#إذا defined(FEAT_EVAL)
	    // Save delay load import modules.
	    // Either SSOP_LOCALOPTIONS or SSOP_OPTIONS require them
	    لكل (sid = 1; sid <= script_اشیاء.ga_len; ++sid)
	    {
		si = SCRIPT_شے(sid);

		// Autoload script paths may be absolute, relative to the
		// current script or relative to a 'runtimepath' directory
		// Ignore if missing
		إذا ((si->sn_autoload_اگیتر || si->sn_import_autoload)
			&& file_is_readable(si->sn_name))
		{
		    // Check if conflicts with a previous import
		    صحيح b_sid = sid - 1;
		    char_u *name = gettail(si->sn_name);

		    لكل (; b_sid; --b_sid)
		    {
			scriptitem_T *b_si = SCRIPT_شے(b_sid);

			// Only autoload may conflict. Ignore if missing
			إذا ((!b_si->sn_autoload_اگیتر && !b_si->sn_import_autoload)
				|| !file_is_readable(b_si->sn_name))
			    استمر;

			// compare prefixes if available
			إذا (si->sn_autoload_اگیتر != عدم && b_si->sn_autoload_اگیتر != عدم
				&& (STRCMP(si->sn_autoload_اگیتر, b_si->sn_autoload_اگیتر) == 0))
			    اكسر;

			// otherwise compare tails
			char_u *b_name = gettail(b_si->sn_name);
			إذا (STRCMP(name, b_name) == 0)
			    اكسر;
		    }

		    // import the auto script if there are no conflicts
		    إذا (fprintf(fd, "%simport autoload '%s'", b_sid ? "# " : "", si->sn_name) < 0 ||
			put_eol(fd) == FAIL)
		    failed = TRUE;
		}
	    }
#جے_انت
	}

#جے_تعریف_شدہ FEAT_SESSION
	إذا (!failed && view_session)
	{
	    إذا (put_سطر(fd, "const so_save: number = &g:so | const siso_save: number = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1") == FAIL)
		failed = TRUE;
	    إذا (eap->cmdidx == CMD_mksession)
	    {
		char_u *dirnow;	 // current directory

		dirnow = alloc(MAXPATHL);
		إذا (dirnow == عدم)
		    failed = TRUE;
		وإلا
		{
		    // Change to session file's dir.
		    إذا (mch_dirname(dirnow, MAXPATHL) == FAIL
					    || mch_chdir((حرف *)dirnow) != 0)
			*dirnow = NUL;
		    إذا (*dirnow != NUL && (ssop_flags & SSOP_SESDIR))
		    {
			إذا (vim_chdirfile(fname, عدم) == OK)
			    shorten_fnames(TRUE);
		    }
		    وإلا إذا (*dirnow != NUL
			   && (ssop_flags & SSOP_CURDIR) && globaldir != عدم)
		    {
			إذا (mch_chdir((حرف *)globaldir) == 0)
			    shorten_fnames(TRUE);
		    }

		    failed |= (makeopens(fd, dirnow) == FAIL);

		    // restore original dir
		    إذا (*dirnow != NUL && ((ssop_flags & SSOP_SESDIR)
			|| ((ssop_flags & SSOP_CURDIR) && globaldir != عدم)))
		    {
			إذا (mch_chdir((حرف *)dirnow) != 0)
			    emsg(_(e_cannot_go_پٹھ_منزل_پچھلا_directory));
			shorten_fnames(TRUE);
		    }
		    vim_free(dirnow);
		}
	    }
	    وإلا
	    {
		failed |= put_سطر(fd, "var cpo_save: string") == FAIL;
		failed |= (put_view(fd, curwin, curtab, !using_vdir, flagp, -1,
								عدم) == FAIL);
	    }
	    إذا (put_سطر(fd, "&g:so = so_save | &g:siso = siso_save") == FAIL)
		failed = TRUE;
# ifdef FEAT_تلاش_EXTRA
	    إذا (no_hlsearch && put_سطر(fd, "nohlsearch") == FAIL)
		failed = TRUE;
# endif
	    إذا (put_سطر(fd, "doautoall SessionLoadPost") == FAIL)
		failed = TRUE;
	    إذا (eap->cmdidx == CMD_mksession)
	    {
		إذا (put_سطر(fd, "unlet g:SessionLoad") == FAIL)
		    failed = TRUE;
	    }
	}
#جے_انت

	إذا (put_سطر(fd, "# vim: set ft=vim :") == FAIL)
	    failed = TRUE;

	failed |= fclose(fd);

	إذا (failed)
	    emsg(_(e_error_while_writing));
#إذا defined(FEAT_SESSION)
	وإلا إذا (eap->cmdidx == CMD_mksession)
	{
	    // successful session write - set this_session var
	    char_u	*tbuf;

	    tbuf = alloc(MAXPATHL);
	    إذا (tbuf != عدم)
	    {
		إذا (vim_بھریا_Name(fname, tbuf, MAXPATHL, FALSE) == OK)
		    سیٹ_vim_var_string(VV_THIS_SESSION, tbuf, -1);
		vim_free(tbuf);
	    }
	}
#جے_انت
#جے_تعریف_شدہ MKSESSION_NL
	mksession_nl = FALSE;
#جے_انت
    }

#جے_تعریف_شدہ FEAT_BROWSE
theend:
    vim_free(browseFile);
#جے_انت
#جے_تعریف_شدہ FEAT_SESSION
    vim_free(viewFile);
#جے_انت

    apply_autocmds(EVENT_SESSIONWRITEPOST, عدم, عدم, FALSE, curbuf);
}

#إذا defined(FEAT_VIMINFO) || defined(FEAT_SESSION)
    var_flavour_T
var_flavour(char_u *varname)
{
    char_u *p = varname;

    إذا (ASCII_ISUPPER(*p))
    {
	طالما (*(++p))
	    إذا (ASCII_ISLOWER(*p))
		أرجع VAR_FLAVOUR_SESSION;
	أرجع VAR_FLAVOUR_VIMINFO;
    }
    وإلا
	أرجع VAR_FLAVOUR_DEFAULT;
}
#جے_انت

/*
 * Write end-of-line character(s) for ":mkexrc", ":mkvimrc" and ":mksession".
 * Return FAIL for a write error.
 */
    صحيح
put_eol(FILE *fd)
{
    إذا (
#جے_تعریف_شدہ USE_CRNL
	    (
# ifdef MKSESSION_NL
	     !mksession_nl &&
# endif
	     (putc('\r', fd) < 0)) ||
#جے_انت
	    (putc('\n', fd) < 0))
	أرجع FAIL;
    أرجع OK;
}

/*
 * Write a line to "fd".
 * Return FAIL for a write error.
 */
    صحيح
put_سطر(FILE *fd, حرف *s)
{
    إذا (fputs(s, fd) < 0 || put_eol(fd) == FAIL)
	أرجع FAIL;
    أرجع OK;
}
