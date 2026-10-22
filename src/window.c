/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

நிலையான வெறுமை cmd_with_மொத்தம்(எழுத்து *cmd, char_u *bufp, size_t bufsize, long Prenum);
நிலையான வெறுமை win_init(win_T *newp, win_T *oldp, முழு flags);
நிலையான வெறுமை win_init_some(win_T *newp, win_T *oldp);
நிலையான வெறுமை frame_comp_pos(frame_T *topfrp, முழு *row, முழு *col);
நிலையான வெறுமை frame_setheight(frame_T *curfrp, முழு height);
நிலையான வெறுமை frame_setwidth(frame_T *curfrp, முழு width);
நிலையான வெறுமை win_exchange(long);
நிலையான வெறுமை win_rotate(முழு, முழு);
நிலையான வெறுமை win_equal_rec(win_T *next_curwin, முழு current, frame_T *topfr, முழு dir, முழு col, முழு row, முழு width, முழு height);
நிலையான வெறுமை trigger_winnewpre(வெறுமை);
நிலையான வெறுமை trigger_winclosed(win_T *win);
நிலையான win_T *win_free_mem(win_T *win, முழு *dirp, tabpage_T *tp);
நிலையான frame_T *win_altframe(win_T *win, tabpage_T *tp);
நிலையான tabpage_T *alt_tabpage(வெறுமை);
நிலையான win_T *frame2win(frame_T *frp);
நிலையான முழு frame_has_win(frame_T *frp, win_T *wp);
நிலையான வெறுமை win_fix_scroll(முழு resize);
நிலையான வெறுமை win_fix_cursor(முழு normal);
நிலையான வெறுமை frame_new_height(frame_T *topfrp, முழு height, முழு topfirst, முழு wfh, முழு set_ch);
நிலையான முழு frame_fixed_height(frame_T *frp);
நிலையான முழு frame_fixed_width(frame_T *frp);
நிலையான வெறுமை frame_add_statusline(frame_T *frp);
நிலையான வெறுமை frame_new_width(frame_T *topfrp, முழு width, முழு leftfirst, முழு wfw);
நிலையான வெறுமை frame_set_vsep(frame_T *frp, முழு add);
நிலையான முழு frame_minwidth(frame_T *topfrp, win_T *next_curwin);
நிலையான வெறுமை frame_fix_width(win_T *wp);
நிலையான முழு win_alloc_firstwin(win_T *oldwin);
நிலையான வெறுமை new_frame(win_T *wp);
நிலையான tabpage_T *alloc_tabpage(வெறுமை);
நிலையான முழு leave_tabpage(buf_T *new_curbuf, முழு trigger_leave_autocmds);
நிலையான வெறுமை enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, முழு trigger_enter_autocmds, முழு trigger_leave_autocmds);
நிலையான வெறுமை frame_fix_height(win_T *wp);
நிலையான முழு frame_minheight(frame_T *topfrp, win_T *next_curwin);
நிலையான முழு frame_wincount_in_height(frame_T *topfrp);
நிலையான முழு may_open_tabpage(வெறுமை);
நிலையான முழு win_enter_ext(win_T *wp, முழு flags);
நிலையான வெறுமை win_free(win_T *wp, tabpage_T *tp);
நிலையான வெறுமை win_append(win_T *after, win_T *wp);
நிலையான வெறுமை frame_append(frame_T *after, frame_T *frp);
நிலையான வெறுமை frame_செருகு(frame_T *before, frame_T *frp);
நிலையான வெறுமை frame_remove(frame_T *frp);
நிலையான வெறுமை win_goto_ver(முழு up, long மொத்தம்);
நிலையான வெறுமை win_goto_hor(முழு இடது, long மொத்தம்);
நிலையான வெறுமை frame_add_height(frame_T *frp, முழு n);
நிலையான வெறுமை last_status_rec(frame_T *fr, முழு statusline);
#எனில் defined(FEAT_STL_OPT)
நிலையான முழு frame_find_global_stlh_rec(frame_T *frp, முழு h);
நிலையான வெறுமை frame_change_statusline_height_rec(frame_T *frp, முழு global_stlh);
#endif
நிலையான வெறுமை frame_flatten(frame_T *frp);
நிலையான வெறுமை winframe_restore(win_T *wp, முழு dir, frame_T *unflat_altfr);

நிலையான முழு make_snapshot_rec(frame_T *fr, frame_T **frp);
நிலையான வெறுமை clear_snapshot(tabpage_T *tp, முழு idx);
நிலையான வெறுமை clear_snapshot_rec(frame_T *fr);
நிலையான முழு check_snapshot_rec(frame_T *sn, frame_T *fr);
நிலையான win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
நிலையான win_T *get_snapshot_curwin(முழு idx);

நிலையான முழு frame_check_height(frame_T *topfrp, முழு height);
நிலையான முழு frame_check_width(frame_T *topfrp, முழு width);

