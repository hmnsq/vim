/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

ایستا تهی cmd_with_شمار(نویسه *cmd, char_u *bufp, size_t bufsize, long Prenum);
ایستا تهی win_init(win_T *newp, win_T *oldp, صحیح flags);
ایستا تهی win_init_some(win_T *newp, win_T *oldp);
ایستا تهی frame_comp_pos(frame_T *topfrp, صحیح *row, صحیح *col);
ایستا تهی frame_setheight(frame_T *curfrp, صحیح height);
ایستا تهی frame_setwidth(frame_T *curfrp, صحیح width);
ایستا تهی win_exchange(long);
ایستا تهی win_rotate(صحیح, صحیح);
ایستا تهی win_equal_rec(win_T *next_curwin, صحیح current, frame_T *topfr, صحیح dir, صحیح col, صحیح row, صحیح width, صحیح height);
ایستا تهی trigger_winnewpre(تهی);
ایستا تهی trigger_winclosed(win_T *win);
ایستا win_T *win_free_mem(win_T *win, صحیح *dirp, tabpage_T *tp);
ایستا frame_T *win_altframe(win_T *win, tabpage_T *tp);
ایستا tabpage_T *alt_tabpage(تهی);
ایستا win_T *frame2win(frame_T *frp);
ایستا صحیح frame_has_win(frame_T *frp, win_T *wp);
ایستا تهی win_fix_scroll(صحیح resize);
ایستا تهی win_fix_cursor(صحیح normal);
ایستا تهی frame_new_height(frame_T *topfrp, صحیح height, صحیح topfirst, صحیح wfh, صحیح set_ch);
ایستا صحیح frame_fixed_height(frame_T *frp);
ایستا صحیح frame_fixed_width(frame_T *frp);
ایستا تهی frame_add_statusline(frame_T *frp);
ایستا تهی frame_new_width(frame_T *topfrp, صحیح width, صحیح leftfirst, صحیح wfw);
ایستا تهی frame_set_vsep(frame_T *frp, صحیح add);
ایستا صحیح frame_minwidth(frame_T *topfrp, win_T *next_curwin);
ایستا تهی frame_fix_width(win_T *wp);
ایستا صحیح win_alloc_firstwin(win_T *oldwin);
ایستا تهی new_frame(win_T *wp);
ایستا tabpage_T *alloc_tabpage(تهی);
ایستا صحیح leave_tabpage(buf_T *new_curbuf, صحیح trigger_leave_autocmds);
ایستا تهی enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, صحیح trigger_enter_autocmds, صحیح trigger_leave_autocmds);
ایستا تهی frame_fix_height(win_T *wp);
ایستا صحیح frame_minheight(frame_T *topfrp, win_T *next_curwin);
ایستا صحیح frame_wincount_in_height(frame_T *topfrp);
ایستا صحیح may_open_tabpage(تهی);
ایستا صحیح win_enter_ext(win_T *wp, صحیح flags);
ایستا تهی win_free(win_T *wp, tabpage_T *tp);
ایستا تهی win_append(win_T *after, win_T *wp);
ایستا تهی frame_append(frame_T *after, frame_T *frp);
ایستا تهی frame_درج(frame_T *before, frame_T *frp);
ایستا تهی frame_remove(frame_T *frp);
ایستا تهی win_goto_ver(صحیح up, long شمار);
ایستا تهی win_goto_hor(صحیح چپ, long شمار);
ایستا تهی frame_add_height(frame_T *frp, صحیح n);
ایستا تهی last_status_rec(frame_T *fr, صحیح statusline);
#اگر defined(FEAT_STL_OPT)
ایستا صحیح frame_find_global_stlh_rec(frame_T *frp, صحیح h);
ایستا تهی frame_change_statusline_height_rec(frame_T *frp, صحیح global_stlh);
#endif
ایستا تهی frame_flatten(frame_T *frp);
ایستا تهی winframe_restore(win_T *wp, صحیح dir, frame_T *unflat_altfr);

ایستا صحیح make_snapshot_rec(frame_T *fr, frame_T **frp);
ایستا تهی clear_snapshot(tabpage_T *tp, صحیح idx);
ایستا تهی clear_snapshot_rec(frame_T *fr);
ایستا صحیح check_snapshot_rec(frame_T *sn, frame_T *fr);
ایستا win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
ایستا win_T *get_snapshot_curwin(صحیح idx);

ایستا صحیح frame_check_height(frame_T *topfrp, صحیح height);
ایستا صحیح frame_check_width(frame_T *topfrp, صحیح width);