நிலையான win_T *win_alloc(win_T *after, முழு hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

நிலையான எழுத்து *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
நிலையான முழு split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
நிலையான முழு close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
நிலையான முழு frame_locked = 0;

#எனில் defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
நிலையான முழு stl_rendered_height = 1;
நிலையான முழு stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
நிலையான முழு stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    வெறுமை
window_layout_lock(வெறுமை)
{
    ++split_disallowed;
    ++close_disallowed;
}

    வெறுமை
window_layout_unlock(வெறுமை)
{
    --split_disallowed;
    --close_disallowed;
}

    முழு
frames_locked(வெறுமை)
{
    திரும்பு frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    முழு
window_layout_locked(எண்ணிக்கை CMD_index cmd)
{
    எனில் (split_disallowed > 0 || close_disallowed > 0)
    {
	எனில் (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	இல்லையெனில்
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    நிலையான வெறுமை
log_frame_layout(frame_T *frame)
{
    ch_log(வெற்று, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == வெற்று ? -1 : frame->fr_win->w_width,
	    frame->fr_win == வெற்று ? -1 : frame->fr_win->w_height,
	    frame->fr_win == வெற்று ? -1 : frame->fr_win->w_id);
    எனில் (frame->fr_child != வெற்று)
    {
	ch_log(வெற்று, "children");
	log_frame_layout(frame->fr_child);
	எனில் (frame->fr_next != வெற்று)
	    ch_log(வெற்று, "END of children");
    }
    எனில் (frame->fr_next != வெற்று)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    முழு
check_can_set_curbuf_disabled(வெறுமை)
{
    எனில் (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    முழு
check_can_set_curbuf_forceit(முழு forceit)
{
    எனில் (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(வெறுமை)
{
    // In cmdwin, the alternative buffer should be used.
    திரும்பு is_in_cmdwin() && prevwin != வெற்று ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = வெற்று;

    எனில் (buf == வெற்று)
	திரும்பு wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    எனில் (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    எனில் (wp == வெற்று && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    திரும்பு wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    வெறுமை
do_window(
    முழு		nchar,
    long	Prenum,
    முழு		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    முழு		type = FIND_DEFINE;
    முழு		len;
#endif
    char_u	cbuf[40];

    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

#define CHECK_CMDWIN \
    செய் { \
	எனில் (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    திரும்பு; \
	} \
    } போது (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    தேர்வு (nchar)
    {
// split current window in two parts, horizontally
    நிலை 'S':
    நிலை Ctrl_S:
    நிலை 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		எனில் (bt_quickfix(curbuf))
		    செல் newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(வெறுமை)win_split((முழு)Prenum, 0);
		நிறுத்து;

// split current window in two parts, vertically
    நிலை Ctrl_V:
    நிலை 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		எனில் (bt_quickfix(curbuf))
		    செல் newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(வெறுமை)win_split((முழு)Prenum, WSP_VERT);
		நிறுத்து;

// split current window and edit alternate file
    நிலை Ctrl_HAT:
    நிலை '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		எனில் (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == வெற்று)
		{
		    எனில் (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    இல்லையெனில்
			semsg(_(e_buffer_nr_not_found), Prenum);
		    நிறுத்து;
		}

		எனில் (!curbuf_locked() && win_split(0, 0) == OK)
		    (வெறுமை)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		நிறுத்து;

// open new window
    நிலை Ctrl_N:
    நிலை 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		எனில் (Prenum)
		    // window height
		    vim_snprintf((எழுத்து *)cbuf, அளவு(cbuf) - 5, "%ld", Prenum);
		இல்லையெனில்
		    cbuf[0] = NUL;
#எனில் defined(FEAT_QUICKFIX)
		எனில் (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		நிறுத்து;

// quit current window
    நிலை Ctrl_Q:
    நிலை 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_மொத்தம்("quit", cbuf, அளவு(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		நிறுத்து;

// close current window
    நிலை Ctrl_C:
    நிலை 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_மொத்தம்("close", cbuf, அளவு(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		நிறுத்து;

#எனில் defined(FEAT_QUICKFIX)
// close preview window
    நிலை Ctrl_Z:
    நிலை 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		நிறுத்து;

// cursor to preview window
    நிலை 'P':
		FOR_ALL_WINDOWS(wp)
		    எனில் (wp->w_p_pvw)
			நிறுத்து;
		எனில் (wp == வெற்று)
		    emsg(_(e_there_is_no_preview_window));
		இல்லையெனில்
		    win_goto(wp);
		நிறுத்து;
#endif

// close all but current window
    நிலை Ctrl_O:
    நிலை 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_மொத்தம்("only", cbuf, அளவு(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		நிறுத்து;

// cursor to next window with wrap around
    நிலை Ctrl_W:
    நிலை 'w':
// cursor to previous window with wrap around
    நிலை 'W':
		CHECK_CMDWIN;
		எனில் (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		இல்லையெனில்
		{
		    எனில் (Prenum)			// go to specified window
		    {
			க்கு (wp = firstwin; --Prenum > 0; )
			{
			    எனில் (wp->w_next == வெற்று)
				நிறுத்து;
			    இல்லையெனில்
				wp = wp->w_next;
			}
		    }
		    இல்லையெனில்
		    {
			எனில் (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    எனில் (wp == வெற்று)
				wp = lastwin;	    // wrap around
			}
			இல்லையெனில்			    // go to next window
			{
			    wp = curwin->w_next;
			    எனில் (wp == வெற்று)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		நிறுத்து;

// cursor to window below
    நிலை 'j':
    நிலை K_DOWN:
    நிலை Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		நிறுத்து;

// cursor to window above
    நிலை 'k':
    நிலை K_UP:
    நிலை Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		நிறுத்து;

// cursor to left window
    நிலை 'h':
    நிலை K_இடது:
    நிலை Ctrl_H:
    நிலை K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		நிறுத்து;

// cursor to right window
    நிலை 'l':
    நிலை K_வலது:
    நிலை Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		நிறுத்து;

// move window to new tab page
    நிலை 'T':
		CHECK_CMDWIN;
		எனில் (one_window())
		    செய்தி(_(m_onlyone));
		இல்லையெனில்
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    எனில் (win_new_tabpage((முழு)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			எனில் (curwin == wp)
			    win_close(curwin, FALSE);
			எனில் (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		நிறுத்து;

// cursor to top-left window
    நிலை 't':
    நிலை Ctrl_T:
		win_goto(firstwin);
		நிறுத்து;

// cursor to bottom-right window
    நிலை 'b':
    நிலை Ctrl_B:
		win_goto(lastwin);
		நிறுத்து;

// cursor to last accessed (previous) window
    நிலை 'p':
    நிலை Ctrl_P:
		எனில் (!win_valid(prevwin))
		    beep_flush();
		இல்லையெனில்
		    win_goto(prevwin);
		நிறுத்து;

// exchange current and next window
    நிலை 'x':
    நிலை Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		நிறுத்து;

// rotate windows downwards
    நிலை Ctrl_R:
    நிலை 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (முழு)Prenum1);    // downwards
		நிறுத்து;

// rotate windows upwards
    நிலை 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (முழு)Prenum1);	    // upwards
		நிறுத்து;

// move window to the very top/bottom/left/right
    நிலை 'K':
    நிலை 'J':
    நிலை 'H':
    நிலை 'L':
		CHECK_CMDWIN;
		எனில் (ONE_WINDOW)
		    beep_flush();
		இல்லையெனில்
		{
		    முழு dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_உச்சி : WSP_BOT);

		    (வெறுமை)win_splitmove(curwin, (முழு)Prenum, dir);
		}
		நிறுத்து;

// make all windows the same width and/or height
    நிலை '=':
		{
		    முழு mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(வெற்று, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		நிறுத்து;

// increase current window height
    நிலை '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (முழு)Prenum1);
		நிறுத்து;

// decrease current window height
    நிலை '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (முழு)Prenum1);
		நிறுத்து;

// set current window height
    நிலை Ctrl__:
    நிலை '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (முழு)Prenum : 9999);
		நிறுத்து;

// increase current window width
    நிலை '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (முழு)Prenum1);
		நிறுத்து;

// decrease current window width
    நிலை '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (முழு)Prenum1);
		நிறுத்து;

// set current window width
    நிலை '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (முழு)Prenum : 9999);
		நிறுத்து;

// jump to tag and split window if tag exists (in preview window)
#எனில் defined(FEAT_QUICKFIX)
    நிலை '}':
		CHECK_CMDWIN;
		எனில் (Prenum)
		    g_do_tagpreview = Prenum;
		இல்லையெனில்
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    நிலை ']':
    நிலை Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		எனில் (Prenum)
		    postponed_split = Prenum;
		இல்லையெனில்
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		எனில் (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		நிறுத்து;

// edit file name under cursor in a new window
    நிலை 'f':
    நிலை 'F':
    நிலை Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		எனில் (check_text_or_curbuf_locked(வெற்று))
		    நிறுத்து;

		ptr = grab_file_name(Prenum1, &lnum);
		எனில் (ptr != வெற்று)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = வெற்று;
		    எனில் ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    எனில் (wp == வெற்று && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			எனில் (do_ecmd(0, ptr, வெற்று, வெற்று, ECMD_LASTL,
						   ECMD_HIDE, வெற்று) == FAIL)
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
			இல்லையெனில்
			    wp = curwin;
		    }

		    எனில் (wp != வெற்று && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		நிறுத்து;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    நிலை 'i':			    // Go to any match
    நிலை Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    நிலை 'd':			    // Go to definition, using 'define'
    நிலை Ctrl_D:
		CHECK_CMDWIN;
		எனில் ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    நிறுத்து;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		எனில் (ptr == வெற்று)
		    நிறுத்து;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		நிறுத்து;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#எனில் defined(FEAT_QUICKFIX)
    நிலை K_KENTER:
    நிலை CAR:
		எனில் (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		நிறுத்து;
#endif

// CTRL-W g  extended commands
    நிலை 'g':
    நிலை Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_சாவிகள்;   // no mapping for xchar, but allow key codes
		எனில் (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_சாவிகள்;
		(வெறுமை)add_to_showcmd(xchar);

		தேர்வு (xchar)
		{
#எனில் defined(FEAT_QUICKFIX)
		    நிலை '}':
			xchar = Ctrl_RSB;
			எனில் (Prenum)
			    g_do_tagpreview = Prenum;
			இல்லையெனில்
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    நிலை ']':
		    நிலை Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			எனில் (Prenum)
			    postponed_split = Prenum;
			இல்லையெனில்
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			நிறுத்து;

		    நிலை 'f':	    // CTRL-W gf: "gf" in a new tab page
		    நிலை 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			செல் wingotofile;

		    நிலை 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((முழு)Prenum);
			நிறுத்து;

		    நிலை 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(முழு)Prenum1);
			நிறுத்து;

		    நிலை TAB:	    // CTRL-W g<Tab>: go to last used tab page
			எனில் (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			நிறுத்து;

		    இயல்பு:
			beep_flush();
			நிறுத்து;
		}
		நிறுத்து;

    இயல்பு:	beep_flush();
		நிறுத்து;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    வெறுமை
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    தேர்வு (*arg)
    {
    நிலை 'S':
    நிலை Ctrl_S:
    நிலை 's':
    நிலை Ctrl_N:
    நிலை 'n':
    நிலை 'j':
    நிலை Ctrl_J:
    நிலை 'k':
    நிலை Ctrl_K:
    நிலை 'T':
    நிலை Ctrl_R:
    நிலை 'r':
    நிலை 'R':
    நிலை 'K':
    நிலை 'J':
    நிலை '+':
    நிலை '-':
    நிலை Ctrl__:
    நிலை '_':
    நிலை '|':
    நிலை ']':
    நிலை Ctrl_RSB:
    நிலை 'g':
    நிலை Ctrl_G:
    நிலை Ctrl_V:
    நிலை 'v':
    நிலை 'h':
    நிலை Ctrl_H:
    நிலை 'l':
    நிலை Ctrl_L:
    நிலை 'H':
    நிலை 'L':
    நிலை '>':
    நிலை '<':
#எனில் defined(FEAT_QUICKFIX)
    நிலை '}':
#endif
    நிலை 'f':
    நிலை 'F':
    நிலை Ctrl_F:
#ifdef FEAT_FIND_ID
    நிலை 'i':
    நிலை Ctrl_I:
    நிலை 'd':
    நிலை Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		நிறுத்து;

    நிலை Ctrl_HAT:
    நிலை '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		நிறுத்து;

    நிலை Ctrl_Q:
    நிலை 'q':
    நிலை Ctrl_C:
    நிலை 'c':
    நிலை Ctrl_O:
    நிலை 'o':
    நிலை Ctrl_W:
    நிலை 'w':
    நிலை 'W':
    நிலை 'x':
    நிலை Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		நிறுத்து;

#எனில் defined(FEAT_QUICKFIX)
    நிலை Ctrl_Z:
    நிலை 'z':
    நிலை 'P':
#endif
    நிலை 't':
    நிலை Ctrl_T:
    நிலை 'b':
    நிலை Ctrl_B:
    நிலை 'p':
    நிலை Ctrl_P:
    நிலை '=':
    நிலை CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		நிறுத்து;
    }
}

    நிலையான வெறுமை
cmd_with_மொத்தம்(
    எழுத்து	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    எனில் (Prenum > 0)
	vim_snprintf((எழுத்து *)bufp, bufsize, "%s %ld", cmd, Prenum);
    இல்லையெனில்
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    முழு
check_split_disallowed(win_T *wp)
{
    எனில் (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	திரும்பு FAIL;
    }
    எனில் (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	திரும்பு FAIL;
    }
    திரும்பு OK;
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
    முழு
win_split(முழு size, முழு flags)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு FAIL;

    எனில் (check_split_disallowed(curwin) == FAIL)
	திரும்பு FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    எனில் (may_open_tabpage() == OK)
	திரும்பு OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    எனில் ((flags & WSP_உச்சி) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	திரும்பு FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    எனில் (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    இல்லையெனில்
	clear_snapshot(curtab, SNAP_HELP_IDX);

    எனில் (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    இல்லையெனில்
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    திரும்பு win_split_ins(size, flags, வெற்று, 0, வெற்று);
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
    முழு
win_split_ins(
    முழு		size,
    முழு		flags,
    win_T	*new_wp,
    முழு		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    முழு		new_size = size;
    முழு		i;
    முழு		need_status = 0;
    முழு		do_equal = FALSE;
    முழு		needed;
    முழு		available;
    முழு		oldwin_height = 0;
    முழு		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    முழு		before;
    முழு		minheight;
    முழு		wmh1;
    முழு		did_set_fraction = FALSE;
    முழு		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    எனில் (new_wp == வெற்று)
	trigger_winnewpre();

    எனில் (flags & WSP_உச்சி)
	oldwin = firstwin;
    இல்லையெனில் எனில் (flags & WSP_BOT)
	oldwin = lastwin;
    இல்லையெனில்
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    எனில் (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	எனில் (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    செல் theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    எனில் (gui.in_use)
	out_flush();
#endif

    எனில் (flags & WSP_VERT)
    {
	முழு	wmw1;
	முழு	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	எனில் (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	எனில் (flags & (WSP_BOT | WSP_உச்சி))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	இல்லையெனில் எனில் (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    க்கு (frp = oldwin->w_frame->fr_parent; frp != வெற்று;
							frp = frp->fr_parent)
	    {
		எனில் (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			எனில் (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	இல்லையெனில்
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	எனில் (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    செல் theend;
	}
	எனில் (new_size == 0)
	    new_size = oldwin->w_width / 2;
	எனில் (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	எனில் (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	எனில் (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	எனில் (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	எனில் (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != வெற்று)
	    do_equal = TRUE;
    }
    இல்லையெனில்
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	எனில் (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	எனில் (flags & (WSP_BOT | WSP_உச்சி))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	இல்லையெனில் எனில் (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    க்கு (frp = oldwin->w_frame->fr_parent; frp != வெற்று;
							frp = frp->fr_parent)
	    {
		எனில் (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			எனில் (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	இல்லையெனில்
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	எனில் (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    செல் theend;
	}
	oldwin_height = oldwin->w_height;
	எனில் (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	முழு new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != வெற்று ? new_wp : curwin);
	எனில் (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	எனில் (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	எனில் (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	எனில் (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	எனில் (oldwin->w_p_wfh)
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
	எனில் (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != வெற்று)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    எனில் ((flags & WSP_உச்சி) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	எனில் (new_wp == வெற்று)
	    wp = win_alloc(oldwin, FALSE);
	இல்லையெனில்
	    win_append(oldwin, wp);
    }
    இல்லையெனில்
    {
	எனில் (new_wp == வெற்று)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	இல்லையெனில்
	    win_append(oldwin->w_prev, wp);
    }

    எனில் (new_wp == வெற்று)
    {
	எனில் (wp == வெற்று)
	    செல் theend;

	new_frame(wp);
	எனில் (wp->w_frame == வெற்று)
	{
	    win_free(wp, வெற்று);
	    செல் theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    எனில் (to_flatten != வெற்று)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    எனில் (flags & (WSP_உச்சி | WSP_BOT))
    {
	எனில் ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    எனில் (flags & WSP_BOT)
		போது (curfrp->fr_next != வெற்று)
		    curfrp = curfrp->fr_next;
	}
	இல்லையெனில்
	    curfrp = topframe;
	before = (flags & WSP_உச்சி);
    }
    இல்லையெனில்
    {
	curfrp = oldwin->w_frame;
	எனில் (flags & WSP_BELOW)
	    before = FALSE;
	இல்லையெனில் எனில் (flags & WSP_ABOVE)
	    before = TRUE;
	இல்லையெனில் எனில் (flags & WSP_VERT)
	    before = !p_spr;
	இல்லையெனில்
	    before = !p_sb;
    }
    எனில் (curfrp->fr_parent == வெற்று || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = வெற்று;
	frp->fr_prev = வெற்று;
	curfrp->fr_child = frp;
	curfrp->fr_win = வெற்று;
	curfrp = frp;
	எனில் (frp->fr_win != வெற்று)
	    oldwin->w_frame = frp;
	இல்லையெனில்
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    எனில் (new_wp == வெற்று)
	frp = wp->w_frame;
    இல்லையெனில்
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    எனில் (before)
	frame_செருகு(curfrp, frp);
    இல்லையெனில்
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    எனில் (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    எனில் (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	எனில் (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	எனில் (flags & (WSP_உச்சி | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    முழு stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	இல்லையெனில்
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
	எனில் (before)
	    wp->w_vsep_width = 1;
	இல்லையெனில்
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	எனில் (flags & (WSP_உச்சி | WSP_BOT))
	{
	    எனில் (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_உச்சி) != 0)), flags & WSP_உச்சி,
								       FALSE);
	}
	இல்லையெனில்
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	எனில் (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	இல்லையெனில்		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    இல்லையெனில்
    {
	// width and column of new window is same as current window
	எனில் (flags & (WSP_உச்சி | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	இல்லையெனில்
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	முழு old_status_height = oldwin->w_status_height;
	எனில் (flags & (WSP_உச்சி | WSP_BOT))
	{
	    முழு new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    எனில் (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    எனில் (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_உச்சி, FALSE,
									FALSE);
	}
	இல்லையெனில்
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	எனில் (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	இல்லையெனில்		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    எனில் (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    எனில் (flags & (WSP_உச்சி | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    எனில் (need_status)
    {
	செய்தி_row = Rows - 1;
	செய்தி_col = sc_col;
	செய்தி_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	செய்தி_row = Rows - 1;
	செய்தி_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    எனில் (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    இல்லையெனில் எனில் (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    எனில் (flags & WSP_VERT)
    {
	i = p_wiw;
	எனில் (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	எனில் (gui.in_use)
	    gui_init_which_components(வெற்று);
#endif
    }
    இல்லையெனில்
    {
	i = p_wh;
	எனில் (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (வெறுமை)win_enter_ext(wp, (new_wp == வெற்று ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    எனில் (flags & WSP_VERT)
	p_wiw = i;
    இல்லையெனில்
	p_wh = i;
    retval = OK;

theend:
    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    திரும்பு retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    நிலையான வெறுமை
win_init(win_T *newp, win_T *oldp, முழு flags UNUSED)
{
    முழு		i;

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
    எனில் (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = வெற்று;
	newp->w_llist_ref = வெற்று;
    }
    இல்லையெனில்
	copy_loclist_அடுக்கு(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == வெற்று)
				    ? வெற்று : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == வெற்று)
				    ? வெற்று : vim_strsave(oldp->w_prevdir);

    எனில் (*p_spk != 'c')
    {
	எனில் (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    க்கு (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	எனில் (tag->tagname != வெற்று)
	    tag->tagname = vim_strsave(tag->tagname);
	எனில் (tag->user_data != வெற்று)
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
	bool pushed = தள்ளு_highlight_overrides(newp->w_hl, newp->w_hl_len);
	எனில் (newp->w_buffer->b_term != வெற்று)
	    term_init_default_colors(newp->w_buffer->b_term);
	எனில் (pushed)
	    எடு_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    நிலையான வெறுமை
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
    முழு
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	எனில் (wp == win)
	    திரும்பு TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp == win)
	    திரும்பு TRUE;
#endif
    திரும்பு FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    முழு
win_valid(win_T *win)
{
    win_T	*wp;

    எனில் (win == வெற்று)
	திரும்பு FALSE;
    FOR_ALL_WINDOWS(wp)
	எனில் (wp == win)
	    திரும்பு TRUE;
    திரும்பு win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(முழு id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_id == id)
	    திரும்பு wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	எனில் (wp->w_id == id)
	    திரும்பு wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp->w_id == id)
	    திரும்பு wp;
#endif
    திரும்பு வெற்று;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    முழு
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    எனில் (win == வெற்று)
	திரும்பு FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    எனில் (wp == win)
		திரும்பு TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    எனில் (wp == win)
		திரும்பு TRUE;
#endif
    }
    திரும்பு win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    முழு
win_மொத்தம்(வெறுமை)
{
    win_T	*wp;
    முழு		மொத்தம் = 0;

    FOR_ALL_WINDOWS(wp)
	++மொத்தம்;
    திரும்பு மொத்தம்;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    முழு
make_windows(
    முழு		மொத்தம்,
    முழு		vertical)  // split windows vertically if TRUE
{
    முழு		maxcount;
    முழு		todo;

    எனில் (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    இல்லையெனில்
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    எனில் (maxcount < 2)
	maxcount = 2;
    எனில் (மொத்தம் > maxcount)
	மொத்தம் = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    எனில் (மொத்தம் > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    க்கு (todo = மொத்தம் - 1; todo > 0; --todo)
	எனில் (vertical)
	{
	    எனில் (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		நிறுத்து;
	}
	இல்லையெனில்
	{
	    எனில் (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		நிறுத்து;
	}

    unblock_autocmds();

    // return actual number of windows
    திரும்பு (மொத்தம் - todo);
}

/*
 * Exchange current and next window
 */
    நிலையான வெறுமை
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    முழு		temp;

    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;
    எனில் (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	திரும்பு;
    }
    எனில் (text_or_buf_locked())
    {
	beep_flush();
	திரும்பு;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    எனில் (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	போது (frp != வெற்று && --Prenum > 0)
	    frp = frp->fr_next;
    }
    இல்லையெனில் எனில் (curwin->w_frame->fr_next != வெற்று)	// Swap with next
	frp = curwin->w_frame->fr_next;
    இல்லையெனில்    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    எனில் (frp == வெற்று || frp->fr_win == வெற்று || frp->fr_win == curwin)
	திரும்பு;
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
    எனில் (wp->w_prev != curwin)
    {
	win_remove(curwin, வெற்று);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_செருகு(frp, curwin->w_frame);
    }
    எனில் (wp != wp2)
    {
	win_remove(wp, வெற்று);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	எனில் (frp2 == வெற்று)
	    frame_செருகு(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	இல்லையெனில்
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    எனில் (temp != wp->w_status_height)
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
#எனில் defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    எனில் (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    இல்லையெனில் எனில் (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    நிலையான வெறுமை
win_rotate(முழு upwards, முழு மொத்தம்)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    முழு		n;

    எனில் (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	திரும்பு;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	எனில் (frp->fr_win == வெற்று)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    திரும்பு;
	}

    போது (மொத்தம்--)
    {
	எனில் (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, வெற்று);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    க்கு ( ; frp->fr_next != வெற்று; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	இல்லையெனில்			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    க்கு (frp = curwin->w_frame; frp->fr_next != வெற்று;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, வெற்று);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_செருகு(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	எனில் (n != wp1->w_status_height)
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
#எனில் defined(FEAT_STL_OPT)
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
    முழு
win_splitmove(win_T *wp, முழு size, முழு flags)
{
    முழு		dir;
    முழு		height = wp->w_height;
    frame_T	*unflat_altfr;

    எனில் (ONE_WINDOW)
	திரும்பு OK;	// nothing to do
    எனில் (check_split_disallowed(wp) == FAIL)
	திரும்பு FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, வெற்று, &unflat_altfr);
    win_remove(wp, வெற்று);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    எனில் (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	திரும்பு FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    எனில் (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	எனில் (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#எனில் defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    திரும்பு OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    வெறுமை
win_move_after(win_T *win1, win_T *win2)
{
    முழு		height;

    // check if the arguments are reasonable
    எனில் (win1 == win2)
	திரும்பு;

    // check if there is something to do
    எனில் (win2->w_next != win1)
    {
	எனில் (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    திரும்பு;
	}

	// may need to move the status line/vertical separator of the last
	// window
	எனில் (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    எனில் (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	இல்லையெனில் எனில் (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    எனில் (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, வெற்று);
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
    வெறுமை
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    முழு		current,	// do only frame with current window
    முழு		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    எனில் (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == வெற்று ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#எனில் defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    எனில் (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    நிலையான வெறுமை
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    முழு		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    முழு		dir,		// 'v', 'h' or 'b', see win_equal()
    முழு		col,		// horizontal position for frame
    முழு		row,		// vertical position for frame
    முழு		width,		// new width of frame
    முழு		height)		// new height of frame
{
    முழு		n, m;
    முழு		extra_sep = 0;
    முழு		wincount, totwincount = 0;
    frame_T	*fr;
    முழு		next_curwin_size = 0;
    முழு		room = 0;
    முழு		new_size;
    முழு		has_next_curwin = 0;
    முழு		hnc;

    எனில் (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	எனில் (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    இல்லையெனில் எனில் (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	எனில் (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    எனில் (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    இல்லையெனில்
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
	    எனில் (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    இல்லையெனில்
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    எனில் (!frame_fixed_width(fr))
			தொடர்;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    எனில் (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			எனில் (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    இல்லையெனில்
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == வெற்று
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    எனில் (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		எனில் (next_curwin_size == -1)
		{
		    எனில் (!has_next_curwin)
			next_curwin_size = 0;
		    இல்லையெனில் எனில் (totwincount > 1
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
		    இல்லையெனில்
			next_curwin_size = p_wiw;
		}
	    }

	    எனில் (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    எனில் (fr->fr_next == வெற்று)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    இல்லையெனில் எனில் (dir == 'v')
		new_size = fr->fr_width;
	    இல்லையெனில் எனில் (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    இல்லையெனில்
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == வெற்று ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		எனில் (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		இல்லையெனில்
		    hnc = FALSE;
		எனில் (hnc)	    // don't count next_curwin
		    --wincount;
		எனில் (totwincount == 0)
		    new_size = room;
		இல்லையெனில்
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		எனில் (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    எனில் (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		இல்லையெனில்
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    எனில் (!current || dir != 'v' || topfr->fr_parent != வெற்று
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    இல்லையெனில் // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	எனில் (dir != 'h')			// equalize frame heights
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
	    எனில் (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    இல்லையெனில்
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    எனில் (!frame_fixed_height(fr))
			தொடர்;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    எனில் (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			எனில் (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    இல்லையெனில்
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    எனில் (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		எனில் (next_curwin_size == -1)
		{
		    எனில் (!has_next_curwin)
			next_curwin_size = 0;
		    இல்லையெனில் எனில் (totwincount > 1
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
		    இல்லையெனில்
			next_curwin_size = p_wh;
		}
	    }

	    எனில் (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    எனில் (fr->fr_next == வெற்று)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    இல்லையெனில் எனில் (dir == 'h')
		new_size = fr->fr_height;
	    இல்லையெனில் எனில் (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    இல்லையெனில்
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		எனில் (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		இல்லையெனில்
		    hnc = FALSE;
		எனில் (hnc)	    // don't count next_curwin
		    --wincount;
		எனில் (totwincount == 0)
		    new_size = room;
		இல்லையெனில்
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		எனில் (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		இல்லையெனில்
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    எனில் (!current || dir != 'h' || topfr->fr_parent != வெற்று
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

#எனில் defined(FEAT_JOB_CHANNEL)
    வெறுமை
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    எனில் (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	திரும்பு;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_செருகு = restart_edit;
    எனில் (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    எனில் ((State & MODE_செருகு) && !stop_செருகு_mode)
    {
	stop_செருகு_mode = TRUE;
	எனில் (win->w_buffer->b_prompt_செருகு == NUL)
	    win->w_buffer->b_prompt_செருகு = 'A';
    }
}

    வெறுமை
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    எனில் (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	திரும்பு;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    எனில் (win->w_buffer->b_prompt_செருகு != NUL)
	stop_செருகு_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    எனில் ((State & MODE_செருகு) == 0)
	restart_edit = win->w_buffer->b_prompt_செருகு;
}
#endif

    நிலையான வெறுமை
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_வரிகள்_valid = 0;
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
#எனில் defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    வெறுமை
curwin_init(வெறுமை)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    வெறுமை
close_windows(
    buf_T	*buf,
    முழு		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    முழு		மொத்தம் = tabpage_index(வெற்று);

    ++RedrawingDisabled;

    க்கு (wp = firstwin; wp != வெற்று && !ONE_WINDOW; )
    {
	எனில் (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    எனில் (window_layout_locked(CMD_SIZE))
		செல் theend;  // Only give one error message.
	    எனில் (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		நிறுத்து;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	இல்லையெனில்
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    க்கு (tp = first_tabpage; tp != வெற்று; tp = nexttp)
    {
	nexttp = tp->tp_next;
	எனில் (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		எனில் (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    எனில் (window_layout_locked(CMD_SIZE))
			செல் theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    நிறுத்து;
		}
    }

theend:
    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;

    எனில் (மொத்தம் != tabpage_index(வெற்று))
	apply_autocmds(EVENT_TABCLOSED, வெற்று, வெற்று, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    முழு
last_window(வெறுமை)
{
    திரும்பு (one_window() && first_tabpage->tp_next == வெற்று);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    முழு
one_window(வெறுமை)
{
    win_T	*wp;
    முழு		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	எனில் (!is_aucmd_win(wp))
	{
	    எனில் (seen_one)
		திரும்பு FALSE;
	    seen_one = TRUE;
	}
    }
    திரும்பு TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    நிலையான முழு
close_last_window_tabpage(
    win_T	*win,
    முழு		free_buf,
    tabpage_T   *prev_curtab)
{
    எனில் (!ONE_WINDOW)
	திரும்பு FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != வெற்று);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    எனில் (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    எனில் (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, வெற்று, வெற்று, FALSE, curbuf);
#எனில் defined(FEAT_TABPANEL)
    எனில் (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, வெற்று, வெற்று, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, வெற்று, வெற்று, FALSE, curbuf);
    எனில் (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
    திரும்பு TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    நிலையான வெறுமை
win_close_buffer(win_T *win, முழு action, முழு abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    எனில் (win->w_buffer != வெற்று)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    எனில் (win->w_buffer != வெற்று && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    எனில் (win->w_buffer != வெற்று)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	எனில் (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	எனில் (!bufref_valid(&bufref))
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
    முழு
win_close(win_T *win, முழு free_buf)
{
    win_T	*wp;
    முழு		other_buffer = FALSE;
    முழு		close_curwin = FALSE;
    முழு		dir;
    முழு		help_window = FALSE;
    முழு		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    முழு		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    முழு		did_decrement = FALSE;
#endif

#எனில் defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    எனில் (may_close_term_popup() == OK)
	திரும்பு OK;
#endif
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு FAIL;

    எனில் (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	திரும்பு FAIL;
    }
    எனில் (window_layout_locked(CMD_close))
	திரும்பு FAIL;

    எனில் (win_locked(win) || (win->w_buffer != வெற்று
					       && win->w_buffer->b_locked > 0))
	திரும்பு FAIL; // window is already being closed
    எனில் (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	திரும்பு FAIL;
    }
    எனில் ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	திரும்பு FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    எனில் (close_last_window_tabpage(win, free_buf, prev_curtab))
	திரும்பு FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    எனில் (bt_help(win->w_buffer))
	help_window = TRUE;
    இல்லையெனில்
	clear_snapshot(curtab, SNAP_HELP_IDX);

    எனில் (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    இல்லையெனில்
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    எனில் (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, வெற்று));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	எனில் (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    எனில் (!win_valid(win))
		திரும்பு FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	    எனில் (!win_valid(win))
		திரும்பு FAIL;
	    --win->w_locked;
	    எனில் (last_window())
		திரும்பு FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (!win_valid(win))
	    திரும்பு FAIL;
	--win->w_locked;
	எனில் (last_window())
	    திரும்பு FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	எனில் (aborting())
	    திரும்பு FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    எனில் (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    எனில் (popup_win_closed(win) && !win_valid(win))
	திரும்பு FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    எனில் (!win_valid_any_tab(win))
	திரும்பு OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    எனில் (win_valid(win) && win->w_buffer == வெற்று
#எனில் defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	எனில் (curwin->w_buffer == வெற்று)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    எனில் (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == வெற்று)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	திரும்பு FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    எனில் (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	திரும்பு FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    எனில் (win->w_buffer != வெற்று)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, வெற்று);

    எனில் (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	எனில் (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    எனில் (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	எனில் (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    க்கு (;;)
	    {
		எனில் (wp->w_next == வெற்று)
		    wp = firstwin;
		இல்லையெனில்
		    wp = wp->w_next;
		எனில் (wp == curwin)
		    நிறுத்து;
		எனில் (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    நிறுத்து;
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

    எனில் (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    இல்லையெனில்
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    எனில் (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#இல்லையெனில்
	(வெறுமை)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	எனில் (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
    }

    எனில் (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != வெற்று)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, வெற்று, வெற்று, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    எனில் (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    எனில் (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    எனில் (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	முழு	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    எனில் (dwin->w_p_diff)
		++diffcount;
	எனில் (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#எனில் defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    எனில் (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(வெற்று);
#endif

    redraw_all_later(UPD_NOT_VALID);
    திரும்பு OK;
}

    நிலையான வெறுமை
trigger_winnewpre(வெறுமை)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, வெற்று, வெற்று, FALSE, வெற்று);
    window_layout_unlock();
}

    நிலையான வெறுமை
trigger_winclosed(win_T *win)
{
    நிலையான முழு	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    எனில் (recursive)
	திரும்பு;
    recursive = TRUE;
    vim_snprintf((எழுத்து *)winid, அளவு(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    வெறுமை
trigger_tabclosedpre(tabpage_T *tp)
{
    நிலையான முழு	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    எனில் (!has_tabclosedpre() || recursive)
	திரும்பு;

    எனில் (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, வெற்று, வெற்று, FALSE, வெற்று);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    எனில் (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    இல்லையெனில்
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    வெறுமை
snapshot_windows_scroll_size(வெறுமை)
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

நிலையான முழு did_initial_scroll_size_snapshot = FALSE;

    வெறுமை
may_make_initial_scroll_size_snapshot(வெறுமை)
{
    எனில் (!did_initial_scroll_size_snapshot)
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
    நிலையான dict_T *
make_win_info_dict(
	முழு width,
	முழு height,
	முழு topline,
# ifdef FEAT_DIFF
	முழு topfill,
# endif
	முழு leftcol,
	முழு skipcol)
{
    dict_T *d = dict_alloc();
    எனில் (d == வெற்று)
	திரும்பு வெற்று;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    போது (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_எண்;

	tv.vval.v_எண் = width;
	எனில் (dict_add_tv(d, "width", &tv) == FAIL)
	    நிறுத்து;
	tv.vval.v_எண் = height;
	எனில் (dict_add_tv(d, "height", &tv) == FAIL)
	    நிறுத்து;
	tv.vval.v_எண் = topline;
	எனில் (dict_add_tv(d, "topline", &tv) == FAIL)
	    நிறுத்து;
# ifdef FEAT_DIFF
	tv.vval.v_எண் = topfill;
# இல்லையெனில்
	tv.vval.v_எண் = 0;
# endif
	எனில் (dict_add_tv(d, "topfill", &tv) == FAIL)
	    நிறுத்து;
	tv.vval.v_எண் = leftcol;
	எனில் (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    நிறுத்து;
	tv.vval.v_எண் = skipcol;
	எனில் (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    நிறுத்து;
	திரும்பு d;
    }
    dict_unref(d);
    திரும்பு வெற்று;
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
    நிலையான வெறுமை
check_window_scroll_resize(
	முழு	*size_மொத்தம்,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    முழு listidx = 0;
    முழு tot_width = 0;
    முழு tot_height = 0;
    முழு tot_topline = 0;
# ifdef FEAT_DIFF
    முழு tot_topfill = 0;
# endif
    முழு tot_leftcol = 0;
    முழு tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	முழு ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	முழு size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	எனில் (size_changed)
	{
#ifdef FEAT_EVAL
	    எனில் (winlist != வெற்று)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_எண்;
		tv.vval.v_எண் = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    இல்லையெனில்
#endif
		எனில் (size_மொத்தம் != வெற்று)
	    {
		++*size_மொத்தம்;
		எனில் (*first_size_win == வெற்று)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		எனில் (*first_scroll_win == வெற்று && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	முழு scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	எனில் (scroll_changed
	    && first_scroll_win != வெற்று && *first_scroll_win == வெற்று)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	எனில் ((size_changed || scroll_changed) && v_event != வெற்று)
	{
	    // Add info about this window to the v:event dictionary.
	    முழு width = wp->w_width - wp->w_last_width;
	    முழு height = wp->w_height - wp->w_last_height;
	    முழு topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    முழு topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    முழு leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    முழு skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    எனில் (d == வெற்று)
		நிறுத்து;
	    எழுத்து winid[NUMBUFLEN];
	    vim_snprintf(winid, அளவு(winid), "%d", wp->w_id);
	    எனில் (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		நிறுத்து;
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
    எனில் (v_event != வெற்று)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	எனில் (alldict != வெற்று)
	{
	    எனில் (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    இல்லையெனில்
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    வெறுமை
may_trigger_win_scrolled_resized(வெறுமை)
{
    நிலையான முழு	    recursive = FALSE;
    முழு		    do_resize = has_winresized();
    முழு		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    எனில் (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	திரும்பு;

    முழு size_மொத்தம் = 0;
    win_T *first_scroll_win = வெற்று, *first_size_win = வெற்று;
    check_window_scroll_resize(&size_மொத்தம், &first_scroll_win, &first_size_win,
								    வெற்று, வெற்று);
    முழு trigger_resize = do_resize && size_மொத்தம் > 0;
    முழு trigger_scroll = do_scroll && first_scroll_win != வெற்று;
    எனில் (!trigger_resize && !trigger_scroll)
	திரும்பு;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = வெற்று;
    எனில் (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_பொருட்கள்(size_மொத்தம்);
	எனில் (windows_list != வெற்று)
	    check_window_scroll_resize(வெற்று, வெற்று, வெற்று, windows_list, வெற்று);
    }

    dict_T *scroll_dict = வெற்று;
    எனில் (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	எனில் (scroll_dict != வெற்று)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(வெற்று, வெற்று, வெற்று, வெற்று, scroll_dict);
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
    எனில் (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != வெற்று
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	எனில் (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_பொருட்கள்_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((எழுத்து *)winid, அளவு(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    எனில் (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != வெற்று
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", வெற்று);
	dict_set_பொருட்கள்_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((எழுத்து *)winid, அளவு(winid), "%d",
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
    வெறுமை
win_close_othertab(win_T *win, முழு free_buf, tabpage_T *tp)
{
    win_T	*wp;
    முழு		dir;
    tabpage_T   *ptp = வெற்று;
    முழு		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    எனில் (window_layout_locked(CMD_SIZE))
	திரும்பு;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    எனில் (win_locked(win) || (win->w_buffer != வெற்று
					       && win->w_buffer->b_locked > 0))
	திரும்பு; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    எனில் (win->w_buffer != வெற்று)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	எனில் (!win_valid_any_tab(win))
	    திரும்பு;
    }

    எனில் (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	எனில் (!win_valid_any_tab(win))
	    திரும்பு;
    }

    எனில் (win->w_buffer != வெற்று)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    க்கு (ptp = first_tabpage; ptp != வெற்று && ptp != tp; ptp = ptp->tp_next)
	;
    எனில் (ptp == வெற்று || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	எனில் (win_valid_any_tab(win) && win->w_buffer == வெற்று)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    எனில் (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	திரும்பு;
    }

    // Autocommands may have closed the window already.
    க்கு (wp = tp->tp_firstwin; wp != வெற்று && wp != win; wp = wp->w_next)
	;
    எனில் (wp == வெற்று)
	திரும்பு;

    // When closing the last window in a tab page remove the tab page.
    எனில் (tp->tp_firstwin == tp->tp_lastwin)
    {
	முழு	h = tabline_height();

	எனில் (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	இல்லையெனில்
	{
	    க்கு (ptp = first_tabpage; ptp != வெற்று && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    எனில் (ptp == வெற்று)
	    {
		internal_error("win_close_othertab()");
		திரும்பு;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	எனில் (h != tabline_height())
	    shell_new_rows();
    }

    எனில் (win->w_buffer != வெற்று)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    எனில் (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    நிலையான win_T *
win_free_mem(
    win_T	*win,
    முழு		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == வெற்று ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, வெற்று);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    எனில் (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    திரும்பு wp;
}

#எனில் defined(EXITFREE)
    வெறுமை
win_free_all(வெறுமை)
{
    முழு		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = வெற்று;
    cmdwin_win = வெற்று;

    போது (first_tabpage->tp_next != வெற்று)
	tabpage_close(TRUE);

    க்கு (முழு i = 0; i < AUCMD_WIN_மொத்தம்; ++i)
	எனில் (aucmd_win[i].auc_win != வெற்று)
	{
	    (வெறுமை)win_free_mem(aucmd_win[i].auc_win, &dummy, வெற்று);
	    aucmd_win[i].auc_win = வெற்று;
	}

    போது (firstwin != வெற்று)
	(வெறுமை)win_free_mem(firstwin, &dummy, வெற்று);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = வெற்று;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    முழு		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    முழு		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    எனில் (tp == வெற்று ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	திரும்பு வெற்று;

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
    எனில் (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != வெற்று)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    எனில் (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	எனில் (frp2->fr_win != வெற்று && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    போது (frp != வெற்று || frp3 != வெற்று)
	    {
		எனில் (frp != வெற்று)
		{
		    எனில் (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			நிறுத்து;
		    }
		    frp = frp->fr_prev;
		}
		எனில் (frp3 != வெற்று)
		{
		    எனில் (frp3->fr_win != வெற்று && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			நிறுத்து;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    இல்லையெனில்
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	எனில் (frp2->fr_win != வெற்று && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    போது (frp != வெற்று || frp3 != வெற்று)
	    {
		எனில் (frp != வெற்று)
		{
		    எனில் (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			நிறுத்து;
		    }
		    frp = frp->fr_prev;
		}
		எனில் (frp3 != வெற்று)
		{
		    எனில் (frp3->fr_win != வெற்று && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			நிறுத்து;
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
    எனில் (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    எனில் (unflat_altfr == வெற்று)
	frame_flatten(frp2);
    இல்லையெனில்
	*unflat_altfr = frp2;

    frame_locked--;

    திரும்பு wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    நிலையான வெறுமை
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    எனில் (frp->fr_next != வெற்று || frp->fr_prev != வெற்று)
	திரும்பு;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    எனில் (frp->fr_win != வெற்று)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    எனில் (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    எனில் (frp != வெற்று && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	எனில் (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	எனில் (frp2->fr_prev != வெற்று)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	க்கு (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    எனில் (frp3->fr_next == வெற்று)
	    {
		frp3->fr_next = frp2->fr_next;
		எனில் (frp2->fr_next != வெற்று)
		    frp2->fr_next->fr_prev = frp3;
		நிறுத்து;
	    }
	}
	எனில் (topframe->fr_child == frp2)
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
    நிலையான வெறுமை
winframe_restore(win_T *wp, முழு dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    எனில் (frp->fr_prev != வெற்று)
	frame_append(frp->fr_prev, frp);
    இல்லையெனில்
	frame_செருகு(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    எனில் (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != வெற்று)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    எனில் (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != வெற்று)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    எனில் (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    இல்லையெனில் எனில் (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    எனில் (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	முழு	row = topleft->w_winrow;
	முழு	col = topleft->w_wincol;

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
    நிலையான frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    எனில் (tp == வெற்று ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	திரும்பு alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    எனில் (frp->fr_prev == வெற்று)
	திரும்பு frp->fr_next;
    எனில் (frp->fr_next == வெற்று)
	திரும்பு frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    எனில் (frp->fr_parent != வெற்று && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    எனில் (frp->fr_parent != வெற்று && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    எனில் (frp->fr_parent != வெற்று && frp->fr_parent->fr_layout == FR_ROW)
    {
	எனில் (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    இல்லையெனில்
    {
	எனில் (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    திரும்பு target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    நிலையான tabpage_T *
alt_tabpage(வெறுமை)
{
    tabpage_T	*tp = வெற்று;
    முழு		forward;

    // Use the last accessed tab page, if possible.
    எனில் ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	திரும்பு lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != வெற்று &&
	    ((tcl_flags & TCL_இடது) == 0 || curtab == first_tabpage);

    எனில் (forward)
	tp = curtab->tp_next;
    இல்லையெனில்
	// Use the previous tab page.
	க்கு (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    திரும்பு tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    நிலையான win_T *
frame2win(frame_T *frp)
{
    போது (frp->fr_win == வெற்று)
	frp = frp->fr_child;
    திரும்பு frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    நிலையான முழு
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    எனில் (frp->fr_layout == FR_LEAF)
	திரும்பு frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	எனில் (frame_has_win(p, wp))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
நிலையான முழு min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    நிலையான வெறுமை
frame_new_height(
    frame_T	*topfrp,
    முழு		height,
    முழு		topfirst,	// resize topmost contained frame first
    முழு		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    முழு		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    முழு		extra_வரிகள்;
    முழு		h;

    எனில் (topfrp->fr_parent == வெற்று && set_ch)
    {
	// topframe: update the command line height, with side effects.
	முழு new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	முழு save_ch = min_set_ch;
	எனில் (new_ch != p_ch)
	    set_option_மதிப்பு((char_u *)"cmdheight", new_ch, வெற்று, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    எனில் (topfrp->fr_win != வெற்று)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    இல்லையெனில் எனில் (topfrp->fr_layout == FR_ROW)
    {
	செய்
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		எனில் (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    நிறுத்து;
		}
	    }
	}
	போது (frp != வெற்று);
    }
    இல்லையெனில்    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	எனில் (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    போது (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		எனில் (frp == வெற்று)
		    திரும்பு;	    // no frame without 'wfh', give up
	    }
	எனில் (!topfirst)
	{
	    // Find the bottom frame of this column
	    போது (frp->fr_next != வெற்று)
		frp = frp->fr_next;
	    எனில் (wfh)
		// Advance back for frames with one window with 'wfh' set.
		போது (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_வரிகள் = height - topfrp->fr_height;
	எனில் (extra_வரிகள் < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    போது (frp != வெற்று)
	    {
		h = frame_minheight(frp, வெற்று);
		எனில் (frp->fr_height + extra_வரிகள் < h)
		{
		    extra_வரிகள் += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		இல்லையெனில்
		{
		    frame_new_height(frp, frp->fr_height + extra_வரிகள்,
							topfirst, wfh, set_ch);
		    நிறுத்து;
		}
		எனில் (topfirst)
		{
		    செய்
			frp = frp->fr_next;
		    போது (wfh && frp != வெற்று && frame_fixed_height(frp));
		}
		இல்லையெனில்
		{
		    செய்
			frp = frp->fr_prev;
		    போது (wfh && frp != வெற்று && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		எனில் (frp == வெற்று)
		    height -= extra_வரிகள்;
	    }
	}
	இல்லையெனில் எனில் (extra_வரிகள் > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_வரிகள், topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    நிலையான முழு
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    எனில் (frp->fr_win != வெற்று)
	திரும்பு frp->fr_win->w_p_wfh;

    எனில் (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    எனில் (frame_fixed_height(frp))
		திரும்பு TRUE;
	திரும்பு FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	எனில் (!frame_fixed_height(frp))
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    நிலையான முழு
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    எனில் (frp->fr_win != வெற்று)
	திரும்பு frp->fr_win->w_p_wfw;

    எனில் (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    எனில் (frame_fixed_width(frp))
		திரும்பு TRUE;
	திரும்பு FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	எனில் (!frame_fixed_width(frp))
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    நிலையான வெறுமை
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    எனில் (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    இல்லையெனில் எனில் (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    இல்லையெனில் // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	க்கு (frp = frp->fr_child; frp->fr_next != வெற்று; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    நிலையான வெறுமை
frame_new_width(
    frame_T	*topfrp,
    முழு		width,
    முழு		leftfirst,	// resize leftmost contained frame first
    முழு		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    முழு		extra_cols;
    முழு		w;
    win_T	*wp;

    எனில் (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	க்கு (frp = topfrp; frp->fr_parent != வெற்று; frp = frp->fr_parent)
	    எனில் (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != வெற்று)
		நிறுத்து;
	எனில் (frp->fr_parent == வெற்று)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    இல்லையெனில் எனில் (topfrp->fr_layout == FR_COL)
    {
	செய்
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		எனில் (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    நிறுத்து;
		}
	    }
	} போது (frp != வெற்று);
    }
    இல்லையெனில்    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	எனில் (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    போது (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		எனில் (frp == வெற்று)
		    திரும்பு;	    // no frame without 'wfw', give up
	    }
	எனில் (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    போது (frp->fr_next != வெற்று)
		frp = frp->fr_next;
	    எனில் (wfw)
		// Advance back for frames with one window with 'wfw' set.
		போது (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	எனில் (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    போது (frp != வெற்று)
	    {
		w = frame_minwidth(frp, வெற்று);
		எனில் (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		இல்லையெனில்
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    நிறுத்து;
		}
		எனில் (leftfirst)
		{
		    செய்
			frp = frp->fr_next;
		    போது (wfw && frp != வெற்று && frame_fixed_width(frp));
		}
		இல்லையெனில்
		{
		    செய்
			frp = frp->fr_prev;
		    போது (wfw && frp != வெற்று && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		எனில் (frp == வெற்று)
		    width -= extra_cols;
	    }
	}
	இல்லையெனில் எனில் (extra_cols > 0)
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
    நிலையான வெறுமை
frame_set_vsep(frame_T *frp, முழு add)
{
    win_T	*wp;

    எனில் (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	எனில் (add && wp->w_vsep_width == 0)
	{
	    எனில் (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	இல்லையெனில் எனில் (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    இல்லையெனில் எனில் (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    இல்லையெனில் // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	போது (frp->fr_next != வெற்று)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    நிலையான வெறுமை
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    நிலையான வெறுமை
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
    நிலையான முழு
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    முழு		m;
    முழு		n;

    எனில் (topfrp->fr_win != வெற்று)
    {
	எனில் (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	இல்லையெனில்
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    எனில் (topfrp->fr_win == curwin && next_curwin == வெற்று)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		எனில் (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    இல்லையெனில் எனில் (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    எனில் (n > m)
		m = n;
	}
    }
    இல்லையெனில்
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    திரும்பு m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
நிலையான முழு
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    முழு		c = 0;
    முழு		n;

    எனில் (topfrp->fr_win != வெற்று)
	c = 1;
    இல்லையெனில் எனில் (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    எனில் (n > c)
		c = n;
	}
    }
    இல்லையெனில்
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    திரும்பு c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    நிலையான முழு
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    முழு		m, n;

    எனில் (topfrp->fr_win != வெற்று)
    {
	எனில் (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	இல்லையெனில்
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    எனில் (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == வெற்று)
		++m;
	}
    }
    இல்லையெனில் எனில் (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    எனில் (n > m)
		m = n;
	}
    }
    இல்லையெனில்
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    திரும்பு m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    வெறுமை
close_others(
    முழு		message,
    முழு		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    முழு		r;

    எனில் (one_window())
    {
	எனில் (message && !autocmd_busy)
	    செய்தி(_(m_onlyone));
	திரும்பு;
    }

    // Be very careful here: autocommands may change the window layout.
    க்கு (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	எனில் (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	எனில் (wp == curwin)		// don't close current window
	    தொடர்;

	// autoccommands messed this one up
	எனில் (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = வெற்று;
	    win_close(wp, 0);
	    தொடர்;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	எனில் (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    தொடர்;
	}
	எனில் (!r)
	{
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    எனில் (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		எனில் (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    தொடர்;
		}
	    }
	    எனில் (bufIsChanged(wp->w_buffer))
#endif
		தொடர்;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    எனில் (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    வெறுமை
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
நிலையான முழு command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    வெறுமை
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
    முழு
win_alloc_first(வெறுமை)
{
    எனில் (win_alloc_firstwin(வெற்று) == FAIL)
	திரும்பு FAIL;

    first_tabpage = alloc_tabpage();
    எனில் (first_tabpage == வெற்று)
	திரும்பு FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    திரும்பு OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(வெறுமை)
{
    win_T *wp;

    wp = win_alloc(வெற்று, TRUE);
    எனில் (wp == வெற்று)
	திரும்பு வெற்று;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    திரும்பு wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    வெறுமை
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
    நிலையான முழு
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(வெற்று, FALSE);
    எனில் (curwin == வெற்று)
	திரும்பு FAIL;
    எனில் (oldwin == வெற்று)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(வெற்று, வெற்று, 1L, BLN_LISTED);
	எனில் (curwin == வெற்று || curbuf == வெற்று)
	    திரும்பு FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    இல்லையெனில்
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    எனில் (curwin->w_frame == வெற்று)
	திரும்பு FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    திரும்பு OK;
}

/*
 * Create a frame for window "wp".
 */
    நிலையான வெறுமை
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    எனில் (frp == வெற்று)
	திரும்பு;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    வெறுமை
win_init_size(வெறுமை)
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
    நிலையான tabpage_T *
alloc_tabpage(வெறுமை)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    முழு		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    எனில் (tp == வெற்று)
	திரும்பு வெற்று;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    எனில் (tp->tp_vars == வெற்று)
    {
	vim_free(tp);
	திரும்பு வெற்று;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    க்கு (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    திரும்பு tp;
}

    வெறுமை
free_tabpage(tabpage_T *tp)
{
    முழு idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    போது (tp->tp_first_popupwin != வெற்று)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    க்கு (idx = 0; idx < SNAP_மொத்தம்; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    எனில் (tp == lastused_tabpage)
	lastused_tabpage = வெற்று;
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
    முழு
win_new_tabpage(முழு after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    முழு		n;
#எனில் defined(FEAT_TABPANEL)
    முழு prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    எனில் (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	திரும்பு FAIL;
    }
    எனில் (window_layout_locked(CMD_tabnew))
	திரும்பு FAIL;

    newtp = alloc_tabpage();
    எனில் (newtp == வெற்று)
	திரும்பு FAIL;

    // Remember the current windows in this Tab page.
    எனில் (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	திரும்பு FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == வெற்று)
				    ? வெற்று : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    எனில் (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	எனில் (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	இல்லையெனில்
	{
	    எனில் (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		க்கு (tp = first_tabpage; tp->tp_next != வெற்று
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

#எனில் defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#எனில் defined(FEAT_TABPANEL)
	எனில் (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    முழு		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		எனில் (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    எனில் (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, வெற்று, வெற்று, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, வெற்று, வெற்று, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, வெற்று, வெற்று, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, வெற்று, வெற்று, FALSE, curbuf);
	திரும்பு OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    திரும்பு FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    நிலையான முழு
may_open_tabpage(வெறுமை)
{
    முழு		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    எனில் (n == 0)
	திரும்பு FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    திரும்பு win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    முழு
make_tabpages(முழு maxcount)
{
    முழு		மொத்தம் = maxcount;
    முழு		todo;

    // Limit to 'tabpagemax' tabs.
    எனில் (மொத்தம் > p_tpm)
	மொத்தம் = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    க்கு (todo = மொத்தம் - 1; todo > 0; --todo)
	எனில் (win_new_tabpage(0) == FAIL)
	    நிறுத்து;

    unblock_autocmds();

    // return actual number of tab pages
    திரும்பு (மொத்தம் - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    முழு
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	எனில் (tp == tpc)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    முழு
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	எனில் (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		எனில் (win_valid_any_tab(wp))
		    திரும்பு TRUE;
	    }
	    திரும்பு FALSE;
	}
    }
    // shouldn't happen
    திரும்பு FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    வெறுமை
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    எனில் (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    இல்லையெனில்
    {
	க்கு (ptp = first_tabpage; ptp != வெற்று && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	ப்ரதிஜ்ஞா(ptp != வெற்று);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(முழு n)
{
    tabpage_T	*tp;
    முழு		i = 1;

    எனில் (n == 0)
	திரும்பு curtab;

    க்கு (tp = first_tabpage; tp != வெற்று && i != n; tp = tp->tp_next)
	++i;
    திரும்பு tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    முழு
tabpage_index(tabpage_T *ftp)
{
    முழு		i = 1;
    tabpage_T	*tp;

    க்கு (tp = first_tabpage; tp != வெற்று && tp != ftp; tp = tp->tp_next)
	++i;
    திரும்பு i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    நிலையான முழு
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    முழு		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    எனில் (trigger_leave_autocmds)
    {
	எனில் (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	    எனில் (curtab != tp)
		திரும்பு FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (curtab != tp)
	    திரும்பு FAIL;
	apply_autocmds(EVENT_TABLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (curtab != tp)
	    திரும்பு FAIL;
    }

    reset_dragwin();
#எனில் defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    எனில் (gui.in_use)
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
    எனில் (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = வெற்று;
    lastwin = வெற்று;
    திரும்பு OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    நிலையான வெறுமை
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    முழு		trigger_enter_autocmds,
    முழு		trigger_leave_autocmds)
{
    முழு		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    எனில் (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	முழு new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_மதிப்பு((char_u *)"cmdheight", new_ch, வெற்று, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (வெறுமை)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    எனில் (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    எனில் (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	எனில் (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	இல்லையெனில்
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#எனில் defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    எனில் (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    வெறுமை
goto_tabpage(முழு n)
{
    tabpage_T	*tp = வெற்று;  // shut up compiler
    tabpage_T	*ttp;
    முழு		i;

    எனில் (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_செய்தி();
	திரும்பு;
    }

    // If there is only one it can't work.
    எனில் (first_tabpage->tp_next == வெற்று)
    {
	எனில் (n > 1)
	    beep_flush();
	திரும்பு;
    }

    எனில் (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	எனில் (curtab->tp_next == வெற்று)
	    tp = first_tabpage;
	இல்லையெனில்
	    tp = curtab->tp_next;
    }
    இல்லையெனில் எனில் (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	க்கு (i = n; i < 0; ++i)
	{
	    க்கு (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != வெற்று;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    இல்லையெனில் எனில் (n == 9999)
    {
	// Go to last tab page.
	க்கு (tp = first_tabpage; tp->tp_next != வெற்று; tp = tp->tp_next)
	    ;
    }
    இல்லையெனில்
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	எனில் (tp == வெற்று)
	{
	    beep_flush();
	    திரும்பு;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    எனில் (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    வெறுமை
goto_tabpage_tp(
    tabpage_T	*tp,
    முழு		trigger_enter_autocmds,
    முழு		trigger_leave_autocmds)
{
    எனில் (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_செய்தி(வெற்று, 0);

    skip_win_fix_scroll = TRUE;
    எனில் (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	எனில் (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	இல்லையெனில்
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    முழு
goto_tabpage_lastused(வெறுமை)
{
    எனில் (!valid_tabpage(lastused_tabpage))
	திரும்பு FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    திரும்பு OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    வெறுமை
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    எனில் (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	எனில் (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    வெறுமை
tabpage_move(முழு nr)
{
    முழு		n = 1;
    tabpage_T	*tp, *tp_dst;

    எனில் (first_tabpage->tp_next == வெற்று)
	திரும்பு;

    எனில் (tabpage_move_disallowed)
	திரும்பு;

    க்கு (tp = first_tabpage; tp->tp_next != வெற்று && n < nr; tp = tp->tp_next)
	++n;

    எனில் (tp == curtab || (nr > 0 && tp->tp_next != வெற்று
						    && tp->tp_next == curtab))
	திரும்பு;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    எனில் (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    இல்லையெனில்
    {
	FOR_ALL_TABPAGES(tp)
	    எனில் (tp->tp_next == curtab)
		நிறுத்து;
	எனில் (tp == வெற்று)	// "cannot happen"
	    திரும்பு;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    எனில் (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    இல்லையெனில்
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
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
    வெறுமை
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;
    எனில் (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	திரும்பு;
    }
#endif
    எனில் (text_or_buf_locked())
    {
	beep_flush();
	திரும்பு;
    }

    எனில் (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    இல்லையெனில் எனில் (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    எனில் (!win_valid(wp))
	திரும்பு;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    எனில் (win_valid(owp) && owp->w_p_cole > 0 && !செய்தி_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    எனில் (curwin->w_p_cole > 0 && !செய்தி_scrolled)
	need_cursor_வரி_redraw = TRUE;
#endif
}

#எனில் defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(முழு winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (--winnr == 0)
	    நிறுத்து;
    திரும்பு wp;
}
#endif

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp == win)
		திரும்பு tp;
    திரும்பு வெற்று;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, முழு up, long மொத்தம்)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    எனில் (popup_is_popup(wp))
	// popups don't have neighbors.
	திரும்பு வெற்று;
#endif
    foundfr = wp->w_frame;
    போது (மொத்தம்--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	க்கு (;;)
	{
	    எனில் (fr == tp->tp_topframe)
		செல் end;
	    எனில் (up)
		nfr = fr->fr_prev;
	    இல்லையெனில்
		nfr = fr->fr_next;
	    எனில் (fr->fr_parent->fr_layout == FR_COL && nfr != வெற்று)
		நிறுத்து;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	க்கு (;;)
	{
	    எனில் (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		நிறுத்து;
	    }
	    fr = nfr->fr_child;
	    எனில் (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		போது (fr->fr_next != வெற்று
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    எனில் (nfr->fr_layout == FR_COL && up)
		போது (fr->fr_next != வெற்று)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    திரும்பு foundfr != வெற்று ? foundfr->fr_win : வெற்று;
}

/*
 * Move to window above or below "count" times.
 */
    நிலையான வெறுமை
win_goto_ver(
    முழு		up,		// TRUE to go to win above
    long	மொத்தம்)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    எனில் (ERROR_IF_TERM_POPUP_WINDOW)
	திரும்பு;
#endif
    win = win_vert_neighbor(curtab, curwin, up, மொத்தம்);
    எனில் (win != வெற்று)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, முழு இடது, long மொத்தம்)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    எனில் (popup_is_popup(wp))
	// popups don't have neighbors.
	திரும்பு வெற்று;
#endif
    foundfr = wp->w_frame;
    போது (மொத்தம்--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	க்கு (;;)
	{
	    எனில் (fr == tp->tp_topframe)
		செல் end;
	    எனில் (இடது)
		nfr = fr->fr_prev;
	    இல்லையெனில்
		nfr = fr->fr_next;
	    எனில் (fr->fr_parent->fr_layout == FR_ROW && nfr != வெற்று)
		நிறுத்து;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	க்கு (;;)
	{
	    எனில் (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		நிறுத்து;
	    }
	    fr = nfr->fr_child;
	    எனில் (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		போது (fr->fr_next != வெற்று
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    எனில் (nfr->fr_layout == FR_ROW && இடது)
		போது (fr->fr_next != வெற்று)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    திரும்பு foundfr != வெற்று ? foundfr->fr_win : வெற்று;
}

/*
 * Move to left or right window.
 */
    நிலையான வெறுமை
win_goto_hor(
    முழு		இடது,		// TRUE to go to left win
    long	மொத்தம்)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    எனில் (ERROR_IF_TERM_POPUP_WINDOW)
	திரும்பு;
#endif
    win = win_horz_neighbor(curtab, curwin, இடது, மொத்தம்);
    எனில் (win != வெற்று)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    வெறுமை
win_enter(win_T *wp, முழு undo_sync)
{
    (வெறுமை)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    வெறுமை
win_fix_current_dir(வெறுமை)
{
    எனில் (curwin->w_localdir != வெற்று || curtab->tp_localdir != வெற்று)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	எனில் (globaldir == வெற்று)
	{
	    char_u	cwd[MAXPATHL];

	    எனில் (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	எனில் (curwin->w_localdir != வெற்று)
	    dirname = curwin->w_localdir;
	இல்லையெனில்
	    dirname = curtab->tp_localdir;

	எனில் (mch_chdir((எழுத்து *)dirname) == 0)
	{
	    last_chdir_reason = வெற்று;
	    shorten_fnames(TRUE);
	}
    }
    இல்லையெனில் எனில் (globaldir != வெற்று)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((எழுத்து *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = வெற்று;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    நிலையான முழு
win_enter_ext(win_T *wp, முழு flags)
{
    முழு		other_buffer = FALSE;
    முழு		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    முழு		did_decrement = FALSE;

    எனில் (wp == curwin && curwin_invalid == 0)	// nothing to do
	திரும்பு FALSE;

#ifdef FEAT_JOB_CHANNEL
    எனில் (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    எனில் (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	எனில் (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	    other_buffer = TRUE;
	    எனில் (!win_valid(wp))
		திரும்பு FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (!win_valid(wp))
	    திரும்பு FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	எனில் (aborting())
	    திரும்பு FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    எனில் ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    எனில் (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    எனில் (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    எனில் (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    எனில் (!virtual_active())
	curwin->w_cursor.coladd = 0;
    எனில் (*p_spk == 'c')		// assume cursor position needs updating
	changed_வரி_abv_curs();
    இல்லையெனில்
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    எனில் (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    எனில் (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, வெற்று, வெற்று, FALSE, curbuf);
    எனில் (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    எனில் (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    எனில் (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    எனில் (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((முழு)p_wh);
    இல்லையெனில் எனில் (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    எனில் (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((முழு)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    திரும்பு did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = வெற்று;

    எனில் (curwin->w_buffer == buf)
	wp = curwin;
    இல்லையெனில்
	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_buffer == buf)
		நிறுத்து;
    எனில் (wp != வெற்று)
	win_enter(wp, FALSE);
    திரும்பு wp;
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

    எனில் (wp != வெற்று)
	திரும்பு wp;

    FOR_ALL_TABPAGES(tp)
	எனில் (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		எனில் (wp->w_buffer == buf)
		    நிறுத்து;
	    எனில் (wp != வெற்று)
	    {
		goto_tabpage_win(tp, wp);
		எனில் (curwin != wp)
		    wp = வெற்று;	// something went wrong
		நிறுத்து;
	    }
	}
    திரும்பு wp;
}

நிலையான முழு last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    நிலையான win_T *
win_alloc(win_T *after, முழு hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    எனில் (new_wp == வெற்று)
	திரும்பு வெற்று;

    எனில் (win_alloc_வரிகள்(new_wp) == FAIL)
    {
	vim_free(new_wp);
	திரும்பு வெற்று;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    எனில் (new_wp->w_vars == வெற்று)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	திரும்பு வெற்று;
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
    எனில் (!hidden)
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
    எனில் (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_இடது],
		SBAR_இடது, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_வலது],
		SBAR_வலது, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_தேடல்_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    திரும்பு new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    நிலையான வெறுமை
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    முழு		i;
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
    எனில் (wp->w_stl_click != வெற்று)
    {
	க்கு (i = 0; i < wp->w_stl_click_மொத்தம்; i++)
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

	எனில் (prevwin == wp)
	    prevwin = வெற்று;
	FOR_ALL_TABPAGES(ttp)
	    எனில் (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = வெற்று;
    }
    win_free_lsize(wp);

    க்கு (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    எனில் (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    எனில் (wip2 != wip && wip2->wi_win == வெற்று)
		    {
			எனில் (wip2->wi_next != வெற்று)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			எனில் (wip2->wi_prev == வெற்று)
			    buf->b_wininfo = wip2->wi_next;
			இல்லையெனில்
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			நிறுத்து;
		    }

		wip->wi_win = வெற்று;
	    }

#ifdef FEAT_தேடல்_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_இடது]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_வலது]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    க்கு (i = 0; i < 4; ++i)
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

    எனில் (win_valid_any_tab(wp))
	win_remove(wp, tp);
    எனில் (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    இல்லையெனில்
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    முழு
win_unlisted(win_T *wp)
{
    திரும்பு is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    வெறுமை
win_free_popup(win_T *win)
{
    எனில் (win->w_buffer != வெற்று)
    {
	எனில் (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	இல்லையெனில்
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	எனில் (win->w_buffer != வெற்று)
	    --win->w_buffer->b_nwindows;
    }
# எனில் defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    எனில் (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, வெற்று);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    நிலையான வெறுமை
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    எனில் (after == வெற்று)	    // after NULL is in front of the first
	before = firstwin;
    இல்லையெனில்
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    எனில் (after == வெற்று)
	firstwin = wp;
    இல்லையெனில்
	after->w_next = wp;
    எனில் (before == வெற்று)
	lastwin = wp;
    இல்லையெனில்
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    வெறுமை
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    எனில் (wp->w_prev != வெற்று)
	wp->w_prev->w_next = wp->w_next;
    இல்லையெனில் எனில் (tp == வெற்று)
	firstwin = curtab->tp_firstwin = wp->w_next;
    இல்லையெனில்
	tp->tp_firstwin = wp->w_next;

    எனில் (wp->w_next != வெற்று)
	wp->w_next->w_prev = wp->w_prev;
    இல்லையெனில் எனில் (tp == வெற்று)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    இல்லையெனில்
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    நிலையான வெறுமை
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    எனில் (frp->fr_next != வெற்று)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    நிலையான வெறுமை
frame_செருகு(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    எனில் (frp->fr_prev != வெற்று)
	frp->fr_prev->fr_next = frp;
    இல்லையெனில்
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    நிலையான வெறுமை
frame_remove(frame_T *frp)
{
    எனில் (frp->fr_prev != வெற்று)
	frp->fr_prev->fr_next = frp->fr_next;
    இல்லையெனில்
	frp->fr_parent->fr_child = frp->fr_next;
    எனில் (frp->fr_next != வெற்று)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    முழு
win_alloc_வரிகள்(win_T *wp)
{
    wp->w_வரிகள்_valid = 0;
    wp->w_வரிகள் = ALLOC_CLEAR_MULT(wline_T, Rows);
    எனில் (wp->w_வரிகள் == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * free lsize arrays for a window
 */
    வெறுமை
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    எனில் (wp != வெற்று)
	VIM_CLEAR(wp->w_வரிகள்);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    வெறுமை
shell_new_rows(வெறுமை)
{
    முழு		h = (முழு)ROWS_AVAIL;

    எனில் (firstwin == வெற்று)	// not initialized yet
	திரும்பு;
    எனில் (h < frame_minheight(topframe, வெற்று))
	h = frame_minheight(topframe, வெற்று);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    எனில் (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    எனில் (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#எனில் 0
    // Disabled: don't want making the screen smaller make a window larger.
    எனில் (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    வெறுமை
shell_new_columns(வெறுமை)
{
    எனில் (firstwin == வெற்று)	// not initialized yet
	திரும்பு;

#எனில் defined(FEAT_TABPANEL)
    நிலையான முழு prev_tp_width;
    நிலையான முழு prev_wincol;
    நிலையான முழு prev_fr_width;
#endif
    முழு w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    எனில் (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#எனில் defined(FEAT_TABPANEL)
    முழு tp_width = tabpanel_width();
    முழு tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    எனில் (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    எனில் (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (முழு)Rows, 0, (முழு)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    எனில் (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#எனில் 0
    // Disabled: don't want making the screen smaller make a window larger.
    எனில் (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    வெறுமை
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, அளவு(முழு), 1);
    எனில் (ga_grow(gap, win_மொத்தம்() * 2 + 1) == FAIL)
	திரும்பு;

    // first entry is the total lines available for windows
    ((முழு *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((முழு *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((முழு *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    வெறுமை
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    முழு		i, j;

    எனில் (win_மொத்தம்() * 2 + 1 == gap->ga_len
	    && ((முழு *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	க்கு (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((முழு *)gap->ga_data)[i++]);
		win_setheight_win(((முழு *)gap->ga_data)[i++], wp);
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
    வெறுமை
win_comp_pos(வெறுமை)
{
    முழு		row = tabline_height();
    முழு		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#எனில் defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    நிலையான வெறுமை
frame_comp_pos(frame_T *topfrp, முழு *row, முழு *col)
{
    win_T	*wp;
    frame_T	*frp;
    முழு		startcol;
    முழு		startrow;
    முழு		h;

    wp = topfrp->fr_win;
    எனில் (wp != வெற்று)
    {
	எனில் (wp->w_winrow != *row || wp->w_wincol != *col)
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
    இல்லையெனில்
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    எனில் (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    இல்லையெனில்
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    வெறுமை
win_ensure_size(வெறுமை)
{
    எனில் (curwin->w_height == 0)
	win_setheight(1);
    எனில் (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    வெறுமை
win_setheight(முழு height)
{
    win_setheight_win(height, curwin);
#எனில் defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    வெறுமை
win_setheight_win(முழு height, win_T *win)
{
    எனில் (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	எனில் (height < p_wmh)
	    height = p_wmh;
	எனில் (height == 0)
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
    நிலையான வெறுமை
frame_setheight(frame_T *curfrp, முழு height)
{
    முழு		room;		// total number of lines available
    முழு		take;		// number of lines taken from other windows
    முழு		room_cmdline;	// lines available from cmdline
    முழு		run;
    frame_T	*frp;
    முழு		h;
    முழு		room_reserved;

    // If the height already is the desired value, nothing to do.
    எனில் (curfrp->fr_height == height)
	திரும்பு;

    எனில் (curfrp->fr_parent == வெற்று)
    {
	// topframe: can only change the command line height
	எனில் (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    இல்லையெனில் எனில் (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, வெற்று);
	எனில் (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    இல்லையெனில்
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
	க்கு (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		எனில் (frp != curfrp
			&& frp->fr_win != வெற்று
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		எனில் (frp != curfrp)
		    room -= frame_minheight(frp, வெற்று);
	    }
	    எனில் (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    இல்லையெனில்
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		எனில் (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    எனில் (height <= room + room_cmdline)
		நிறுத்து;
	    எனில் (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		நிறுத்து;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (முழு)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	எனில் (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	எனில் (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	எனில் (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    எனில் (take < room_cmdline)
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
	க்கு (run = 0; run < 2; ++run)
	{
	    எனில் (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    இல்லையெனில்
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    போது (frp != வெற்று && take != 0)
	    {
		h = frame_minheight(frp, வெற்று);
		எனில் (room_reserved > 0
			&& frp->fr_win != வெற்று
			&& frp->fr_win->w_p_wfh)
		{
		    எனில் (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    இல்லையெனில்
		    {
			எனில் (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    இல்லையெனில்
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		எனில் (run == 0)
		    frp = frp->fr_next;
		இல்லையெனில்
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    வெறுமை
win_setwidth(முழு width)
{
    win_setwidth_win(width, curwin);
}

    வெறுமை
win_setwidth_win(முழு width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    எனில் (wp == curwin)
    {
	எனில் (width < p_wmw)
	    width = p_wmw;
	எனில் (width == 0)
	    width = 1;
    }
    இல்லையெனில் எனில் (width < 0)
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
    நிலையான வெறுமை
frame_setwidth(frame_T *curfrp, முழு width)
{
    முழு		room;		// total number of lines available
    முழு		take;		// number of lines taken from other windows
    முழு		run;
    frame_T	*frp;
    முழு		w;
    முழு		room_reserved;

    // If the width already is the desired value, nothing to do.
    எனில் (curfrp->fr_width == width)
	திரும்பு;

    எனில் (curfrp->fr_parent == வெற்று)
	// topframe: can't change width
	திரும்பு;

    எனில் (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, வெற்று);
	எனில் (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    இல்லையெனில்
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	க்கு (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		எனில் (frp != curfrp
			&& frp->fr_win != வெற்று
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		எனில் (frp != curfrp)
		    room -= frame_minwidth(frp, வெற்று);
	    }

	    எனில் (width <= room)
		நிறுத்து;
	    எனில் (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		நிறுத்து;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (முழு)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	எனில் (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	எனில் (take < 0 && room - curfrp->fr_width < room_reserved)
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
	க்கு (run = 0; run < 2; ++run)
	{
	    எனில் (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    இல்லையெனில்
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    போது (frp != வெற்று && take != 0)
	    {
		w = frame_minwidth(frp, வெற்று);
		எனில் (room_reserved > 0
			&& frp->fr_win != வெற்று
			&& frp->fr_win->w_p_wfw)
		{
		    எனில் (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    இல்லையெனில்
		    {
			எனில் (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    இல்லையெனில்
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		எனில் (run == 0)
		    frp = frp->fr_next;
		இல்லையெனில்
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    வெறுமை
win_setminheight(வெறுமை)
{
    முழு		room;
    முழு		needed;
    முழு		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    போது (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	எனில் (room >= needed)
	    நிறுத்து;
	--p_wmh;
	எனில் (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    வெறுமை
win_setminwidth(வெறுமை)
{
    முழு		room;
    முழு		needed;
    முழு		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    போது (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, வெற்று);
	எனில் (room >= needed)
	    நிறுத்து;
	--p_wmw;
	எனில் (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    வெறுமை
win_drag_status_வரி(win_T *dragwin, முழு offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    முழு		room;
    முழு		up;	// if TRUE, drag status line up, otherwise down
    முழு		n;

    fr = dragwin->w_frame;
    curfr = fr;
    எனில் (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	எனில் (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    எனில் (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    போது (curfr != topframe && curfr->fr_next == வெற்று)
    {
	எனில் (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	எனில் (fr != topframe)
	    fr = fr->fr_parent;
    }

    எனில் (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	எனில் (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, வெற்று);
	}
	இல்லையெனில்
	{
	    room = 0;
	    க்கு (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, வெற்று);
		எனில் (fr == curfr)
		    நிறுத்து;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    இல்லையெனில்    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	எனில் (curfr->fr_next == வெற்று)
	    --room;
	இல்லையெனில்
	    room -= p_ch;
	எனில் (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, வெற்று);
	fr = curfr;			// put fr at window that grows
    }

    எனில் (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    எனில் (offset <= 0)
	திரும்பு;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    எனில் (fr != வெற்று)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    எனில் (up)
	fr = curfr;		// current frame gets smaller
    இல்லையெனில்
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    போது (fr != வெற்று && offset > 0)
    {
	n = frame_minheight(fr, வெற்று);
	எனில் (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	இல்லையெனில்
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    நிறுத்து;
	}
	எனில் (up)
	    fr = fr->fr_prev;
	இல்லையெனில்
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
    வெறுமை
win_drag_vsep_வரி(win_T *dragwin, முழு offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    முழு		room;
    முழு		இடது;	// if TRUE, drag separator line left, otherwise right
    முழு		n;

    fr = dragwin->w_frame;
    எனில் (fr == topframe)		// only one window (cannot happen?)
	திரும்பு;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    எனில் (fr->fr_layout != FR_ROW)
    {
	எனில் (fr == topframe)	// only a column of windows (cannot happen?)
	    திரும்பு;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    போது (curfr->fr_next == வெற்று)
    {
	எனில் (fr == topframe)
	    நிறுத்து;
	curfr = fr;
	fr = fr->fr_parent;
	எனில் (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    எனில் (offset < 0) // drag left
    {
	இடது = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	க்கு (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, வெற்று);
	    எனில் (fr == curfr)
		நிறுத்து;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    இல்லையெனில்    // drag right
    {
	இடது = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, வெற்று);
	fr = curfr;			// put fr at window that grows
    }

    எனில் (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    எனில் (offset <= 0)		// No room at all, quit.
	திரும்பு;
    எனில் (fr == வெற்று)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	திரும்பு;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, இடது, FALSE);

    // shrink other frames: current and at the left or at the right
    எனில் (இடது)
	fr = curfr;		// current frame gets smaller
    இல்லையெனில்
	fr = curfr->fr_next;	// next frame gets smaller

    போது (fr != வெற்று && offset > 0)
    {
	n = frame_minwidth(fr, வெற்று);
	எனில் (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !இடது, FALSE);
	}
	இல்லையெனில்
	{
	    frame_new_width(fr, fr->fr_width - offset, !இடது, FALSE);
	    நிறுத்து;
	}
	எனில் (இடது)
	    fr = fr->fr_prev;
	இல்லையெனில்
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
    வெறுமை
set_fraction(win_T *wp)
{
    எனில் (wp->w_height > 1)
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
    நிலையான வெறுமை
win_fix_scroll(முழு resize)
{
    எனில் (*p_spk == 'c')
	திரும்பு;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	எனில் (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    எனில் (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    {
		முழு diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		எனில் (diff > 0)
		    cursor_down_inner(wp, diff);
		இல்லையெனில்
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		எனில் (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    இல்லையெனில் எனில் (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    எனில் (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    இல்லையெனில் எனில் (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    நிலையான வெறுமை
win_fix_cursor(முழு normal)
{
    win_T	*wp = curwin;

    எனில் (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_வரி_மொத்தம் < wp->w_height)
	திரும்பு;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_மதிப்பு());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T உச்சி = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    எனில் (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_வரி_மொத்தம்) != 1)
	nlnum = bot;
    இல்லையெனில் எனில் (lnum < உச்சி && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : உச்சி;

    எனில் (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	எனில் (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	இல்லையெனில்  // Scroll instead when not in normal mode.
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
    வெறுமை
win_new_height(win_T *wp, முழு height)
{
    முழு		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    எனில் (height < 0)
	height = 0;
    எனில் (wp->w_height == height)
	திரும்பு;	    // nothing to do

    எனில் (wp->w_height > 0)
    {
	எனில் (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	எனில் (wp->w_height != prev_height)
	    திரும்பு;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	எனில் (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    எனில் (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	எனில் (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    வெறுமை
scroll_to_fraction(win_T *wp, முழு prev_height)
{
    linenr_T	lnum;
    முழு		sline, வரி_size;
    முழு		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    எனில் (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_வரி_மொத்தம் || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	எனில் (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	வரி_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - வரி_size;

	எனில் (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    முழு rows = plines_win(wp, lnum, FALSE);

	    எனில் (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - வரி_size;
	    }
	}

	எனில் (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    முழு		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = வரி_size;
	    எனில் (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		போது (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	இல்லையெனில் எனில் (sline > 0)
	{
	    போது (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, வெற்று, TRUE, வெற்று);
		எனில் (lnum == 1)
		{
		    // first line in buffer is folded
		    வரி_size = 1;
		    --sline;
		    நிறுத்து;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		எனில் (lnum == wp->w_topline)
		    வரி_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		இல்லையெனில்
#endif
		    வரி_size = plines_win(wp, lnum, TRUE);
		sline -= வரி_size;
	    }

	    எனில் (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, வெற்று, &lnum, TRUE, வெற்று);
#endif
		lnum++;
		wp->w_wrow -= வரி_size + sline;
	    }
	    இல்லையெனில் எனில் (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    எனில் (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    எனில் (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    வெறுமை
win_new_width(win_T *wp, முழு width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_வரிகள்_valid = 0;
    changed_வரி_abv_curs_win(wp);
    invalidate_botline_win(wp);

    எனில் (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    வெறுமை
win_comp_scroll(win_T *wp)
{
#எனில் defined(FEAT_EVAL)
    முழு old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    எனில் (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#எனில் defined(FEAT_EVAL)
    எனில் (wp->w_p_scr != old_w_p_scr)
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
    வெறுமை
command_height(வெறுமை)
{
    முழு		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    போது (frp->fr_width != topframe->fr_width && frp->fr_parent != வெற்று)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    போது (frp->fr_prev != வெற்று && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    போது (p_ch > old_p_ch && command_frame_height)
    {
	எனில் (frp == வெற்று)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    நிறுத்து;
	}
	முழு h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, வெற்று));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    எனில் (p_ch < old_p_ch && command_frame_height && frp != வெற்று)
	frame_add_height(frp, (முழு)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    எனில் (செய்தி_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (முழு)Rows, 0, (முழு)Columns, ' ', ' ', 0);
	செய்தி_row = cmdline_row;
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
    நிலையான வெறுமை
frame_add_height(frame_T *frp, முழு n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    க்கு (;;)
    {
	frp = frp->fr_parent;
	எனில் (frp == வெற்று)
	    நிறுத்து;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    வெறுமை
last_status(
    முழு		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#எனில் defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    நிலையான வெறுமை
last_status_rec(frame_T *fr, முழு statusline)
{
    frame_T	*fp;
    win_T	*wp;

    எனில் (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	எனில் (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	இல்லையெனில் எனில் (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    போது (fp->fr_height <= frame_minheight(fp, வெற்று))
	    {
		எனில் (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    திரும்பு;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		எனில் (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != வெற்று)
		    fp = fp->fr_prev;
		இல்லையெனில்
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    எனில் (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    இல்லையெனில்
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	எனில் (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    இல்லையெனில் எனில் (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	எனில் (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	இல்லையெனில்
	{
	    frame_T	*fp2;
	    முழு		max_stlh = 0;
	    முழு		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		எனில் (fp->fr_win != வெற்று && fp->fr_win->w_status_height == 0)
		{
		    முழு h = statusline_height(fp->fr_win);
		    எனில் (h > max_stlh)
			max_stlh = h;
		}
	    எனில் (max_stlh == 0)
		திரும்பு;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    போது (fp2->fr_height - frame_minheight(fp2, வெற்று) < max_stlh)
	    {
		எனில் (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    திரும்பு;
		}
		எனில் (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != வெற்று)
		    fp2 = fp2->fr_prev;
		இல்லையெனில்
		    fp2 = fp2->fr_parent;
	    }

	    எனில் (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    இல்லையெனில்
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		எனில் (fp->fr_win != வெற்று)
		{
		    wp = fp->fr_win;
		    எனில் (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			எனில் (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		இல்லையெனில்
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    இல்லையெனில்
    {
	// horizontally split window, set status line for last one
	க்கு (fp = fr->fr_child; fp->fr_next != வெற்று; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#எனில் defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    வெறுமை
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == வெற்று) ? p_stl : wp->w_p_stl;
    முழு		opt_flags = (wp == வெற்று) ? 0 : OPT_LOCAL;
    முழு		*height = (wp == வெற்று) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == வெற்று ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    நிலையான முழு
frame_find_global_stlh_rec(frame_T *frp, முழு h)
{
    எனில் (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	எனில் (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    முழு win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    எனில் (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    இல்லையெனில்
    {
	க்கு (frp = frp->fr_child; frp != வெற்று; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    திரும்பு h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    நிலையான வெறுமை
frame_change_statusline_height_rec(frame_T *frp, முழு global_stlh)
{
    எனில் (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	எனில் (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    முழு win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    எனில் (wp->w_p_stlo == வெற்று || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		எனில் (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    எனில் (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    முழு	     saved_topfill = wp->w_topfill;
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
    இல்லையெனில்
    {
	க்கு (frp = frp->fr_child; frp != வெற்று; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    முழு
frame_change_statusline_height(வெறுமை)
{
    tabpage_T	*tp;
    முழு		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    திரும்பு global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    வெறுமை
set_stlo_mh(முழு mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    முழு
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    முழு		l_stlo_fh = FALSE;
    முழு		l_stlo_mh = 1;

    எனில் (stlopt != வெற்று)
	p = stlopt;
    இல்லையெனில்
	p = wp->w_p_stlo;

    போது (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	எனில் (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    எனில் (l_stlo_mh < 1)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    எனில் (wp == வெற்று)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    இல்லையெனில்
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    திரும்பு OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    முழு
statusline_height(win_T *wp UNUSED)
{
    முழு stl_height = 1;
#எனில் defined(FEAT_STL_OPT)
    bool fixed;
    முழு l_stl_rendered_height;

    எனில் (wp != வெற்று && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    இல்லையெனில்
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != வெற்று && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    எனில் (!fixed)
    {
	முழு has_custom_stl = (*p_stl != NUL)
	    || (wp != வெற்று && *wp->w_p_stl != NUL);
	எனில் (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	இல்லையெனில்
	    stl_height = 1;
    }
#endif

    திரும்பு stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    முழு
tabline_height(வெறுமை)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    எனில் (gui_use_tabline())
	திரும்பு 0;
#endif
    தேர்வு (p_stal)
    {
	நிலை 0: திரும்பு 0;
	நிலை 1: திரும்பு (first_tabpage->tp_next == வெற்று) ? 0 : 1;
    }
    திரும்பு 1;
}

/*
 * Return the height of the last window's statusline.
 */
    முழு
last_stl_height(
    முழு		morewin)	// pretend there are two or more windows
{
    திரும்பு (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    முழு
min_rows(வெறுமை)
{
    எனில் (firstwin == வெற்று)	// not initialized yet
	திரும்பு MIN_வரிகள்;

    திரும்பு frame_minheight(curtab->tp_topframe, வெற்று) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    முழு
min_rows_for_all_tabpages(வெறுமை)
{
    முழு		total;
    tabpage_T	*tp;
    முழு		n;

    எனில் (firstwin == வெற்று)	// not initialized yet
	திரும்பு MIN_வரிகள்;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, வெற்று);
	எனில் (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    திரும்பு total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    முழு
only_one_window(வெறுமை)
{
    முழு		மொத்தம் = 0;
    win_T	*wp;

#எனில் defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    எனில் (popup_is_popup(curwin))
	திரும்பு FALSE;
#endif

    // If there is another tab page there always is another window.
    எனில் (first_tabpage->tp_next != வெற்று)
	திரும்பு FALSE;

    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_buffer != வெற்று
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++மொத்தம்;
    திரும்பு (மொத்தம் <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    நிலையான வெறுமை
check_lnums_both(முழு do_curwin, முழு nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    முழு need_adjust;

	    எனில் (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்;
	    எனில் (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    எனில் (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_வரி_மொத்தம்;
	    எனில் (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_வரி_மொத்தம்;
	    எனில் (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    வெறுமை
check_lnums(முழு do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    வெறுமை
check_lnums_nested(முழு do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    வெறுமை
reset_lnums(வெறுமை)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    எனில் (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    எனில் (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    எனில் (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
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
    முழு
make_snapshot(முழு idx)
{
    clear_snapshot(curtab, idx);
    எனில் (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

    நிலையான முழு
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    எனில் (*frp == வெற்று)
	திரும்பு FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    எனில் (fr->fr_next != வெற்று)
    {
	எனில் (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    திரும்பு FAIL;
    }
    எனில் (fr->fr_child != வெற்று)
    {
	எனில் (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    திரும்பு FAIL;
    }
    எனில் (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    திரும்பு OK;
}

/*
 * Remove any existing snapshot.
 */
    நிலையான வெறுமை
clear_snapshot(tabpage_T *tp, முழு idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = வெற்று;
}

    நிலையான வெறுமை
clear_snapshot_rec(frame_T *fr)
{
    எனில் (fr == வெற்று)
	திரும்பு;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    நிலையான win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    எனில் (ft->fr_next != வெற்று)
    {
	எனில் ((wp = get_snapshot_curwin_rec(ft->fr_next)) != வெற்று)
	    திரும்பு wp;
    }
    எனில் (ft->fr_child != வெற்று)
    {
	எனில் ((wp = get_snapshot_curwin_rec(ft->fr_child)) != வெற்று)
	    திரும்பு wp;
    }

    திரும்பு ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    நிலையான win_T *
get_snapshot_curwin(முழு idx)
{
    எனில் (curtab->tp_snapshot[idx] == வெற்று)
	திரும்பு வெற்று;

    திரும்பு get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    வெறுமை
restore_snapshot(
    முழு		idx,
    முழு		close_curwin)	    // closing current window
{
    win_T	*wp;

    எனில் (curtab->tp_snapshot[idx] != வெற்று
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	எனில் (wp != வெற்று && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    நிலையான முழு
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    எனில் (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == வெற்று) != (fr->fr_next == வெற்று)
	    || (sn->fr_child == வெற்று) != (fr->fr_child == வெற்று)
	    || (sn->fr_next != வெற்று
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != வெற்று
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != வெற்று && !win_valid(sn->fr_win)))
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    நிலையான win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = வெற்று;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    எனில் (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    எனில் (sn->fr_next != வெற்று)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	எனில் (wp2 != வெற்று)
	    wp = wp2;
    }
    எனில் (sn->fr_child != வெற்று)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	எனில் (wp2 != வெற்று)
	    wp = wp2;
    }
    திரும்பு wp;
}

#எனில் defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    முழு
win_hasvertsplit(வெறுமை)
{
    frame_T	*fr;

    எனில் (topframe->fr_layout == FR_ROW)
	திரும்பு TRUE;

    எனில் (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    எனில் (fr->fr_layout == FR_ROW)
		திரும்பு TRUE;

    திரும்பு FALSE;
}
#endif

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    முழு
get_win_எண்(win_T *wp, win_T *first_win)
{
    முழு		i = 1;
    win_T	*w;

    க்கு (w = first_win; w != வெற்று && w != wp; w = W_NEXT(w))
	++i;

    எனில் (w == வெற்று)
	திரும்பு 0;
    இல்லையெனில்
	திரும்பு i;
}

    முழு
get_tab_எண்(tabpage_T *tp)
{
    முழு		i = 1;
    tabpage_T	*t;

    க்கு (t = first_tabpage; t != வெற்று && t != tp; t = t->tp_next)
	++i;

    எனில் (t == வெற்று)
	திரும்பு 0;
    இல்லையெனில்
	திரும்பு i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    நிலையான முழு
frame_check_height(frame_T *topfrp, முழு height)
{
    frame_T *frp;

    எனில் (topfrp->fr_height != height)
	திரும்பு FALSE;

    எனில் (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    எனில் (frp->fr_height != height)
		திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    நிலையான முழு
frame_check_width(frame_T *topfrp, முழு width)
{
    frame_T *frp;

    எனில் (topfrp->fr_width != width)
	திரும்பு FALSE;

    எனில் (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    எனில் (frp->fr_width != width)
		திரும்பு FALSE;

    திரும்பு TRUE;
}

#எனில் defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    நிலையான முழு
int_cmp(மாறிலி வெறுமை *pa, மாறிலி வெறுமை *pb)
{
    மாறிலி முழு a = *(மாறிலி முழு *)pa;
    மாறிலி முழு b = *(மாறிலி முழு *)pb;
    எனில் (a > b)
	திரும்பு 1;
    எனில் (a < b)
	திரும்பு -1;
    திரும்பு 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    எழுத்து *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    முழு		tw;
    முழு		col;
    முழு		மொத்தம் = 0;
    முழு		color_cols[256];
    முழு		i;
    முழு		j = 0;

    எனில் (wp != வெற்று && wp->w_buffer == வெற்று)
	திரும்பு வெற்று;  // buffer was closed

    எனில் (cc != வெற்று)
	s = cc;
    இல்லையெனில் எனில் (wp != வெற்று)
	s = wp->w_p_cc;

    எனில் (wp != வெற்று)
	tw = wp->w_buffer->b_p_tw;
    இல்லையெனில்
	// buffer-local value not set, assume zero
	tw = 0;

    போது (*s != NUL && மொத்தம் < 255)
    {
	எனில் (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    எனில் (!VIM_ISDIGIT(*s))
		திரும்பு e_invalid_argument;
	    col = col * getdigits(&s);
	    எனில் (tw == 0)
		செல் skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    எனில் (col < 0)
		செல் skip;
	}
	இல்லையெனில் எனில் (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	இல்லையெனில்
	    திரும்பு e_invalid_argument;
	color_cols[மொத்தம்++] = col - 1;  // 1-based to 0-based
skip:
	எனில் (*s == NUL)
	    நிறுத்து;
	எனில் (*s != ',')
	    திரும்பு e_invalid_argument;
	எனில் (*++s == NUL)
	    திரும்பு e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    எனில் (wp == வெற்று)
	திரும்பு வெற்று;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    எனில் (மொத்தம் == 0)
	wp->w_p_cc_cols = வெற்று;
    இல்லையெனில்
    {
	wp->w_p_cc_cols = ALLOC_MULT(முழு, மொத்தம் + 1);
	எனில் (wp->w_p_cc_cols != வெற்று)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, மொத்தம், அளவு(முழு), int_cmp);

	    க்கு (i = 0; i < மொத்தம்; ++i)
		// skip duplicates
		எனில் (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    திரும்பு வெற்று;  // no error
}
#endif

    முழு
get_last_winid(வெறுமை)
{
    திரும்பு last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   முழு
win_locked(win_T *wp)
{
    திரும்பு wp->w_locked;
}