ایستا win_T *win_alloc(win_T *after, صحیح hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

ایستا نویسه *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ایستا صحیح split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ایستا صحیح close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ایستا صحیح frame_locked = 0;

#اگر defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ایستا صحیح stl_rendered_height = 1;
ایستا صحیح stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ایستا صحیح stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    تهی
window_layout_lock(تهی)
{
    ++split_disallowed;
    ++close_disallowed;
}

    تهی
window_layout_unlock(تهی)
{
    --split_disallowed;
    --close_disallowed;
}

    صحیح
frames_locked(تهی)
{
    بازگشت frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    صحیح
window_layout_locked(شمارش CMD_index cmd)
{
    اگر (split_disallowed > 0 || close_disallowed > 0)
    {
	اگر (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	وگرنه
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ایستا تهی
log_frame_layout(frame_T *frame)
{
    ch_log(پوچ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == پوچ ? -1 : frame->fr_win->w_width,
	    frame->fr_win == پوچ ? -1 : frame->fr_win->w_height,
	    frame->fr_win == پوچ ? -1 : frame->fr_win->w_id);
    اگر (frame->fr_child != پوچ)
    {
	ch_log(پوچ, "children");
	log_frame_layout(frame->fr_child);
	اگر (frame->fr_next != پوچ)
	    ch_log(پوچ, "END of children");
    }
    اگر (frame->fr_next != پوچ)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    صحیح
check_can_set_curbuf_disabled(تهی)
{
    اگر (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    صحیح
check_can_set_curbuf_forceit(صحیح forceit)
{
    اگر (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(تهی)
{
    // In cmdwin, the alternative buffer should be used.
    بازگشت is_in_cmdwin() && prevwin != پوچ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = پوچ;

    اگر (buf == پوچ)
	بازگشت wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    اگر (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    اگر (wp == پوچ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    بازگشت wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    تهی
do_window(
    صحیح		nchar,
    long	Prenum,
    صحیح		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    صحیح		type = FIND_DEFINE;
    صحیح		len;
#endif
    char_u	cbuf[40];

    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

#define CHECK_CMDWIN \
    انجامبده { \
	اگر (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    بازگشت; \
	} \
    } وقتی (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    انتخاب (nchar)
    {
// split current window in two parts, horizontally
    حالت 'S':
    حالت Ctrl_S:
    حالت 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		اگر (bt_quickfix(curbuf))
		    برو newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(تهی)win_split((صحیح)Prenum, 0);
		بشکن;

// split current window in two parts, vertically
    حالت Ctrl_V:
    حالت 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		اگر (bt_quickfix(curbuf))
		    برو newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(تهی)win_split((صحیح)Prenum, WSP_VERT);
		بشکن;

// split current window and edit alternate file
    حالت Ctrl_HAT:
    حالت '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		اگر (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == پوچ)
		{
		    اگر (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    وگرنه
			semsg(_(e_buffer_nr_not_found), Prenum);
		    بشکن;
		}

		اگر (!curbuf_locked() && win_split(0, 0) == OK)
		    (تهی)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		بشکن;

// open new window
    حالت Ctrl_N:
    حالت 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		اگر (Prenum)
		    // window height
		    vim_snprintf((نویسه *)cbuf, اندازه(cbuf) - 5, "%ld", Prenum);
		وگرنه
		    cbuf[0] = NUL;
#اگر defined(FEAT_QUICKFIX)
		اگر (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		بشکن;

// quit current window
    حالت Ctrl_Q:
    حالت 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("quit", cbuf, اندازه(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		بشکن;

// close current window
    حالت Ctrl_C:
    حالت 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("close", cbuf, اندازه(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		بشکن;

#اگر defined(FEAT_QUICKFIX)
// close preview window
    حالت Ctrl_Z:
    حالت 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		بشکن;

// cursor to preview window
    حالت 'P':
		FOR_ALL_WINDOWS(wp)
		    اگر (wp->w_p_pvw)
			بشکن;
		اگر (wp == پوچ)
		    emsg(_(e_there_is_no_preview_window));
		وگرنه
		    win_goto(wp);
		بشکن;
#endif

// close all but current window
    حالت Ctrl_O:
    حالت 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("only", cbuf, اندازه(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		بشکن;

// cursor to next window with wrap around
    حالت Ctrl_W:
    حالت 'w':
// cursor to previous window with wrap around
    حالت 'W':
		CHECK_CMDWIN;
		اگر (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		وگرنه
		{
		    اگر (Prenum)			// go to specified window
		    {
			برای (wp = firstwin; --Prenum > 0; )
			{
			    اگر (wp->w_next == پوچ)
				بشکن;
			    وگرنه
				wp = wp->w_next;
			}
		    }
		    وگرنه
		    {
			اگر (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    اگر (wp == پوچ)
				wp = lastwin;	    // wrap around
			}
			وگرنه			    // go to next window
			{
			    wp = curwin->w_next;
			    اگر (wp == پوچ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		بشکن;

// cursor to window below
    حالت 'j':
    حالت K_DOWN:
    حالت Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		بشکن;

// cursor to window above
    حالت 'k':
    حالت K_UP:
    حالت Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		بشکن;

// cursor to left window
    حالت 'h':
    حالت K_چپ:
    حالت Ctrl_H:
    حالت K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		بشکن;

// cursor to right window
    حالت 'l':
    حالت K_راست:
    حالت Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		بشکن;

// move window to new tab page
    حالت 'T':
		CHECK_CMDWIN;
		اگر (one_window())
		    پیام(_(m_onlyone));
		وگرنه
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    اگر (win_new_tabpage((صحیح)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			اگر (curwin == wp)
			    win_close(curwin, FALSE);
			اگر (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		بشکن;

// cursor to top-left window
    حالت 't':
    حالت Ctrl_T:
		win_goto(firstwin);
		بشکن;

// cursor to bottom-right window
    حالت 'b':
    حالت Ctrl_B:
		win_goto(lastwin);
		بشکن;

// cursor to last accessed (previous) window
    حالت 'p':
    حالت Ctrl_P:
		اگر (!win_valid(prevwin))
		    beep_flush();
		وگرنه
		    win_goto(prevwin);
		بشکن;

// exchange current and next window
    حالت 'x':
    حالت Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		بشکن;

// rotate windows downwards
    حالت Ctrl_R:
    حالت 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (صحیح)Prenum1);    // downwards
		بشکن;

// rotate windows upwards
    حالت 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (صحیح)Prenum1);	    // upwards
		بشکن;

// move window to the very top/bottom/left/right
    حالت 'K':
    حالت 'J':
    حالت 'H':
    حالت 'L':
		CHECK_CMDWIN;
		اگر (ONE_WINDOW)
		    beep_flush();
		وگرنه
		{
		    صحیح dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_بالاترین : WSP_BOT);

		    (تهی)win_splitmove(curwin, (صحیح)Prenum, dir);
		}
		بشکن;

// make all windows the same width and/or height
    حالت '=':
		{
		    صحیح mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(پوچ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		بشکن;

// increase current window height
    حالت '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (صحیح)Prenum1);
		بشکن;

// decrease current window height
    حالت '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (صحیح)Prenum1);
		بشکن;

// set current window height
    حالت Ctrl__:
    حالت '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (صحیح)Prenum : 9999);
		بشکن;

// increase current window width
    حالت '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (صحیح)Prenum1);
		بشکن;

// decrease current window width
    حالت '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (صحیح)Prenum1);
		بشکن;

// set current window width
    حالت '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (صحیح)Prenum : 9999);
		بشکن;

// jump to tag and split window if tag exists (in preview window)
#اگر defined(FEAT_QUICKFIX)
    حالت '}':
		CHECK_CMDWIN;
		اگر (Prenum)
		    g_do_tagpreview = Prenum;
		وگرنه
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    حالت ']':
    حالت Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		اگر (Prenum)
		    postponed_split = Prenum;
		وگرنه
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		اگر (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		بشکن;

// edit file name under cursor in a new window
    حالت 'f':
    حالت 'F':
    حالت Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		اگر (check_text_or_curbuf_locked(پوچ))
		    بشکن;

		ptr = grab_file_name(Prenum1, &lnum);
		اگر (ptr != پوچ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = پوچ;
		    اگر ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    اگر (wp == پوچ && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			اگر (do_ecmd(0, ptr, پوچ, پوچ, ECMD_LASTL,
						   ECMD_HIDE, پوچ) == FAIL)
			{
			    /*
			     * Note: if FEAT_EVAL is defined and do_ecmd() is aborted resulting
			     * in got_int be true, win_close() unconditionally fails. In such
			     * case, the window split for do_ecmd() is left unclosed, i.e. the
			     * current window is just duplicated. To avoid this, save and load
			     * got_int value before and after closing the window.
			     */
			    sig_atomic_t    old_got_int = got_int;
			    got_int = FALSE;

			    // Failed to open the file, close the window
			    // opened for it.
			    win_close(curwin, FALSE);
			    got_int = got_int || old_got_int;

			    goto_tabpage_win(oldtab, oldwin);
			}
			وگرنه
			    wp = curwin;
		    }

		    اگر (wp != پوچ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		بشکن;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    حالت 'i':			    // Go to any match
    حالت Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    حالت 'd':			    // Go to definition, using 'define'
    حالت Ctrl_D:
		CHECK_CMDWIN;
		اگر ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    بشکن;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		اگر (ptr == پوچ)
		    بشکن;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		بشکن;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#اگر defined(FEAT_QUICKFIX)
    حالت K_KENTER:
    حالت CAR:
		اگر (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		بشکن;
#endif

// CTRL-W g  extended commands
    حالت 'g':
    حالت Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_کلیدها;   // no mapping for xchar, but allow key codes
		اگر (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_کلیدها;
		(تهی)add_to_showcmd(xchar);

		انتخاب (xchar)
		{
#اگر defined(FEAT_QUICKFIX)
		    حالت '}':
			xchar = Ctrl_RSB;
			اگر (Prenum)
			    g_do_tagpreview = Prenum;
			وگرنه
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    حالت ']':
		    حالت Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			اگر (Prenum)
			    postponed_split = Prenum;
			وگرنه
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			بشکن;

		    حالت 'f':	    // CTRL-W gf: "gf" in a new tab page
		    حالت 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			برو wingotofile;

		    حالت 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((صحیح)Prenum);
			بشکن;

		    حالت 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(صحیح)Prenum1);
			بشکن;

		    حالت TAB:	    // CTRL-W g<Tab>: go to last used tab page
			اگر (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			بشکن;

		    پیشفرض:
			beep_flush();
			بشکن;
		}
		بشکن;

    پیشفرض:	beep_flush();
		بشکن;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    تهی
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    انتخاب (*arg)
    {
    حالت 'S':
    حالت Ctrl_S:
    حالت 's':
    حالت Ctrl_N:
    حالت 'n':
    حالت 'j':
    حالت Ctrl_J:
    حالت 'k':
    حالت Ctrl_K:
    حالت 'T':
    حالت Ctrl_R:
    حالت 'r':
    حالت 'R':
    حالت 'K':
    حالت 'J':
    حالت '+':
    حالت '-':
    حالت Ctrl__:
    حالت '_':
    حالت '|':
    حالت ']':
    حالت Ctrl_RSB:
    حالت 'g':
    حالت Ctrl_G:
    حالت Ctrl_V:
    حالت 'v':
    حالت 'h':
    حالت Ctrl_H:
    حالت 'l':
    حالت Ctrl_L:
    حالت 'H':
    حالت 'L':
    حالت '>':
    حالت '<':
#اگر defined(FEAT_QUICKFIX)
    حالت '}':
#endif
    حالت 'f':
    حالت 'F':
    حالت Ctrl_F:
#ifdef FEAT_FIND_ID
    حالت 'i':
    حالت Ctrl_I:
    حالت 'd':
    حالت Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		بشکن;

    حالت Ctrl_HAT:
    حالت '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		بشکن;

    حالت Ctrl_Q:
    حالت 'q':
    حالت Ctrl_C:
    حالت 'c':
    حالت Ctrl_O:
    حالت 'o':
    حالت Ctrl_W:
    حالت 'w':
    حالت 'W':
    حالت 'x':
    حالت Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		بشکن;

#اگر defined(FEAT_QUICKFIX)
    حالت Ctrl_Z:
    حالت 'z':
    حالت 'P':
#endif
    حالت 't':
    حالت Ctrl_T:
    حالت 'b':
    حالت Ctrl_B:
    حالت 'p':
    حالت Ctrl_P:
    حالت '=':
    حالت CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		بشکن;
    }
}

    ایستا تهی
cmd_with_شمار(
    نویسه	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    اگر (Prenum > 0)
	vim_snprintf((نویسه *)bufp, bufsize, "%s %ld", cmd, Prenum);
    وگرنه
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    صحیح
check_split_disallowed(win_T *wp)
{
    اگر (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	بازگشت FAIL;
    }
    اگر (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * split the current window, implements CTRL-W s and :split
 *
 * "size" is the height or width for the new window, 0 to use half of current
 * height or width.
 *
 * "flags":
 * WSP_ROOM: require enough room for new window
 * WSP_VERT: vertical split.
 * WSP_TOP:  open window at the top-left of the shell (help window).
 * WSP_BOT:  open window at the bottom-right of the shell (quickfix window).
 * WSP_HELP: creating the help window, keep layout snapshot
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
win_split(صحیح size, صحیح flags)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت FAIL;

    اگر (check_split_disallowed(curwin) == FAIL)
	بازگشت FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    اگر (may_open_tabpage() == OK)
	بازگشت OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    اگر ((flags & WSP_بالاترین) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	بازگشت FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    اگر (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    وگرنه
	clear_snapshot(curtab, SNAP_HELP_IDX);

    اگر (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    وگرنه
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    بازگشت win_split_ins(size, flags, پوچ, 0, پوچ);
}

/*
 * When "new_wp" is NULL: split the current window in two.
 * When "new_wp" is not NULL: insert this window at the far
 * top/left/right/bottom.
 * When "to_flatten" is not NULL: flatten this frame before reorganising frames;
 * remains unflattened on failure.
 *
 * On failure, if "new_wp" was not NULL, no changes will have been made to the
 * window layout or sizes.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
win_split_ins(
    صحیح		size,
    صحیح		flags,
    win_T	*new_wp,
    صحیح		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    صحیح		new_size = size;
    صحیح		i;
    صحیح		need_status = 0;
    صحیح		do_equal = FALSE;
    صحیح		needed;
    صحیح		available;
    صحیح		oldwin_height = 0;
    صحیح		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    صحیح		before;
    صحیح		minheight;
    صحیح		wmh1;
    صحیح		did_set_fraction = FALSE;
    صحیح		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    اگر (new_wp == پوچ)
	trigger_winnewpre();

    اگر (flags & WSP_بالاترین)
	oldwin = firstwin;
    وگرنه اگر (flags & WSP_BOT)
	oldwin = lastwin;
    وگرنه
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    اگر (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	اگر (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    برو theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    اگر (gui.in_use)
	out_flush();
#endif

    اگر (flags & WSP_VERT)
    {
	صحیح	wmw1;
	صحیح	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	اگر (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	اگر (flags & (WSP_BOT | WSP_بالاترین))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	وگرنه اگر (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    برای (frp = oldwin->w_frame->fr_parent; frp != پوچ;
							frp = frp->fr_parent)
	    {
		اگر (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			اگر (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	وگرنه
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	اگر (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    برو theend;
	}
	اگر (new_size == 0)
	    new_size = oldwin->w_width / 2;
	اگر (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	اگر (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	اگر (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	اگر (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	اگر (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != پوچ)
	    do_equal = TRUE;
    }
    وگرنه
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	اگر (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	اگر (flags & (WSP_BOT | WSP_بالاترین))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	وگرنه اگر (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    برای (frp = oldwin->w_frame->fr_parent; frp != پوچ;
							frp = frp->fr_parent)
	    {
		اگر (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			اگر (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	وگرنه
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	اگر (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    برو theend;
	}
	oldwin_height = oldwin->w_height;
	اگر (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	صحیح new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != پوچ ? new_wp : curwin);
	اگر (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	اگر (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	اگر (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	اگر (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	اگر (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    set_fraction(oldwin);
	    did_set_fraction = TRUE;

	    win_setheight_win(oldwin->w_height + new_size
		    + statusline_height(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_height = oldwin->w_height;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	اگر (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != پوچ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    اگر ((flags & WSP_بالاترین) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	اگر (new_wp == پوچ)
	    wp = win_alloc(oldwin, FALSE);
	وگرنه
	    win_append(oldwin, wp);
    }
    وگرنه
    {
	اگر (new_wp == پوچ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	وگرنه
	    win_append(oldwin->w_prev, wp);
    }

    اگر (new_wp == پوچ)
    {
	اگر (wp == پوچ)
	    برو theend;

	new_frame(wp);
	اگر (wp->w_frame == پوچ)
	{
	    win_free(wp, پوچ);
	    برو theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    اگر (to_flatten != پوچ)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    اگر (flags & (WSP_بالاترین | WSP_BOT))
    {
	اگر ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    اگر (flags & WSP_BOT)
		وقتی (curfrp->fr_next != پوچ)
		    curfrp = curfrp->fr_next;
	}
	وگرنه
	    curfrp = topframe;
	before = (flags & WSP_بالاترین);
    }
    وگرنه
    {
	curfrp = oldwin->w_frame;
	اگر (flags & WSP_BELOW)
	    before = FALSE;
	وگرنه اگر (flags & WSP_ABOVE)
	    before = TRUE;
	وگرنه اگر (flags & WSP_VERT)
	    before = !p_spr;
	وگرنه
	    before = !p_sb;
    }
    اگر (curfrp->fr_parent == پوچ || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = پوچ;
	frp->fr_prev = پوچ;
	curfrp->fr_child = frp;
	curfrp->fr_win = پوچ;
	curfrp = frp;
	اگر (frp->fr_win != پوچ)
	    oldwin->w_frame = frp;
	وگرنه
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    اگر (new_wp == پوچ)
	frp = wp->w_frame;
    وگرنه
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    اگر (before)
	frame_درج(curfrp, frp);
    وگرنه
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    اگر (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    اگر (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	اگر (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	اگر (flags & (WSP_بالاترین | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    صحیح stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	وگرنه
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_height(wp, VISIBLE_HEIGHT(oldwin));
	    wp->w_status_height = oldwin->w_status_height;
	}
	frp->fr_height = curfrp->fr_height;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_width(wp, new_size);
	اگر (before)
	    wp->w_vsep_width = 1;
	وگرنه
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	اگر (flags & (WSP_بالاترین | WSP_BOT))
	{
	    اگر (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_بالاترین) != 0)), flags & WSP_بالاترین,
								       FALSE);
	}
	وگرنه
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	اگر (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	وگرنه		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    وگرنه
    {
	// width and column of new window is same as current window
	اگر (flags & (WSP_بالاترین | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	وگرنه
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	صحیح old_status_height = oldwin->w_status_height;
	اگر (flags & (WSP_بالاترین | WSP_BOT))
	{
	    صحیح new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    اگر (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    اگر (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_بالاترین, FALSE,
									FALSE);
	}
	وگرنه
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	اگر (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	وگرنه		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    اگر (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    اگر (flags & (WSP_بالاترین | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    اگر (need_status)
    {
	پیام_row = Rows - 1;
	پیام_col = sc_col;
	پیام_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	پیام_row = Rows - 1;
	پیام_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    اگر (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    وگرنه اگر (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    اگر (flags & WSP_VERT)
    {
	i = p_wiw;
	اگر (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	اگر (gui.in_use)
	    gui_init_which_components(پوچ);
#endif
    }
    وگرنه
    {
	i = p_wh;
	اگر (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (تهی)win_enter_ext(wp, (new_wp == پوچ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    اگر (flags & WSP_VERT)
	p_wiw = i;
    وگرنه
	p_wh = i;
    retval = OK;

theend:
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    بازگشت retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ایستا تهی
win_init(win_T *newp, win_T *oldp, صحیح flags UNUSED)
{
    صحیح		i;

    newp->w_buffer = oldp->w_buffer;
#ifdef FEAT_SYN_HL
    newp->w_s = &(oldp->w_buffer->b_s);
#endif
    oldp->w_buffer->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_set_curswant = oldp->w_set_curswant;
    newp->w_topline = oldp->w_topline;
#ifdef FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#endif
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_row = oldp->w_prev_fraction_row;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#ifdef FEAT_QUICKFIX
    اگر (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = پوچ;
	newp->w_llist_ref = پوچ;
    }
    وگرنه
	copy_loclist_پشته(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == پوچ)
				    ? پوچ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == پوچ)
				    ? پوچ : vim_strsave(oldp->w_prevdir);

    اگر (*p_spk != 'c')
    {
	اگر (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    برای (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	اگر (tag->tagname != پوچ)
	    tag->tagname = vim_strsave(tag->tagname);
	اگر (tag->user_data != پوچ)
	    tag->user_data = vim_strsave(tag->user_data);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#ifdef FEAT_FOLDING
    copyFoldingState(oldp, newp);
#endif

    win_init_some(newp, oldp);

#ifdef FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = افزودن_highlight_overrides(newp->w_hl, newp->w_hl_len);
	اگر (newp->w_buffer->b_term != پوچ)
	    term_init_default_colors(newp->w_buffer->b_term);
	اگر (pushed)
	    برداشت_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ایستا تهی
win_init_some(win_T *newp, win_T *oldp)
{
    // Use the same argument list.
    newp->w_alist = oldp->w_alist;
    ++newp->w_alist->al_refcount;
    newp->w_arg_idx = oldp->w_arg_idx;

    // copy options from existing window
    win_copy_options(oldp, newp);
}

/*
 * Return TRUE if "win" is a global popup or a popup in the current tab page.
 */
    صحیح
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp == win)
	    بازگشت TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp == win)
	    بازگشت TRUE;
#endif
    بازگشت FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    صحیح
win_valid(win_T *win)
{
    win_T	*wp;

    اگر (win == پوچ)
	بازگشت FALSE;
    FOR_ALL_WINDOWS(wp)
	اگر (wp == win)
	    بازگشت TRUE;
    بازگشت win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(صحیح id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_id == id)
	    بازگشت wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_id == id)
	    بازگشت wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_id == id)
	    بازگشت wp;
#endif
    بازگشت پوچ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    صحیح
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    اگر (win == پوچ)
	بازگشت FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    اگر (wp == win)
		بازگشت TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    اگر (wp == win)
		بازگشت TRUE;
#endif
    }
    بازگشت win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    صحیح
win_شمار(تهی)
{
    win_T	*wp;
    صحیح		شمار = 0;

    FOR_ALL_WINDOWS(wp)
	++شمار;
    بازگشت شمار;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    صحیح
make_windows(
    صحیح		شمار,
    صحیح		vertical)  // split windows vertically if TRUE
{
    صحیح		maxcount;
    صحیح		todo;

    اگر (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    وگرنه
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    اگر (maxcount < 2)
	maxcount = 2;
    اگر (شمار > maxcount)
	شمار = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    اگر (شمار > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    برای (todo = شمار - 1; todo > 0; --todo)
	اگر (vertical)
	{
	    اگر (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		بشکن;
	}
	وگرنه
	{
	    اگر (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		بشکن;
	}

    unblock_autocmds();

    // return actual number of windows
    بازگشت (شمار - todo);
}

/*
 * Exchange current and next window
 */
    ایستا تهی
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    صحیح		temp;

    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    اگر (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	بازگشت;
    }
    اگر (text_or_buf_locked())
    {
	beep_flush();
	بازگشت;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    اگر (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	وقتی (frp != پوچ && --Prenum > 0)
	    frp = frp->fr_next;
    }
    وگرنه اگر (curwin->w_frame->fr_next != پوچ)	// Swap with next
	frp = curwin->w_frame->fr_next;
    وگرنه    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    اگر (frp == پوچ || frp->fr_win == پوچ || frp->fr_win == curwin)
	بازگشت;
    wp = frp->fr_win;

/*
 * 1. remove curwin from the list. Remember after which window it was in wp2
 * 2. insert curwin before wp in the list
 * if wp != wp2
 *    3. remove wp from the list
 *    4. insert wp after wp2
 * 5. exchange the status line height and vsep width.
 */
    wp2 = curwin->w_prev;
    frp2 = curwin->w_frame->fr_prev;
    اگر (wp->w_prev != curwin)
    {
	win_remove(curwin, پوچ);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_درج(frp, curwin->w_frame);
    }
    اگر (wp != wp2)
    {
	win_remove(wp, پوچ);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	اگر (frp2 == پوچ)
	    frame_درج(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	وگرنه
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    اگر (temp != wp->w_status_height)
    {
	win_new_height(curwin, curwin->w_height + temp - wp->w_status_height);
	win_new_height(wp, wp->w_height + wp->w_status_height - temp);
    }
    curwin->w_status_height = wp->w_status_height;
    wp->w_status_height = temp;
    temp = curwin->w_vsep_width;
    curwin->w_vsep_width = wp->w_vsep_width;
    wp->w_vsep_width = temp;

    frame_fix_height(curwin);
    frame_fix_height(wp);
    frame_fix_width(curwin);
    frame_fix_width(wp);

    win_comp_pos();		// recompute window positions
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    اگر (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    وگرنه اگر (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ایستا تهی
win_rotate(صحیح upwards, صحیح شمار)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    صحیح		n;

    اگر (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	بازگشت;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	اگر (frp->fr_win == پوچ)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    بازگشت;
	}

    وقتی (شمار--)
    {
	اگر (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, پوچ);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    برای ( ; frp->fr_next != پوچ; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	وگرنه			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    برای (frp = curwin->w_frame; frp->fr_next != پوچ;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, پوچ);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_درج(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	اگر (n != wp1->w_status_height)
	{
	    win_new_height(wp2, wp2->w_height + n - wp1->w_status_height);
	    win_new_height(wp1, wp1->w_height + wp1->w_status_height - n);
	}
	wp2->w_status_height = wp1->w_status_height;
	wp1->w_status_height = n;
	frame_fix_height(wp1);
	frame_fix_height(wp2);
	n = wp2->w_vsep_width;
	wp2->w_vsep_width = wp1->w_vsep_width;
	wp1->w_vsep_width = n;
	frame_fix_width(wp1);
	frame_fix_width(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    صحیح
win_splitmove(win_T *wp, صحیح size, صحیح flags)
{
    صحیح		dir;
    صحیح		height = wp->w_height;
    frame_T	*unflat_altfr;

    اگر (ONE_WINDOW)
	بازگشت OK;	// nothing to do
    اگر (check_split_disallowed(wp) == FAIL)
	بازگشت FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, پوچ, &unflat_altfr);
    win_remove(wp, پوچ);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    اگر (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	بازگشت FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    اگر (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	اگر (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#اگر defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    بازگشت OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    تهی
win_move_after(win_T *win1, win_T *win2)
{
    صحیح		height;

    // check if the arguments are reasonable
    اگر (win1 == win2)
	بازگشت;

    // check if there is something to do
    اگر (win2->w_next != win1)
    {
	اگر (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    بازگشت;
	}

	// may need to move the status line/vertical separator of the last
	// window
	اگر (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    اگر (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	وگرنه اگر (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    اگر (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, پوچ);
	frame_remove(win1->w_frame);
	win_append(win2, win1);
	frame_append(win2->w_frame, win1->w_frame);

	win_comp_pos();	// recompute w_winrow for all windows
	redraw_later(UPD_NOT_VALID);
    }
    win_enter(win1, FALSE);
}

/*
 * Make all windows the same height.
 * 'next_curwin' will soon be the current window, make sure it has enough
 * rows.
 */
    تهی
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    صحیح		current,	// do only frame with current window
    صحیح		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    اگر (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == پوچ ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    اگر (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ایستا تهی
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    صحیح		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    صحیح		dir,		// 'v', 'h' or 'b', see win_equal()
    صحیح		col,		// horizontal position for frame
    صحیح		row,		// vertical position for frame
    صحیح		width,		// new width of frame
    صحیح		height)		// new height of frame
{
    صحیح		n, m;
    صحیح		extra_sep = 0;
    صحیح		wincount, totwincount = 0;
    frame_T	*fr;
    صحیح		next_curwin_size = 0;
    صحیح		room = 0;
    صحیح		new_size;
    صحیح		has_next_curwin = 0;
    صحیح		hnc;

    اگر (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	اگر (topfr->fr_height != height || topfr->fr_win->w_winrow != row
		|| topfr->fr_width != width || topfr->fr_win->w_wincol != col
	   )
	{
	    topfr->fr_win->w_winrow = row;
	    frame_new_height(topfr, height, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = col;
	    frame_new_width(topfr, width, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    وگرنه اگر (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	اگر (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    اگر (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    وگرنه
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_next_curwin = frame_has_win(topfr, next_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = frame_minwidth(topfr, next_curwin);
	    room = width - m;
	    اگر (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    وگرنه
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    اگر (!frame_fixed_width(fr))
			ادامه;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    اگر (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			اگر (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    وگرنه
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == پوچ
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    اگر (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		اگر (next_curwin_size == -1)
		{
		    اگر (!has_next_curwin)
			next_curwin_size = 0;
		    وگرنه اگر (totwincount > 1
			    && (room + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			next_curwin_size = (room + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			room -= next_curwin_size - p_wiw;
		    }
		    وگرنه
			next_curwin_size = p_wiw;
		}
	    }

	    اگر (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    اگر (fr->fr_next == پوچ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    وگرنه اگر (dir == 'v')
		new_size = fr->fr_width;
	    وگرنه اگر (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وگرنه
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == پوچ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		اگر (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		وگرنه
		    hnc = FALSE;
		اگر (hnc)	    // don't count next_curwin
		    --wincount;
		اگر (totwincount == 0)
		    new_size = room;
		وگرنه
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		اگر (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    اگر (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		وگرنه
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    اگر (!current || dir != 'v' || topfr->fr_parent != پوچ
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    وگرنه // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	اگر (dir != 'h')			// equalize frame heights
	{
	    totwincount = frame_wincount_in_height(topfr);
	    has_next_curwin = frame_has_win(topfr, next_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = frame_minheight(topfr, next_curwin);
	    room = height - m;
	    اگر (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    وگرنه
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    اگر (!frame_fixed_height(fr))
			ادامه;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    اگر (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			اگر (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    وگرنه
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    اگر (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		اگر (next_curwin_size == -1)
		{
		    اگر (!has_next_curwin)
			next_curwin_size = 0;
		    وگرنه اگر (totwincount > 1
			    && (room + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			next_curwin_size = (room + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			room -= next_curwin_size - p_wh;
		    }
		    وگرنه
			next_curwin_size = p_wh;
		}
	    }

	    اگر (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    اگر (fr->fr_next == پوچ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    وگرنه اگر (dir == 'h')
		new_size = fr->fr_height;
	    وگرنه اگر (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وگرنه
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		اگر (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		وگرنه
		    hnc = FALSE;
		اگر (hnc)	    // don't count next_curwin
		    --wincount;
		اگر (totwincount == 0)
		    new_size = room;
		وگرنه
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		اگر (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		وگرنه
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    اگر (!current || dir != 'h' || topfr->fr_parent != پوچ
		    || (new_size != fr->fr_height)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							     width, new_size);
	    row += new_size;
	    height -= new_size;
	    totwincount -= wincount;
	}
    }
}

#اگر defined(FEAT_JOB_CHANNEL)
    تهی
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    اگر (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	بازگشت;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_درج = restart_edit;
    اگر (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    اگر ((State & MODE_درج) && !stop_درج_mode)
    {
	stop_درج_mode = TRUE;
	اگر (win->w_buffer->b_prompt_درج == NUL)
	    win->w_buffer->b_prompt_درج = 'A';
    }
}

    تهی
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    اگر (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	بازگشت;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    اگر (win->w_buffer->b_prompt_درج != NUL)
	stop_درج_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    اگر ((State & MODE_درج) == 0)
	restart_edit = win->w_buffer->b_prompt_درج;
}
#endif

    ایستا تهی
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_سطرها_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.col = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.col = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.col = 0;
    wp->w_topline = 1;
#ifdef FEAT_DIFF
    wp->w_topfill = 0;
#endif
    wp->w_botline = 2;
    wp->w_valid = 0;
#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_buffer->b_s;
#endif
#ifdef FEAT_TERMINAL
    term_reset_hlfwin(wp);
#endif
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    تهی
curwin_init(تهی)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    تهی
close_windows(
    buf_T	*buf,
    صحیح		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    صحیح		شمار = tabpage_index(پوچ);

    ++RedrawingDisabled;

    برای (wp = firstwin; wp != پوچ && !ONE_WINDOW; )
    {
	اگر (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    اگر (window_layout_locked(CMD_SIZE))
		برو theend;  // Only give one error message.
	    اگر (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		بشکن;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	وگرنه
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    برای (tp = first_tabpage; tp != پوچ; tp = nexttp)
    {
	nexttp = tp->tp_next;
	اگر (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		اگر (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    اگر (window_layout_locked(CMD_SIZE))
			برو theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    بشکن;
		}
    }

theend:
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    اگر (شمار != tabpage_index(پوچ))
	apply_autocmds(EVENT_TABCLOSED, پوچ, پوچ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    صحیح
last_window(تهی)
{
    بازگشت (one_window() && first_tabpage->tp_next == پوچ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    صحیح
one_window(تهی)
{
    win_T	*wp;
    صحیح		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (!is_aucmd_win(wp))
	{
	    اگر (seen_one)
		بازگشت FALSE;
	    seen_one = TRUE;
	}
    }
    بازگشت TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ایستا صحیح
close_last_window_tabpage(
    win_T	*win,
    صحیح		free_buf,
    tabpage_T   *prev_curtab)
{
    اگر (!ONE_WINDOW)
	بازگشت FALSE;

    buf_T	*old_curbuf = curbuf;
    tabpage_T	*save_lastused = lastused_tabpage;

    /*
     * Closing the last window in a tab page.  First go to another tab
     * page and then close the window and the tab page.  This avoids that
     * curwin and curtab are invalid while we are freeing memory, they may
     * be used in GUI events.
     * Don't trigger *Enter autocommands yet, they may use wrong values, so do
     * that below.
     * Do trigger *Leave autocommands, unless win->w_buffer is NULL, in which
     * case they have already been triggered.
     */
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != پوچ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    اگر (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    اگر (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, پوچ, پوچ, FALSE, curbuf);
#اگر defined(FEAT_TABPANEL)
    اگر (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, پوچ, پوچ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, پوچ, پوچ, FALSE, curbuf);
    اگر (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
    بازگشت TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ایستا تهی
win_close_buffer(win_T *win, صحیح action, صحیح abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    اگر (win->w_buffer != پوچ)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    اگر (win->w_buffer != پوچ && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    اگر (win->w_buffer != پوچ)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	اگر (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	اگر (!bufref_valid(&bufref))
	    curbuf = firstbuf;
    }
}

/*
 * Close window "win".  Only works for the current tab page.
 * If "free_buf" is TRUE related buffer may be unloaded.
 *
 * Called by :quit, :close, :xit, :wq and findtag().
 * Returns FAIL when the window was not closed.
 */
    صحیح
win_close(win_T *win, صحیح free_buf)
{
    win_T	*wp;
    صحیح		other_buffer = FALSE;
    صحیح		close_curwin = FALSE;
    صحیح		dir;
    صحیح		help_window = FALSE;
    صحیح		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    صحیح		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    صحیح		did_decrement = FALSE;
#endif

#اگر defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    اگر (may_close_term_popup() == OK)
	بازگشت OK;
#endif
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت FAIL;

    اگر (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	بازگشت FAIL;
    }
    اگر (window_layout_locked(CMD_close))
	بازگشت FAIL;

    اگر (win_locked(win) || (win->w_buffer != پوچ
					       && win->w_buffer->b_locked > 0))
	بازگشت FAIL; // window is already being closed
    اگر (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	بازگشت FAIL;
    }
    اگر ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	بازگشت FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    اگر (close_last_window_tabpage(win, free_buf, prev_curtab))
	بازگشت FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    اگر (bt_help(win->w_buffer))
	help_window = TRUE;
    وگرنه
	clear_snapshot(curtab, SNAP_HELP_IDX);

    اگر (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    وگرنه
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    اگر (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, پوچ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	اگر (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    اگر (!win_valid(win))
		بازگشت FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, پوچ, پوچ, FALSE, curbuf);
	    اگر (!win_valid(win))
		بازگشت FAIL;
	    --win->w_locked;
	    اگر (last_window())
		بازگشت FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, پوچ, پوچ, FALSE, curbuf);
	اگر (!win_valid(win))
	    بازگشت FAIL;
	--win->w_locked;
	اگر (last_window())
	    بازگشت FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	اگر (aborting())
	    بازگشت FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    اگر (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    اگر (popup_win_closed(win) && !win_valid(win))
	بازگشت FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    اگر (!win_valid_any_tab(win))
	بازگشت OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    اگر (win_valid(win) && win->w_buffer == پوچ
#اگر defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	اگر (curwin->w_buffer == پوچ)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    اگر (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == پوچ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	بازگشت FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    اگر (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	بازگشت FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    اگر (win->w_buffer != پوچ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, پوچ);

    اگر (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	اگر (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    اگر (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	اگر (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    برای (;;)
	    {
		اگر (wp->w_next == پوچ)
		    wp = firstwin;
		وگرنه
		    wp = wp->w_next;
		اگر (wp == curwin)
		    بشکن;
		اگر (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    بشکن;
		}
	    }
	}
#endif
	curbuf = curwin->w_buffer;
	close_curwin = TRUE;

	// The cursor position may be invalid if the buffer changed after last
	// using the window.
	check_cursor();
    }

    /*
     * If last window has a status line now and we don't want one, remove the
     * status line.  Do this before win_equal(), because it may change the
     * height of a window
     */
    last_status(FALSE);

    اگر (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    وگرنه
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    اگر (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#وگرنه
	(تهی)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	اگر (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
    }

    اگر (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != پوچ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, پوچ, پوچ, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    اگر (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    اگر (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    اگر (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	صحیح	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    اگر (dwin->w_p_diff)
		++diffcount;
	اگر (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#اگر defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    اگر (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(پوچ);
#endif

    redraw_all_later(UPD_NOT_VALID);
    بازگشت OK;
}

    ایستا تهی
trigger_winnewpre(تهی)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, پوچ, پوچ, FALSE, پوچ);
    window_layout_unlock();
}

    ایستا تهی
trigger_winclosed(win_T *win)
{
    ایستا صحیح	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    اگر (recursive)
	بازگشت;
    recursive = TRUE;
    vim_snprintf((نویسه *)winid, اندازه(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    تهی
trigger_tabclosedpre(tabpage_T *tp)
{
    ایستا صحیح	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    اگر (!has_tabclosedpre() || recursive)
	بازگشت;

    اگر (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, پوچ, پوچ, FALSE, پوچ);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    اگر (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    وگرنه
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    تهی
snapshot_windows_scroll_size(تهی)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_last_topline = wp->w_topline;
#ifdef FEAT_DIFF
	wp->w_last_topfill = wp->w_topfill;
#endif
	wp->w_last_leftcol = wp->w_leftcol;
	wp->w_last_skipcol = wp->w_skipcol;
	wp->w_last_width = wp->w_width;
	wp->w_last_height = wp->w_height;
    }
}

ایستا صحیح did_initial_scroll_size_snapshot = FALSE;

    تهی
may_make_initial_scroll_size_snapshot(تهی)
{
    اگر (!did_initial_scroll_size_snapshot)
    {
	did_initial_scroll_size_snapshot = TRUE;
	snapshot_windows_scroll_size();
    }
}

#ifdef FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ایستا dict_T *
make_win_info_dict(
	صحیح width,
	صحیح height,
	صحیح topline,
# ifdef FEAT_DIFF
	صحیح topfill,
# endif
	صحیح leftcol,
	صحیح skipcol)
{
    dict_T *d = dict_alloc();
    اگر (d == پوچ)
	بازگشت پوچ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    وقتی (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_عدد;

	tv.vval.v_عدد = width;
	اگر (dict_add_tv(d, "width", &tv) == FAIL)
	    بشکن;
	tv.vval.v_عدد = height;
	اگر (dict_add_tv(d, "height", &tv) == FAIL)
	    بشکن;
	tv.vval.v_عدد = topline;
	اگر (dict_add_tv(d, "topline", &tv) == FAIL)
	    بشکن;
# ifdef FEAT_DIFF
	tv.vval.v_عدد = topfill;
# وگرنه
	tv.vval.v_عدد = 0;
# endif
	اگر (dict_add_tv(d, "topfill", &tv) == FAIL)
	    بشکن;
	tv.vval.v_عدد = leftcol;
	اگر (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    بشکن;
	tv.vval.v_عدد = skipcol;
	اگر (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    بشکن;
	بازگشت d;
    }
    dict_unref(d);
    بازگشت پوچ;
}
#endif

// Return values of check_window_scroll_resize():
#define CWSR_SCROLLED	1  // at least one window scrolled
#define CWSR_RESIZED	2  // at least one window size changed

/*
 * This function is used for three purposes:
 * 1. Goes over all windows in the current tab page and sets:
 *    "size_count" to the nr of windows with size changes.
 *    "first_scroll_win" to the first window with any relevant changes.
 *    "first_size_win" to the first window with size changes.
 *
 * 2. When the first three arguments are NULL and "winlist" is not NULL,
 *    "winlist" is set to the list of window IDs with size changes.
 *
 * 3. When the first three arguments are NULL and "v_event" is not NULL,
 *    information about changed windows is added to "v_event".
 */
    ایستا تهی
check_window_scroll_resize(
	صحیح	*size_شمار,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    صحیح listidx = 0;
    صحیح tot_width = 0;
    صحیح tot_height = 0;
    صحیح tot_topline = 0;
# ifdef FEAT_DIFF
    صحیح tot_topfill = 0;
# endif
    صحیح tot_leftcol = 0;
    صحیح tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	صحیح ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	صحیح size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	اگر (size_changed)
	{
#ifdef FEAT_EVAL
	    اگر (winlist != پوچ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_عدد;
		tv.vval.v_عدد = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    وگرنه
#endif
		اگر (size_شمار != پوچ)
	    {
		++*size_شمار;
		اگر (*first_size_win == پوچ)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		اگر (*first_scroll_win == پوچ && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	صحیح scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	اگر (scroll_changed
	    && first_scroll_win != پوچ && *first_scroll_win == پوچ)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	اگر ((size_changed || scroll_changed) && v_event != پوچ)
	{
	    // Add info about this window to the v:event dictionary.
	    صحیح width = wp->w_width - wp->w_last_width;
	    صحیح height = wp->w_height - wp->w_last_height;
	    صحیح topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    صحیح topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    صحیح leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    صحیح skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    اگر (d == پوچ)
		بشکن;
	    نویسه winid[NUMBUFLEN];
	    vim_snprintf(winid, اندازه(winid), "%d", wp->w_id);
	    اگر (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		بشکن;
	    }
	    --d->dv_refcount;

	    tot_width += abs(width);
	    tot_height += abs(height);
	    tot_topline += abs(topline);
# ifdef FEAT_DIFF
	    tot_topfill += abs(topfill);
# endif
	    tot_leftcol += abs(leftcol);
	    tot_skipcol += abs(skipcol);
	}
#endif
    }

#ifdef FEAT_EVAL
    اگر (v_event != پوچ)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	اگر (alldict != پوچ)
	{
	    اگر (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    وگرنه
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    تهی
may_trigger_win_scrolled_resized(تهی)
{
    ایستا صحیح	    recursive = FALSE;
    صحیح		    do_resize = has_winresized();
    صحیح		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    اگر (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	بازگشت;

    صحیح size_شمار = 0;
    win_T *first_scroll_win = پوچ, *first_size_win = پوچ;
    check_window_scroll_resize(&size_شمار, &first_scroll_win, &first_size_win,
								    پوچ, پوچ);
    صحیح trigger_resize = do_resize && size_شمار > 0;
    صحیح trigger_scroll = do_scroll && first_scroll_win != پوچ;
    اگر (!trigger_resize && !trigger_scroll)
	بازگشت;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = پوچ;
    اگر (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_اقلام(size_شمار);
	اگر (windows_list != پوچ)
	    check_window_scroll_resize(پوچ, پوچ, پوچ, windows_list, پوچ);
    }

    dict_T *scroll_dict = پوچ;
    اگر (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	اگر (scroll_dict != پوچ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(پوچ, پوچ, پوچ, پوچ, scroll_dict);
	}
    }
#endif

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_size();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    window_layout_lock();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    اگر (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != پوچ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	اگر (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_اقلام_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((نویسه *)winid, اندازه(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    اگر (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != پوچ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", پوچ);
	dict_set_اقلام_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((نویسه *)winid, اندازه(winid), "%d",
						       first_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   first_scroll_win->w_buffer);
#ifdef FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#endif
    }

    recursive = FALSE;
    window_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    تهی
win_close_othertab(win_T *win, صحیح free_buf, tabpage_T *tp)
{
    win_T	*wp;
    صحیح		dir;
    tabpage_T   *ptp = پوچ;
    صحیح		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    اگر (window_layout_locked(CMD_SIZE))
	بازگشت;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    اگر (win_locked(win) || (win->w_buffer != پوچ
					       && win->w_buffer->b_locked > 0))
	بازگشت; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    اگر (win->w_buffer != پوچ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	اگر (!win_valid_any_tab(win))
	    بازگشت;
    }

    اگر (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	اگر (!win_valid_any_tab(win))
	    بازگشت;
    }

    اگر (win->w_buffer != پوچ)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    برای (ptp = first_tabpage; ptp != پوچ && ptp != tp; ptp = ptp->tp_next)
	;
    اگر (ptp == پوچ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	اگر (win_valid_any_tab(win) && win->w_buffer == پوچ)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    اگر (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	بازگشت;
    }

    // Autocommands may have closed the window already.
    برای (wp = tp->tp_firstwin; wp != پوچ && wp != win; wp = wp->w_next)
	;
    اگر (wp == پوچ)
	بازگشت;

    // When closing the last window in a tab page remove the tab page.
    اگر (tp->tp_firstwin == tp->tp_lastwin)
    {
	صحیح	h = tabline_height();

	اگر (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	وگرنه
	{
	    برای (ptp = first_tabpage; ptp != پوچ && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    اگر (ptp == پوچ)
	    {
		internal_error("win_close_othertab()");
		بازگشت;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	اگر (h != tabline_height())
	    shell_new_rows();
    }

    اگر (win->w_buffer != پوچ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    اگر (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ایستا win_T *
win_free_mem(
    win_T	*win,
    صحیح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == پوچ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, پوچ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    اگر (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    بازگشت wp;
}

#اگر defined(EXITFREE)
    تهی
win_free_all(تهی)
{
    صحیح		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = پوچ;
    cmdwin_win = پوچ;

    وقتی (first_tabpage->tp_next != پوچ)
	tabpage_close(TRUE);

    برای (صحیح i = 0; i < AUCMD_WIN_شمار; ++i)
	اگر (aucmd_win[i].auc_win != پوچ)
	{
	    (تهی)win_free_mem(aucmd_win[i].auc_win, &dummy, پوچ);
	    aucmd_win[i].auc_win = پوچ;
	}

    وقتی (firstwin != پوچ)
	(تهی)win_free_mem(firstwin, &dummy, پوچ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = پوچ;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    صحیح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    صحیح		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    اگر (tp == پوچ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	بازگشت پوچ;

    frame_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_parent);
    row = wp->w_winrow;
    col = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    اگر (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != پوچ)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    اگر (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	اگر (frp2->fr_win != پوچ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    وقتی (frp != پوچ || frp3 != پوچ)
	    {
		اگر (frp != پوچ)
		{
		    اگر (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			بشکن;
		    }
		    frp = frp->fr_prev;
		}
		اگر (frp3 != پوچ)
		{
		    اگر (frp3->fr_win != پوچ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			بشکن;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    وگرنه
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	اگر (frp2->fr_win != پوچ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    وقتی (frp != پوچ || frp3 != پوچ)
	    {
		اگر (frp != پوچ)
		{
		    اگر (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			بشکن;
		    }
		    frp = frp->fr_prev;
		}
		اگر (frp3 != پوچ)
		{
		    اگر (frp3->fr_win != پوچ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			بشکن;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_width(frp2, frp2->fr_width + frp_close->fr_width,
			    frp2 == frp_close->fr_next, !frame_fixed_width(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    اگر (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    اگر (unflat_altfr == پوچ)
	frame_flatten(frp2);
    وگرنه
	*unflat_altfr = frp2;

    frame_locked--;

    بازگشت wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ایستا تهی
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    اگر (frp->fr_next != پوچ || frp->fr_prev != پوچ)
	بازگشت;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    اگر (frp->fr_win != پوچ)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    اگر (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    اگر (frp != پوچ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	اگر (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	اگر (frp2->fr_prev != پوچ)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	برای (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    اگر (frp3->fr_next == پوچ)
	    {
		frp3->fr_next = frp2->fr_next;
		اگر (frp2->fr_next != پوچ)
		    frp2->fr_next->fr_prev = frp3;
		بشکن;
	    }
	}
	اگر (topframe->fr_child == frp2)
	    topframe->fr_child = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    ایستا تهی
winframe_restore(win_T *wp, صحیح dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    اگر (frp->fr_prev != پوچ)
	frame_append(frp->fr_prev, frp);
    وگرنه
	frame_درج(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    اگر (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != پوچ)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    اگر (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != پوچ)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    اگر (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    وگرنه اگر (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    اگر (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	صحیح	row = topleft->w_winrow;
	صحیح	col = topleft->w_wincol;

	frame_comp_pos(frp->fr_parent, &row, &col);
    }
}

/*
 * Return a pointer to the frame that will receive the empty screen space that
 * is left over after "win" is closed.
 *
 * If 'splitbelow' or 'splitright' is set, the space goes above or to the left
 * by default.  Otherwise, the free space goes below or to the right.  The
 * result is that opening a window and then immediately closing it will
 * preserve the initial window layout.  The 'wfh' and 'wfw' settings are
 * respected when possible.
 */
    ایستا frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    اگر (tp == پوچ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	بازگشت alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    اگر (frp->fr_prev == پوچ)
	بازگشت frp->fr_next;
    اگر (frp->fr_next == پوچ)
	بازگشت frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    اگر (frp->fr_parent != پوچ && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    اگر (frp->fr_parent != پوچ && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    اگر (frp->fr_parent != پوچ && frp->fr_parent->fr_layout == FR_ROW)
    {
	اگر (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    وگرنه
    {
	اگر (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    بازگشت target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ایستا tabpage_T *
alt_tabpage(تهی)
{
    tabpage_T	*tp = پوچ;
    صحیح		forward;

    // Use the last accessed tab page, if possible.
    اگر ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	بازگشت lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != پوچ &&
	    ((tcl_flags & TCL_چپ) == 0 || curtab == first_tabpage);

    اگر (forward)
	tp = curtab->tp_next;
    وگرنه
	// Use the previous tab page.
	برای (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    بازگشت tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ایستا win_T *
frame2win(frame_T *frp)
{
    وقتی (frp->fr_win == پوچ)
	frp = frp->fr_child;
    بازگشت frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ایستا صحیح
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    اگر (frp->fr_layout == FR_LEAF)
	بازگشت frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	اگر (frame_has_win(p, wp))
	    بازگشت TRUE;
    بازگشت FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ایستا صحیح min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ایستا تهی
frame_new_height(
    frame_T	*topfrp,
    صحیح		height,
    صحیح		topfirst,	// resize topmost contained frame first
    صحیح		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    صحیح		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    صحیح		extra_سطرها;
    صحیح		h;

    اگر (topfrp->fr_parent == پوچ && set_ch)
    {
	// topframe: update the command line height, with side effects.
	صحیح new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	صحیح save_ch = min_set_ch;
	اگر (new_ch != p_ch)
	    set_option_مقدار((char_u *)"cmdheight", new_ch, پوچ, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    اگر (topfrp->fr_win != پوچ)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    وگرنه اگر (topfrp->fr_layout == FR_ROW)
    {
	انجامبده
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		اگر (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    بشکن;
		}
	    }
	}
	وقتی (frp != پوچ);
    }
    وگرنه    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	اگر (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    وقتی (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		اگر (frp == پوچ)
		    بازگشت;	    // no frame without 'wfh', give up
	    }
	اگر (!topfirst)
	{
	    // Find the bottom frame of this column
	    وقتی (frp->fr_next != پوچ)
		frp = frp->fr_next;
	    اگر (wfh)
		// Advance back for frames with one window with 'wfh' set.
		وقتی (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_سطرها = height - topfrp->fr_height;
	اگر (extra_سطرها < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    وقتی (frp != پوچ)
	    {
		h = frame_minheight(frp, پوچ);
		اگر (frp->fr_height + extra_سطرها < h)
		{
		    extra_سطرها += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		وگرنه
		{
		    frame_new_height(frp, frp->fr_height + extra_سطرها,
							topfirst, wfh, set_ch);
		    بشکن;
		}
		اگر (topfirst)
		{
		    انجامبده
			frp = frp->fr_next;
		    وقتی (wfh && frp != پوچ && frame_fixed_height(frp));
		}
		وگرنه
		{
		    انجامبده
			frp = frp->fr_prev;
		    وقتی (wfh && frp != پوچ && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		اگر (frp == پوچ)
		    height -= extra_سطرها;
	    }
	}
	وگرنه اگر (extra_سطرها > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_سطرها, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ایستا صحیح
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    اگر (frp->fr_win != پوچ)
	بازگشت frp->fr_win->w_p_wfh;

    اگر (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    اگر (frame_fixed_height(frp))
		بازگشت TRUE;
	بازگشت FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	اگر (!frame_fixed_height(frp))
	    بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ایستا صحیح
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    اگر (frp->fr_win != پوچ)
	بازگشت frp->fr_win->w_p_wfw;

    اگر (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    اگر (frame_fixed_width(frp))
		بازگشت TRUE;
	بازگشت FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	اگر (!frame_fixed_width(frp))
	    بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ایستا تهی
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    اگر (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    وگرنه اگر (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    وگرنه // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	برای (frp = frp->fr_child; frp->fr_next != پوچ; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ایستا تهی
frame_new_width(
    frame_T	*topfrp,
    صحیح		width,
    صحیح		leftfirst,	// resize leftmost contained frame first
    صحیح		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    صحیح		extra_cols;
    صحیح		w;
    win_T	*wp;

    اگر (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	برای (frp = topfrp; frp->fr_parent != پوچ; frp = frp->fr_parent)
	    اگر (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != پوچ)
		بشکن;
	اگر (frp->fr_parent == پوچ)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    وگرنه اگر (topfrp->fr_layout == FR_COL)
    {
	انجامبده
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		اگر (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    بشکن;
		}
	    }
	} وقتی (frp != پوچ);
    }
    وگرنه    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	اگر (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    وقتی (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		اگر (frp == پوچ)
		    بازگشت;	    // no frame without 'wfw', give up
	    }
	اگر (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    وقتی (frp->fr_next != پوچ)
		frp = frp->fr_next;
	    اگر (wfw)
		// Advance back for frames with one window with 'wfw' set.
		وقتی (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	اگر (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    وقتی (frp != پوچ)
	    {
		w = frame_minwidth(frp, پوچ);
		اگر (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		وگرنه
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    بشکن;
		}
		اگر (leftfirst)
		{
		    انجامبده
			frp = frp->fr_next;
		    وقتی (wfw && frp != پوچ && frame_fixed_width(frp));
		}
		وگرنه
		{
		    انجامبده
			frp = frp->fr_prev;
		    وقتی (wfw && frp != پوچ && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		اگر (frp == پوچ)
		    width -= extra_cols;
	    }
	}
	وگرنه اگر (extra_cols > 0)
	{
	    // increase width of rightmost frame
	    frame_new_width(frp, frp->fr_width + extra_cols, leftfirst, wfw);
	}
    }
    topfrp->fr_width = width;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ایستا تهی
frame_set_vsep(frame_T *frp, صحیح add)
{
    win_T	*wp;

    اگر (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	اگر (add && wp->w_vsep_width == 0)
	{
	    اگر (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	وگرنه اگر (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    وگرنه اگر (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    وگرنه // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	وقتی (frp->fr_next != پوچ)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ایستا تهی
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    ایستا تهی
frame_fix_height(win_T *wp)
{
    wp->w_frame->fr_height = VISIBLE_HEIGHT(wp) + wp->w_status_height;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ایستا صحیح
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    صحیح		m;
    صحیح		n;

    اگر (topfrp->fr_win != پوچ)
    {
	اگر (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	وگرنه
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    اگر (topfrp->fr_win == curwin && next_curwin == پوچ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		اگر (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    وگرنه اگر (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    اگر (n > m)
		m = n;
	}
    }
    وگرنه
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    بازگشت m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ایستا صحیح
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    صحیح		c = 0;
    صحیح		n;

    اگر (topfrp->fr_win != پوچ)
	c = 1;
    وگرنه اگر (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    اگر (n > c)
		c = n;
	}
    }
    وگرنه
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    بازگشت c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ایستا صحیح
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    صحیح		m, n;

    اگر (topfrp->fr_win != پوچ)
    {
	اگر (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	وگرنه
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    اگر (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == پوچ)
		++m;
	}
    }
    وگرنه اگر (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    اگر (n > m)
		m = n;
	}
    }
    وگرنه
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    بازگشت m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    تهی
close_others(
    صحیح		message,
    صحیح		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    صحیح		r;

    اگر (one_window())
    {
	اگر (message && !autocmd_busy)
	    پیام(_(m_onlyone));
	بازگشت;
    }

    // Be very careful here: autocommands may change the window layout.
    برای (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	اگر (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	اگر (wp == curwin)		// don't close current window
	    ادامه;

	// autoccommands messed this one up
	اگر (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = پوچ;
	    win_close(wp, 0);
	    ادامه;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	اگر (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ادامه;
	}
	اگر (!r)
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		اگر (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ادامه;
		}
	    }
	    اگر (bufIsChanged(wp->w_buffer))
#endif
		ادامه;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    اگر (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    تهی
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ایستا صحیح command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    تهی
use_tabpage(tabpage_T *tp)
{
    curtab = tp;
    topframe = curtab->tp_topframe;
    firstwin = curtab->tp_firstwin;
    lastwin = curtab->tp_lastwin;
    curwin = curtab->tp_curwin;
}

/*
 * Allocate the first window and put an empty buffer in it.
 * Called from main().
 * Return FAIL when something goes wrong (out of memory).
 */
    صحیح
win_alloc_first(تهی)
{
    اگر (win_alloc_firstwin(پوچ) == FAIL)
	بازگشت FAIL;

    first_tabpage = alloc_tabpage();
    اگر (first_tabpage == پوچ)
	بازگشت FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    بازگشت OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(تهی)
{
    win_T *wp;

    wp = win_alloc(پوچ, TRUE);
    اگر (wp == پوچ)
	بازگشت پوچ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    بازگشت wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    تهی
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_buffer = buf;
    ++buf->b_nwindows;
    win_init_empty(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ایستا صحیح
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(پوچ, FALSE);
    اگر (curwin == پوچ)
	بازگشت FAIL;
    اگر (oldwin == پوچ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(پوچ, پوچ, 1L, BLN_LISTED);
	اگر (curwin == پوچ || curbuf == پوچ)
	    بازگشت FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    وگرنه
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    اگر (curwin->w_frame == پوچ)
	بازگشت FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    بازگشت OK;
}

/*
 * Create a frame for window "wp".
 */
    ایستا تهی
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    اگر (frp == پوچ)
	بازگشت;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    تهی
win_init_size(تهی)
{
    firstwin->w_height = ROWS_AVAIL;
    firstwin->w_prev_height = ROWS_AVAIL;
    topframe->fr_height = ROWS_AVAIL;
    firstwin->w_width = topframe->fr_width;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ایستا tabpage_T *
alloc_tabpage(تهی)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    صحیح		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    اگر (tp == پوچ)
	بازگشت پوچ;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    اگر (tp->tp_vars == پوچ)
    {
	vim_free(tp);
	بازگشت پوچ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    برای (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    بازگشت tp;
}

    تهی
free_tabpage(tabpage_T *tp)
{
    صحیح idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    وقتی (tp->tp_first_popupwin != پوچ)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    برای (idx = 0; idx < SNAP_شمار; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    اگر (tp == lastused_tabpage)
	lastused_tabpage = پوچ;
#ifdef FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#endif

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#ifdef FEAT_PYTHON
    python_tabpage_free(tp);
#endif

#ifdef FEAT_PYTHON3
    python3_tabpage_free(tp);
#endif

    vim_free(tp);
}

/*
 * Create a new Tab page with one window.
 * It will edit the current buffer, like after ":split".
 * When "after" is 0 put it just after the current Tab page.
 * Otherwise put it just before tab page "after".
 *
 * Does not trigger WinNewPre, since the window structures
 * are not completely setup yet and could cause dereferencing
 * NULL pointers
 *
 * Return FAIL or OK.
 */
    صحیح
win_new_tabpage(صحیح after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    صحیح		n;
#اگر defined(FEAT_TABPANEL)
    صحیح prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    اگر (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	بازگشت FAIL;
    }
    اگر (window_layout_locked(CMD_tabnew))
	بازگشت FAIL;

    newtp = alloc_tabpage();
    اگر (newtp == پوچ)
	بازگشت FAIL;

    // Remember the current windows in this Tab page.
    اگر (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	بازگشت FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == پوچ)
				    ? پوچ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    اگر (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	اگر (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	وگرنه
	{
	    اگر (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		برای (tp = first_tabpage; tp->tp_next != پوچ
					       && n < after; tp = tp->tp_next)
		    ++n;
	    }
	    newtp->tp_next = tp->tp_next;
	    tp->tp_next = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_size();
	firstwin->w_winrow = tabline_height();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	last_status(FALSE);

	lastused_tabpage = prev_tp;

#اگر defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#اگر defined(FEAT_TABPANEL)
	اگر (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    صحیح		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		اگر (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    اگر (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, پوچ, پوچ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, پوچ, پوچ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, پوچ, پوچ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, پوچ, پوچ, FALSE, curbuf);
	بازگشت OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    بازگشت FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ایستا صحیح
may_open_tabpage(تهی)
{
    صحیح		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    اگر (n == 0)
	بازگشت FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    بازگشت win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    صحیح
make_tabpages(صحیح maxcount)
{
    صحیح		شمار = maxcount;
    صحیح		todo;

    // Limit to 'tabpagemax' tabs.
    اگر (شمار > p_tpm)
	شمار = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    برای (todo = شمار - 1; todo > 0; --todo)
	اگر (win_new_tabpage(0) == FAIL)
	    بشکن;

    unblock_autocmds();

    // return actual number of tab pages
    بازگشت (شمار - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    صحیح
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	اگر (tp == tpc)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    صحیح
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	اگر (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		اگر (win_valid_any_tab(wp))
		    بازگشت TRUE;
	    }
	    بازگشت FALSE;
	}
    }
    // shouldn't happen
    بازگشت FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    تهی
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    اگر (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    وگرنه
    {
	برای (ptp = first_tabpage; ptp != پوچ && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	أكد(ptp != پوچ);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(صحیح n)
{
    tabpage_T	*tp;
    صحیح		i = 1;

    اگر (n == 0)
	بازگشت curtab;

    برای (tp = first_tabpage; tp != پوچ && i != n; tp = tp->tp_next)
	++i;
    بازگشت tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    صحیح
tabpage_index(tabpage_T *ftp)
{
    صحیح		i = 1;
    tabpage_T	*tp;

    برای (tp = first_tabpage; tp != پوچ && tp != ftp; tp = tp->tp_next)
	++i;
    بازگشت i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ایستا صحیح
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    صحیح		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    اگر (trigger_leave_autocmds)
    {
	اگر (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, پوچ, پوچ, FALSE, curbuf);
	    اگر (curtab != tp)
		بازگشت FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, پوچ, پوچ, FALSE, curbuf);
	اگر (curtab != tp)
	    بازگشت FAIL;
	apply_autocmds(EVENT_TABLEAVE, پوچ, پوچ, FALSE, curbuf);
	اگر (curtab != tp)
	    بازگشت FAIL;
    }

    reset_dragwin();
#اگر defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    اگر (gui.in_use)
	gui_remove_scrollbars();
#endif
#ifdef FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#endif
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_Rows = ROWS_AVAIL;
    اگر (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = پوچ;
    lastwin = پوچ;
    بازگشت OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ایستا تهی
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    صحیح		trigger_enter_autocmds,
    صحیح		trigger_leave_autocmds)
{
    صحیح		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    اگر (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	صحیح new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_مقدار((char_u *)"cmdheight", new_ch, پوچ, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (تهی)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = next_prevwin;

    last_status(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#ifdef FEAT_DIFF
    diff_need_scrollbind = TRUE;
#endif

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    اگر (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    اگر (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	اگر (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	وگرنه
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#اگر defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    اگر (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, پوچ, پوچ, FALSE, curbuf);
	اگر (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    تهی
goto_tabpage(صحیح n)
{
    tabpage_T	*tp = پوچ;  // shut up compiler
    tabpage_T	*ttp;
    صحیح		i;

    اگر (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_پیام();
	بازگشت;
    }

    // If there is only one it can't work.
    اگر (first_tabpage->tp_next == پوچ)
    {
	اگر (n > 1)
	    beep_flush();
	بازگشت;
    }

    اگر (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	اگر (curtab->tp_next == پوچ)
	    tp = first_tabpage;
	وگرنه
	    tp = curtab->tp_next;
    }
    وگرنه اگر (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	برای (i = n; i < 0; ++i)
	{
	    برای (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != پوچ;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    وگرنه اگر (n == 9999)
    {
	// Go to last tab page.
	برای (tp = first_tabpage; tp->tp_next != پوچ; tp = tp->tp_next)
	    ;
    }
    وگرنه
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	اگر (tp == پوچ)
	{
	    beep_flush();
	    بازگشت;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    اگر (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    تهی
goto_tabpage_tp(
    tabpage_T	*tp,
    صحیح		trigger_enter_autocmds,
    صحیح		trigger_leave_autocmds)
{
    اگر (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_پیام(پوچ, 0);

    skip_win_fix_scroll = TRUE;
    اگر (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	اگر (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	وگرنه
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    صحیح
goto_tabpage_lastused(تهی)
{
    اگر (!valid_tabpage(lastused_tabpage))
	بازگشت FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    بازگشت OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    تهی
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    اگر (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	اگر (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    تهی
tabpage_move(صحیح nr)
{
    صحیح		n = 1;
    tabpage_T	*tp, *tp_dst;

    اگر (first_tabpage->tp_next == پوچ)
	بازگشت;

    اگر (tabpage_move_disallowed)
	بازگشت;

    برای (tp = first_tabpage; tp->tp_next != پوچ && n < nr; tp = tp->tp_next)
	++n;

    اگر (tp == curtab || (nr > 0 && tp->tp_next != پوچ
						    && tp->tp_next == curtab))
	بازگشت;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    اگر (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    وگرنه
    {
	FOR_ALL_TABPAGES(tp)
	    اگر (tp->tp_next == curtab)
		بشکن;
	اگر (tp == پوچ)	// "cannot happen"
	    بازگشت;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    اگر (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    وگرنه
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    تهی
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    اگر (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	بازگشت;
    }
#endif
    اگر (text_or_buf_locked())
    {
	beep_flush();
	بازگشت;
    }

    اگر (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    وگرنه اگر (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    اگر (!win_valid(wp))
	بازگشت;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    اگر (win_valid(owp) && owp->w_p_cole > 0 && !پیام_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    اگر (curwin->w_p_cole > 0 && !پیام_scrolled)
	need_cursor_سطر_redraw = TRUE;
#endif
}

#اگر defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(صحیح winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (--winnr == 0)
	    بشکن;
    بازگشت wp;
}
#endif

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp == win)
		بازگشت tp;
    بازگشت پوچ;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, صحیح up, long شمار)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
	// popups don't have neighbors.
	بازگشت پوچ;
#endif
    foundfr = wp->w_frame;
    وقتی (شمار--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	برای (;;)
	{
	    اگر (fr == tp->tp_topframe)
		برو end;
	    اگر (up)
		nfr = fr->fr_prev;
	    وگرنه
		nfr = fr->fr_next;
	    اگر (fr->fr_parent->fr_layout == FR_COL && nfr != پوچ)
		بشکن;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	برای (;;)
	{
	    اگر (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		بشکن;
	    }
	    fr = nfr->fr_child;
	    اگر (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		وقتی (fr->fr_next != پوچ
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    اگر (nfr->fr_layout == FR_COL && up)
		وقتی (fr->fr_next != پوچ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    بازگشت foundfr != پوچ ? foundfr->fr_win : پوچ;
}

/*
 * Move to window above or below "count" times.
 */
    ایستا تهی
win_goto_ver(
    صحیح		up,		// TRUE to go to win above
    long	شمار)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    اگر (ERROR_IF_TERM_POPUP_WINDOW)
	بازگشت;
#endif
    win = win_vert_neighbor(curtab, curwin, up, شمار);
    اگر (win != پوچ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, صحیح چپ, long شمار)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
	// popups don't have neighbors.
	بازگشت پوچ;
#endif
    foundfr = wp->w_frame;
    وقتی (شمار--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	برای (;;)
	{
	    اگر (fr == tp->tp_topframe)
		برو end;
	    اگر (چپ)
		nfr = fr->fr_prev;
	    وگرنه
		nfr = fr->fr_next;
	    اگر (fr->fr_parent->fr_layout == FR_ROW && nfr != پوچ)
		بشکن;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	برای (;;)
	{
	    اگر (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		بشکن;
	    }
	    fr = nfr->fr_child;
	    اگر (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		وقتی (fr->fr_next != پوچ
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    اگر (nfr->fr_layout == FR_ROW && چپ)
		وقتی (fr->fr_next != پوچ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    بازگشت foundfr != پوچ ? foundfr->fr_win : پوچ;
}

/*
 * Move to left or right window.
 */
    ایستا تهی
win_goto_hor(
    صحیح		چپ,		// TRUE to go to left win
    long	شمار)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    اگر (ERROR_IF_TERM_POPUP_WINDOW)
	بازگشت;
#endif
    win = win_horz_neighbor(curtab, curwin, چپ, شمار);
    اگر (win != پوچ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    تهی
win_enter(win_T *wp, صحیح undo_sync)
{
    (تهی)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    تهی
win_fix_current_dir(تهی)
{
    اگر (curwin->w_localdir != پوچ || curtab->tp_localdir != پوچ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	اگر (globaldir == پوچ)
	{
	    char_u	cwd[MAXPATHL];

	    اگر (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	اگر (curwin->w_localdir != پوچ)
	    dirname = curwin->w_localdir;
	وگرنه
	    dirname = curtab->tp_localdir;

	اگر (mch_chdir((نویسه *)dirname) == 0)
	{
	    last_chdir_reason = پوچ;
	    shorten_fnames(TRUE);
	}
    }
    وگرنه اگر (globaldir != پوچ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((نویسه *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = پوچ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ایستا صحیح
win_enter_ext(win_T *wp, صحیح flags)
{
    صحیح		other_buffer = FALSE;
    صحیح		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    صحیح		did_decrement = FALSE;

    اگر (wp == curwin && curwin_invalid == 0)	// nothing to do
	بازگشت FALSE;

#ifdef FEAT_JOB_CHANNEL
    اگر (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    اگر (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	اگر (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, پوچ, پوچ, FALSE, curbuf);
	    other_buffer = TRUE;
	    اگر (!win_valid(wp))
		بازگشت FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, پوچ, پوچ, FALSE, curbuf);
	اگر (!win_valid(wp))
	    بازگشت FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	اگر (aborting())
	    بازگشت FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    اگر ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    اگر (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    اگر (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    اگر (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    اگر (!virtual_active())
	curwin->w_cursor.coladd = 0;
    اگر (*p_spk == 'c')		// assume cursor position needs updating
	changed_سطر_abv_curs();
    وگرنه
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    اگر (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#endif

    win_fix_current_dir();

#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Careful: autocommands may close the window and make "wp" invalid
    اگر (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, پوچ, پوچ, FALSE, curbuf);
    اگر (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, پوچ, پوچ, FALSE, curbuf);
	اگر (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    اگر (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    اگر (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    اگر (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((صحیح)p_wh);
    وگرنه اگر (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    اگر (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((صحیح)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    بازگشت did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = پوچ;

    اگر (curwin->w_buffer == buf)
	wp = curwin;
    وگرنه
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_buffer == buf)
		بشکن;
    اگر (wp != پوچ)
	win_enter(wp, FALSE);
    بازگشت wp;
}

/*
 * Jump to the first open window in any tab page that contains buffer "buf",
 * if one exists. First search in the windows present in the current tab page.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_tab(buf_T *buf)
{
    win_T	*wp = buf_jump_open_win(buf);
    tabpage_T	*tp;

    اگر (wp != پوچ)
	بازگشت wp;

    FOR_ALL_TABPAGES(tp)
	اگر (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		اگر (wp->w_buffer == buf)
		    بشکن;
	    اگر (wp != پوچ)
	    {
		goto_tabpage_win(tp, wp);
		اگر (curwin != wp)
		    wp = پوچ;	// something went wrong
		بشکن;
	    }
	}
    بازگشت wp;
}

ایستا صحیح last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ایستا win_T *
win_alloc(win_T *after, صحیح hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    اگر (new_wp == پوچ)
	بازگشت پوچ;

    اگر (win_alloc_سطرها(new_wp) == FAIL)
    {
	vim_free(new_wp);
	بازگشت پوچ;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    اگر (new_wp->w_vars == پوچ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	بازگشت پوچ;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#endif

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    block_autocmds();

    /*
     * link the window in the window list
     */
    اگر (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_width = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#ifdef FEAT_DIFF
    new_wp->w_topfill = 0;
#endif
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_row = -1;

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_چپ],
		SBAR_چپ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_راست],
		SBAR_راست, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_جستجو_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    بازگشت new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ایستا تهی
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    صحیح		i;
    buf_T	*buf;
    wininfo_T	*wip;

#ifdef FEAT_FOLDING
    clearFolding(wp);
#endif

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    block_autocmds();

#ifdef FEAT_LUA
    lua_window_free(wp);
#endif

#ifdef FEAT_MZSCHEME
    mzscheme_window_free(wp);
#endif

#ifdef FEAT_PERL
    perl_win_free(wp);
#endif

#ifdef FEAT_PYTHON
    python_window_free(wp);
#endif

#ifdef FEAT_PYTHON3
    python3_window_free(wp);
#endif

#ifdef FEAT_TCL
    tcl_window_free(wp);
#endif

#ifdef FEAT_RUBY
    ruby_window_free(wp);
#endif

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    اگر (wp->w_stl_click != پوچ)
    {
	برای (i = 0; i < wp->w_stl_click_شمار; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#ifdef FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#endif

    {
	tabpage_T	*ttp;

	اگر (prevwin == wp)
	    prevwin = پوچ;
	FOR_ALL_TABPAGES(ttp)
	    اگر (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = پوچ;
    }
    win_free_lsize(wp);

    برای (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    اگر (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    اگر (wip2 != wip && wip2->wi_win == پوچ)
		    {
			اگر (wip2->wi_next != پوچ)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			اگر (wip2->wi_prev == پوچ)
			    buf->b_wininfo = wip2->wi_next;
			وگرنه
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			بشکن;
		    }

		wip->wi_win = پوچ;
	    }

#ifdef FEAT_جستجو_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_چپ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_راست]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    برای (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_mask);
    vim_free(wp->w_popup_mask_cells);
#endif

#ifdef FEAT_SYN_HL
    vim_free(wp->w_p_cc_cols);
#endif

    اگر (win_valid_any_tab(wp))
	win_remove(wp, tp);
    اگر (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    وگرنه
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    صحیح
win_unlisted(win_T *wp)
{
    بازگشت is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    تهی
win_free_popup(win_T *win)
{
    اگر (win->w_buffer != پوچ)
    {
	اگر (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	وگرنه
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	اگر (win->w_buffer != پوچ)
	    --win->w_buffer->b_nwindows;
    }
# اگر defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    اگر (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, پوچ);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    ایستا تهی
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    اگر (after == پوچ)	    // after NULL is in front of the first
	before = firstwin;
    وگرنه
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    اگر (after == پوچ)
	firstwin = wp;
    وگرنه
	after->w_next = wp;
    اگر (before == پوچ)
	lastwin = wp;
    وگرنه
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    تهی
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    اگر (wp->w_prev != پوچ)
	wp->w_prev->w_next = wp->w_next;
    وگرنه اگر (tp == پوچ)
	firstwin = curtab->tp_firstwin = wp->w_next;
    وگرنه
	tp->tp_firstwin = wp->w_next;

    اگر (wp->w_next != پوچ)
	wp->w_next->w_prev = wp->w_prev;
    وگرنه اگر (tp == پوچ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    وگرنه
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ایستا تهی
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    اگر (frp->fr_next != پوچ)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ایستا تهی
frame_درج(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    اگر (frp->fr_prev != پوچ)
	frp->fr_prev->fr_next = frp;
    وگرنه
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ایستا تهی
frame_remove(frame_T *frp)
{
    اگر (frp->fr_prev != پوچ)
	frp->fr_prev->fr_next = frp->fr_next;
    وگرنه
	frp->fr_parent->fr_child = frp->fr_next;
    اگر (frp->fr_next != پوچ)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    صحیح
win_alloc_سطرها(win_T *wp)
{
    wp->w_سطرها_valid = 0;
    wp->w_سطرها = ALLOC_CLEAR_MULT(wline_T, Rows);
    اگر (wp->w_سطرها == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * free lsize arrays for a window
 */
    تهی
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    اگر (wp != پوچ)
	VIM_CLEAR(wp->w_سطرها);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    تهی
shell_new_rows(تهی)
{
    صحیح		h = (صحیح)ROWS_AVAIL;

    اگر (firstwin == پوچ)	// not initialized yet
	بازگشت;
    اگر (h < frame_minheight(topframe, پوچ))
	h = frame_minheight(topframe, پوچ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    اگر (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    اگر (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#اگر 0
    // Disabled: don't want making the screen smaller make a window larger.
    اگر (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    تهی
shell_new_columns(تهی)
{
    اگر (firstwin == پوچ)	// not initialized yet
	بازگشت;

#اگر defined(FEAT_TABPANEL)
    ایستا صحیح prev_tp_width;
    ایستا صحیح prev_wincol;
    ایستا صحیح prev_fr_width;
#endif
    صحیح w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    اگر (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#اگر defined(FEAT_TABPANEL)
    صحیح tp_width = tabpanel_width();
    صحیح tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    اگر (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    اگر (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (صحیح)Rows, 0, (صحیح)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    اگر (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#اگر 0
    // Disabled: don't want making the screen smaller make a window larger.
    اگر (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    تهی
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, اندازه(صحیح), 1);
    اگر (ga_grow(gap, win_شمار() * 2 + 1) == FAIL)
	بازگشت;

    // first entry is the total lines available for windows
    ((صحیح *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((صحیح *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((صحیح *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    تهی
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    صحیح		i, j;

    اگر (win_شمار() * 2 + 1 == gap->ga_len
	    && ((صحیح *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	برای (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((صحیح *)gap->ga_data)[i++]);
		win_setheight_win(((صحیح *)gap->ga_data)[i++], wp);
	    }
	}
	// recompute the window positions
	win_comp_pos();
    }
}

/*
 * Update the position for all windows, using the width and height of the
 * frames.
 * Returns the row just after the last window.
 */
    تهی
win_comp_pos(تهی)
{
    صحیح		row = tabline_height();
    صحیح		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ایستا تهی
frame_comp_pos(frame_T *topfrp, صحیح *row, صحیح *col)
{
    win_T	*wp;
    frame_T	*frp;
    صحیح		startcol;
    صحیح		startrow;
    صحیح		h;

    wp = topfrp->fr_win;
    اگر (wp != پوچ)
    {
	اگر (wp->w_winrow != *row || wp->w_wincol != *col)
	{
	    // position changed, redraw
	    wp->w_winrow = *row;
	    wp->w_wincol = *col;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_status = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_HEIGHT(wp) + wp->w_status_height;
	*row += h > topfrp->fr_height ? topfrp->fr_height : h;
	*col += wp->w_width + wp->w_vsep_width;
    }
    وگرنه
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    اگر (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    وگرنه
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    تهی
win_ensure_size(تهی)
{
    اگر (curwin->w_height == 0)
	win_setheight(1);
    اگر (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    تهی
win_setheight(صحیح height)
{
    win_setheight_win(height, curwin);
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    تهی
win_setheight_win(صحیح height, win_T *win)
{
    اگر (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	اگر (height < p_wmh)
	    height = p_wmh;
	اگر (height == 0)
	    height = 1;
	height += WINBAR_HEIGHT(curwin);
    }

    frame_setheight(win->w_frame, height + win->w_status_height);

    // recompute the window positions
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Set the height of a frame to "height" and take care that all frames and
 * windows inside it are resized.  Also resize frames on the left and right if
 * the are in the same FR_ROW frame.
 *
 * Strategy:
 * If the frame is part of a FR_COL frame, try fitting the frame in that
 * frame.  If that doesn't work (the FR_COL frame is too small), recursively
 * go to containing frames to resize them and make room.
 * If the frame is part of a FR_ROW frame, all frames must be resized as well.
 * Check for the minimal height of the FR_ROW frame.
 * At the top level we can also use change the command line height.
 */
    ایستا تهی
frame_setheight(frame_T *curfrp, صحیح height)
{
    صحیح		room;		// total number of lines available
    صحیح		take;		// number of lines taken from other windows
    صحیح		room_cmdline;	// lines available from cmdline
    صحیح		run;
    frame_T	*frp;
    صحیح		h;
    صحیح		room_reserved;

    // If the height already is the desired value, nothing to do.
    اگر (curfrp->fr_height == height)
	بازگشت;

    اگر (curfrp->fr_parent == پوچ)
    {
	// topframe: can only change the command line height
	اگر (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    وگرنه اگر (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, پوچ);
	اگر (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    وگرنه
    {
	/*
	 * Column of frames: try to change only frames in this column.
	 */
	/*
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the height to it.
	 * Try not to reduce the height of a window with 'winfixheight' set.
	 */
	برای (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		اگر (frp != curfrp
			&& frp->fr_win != پوچ
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		اگر (frp != curfrp)
		    room -= frame_minheight(frp, پوچ);
	    }
	    اگر (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    وگرنه
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		اگر (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    اگر (height <= room + room_cmdline)
		بشکن;
	    اگر (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		بشکن;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (صحیح)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	اگر (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	اگر (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	اگر (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    اگر (take < room_cmdline)
		room_cmdline = take;
	    take -= room_cmdline;
	    topframe->fr_height += room_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	frame_new_height(curfrp, height, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	برای (run = 0; run < 2; ++run)
	{
	    اگر (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    وگرنه
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    وقتی (frp != پوچ && take != 0)
	    {
		h = frame_minheight(frp, پوچ);
		اگر (room_reserved > 0
			&& frp->fr_win != پوچ
			&& frp->fr_win->w_p_wfh)
		{
		    اگر (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    وگرنه
		    {
			اگر (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		وگرنه
		{
		    اگر (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    وگرنه
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		اگر (run == 0)
		    frp = frp->fr_next;
		وگرنه
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    تهی
win_setwidth(صحیح width)
{
    win_setwidth_win(width, curwin);
}

    تهی
win_setwidth_win(صحیح width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    اگر (wp == curwin)
    {
	اگر (width < p_wmw)
	    width = p_wmw;
	اگر (width == 0)
	    width = 1;
    }
    وگرنه اگر (width < 0)
	width = 0;

    frame_setwidth(wp->w_frame, width + wp->w_vsep_width);

    // recompute the window positions
    win_comp_pos();

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Set the width of a frame to "width" and take care that all frames and
 * windows inside it are resized.  Also resize frames above and below if the
 * are in the same FR_ROW frame.
 *
 * Strategy is similar to frame_setheight().
 */
    ایستا تهی
frame_setwidth(frame_T *curfrp, صحیح width)
{
    صحیح		room;		// total number of lines available
    صحیح		take;		// number of lines taken from other windows
    صحیح		run;
    frame_T	*frp;
    صحیح		w;
    صحیح		room_reserved;

    // If the width already is the desired value, nothing to do.
    اگر (curfrp->fr_width == width)
	بازگشت;

    اگر (curfrp->fr_parent == پوچ)
	// topframe: can't change width
	بازگشت;

    اگر (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, پوچ);
	اگر (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    وگرنه
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	برای (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		اگر (frp != curfrp
			&& frp->fr_win != پوچ
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		اگر (frp != curfrp)
		    room -= frame_minwidth(frp, پوچ);
	    }

	    اگر (width <= room)
		بشکن;
	    اگر (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		بشکن;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (صحیح)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	اگر (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	اگر (take < 0 && room - curfrp->fr_width < room_reserved)
	    room_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	frame_new_width(curfrp, width, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	برای (run = 0; run < 2; ++run)
	{
	    اگر (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    وگرنه
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    وقتی (frp != پوچ && take != 0)
	    {
		w = frame_minwidth(frp, پوچ);
		اگر (room_reserved > 0
			&& frp->fr_win != پوچ
			&& frp->fr_win->w_p_wfw)
		{
		    اگر (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    وگرنه
		    {
			اگر (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		وگرنه
		{
		    اگر (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    وگرنه
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		اگر (run == 0)
		    frp = frp->fr_next;
		وگرنه
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    تهی
win_setminheight(تهی)
{
    صحیح		room;
    صحیح		needed;
    صحیح		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    وقتی (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	اگر (room >= needed)
	    بشکن;
	--p_wmh;
	اگر (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    تهی
win_setminwidth(تهی)
{
    صحیح		room;
    صحیح		needed;
    صحیح		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    وقتی (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, پوچ);
	اگر (room >= needed)
	    بشکن;
	--p_wmw;
	اگر (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    تهی
win_drag_status_سطر(win_T *dragwin, صحیح offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    صحیح		room;
    صحیح		up;	// if TRUE, drag status line up, otherwise down
    صحیح		n;

    fr = dragwin->w_frame;
    curfr = fr;
    اگر (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	اگر (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    اگر (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    وقتی (curfr != topframe && curfr->fr_next == پوچ)
    {
	اگر (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	اگر (fr != topframe)
	    fr = fr->fr_parent;
    }

    اگر (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	اگر (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, پوچ);
	}
	وگرنه
	{
	    room = 0;
	    برای (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, پوچ);
		اگر (fr == curfr)
		    بشکن;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    وگرنه    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	اگر (curfr->fr_next == پوچ)
	    --room;
	وگرنه
	    room -= p_ch;
	اگر (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, پوچ);
	fr = curfr;			// put fr at window that grows
    }

    اگر (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    اگر (offset <= 0)
	بازگشت;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    اگر (fr != پوچ)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    اگر (up)
	fr = curfr;		// current frame gets smaller
    وگرنه
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    وقتی (fr != پوچ && offset > 0)
    {
	n = frame_minheight(fr, پوچ);
	اگر (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	وگرنه
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    بشکن;
	}
	اگر (up)
	    fr = fr->fr_prev;
	وگرنه
	    fr = fr->fr_next;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    تهی
win_drag_vsep_سطر(win_T *dragwin, صحیح offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    صحیح		room;
    صحیح		چپ;	// if TRUE, drag separator line left, otherwise right
    صحیح		n;

    fr = dragwin->w_frame;
    اگر (fr == topframe)		// only one window (cannot happen?)
	بازگشت;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    اگر (fr->fr_layout != FR_ROW)
    {
	اگر (fr == topframe)	// only a column of windows (cannot happen?)
	    بازگشت;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    وقتی (curfr->fr_next == پوچ)
    {
	اگر (fr == topframe)
	    بشکن;
	curfr = fr;
	fr = fr->fr_parent;
	اگر (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    اگر (offset < 0) // drag left
    {
	چپ = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	برای (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, پوچ);
	    اگر (fr == curfr)
		بشکن;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    وگرنه    // drag right
    {
	چپ = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, پوچ);
	fr = curfr;			// put fr at window that grows
    }

    اگر (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    اگر (offset <= 0)		// No room at all, quit.
	بازگشت;
    اگر (fr == پوچ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	بازگشت;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, چپ, FALSE);

    // shrink other frames: current and at the left or at the right
    اگر (چپ)
	fr = curfr;		// current frame gets smaller
    وگرنه
	fr = curfr->fr_next;	// next frame gets smaller

    وقتی (fr != پوچ && offset > 0)
    {
	n = frame_minwidth(fr, پوچ);
	اگر (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !چپ, FALSE);
	}
	وگرنه
	{
	    frame_new_width(fr, fr->fr_width - offset, !چپ, FALSE);
	    بشکن;
	}
	اگر (چپ)
	    fr = fr->fr_prev;
	وگرنه
	    fr = fr->fr_next;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#define FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    تهی
set_fraction(win_T *wp)
{
    اگر (wp->w_height > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((long)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (long)wp->w_height;
}

/*
 * Handle scroll position, depending on 'splitkeep'.  Replaces the
 * scroll_to_fraction() call from win_new_height() if 'splitkeep' is "screen"
 * or "topline".  Instead we iterate over all windows in a tabpage and
 * calculate the new scroll position.
 * TODO: Ensure this also works with wrapped lines.
 * Requires a not fully visible cursor line to be allowed at the bottom of
 * a window ("zb"), probably only when 'smoothscroll' is also set.
 */
    ایستا تهی
win_fix_scroll(صحیح resize)
{
    اگر (*p_spk == 'c')
	بازگشت;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	اگر (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    اگر (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_سطر_شمار)
	    {
		صحیح diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		اگر (diff > 0)
		    cursor_down_inner(wp, diff);
		وگرنه
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		اگر (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    وگرنه اگر (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    اگر (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    وگرنه اگر (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ایستا تهی
win_fix_cursor(صحیح normal)
{
    win_T	*wp = curwin;

    اگر (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_سطر_شمار < wp->w_height)
	بازگشت;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_مقدار());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T بالاترین = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    اگر (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_سطر_شمار) != 1)
	nlnum = bot;
    وگرنه اگر (lnum < بالاترین && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : بالاترین;

    اگر (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	اگر (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	وگرنه  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_to_fraction(wp, wp->w_prev_height);
	    validate_botline();
	}
    }
}

/*
 * Set the height of a window.
 * "height" excludes any window toolbar.
 * This takes care of the things inside the window, not what happens to the
 * window position, the frame or to other windows.
 */
    تهی
win_new_height(win_T *wp, صحیح height)
{
    صحیح		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    اگر (height < 0)
	height = 0;
    اگر (wp->w_height == height)
	بازگشت;	    // nothing to do

    اگر (wp->w_height > 0)
    {
	اگر (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	اگر (wp->w_height != prev_height)
	    بازگشت;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	اگر (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    اگر (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	اگر (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    تهی
scroll_to_fraction(win_T *wp, صحیح prev_height)
{
    linenr_T	lnum;
    صحیح		sline, سطر_size;
    صحیح		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    اگر (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_سطر_شمار || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	اگر (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	سطر_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - سطر_size;

	اگر (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    صحیح rows = plines_win(wp, lnum, FALSE);

	    اگر (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - سطر_size;
	    }
	}

	اگر (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    صحیح		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = سطر_size;
	    اگر (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		وقتی (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	وگرنه اگر (sline > 0)
	{
	    وقتی (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, پوچ, TRUE, پوچ);
		اگر (lnum == 1)
		{
		    // first line in buffer is folded
		    سطر_size = 1;
		    --sline;
		    بشکن;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		اگر (lnum == wp->w_topline)
		    سطر_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		وگرنه
#endif
		    سطر_size = plines_win(wp, lnum, TRUE);
		sline -= سطر_size;
	    }

	    اگر (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, پوچ, &lnum, TRUE, پوچ);
#endif
		lnum++;
		wp->w_wrow -= سطر_size + sline;
	    }
	    وگرنه اگر (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    اگر (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    اگر (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    تهی
win_new_width(win_T *wp, صحیح width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_سطرها_valid = 0;
    changed_سطر_abv_curs_win(wp);
    invalidate_botline_win(wp);

    اگر (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    تهی
win_comp_scroll(win_T *wp)
{
#اگر defined(FEAT_EVAL)
    صحیح old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    اگر (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#اگر defined(FEAT_EVAL)
    اگر (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#endif
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    تهی
command_height(تهی)
{
    صحیح		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    وقتی (frp->fr_width != topframe->fr_width && frp->fr_parent != پوچ)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    وقتی (frp->fr_prev != پوچ && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    وقتی (p_ch > old_p_ch && command_frame_height)
    {
	اگر (frp == پوچ)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    بشکن;
	}
	صحیح h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, پوچ));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    اگر (p_ch < old_p_ch && command_frame_height && frp != پوچ)
	frame_add_height(frp, (صحیح)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    اگر (پیام_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (صحیح)Rows, 0, (صحیح)Columns, ' ', ' ', 0);
	پیام_row = cmdline_row;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_used = p_ch;
    min_set_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ایستا تهی
frame_add_height(frame_T *frp, صحیح n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    برای (;;)
    {
	frp = frp->fr_parent;
	اگر (frp == پوچ)
	    بشکن;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    تهی
last_status(
    صحیح		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#اگر defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    ایستا تهی
last_status_rec(frame_T *fr, صحیح statusline)
{
    frame_T	*fp;
    win_T	*wp;

    اگر (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	اگر (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	وگرنه اگر (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    وقتی (fp->fr_height <= frame_minheight(fp, پوچ))
	    {
		اگر (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    بازگشت;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		اگر (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != پوچ)
		    fp = fp->fr_prev;
		وگرنه
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    اگر (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    وگرنه
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	اگر (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    وگرنه اگر (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	اگر (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	وگرنه
	{
	    frame_T	*fp2;
	    صحیح		max_stlh = 0;
	    صحیح		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		اگر (fp->fr_win != پوچ && fp->fr_win->w_status_height == 0)
		{
		    صحیح h = statusline_height(fp->fr_win);
		    اگر (h > max_stlh)
			max_stlh = h;
		}
	    اگر (max_stlh == 0)
		بازگشت;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    وقتی (fp2->fr_height - frame_minheight(fp2, پوچ) < max_stlh)
	    {
		اگر (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    بازگشت;
		}
		اگر (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != پوچ)
		    fp2 = fp2->fr_prev;
		وگرنه
		    fp2 = fp2->fr_parent;
	    }

	    اگر (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    وگرنه
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		اگر (fp->fr_win != پوچ)
		{
		    wp = fp->fr_win;
		    اگر (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			اگر (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		وگرنه
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    وگرنه
    {
	// horizontally split window, set status line for last one
	برای (fp = fr->fr_child; fp->fr_next != پوچ; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#اگر defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    تهی
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == پوچ) ? p_stl : wp->w_p_stl;
    صحیح		opt_flags = (wp == پوچ) ? 0 : OPT_LOCAL;
    صحیح		*height = (wp == پوچ) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == پوچ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ایستا صحیح
frame_find_global_stlh_rec(frame_T *frp, صحیح h)
{
    اگر (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	اگر (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    صحیح win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    اگر (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    وگرنه
    {
	برای (frp = frp->fr_child; frp != پوچ; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    بازگشت h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ایستا تهی
frame_change_statusline_height_rec(frame_T *frp, صحیح global_stlh)
{
    اگر (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	اگر (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    صحیح win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    اگر (wp->w_p_stlo == پوچ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		اگر (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    اگر (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    صحیح	     saved_topfill = wp->w_topfill;
# endif
	    win_new_height(wp, win_free_height - wp->w_status_height);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    وگرنه
    {
	برای (frp = frp->fr_child; frp != پوچ; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    صحیح
frame_change_statusline_height(تهی)
{
    tabpage_T	*tp;
    صحیح		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    بازگشت global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    تهی
set_stlo_mh(صحیح mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    صحیح
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    صحیح		l_stlo_fh = FALSE;
    صحیح		l_stlo_mh = 1;

    اگر (stlopt != پوچ)
	p = stlopt;
    وگرنه
	p = wp->w_p_stlo;

    وقتی (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	اگر (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    اگر (l_stlo_mh < 1)
		بازگشت FAIL;
	}
	وگرنه اگر (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    اگر (wp == پوچ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    وگرنه
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    بازگشت OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    صحیح
statusline_height(win_T *wp UNUSED)
{
    صحیح stl_height = 1;
#اگر defined(FEAT_STL_OPT)
    bool fixed;
    صحیح l_stl_rendered_height;

    اگر (wp != پوچ && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    وگرنه
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != پوچ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    اگر (!fixed)
    {
	صحیح has_custom_stl = (*p_stl != NUL)
	    || (wp != پوچ && *wp->w_p_stl != NUL);
	اگر (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	وگرنه
	    stl_height = 1;
    }
#endif

    بازگشت stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    صحیح
tabline_height(تهی)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    اگر (gui_use_tabline())
	بازگشت 0;
#endif
    انتخاب (p_stal)
    {
	حالت 0: بازگشت 0;
	حالت 1: بازگشت (first_tabpage->tp_next == پوچ) ? 0 : 1;
    }
    بازگشت 1;
}

/*
 * Return the height of the last window's statusline.
 */
    صحیح
last_stl_height(
    صحیح		morewin)	// pretend there are two or more windows
{
    بازگشت (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    صحیح
min_rows(تهی)
{
    اگر (firstwin == پوچ)	// not initialized yet
	بازگشت MIN_سطرها;

    بازگشت frame_minheight(curtab->tp_topframe, پوچ) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    صحیح
min_rows_for_all_tabpages(تهی)
{
    صحیح		total;
    tabpage_T	*tp;
    صحیح		n;

    اگر (firstwin == پوچ)	// not initialized yet
	بازگشت MIN_سطرها;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, پوچ);
	اگر (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    بازگشت total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    صحیح
only_one_window(تهی)
{
    صحیح		شمار = 0;
    win_T	*wp;

#اگر defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    اگر (popup_is_popup(curwin))
	بازگشت FALSE;
#endif

    // If there is another tab page there always is another window.
    اگر (first_tabpage->tp_next != پوچ)
	بازگشت FALSE;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_buffer != پوچ
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++شمار;
    بازگشت (شمار <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ایستا تهی
check_lnums_both(صحیح do_curwin, صحیح nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    صحیح need_adjust;

	    اگر (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار;
	    اگر (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	    اگر (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_سطر_شمار;
	    اگر (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_سطر_شمار;
	    اگر (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    تهی
check_lnums(صحیح do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    تهی
check_lnums_nested(صحیح do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    تهی
reset_lnums(تهی)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    اگر (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    اگر (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    اگر (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_سطر_شمار)
		wp->w_valid &= ~VALID_TOPLINE;
	}
}

/*
 * A snapshot of the window sizes, to restore them after closing the help
 * or other window.
 * Only these fields are used:
 * fr_layout
 * fr_width
 * fr_height
 * fr_next
 * fr_child
 * fr_win (only valid for the old curwin, NULL otherwise)
 */

/*
 * Create a snapshot of the current frame sizes.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 * Return FAIL if out of memory, OK otherwise.
 */
    صحیح
make_snapshot(صحیح idx)
{
    clear_snapshot(curtab, idx);
    اگر (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	بازگشت FAIL;
    }
    بازگشت OK;
}

    ایستا صحیح
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    اگر (*frp == پوچ)
	بازگشت FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    اگر (fr->fr_next != پوچ)
    {
	اگر (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    بازگشت FAIL;
    }
    اگر (fr->fr_child != پوچ)
    {
	اگر (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    بازگشت FAIL;
    }
    اگر (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    بازگشت OK;
}

/*
 * Remove any existing snapshot.
 */
    ایستا تهی
clear_snapshot(tabpage_T *tp, صحیح idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = پوچ;
}

    ایستا تهی
clear_snapshot_rec(frame_T *fr)
{
    اگر (fr == پوچ)
	بازگشت;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ایستا win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    اگر (ft->fr_next != پوچ)
    {
	اگر ((wp = get_snapshot_curwin_rec(ft->fr_next)) != پوچ)
	    بازگشت wp;
    }
    اگر (ft->fr_child != پوچ)
    {
	اگر ((wp = get_snapshot_curwin_rec(ft->fr_child)) != پوچ)
	    بازگشت wp;
    }

    بازگشت ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ایستا win_T *
get_snapshot_curwin(صحیح idx)
{
    اگر (curtab->tp_snapshot[idx] == پوچ)
	بازگشت پوچ;

    بازگشت get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    تهی
restore_snapshot(
    صحیح		idx,
    صحیح		close_curwin)	    // closing current window
{
    win_T	*wp;

    اگر (curtab->tp_snapshot[idx] != پوچ
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	اگر (wp != پوچ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ایستا صحیح
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    اگر (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == پوچ) != (fr->fr_next == پوچ)
	    || (sn->fr_child == پوچ) != (fr->fr_child == پوچ)
	    || (sn->fr_next != پوچ
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != پوچ
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != پوچ && !win_valid(sn->fr_win)))
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ایستا win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = پوچ;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    اگر (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    اگر (sn->fr_next != پوچ)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	اگر (wp2 != پوچ)
	    wp = wp2;
    }
    اگر (sn->fr_child != پوچ)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	اگر (wp2 != پوچ)
	    wp = wp2;
    }
    بازگشت wp;
}

#اگر defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    صحیح
win_hasvertsplit(تهی)
{
    frame_T	*fr;

    اگر (topframe->fr_layout == FR_ROW)
	بازگشت TRUE;

    اگر (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    اگر (fr->fr_layout == FR_ROW)
		بازگشت TRUE;

    بازگشت FALSE;
}
#endif

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    صحیح
get_win_عدد(win_T *wp, win_T *first_win)
{
    صحیح		i = 1;
    win_T	*w;

    برای (w = first_win; w != پوچ && w != wp; w = W_NEXT(w))
	++i;

    اگر (w == پوچ)
	بازگشت 0;
    وگرنه
	بازگشت i;
}

    صحیح
get_tab_عدد(tabpage_T *tp)
{
    صحیح		i = 1;
    tabpage_T	*t;

    برای (t = first_tabpage; t != پوچ && t != tp; t = t->tp_next)
	++i;

    اگر (t == پوچ)
	بازگشت 0;
    وگرنه
	بازگشت i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ایستا صحیح
frame_check_height(frame_T *topfrp, صحیح height)
{
    frame_T *frp;

    اگر (topfrp->fr_height != height)
	بازگشت FALSE;

    اگر (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    اگر (frp->fr_height != height)
		بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ایستا صحیح
frame_check_width(frame_T *topfrp, صحیح width)
{
    frame_T *frp;

    اگر (topfrp->fr_width != width)
	بازگشت FALSE;

    اگر (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    اگر (frp->fr_width != width)
		بازگشت FALSE;

    بازگشت TRUE;
}

#اگر defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ایستا صحیح
int_cmp(ثابت تهی *pa, ثابت تهی *pb)
{
    ثابت صحیح a = *(ثابت صحیح *)pa;
    ثابت صحیح b = *(ثابت صحیح *)pb;
    اگر (a > b)
	بازگشت 1;
    اگر (a < b)
	بازگشت -1;
    بازگشت 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    نویسه *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    صحیح		tw;
    صحیح		col;
    صحیح		شمار = 0;
    صحیح		color_cols[256];
    صحیح		i;
    صحیح		j = 0;

    اگر (wp != پوچ && wp->w_buffer == پوچ)
	بازگشت پوچ;  // buffer was closed

    اگر (cc != پوچ)
	s = cc;
    وگرنه اگر (wp != پوچ)
	s = wp->w_p_cc;

    اگر (wp != پوچ)
	tw = wp->w_buffer->b_p_tw;
    وگرنه
	// buffer-local value not set, assume zero
	tw = 0;

    وقتی (*s != NUL && شمار < 255)
    {
	اگر (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    اگر (!VIM_ISDIGIT(*s))
		بازگشت e_invalid_argument;
	    col = col * getdigits(&s);
	    اگر (tw == 0)
		برو skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    اگر (col < 0)
		برو skip;
	}
	وگرنه اگر (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	وگرنه
	    بازگشت e_invalid_argument;
	color_cols[شمار++] = col - 1;  // 1-based to 0-based
skip:
	اگر (*s == NUL)
	    بشکن;
	اگر (*s != ',')
	    بازگشت e_invalid_argument;
	اگر (*++s == NUL)
	    بازگشت e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    اگر (wp == پوچ)
	بازگشت پوچ;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    اگر (شمار == 0)
	wp->w_p_cc_cols = پوچ;
    وگرنه
    {
	wp->w_p_cc_cols = ALLOC_MULT(صحیح, شمار + 1);
	اگر (wp->w_p_cc_cols != پوچ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, شمار, اندازه(صحیح), int_cmp);

	    برای (i = 0; i < شمار; ++i)
		// skip duplicates
		اگر (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    بازگشت پوچ;  // no error
}
#endif

    صحیح
get_last_winid(تهی)
{
    بازگشت last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   صحیح
win_locked(win_T *wp)
{
    بازگشت wp->w_locked;
}
